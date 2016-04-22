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
* FILE : octree.c
********************************************************************/

#define OCTREE_C

#include <stdlib.h>

#include "octree.h"


/********************************************************************
* Function : OcNode_FillColormapRec() (INTERNAL)
* Purpose : Fills a colormap by traversing the octree recursively.
* Pre : pThis points to an initialized OcNode structure,
*       pColormap points to the array where the colors should be
*       stored.
*       nColorIndex contains the current index where the next color
*       should be stored.
* Post : pColormap now contains all the colors in the octree
*        defined by pThis. pColormap has also been incremented after
*        these colors, thus pointing to the next undefined position.
*        Returnvalue is the new color index where the next color
*        be stored.
********************************************************************/
int OcNode_FillColormapRec(struct OcNode *pThis,
									unsigned long *pColormap,
									int nColorIndex)
{
	unsigned long ulRGB;
	if (pThis != NULL)
	{
		if (pThis->nCount > 0)
		{
			ulRGB = 0;
			ulRGB |= (pThis->nRed / pThis->nCount) << 16;
			ulRGB |= (pThis->nGreen / pThis->nCount) << 8;
			ulRGB |= (pThis->nBlue / pThis->nCount);
			pThis->nColorIndex = nColorIndex;
			pColormap[nColorIndex++] = ulRGB;
		} else
		{
			if (pThis->prgb != NULL)
				nColorIndex = OcNode_FillColormapRec(pThis->prgb, pColormap, nColorIndex);
			if (pThis->prgB != NULL)
				nColorIndex = OcNode_FillColormapRec(pThis->prgB, pColormap, nColorIndex);
			if (pThis->prGb != NULL)
				nColorIndex = OcNode_FillColormapRec(pThis->prGb, pColormap, nColorIndex);
			if (pThis->prGB != NULL)
				nColorIndex = OcNode_FillColormapRec(pThis->prGB, pColormap, nColorIndex);
			if (pThis->pRgb != NULL)
				nColorIndex = OcNode_FillColormapRec(pThis->pRgb, pColormap, nColorIndex);
			if (pThis->pRgB != NULL)
				nColorIndex = OcNode_FillColormapRec(pThis->pRgB, pColormap, nColorIndex);
			if (pThis->pRGb != NULL)
				nColorIndex = OcNode_FillColormapRec(pThis->pRGb, pColormap, nColorIndex);
			if (pThis->pRGB != NULL)
				nColorIndex = OcNode_FillColormapRec(pThis->pRGB, pColormap, nColorIndex);
		}
	}
	return nColorIndex;
}

/********************************************************************
* Function : OcNode_DestructTree()
* Purpose : Frees an entire octree by it's root node.
* Pre : pThis points to the root node of the octree that is to be
*       deleted.
* Post : pThis and all it's children have been freed.
********************************************************************/
void OcNode_DestructTree(struct OcNode *pThis)
{
	if (pThis != NULL)
	{
		OcNode_DestructTree(pThis->prgb);
		OcNode_DestructTree(pThis->prgB);
		OcNode_DestructTree(pThis->prGb);
		OcNode_DestructTree(pThis->prGB);
		OcNode_DestructTree(pThis->pRgb);
		OcNode_DestructTree(pThis->pRgB);
		OcNode_DestructTree(pThis->pRGb);
		OcNode_DestructTree(pThis->pRGB);

		free((void *)pThis);
	}
}

/********************************************************************
* Function : Octree_Destruct()
* Purpose : Frees all memory of an Octree.
* Pre : pThis points to an initialized Octree structure.
* Post : pThis points to an invalid Octree structure that used no
*        more memory.
********************************************************************/
void Octree_Destruct(struct Octree *pThis)
{
	struct OcNode *pOcNode;
	struct OcNode *pNextOcNode;

	/* Iterate for bucket dumping. */
	pOcNode = pThis->pBucket;
	while (pOcNode != NULL)
	{
		pNextOcNode = pOcNode->pNextEqual;
		free(pOcNode);
		pOcNode = pNextOcNode;
	}

	/* And clean up the octree. */
	OcNode_DestructTree(pThis->pRoot);
}

