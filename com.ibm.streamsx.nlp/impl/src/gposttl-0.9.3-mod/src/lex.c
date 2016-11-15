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
/* Added by Golam Mortuza Hossain */
#include <stdlib.h>
#include <string.h>
/*g.m.h */
#include "useful.h"
#include "lex.h"

char *append_with_space(char *w1, char *w2)
//     char *w1,*w2;
{
  char *result;
  
  result = 
    (char *)malloc((strlen(w1) + strlen(w2)+2) * sizeof(char)); 
  strcpy(result,w1);
  strcat(result," ");
  strcat(result,w2);
  return(result); }




char *append_with_char(char *w1, char *w2, char w3)
{
  char *result;
  
  result = 
    (char *)malloc((strlen(w1) + strlen(w2)+2) * sizeof(char)); 
  sprintf(result,"%s%c%s",w1,w3,w2);
  return(result);
}
  


char **perl_split(char *buf)
//     char *buf;
{
  char **return_buf;
  int cntr = 0;
  char *temp,*temp2;

  temp2 = (char *)malloc(sizeof(char)*(1+strlen(buf)));
  while(*buf == ' ' || *buf == '\t') ++buf;
  strcpy(temp2,buf);
  return_buf = (char **) malloc(sizeof(char *) * ((numspaces(temp2)+1) + 2));
  return_buf[cntr++] = (char *)strtok(temp2," \t");
  while ((temp = (char *)strtok(NULL," \t"))) 
	if (temp != NULL) {
		return_buf[cntr] = temp;
		++cntr;}
  return_buf[cntr] = NULL;
  return(return_buf); }


// THIS IS REALLY STRANGE! Brill allocates two additional blocks, and
// use one of them to store NULL?
char **perl_split_independent(char *buf)
//     char *buf;
{
  char **return_buf;
  int cntr = 0;
  char *temp;

  while(*buf == ' ' || *buf == '\t') ++buf;
  return_buf = (char **) malloc(sizeof(char *) * (numspaces(buf)+3));

  return_buf[cntr++] = (char *)mystrdup((char *)strtok(buf," \t"));
  while ((temp = (char *)strtok(NULL,"\t ")) != NULL) {
    return_buf[cntr] =(char *)mystrdup(temp);
    ++cntr;
  }
  return_buf[cntr] = NULL;

  return(return_buf); }




char **perl_split_on_char(char *buf, char achar)
{
  char **return_buf;
  int cntr = 0;
  char *temp,temp2[2],*temp3;

  temp3 = (char *)malloc(sizeof(char)*(1+strlen(buf)));
  temp2[0] = achar; temp2[1] = '\0';
  return_buf = (char **) malloc(sizeof(char *) * ((numchars(temp3,achar)+1) + 2));
  return_buf[cntr++] = (char *)strtok(temp3,temp2);
  while ((temp = (char *)strtok(NULL,temp2))) 
	if (temp != NULL) {
		return_buf[cntr] = temp;
		++cntr;}
  return_buf[cntr] = NULL;
  return(return_buf); }




char **perl_split_on_nothing(char *buf)
//     char *buf;
{
  char **return_buf;
  int cntr;
  char *temp2;  

  temp2 = (char *)malloc(sizeof(char)*(1+strlen(buf)));
  strcpy(temp2,buf);
  
  return_buf = (char **) malloc(sizeof(char *) * (strlen(buf)+1));
  for (cntr = 0; cntr < (int)strlen(buf); ++cntr) {
    return_buf[cntr] = (char *)malloc(sizeof(char)*2);
    return_buf[cntr][0] = temp2[cntr];
    return_buf[cntr][1] = '\0'; }
    return_buf[cntr] = NULL;
    return(return_buf); 
}

char *perl_explode(char *buf)
//     char *buf;
{
  char *return_buf;
  int cntr;
 

  
  return_buf = (char *) malloc(sizeof(char) * ((strlen(buf)*2)+1));
  for (cntr = 0; (cntr/2) < (int)strlen(buf); cntr+=2) {
    return_buf[cntr] = buf[cntr/2];
    return_buf[cntr+1] = ' '; }
    return_buf[cntr-1] = '\0';
    return(return_buf); 
}

char **perl_free(char **ptr)
//     char **ptr;
    
{

/*  while (ptr[count] != NULL) {
    free(ptr[count]);
    count++;
  }}*/

  free(ptr[0]); 

  return ptr;
}



int numspaces(char *buf)
//     char *buf;
{
  int tot,count;
  tot = 0;
  for (count = 0; count < (int)strlen(buf); ++count) 
    if (buf[count]==' ')
      ++tot;
  return(tot); }
    
int numchars(char *buf,char achar)
//     char *buf,achar;
{
  int tot,count;
  tot = 0;
  for (count = 0; count < (int)strlen(buf); ++count) 
    if (buf[count]== achar)
      ++tot;
  return(tot); }
    


char *return_tag(char *theword)
//     char *theword;
{
  char *tempword;
  tempword = (char *)strchr(theword,'/');
  if (tempword != NULL)  return (tempword+1); 
  else return(NULL); }


char *before_tag(char *theword)
//     char *theword;
{
  int count = 0;
  
  while (theword[count] != '\0' &&
	 theword[count] != '/')
    count++;
  if (theword[count] == '/')
    theword[count] = '\0';
  return(theword); }



