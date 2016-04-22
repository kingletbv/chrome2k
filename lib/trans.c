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
* FILE : trans.c
********************************************************************/

#include <stdlib.h>	/* For NULL Constant. */
#include <math.h>		/* For sqrt() etc. */
#include "trans.h"

#include "vector.h"

/********************************************************************
* Function : Transformation_Construct()
* Purpose : Initializes a Transformation structure.
* Pre : pThis points to a Transformation structure.
* Post : pThis points to an initialized Transformation structure.
*        The actual Transformation is a nop.
********************************************************************/
void Transformation_Construct(struct Transformation *pThis)
{
	pThis->Rotation[0][0] = 1.f;
	pThis->Rotation[0][1] = 0.f;
	pThis->Rotation[0][2] = 0.f;
	pThis->Rotation[1][0] = 0.f;
	pThis->Rotation[1][1] = 1.f;
	pThis->Rotation[1][2] = 0.f;
	pThis->Rotation[2][0] = 0.f;
	pThis->Rotation[2][1] = 0.f;
	pThis->Rotation[2][2] = 1.f;
	
	pThis->Translation.V[0] = 0.f;
	pThis->Translation.V[1] = 0.f;
	pThis->Translation.V[2] = 0.f;
}

/********************************************************************
* Function : Transformation_Inverse()
* Purpose : Computes the inverse of a Transformation.
* Pre : pThis points to an initialized Transformation structure and
*       pTarget points to a Transformation structure. pThis can be
*       the same as pTarget.
* Post : pTarget now contains the inverse transformation of pThis.
********************************************************************/
void Transformation_Inverse(struct Transformation *pThis,
                            struct Transformation *pTarget)
{
	struct Transformation Temp;
	
	/* The inverse rotation matrix is simply it's transpose.
	 * This is due to the fact that it can only be a rotation,
	 * scaling and shearing are not allowed. */
	Temp.Rotation[0][0] = pThis->Rotation[0][0];
	Temp.Rotation[0][1] = pThis->Rotation[1][0];
	Temp.Rotation[0][2] = pThis->Rotation[2][0];

	Temp.Rotation[1][0] = pThis->Rotation[0][1];
	Temp.Rotation[1][1] = pThis->Rotation[1][1];
	Temp.Rotation[1][2] = pThis->Rotation[2][1];

	Temp.Rotation[2][0] = pThis->Rotation[0][2];
	Temp.Rotation[2][1] = pThis->Rotation[1][2];
	Temp.Rotation[2][2] = pThis->Rotation[2][2];
	
	/* The inverse of the translation vector is the negated 
	 * translation vector multiplied with the inverse rotation matrix. */
	Temp.Translation.V[0] = -pThis->Translation.V[0] * Temp.Rotation[0][0] -
	                        pThis->Translation.V[1] * Temp.Rotation[0][1] -
	                        pThis->Translation.V[2] * Temp.Rotation[0][2];
	Temp.Translation.V[1] = -pThis->Translation.V[0] * Temp.Rotation[1][0] -
	                        pThis->Translation.V[1] * Temp.Rotation[1][1] -
	                        pThis->Translation.V[2] * Temp.Rotation[1][2];
	Temp.Translation.V[2] = -pThis->Translation.V[0] * Temp.Rotation[2][0] -
	                        pThis->Translation.V[1] * Temp.Rotation[2][1] -
	                        pThis->Translation.V[2] * Temp.Rotation[2][2];

	/* Now simply copy the structure to the target. */
	*pTarget = Temp;
}

/********************************************************************
* Function : Transformation_ConcatenateRotation()
* Purpose : Computes the concatenated rotation of pThis and pOther
*           and stores the result in pTarget. This produces a
*           that's equivalent to first applying pOther and then
*           pThis.
* Pre : pThis and pOther point to initialized Transformation
*       structures, pTarget points to a Transformation structure.
* Post : The rotation matrix of pTarget contains the concatenation of
*        the rotation matrix of pThis and pOther.
********************************************************************/
void Transformation_ConcatenateRotation(struct Transformation *pThis,
													 struct Transformation *pOther,
													 struct Transformation *pTarget)
{
	struct Transformation Temp;
	struct Vector translation;
	
