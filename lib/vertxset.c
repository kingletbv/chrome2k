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
* FILE : vertxset.c
********************************************************************/

#define VERTXSET_C

#include <stdlib.h>

#include "vertxset.h"

/********************************************************************
* Function : VertexSet_Construct()
* Purpose : Initializes a VertexSet structure.
* Pre : pThis points to an VertexSet structure.
* Post : pThis points to an initialized VertexSet structure,
*        containing 0 vertices.
********************************************************************/
void VertexSet_Construct(struct VertexSet *pThis)
{
	/* Just call the macro version. */
	VertexSet_ConstructM(pThis);
}

/********************************************************************
* Function : VertexSet_Destruct()
* Purpose : Frees all memory associated with the VertexSet structure,
*           does NOT free the VertexSet structure itself.
* Pre : pThis points to an initialized VertexSet structure.
* Post : pThis points to an invalid VertexSet structure that has no
*        memory allocated.
********************************************************************/
void VertexSet_Destruct(struct VertexSet *pThis)
{
	/* Just call the macro version. */
	VertexSet_DestructM(pThis);
}

/********************************************************************
* Function : VertexSet_Expand()
* Purpose : Expands the allocation space in a VertexSet.
* Pre : pThis points to an initialized VertexSet structure.
* Post : If the returnvalue is 1, pThis points to an initialized
*        VertexSet structure with EXPAND_SIZE more vertices
*        available.
*        If the returnvalue is 0, a memory failure occured.
********************************************************************/
int VertexSet_Expand(struct VertexSet *pThis)
{
	struct Vertex *p;
	int n;
	
	/* Allocate new space. */
	p = (struct Vertex *)malloc(sizeof(struct Vertex) * (pThis->nAlloc + EXPAND_SIZE));
	
	/* Check for memory failure. */
	if (p != NULL)
	{
		/* Check if there was a previous arVertices array. */
		if (pThis->arVertices != NULL)
		{
			/* Copy the old to the new */
			for (n = 0; n < pThis->nCount; n++)
				p[n] = pThis->arVertices[n];
			
			/* Free the old. */
			free(pThis->arVertices);
		}
		
		/* Set the new. */
		pThis->arVertices = p;
		pThis->nAlloc += EXPAND_SIZE;		

		return 1;
	} else
	{	return 0;
	}
}

/********************************************************************
* Function : VertexSet_Add()
* Purpose : Adds a new vertex to a VertexSet.
* Pre : pThis points to an initialized VertexSet structure.
* Post : If the returnvalue is 1, pThis now contains the new vertex.
*        If the returnvalue is 0, a memory failure occured.
********************************************************************/
int VertexSet_Add(struct VertexSet *pThis, struct Vertex *pVertex)
{
	/* Just redirect to the macro version. */
	return VertexSet_AddM(pThis, pVertex);
}
