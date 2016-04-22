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
* FILE : colormgr.c
********************************************************************/

#define COLORMGR_C

#include <stdlib.h>

#include "colormgr.h"

/********************************************************************
* Function : ColorManager_Construct()
* Purpose : Initializes a ColorManager structure.
* Pre : pThis points to an uninitialized ColorManager structure.
* Post : pThis points to an initialized ColorManager structure that
*        contains nothing.
********************************************************************/
void ColorManager_Construct(struct ColorManager *pThis)
{
	unsigned int n;

	pThis->pLmap256Head = NULL;
	/* Set all hash entries to NULL. */
	for (n = 0; n < LIGHTMAP256_HASH; n++)
	{
		pThis->Lmap256Hash[n] = NULL;
	}

	pThis->pLmap1Head = NULL;
	/* Set all hash entries to NULL. */
	for (n = 0; n < LIGHTMAP1_HASH; n++)
	{
		pThis->Lmap1Hash[n] = NULL;
	}

	/* Construct Octree. */
	Octree_ConstructM(&(pThis->ColOctree));

	/* Initialize colormap, grayscale, 0 is black, 255 is white. */
	for (n = 0; n < 255; n++)
	{	pThis->Colormap[n] = n | (n << 8) | (n << 16);
	}
}

/********************************************************************
* Function : ColorManager_Destruct()
* Purpose : Frees all memory of a ColorManager structure.
* Pre : pThis points to an initialized ColorManager structure.
* Post : pThis points to an invalid ColorManager structure that uses
*        no memory.
********************************************************************/
void ColorManager_Destruct(struct ColorManager *pThis)
{
	int n;
	struct Lightmap256 *pLmap256, *pTLmap256;
	struct Lightmap1 *pLmap1, *pTLmap1;

	/* Free the Lightmap256 hash table. */
	for (n = 0; n < LIGHTMAP256_HASH; n++)
	{
		pLmap256 = pThis->Lmap256Hash[n];

		/* Free the hash chain. */
		while (pLmap256 != NULL)
		{
			pTLmap256 = pLmap256->pNext;
			free(pLmap256);
			pLmap256 = pTLmap256;
		}
	}

	/* Free the Lightmap1 hash table. */
	for (n = 0; n < LIGHTMAP1_HASH; n++)
	{
		pLmap1 = pThis->Lmap1Hash[n];

		/* Free the hash chain. */
		while (pLmap1 != NULL)
		{
			pTLmap1 = pLmap1->pNext;
			free(pLmap1);
			pLmap1 = pTLmap1;
		}
	}

	/* Destroy the Octree. */
	Octree_Destruct(&(pThis->ColOctree));
}

/********************************************************************
* Function : ColorManager_GetLightmap256()
* Purpose : Checks if a Lightmap256 structure with the intensity
*           range ulRGB0..ulRGB255 already exists, if so, it returns
*           the existing Lightmap256 structure, otherwise it builds
*           a new Lightmap256 structure with the specified 
*           intensity range and returns that.
*           This is used for providing shared lightmaps, thus saving
*           a lot of CPU time and memory as they need to be specified
*           for almost every polygon.
* Pre : pThis points to an initialized ColorManager structure.
*       ulRGB0..ulRGB255 specifies the intensity range.
* Post : If the returnvalue != NULL, it is the Lightmap256 structure
*        for the specified intensity range. It has already been
*        attached so don't call Lightmap256_AttachM(). DO call
*        Lightmap256_ReleaseM() when you don't use it anymore.
*        If the returnvalue == NULL, a memory allocation failure
*        occured.
********************************************************************/
struct Lightmap256 *ColorManager_GetLightmap256(struct ColorManager *pThis,
																unsigned long ulRGB0,
																unsigned long ulRGB255)
{
	unsigned long nIndex;
	struct Lightmap256 *pLmap;

	/* Try to find an identical Lightmap. */
	nIndex = ColorManager_Lightmap256HashkeyM(ulRGB0, ulRGB255);

	pLmap = pThis->Lmap256Hash[nIndex];

	while (pLmap != NULL)
	{
		if ((pLmap->ulRGB0 == ulRGB0) &&
			 (pLmap->ulRGB255 == ulRGB255))
		{	/* Attach to the Lightmap256 for reference administration. */
			Lightmap256_AttachM(pLmap);
			return pLmap;	/* Gotcha!, it already exists. */
		}
		pLmap = pLmap->pNext;
	}

	/* The specified Lightmap256 structure didn't exist.
	 * Build a new one. */
	pLmap = (struct Lightmap256 *)malloc(sizeof(struct Lightmap256));

	if (pLmap != NULL)
	{
		/* Initialize new Lightmap. */
		Lightmap256_ConstructM(pLmap);
		pLmap->ulRGB0 = ulRGB0;
		pLmap->ulRGB255 = ulRGB255;

		/* Link into hashtable. */
		pLmap->pNext = pThis->Lmap256Hash[nIndex];
		pThis->Lmap256Hash[nIndex] = pLmap;
		
		/* Link into list. */
		pLmap->pLNext = pThis->pLmap256Head;
		pThis->pLmap256Head = pLmap;

		/* Attach to the Lightmap256 for reference administration. */
		Lightmap256_AttachM(pLmap);
	}

	return pLmap;
}

