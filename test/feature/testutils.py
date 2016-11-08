# Copyright (C) 2016 International Business Machines Corporation.
# All Rights Reserved.


import sys, os, glob
import subprocess as su

class TestFailure(Exception):
    'The exception object for a failed test case'
    def __init__(self, out, err):
        self.stdout = out
        self.stderr = err

    def say(self, test_name):
        print(test_name + ' fail:\n' + \
              '\tstdout: ' + self.stdout + '\n' + \
              '\tstderr: ' + self.stderr + '\n')

def remove_f(name):
    '''Removes a file but ignores not existing files errors'''
    try:
        os.remove(name)
    except OSError as e:
        if e.errno != 2:
            raise

def remove_globbing(pattern):
    '''removes files with globbing'''
    names = glob.glob(pattern)
    for f in names:
        print 'remove:', f
        os.remove(f)

def run_checked(args=list()):
    '''Print args and execute command checked. Throws subprocess.CalledProcessError if command returns a non zero return code'''
    print 'execute:', args
    r = su.check_call(args)
    print 'returned:', r

def run_command(command):
    '''Execute command and return iterator of stdout'''
    p = su.Popen(command, stdout=su.PIPE, stderr=su.STDOUT)
    return iter(p.stdout.readline, b'')

def get_major_version(python_command):
    '''return the major version of the python interpreter'''
    print "script python version: "+sys.version.split(' ')[0]
    pc = python_command + ' --version'
    command = pc.split()
    for line in run_command(command):
        print(line)
        if line.startswith("Python 3."):
            print "version 3"
            return(3)
        elif line.startswith("Python 2."):
            print "version 2"
            return(2)
        else:
            sys.stderr.write("You need python 2.7 or later to run this test case\n")
            exit(1)    

