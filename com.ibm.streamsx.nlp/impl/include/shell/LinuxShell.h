// Copyright (C) 2016  International Business Machines Corporation
// All Rights Reserved

#ifndef LINUX_SHELL_H
#define LINUX_SHELL_H

#include <sys/wait.h>
#include <sys/select.h>

#include <cassert>
#include <cerrno>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

#include <SPL/Runtime/Utility/CV.h>
#include <SPL/Runtime/Utility/Mutex.h>
#include <SPL/Runtime/Utility/RuntimeUtility.h>
#include <SPL/Runtime/Common/RuntimeException.h>


namespace com { namespace ibm { namespace streamsx { namespace linuxshell {


        //~~~~~~~~~~~~~~~~~~ exception class for LinuxShell ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        
        class LinuxShellException : public std::runtime_error
        {
        public:
        LinuxShellException(std::string const & e) : std::runtime_error(e) {}
        };
        
        
        //~~~~~~~~~~~~~~~~~~ LinuxShell class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        
        class LinuxShell
        {
          
        public:
          
          /// Constructor
          LinuxShell() : shutdown_(false), terminated_(false) {}
          
          /// Destructor
          ~LinuxShell() { terminate(); }
          
          enum ReasonCode
          {
            Shutdown,      // 0, Terminated due to explicit shutdown
            Unknown,       // 1, Terminated due to an unknown reason (signal, coredump, etc.)
            Exit           // 2, Terminated due to pipe exiting with a status
          };
          

        private:

          pid_t child_; // child process

          enum { READ=0, WRITE=1 };
          int stdinPipe_[2]; // pipe for STDIN of the linux pipe
          int stdoutPipe_[2]; // pipe for the STDOUT of the linux pipe
          int stderrPipe_[2]; // pipe for the STDERR of the linux pipe

          int maxfd_; // max file descriptor + 1 for select() on STDOUT and STDIN
          fd_set readSet_; // file descriptor set for select() on STDOUT and STDIN

          std::string stdoutBuffer_; // temporary result holder for STDOUT
          std::string stderrBuffer_; // temporary result holder for STDERR
          bool stdoutClosed_; // completion status of STDOUT
          bool stderrClosed_; // completion status of STDERR

          ReasonCode exitReason_;
          int exitCode_;
          
          SPL::CV cv_;
          SPL::Mutex mutex_;
          volatile bool shutdown_;
          volatile bool terminated_;
          


          // create an 'argv' parameter for the exec() system function
          
        private:
          char const ** getArguments(const std::string & name, std::vector<std::string> const & args) 
          {
            size_t argc = args.size() + 1;
            char const ** argv = new char const *[argc+1];
            
            argv[argc] = NULL;
            argv[0] = name.c_str();
            std::vector<std::string>::const_iterator it = args.begin();
            for(unsigned i=0; it!=args.end(); ++i, ++it) 
              argv[i+1] = (*it).c_str();
            return argv;
          }



          // write a buffer of text to STDIN, return true if successful, or false if not

        private:
          bool writeToPipe(int fd, char const * buffer, size_t count)
          {
            while(count>0) {
              ssize_t n = ::write(fd, buffer, count);        
              if (n==-1 && errno!=EINTR) {
                return false;
              } else if (n>0) {
                count -= n;
                buffer += n;
              }
            }
            return true;
          }


          // append text from STDOUT or STDERR onto a buffer, and return the number of characters received

        private:
          int readFromPipe(int fd, std::string & buffer)
          {
            char buf[1024]; 
            ssize_t count = read(fd, buf, 1023);
            if (count>0) { 
              buf[count] = '\0';
              buffer += buf;
            }
            return count;
          }

          

          /// set up pipes to and from a shell's STDIN, STDOUT, and STDERR
          /// @param the pipe command. E.g.: "sed 's/streams/STREAMS/g' | sed 's/spl/SPL/g'"
          /// @throws LinuxShellException if an error occurs during the pipe setup

