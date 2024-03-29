# Licensed Materials - Property of IBM
# Text Toolkit for IBM Streams
# (c) Copyright IBM Corp. 2016
# All rights reserved.                               
.PHONY: build all clean distributed

SPLC_FLAGS = -a
OUTPUT_DIR = output
TO_BE_DELETED = 
ifeq ($(STREAMS_STUDIO_BUILDING), 1)
  # Building from Streams Studio, use env vars set by studio
  SPLC = $(STREAMS_STUDIO_SC_PATH)
  SPL_PATH = $(STREAMS_STUDIO_SPL_PATH)
  OUTPUT_DIR = $(STREAMS_STUDIO_OUTPUT_DIRECTORY)
else ifdef STREAMS_BUILDSERVICE
  SPLC = $(STREAMS_INSTALL)/bin/sc
  SPL_PATH = ../toolkits:$(STREAMS_INSTALL)/toolkits
  STREAMSX_NLP_TOOLKIT = ../toolkits/com.ibm.streamsx.nlp
else
  # Building outside of Streams Studio...
  ifeq ($(STREAMS_INSTALL),)
    $(error error: environment variable STREAMS_INSTALL has to be set)
  endif
  # Set this variable if streamsx.nlp sample or toolkit location changed in your environment.
  STREAMSX_NLP_TOOLKIT ?= ../../com.ibm.streamsx.nlp
  SPLC = $(STREAMS_INSTALL)/bin/sc
  SPL_PATH = $(STREAMSX_NLP_TOOLKIT)
  # delete directories and files generated and required by StreamsStudio
  TO_BE_DELETED += bin impl .toolkitList .classpath
endif
TO_BE_DELETED += $(OUTPUT_DIR)

SPLC_FLAGS += -t $(SPL_PATH)

SPL_MAIN_COMPOSITE = nlp.sample::UimaTextGeneratedTypes
SPL_CMD_ARGS ?=

CREATE_TYPES_BIN=$(STREAMSX_NLP_TOOLKIT)/bin/createTypes.pl

build: gentypes distributed 

all: clean build

gentypes:
	chmod +x $(CREATE_TYPES_BIN)
	$(CREATE_TYPES_BIN) --pearFile=etc/troubleful9.pear --namespace nlp.sample

distributed:
	$(SPLC) $(SPLC_FLAGS) -M $(SPL_MAIN_COMPOSITE) $(SPL_CMD_ARGS)

clean:
	$(SPLC) -C -M $(SPL_MAIN_COMPOSITE)
	rm -rf $(TO_BE_DELETED)
	rm -f nlp.sample/TypesGenerated.spl

prepare: gentypes
	$(STREAMS_INSTALL)/bin/spl-make-toolkit -i .
