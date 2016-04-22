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
* FILE : scvtxset.c
********************************************************************/

#define SCVTXSET_C

#include <stdlib.h>

#include "scvtxset.h"

/********************************************************************
* Function : ScreenVertexSet_Construct()
* Purpose : Initializes a ScreenVertexSet structure.
* Pre : pThis points to a ScreenVertexSet structure.
* Post : pThis points to an initialized ScreenVertexSet structure,
*        containing 0 screen vertices.
********************************************************************/
void ScreenVertexSet_Construct(struct ScreenVertexSet *pThis)
{	/* Call the macro version */
	ScreenVertexSet_ConstructM(pThis);
}

/********************************************************************
* Function : ScreenVertexSet_Destruct()
* Purpose : Frees all memory associated with the ScreenVertexSet
*           structure, does NOT free the ScreenVertexSet structure
*           itself.
* Pre : pThis points to an initialized ScreenVertexSet structure.
* Post : pThis points to an invalid ScreenVertexSet structure that
*        has no memory allocated.
********************************************************************/
void ScreenVertexSet_Destruct(struct ScreenVertexSet *pThis)
{	/* Call the macro version */
	ScreenVertexSet_DestructM(pThis);
}

/********************************************************************
* Function : ScreenVertexSet_Expand()
* Purpose : Expands the allocation space in a ScreenVertexSet
*           structure.
* Pre : pThis points to an initializes ScreenVertexSet structure
* Post : If the returnvalue is 1, pThis points to an initialized
*        ScreenVertexSet with EXPAND_SIZE more screen vertices
*        available.
*        If the returnvalue is 0, a memory allocation failure
*        occured.
********************************************************************/
int ScreenVertexSet_Expand(struct ScreenVertexSet *pThis)
{
	struct ScreenVertex *p;
	int n;
	
	/* Allocate new space. */
	p = (struct ScreenVertex *)malloc(sizeof(struct ScreenVertex) * (pThis->nAlloc + EXPAND_SIZE));
	
	/* Check for a memory failure. */
	if (p != NULL)
	{
		/* Check if there was a previous arScrVertices array. */
		if (pThis->arScrVertices != NULL)
		{
			/* Copy the old to the new. */
			for (n = 0; n < pThis->nCount; n++)
			{	p[n] = pThis->arScrVertices[n];
			}
			
			/* Free the old. */
			free((void *)pThis->arScrVertices);
		}
		
		/* Set the new. */
		pThis->arScrVertices = p;
		pThis->nAlloc += EXPAND_SIZE;
		
		return 1;
	} else
	{	return 0;	/* Memory allocation failure. */
	}
}

/********************************************************************
* Function : ScreenVertexSet_Add()
* Purpose : Adds a new ScreenVertex to a ScreenVertexSet structure.
* Pre : pThis points to an initialized ScreenVertexSet structure.
* Post : If the returnvalue is 1, pThis now contains the new 
*        ScreenVertex.
*        If the returnvalue is 0, a memory allocation failure
*        occured.
********************************************************************/
int ScreenVertexSet_Add(struct ScreenVertexSet *pThis,
								struct ScreenVertex *pScrVertex)
{	/* Call the macro version */
	return ScreenVertexSet_AddM(pThis, pScrVertex);
}

/********************************************************************
* Function : ScreenVertexSet_GetNew()
* Purpose : Creates space for a new ScreenVertex structure.
* Pre : pThis points to an initialized ScreenVertexSet structure.
* Post : If the returnvalue != NULL, it is a pointer to the new
*        ScreenVertex structure (which has NOT been initialized!).
*        If the returnvalue == NULL, a memory allocation failure 
*        occured.
********************************************************************/
struct ScreenVertex *ScreenVertexSet_GetNew(struct ScreenVertexSet *pThis)
{	/* Call the macro version */
	return ScreenVertexSet_GetNewM(pThis);
}