	/* Transformation Concatenation : S = P * Q,
	 * sR = pR * qR
	 */
	
	/* Rotation Concatenation, multiply the rotation matrices. 
	 * sR = pR * qR.
	 */
	Temp.Rotation[0][0] = pThis->Rotation[0][0] * pOther->Rotation[0][0] +
								 pThis->Rotation[0][1] * pOther->Rotation[1][0] +
								 pThis->Rotation[0][2] * pOther->Rotation[2][0];
	Temp.Rotation[1][0] = pThis->Rotation[1][0] * pOther->Rotation[0][0] +
								 pThis->Rotation[1][1] * pOther->Rotation[1][0] +
								 pThis->Rotation[1][2] * pOther->Rotation[2][0];
	Temp.Rotation[2][0] = pThis->Rotation[2][0] * pOther->Rotation[0][0] +
								 pThis->Rotation[2][1] * pOther->Rotation[1][0] +
								 pThis->Rotation[2][2] * pOther->Rotation[2][0];

	Temp.Rotation[0][1] = pThis->Rotation[0][0] * pOther->Rotation[0][1] +
								 pThis->Rotation[0][1] * pOther->Rotation[1][1] +
								 pThis->Rotation[0][2] * pOther->Rotation[2][1];
	Temp.Rotation[1][1] = pThis->Rotation[1][0] * pOther->Rotation[0][1] +
								 pThis->Rotation[1][1] * pOther->Rotation[1][1] +
								 pThis->Rotation[1][2] * pOther->Rotation[2][1];
	Temp.Rotation[2][1] = pThis->Rotation[2][0] * pOther->Rotation[0][1] +
								 pThis->Rotation[2][1] * pOther->Rotation[1][1] +
								 pThis->Rotation[2][2] * pOther->Rotation[2][1];

	Temp.Rotation[0][2] = pThis->Rotation[0][0] * pOther->Rotation[0][2] +
								 pThis->Rotation[0][1] * pOther->Rotation[1][2] +
								 pThis->Rotation[0][2] * pOther->Rotation[2][2];
	Temp.Rotation[1][2] = pThis->Rotation[1][0] * pOther->Rotation[0][2] +
								 pThis->Rotation[1][1] * pOther->Rotation[1][2] +
								 pThis->Rotation[1][2] * pOther->Rotation[2][2];
	Temp.Rotation[2][2] = pThis->Rotation[2][0] * pOther->Rotation[0][2] +
								 pThis->Rotation[2][1] * pOther->Rotation[1][2] +
								 pThis->Rotation[2][2] * pOther->Rotation[2][2];

	/* Copy the structure to the target. */
	/* Preserve translation. */
	translation = pTarget->Translation;
	*pTarget = Temp;
	pTarget->Translation = translation;
}

/********************************************************************
* Function : Transformation_Concatenate()
* Purpose : Computes the concatenated transformation of pThis and
*           pOther and puts the result in pTarget. This produces a
*           transformation which is equivalent to first applying
*           pOther and then pThis.
* Pre : pThis and pOther point to initialized Transformation
*       structures, pTarget points to a Transformation structure.
*       pTarget may be identical to pThis or pOther.
* Post : pTarget contains the concatenation of Transformations pThis
*        and pOther.
********************************************************************/
void Transformation_Concatenate(struct Transformation *pThis,
                                struct Transformation *pOther,
                                struct Transformation *pTarget)
{
	struct Transformation Temp;
	
	/* Transformation Concatenation : S = P * Q,
	 * sR = pR * qR
	 * sT = pT + pR * qT.
	 */
	
