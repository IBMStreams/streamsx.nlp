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
#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include "lex.h"
#include "darray.h"
#include "registry.h"
#include "memory.h"
#include "useful.h"
#include "tagger.h"

/*Added by Golam Mortuza Hossain */
#include "enhance_penntag.h"
/* g.m.h */


char            staart[] = "STAART";


void change_the_tag(char **theentry,char *thetag, int theposition)
//  char **theentry, *thetag;
//  int theposition;

{
  char* oldTag=theentry[theposition];

  if (oldTag!=staart) free(oldTag);
  theentry[theposition] = mystrdup(thetag);
}


char **FinalStateTagger(char **buf, int numSen, 
			Registry seenTagging, Registry words,
			Registry lemma_hash, Darray cRuleArray,
			int enhance_penntag)
{
        char            *bufp;
	char            *atempstrptr;
	//char            *p_line;	/* input line buffer */
	//char            line[MAXLINELEN];
	//char            space[500];
	char            **word_corpus_array,**tag_corpus_array;
	int            word_corpus_array_index,tag_corpus_array_index;
	char          **split_ptr,**split_ptr2;
	int             corpus_size/*, numwords*/;
	int             i, /*j,*/ count,tempcount1,tempcount2,arraySize;
	char            old[MAXTAGLEN], new_a[MAXTAGLEN], when[50],
	                tag[MAXTAGLEN], lft[MAXTAGLEN], rght[MAXTAGLEN],
	                prev1[MAXTAGLEN], prev2[MAXTAGLEN], next1[MAXTAGLEN],
	                next2[MAXTAGLEN], curtag[MAXTAGLEN],
	                curwd[MAXWORDLEN],
	                tempstr[MAXWORDLEN],word[MAXWORDLEN],/* **perl_split_ptr,*/
	                /* **perl_split_ptr2, *atempstr,*/ atempstr2[256];
/*Added by Golam Mortuza Hossain */
	char            my_lemma[MAXWORDLEN],*my_ptr;
	char            my_word [MAXWORDLEN] ;
	char            my_word_lc [MAXWORDLEN] ;
	char            my_tag [MAXTAGLEN] ;
	char            my_line[2*MAXWORDLEN+MAXTAGLEN+4] ;
/* g.m.h */

	
	
	// IMPORTANT: numSen=0 will CAUSE DISASTER!

	for(i=0,arraySize = 0; i < numSen; i++) {
	  if (not_just_blank(buf[i])){
	    arraySize +=2;
	    arraySize += num_words(buf[i]) + 1;}
	}

	word_corpus_array = (char **)malloc(sizeof(char *) * arraySize);
	tag_corpus_array = (char **)malloc(sizeof(char *) * arraySize);
	word_corpus_array_index = tag_corpus_array_index = 0;

/* read in corpus from buf.  This corpus has already been tagged (output of */
/* the start state tagger) */

	for(count=0; count<numSen; count++) {
	  if (not_just_blank(buf[count])){  
	    word_corpus_array[word_corpus_array_index++] = staart;
	    word_corpus_array[word_corpus_array_index++] = staart;
	    tag_corpus_array[tag_corpus_array_index++] = staart;
	    tag_corpus_array[tag_corpus_array_index++] = staart;
	    split_ptr = perl_split(buf[count]);
	    split_ptr2 = split_ptr;
	    while (*split_ptr != NULL) {
	      atempstrptr = strrchr(*split_ptr,'/');
	      if (atempstrptr == NULL) {
		perror("Every word in the input to the final state tagger must be tagged");
		fprintf(stderr, "Offensive line: %s\n", buf[count]);
		return(0);}
	      *atempstrptr = '\0';
	      ++atempstrptr;
	      word_corpus_array[word_corpus_array_index++] =
		mystrdup(*split_ptr);
	      tag_corpus_array[tag_corpus_array_index++] = 
		mystrdup(atempstrptr);
	      ++split_ptr;
	    }
	    free(*split_ptr2);
	    free(split_ptr2);
	  }
	}

	// read in rule from cRuleArray, and process each rule
	corpus_size = tag_corpus_array_index - 1;
	for (i=0;i<(int)Darray_len(cRuleArray);i++) {
	  split_ptr = perl_split(reinterpret_cast<char*>(Darray_get(cRuleArray,i)));
	  strcpy(old, split_ptr[0]);
	  strcpy(new_a, split_ptr[1]);
	  strcpy(when, split_ptr[2]);
	    
	  if (strcmp(when, "NEXTTAG") == 0 ||
	      strcmp(when, "NEXT2TAG") == 0 ||
	      strcmp(when, "NEXT1OR2TAG") == 0 ||
	      strcmp(when, "NEXT1OR2OR3TAG") == 0 ||
	      strcmp(when, "PREVTAG") == 0 ||
	      strcmp(when, "PREV2TAG") == 0 ||
	      strcmp(when, "PREV1OR2TAG") == 0 ||
	      strcmp(when, "PREV1OR2OR3TAG") == 0) {
	    strcpy(tag, split_ptr[3]);
	  } 
	  else if (strcmp(when, "NEXTWD") == 0 ||
		   strcmp(when, "CURWD") == 0 ||
		   strcmp(when, "NEXT2WD") == 0 ||
		   strcmp(when, "NEXT1OR2WD") == 0 ||
		   strcmp(when, "NEXT1OR2OR3WD") == 0 ||
		   strcmp(when, "PREVWD") == 0 ||
		   strcmp(when, "PREV2WD") == 0 ||
		   strcmp(when, "PREV1OR2WD") == 0 ||
		   strcmp(when, "PREV1OR2OR3WD") == 0) {
	    strcpy(word, split_ptr[3]); 
	    }
	  else if (strcmp(when, "SURROUNDTAG") == 0) {
	    strcpy(lft, split_ptr[3]);
	    strcpy(rght, split_ptr[4]);
	  } else if (strcmp(when, "PREVBIGRAM") == 0) {
	    strcpy(prev1, split_ptr[3]);
	    strcpy(prev2, split_ptr[4]);
	  } else if (strcmp(when, "NEXTBIGRAM") == 0) {
	    strcpy(next1, split_ptr[3]);
	    strcpy(next2, split_ptr[4]);
	  }else if (strcmp(when,"LBIGRAM") == 0||
		    strcmp(when,"WDPREVTAG") == 0) {
	    strcpy(prev1,split_ptr[3]);
	    strcpy(word,split_ptr[4]); 
	  } else if (strcmp(when,"RBIGRAM") == 0 ||
		     strcmp(when,"WDNEXTTAG") == 0) {
	    strcpy(word,split_ptr[3]);
	    strcpy(next1,split_ptr[4]); 
	  } else if (strcmp(when,"WDAND2BFR")== 0 ||
		     strcmp(when,"WDAND2TAGBFR")== 0) {
	    strcpy(prev2,split_ptr[3]);
	    strcpy(word,split_ptr[4]);}
	  else if (strcmp(when,"WDAND2AFT")== 0 ||
		   strcmp(when,"WDAND2TAGAFT")== 0) {
	    strcpy(next2,split_ptr[4]);
	    strcpy(word,split_ptr[3]);}
	  
	  for (count = 0; count <= corpus_size; ++count) {
	    strcpy(curtag, tag_corpus_array[count]);
	    if (strcmp(curtag, old) == 0) {
	      strcpy(curwd,word_corpus_array[count]);
	      sprintf(atempstr2,"%s %s",curwd,new_a);
	      if (! RESTRICT_MOVE || 
		  ! Registry_get(words,curwd) ||
		  Registry_get(seenTagging,atempstr2)) {
		
		if (strcmp(when, "SURROUNDTAG") == 0) {
		  if (count < corpus_size && count > 0) {
		    if (strcmp(lft, tag_corpus_array[count - 1]) == 0 &&
			strcmp(rght, tag_corpus_array[count + 1]) == 0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		} else if (strcmp(when, "NEXTTAG") == 0) {
		  if (count < corpus_size) {
		    if (strcmp(tag,tag_corpus_array[count + 1]) == 0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		}  
		else if (strcmp(when, "CURWD") == 0) {
		  if (strcmp(word, word_corpus_array[count]) == 0)
		    change_the_tag(tag_corpus_array, new_a, count);
		} 
		else if (strcmp(when, "NEXTWD") == 0) {
		  if (count < corpus_size) {
		    if (strcmp(word, word_corpus_array[count + 1]) == 0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		} 
		else if (strcmp(when, "RBIGRAM") == 0) {
		  if (count < corpus_size) {
		    if (strcmp(word, word_corpus_array[count]) ==
			0 &&
			strcmp(next1, word_corpus_array[count+1]) ==
			0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		} 
		else if (strcmp(when, "WDNEXTTAG") == 0) {
		  if (count < corpus_size) {
		    if (strcmp(word, word_corpus_array[count]) ==
			0 &&
			strcmp(next1, tag_corpus_array[count+1]) ==
			0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		}
		
		else if (strcmp(when, "WDAND2AFT") == 0) {
		  if (count < corpus_size-1) {
		    if (strcmp(word, word_corpus_array[count]) ==
			0 &&
			strcmp(next2, word_corpus_array[count+2]) ==
			0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		}
		else if (strcmp(when, "WDAND2TAGAFT") == 0) {
		  if (count < corpus_size-1) {
		    if (strcmp(word, word_corpus_array[count]) ==
			0 &&
			strcmp(next2, tag_corpus_array[count+2]) ==
			0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		}
		
		else if (strcmp(when, "NEXT2TAG") == 0) {
		  if (count < corpus_size - 1) {
		    if (strcmp(tag, tag_corpus_array[count + 2]) == 0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		} else if (strcmp(when, "NEXT2WD") == 0) {
		  if (count < corpus_size - 1) {
		    if (strcmp(word, word_corpus_array[count + 2]) == 0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		} else if (strcmp(when, "NEXTBIGRAM") == 0) {
		  if (count < corpus_size - 1) {
		    if
		      (strcmp(next1, tag_corpus_array[count + 1]) == 0 &&
		       strcmp(next2, tag_corpus_array[count + 2]) == 0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		} else if (strcmp(when, "NEXT1OR2TAG") == 0) {
		  if (count < corpus_size) {
		    if (count < corpus_size-1) 
		      tempcount1 = count+2;
		    else 
		      tempcount1 = count+1;
		    if
		      (strcmp(tag, tag_corpus_array[count + 1]) == 0 ||
		       strcmp(tag, tag_corpus_array[tempcount1]) == 0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		}  else if (strcmp(when, "NEXT1OR2WD") == 0) {
		  if (count < corpus_size) {
		    if (count < corpus_size-1) 
		      tempcount1 = count+2;
		    else 
		      tempcount1 = count+1;
		    if
		      (strcmp(word, word_corpus_array[count + 1]) == 0 ||
		       strcmp(word, word_corpus_array[tempcount1]) == 0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		}   else if (strcmp(when, "NEXT1OR2OR3TAG") == 0) {
		  if (count < corpus_size) {
		    if (count < corpus_size -1)
		      tempcount1 = count+2;
		    else 
		      tempcount1 = count+1;
		    if (count < corpus_size-2)
		      tempcount2 = count+3;
		    else 
		      tempcount2 =count+1;
		    if
		      (strcmp(tag, tag_corpus_array[count + 1]) == 0 ||
		       strcmp(tag, tag_corpus_array[tempcount1]) == 0 ||
		       strcmp(tag, tag_corpus_array[tempcount2]) == 0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		} else if (strcmp(when, "NEXT1OR2OR3WD") == 0) {
		  if (count < corpus_size) {
		    if (count < corpus_size -1)
		      tempcount1 = count+2;
		    else 
		      tempcount1 = count+1;
		    if (count < corpus_size-2)
		      tempcount2 = count+3;
		    else 
		      tempcount2 =count+1;
		    if
		      (strcmp(word, word_corpus_array[count + 1]) == 0 ||
		       strcmp(word, word_corpus_array[tempcount1]) == 0 ||
		       strcmp(word, word_corpus_array[tempcount2]) == 0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		}  else if (strcmp(when, "PREVTAG") == 0) {
		  if (count > 0) {
		    if (strcmp(tag, tag_corpus_array[count - 1]) == 0) {
		      change_the_tag(tag_corpus_array, new_a, count);
		    }
		  }
		} else if (strcmp(when, "PREVWD") == 0) {
		  if (count > 0) {
		    if (strcmp(word, word_corpus_array[count - 1]) == 0) {
		      change_the_tag(tag_corpus_array, new_a, count);
		    }
		  }
		} 
		else if (strcmp(when, "LBIGRAM") == 0) {
		  if (count > 0) {
		    if (strcmp(word, word_corpus_array[count]) ==
			0 &&
			strcmp(prev1, word_corpus_array[count-1]) ==
			0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		}
		else if (strcmp(when, "WDPREVTAG") == 0) {
		  if (count > 0) {
		    if (strcmp(word, word_corpus_array[count]) ==
			0 &&
			strcmp(prev1, tag_corpus_array[count-1]) ==
			0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		}
		else if (strcmp(when, "WDAND2BFR") == 0) {
		  if (count > 1) {
		    if (strcmp(word, word_corpus_array[count]) ==
			0 &&
			strcmp(prev2, word_corpus_array[count-2]) ==
			0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		}
		else if (strcmp(when, "WDAND2TAGBFR") == 0) {
		  if (count > 1) {
		    if (strcmp(word, word_corpus_array[count]) ==
			0 &&
			strcmp(prev2, tag_corpus_array[count-2]) ==
			0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		}
		
		else if (strcmp(when, "PREV2TAG") == 0) {
		  if (count > 1) {
		    if (strcmp(tag, tag_corpus_array[count - 2]) == 0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		} else if (strcmp(when, "PREV2WD") == 0) {
		  if (count > 1) {
		    if (strcmp(word, word_corpus_array[count - 2]) == 0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		} else if (strcmp(when, "PREV1OR2TAG") == 0) {
		  if (count > 0) {
		    if (count > 1) 
		      tempcount1 = count-2;
		    else
		      tempcount1 = count-1;
		    if (strcmp(tag, tag_corpus_array[count - 1]) == 0 ||
			strcmp(tag, tag_corpus_array[tempcount1]) == 0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		} else if (strcmp(when, "PREV1OR2WD") == 0) {
		  if (count > 0) {
		    if (count > 1) 
		      tempcount1 = count-2;
		    else
		      tempcount1 = count-1;
		    if (strcmp(word, word_corpus_array[count - 1]) == 0 ||
			strcmp(word, word_corpus_array[tempcount1]) == 0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		} else if (strcmp(when, "PREV1OR2OR3TAG") == 0) {
		  if (count > 0) {
		      if (count>1) 
			tempcount1 = count-2;
		      else 
			tempcount1 = count-1;
		      if (count >2) 
			tempcount2 = count-3;
		      else 
			tempcount2 = count-1;
		      if (strcmp(tag, tag_corpus_array[count - 1]) == 0 ||
			  strcmp(tag, tag_corpus_array[tempcount1]) == 0 ||
			  strcmp(tag, tag_corpus_array[tempcount2]) == 0)
			change_the_tag(tag_corpus_array, new_a, count);
		    }
		} else if (strcmp(when, "PREV1OR2OR3WD") == 0) {
		  if (count > 0) {
		    if (count>1) 
		      tempcount1 = count-2;
		    else 
		      tempcount1 = count-1;
		    if (count >2) 
		      tempcount2 = count-3;
		    else 
		      tempcount2 = count-1;
		    if (strcmp(word, word_corpus_array[count - 1]) == 0 ||
			strcmp(word, word_corpus_array[tempcount1]) == 0 ||
			strcmp(word, word_corpus_array[tempcount2]) == 0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		} else if (strcmp(when, "PREVBIGRAM") == 0) {
		  if (count > 1) {
		    if (strcmp(prev2, tag_corpus_array[count - 1]) == 0 &&
			strcmp(prev1, tag_corpus_array[count - 2]) == 0)
		      change_the_tag(tag_corpus_array, new_a, count);
		  }
		}
		else 
		  fprintf(stderr,
			  "ERROR: %s is not an allowable transform type\n",
			  when);
	      }
	    }
	  }  /* end: for (count =0; count <= corpus_size; ++count) */
	  free(*split_ptr);      // leak plugged by Benjamin Han
	  free(split_ptr);
	}  /* end: for (i=0;i<Darray_get(cRuleArray);i++) */

        i=0;
        bufp = buf[i];
	bufp[0]=0;

        for (count = 0; count <= corpus_size; ++count) {
          strcpy(tempstr, tag_corpus_array[count]);
          if (strcmp(tempstr,"STAART")==0 && 
	      strcmp(tag_corpus_array[count + 1],"STAART")==0 &&
	      count) {
            ++i;
            bufp = buf[i];
	    bufp[0]=0;
          } else if (strcmp(tempstr,"STAART")) {
/*Added by Golam Mortuza Hossain */
	    strcpy(my_word, word_corpus_array[count]);
	    strcpy(my_tag, tempstr);
	    my_ptr = reinterpret_cast<char*>(Registry_get(lemma_hash, my_word)) ;
            if ( my_ptr ) strcpy ( my_lemma, my_ptr);
	    else	
	    	{
	    	strcpy(my_word_lc, my_word); mylc(my_word_lc) ;
	    	my_ptr = reinterpret_cast<char*>(Registry_get(lemma_hash, my_word_lc)) ;
          	
		if ( my_ptr ) strcpy ( my_lemma, my_ptr);
	   	else	strcpy ( my_lemma, "<unknown>" ) ;
	    	}

	    if ( enhance_penntag )
	         {
		 my_ptr = 
		 EnhancePennTag( my_line, my_word, my_tag, my_lemma) ;
		 bufp = strcat(bufp, my_ptr);
		 }
	   else	
	   	 {
		    bufp = strcat(bufp, my_word);
        	    bufp = strcat(bufp, "/");
        	    bufp = strcat(bufp, my_tag);
		    bufp = strcat(bufp, " ");
	   	 }
/*g.m.h */

	    }
        }

	// Benjamin Han: Gee... these puppies need to be freed...
	for (i=0;i<arraySize;i++)
	  {
	    if (word_corpus_array[i]!=staart) free(word_corpus_array[i]);
	    if (tag_corpus_array[i]!=staart && tag_corpus_array[i]!=new_a)
	      free(tag_corpus_array[i]);
	  }
	free(word_corpus_array);
	free(tag_corpus_array);
	
	return buf;
}
