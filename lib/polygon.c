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
* FILE : polygon.c
********************************************************************/

#define POLYGON_C

#include <stdlib.h>
#include <math.h>

#include "polygon.h"

#ifdef DEBUGC
#include <stdio.h>
#endif

/********************************************************************
* Function : Polygon_Destruct(pThis)
* Purpose : Frees all memory associated with a polygon structure.
* Pre : pThis points to an initialized polygon structure.
* Post : pThis points to a polygon structure that doesn't use any
*        memory and is invalid.
********************************************************************/
void Polygon_Destruct(struct Polygon *pThis)
{
	/* Just call the macro version. */
	Polygon_DestructM(pThis);
}

/********************************************************************
* Function : Polygon_ExtractNormal(pThis, pVertices, pNormal)
* Purpose : For determining the normal of a polygon, given the
*           polygon's vertices.
* Pre : pThis points to an initialized polygon structure, pVertices
*       points to the polygons belonging to that structure. pNormal
*       points to a vector structure.
* Post : pNormal contains the normal vector for the polygon pThis.
*        The normal points toward the viewer if the vertices of the
*        polygon are in counterclockwise order.
********************************************************************/
void Polygon_ExtractNormal(struct Polygon *pThis,
									struct VertexSet *pVertices,
									struct Vector *pNormal)
{
	/* We'll use Newell's method for getting a plane equation for
	 * a polygon to get the normal vector. 
	 * A detailed description of how this all works can be found in
	 * Graphics Gems III. */
	int i;
	float len;
	struct Vertex *pVertex0, *pVertex1;
	int n1,n2,nc;

#ifdef DEBUGC
	fprintf(stderr, "Polygon_ExtractNormal(%lx, %lx, %lx)\n", pThis, pVertices, pNormal);
#endif
	
	/* Clear the normal vector. */
	Vector_ConstructM(pNormal);

#ifdef DEBUGC
	fprintf(stderr, "\tIndexSet_GetCountM = %d\n", IndexSet_GetCountM(&(pThis->Vertices)));
#endif
	for (i = 0; i < IndexSet_GetCountM(&(pThis->Vertices)); i++)
	{
		n1 = IndexSet_GetIndexM(&(pThis->Vertices), i);
		nc = (i + 1) % IndexSet_GetCountM(&(pThis->Vertices));
		n2 = IndexSet_GetIndexM(&(pThis->Vertices), nc);

		pVertex0 = VertexSet_GetVertexM(pVertices, n1);
		pVertex1 = VertexSet_GetVertexM(pVertices, n2);

		pNormal->V[0] += (pVertex0->Position.V[1] - pVertex1->Position.V[1]) *
							 (pVertex0->Position.V[2] + pVertex1->Position.V[2]);
		pNormal->V[1] += (pVertex0->Position.V[2] - pVertex1->Position.V[2]) *
							 (pVertex0->Position.V[0] + pVertex1->Position.V[0]);
		pNormal->V[2] += (pVertex0->Position.V[0] - pVertex1->Position.V[0]) *
							 (pVertex0->Position.V[1] + pVertex1->Position.V[1]);
#ifdef DEBUGC
		printf("\tIteration #%d,\n", i);
		printf("\tpNormal(%f,%f,%f)\n", pNormal->V[0], pNormal->V[1], pNormal->V[2]);
		printf("\tpVertex0->Position(%f,%f,%f)\n", pVertex0->Position.V[0],
																 pVertex0->Position.V[1],
																 pVertex0->Position.V[2]);
		printf("\tpVertex1->Position(%f,%f,%f)\n", pVertex1->Position.V[0],
																 pVertex1->Position.V[1],
																 pVertex1->Position.V[2]);
#endif
	}
	len = (float)sqrt(pNormal->V[0] * pNormal->V[0] +
							pNormal->V[1] * pNormal->V[1] +
							pNormal->V[2] * pNormal->V[2]);
	if (len != 0.f)
	{
		pNormal->V[0] /= len;
		pNormal->V[1] /= len;
		pNormal->V[2] /= len;
	}
}
	
