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
* FILE : model.c
********************************************************************/

#define MODEL_C

#include <stdlib.h>
#include <float.h>		/* For max and min of the float datatype. */
#include <math.h>			/* For sqrt() */

#include "model.h"

#include "vector.h"
#include "vertex.h"
#include "vertxset.h"

/********************************************************************
* Function : Model_Construct()
* Purpose : Initializes a Model structure.
* Pre : pThis points to a Model structure.
* Post : pThis points to an initialized Model structure.
********************************************************************/
void Model_Construct(struct Model *pThis)
{	/* Call macro version */
	Model_ConstructM(pThis);
}

/********************************************************************
* Function : Model_CalcBoundingSphere()
* Purpose : Fills in the Centerpoint and fRadius fields of a Model
*           structure given the Model's vertices. The Centerpoint and
*           fRadius fields describe the bounding sphere of the Model
*           which is used for (quick) intersection testing during
*           clipping and BSP insertion.
* Pre : pThis points to an initialized Model structure that contains
*       a model (i.e. there are vertices in the Model).
* Post : pThis now has valid Centerpoint and fRadius fields.
* Note : This uses an algorithm that can be found in Graphics Gems I,
*        in the article "An efficient bounding sphere" by Jack
*        Ritter (page 301).
********************************************************************/
void Model_CalcBoundingSphere(struct Model *pThis)
{
	struct Vector xmin, xmax, ymin, ymax, zmin, zmax, dia1, dia2, cen;
	struct Vertex *pV;
	int n;
	float dx, dy, dz;
	float xspan, yspan, zspan, maxspan;
	float rad_sq, rad, old_to_p_sq, old_to_p, old_to_new;
	
	/* Constants are from include file <float.h>,
	 * initialize minima and maxima points. */
	xmin.V[0] = ymin.V[1] = zmin.V[2] = FLT_MAX;
	xmax.V[0] = ymax.V[1] = zmax.V[2] = FLT_MIN;
	
	/* FIRST PASS : find 6 minima/maxima points */
	for (n = 0; n < VertexSet_GetCountM(&(pThis->Vertices)); n++)
	{
		/* Get the current vertex. */
		pV = VertexSet_GetVertexM(&(pThis->Vertices), n);
		
		/* Check bounds. */
		/* xmin */
		if (pV->Position.V[0] < xmin.V[0])
			xmin = pV->Position;
		/* xmax */
		if (pV->Position.V[0] > xmax.V[0])
			xmax = pV->Position;
		/* ymin */
		if (pV->Position.V[1] < ymin.V[1])
			ymin = pV->Position;
		/* ymax */
		if (pV->Position.V[1] > ymax.V[1])
			ymax = pV->Position;
		/* zmin */
		if (pV->Position.V[2] < zmin.V[2])
			zmin = pV->Position;
		/* zmax */
		if (pV->Position.V[2] > zmax.V[2])
			zmax = pV->Position;
	}
	
	/* Set xspan = distance between the 2 points xmin & xmax (squared) */
	dx = xmax.V[0] - xmin.V[0];
	dy = xmax.V[1] - xmin.V[1];
	dz = xmax.V[2] - xmin.V[2];
	xspan = dx * dx + dy * dy + dz * dz;
	
	/* same for y span */
	dx = ymax.V[0] - ymin.V[0];
	dy = ymax.V[1] - ymin.V[1];
	dz = ymax.V[2] - ymin.V[2];
	yspan = dx * dx + dy * dy + dz * dz;

	/* same for z span */
	dx = zmax.V[0] - zmin.V[0];
	dy = zmax.V[1] - zmin.V[1];
	dz = zmax.V[2] - zmin.V[2];
	zspan = dx * dx + dy * dy + dz * dz;
	
	/* Set points dia1 and dia2 to the maximally separated pair */
	dia1 = xmin;
	dia2 = xmax;	/* assume xspan is biggest. */
	maxspan = xspan;
	if (yspan > maxspan)
	{	maxspan = yspan;
		dia1 = ymin;
		dia2 = ymax;
	}
	if (zspan > maxspan)
	{	dia1 = zmin;
		dia2 = zmax;
	}
	
	/* dia1, dia2 is the diameter of the initial sphere.
	 * calculate initial center. */
	cen.V[0] = (dia1.V[0] + dia2.V[0]) / 2.f;
	cen.V[1] = (dia1.V[1] + dia2.V[1]) / 2.f;
	cen.V[2] = (dia1.V[2] + dia2.V[2]) / 2.f;
	/* Calculate initial radius**2 and radius. */
	dx = dia2.V[0] - cen.V[0];	/* x component of radius vector */
	dy = dia2.V[1] - cen.V[1];	/* y component of radius vector */
	dz = dia2.V[2] - cen.V[2];	/* z component of radius vector */
	rad_sq = dx * dx + dy * dy + dz * dz;
	rad = (float)sqrt(rad_sq);
	
	/* SECOND PASS : increment current sphere */
	for (n = 0; n < VertexSet_GetCountM(&(pThis->Vertices)); n++)
	{
		/* Get the current vertex. */
		pV = VertexSet_GetVertexM(&(pThis->Vertices), n);

		dx = pV->Position.V[0] - cen.V[0];
		dy = pV->Position.V[1] - cen.V[1];
		dz = pV->Position.V[2] - cen.V[2];
		
		old_to_p_sq = dx * dx + dy * dy + dz * dz;
		
		/* Do r**2 test first. */
		if (old_to_p_sq > rad_sq)
		{	/* This point is outside of current sphere */
			old_to_p = (float)sqrt(old_to_p_sq);
			
			/* Calculate radius of new sphere. */
			rad = (rad + old_to_p) / 2.f;
			rad_sq = rad * rad;				/* For next r**2 compare. */
			old_to_new = old_to_p - rad;
			
			/* Calculate center of new sphere */
			cen.V[0] = (rad * cen.V[0] + old_to_new * pV->Position.V[0]) / old_to_p;
			cen.V[1] = (rad * cen.V[1] + old_to_new * pV->Position.V[1]) / old_to_p;
			cen.V[2] = (rad * cen.V[2] + old_to_new * pV->Position.V[2]) / old_to_p;
		}
	}
	
	/* Done, cen contains Centerpoint, rad contains fRadius. */
	pThis->Centerpoint = cen;
	pThis->fRadius = rad;
}

