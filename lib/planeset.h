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
* FILE : planeset.h
* Purpose : Header file for the PlaneSet structure.
* Description : The PlaneSet structure describes a set of planes. It
*               is primarily used to store the planes an Actor's
*               Model has to be clipped with for future processing.
********************************************************************/

#ifndef PLANESET_H
#define PLANESET_H

#include "plane.h"

/* Only define EXPAND_SIZE if this is included from the original C
 * file. */
#ifdef PLANESET_C
#define EXPAND_SIZE 10
#endif

struct PlaneSet
{
	int	nAlloc;					/* Number of planes allocated for. */
	int	nCount;					/* Number of planes maintained. */
	struct Plane	*arPlanes;	/* Array containing the actual
										 * planes. */
};

/* PlaneSet_Construct(pThis),
 * PlaneSet_ConstructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Initializes a PlaneSet structure, sets the allocation to 0.
 */
void PlaneSet_Construct(struct PlaneSet *pThis);
#define PlaneSet_ConstructM(pThis)\
(	(pThis)->nAlloc = 0,\
	(pThis)->nCount = 0,\
	(pThis)->arPlanes = NULL\
)

/* PlaneSet_Destruct(pThis),
 * PlaneSet_DestructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Frees all memory used IN the structure, doesn't free the
 * pointer itself.
 */
void PlaneSet_Destruct(struct PlaneSet *pThis);
#define PlaneSet_DestructM(pThis)\
	if ((pThis)->arPlanes != NULL)\
	{	free((pThis)->arPlanes);\
	}

/* PlaneSet_Expand(pThis),
 * Expands the number of allocated planes in pThis by EXPAND_SIZE.
 * Returns 1 if succesful, 0 otherwise (memory failure).
 */
int PlaneSet_Expand(struct PlaneSet *pThis);

/* PlaneSet_Add(pThis, pPlane),
 * PlaneSet_AddM(pThis, pPlane),
 * Adds a new Plane to the PlaneSet structure.
 * Returns 1 if succesful, 0 otherwise (memory failure).
 */
int PlaneSet_Add(struct PlaneSet *pThis, struct Plane *pPlane);
#define PlaneSet_AddM(pThis, pPlane)\
(	(pThis)->nCount < (pThis)->nAlloc ?\
	(	(pThis)->arPlanes[(pThis)->nCount] = *(pPlane),\
		((pThis)->nCount)++,\
		1\
	):(\
		PlaneSet_Expand(pThis) ?\
		(	(pThis)->arPlanes[(pThis)->nCount] = *(pPlane),\
			((pThis)->nCount)++,\
			1\
		):(\
			0	/* Mem Failure */\
		)\
	)\
)

/* PlaneSet_GetPlaneM(pThis, nIndex),
 * Retrieves a pointer to the plane at index nIndex.
 * (due to the simplicity of this function, only a macro version
 * is available.)
 */
#define PlaneSet_GetPlaneM(pThis, nIndex)\
	(&(pThis)->arPlanes[(nIndex)])

/* PlaneSet_GetCountM(pThis),
 * Retrieves the number of planes in the set.
 * (due to the simplicity of this function, only a macro version
 * is available.)
 */
#define PlaneSet_GetCountM(pThis)\
	((pThis)->nCount)

 #endif
