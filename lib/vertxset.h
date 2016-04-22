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
* FILE : vertxset.h
* Purpose : Header file for the VertexSet structure.
* Description : The VertexSet is used for maintaining a collection
*               of vertices. It handles things like memory allocation
*               and is much like the IndexSet except it contains
*               Vertex structures instead of indices.
********************************************************************/

#ifndef VERTXSET_H
#define VERTXSET_H

#include "vertex.h"

/* Only specify EXPAND_SIZE if this is from the original C file. */
#ifdef VERTXSET_C
#define EXPAND_SIZE 10
#endif

struct VertexSet
{
	int	nAlloc;					/* Number of vertices allocated for. */
	int	nCount;					/* Number of vertices maintained. */
	struct Vertex *arVertices;	/* Array containing the actual
										 * vertices. */
};

/* VertexSet_Construct(pThis),
 * VertexSet_ConstructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Initializes a VertexSet structure, sets the allocation to 0.
 */
void VertexSet_Construct(struct VertexSet *pThis);
#define VertexSet_ConstructM(pThis)\
(	(pThis)->nAlloc = 0,\
	(pThis)->nCount = 0,\
	(pThis)->arVertices = NULL\
)

/* VertexSet_Destruct(pThis),
 * VertexSet_DestructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Frees all memory associated IN the structure, doesn't free the
 * pointer itself.
 */
void VertexSet_Destruct(struct VertexSet *pThis);
#define VertexSet_DestructM(pThis)\
	if ((pThis)->arVertices != NULL)\
	{	free((pThis)->arVertices);\
	}

/* VertexSet_Expand(pThis),
 * Expands the number of allocated vertices in pThis by EXPAND_SIZE.
 * Returns 1 if succesful, 0 otherwise (memory allocation failure).
 */
int VertexSet_Expand(struct VertexSet *pThis);

/* VertexSet_Add(pThis, pVertex),
 * VertexSet_AddM(pThis, pVertex),
 * Adds a new vertex to the VertexSet structure.
 * Returns 1 if succesful, 0 otherwise (memory allocation failure).
 */
int VertexSet_Add(struct VertexSet *pThis, struct Vertex *pVertex);
#define VertexSet_AddM(pThis, pVertex)\
(	(pThis)->nCount < (pThis)->nAlloc ?\
	(	(pThis)->arVertices[(pThis)->nCount] = *(pVertex), ((pThis)->nCount)++,1\
	):(\
		VertexSet_Expand(pThis) ?\
		(	(pThis)->arVertices[(pThis)->nCount] = *(pVertex), ((pThis)->nCount)++,1\
		):(\
			0 /* Mem failure */\
		)\
	)\
)

/* VertexSet_GetVertexM(pThis, nIndex),
 * Retrieves a pointer to the vertex at index nIndex.
 * (due to the simplicity of this function, only a macro version is
 *  available.)
 */
#define VertexSet_GetVertexM(pThis, nIndex)\
	(&(pThis)->arVertices[(nIndex)])

/* VertexSet_GetCountM(pThis),
 * Retrieves the number of vertices in the set.
 * (due to the simplicity of this function, only a macro version is
 *  available.)
 */
#define VertexSet_GetCountM(pThis)\
	((pThis)->nCount)

#endif
