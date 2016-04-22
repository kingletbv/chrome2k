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
* FILE : hplane.c
********************************************************************/

#define HPLANE_C

#include <stdlib.h>
#include <limits.h>
#ifdef DEBUGC
#include <stdio.h>
#endif

#include "hplane.h"
#include "vertex.h"
#include "floatset.h"
#include "vertxset.h"
#include "indexset.h"
#include "polyset.h"
#include "polygon.h"

/********************************************************************
* Function : HPlane_Construct()
* Purpose : Initializes a SINGLE HPlane structure.
* Pre : pThis points to an invalid HPlane structure.
* Post : pThis points to an initialized HPlane structure.
********************************************************************/
void HPlane_Construct(struct HPlane *pThis)
{	IndexSet_ConstructM(&(pThis->InsideIndices));
	IndexSet_ConstructM(&(pThis->OutsideIndices));
	pThis->nInsideLeafCount = 0;
	pThis->pInSubtree = NULL;
	pThis->pOutSubtree = NULL;
	Plane_ConstructM(&(pThis->BinPlane));
}

/********************************************************************
* Function : HPlane_ConstructTree()
* Purpose : Builds a BSP Tree (consisting of HyperPlanes) from a
*           PolySet and a VertexSet. After this (recursive & slow)
*           process, the BSP Tree can be used in a Model for 
*           displaying.
* Pre : pPolygons points to an initialized PolySet structure,
*       pVertices points to an initialized VertexSet structure.
*       pVertices contains the vertices that are referenced by the
*       polygons contained in pPolygons.
*       pNewPolygons points to an initialized PolySet structure,
*       preferably with no polygons in it.
* Post : If the returnvalue is NULL, a memory allocation failure
*        occured or there were no polygons.
*        If the returnvalue is not NULL, pVertices has new vertices
*        appended to it that were required for the BSP tree,
*        pNewPolygons contains all the polygons referenced to in
*        the HPlane BSP tree. The returnvalue points to the first
*        (root) node of the BSP tree.
* Bug : When a memory failure occurs, only a partial BSP Tree will
*       be produced. This BSP Tree will then be returned without any
*       notice of the memory failure.
********************************************************************/
struct HPlane *HPlane_ConstructTree(struct PolySet *pPolygons,
												struct VertexSet *pVertices,
												struct PolySet *pNewPolygons)
{
	struct PolySet InSpacePolys;		/* Polygons for the IN side of the
												 * plane. */
	struct PolySet OutSpacePolys;		/* Polygons for the OUT side of the
												 * plane. */
	struct FloatSet VertDistances;	/* Distance of all vertices to a
												 * plane. */
	struct Plane Intersector;			/* Plane used for this intersection. */
	float fDistance;						/* Variable used for distance
												 * computations. */
	struct Vertex *pVertex;				/* Dummy pVertex used for retrieval. */
	int h, k,l,m;							/* Dummy int's used for loops. */
	int IntersectorCount;				/* Number of polygons that intersected
												 * with a given plane. */
	int LeastInterCount;					/* The currently lowest number of
												 * polygons that intersected with a
												 * given plane. */
	int IntersectorIndex;				/* Index that specifies the current
												 * best candidate for a split. */
	int bNeg, bPos;						/* Two booleans, bNeg determines if there
												 * are negative distances for a given
												 * polygon, bPos determines if there are
												 * positive distances for a given polygon.
												 */
	struct Polygon *pPoly;				/* Dummy polygon pointer. */
	struct HPlane *pHPlane;				/* HPlane for current subspace. */
	struct Vector Normal;				/* Normal vector for a given polygon. */
	struct Polygon InPol;				/* Polygon for Inside splitted polygons. */
	struct Polygon OutPol;				/* Polygon for Outside splitted polygons. */
	int nPolyIndex;						/* Index of polygons just added to 
												 * pNewPolygons. */

