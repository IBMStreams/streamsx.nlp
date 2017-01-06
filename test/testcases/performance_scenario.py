# Copyright (C)2016, International Business Machines Corporation
# All rights reserved.

import testutils as tt

def test_execution(name,  python_command):
	'''Execute the test case'''
	tt.run_checked(['output/aql/bin/standalone', 'loop=100'])
	tt.run_checked(['output/uima/bin/standalone', 'loop=100'])
	tt.run_checked(['output/ruta/bin/standalone', 'loop=100'])
	tt.run_checked(['output/uima/bin/standalone', 'loop=100', 'pear=EmptyUima.pear'])
	tt.run_checked(['output/ruta/bin/standalone', 'loop=100', 'pear=EmptyRuta.pear'])

def test_cleanup(name):
	'''Removes all output files which are produced during test execution'''


