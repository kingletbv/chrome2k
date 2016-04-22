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
* FILE : pmodel.c
********************************************************************/

#define PMODEL_C

/* Mathematical pi constant. */
#define PM_PI 3.141592654f

#include <stdlib.h>
#include <math.h>			/* sqrt(), sin(), cos() etc. */

#include "pmodel.h"
#include "vertex.h"
#include "polygon.h"
#include "polyset.h"
#include "indexset.h"
#include "vertxset.h"

/********************************************************************
* Function : Model_NewBox()
* Purpose : Generates a box.
* Pre : fXRadius, fYRadius and fZRadius specify resp. distances of
*       faces perpendicular to the X, Y and Z axes.
* Post : If the returnvalue != NULL, it is a pointer to the new
*        model. The color of all the faces in the model is 1.
*        If the returnvalue == NULL, a memory allocation failure
*        occured.
* Note : This routine has become a bit messy due to error detection,
*        I DID however avoid a goto (but it's probably not worth it).
********************************************************************/
struct Model *Model_NewBox(float fXRadius, float fYRadius, float fZRadius)
{
	struct Model *pModel;
	struct Vertex vtx;
	struct Polygon pol;
	struct PolySet pset;

	Vertex_ConstructM(&vtx);
	Polygon_ConstructM(&pol);
	PolySet_ConstructM(&pset);

	/* Allocate a new model structure. */
  	pModel = (struct Model *)malloc(sizeof(struct Model));