	/* Check if this subspace is empty or solid. */
	if ((pPolygons == NULL) ||
		 (pPolygons->nCount == 0))
	{	/* No more polygons to insert, return NULL. */
		return NULL;
	} else
	{
		/* Initialize some variables. */
		FloatSet_ConstructM(&VertDistances);
		PolySet_ConstructM(&OutSpacePolys);
		PolySet_ConstructM(&InSpacePolys);
		Plane_ConstructM(&Intersector);
		Polygon_ConstructM(&InPol);
		Polygon_ConstructM(&OutPol);
		
		/* Determine best polygon for fitting by iterating each polygon,
		 * building a plane and performing a distance check for all
		 * vertices. */
		/* Set the LeastInterCount to the Roof of an integer. */
		LeastInterCount = INT_MAX;			/* From : <limits.h> */
#ifdef DEBUGC
		printf("HPlane_ConstructTree() -> Initial run, trying to determine best plane.\n");
#endif
		for (k = 0; k < pPolygons->nCount; k++)
		{
			/* Build a plane for the current polygon. */
			Polygon_ExtractPlane(PolySet_GetPolygonM(pPolygons, k), 
										pVertices,
										&Intersector);
			
			/* Set number of distances to 0. This ensures that the
			 * distances will receive the same index as their
			 * vertex counterparts. */
			VertDistances.nCount = 0;
			
			/* Iterate all vertices & produce a distance table. */
			for (l = 0; l < pVertices->nCount; l++)
			{
				/* Compute the distance of this vertex l to the Intersector. */
				pVertex = VertexSet_GetVertexM(pVertices, l);
				fDistance = Plane_DistanceOfVectorM(&Intersector, &(pVertex->Position));
				
				FloatSet_AddM(&VertDistances, fDistance);
			}
			
			/* Produce a count of the number of polygons that intersect
			 * with this plane. */
			IntersectorCount = 0;			/* Start at 0. */
			for (l = 0; l < pPolygons->nCount; l++)
			{
				bNeg = 0;	/* No vertices have been marked as negative. */
				bPos = 0;	/* No vertices have been marked as positive. */
				
				pPoly = PolySet_GetPolygonM(pPolygons, l);
				
				/* Iterate all the polygon's vertices. */
				for (m = 0; m < pPoly->Vertices.nCount; m++)
				{
					/* Retrieve the current distance value for this
					 * vertex. */
					h = IndexSet_GetIndexM(&(pPoly->Vertices), m);
					fDistance = FloatSet_GetFloatM(&VertDistances, h);
					
					/* Check if it is positive... */
					if (fDistance >= ISONPLANE)
						bPos = 1;			/* Consider it positive. */
					
					/* Check if it is negative... */
					if (fDistance <= -ISONPLANE)
						bNeg = 1;			/* Consider it negative. */
				}
				
				/* If the polygon has both positive and negative vertices,
				 * it intersects the plane and we need to increment the
				 * IntersectorCount. */
				if (bPos && bNeg)
					IntersectorCount++;
			}
			
			/* We now have an intersector count, check if it is lower (better)
			 * than the currently lowest intersector count. */
			if (IntersectorCount < LeastInterCount)
			{
				/* Our plane is better than the current plane,
				 * replace it. */
				LeastInterCount = IntersectorCount;
				IntersectorIndex = k;
			}
		}
		
		/* IntersectorIndex now holds the index to the polygon we'll use
		 * as the splitting polygon for this node. */
		/* Build a new HPlane. */
		pHPlane = (struct HPlane *)malloc(sizeof(struct HPlane));
		
		/* Check for memory failure. */
		if (pHPlane != NULL)
		{
			/* Initialize the HPlane. */
			HPlane_ConstructM(pHPlane);
			pHPlane->pInSubtree = NULL;
			pHPlane->pOutSubtree = NULL;
			Plane_ConstructM(&(pHPlane->BinPlane));
			IndexSet_ConstructM(&(pHPlane->InsideIndices));
			IndexSet_ConstructM(&(pHPlane->OutsideIndices));
#ifdef DEBUGC
			printf("HPlane_ConstructTree() -> Index of intersector = %d\n", IntersectorIndex);
#endif			
			/* Compute splitting plane. */
			Polygon_ExtractPlane(PolySet_GetPolygonM(pPolygons, IntersectorIndex), 
										pVertices,
										&(pHPlane->BinPlane));
			/* Initialize the Intersector plane. */
			Intersector = pHPlane->BinPlane;
			/* Build a new vertex distance table for the Splitting plane. */
			VertDistances.nCount = 0;		/* Reset distance count to 0. */
			for (k = 0; k < pVertices->nCount; k++)
			{
				/* Compute the distance of this vertex l to the Intersector. */
				pVertex = VertexSet_GetVertexM(pVertices, k);
				fDistance = Plane_DistanceOfVectorM(&Intersector, &(pVertex->Position));
				
				if (!FloatSet_AddM(&VertDistances, fDistance))
				{	/* Memory failure. */
#ifdef DEBUGC
					printf("HPlane_ConstructTree() -> MemFailure at point #1\n");
#endif			
					FloatSet_DestructM(&VertDistances);
					PolySet_DestructM(&OutSpacePolys);
					PolySet_DestructM(&InSpacePolys);
					Polygon_DestructM(&InPol);
					Polygon_DestructM(&OutPol);
					free(pHPlane);
					return NULL;
				}
			}
			
			/* Iterate all polygons for classification. */
			for (k = 0; k < pPolygons->nCount; k++)
			{
				bNeg = 0;	/* No vertices have been marked as negative. */
				bPos = 0;	/* No vertices have been marked as positive. */
				
				pPoly = PolySet_GetPolygonM(pPolygons, k);
				
				/* Iterate all the polygon's vertices. */
				for (m = 0; m < pPoly->Vertices.nCount; m++)
				{
					/* Retrieve the current distance value for this
					 * vertex. */
					h = IndexSet_GetIndexM(&(pPoly->Vertices), m);
					fDistance = FloatSet_GetFloatM(&VertDistances, h);
					
					/* Check if it is positive... */
					if (fDistance >= ISONPLANE)
						bPos = 1;			/* Consider it positive. */
					
					/* Check if it is negative... */
					if (fDistance <= -ISONPLANE)
						bNeg = 1;			/* Consider it negative. */
				}
				
				/* Next, figure out what to do with the polygon.
				 * There are 4 cases possible :
				 * ------------+-------------+---------------------------
				 * bPos = TRUE | bNeg = TRUE | What to do
				 * ------------+-------------+---------------------------
				 * No          | No          | Polygon is coplanar, check
				 *             |             | polygon's normal vector,
				 *             |             | add to pNewPolygons and
				 *             |             | add to pHPlane, which side
				 *             |             | depends on the angle of
				 *             |             | normal vector and the
				 *             |             | split plane's normal vec.
				 * ------------+-------------+---------------------------
				 * No          | Yes         | Polygon is entirely in
				 *             |             | IN subspace. Add polygon
				 *             |             | to InSpacePolys.
				 * ------------+-------------+---------------------------
				 * Yes         | No          | Polygon is entirely in
				 *             |             | OUT subspace. Add polygon
				 *             |             | to OutSpacePolys.
				 * ------------+-------------+---------------------------
				 * Yes         | Yes         | Polygon spans split plane,
				 *             |             | split polygon and insert
				 *             |             | both fragments in
				 *             |             | corresponding subspace
				 *             |             | set.
				 */
				if (((!bPos) && (!bNeg)) || (IntersectorIndex == k))
				{	/* Polygon is neither on negative side, nor on
					 * the positive side. Polygon has to be coplanar or
					 * it's the splitter polygon. (If the splitter polygon
					 * is REALLY crap (non-planar) it may be considered
					 * a spanning polygon which is why we check for it
					 * here once more. */
					 
					/* Polygon is not on negative side either, polygon
					 * has to be coplanar. */

					/* Extract polygon's normal vector. */
					Polygon_ExtractNormal(pPoly, pVertices, &(Normal));
						
					/* Add polygon to pNewPolygons. */
					nPolyIndex = pNewPolygons->nCount;		/* Get index of polygon
																			 * insertion below */
					if (PolySet_AddM(pNewPolygons, pPoly))
					{	/* Succesfully added polygon to PolySet.						
						
						/* Check angle of Normal vector with the split plane's
						 * normal vector. */
						if ((Normal.V[0] * pHPlane->BinPlane.Normal.V[0] +
							  Normal.V[1] * pHPlane->BinPlane.Normal.V[1] +
							  Normal.V[2] * pHPlane->BinPlane.Normal.V[2]) >= 0.f)
						{	/* Polygon's normal vector lies in the same direction
							 * as the split plane's normal vector,
							 * Add the polygon to the OutsideIndices. */
							if (!IndexSet_AddM(&(pHPlane->OutsideIndices), nPolyIndex))
							{	/* Failed to add the index of the new polygon due to
								 * a memory failure. Clean up and return NULL. */
#ifdef DEBUGC
								printf("HPlane_ConstructTree() -> MemFailure at point #2\n");
#endif			
								FloatSet_DestructM(&VertDistances);
								PolySet_DestructM(&OutSpacePolys);
								PolySet_DestructM(&InSpacePolys);
								Polygon_DestructM(&InPol);
								Polygon_DestructM(&OutPol);
								free(pHPlane);
								return NULL;
							}
						} else
						{	/* Polygon's normal vector lies in the opposite direction
							 * as the split plane's normal vector,
							 * Add the polygon to the InsideIndices. */
							if (!IndexSet_AddM(&(pHPlane->InsideIndices), nPolyIndex))
							{	/* Failed to add the index of the new polygon due to
								 * a memory failure. Clean up and return NULL. */
#ifdef DEBUGC
								printf("HPlane_ConstructTree() -> MemFailure at point #3\n");
#endif			
								FloatSet_DestructM(&VertDistances);
								PolySet_DestructM(&OutSpacePolys);
								PolySet_DestructM(&InSpacePolys);
								Polygon_DestructM(&InPol);
								Polygon_DestructM(&OutPol);
								free(pHPlane);
								return NULL;
							}
						}
					} else
					{	/* Failed to add the polygon to pNewPolygons due to
						 * lack of memory.
						 * Clean up and return NULL. */
#ifdef DEBUGC
						printf("HPlane_ConstructTree() -> MemFailure at point #4\n");
#endif			
						FloatSet_DestructM(&VertDistances);
						PolySet_DestructM(&OutSpacePolys);
						PolySet_DestructM(&InSpacePolys);
						Polygon_DestructM(&InPol);
						Polygon_DestructM(&OutPol);
						free(pHPlane);
						return NULL;
					}
				} else
				if ((!bPos) && (bNeg))
				{	/* Polygon lies on negative (IN) side. */
					/* Add it to the InSpacePolys. */
					if (!PolySet_AddM(&InSpacePolys, pPoly))
					{	/* Failed to add the polygon to pNewPolygons due to
						 * lack of memory.
						 * Clean up and return NULL. */
#ifdef DEBUGC
						printf("HPlane_ConstructTree() -> MemFailure at point #5\n");
#endif			
						FloatSet_DestructM(&VertDistances);
						PolySet_DestructM(&OutSpacePolys);
						PolySet_DestructM(&InSpacePolys);
						Polygon_DestructM(&InPol);
						Polygon_DestructM(&OutPol);
						free(pHPlane);
						return NULL;
					}
				} else
				if ((bPos) && (!bNeg))
				{	/* Polygon lies on positive (OUT) side. */
					/* Add it to the OutSpacePolys. */
					if (!PolySet_AddM(&OutSpacePolys, pPoly))
					{	/* Failed to add the polygon to pNewPolygons due to
						 * lack of memory.
						 * Clean up and return NULL. */
#ifdef DEBUGC
						printf("HPlane_ConstructTree() -> MemFailure at point #6\n");
#endif			
						FloatSet_DestructM(&VertDistances);
						PolySet_DestructM(&OutSpacePolys);
						PolySet_DestructM(&InSpacePolys);
						Polygon_DestructM(&InPol);
						Polygon_DestructM(&OutPol);
						free(pHPlane);
						return NULL;
					}
				} else
				{	/* Polygon spans the splitter plane.
					 * Build two seperate polygons and add them
					 * to both OutSpacePolys and InSpacePolys. */
					InPol.Vertices.nCount = 0;
					InPol.ulRGB = pPoly->ulRGB;
					OutPol.Vertices.nCount = 0;
					OutPol.ulRGB = pPoly->ulRGB;
					if ((!HPlane_SplitPolygon(pHPlane, pPoly, &VertDistances, pVertices, &InPol, &OutPol)) ||
						 (!PolySet_AddM(&OutSpacePolys, &OutPol)) ||
						 (!PolySet_AddM(&InSpacePolys, &InPol)))
					{
						/* A memory failure occured in any of the above three operations. */
						/* Clean up & return NULL. */
#ifdef DEBUGC
						printf("HPlane_ConstructTree() -> MemFailure at point #7\n");
#endif			
						FloatSet_DestructM(&VertDistances);
						PolySet_DestructM(&OutSpacePolys);
						PolySet_DestructM(&InSpacePolys);
						Polygon_DestructM(&InPol);
						Polygon_DestructM(&OutPol);
						free(pHPlane);
						return NULL;
					}
				}
			} /* End of polygon iteration for classification. */
		} else	/* MemFailure check for pHPlane allocation. */
		{	/* Failed to allocate pHPlane. */
			/* Clean up & return NULL. */
#ifdef DEBUGC
			printf("HPlane_ConstructTree() -> MemFailure at point #8\n");
#endif			
			FloatSet_DestructM(&VertDistances);
			PolySet_DestructM(&OutSpacePolys);
			PolySet_DestructM(&InSpacePolys);
			Polygon_DestructM(&InPol);
			Polygon_DestructM(&OutPol);
			return NULL;
		}
		
		/* If we made it this far, our status should be something like this :
		 * OutSpacePolys, contains the polygons that should be inserted in
		 *                the out-subspace.
		 * InSpacePolys, contains the polygons that should be inserted in
		 *               the in-subspace.
		 * VertDistances, should be trashed, has memory locked.
		 * InPol, should be trashed, has memory locked.
		 * OutPol, should be trashed, has memory locked.
		 * pHPlane, contains the current Hyperplane, with some linked indices
		 *          for the coplanar planes.
		 */
		/* First we'll free some memory. */
		FloatSet_DestructM(&VertDistances);
		Polygon_DestructM(&InPol);
		Polygon_DestructM(&OutPol);
		
		/* Now we can go into recursion. */

		/* Call for in-plane. */
		pHPlane->pInSubtree = HPlane_ConstructTree(&InSpacePolys, pVertices, pNewPolygons);

		/* Now we can free the InSpacePolys because they're not needed anymore. */
		PolySet_DestructM(&InSpacePolys);

		/* Call for out-plane. */
		pHPlane->pOutSubtree = HPlane_ConstructTree(&OutSpacePolys, pVertices, pNewPolygons);
		
		/* We're almost done. Free remaining memory. */
		PolySet_DestructM(&OutSpacePolys);
		
		/* And return the HPlane we so painfully created. */
		return pHPlane;

	} /* Void subspace check. */
}

