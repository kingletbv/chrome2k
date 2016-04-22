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
* FILE : vpoint.c
********************************************************************/

#define VPOINT_C

#include <stdlib.h>
#include <math.h>			/* sin, cos etc. */

#include "vpoint.h"
#include "scrvertx.h"
#include "trans.h"
#include "lmap1.h"
#include "lmap256.h"

static void Viewpoint_DrawPolygon(struct Viewpoint *pThis, struct Polygon *pPoly);

/********************************************************************
* Function : Viewpoint_Construct()
* Purpose : Initializes the Viewpoint setting the bitmap to NULL.
* Pre : pThis points to a Viewpoint structure.
* Post : pThis points to an initialized Viewpoint structure with no
*        bitmap attached and the field of view set to 60 degrees.
********************************************************************/
void Viewpoint_Construct(struct Viewpoint *pThis)
{	/* Call the macro version. */
	Viewpoint_ConstructM(pThis);
}

/********************************************************************
* Function : Viewpoint_PrecalcFrustrum()
* Arguments : Besides the Viewpoint, size and position of window whithin bitmap.
* Purpose : Builds the standard 4 planes that define the view
*           frustrum from the current field of view (fXFOV and 
*           fYFOV).
* Pre : pThis points to an initialized Viewpoint structure.
* Post : If the returnvalue is 1, pThis points to an initialized
*        Viewpoint structure with 4 view frustrum planes installed.
*        If the returnvalue is 0, a memory allocation failure
*        occured.
* Note : Any previously set frustrum planes will be lost even if
*        this routine did not set them.
* Note-2 : It's VERY IMPORTANT to call this function when the field
*          of view is changed. The frustrum planes concept is the
*          ONLY clipping mechanism. Without correct frustrum planes,
*          the drawing WILL go outside the bitmap!
********************************************************************/
int Viewpoint_PrecalcFrustrum(struct Viewpoint *pThis)
{
	struct Plane FrustrumPlane;
	float fXFOV;
	float fYFOV;

	Plane_ConstructM(&FrustrumPlane);

	/* Insert the 4 frustrum planes. */
	fXFOV = pThis->fXFOV;
	fYFOV = pThis->fYFOV;

	/* These will be slightly away from the view position (0,0,0)
	 * to prevent a division by zero (clipping will then not allow
	 * points with a Z of 0).
	 * Normals point to the inside of the view frustrum (clipping
	 * is done in the direction of the normal. */

	/* Left plane. */
	FrustrumPlane.Normal.V[0] = (float) cos(fXFOV);
	FrustrumPlane.Normal.V[1] = 0.f;
	FrustrumPlane.Normal.V[2] = (float) sin(fXFOV);
	FrustrumPlane.Distance = 0.00001f;
	if (!PlaneSet_AddM(&(pThis->FrustrumPlanes), &FrustrumPlane))
		return 0;

	/* Right plane. */
	FrustrumPlane.Normal.V[0] = -(float) cos(fXFOV);
	FrustrumPlane.Normal.V[1] = 0.f;
	FrustrumPlane.Normal.V[2] = (float) sin(fXFOV);
	FrustrumPlane.Distance = 0.00001f;
	if (!PlaneSet_AddM(&(pThis->FrustrumPlanes), &FrustrumPlane))
		return 0;

	/* Top plane. */
	FrustrumPlane.Normal.V[0] = 0.f;
	FrustrumPlane.Normal.V[1] = (float) cos(fYFOV);
	FrustrumPlane.Normal.V[2] = (float) sin(fYFOV);
	FrustrumPlane.Distance = 0.00001f;
	if (!PlaneSet_AddM(&(pThis->FrustrumPlanes), &FrustrumPlane))
		return 0;

	/* Bottom plane. */
	FrustrumPlane.Normal.V[0] = 0.f;
	FrustrumPlane.Normal.V[1] = -(float) cos(fYFOV);
	FrustrumPlane.Normal.V[2] = (float) sin(fYFOV);
	FrustrumPlane.Distance = 0.00001f;
	if (!PlaneSet_AddM(&(pThis->FrustrumPlanes), &FrustrumPlane))
		return 0;

	/* Success. */
	return 1;
}

