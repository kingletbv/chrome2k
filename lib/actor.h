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
* FILE : actor.h
* Purpose : Header file for the Actor structure.
* Description : The Actor structure describes an object (Actor) as it
*               exists in 3D space. It's geometry is fully based on a
*               Model structure. Multiple Actors may use the same
*               Model for their geometry. All temporary variables or
*               position dependant variables are stored in the Actor
*               structure so multiple Actors using the same Model
*               don't interfere with eachoter. Because of this, the
*               Actor structure is rather large and looks
*               complicated.
********************************************************************/

#ifndef ACTOR_H
#define ACTOR_H

#include "model.h"
#include "trans.h"
#include "planeset.h"
#include "actptset.h"
#include "frame.h"
#include "scvtxset.h"
#include "floatset.h"

struct Actor
{
	/* Pointer to the next Actor in the list. Actors are prepared for
	 * display in the order of this list. The largest Actors should go
	 * first for correct display. Other Actors are inserted into the
	 * BSP Trees of Actors already inserted. This also determines the
	 * order of display in rendering and overlapping problems. */
	struct Actor *pNext;

	/* Pointer to the Model on which the geometry of this Actor
	 * is based. */
	struct Model *pModel;

	/* Frame which describes the position, orientation and position
	 * in the world hierarchy of this Actor. */
	struct Frame	ActorFrame;

	/* Collection of all the planes that the Actor's Model must
	 * be clipped with. The side opposite to the plane's normal
	 * will be removed for each plane, thus the volume described
	 * will always be convex.
	 * Planes marked for clipping (such as the view frustrum's
	 * planes) will be added here if the bounding sphere (from the
	 * Model pModel) intersects the plane. */
/// NOT USED ANYWHERE!!!
	struct PlaneSet	ClippingPlanes;

	/* Set containing pointers to all Actors that are contained
	 * within the subspaces of this Actor. The total number of
	 * pointers is identical to the number of SubSpaces in the
	 * Actor's Model (pModel).
	 * During the first Display phase, Actor's inserted into the
	 * Display BSP Tree after this Actor may be inserted in the
	 * BSP Tree of this Actor. Because the Model's BSP Tree may
	 * not be altered (because multiple Actors may be using it)
	 * the BSP Tree leaf contents are inserted here. */
	struct ActorPtrSet	SubActorSet;

	/* Viewpoint origin. This specifies the location of the
	 * viewpoint in the Actor's frame. It is used by the
	 * Viewpoint_Draw() function to traverse this Viewpoint's
	 * BSP tree. It is filled by the Viewpoint_PrepActorsForDraw()
	 * function. */
	struct Vector	ViewpointOrigin;

	/* Set containing polygons. This is the first clipped polygon
	 * buffer. Here the polygons from the clip operations reside. */
	struct PolySet		ClippedPolySetA;

	/* Set containing polygons. This is the second clipped polygon
	 * buffer. Here polygons from the clip operations reside. */
	struct PolySet		ClippedPolySetB;

	/* Pointer to the current source polyset. This defines where
	 * the polygons are. Initially this would point into the Model,
	 * but after clipping it would point to either ClippedPolySetA or
	 * ClippedPolySetB. */
	struct PolySet		*pSrcPolySet;

	/* Pointer to the current target polyset. This defines where
	 * the newly created polygons (from a clipping process) should go.
	 * Initially this is set to ClippedPolySetA, but when
	 * ClippedPolySetA has been filled, it becomes ClippedPolySetB.
	 * The two sets exchange eachother in both pSrcPolySet and
	 * pTrgPolySet, thus avoiding overwriting polygons. */
	struct PolySet		*pTrgPolySet;

	/* Set containing vertices. These vertices were created from
	 * intersections with clipping planes. They can be identified by
	 * having a negative index in a polygon. */
	struct VertexSet	ClippedVertexSet;

	/* Sets containing 2D position of vertices and some shading
	 * information. We have two of these, one for normal vertices
	 * and one for clipped vertices. */
	struct ScreenVertexSet	NormalScreenVertices;
	struct ScreenVertexSet	ClippedScreenVertices;

	/* Set containing lighting intensities for polygons.
	 * Only valid for those polygons that actually need
	 * intensity for a polygon. */
	struct FloatSet	NormalPolygonIntensities;
	struct FloatSet	ClippedPolygonIntensities;
};


/* Actor_Construct(pThis),
 * Actor_ConstructM(pThis),
 * Initializes an Actor structure. */
void Actor_Construct(struct Actor *pThis);
#define Actor_ConstructM(pThis)\
	Actor_Construct(pThis)

/* Actor_Destruct(pThis),
 * Actor_DestructM(pThis), (REDUNDANT MACRO)
 * Frees all memory associated with an actor. */
void Actor_Destruct(struct Actor *pThis);
#define Actor_DestructM(pThis)\
	Actor_Destruct(pThis)

/* Actor_InsertActor(pThis, pActor),
 * Inserts an actor pActor into actor pThis. What this effectively
 * does is it determines the subspace in which the centerpoint of
 * pActor is in pThis. Then it checks if pThis' SubActorSet contains
 * an Actor for that subspace. If it doesn't (ptr == NULL), the
 * Actor is put in that subspace, otherwise the procedure is
 * recursively applied for the Actor already in that subspace.
 */
int Actor_InsertActor(struct Actor *pThis,
			 struct Actor *pActor);

/* Actor_SetModel(pThis, pModel),
 * Attaches a Model to an actor.
 * This intializes several buffers.
 */
int Actor_SetModel(struct Actor *pThis,
			 struct Model *pModel);

#ifdef DEBUGC
/* Actor_DumpScreenVertices(pThis),
 * Dumps all screen vertex coordinates contained in pThis to stdout.
 */
void Actor_DumpScreenVertices(struct Actor *pThis);
#endif

#ifdef DEBUGC
/* Actor_DumpScreenPolygons(pThis),
 * Dumps all polygons to screen by their screen coordinates.
 */
void Actor_DumpScreenPolygons(struct Actor *pThis);
#endif
#endif