/********************************************************************
* Function : Octree_InsertColorRec()
* Purpose : Inserts a color into a color Octree structure.
* Pre : pThis points to an initialized Octree structure, pOcNode
*       points to an initialized OcNode structure, nRed, nGreen and
*       nBlue contain the color intensities, ranging from 0 to 255.
* Post : If the returnvalue is 1, the color was succesfully added.
*        If the returnvalue is 0, there was a memory failure.
********************************************************************/
int Octree_InsertColorRec(struct Octree *pThis,
								  struct OcNode *pOcNode,
								  int nRed,
								  int nGreen,
								  int nBlue)
{
	struct OcNode **ppChildNode;
	int	nMask, nCount;

	/* Determine wether the current OcNode
	 * pOcNode is a leaf.
	 * It is a leaf when there are RGB values stored in
	 * it OR if it is on the last level (0..7, -> level 7). */
	if ((pOcNode->nCount != 0) ||
		 (pOcNode->nLevel >= 7))
	{
		/* Check if this is a NEW color to be added. */
		if (pOcNode->nCount == 0)
		{	/* A new color is to be added. */
			/* Increment the leaf count in the Octree structure. */
			pThis->nLeafCount++;
		}

		/* Just add the color. */
		pOcNode->nCount++;
		pOcNode->nRed += nRed;
		pOcNode->nGreen += nGreen;
		pOcNode->nBlue += nBlue;
		return 1;
	} else
	{
		/* Find child to go into. */
		/* Find a mask upon which to base the decision.
		 * This means finding a decision bit.
		 * For level 0 this is bit 7 (128),
		 * For level 1 this is bit 6 (64) etc. */
		nMask = 128;
		nMask = nMask >> pOcNode->nLevel;
		if (nMask & nRed)
		{	/* Big R */
			if (nMask & nGreen)
			{	/* Big G */
				if (nMask & nBlue)
				{	/* Big B */
					ppChildNode = &(pOcNode->pRGB);
				} else
				{	/* Small B */
					ppChildNode = &(pOcNode->pRGb);
				}
			} else
			{	/* Small G */
				if (nMask & nBlue)
				{	/* Big B */
					ppChildNode = &(pOcNode->pRgB);
				} else
				{	/* Small B */
					ppChildNode = &(pOcNode->pRgb);
				}
			}
		} else
		{	/* Small R */
			if (nMask & nGreen)
			{	/* Big G */
				if (nMask & nBlue)
				{	/* Big B */
					ppChildNode = &(pOcNode->prGB);
				} else
				{	/* Small B */
					ppChildNode = &(pOcNode->prGb);
				}
			} else
			{	/* Small G */
				if (nMask & nBlue)
				{	/* Big B */
					ppChildNode = &(pOcNode->prgB);
				} else
				{	/* Small B */
					ppChildNode = &(pOcNode->prgb);
				}
			}
		}

		/* Check if ppChildNode is NULL,
		 * If so, make a new child node. */
		if ((*ppChildNode) == NULL)
		{
			/* Allocate a new childnode. */
			/* Check if we have one in our recycable bucket. */
			if (pThis->pBucket == NULL)
				*ppChildNode = (struct OcNode *)malloc(sizeof(struct OcNode));
			else
			{
				*ppChildNode = pThis->pBucket;
				pThis->pBucket = pThis->pBucket->pNextEqual;
			}
		
			if ((*ppChildNode) != NULL)
			{
				OcNode_ConstructM((*ppChildNode));
				/* Initialize some fields. */
				(*ppChildNode)->nLevel = pOcNode->nLevel + 1;
				/* The childnode is now ready. */

				/* Check if the current OcNode (pOcNode) has become
				 * a reducible (more than 2 children), this happens
				 * when the children count has just become 2. */
				/* Produce a count of non-empty children. */
				nCount = 0;
				if (pOcNode->prgb != NULL) nCount++;
				if (pOcNode->prgB != NULL) nCount++;
				if (pOcNode->prGb != NULL) nCount++;
				if (pOcNode->prGB != NULL) nCount++;
				if (pOcNode->pRgb != NULL) nCount++;
				if (pOcNode->pRgB != NULL) nCount++;
				if (pOcNode->pRGb != NULL) nCount++;
				if (pOcNode->pRGB != NULL) nCount++;

				if (nCount == 2)
				{
					/* Number of children has just become 2,
					 * add it as a reducable. */
					pOcNode->pNextEqual = pThis->arpReducableHeads[pOcNode->nLevel];
					pThis->arpReducableHeads[pOcNode->nLevel] = pOcNode;
				}

			} else
			{	/* Mem failure. */
				return 0;
			}
		}

		/* We're adding a new color to one of pOcNode's children so increment
		 * the child color count in pOcNode. */
		pOcNode->nChildColCount++;

		/* Call recursively for adding to child. */
		return Octree_InsertColorRec(pThis, *ppChildNode, nRed, nGreen, nBlue);
	}
}

