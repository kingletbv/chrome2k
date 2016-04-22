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
* FILE : frame.c
********************************************************************/

#include <stdlib.h>	/* For NULL Constant */
#include "frame.h"

#include "trans.h"

/********************************************************************
* Function : Frame_Construct()
* Purpose : Initializes the frame structure pThis.
* Pre : pThis points to a Frame structure.
* Post : pThis points to a Frame structure with a NO-OP
*        transformation and directly connected to the Root Frame.
********************************************************************/
void Frame_Construct(struct Frame *pThis)
{
	/* Call macro version. */
	Frame_ConstructM(pThis);
}	
/********************************************************************
* Function : Frame_GetTransformationToRoot()
* Purpose : Builds a transformation from Frame pThis to the root
*           Frame.
* Pre : pThis points to an initialized Frame structure, pTarget
*       points to a Transformation structure.
* Post : pTarget contains the transformation needed to go from Frame
*        pThis to the root Frame.
********************************************************************/
void Frame_GetTransformationToRoot(struct Frame *pThis,
                                   struct Transformation *pTarget)
{
	/* Concatenate all transformations from pThis to the root. */
	struct Frame *pCurrent;
	
	/* Check for Root Frame. */
	if (pThis == NULL)
	{	/* Just initialize pTarget. */
		Transformation_Construct(pTarget);
	} else
	{	/* Get the first transformation. */
		*pTarget = pThis->TransformationToParent;
		
		/* And start at pThis' parent for concatenation. */
		pCurrent = pThis->pParent;
		
		/* Loop until we've reached the parent. */
		while (pCurrent != NULL)
		{
			/* Concatenate the transformation of the current Frame to that
			 * of it's parent with the transformation we've build up. 
			 * Store the result in pTarget (accumulate the result). */
			Transformation_Concatenate(&(pCurrent->TransformationToParent),
			                           pTarget,
			                           pTarget);
			/* And jump one level higher. */
			pCurrent = pCurrent->pParent;
		}
	}
}
	
/********************************************************************
* Function : Frame_GetTransformationFromRoot()
* Purpose : Builds a transformation from the Root Frame to frame
*           pThis.
* Pre : pThis points to an initialized Frame structure, pTarget
*       points to a Transformation structure.
* Post : pTarget points to the transformation from the Root Frame
*        to Frame pThis.
********************************************************************/
void Frame_GetTransformationFromRoot(struct Frame *pThis,
                                     struct Transformation *pTarget)
{
	/* Compute the transformation from pThis to the Root and use the
	 * inverse. */

	/* Check for Root Frame. */
	if (pThis == NULL)
	{	/* Just initialize pTarget. */
		Transformation_Construct(pTarget);
	} else
	{	Frame_GetTransformationToRoot(pThis, pTarget);
		Transformation_Inverse(pTarget, pTarget);
	}
}
	
/********************************************************************
* Function : Frame_GetTransformationToFrame()
* Purpose : Builds a transformation from Frame pThis to pOther.
* Pre : pThis and pOther point to initialized Frame structures.
*       pTarget points to a Transformation structure
* Post : pTarget points to the transformation from Frame pThis to
*        pOther.
********************************************************************/
void Frame_GetTransformationToFrame(struct Frame *pThis,
                                    struct Frame *pOther,
                                    struct Transformation *pTarget)
{
	/* Build the transformation to the root for Frame pThis,
	 * build the transformation from the root for Frame pOther and
	 * concatenate these two. */
	struct Transformation TransThisToRoot;
	struct Transformation TransOtherFromRoot;
	
	Frame_GetTransformationToRoot(pThis, &TransThisToRoot);
	Frame_GetTransformationFromRoot(pOther, &TransOtherFromRoot);
	
	Transformation_Concatenate(&TransOtherFromRoot, &TransThisToRoot,
	                           pTarget);
}

/********************************************************************
* Function : Frame_RelocateFrame()
* Purpose : Reattaches Frame pThis to it's new parent pNewParent
*           without changing it's position or orientation relative
*           to the Root Frame.
* Pre : pThis and pNewParent point to initialized Frame structures.
* Post : pThis has been relocated below pNewParent. Consequently the
*        pThis->pParent pointer now points to pNewParent. The
*        Transformation contained in pThis has been altered so that
*        the actual position and orientation of pThis have not
*        changed.
********************************************************************/
void Frame_RelocateFrame(struct Frame *pThis,
                         struct Frame *pNewParent)
{
	/* Get the Transformation from pThis to pNewParent and use that
	 * as the new transformation for pThis. */
	struct Transformation Temp;
	
	Frame_GetTransformationToFrame(pThis, pNewParent, &Temp);
	
	pThis->TransformationToParent = Temp;
	
	pThis->pParent = pNewParent;
}
