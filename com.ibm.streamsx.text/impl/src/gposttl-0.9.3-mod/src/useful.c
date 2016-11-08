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
#include "useful.h"

char *mystrdup(char *thestr) 
{
  return((char *)strcpy((char *)malloc((strlen(thestr)+1)*(sizeof(char))),thestr));
}

int not_just_blank(char *thestr)
//char *thestr;
/* make sure not just processing a no-character line */
{ 
  char *thestr2;
  thestr2 = thestr;
  while(*thestr2 != '\0') {
    if (*thestr2 != ' ' && *thestr2 != '\t' && *thestr2 != '\n'){
      return(1); }
    ++thestr2;
  }
  return(0);
}

int num_words(char *thestr)
//  char *thestr;
{
  int count,returncount;
  
  returncount=0;
  count=0;
  while (thestr[count] != '\0' && (thestr[count] == ' ' 
	 || thestr[count] == '\t')) ++count;
  while (thestr[count++] != '\0') {
    if (thestr[count-1] == ' ' || thestr[count-1] == '\t') {
      ++returncount;
      while (thestr[count] == ' ' || thestr[count] == '\t')
	++count;
      if (thestr[count] == '\0') --returncount;
    }
  }
  return(returncount);
}
