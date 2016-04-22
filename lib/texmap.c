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
* FILE : texmap.c
********************************************************************/

#include <stdlib.h>
#include <string.h>

#include "texmap.h"

/* Inlined MS-Visual C Intel ASM is used if defined. */
/*#define VCINTEL_ASM*/

/********************************************************************
* Function : TextureMap_AttachToColorMgr()
* Purpose : Requests the colors required for displaying the bitmap
*           from a ColorManager.
*           After this, call ColorManager_AssignColors() and then
*           call TextureMap_Prepare() to actually use the bitmap
*           without messing everything up.
* Pre : pThis points to an initialized TextureMap structure,
*       pClrMgr points to an initialized ColorManager structure.
* Post : If the returnvalue != 0, the TextureMap was succesfully
*        attached to the ColorManager.
*        Otherwise the TextureMap may have only partially attached
*        to the color manager and cannot not be used.
********************************************************************/
int TextureMap_AttachToColorMgr(struct TextureMap *pThis, struct ColorManager *pClrMgr)
{
	unsigned long n;

	/* Attach colors & retrieve lightmaps. */
	for (n = 0; n < pThis->ulColors; n++)
		if (NULL == (pThis->arpLMaps[n] = ColorManager_GetLightmap1(pClrMgr, pThis->aulPalette[n])))
		{	/* Failed to get the lightmap, remove references to all created lightmaps
			 * and return failure. */
			while (--n > 0)
			{	/* Release Lightmap, the ColorManager will clean it up if no other requests
				 * are referencing it. */
				Lightmap1_ReleaseM(pThis->arpLMaps[n]);

				/* Set it to NULL so TextureMap no longer assumes it's referencing this
				 * lightmap. */
				pThis->arpLMaps[n] = NULL;
			}
			return 0;
		}
	return 1;	/* Success ! */
}


/********************************************************************
* Function : TextureMap_Prepare()
* Purpose : Creates the bitmap required for displaying the texture.
*           This is required or otherwise the texture will be
*           black or in completely the wrong colors.
* Pre : pThis points to a TextureMap structure upon which a call to
*       TextureMap_AttachToColorMgr() has been made (*ESSENTIAL*).
* Post : If the returnvalue != 0, the texturemap can now be displayed
*        Otherwise no call to TextureMap_AttachToColorMgr() preceded
*        this call (or the call failed).
********************************************************************/
int TextureMap_Prepare(struct TextureMap *pThis)
{
	int n;
	/* Replace colormap index for CMBmp with indices from lightmap
	 * instead of indices into private palette. */
	for (n = 0; n < (256 * 256); n++)
	{
		struct Lightmap1 *pLMap1;
		pLMap1 = pThis->arpLMaps[(pThis)->Bitmap[n]];

		if (pLMap1 == NULL)
		{
			/* A NULL lightmap was found (this was not supposed to happen).
			 * Return 0. */
			return 0;
		}

		/* Assign appropriate index. */
		pThis->CMBmp[n] = pLMap1->nIndex;
	}
	return 1;
}

