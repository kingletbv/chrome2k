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
* FILE : hplane.h
* Purpose : Header for the hyperplane structure, which is used for
*           bsp tree nodes.
********************************************************************/

#ifndef HPLANE_H
#define HPLANE_H

#include "vector.h"
#include "plane.h"
#include "indexset.h"
#include "vertxset.h"
#include "polyset.h"
#include "floatset.h"
#include "polygon.h"
#include "hplane.h"

/* Constant for the planar property. Vertices closer than this value
 * to a plane are considered on that plane. */
#ifdef HPLANE_C
#define ISONPLANE 0.001f
#endif

struct HPlane
{
	struct Plane BinPlane;							/* The Binary Intersection
															 * plane. */

	struct HPlane	*pInSubtree;					/* The Inside subtree, this is
															 * the area opposite to
															 * BinPlane's normal vector.
															 * This subspace is considered
															 * solid if it is NULL. */
	struct HPlane	*pOutSubtree;					/* The Outside subtree, this is
															 * the area in the direction
															 * of BinPlane's normal vector.
															 * This subspace is considered
															 * empty if it is NULL. */

	int	nInsideLeafCount;							/* Number of leafs in Inside
															 * subtree. If pInSubtree is
															 * NULL this should be 1. */

	struct IndexSet	InsideIndices;				/* Indices to the polygons
															 * visible from the In Side of
															 * this HPlane, these must be
															 * coplanar with BinPlane. */
	struct IndexSet	OutsideIndices;			/* Indices to the polygons
															 * visible from the Out Side of
															 * this HPlane, these must be
															 * coplanar with BinPlane. */
};

/* HPlane_Construct(pThis),
 * HPlane_ConstructM(pThis), (REDUNDANT MACRO)
 * Initializes a single HPlane structure. */
void HPlane_Construct(struct HPlane *pThis);
#define HPlane_ConstructM(pThis)\
	HPlane_Construct(pThis)

/* HPlane_ConstructTree(pPolygons, pVertices, pNewPolygons),
 * Constructs a full BSP tree given a PolySet (pPolygons) and a
 * VertexSet (pVertices).
 */
struct HPlane *HPlane_ConstructTree(struct PolySet *pPolygons,
												struct VertexSet *pVertices,
												struct PolySet *pNewPoygons);

/* HPlane_ConstructTreeQuick(pPolygons, pVertices, pNewPolygons),
 * Constructs a full BSP tree given a PolySet (pPolygons) and a
 * VertexSet (pVertices).
 * The difference with HPlane_ConstructTree() is that this function
 * doesn't attempt to produce a good tree, but to produce one
 * quickly.
 */
struct HPlane *HPlane_ConstructTreeQuick(struct PolySet *pPolygons,
													  struct VertexSet *pVertices,
													  struct PolySet *pNewPolygons);


/* HPlane_DestroyTree(pThis),
 * Frees a whole tree of HPlanes, including it's root pThis.
 */
void HPlane_DestroyTree(struct HPlane *pThis);

/* HPlane_Destruct(pThis),
 * HPlane_DestructM(pThis) (REDUNDANT MACRO),
 * Frees all memory associated with a SINGLE HPlane structure,
 * does *NOT* free the HPlane structure itself, nor any of it's
 * children. */
void HPlane_Destruct(struct HPlane *pThis);
#define HPlane_DestructM(pThis)\
	HPlane_Destruct(pThis)

/* HPlane_SplitPolygon(pThis, pPolygon, pDistances, pVertices,
 *                     pInsidePol, pOutsidePol)
 * Splits a polygon pPolygon that spans a HPlane pThis into two
 * seperate polygons pInsidePol and pOutsidePol based on the
 * distances of it's vertices relative to the plane pDistances and
 * it's vertices pVertices.
 * Intersecting vertices consist of interpolated vertices. The
 * intersecting vertices are added to pVertices.
 */
int HPlane_SplitPolygon(struct HPlane *pThis, struct Polygon *pPolygon,
								struct FloatSet *pDistances,
								struct VertexSet *pVertices,
								struct Polygon *pInSidePol,
								struct Polygon *pOutSidePol);

/* HPlane_CalculateLeafCount(pThis)
 * Traverses a polygon and returns the total number of leafs. During this
 * process, the nInsideLeafCount fields in all HPlanes are also updated.
 * The total number of leafs can be seen as the number of NULL pointers
 * in the pInSubtree and pOutSubtree fields for a given tree.
 */
int HPlane_CalculateLeafCount(struct HPlane *pThis);

/* HPlane_GetVectorSubspaceIndex(pThis, pVector)
 * Traverses a hyperplane tree and returns the index of the subspace
 * in which pVector is located.
 */
int HPlane_GetVectorSubspaceIndex(struct HPlane *pThis,
											 struct Vector *pVector);

#ifdef DEBUGC
/* HPlane_DebugDump(pThis, nIndent)
 * Displays the whole tree to stdout. Used for debug builds.
 */
void HPlane_DebugDump(struct HPlane *pThis, int nIndent);
#endif

#endif
