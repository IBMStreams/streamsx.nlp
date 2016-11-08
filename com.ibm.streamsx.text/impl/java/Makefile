# Licensed Materials - Property of IBM
# Text Toolkit for IBM Streams
# (c) Copyright IBM Corp. 2016
# All rights reserved. 
.PHONY: all clean compile

ifneq ($(realpath $(STREAMS_INSTALL)),)
STREAMS_BIN=$(STREAMS_INSTALL)/bin
else 
$(error STREAMS_INSTALL not defined)
endif

# compiler settings ####################################################################
JFLAGS = -sourcepath ./src -d ./bin -cp $(STREAMS_INSTALL)/lib/com.ibm.streams.operator.jar:../lib/apache-uima/uima-core.jar:../lib/apache-uima/ruta-core-2.4.0.jar:../lib/apache-uima/uimafit-core-2.1.0.jar
JAVAC = $(STREAMS_INSTALL)/java/bin/javac
JAVA_FILES_LIST=.sources

# targets ##############################################################################

# starts all targets
all: clean compile
	@echo $@


# compile the sources
compile:
	@echo "compiles source files *************************************************"
	find -name "*.java" > $(JAVA_FILES_LIST)
	mkdir bin
	$(JAVAC) $(JFLAGS) @$(JAVA_FILES_LIST)
	rm -f $(JAVA_FILES_LIST)
	@echo "***********************************************************************"
	@echo ""

# clean up generated artifacts
clean:
	@echo "***********************************************************************"
	@echo "$@: remove classes"
	rm -rf bin
	@echo "***********************************************************************"
	@echo "$@: remove generated files"
	rm -f ../../com.ibm.streamsx.text/RutaText/RutaText.xml
	rm -f ../../com.ibm.streamsx.text/RutaCas/RutaCas.xml
	rm -f ../../com.ibm.streamsx.text/UimaText/UimaText.xml
	rm -f ../../com.ibm.streamsx.text/UimaCas/UimaCas.xml
	@echo ""

