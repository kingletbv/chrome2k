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
* FILE : vpoint.h
* Purpose : Header file for the Viewpoint structure.
* Description : The Viewpoint structure describes a view into a
*               world. Associated with a viewpoint is also the bitmap
*               into which the world is to be rendered.
********************************************************************/

#ifndef VPOINT_H
#define VPOINT_H

#include "frame.h"
#include "planeset.h"
#include "actor.h"
#include "scvtxset.h"
#include "edgetbl.h"

struct Viewpoint
{
	/* Frame describing current position and orientation of the
	 * viewpoint. The viewpoint is defined as looking into the
	 * Z direction with the X axis pointing to the right and the Y
	 * axis pointing down. */
	struct Frame	VpointFrame;

	/* View frustrum planes. The view frustrum consists of at least
	 * 4 planes (excluding front- and backplanes which are optional).
	 * Only Actors that are (fully or partially) inside the view
	 * frustrum are visible. Actors partially inside the view
	 * frustrum will be clipped to the view frustrum.
	 * This is the only clipping mechanism that prevents writing
	 * outside the bitmap's width and height. Rasterisation doesn't
	 * perform clipping. */
	struct PlaneSet	FrustrumPlanes;

	/* Pointer to the Root actor. All other actors will be inserted
	 * into the BSP tree of this actor to form a full BSP tree of the
	 * whole 3D world. This tree is filled by the
	 * Viewpoint_PrepActorsForDraw() function and used by the
	 * Viewpoint_Draw() function (which traverses the BSP tree and
	 * draws the polygons it encounters).
	 */
	struct Actor	*pRootActor;

	/* Some flags for multiple purposes.
	 * Currently only used to determine if we should
	 * render in truecolor of indexed mode. */
	unsigned int nRendermode : 1;

	/* Bitmap information. The bitmap consists of a width, height,
	 * pixelrow and a pointer to the bitmap.
	 * Width, height and pixelrow are specified in pixels.
	 * The pixelrow value represents the number of pixels on a single
	 * scanline. An example for when it can be used is when the
	 * Viewpoint is to be drawn in a small rectangle onto the screen
	 * instead of the whole screen. In this case, the pixelrow value
	 * represents the number of pixels on the screen and the width
	 * represents the number of pixels in the rectangle where the
	 * viewpoint is to be rendered. */
	int	nWidth;
	int	nHeight;
	int	nPixelRow;
	unsigned char	*pBitmap;

	/* EdgeTable used for rendering polygons in the above bitmap.
	 * This should be in Viewpoint to preserve cache, and in the
	 * Window to allow paralellized rendering. */
	struct EdgeTable	PolyEdgeTable;
	
	/* Field of view (FOV). The field of view is specified in radians.
	 * The values here actually describe HALF the field of view. */
	float	fXFOV;
	float	fYFOV;

	/* Scaling Multipliers. The scaling multipliers are used for
	 * altering the object to view transformation so that the X and
	 * Y coordinates (after division by Z) match the nWidth and
	 * nHeight parameters with the horizontal field of view and
	 * vertical field of view.
	 * The scaling multipliers can be calculated from the following
	 * formula's :
	 * fXMultiplier = (nWidth / 2) / tan(fXFOV)
	 * fYMultiplier = (nHeight / 2) / tan(fYFOV)
	 */
	float	fXMultiplier;
	float	fYMultiplier;

	/* A FloatSet available for multiple purposes. Putting it here
	 * prevents reallocating and freeing on a per frame basis. Now
	 * it just expands on an as needed basis. */
	struct FloatSet	TempFloatSet;
	struct FloatSet	TempFloatSet2;
};

/* Viewpoint_Construct(pThis),
 * Viewpoint_ConstructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Initializes the viewpoint, setting the bitmap to NULL. */
void Viewpoint_Construct(struct Viewpoint *pThis);
#define Viewpoint_ConstructM(pThis)\
(	Frame_Construct(&((pThis)->VpointFrame)),\
	(pThis)->nWidth = 0,\
	(pThis)->nHeight = 0,\
	(pThis)->nPixelRow = 0,\
	(pThis)->pBitmap = NULL,\
	(pThis)->nRendermode = 1,\
	(pThis)->pRootActor = NULL,\
	(pThis)->fXMultiplier = 0.f,\
	(pThis)->fYMultiplier = 0.f,\
	(pThis)->fXFOV = 0.5235987757f,\
	(pThis)->fYFOV = 0.5235987757f,\
	PlaneSet_Construct(&((pThis)->FrustrumPlanes)),\
	FloatSet_Construct(&((pThis)->TempFloatSet)),\
	FloatSet_Construct(&((pThis)->TempFloatSet2)),\
	EdgeTable_Construct(&((pThis)->PolyEdgeTable))\
)

