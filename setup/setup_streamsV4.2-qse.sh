#!/bin/bash
# Copyright (C)2016, International Business Machines Corporation
# All rights reserved.

DIR=$(dirname "$(readlink -f "$0")")

# install for python 2.6
cmd="sudo yum install python-argparse"
echo "$cmd"
eval $cmd

# upgrade pip (python 3.5)
cmd="sudo /opt/pyenv/versions/3.5.1/bin/pip install --upgrade pip"
echo "$cmd"
eval $cmd

# install required modules (python 3.5)
cmd="sudo /opt/pyenv/versions/3.5.1/bin/pip install -r $DIR/requirements_3.5.txt"
echo "$cmd"
eval $cmd