	/* Rotation Concatenation, multiply the rotation matrices. 
	 * sR = pR * qR.
	 */
	Temp.Rotation[0][0] = pThis->Rotation[0][0] * pOther->Rotation[0][0] +
								 pThis->Rotation[0][1] * pOther->Rotation[1][0] +
								 pThis->Rotation[0][2] * pOther->Rotation[2][0];
	Temp.Rotation[1][0] = pThis->Rotation[1][0] * pOther->Rotation[0][0] +
								 pThis->Rotation[1][1] * pOther->Rotation[1][0] +
								 pThis->Rotation[1][2] * pOther->Rotation[2][0];
	Temp.Rotation[2][0] = pThis->Rotation[2][0] * pOther->Rotation[0][0] +
								 pThis->Rotation[2][1] * pOther->Rotation[1][0] +
								 pThis->Rotation[2][2] * pOther->Rotation[2][0];

	Temp.Rotation[0][1] = pThis->Rotation[0][0] * pOther->Rotation[0][1] +
								 pThis->Rotation[0][1] * pOther->Rotation[1][1] +
								 pThis->Rotation[0][2] * pOther->Rotation[2][1];
	Temp.Rotation[1][1] = pThis->Rotation[1][0] * pOther->Rotation[0][1] +
								 pThis->Rotation[1][1] * pOther->Rotation[1][1] +
								 pThis->Rotation[1][2] * pOther->Rotation[2][1];
	Temp.Rotation[2][1] = pThis->Rotation[2][0] * pOther->Rotation[0][1] +
								 pThis->Rotation[2][1] * pOther->Rotation[1][1] +
								 pThis->Rotation[2][2] * pOther->Rotation[2][1];

	Temp.Rotation[0][2] = pThis->Rotation[0][0] * pOther->Rotation[0][2] +
								 pThis->Rotation[0][1] * pOther->Rotation[1][2] +
								 pThis->Rotation[0][2] * pOther->Rotation[2][2];
	Temp.Rotation[1][2] = pThis->Rotation[1][0] * pOther->Rotation[0][2] +
								 pThis->Rotation[1][1] * pOther->Rotation[1][2] +
								 pThis->Rotation[1][2] * pOther->Rotation[2][2];
	Temp.Rotation[2][2] = pThis->Rotation[2][0] * pOther->Rotation[0][2] +
								 pThis->Rotation[2][1] * pOther->Rotation[1][2] +
								 pThis->Rotation[2][2] * pOther->Rotation[2][2];

	/* New translation vector
	 * sT = pT + pR * qT.
	 */
	Temp.Translation.V[0] = pThis->Translation.V[0] +
	                        pThis->Rotation[0][0] * pOther->Translation.V[0] +
	                        pThis->Rotation[0][1] * pOther->Translation.V[1] +
	                        pThis->Rotation[0][2] * pOther->Translation.V[2];

	Temp.Translation.V[1] = pThis->Translation.V[1] +
	                        pThis->Rotation[1][0] * pOther->Translation.V[0] +
	                        pThis->Rotation[1][1] * pOther->Translation.V[1] +
	                        pThis->Rotation[1][2] * pOther->Translation.V[2];

	Temp.Translation.V[2] = pThis->Translation.V[2] +
	                        pThis->Rotation[2][0] * pOther->Translation.V[0] +
	                        pThis->Rotation[2][1] * pOther->Translation.V[1] +
	                        pThis->Rotation[2][2] * pOther->Translation.V[2];

	/* Copy the structure to the target. */
	*pTarget = Temp;
}

