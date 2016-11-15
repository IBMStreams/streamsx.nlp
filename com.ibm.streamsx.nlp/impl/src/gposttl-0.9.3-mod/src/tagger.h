/*
   Original Copyright notice:

   Copyright (C) 1993 MIT and University of Pennsylvania 
   Written by Eric Brill

   THIS SOFTWARE IS PROVIDED "AS IS", AND M.I.T. MAKES NO
   REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.  By way of
   example, but not limitation, M.I.T. MAKES NO REPRESENTATIONS OR
   WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE
   OR THAT THE USE OF THE LICENSED SOFTWARE OR DOCUMENTATION WILL NOT
   INFRINGE ANY THIRD PARTY PATENTS, COPYRIGHTS, TRADEMARKS OR OTHER
   RIGHTS.  

 */

#ifndef _tagger_h_
#define _tagger_h_

#include "registry.h"
#include "lex.h"
#include <stdio.h>

#define MAXLINELEN 5000
#define MAXTAGLEN 256  /* max char length of pos tags */
#define MAXWORDLEN 256 /* max char length of words */
#define MAXAFFIXLEN 5  /* max length of affixes being considered */
#define RESTRICT_MOVE 1   /* if this is set to 1, then a rule "change a tag */
			  /* from x to y" will only apply to a word if:
			     a) the word was not in the training set or
			     b) the word was tagged with y at least once in
			     the training set  
			     When training on a very small corpus, better
			     performance might be obtained by setting this to
			     0, but for most uses it should be set to 1 */
/*Added by Golam Mortuza Hossain */
typedef struct {
		char tag[MAXTAGLEN];
		char lemma[MAXWORDLEN];
		
} TagLemma ;
/* g.m.h */

void FreeRegistryEntry (void* key, void* value, void* ptr);

void Tagger (FILE *lexicon, FILE *bigrams, FILE *lRuleFile, FILE *cRuleFile,
	     Registry *lexicon_hash, Registry *lemma_hash,
	     Registry *good_right_hash,
	     Registry *good_left_hash, Registry *seenTagging,
	     Darray *bigramArray, Darray *lRuleArray, Darray *cRuleArray);

/* buffer contains numSen sentences (lines). */
char **StartStateTagger(char **buffer, int numSen, Registry lexicon_hash,
			Registry good_right_hash, Registry good_left_hash,
			Darray bigramArray, Darray lRuleArray);

char **FinalStateTagger(char **buf, int numSen, 
			Registry seenTagging, Registry words,
			Registry lemma_hash, Darray cRuleArray,
			int enhanced_penntag);

#endif /* _tagger_h_ */
