# Licensed Materials - Property of IBM
# Text Toolkit for IBM Streams
# (c) Copyright IBM Corp. 2016
# All rights reserved.                               
.PHONY: build all clean distributed

# Fill in streamsx.nlp toolkit location here.
STREAMSX_NLP_TOOLKIT ?= ../../com.ibm.streamsx.nlp

SPLC = $(STREAMS_INSTALL)/bin/sc
SPLC_FLAGS = -a -t $(STREAMSX_NLP_TOOLKIT)
SPL_MAIN_COMPOSITE = nlp.sample::UimaTextGeneratedTypes
SPL_CMD_ARGS ?=

CREATE_TYPES_BIN=$(STREAMSX_NLP_TOOLKIT)/bin/createTypes.pl


build: gentypes distributed 

all: clean build

gentypes:
	$(CREATE_TYPES_BIN) --pearFile=etc/troubleful9.pear --namespace nlp.sample

distributed:
	$(SPLC) $(SPLC_FLAGS) -M $(SPL_MAIN_COMPOSITE) $(SPL_CMD_ARGS)

clean: 
	$(SPLC) $(SPLC_FLAGS) -C -M $(SPL_MAIN_COMPOSITE)
	rm -f nlp.sample/TypesGenerated.spl
	rm -rf output
	rm -rf impl
	rm -rf bin
	rm -f .toolkitList

prepare: gentypes
	$(STREAMS_INSTALL)/bin/spl-make-toolkit -i .
