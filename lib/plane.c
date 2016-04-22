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
* FILE : plane.c
********************************************************************/

#include "plane.h"

#include "vector.h"
#include "vertex.h"
#include "vertxset.h"
#include "polygon.h"
#include "floatset.h"

/********************************************************************
* Function : Plane_Construct()
* Purpose : Initializes a plane structure.
* Pre : pThis points to a Plane structure.
* Post : pThis contains a plane whose normal points in the Z axis
*        direction and whose distance from the origin is 0.
********************************************************************/
void Plane_Construct(struct Plane *pThis)
{
	pThis->Normal.V[0] = 0.f;
	pThis->Normal.V[1] = 0.f;
	pThis->Normal.V[2] = 1.f;
	
	pThis->Distance = 0.f;
}

/********************************************************************
* Function : Plane_DistanceOfVector()
* Purpose : Calculates the distance of pVector from Plane pThis.
* Pre : pThis points to an initialized Plane structure. pVector
*       points to an initialized Vector structure.
* Post : Returnvalue contains the distance of pVector from the Plane
*        pThis. This value may be negative in which case pVector lies
*        in the backside of the plane.
********************************************************************/
float Plane_DistanceOfVector(struct Plane *pThis,
                             struct Vector *pVector)
{
	return Plane_DistanceOfVectorM(pThis, pVector);
}

/********************************************************************
* Function : Plane_ClipPolygon()
* Purpose : Clips a polygon pSrcPolygon to a plane pThis, removing
*           the side of the polygon opposite to the plane's normal.
* Pre : pThis points to an initialized plane structure.
*       pSrcPolygon points to an initialized polygon structure
*       containing the polygon to be clipped. Negative indices
*       point to vertices in pTrgVertices and it's distances can
*       be found in pNIDistances.
*       pDistances points to an initialized FloatSet structure
*       containing the distances to the plane for all positive
*       indices in pSrcPolygon.
*       pSrcVertices points to an initialized VertexSet structure
*       containing the vertices for all positive indices in
*       pSrcPolygon.
*       pNIDistances points to an initialized FloatSet structure
*       containing the distances to the plane for all negative
*       indices in pSrcPolygon.
*       pTrgPolygon points to an initialized Polygon structure that
*       is empty.
*       pTrgVertices points to an initialized VertexSet structure
*       containing the vertices for all negative indices in
*       pSrcPolygon.
* Post : If the returnvalue is 1, pTrgPolygon now contains the
*        clipped polygon. Newly created vertices have negative
*        indices that point into pTrgVertices.
*        pTrgVertices now also contains all newly created vertices.
********************************************************************/
int Plane_ClipPolygon(struct Plane *pThis,
							 struct Polygon *pSrcPolygon,
							 struct FloatSet *pDistances,
							 struct VertexSet *pSrcVertices,
							 struct FloatSet *pNIDistances,
							 struct Polygon *pTrgPolygon,
							 struct VertexSet *pTrgVertices)
{
	int n;
	int nwIndex;				/* Index for a new vertex. */
	float fLastDistance;		/* Last distance from plane. */
	int LastVIndex;			/* Last Vertex index. */
	float fDistance;			/* Current distance from plane. */
	int VIndex;					/* Current Vertex Index. */
	struct Vertex *pV0;		/* Last vertex ptr. */
	struct Vertex *pV1;		/* Current vertex ptr. */
	struct Vertex TV;			/* Temporary result vertex. */
	float fInterpol;			/* Interpolation value. */
	
	/* Iterate through all edges of the polygon and intersect
	 * with the plane if needed. */

	/* Set shading information in target polygon. */
	pTrgPolygon->ulRGB = pSrcPolygon->ulRGB;
	pTrgPolygon->pLightmap = pSrcPolygon->pLightmap;
	pTrgPolygon->nFlags = pSrcPolygon->nFlags;
	
