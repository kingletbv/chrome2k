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
* FILE : indexset.h
* Purpose : Header file for the IndexSet structure.
* Description : The IndexSet is used for maintaining a collection of
*               indices (integers). It handles things like memory
*               allocation. It is used for specifying the vertices
*               of a polygon, the polygons attached to a hyperplane
*               in a BSP tree, etc.
********************************************************************/

#ifndef INDEXSET_H
#define INDEXSET_H

/* Only specify EXPAND_SIZE if this is from the original C file. */
#ifdef INDEXSET_C	
#define EXPAND_SIZE 10
#endif

struct IndexSet
{
	int	nAlloc;				/* Number of indices allocated for. */
	int	nCount;				/* Number of indices maintained. */
	int	*arIndices;			/* Array containing the actual indices. */
};


/* IndexSet_Construct(pThis),
 * IndexSet_ConstructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Initializes an IndexSet structure, sets the allocation to 0. 
 */
void IndexSet_Construct(struct IndexSet *pThis);
#define IndexSet_ConstructM(pThis)\
(	(pThis)->nAlloc = 0,\
	(pThis)->nCount = 0,\
	(pThis)->arIndices = NULL\
)

/* IndexSet_Destruct(pThis),
 * IndexSet_DestructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Frees all memory associated IN the structure, doesn't free the
 * pointer itself.
 */
void IndexSet_Destruct(struct IndexSet *pThis);
#define IndexSet_DestructM(pThis)\
	if ((pThis)->arIndices != NULL)\
	{	free((pThis)->arIndices);\
	}

/* IndexSet_Clone(pThis, pClone),
 * IndexSet_CloneM(pThis, pClone), (REDUNDANT MACRO)
 * Makes a clone of pThis in pClone.
 * If the returnvalue is 1, the clone was succesful,
 * if the returnvalue is 0, a memory failure occured.
 */
int IndexSet_Clone(struct IndexSet *pThis, struct IndexSet *pClone);
#define IndexSet_CloneM(pThis, pClone)\
	IndexSet_Clone(pThis, pClone)

/* IndexSet_Expand(pThis),
 * Expands the number of allocated indices in pThis by EXPAND_SIZE.
 * Returns 1 if succesful, 0 otherwise (memory allocation failure).
 */
int IndexSet_Expand(struct IndexSet *pThis);

/* IndexSet_Add(pThis, nIndex),
 * IndexSet_AddM(pThis, nIndex),
 * Adds a new index to the IndexSet structure.
 * Returns 1 if succesful, 0 otherwise (memory allocation failure).
 */
int IndexSet_Add(struct IndexSet *pThis, int nIndex);
#define IndexSet_AddM(pThis, nIndex)\
(	(pThis)->nCount < (pThis)->nAlloc ?\
	(	(pThis)->arIndices[(pThis)->nCount] = (nIndex), ((pThis)->nCount)++,1\
	):(\
		IndexSet_Expand(pThis) ?\
		(	(pThis)->arIndices[(pThis)->nCount] = (nIndex), ((pThis)->nCount)++,1\
		):(\
			0 /* Mem Failure */\
		)\
	)\
)

/* IndexSet_GetIndexM(pThis, nIndex),
 * Retrieves the index at index nIndex.
 * (due to the simplicity of this function, only a macro version is available.)
 */
#define IndexSet_GetIndexM(pThis, nIndex)\
	((pThis)->arIndices[(nIndex)])

/* IndexSet_GetCountM(pThis),
 * Retrieves the number of indices in the set.
 * (due to the simplicity of this function, only a macro version is available.)
 */
#define IndexSet_GetCountM(pThis)\
	((pThis)->nCount)

#endif
