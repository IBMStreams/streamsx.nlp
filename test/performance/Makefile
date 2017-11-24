# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# **************************************************************** 
# THIS SAMPLE CODE IS PROVIDED ON AN "AS IS" BASIS. IBM MAKES NO   
# REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED, CONCERNING    
# USE OF THE SAMPLE CODE, OR THE COMPLETENESS OR ACCURACY OF THE   
# SAMPLE CODE. IBM DOES NOT WARRANT UNINTERRUPTED OR ERROR-FREE    
# OPERATION OF THIS SAMPLE CODE. IBM IS NOT RESPONSIBLE FOR THE    
# RESULTS OBTAINED FROM THE USE OF THE SAMPLE CODE OR ANY PORTION  
# OF THIS SAMPLE CODE.                                             
#                                                                  
# LIMITATION OF LIABILITY. IN NO EVENT WILL IBM BE LIABLE TO ANY   
# PARTY FOR ANY DIRECT, INDIRECT, SPECIAL OR OTHER CONSEQUENTIAL   
# DAMAGES FOR ANY USE OF THIS SAMPLE CODE, THE USE OF CODE FROM    
# THIS [ SAMPLE PACKAGE,] INCLUDING, WITHOUT LIMITATION, ANY LOST  
# PROFITS, BUSINESS INTERRUPTION, LOSS OF PROGRAMS OR OTHER DATA   
# ON YOUR INFORMATION HANDLING SYSTEM OR OTHERWISE.                
#                                                                  
# (C) Copyright IBM Corp. 2010, 2014  All Rights reserved.         
#                                                                  
# end_generated_IBM_copyright_prolog                               
.PHONY: build all clean ruta aql uima

SPLC = $(STREAMS_INSTALL)/bin/sc
SPLC_FLAGS = -a  
SPL_DATA_DIR = ./data
SPL_CMD_ARGS ?= 
AQL_SPL_MAIN_COMPOSITE=aql::Main
RUTA_SPL_MAIN_COMPOSITE=ruta::Main
UIMA_SPL_MAIN_COMPOSITE=uima::Main

# Fill in streamsx.nlp toolkit location here.
STREAMSX_NLP_TOOLKIT ?= ../../com.ibm.streamsx.nlp

## modify below to point the installation of the text toolkit
TEXT_TOOLKIT_INSTALL ?= $(STREAMS_INSTALL)/toolkits/com.ibm.streams.text

### determine 'streamtool'
ifneq ($(realpath $(STREAMS_INSTALL)),)
ST=$(STREAMS_INSTALL)/bin/streamtool
else 
$(error STREAMS_INSTALL not defined)
endif
STANDALONE_BUILD =
STANDALONE_REQUIRED = $(shell $(ST) version | perl -ne 'm/^Version=(.*)/ && (print ($$1 =~ m/^3\.2\.[01]\./ ? "yes" : "no"));')
ifeq ("$(STANDALONE_REQUIRED)","no")
STANDALONE_REQUIRED = $(shell $(ST) version | perl -ne 'm/^Version=(.*)/ && (print ($$1 =~ m/^4\.[01]\./ ? "yes" : "no"));')
endif
ifeq ("$(STANDALONE_REQUIRED)","yes")
STANDALONE_FLAG = -T
endif
AQL_OUTPUT_DIR=output/aql
RUTA_OUTPUT_DIR=output/ruta
UIMA_OUTPUT_DIR=output/uima

build: aql ruta uima

all: clean build

aql:
	$(SPLC) $(SPLC_FLAGS) $(STANDALONE_FLAG) -M $(AQL_SPL_MAIN_COMPOSITE) --data-directory=data $(SPL_CMD_ARGS) -t $(TEXT_TOOLKIT_INSTALL) --output-directory $(AQL_OUTPUT_DIR)
	$(STANDALONE_BUILD_AQL)

ruta:
	$(SPLC) $(SPLC_FLAGS) $(STANDALONE_FLAG) -M $(RUTA_SPL_MAIN_COMPOSITE) --data-directory $(SPL_DATA_DIR) $(SPL_CMD_ARGS) -t $(STREAMSX_NLP_TOOLKIT) --output-directory $(RUTA_OUTPUT_DIR)
	$(STANDALONE_BUILD_RUTA)

uima:
	$(SPLC) $(SPLC_FLAGS) $(STANDALONE_FLAG) -M $(UIMA_SPL_MAIN_COMPOSITE) --data-directory $(SPL_DATA_DIR) $(SPL_CMD_ARGS) -t $(STREAMSX_NLP_TOOLKIT) --output-directory $(UIMA_OUTPUT_DIR)
	$(STANDALONE_BUILD_UIMA)

clean: clean-ruta clean-aql clean-uima
	rm -rf output
	rm -rf doc
	rm -rf bin
	rm -f data/*.out
	rm -f .toolkitList

clean-aql: 
	$(SPLC) $(SPLC_FLAGS) -C -M $(AQL_SPL_MAIN_COMPOSITE)

clean-ruta: 
	$(SPLC) $(SPLC_FLAGS) -C -M $(RUTA_SPL_MAIN_COMPOSITE)

clean-uima: 
	$(SPLC) $(SPLC_FLAGS) -C -M $(UIMA_SPL_MAIN_COMPOSITE)