/********************************************************************
* Function : HPlane_ConstructTreeQuick()
* Purpose : Builds a BSP Tree (consisting of HyperPlanes) from a
*           PolySet and a VertexSet. After this (recursive & slow)
*           process, the BSP Tree can be used in a Model for 
*           displaying. The difference of this function and
*           HPlane_ConstructTree() is that this function tries to
*           produce a BSP tree quickly whereas HPlane_ConstructTree()
*           tries to produce a good BSP tree with few leaves.
* Pre : pPolygons points to an initialized PolySet structure,
*       pVertices points to an initialized VertexSet structure.
*       pVertices contains the vertices that are referenced by the
*       polygons contained in pPolygons.
*       pNewPolygons points to an initialized PolySet structure,
*       preferably with no polygons in it.
* Post : If the returnvalue is NULL, a memory allocation failure
*        occured or there were no polygons.
*        If the returnvalue is not NULL, pVertices has new vertices
*        appended to it that were required for the BSP tree,
*        pNewPolygons contains all the polygons referenced to in
*        the HPlane BSP tree. The returnvalue points to the first
*        (root) node of the BSP tree.
* Bug : When a memory failure occurs, only a partial BSP Tree will
*       be produced. This BSP Tree will then be returned without any
*       notice of the memory failure.
********************************************************************/
struct HPlane *HPlane_ConstructTreeQuick(struct PolySet *pPolygons,
													  struct VertexSet *pVertices,
													  struct PolySet *pNewPolygons)
{
	struct PolySet InSpacePolys;		/* Polygons for the IN side of the
												 * plane. */
	struct PolySet OutSpacePolys;		/* Polygons for the OUT side of the
												 * plane. */
	struct FloatSet VertDistances;	/* Distance of all vertices to a
												 * plane. */
	struct Plane Intersector;			/* Plane used for this intersection. */
	float fDistance;						/* Variable used for distance
												 * computations. */
	struct Vertex *pVertex;				/* Dummy pVertex used for retrieval. */
	int h, k, m;							/* Dummy int's used for loops. */
	int IntersectorIndex;				/* Index that specifies the current
												 * best candidate for a split. */
	int bNeg, bPos;						/* Two booleans, bNeg determines if there
												 * are negative distances for a given
												 * polygon, bPos determines if there are
												 * positive distances for a given polygon.
												 */
	struct Polygon *pPoly;				/* Dummy polygon pointer. */
	struct HPlane *pHPlane;				/* HPlane for current subspace. */
	struct Vector Normal;				/* Normal vector for a given polygon. */
	struct Polygon InPol;				/* Polygon for Inside splitted polygons. */
	struct Polygon OutPol;				/* Polygon for Outside splitted polygons. */
	int nPolyIndex;						/* Index of polygons just added to 
												 * pNewPolygons. */