/********************************************************************
* Function : Transformation_Transform()
* Purpose : Transforms a single vector through Transformation pThis.
* Pre : pThis points to an initialized Transformation structure,
*       pSource points to an initialized Vector structure,
*       pTarget points to a Vector structure. pTarget may be
*       identical to pSource.
* Post : pTarget contains the transformed pSource vector.
********************************************************************/
void Transformation_Transform(struct Transformation *pThis,
                              struct Vector *pSource,
                              struct Vector *pTarget)
{
	/* Multiply pSource with the rotation matrix and add pThis' translation
	 * vector. */
	struct Vector Temp;
/**	Temp.V[0] = pThis->Translation.V[0] +
*	            pThis->Rotation[0][0] * pSource->V[0] +
*	            pThis->Rotation[0][1] * pSource->V[1] +
*	            pThis->Rotation[0][2] * pSource->V[2];
*
*	Temp.V[1] = pThis->Translation.V[1] +
*	            pThis->Rotation[1][0] * pSource->V[0] +
*	            pThis->Rotation[1][1] * pSource->V[1] +
*	            pThis->Rotation[1][2] * pSource->V[2];
*
*	Temp.V[2] = pThis->Translation.V[2] +
*	            pThis->Rotation[2][0] * pSource->V[0] +
*	            pThis->Rotation[2][1] * pSource->V[1] +
*	            pThis->Rotation[2][2] * pSource->V[2];
************/
	Transformation_TransformM( pThis, pSource, &Temp );
	*pTarget = Temp;
}

/********************************************************************
* Function : Transformation_InvTransform()
* Purpose : Transforms Vector pSource through the inverse of pThis
*           and stores the result in pTarget. This is like inverting
*           pThis and then calling Transformation_Transform().
* Pre : pThis points to an initialized Transformation structure,
*       pSource points to an initialized Vector structure,
*       pTarget points to a Vector structure.
* Post : pTarget now contains pSource transformed by the inverse of
*        transformation pThis.
********************************************************************/
void Transformation_InvTransform(struct Transformation *pThis,
											struct Vector *pSource,
											struct Vector *pTarget)
{
	/* Subtract pThis's translation vector and multiply by the transpose
	 * (=inverse) of the rotation matrix. */
	struct Vector Temp, Temp2;
	Temp.V[0] = pSource->V[0] - pThis->Translation.V[0];
	Temp.V[1] = pSource->V[1] - pThis->Translation.V[1];
	Temp.V[2] = pSource->V[2] - pThis->Translation.V[2];
	
	Temp2.V[0] = pThis->Rotation[0][0] * Temp.V[0] +
					 pThis->Rotation[1][0] * Temp.V[1] +
					 pThis->Rotation[2][0] * Temp.V[2];
	Temp2.V[1] = pThis->Rotation[0][1] * Temp.V[0] +
					 pThis->Rotation[1][1] * Temp.V[1] +
					 pThis->Rotation[2][1] * Temp.V[2];
	Temp2.V[2] = pThis->Rotation[0][2] * Temp.V[0] +
					 pThis->Rotation[1][2] * Temp.V[1] +
					 pThis->Rotation[2][2] * Temp.V[2];
	*pTarget = Temp2;
}
	
/********************************************************************
* Function : Transformation_ScaleXYRow()
* Purpose : Scales the X and Y row of transformation pThis by
*           fScaleX and fScaleY respectively. This should not be used
*           on Transformations used in the 3D hierarchy because it
*           disables the Inverse function from working properly.
* Pre : pThis points to an initialized Transformation structure,
*       fScaleX and fScaleY are the scaling factors for resp. the X
*       and Y row of the rotation matrix.
* Post : The rotation matrix of pThis has rescaled X and Y rows
*        according to fScaleX and fScaleY.
********************************************************************/
void Transformation_ScaleXYRow(struct Transformation *pThis,
										 float fScaleX, float fScaleY)
{	/* Call the macro version. */
	Transformation_ScaleXYRowM(pThis, fScaleX, fScaleY);
}