/********************************************************************
* Function : Octree_GetReducable()
* Purpose : Finds the best node for reduction, does this primarily
*           on depth in tree (deeper is better), secondarily on the
*           number of colors stored in the children of the octree.
* Pre : pThis points to an initialized Octree structure.
* Post : Returnvalue is a pointer to the OcNode structure that is 
*        the best reducable or NULL if there were no reducables. (The
*        latter occurs when there are no more than one distinct
*        colors in the octree).
********************************************************************/
struct OcNode *Octree_GetReducable(struct Octree *pThis)
{
	struct OcNode *pOcNode;
	struct OcNode *pTargetNode;
	int n;

	pTargetNode = NULL;
	n = 7;
	while (n >= 0)
	{
		/* Find head of linked list at current level. */
		pOcNode = pThis->arpReducableHeads[n];

		/* Iterate linked list of reducables. */
		while (pOcNode != NULL)
		{
			if (pTargetNode == NULL)
			{	pTargetNode = pOcNode;
			} else
			if (pTargetNode->nChildColCount > pOcNode->nChildColCount)
			{	pTargetNode = pOcNode;
			}

			pOcNode = pOcNode->pNextEqual;
		}

		/* Did we find a reducable ? */
		if (pTargetNode != NULL)
			return pTargetNode;	/* Return it. */

		n--;
	}

	return pTargetNode;
}

/********************************************************************
* Function : Octree_CollapseNode()
* Purpose : Sums all children of pOcNode into pOcNode.
* Pre : pThis points to an initialized Octree structure,
*       pOcNode points to an initialized OcNode structure
*       that is part of Octree pThis.
* Post : pOcNode has no more children and has itself become a leaf.
********************************************************************/
void Octree_CollapseNode(struct Octree *pThis,
                         struct OcNode *pOcNode)
{
	struct OcNode **ppOcNode;
	/* Check if pOcNode is not a leaf. */
	if (pOcNode->nCount == 0)
		pThis->nLeafCount++;		/* It will become one. */

	if (pOcNode->prgb != NULL)
	{	Octree_CollapseNode(pThis, pOcNode->prgb);
		OcNode_SumOcNodeM(pOcNode, pOcNode->prgb);
		pOcNode->prgb->pNextEqual = pThis->pBucket;
		pThis->pBucket = pOcNode->prgb;
		pThis->nLeafCount--;
		pOcNode->prgb = NULL;
	}
	if (pOcNode->prgB != NULL)
	{	Octree_CollapseNode(pThis, pOcNode->prgB);
		OcNode_SumOcNodeM(pOcNode, pOcNode->prgB);
		pOcNode->prgB->pNextEqual = pThis->pBucket;
		pThis->pBucket = pOcNode->prgB;
		pThis->nLeafCount--;
		pOcNode->prgB = NULL;
	}
	if (pOcNode->prGb != NULL)
	{	Octree_CollapseNode(pThis, pOcNode->prGb);
		OcNode_SumOcNodeM(pOcNode, pOcNode->prGb);
		pOcNode->prGb->pNextEqual = pThis->pBucket;
		pThis->pBucket = pOcNode->prGb;
		pThis->nLeafCount--;
		pOcNode->prGb = NULL;
	}
	if (pOcNode->prGB != NULL)
	{	Octree_CollapseNode(pThis, pOcNode->prGB);
		OcNode_SumOcNodeM(pOcNode, pOcNode->prGB);
		pOcNode->prGB->pNextEqual = pThis->pBucket;
		pThis->pBucket = pOcNode->prGB;
		pThis->nLeafCount--;
		pOcNode->prGB = NULL;
	}
	if (pOcNode->pRgb != NULL)
	{	Octree_CollapseNode(pThis, pOcNode->pRgb);
		OcNode_SumOcNodeM(pOcNode, pOcNode->pRgb);
		pOcNode->pRgb->pNextEqual = pThis->pBucket;
		pThis->pBucket = pOcNode->pRgb;
		pThis->nLeafCount--;
		pOcNode->pRgb = NULL;
	}
	if (pOcNode->pRgB != NULL)
	{	Octree_CollapseNode(pThis, pOcNode->pRgB);
		OcNode_SumOcNodeM(pOcNode, pOcNode->pRgB);
		pOcNode->pRgB->pNextEqual = pThis->pBucket;
		pThis->pBucket = pOcNode->pRgB;
		pThis->nLeafCount--;
		pOcNode->pRgB = NULL;
	}
	if (pOcNode->pRGb != NULL)
	{	Octree_CollapseNode(pThis, pOcNode->pRGb);
		OcNode_SumOcNodeM(pOcNode, pOcNode->pRGb);
		pOcNode->pRGb->pNextEqual = pThis->pBucket;
		pThis->pBucket = pOcNode->pRGb;
		pThis->nLeafCount--;
		pOcNode->pRGb = NULL;
	}
	if (pOcNode->pRGB != NULL)
	{	Octree_CollapseNode(pThis, pOcNode->pRGB);
		OcNode_SumOcNodeM(pOcNode, pOcNode->pRGB);
		pOcNode->pRGB->pNextEqual = pThis->pBucket;
		pThis->pBucket = pOcNode->pRGB;
		pThis->nLeafCount--;
		pOcNode->pRGB = NULL;
	}

