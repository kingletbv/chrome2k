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
* FILE : lmap256.c
********************************************************************/

#define LMAP256_C

#include "lmap256.h"

/********************************************************************
* Function : Lightmap256_RequestColors()
* Purpose : Adds all intensity colors of Lightmap256 pThis to an
*           Octree.
*           The colors are calculated from the ulRGB value in pThis.
* Pre : pThis points to the Lightmap256 that should add it's colors.
*       pOctree points to the Octree to which the colors should be
*       added.
* Post : If the returnvalue is 1, pOctree now contains all the
*        colors.
*        If the returnvalue is 0, a memory allocation failure
*        occured.
********************************************************************/
int Lightmap256_RequestColors(struct Lightmap256 *pThis,
				struct Octree *pOctree)
{
	int n;
	unsigned long ulRGB;
	unsigned int nRed0, nGreen0, nBlue0;
	unsigned int nRed255, nGreen255, nBlue255;
	unsigned int nRed, nGreen, nBlue;

	nRed0 = (pThis->ulRGB0 >> 16) & 0xFF;
	nGreen0 = (pThis->ulRGB0 >> 8) & 0xFF;
	nBlue0 = pThis->ulRGB0 & 0xFF;

	nRed255 = (pThis->ulRGB255 >> 16) & 0xFF;
	nGreen255 = (pThis->ulRGB255 >> 8) & 0xFF;
	nBlue255 = (pThis->ulRGB255) & 0xFF;

	/* Interpolate RGB color from ulRGB0 to ulRGB255 and
	 * add each color to the octree. */
	for (n = 0; n < 256; n++)
	{	
		nRed = nRed0 + (((nRed255 - nRed0) * n) / 256);
		nGreen = nGreen0 + (((nGreen255 - nGreen0) * n) / 256);
		nBlue = nBlue0 + (((nBlue255 - nBlue0) * n) / 256);

		ulRGB = nRed << 16;
		ulRGB |= nGreen << 8;
		ulRGB |= nBlue;

		if (!Octree_AddColor(pOctree, ulRGB))
			return 0;	/* Mem failure. */
	}
}

/********************************************************************
* Function : Lightmap256_AssignColorIndices()
* Purpose : Assigns colormap indices to the Lightmap256 structure.
* Pre : pThis points to an initialized Lightmap256 structure.
*       pOctree points to an initialized Octree structure.
*       A call to Lightmap256_RequestColors() with pThis and pOctree
*       has been made before this call.
* Post : pThis now contains valid colormap indices.
********************************************************************/
void Lightmap256_AssignColorIndices(struct Lightmap256 *pThis,
												struct Octree *pOctree)
{
	int n;
	unsigned long ulRGB;
	int nRed0, nGreen0, nBlue0;
	int nRed255, nGreen255, nBlue255;
	int nRed, nGreen, nBlue;

	nRed0 = (pThis->ulRGB0 & 0xFF0000) >> 16;
	nGreen0 = (pThis->ulRGB0 & 0xFF00) >> 8;
	nBlue0 = (pThis->ulRGB0 & 0xFF);

	nRed255 = (pThis->ulRGB255 & 0xFF0000) >> 16;
	nGreen255 = (pThis->ulRGB255 & 0xFF00) >> 8;
	nBlue255 = (pThis->ulRGB255 & 0xFF);

	/* Interpolate RGB color from ulRGB0 to ulRGB255 and
	 * retrieve each colorindex from the octree. */
	for (n = 0; n < 256; n++)
	{	
		nRed = nRed0 + (((nRed255 - nRed0) * n) / 256);
		nGreen = nGreen0 + (((nGreen255 - nGreen0) * n) / 256);
		nBlue = nBlue0 + (((nBlue255 - nBlue0) * n) / 256);

		ulRGB = nRed << 16;
		ulRGB |= nGreen << 8;
		ulRGB |= nBlue;

		pThis->arIndices[n] = Octree_FindColorIndex(pOctree, ulRGB);
	}
}
