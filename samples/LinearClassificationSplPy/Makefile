# Copyright (C) 2017, International Business Machines Corporation. 
# All Rights Reserved.

.PHONY: all distributed clean build

ifeq ($(STREAMS_INSTALL),)
    $(error error: environment variable STREAMS_INSTALL has to be set)
else
    ST=$(STREAMS_INSTALL)/bin/streamtool
endif

STREAMS_TOPOLOGY_TOOLKIT ?= $(STREAMS_INSTALL)/toolkits/com.ibm.streamsx.topology

#VERS=$(shell bash -c "V=$(ST); if [[ ${V} == *Version=4.2* ]]; then echo true; else echo false; fi")
VERS=$(shell bash -c "$(ST) version | grep Version")
VERS_FITS=$(shell bash -c "if [[ $(VERS) == *Version=4.2.* ]]; then echo true; else echo false; fi")
$(info "Streams $(VERS)")
$(info "Version is fine $(VERS_FITS)")
ifeq ("$(VERS_FITS)", "false")
    $(warning The recommended Streams version 4.2.x. Your streams version is not 4.2.x Proceed at your own peril)
endif

SPLC_FLAGS = -a
OUTPUT_DIR = output
TO_BE_DELETED = 
ifeq ($(STREAMS_STUDIO_BUILDING), 1)
  # Building from Streams Studio, use env vars set by studio
  SPLC = $(STREAMS_STUDIO_SC_PATH)
  SPL_PATH = $(STREAMS_STUDIO_SPL_PATH)
  OUTPUT_DIR = $(STREAMS_STUDIO_OUTPUT_DIRECTORY)
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

SPL_MAIN_COMPOSITE = nlp.sample::LinearClassificationPythonSample
DATA_DIR=/tmp
SPL_CMD_ARGS ?=

all: clean distributed

build: distributed

distributed: mysamples/LinearClassification/LinearClassification.xml
	$(SPLC) $(SPLC_FLAGS) -M $(SPL_MAIN_COMPOSITE) --data-directory $(DATA_DIR) $(SPL_CMD_ARGS)

clean:
	$(SPLC) -C -M $(SPL_MAIN_COMPOSITE)
	rm -rf $(TO_BE_DELETED)
	rm -rf mysamples
	rm -rf opt/python/streams/__pycache__
	rm -rf opt/.__splpy

mysamples/LinearClassification/LinearClassification.xml: opt/python/streams/mysamples.py
	python $(STREAMS_TOPOLOGY_TOOLKIT)/bin/spl-python-extract.py -i .


prepare: mysamples/LinearClassification/LinearClassification.xml
	$(STREAMS_INSTALL)/bin/spl-make-toolkit -i .

