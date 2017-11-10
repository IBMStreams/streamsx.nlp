# Copyright (C) 2017, International Business Machines Corporation. 
# All Rights Reserved.

.PHONY: all distributed clean build

ifeq ($(STREAMS_INSTALL),)
    $(error error: environment variable STREAMS_INSTALL has to be set)
else
    ST=$(STREAMS_INSTALL)/bin/streamtool
    SPLC = $(STREAMS_INSTALL)/bin/sc
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
SPLC = $(STREAMS_INSTALL)/bin/sc
SPL_MAIN_COMPOSITE = myapplication::Main
DATA_DIR=/tmp
SPL_CMD_ARGS ?=

all: clean distributed

build: distributed

distributed: mysamples/LinearClassification/LinearClassification.xml
	$(SPLC) $(SPLC_FLAGS) -M $(SPL_MAIN_COMPOSITE) --data-directory $(DATA_DIR) $(SPL_CMD_ARGS)

clean:
	$(SPLC) -C -M $(SPL_MAIN_COMPOSITE) --data-directory $(DATA_DIR)
	rm -rf mysamples
	rm -rf impl
	rm -rf opt/python/streams/__pycache__
	rm -rf opt/.__splpy

mysamples/LinearClassification/LinearClassification.xml: opt/python/streams/mysamples.py
	python $(STREAMS_TOPOLOGY_TOOLKIT)/bin/spl-python-extract.py -i .



