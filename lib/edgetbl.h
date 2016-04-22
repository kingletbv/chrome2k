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
* FILE : edgetbl.h
* Purpose : Header file for the EdgeTable structure.
* Description : The EdgeTable structure describes the table used for
*               scan converting polygons. For each scanline the table
*               maintains two X values which describe the start and
*               ending positions for a polygon.
*               Only polygons whose vertices (ScreenVertex
*               structures) are counterclockwise can be rendered.
********************************************************************/

#ifndef EDGETBL_H
#define EDGETBL_H

#include "scrvertx.h"
typedef unsigned long unsigned_int_32; /* Use for now... */

struct EdgeTable
{
	/* Number of scanlines maintained in the structure.
	 * This is NOT dynamic. Scan converting polygons that cross
	 * the number of scanlines WILL hang your machine (as there
	 * is no 2D clipping). */
	int	nScanlines;

	/* Minimum and Maximum scanline. These describe the actual
	 * area in which the Span Start and End values are valid. */
	int	nMinScan;
	int	nMaxScan;
	
	/* Array containing nScanlines shorts which describe the
	 * start X positions of a span. */
	short	*arSpanStartValues;
	
	/* Array containing nScanlines shorts which describe the
	 * ending X positions of span. */
	short	*arSpanEndValues;
};

/* EdgeTable_Construct(pThis),
 * EdgeTable_ConstructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Initializes an EdgeTable.
 */
void EdgeTable_Construct(struct EdgeTable *pThis);
#define EdgeTable_ConstructM(pThis)\
(	(pThis)->nScanlines = 0,\
	(pThis)->nMinScan = 0,\
	(pThis)->nMaxScan = 0,\
	(pThis)->arSpanStartValues = NULL,\
	(pThis)->arSpanEndValues = NULL\
)

/* EdgeTable_Destruct(pThis),
 * EdgeTable_DestructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Frees all memory associated with an EdgeTable structure.
 */
void EdgeTable_Destruct(struct EdgeTable *pThis);
#define EdgeTable_DestructM(pThis)\
(	(NULL != (pThis)->arSpanStartValues) ?\
	(	free((void *)(pThis)->arSpanStartValues)\
	):(0),\
	(NULL != (pThis)->arSpanEndValues) ?\
	(	free((void *)(pThis)->arSpanEndValues)\
	):(0)\
)

/* EdgeTable_Whipe(pThis),
 * EdgeTable_WhipeM(pThis),
 * Cleans an EdgeTable. Call this before adding a new polygon.
 */
void EdgeTable_Whipe(struct EdgeTable *pThis);
#define EdgeTable_WhipeM(pThis)\
(	(pThis)->nMinScan = (pThis)->nScanlines,\
	(pThis)->nMaxScan = 0\
)

/* EdgeTable_AtLeast(pThis, nScanLines),
 * Guarantees that there are at least nScanLines available in
 * the EdgeTable pThis. Don't call this function when in the
 * middle of a polygon.
 */
int EdgeTable_AtLeast(struct EdgeTable *pThis, int nScanLines);

/* EdgeTable_AddEdge(pThis, pSrcVtx, pTrgVtx),
 * Adds an edge to an EdgeTable. */
void EdgeTable_AddEdge(struct EdgeTable *pThis,
							  struct ScreenVertex *pSrcVtx,
							  struct ScreenVertex *pTrgVtx);

/* EdgeTable_SolidFill(pThis, nColor, nBytesPerRow, pBitmap),
 * Fills bitmap pBitmap (having nBytesPerRow bytes per row) with
 * the spans stored in EdgeTable using value nColor.
 */
void EdgeTable_SolidFill(struct EdgeTable *pThis, unsigned char nColor, 
                         short nBytesPerRow, unsigned char *pBitmap);

/* EdgeTable_SolidFill32(pThis, aRGB, nBytesPerRow, pBitmap),
 * Fills bitmap pBitmap (having nBytesPerRow bytes per row) with
 * the spans stored in EdgeTable using color aRGB.
 * Created because OpenPTC preferes to work with 32-bit colors.
 */
void EdgeTable_SolidFill32(struct EdgeTable *pThis, unsigned_int_32 aRGB,
	 short nBytesPerRow, unsigned_int_32 *pBitmap);

#endif
