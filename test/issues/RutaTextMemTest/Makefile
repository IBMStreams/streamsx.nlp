# Licensed Materials - Property of IBM
# Text Toolkit for IBM Streams
# (c) Copyright IBM Corp. 2016
# All rights reserved.                                  
.PHONY: build all clean distributed

# Fill in streamsx.text toolkit location here.
STREAMSX_NLP_TOOLKIT ?= ../../../com.ibm.streamsx.nlp

SPLC = $(STREAMS_INSTALL)/bin/sc
SPLC_FLAGS = -a -t $(STREAMSX_NLP_TOOLKIT) 
SPL_MAIN_COMPOSITE = RutaTextMemTest
SPL_DATA_DIR = ./data
SPL_CMD_ARGS ?= 

### determine 'streamtool'
ifneq ($(realpath $(STREAMS_INSTALL)),)
ST=$(STREAMS_INSTALL)/bin/streamtool
else 
$(error STREAMS_INSTALL not defined)
endif
STANDALONE_FLAG =
STANDALONE_REQUIRED = $(shell $(ST) version | perl -ne 'm/^Version=(.*)/ && (print ($$1 =~ m/^3\.2\.[01]\./ ? "yes" : "no"));')
ifeq ("$(STANDALONE_REQUIRED)","no")
STANDALONE_REQUIRED = $(shell $(ST) version | perl -ne 'm/^Version=(.*)/ && (print ($$1 =~ m/^4\.[01]\./ ? "yes" : "no"));')
endif
ifeq ("$(STANDALONE_REQUIRED)","yes")
STANDALONE_FLAG = -T
DISTRIBUTED_OUTPUT_DIR = output/Distributed
DISTRIBUTED_BUILD = $(SPLC) $(SPLC_FLAGS) -M $(SPL_MAIN_COMPOSITE) --data-directory $(SPL_DATA_DIR) $(SPL_CMD_ARGS) --output-directory $(DISTRIBUTED_OUTPUT_DIR)
else
DISTRIBUTED_BUILD = echo "no extra build required for distributed"
endif


build: standalone 

all: clean build

distributed:
	$(DISTRIBUTED_BUILD)

standalone:
	$(SPLC) $(SPLC_FLAGS) $(STANDALONE_FLAG) -M $(SPL_MAIN_COMPOSITE) --data-directory $(SPL_DATA_DIR) $(SPL_CMD_ARGS)

clean: 
	$(SPLC) $(SPLC_FLAGS) -C -M $(SPL_MAIN_COMPOSITE)
	rm -rf output
	rm -rf doc
	rm -rf bin
	rm -f data/out*
	rm -f mem*