	/* Remove pOcNode as a reducable. */
	ppOcNode = &(pThis->arpReducableHeads[pOcNode->nLevel]);
	while (*ppOcNode != NULL)
	{
		if (*ppOcNode == pOcNode)
		{
			*ppOcNode = pOcNode->pNextEqual;
			return;
		}
		ppOcNode = &((*ppOcNode)->pNextEqual);
	}
}

/********************************************************************
* Function : Octree_AddColor()
* Purpose : Function for adding a color to an Octree.
* Pre : pThis points to an initialized Octree structure, ulRGB
*       contains the color to be added in 0xRRGGBB format.
*       nMaxColors defines the maximum number of distinct colors
*       allowed after quantization.
* Post : If the returnvalue is 1, pThis contains the color.
*        If the returnvalue is 0, a memory allocation failure
*			occured.
********************************************************************/
int Octree_AddColor(struct Octree *pThis,
						  unsigned long ulRGB)
{
	int nRed, nGreen, nBlue;
	struct OcNode *pOcNode;

	/* Check if current colorcount exceeds max.
	 * Reduce colors if so. */
	while (pThis->nLeafCount >= pThis->nMaxColors)
	{	pOcNode = Octree_GetReducable(pThis);
		if (pOcNode != NULL)
		{	Octree_CollapseNode(pThis, pOcNode);
		}
	}

	nRed = (ulRGB & 0xFF0000) >> 16;
	nGreen = (ulRGB & 0xFF00) >> 8;
	nBlue = (ulRGB & 0xFF);

	if (pThis->pRoot == NULL)
	{
		/* Allocate a new root node. */
		if (pThis->pBucket != NULL)
		{
			pThis->pRoot = pThis->pBucket;
			pThis->pBucket = pThis->pBucket->pNextEqual;
		} else
		{
			pThis->pRoot = (struct OcNode *)malloc(sizeof(struct OcNode));

			if (pThis->pRoot == NULL)
			{	return 0;	/* Memory allocation failure. */
			}
		}

		/* Construct the root node. */
		OcNode_ConstructM((pThis->pRoot));
	}

	/* Let recursive routine handle the rest. */
	return Octree_InsertColorRec(pThis, pThis->pRoot,
										  nRed, nGreen, nBlue);
}

/********************************************************************
* Function : Octree_FillColormap()
* Purpose : Fills a colormap with the colors in the octree.
* Pre : pThis points to an initialized Octree structure, pColormap
*       points to the colormap to be filled (consisting of at least
*       pThis->nLeafCount entries).
* Post : pColormap now contains the RGB values for the colors,
*        initialized using the Octree. The RGB values are encoded as
*        unsigned longs in the format 0xRRGGBB.
********************************************************************/
void Octree_FillColormap(struct Octree *pThis,
								 unsigned long *pColormap)
{
	OcNode_FillColormapRec(pThis->pRoot, pColormap, pThis->nColorOffset);
}

