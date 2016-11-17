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

#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __GNUG__
#include <unistd.h>
#endif

#include "darray.h"
#include "useful.h"
#include "tagger.h"

void Tagger (FILE *lexicon, FILE *bigrams, FILE *lRuleFile, FILE *cRuleFile,
	     Registry *lexicon_hash, Registry *lemma_hash,
	     Registry *good_right_hash,
	     Registry *good_left_hash, Registry *seenTagging,
	     Darray *bigramArray, Darray *lRuleArray, Darray *cRuleArray)
{
  char line[MAXLINELEN];
  char space[500];
  char word[MAXWORDLEN],tag[MAXTAGLEN];
  //char bigram1[MAXWORDLEN],bigram2[MAXWORDLEN];
  char **perl_split_ptr,**perl_split_ptr2,*atempstr,**temp_perl_split_ptr;
  char *tempruleptr;
  //char bigram_space[MAXWORDLEN*2];
  int numLexiconEntries;
/*Added by Golam Mortuza Hossain */
  char lemma[MAXWORDLEN] ;
  *lemma_hash = Registry_create(Registry_strcmp,Registry_strhash);
/* g.m.h */
  Bool res; // result of Registry_add(), if false then the memory allocated (e.g.with mystrdup()) for name/obj must be freed
  char* tempNamePtr;
  char* tempObjPtr;

  /* Benjamin Han 100400: time for creativity! */
  *lexicon_hash = Registry_create(Registry_strcmp,Registry_strhash);
  *good_right_hash = Registry_create(Registry_strcmp,Registry_strhash);
  *good_left_hash = Registry_create(Registry_strcmp,Registry_strhash);
  *seenTagging = Registry_create(Registry_strcmp,Registry_strhash);
  *lRuleArray = Darray_create();
  *cRuleArray = Darray_create();
  *bigramArray = Darray_create();

  /* lexicon hash stores the most likely tag for all known words.
     we can have a separate wordlist and lexicon file because unsupervised
     learning    can add to wordlist, while not adding to lexicon.  For
     example, if a big    untagged corpus is about to be tagged, the wordlist
     can be extended to    include words in that corpus, while the lexicon
     remains static.    Lexicon is file of form: 
     word t1 t2 ... tn 
     where t1 is the most likely tag for the word, and t2...tn are alternate
     tags, in no particular order. */
  /* read through once to get size */
  for (numLexiconEntries=0;fgets(line,sizeof(line),lexicon) != NULL;
       numLexiconEntries+=num_words(line))
    if (not_just_blank(line)) line[strlen(line) - 1] = '\0';

  fseek(lexicon, (long)0 , SEEK_SET);

  /* just need word and most likely tag from lexicon (first tag entry) */
  /* Benjamin Han 100400: originally it's hinted by the # of lines in lexicon
	 file */
  Registry_size_hint(*lexicon_hash,numLexiconEntries);
/*Added by Golam Mortuza Hossain */
  Registry_size_hint(*lemma_hash,numLexiconEntries);
/* g.m.h */
 
  while(fgets(line,sizeof(line),lexicon) != NULL) {
    if (not_just_blank(line)) {
      line[strlen(line) - 1] = '\0';
      /*Added by Golam Mortuza Hossain */
      sscanf(line,"%s%s%s",word, lemma, tag);
      tempNamePtr = (char *)mystrdup(word);
      tempObjPtr = (char *)mystrdup(lemma);
      res = Registry_add(*lemma_hash, tempNamePtr, tempObjPtr);
      if (!res) {
        free(tempNamePtr);
        free(tempObjPtr);
      }
      /* It would have been much better to just use
       * "struct" and put "lemma" in lexicon hash. But
       * it does not seem to be working by simple hacking*/
      /* g.m.h */
      tempNamePtr = (char *)mystrdup(word);
      tempObjPtr = (char *)mystrdup(tag);
      res = Registry_add(*lexicon_hash, tempNamePtr, tempObjPtr);
      if (!res) {
        free(tempNamePtr);
        free(tempObjPtr);
      }
    }
  }

  /* read in lexical rule file */
  while(fgets(line,sizeof(line),lRuleFile) != NULL) {
    if (not_just_blank(line)){
      line[strlen(line) - 1] = '\0';
      Darray_addh(*lRuleArray,mystrdup(line));
      perl_split_ptr = perl_split(line);
      temp_perl_split_ptr = perl_split_ptr;
      if (strcmp(perl_split_ptr[1],"goodright") == 0) {
        tempruleptr = mystrdup(perl_split_ptr[0]);
        res = Registry_add(*good_right_hash,tempruleptr,(char *)1);
        if (!res) {
          free(tempruleptr);
        }
      }
      else if (strcmp(perl_split_ptr[2],"fgoodright") == 0) {
        tempruleptr = mystrdup(perl_split_ptr[1]);
        res = Registry_add(*good_right_hash,tempruleptr,(char *)1);
        if (!res) {
          free(tempruleptr);
        }
      }
      else if (strcmp(perl_split_ptr[1],"goodleft") == 0) {
        tempruleptr = mystrdup(perl_split_ptr[0]);
        res = Registry_add(*good_left_hash,tempruleptr,(char *)1);
        if (!res) {
          free(tempruleptr);
        }
      }
      else if (strcmp(perl_split_ptr[2],"fgoodleft") == 0) {
        tempruleptr = mystrdup(perl_split_ptr[1]);
        res = Registry_add(*good_left_hash,tempruleptr,(char *)1);
        if (!res) {
          free(tempruleptr);
        }
      }
      free(*perl_split_ptr);
      free(perl_split_ptr);
    }
  }

  /* read in bigram file */
  /* Benjamin Han 100400: I store the contents in bigramArray so
     we don't have to do file IO everytime the start-state-tagger is
     invoked. */
  while(fgets(line,sizeof(line),bigrams) != NULL){
    if(not_just_blank(line)) {
      line[strlen(line) - 1] = '\0';
      atempstr = (char *)malloc(sizeof(char)*(strlen(line)+1));
      strcpy(atempstr,line);
      Darray_addh(*bigramArray,atempstr);
    }
  }

  fseek(lexicon, (long)0 , SEEK_SET);

  /* read in the lexicon for the final-state-tagger */
  Registry_size_hint(*seenTagging,numLexiconEntries);
  
  /* Benjamin Han 100500: MISSING RESTRICT_MOVE section?
     Answer: Brill used registry WORDS while I use lexicon_hash to replace
             his WORDS (see POST::Run) - the only difference is in WORDS 
             every value is 1 while in lexicon_hash a values is the first 
             tag following the word in the lexicon file. */
  while(fgets(line,sizeof(line),lexicon) != NULL) {
    if (not_just_blank(line)) {
      line[strlen(line) - 1] = '\0';
      perl_split_ptr = perl_split(line);
      perl_split_ptr2 = perl_split_ptr;
      ++perl_split_ptr;
      while(*perl_split_ptr != NULL) {
        sprintf(space,"%s %s",*perl_split_ptr2,*perl_split_ptr);
        char* namePtr=mystrdup(space);
        res = Registry_add(*seenTagging,namePtr,(char *)1);
        if (!res) {
          free(namePtr);
        }
        ++perl_split_ptr;
      }
      free(*perl_split_ptr2);
      free(perl_split_ptr2);
    }
  }

  /* read in contextual rule */
  while(fgets(line, sizeof(line), cRuleFile) != NULL)
    if (not_just_blank(line)) {
      line[strlen(line) - 1] = '\0';
      Darray_addh(*cRuleArray,mystrdup(line));
    }
}