/********************************************************************
* Function : Viewpoint_PrepActorsForDraw()
* Purpose : Prepares the list of Actors pActors for drawing in the
*           pThis viewpoint.
* Pre : pThis points to an initialized Viewpoint structure. pActors
*       points to an initialized Actor structure with optionally
*       more Actors in it's tail (thus forming a linked list).
* Post : If the returnvalue is 1, the Actors in pActor have been
*        succesfully initialized for display.
*        If the returnvalue is 0, a memory failure occured.
* Note : Between Viewpoint_PrepActorsForDraw() and Viewpoint_Draw()
*        no other Viewpoint_PrepActorsForDraw() calls may be made for
*        other viewpoints in the same world.
* Bugs : All vertices every encountered will be recalculated every
*        time for all planes. This is quite inefficient because we
*        only need to calculate the distances for those vertices that
*        are still used by the polygons. 
********************************************************************/
int Viewpoint_PrepActorsForDraw(struct Viewpoint *pThis, struct Actor *pActors)
{
	struct Transformation TransFromActor;
	struct Transformation TransToViewpoint;
	struct Transformation FinalTrans;
	struct Vector Temporarypoint;
	struct Vector Centerpoint;
	struct Vector VPos;
	struct Plane *pFrustrumPlane;
	struct Plane TFPlane;			/* Frustrum Plane in Actor Frame. */
	struct Polygon *pSrcPoly;
	struct Polygon *pTrgPoly;
	struct Vertex *pTempVert;
	struct ScreenVertex *pSV;
	float fTempDistance;
	int n, m;
	int nXOfs, nYOfs;
	int bDropActor;
	float fCPDistance;
	
	/* Build transformation from Root to Viewpoint frame. */
	Frame_GetTransformationFromRoot(&(pThis->VpointFrame), &TransToViewpoint);

	/* Make sure that the old tree is not reused.
	 * Start over again. */
	pThis->pRootActor = NULL;

	/* Iterate all actors. */
	while (pActors != NULL)
	{
		/* Convert sphere bounding volume to viewspace. */

		/* Build transformation from Actor frame to Root frame. */
		Frame_GetTransformationToRoot(&(pActors->ActorFrame), &TransFromActor);

#ifndef NO_INLINE
		/* Transform centerpoint to root frame. */
		Transformation_TransformM(&TransFromActor, &(pActors->pModel->Centerpoint), &Temporarypoint);
		/* Transform centerpoint to Viewpoint frame. */
		Transformation_TransformM(&TransToViewpoint, &Temporarypoint, &Centerpoint);
#else
		/* Transform centerpoint to root frame. */
		Transformation_Transform(&TransFromActor, &(pActors->pModel->Centerpoint), &Centerpoint);
		/* Transform centerpoint to Viewpoint frame. */
		Transformation_Transform(&TransToViewpoint, &Centerpoint, &Centerpoint);
#endif
		
		/* Centerpoint now is in the Viewpoint frame. */
		/* Initialize the buffer swapping pointers in the current Actor. */
		pActors->pSrcPolySet = &(pActors->pModel->Polygons);
		pActors->pTrgPolySet = &(pActors->ClippedPolySetA);
		pActors->ClippedPolySetA.nCount = 0;
		pActors->ClippedPolySetB.nCount = 0;
		pActors->ClippedVertexSet.nCount = 0;

		/* Iterate all frustrum planes. */
		bDropActor = 0;
		for (n = 0; !bDropActor && (n < PlaneSet_GetCountM(&(pThis->FrustrumPlanes))); n++)
		{
			/* Get the frustrum plane at index n. */
			pFrustrumPlane = PlaneSet_GetPlaneM(&(pThis->FrustrumPlanes), n);
			
			/* Check bounding sphere against plane.
			 * This consists of checking the distance of the Centerpoint from
			 * the plane and then checking if that is within the radius or
			 * completely inside etc. */
			fCPDistance = Plane_DistanceOfVectorM(pFrustrumPlane, &Centerpoint);
			
			/* Check what the distance means... 3 possibilities :
			 * fCPDistance < -Radius ?
			 *    1. Actor is fully outside (i.e. not visible), proceed with
			 *       next Actor. (bDropActor = TRUE).
			 * ELSE
			 *    fCPDistance < Radius ?
			 *       2. Actor is intersecting the plane, clip to plane,
			 *          proceed with next plane.
			 *    ELSE
			 *       3. Actor is fully inside, proceed with next plane.
			 */
			if (fCPDistance < -(pActors->pModel->fRadius))
			{	/* Actor is fully outside this plane and therefore not
				 * visible, proceed with next Actor. */
				bDropActor = 1;
			} else
			{	if (fCPDistance < pActors->pModel->fRadius)
				{	/* Actor is intersecting the plane, clip polygons to plane. */
					Transformation_InvTransformPlane(&TransToViewpoint, pFrustrumPlane, &TFPlane);
					Transformation_InvTransformPlane(&TransFromActor, &TFPlane, &TFPlane);
					
					/* Produce a set of distances from the plane for all vertices. */
					pThis->TempFloatSet.nCount = 0;	/* Reset floatset for distances. */
					pThis->TempFloatSet2.nCount = 0;	/* Reset floatset for secondary distances. */
					
					/* Iterate all normal vertices. */
					for (m = 0; m < VertexSet_GetCountM(&(pActors->pModel->Vertices)); m++)
					{
						/* Calculate distance & add it to the DistSet. */
						pTempVert = VertexSet_GetVertexM(&(pActors->pModel->Vertices), m);
						fTempDistance = Plane_DistanceOfVectorM(&TFPlane, &(pTempVert->Position));
						FloatSet_AddM(&(pThis->TempFloatSet), fTempDistance);
					}
					/* Iterate all negatively indexed vertices. */
					for (m = 0; m < VertexSet_GetCountM(&(pActors->ClippedVertexSet)); m++)
					{
						/* Calculate distance & add it to the DistSet. */
						pTempVert = VertexSet_GetVertexM(&(pActors->ClippedVertexSet), m);
						fTempDistance = Plane_DistanceOfVectorM(&TFPlane, &(pTempVert->Position));
						FloatSet_AddM(&(pThis->TempFloatSet2), fTempDistance);
					}
					

					/* Iterate all polygons from pSrcPolySet. */
					for (m = 0; m < PolySet_GetCountM(pActors->pSrcPolySet); m++)
					{
						/* Get the source polygon. */
						pSrcPoly = PolySet_GetPolygonM(pActors->pSrcPolySet, m);

						
						/* Get a ptr to a new target polygon. */
						pTrgPoly = PolySet_GetNewM(pActors->pTrgPolySet);
						
						/* Check for memory failure */
						if (pTrgPoly == NULL)
						{
							return 0;
						}
						/* Clip Polygon pSrcPoly using pThis->TempFloatSet and store
						 * result in pTrgPoly. */
						if (!Plane_ClipPolygon(&TFPlane, pSrcPoly, &(pThis->TempFloatSet),
													  &(pActors->pModel->Vertices),
													  &(pThis->TempFloatSet2), pTrgPoly, 
													  &(pActors->ClippedVertexSet)))
						{
							return 0;	/* Memory failure. */
						}
					}
					
					/* Swap buffer pointers around. */
					if (pActors->pTrgPolySet == &(pActors->ClippedPolySetA))
					{	pActors->pTrgPolySet = &(pActors->ClippedPolySetB);
						pActors->pSrcPolySet = &(pActors->ClippedPolySetA);
					} else
					{	pActors->pTrgPolySet = &(pActors->ClippedPolySetA);	
						pActors->pSrcPolySet = &(pActors->ClippedPolySetB);
					}
					pActors->pTrgPolySet->nCount = 0;	/* Reset target buffer. */

				}
			}
		}	/* For loop for all planes. */

		
		/* If the actor should not be dropped, (!bDropActor)
		 * add the actor to the display BSP tree. */
		if (!bDropActor)
		{	/* Add the actor the the viewpoint's display BSP tree. */
			/* But first clean it's SubActorSet.
			 * Because the count value in the SubActorSet is not used,
			 * we need to clear all allocated pointers. */
			for (n = 0; n < pActors->SubActorSet.nAlloc; n++)
				ActorPtrSet_GetActorPtrM(&(pActors->SubActorSet), n) = NULL;

			if (pThis->pRootActor == NULL)
			{	pThis->pRootActor = pActors;	/* This is the first actor. */
			} else
			{	/* This is not the first actor, so insert this actor into
				 * the existing tree of actors. */
				if (!Actor_InsertActor(pThis->pRootActor, pActors))
				{	/* A memory failure occured during the insertion. */
					return 0;
				}
			}
			
			/* Transform all vertices from there 3D position to 2D screen
			 * coordinates. */
			/* Concatenate the transformation from the Actor to the Root
			 * with the transformation from the Root to the Viewpoint. */
			Transformation_Concatenate(&TransToViewpoint, &TransFromActor, &FinalTrans);
			
			/* Transform point (0,0,0) from the Viewpoint Frame to the Actor
			 * Frame. This is the Viewpoint's origin and is used in the Draw
			 * stage to traverse the Actor's BSP tree. */
			Vector_ConstructM(&VPos);
			Transformation_InvTransform(&FinalTrans, &VPos, &(pActors->ViewpointOrigin));
			
			/* Scale the transformation X and Y rows with the Multipliers. */
			Transformation_ScaleXYRowM(&FinalTrans, pThis->fXMultiplier, pThis->fYMultiplier);
			
			/* FinalTrans now contains the transformation we need to go from
			 * the Actor frame to the Viewpoint Frame whereby the X and Y axes
			 * have been scaled as such that after division by Z each vertex
			 * will represent the screen coordinate whereby the center of the
			 * screen is at (0,0). */
			/* Initializes viewpoint's Screen Vertex Sets. */
			pActors->NormalScreenVertices.nCount = 0;
			pActors->ClippedScreenVertices.nCount = 0;
			
			/* Produce a center of the screen offset from top left. */
			nXOfs = pThis->nWidth / 2;
			nYOfs = pThis->nHeight / 2;
			
			/* Transform Normal Vertices. */
			for (n = 0; n < VertexSet_GetCountM(&(pActors->pModel->Vertices)); n++)
			{
				/* Get a new ScreenVertex. */
				pSV = ScreenVertexSet_GetNewM(&(pActors->NormalScreenVertices));
				
				if (pSV == NULL)
					return 0;	/* Memory allocation failure. */

				/* Transform vertex position. */
				Transformation_TransformM(&FinalTrans, &(VertexSet_GetVertexM(&(pActors->pModel->Vertices), n)->Position), &VPos);

				/* VPos now contains the 3D position of the n'th vertex in 
				 * rescaled viewpoint space. */

				/* Make VPos 2D by division & adding half the width to the
				 * center of the screen. 
				 * THIS IS THE ACTUAL PERSPECTIVE TRANSFORMATION ! */
				if (VPos.V[2] != 0.f)
				{	/* This test should not be needed.... */
					pSV->nX = nXOfs + (short)(VPos.V[0] / VPos.V[2]);
					pSV->nY = nYOfs + (short)(VPos.V[1] / VPos.V[2]);
				}
			}
			
			/* Transform Clipped Vertices. */
			for (n = 0; n < VertexSet_GetCountM(&(pActors->ClippedVertexSet)); n++)
			{
				/* Get a new ScreenVertex. */
				pSV = ScreenVertexSet_GetNewM(&(pActors->ClippedScreenVertices));
				
				if (pSV == NULL)
					return 0;	/* Memory allocation failure. */

				/* Transform vertex position. */
				Transformation_TransformM(&FinalTrans, &(VertexSet_GetVertexM(&(pActors->ClippedVertexSet), n)->Position), &VPos);

				/* VPos now contains the 3D position of the n'th clipped vertex in
				 * rescaled viewpoint space. */
				/* Make VPos 2D by division & adding half the width to the
				 * center of the screen. 
				 * THIS IS THE ACTUAL PERSPECTIVE TRANSFORMATION ! */
				if (VPos.V[2] != 0.f)
				{	/* This test should not be needed.... */
					pSV->nX = nXOfs + (short)(VPos.V[0] / VPos.V[2]);
					pSV->nY = nYOfs + (short)(VPos.V[1] / VPos.V[2]);
				}
			}
		}
		
		/* Proceed with next actor in the list. */
		pActors = pActors->pNext;
	}
	/* Success! */
	return 1;
}

