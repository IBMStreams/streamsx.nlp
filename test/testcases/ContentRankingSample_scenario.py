# Copyright (C)2016, International Business Machines Corporation
# All rights reserved.

import testutils as tt
import shutil
import sys

def test_execution(name, python_command):
	'''Execute the test case'''
	#tt.assert_pass(err != 0, stdout, stderr)
	print "Execute scenario ContentRankingSample"
	tt.run_checked(['output/ModelBuilder/bin/standalone', 'pythonCommand='+python_command])

	# the test script runs in python2
	# check the python version in the environment since the Streams job might use a different python version
	ver = tt.get_major_version(python_command)
	shutil.copy('data/model_KB/d_lemms.json.provided'+str(ver), 'data/model_KB/d_lemms.json')
	shutil.copy('data/model_KB/kb_lstm_model.pklz.provided'+str(ver), 'data/model_KB/kb_lstm_model.pklz')


	print 'XXXXXXXXXXXXXXXXXXXXXXXXXXX'
	tt.run_checked(['output/bin/standalone', 'pythonCommand='+python_command])
	print 'XXXXXXXXXXXXXXXXXXXXXXXXXXX'
	tt.run_checked(['diff', 'data/out.txt', 'data/expected'+str(ver)+'.txt'])

def test_cleanup(name):
	'''Removes all output files which are produced during test execution'''
	tt.remove_f('data/out.txt')
	tt.remove_f('data/model_KB/d_lemms.json')
	tt.remove_f('data/model_KB/kb_lstm_model.pklz')

