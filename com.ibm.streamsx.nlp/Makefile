# Licensed Materials - Property of IBM
# Text Toolkit for IBM Streams
# (c) Copyright IBM Corp. 2016
# All rights reserved. 
.PHONY = all clean install

### determine Streams version
ifneq ($(realpath $(STREAMS_INSTALL)),)
STREAMS_BIN=$(STREAMS_INSTALL)/bin
else 
$(error STREAMS_INSTALL not defined)
endif

### determine Streams version
SUPPORTED_KAVERI_VERSIONS:=3.2.0
SUPPORTED_KAVERI_VERSIONS+=3.2.1

ifneq ($(STREAMS_VERSION),)
ST_VERSION=$(STREAMS_VERSION)
endif
ifeq ($(ST_VERSION),)
ST				=$(realpath $(STREAMS_BIN)/streamtool)
ifneq ($(ST),)
ST_VERSION=$(shell $(ST) version | grep Version | sed 's/^.*=//')
else
ST_VERSION="4.0.0.0"
$(warning Streams version not define - set default:$(ST_VERSION))  
endif
endif
ST_VER_SHORT	=$(shell sed 's/\([0-9]*\.[0-9]*\.[0-9]*\)\..*/\1/' <<< $(ST_VERSION))

### SPL tools
SPL_MAKE_TOOLKIT		:=$(STREAMS_INSTALL)/bin/spl-make-toolkit -i . -m -s

### PATH - for ppc64 builds, PATH must include the AT compiler
ARCH := $(shell uname -i)
ifeq ($(ARCH),ppc64le)
COMPILER_VERSION = at8.0
PATH:=/opt/$(COMPILER_VERSION)/bin:$(PATH)
endif
ifeq ($(ARCH),ppc64)
COMPILER_VERSION = at7.0
ifneq ($(filter $(ST_VER_SHORT),$(SUPPORTED_KAVERI_VERSIONS)),)
COMPILER_VERSION = at5.0
endif
ifneq ($(realpath /opt/$(COMPILER_VERSION)/bin64),)
PATH:=/opt/$(COMPILER_VERSION)/bin64:$(PATH)
else 
$(error AT compiler not found in path : /opt/$(COMPILER_VERSION)/bin64)
endif
ifneq ($(realpath /opt/$(COMPILER_VERSION)/bin),)
PATH:=/opt/$(COMPILER_VERSION)/bin:$(PATH)
else 
$(error AT compiler not found in path : /opt/$(COMPILER_VERSION)/bin)
endif
$(info using $(COMPILER_VERSION) compiler)
endif



all: install

toolkit.xml: java-build
	rm -f toolkit.xml; $(SPL_MAKE_TOOLKIT)

java-build:
	cd impl/java; make all

text-gposttl:
	cd impl/src/gposttl-0.9.3-mod/src/; make all

prepare-header:
	./tkinstall.pl

text-libs:
	cd impl/src/; make -f Makefile.libtextutils all

install: text-gposttl prepare-header text-libs toolkit.xml

clean: clean-doc
	spl-make-toolkit -c -i . -m
	cd impl/src/gposttl-0.9.3-mod/src/; make clean
	cd impl/src/; make -f Makefile.libtextutils clean
	cd impl/java/; make clean
	rm -f .toolkitList
	rm -rf output

make-doc: toolkit.xml
	spl-make-doc -i . --include-composite-operator-diagram

clean-doc:
	rm -rf doc/spldoc