	/* Check if this subspace is empty or solid. */
	if ((pPolygons == NULL) ||
		 (pPolygons->nCount == 0))
	{	/* No more polygons to insert, return NULL. */
		return NULL;
	} else
	{
		/* Initialize some variables. */
		FloatSet_ConstructM(&VertDistances);
		PolySet_ConstructM(&OutSpacePolys);
		PolySet_ConstructM(&InSpacePolys);
		Plane_ConstructM(&Intersector);
		Polygon_ConstructM(&InPol);
		Polygon_ConstructM(&OutPol);
		
		/* Just take the first polygon. */
		IntersectorIndex = 0;
		
		/* IntersectorIndex now holds the index to the polygon we'll use
		 * as the splitting polygon for this node. */
		/* Build a new HPlane. */
		pHPlane = (struct HPlane *)malloc(sizeof(struct HPlane));
		
		/* Check for memory failure. */
		if (pHPlane != NULL)
		{
			/* Initialize the HPlane. */
			HPlane_ConstructM(pHPlane);
			pHPlane->pInSubtree = NULL;
			pHPlane->pOutSubtree = NULL;
			Plane_ConstructM(&(pHPlane->BinPlane));
			IndexSet_ConstructM(&(pHPlane->InsideIndices));
			IndexSet_ConstructM(&(pHPlane->OutsideIndices));
#ifdef DEBUGC
			printf("HPlane_ConstructTree() -> Index of intersector = %d\n", IntersectorIndex);
#endif			
			/* Compute splitting plane. */
			Polygon_ExtractPlane(PolySet_GetPolygonM(pPolygons, IntersectorIndex), 
										pVertices,
										&(pHPlane->BinPlane));
			/* Initialize the Intersector plane. */
			Intersector = pHPlane->BinPlane;
			/* Build a new vertex distance table for the Splitting plane. */
			VertDistances.nCount = 0;		/* Reset distance count to 0. */
			for (k = 0; k < pVertices->nCount; k++)
			{
				/* Compute the distance of this vertex l to the Intersector. */
				pVertex = VertexSet_GetVertexM(pVertices, k);
				fDistance = Plane_DistanceOfVectorM(&Intersector, &(pVertex->Position));
				
				if (!FloatSet_AddM(&VertDistances, fDistance))
				{	/* Memory failure. */
#ifdef DEBUGC
					printf("HPlane_ConstructTree() -> MemFailure at point #1\n");
#endif			
					FloatSet_DestructM(&VertDistances);
					PolySet_DestructM(&OutSpacePolys);
					PolySet_DestructM(&InSpacePolys);
					Polygon_DestructM(&InPol);
					Polygon_DestructM(&OutPol);
					free(pHPlane);
					return NULL;
				}
			}
			
			/* Iterate all polygons for classification. */
			for (k = 0; k < pPolygons->nCount; k++)
			{
				bNeg = 0;	/* No vertices have been marked as negative. */
				bPos = 0;	/* No vertices have been marked as positive. */
				
				pPoly = PolySet_GetPolygonM(pPolygons, k);
				
				/* Iterate all the polygon's vertices. */
				for (m = 0; m < pPoly->Vertices.nCount; m++)
				{
					/* Retrieve the current distance value for this
					 * vertex. */
					h = IndexSet_GetIndexM(&(pPoly->Vertices), m);
					fDistance = FloatSet_GetFloatM(&VertDistances, h);
					
					/* Check if it is positive... */
					if (fDistance >= ISONPLANE)
						bPos = 1;			/* Consider it positive. */
					
					/* Check if it is negative... */
					if (fDistance <= -ISONPLANE)
						bNeg = 1;			/* Consider it negative. */
				}
				
				/* Next, figure out what to do with the polygon.
				 * There are 4 cases possible :
				 * ------------+-------------+---------------------------
				 * bPos = TRUE | bNeg = TRUE | What to do
				 * ------------+-------------+---------------------------
				 * No          | No          | Polygon is coplanar, check
				 *             |             | polygon's normal vector,
				 *             |             | add to pNewPolygons and
				 *             |             | add to pHPlane, which side
				 *             |             | depends on the angle of
				 *             |             | normal vector and the
				 *             |             | split plane's normal vec.
				 * ------------+-------------+---------------------------
				 * No          | Yes         | Polygon is entirely in
				 *             |             | IN subspace. Add polygon
				 *             |             | to InSpacePolys.
				 * ------------+-------------+---------------------------
				 * Yes         | No          | Polygon is entirely in
				 *             |             | OUT subspace. Add polygon
				 *             |             | to OutSpacePolys.
				 * ------------+-------------+---------------------------
				 * Yes         | Yes         | Polygon spans split plane,
				 *             |             | split polygon and insert
				 *             |             | both fragments in
				 *             |             | corresponding subspace
				 *             |             | set.
				 */
				if (((!bPos) && (!bNeg)) || (IntersectorIndex == k))
				{	/* Polygon is neither on negative side, nor on
					 * the positive side. Polygon has to be coplanar or
					 * it's the splitter polygon. (If the splitter polygon
					 * is REALLY crap (non-planar) it may be considered
					 * a spanning polygon which is why we check for it
					 * here once more. */
					 
					/* Polygon is not on negative side either, polygon
					 * has to be coplanar. */

					/* Extract polygon's normal vector. */
					Polygon_ExtractNormal(pPoly, pVertices, &(Normal));
						
					/* Add polygon to pNewPolygons. */
					nPolyIndex = pNewPolygons->nCount;		/* Get index of polygon
																			 * insertion below */
					if (PolySet_AddM(pNewPolygons, pPoly))
					{	/* Succesfully added polygon to PolySet.						
						
						/* Check angle of Normal vector with the split plane's
						 * normal vector. */
						if ((Normal.V[0] * pHPlane->BinPlane.Normal.V[0] +
							  Normal.V[1] * pHPlane->BinPlane.Normal.V[1] +
							  Normal.V[2] * pHPlane->BinPlane.Normal.V[2]) >= 0.f)
						{	/* Polygon's normal vector lies in the same direction
							 * as the split plane's normal vector,
							 * Add the polygon to the OutsideIndices. */
							if (!IndexSet_AddM(&(pHPlane->OutsideIndices), nPolyIndex))
							{	/* Failed to add the index of the new polygon due to
								 * a memory failure. Clean up and return NULL. */
#ifdef DEBUGC
								printf("HPlane_ConstructTree() -> MemFailure at point #2\n");
#endif			
								FloatSet_DestructM(&VertDistances);
								PolySet_DestructM(&OutSpacePolys);
								PolySet_DestructM(&InSpacePolys);
								Polygon_DestructM(&InPol);
								Polygon_DestructM(&OutPol);
								free(pHPlane);
								return NULL;
							}
						} else
						{	/* Polygon's normal vector lies in the opposite direction
							 * as the split plane's normal vector,
							 * Add the polygon to the InsideIndices. */
							if (!IndexSet_AddM(&(pHPlane->InsideIndices), nPolyIndex))
							{	/* Failed to add the index of the new polygon due to
								 * a memory failure. Clean up and return NULL. */
#ifdef DEBUGC
								printf("HPlane_ConstructTree() -> MemFailure at point #3\n");
#endif			
								FloatSet_DestructM(&VertDistances);
								PolySet_DestructM(&OutSpacePolys);
								PolySet_DestructM(&InSpacePolys);
								Polygon_DestructM(&InPol);
								Polygon_DestructM(&OutPol);
								free(pHPlane);
								return NULL;
							}
						}
					} else
					{	/* Failed to add the polygon to pNewPolygons due to
						 * lack of memory.
						 * Clean up and return NULL. */
#ifdef DEBUGC
						printf("HPlane_ConstructTree() -> MemFailure at point #4\n");
#endif			
						FloatSet_DestructM(&VertDistances);
						PolySet_DestructM(&OutSpacePolys);
						PolySet_DestructM(&InSpacePolys);
						Polygon_DestructM(&InPol);
						Polygon_DestructM(&OutPol);
						free(pHPlane);
						return NULL;
					}
				} else
				if ((!bPos) && (bNeg))
				{	/* Polygon lies on negative (IN) side. */
					/* Add it to the InSpacePolys. */
					if (!PolySet_AddM(&InSpacePolys, pPoly))
					{	/* Failed to add the polygon to pNewPolygons due to
						 * lack of memory.
						 * Clean up and return NULL. */
#ifdef DEBUGC
						printf("HPlane_ConstructTree() -> MemFailure at point #5\n");
#endif			
						FloatSet_DestructM(&VertDistances);
						PolySet_DestructM(&OutSpacePolys);
						PolySet_DestructM(&InSpacePolys);
						Polygon_DestructM(&InPol);
						Polygon_DestructM(&OutPol);
						free(pHPlane);
						return NULL;
					}
				} else
				if ((bPos) && (!bNeg))
				{	/* Polygon lies on positive (OUT) side. */
					/* Add it to the OutSpacePolys. */
					if (!PolySet_AddM(&OutSpacePolys, pPoly))
					{	/* Failed to add the polygon to pNewPolygons due to
						 * lack of memory.
						 * Clean up and return NULL. */
#ifdef DEBUGC
						printf("HPlane_ConstructTree() -> MemFailure at point #6\n");
#endif			
						FloatSet_DestructM(&VertDistances);
						PolySet_DestructM(&OutSpacePolys);
						PolySet_DestructM(&InSpacePolys);
						Polygon_DestructM(&InPol);
						Polygon_DestructM(&OutPol);
						free(pHPlane);
						return NULL;
					}
				} else
				{	/* Polygon spans the splitter plane.
					 * Build two seperate polygons and add them
					 * to both OutSpacePolys and InSpacePolys. */
					InPol.Vertices.nCount = 0;
					InPol.ulRGB = pPoly->ulRGB;
					InPol.pLightmap = pPoly->pLightmap;
					OutPol.Vertices.nCount = 0;
					OutPol.ulRGB = pPoly->ulRGB;
					OutPol.pLightmap = pPoly->pLightmap;
					if ((!HPlane_SplitPolygon(pHPlane, pPoly, &VertDistances, pVertices, &InPol, &OutPol)) ||
						 (!PolySet_AddM(&OutSpacePolys, &OutPol)) ||
						 (!PolySet_AddM(&InSpacePolys, &InPol)))
					{
						/* A memory failure occured in any of the above three operations. */
						/* Clean up & return NULL. */
#ifdef DEBUGC
						printf("HPlane_ConstructTree() -> MemFailure at point #7\n");
#endif			
						FloatSet_DestructM(&VertDistances);
						PolySet_DestructM(&OutSpacePolys);
						PolySet_DestructM(&InSpacePolys);
						Polygon_DestructM(&InPol);
						Polygon_DestructM(&OutPol);
						free(pHPlane);
						return NULL;
					}
				}
			} /* End of polygon iteration for classification. */
		} else	/* MemFailure check for pHPlane allocation. */
		{	/* Failed to allocate pHPlane. */
			/* Clean up & return NULL. */
#ifdef DEBUGC
			printf("HPlane_ConstructTree() -> MemFailure at point #8\n");
#endif			
			FloatSet_DestructM(&VertDistances);
			PolySet_DestructM(&OutSpacePolys);
			PolySet_DestructM(&InSpacePolys);
			Polygon_DestructM(&InPol);
			Polygon_DestructM(&OutPol);
			return NULL;
		}
		
		/* If we made it this far, our status should be something like this :
		 * OutSpacePolys, contains the polygons that should be inserted in
		 *                the out-subspace.
		 * InSpacePolys, contains the polygons that should be inserted in
		 *               the in-subspace.
		 * VertDistances, should be trashed, has memory locked.
		 * InPol, should be trashed, has memory locked.
		 * OutPol, should be trashed, has memory locked.
		 * pHPlane, contains the current Hyperplane, with some linked indices
		 *          for the coplanar planes.
		 */
		/* First we'll free some memory. */
		FloatSet_DestructM(&VertDistances);
		Polygon_DestructM(&InPol);
		Polygon_DestructM(&OutPol);
		
		/* Now we can go into recursion. */

		/* Call for in-plane. */
		pHPlane->pInSubtree = HPlane_ConstructTree(&InSpacePolys, pVertices, pNewPolygons);

		/* Now we can free the InSpacePolys because they're not needed anymore. */
		PolySet_DestructM(&InSpacePolys);

		/* Call for out-plane. */
		pHPlane->pOutSubtree = HPlane_ConstructTree(&OutSpacePolys, pVertices, pNewPolygons);
		
		/* We're almost done. Free remaining memory. */
		PolySet_DestructM(&OutSpacePolys);
		
		/* And return the HPlane we so painfully created. */
		return pHPlane;

	} /* Void subspace check. */
}

