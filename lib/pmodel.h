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
* FILE : pmodel.h
* Purpose : Header file for procedural model generation functions.
* Description : Procedural models are a way to generate simple 3D
*               models such as spheres, boxes, cubes and cylinders.
*               All functions belong to the Model structure.
********************************************************************/

#ifndef PMODEL_H
#define PMODEL_H

#include "model.h"
#include "vertxset.h"

/* Model_NewSphere(fRadius, nSweepCount, nSegmentCount),
 * Generates a sphere with a given radius. The main axis is the Y
 * axis, on that axis, nSegmentCount separate segments will be
 * generated. This defines a 'half moon' curve which will be swept
 * using nSweepCount parts.
 */
struct Model *Model_NewSphere(float fRadius,
										int nSweepCount, int nSegmentCount);

/* Model_NewCube(fRadius),
 * Generates a cube of which all faces are fRadius in distance from
 * the center of the cube.
 */
struct Model *Model_NewCube(float fRadius);

/* Model_NewBox(fXRadius, fYRadius, fZRadius),
 * Generates a box of which faces perpendicular with the X axis are
 * fXRadius from the center, faces perpendicular with the Y axis are
 * fYRadius from the center and faces perpendicular with the Z axis
 * are fZRadius from the center.
 */
struct Model *Model_NewBox(float fXRadius, float fYRadius, float fZRadius);

/* Model_NewCylinder(fRadius, fLength, nSweepCount),
 * Generates a cylinder fLength long and with a radius of fRadius. The
 * nSweepCount value specifies the number of faces on the hull of the
 * cylinder. The total number of faces is therefore nSweepCount + 2.
 */
struct Model *Model_NewCylinder(float fRadius, float fLength, int nSweepCount);

/* Model_NewSweep(pVertices, nSweepCount),
 * Generates a sweep of all 2D vertices define in pVertices. The vertices
 * must lie in the Z plane and have vertices with all X positions > 0.
 * Primarily used for Cylinder and Sphere construction. */
struct Model *Model_NewSweep(struct VertexSet *pVertices, int nSweepCount);

#endif
