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
* FILE : colormgr.h
* Purpose : Header file for the ColorManager structure.
* Description : The ColorManager structure manages the 256 color
*               palette and all the lightmaps that are used with it.
********************************************************************/

#ifndef COLORMGR_H
#define COLORMGR_H

#include "octree.h"
#include "lmap256.h"
#include "lmap1.h"

/* Hashkey modulo size for Lightmap256 structures. */
#define LIGHTMAP256_HASH	127
#define LIGHTMAP1_HASH		127

struct ColorManager
{
	struct Octree	ColOctree;	/* Octree containing all colors,
										 * used for color quantization. */

	unsigned long	Colormap[256];	/* Colormap structure containing RGBs
											 * for the color indices. */
	
	/* Hashtable for the Lightmap256 structures managed by
	 * this Colormanager. */
	struct Lightmap256	*pLmap256Head;	/* Linked by pLNext. */
	struct Lightmap256	*Lmap256Hash[LIGHTMAP256_HASH];

	/* Hashtable for the Lightmap1 structures managed by
	 * this ColorManager. */
	struct Lightmap1		*pLmap1Head;	/* Linked by pLNext. */
	struct Lightmap1		*Lmap1Hash[LIGHTMAP1_HASH];
};

/* ColorManager_Construct(pThis),
 * ColorManager_ConstructM(pThis),
 * Initializes a ColorManager structure. */
void ColorManager_Construct(struct ColorManager *pThis);
#define ColorManager_ConstructM(pThis)\
	ColorManager_Construct(pThis)

/* ColorManager_Destruct(pThis),
 * Frees all memory of a ColorManager structure. */
void ColorManager_Destruct(struct ColorManager *pThis);

/* ColorManager_Lightmap256HashkeyM(ulRGB0, ulRGB255),
 * Calculates the index at which a Lightmap256 structure with
 * intensity range ulRGB0..ulRGB255 should be stored in the
 * Lmap256Hash table.
 */
#define ColorManager_Lightmap256HashkeyM(ulRGB0, ulRGB255)\
(	((ulRGB0) + (ulRGB255)) % LIGHTMAP256_HASH\
)

/* ColorManager_Lightmap1HashkeyM(ulRGB),
 * Calculates the index at which a Lightmap1 structure with
 * color ulRGB should be stored in the Lmap1Hash table. */
#define ColorManager_Lightmap1HashkeyM(ulRGB)\
(	(ulRGB) % LIGHTMAP1_HASH\
)

/* ColorManager_GetLightmap256(pThis, ulRGB0, ulRGB255),
 * Checks if a Lightmap256 structure with the intensity
 * range ulRGB0..ulRGB255 already exists, if so, it returns
 * the existing Lightmap256 structure, otherwise it builds
 * a new Lightmap256 structure with the specified 
 * intensity range and returns that.
 */
struct Lightmap256 *ColorManager_GetLightmap256(struct ColorManager *pThis,
																unsigned long ulRGB0,
																unsigned long ulRGB255);

/* ColorManager_GetLightmap1(pThis, ulRGB),
 * Checks if a Lightmap1 structure with the color ulRGB already
 * exists, if so, it returns the existing Lightmap1 structure, otherwise
 * it builds a new Lightmap1 structure with the specified RGB color and
 * returns that. */
struct Lightmap1 *ColorManager_GetLightmap1(struct ColorManager *pThis,
														  unsigned long ulRGB);

/* ColorManager_AssignColors(pThis),
 * Evaluates the colors requested and assigns indices to the colors. */
int ColorManager_AssignColors(struct ColorManager *pThis);


#endif