/********************************************************************
* Function : HPlane_Destruct()
* Purpose : Frees all memory associated with a SINGLE HPlane
*           structure. This DOES -=>NOT<=- free pThis or any of it's
*           children from memory.
********************************************************************/
void HPlane_Destruct(struct HPlane *pThis)
{	IndexSet_DestructM(&(pThis->InsideIndices));
	IndexSet_DestructM(&(pThis->OutsideIndices));
}
	
/********************************************************************
* Function : HPlane_DestroyTree()
* Purpose : Frees a whole BSP tree consisting of HPlane structures
*           from memory.
* Pre : pThis points to the first (root) HPlane node of the BSP tree.
* Post : pThis and all children of pThis have been freed from memory.
********************************************************************/
void HPlane_DestroyTree(struct HPlane *pThis)
{	if (pThis != NULL)
	{	HPlane_DestroyTree(pThis->pInSubtree);
		HPlane_DestroyTree(pThis->pOutSubtree);
		HPlane_Destruct(pThis);
		free((void *)pThis);
	}
}

/********************************************************************
* Function : HPlane_SplitPolygon()
* Purpose : Splits a polygon that spans a plane in two smaller
*           polygons.
* Pre : pThis points to an initialized HPlane structure,
*       pPolygon points to an initialized Polygon structure that
*       spans the plane pThis,
*       pDistances points to an initialized FloatSet structure that
*       contains distances to the plane for all vertices, accessable
*       by the same index as in pVertices.
*       pVertices points to an initialized VertexSet structure that
*       contains all vertices used by pPolygon.
*       pInsidePol points to an initialized polygon structure with
*       0 vertices.
*       pOutsidePol points to an initialized polygon structure with
*       0 vertices.
* Post : pInsidePol contains a polygon for the splitted part opposite
*        to pHPlane's normal.
*        pOutsidePol contains a polygon for the splitted part in the
*        direction of pHPlane's normal.
*        pVertices has new vertices added for edges that intersected
*        the plane.
* Note : Vertex interpolation is done in this function, but should
*        really be done in the Vertex structure file.
********************************************************************/
int HPlane_SplitPolygon(struct HPlane *pThis, struct Polygon *pPolygon,
								struct FloatSet *pDistances,
								struct VertexSet *pVertices,
								struct Polygon *pInSidePol,
								struct Polygon *pOutSidePol)
{
	int n;
	int nVIndex;		/* Vertex index. */
	int nLastVIndex;	/* Last Vertex index. */
	float VDistance;	/* Vertex Distance. */
	float LastVDistance;	/* Last Vertex Distance. */
	struct Vertex IVert;	/* Vertex for interpolation result. */
	struct Vertex *pV0, *pV1;	/* Vertex pointers for interpolation. */
	float fInterpol;		/* Interpolation multiplier. (Temp. var.) */
	int nNVIndex;		/* New Vertex Index, index of interpolated vertices. */
	
