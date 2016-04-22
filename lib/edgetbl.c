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
* FILE : edgetbl.c
********************************************************************/

#define EDGETBL_C

#include <stdlib.h>
#ifdef DEBUGC
#include <stdio.h>
#endif

#include "edgetbl.h"

/* Inlined MS-Visual C Intel ASM is used if defined.
 #define VCINTEL_ASM */

/********************************************************************
* Function : EdgeTable_Construct()
* Purpose : Initializes an EdgeTable structure.
* Pre : pThis points to an EdgeTable structure.
* Post : pThis points to an initialized EdgeTable structure.
********************************************************************/
void EdgeTable_Construct(struct EdgeTable *pThis)
{
	/* Call macro version. */
	EdgeTable_ConstructM(pThis);
}

/********************************************************************
* Function : EdgeTable_Destruct()
* Purpose : Frees all memory associated with an EdgeTable structure.
* Pre : pThis points to an initialized EdgeTable structure.
* Post : pThis points to an invalid EdgeTable structure that uses
*        no more memory.
********************************************************************/
void EdgeTable_Destruct(struct EdgeTable *pThis)
{	/* Call the macro version. */
	EdgeTable_DestructM(pThis);
}

/********************************************************************
* Function : EdgeTable_Whipe()
* Purpose : Cleans an EdgeTable from any polygon it may contain.
* Pre : pThis points to an initialized EdgeTable structure.
* Post : pThis points to an initialized EdgeTable structure
*        containing no spans.
********************************************************************/
void EdgeTable_Whipe(struct EdgeTable *pThis)
{
	/* Call macro version. */
	EdgeTable_Whipe(pThis);
}

/********************************************************************
* Function : EdgeTable_AtLeast()
* Purpose : Guarantees that there are at least nScanLines scanlines
*           available.
* Pre : pThis points to an initialized EdgeTable structure.
*       nScanLines is the number of scanlines required.
* Post : If the returnvalue is 1, pThis now contains at least
*        nScanLines scanlines.
*        If the returnvalue is 0, a memory failure occured.
********************************************************************/
int EdgeTable_AtLeast(struct EdgeTable *pThis, int nScanLines)
{
	short	*p1,*p2;
	int n;
	/* Check if we have enough scanlines. */
	if (pThis->nScanlines >= nScanLines)
	{	return 1;	/* Already enough scanlines available. */
	} else
	{	/* Allocate new start of spans. */
		p1 = (short *)malloc(sizeof(short) * nScanLines);
		
		/* Check for memory failure */
		if (p1 == NULL)
		{	/* Memory Failure. */
			return 0;
		}
		
		for (n = 0; n < nScanLines; n++)
			p1[n] = 0;

		/* Allocate new end of spans. */
		p2 = (short *)malloc(sizeof(short) * nScanLines);
		
		/* Check for memory failure */
		if (p2 == NULL)
		{	/* Memory failure. */
			free(p1);
			return 0;
		}

		for (n = 0; n < nScanLines; n++)
			p2[n] = 0;
		
		/* Got the memory, free the old span arrays, set the new. */
		free((void *)pThis->arSpanStartValues);
		free((void *)pThis->arSpanEndValues);
		pThis->arSpanStartValues = p1;
		pThis->arSpanEndValues = p2;
		pThis->nScanlines = nScanLines;
		return 1;
	}
}

/********************************************************************
* Function : EdgeTable_AddEdge()
* Purpose : Adds an edge defined by the two ScreenVertex structures
*           pSrcVtx and pTrgVtx to the EdgeTable pThis.
* Pre : pThis points to an initialized EdgeTable structure,
*       pSrcVtx and pTrgVtx point to two ScreenVertex structures that
*       together define an edge of a counterclockwise convex polygon.
* Post : The EdgeTable contains the edge specified by pSrcVtx and
*        pTrgVtx.
********************************************************************/
void EdgeTable_AddEdge(struct EdgeTable *pThis,
							  struct ScreenVertex *pSrcVtx,
							  struct ScreenVertex *pTrgVtx)
{
	short	dx, dy;			/* Delta X and Delta Y values. */
	short	*pSpan;			/* Ptr to span list of edge. */
	short	sx, sy;			/* Start X and Start Y values. */
	short denominator;
	short increment;

#ifdef DEBUGC
	printf("EdgeTable_AddEdge() -> Adding edge from (%d,%d) to (%d,%d).\n", pSrcVtx->nX, pSrcVtx->nY,
																									pTrgVtx->nX, pTrgVtx->nY);	
#endif
	dx = pTrgVtx->nX - pSrcVtx->nX;
	dy = pTrgVtx->nY - pSrcVtx->nY;

	/* Ignore horizontal edges. */
	if (dy == 0)
		return;
	
