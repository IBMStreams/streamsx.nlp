//
// *******************************************************************************
// * Copyright (C)2016, International Business Machines Corporation *
// * All rights reserved. *
// *******************************************************************************
//
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include "../../../impl/include/gposttl/gposttl.h"
#include "../../../impl/include/gposttl/tagger_controller.h"
#include "../../../impl/include/gposttl/tokenizer.h"

// Define SPL types and functions
#include <SPL/Runtime/Function/SPLFunctions.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Function/UtilFunctions.h>

#include <streams_boost/tokenizer.hpp>

#include "UtilsFunctions.h"

namespace com { namespace ibm { namespace streamsx { namespace nlp { namespace utils {

SPL::boolean initializeLemmatizer()
{
	SPL::boolean result = true;
	// required files for initialization are in the toolkit etc/gposttl directory
	SPL::rstring directory = getToolkitPath() + "/etc/gposttl";
	SPLAPPTRC(L_INFO, "initialize_tagger(" << directory << ")", "");
	int res = initialize_tagger((char*)directory.c_str());
	if (-1 == res) {
    		result = false;
		SPLAPPTRC(L_ERROR, "initializeLemmatizer failed", "");
	}
	SPLAPPTRC(L_INFO, "return(" << result << ")", "");
	return (result);
}

SPL::boolean initializeLemmatizer(SPL::rstring const & directory)
{
	SPL::boolean result = true;
	SPLAPPTRC(L_INFO, "initialize_tagger(" << directory << ")", "");
	int res = initialize_tagger((char*)directory.c_str());
	if (-1 == res) {
    		result = false;
		SPLAPPTRC(L_ERROR, "initializeLemmatizer failed", "");
	}
	SPLAPPTRC(L_INFO, "return(" << result << ")", "");
	return (result);
}

void lemmatize(SPL::rstring const text, SPL::list<SPL::rstring>& words, SPL::list<SPL::rstring>& pos, SPL::list<SPL::rstring>& lemmas)
{
	SPL::rstring res = "";
	char* tokenizerBuf = Tokenizer((char*)text.c_str());
	char* tagBuf = tag(tokenizerBuf, 1);
	res.append(tagBuf);
    free(tokenizerBuf);
    free(tagBuf);

	streams_boost::char_separator<char> sep("\n");
	streams_boost::tokenizer<streams_boost::char_separator<char> > tokens(res, sep);
	for ( streams_boost::tokenizer<streams_boost::char_separator<char> >::iterator it = tokens.begin();
		it != tokens.end(); ++it) {
		
		streams_boost::char_separator<char> sep2("\x09");
		streams_boost::tokenizer<streams_boost::char_separator<char> > tokens2(*it,sep2);
		int xx = 0;
		for ( streams_boost::tokenizer<streams_boost::char_separator<char> >::iterator it2 = tokens2.begin();
			it2 != tokens2.end(); ++it2) {
			xx++;
			if ( xx == 1 ) { // words
				words.push_back(*it2);
			}
			if ( xx == 2 ) { // pos
				pos.push_back(*it2);
			}
			if ( xx == 3 ) { // lemma
				lemmas.push_back(*it2);
			}
		}
	}
}

void lemmatize(SPL::rstring const text, SPL::list<SPL::rstring>& lemmas)
{
	SPL::rstring res = "";
	char* tokenizerBuf = Tokenizer((char*)text.c_str());
	char* tagBuf = tag(tokenizerBuf, 1);
	res.append(tagBuf);
    free(tokenizerBuf);
    free(tagBuf);

	streams_boost::char_separator<char> sep("\n");
	streams_boost::tokenizer<streams_boost::char_separator<char> > tokens(res, sep);
	for ( streams_boost::tokenizer<streams_boost::char_separator<char> >::iterator it = tokens.begin();
		it != tokens.end(); ++it) {
		
		streams_boost::char_separator<char> sep2("\x09");
		streams_boost::tokenizer<streams_boost::char_separator<char> > tokens2(*it,sep2);
		int xx = 0;
		for ( streams_boost::tokenizer<streams_boost::char_separator<char> >::iterator it2 = tokens2.begin();
			it2 != tokens2.end(); ++it2) {
			xx++;
			if ( xx == 3 ) { // lemma
				lemmas.push_back(*it2);
			}
		}
	}
}

void generateNgram(SPL::list<SPL::rstring> const & words, SPL::uint32 size, SPL::list<SPL::rstring>& terms)
{
	SPL::uint32 numWords = SPL::Functions::Collections::size(words);
	for (int i = 0; i < (numWords-size+1); i++) {
		SPL::rstring term = "";
    		int start = i;
    		int end = i + size;
    		for (int w = start; w < end; w++){
			if ("" == term) {
				term = words[w]; // first word
			}
			else {
				term = term + " " + words[w]; // append words
			}
    		}
		// add term to output list
		terms.push_back(term); 
	}
}

void generateNgram(SPL::list<SPL::rstring> const & words, SPL::uint32 minSize, SPL::uint32 maxSize, SPL::list<SPL::rstring>& terms)
{
	if (minSize > maxSize) {
		minSize = maxSize;
	}
	for (int ngramSize = minSize; ngramSize <= maxSize; ngramSize++) {
		SPL::uint32 numWords = SPL::Functions::Collections::size(words);
		for (int i = 0; i < (numWords-ngramSize+1); i++) {
			SPL::rstring term = "";
	    		int start = i;
	    		int end = i + ngramSize;
	    		for (int w = start; w < end; w++){
				if ("" == term) {
					term = words[w]; // first word
				}
				else {
					term = term + " " + words[w]; // append words
				}
	    		}
			// add term to output list
			terms.push_back(term); 
		}
	}
}

} } } } } // namespace
