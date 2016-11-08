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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <malloc.h>
#include "lex.h"
#include "darray.h"
#include "registry.h"
#include "memory.h"
#include "useful.h"

#include "tagger.h"

void SetTagVal (Darray tagVal, Darray freeptrs, int count, char* val, 
		int* first)
{
  // Benjamin Han: add val for later destruction
  if (*first) 
    {
      Darray_addh(freeptrs,val);
      *first=0;
    }
  Darray_set(tagVal,count,val);
}

void FreeRegistryEntry (void* key, void* value, void* ptr)
{
  free((char*)key);

  // Benjamin Han: Brill used ((char*) 1) as the default value
  if ((char*)value!=(char*)1) free((char*)value);
}

char **StartStateTagger(char **buffer, int numSen, Registry lexicon_hash,
			Registry good_right_hash, Registry good_left_hash,
			Darray bigramArray, Darray lRuleArray)
{
	char *bufp;
        //char *tempruleptr;
	char *atempptr;
	char line[MAXLINELEN];      /* input line buffer */
	char *linecopy;
	Darray tag_array_key,tag_array_val;
	char /* word[MAXWORDLEN],tag[MAXTAGLEN],*word2,*tag2,*/
	   *tempstr,*tempstr2;
	char **perl_split_ptr,**temp_perl_split_ptr,**therule,**therule2;
	char bigram1[MAXWORDLEN],bigram2[MAXWORDLEN];
	char noun[10],proper[10];
/* Added by Golam Mortuza Hossain */
	char number[10];
/* g.m.h */
	int count,count2,count3,rulesize,tempcount, numwords, i, j;
	char tempstr_space[MAXWORDLEN+MAXAFFIXLEN],bigram_space[MAXWORDLEN*2];
	//int numlexiconentries=0;
	//int numwordentries=0;
	int temp;
	
	Registry tag_hash,ntot_hash,bigram_hash;

	// added by Benjamin Han for freeing all allocated char*
	int first;
	Darray freeptrs;     

	ntot_hash = Registry_create(Registry_strcmp,Registry_strhash);
	bigram_hash = Registry_create(Registry_strcmp,Registry_strhash);

	/* Read in corpus to be tagged.  Actually, just record word list, */
	/* since each word will get the same tag, regardless of context. */
	for(i=0; i < numSen; i++) {
	  
	  temp=strlen(buffer[i]);
	  
	  linecopy = (char *)malloc((temp+1)*(sizeof(char)));
	  strcpy(linecopy, buffer[i]);

	  if (not_just_blank(linecopy)){

		char *assigned=(char*)malloc((temp=num_words(linecopy)+1)*sizeof(char));

	    perl_split_ptr = perl_split_independent(linecopy);
	    for (temp_perl_split_ptr = perl_split_ptr, j=0;
		 *temp_perl_split_ptr != NULL;
		 j++,++temp_perl_split_ptr) 

	      if (Registry_get(lexicon_hash,(char *)*temp_perl_split_ptr) 
		  == NULL) 
		{
		  if (Registry_add(ntot_hash,(char *)*temp_perl_split_ptr,
				   (char *)1) == Bool_FALSE)
		    assigned[j]=0;
		  else assigned[j]=1;
		  
		  // Added by Benjamin Han: it's not part of Brill's algorithm.
		  //                        Doe it help?
		  // for the words not found in the lexicon we add the
		  // surrounding bigrams into bigram_hash for the
		  // good_left/right and fgood_left/right lexical rules
	
		  if (j!=0 && j!=temp-1)
		    {
		      sprintf(bigram_space,"%s %s",
			      (char *)*temp_perl_split_ptr,
			      (char *)*(temp_perl_split_ptr+1));
		      Registry_add(bigram_hash,mystrdup(bigram_space),
				   (char *)1);
		      sprintf(bigram_space,"%s %s",
			      (char *)*(temp_perl_split_ptr-1),
			      (char *)*temp_perl_split_ptr);
		      Registry_add(bigram_hash,mystrdup(bigram_space),
				   (char *)1);
		    }
		  else if (j!=temp-1)
		    {
		      sprintf(bigram_space,"%s %s",
			      (char *)*temp_perl_split_ptr,
			      (char *)*(temp_perl_split_ptr+1));
		      Registry_add(bigram_hash,mystrdup(bigram_space),
				   (char *)1);
		    }
		  else if (j!=0)
		    {
		      sprintf(bigram_space,"%s %s",
			      (char *)*(temp_perl_split_ptr-1),
			      (char *)*temp_perl_split_ptr);
		      Registry_add(bigram_hash,mystrdup(bigram_space),
				   (char *)1);
		    } 
		}
	      else assigned[j]=0;

	    for (temp_perl_split_ptr = perl_split_ptr,j=0;j<temp;
		 j++,++temp_perl_split_ptr)
	      if (!assigned[j]) free(*temp_perl_split_ptr);

	    free(perl_split_ptr);
		free(assigned);
	  }
	  free(linecopy);
	}

	tag_array_key = Darray_create();
	tag_array_val = Darray_create();
	Registry_fetch_contents(ntot_hash,tag_array_key,tag_array_val);

	// Benjamin Han: Now tag_array_val contains a bunch of (char*)1

	/*  READ IN THE BIGRAM HERE (from bigramArray) */
	// Benjamin Han: we're sure that every element in bigramArray
	// is of length > 1, as initialized in function Tagger
	for(i=0; i < (int) Darray_len(bigramArray); i++) {
	  sscanf((char*)Darray_get(bigramArray,i),"%s%s",bigram1,bigram2);
	  if (Registry_get(good_right_hash,bigram1) &&
	      Registry_get(ntot_hash,bigram2)) {
	    sprintf(bigram_space,"%s %s",bigram1,bigram2);
	    Registry_add(bigram_hash,mystrdup(bigram_space),(char *)1);
	  }
	  if (Registry_get(good_left_hash,bigram2) &&
	      Registry_get(ntot_hash,bigram1)) {
	    sprintf(bigram_space,"%s %s",bigram1,bigram2);
	    Registry_add(bigram_hash,mystrdup(bigram_space),(char *)1);
	  }
	}


/********** START STATE ALGORITHM
  YOU CAN USE OR EDIT ONE OF THE TWO START STATE ALGORITHMS BELOW, 
  # OR REPLACE THEM WITH YOUR OWN ************************/

	strcpy(noun,"NN");
	strcpy(proper,"NNP");
	strcpy(number,"CD");
	
/* UNCOMMENT THIS AND COMMENT OUT START STATE 2 IF ALL UNKNOWN WORDS
   SHOULD INITIALLY BE ASSUMED TO BE TAGGED WITH "NN".
   YOU CAN ALSO CHANGE "NN" TO A DIFFERENT TAG IF APPROPRIATE. */

      /*** START STATE 1 ***/
/*   for (count=0; count < Darray_len(tag_array_val);++count) 
	Darray_set(tag_array_val,count,noun); */

/* THIS START STATE ALGORITHM INITIALLY TAGS ALL UNKNOWN WORDS WITH TAG 
   "NN" (singular common noun) UNLESS THEY BEGIN WITH A CAPITAL LETTER, 
   IN WHICH CASE THEY ARE TAGGED WITH "NNP" (singular proper noun)
   YOU CAN CHANGE "NNP" and "NN" TO DIFFERENT TAGS IF APPROPRIATE.*/

     /*** START STATE 2 ***/
      for (count=0; count < (int)Darray_len(tag_array_val);++count) 
	{
/* Added by Golam Mortuza Hossain: If unknown words starts with number
 * then assume it as being number  */
	  if (((char *)Darray_get(tag_array_key,count))[0] >='0' && 
	      ((char *)Darray_get(tag_array_key,count))[0] <= '9') 
	    Darray_set(tag_array_val,count,number); 
/* g.m.h. */
	  else if (((char *)Darray_get(tag_array_key,count))[0] >='A' && 
	      ((char *)Darray_get(tag_array_key,count))[0] <= 'Z') 
	    Darray_set(tag_array_val,count,proper); 
	  else
	    Darray_set(tag_array_val,count,noun); }


/******************* END START STATE ALGORITHM ****************/
      
        freeptrs=Darray_create();

	for (count=0,first=1;
	     count < (int)Darray_len(lRuleArray);
	     ++count,first=1) {
	  strcpy(line, (char *)Darray_get(lRuleArray,count));
	  /*fprintf(stderr,"RULE IS: %s\n",line);*/
	  /*fprintf(stderr,"s");*/
	  therule = perl_split_independent(line);

	  // Benjamin Han: the following comment is TOTALLY UNTRUE!

/* we don't worry about freeing "rule" space, as this is a small fraction
   of total memory used */
	  therule2 = &therule[1];
	  rulesize=0;
	  perl_split_ptr = therule;
	  while(*(++perl_split_ptr) != NULL) {
	    ++rulesize;}

	  // Benjamin Han: STRANGE THING: rulesize is actually one
	  // less than the real rule size -- that explains why in
	  // LEXICALRULEFILE we have those trailing useless x's and
	  // why we always use therule[rulesize-1] in Darray_set()
	  // Also note perl_split_independent does strange things
	  // also: it allocates 1 more elem beyond the one storing
	  // NULL (NULL is used to signal the end of the array)

	  if (strcmp(therule[1],"char") == 0) {
	    for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	      if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[rulesize-1]) !=0) {
		if(strpbrk(reinterpret_cast<const char*>(Darray_get(tag_array_key,count2)), therule[0]) !=
		   NULL) {
		  SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		}
	      }
	    }
	  }
	  else if (strcmp(therule2[1],"fchar") == 0) { 
	    for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	      if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[0]) ==0) {
		if(strpbrk(reinterpret_cast<const char*>(Darray_get(tag_array_key,count2)), therule2[0]) !=
		   NULL) {
		  SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		}
	      }
	    }
	  }
	  else if (strcmp(therule[1],"deletepref") == 0) {
	    for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	      if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[rulesize-1])
		  !=0) {
		tempstr = reinterpret_cast<char*>(Darray_get(tag_array_key,count2));
		for (count3=0;count3<atoi(therule[2]);++count3) {
		  if (tempstr[count3] != therule[0][count3])
		    break;}
		if (count3 == atoi(therule[2])) {
		  tempstr += atoi(therule[2]);
		  if (Registry_get(lexicon_hash,(char *)tempstr) != NULL) {
		    SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		  }
		}
	      }
	    }
	  }
	  
	  else if (strcmp(therule2[1],"fdeletepref") == 0) {
	    for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	      if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[0]) == 0){ 
		tempstr=reinterpret_cast<char*>(Darray_get(tag_array_key,count2));
		for (count3=0;count3<atoi(therule2[2]);++count3) {
		  if (tempstr[count3] != therule2[0][count3])
		    break;}
		if (count3 == atoi(therule2[2])) {
		  tempstr += atoi(therule2[2]);
		  if (Registry_get(lexicon_hash,(char *)tempstr) != NULL){
		    SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		  }
		}
	      }
	    }
	  }
	  
	 
	  else if (strcmp(therule[1],"haspref") == 0) {
	    for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	      if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[rulesize-1])
		  !=0) {
		tempstr = reinterpret_cast<char*>(Darray_get(tag_array_key,count2));
		for (count3=0;count3<atoi(therule[2]);++count3) {
		  if (tempstr[count3] != therule[0][count3])
		    break;}
		if (count3 == atoi(therule[2])) {
		  SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		}
	      }
	    }
	  }
	  
	  else if (strcmp(therule2[1],"fhaspref") == 0) {
	    for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	      if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[0]) == 0){ 
		tempstr=reinterpret_cast<char*>(Darray_get(tag_array_key,count2));
		for (count3=0;count3<atoi(therule2[2]);++count3) {
		  if (tempstr[count3] != therule2[0][count3])
		    break;}
		if (count3 == atoi(therule2[2])) {
		  SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		}
	      }
	    }
	  }

	  
	  else if (strcmp(therule[1],"deletesuf") == 0) {
	    for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	      if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[rulesize-1])
		  !=0) {
		tempstr = reinterpret_cast<char*>(Darray_get(tag_array_key,count2));
		tempcount=strlen(tempstr)-atoi(therule[2]);
		for (count3=tempcount;
		     count3<(int)strlen(tempstr); ++count3) {
		  if (tempstr[count3] != therule[0][count3-tempcount])
		    break;}
		if (count3 == (int)strlen(tempstr)) {
		  tempstr2 = mystrdup(tempstr);
		  tempstr2[tempcount] = '\0';
		  if (Registry_get(lexicon_hash,(char *)tempstr2) != NULL){
		    SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		  }
		  free(tempstr2);
		}
	      }
	    }
	  }
	  
	  else if (strcmp(therule2[1],"fdeletesuf") == 0) {
	    for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	      if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[0]) == 0){ 
		tempstr=reinterpret_cast<char*>(Darray_get(tag_array_key,count2));
		tempcount=strlen(tempstr)-atoi(therule2[2]);
		for (count3=tempcount;
		     count3<(int)strlen(tempstr); ++count3) {
		  if (tempstr[count3] != therule2[0][count3-tempcount])
		    break;}
		if (count3 == (int)strlen(tempstr)){
		  tempstr2 = mystrdup(tempstr);
		  tempstr2[tempcount] = '\0';
		  if (Registry_get(lexicon_hash,(char *)tempstr2) != NULL){
		    SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		  }
		  free(tempstr2);
		}
	      }
	    }
	  }
	  else if (strcmp(therule[1],"hassuf") == 0) {
	    for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	      if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[rulesize-1])
		  !=0) {
		tempstr = reinterpret_cast<char*>(Darray_get(tag_array_key,count2));
		tempcount=strlen(tempstr)-atoi(therule[2]);
		for (count3=tempcount;
		     count3<(int)strlen(tempstr); ++count3) {
		  if (tempstr[count3] != therule[0][count3-tempcount])
		    break;}
		if (count3 == (int)strlen(tempstr)) {
		  SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		}
	      }
	    }
	  }
	  
	  else if (strcmp(therule2[1],"fhassuf") == 0) {
	    for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	      if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[0]) == 0){ 
		tempstr=reinterpret_cast<char*>(Darray_get(tag_array_key,count2));
		tempcount = strlen(tempstr)-atoi(therule2[2]);
		for (count3=tempcount;
		     count3<(int)strlen(tempstr); ++count3) {
		  if (tempstr[count3] != therule2[0][count3-tempcount])
		    break;}
		if (count3 == (int)strlen(tempstr)){
		  SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		}
	      }
	    }
	  }
	  
	  else if (strcmp(therule[1],"addpref") == 0) {
	    for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	      if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[rulesize-1])
		  == 0){
		sprintf(tempstr_space,"%s%s",
			therule[0],reinterpret_cast<char*>(Darray_get(tag_array_key,count2)));
		if (Registry_get(lexicon_hash,(char *)tempstr_space) != NULL){
		  SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		}
	      }
	    }
	  }

	   else if (strcmp(therule2[1],"faddpref") == 0) {
	    for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	      if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[rulesize-1]) == 0){
		sprintf(tempstr_space,"%s%s",therule2[0],
			reinterpret_cast<char*>(Darray_get(tag_array_key,count2)));
		if (Registry_get(lexicon_hash,(char *)tempstr_space) != NULL){
		  SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		}
	      }
	    }
	  }


	   else if (strcmp(therule[1],"addsuf") == 0) {
	    for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	      if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[rulesize-1])
		  !=0) {
		sprintf(tempstr_space,"%s%s",
		       reinterpret_cast<char*>(Darray_get(tag_array_key,count2)),
		       therule[0]);
		if (Registry_get(lexicon_hash,(char *)tempstr_space) != NULL){
		  SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		}
	      }
	    }
	  }
	  
	  
	   else if (strcmp(therule2[1],"faddsuf") == 0) {
	    for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	      if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[0])
		  ==0) {
		sprintf(tempstr_space,"%s%s",
			reinterpret_cast<char*>(Darray_get(tag_array_key,count2)),
			therule2[0]);
		if (Registry_get(lexicon_hash,(char *)tempstr_space) != NULL){
		  SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		}
	      }
	    }
	  }

	  
	   else if (strcmp(therule[1],"goodleft") == 0) {
	     for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	       if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[rulesize-1])
		   !=0) {
		 sprintf(bigram_space,"%s %s",
			reinterpret_cast<char*>(Darray_get(tag_array_key,count2)),therule[0]);
		 if (Registry_get(bigram_hash,(char *)bigram_space) != NULL) {
		   SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		 }
	       }
	     }
	   }

	   else if (strcmp(therule2[1],"fgoodleft") == 0) {
	     for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	       if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[0])
		   ==0) {
		 sprintf(bigram_space,"%s %s",reinterpret_cast<char*>(Darray_get(tag_array_key,count2)),therule2[0]);
		 if (Registry_get(bigram_hash,(char *)bigram_space) != NULL) {
		   SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		 }
	       }
	     }
	   }
	  
	  else if (strcmp(therule[1],"goodright") == 0) {
	     for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	       if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[rulesize-1])
		   !=0) {
		 sprintf(bigram_space,"%s %s",therule[0],reinterpret_cast<char*>(Darray_get(tag_array_key,count2)));
		 if (Registry_get(bigram_hash,(char *)bigram_space) != NULL) {
		   SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		 }
	       }
	     }
	   }

	   else if (strcmp(therule2[1],"fgoodright") == 0) {
	     for (count2=0;count2<(int)Darray_len(tag_array_key);++count2) {
	       if (strcmp(reinterpret_cast<const char*>(Darray_get(tag_array_val,count2)),therule[0])
		   ==0) {
		 sprintf(bigram_space,"%s %s",therule2[0],reinterpret_cast<char*>(Darray_get(tag_array_key,count2)));
		 if (Registry_get(bigram_hash,(char *)bigram_space) != NULL) {
		   SetTagVal(tag_array_val,freeptrs,count2,therule[rulesize-1],&first);
		 }
	       }
	     }
	   }

	  // Benjamin Han: he didn't clean after use
	  for (i=0;i<rulesize-1;i++) free(therule[i]);

	  // Benjamin Han: first==1 indicates that we never set
	  // therule[rulesize-1] to tag_array_val
	  if (first) free(therule[rulesize-1]);

	  // Benjamin Han: oh my it's an additional (and useless) allocation
	  free(therule[rulesize]);
	  free(therule);
	}

	/* now go from darray to hash table */
		  
	tag_hash = Registry_create(Registry_strcmp,Registry_strhash);
	for (count=0;count<(int)Darray_len(tag_array_key);++count) {
	  Registry_add(tag_hash,
		       (char *)Darray_get(tag_array_key,count),
		       (char *)Darray_get(tag_array_val,count)); }
	count=0;


	for(i=0; i<numSen; i++) {
	  if(not_just_blank(buffer[i])) {

	    // Benjamin Han: STRANGE! num_words returns one less than
	    // the real count!  what is on earth the difference
	    // between this and numspaces in Utils/lex.c? oh my.
	    numwords = num_words(buffer[i])+1;
	    bufp = (char *)malloc(sizeof(char) * 
				  (strlen(buffer[i])+1+(numwords*MAXTAGLEN)));
	    bufp[0]=0;
 	    perl_split_ptr = perl_split(buffer[i]);
	    temp_perl_split_ptr = perl_split_ptr;
	    while (*temp_perl_split_ptr != NULL) {
	      if ((atempptr = strchr(*temp_perl_split_ptr,'/')) != NULL
		  && *(atempptr+1) == '/') {
		
	      /* a word can be pretagged by putting two slashes between the */
	      /* word and the tag ::  The boy//NN ate . */
	      /* if a word is pretagged, we just spit out the pretagging */
		bufp = strcat(bufp, *temp_perl_split_ptr);
		bufp = strcat(bufp, " "); }
	      else if
		((tempstr = reinterpret_cast<char*>(Registry_get(lexicon_hash,*temp_perl_split_ptr)))
		 != NULL) {
		bufp = strcat(bufp, *temp_perl_split_ptr);
		bufp = strcat(bufp, "/");
		bufp = strcat(bufp, tempstr);
	      	bufp = strcat(bufp, " "); }
	      else {
		bufp = strcat(bufp, *temp_perl_split_ptr);
		bufp = strcat(bufp, "/");
		bufp = strcat(bufp, 
			      reinterpret_cast<const char*>(Registry_get(tag_hash,*temp_perl_split_ptr))); 
		bufp = strcat(bufp, " "); }
	      
	      ++temp_perl_split_ptr;
	    }
	    free(*perl_split_ptr);
	    free(perl_split_ptr);

	    free(buffer[i]);
	    buffer[i] = bufp;
	  }
	}

	// Benjamin Han: the content of tag_hash came from tag_array_*
	// -- it'll be freed by the later two for-loops on tag_array_*
	Registry_destroy(tag_hash);

	// Benjamin Han: the content of ntot_hash was duplicated in
	// tag_array_key -- it will be deallocated in the for loop
	// below
	Registry_destroy(ntot_hash);

	// Benjamin Han: we DO need to free the content of bigram_hash
	Registry_traverse(bigram_hash,FreeRegistryEntry,NULL);
	Registry_destroy(bigram_hash);

	// Benjamin Han: tag_array_key is first initialized by 
	// Registry_fetch_contents(ntot_hash,tag_array_key,tag_array_val);
	// and ntot_hash is initialized by 
	// Registry_add(ntot_hash,(char *)*temp_perl_split_ptr,
	//              (char *)1). NOTE THAT (char *)1 -- IT'S BAD!
	// Conclusion: it's safe to free every tag_array_key element.
	for (count=0;count<(int)Darray_len(tag_array_key);++count)
	  free((char *)Darray_get(tag_array_key,count));

	// Benjamin Han: now free all allocated tag values
	for (count=0;count<(int)Darray_len(freeptrs);++count)
	  free((char *)Darray_get(freeptrs,count));

	Darray_destroy(tag_array_key);
	Darray_destroy(tag_array_val);
	Darray_destroy(freeptrs);

	return buffer;
}