/********************************************************************
* Function : Model_LinkToColorManager()
* Purpose : Links a Model to a ColorManager so it can get the colors
*           it needs for drawing.
* Pre : pThis points to an initialized Model structure,
*       pColorManager points to an initialized ColorManager
*       structure.
* Post : If the returnvalue is 0, a memory failure occured otherwise
*        the Model is now linked to the ColorManager.
********************************************************************/
int Model_LinkToColorManager(struct Model *pThis,
									  struct ColorManager *pColorManager)
{
	/* Iterate all polygons and link the polygons. */
	int n;
	for (n = 0; n < pThis->Polygons.nCount; n++)
	{
		if (!Polygon_LinkToColorManager(&(pThis->Polygons.arPolygons[n]), pColorManager))
			return 0;	/* Mem failure. */
	}
	return 1;
}

/********************************************************************
* Function : Model_RequestColors()
* Purpose : Requests the colors a model needs for drawing.
* Pre : pThis points to an initialized Model structure, pOctree
*       points to an initialized Octree structure.
* Post : If the returnvalue is 1, all colors have been requested (and
*        added to the octree).
*        If the returnvalue is 0, a memory allocation failure
*        occured.
********************************************************************/
int Model_RequestColors(struct Model *pThis, struct Octree *pOctree)
{
	/* Iterate all polygons and fill in the colors. */
	int n;
	for (n = 0; n < pThis->Polygons.nCount; n++)
	{
		if (!Octree_AddColor(pOctree, pThis->Polygons.arPolygons[n].ulRGB))
			return 0;	/* Mem failure. */
	}
	return 1;
}

/********************************************************************
* Function : Model_UpdateColorIndices()
* Purpose : Updates the color indices for all polygons in a Model by
*           looking them up in an Octree.
* Pre : pThis points to an initialized Model structure,
*       pOctree points to an initialized Octree structure for
*       which Model_RequestColors() has been called with pThis.
* Post : All color indices in all polygons have been set.
********************************************************************/
void Model_UpdateColorIndices(struct Model *pThis, struct Octree *pOctree)
{
	/* Iterate all polygons and retrieve the color indices. */
	int n;
	for (n = 0; n < pThis->Polygons.nCount; n++)
	{	/* pThis->Polygons.arPolygons[n].nColor = Octree_FindColorIndex(pOctree, pThis->Polygons.arPolygons[n].ulRGB); */
		/* Dummy function. */
	}
}
