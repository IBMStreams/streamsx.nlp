#ifndef _gposttl_h_
#define _gposttl_h_

#include <iostream>
#include <string>
#include <set>
#include <vector>
using namespace std;

int initialize_tagger( char *envp );
int check_and_tag( char *ptr0, int enhance_penntag, set<string> &dst);

//
//	find all nouns and noun phrases in input
//		and return these in dst
//
void FindNounPhrase(string &input, set<string> &dst);

void DestroyPosTagger();

void tokenize(const string& str, vector<string> &tokens,
		const string& delimiter, const bool trimEmpty);
#endif