/********************************************************************
* Function : Viewpoint_DrawActorTree() (Used by Viewpoint_DrawActor)
* Purpose : Recursive function that traverses an entire HPlane
*           tree and renders polygons in the required order.
* Pre : pThis points to an initialized Viewpoint structure with
*       a bitmap associated to it. pActor points to an initialized
*       Actor structure. pPlane points to a HPlane structure from the
*       tree of with pActor's model. nLevel indicates the number
*       of subspaces left in the tree of pPlane.
*       Viewpoint_PrepActorsForDraw() must have been called on the
*       Viewpoint structure (pThis) and the Actor (pActor).
* Post : The bitmap in the Viewpoint (pThis->pBitmap) now contains
*        the whole subtree of pPlane (including all actors in the
*        subspaces) drawn.
********************************************************************/
void Viewpoint_DrawActorTree(struct Viewpoint *pThis,
								  struct Actor *pActor,
								  struct HPlane *pPlane,
								  int nLevel)
{
	int k, n, m;
	struct Polygon *pPoly;
	struct ScreenVertex *pSV, *pLastSV;

	/* Check if we reached one of our tree's leafs. */
	if (pPlane == NULL)
	{	/* We've reached a leaf, setup a new tree
		 * for traversal. */

		/* Check if there is an Actor in this leaf. */
		pActor = ActorPtrSet_GetActorPtrM(&(pActor->SubActorSet), nLevel);
		if (pActor != NULL)
		{	/* Call ourselves recursively, but now using
			 * the embedded Actor from the current subspace. */
			Viewpoint_DrawActorTree(pThis,pActor, pActor->pModel->pRoot, 0);
		}
	} else
	{	/* Check on what side the viewpoint's origin is on this
		 * given hyperplane. */
		if (0.f < Plane_DistanceOfVectorM(&(pPlane->BinPlane), &(pActor->ViewpointOrigin)))
		{
			/* The viewpoint is on the outside of the plane.
			 * first draw the inside, then draw the polygons that
			 * are coplanar with the current plane and visible from
			 * the outside of the plane,
			 * then draw the outside. This is Back to Front
			 * drawing. */
			Viewpoint_DrawActorTree(pThis, pActor, pPlane->pInSubtree, nLevel);
			
			/* Iterate all polygons visible from the outside of the plane. */
			for (n = 0; n < IndexSet_GetCountM(&(pPlane->OutsideIndices)); n++)
			{	/* Get index of polygon. */
				m = IndexSet_GetIndexM(&(pPlane->OutsideIndices), n);
				/* Get polygon from index. */
				pPoly = PolySet_GetPolygonM(pActor->pSrcPolySet, m);

				/* Get last vertex of polygon. */
				m = IndexSet_GetCountM(&(pPoly->Vertices)) - 1;
				/* Only display polygons with more than 2 vertices. */
				if (m > 1)
				{
					m = IndexSet_GetIndexM(&(pPoly->Vertices), m);
	
					/* Get ScreenVertex for vertex j. */
					if (m < 0)
						pLastSV = ScreenVertexSet_GetScreenVertexM(&(pActor->ClippedScreenVertices), ~m);
					else
						pLastSV = ScreenVertexSet_GetScreenVertexM(&(pActor->NormalScreenVertices), m);
					
					/* Iterate all vertices of poly, building spans from them in the
					 * edge table. */
					EdgeTable_WhipeM(&(pThis->PolyEdgeTable));
					for (m = 0; m < IndexSet_GetCountM(&(pPoly->Vertices)); m++)
					{	/* Get ScreenVertex. */
						k = IndexSet_GetIndexM(&(pPoly->Vertices), m);
						if (k < 0)
							pSV = ScreenVertexSet_GetScreenVertexM(&(pActor->ClippedScreenVertices), ~k);
						else
							pSV = ScreenVertexSet_GetScreenVertexM(&(pActor->NormalScreenVertices), k);
						EdgeTable_AddEdge(&(pThis->PolyEdgeTable), pLastSV, pSV);
						pLastSV = pSV;
					}
					
					/* Draw the polygon. */
					Viewpoint_DrawPolygon (pThis, pPoly);
				}
			}
			
			/* Draw the outside. */
			Viewpoint_DrawActorTree(pThis, pActor, pPlane->pOutSubtree,
											nLevel + pPlane->nInsideLeafCount);
		} else
		{
			/* The viewpoint is on the inside of the plane.
			 * first draw the outside, then draw the polygons that
			 * are coplanar with the current plane and visible from
			 * the inside of the plane,
			 * then draw the inside. This is Back to Front
			 * drawing. */
			Viewpoint_DrawActorTree(pThis, pActor, pPlane->pOutSubtree,
											nLevel + pPlane->nInsideLeafCount);
			
			/* Iterate all polygons visible from the inside of the plane. */
			for (n = 0; n < IndexSet_GetCountM(&(pPlane->InsideIndices)); n++)
			{	/* Get index of polygon. */
				m = IndexSet_GetIndexM(&(pPlane->InsideIndices), n);
				/* Get polygon from index. */
				pPoly = PolySet_GetPolygonM(pActor->pSrcPolySet, m);

				/* Get last vertex of polygon. */
				m = IndexSet_GetCountM(&(pPoly->Vertices)) - 1;
				/* Only display polygons with more than 2 vertices. */
				if (m > 1)
				{
					m = IndexSet_GetIndexM(&(pPoly->Vertices), m);
	
					/* Get ScreenVertex for vertex m. */
					if (m < 0)
						pLastSV = ScreenVertexSet_GetScreenVertexM(&(pActor->ClippedScreenVertices), ~m);
					else
						pLastSV = ScreenVertexSet_GetScreenVertexM(&(pActor->NormalScreenVertices), m);
					
					/* Iterate all vertices of poly, building spans from them in the
					 * edge table. */
					EdgeTable_WhipeM(&(pThis->PolyEdgeTable));
					for (m = 0; m < IndexSet_GetCountM(&(pPoly->Vertices)); m++)
					{	/* Get ScreenVertex. */
						k = IndexSet_GetIndexM(&(pPoly->Vertices), m);
						if (k < 0)
							pSV = ScreenVertexSet_GetScreenVertexM(&(pActor->ClippedScreenVertices), ~k);
						else
							pSV = ScreenVertexSet_GetScreenVertexM(&(pActor->NormalScreenVertices), k);
						EdgeTable_AddEdge(&(pThis->PolyEdgeTable), pLastSV, pSV);
						pLastSV = pSV;
					}

					/* Draw the polygon. */
					Viewpoint_DrawPolygon (pThis, pPoly);
				}
			}
			/* Draw the inside. */
			Viewpoint_DrawActorTree(pThis, pActor, pPlane->pInSubtree, nLevel);
		}
	}				
}
	
