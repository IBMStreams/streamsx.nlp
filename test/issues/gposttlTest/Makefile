# Licensed Materials - Property of IBM
# (c) Copyright IBM Corp. 2016
# All rights reserved. 

CC=g++

STREAMSX_NLP_TOOLKIT ?= ../../../com.ibm.streamsx.nlp
GPOSTTL_SRC_DIR = $(STREAMSX_NLP_TOOLKIT)/impl/src/gposttl-0.9.3-mod/src

all: clean gposttl test

gposttl:
	mkdir gposttl
	mkdir lib
	cd $(GPOSTTL_SRC_DIR); make clean header libGPoSTTL.a 
	cp $(GPOSTTL_SRC_DIR)/libGPoSTTL.a ./lib
	cp $(STREAMSX_NLP_TOOLKIT)/etc/gposttl/* ./gposttl

test: Test.cpp
	$(CC) -o test Test.cpp -I $(STREAMSX_NLP_TOOLKIT)/impl/include -L./lib -lGPoSTTL 

clean:
	rm -rf gposttl
	rm -rf lib
	rm -f test
	rm -f $(GPOSTTL_SRC_DIR)/libGPoSTTL.a