/********************************************************************
* Function : Transformation_TransformPlane()
* Purpose : Rotates a plane through Transformation pThis. Use this
*           for walking and surface routines.
* Pre : pThis points to an initialized Transformation structure,
*       pSource points to an initialized Plane structure,
*       pTarget points to a Plane structure. pTarget may be identical
*       to pSource.
* Post : pTarget contains the transformed pSource plane.
********************************************************************/
void Transformation_TransformPlane(struct Transformation *pThis,
                                   struct Plane *pSource,
                                   struct Plane *pTarget)
{
	/* For the Plane Transformation : P = T.Q
	 * whereby P is the transformed Plane,
	 *         T is the Transformation and
	 *         Q is the original Plane.
	 * The new normal vector Pnormal is :
	 * Pnormal = Trotate.Qnormal
	 * The new distance Pdistance is :
	 * Pdistance = Qdistance + Ttranslate.Pnormal
	 */

	struct Plane Temp;
	
	/* Compute the new normal. */
	Transformation_Rotate(pThis, &(pSource->Normal), &(Temp.Normal));
	
	/* Compute the new distance. */
	Temp.Distance = pSource->Distance +
	                pThis->Translation.V[0] * pSource->Normal.V[0] +
	                pThis->Translation.V[1] * pSource->Normal.V[1] +
	                pThis->Translation.V[2] * pSource->Normal.V[2];
	
	/* Copy the result. */
	*pTarget = Temp;
}

/********************************************************************
* Function : Transformation_InvTransformPlane()
* Purpose : Transforms a plane through the inverse of Transformation
*				pThis. This is like first inverting the transformation
*           and then transforming the plane.
*           Use this for walking and surface routines.
* Pre : pThis points to an initialized Transformation structure,
*       pSource points to an initialized Plane structure,
*       pTarget points to a Plane structure. pTarget may be identical
*       to pSource.
* Post : pTarget contains the transformed pSource plane.
********************************************************************/
void Transformation_InvTransformPlane(struct Transformation *pThis,
                                   struct Plane *pSource,
                                   struct Plane *pTarget)
{
	/* For the Plane Transformation : P = T.Q
	 * whereby P is the transformed Plane, (given)
	 *         T is the Transformation (given) and
	 *         Q is the original Plane. (required)
	 * We want to have Q, the original plane. These formula's have
	 * been derived from the normal plane transformation in
	 * Transformation_TransformPlane().
	 * The original distance Qdistance is :
	 * Qdistance = Pdistance - Ttranslate.Pnormal
	 * The original normal vector Qnormal is :
	 * Qnormal = TinvRotate.Pnormal
	 */

	struct Plane Temp;
	
	/* Compute the original distance. */
	Temp.Distance = pSource->Distance -
	                pThis->Translation.V[0] * pSource->Normal.V[0] -
	                pThis->Translation.V[1] * pSource->Normal.V[1] -
	                pThis->Translation.V[2] * pSource->Normal.V[2];

	/* Compute the new normal. */
	Transformation_InvRotate(pThis, &(pSource->Normal), &(Temp.Normal));
	
	/* Copy the result. */
	*pTarget = Temp;
}

/********************************************************************
* Function : Transformation_Rotate()
* Purpose : Rotates a single vector through Transformation pThis. Use
*           this for transforming direction vectors.
* Pre : pThis points to an initialized Transformation structure,
*       pSource points to an initialized Vector structure,
*       pTarget points to a Vector structure. pTarget may be
*       identical to pSource.
* Post : pTarget contains the rotated pSource vector.
********************************************************************/
void Transformation_Rotate(struct Transformation *pThis,
                           struct Vector *pSource,
                           struct Vector *pTarget)
{
	/* Multiply pSource with the rotation matrix. */
	struct Vector Temp;
	Temp.V[0] = pThis->Rotation[0][0] * pSource->V[0] +
	            pThis->Rotation[0][1] * pSource->V[1] +
	            pThis->Rotation[0][2] * pSource->V[2];