	/* Classify the edge.
	 * 4 cases possible, depending on the sign of dx and dy. */
	if (dy < 0)
	{	/* The edge belongs to the right side of the counterclockwise
		 * polygon. */
		/* Flip the edge. */
		dx = -dx;
		dy = -dy;
		/* Initialize Start X and Y positions. */
		sx = pTrgVtx->nX;
		sy = pTrgVtx->nY;
		/* Update Scan min and max. */
		if (sy < pThis->nMinScan)
			pThis->nMinScan = sy;
		if (pSrcVtx->nY > pThis->nMaxScan)
			pThis->nMaxScan = pSrcVtx->nY;
		/* Set the pointer. */
		pSpan = pThis->arSpanEndValues + sy;
		
		if (dx >= 0)
		{	/* Edge goes to the right. */
			denominator = dy;
			increment = dy >> 1;
			while (dy >= 0)
			{	/* Output span X position. */
				*(pSpan++) = sx;
				increment += dx;
				while (increment >= denominator)
				{	sx++;
					increment -= denominator;
				}
				dy--;
			}
		} else
		{	/* Edge goes to the left. */
			denominator = dy;
			increment = dy >> 1;
			dx = -dx;
			while (dy >= 0)
			{	/* Output span X position. */
				*(pSpan++) = sx;
				increment += dx;
				while (increment > denominator)
				{	sx--;
					increment -= denominator;
				}
				dy--;
			}
		}
	} else
	{	/* The edge belongs to the left side of the counterclockwise
		 * polygon. */
		/* Initialize Start X and Y positions. */
		sx = pSrcVtx->nX;
		sy = pSrcVtx->nY;
		/* Update Scan min and max. */
		if (sy < pThis->nMinScan)
			pThis->nMinScan = sy;
		if (pTrgVtx->nY > pThis->nMaxScan)
			pThis->nMaxScan = pTrgVtx->nY;
		/* Set the pointer. */
		pSpan = pThis->arSpanStartValues + sy;
		if (dx >= 0)
		{	/* Edge goes to the right. */
			denominator = dy;
			increment = dy >> 1;
			while (dy >= 0)
			{	/* Output span X position. */
				*(pSpan++) = sx;
				increment += dx;
				while (increment > denominator)
				{	sx++;
					increment -= denominator;
				}
				dy--;
			}
		} else
		{	/* Edge goes to the left. */
			denominator = dy;
			increment = dy >> 1;
			dx = -dx;
			while (dy >= 0)
			{	/* Output span X position. */
				*(pSpan++) = sx;
				increment += dx;
				while (increment >= denominator)
				{	sx--;
					increment -= denominator;
				}
				dy--;
			}
		}
	}
}

