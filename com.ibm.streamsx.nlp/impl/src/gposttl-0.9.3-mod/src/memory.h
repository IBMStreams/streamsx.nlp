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

#ifndef _Memory_h_
#define _Memory_h_

#include <stddef.h>

#include "sysdep.h"

#ifdef __STDC__
extern VOIDP Memory_allocate(size_t);
extern VOIDP Memory_reallocate(VOIDP, size_t);
extern NORET Memory_free(VOIDP);
extern long Memory_unfreed_bytes(NOARGS);
#else
extern VOIDP Memory_allocate();
extern VOIDP Memory_reallocate();
extern NORET Memory_free();
extern long Memory_unfreed_bytes(NOARGS);
#endif /* __STDC__ */

#endif
