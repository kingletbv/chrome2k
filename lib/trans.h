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
* FILE : trans.h
* Purpose : Header for the transformation mathematical primitive.
* Description : The Transformation structure is the mathematical
*               primitive used in the transformation hierachy. The
*               hierarchy is described by Frame structures.
********************************************************************/

#ifndef TRANS_H
#define TRANS_H

#include "vector.h"
#include "plane.h"

struct Transformation
{
	float Rotation[3][3];	/* 3 by 3 rotation matrix, must contain
				 * an orthogonal rotation matrix.
				 * The first index is the row, the
				 * second is the column. */

	struct Vector Translation;		/* Translation vector */
};

/* Transformation_Construct(pThis)
 * Initializes transformation pThis to a "no operation" transformation.
 */
void Transformation_Construct(struct Transformation *pThis);

/* Transformation_Inverse(pThis, pTarget)
 * Computes the inverse of transformation pThis and puts the result in
 * pTarget.
 */
void Transformation_Inverse(struct Transformation *pThis,
                            struct Transformation *pTarget);

/* Transformation_Concatenate(pThis, pOther, pTarget)
 * Concatenates transformations pThis and pOther and puts the resulting
 * Transformation in pTarget.
 */
void Transformation_Concatenate(struct Transformation *pThis,
                                struct Transformation *pOther,
                                struct Transformation *pTarget);

/* Transformation_ConcatenateRotation(pThis, pOther, pTarget)
 * The same as Transformation_Concatenate, but now it ignores
 * translation. */
void Transformation_ConcatenateRotation(struct Transformation *pThis,
					 struct Transformation *pOther,
					 struct Transformation *pTarget);

/* Transformation_Transform(pThis, pSource, pTarget)
 * Transforms vector pSource through pThis and puts the resulting
 * vector in pTarget.
 */
void Transformation_Transform(struct Transformation *pThis,
                              struct Vector *pSource,
                              struct Vector *pTarget);


/* Multiply pSource with the rotation matrix and add pThis' translation
 * vector. Target must not be source! */
#define Transformation_TransformM( pThis, pSource, pTarget) \
(	(pTarget)->V[0] = (pThis)->Translation.V[0] + \
	            (pThis)->Rotation[0][0] * (pSource)->V[0] + \
	            (pThis)->Rotation[0][1] * (pSource)->V[1] + \
	            (pThis)->Rotation[0][2] * (pSource)->V[2] \
, \
	(pTarget)->V[1] = (pThis)->Translation.V[1] + \
	            (pThis)->Rotation[1][0] * (pSource)->V[0] + \
	            (pThis)->Rotation[1][1] * (pSource)->V[1] + \
	            (pThis)->Rotation[1][2] * (pSource)->V[2] \
, \
	(pTarget)->V[2] = (pThis)->Translation.V[2] + \
	            (pThis)->Rotation[2][0] * (pSource)->V[0] + \
	            (pThis)->Rotation[2][1] * (pSource)->V[1] + \
	            (pThis)->Rotation[2][2] * (pSource)->V[2] \
)



/* Transformation_InvTransform(pThis, pSource, pTarget)
 * Transforms vector pSource through the inverse of pThis and
 * puts the resulting vector in pTarget.
 * This is like inverting pThis and then calling
 * Transformation_Transform().
 */
void Transformation_InvTransform(struct Transformation *pThis,
				struct Vector *pSource,
				struct Vector *pTarget);

/* Transformation_TransformPlane(pThis, pSource, pTarget)
 * Transforms plane pSource through pThis and puts the resulting
 * plane in pTarget.
 */
void Transformation_TransformPlane(struct Transformation *pThis,
                                   struct Plane *pSource,
                                   struct Plane *pTarget);

/* Transformation_InvTransformPlane(pThis, pSource, pTarget)
 * Transforms plane pSource through the inverse of pThis and
 * puts the resulting plane in pTarget.
 * This is like inverting pThis and then calling 
 * Transformation_TransformPlane().
 */
void Transformation_InvTransformPlane(struct Transformation *pThis,
                                      struct Plane *pSource,
                                      struct Plane *pTarget);


/* Transformation_Rotate(pThis, pSource, pTarget)
 * Transforms vector pSource through pThis performing only the rotation and
 * puts the resulting vector in pTarget.
 */
void Transformation_Rotate(struct Transformation *pThis,
                           struct Vector *pSource,
                           struct Vector *pTarget);

/* Transformation_InvRotate(pThis, pSource, pTarget)
 * Transforms vector pSource through the inverse of pThis,
 * performing only the rotation and puts the resulting vector
 * in pTarget. This is like inverting pThis and then calling
 * Transformation_Rotate().
 */
void Transformation_InvRotate(struct Transformation *pThis,
                              struct Vector *pSource,
                              struct Vector *pTarget);

/* Transformation_ScaleXYRow(pThis, fScale)
 * Transformation_ScaleXYRowM(pThis, fScale)
 * Scales the X and Y row of the transformation pThis by fScaleX
 * and fScaleY. This should not be used on transformations that are
 * attached to the 3D hierarchy(!) because the inverse function will
 * not work properly anymore.
 * Note also that the translation vector gets the multiplication too.
 */
void Transformation_ScaleXYRow(struct Transformation *pThis,
										 float fScaleX, float fScaleY);
#define Transformation_ScaleXYRowM(pThis, fScaleX, fScaleY)\
(	(pThis)->Rotation[0][0] *= fScaleX,\
	(pThis)->Rotation[0][1] *= fScaleX,\
	(pThis)->Rotation[0][2] *= fScaleX,\
	(pThis)->Rotation[1][0] *= fScaleY,\
	(pThis)->Rotation[1][1] *= fScaleY,\
	(pThis)->Rotation[1][2] *= fScaleY,\
	(pThis)->Translation.V[0] *= fScaleX,\
	(pThis)->Translation.V[1] *= fScaleY\
)

/* Transformation_ForceRotationXY(pThis),
 * Takes the X and Y axes of a Transformation's rotation matrix and produces
 * a new matrix, primarily based on the X secondarily based on the Y and
 * with a newly calculated Z vector which is orthogonal and can thus be used
 * as a Transformation in the Transformation Hierarchy. */
void Transformation_ForceRotationXY(struct Transformation *pThis);

/* Transformation_ForceRotationZY(pThis),
 * Takes the Z and Y axes of a Transformation's rotation matrix and produces
 * a new matrix, primarily based on the Z secondarily based on the Y and
 * with a newly calculated X vector. This function is very similair to
 * Transformation_ForceRotationXY(), but instead it primarily uses the Z
 * axis and is therefore useful for aiming viewpoints at objects etc. */
void Transformation_ForceRotationZY(struct Transformation *pThis);

/* Transformation_MakeRotateArbitrary(pThis, pAxis, fAngle),
 * Fills the rotation matrix of pThis as a rotation around an
 * axis pAxis with an angle of fAngle. */
void Transformation_MakeRotateArbitrary(struct Transformation *pThis,
					 struct Vector *pAxis,
					 float fAngle);

#endif