        public:
          void setup(std::string const & command) {
            
            // reset the 'terminated_' flag so that the terminate() method below 
            //will wait for the child process and capture its exit code
            terminated_ = false;
            exitReason_ = Unknown;
            exitCode_ = -1;
            child_ = -1;

            // create some pipes for the shell
            if (pipe(stdinPipe_)) 
              throw LinuxShellException(SPL::RuntimeUtility::getErrorNoStr());
            if (pipe(stdoutPipe_)) 
              throw LinuxShellException(SPL::RuntimeUtility::getErrorNoStr());
            if (pipe(stderrPipe_))
              throw LinuxShellException(SPL::RuntimeUtility::getErrorNoStr());

            // set up file descriptors for the shell (need to be closed upon the exec call)
            fcntl(stdinPipe_[WRITE], F_SETFD, FD_CLOEXEC);
            fcntl(stdoutPipe_[READ], F_SETFD, FD_CLOEXEC);
            fcntl(stderrPipe_[READ], F_SETFD, FD_CLOEXEC);

            // fork a copy of this process
            pid_t pid = fork(); 
            if(pid == -1) 
              throw LinuxShellException(SPL::RuntimeUtility::getErrorNoStr());

            // handle each side of the process fork
            if(pid) { 
              child_ = pid;
              // We are in the parent process 
              // Close unused side of pipes  
              close(stdinPipe_[READ]);
              close(stdoutPipe_[WRITE]); 
              close(stderrPipe_[WRITE]);
              // Set the in sides of the pipes to non-blocking
              fcntl(stdoutPipe_[READ], F_SETFL, O_NONBLOCK);
              fcntl(stderrPipe_[READ], F_SETFL, O_NONBLOCK);
              // Setup select descriptors
              FD_ZERO(&readSet_);
              FD_SET(stdoutPipe_[READ], &readSet_);
              FD_SET(stderrPipe_[READ], &readSet_);
              stdoutClosed_ = stderrClosed_ = false;
              maxfd_ = 1 + std::max(stdoutPipe_[READ], stderrPipe_[READ]);
              return;
            } else { 
              // We are in the child process
              // Close and replace standard in with the input side of the pipe     
              if(0>dup2(stdinPipe_[READ],STDIN_FILENO))                                          
                throw LinuxShellException(SPL::RuntimeUtility::getErrorNoStr());     
              // Close and replace standard out with the output side of the pipe 
              if(0>dup2(stdoutPipe_[WRITE],STDOUT_FILENO))                                          
                throw LinuxShellException(SPL::RuntimeUtility::getErrorNoStr());
              // Close and replace standard err with the output side of the pipe 
              if(0>dup2(stderrPipe_[WRITE],STDERR_FILENO))                                          
                throw LinuxShellException(SPL::RuntimeUtility::getErrorNoStr());     
              // execute the shell
              std::string name = "/bin/sh";
              std::vector<std::string> args;
              args.push_back("-c");
              args.push_back(command);
              char const ** argv(getArguments(name, args));
              // Replace the child fork with a new process 
              if(execvp(argv[0], const_cast<char * const *>(argv)) == -1) {
                throw LinuxShellException(SPL::RuntimeUtility::getErrorNoStr());
              }
            }
          }



        /// Write a line to the standard input of the pipe. This call will block
        /// if the pipe is full.
        /// @param line line to be written
        /// @throws LinuxShellException if the pipe has terminated. The
        /// getTerminationInfo() function should be used to check for the
        /// reason.

        public:
          void writeLine(std::string const & line)
          {
            if (!writeToPipe(stdinPipe_[WRITE], line.data(), line.length())) {
              terminate();
              throw LinuxShellException(getTermInfoExplanation()); 
            }
            if (!writeToPipe(stdinPipe_[WRITE], "\n", 1)) {
              terminate();
              throw LinuxShellException(getTermInfoExplanation()); 
            }
          }


        /// Read a line. If both standard output and standard error have a line
        /// available, return both. Otherwise, return one. This call will block
        /// until there is a line to read.
        /// @param res (out parameter) the read line(s)
        /// @return 
        /// 1 if a standard output line was returned, bu standard error was not
        /// 2 if a standard error line was returned, but standard output was not
        /// 3 if both standard output and standard error were returned
        /// -1 if both standard output and standard error have been closed by the shell
        /// -2 if something went wrong in the function's read loop
        /// @throws LinuxShellException if the pipe has terminated. The
        /// getTerminationInfo() function should be used to check for the
        /// reason.

