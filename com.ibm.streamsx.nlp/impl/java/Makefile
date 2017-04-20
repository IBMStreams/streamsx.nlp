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
SRC_GEN = ./src-gen
BUILD_DIR = ./build
JFLAGS = -sourcepath ./src -d $(BUILD_DIR) -cp $(STREAMS_INSTALL)/lib/com.ibm.streams.operator.jar:../lib/apache-uima/uima-core.jar:../lib/apache-uima/ruta-core-2.4.0.jar:../lib/apache-uima/uimafit-core-2.1.0.jar -s $(SRC_GEN)
JAVAC = $(STREAMS_INSTALL)/java/bin/javac
JAR = $(STREAMS_INSTALL)/java/bin/jar
JAVA_FILES_LIST=.sources
JAR_FILE = ../lib/com.ibm.streamsx.nlp.jar

# targets ##############################################################################

# starts all targets
all: clean compile
	@echo $@


# compile the sources
compile:
	@echo "compiles source files *************************************************"
	find -name "*.java" > $(JAVA_FILES_LIST)
	mkdir -p $(BUILD_DIR)
	mkdir -p $(SRC_GEN)
	$(JAVAC) $(JFLAGS) @$(JAVA_FILES_LIST)
	rm -f $(JAR_FILE) $(JAVA_FILES_LIST)
	$(JAR) cvf $(JAR_FILE) -C $(BUILD_DIR) .
	@echo "***********************************************************************"
	@echo ""

# clean up generated artifacts
clean:
	@echo "***********************************************************************"
	@echo "$@: remove classes"
	rm -rf bin
	rm -rf $(BUILD_DIR)
	rm -rf $(SRC_GEN)
	rm -f $(JAR_FILE)
	@echo "***********************************************************************"
	@echo "$@: remove generated files"
	rm -f ../../com.ibm.streamsx.nlp/RutaText/RutaText.xml
	rm -f ../../com.ibm.streamsx.nlp/RutaCas/RutaCas.xml
	rm -f ../../com.ibm.streamsx.nlp/UimaText/UimaText.xml
	rm -f ../../com.ibm.streamsx.nlp/UimaCas/UimaCas.xml
	@echo ""