	/* Initialize variables for loop. */
	/* Find last vertex index. */
	n = IndexSet_GetCountM(&(pSrcPolygon->Vertices));
	/* If there are no vertices in the polygon (or less than 2), 
	 * there's no polygon to clip so we can stop this right away.
	 * Note that this is not an error. */
	if (n < 2)
		return 1;
	n = n - 1;
	LastVIndex = IndexSet_GetIndexM(&(pSrcPolygon->Vertices), n);

	/* Find last vertex's distance from plane. */
	if (LastVIndex < 0)
 		fLastDistance = FloatSet_GetFloatM(pNIDistances, ~LastVIndex);
	else
		fLastDistance = FloatSet_GetFloatM(pDistances, LastVIndex);

	/* Iterate for all edges. */
	for (n = 0; n < IndexSet_GetCountM(&(pSrcPolygon->Vertices)); n++)
	{
		/* Find vertex index. */
		VIndex = IndexSet_GetIndexM(&(pSrcPolygon->Vertices), n);

		/* Find vertex's distance from plane. */
		if (VIndex < 0)
			fDistance = FloatSet_GetFloatM(pNIDistances, ~VIndex);
		else
			fDistance = FloatSet_GetFloatM(pDistances, VIndex);
		
		/* Classify edge, 4 possibilities :
		 * Sign of   | Sign of       | Action
		 * fDistance | fLastDistance |
		 * ----------+---------------+-------------------------------------
		 * Positive  | Positive      | Add VIndex to pTrgPolygon.
		 * ----------+---------------+-------------------------------------
		 * Positive  | Negative      | Calculate intersection & add to
		 *           |               | pTrgPolygon. Add VIndex to
		 *           |               | pTrgPolygon.
		 * ----------+---------------+-------------------------------------
		 * Negative  | Positive      | Calculate the intersection & add to
		 *           |               | pTrgPolygon.
		 * ----------+---------------+-------------------------------------
		 * Negative  | Negative      | Drop vertex. (Do nothing).
		 * ----------+---------------+-------------------------------------
		 */
		/* If either is positive and the other negative,
		 * add the intersection vertex. */
		if ((fDistance >= 0.f) != (fLastDistance >= 0.f))
		{
			/* Find the intersection vertex & add it. */

			/* Get the two vertices. */
			if (VIndex < 0)
				pV1 = VertexSet_GetVertexM(pTrgVertices, ~VIndex);
			else
				pV1 = VertexSet_GetVertexM(pSrcVertices, VIndex);

			if (LastVIndex < 0)
				pV0 = VertexSet_GetVertexM(pTrgVertices, ~LastVIndex);
			else
				pV0 = VertexSet_GetVertexM(pSrcVertices, LastVIndex);

			/* Interpolate the two vertices. */
			if (fDistance != fLastDistance)		/* Avoid division by zero. */
			{	
				fInterpol = fLastDistance / (fLastDistance - fDistance);
			} else
			{	fInterpol = 0.f;
			}
			
			Vertex_InterpolateM(pV0, pV1, fInterpol, &TV);

			/* Add the interpolated vertex to the target vertexset. */
			if (!VertexSet_AddM(pTrgVertices, &TV))
			{
				return 0;	/* Mem failure */
			}
			
			nwIndex = VertexSet_GetCountM(pTrgVertices) - 1;

			/* Add the negated index to pTrgPoly. */
			if (!IndexSet_AddM(&(pTrgPolygon->Vertices), ~nwIndex))
			{
				return 0;	/* Mem failure */
			}
		}
	
		/* If the current vertex is on the right side, add it to the
		 * target polygon. */
		if (fDistance >= 0.f)
		{	/* Add it to the target polygon. */
			if (!IndexSet_AddM(&(pTrgPolygon->Vertices), VIndex))
			{
				return 0;	/* Mem failure */
			}
		}
		
		/* Switch around, current becomes last. */
		LastVIndex = VIndex;
		fLastDistance = fDistance;
	}
	
	/* Done. */
	return 1;
}

	