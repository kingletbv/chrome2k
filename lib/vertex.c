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
* FILE : vertex.c
********************************************************************/

#define VERTEX_C

#include "vertex.h"

/********************************************************************
* Function : Vertex_Interpolate()
* Purpose : Interpolates two vertices producing a third. This is used
*           primarily in BSP Tree building and clipping operations.
* Pre : pThis points to an initialized Vertex structure, pThat
*       points to an initialized Vertex structure, fInterpol is
*       a value between 0 and 1 that determines the interpolation,
*       pTarget points to a Vertex Structure.
* Post : pTarget contains the interpolated vertex. If fInterpol
*        was 0, it will be entirely pThis, if fInterpol was 1, it
*        will be entirely pThat. If fInterpol was somewhere
*        between 0 and 1, pTarget contains the lineair interpolation
*        between pThis and pThat on that position.
********************************************************************/
void Vertex_Interpolate(struct Vertex *pThis, struct Vertex *pThat,
								float fInterpol, struct Vertex *pTarget)
{
	/* Just call the macro version. */
	Vertex_InterpolateM(pThis, pThat, fInterpol, pTarget);
}