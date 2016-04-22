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
* FILE : indexset.c
********************************************************************/

#define INDEXSET_C

#include <stdlib.h>

#include "indexset.h"

/********************************************************************
* Function : IndexSet_Construct()
* Purpose : Initializes an IndexSet.
* Pre : pThis points to an IndexSet structure.
* Post : pThis points to an initialized IndexSet structure,
*        containing 0 indices.
********************************************************************/
void IndexSet_Construct(struct IndexSet *pThis)
{	/* Just call the macro version to handle these things for us. */
	IndexSet_ConstructM(pThis);
}

/********************************************************************
* Function : IndexSet_Destruct()
* Purpose : Frees all memory associated with an IndexSet, does NOT
*            free the structure itself.
* Pre : pThis points to an initialized IndexSet structure.
* Post : pThis points to an invalid IndexSet structure that has no
*        memory allocated.
********************************************************************/
void IndexSet_Destruct(struct IndexSet *pThis)
{	/* Just call the macro version to handle these things for us. */
	IndexSet_DestructM(pThis);
}

/********************************************************************
* Function : IndexSet_Clone()
* Purpose : Clones an indexset into another indexset.
* Pre : pThis points to an initialized IndexSet structure, pClone
*       points to an initialized IndexSet structure.
* Post : If the returnvalue is 1, pClone now is an IndexSet with the
*        same contents as pThis but without using the same memory.
*        If the returnvalue is 0, a memory allocation failure
*        occured. (pThis remains the same, pClone is unstable.)
********************************************************************/
int IndexSet_Clone(struct IndexSet *pThis, struct IndexSet *pClone)
{
	int n;
	/* Allocate new space for the clone. */
	if ((pClone->nAlloc) < (pThis->nCount))
	{	/* New space is required, pClone doesn't have enough space
		 * to hold the clone. */
		if (pClone->arIndices != NULL)
		{	/* Free existing space. */
			free((void *)pClone->arIndices);
		}
		pClone->arIndices = (int *)malloc(sizeof(int) * pThis->nCount);
		pClone->nAlloc = pThis->nCount;
	}
		
	/* Check for a memory failure. */
	if (pClone->arIndices != NULL)
	{
		/* Clone the contents. */
		for (n = 0; n < pThis->nCount; n++)
		{
			pClone->arIndices[n] = pThis->arIndices[n];
		}
		
		/* Set the number of indices in the clone. */
		pClone->nCount = pThis->nCount;
	
		return 1;
	} else
	{
#ifdef DEBUGC
		printf("IndexSet_Clone() -> MemFailure at point #0\n");
		printf("\tFailure caused by attempt for %d indices.\n", pThis->nAlloc);
#endif
		return 0;		/* Memory allocation failure. */
	}
}

/********************************************************************
* Function : IndexSet_Expand()
* Purpose : Expands the allocation space in an IndexSet.
* Pre : pThis points to an initialized IndexSet structure.
* Post : if the returnvalue is 1, pThis points to an initialized
*        IndexSet structure with EXPAND_SIZE more indices available.
*        If the returnvalue is 0, a memory failure occured.
********************************************************************/
int IndexSet_Expand(struct IndexSet *pThis)
{
	int *p;
	int n;
	
	/* Allocate new space. */
	p = (int *)malloc(sizeof(int) * (pThis->nAlloc + EXPAND_SIZE));
	
	/* Check for memory failure. */
	if (p != NULL)
	{
		/* Check if there was a previous arIndices array. */
		if (pThis->arIndices != NULL)
		{
			/* Copy the old to the new. */
			for (n = 0; n < pThis->nCount; n++)
				p[n] = pThis->arIndices[n];
			
			/* Free the old. */
			free(pThis->arIndices);
		}
	
		/* Set the new. */
		pThis->arIndices = p;
		pThis->nAlloc += EXPAND_SIZE;
		
		return 1;
	} else
	{	return 0;
	}
}

/********************************************************************
* Function : IndexSet_Add()
* Purpose : Adds a new index to the IndexSet.
* Pre : pThis points to an initialized IndexSet structure.
* Post : If the return value is 1, pThis now contains the new index.
*        If the return value is 0, the function failed due to lack
*        of memory.
********************************************************************/
int IndexSet_Add(struct IndexSet *pThis, int nIndex)
{
	/* Just redirect to the macro version. */
	return IndexSet_AddM(pThis, nIndex);
}
