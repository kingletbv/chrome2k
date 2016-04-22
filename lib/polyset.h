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
* FILE : polyset.h
* Purpose : Header file for the PolySet structure.
* Description : The PolySet structure describes a set of polygons.
*               It handles things like memory allocation and is
*               much like the VertexSet except it contains
*               Polygon structures.
********************************************************************/

#ifndef POLYSET_H
#define POLYSET_H

#include "polygon.h"

/* Only specify EXPAND_SIZE if this is from the original C file. */
#ifdef POLYSET_C
#define EXPAND_SIZE 10
#endif

struct PolySet
{
	int	nAlloc;								/* Number of polygons allocated
													 * for. */
	int	nCount;								/* Number of polygons
													 * maintained. */
	struct Polygon	*arPolygons;			/* Array containing actual
													 * indices. */
};

/* PolySet_Construct(pThis),
 * PolySet_ConstructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Initializes a PolySet structure, set the allocation to 0.
 */
void PolySet_Construct(struct PolySet *pThis);
#define PolySet_ConstructM(pThis)\
(	(pThis)->nAlloc = 0,\
	(pThis)->nCount = 0,\
	(pThis)->arPolygons = NULL\
)

/* PolySet_Destruct(pThis),
 * PolySet_DestructM(pThis), (REDUNDANT MACRO)
 * Frees all memory associated IN the structure, doesn't free the
 * pointer itself.
 * Note that there is no macro version of this function available
 * due to an internal loop that is required to clean up all
 * seperate polygon structures.
 */
void PolySet_Destruct(struct PolySet *pThis);
#define PolySet_DestructM(pThis)\
	PolySet_Destruct(pThis)

/* PolySet_Expand(pThis),
 * Expands the number of allocated polygons in pThis by EXPAND_SIZE.
 * Returns 1 if succesful, 0 otherwise (memory allocation failure).
 */
int PolySet_Expand(struct PolySet *pThis);

/* PolySet_Add(pThis, pPolygon),
 * PolySet_AddM(pThis, pPolygon), (REDUNDANT MACRO)
 * Adds a new polygon to a PolySet structure. Note that the polygon is fully
 * duplicated and thus doesn't use the same allocated space as pPolygon in
 * the PolySet.
 * Note that there is no macro version of this function available
 * due to the extra duplication logic required.
 */
int PolySet_Add(struct PolySet *pThis, struct Polygon *pPolygon);
#define PolySet_AddM(pThis, pPolygon)\
	PolySet_Add(pThis, pPolygon)

/* PolySet_GetNew(pThis),
 * PolySet_GetNewM(pThis), (REDUNDANT MACRO)
 * Creates & Constructs a new Polygon structure in the PolySet structure. The
 * polygon will be initialized and may already have some memory allocated
 * to it.
 * Call this function to create new polygons in a polyset. A pointer is
 * returned to the new polygon.
 * If the returnvalue is NULL, a memory allocation failure occured. */
struct Polygon *PolySet_GetNew(struct PolySet *pThis);
#define PolySet_GetNewM(pThis)\
	PolySet_GetNew(pThis)

/* PolySet_GetPolygonM(pThis, nIndex),
 * Retrieves a pointer to the polygon at index nIndex.
 */
#define PolySet_GetPolygonM(pThis, nIndex)\
(	&((pThis)->arPolygons[(nIndex)])\
)

/* PolySet_GetCountM(pThis),
 * Returns the number of polygons in the set.
 */
#define PolySet_GetCountM(pThis)\
(	(pThis)->nCount\
)

#endif
