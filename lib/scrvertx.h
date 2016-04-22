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
* FILE : scrvertx.h
* Purpose : Header file for the ScreenVertex structure.
* Description : The ScreenVertex structure describes a vertex in 2D
*               coordinates where they would appear on screen.
*               It also stores things like intensity of light at the
*               vertex (for gouraud shading) and X and Y chrome
*               map coordinates.
*               Note that texture mapping coordinates can NOT be
*               stored in the ScreenVertex structure because multiple
*               polygons can have different textures while using the
*               same vertices.
********************************************************************/

#ifndef SCRVERTX_H
#define SCRVERTX_H

struct ScreenVertex
{
	short				nX, nY;		/* 2D screen coordinate of vertex. */

	unsigned char	nIntensity;	/* Intensity of vertex (ranges from
										 * 0 to 255 for resp. fully dark and
										 * fully lit). */
	
	short				nCX, nCY;	/* X and Y coordinates for chrome
										 * mapping. */
};

/* ScreenVertex_ConstructM(pThis),
 * Initializes a ScreenVertex structure, setting the intensity to full
 * and all coordinates to 0,0.
 * NOTE : It's not really required to call this function because the
 *        members of the structure don't contain anything vital. 
 */
#define ScreenVertex_ConstructM(pThis)\
(	(pThis)->nX = 0,\
	(pThis)->nY = 0,\
	(pThis)->nIntensity = 255,\
	(pThis)->nCX = 0,\
	(pThis)->nCY = 0\
)

#endif
