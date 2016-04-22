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
* FILE : actor.c
********************************************************************/

#define ACTOR_C

#include <stdlib.h>
#include <math.h>

#include "actor.h"

/********************************************************************
* Function : Actor_Construct()
* Purpose : Initializes an Actor structure.
* Pre : pThis points to an invalid Actor structure.
* Post : pThis points to an initialized Actor structure.
********************************************************************/
void Actor_Construct(struct Actor *pThis)
{
	pThis->pNext = NULL;				/* Actor is not in a list. */
	pThis->pModel = NULL;			/* Actor has no model yet. */
	Frame_ConstructM(&(pThis->ActorFrame));
	PlaneSet_ConstructM(&(pThis->ClippingPlanes));
	ActorPtrSet_ConstructM(&(pThis->SubActorSet));
	PolySet_ConstructM(&(pThis->ClippedPolySetA));
	PolySet_ConstructM(&(pThis->ClippedPolySetB));

	pThis->pSrcPolySet = NULL;		/* No clipping process is running. */
	pThis->pTrgPolySet = NULL;		/* No clipping process is running. */
	
	VertexSet_ConstructM(&(pThis->ClippedVertexSet));
	
	ScreenVertexSet_Construct(&(pThis->NormalScreenVertices));
	ScreenVertexSet_Construct(&(pThis->ClippedScreenVertices));
	Vector_ConstructM(&(pThis->ViewpointOrigin));
}

/********************************************************************
* Function : Actor_Destruct()
* Purpose : Frees all memory associated with an Actor structure.
* Pre : pThis points to an initialized Actor structure.
* Post : pThis points to an invalid Actor structure that uses no
*        memory.
********************************************************************/
void Actor_Destruct(struct Actor *pThis)
{
	PlaneSet_DestructM(&(pThis->ClippingPlanes));
	ActorPtrSet_DestructM(&(pThis->SubActorSet));
	PolySet_DestructM(&(pThis->ClippedPolySetA));
	PolySet_DestructM(&(pThis->ClippedPolySetB));
	VertexSet_DestructM(&(pThis->ClippedVertexSet));
	ScreenVertexSet_Destruct(&(pThis->NormalScreenVertices));
	ScreenVertexSet_Destruct(&(pThis->ClippedScreenVertices));
}

/********************************************************************
* Function : Actor_SetModel()
* Purpose : Sets the pointer to an Actor's Model. The function 
*           initializes some buffers which must have a certain size
*           depending on the Model structure. Always use this
*           function to set a new Model structure for an Actor.
* Pre : pThis points to an initialized Actor structure, pModel points
*       to an initialized Model structure.
* Post : If the returnvalue is 1, Actor pThis now uses Model pModel
*        for it's geometry.
*        If the returnvalue is 0, a memory allocation failure
*        occured.
********************************************************************/
int Actor_SetModel(struct Actor *pThis, struct Model *pModel)
{	
	int n;
	/* Set the model pointer. */
	pThis->pModel = pModel;
	
	/* Count the total number of leafs. */
	n = HPlane_CalculateLeafCount(pModel->pRoot);
	
	/* The total number of leafs is identical to the total number
	 * of subspaces (they're the same thing). We need to ensure
	 * we have at least 'n' Actor BSP links in the SubActorSet of
	 * our Actor structure. */
	if (ActorPtrSet_AtLeast(&(pThis->SubActorSet), n))
	{	/* Succesfully expanded Actor space. */
		return 1;
	} else
	{	return 0;
	}
}

