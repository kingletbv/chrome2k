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
* FILE : vertex.h
* Purpose : Header file for the vertex structure.
* Description : The vertex is used for describing the corners of
*               polygons. Apart from a 3D coordinate, a vertex can
*               also contain shading information specific for the
*               vertex. (For example, normal vectors at the vertex
*               when gouraud shading is used.)
*********************************************************************/

#ifndef VERTEX_H
#define VERTEX_H

#include "vector.h"

struct Vertex
{
	struct Vector	Position;		/* Position in 3D space of the vector. */
	struct Vector	Normal;			/* Normal vector at the vertex, used for
											 * gouraud shading. */
};

/* Vertex_Construct(pThis),
 * Vertex_ConstructM(pThis),
 * Initializes the vertex to have position (0,0,0) and normal vector (0,0,1).
 */
void Vertex_Construct(struct Vertex *pThis);
#define Vertex_ConstructM(pThis)\
	(pThis)->Position.V[0] = 0.f,\
	(pThis)->Position.V[1] = 0.f,\
	(pThis)->Position.V[2] = 0.f,\
	(pThis)->Normal.V[0] = 0.f,\
	(pThis)->Normal.V[1] = 0.f,\
	(pThis)->Normal.V[2] = 0.f

/* Vertex_Interpolate(pThis, pThat, fInterpol, pTarget),
 * Vertex_InterpolateM(pThis, pThat, fInterpol, pTarget),
 * Interpolates between vertex pThis and pThat using fInterpol
 * and stores the result in pTarget.
 * fInterpol determines the weight of the interpolation,
 * 0 is entirely pThis, 1 if entirely pThat.
 */
void Vertex_Interpolate(struct Vertex *pThis, struct Vertex *pThat,
								float fInterpol, struct Vertex *pTarget);
#define Vertex_InterpolateM(pThis, pThat, fInterpol, pTarget)\
(	(pTarget)->Position.V[0] = (pThis)->Position.V[0] + \
                            ((pThat)->Position.V[0] - \
                             (pThis)->Position.V[0]) * (fInterpol), \
	(pTarget)->Position.V[1] = (pThis)->Position.V[1] + \
                            ((pThat)->Position.V[1] - \
                             (pThis)->Position.V[1]) * (fInterpol), \
	(pTarget)->Position.V[2] = (pThis)->Position.V[2] + \
                            ((pThat)->Position.V[2] - \
                             (pThis)->Position.V[2]) * (fInterpol) \
)

#endif
