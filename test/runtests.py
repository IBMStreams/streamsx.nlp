#!/usr/bin/env python2

# Copyright (C) 2016 International Business Machines Corporation.
# All Rights Reserved.

import sys, argparse, os
import subprocess as su
import testutils as tu

class DuplicateTestFailure(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return 'DuplicateTestFailure:' + self.value

def execute_test(test_path, clean, python_command):
    '''Clean the test directory, build the test case and execute the test'''
    global test_success
    print '\n**** execute_test:', test_path, '                                  ****'
    topdir = os.getcwd()
    test_dir, test_name = os.path.split(test_path)
    scenario_name = test_name + '_scenario'
    #print scenario_name
    try:
        os.chdir(test_path)
        
        #sys.path.append(os.getcwd())
        tester = __import__(scenario_name)
        
        tester.test_cleanup(test_path)
        su.check_call(['make', 'clean'])
        print '** Clean test directory finished **'
        
        if not clean:
            su.check_call(['make', 'all'])
            print '** Build finished', '**'
            tester.test_execution(test_path, python_command)
            print '** Test finished', '**'
            
        test_success = True
        print '**** execute_test:', test_path, 'finished: SUCCESS               ****'
    except tu.TestFailure as tf:
        print 'Exception: TestFailure'
        tf.say(test_path)
    except ImportError as ie:
        print 'Exception: ImportError'
        print test_path + ' fail:\n' + \
              '\tunable to import ' + scenario_name + '/scenario.py'
        print ie
    except su.CalledProcessError as ce:
        print 'Exception: CalledProcessError'
        print ce
    except OSError as oe:
        print oe
        if oe.errno != 2:
            raise
    finally:
        if 'tester' in dir():
            del tester
        if 'scenario' in sys.modules:
            del sys.modules[scenario_name]
        #if os.getcwd() in sys.path:
        #    sys.path.remove(os.getcwd())
        #tu.remove_f('scenario.pyc')
        os.chdir(topdir)

def main():
    '''Test case execution main function'''
    global cases_executed, cases_success, cases_failed, test_success
    
    parser = argparse.ArgumentParser(
        description='''\
This script runs a test-case or a list of test-cases.

The test PATH must be a path to the (project/toolkit) directory with the test case.
You may enter a list of test case names (pathes) with the --test parameter,
or you may prepare a FILE with the list of test-cases an use the --file option.
Each line in the file represents an absolute or relative path to the test case directory.
Lines starting with a "#" are ignored.

The test case execution expects in SCENARIO_DIR a script that controls the variable part of the test-case
execution. This script must have a name that is composed of the last component of PATH and "_scenario.py".''',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog = '''\
The program returns:
    0, if all test cases were executed successfully
    1, if one ore more test cases failed
    2, if the program was called with invalid parameters or in case of an severe error''')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--test', '-t', dest='test', metavar='PATH', nargs="+", help='test to run (path to the test case directory)')
    group.add_argument('--file', '-f', dest='file', help='file of test PATH')
    parser.add_argument('--directory', '-d', dest='dir', metavar='SCENARIO_DIR', default='testcases', help='directory which contains the test scenario scripts: "NAME_scenario.py"')
    parser.add_argument('--noToolkitMake', '-n', dest='no_Toolkit_make', action='store_true', help='Suppress toolkit build or clean. Per default the streamsx.nlp toolkit is build before the test execution starts. If this option is set, no toolkit make is done.')
    parser.add_argument('--clean', '-c', dest='clean', action='store_true', help='Execute only the clean up functions. If this option is active, the script does not perform any test. This function cleans code and cleans generated data files')
    parser.add_argument('--python', '-p', dest='python_command', default='python', help='Python command to be used in test samples using python')
    args = parser.parse_args()
    
    test_list = []
    if args.test:
        for line in args.test:
            line2 = line.rstrip('/')
            if line2 != '':
                test_list.append(line2)
    if args.file:
        file = open(args.file, 'r')
        for line in file:
            line2 = line.rstrip().lstrip().rstrip('/')
            if (line2 != '') and (line2[0] != '#'):
                test_list.append(line2)

    testcases = ''
    for t in test_list:
        if len(testcases) > 0:
            testcases += '; '
        testcases += t
    print '******** Test cases to execute:                                      ********\n', testcases, '\n'
    if not args.no_Toolkit_make:
        print '******** Make toolkit:                                               ********\n'
        topdir = os.getcwd()
        os.chdir('../com.ibm.streamsx.nlp')
        su.check_call(['make', 'clean'])
        if not args.clean:
            su.check_call(['make', 'all'])
        os.chdir(topdir)

    print '******** Execute tests of test case list                             ********'
    if (args.dir != '') and (args.dir[0] == '/'):
        scenariopath = args.dir
    else:
        scenariopath = os.getcwd() + '/' + args.dir
    sys.path.append(scenariopath)
    #print sys.path
    for t in test_list:
        test_success = False
        if t in cases_executed:
            raise DuplicateTestFailure(t)
        cases_executed.add(t)
        
        execute_test(t, args.clean, args.python_command)
        if test_success:
            cases_success += 1
        else:
            cases_failed.append(t)

    print '\n**************************************************************************'
    print 'Failed test cases:', cases_failed
    print '\nEXECUTED:', len(cases_executed), '\nSUCCESS:', cases_success, '\nERROR:', len(cases_failed)
    if len(cases_executed) != cases_success:
        sys.exit(1)

cases_executed = set()  #the set with the executed test cases
cases_success = 0
cases_failed = [] #the list of the failed test cases
test_success = False #global var with success indicator

if __name__ == '__main__':
	main()

