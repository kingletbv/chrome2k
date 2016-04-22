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
* FILE : lmap1.h
* Purpose : Header file for the Lightmap1 structure.
* Description : The Lightmap1 structure describes a single colormap
*               index. It is primarily used for polygons that have 
*               ambient lighting only and therefore don't change in
*               color.
********************************************************************/

#ifndef LMAP1_H
#define LMAP1_H

#include "octree.h"

struct Lightmap1
{
	int	nRefCount;					/* Number of references to this
											 * Lightmap1, when this hits 0,
											 * the structure is destroyed. */
	struct Lightmap1		*pNext;	/* Pointer to next Lightmap1 for
											 * linked hash lists. */
	struct Lightmap1		*pLNext;	/* Pointer to next Lightmap1 for
											 * normal linked lists. */
	unsigned long	ulRGB;			/* 0xRRGGBB unsigned long color.
											 * Specifies the RGB value */
	unsigned char	nIndex;			/* Colormap index. */
};

/* Lightmap1_ConstructM(pThis),
 * Initializes a Lightmap1 structure. The structure will have it's
 * RefCount initialized to 0, so add a reference before using the structure.
 * (doesn't initialize the index). */
#define Lightmap1_ConstructM(pThis)\
(	(pThis)->nRefCount = 0,\
	(pThis)->pNext = NULL,\
	(pThis)->pLNext = NULL,\
	(pThis)->ulRGB = 0xFFFFFF,\
	(pThis)->nIndex = 255\
)

/* Lightmap1_AttachM(pThis),
 * Increments the reference count in a Lightmap256. */
#define Lightmap1_AttachM(pThis)\
(	(pThis)->nRefCount++\
)

/* Lightmap1_ReleaseM(pThis),
 * Decrements the reference count in a Lightmap256. */ 
#define Lightmap1_ReleaseM(pThis)\
(	(pThis)->nRefCount--\
)

/* Lightmap1_RequestColorM(pThis, pOctree) (NEEDS octree.h INCLUDED),
 * Adds the colors required for the lightmap to an Octree.
 * This doesn't assign the colors, which is done by
 * Lightmap1_AssignColorIndexM().
 */
#define Lightmap1_RequestColorM(pThis, pOctree)\
(	Octree_AddColor((pOctree),	(pThis)->ulRGB)\
)

/* Lightmap1_AssignColorIndexM(pThis, pOctree) (NEEDS octree.h INCLUDED),
 * Assigns a colormap index to the entries of pThis. */
#define Lightmap1_AssignColorIndexM(pThis, pOctree)\
(	(pThis)->nIndex = Octree_FindColorIndex((pOctree), (pThis)->ulRGB)\
)

#endif
