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
* FILE : octree.h
* Purpose : Header file for the Octree and OcNode structures.
* Description : The Octree structure is used for color quantization.
*               It allows requesting colors in 24 bit and receiving
*               the colors in 8 bit (colormapped). Thus we can treat
*               colors as 24 bit values while drawing to an 8 bit 
*               screen.
********************************************************************/

#ifndef OCTREE_H
#define OCTREE_H

struct OcNode
{
	struct OcNode	*pNextEqual;	/* Next OcNode in same level. */

	struct OcNode	*prgb;		/* Children of this node,		*/
	struct OcNode	*prgB;		/* small axis letter denotes	*/
	struct OcNode	*prGb;		/* a component < 0.5, large	*/
	struct OcNode	*prGB;		/* letter denotes a component	*/
	struct OcNode	*pRgb;		/* >= 0.5.							*/
	struct OcNode	*pRgB;
	struct OcNode	*pRGb;
	struct OcNode	*pRGB;

	int	nRed, nGreen, nBlue;	/* RGB color contained in node. */
	int	nCount;					/* Number of RGB colors summed
										 * together. */
	int	nChildColCount;		/* Number of RGB colors stored in this
										 * node's children. */
	int	nLevel;					/* Level in Octree of this node, can
										 * range from 0 to 7. */
	int	nColorIndex;			/* Index in colormap of this color.
										 * This is only valid when this node is
										 * a leaf. */
};

/* OcNode_ConstructM(pThis), (NEEDS stdlib.h INCLUDED)
 * Initializes an OcNode structure. */
#define OcNode_ConstructM(pThis)\
(	(pThis)->pNextEqual = NULL,\
	(pThis)->prgb = NULL,\
	(pThis)->prgB = NULL,\
	(pThis)->prGb = NULL,\
	(pThis)->prGB = NULL,\
	(pThis)->pRgb = NULL,\
	(pThis)->pRgB = NULL,\
	(pThis)->pRGb = NULL,\
	(pThis)->pRGB = NULL,\
	(pThis)->nRed = 0,\
	(pThis)->nGreen = 0,\
	(pThis)->nBlue = 0,\
	(pThis)->nCount = 0,\
	(pThis)->nChildColCount = 0,\
	(pThis)->nLevel = 0,\
	(pThis)->nColorIndex = 0\
)

/* OcNode_IsLeafM(pThis), (NEEDS stdlib.h INCLUDED)
 * Checks if the node is a leaf. Returns true if
 * this is the case, false (0) otherwise.
 * Note : An OcNode is considered a leaf if it has no
 *        children.
 */
#define OcNode_IsLeafM(pThis)\
(\
	((pThis)->prgb == NULL) &&\
	((pThis)->prgB == NULL) &&\
	((pThis)->prGb == NULL) &&\
	((pThis)->prGB == NULL) &&\
	((pThis)->pRgb == NULL) &&\
	((pThis)->pRgB == NULL) &&\
	((pThis)->pRGb == NULL) &&\
	((pThis)->pRGB == NULL)\
)

/* OcNode_SumOcNodeM(pThis, pOther), (NEEDS stdlib.h INCLUDED)
 * Adds the values of one OcNode to another, effectively
 * averaging them. */
#define OcNode_SumOcNodeM(pThis, pOther)\
(\
	(pThis)->nRed += (pOther)->nRed,\
	(pThis)->nGreen += (pOther)->nGreen,\
	(pThis)->nBlue += (pOther)->nBlue,\
	(pThis)->nCount += (pOther)->nCount\
)

/* OcNode_FillColormapRec(pThis, pColormap),
 * Fills pColormap with all leaf colors of Octree pThis.
 * During this operation, indices to the colors are assigned
 * in the octree.
 * Used internally by Octree_FillColormap(). */
int OcNode_FillColormapRec(struct OcNode *pThis,
									unsigned long *pColormap,
									int nColorIndex);

struct Octree
{
	struct OcNode	*pRoot;					/* Root node of octree. */

	struct OcNode	*arpReducableHeads[8];
													/* Pointers to the heads of
													 * the level lists of
													 * reducables. */
	
	int	nLeafCount;							/* Total number of leaves
													 * in the Octree, this is
													 * the number of colors
													 * currently stored in the
													 * Octree. */
	struct OcNode	*pBucket;				/* Unused OcNodes ready for
													 * recycling. (Prevents
													 * mallocs, thus improving
													 * performance). */
	int	nMaxColors;							/* Maximum number of colors
													 * allowed after color
													 * quantization. */
	int	nColorOffset;						/* Base value from which to
													 * assign color indices. */
};


/* Octree_ConstructM(pThis),
 * Initializes an Octree structure. */
#define Octree_ConstructM(pThis)\
(	(pThis)->pRoot = NULL,\
	(pThis)->arpReducableHeads[0] = NULL,\
	(pThis)->arpReducableHeads[1] = NULL,\
	(pThis)->arpReducableHeads[2] = NULL,\
	(pThis)->arpReducableHeads[3] = NULL,\
	(pThis)->arpReducableHeads[4] = NULL,\
	(pThis)->arpReducableHeads[5] = NULL,\
	(pThis)->arpReducableHeads[6] = NULL,\
	(pThis)->arpReducableHeads[7] = NULL,\
	(pThis)->nLeafCount = 0,\
	(pThis)->pBucket = NULL,\
	(pThis)->nMaxColors = 256,\
	(pThis)->nColorOffset = 0\
)

/* Octree_Destruct(pThis),
 * Frees all memory of an Octree. */
void Octree_Destruct(struct Octree *pThis);

/* Octree_InsertColor(pThis, pOcNode, nRed, nGreen, nBlue),
 * Inserts a color into an octree defined by pThis,
 * pOcNode specifies the current node in which to insert
 * the color (nRed, nGreen, nBlue).
 */
int Octree_InsertColorRec(struct Octree *pThis,
								  struct OcNode *pOcNode,
								  int nRed,
								  int nGreen,
								  int nBlue);

/* Octree_GetReducable(pThis),
 * Retrieves the best node for reduction, this is the node
 * deepest in the tree that has 2 or more direct children and that
 * has the least total number of children.
 */
struct OcNode *Octree_GetReducable(struct Octree *pThis);

/* Octree_CollapseNode(pThis, pOcNode),
 * Sums ALL children of pOcNode into pOcNode.
 */
void Octree_CollapseNode(struct Octree *pThis,
                         struct OcNode *pOcNode);

/* Octree_AddColor(pThis, ulRGB),
 * Adds a color to an Octree. This function should be used
 * to add a color to the Octree.
 */
int Octree_AddColor(struct Octree *pThis,
						  unsigned long ulRGB);

/* Octree_FillColormap(pThis, pColormap),
 * Fills colormap pColormap with the RGB triples encoded as
 * unsigned longs in the format 0xRRGGBB. The colors can be
 * looked up be the Octree_FindColorIndex() function.
 * The first color is added at offset pThis->nColorOffset.
 */
void Octree_FillColormap(struct Octree *pThis,
								 unsigned long *pColormap);

/* Octree_FindColorIndex(pThis, ulRGB),
 * Searches the octree pThis for the best color match for ulRGB and
 * returns the index as it would appear in a colormap built by
 * Octree_FillColormap().
 */
int Octree_FindColorIndex(struct Octree *pThis,
								  unsigned long ulRGB);

#endif