/********************************************************************
* Function : ColorManager_GetLightmap1()
* Purpose : Checks if a Lightmap1 structure with the color ulRGB
*           already exists, if so, it returns the existing Lightmap1
*           structure, otherwise it builds a new Lightmap1 structure
*           with the specified color and returns that.
*           This is used for providing shared lightmaps, thus saving
*           a lot of CPU time and memory as they need to be specified
*           for every polygon.
* Pre : pThis points to an initialized ColorManager structure.
*       ulRGB specifies the color of the Lightmap1 structure.
* Post : If the returnvalue != NULL, it is the Lightmap1 structure
*        for the specified color. It has already been attached so
*        don't call Lightmap1_AttachM(). DO call Lightmap1_ReleaseM()
*        when you don't use it anymore.
*        If the returnvalue == NULL, a memory allocation failure
*        occured.
********************************************************************/
struct Lightmap1 *ColorManager_GetLightmap1(struct ColorManager *pThis,
														  unsigned long ulRGB)
{	unsigned long nIndex;
	struct Lightmap1 *pLmap;

	/* Try to find an identical Lightmap. */
	nIndex = ColorManager_Lightmap1HashkeyM(ulRGB);

	pLmap = pThis->Lmap1Hash[nIndex];

	while (pLmap != NULL)
	{
		if (pLmap->ulRGB == ulRGB)
		{	/* Attach to the Lightmap1 for reference administration. */
			Lightmap1_AttachM(pLmap);
			return pLmap;	/* Gotcha!, it already exists. */
		}
		pLmap = pLmap->pNext;
	}

	/* The specified Lightmap1 structure didn't exist.
	 * Build a new one. */
	pLmap = (struct Lightmap1 *)malloc(sizeof(struct Lightmap1));

	if (pLmap != NULL)
	{
		/* Initialize new Lightmap. */
		Lightmap1_ConstructM(pLmap);
		pLmap->ulRGB = ulRGB;

		/* Link into hashtable. */
		pLmap->pNext = pThis->Lmap1Hash[nIndex];
		pThis->Lmap1Hash[nIndex] = pLmap;
		
		/* Link into list. */
		pLmap->pLNext = pThis->pLmap1Head;
		pThis->pLmap1Head = pLmap;

		/* Attach to the Lightmap1 for reference administration. */
		Lightmap1_AttachM(pLmap);
	}

	return pLmap;
}

/********************************************************************
* Function : ColorManager_AssignColors()
* Purpose : Assigns the requested colors to colormap indices, in
*           so it also calculates a new colormap.
* Pre : pThis points to an initialized ColorManager structure.
* Post : If the returnvalue is 0, a memory failure occured otherwise
*        pThis points to an initialized ColorManager structure of
*        which all requested colors have received a colormap index
*        and whose Colormap has been initialized.
*        Any color structures that are no longer referenced will be
*        deleted.
********************************************************************/
int ColorManager_AssignColors(struct ColorManager *pThis)
{
	struct Lightmap256 *pLmap256;
	struct Lightmap256 **ppLmap256;
	struct Lightmap1 *pLmap1;
	struct Lightmap1 **ppLmap1;

	/* Iterate all Lightmap256 structures contained in the Colormanager,
	 * Request their colors, and remove structures if they're not
	 * referenced anymore. */
	ppLmap256 = &(pThis->pLmap256Head);
	while ((*ppLmap256) != NULL)
	{
		pLmap256 = *ppLmap256;
		/* Check if the Lightmap256 should be deleted because nothing
		 * is referencing it. */
		if (pLmap256->nRefCount <= 0)
		{
			/* Destroy the lightmap. */
			*ppLmap256 = pLmap256->pLNext;
			free((void *)pLmap256);
		} else
		{
			/* Request the colors of the Lightmap256 structure. */
			if (!Lightmap256_RequestColors(pLmap256, &(pThis->ColOctree)))
			{	/* Mem failure caused the request to fail. */
				return 0;
			}
			ppLmap256 = &(pLmap256->pLNext);
		}
	}

	/* Iterate all Lightmap1 structures.
	 * Request their colors, and remove structures if they're not
	 * referenced anymore. */
	ppLmap1 = &(pThis->pLmap1Head);
	while ((*ppLmap1) != NULL)
	{
		pLmap1 = *ppLmap1;
		/* Check if the Lightmap1 should be deleted because nothing
		 * is referencing it. */
		if (pLmap1->nRefCount <= 0)
		{
			/* Destroy the lightmap. */
			*ppLmap1 = pLmap1->pLNext;
			free((void *)pLmap1);
		} else
		{
			/* Request the colors of the Lightmap1 structure. */
			if (!Lightmap1_RequestColorM(pLmap1, &(pThis->ColOctree)))
			{	/* Mem failure caused the request to fail. */
				return 0;
			}
			ppLmap1 = &(pLmap1->pLNext);
		}
	}

	/* Fill the colormap with RGB values. */
	Octree_FillColormap(&(pThis->ColOctree), pThis->Colormap);

	/* Iterate all Lightmap256 structures again, but now assign their
	 * colorindices. */
	pLmap256 = pThis->pLmap256Head;
	while (pLmap256 != NULL)
	{
		/* Assign the colorindices. */
		Lightmap256_AssignColorIndices(pLmap256, &(pThis->ColOctree));

		pLmap256 = pLmap256->pLNext;
	}

	/* Iterate all Lightmap1 structures, but now assign their
	 * colorindices. */
	pLmap1 = pThis->pLmap1Head;
	while (pLmap1 != NULL)
	{
		/* Assign the colorindices. */
		Lightmap1_AssignColorIndexM(pLmap1, &(pThis->ColOctree));

		pLmap1 = pLmap1->pLNext;
	}

	return 1;
}
