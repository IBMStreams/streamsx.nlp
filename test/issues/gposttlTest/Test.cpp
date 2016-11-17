//
// *******************************************************************************
// * Copyright (C)2016, International Business Machines Corporation *
// * All rights reserved. *
// *******************************************************************************
//
#include <string>
#include <cstdlib>

#include "gposttl/gposttl.h"
#include "gposttl/tagger.h"
#include "gposttl/tagger_controller.h"
#include "gposttl/tokenizer.h"
#include "gposttl/enhance_penntag.h"


using namespace std;


int main(int argc, char** argv) {

	string base = "./gposttl";
	initialize_tagger((char*)base.c_str());

	int enhance_penntag = 1;
	string s0 = "The quick brown fox jumps over the lazy dog";
	char* tokenizerBuf = Tokenizer((char*)s0.c_str());
	char* tagBuf = tag(tokenizerBuf, enhance_penntag);
	string out;
	out.append(tagBuf);
	cout << out << endl;

	free(tokenizerBuf);
	free(tagBuf);

	destroy_tagger();
	return 0;
}