/********************************************************************
* Function : Viewpoint_Draw()
* Purpose : Draws all Actors that were prepared for drawing by a call
*           to Viewpoint_PrepActorsForDraw().
* Pre : pThis points to an initialized Viewpoint structure with
*       a bitmap associated with it. pThis has just been used in a
*       Viewpoint_PrepActorsForDraw() call.
* Post : All Actors were rendered in correct order in the pBitmap
*        bitmap in the Viewpoint structure.
********************************************************************/
int Viewpoint_Draw(struct Viewpoint *pThis)
{
	/* Call the Viewpoint_DrawActorTree() helper function. */
	/* Only draw something when there is an Actor inside
	 * the View Frustrum. */
	if (pThis->pRootActor != NULL)
		Viewpoint_DrawActorTree(pThis, pThis->pRootActor,
					pThis->pRootActor->pModel->pRoot,
					0);
	return 1;
}

/********************************************************************
* Function : Viewpoint_SetRendermode()
* Purpose : Select the kind of rendering we want in this Viewpoint.
*         See header about accepted modes.
* Post : Returns 1 if this Viewpoint successfully accepted this mode.
*       All rendering using this Viewpoint will use this mode,
*       other possible Viewpoints will not be affected.
********************************************************************/
int Viewpoint_SetRendermode(struct Viewpoint *pThis, unsigned char mode)
{
	switch( mode )
	{
		case CHROME_VIEWPOINT_RENDERMODE_TRUECOLOR_32:
		case CHROME_VIEWPOINT_RENDERMODE_INDEXED_8:
			pThis->nRendermode = mode;
			break;
		default:
			return 0;
	}
	return 1;
}

