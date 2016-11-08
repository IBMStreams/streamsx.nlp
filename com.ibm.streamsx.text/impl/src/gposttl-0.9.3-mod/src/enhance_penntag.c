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

#include "enhance_penntag.h"

char * mylc( char *word )
{
	int ch,i=0 ;
	while ((ch=word[i]) != '\0')
	{
	word[i] = tolower(word[i]) ;
	i++;
	}
	word[i] = '\0' ;
	return &word[0] ;
}

char * EnhancePennTag (char *buf, char *word, char *tag, char *lemma )
{
	const char *verb_map[] = {
		"VB", "VH", "VV", 
		"VBD", "VHD", "VVD", 
		"VBG", "VHG", "VVG", 
		"VBN", "VHN", "VVN", 
		"VBP", "VHP", "VVP", 
		"VBZ", "VHZ", "VVZ" } ; 
	
	const char *tag_map[] = {
		"PRP", "PP",
		"PRP$", "PP$",
		"NNP", "NP",
		"NNPS", "NPS",
		".", "SENT" } ;

	const char *have_verbs[] =
		{"have", "has", "had", "having", 
		"'ve", "'d" } ;
	
	const char *be_verbs[] =
		{"be", "been", "being", "am", 
		"is", "are", "was", "were", "'m" } ;

	int i=0, j=0;
	char sgml1[2]= "<", sgml2[2]= ">" ;

/* */
	char	my_word [MAXWORDLEN] ;
	strcpy(my_word, word);

/* Check for sgml tag <XXXX> */
	if ( strstr(word,&sgml1[0]) && strstr(word,&sgml2[0]) )
		{
		sprintf( buf,"%s\n", word ) ;
		return &buf[0] ;
		}
/* If lemma is "<unknown>" then substitute word in lemma */
	if ( strcmp(lemma, "<unknown>")==0 && 
		strcmp(tag, "NNPS") !=0 && strcmp(tag, "NNS") !=0 && 
		strcmp(tag, "VBG") !=0 && strcmp(tag, "VBN") !=0 && 
		strcmp(tag, "VBZ") !=0 && strcmp(tag, "VBD") !=0 )

		strcpy(lemma, mylc(my_word)) ;

/* perform tag mapping */
	for ( i=0; i<5; i++ )
	{
	if ( strcmp(tag, tag_map[2*i])==0 )
		{
		strcpy(tag,tag_map[2*i+1]) ;
		sprintf( buf,"%s\t%s\t%s\n", word, tag, lemma ) ;
		return &buf[0] ;
		}
	}
/* Now perform verb mapping */
	for ( i=0; i<6; i++ )
	{
	if ( strcmp(tag, verb_map[3*i])==0 )
 	 {
		/* For have */
		for (j=0;j<6;j++)
		{
		if ( strcmp(mylc(my_word), have_verbs[j])==0 )
			{
			strcpy(tag,verb_map[3*i+1]) ;
			strcpy(lemma,have_verbs[0]) ;
			sprintf( buf,"%s\t%s\t%s\n", word, tag, lemma ) ;
			return &buf[0] ;
			}
		}
		/* For be */
		for (j=0;j<9;j++)
		{
		if ( strcmp(mylc(my_word), be_verbs[j])==0 )
			{
//			strcpy(tag,verb_map[3*i]) ;
			strcpy(lemma, be_verbs[0]) ;
			sprintf( buf,"%s\t%s\t%s\n", word, tag, lemma ) ;
			return &buf[0] ;
			}
		}
		/* For everything else */
		strcpy(tag,verb_map[3*i+2]) ;
		sprintf( buf,"%s\t%s\t%s\n", word, tag, lemma ) ;
		return &buf[0] ;
		
	 }
	}



	sprintf( buf,"%s\t%s\t%s\n", word, tag, lemma ) ;
	return &buf[0] ;
}




