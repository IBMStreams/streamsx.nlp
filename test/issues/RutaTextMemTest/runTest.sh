#!/bin/bash
# Copyright (C)2016, International Business Machines Corporation
# All rights reserved.


cmd="../scripts/runTestMem.pl --main=RutaTextMemTest --iterations=5 --interval=60"
echo "$cmd"
eval $cmd

