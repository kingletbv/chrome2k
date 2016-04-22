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
* FILE : planeset.c
********************************************************************/

#define PLANESET_C

#include <stdlib.h>

#include "planeset.h"

/********************************************************************
* Function : PlaneSet_Construct()
* Purpose : Initializes a PlaneSet structure.
* Pre : pThis points to a PlaneSet structure.
* Post : pThis points to an initialized PlaneSet structure,
*        containing 0 planes.
********************************************************************/
void PlaneSet_Construct(struct PlaneSet *pThis)
{	/* Call the macro version. */
	PlaneSet_ConstructM(pThis);
}

/********************************************************************
* Function : PlaneSet_Destruct()
* Purpose : Frees all memory associated with a PlaneSet structure,
*           this does NOT free the PlaneSet structure itself.
* Pre : pThis points to an initialized PlaneSet structure.
* Post : pThis points to an invalid PlaneSet structure that has no
*        memory allocated.
********************************************************************/
void PlaneSet_Destruct(struct PlaneSet *pThis)
{	/* Call the macro version. */
	PlaneSet_DestructM(pThis);
}

/********************************************************************
* Function : PlaneSet_Expand()
* Purpose : Expands the allocation space in a PlaneSet.
* Pre : pThis points to an initialized PlaneSet structure.
* Post : If the returnvalue is 1, pThis points to an initialized
*        PlaneSet structure with EXPAND_SIZE more planes available.
*        If the returnvalue is 0, a memory failure occured.
********************************************************************/
int PlaneSet_Expand(struct PlaneSet *pThis)
{
	struct Plane *p;
	int n;
	
	/* Allocate new space. */
	p = (struct Plane *)malloc(sizeof(struct Plane) * (pThis->nAlloc + EXPAND_SIZE));
	
	/* Check for memory failure. */
	if (p != NULL)
	{
		/* Check if there was a previous arPlanes array. */
		if (pThis->arPlanes != NULL)
		{
			/* Copy the old to the new. */
			for (n = 0; n < pThis->nCount; n++)
				p[n] = pThis->arPlanes[n];
			
			/* Free the old. */
			free(pThis->arPlanes);
		}
		
		/* Set the new. */
		pThis->arPlanes = p;
		pThis->nAlloc += EXPAND_SIZE;
		
		return 1;
	} else
	{	/* Memory failure. */
		return 0;
	}
}

/********************************************************************
* Function : PlaneSet_Add()
* Purpose : Adds a new Plane to a PlaneSet.
* Pre : pThis points to an initialized PlaneSet structure.
* Post : If the returnvalue is 1, pThis now contains the new plane.
*        If the returnvalue is 0, a memory failure occured.
********************************************************************/
int PlaneSet_Add(struct PlaneSet *pThis, struct Plane *pPlane)
{	/* Call the macro version. */
	return PlaneSet_AddM(pThis, pPlane);
}
