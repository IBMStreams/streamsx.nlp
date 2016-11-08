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

#ifndef _BOOL_H_
#define _BOOL_H_

/* Note: It is an abstraction violation to use the C "boolean"
 * operators (&&, ||, etc.) on Bool objects.  Those operators
 * really operate on type "int".
 *
 * Note: Bool_and() and Bool_or() do not short circuit.
 * Bool_cand() and Bool_cor() do.  
 */ 

/* The identifier en_Bool and the values are private */

#ifdef Bool
#undef Bool
#endif

#ifdef __STDC__
typedef enum Bool_en {Bool_FALSE=0, Bool_TRUE=1} Bool;
#else
typedef int Bool;
#define Bool_FALSE 0
#define Bool_TRUE  1
#endif /* __STDC__ */

/* The macro expansions are private */

#define Bool_to_int(a) ((int)a)
#define Bool_to_char(a) ((char)a)
#define Bool_create(integral_value) (((integral_value) != 0)?(Bool_TRUE):(Bool_FALSE))
/* Some type conversions are omitted for efficiency (with stupid compilers) */
#define Bool_or(a,b) ((a)|(b))
#define Bool_and(a,b) ((a)&(b))
#define Bool_cor(a,b) ((a)||(b))
#define Bool_cand(a,b) ((a)&&(b))
#define Bool_not(a) ((a)^(Bool_TRUE))

#endif
