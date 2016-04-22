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
* FILE : floatset.h
* Purpose : Header file for the FloatSet structure.
* Description : The FloatSet is used for maintaining a collection of
*               float values. It is used for things like building
*               BSP Trees and clipping against planes.
********************************************************************/

#ifndef FLOATSET_H
#define FLOATSET_H

/* Only specify EXPAND_SIZE if this is from the original C file. */
#ifdef FLOATSET_C
#define EXPAND_SIZE 10
#endif

struct FloatSet
{
	int	nAlloc;				/* Number of floats allocated for. */
	int	nCount;				/* Number of floats in use. */
	float	*arFloats;			/* Array containing actual floats. */
};

/* FloatSet_Construct(pThis),
 * FloatSet_ConstructM(pThis),
 * Initializes a FloatSet structure, sets the allocation to 0.
 */
void FloatSet_Construct(struct FloatSet *pThis);
#define FloatSet_ConstructM(pThis)\
(	(pThis)->nAlloc = 0,\
	(pThis)->nCount = 0,\
	(pThis)->arFloats = NULL\
)

/* FloatSet_Destruct(pThis),
 * FloatSet_DestructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Frees all memory associated IN the structure, doesn't free the
 * pointer itself.
 */
void FloatSet_Destruct(struct FloatSet *pThis);
#define FloatSet_DestructM(pThis)\
	if ((pThis)->arFloats != NULL)\
	{	free((pThis)->arFloats);\
	}

/* FloatSet_Expand(pThis),
 * Expands the number of allocated floats in pThis by EXPAND_SIZE.
 * Returns 1 if succesful, 0 otherwise (memory allocation failure).
 */
int FloatSet_Expand(struct FloatSet *pThis);

/* FloatSet_Add(pThis, fFloat),
 * FloatSet_AddM(pThis, fFloat),
 * Adds a new float to the FloatSet structure.
 * Returns 1 if succesful, 0 otherwise (memory allocation failure).
 */
int FloatSet_Add(struct FloatSet *pThis, float fFloat);
#define FloatSet_AddM(pThis, fFloat)\
(	(pThis)->nCount < (pThis)->nAlloc ?\
	(	(pThis)->arFloats[(pThis)->nCount] = (fFloat),\
		((pThis)->nCount)++,\
		1\
	):(\
		FloatSet_Expand(pThis) ?\
		(	(pThis)->arFloats[(pThis)->nCount] = (fFloat),\
			((pThis)->nCount)++,\
			1\
		):(\
			0	/* Mem failure */\
		)\
	)\
)

/* FloatSet_GetFloatM(pThis, nIndex),
 * Retrieves the float at index nIndex.
 * (due to the simplicity of this function, only a macro version is available.)
 */
#define FloatSet_GetFloatM(pThis, nIndex)\
	((pThis)->arFloats[(nIndex)])

/* FloatSet_GetCountM(pThis),
 * Retrieves the number of floats in the set.
 * (due to the simplicity of this function, only a macro version is available.)
 */
#define FloatSet_GetCountM(pThis)\
	((pThis)->nCount)

#endif