/********************************************************************
* Function : Actor_InsertActor()
* Purpose : Inserts an actor pActor into actor pThis. 
* Pre : pThis and pActor point to initialized Actor structures with
*       valid SubActorSet fields.
* Post : If the returnvalue is 1, pThis contains pActor in one of
*        it's subspaces, possibly inside another Actor (recursion).
*        If the returnvalue is 0, an error occured (memory allocation
*        failure).
********************************************************************/
int Actor_InsertActor(struct Actor *pThis, struct Actor *pActor)
{
	int nIndex;
	struct Vector Centerpoint;
	struct Transformation Trans;
	
	/* Get the centerpoint of pActor expressed in the frame of pThis. */
	
	/* First calculate the transformation from pActor to the root. */
	Frame_GetTransformationToRoot(&(pActor->ActorFrame), &Trans);
	
	/* Transform pActor's centerpoint to the root. */
	Transformation_Transform(&Trans, &(pActor->pModel->Centerpoint), &(Centerpoint));
	
	/* Calculate the transformation from the root to pThis. */
	Frame_GetTransformationFromRoot(&(pThis->ActorFrame), &Trans);
	
	/* Transform the centerpoint to the frame of pThis. */
	Transformation_Transform(&Trans, &(Centerpoint), &(Centerpoint));
	
	/* Centerpoint now contains the Centerpoint of pActor expressed in the
	 * frame of pThis. */
	
	/* Find the index of the subspace where the Centerpoint is located in 
	 * pThis. */
	nIndex = HPlane_GetVectorSubspaceIndex(pThis->pModel->pRoot, &(Centerpoint));
	
	/* Check if there already is an Actor in the subspace. */
	if (ActorPtrSet_GetActorPtrM(&(pThis->SubActorSet), nIndex) == NULL)
	{
		/* Insert the Actor here. */
		/* This weird assignment is allowed because we're using a macro. */
		ActorPtrSet_GetActorPtrM(&(pThis->SubActorSet), nIndex) = pActor;

		return 1;
	} else
	{
		/* Recurse through the Actor that's taking our place. */
		return Actor_InsertActor(ActorPtrSet_GetActorPtrM(&(pThis->SubActorSet), nIndex),
										 pActor);
	}
}

#ifdef DEBUGC
/********************************************************************
* Function : Actor_DumpScreenVertices() (DEBUG ONLY)
* Purpose : Dumps all screen vertices to stdout.
* Pre : pThis points to an initialized Actor structure.
* Post : All screen vertices have been dumped to stdout.
********************************************************************/
void Actor_DumpScreenVertices(struct Actor *pThis)
{
	int n;
	struct ScreenVertex *pSV;
	
	printf("Actor_DumpScreenVertices() -> Normal Screen Vertices :\n");
	for (n = 0; n < ScreenVertexSet_GetCountM(&(pThis->NormalScreenVertices)); n++)
	{
		/* Dump the coordinates to the screen. */
		pSV = ScreenVertexSet_GetScreenVertexM(&(pThis->NormalScreenVertices), n);
		printf("\t(%d, %d)\n", pSV->nX, pSV->nY);
	}

	printf("Actor_DumpScreenVertices() -> Clipped Screen Vertices :\n");
	for (n = 0; n < ScreenVertexSet_GetCountM(&(pThis->ClippedScreenVertices)); n++)
	{
		/* Dump the coordinates to the screen. */
		pSV = ScreenVertexSet_GetScreenVertexM(&(pThis->ClippedScreenVertices), n);
		printf("\t(%d, %d)\n", pSV->nX, pSV->nY);
	}
}
#endif

#ifdef DEBUGC
/********************************************************************
* Function : Actor_DumpScreenPolygons()
* Purpose : Dumps all polygons to stdout by their screen coordinates.
* Pre : pThis points to an initialized Actor structure.
* Post : All polygons that are to be displayed have been dumped to
*        stdout.
********************************************************************/
void Actor_DumpScreenPolygons(struct Actor *pThis)
{
	int	n, m, k;
	struct Polygon *pPoly;
	struct ScreenVertex *pSV;
	
	/* Iterate all normal polygons. */
	printf("Actor_DumpScreenPolygons() -> %d polygons.\n", PolySet_GetCountM(pThis->pSrcPolySet));
	
	for (n = 0; n < PolySet_GetCountM(pThis->pSrcPolySet); n++)
	{	/* Get current polygon. */
		pPoly = PolySet_GetPolygonM(pThis->pSrcPolySet, n);
		
		/* Iterate all of the polygon's vertices. */
		printf("\t# vertices = %d, color = %d ('%c')\n", IndexSet_GetCountM(&(pPoly->Vertices)), pPoly->nColor, pPoly->nColor);
		
		for (m = 0; m < IndexSet_GetCountM(&(pPoly->Vertices)); m++)
		{
			k = IndexSet_GetIndexM(&(pPoly->Vertices), m);
			
			/* Get ScreenVertex. */
			if (k < 0)
				pSV = ScreenVertexSet_GetScreenVertexM(&(pThis->ClippedScreenVertices), ~k);
			else
				pSV = ScreenVertexSet_GetScreenVertexM(&(pThis->NormalScreenVertices), k);
			
			/* Display screen vertex. */
			printf("\t\t[%d](%d,%d)\n", k, (int)pSV->nX, (int)pSV->nY);
		}
	}
}
#endif