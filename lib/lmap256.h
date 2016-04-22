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
* FILE : lmap256.h
* Purpose : Header file for the Lightmap256 structure.
* Description : The Lightmap256 structure describes 256 colormap
*               indices sorted on gradient. It forms the link between
*               intensity and colormap indices.
********************************************************************/

#ifndef LMAP256_H
#define LMAP256_H

#include "octree.h"

struct Lightmap256
{
	int	nRefCount;					/* Number of references to this
											 * Lightmap256, when this hits 0,
											 * the structure is destroyed. */
	struct Lightmap256	*pNext;	/* Pointer to next Lightmap256 for
											 * linked hash lists. */
	struct Lightmap256	*pLNext;	/* Pointer to next Lightmap256 for
											 * normal linked lists. */
	unsigned long	ulRGB0;			/* 0xRRGGBB unsigned long color.
											 * Specifies the RGB value at index
											 * 0. */
	unsigned long	ulRGB255;		/* 0xRRGGBB unsigned long color.
											 * Specifies the RGB value at index
											 * 255. */
	unsigned char	arIndices[256];
											/* Colormap indices. */
};

/* Lightmap256_ConstructM(pThis),
 * Initializes a Lightmap256 structure. The structure will have it's
 * RefCount initialized to 0, so add a reference before using the structure.
 * (doesn't initialize the indices). */
#define Lightmap256_ConstructM(pThis)\
(	(pThis)->nRefCount = 0,\
	(pThis)->pNext = NULL,\
	(pThis)->pLNext = NULL,\
	(pThis)->ulRGB0 = 0x000000,\
	(pThis)->ulRGB255 = 0xFFFFFF\
)

/* Lightmap256_AttachM(pThis),
 * Increments the reference count in a Lightmap256. */
#define Lightmap256_AttachM(pThis)\
(	(pThis)->nRefCount++\
)

/* Lightmap256_ReleaseM(pThis),
 * Decrements the reference count in a Lightmap256. */ 
#define Lightmap256_ReleaseM(pThis)\
(	(pThis)->nRefCount--\
)

/* Lightmap256_RequestColors(pThis, pOctree),
 * Adds the colors required for the lightmap to an Octree.
 * This doesn't assign the colors, which is done by
 * Lightmap256_AssignColorIndices().
 */
int Lightmap256_RequestColors(struct Lightmap256 *pThis,
										struct Octree *pOctree);

/* Lightmap256_AssignColorIndices(pThis, pOctree),
 * Assigns colormap indices to the entries of pThis. */
void Lightmap256_AssignColorIndices(struct Lightmap256 *pThis,
												struct Octree *pOctree);

#endif
