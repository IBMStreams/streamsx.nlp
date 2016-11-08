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

#ifndef _SYSDEP_H_
#define _SYSDEP_H_

#define NORET void

/* CONSTVOIDP is for pointers to non-modifyable void objects */

#ifdef __STDC__
typedef const void * CONSTVOIDP;
typedef void * VOIDP;
#define NOARGS void
#define PROTOTYPE(x) x
#else
typedef char * VOIDP;
typedef char * CONSTVOIDP;
#define NOARGS
#define PROTOTYPE(x) ()
#endif /* __STDC__ */

#endif /* ifndef _SYSDEP_H_ */
