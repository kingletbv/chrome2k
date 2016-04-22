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
* FILE : scvtxset.h
* Purpose : Header file for the ScreenVertexSet structure.
* Description : The ScreenVertexSet structure describes a set of
*               ScreenVertex structures. They're used for the
*					 transition from 3D to 2D.
********************************************************************/

#ifndef SCVTXSET_H
#define SCVTXSET_H

#include "scrvertx.h"

/* Only specify EXPAND_SIZE if this is from the original C file. */
#ifdef SCVTXSET_C
#define EXPAND_SIZE 10
#endif

struct ScreenVertexSet
{
	int	nAlloc;				/* Number of screen vertices allocated for. */
	int	nCount;				/* Number of screen vertices maintained. */
	struct ScreenVertex	*arScrVertices;
									/* Array containing the actual vertices. */
};

/* ScreenVertexSet_Construct(pThis),
 * ScreenVertexSet_ConstructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Initializes a ScreenVertexSet structure, sets the allocation to 0.
 */
void ScreenVertexSet_Construct(struct ScreenVertexSet *pThis);
#define ScreenVertexSet_ConstructM(pThis)\
(	(pThis)->nAlloc = 0,\
	(pThis)->nCount = 0,\
	(pThis)->arScrVertices = NULL\
)

/* ScreenVertexSet_Destruct(pThis),
 * ScreenVertexSet_DestructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Frees all memory associated with the structure, doesn't free the
 * pointer itself.
 */
void ScreenVertexSet_Destruct(struct ScreenVertexSet *pThis);
#define ScreenVertexSet_DestructM(pThis)\
	if ((pThis)->arScrVertices != NULL)\
	{	free((pThis)->arScrVertices);\
	}

/* ScreenVertexSet_Expand(pThis),
 * Expands the number of allocated screen vertices in pThis by EXPAND_SIZE.
 * Returns 1 if succesful, 0 otherwise (memory allocation failure).
 */
int ScreenVertexSet_Expand(struct ScreenVertexSet *pThis);

/* ScreenVertexSet_Add(pThis, pScrVertex),
 * ScreenVertexSet_AddM(pThis, pScrVertex),
 * Adds a new ScreenVertex structure to the ScreenVertexSet structure.
 * Returns 1 if succesful, 0 otherwise (memory allocation failure).
 */
int ScreenVertexSet_Add(struct ScreenVertexSet *pThis,
								struct ScreenVertex *pScrVertex);
#define ScreenVertexSet_AddM(pThis, pScrVertex)\
(	(pThis)->nCount < (pThis)->nAlloc ?\
	(	(pThis)->arScrVertices[(pThis)->nCount] = *(pScrVertex),\
		((pThis)->nCount)++,\
		1\
	):(\
		ScreenVertexSet_Expand(pThis) ?\
		(	(pThis)->arScrVertices[(pThis)->nCount] = *(pScrVertex),\
			((pThis)->nCount)++,\
			1\
		):(\
			0	/* Mem failure */ \
		)\
	)\
)

/* ScreenVertexSet_GetNew(pThis),
 * ScreenVertexSet_GetNewM(pThis), (NEEDS stdlib.h INCLUDED)
 * Creates space for a new ScreenVertex structure. Does NOT initialize the
 * structure! Returns a pointer to the new ScreenVertex structure, if this
 * is NULL, a memory allocation failure occured.
 */
struct ScreenVertex *ScreenVertexSet_GetNew(struct ScreenVertexSet *pThis);
#define ScreenVertexSet_GetNewM(pThis)\
(	(pThis)->nCount < (pThis)->nAlloc ?\
	(	&((pThis)->arScrVertices[((pThis)->nCount)++])\
	):(\
		ScreenVertexSet_Expand(pThis) ?\
		(	&((pThis)->arScrVertices[((pThis)->nCount)++])\
		):(\
			NULL	/* Mem failure */ \
		)\
	)\
)
		

/* ScreenVertexSet_GetScreenVertexM(pThis, nIndex),
 * Retrieves a pointer to the ScreenVertex at index nIndex.
 * (due to the simplicity of this function, only a macro version is
 *  available).
 */
#define ScreenVertexSet_GetScreenVertexM(pThis, nIndex)\
	(&(pThis)->arScrVertices[(nIndex)])

/* ScreenVertexSet_GetCountM(pThis),
 * Retrieves the number of screenvertices in the set.
 * (due to the simplicity of this function, only a macro version is
 *  available).
 */
#define ScreenVertexSet_GetCountM(pThis)\
	((pThis)->nCount)

#endif
