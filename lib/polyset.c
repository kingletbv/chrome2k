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
* FILE : polyset.c
********************************************************************/

#define POLYSET_C

#include <stdlib.h>

#include "polyset.h"

/********************************************************************
* Function : PolySet_Construct()
* Purpose : Initializes a PolySet.
* Pre : pThis points to a PolySet structure.
* Post : pThis points to an initialized PolySet structure with 0
*        polygons.
********************************************************************/
void PolySet_Construct(struct PolySet *pThis)
{
	/* Just call the macro version. */
	PolySet_ConstructM(pThis);
}

/********************************************************************
* Function : PolySet_Destruct()
* Purpose : Destroys a PolySet, calling Destruct for all polygons.
* Pre : pThis points to an initialized PolySet structure.
* Post : pThis has been fully cleaned up and is now invalid. pThis
*        itself has not been freed.
********************************************************************/
void PolySet_Destruct(struct PolySet *pThis)
{
	/* Cannot create a macro version for this because of the loop
	 * below, which requires a variable. The macro version calls this
	 * function. */
	/* Clean up all polygons contained in this PolySet. */
	int n;
	for (n = 0; n < PolySet_GetCountM(pThis); n++)
	{
		Polygon_DestructM(PolySet_GetPolygonM(pThis, n));
	}
	
	if (pThis->arPolygons != NULL)
		free(pThis->arPolygons);
}

/********************************************************************
* Function : PolySet_Expand(pThis)
* Purpose : Expands the allocated space in a PolySet.
* Pre : pThis points to an initialized PolySet structure.
* Post : If the returnvalue is 1, pThis points to an initialized
*        PolySet with EXPAND_SIZE more polygons available.
*        If the returnvalue is 0, a memory failure occured.
********************************************************************/
int PolySet_Expand(struct PolySet *pThis)
{
	struct Polygon *p;
	int  n;
	
	/* Allocate new space. */
	p = (struct Polygon *)malloc(sizeof(struct Polygon) * (pThis->nAlloc + EXPAND_SIZE));

	/* Set the unused polygon indexset index pointers to NULL.
	 * This enables reusability of previously allocated polygon indexsets
	 * which are not used anymore. */
	for (n = pThis->nCount; n < (pThis->nAlloc + EXPAND_SIZE); n++)
	{	Polygon_ConstructM(&(p[n]));
	}

	/* Check for memory failure. */
	if (p != NULL)
	{
		/* Check if there was a previous arPolygons array. */
		if (pThis->arPolygons != NULL)
		{
			/* Copy the old to the new. */
			for (n = 0; n < pThis->nCount; n++)
			{
				/* Because we're going to throw away the old array, we don't
				 * need to clone the polygons, we can just copy them, including
				 * all there pointers. */
				p[n] = pThis->arPolygons[n];
			}
			
			/* Free the old. */
			free(pThis->arPolygons);
		}
		
		/* Set the new. */
		pThis->arPolygons = p;
		pThis->nAlloc += EXPAND_SIZE;
		
		return 1;
	} else
	{
		return 0;
	}
}

/********************************************************************
* Function : PolySet_Add()
* Purpose : Adds a new polygon the PolySet.
* Pre : pThis points to an initialized pThis PolySet structure,
*       pPolygon points to an initialized Polygon structure.
* Post : pThis points to an initialized pThis structure now including
*        a cloned version of pPolygon.
********************************************************************/
int PolySet_Add(struct PolySet *pThis, struct Polygon *pPolygon)
{
	if (pThis->nCount < pThis->nAlloc)
	{	/* There are enough unused polygons available. */
		/* Clone the polygon in here. */
		if (Polygon_CloneM(&(pThis->arPolygons[pThis->nCount]), pPolygon))
		{	/* Increment the internal polygon count. */
			(pThis->nCount)++;
			return 1;
		} else
		{	/* A memory failure occured. */
#ifdef DEBUGC
			printf("PolySet_Add() -> Memfailure at point #0\n");
#endif
			return 0;
		}
	} else
	{	/* We need more unused polygons. */
		if (PolySet_Expand(pThis))
		{	/* We now have more unused polygons. */
			/* Clone the polygon in here. */
			if (Polygon_CloneM(&(pThis->arPolygons[pThis->nCount]), pPolygon))
			{	/* Increment the internal polygon count. */
				(pThis->nCount)++;
				return 1;
			} else
			{	/* A memory failure occured during cloning. */
#ifdef DEBUGC
				printf("PolySet_Add() -> Memfailure at point #1\n");
#endif
				return 0;
			}
		} else
		{	/* A memory failure occured during polyset expansion. */
#ifdef DEBUGC
			printf("PolySet_Add() -> Memfailure at point #2\n");
#endif
			return 0;
		}
	}
}

/********************************************************************
* Function : PolySet_GetNew()
* Purpose : Creates a new Polygon structure in a PolySet structure.
* Pre : pThis points to an initialized PolySet structure.
* Post : If the returnvalue != NULL, returnvalue is a pointer to the
*        new Polygon structure. This polygon structure is initialized
*        and contains no vertices. It may however already have memory
*        allocated to it, if the PolySet has been reused.
*        If the returnvalue is NULL, a memory allocation failure
*        occured.
********************************************************************/
struct Polygon *PolySet_GetNew(struct PolySet *pThis)
{
	/* Check if there is enough memory. */
	if (pThis->nCount < pThis->nAlloc)
	{
		/* Reset the polygon at index nCount. */
		pThis->arPolygons[pThis->nCount].Vertices.nCount = 0;
		pThis->arPolygons[pThis->nCount].pLightmap = NULL;
		/* Increment nCount. */
		pThis->nCount ++;
		
		return &(pThis->arPolygons[pThis->nCount - 1]);
	} else
	{	/* Not enough memory, so allocate some more first. */
		if (PolySet_Expand(pThis))
		{
			/* Reset the polygon at index nCount. */
			pThis->arPolygons[pThis->nCount].Vertices.nCount = 0;
			pThis->arPolygons[pThis->nCount].pLightmap = NULL;
			/* Increment nCount. */
			pThis->nCount ++;
			
			return &(pThis->arPolygons[pThis->nCount - 1]);
		} else
		{	return NULL;		/* Memory allocation failure. */
		}
	}
}