        public:
          int readLine(std::string & outline, std::string & errline)
          {
            bool haveOutline = false, haveErrline = false;
            outline.clear();
            errline.clear();

            // keep reading from STDOUT and STDERR until one or both has a complete line of text
            while (!haveOutline && !haveErrline) {
              
              // find the next newline character in the STDOUT text already received
              size_t position = stdoutBuffer_.find("\n");
              if(position!=std::string::npos) {
                haveOutline = true;
                outline = stdoutBuffer_.substr(0, position);
                stdoutBuffer_ = stdoutBuffer_.substr(position+1);
              } else if (stdoutClosed_ && !stdoutBuffer_.empty()) {
                haveOutline = true;
                outline = stdoutBuffer_;
                stdoutBuffer_.clear();
              }

              // find the next newline character in the STDERR text already received
              position = stderrBuffer_.find("\n");
              if(position!=std::string::npos) {
                haveErrline = true;
                errline = stderrBuffer_.substr(0, position);
                stderrBuffer_ = stderrBuffer_.substr(position+1);
              } else if (stderrClosed_ && !stderrBuffer_.empty()) {
                haveErrline = true;
                errline = stderrBuffer_;
                stderrBuffer_.clear();
              }

              // if we have a complete line from either STDOUT or STDERR, return it (or both)
              if(haveOutline || haveErrline) {
                return (haveOutline ? 1 : 0) + (haveErrline ? 2 : 0);
              }

              // if both STDOUT and STDERR have returned 'end of file', terminate the shell and return 'end-of-shell'
              if(stdoutClosed_ && stderrClosed_) {
                terminate();
                if(exitReason_==Shutdown) return -1;
                throw LinuxShellException(getTermInfoExplanation()); 
              }

              // otherwise, wait for more text from the shell to arrive and read it into the buffers
              fd_set workSet;
              memcpy(&workSet, &readSet_, sizeof(readSet_));
              select(maxfd_, &workSet, NULL, NULL, NULL);
              if(!stdoutClosed_) {
                int count = readFromPipe(stdoutPipe_[READ], stdoutBuffer_);
                stdoutClosed_ = (count==0);
                if (count==-1 && errno!=EAGAIN && errno!=EINTR) {
                  terminate();
                  throw LinuxShellException(getTermInfoExplanation()); 
                } }
              if(!stderrClosed_) {
                int count = readFromPipe(stderrPipe_[READ], stderrBuffer_);
                stderrClosed_ = (count==0);
                if (count==-1 && errno!=EAGAIN && errno!=EINTR) {
                  terminate();
                  throw LinuxShellException(getTermInfoExplanation()); 
                } }
            }

            // this should never happen
            return -2;
          }


        /// Shutdown the pipe. This call will block if the pipe is healthy and
        /// is not completely drained. The reading thread should continue to
        /// issue readLine() calls until the pipe is drained.
        /// @param wait if true, the call will block until the pipe is
        /// drained. Otherwise, the call will return without blocking.

        public:
          void shutdown(bool wait=true)
          {
            SPL::AutoMutex am(mutex_);
            
            if(shutdown_) return;
            shutdown_ = true;
            
            if(terminated_) return;
            
            close(stdinPipe_[WRITE]);
            if(wait) 
              while(!terminated_)
                cv_.wait(mutex_);
          }
          


        // wait for the child process to finish and capture its completion code, or,
        // if the PE is shutting down, kill the child process

        public:
          void terminate() 
          {
            SPL::AutoMutex am(mutex_);
            
            if(terminated_) return;
            terminated_ = true;
            
            // wait for the child process to finish, but no more than 100 milliseconds
            int status; 
            int rc;
            for (int i=0; i<10; i++) { 
              rc = waitpid(child_, &status, WNOHANG); 
              if (rc>0) break;
              usleep(10000);
            }
            
            // capture the exit code from the child process and the reason it finished,
            // or kill it if it has not finished 
            if (rc==-1) {
              throw LinuxShellException(strerror(rc)); 
            } else if (rc==child_) {
              if(WIFEXITED(status)) { 
                exitReason_ = shutdown_ && WEXITSTATUS(status)==0 ? Shutdown : Exit;
                exitCode_ = WEXITSTATUS(status);
              } else {            
                exitReason_ = Unknown;
                exitCode_ = -2;
              }
            } else {
              exitReason_ = shutdown_ && rc==0 ? Shutdown : Unknown;
              exitCode_ = -3;
              ::kill(child_, SIGKILL);
              ::waitpid(child_, &status, 0); 
            } 
            
            if(shutdown_) {
              cv_.signal();
            } else {
              close(stdinPipe_[WRITE]);
            }
            close(stdoutPipe_[READ]); 
            close(stderrPipe_[READ]);
          }
          
          
        public:
          std::string getTermInfoExplanation()
            {
              SPL::AutoMutex am(mutex_);
              std::ostringstream explanation;
              if      (exitReason_ == LinuxShell::Shutdown) explanation << "Shutdown";
              else if (exitReason_ == LinuxShell::Unknown ) explanation << "Unknown" ;
              else if (exitReason_ == LinuxShell::Exit    ) explanation << "Exit"    ; 
              else assert(!"should not happen");
              return explanation.str();
            }
          
          
        
        public:
          int getTermInfoExitCode() 
          {
            SPL::AutoMutex am(mutex_);
            return exitCode_;
          }
          
        };
        
} } } } /* namespace com { namespace ibm { namespace streamsx { namespace linuxshell { */

#endif /* LINUX_SHELL_H */