	/* Copy polygon properties. */
	pInSidePol->pLightmap = pPolygon->pLightmap;
	pInSidePol->nFlags = pPolygon->nFlags;
	pInSidePol->ulRGB = pPolygon->ulRGB;
	pOutSidePol->pLightmap = pPolygon->pLightmap;
	pOutSidePol->nFlags = pPolygon->nFlags;
	pOutSidePol->ulRGB = pPolygon->ulRGB;

	/* Get the polygon's last vertex index. */
	n = pPolygon->Vertices.nCount - 1;
	nLastVIndex = IndexSet_GetIndexM(&(pPolygon->Vertices), n);
	/* Get the last vertex index's distance. */
	LastVDistance = FloatSet_GetFloatM(pDistances, nLastVIndex);
	
	for (n = 0; n < pPolygon->Vertices.nCount; n++)
	{
		/* Get the index for this vertex. */
		nVIndex = IndexSet_GetIndexM(&(pPolygon->Vertices), n);
		/* Get the distance for this vertex. */
		VDistance = FloatSet_GetFloatM(pDistances, nVIndex);
		
		/* Classify this edge, 4 possibilities :
		 * ----------+---------------+---------------------------------
		 * Sign of   | Sign of       | Action
		 * VDistance | LastVDistance | 
		 * ----------+---------------+---------------------------------
		 * Positive  | Positive      | Add vertex to pOutSidePol
		 * ----------+---------------+---------------------------------
		 * Positive  | Negative      | Compute intersection vertex,
		 *           |               | add intersection vertex to both
		 *           |               | pInSidePol and pOutSidePol.
		 *           |               | Add Vertex to pOutSidePol.
		 * ----------+---------------+---------------------------------
		 * Negative  | Positive      | Compute intersection vertex,
		 *           |               | add intersection vertex to both
		 *           |               | pInSidePol and pOutSidePol.
		 *           |               | Add Vertex to pInSidePol.
		 * ----------+---------------+---------------------------------
		 * Negative  | Negative      | Add vertex to pInSidePol.
		 * ----------+---------------+---------------------------------
		 */
		if (((VDistance < 0.f) && (LastVDistance >= 0.f)) ||
			 ((VDistance >= 0.f) && (LastVDistance < 0.f)))
		{	/* Last vertex is negative. */
			/* Compute intersection vertex,
			 * add intersection vertex to both InSidePol and
			 * OutSidePol. */
			/* Add vertex to OutSidePol. */
			
			/* Get the two vertices. */
			pV0 = VertexSet_GetVertexM(pVertices, nLastVIndex);
			pV1 = VertexSet_GetVertexM(pVertices, nVIndex);
			
			/* Interpolate the two vertices. */
			if (VDistance != LastVDistance)	/* Avoid division by zero. */
				fInterpol = LastVDistance / (LastVDistance - VDistance);
			else
				fInterpol = 0.f;
			
			Vertex_InterpolateM(pV0, pV1, fInterpol, &IVert);

			/* Add the interpolated vertex to the vertexset. */
			VertexSet_AddM(pVertices, &IVert);
			
			/* Get the index of the new vertex (which is the last index). */
			nNVIndex = pVertices->nCount - 1;
			
			/* Add vertex to both pInSidePol and pOutSidePol. */
			if (!IndexSet_AddM(&(pOutSidePol->Vertices), nNVIndex))
				return 0;		/* Mem Failure. */
			if (!IndexSet_AddM(&(pInSidePol->Vertices), nNVIndex))
				return 0;		/* Mem Failure. */
		}
		if (VDistance < 0.f)
		{	if (!IndexSet_AddM(&(pInSidePol->Vertices), nVIndex))
				return 0;		/* Mem Failure. */
		} else
		{	if (!IndexSet_AddM(&(pOutSidePol->Vertices), nVIndex))
				return 0;		/* Mem Failure. */
		}
		LastVDistance = VDistance;
		nLastVIndex = nVIndex;
	}
	return 1;
}

