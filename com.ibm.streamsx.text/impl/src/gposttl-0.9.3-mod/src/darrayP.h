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

#ifndef _darrayP_h_
#define _darrayP_h_

#include "sysdep.h"
#include "memory.h"

#include "darray.h"

typedef struct st_Darray {
  unsigned length;
  VOIDP *storage;
  unsigned storage_offset;
  unsigned storage_length;
} Darray_rep;

#define draise(p_to_rep) ((Darray)p_to_rep)
#define dlower(obj) ((Darray_rep *)obj)
#define dcreate() ((Darray_rep *)Memory_allocate(sizeof(Darray_rep)))
/*#define destroy(p_to_rep) (Memory_free(p_to_rep))*/

#define MAX_GROW_STEP 100	/* Defines the maximum amount to increase
				   storage_length when storage is full.
				   A call to Darray_hint can cause a larger
				   grow to occur; this only applies to 
				   "on-demand grows" */

enum grow_direction {GROW_HIGH, GROW_LOW};

#ifdef __STDC__
static NORET grow(Darray_rep *, enum grow_direction, unsigned);
#else
static NORET grow();
#endif /* __STDC__ */

/* Invariant conditions:
 * 
 * Each array component (object) is stored in a void pointer.
 *
 * [storage_length] >= 1
 * 
 * [storage] is pointer to a a heap object large enough for
 * [storage_length] pointers to void,
 *
 * [storage_length] >= [length]
 * 
 * The n-th element (index=n-1) of the dynamic array is located at 
 * address [storage]+[storage_offset]+n-1.
 *
 */ 
#endif /* ifndef _darrayP_h_ */
