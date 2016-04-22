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
* FILE : texmap.h
* Purpose : Header file for the TextureMap structure.
* Description : The TextureMap structure manages 256x256 256 color
*               texture maps and allows lightmap allocation to be
*               done with it.
********************************************************************/

#ifndef TEXMAP_H
#define TEXMAP_H

#include <stdio.h>
#include "colormgr.h"

struct TextureMap
{
	unsigned long ulColors;			/* Number of colors to allocate. */
	unsigned long aulPalette[256];	/* Original Palette for the bitmap, containing
									 * RGB values. */

	unsigned char	Bitmap[256 * 256];
									/* Original Bitmap data,
									 * from Left to Right, Top to Bottom.
									 * Values index colors in aulPalette. */
	struct Lightmap1	*arpLMaps[256];
									/* Assigned Lightmap1 colors. */

	unsigned char	CMBmp[256 * 256];
									/* Colormanaged Bitmap data.
									 * from Left to Right, Top to Bottom.
									 * Values can be used Directly on screen. */
};
/* TextureMap_ConstructM(struct TextureMap *pThis),
 * TextureMap construction, as default, no bitmap & no colors to attach. */
#define TextureMap_ConstructM(pThis)\
{\
	int n;\
	/* Clear bitmaps */\
	for (n = 0; n < (256 * 256); n++)\
		(pThis)->Bitmap[n] = (pThis)->CMBmp[n] = 0;\
	/* Clear palette & lightmaps */\
	for (n = 0; n < 256; n++)\
	{	(pThis)->aulPalette[n] = 0;\
		(pThis)->arpLMaps[n] = NULL;\
	}\
	/* No colors as default. */\
	(pThis)->ulColors = 0;\
}

/* TextureMap_DestructM(struct TextureMap *pThis),
 * TextureMap destructor, detaches from all attachments to the colormap. */
#define TextureMap_DestructM(pThis)\
{\
	int n;\
	/* Detach from all Lightmap1 structures. */\
	for (n = 0; n < (pThis)->ulColors; n++)\
	{\
		if ((pThis)->arpLMaps[n] != NULL)\
		{\
			Lightmap1_ReleaseM((pThis)->arpLMaps[n]);\
		}\
	}\
}

/* TextureMap_AttachToColorMgr(),
 * Attaches the texture map to the colormanager, assigning it it's colors. */
int TextureMap_AttachToColorMgr(struct TextureMap *pThis, struct ColorManager *pClrMgr);


/* TextureMap_Prepare(),
 * TextureMap_PrepareM() (MACRO version),
 * Creates the bitmap used for displaying (i.e. with the correct screen color indices).
 * NOTE FOR MACRO VERSION :
 *        This call *MUST* have been preceded by a call to TextureMap_AttachColorMgr()
 *        otherwise your system *WILL* hang due to a page fault! (NULL ptr reference).
 *        The function version catches this case and returns 0 if it happens. */
int TextureMap_Prepare(struct TextureMap *pThis);

#define TextureMap_PrepareM(pThis)\
{\
	int n;\
	for (n = 0; n < (256 * 256); n++)\
		(pThis)->CMBmp[n] = (pThis)->arpLMaps[(pThis)->Bitmap[n]]->nIndex;\
}

/* TextureMap_ReadTex(),
 * Reads a texturemap in the TEX0 format (256x256 x 256 colors) from the specified file;
 * the file should already be open.
 */
int TextureMap_ReadTex(struct TextureMap *pThis, FILE *fp);

/* TextureMap_Draw(),
 * Draws a given area of the texturemap to a given location in the buffer.
 * Note that this function doesn't do the color requests, use TextureMap_Prepare for
 * that. This function merely displays using the results of the color requests. */
void TextureMap_Draw(struct TextureMap *pThis, 
					 int nSX, int nSY, int nW, int nH,	/* Source rectangle */
					 int nTX, int nTY,					/* Target top left point */
					 unsigned char *pBitmap, int nPixelRow);		/* Target Bitmap */

#endif /* TEXMAP_H */