/********************************************************************
* Function : Viewpoint_DrawPolygon()
* Purpose : Helper to Viewpoint_DrawActorTree, is not supposed
*         to be used from anywhere else!
* Pre : pThis points to an initialized Viewpoint
*     pPoly points to a Polygon that have been prepared for drawing.
********************************************************************/
static void Viewpoint_DrawPolygon(struct Viewpoint *pThis, struct Polygon *pPoly)
{
	struct Lightmap256 *pLmap256;
	struct Lightmap1 *pLmap1;

	if (CHROME_VIEWPOINT_RENDERMODE_INDEXED_8 == pThis->nRendermode)
	{
		switch (pPoly->nFlags)
		{	case PF_STATICCOLOR :
			{	pLmap1 = (struct Lightmap1 *)pPoly->pLightmap;
				if (pLmap1 == NULL)
				{	// A problem, there's no lightmap.
					// Use color 0 for this.
					EdgeTable_SolidFill(&(pThis->PolyEdgeTable), 0,
								  (short)pThis->nPixelRow, pThis->pBitmap);
				} else
					EdgeTable_SolidFill(&(pThis->PolyEdgeTable), (unsigned char)pLmap1->nIndex,
								  (short)pThis->nPixelRow, pThis->pBitmap);
			}break;
			case PF_DYNACOLOR :
			{	pLmap256 = (struct Lightmap256 *)pPoly->pLightmap;
				if (pLmap256 == NULL)
				{	// There's no lightmap (this should not happen)
					// Use color 0.
					EdgeTable_SolidFill(&(pThis->PolyEdgeTable), 0,
								  (short)pThis->nPixelRow, pThis->pBitmap);
				} else
					EdgeTable_SolidFill(&(pThis->PolyEdgeTable), (unsigned char)pLmap256->arIndices[255],
								  (short)pThis->nPixelRow, pThis->pBitmap);
			}break;
		}
	}		
	else /* Using truecolor */
	{
		switch (pPoly->nFlags)
		{	case PF_STATICCOLOR :
			{	EdgeTable_SolidFill32(&(pThis->PolyEdgeTable), pPoly->ulRGB,
							  (short)pThis->nPixelRow, (unsigned_int_32 *) pThis->pBitmap);
			}break;
			case PF_DYNACOLOR :
			{	EdgeTable_SolidFill32(&(pThis->PolyEdgeTable), pPoly->ulRGB,
							  (short)pThis->nPixelRow, (unsigned_int_32 *) pThis->pBitmap);
			}break;
		}
	}
}