/********************************************************************
* Function : HPlane_CalculateLeafCount()
* Purpose : Counts the number of NULL pointers in the pInSubtree
*           and pOutSubtree fields for a given HPlane tree. During
*           this process all nInsideLeafCount fields are also
*           updated.
* Pre : pThis points to an initialized HPlane structure which forms
*       the root node of the tree to count on.
* Post : All fields of the tree pointed to by pThis have their
*        nInsideLeafCount fields set correctly and the returnvalue
*        represents the total number of NULL pointers (leafs) in the
*        pInSubtree and pOutSubtree fields for the whole tree.
********************************************************************/
int HPlane_CalculateLeafCount(struct HPlane *pThis)
{
	if (pThis == NULL)
	{
		/* Only 1 NULL pointer, pThis is it. */
		return 1;
	} else
	{
		/* Update inside leaf count. */
		pThis->nInsideLeafCount = HPlane_CalculateLeafCount(pThis->pInSubtree);

		/* Return total leaf count. */
		return pThis->nInsideLeafCount + HPlane_CalculateLeafCount(pThis->pOutSubtree);
	}
}

/********************************************************************
* Function : HPlane_GetVectorSubspaceIndex()
* Purpose : Retrieves the index of the subspace from BSP Tree pThis
*           in which pVector is located.
* Pre : pThis points to an initialized HPlane node, pVector points
*       to an initialized Vector structure. The tree pointed to by
*       pThis MUST be processed by HPlane_CalculateLeafCount()
*       because this function depends heavily on the nInsideLeafCount
*       field in HPlane.
* Post : Returnvalue represents the leaf number of the subspace in
*        which pVector is located.
********************************************************************/
int HPlane_GetVectorSubspaceIndex(struct HPlane *pThis,
											 struct Vector *pVector)
{
	if (pThis == NULL)
	{	return 0;		/* Index at 0 (we're in the leaf). */
	} else
	{	/* Classify the side at which pVector is. */
		if (0.f > Plane_DistanceOfVectorM(&(pThis->BinPlane), pVector))
		{
			/* pVector is on side opposite of the plane's normal (IN side). */
			return HPlane_GetVectorSubspaceIndex(pThis->pInSubtree, pVector);
		} else
		{
			/* pVector is on side of the plane's normal (OUT side). */
			return pThis->nInsideLeafCount +
					 HPlane_GetVectorSubspaceIndex(pThis->pOutSubtree, pVector);
		}
	}
}