	Temp.V[1] = pThis->Rotation[1][0] * pSource->V[0] +
	            pThis->Rotation[1][1] * pSource->V[1] +
	            pThis->Rotation[1][2] * pSource->V[2];

	Temp.V[2] = pThis->Rotation[2][0] * pSource->V[0] +
	            pThis->Rotation[2][1] * pSource->V[1] +
	            pThis->Rotation[2][2] * pSource->V[2];

	*pTarget = Temp;
}

/********************************************************************
* Function : Transformation_InvRotate()
* Purpose : Rotates a single vector through the inverse of
*           Transformation pThis. This is like first inverting the
*           Transformation and then rotating the vector.
*           Use this for transforming direction vectors.
* Pre : pThis points to an initialized Transformation structure,
*       pSource points to an initialized Vector structure,
*       pTarget points to a Vector structure. pTarget may be
*       identical to pSource.
* Post : pTarget contains the rotated pSource vector.
********************************************************************/
void Transformation_InvRotate(struct Transformation *pThis,
                        	   struct Vector *pSource,
                        	   struct Vector *pTarget)
{
	/* Multiply pSource with the transpose rotation matrix. */
	struct Vector Temp;
	Temp.V[0] = pThis->Rotation[0][0] * pSource->V[0] +
	            pThis->Rotation[1][0] * pSource->V[1] +
	            pThis->Rotation[2][0] * pSource->V[2];

	Temp.V[1] = pThis->Rotation[0][1] * pSource->V[0] +
	            pThis->Rotation[1][1] * pSource->V[1] +
	            pThis->Rotation[2][1] * pSource->V[2];

	Temp.V[2] = pThis->Rotation[0][2] * pSource->V[0] +
	            pThis->Rotation[1][2] * pSource->V[1] +
	            pThis->Rotation[2][2] * pSource->V[2];

	*pTarget = Temp;
}

/********************************************************************
* Function : Transformation_ForceRotationXY()
* Purpose : Converts an existing transformation and forces it to
*           become a rotation. It does this by using two outproducts,
*           the first calculates the Z axis from X and Y, the second
*           calculates the Y axis from the Z and X axes.
*           The X axis is always contained (except it's length which
*           is normalized), the Y axis is used to calculate the Z
*           axis. The new Z axis is then used to calculate the Y.
*           Therefore, the Z axis contents are ignored.
* Pre : pThis points to an initialized Transformation structure.
* Post : pThis points to an initialized Transformation structure that
*        contains a valid 3D transformation using only Rotation and
*        Translation.
********************************************************************/
void Transformation_ForceRotationXY(struct Transformation *pThis)
{
	float fLen;

	/* Normalize the X vector (which is the first column). */
	fLen = (float)sqrt(pThis->Rotation[0][0] * pThis->Rotation[0][0] +
							 pThis->Rotation[1][0] * pThis->Rotation[1][0] +
							 pThis->Rotation[2][0] * pThis->Rotation[2][0]);

	if (fLen == 0.f)
	{	/* We have a problem, a Zero in the length IS a problem.
		 * To solve this problem, fill the X in as (1,0,0) (we MUST
		 * have a rotation!). */
		pThis->Rotation[0][0] = 1.f;
		/* The other floats are already zero (otherwise we wouldn't have
		 * a zero length). */
	} else
	{	pThis->Rotation[0][0] /= fLen;
		pThis->Rotation[1][0] /= fLen;
		pThis->Rotation[2][0] /= fLen;
	}
	
	/* Calculate the outproduct of the X and Y vectors, producing the
	 * unnormalized Z vector. */
	pThis->Rotation[0][2] = pThis->Rotation[1][0] * pThis->Rotation[2][1] -
									pThis->Rotation[2][0] * pThis->Rotation[1][1];
	pThis->Rotation[1][2] = pThis->Rotation[2][0] * pThis->Rotation[0][1] -
									pThis->Rotation[0][0] * pThis->Rotation[2][1];
	pThis->Rotation[2][2] = pThis->Rotation[0][0] * pThis->Rotation[1][1] -
									pThis->Rotation[1][0] * pThis->Rotation[0][1];
	
	/* Normalize the new Z vector. */
	fLen = (float)sqrt(pThis->Rotation[0][2] * pThis->Rotation[0][2] +
							 pThis->Rotation[1][2] * pThis->Rotation[1][2] +
							 pThis->Rotation[2][2] * pThis->Rotation[2][2]);
	
	if (fLen == 0.f)
	{	/* The X and Y axes where the same vector, this caused the
		 * outproduct to become (0,0,0). Make the Z vector (0,0,1)
		 * so we have a valid rotation. */
		pThis->Rotation[2][2] = 1.f;
		/* The other floats are already zero (otherwise we wouldn't
		 * have a zero length. */
	} else
	{	pThis->Rotation[0][2] /= fLen;
		pThis->Rotation[1][2] /= fLen;
		pThis->Rotation[2][2] /= fLen;
	}
	
	/* Reproduce the Y vector. This guarantees that the resulting
	 * transformation is orthogonal. 
	 * The Y vector is Z x Y.
	 * Because X and Z are defined to be of unit length, Y should
	 * also be of unit length automatically. */
	pThis->Rotation[0][1] = pThis->Rotation[1][2] * pThis->Rotation[2][0] -
									pThis->Rotation[2][2] * pThis->Rotation[1][0];
	pThis->Rotation[1][1] = pThis->Rotation[2][2] * pThis->Rotation[0][0] -
									pThis->Rotation[0][2] * pThis->Rotation[2][0];
	pThis->Rotation[2][1] = pThis->Rotation[0][2] * pThis->Rotation[1][0] -
									pThis->Rotation[1][2] * pThis->Rotation[0][0];
}

