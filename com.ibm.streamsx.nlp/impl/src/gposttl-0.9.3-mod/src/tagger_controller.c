/*
 
Copyright (C) 2002\-2004, Jimmy Lin

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

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tagger_controller.h"
#include "lex.h"
#include "darray.h"
#include "useful.h"
#include "memory.h"
#include "tagger.h"

#define BUF_SIZE 10240

Registry lexicon_hash, lemma_hash, good_right_hash, good_left_hash;
Registry seenTagging_hash;

Darray lRuleArray,cRuleArray,bigramArray;

/* initializes the tagger by loading all relevant data files, given a
   base path to the location of those files.  */

int _initialize(char *basepath) {
  FILE *lexicon, *bigrams, *lRuleFile, *cRuleFile;
  char *filename = (char*) malloc(strlen(basepath)+80);
  //int i=0;

/*Added by Golam Mortuza Hossain */
  sprintf(filename, "%s/lemmatized_lexicon", basepath);
  lexicon = fopen(filename, "r");
  if ( lexicon == NULL )
  	{
  	free(filename);
	return -1 ;
	}

  sprintf(filename, "%s/BIGRAMS", basepath);
  bigrams = fopen(filename, "r");
  if ( bigrams == NULL )
 	 {
		fclose( lexicon );
  		free(filename);
		return -1 ;
	 }
  
  sprintf(filename, "%s/LEXICALRULEFILE", basepath);
  lRuleFile = fopen(filename, "r");
  if (  lRuleFile == NULL )
 	 {
		 fclose( lexicon );
		 fclose( bigrams );
  		 free(filename);
		 return -1 ;
	 }
  
  sprintf(filename, "%s/CONTEXTUALRULEFILE", basepath);
  cRuleFile = fopen(filename, "r");
  if (  cRuleFile == NULL )
 	 {
		 fclose( lexicon );
		 fclose( bigrams );
		 fclose( lRuleFile );
  		 free(filename);
		 return -1 ;
	 }
  

/* g.m.h */
/*
  sprintf(filename, "%s/LEXICON", basepath);
  lexicon = fopen(filename, "r");

  sprintf(filename, "%s/BIGRAMS", basepath);
  bigrams = fopen(filename, "r");
  sprintf(filename, "%s/LEXICALRULEFILE", basepath);
  lRuleFile = fopen(filename, "r");
  sprintf(filename, "%s/CONTEXTUALRULEFILE", basepath);
  cRuleFile = fopen(filename, "r");

  if ( lexicon == NULL || bigrams == NULL || 
       lRuleFile == NULL || cRuleFile == NULL ) {
    printf("Datafiles missing!\n");
    return;
  }
*/
 
   Tagger (lexicon, bigrams, lRuleFile, cRuleFile,
	  &lexicon_hash, &lemma_hash, &good_right_hash,
	  &good_left_hash, &seenTagging_hash,
	  &bigramArray, &lRuleArray, &cRuleArray);

  fclose(cRuleFile);
  fclose(lRuleFile);
  fclose(bigrams);
  fclose(lexicon);
  free(filename);

  return 0;
}

/* tags a string.  Note that the code seg faults if it hasn't been
   properly initialized. */
char *tag(char *buf, int enhanced_penntag) {
  int numSen = 1;
  char **buffer = (char **) malloc(sizeof(char *));
  char *output ;
  
  buffer[0] = (char *) malloc((strlen(buf)+1)*sizeof(char *));
  strcpy(buffer[0], buf);

  buffer = StartStateTagger (buffer, numSen, lexicon_hash,
			     good_right_hash, good_left_hash,
			     bigramArray, lRuleArray);

  
/* lexicon_hash is effectively the same as a word_hash, which
     is read from the lexicon file and originally used here. We
     just replace word_hash with lexicon_hash here (save memory!) */
  buffer = FinalStateTagger (buffer, numSen, seenTagging_hash,
			     lexicon_hash, lemma_hash, cRuleArray,
			     enhanced_penntag ) ;
  
/*Added by Golam Mortuza Hossain */
  output = buffer[0]; free(buffer); 
  return output ;
/* g.m.h */
/*  return buffer[0]; */
}

/* performs general cleanup */
void destroy_tagger(void) {
  int i;

  Registry_traverse(lexicon_hash,FreeRegistryEntry,NULL);
  Registry_destroy(lexicon_hash);

/*Added by Golam Mortuza Hossain */
  Registry_traverse(lemma_hash,FreeRegistryEntry,NULL);
  Registry_destroy(lemma_hash);
/* g.m.h */

  Registry_traverse(good_right_hash,FreeRegistryEntry,NULL);
  Registry_destroy(good_right_hash);

  Registry_traverse(good_left_hash,FreeRegistryEntry,NULL);
  Registry_destroy(good_left_hash);

  Registry_traverse(seenTagging_hash,FreeRegistryEntry,NULL);
  Registry_destroy(seenTagging_hash);

  for (i=0;i<(int)Darray_len(lRuleArray);++i)
    free((char*)Darray_get(lRuleArray,i));

  for (i=0;i<(int)Darray_len(cRuleArray);++i)
    free((char*)Darray_get(cRuleArray,i));

  for (i=0;i<(int)Darray_len(bigramArray);++i)
    free((char*)Darray_get(bigramArray,i));

  Darray_destroy(lRuleArray);
  Darray_destroy(cRuleArray);
  Darray_destroy(bigramArray);
}
