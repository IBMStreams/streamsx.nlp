/*
___________________________________________________________________

This file is a part of GPoSTTL: An enhanced version of Brill's
rule-based Parts-of-Speech Tagger with built-in Tokenizer and
Lemmatizer by Golam Mortuza Hossain <gmhossain at gmail.com>.
This file can be redistributed and/or modified under the terms of
either the license of GPoSTTL or optionally under GPL as below.
___________________________________________________________________
 
Copyright (C) 2007-2008, Golam Mortuza Hossain <gmhossain@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU General Public License for more details.
 
You should have received a copy of the GNU General Public
License along with this program; if not, write to the Free
Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.
__________________________________________________________________

*/

#define VERSION "0.9.3"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "gposttl.h"
#include "tagger_controller.h"
#include "lex.h"
#include "darray.h"
#include "useful.h"
#include "memory.h"
#include "tagger.h"
#include "tokenizer.h"
#include "enhance_penntag.h"

#define  INPUTLEN  4*10240


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

int initialize_tagger( char *envp )
{
	char basepath[250] = "../data" ;
	int i ;
	
	if ( envp != NULL ) 
	{
		sprintf( basepath,"%s", envp);
		i = _initialize( basepath );
		if ( i == 0 ) return 0;
	}
	
	#ifdef HAVE_CONFIG_H
	sprintf( basepath,"%s/%s", DATADIR, PACKAGE); 
	i = _initialize( basepath );
	if ( i == 0 ) return 0;
	#endif

	
	return -1 ; 
}

//
// http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
//
void tokenize(const string& str, vector<string> &tokens,
		const string& delimiters=" \t\n", const bool trimEmpty=false)
{
	string::size_type pos, lastPos = 0;
	while(true)
	{
		pos = str.find_first_of(delimiters, lastPos);
		if (pos == string::npos) 
		{
			pos = str.length();
			if (pos != lastPos || !trimEmpty)
				tokens.push_back(str.substr(lastPos, pos-lastPos));
			break;
		}
		else 
		{
			if (pos != lastPos || !trimEmpty)
				tokens.push_back(str.substr(lastPos, pos-lastPos));
		}

		lastPos = pos + 1; 
	}
}

/* check for tag */
int check_and_tag ( char *ptr0, int  enhance_penntag, set<string> &dst)
{
	vector<string> tokens;
	tokenize(tag(Tokenizer(ptr0), enhance_penntag), tokens);
	/*cout << "t{";
	for (vector<string>::iterator it=tokens.begin(); it!=tokens.end(); it++) {
		cout << (string)(*it) << ", ";
	}
	cout << "}" << endl;*/
	
	int pos_tag = 1, pos_word = 2, pos_first_noun;
	bool noun_found = false;
	string tmp_s = "";
	while(pos_word < (int)tokens.size()) {
		if (!noun_found) {
		//	if (tokens[pos_tag].at(0) == 'N') {
			if (tokens[pos_tag].compare(0, 1, "N") == 0) {
				// first noun in a series found
				pos_first_noun = pos_word;
				noun_found = true;
			}  
		} else {
		//	if (tokens[pos_tag].at(0) != 'N') {
			if (tokens[pos_tag].compare(0, 1, "N") != 0) {
				// insert the series of consecutive nouns
				tmp_s.clear();
				while (pos_first_noun < pos_word) {
					tmp_s.append(tokens[pos_first_noun] + " ");
					pos_first_noun += 3;	
				}
				dst.insert(tmp_s);
 				//cout << "NNP: " << tmp_s << endl;
				noun_found = false;
			}
		}
		pos_tag += 3;
		pos_word += 3;
	}
	if (noun_found) {
		// insert the series of consecutive nouns
		tmp_s.clear();	
		while (pos_first_noun < pos_word) {
			tmp_s.append(tokens[pos_first_noun] + " ");
			pos_first_noun += 3;	
		}
		dst.insert(tmp_s);
 		//cout << "NNP: " << tmp_s << endl;
		noun_found = false;
	}

	/*cout << "{";
	for (set<string>::iterator it=dst.begin(); it!=dst.end(); it++) {
		cout << (string)(*it) << ", ";
	}
	cout << "}" << endl*/;
		
	return 0 ;		
}

void FindNounPhrase(string &input, set<string> &dst) {
	static int enhance_penntag = 1;

	// the tagger assumes that the last letter in the input string
	//	is '\n'
	if (input.size() > 0) {
		if (input.at(input.size()-1) != '\n')
			input.append("\n");
	}	

	// replace all '\n' with ' ';
	for (int pos = 0; pos<(int)input.length(); pos++)
		if (input[pos] == '\n') {input[pos]=' ';} 
	
	check_and_tag( const_cast<char*>(input.c_str()), enhance_penntag, dst);
}

void DestroyPosTagger()
{
	destroy_tagger (); 
}


