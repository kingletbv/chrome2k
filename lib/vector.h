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
* FILE : vector.h
* Purpose : Header for the Vector structure.
* Description : The Vector structure is a mathematical primitive that
*               is used for 3D points and vectors such as vertices,
*               normals etc.
********************************************************************/

#ifndef VECTOR_H
#define VECTOR_H

struct Vector
{
	float		V[3];										/* The actual Vector
															 * coordinates. */
};

/* Vector_ConstructM(pThis),
 * Clears a vector, initializing it to (0,0,0).
 */
#define Vector_ConstructM(pThis)\
(	(pThis)->V[0] = 0.f,\
	(pThis)->V[1] = 0.f,\
	(pThis)->V[2] = 0.f\
)
#endif
