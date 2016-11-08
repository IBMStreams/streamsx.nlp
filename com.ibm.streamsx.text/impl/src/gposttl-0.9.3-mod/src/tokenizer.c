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
#include<stdio.h>
#include<stdlib.h>

#include "tokenizer.h"

// replace
char *replace(char *src, const char *replace, const char *to)
{

        char *dest = NULL;
        char *retstr = NULL/*, *tmp*/;
		
        if(src == NULL ) return NULL;

//returning the src if replace is NULL
	if(replace == NULL || to == NULL) 
	{
	dest = (char *)malloc(strlen(src) + 1);
        if(dest == NULL) return NULL;
	else strcpy(dest, src); 
	return dest;
	}

//if replace is found in src
	if((retstr = strstr(src, replace)))
	{
	int beforelen = retstr - src;
        int replacelen = strlen(replace);
        int tolen = strlen(to);
	int n=1 ;

// First check number of occarence 	
	while ((retstr = strstr( &src[beforelen + replacelen], replace)))
		{ 
		beforelen = retstr - src; 
		n++ ;
		}

// Allocate memory	
	dest = (char *)malloc( (strlen(src) + 1 + n*(tolen - replacelen)));
	if(dest == NULL) return NULL;

// Replace all occurence
	strcpy(dest,"");
	retstr = strstr(src, replace) ;
	do	
		{
		beforelen = retstr - src;
		retstr = src ;
		strncat(dest, retstr, beforelen);
		strcat(dest, to);
		retstr = &src[beforelen + replacelen] ;	
		src = retstr ;
		}
	while ((retstr = strstr( retstr, replace))) ;
	
	strcat( dest, src) ; 
	return dest;
	}
 	
	else //return the string itself
	{
	dest = (char *)malloc(strlen(src) + 1);
	if(dest == NULL) return NULL;
	else strcpy(dest, src); 
	return dest;
	}
}

char *Tokenizer ( char *buff )
{

	const char *token_list[] = {
		"\"", " \" ",
		",", " , ",
		";", " ; ",
		":", " : ",
		"? ", " ? ",
		"! ", " ! ",
		". ", " . ",
	    
		"[", " [ ",
		"]", " ] ",
		" (", " ( ",
		") ", " ) ",
		").", " ).",
//		"(", " ( ",
//		")", " ) ",
//		"{", " { ",
//		"}", " } ",
		"<", " < ",
		">", " > ",
		"--", " -- ",

		"'s ", " 's ",
		"'S ", " 'S ",
		"'m ", " 'm ",
		"'M ", " 'M ",
		"'d ", " 'd ",
		"'D ", " 'D ",
		"'ll ", " 'll ",
		"'re ", " 're ",
		"'ve ", " 've ",
		" can't ", " can n't ",
		" Can't ", " Can n't ",
		"n't ", " n't ",
		"'LL ", " 'LL ",
		"'RE ", " 'RE ",
		"'VE ", " 'VE ",
		"N'T ", " N'T ",
	
		" Cannot ", " Can not ",
		" cannot ", " can not ",
		" D'ye ", " D' ye ",
		" d'ye ", " d' ye ",
		" Gimme ", " Gim me ",
		" gimme ", " gim me ",
		" Gonna ", " Gon na ",
		" gonna ", " gon na ",
		" Gotta ", " Got ta ",
		" gotta ", " got ta ",
		" Lemme ", " Lem me ",
		" lemme ", " lem me ",
		" More'n ", " More 'n ",
		" more'n ", " more 'n ",
		"'Tis ", " 'T is ",
		"'tis ", " 't is ",
		"'Twas ", " 'T was ",
		"'twas ", " 't was ",
		" Wanna ", " Wan na ",
		" wanna ", " wanna ",
	
	/* Since "." requires special treatment.
	first lets replace all ". " by " . " */
//		". ", " . ",
	
	/* Now correct for known cases */
		
		"Let 's ", "Let's ",
	
		".. .", " ...",
		"Adm .", "Adm.",
		"Aug .", "Aug.",
		"Ave .", "Ave.",
		"Brig .", "Brig.",
		"Bros .", "Bros.",
		"CO .", "CO.", 
		"CORP .", "CORP.",
		"COS .", "COS.", 
		"Capt .", "Capt.",
		"Co .", "Co.",
		"Col .", "Col.",
		"Colo .", "Colo.",
		"Corp .", "Corp.",
		"Cos .", "Cos.",
		"Dec .", "Dec.",
		"Del .", "Del.",
		"Dept .", "Dept.",
		"Dr .", "Dr.",
		"Drs .", "Drs.",
		"Etc .", "Etc.",
		"Feb .", "Feb.",
		"Ft .", "Ft.",
		"Ga .", "Ga.",
		"Gen .", "Gen.",
		"Gov .", "Gov.",
		"Hon .", "Hon.",
		"INC .", "INC.",
		"Inc .", "Inc.",
		"Ind .", "Ind.",
		"Jan .", "Jan.",
		"Jr .", "Jr.",
		"Kan .", "Kan.",
		"Ky .", "Ky.",
		"La .", "La.",
		"Lt .", "Lt.",
		"Ltd .", "Ltd.",
		"Maj .", "Maj.",
		"Md .", "Md.",
		"Messrs .", "Messrs.",
		"Mfg .", "Mfg.",
		"Miss .", "Miss.",
		"Mo .", "Mo.",
		"Mr .", "Mr.",
		"Mrs .", "Mrs.",
		"Ms .", "Ms.",
		"Nev .", "Nev.",
		"No .", "No.",
		"Nos .", "Nos.",
		"Nov .", "Nov.",
		"Oct .", "Oct.",
		"Ph .", "Ph.",
		"Prof .", "Prof.",
		"Prop .", "Prop.",
		"Pty .", "Pty.",
		"Rep .", "Rep.",
		"Reps .", "Reps.",
		"Rev .", "Rev.",
		"S.p.A .", "S.p.A.",
		"Sen .", "Sen.",
		"Sens .", "Sens.",
		"Sept .", "Sept.",
		"Sgt .", "Sgt.",
		"Sr .", "Sr.",
		"St .", "St.",
		"Va .", "Va.",
		"Vt .", "Vt.",
		"U.S .", "U.S.",
		"Wyo .", "Wyo.",
		"a.k.a .", "a.k.a.",
		"a.m .", "a.m.",
		"cap .", "cap.",
		"e.g .", "e.g.",
		"eg .", "eg.",
		"etc .", "etc.",
		"ft .", "ft.",
		"i.e .", "i.e.",
		"p.m .", "p.m.",
		"v .", "v.",
		"v.B .", "v.B.",
		"v.w .", "v.w.",
		"vs .", "vs.",
		"__END__", "__END__" }; /* Just to stop loop */
	
	char *src, *dest;
	int i=0/*, len*/;

	src = (char *)malloc(strlen(buff) + 1);
	if(src == NULL) return buff;
	strcpy( src, buff) ;
	
	while (strcmp(token_list[2*i],"__END__") != 0 )
	{
	dest = replace( src, token_list[2*i], token_list[2*i+1]) ;
	free(src) ; 
	if ( dest == NULL ) return buff;	
	
	src = dest; i++ ;
	}
	
	return src ;
}