#ifdef DEBUGC
/********************************************************************
* Function : HPlane_DebugDump()
* Purpose : Writes the HPlane tree in ASCII to stdout.
* Pre : pThis points to the first root node of the tree to display
*       at indentation nIndent.
* Post : pThis, and all it's children, have been printed in a tree
*        shape.
********************************************************************/
void HPlane_DebugDump(struct HPlane *pThis, int nIndent)
{
	int n;
	/* Check if pThis is NULL. */
	if (pThis == NULL)
	{	/* Print indentation. */
		for (n = 0; n < nIndent; n++)
			putchar(' ');
		printf("[N]\n");
	} else
	{
		/* First do RIGHT subtree (the OUT subtree) */
		HPlane_DebugDump(pThis->pOutSubtree, nIndent + 3);
		
		/* Now print this node. */
		for (n = 0; n < nIndent; n++)
			putchar(' ');
		printf("[(%f,%f,%f),%f]\n", pThis->BinPlane.Normal.V[0],
											 pThis->BinPlane.Normal.V[1],
											 pThis->BinPlane.Normal.V[2],
											 pThis->BinPlane.Distance);
		/* Now do LEFT subtree (the IN subtree) */
		HPlane_DebugDump(pThis->pInSubtree, nIndent + 3);
	}
}
#endif
