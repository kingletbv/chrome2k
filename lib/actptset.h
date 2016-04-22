/* Chrome - Small 3D library...
 * Copyright (C) Martijn Boekhorst <m.boekhorst@pi.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
/********************************************************************
* FILE : actptset.h
* Purpose : Header file for the ActorPtrSet structure.
* Description : The ActorPtrSet is used to patch Actors into the
*               subspaces of a Model's BSP Tree. The ActorPtrSet is
*               found in the Actor structure where it forms the link
*               between the Model's BSP Tree and the Display BSP
*               tree.
********************************************************************/

#ifndef ACTPTSET_H
#define ACTPTSET_H

#ifndef ACTOR_H
struct Actor;
#endif

/* Only specify EXPAND_SIZE if this file is included from the original
 * C file. */
#ifdef ACTPTSET_C
#define EXPAND_SIZE 10
#endif

struct ActorPtrSet
{
	int	nAlloc;							/* Number of pointers allocated
												 * for. */
	int	nCount;							/* Number of pointers maintained.
												 */
	struct Actor	**arPtrs;			/* Array containing the actual
												 * pointers. */
};

/* ActorPtrSet_Construct(pThis),
 * ActorPtrSet_ConstructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Initializes an ActorPtrSet structure, sets the allocation to 0.
 */
void ActorPtrSet_Construct(struct ActorPtrSet *pThis);
#define ActorPtrSet_ConstructM(pThis)\
(	(pThis)->nAlloc = 0,\
	(pThis)->nCount = 0,\
	(pThis)->arPtrs = NULL\
)

/* ActorPtrSet_Destruct(pThis),
 * ActorPtrSet_DestructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Frees all memory used by an ActorPtrSet structure, doesn't free 
 * the pointer itself.
 */
void ActorPtrSet_Destruct(struct ActorPtrSet *pThis);
#define ActorPtrSet_DestructM(pThis)\
	if ((pThis)->arPtrs != NULL)\
	{	free((pThis)->arPtrs);\
	}

/* ActorPtrSet_Expand(pThis),
 * Expands the number of pointers allocated in pThis by EXPAND_SIZE.
 * Returns 1 if succesful, 0 otherwise (memory allocation failure).
 */
int ActorPtrSet_Expand(struct ActorPtrSet *pThis);

/* ActorPtrSet_Add(pThis, pActor),
 * ActorPtrSet_AddM(pThis, pActor),
 * Adds a new pointer to an Actor to the ActorPtrSet structure.
 * Returns 1 if succesful, 0 otherwise (memory allocation failure).
 */
int ActorPtrSet_Add(struct ActorPtrSet *pThis, struct Actor *pActor);
#define ActorPtrSet_AddM(pThis, pActor)\
(	(pThis)->nCount < (pThis)->nAlloc ?\
	(	(pThis)->arPtrs[(pThis)->nCount] = (pActor),\
		((pThis)->nCount)++,\
		1\
	):(\
		ActorPtrSet_Expand(pThis) ?\
		(	(pThis)->arPtrs[(pThis)->nCount] = (pActor),\
			((pThis)->nCount)++,\
			1\
		):(\
			0	/* Mem failure */ \
		)\
	)\
)

/* ActorPtrSet_AtLeast(pThis, nLeast),
 * Guarantees that there are at least nLeast entries available in
 * the ActorPtrSet pThis.
 */
int ActorPtrSet_AtLeast(struct ActorPtrSet *pThis, int nLeast);

/* ActorPtrSet_GetActorPtrM(pThis, nIndex),
 * Retrieves the Actor pointer at index nIndex.
 * (due to the simplicity of this function, only a macro version is
 *  available.)
 */
#define ActorPtrSet_GetActorPtrM(pThis, nIndex)\
	((pThis)->arPtrs[(nIndex)])

/* ActorPtrSet_GetCountM(pThis),
 * Retrieves the number of Actor pointers in the set.
 * (due to the simplicity of this function, only a macro version is
 *  available.)
 */
#define ActorPtrSet_GetCountM(pThis)\
	((pThis)->nCount)


#endif
