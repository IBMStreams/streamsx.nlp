# Copyright (C)2016, International Business Machines Corporation
# All rights reserved.

import testutils as tt

def test_execution(name, python_command):
	'''Execute the test case'''
	#tt.assert_pass(err != 0, stdout, stderr)
	print "Execute scenario LinearClassificationSample"
	tt.run_checked(['output/ModelBuilder/bin/standalone', 'pythonCommand='+python_command])
	tt.run_checked(['output/bin/standalone', 'pythonCommand='+python_command])
	tt.run_checked(['diff', 'data/out2.txt', 'data/expected.txt'])
	tt.run_checked(['output/ModelBuilder/bin/standalone', 'pythonCommand='+python_command, 'trainingFile=training2Classes.csv'])
	tt.run_checked(['output/bin/standalone', 'pythonCommand='+python_command])
	tt.run_checked(['diff', 'data/out2.txt', 'data/expected2Classes.txt'])

def test_cleanup(name):
	'''Removes all output files which are produced during test execution'''
	tt.remove_f('data/out.txt')
	tt.remove_f('data/out2.txt')
	tt.remove_globbing('data/LinearSVC_model.pkl*')
	tt.remove_globbing('data/TfidfVectorizer_model.pkl*')