/********************************************************************
* Function : Polygon_ExtractPlane
* Purpose : Builds a plane equation from a given polygon. If the
*           polygon's vertices are not exactly in the same plane, a
*           best-fit plane is produced.
* Pre : pThis points to an initialized Polygon structure, pVertices
*       points to an initialized VertexSet containing the vertices
*       for pThis. pPlane points to a Plane structure.
* Post : pPlane contains the plane equation for the polygon.
********************************************************************/
void Polygon_ExtractPlane(struct Polygon *pThis,
								  struct VertexSet *pVertices,
								  struct Plane *pPlane)
{
	struct Vector CenterVector;
	struct Vertex *pVertex;
	int n, m;
	
	/* First extract the polygon's normal. */
	Polygon_ExtractNormal(pThis, pVertices, &(pPlane->Normal));
	
	/* Now compute an average center vertex.
	 * This is defined to be on the best-fit plane. */
	Vector_ConstructM(&CenterVector);
	for (n = 0; n < pThis->Vertices.nCount; n++)
	{
		/* Get the vertex index we need for vertex n of the polygon. */
		m = IndexSet_GetIndexM(&(pThis->Vertices), n);
		/* Get the vertex. */
		pVertex = VertexSet_GetVertexM(pVertices, m);
		
		/* Sum the vertex's position. */
		CenterVector.V[0] += pVertex->Position.V[0];
		CenterVector.V[1] += pVertex->Position.V[1];
		CenterVector.V[2] += pVertex->Position.V[2];
	}
	
	/* Divide the sum by the count to produce the centervector. */
	if (pThis->Vertices.nCount != 0)		/* Avoid division by zero. */
	{
		CenterVector.V[0] /= pThis->Vertices.nCount;
		CenterVector.V[1] /= pThis->Vertices.nCount;
		CenterVector.V[2] /= pThis->Vertices.nCount;
	}
	
	/* We can now produce the distance from the origin, which is the
	 * inproduct of the CenterVector and the Normal Vector. */
	pPlane->Distance = CenterVector.V[0] * pPlane->Normal.V[0] +
							 CenterVector.V[1] * pPlane->Normal.V[1] +
							 CenterVector.V[2] * pPlane->Normal.V[2];
#ifdef DEBUGC
	printf("\tComputed plane : [(%f,%f,%f),%f]\n", pPlane->Normal.V[0],
																  pPlane->Normal.V[1],
																  pPlane->Normal.V[2],
																  pPlane->Distance);
#endif
}
		
/********************************************************************
* Function : Polygon_LinkToColorManager()
* Purpose : Links a Polygon to a ColorManager, which allows the
*           Polygon to be drawn because it then knows what indices
*           should be used to render it's colors.
* Pre : pThis points to an initialized Polygon structure.
*       pColorManager points to an initialized ColorManager
*       structure.
* Post : If the returnvalue is 0, a memory failure occured, otherwise
*        the specified Polygon is now linked to the ColorManager.
********************************************************************/
int Polygon_LinkToColorManager(struct Polygon *pThis,
				 struct ColorManager *pColorManager)
{
	struct Lightmap1 *pLmap1;
	struct Lightmap256 *pLmap256;

	/* The type of colors required is dependant on the type
	 * of shading that is to be applied to the polygon. */
	switch (pThis->nFlags)
	{	case PF_STATICCOLOR :
		{	/* Static color, color of polygon is always the same,
			 * only one color required. Use a Lightmap1 structure. */
			pLmap1 = ColorManager_GetLightmap1(pColorManager, pThis->ulRGB);
			if (pLmap1 == NULL)
				return 0;		/* Memory failure. */
			pThis->pLightmap = (void *)pLmap1;
		}break;

		case PF_DYNACOLOR :
		{	/* Dynamic color, color of polygon depends on lighting
			 * conditions. Use a Lightmap256 structure.
			 * For now default to black for 0 intensity. */
			pLmap256 = ColorManager_GetLightmap256(pColorManager, 0x000000, pThis->ulRGB);
			if (pLmap256 == NULL)
				return 0;		/* Memory failure. */
			pThis->pLightmap = (void *)pLmap256;
		}break;
	}
	return 1;
}