/* Viewpoint_SetRendermode(pThis, mode),
 * Selects the mode to render into.
 * You MUST use a mode that is appropriate
 * to the bitmap / surface you want to render into! */
#define CHROME_VIEWPOINT_RENDERMODE_TRUECOLOR_32 0 /* Common truecolor mode, 4 bytes/pixel [aRGB] */
#define CHROME_VIEWPOINT_RENDERMODE_INDEXED_8 1 /* Common 256 color mode, 1 byte/pixel */
int Viewpoint_SetRendermode(struct Viewpoint *pThis, unsigned char mode);

/* Viewpoint_Destruct(pThis),
 * Viewpoint_DestructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Frees all memory associated with the viewpoint structure. */
void Viewpoint_Destruct(struct Viewpoint *pThis);
#define Viewpoint_DestructM(pThis)\
(	PlaneSet_Destruct(&((pThis)->FrustrumPlanes)),\
	FloatSet_Destruct(&((pThis)->TempFloatSet)),\
	FloatSet_Destruct(&((pThis)->TempFloatSet2)),\
	EdgeTable_Destruct(&((pThis)->PolyEdgeTable))\
)

/* Viewpoint_PrecalcM(pThis), (NEEDS math.h INCLUDED)
 * Initializes the fXMultiplier and fYMultiplier values from nWidth, 
 * nHeight, fXFOV and fYFOV. Call this when any of the variables has
 * changed to prepare the Viewpoint for display.
 * This function **MUST** be called after either the field of view OR
 * the dimensions of the bitmap have been changed.
 */
#define Viewpoint_PrecalcM(pThis)\
(	(pThis)->fXMultiplier = ((pThis)->nWidth / 2) / (float)tan((pThis)->fXFOV),\
	(pThis)->fYMultiplier = ((pThis)->nHeight/ 2) / (float)tan((pThis)->fYFOV)\
)

/* Viewpoint_PrecalcFrustrum(pThis),
 * Builds the standard 4 planes that define the view frustrum.
 * This function depends on correct values for fXFOV and fYFOV
 * (the field of view).
 * This function **MUST** be called after the field of view has been
 * changed, but after the viewport has been setup with Viewpoint_Precalc!
 * NOTE : Any additionally defined planes will be lost...
 * The parameters define a 'subwindow' in the bitmap.
 * This allows for big images to be rendered in small chunks.
 */
int Viewpoint_PrecalcFrustrum(struct Viewpoint *pThis );

/* Viewpoint_PrepActorsForDraw(pThis, pActors),
 * Prepares a list of Actors pActors for display from the Viewpoint
 * pThis. What this effectively does is it inserts all Actors in the
 * linked list specified by pActors in the viewpoint's BSP Tree (in
 * the order of appearance in the list). Also rejecting Actors that
 * are not visible (i.e. not in the view frustrum).
 * After preparing the Actors for drawing, this call should be
 * followed by a call to Viewpoint_Draw() which renders the viewpoint
 * to the bitmap.
 * The reason for splitting these calls is that the programmer may
 * want to start a blitter during Viewpoint_PrepActorsForDraw()
 * to clear the bitmap before any drawing takes place, thus saving
 * time by running two essential processes concurrently.
 */
int Viewpoint_PrepActorsForDraw(struct Viewpoint *pThis, struct Actor *pActors);

/* Viewpoint_Draw(pThis),
 * Renders all actors that have been prepared for drawing by
 * Viewpoint_PrepActorsForDraw() into the standard bitmap
 * associated to pThis Viewpoint structure.
 */
int Viewpoint_Draw(struct Viewpoint *pThis);

/* Viewpoint_DrawActorTree(pThis, pActor, pPlane, nLevel),
 * Renders all polygons and actors in a given hyperplane tree.
 * This is a helper function for Viewpoint_Draw().
 */
void Viewpoint_DrawActorTree(struct Viewpoint *pThis,
									  struct Actor *pActor,
									  struct HPlane *pPlane,
									  int nLevel);
#endif
