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
* FILE : floatset.c
********************************************************************/

#define FLOATSET_C

#include <stdlib.h>

#include "floatset.h"

/********************************************************************
* Function : FloatSet_Construct()
* Purpose : Initializes a FloatSet.
* Pre : pThis points to a FloatSet structure.
* Post : pThis points to an initialized FloatSet structure containing
*        0 floats.
********************************************************************/
void FloatSet_Construct(struct FloatSet *pThis)
{	/* Just call the macro version. */
	FloatSet_ConstructM(pThis);
}

/********************************************************************
* Function : FloatSet_Destruct()
* Purpose : Frees all memory associated with a FloatSet, does NOT
*           free the structure itself.
* Pre : pThis points to an initialized FloatSet structure.
* Post : pThis points to an invalid FloatSet structure that has no
*        memory allocated.
********************************************************************/
void FloatSet_Destruct(struct FloatSet *pThis)
{	/* Just call the macro version. */
	FloatSet_DestructM(pThis);
}

/********************************************************************
* Function : FloatSet_Expand()
* Purpose : Expands the allocation space in a FloatSet structure.
* Pre : pThis points to an initialized FloatSet structure.
* Post : If the returnvalue is 1, pThis points to an initialized
*        FloatSet structure with EXPAND_SIZE more floats available.
*        If the returnvalue is 0, a memory failure occured.
********************************************************************/
int FloatSet_Expand(struct FloatSet *pThis)
{
	float *p;
	int n;
	
	/* Allocate new space. */
	p = (float *)malloc(sizeof(float) * (pThis->nAlloc + EXPAND_SIZE));
	
	/* Check for a memory failure. */
	if (p != NULL)
	{
		/* Check if there was a previous arFloats array. */
		if (pThis->arFloats != NULL)
		{
			/* Copy the old to the new. */
			for (n = 0; n < pThis->nCount; n++)
				p[n] = pThis->arFloats[n];
			
			/* Free the old. */
			free(pThis->arFloats);
		}
		
		/* Set the new. */
		pThis->arFloats = p;
		pThis->nAlloc += EXPAND_SIZE;
		
		return 1;
	} else
	{	return 0;	/* Memory failure. */
	}
}

/********************************************************************
* Function : FloatSet_Add(),
* Purpose : Adds a new float to a FloatSet.
* Pre : pThis points to an initialized FloatSet structure.
* Post : If the returnvalue is 1, pThis now contains the new float.
*        If the returnvalue is 0, a memory allocation failure
*        occured.
********************************************************************/
int FloatSet_Add(struct FloatSet *pThis, float fFloat)
{	/* Just redirect to the macro version. */
	return FloatSet_AddM(pThis, fFloat);
}