/********************************************************************
* Function : Transformation_ForceRotationZY()
* Purpose : Converts an existing transformation and forces it to
*           become a rotation. It does this by using two outproducts,
*           the first calculates the X axis from Z and Y, the second
*           calculates the Y axis from the Z and X axes.
*           The Z axis is always contained (except it's length which
*           is normalized), the Y axis is used to calculate the X
*           axis. The new X axis is then used to calculate the Y.
*           Therefore, the Z axis contents are ignored.
* Pre : pThis points to an initialized Transformation structure.
* Post : pThis points to an initialized Transformation structure that
*        contains a valid 3D transformation using only Rotation and
*        Translation.
********************************************************************/
void Transformation_ForceRotationZY(struct Transformation *pThis)
{
	float fLen;

	/* Normalize the Z vector (which is the third column). */
	fLen = (float)sqrt(pThis->Rotation[0][2] * pThis->Rotation[0][2] +
							 pThis->Rotation[1][2] * pThis->Rotation[1][2] +
							 pThis->Rotation[2][2] * pThis->Rotation[2][2]);

	if (fLen == 0.f)
	{	/* We have a problem, a Zero in the length IS a problem.
		 * To solve this problem, fill the Z in as (0,0,1) (we MUST
		 * have a rotation!). */
		pThis->Rotation[2][2] = 1.f;
		/* The other floats are already zero (otherwise we wouldn't have
		 * a zero length). */
	} else
	{	pThis->Rotation[0][2] /= fLen;
		pThis->Rotation[1][2] /= fLen;
		pThis->Rotation[2][2] /= fLen;
	}
	
	/* Calculate the outproduct of the Y and Z vectors, producing the
	 * unnormalized X vector. */
	pThis->Rotation[0][0] = pThis->Rotation[1][1] * pThis->Rotation[2][2] -
									pThis->Rotation[2][1] * pThis->Rotation[1][2];
	pThis->Rotation[1][0] = pThis->Rotation[2][1] * pThis->Rotation[0][2] -
									pThis->Rotation[0][1] * pThis->Rotation[2][2];
	pThis->Rotation[2][0] = pThis->Rotation[0][1] * pThis->Rotation[1][2] -
									pThis->Rotation[1][1] * pThis->Rotation[0][2];
	
	/* Normalize the new X vector. */
	fLen = (float)sqrt(pThis->Rotation[0][0] * pThis->Rotation[0][0] +
							 pThis->Rotation[1][0] * pThis->Rotation[1][0] +
							 pThis->Rotation[2][0] * pThis->Rotation[2][0]);
	
	if (fLen == 0.f)
	{	/* The Y and Z axes where the same vector, this caused the
		 * outproduct to become (0,0,0). Make the X vector (1,0,0)
		 * so we have a valid rotation. */
		pThis->Rotation[0][0] = 1.f;
		/* The other floats are already zero (otherwise we wouldn't
		 * have a zero length. */
	} else
	{	pThis->Rotation[0][0] /= fLen;
		pThis->Rotation[1][0] /= fLen;
		pThis->Rotation[2][0] /= fLen;
	}
	
	/* Reproduce the Y vector. This guarantees that the resulting
	 * transformation is orthogonal. 
	 * The Y vector is Z x Y.
	 * Because X and Z are defined to be of unit length, Y should
	 * also be of unit length automatically. */
	pThis->Rotation[0][1] = pThis->Rotation[1][2] * pThis->Rotation[2][0] -
									pThis->Rotation[2][2] * pThis->Rotation[1][0];
	pThis->Rotation[1][1] = pThis->Rotation[2][2] * pThis->Rotation[0][0] -
									pThis->Rotation[0][2] * pThis->Rotation[2][0];
	pThis->Rotation[2][1] = pThis->Rotation[0][2] * pThis->Rotation[1][0] -
									pThis->Rotation[1][2] * pThis->Rotation[0][0];
}

