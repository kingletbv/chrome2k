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
* FILE : actptset.c
********************************************************************/

#define ACTPTSET_C

#include <stdlib.h>

#include "actptset.h"

/********************************************************************
* Function : ActorPtrSet_Construct()
* Purpose : Initializes an ActorPtrSet structure.
* Pre : pThis points to an ActorPtrSet structure.
* Post : pThis points to an initialized ActorPtrSet structure 
*        containing 0 pointers.
********************************************************************/
void ActorPtrSet_Construct(struct ActorPtrSet *pThis)
{	/* Call the macro version. */
	ActorPtrSet_ConstructM(pThis);
}

/********************************************************************
* Function : ActorPtrSet_Destruct()
* Purpose : Frees all memory of an ActorPtrSet structure, does NOT
*           free the ActorPtrSet structure itself.
* Pre : pThis points to an initialized ActorPtrSet structure.
* Post : pThis points to an invalid ActorPtrSet structure that has
*        no memory allocated.
********************************************************************/
void ActorPtrSet_Destruct(struct ActorPtrSet *pThis)
{	/* Call the macro version. */
	ActorPtrSet_DestructM(pThis);
}

/********************************************************************
* Function : ActorPtrSet_Expand()
* Purpose : Expands the allocation space in an ActorPtrSet.
* Pre : pThis points to an initialized ActorPtrSet structure.
* Post : If the returnvalue is 1, pThis points to an initialized
*        ActorPtrSet structure with EXPAND_SIZE more pointers
*        available.
*        If the returnvalue is 0, a memory failure occured.
********************************************************************/
int ActorPtrSet_Expand(struct ActorPtrSet *pThis)
{
	struct Actor **p;
	int n;
	
	/* Allocate new space. */
	p = (struct Actor **)malloc(sizeof(struct Actor *) * (pThis->nAlloc + EXPAND_SIZE));
	
	/* Check for memory failure. */
	if (p != NULL)
	{	
		/* Check if there was a previous arPtrs array. */
		if (pThis->arPtrs != NULL)
		{
			/* Copy the old to the new. */
			for (n = 0; n < pThis->nCount; n++)
				p[n] = pThis->arPtrs[n];
			
			/* Free the old. */
			free((void *)pThis->arPtrs);
		}
		
		/* Set the new. */
		pThis->arPtrs = p;
		pThis->nAlloc += EXPAND_SIZE;

		/* Clear the new. */
		for (n = pThis->nCount; n < pThis->nAlloc; n++)
			p[n] = NULL;
				
		return 1;
	} else
	{	return 0;	/* Memory allocation failure. */
	}
}

/********************************************************************
* Function : ActorPtrSet_Add()
* Purpose : Adds a new Actor pointer to an ActorPtrSet.
* Pre : pThis points to an initialized ActorPtrSet structure.
* Post : If the returnvalue is 1, pThis now contains the new pointer.
*        If the returnvalue is 0, a memory failure occured.
********************************************************************/
int ActorPtrSet_Add(struct ActorPtrSet *pThis, struct Actor *pActor)
{	/* Call the macro version. */
	return ActorPtrSet_AddM(pThis, pActor);
}

/********************************************************************
* Function : ActorPtrSet_AtLeast()
* Purpose : Guarantees that there are at least nLeast entries
*           available in the ActorPtrSet pThis.
* Pre : pThis points to an initialized ActorPtrSet structure, nLeast
*       specifies the minimally required number of entries.
* Post : If the returnvalue is 1, pThis now contains at least nLeast
*        ActorPtr entries.
*        If the returnvalue is 0, a memory allocation failure 
*        occurred.
********************************************************************/
int ActorPtrSet_AtLeast(struct ActorPtrSet *pThis, int nLeast)
{
	int n;
	struct Actor **p;
	/* Check the current allocation count. */
	if (pThis->nAlloc < nLeast)
	{	/* Need to allocate more. */
		p = (struct Actor **)malloc(sizeof(struct Actor *) * nLeast);
	
		/* Check for memory failure. */
		if (p != NULL)
		{	
			/* Check if there was a previous arPtrs array. */
			if (pThis->arPtrs != NULL)
			{
				/* Copy the old to the new. */
				for (n = 0; n < pThis->nCount; n++)
					p[n] = pThis->arPtrs[n];
			
				/* Free the old. */
				free((void *)pThis->arPtrs);
			}
		
			/* Set the new. */
			pThis->arPtrs = p;
			pThis->nAlloc = nLeast;
	
			/* Clear the new. */
			for (n = pThis->nCount; n < pThis->nAlloc; n++)
				p[n] = NULL;
				
			return 1;
		} else
		{	return 0;	/* Memory allocation failure. */
		}
	} else
	{	return 1;		/* Already enough entries available. */
	}
}