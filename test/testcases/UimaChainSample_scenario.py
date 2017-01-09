# Copyright (C)2016, International Business Machines Corporation
# All rights reserved.

import testutils as tt

def test_execution(name, python_command):
	'''Execute the test case'''
	tt.run_checked(['output/bin/standalone'])

def test_cleanup(name):
	'''Removes all output files which are produced during test execution'''
	tt.remove_f('data/output_2_operators.txt')
	tt.remove_f('data/output_aggregate.txt')

