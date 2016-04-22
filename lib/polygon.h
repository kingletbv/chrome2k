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
* FILE : polygon.h
* Purpose : Header file for the Polygon structure.
* Description : The polygon structure describes a 3D polygon by
*               indexing in a VertexSet structure. This structure
*               also describes shading aspect for the whole polygon,
*               such as the polygon's color.
********************************************************************/

#ifndef POLYGON_H
#define POLYGON_H

#include "vector.h"
#include "plane.h"
#include "indexset.h"
#include "vertxset.h"
#include "colormgr.h"

enum POLFLAGS {
	PF_STATICCOLOR = 1,			/* Polygon always uses the same color
										 * for shading, pLightmap points to a 
										 * Lightmap1 structure. */
	PF_DYNACOLOR = 2,				/* Polygon has a single color but is
										 * affected by lighting conditions,
										 * pLightmap points to a Lightmap256
										 * structure. */
	PF_DUMMY							/* Dummy to end of enumeration. */
};

struct Polygon
{
	unsigned long	nFlags;		/* One of POLFLAGS, describing the type of
										 * shading that is to be applied to the
										 * Polygon. */
	void	*pLightmap;				/* Void pointer to the lightmap that should
										 * be used for the rendering of this
										 * polygon.
										 * This point to any type of structure,
										 * depending on the contents of nFlags. */
	unsigned long ulRGB;			/* RGB color (0xRRGGBB) of the polygon. */
	struct IndexSet Vertices;	/* Indices to all vertices in the polygon.
										 * The VertexSet that the indices point in
										 * are not specified at a polygon level but
										 * are specified per polyhedron. */
};

/* Polygon_Construct(pThis),
 * Polygon_ConstructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Initializes the polygon to have color white, and have no vertices.
 * Color IS affected by lighting conditions.
 */
void Polygon_Construct(struct Polygon *pThis);
#define Polygon_ConstructM(pThis)\
(	(pThis)->nFlags = PF_STATICCOLOR,\
	(pThis)->pLightmap = NULL,\
	IndexSet_ConstructM(&((pThis)->Vertices)),\
	(pThis)->ulRGB = 0xFFFFFF\
)

/* Polygon_Destruct(pThis),
 * Polygon_DestructM(pThis),
 * Frees all memory associated IN the structure, doesn't free the
 * pointer itself.
 */
void Polygon_Destruct(struct Polygon *pThis);
#define Polygon_DestructM(pThis)\
	IndexSet_Destruct(&((pThis)->Vertices))

/* Polygon_CloneM(pThis, pSrc),
 * Clones pSrc into pThis. Both pThis and pSrc MUST be initialized
 * Polygon structures. pThis will also receive the rendering information
 * stored in pSrc, including any Lightmap, however, the reference count
 * of such Lightmap is NOT incremented. pThis should therefore only be
 * used as a temporary polygon or you should increment the count
 * yourself.
 * Returnvalue is that of IndexSet_Clone(). (0 = failure, 1 = success).
 */
#define Polygon_CloneM(pThis, pSrc)\
(	(pThis)->nFlags = (pSrc)->nFlags,\
	(pThis)->pLightmap = (pSrc)->pLightmap,\
	(pThis)->ulRGB = (pSrc)->ulRGB,\
	IndexSet_Clone(&((pSrc)->Vertices), &((pThis)->Vertices))\
)

/* Polygon_GetCountM(pThis),
 * Retrieves the number of vertices in a polygon.
 * (due to the simplicity of this function, only a macro version is
 *  available.)
 */
#define Polygon_GetCountM(pThis)\
	((pThis)->Vertices.nCount)

/* Polygon_ExtractNormal(),
 * Extracts the normal for a given polygon. The normal vector's
 * direction is defined as being toward the viewer if the polygon's
 * vertices appear counterclockwise.
 * Furthermore, the length of the vector is always 1.
 * This function goes quite heavy on processing power, don't use
 * it realtime. */
void Polygon_ExtractNormal(struct Polygon *pThis,
									struct VertexSet *pVertices,
                           struct Vector *pNormal);

/* Polygon_ExtractPlane(),
 * Builds a plane for a polygon. If the polygon's vertices are not
 * exactly in the same plane, a best-fit plane will be produced.
 */
void Polygon_ExtractPlane(struct Polygon *pThis,
								  struct VertexSet *pVertices,
								  struct Plane *pPlane);

/* Polygon_LinkToColorManager(),
 * Links the polygon to a ColorManager by requesting the colors
 * it needs. This must ALWAYS be called before a polygon is
 * rendered. */
int Polygon_LinkToColorManager(struct Polygon *pThis,
										 struct ColorManager *pColorManager);

#endif
