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
* FILE : model.h
* Purpose : Header file for the Model structure.
* Description : The Model structure describes the shape of an Actor.
*               It is the foundation upon which all objects (Actors)
*               are based. It is used directly for rendering.
********************************************************************/

#ifndef MODEL_H
#define MODEL_H

#include "vector.h"
#include "hplane.h"
#include "vertxset.h"
#include "polyset.h"
#include "octree.h"

struct Model
{
	/* Bounding volume of the Model.
	 * This is a sphere. The sphere has it's own center point, not
	 * (0,0,0) because some Models don't rotate around the center
	 * point but around some other point.
	 */
	struct Vector	Centerpoint;
	float				fRadius;			/* Radius of the bounding sphere. */
	
	/* Binary Space Partioning Tree (BSP Tree or BSPT)
	 * The BSP Tree is described by a pointer to the first hyperplane
	 * (HPlane). Each hyperplane contains two indexsets that specify
	 * which polygons are coplanar with the hyperplane and on what
	 * side of the hyperplane.
	 */
	struct HPlane	*pRoot;
	
	/* Vertices of the Model.
	 * All vertices are stored here. The vertices stored represent the
	 * vertices of the base Model, changes such as transformations in
	 * 3D space and clipping operations are not reflected in the Model
	 * but in the Actor.
	 */
	struct VertexSet	Vertices;
	
	/* Polygons of the Model.
	 * All polygons are stored here. These are referenced by index
	 * from the BSP Tree (pRoot). The polygons stored here represent
	 * the polygons of the base Model, changes such as clipping are
	 * not reflected in the Model but in the Actor.
	 */
	struct PolySet	Polygons;
};

/* Model_Construct(pThis),
 * Model_ConstructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Initializes a Model structure. */
void Model_Construct(struct Model *pThis);
#define Model_ConstructM(pThis)\
(	(pThis)->Centerpoint.V[0] = 0.f,\
	(pThis)->Centerpoint.V[1] = 0.f,\
	(pThis)->Centerpoint.V[2] = 0.f,\
	(pThis)->fRadius = 0.f,\
	(pThis)->pRoot = NULL,\
	VertexSet_Construct(&((pThis)->Vertices)),\
	PolySet_Construct(&((pThis)->Polygons))\
)

/* Model_Destruct(pThis),
 * Model_DestructM(pThis),
 * Frees all memory associated with a Model structure. */
void Model_Destruct(struct Model *pThis);
#define Model_DestructM(pThis)\
(	VertexSet_Destruct(&((pThis)->Vertices)),\
	PolySet_Destruct(&((pThis)->Polygons))\
)

/* Model_CalcBoundingSphere(pThis),
 * Initializes the Centerpoint and fRadius fields (the bounding
 * sphere) of a Model structure from it's vertices.
 * The sphere constructed is usually off by about 7% as a 
 * tradeoff for computational speed. */
void Model_CalcBoundingSphere(struct Model *pThis);

/* Model_LinkToColorManager(pThis, pColorManager)
 * Links a Model to a ColorManager so the Model can be rendered using
 * the colors specified by the ColorManager.
 */
int Model_LinkToColorManager(struct Model *pThis,
									  struct ColorManager *pColorManager);

/* Model_RequestColors(pThis, pOctree),
 * Requests all colors for all polygons in a Model.
 * This doesn't initialize the color indices, use
 * Model_UpdateColorIndices() for that.
 */
int Model_RequestColors(struct Model *pThis, struct Octree *pOctree); 

/* Model_UpdateColorIndices(pThis, pOctree)
 * Defines all color indices for all polygons in a Model.
 * This sets the color indices, after this call the Model can
 * be used for rendering.
 */
void Model_UpdateColorIndices(struct Model *pThis, struct Octree *pOctree);

#endif