	/* Check for memory failure. */
	if (pModel != NULL)
	{
		/* Construct the model structure. */
		Model_ConstructM(pModel);

		/* Build Vertices. */
		vtx.Position.V[0] = -fXRadius;  vtx.Position.V[1] = -fYRadius;  vtx.Position.V[2] = -fZRadius;
		if (VertexSet_AddM(&(pModel->Vertices), &vtx))
		{	vtx.Position.V[0] = -fXRadius;  vtx.Position.V[1] = -fYRadius;  vtx.Position.V[2] = fZRadius;
			if (VertexSet_AddM(&(pModel->Vertices), &vtx))
			{	vtx.Position.V[0] = -fXRadius;  vtx.Position.V[1] = fYRadius;  vtx.Position.V[2] = -fZRadius;
				if (VertexSet_AddM(&(pModel->Vertices), &vtx))
				{	vtx.Position.V[0] = -fXRadius;  vtx.Position.V[1] = fYRadius;  vtx.Position.V[2] = fZRadius;
					if (VertexSet_AddM(&(pModel->Vertices), &vtx))
					{	vtx.Position.V[0] = fXRadius;  vtx.Position.V[1] = -fYRadius;  vtx.Position.V[2] = -fZRadius;
						if (VertexSet_AddM(&(pModel->Vertices), &vtx))
						{	vtx.Position.V[0] = fXRadius;  vtx.Position.V[1] = -fYRadius;  vtx.Position.V[2] = fZRadius;
							if (VertexSet_AddM(&(pModel->Vertices), &vtx))
							{	vtx.Position.V[0] = fXRadius;  vtx.Position.V[1] = fYRadius;  vtx.Position.V[2] = -fZRadius;
								if (VertexSet_AddM(&(pModel->Vertices), &vtx))
								{	vtx.Position.V[0] = fXRadius;  vtx.Position.V[1] = fYRadius;  vtx.Position.V[2] = fZRadius;
									if (VertexSet_AddM(&(pModel->Vertices), &vtx))
									{
										/* Build polygons. */
										/* Front polygon. */
										pol.Vertices.nCount = 0;
										pol.ulRGB = 0xFFFFFF;
										if (IndexSet_AddM(&(pol.Vertices), 0))
										{	if (IndexSet_AddM(&(pol.Vertices), 2))
											{	if (IndexSet_AddM(&(pol.Vertices), 6))
												{	if (IndexSet_AddM(&(pol.Vertices), 4))
													{	if (PolySet_AddM(&pset, &pol))
														{	/* Left Side Polygon. */
															pol.Vertices.nCount = 0;
															pol.ulRGB = 0xFFFFFF;
															if (IndexSet_AddM(&(pol.Vertices), 0))
															{	if (IndexSet_AddM(&(pol.Vertices), 1))
																{	if (IndexSet_AddM(&(pol.Vertices), 3))
																	{	if (IndexSet_AddM(&(pol.Vertices), 2))
																		{	if (PolySet_AddM(&pset, &pol))
																			{	/* Right Side Polygon. */
																				pol.Vertices.nCount = 0;
																				pol.ulRGB = 0xFFFFFF;
																				if (IndexSet_AddM(&(pol.Vertices), 4))
																				{	if (IndexSet_AddM(&(pol.Vertices), 6))
																					{	if (IndexSet_AddM(&(pol.Vertices), 7))
																						{	if (IndexSet_AddM(&(pol.Vertices), 5))
																							{	if (PolySet_AddM(&pset, &pol))
																								{	/* Bottom Side Polygon. */
																									pol.Vertices.nCount = 0;
																									pol.ulRGB = 0xFFFFFF;
																									if (IndexSet_AddM(&(pol.Vertices), 2))
																									{	if (IndexSet_AddM(&(pol.Vertices), 3))
																										{	if (IndexSet_AddM(&(pol.Vertices), 7))
																											{	if (IndexSet_AddM(&(pol.Vertices), 6))
																												{	if (PolySet_AddM(&pset, &pol))
																													{	/* Top Side Polygon. */
																														pol.Vertices.nCount = 0;
																														pol.ulRGB = 0xFFFFFF;
																														if (IndexSet_AddM(&(pol.Vertices), 0))
																														{	if (IndexSet_AddM(&(pol.Vertices), 4))
																															{	if (IndexSet_AddM(&(pol.Vertices), 5))
																																{	if (IndexSet_AddM(&(pol.Vertices), 1))
																																	{	if (PolySet_AddM(&pset, &pol))
																																		{	/* Back Side Polygon. */
																																			pol.Vertices.nCount = 0;
																																			pol.ulRGB = 0xFFFFFF;
																																			if (IndexSet_AddM(&(pol.Vertices), 1))
																																			{	if (IndexSet_AddM(&(pol.Vertices), 5))
																																				{	if (IndexSet_AddM(&(pol.Vertices), 7))
																																					{	if (IndexSet_AddM(&(pol.Vertices), 3))
																																						{	if (PolySet_AddM(&pset, &pol))
																																							{
																																								/* Now build a BSP Tree. */
																																								pModel->pRoot = HPlane_ConstructTree(&pset, &(pModel->Vertices), &(pModel->Polygons));

																																								if (pModel->pRoot != NULL)
																																								{
																																									/* Initialize Leaf Count.
																																									 * This is not really neccessary as this is also done when a Model
																																									 * is linked to an Actor, however we do it just to be on the safe
																																									 * side. */
																																									HPlane_CalculateLeafCount(pModel->pRoot);
																																									
																																									/* And build a bounding sphere
																																									 * We prevent using the standard routine for this because it tends
																																									 * to perform badly on cube's and boxes.
																																									 * As a centerpoint we use (0,0,0) and as radius we use the
																																									 * distance of any vertex. */
																																									pModel->Centerpoint.V[0] = 0.f;
																																									pModel->Centerpoint.V[1] = 0.f;
																																									pModel->Centerpoint.V[2] = 0.f;
																																									pModel->fRadius = (float)sqrt(fXRadius * fXRadius +
																																																			fYRadius * fYRadius +
																																																			fZRadius * fZRadius);
																																									
																																									Polygon_DestructM(&pol);
																																									PolySet_DestructM(&pset);
																																									/* Finished construction. */
																																									return pModel;
																																								}
																																							}
																																						}
																																					}
																																				}
																																			}
																																		}
																																	}
																																}
																															}
																														}
																													}
																												}
																											}
																										}
																									}
																								}
																							}
																						}
																					}
																				}
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		Polygon_DestructM(&pol);
		PolySet_DestructM(&pset);
		free(pModel);
	}
}

/********************************************************************
* Function : Model_NewCube()
* Purpose : Generates a cube.
* Pre : fRadius specifies the size of the cube, the diameter of the
*       cube is fRadius * 2.
* Post : If the returnvalue != NULL, it is a pointer to the new
*        model.
*        If the returnvalue == NULL, a memory allocation failure
*        occured.
* Note : This function just calls Model_NewBox().
********************************************************************/
struct Model *Model_NewCube(float fRadius)
{
	return Model_NewBox(fRadius, fRadius, fRadius);
}

/********************************************************************
* Function : Model_NewSphere()
* Purpose : Generates a sphere.
* Pre : The main axis is the Y axis, on that axis, nSegmentCount
*		  separate segments will be generated. This defines a
*		  'half moon' curve which will be swept using nSweepCount
*		  steps. fRadius specifies the radius of the sphere.
* Post : If the returnvalue != NULL, it is a pointer to the new 
*        model.
*        If the returnvalue == NULL, a memory allocation failure
*        occured.
********************************************************************/
struct Model *Model_NewSphere(float fRadius, int nSweepCount, int nSegmentCount)
{
	struct VertexSet vset;		/* Used for defining the Sweep. */
	struct Vertex vert;
	struct Model *pModel;
	int n;

	VertexSet_ConstructM(&vset);
	Vertex_ConstructM(&vert);
	/* Define the sweep curve. */
	for (n = 1; n < nSegmentCount; n++)
	{
		vert.Normal.V[0] = (float)sin((n * PM_PI) / (float)nSegmentCount);
		vert.Normal.V[1] = -(float)cos((n * PM_PI) / (float)nSegmentCount);
		vert.Normal.V[2] = 0.f;
		vert.Position.V[0] = fRadius * vert.Normal.V[0];
		vert.Position.V[1] = fRadius * vert.Normal.V[1];
		vert.Position.V[2] = 0.f;

		if (!VertexSet_AddM(&vset, &vert))
		{	VertexSet_DestructM(&vset);
			return NULL;	/* Mem failure. */
		}
	}
	
	/* Construct the Sweep. */
	/* The following function returns NULL upon failure,
	 * this doesn't change anything for this function, hence
	 * no checking. */
	pModel = Model_NewSweep(&vset, nSweepCount);

	VertexSet_DestructM(&vset);

	return pModel;
}

/********************************************************************
* Function : Model_NewCylinder()
* Purpose : Generates a cylinder.
* Pre : fRadius defines the radius of the cylinder's hull, fLength
*       defines the length of the cylinder's hull, nSweepCount
*       defines the number of sweep segments that defines the hull.
* Post : If the returnvalue != NULL, it is a pointer to the new
*        cylinder model.
*        If the returnvalue == NULL, a memory allocation failure
*        occured.
********************************************************************/
struct Model *Model_NewCylinder(float fRadius, float fLength, int nSweepCount)
{
	struct VertexSet vset;		/* Used for defining sweep. */
	struct Vertex vert;
	struct Model *pModel;

	VertexSet_ConstructM(&vset);
	Vertex_ConstructM(&vert);

	vert.Position.V[0] = fRadius;
	vert.Position.V[1] = -fLength / 2.f;
	vert.Position.V[2] = 0.f;
	vert.Normal.V[0] = 1.f;
	vert.Normal.V[1] = 0.f;
	vert.Normal.V[2] = 0.f;
	if (VertexSet_AddM(&vset, &vert))
	{	vert.Position.V[0] = fRadius;
		vert.Position.V[1] = fLength / 2.f;
		vert.Position.V[2] = 0.f;
		vert.Normal.V[0] = 1.f;
		vert.Normal.V[1] = 0.f;
		vert.Normal.V[2] = 0.f;
		if (VertexSet_AddM(&vset, &vert))
		{
			/* Construct the model. */
			pModel = Model_NewSweep(&vset, nSweepCount);

			VertexSet_DestructM(&vset);
			return pModel;
		}
	}
	return NULL;
}

/********************************************************************
* Function : Model_NewSweep()
* Purpose : Generates a sweep.
* Pre : pVertices points to a VertexSet structure containing the 
*       2D(!) vertices which must be sweeped. nSweepCount defines
*       the number of sweeps that makes up the total circular sweep.
* Post : If the returnvalue != NULL, it is a pointer to the new Sweep
*        Model.
*        If the returnvalue == NULL, a memory allocation failure
*        occured.
********************************************************************/
struct Model *Model_NewSweep(struct VertexSet *pVertices, int nSweepCount)
{
	struct VertexSet vset;
	struct Polygon pol;
	struct PolySet pset;
	struct Vertex *pV;
	struct Vertex Vert;
	struct Model *pModel;
	float fLen;
	int k, n, nLastSweep, nLastVertex;

	VertexSet_ConstructM(&vset);
	Polygon_ConstructM(&pol);
	PolySet_ConstructM(&pset);
	Vertex_ConstructM(&Vert);

	/* Allocate a new model structure. */
	pModel = (struct Model *)malloc(sizeof(struct Model));

	if (pModel != NULL)
	{
		/* Construct the Model structure. */
		Model_ConstructM(pModel);

		/* Fill the VertexSet with the swept vertices. */
		for (k = 0; k < nSweepCount; k++)
		{	for (n = 0; n < VertexSet_GetCountM(pVertices); n++)
			{
				/* Get the vertex. */
				pV = VertexSet_GetVertexM(pVertices, n);

				/* Rotate the vertex, use k for rotation.
				 * Only rotate in Y plane (Y components stay the same). */
				Vert.Position.V[0] = pV->Position.V[0] * (float)cos((n * PM_PI * 2.f) / (float)nSweepCount);
				Vert.Position.V[1] = pV->Position.V[1];
				Vert.Position.V[2] = pV->Position.V[0] * (float)sin((n * PM_PI * 2.f) / (float)nSweepCount);
				/* Don't forget the Vertex's Normal vector, it may be vital
				 * for shading. */
				Vert.Normal.V[0] = pV->Normal.V[0] * (float)cos((n * PM_PI * 2.f) / (float)nSweepCount);
				Vert.Normal.V[1] = pV->Normal.V[1];
				Vert.Normal.V[2] = pV->Normal.V[2] * (float)sin((n * PM_PI * 2.f) / (float)nSweepCount);
				/* Normalize the Normal vector. */
				fLen = (float)sqrt(pV->Normal.V[0] * pV->Normal.V[0] +
										 pV->Normal.V[1] * pV->Normal.V[1] +
										 pV->Normal.V[2] * pV->Normal.V[2]);
				if (fLen == 0.f)
				{	/* There was no valid normal vector so we have to
					 * create one.
					 * This shouldn't happen, but just in case it did,
					 * we'll write a new (dummy) one which is correct
					 * for a cylinder only. */
					/* This always has length 1. */
					Vert.Normal.V[0] = (float)cos((n * PM_PI * 2.f) / (float)nSweepCount);
					Vert.Normal.V[1] = 0.f;
					Vert.Normal.V[2] = (float)sin((n * PM_PI * 2.f) / (float)nSweepCount);
				}

				/* Add the new vertex to the vertexset. */
				if (!VertexSet_AddM(&vset, &Vert))
				{	/* A memory allocation failure occured. */
					VertexSet_DestructM(&vset);
					Polygon_DestructM(&pol);
					PolySet_DestructM(&pset);
					free(pModel);
					return NULL;
				}
			}
		}

		/* Generate the polygons, they must have their visible side outwards,
		 * which means they have to be counterclockwise. */
		nLastSweep = nSweepCount - 1;
		for (k = 0; k < nSweepCount; k++)
		{
			nLastVertex = 0;
			for (n = 1; n < VertexSet_GetCountM(pVertices); n++)
			{	
				pol.ulRGB = 0xFFFFFF;
				pol.Vertices.nCount = 0;
				IndexSet_Add(&(pol.Vertices), nLastSweep * VertexSet_GetCountM(pVertices) + nLastVertex);
				IndexSet_Add(&(pol.Vertices), nLastSweep * VertexSet_GetCountM(pVertices) + n);
				IndexSet_Add(&(pol.Vertices), k * VertexSet_GetCountM(pVertices) + n);
				IndexSet_Add(&(pol.Vertices), k * VertexSet_GetCountM(pVertices) + nLastVertex);
				if (!PolySet_AddM(&pset, &pol))
				{	/* A memory allocation failure occured. */
					VertexSet_DestructM(&vset);
					Polygon_DestructM(&pol);
					PolySet_DestructM(&pset);
					free(pModel);
					return NULL;
				}			
				nLastVertex = n;
			}
			nLastSweep = k;
		}

		/* Finally, we need to add the top and the bottom shieldings. */
		/* First the top. */
		pol.ulRGB = 0xFFFFFF;
		pol.Vertices.nCount = 0;
		for (k = 0; k < nSweepCount; k++)
		{	
			IndexSet_Add(&(pol.Vertices), k * VertexSet_GetCountM(pVertices));
		}
		if (!PolySet_AddM(&pset, &pol))
		{	/* A memory allocation failure occured. */
			VertexSet_DestructM(&vset);
			Polygon_DestructM(&pol);
			PolySet_DestructM(&pset);
			free(pModel);
			return NULL;
		}			
		/* Then the bottom (notice the counterclockwise loop). */
		pol.ulRGB = 0xFFFFFF;
		pol.Vertices.nCount = 0;
		for (k = nSweepCount - 1; k >= 0; k--)
		{	IndexSet_Add(&(pol.Vertices), k * VertexSet_GetCountM(pVertices) + VertexSet_GetCountM(pVertices) - 1);
		}
		if (!PolySet_AddM(&pset, &pol))
		{	/* A memory allocation failure occured. */
			VertexSet_DestructM(&vset);
			Polygon_DestructM(&pol);
			PolySet_DestructM(&pset);
			free(pModel);
			return NULL;
		}

		/* Construct the BSP Tree. */
		pModel->pRoot = HPlane_ConstructTree(&pset, &(pModel->Vertices), &(pModel->Polygons));

		/* Initialize Leaf Count. */
		HPlane_CalculateLeafCount(pModel->pRoot);

		/* Initialize Bounding Sphere.
		 * Use the approximation technique, this delivers a 'reasonable'
		 * bounding sphere. */
		Model_CalcBoundingSphere(pModel);

		/* Success! */
		VertexSet_DestructM(&vset);
		Polygon_DestructM(&pol);
		PolySet_DestructM(&pset);
		return pModel;
	} else
		return NULL;	/* Memory allocation failure. */
}
