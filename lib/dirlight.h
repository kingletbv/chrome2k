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
* FILE : dirlight.h
* Purpose : Header file for the DirLight structure.
* Description : The DirLight structure describes a directional
*               lightsource. A directional lightsource consists of a
*               vector which defines the direction of the light. The
*               lightsource itself is at infinity. It is generally
*               used to model very distant lightsources like the sun.
********************************************************************/

#ifndef DIRLIGHT_H
#define DIRLIGHT_H

#include "vector.h"
#include "frame.h"

struct DirLight
{
	struct DirLight	*pNext;		/* Next in linked list. */
	struct Frame	DirLightFrame;	/* Specifies frame in which the DirLight
											 * is in the geometry hierarchy. */
	struct Vector	Direction;		/* Direction of Lightsource.
											 * The length of this vector also
											 * defines it's intensity.
											 * A length of 1 is full intensity,
											 * anything smaller is less intensity,
											 * anything longer is overlight. */
};

/* DirLight_ConstructM(), (NEEDS stdlib.h INCLUDED)
 * Initializes a DirLight structure.
 * Full intensity, lighting in Z direction. */
#define DirLight_ConstructM(pThis)\
(	(pThis)->pNext = NULL,\
	Frame_ConstructM(&((pThis)->DirLightFrame)),\
	(pThis)->Direction.V[0] = 0.f,\
	(pThis)->Direction.V[1] = 0.f,\
	(pThis)->Direction.V[2] = 0.f\
)

/* DirLight_CalcIntensityM(),
 * Calculates the intensity of a normal lit by a DirLight.
 * The Normal is assumed to be in the same frame as the lightsource.
 * An intensity is a float ranging from 0 to 1 under normal DirLight
 * conditions, overlight results in values above 1, underlight results
 * in values below 0.
 */
#define DirLight_CalcIntensityM(pThis, pNormal)\
(	((pThis)->Direction.V[0] * (pNormal)->V[0] +\
	 (pThis)->Direction.V[1] * (pNormal)->V[1] +\
	 (pThis)->Direction.V[2] * (pNormal)->V[2]))\
)

#endif