/********************************************************************
* Function : TextureMap_ReadTex()
* Purpose : Reads a texturemap in the TEX0 (256x256 w. 256 colors)
*           from the specified file.
* Pre : pThis points to an initialized TextureMap that should be
*       filled with the texturemap.
*       fp points to an open file where the current position is at
*       the start of the TEX0 file (which may be the component of a
*       larger file.)
* Post : If the returnvalue != 0, the texturemap has been succesfully
*        read and the file's position is at the first byte after the
*        TEX0 file.
*        Otherwise the reading failed and the file's position is
*        undefined.
********************************************************************/
int TextureMap_ReadTex(struct TextureMap *pThis, FILE *fp)
{
	char HeadMarker[5];
	unsigned long ulColors;
	unsigned long n;
	unsigned char red, green, blue;

	HeadMarker[4] = '\0';

	/* Read the Marker */
	if (4 == fread((void *)HeadMarker, 1, 4, fp))
	{
		if (0 == strncmp(HeadMarker, "TEX0", 4))
		{
			/* Header matches, read the number of colors in the color
			 * table. */
			if (1 == fread((void *)&ulColors, sizeof(unsigned long), 1, fp))
			{
				/* Make sure the number of colors is reasonable. */
				if (ulColors <= 256)
				{
					/* Retrieve colors */
					for (n = 0; n < ulColors; n++)
					{
						if ((1 == fread((void *)&red, sizeof(unsigned char), 1, fp)) &&
							(1 == fread((void *)&green, sizeof(unsigned char), 1, fp)) &&
							(1 == fread((void *)&blue, sizeof(unsigned char), 1, fp)))
						{
							pThis->aulPalette[n] = red * 256 * 256 + green * 256 + blue;
						} else
						{
							/* Read failure, stop here.
							 * At this point, the texturemap's colormap is ruined but
							 * won't cause any failures; just an incorrect display of
							 * the old texturemap. */
							return 0;
						}
					}
					pThis->ulColors = ulColors;

					/* Retrieve bitmap marker "TDTA". */
					if (4 == fread((void *)HeadMarker, 1, 4, fp))
					{
						if (0 == strncmp(HeadMarker, "TDTA", 4))
						{
							/* Bitmap header matches, read in bitmap. */
							if ((256 * 256) == fread((void *)pThis->Bitmap, 1, 256*256, fp))
							{
								/* Read in the bitmap.. We're done. */
								return 1;
							}
						}
					}
				}
			}
		}
	}

	/* If we reach this point, one of the if's failed and we were unable to load the
	 * texture. */
	return 0;
}


/********************************************************************
* Function : TextureMap_Draw()
* Purpose : Draws a rectangle from the texturemap to a bitmap.
* Pre : nSX, nSY define the top left coordinates on the texturemap,
*       nW, nH define the width and height of both source and target,
*       nTX, nTY define the top left coordinates on the target
*       bitmap,
*       pBitmap points to the bitmap (array of unsigned char) where
*       the texturemap should be drawn. nPixelRow defines the number
*       of pixels (=bytes) in the bitmap to skip to get to the same
*       X position; one row lower of the bitmap.
* Post : The texturemap was drawn.
********************************************************************/
void TextureMap_Draw(struct TextureMap *pThis,
					 int nSX, int nSY, int nW, int nH,	/* Source rectangle */
					 int nTX, int nTY,					/* Target top left point */
					 unsigned char *pBitmap, int nPixelRow)		/* Target Bitmap */
{
	int	ulMod;					/* Bytes to skip to get to next row after drawing
								 * a single row. */
	int ulSMod;					/* Same as ulMod but now for the source texture. */
	unsigned char	*p;			/* Pointer to the current position in the bitmap. */
	unsigned char	*s;			/* Source pointer to current position on the
								 * texturemap. */
	
	int nX;
	int nY;

	/* Initialize Target Start Pointer. */
	p = pBitmap + nPixelRow * nTY + nTX;

	ulMod = nPixelRow - nW;
	ulSMod = 256 - nW;

	s = pThis->CMBmp + nSX + nSY * 256;

	/* Start looping the loop. */
	nY = nH;

#ifndef VCINTEL_ASM
	while (nY-- > 0)
	{
		nX = nW;
		while (nX-- > 0)
		{
			*(p++) = *(s++);
		}
		p += ulMod;
		s += ulSMod;
	}
#else
	/* We're allowed to do some inline assembly, so here goes. */
__asm {
	MOV	EBX, p
	MOV	EAX, s
	MOV	ECX, nH
lpY:
	PUSH ECX
	MOV	ECX, nW
lpX:
	MOV	dl, [EAX]
	MOV	[EBX], dl
	inc	EAX
	inc	EBX
	dec	ECX
	jnz	lpX
	add	EAX, ulSMod
	add	EBX, ulMod
	POP	ECX
	dec	ECX
	jnz	lpY
}
#endif
}