/********************************************************************
* Function : Octree_FindColorIndex()
* Purpose : Searches Octree pThis for the best color match for the
*           specified color ulRGB.
* Pre : pThis points to an initialized Octree structure upon which
*       Octree_FillColormap() has been called.
*       ulRGB specifies the color to be looked up, encoded as an
*       unsigned long in the RGB format 0xRRGGBB.
* Post : Returnvalue represents the color index that's best for the
*        ulRGB color.
********************************************************************/
int Octree_FindColorIndex(struct Octree *pThis,
								  unsigned long ulRGB)
{
	struct OcNode *pOcNode;
	struct OcNode *pChildNode;
	int nRed, nGreen, nBlue;
	int nMask;

	/* Iterate into the Octree until we reach a leaf.
	 * use ulRGB for decision making. */

	pOcNode = pThis->pRoot;

	/* If there is no octree, return 0 as the index. */
	if (pOcNode == NULL)
		return 0;

	nRed = (ulRGB & 0xFF0000) >> 16;
	nGreen = (ulRGB & 0xFF00) >> 8;
	nBlue = (ulRGB & 0xFF);

	while (pOcNode->nCount == 0)
	{
		/* Find child to go into. */
		/* Find a mask upon which to base the decision.
		 * This means finding a decision bit.
		 * For level 0 this is bit 7 (128),
		 * For level 1 this is bit 6 (64) etc. */
		nMask = 128;
		nMask = nMask >> pOcNode->nLevel;
		if (nMask & nRed)
		{	/* Big R */
			if (nMask & nGreen)
			{	/* Big G */
				if (nMask & nBlue)
				{	/* Big B */
					pChildNode = pOcNode->pRGB;
				} else
				{	/* Small B */
					pChildNode = pOcNode->pRGb;
				}
			} else
			{	/* Small G */
				if (nMask & nBlue)
				{	/* Big B */
					pChildNode = pOcNode->pRgB;
				} else
				{	/* Small B */
					pChildNode = pOcNode->pRgb;
				}
			}
		} else
		{	/* Small R */
			if (nMask & nGreen)
			{	/* Big G */
				if (nMask & nBlue)
				{	/* Big B */
					pChildNode = pOcNode->prGB;
				} else
				{	/* Small B */
					pChildNode = pOcNode->prGb;
				}
			} else
			{	/* Small G */
				if (nMask & nBlue)
				{	/* Big B */
					pChildNode = pOcNode->prgB;
				} else
				{	/* Small B */
					pChildNode = pOcNode->prgb;
				}
			}
		}

		/* Check if pChildNode is NULL,
		 * If so, the RGB color specified was not added
		 * to the tree. Now we need to come up with the
		 * closest matching color (which is difficult)
		 * in the subtree. Our resolution of this problem
		 * is to select any child just as long it isn't 0.*/
		if (pChildNode == NULL)
		{
			if (pOcNode->prgb != NULL)
				pChildNode = pOcNode->prgb;
			else
			if (pOcNode->prgB != NULL)
				pChildNode = pOcNode->prgB;
			else
			if (pOcNode->prGb != NULL)
				pChildNode = pOcNode->prGb;
			else
			if (pOcNode->prGB != NULL)
				pChildNode = pOcNode->prGB;
			else
			if (pOcNode->pRgb != NULL)
				pChildNode = pOcNode->pRgb;
			else
			if (pOcNode->pRgB != NULL)
				pChildNode = pOcNode->pRgB;
			else
			if (pOcNode->pRGb != NULL)
				pChildNode = pOcNode->pRGb;
			else
			if (pOcNode->pRGB != NULL)
				pChildNode = pOcNode->pRGB;

			/* If can't find any children (unlikely), we're
			 * at a dead end and our algorithm has performed
			 * badly. Just return the (incorrect) index
			 * stored at the current OcNode. */
			if (pChildNode == NULL)
				return pOcNode->nColorIndex;
		}

		/* Jump in child node. */
		pOcNode = pChildNode;
	}

	return pOcNode->nColorIndex;
}