/********************************************************************
* Function : Transformation_MakeRotateArbitrary()
* Purpose : Fills the rotation matrix of pThis as a rotation around
*				an axis pAxis with an angle of fAngle.
* Pre : pThis points to an initialized Transformation structure,
*       pAxis points to an initialized Vector structure containing
*       the axis for rotation as a normalized vector along the axis,
*       fAngle specifies the angle of the rotation in radians.
* Post : The rotation matrix of pThis has been replaced by a
*        rotation matrix that contains the requested rotation.
********************************************************************/
void Transformation_MakeRotateArbitrary(struct Transformation *pThis,
													 struct Vector *pAxis,
													 float fAngle)
{
	float	halfTheta;
	float cosHalfTheta;
	float sinHalfTheta;
	float xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;
	float qx, qy, qz, qw;

	halfTheta = 0.5f * fAngle;
	cosHalfTheta = (float)cos(halfTheta);
	sinHalfTheta = (float)sin(halfTheta);

	qx = pAxis->V[0] * sinHalfTheta;
	qy = pAxis->V[1] * sinHalfTheta;
	qz = pAxis->V[2] * sinHalfTheta;
	qw = cosHalfTheta;

	xs = 2.f * qx;	ys = 2.f * qy;	zs = 2.f * qz;
	wx = qw * xs;	wy = qw * ys;	wz = qw * zs;
	xx = qx * xs;	xy = qx * ys;	xz = qx * zs;
	yy = qy * ys;	yz = qy * zs;	zz = qz * zs;

	pThis->Rotation[0][0] = 1.f - (yy + zz);
	pThis->Rotation[0][1] = xy - wz;
	pThis->Rotation[0][2] = xz + wy;

	pThis->Rotation[1][0] = xy + wz;
	pThis->Rotation[1][1] = 1.f - (xx + zz);
	pThis->Rotation[1][2] = yz - wx;

	pThis->Rotation[2][0] = xz - wy;
	pThis->Rotation[2][1] = yz + wx;
	pThis->Rotation[2][2] = 1.f - (xx + yy);

	/* Done. */
}

