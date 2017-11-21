# Licensed Materials - Property of IBM
# NLP Toolkit for IBM Streams
# (c) Copyright IBM Corp. 2016
# All rights reserved.                               

DIRS := $(shell for x in *; do if [ -d $$x ]; then echo -n "$$x "; fi; done)
$(info Samples to build: $(DIRS))
$(info )

.PHONY: all clean 

all:
	for x in $(DIRS); do echo -e "\n######## make sample $$x ########"; cd $$x; make all; cd -; done

clean: clean-doc
	for x in $(DIRS); do cd $$x; make clean; cd -; done

make-doc:
	cd UimaTextGenerated; make gentypes
	for x in $(DIRS); do echo -e "\n######## make-doc sample $$x ########"; cd $$x; spl-make-doc -i . --include-all; cd -; done

clean-doc:
	for x in $(DIRS); do cd $$x; rm -rf doc; cd -; done