/********************************************************************
* Function : EdgeTable_SolidFill()
* Purpose : Fills a bitmap pBitmap with the polygon spans stored in
*           EdgeTable pThis using the byte value nColor.
* Pre : pThis points to an initialized EdgeTable structure, nColor
*       defines the value to use for the fill, nBytesPerRow defines
*       the number of bytes in a single scanline of the target bitmap
*       pBitmap.
* Post : pBitmap now contains the polygon defined in pThis. It is
*        filled by color nColor.
* Notes : This routine may be significantly increased in speed by
*         drawing 4 pixels at a time (using a long). This has not
*         been implemented because the bitmap buffer may be from
*         Windows 95's DirectDraw GameSDK in which case the buffer
*         may use a lot of hardware page flipping which will hang if
*         we cross the buffer boundary by a long.
* Notes : This is actually the SECOND version of this routine as the
*         first didn't work with Microsoft Visual C++'s Global
*         Optimisations and GameSDK's DirectDraw. Reason for this was
*         that the Global Optimisation flag converted a while loop
*         writing bytes to a stosd (writing longs). This write was
*         however not alligned on a long boundary and thus caused
*         DirectDraw's page fault handler (for producing a virtual
*         flat frame buffer) to throw up and hang Win95.
*         This implementation should be significantly faster than
*         the old one because it uses long writes instead of byte
*         writes. It DOES however make sure the long's are long
*         alligned which gives the CPU a single cycle bonus if I'm
*         right.
********************************************************************/
void EdgeTable_SolidFill(struct EdgeTable *pThis, unsigned char nColor,
	 short nBytesPerRow, unsigned char *pBitmap)
{
	/* A simple loop in which we fill the array pBitmap with the
	 * spans from pThis. */
	short	*pStart, *pEnd;
	unsigned char *p;
	int dx, dy;
	int nTrailCount;
	int nLongCount;
	int nByteCount;
	unsigned long LongColor;

#ifdef DEBUGC
	printf("EdgeTable_SolidFill() -> nColor = %d\n", nColor);
#endif

	/* Initialize span lookup. */
	pStart = pThis->arSpanStartValues + pThis->nMinScan;
	pEnd = pThis->arSpanEndValues + pThis->nMinScan;
	/* Initialize bitmap pointer. */
	pBitmap += nBytesPerRow * pThis->nMinScan;
	/* Initialize long used for double writes. */
	LongColor = (unsigned long)nColor;
	LongColor = LongColor << 24 | LongColor << 16 | LongColor << 8 | LongColor;

	dy = pThis->nMaxScan - pThis->nMinScan;
	while (dy > 0)	/* We may be loosing the last scanline here. */
	{	p = pBitmap + *pStart;
		/* Draw dx bytes of nColor at address p. */
		/* The main bunch should be long's because they're faster
		 * in a single write.
		 * However, some platforms & CPU's require us to ensure
		 * that our writes are alligned to long addresses.
		 * Windows' GameSDK DirectDraw is one of the main reasons
		 * for choosing this complicated scenario.
		 * Consequently we first write leading bytes until we're
		 * on a long allignment, then we write as many longs as
		 * needed and finally we append trailing bytes.
		 * We assume that pBitmap is long alligned(!) */

		/* Get total number of pixels to go. */
		dx = *(pEnd++) - *(pStart++);

		/* Get number of bytes needed before long allignment. */
		nByteCount = (-((int)p)) & 3;

		/* Check if there are still bytes left. */
		if (dx < nByteCount)
			nByteCount = dx;		/* Correct, we don't need to allign as
										 * there are not enough bytes to pass
										 * the first long. */
		/* Calculate bytes to go after this step. */
		dx -= nByteCount;

		/* Write leading bytes. */
#ifdef VCINTEL_ASM
		if (nByteCount > 0)
		{
			__asm
			{
				mov	ecx, nByteCount
				mov	al, nColor
				mov	ebx, p
				leadloop:
				mov	[ebx], al
				inc	ebx
				dec	ecx
				jnz	leadloop
			}
			p += nByteCount;
		}
#else
		while (nByteCount > 0)
		{	*(p++) = nColor;
			nByteCount--;
		}
#endif

		/* Write long alligned body.
		 * Long alligned body is truncated division of remaining
		 * bytes by 4. Note that if there are less than 4 bytes
		 * to be done, the shift will cause nLongCount to become
		 * 0 and the operation is cancelled after which dx still
		 * holds the correct amount of bytes to be done. */
		nLongCount = dx >> 2;

#ifdef VCINTEL_ASM
		if (nLongCount > 0)
		{
			__asm
			{
				mov	ecx, nLongCount
				mov	ebx, p
				mov	eax, LongColor
				mov	edx, 4
				bodyloop:
				mov	[ebx], eax
				add	ebx, edx
				dec	ecx
				jnz	bodyloop
			}
			p += nLongCount * 4;
		}
#else
		while (nLongCount > 0)
		{	*(((unsigned long *)p)++) = LongColor;
			nLongCount--;
		}
#endif

		/* Calculate bytes to go after longs. */
		dx &= 3;

		/* Write trailing bytes. */
#ifdef VCINTEL_ASM
		if (dx > 0)
		{	nTrailCount = dx;
			__asm
			{
				mov	ebx, p
				mov	ecx, nTrailCount
				mov	al, nColor
				trailloop:
				mov	[ebx], al
				inc	ebx
				dec	ecx
				jnz	trailloop
			}
			p += dx;
		}
#else
		while (dx > 0)
		{	*(p++) = nColor;
			dx--;
		}
#endif

		pBitmap += nBytesPerRow;
		dy--;
	}
}

/********************************************************************
* Function : EdgeTable_SolidFill32()
* Purpose : Fills a bitmap pBitmap with the polygon spans stored in
*           EdgeTable pThis using a 32 bit aRGB color.
* Pre : pThis points to an initialized EdgeTable structure, aRGB
*       defines the value to use for the fill, nPixelsPerRow defines
*       the number of PIXELS in a single scanline of the target bitmap
*       pBitmap.
* Post : pBitmap now contains the polygon defined in pThis. It is
*        filled by color aRGB.
* Note : THE BITMAP MUST BE AN ALIGNED 32-BIT COLOR BITMAP OR IT
*       WILL SEGFAULT!
********************************************************************/
void EdgeTable_SolidFill32(struct EdgeTable *pThis, unsigned_int_32 aRGB,
	 short nPixelsPerRow, unsigned_int_32 *pBitmap)
{
	/* A simple loop in which we fill the array pBitmap with the
	 * spans from pThis. */
	short	*pStart, *pEnd;
	unsigned_int_32 *p;
	int dx, dy;

#ifdef DEBUGC
	printf("EdgeTable_SolidFill32() -> aRGB = %d\n", aRGB);
#endif

	/* Initialize span lookup. */
	pStart = pThis->arSpanStartValues + pThis->nMinScan;
	pEnd = pThis->arSpanEndValues + pThis->nMinScan;
	/* Initialize bitmap pointer. */
	pBitmap += nPixelsPerRow * pThis->nMinScan;

	dy = pThis->nMaxScan - pThis->nMinScan;
	while (dy > 0)	/* We may be loosing the last scanline here. */
	{	p = pBitmap + *pStart;
		/* Draw dx pixels of aRGB at address p. */
		dx = *(pEnd++) - *(pStart++);

		while (dx-- > 0)
		{	*p++ = aRGB;
		}

		pBitmap += nPixelsPerRow;
		dy--;
	}
}
