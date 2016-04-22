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
* FILE : frame.h
* Purpose : Header for the Frame structure.
* Description : The Geometry Hierarchy consists of Frame structures,
*               each of which has a parent. Thus we get a tree
*               structure that describes the whole hierarchy. Each
*               Frame consists of a pointer to a parent Frame (which
*               may be NULL in which case the parent Frame is the
*               root Frame) and a Transformation to that parent
*               Frame.
********************************************************************/

#ifndef FRAME_H
#define FRAME_H

#include "trans.h"

struct Frame
{
	struct Frame				*pParent;			/* Pointer to parent Frame,
															 * may be NULL in which case
															 * the parent Frame is the 
															 * root frame. */
	struct Transformation	TransformationToParent;
															/* Transformation to the parent
															 * Frame. */
};

/* Frame_Construct(pThis),
 * Frame_ConstructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Initializes a Frame structure. */
void Frame_Construct(struct Frame *pThis);
#define Frame_ConstructM(pThis)\
(	(pThis)->pParent = NULL,\
	Transformation_Construct(&((pThis)->TransformationToParent))\
)

/* Frame_GetTransformationToRoot(pThis, pTarget)
 * Builds a transformation from Frame pThis to the root (NULL Frame).
 */
void Frame_GetTransformationToRoot(struct Frame *pThis,
                                   struct Transformation *pTarget);

/* Frame_GetTransformationFromRoot(pThis, pTarget)
 * Builds a transformation from the root (NULL frame) to pThis. This
 * is the same transformation as the inverse of
 * Frame_GetTransformationToRoot().
 */
void Frame_GetTransformationFromRoot(struct Frame *pThis,
                                     struct Transformation *pTarget);

/* Frame_GetTransformationToFrame(pThis, pOther, pTarget)
 * Builds a transformation from Frame pThis to Frame pOther. This is
 * identical to the concatenation of FromRoot(pOther) x ToRoot(pThis).
 */
void Frame_GetTransformationToFrame(struct Frame *pThis,
                                    struct Frame *pOther,
                                    struct Transformation *pTarget);

/* Frame_RelocateFrame(pThis, pNewParent)
 * Removes pThis from it's current parent and attaches it to it's new
 * parent pNewParent without actually moving it in space.
 * This is very useful when, for example, firing a missile. The moment
 * it has to be detached from the plane frame it will be
 * relocated in the geometry hierarchy below the root frame.
 * In this operation, the missile should not change position.
 */
void Frame_RelocateFrame(struct Frame *pThis,
                         struct Frame *pNewParent);
#endif
