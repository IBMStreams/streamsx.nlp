# Licensed Materials - Property of IBM
# Text Toolkit for IBM Streams
# (c) Copyright IBM Corp. 2016
# All rights reserved.                                  
.PHONY: build all clean distributed

# Fill in streamsx.nlp toolkit location here.
STREAMSX_NLP_TOOLKIT ?= ../../com.ibm.streamsx.nlp

SPLC = $(STREAMS_INSTALL)/bin/sc
SPLC_FLAGS = -a -t $(STREAMSX_NLP_TOOLKIT)
SPL_MAIN_COMPOSITE = nlp.sample::LinearClassificationSample
SPL_CMD_ARGS ?=

build: distributed 

all: clean build

distributed:
	$(SPLC) $(SPLC_FLAGS) -M $(SPL_MAIN_COMPOSITE) $(SPL_CMD_ARGS)

clean: 
	$(SPLC) $(SPLC_FLAGS) -C -M $(SPL_MAIN_COMPOSITE)
	rm -rf output
	rm -rf bin
	rm -f /tmp/out*
	rm -f /tmp/*.pkl*
	rm -f .toolkitList

