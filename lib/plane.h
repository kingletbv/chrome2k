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
* FILE : plane.h
* Purpose : Header for the Plane mathematical primitive.
* Description : The Plane structure is the mathematical primitive
*               used in BSP trees (found in Classes, consisting
*               of hyperplanes).
********************************************************************/

#ifndef PLANE_H
#define PLANE_H

#include "vector.h"

/* Some forward declarations for structures used by pointers. */
struct Polygon;
struct FloatSet;
struct VertexSet;

struct Plane
{
	struct Vector Normal;							/* The plane's normal
															 * vector. It must 
															 * always be a unit
															 * vector. */
	float Distance;									/* Distance of the plane
															 * from the origin of
															 * the coordinate
															 * system. Measured in
															 * Normal Vectors. May
															 * be negative if the 
															 * Normal points in the
															 * direction of the
															 * origin. */
};

/* Plane_Construct(pThis),
 * Plane_ConstructM(pThis),
 * Initializes a plane structure. Normal is the Z-axis, distance
 * from origin is 0.
 */
void Plane_Construct(struct Plane *pThis);
#define Plane_ConstructM(pThis)\
(	(pThis)->Normal.V[0] = 0.f,\
	(pThis)->Normal.V[1] = 0.f,\
	(pThis)->Normal.V[2] = 1.f,\
	(pThis)->Distance = 0.f\
)

/* Plane_DistanceOfVector(pThis, pVector),
 * Plane_DistanceOfVectorM(pThis, pVector),
 * Calculates the distance of the Vector from the plane. This is a
 * negative value if the Vector lies in the BackSide of the Plane.
 */
float Plane_DistanceOfVector(struct Plane *pThis,
                                      struct Vector *pVector);
/* Calculate the inproduct and subtract the distance from the
 * origin. */
#define Plane_DistanceOfVectorM(pThis, pVector) \
(	(pThis)->Normal.V[0] * (pVector)->V[0] +\
	(pThis)->Normal.V[1] * (pVector)->V[1] +\
	(pThis)->Normal.V[2] * (pVector)->V[2] -\
	(pThis)->Distance\
)

/* Plane_ClipPolygon(pThis, pSrcPolygon, pDistances, pSrcVertices,
 *                   pNIDistances, pTrgPolygon, pTrgVertices),
 * Clips a polygon pSrcPolygon to plane pThis (side opposite to
 * the normal is removed) using the distances found in the FloatSet
 * pDistances. Newly created vertices are stored in pTrgVertices and
 * use negative indices in the polygons. Negative indices are also
 * looked up in pTrgVertices, negatively indexed distances are
 * looked up in pNIDistances. The resulting polygon is stored in
 * pTrgPolygon.
 */
int Plane_ClipPolygon(struct Plane *pThis,
							 struct Polygon *pSrcPolygon,
							 struct FloatSet *pDistances,
							 struct VertexSet *pSrcVertices,
							 struct FloatSet *pNIDistances,
							 struct Polygon *pTrgPolygon,
							 struct VertexSet *pTrgVertices);

#endif
