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
* FILE : nffmodel.c
********************************************************************/

#define NFFMODEL_C

#include <stdlib.h>

#include "nffmodel.h"

#include "model.h"
#include "parsebuf.h"
#include "hplane.h"
#include "vertex.h"
#include "vertxset.h"
#include "polygon.h"
#include "polyset.h"

/********************************************************************
* Function : Model_LoadNFF()
* Purpose : Generates a model from an NFF file specified by it's
*           filename.
* Pre : sFilename points to a string containing the filename of the
*       NFF file to be read.
* Post : If the returnvalue != NULL, it is a pointer to the new
*        Model.
*        If the returnvalue == NULL, either the file was not an NFF
*        file or there was a memory allocation failure.
********************************************************************/
struct Model *Model_LoadNFF(char *sFilename, int bQuick)
{
	struct ParseBuf Buf;
	struct Model *pModel;

	ParseBuf_ConstructM(&Buf);
	if (ParseBuf_BuildFromFilename(&Buf, sFilename))
	{
		/* Call NFF reader. */
		pModel = Model_LoadNFFBuffer(&Buf, bQuick);

		ParseBuf_DestructM(&Buf);

		return pModel;
	}
}

/********************************************************************
* Function : Model_LoadNFFBuffer()
* Purpose : Reads an NFF file from a ParseBuf structure.
* Pre : pBuf points to an initialized ParseBuf structure containing
*       the NFF file to be read.
*       If bQuick != 0, it uses a faster algorithm for building the
*       Model's BSP tree that produces a less than optimal BSP tree.
* Post : If the returnvalue != NULL, it is the new Model structure
*        built from the NFF file.
*        If the returnvalue == NULL, either the contents of the 
*        pBuf were not an NFF file or there was a memory allocation
*        failure.
********************************************************************/
struct Model *Model_LoadNFFBuffer(struct ParseBuf *pBuf, int bQuick)
{
	float fRes;
	struct Vertex vert;
	struct VertexSet vset;
	struct Polygon pol;
	struct PolySet pset;
	int nVertOffset;
	int nVertCount;
	int nPolCount;
	int nPVertCount;
	int n, m, k;
	int nVIndex;
	int bBothSides;	/* If true, both sides of polygon
							 * should be visible. */
	int bDone;
	struct Model *pModel;
	unsigned long ulRGB;
	float fDummy;
	int nColorRun;

	/* Match "NFF" token. */
	ParseBuf_SkipNFFWhitespaces(pBuf);
	if (!ParseBuf_MatchString(pBuf, "nff"))
	{
		/* It's NOT an NFF file. */
		return NULL;
	}

	/* It's an NFF file. */
	ParseBuf_SkipNFFWhitespaces(pBuf);
	/* Match "version" tag (optional). */
	if (ParseBuf_MatchString(pBuf, "version"))
	{
		/* Check version number, this may be 2.0, 2.09 or 2.1. */
		ParseBuf_SkipNFFWhitespaces(pBuf);
		if ((!ParseBuf_MatchString(pBuf, "2.09")) &&
			 (!ParseBuf_MatchString(pBuf, "2.00")) &&
			 (!ParseBuf_MatchString(pBuf, "2.10")) &&
			 (!ParseBuf_MatchString(pBuf, "1.70")) &&
			 (!ParseBuf_MatchString(pBuf, "2.0")) &&
			 (!ParseBuf_MatchString(pBuf, "2.1")))
		{
			/* Unsupported version. */
			return NULL;
		}
	}
	ParseBuf_SkipNFFWhitespaces(pBuf);
	/* Skip optional viewpos clause. */
	if (ParseBuf_MatchString(pBuf, "viewpos"))
	{
		ParseBuf_SkipNFFWhitespaces(pBuf);
		ParseBuf_GetFloat(pBuf, &fRes);
		ParseBuf_SkipNFFWhitespaces(pBuf);
		ParseBuf_GetFloat(pBuf, &fRes);
		ParseBuf_SkipNFFWhitespaces(pBuf);
		ParseBuf_GetFloat(pBuf, &fRes);
		ParseBuf_SkipNFFWhitespaces(pBuf);
	}
	/* Skip optional viewdir clause. */
	if (ParseBuf_MatchString(pBuf, "viewdir"))
	{
		ParseBuf_SkipNFFWhitespaces(pBuf);
		ParseBuf_GetFloat(pBuf, &fRes);
		ParseBuf_SkipNFFWhitespaces(pBuf);
		ParseBuf_GetFloat(pBuf, &fRes);
		ParseBuf_SkipNFFWhitespaces(pBuf);
		ParseBuf_GetFloat(pBuf, &fRes);
		ParseBuf_SkipNFFWhitespaces(pBuf);
	}
	
	/* Iterate all objects in the NFF file... */
	Vertex_ConstructM(&vert);
	VertexSet_ConstructM(&vset);
	Polygon_ConstructM(&pol);
	PolySet_ConstructM(&pset);

	while (!ParseBuf_EndOfBuffer(pBuf))
	{
		/* Read / Skip object name. */
		ParseBuf_GetLineString(pBuf, NULL, 0);

		ParseBuf_SkipNFFWhitespaces(pBuf);

		/* Read number of vertices. */
		ParseBuf_GetInt(pBuf, &nVertCount);

		/* Set vertex offset for this object. */
		nVertOffset = vset.nCount;

		/* Retrieve all vertices. */
		n = nVertCount;
		while (n > 0)
		{
			Vertex_ConstructM(&vert);

			ParseBuf_SkipNFFWhitespaces(pBuf);

			/* Retrieve vertex position. */
			ParseBuf_GetFloat(pBuf, &(vert.Position.V[0]));
			ParseBuf_SkipNFFWhitespaces(pBuf);
			ParseBuf_GetFloat(pBuf, &(vert.Position.V[1]));
			ParseBuf_SkipNFFWhitespaces(pBuf);
			ParseBuf_GetFloat(pBuf, &(vert.Position.V[2]));
			ParseBuf_SkipNFFWhitespaces(pBuf);

			/* Optional "norm" normal vector specification. */
			if (ParseBuf_MatchString(pBuf, "norm"))
			{
				ParseBuf_SkipNFFWhitespaces(pBuf);
				ParseBuf_GetFloat(pBuf, &(vert.Normal.V[0]));
				ParseBuf_SkipNFFWhitespaces(pBuf);
				ParseBuf_GetFloat(pBuf, &(vert.Normal.V[1]));
				ParseBuf_SkipNFFWhitespaces(pBuf);
				ParseBuf_GetFloat(pBuf, &(vert.Normal.V[2]));
			}
			ParseBuf_SkipNFFWhitespaces(pBuf);

			/* Optional "rgb" vertex color specification. */
			if (ParseBuf_MatchString(pBuf, "rgb"))
			{
				ParseBuf_SkipNFFWhitespaces(pBuf);
				ParseBuf_GetNFFRGB(pBuf, &ulRGB);
				ParseBuf_SkipNFFWhitespaces(pBuf);
				ParseBuf_GetNFFRGB(pBuf, &ulRGB);
				ParseBuf_SkipNFFWhitespaces(pBuf);
				ParseBuf_GetNFFRGB(pBuf, &ulRGB);
			}
			ParseBuf_SkipNFFWhitespaces(pBuf);
			
			/* Optional "uv" texture map coordinate specification. */
			if (ParseBuf_MatchString(pBuf, "uv"))
			{
				ParseBuf_SkipNFFWhitespaces(pBuf);
				ParseBuf_GetFloat(pBuf, &fDummy);
				ParseBuf_SkipNFFWhitespaces(pBuf);
				ParseBuf_GetFloat(pBuf, &fDummy);
			}
			ParseBuf_SkipNFFWhitespaces(pBuf);

			/* Skip optional "N" automatic normal generation. */
			if (ParseBuf_MatchString(pBuf, "N"))
				ParseBuf_SkipNFFWhitespaces(pBuf);

			/* Add the vertex to the vertex set. */
			if (!VertexSet_AddM(&vset, &vert))
			{
				VertexSet_DestructM(&vset);
				Polygon_DestructM(&pol);
				PolySet_DestructM(&pset);
				return NULL;
			}
			n--;
		}

		/* Retrieve number of polygons. */
		ParseBuf_GetInt(pBuf, &nPolCount);
		ParseBuf_SkipNFFWhitespaces(pBuf);
		n = nPolCount;
		while (n > 0)
		{
			pol.Vertices.nCount = 0;

			/* Retrieve number of vertices on polygon. */
			ParseBuf_GetInt(pBuf, &nPVertCount);
			ParseBuf_SkipNFFWhitespaces(pBuf);

			/* Retrieve the vertex indices. */
			m = nPVertCount;
			while (m > 0)
			{
				ParseBuf_GetInt(pBuf, &nVIndex);
				ParseBuf_SkipNFFWhitespaces(pBuf);

				/* Add the index to the polygon. */
				if (!IndexSet_AddM(&(pol.Vertices), nVIndex + nVertOffset))
				{	/* Mem Failure. */
					VertexSet_DestructM(&vset);
					Polygon_DestructM(&pol);
					PolySet_DestructM(&pset);
					return NULL;
				}

				m--;
			}

			/* Retrieve the polygon's color. */
			ParseBuf_GetNFFRGB(pBuf, &ulRGB);
			pol.ulRGB = ulRGB;

			ParseBuf_SkipNFFWhitespaces(pBuf);
			/* Check for optional "both" flag. */
			if (ParseBuf_MatchString(pBuf, "both"))
			{
				/* Set a flag for a duplicate polygon
				 * with reverse vertex order. */
				bBothSides = 1;
				ParseBuf_SkipNFFWhitespaces(pBuf);
			} else
			{	bBothSides = 0;
				ParseBuf_SkipNFFWhitespaces(pBuf);
			}

			/* Check for optional "_v_" vanilla texture. */
			if (ParseBuf_MatchString(pBuf, "_v_") ||
				 ParseBuf_MatchString(pBuf, "_V_"))
			{
				/* Skip the texture & go to next token. */
				ParseBuf_SkipUntilNFFWhitespace(pBuf);
				ParseBuf_SkipNFFWhitespaces(pBuf);
			}

			/* Check for optional "_s_" shaded texture. */
			if (ParseBuf_MatchString(pBuf, "_s_") ||
				 ParseBuf_MatchString(pBuf, "_S_"))
			{
				/* Skip the texture & go to next token. */
				ParseBuf_SkipUntilNFFWhitespace(pBuf);
				ParseBuf_SkipNFFWhitespaces(pBuf);
			}

			/* Check for optional "_t_" transparent texture. */
			if (ParseBuf_MatchString(pBuf, "_t_") ||
				 ParseBuf_MatchString(pBuf, "_T_"))
			{
				/* Skip the texture & go to next token. */
				ParseBuf_SkipUntilNFFWhitespace(pBuf);
				ParseBuf_SkipNFFWhitespaces(pBuf);
			}

			/* Check for optional "_u_" shaded and transparent texture. */
			if (ParseBuf_MatchString(pBuf, "_u_") ||
				 ParseBuf_MatchString(pBuf, "_U_"))
			{
				/* Skip the texture & go to next token. */
				ParseBuf_SkipUntilNFFWhitespace(pBuf);
				ParseBuf_SkipNFFWhitespaces(pBuf);
			}

			/* Check for texture fitting options. */
			bDone = 0;
			while (!bDone)
			{
				/* Check for optional "trans" texture translation. */
				if (ParseBuf_MatchString(pBuf, "trans") ||
					 ParseBuf_MatchString(pBuf, "TRANS"))
				{
					/* Skip whitespaces & translation vector. */
					ParseBuf_SkipNFFWhitespaces(pBuf);
					ParseBuf_SkipUntilNFFWhitespace(pBuf);
					ParseBuf_SkipNFFWhitespaces(pBuf);
					ParseBuf_SkipUntilNFFWhitespace(pBuf);
					ParseBuf_SkipNFFWhitespaces(pBuf);
				} else
				/* Check for optional "rot" texture rotation. */
				if (ParseBuf_MatchString(pBuf, "rot") ||
					 ParseBuf_MatchString(pBuf, "ROT"))
				{
					/* Skip whitespaces & rotation value. */
					ParseBuf_SkipNFFWhitespaces(pBuf);
					ParseBuf_SkipUntilNFFWhitespace(pBuf);
					ParseBuf_SkipNFFWhitespaces(pBuf);
				} else
				/* Check for optional "mirror" texture mirroring. */
				if (ParseBuf_MatchString(pBuf, "mirror") ||
					 ParseBuf_MatchString(pBuf, "MIRROR"))
				{
					/* Just skip whitespaces. */
					ParseBuf_SkipNFFWhitespaces(pBuf);
				} else
				/* Check for optional "id=" polygon ID tagging. */
				if (ParseBuf_MatchString(pBuf, "id=") ||
					 ParseBuf_MatchString(pBuf, "ID="))
				{
					/* Skip whitespaces & ID value. */
					ParseBuf_SkipUntilNFFWhitespace(pBuf);
					ParseBuf_SkipNFFWhitespaces(pBuf);
				} else
				/* Check for optional "-" hyphen for portal naming. */
				if (ParseBuf_MatchString(pBuf, "-"))
				{
					/* Skip portal name & whitespace. */
					ParseBuf_SkipUntilNFFWhitespace(pBuf);
					ParseBuf_SkipNFFWhitespaces(pBuf);
				} else
					/* Nothing matched, exit texure options loop. */
					bDone = 1;
			}

			/* Add polygon to set. */
			if (!PolySet_AddM(&pset, &pol))
			{
				/* Mem failure. */
				VertexSet_DestructM(&vset);
				Polygon_DestructM(&pol);
				PolySet_DestructM(&pset);
				return NULL;
			}

			if (bBothSides)
			{
				/* Add reverse polygon to set, this
				 * inverses the polygon's normal and
				 * makes the other (clockwise) side
				 * visible. */
				/* First reverse polygon. */
				m = nPVertCount / 2;
				for (m = 0; m < (nPVertCount / 2); m++)
				{
					/* Swap two vertex indices. */
					k = IndexSet_GetIndexM(&(pol.Vertices), m);
					IndexSet_GetIndexM(&(pol.Vertices), m) = IndexSet_GetIndexM(&(pol.Vertices), (nPVertCount - 1) - m);
					IndexSet_GetIndexM(&(pol.Vertices), (nPVertCount - 1) - m) = k;
				}

				/* Add reversed polygon to set. */
				if (!PolySet_AddM(&pset, &pol))
				{
					/* Mem failure. */
					VertexSet_DestructM(&vset);
					Polygon_DestructM(&pol);
					PolySet_DestructM(&pset);
					return NULL;
				}
			}
			n--;
		}
	}

	/* Prepare the Model. */
	pModel = (struct Model *)malloc(sizeof(struct Model));
	if (pModel == NULL)
	{	/* Mem failure. */
		VertexSet_DestructM(&vset);
		Polygon_DestructM(&pol);
		PolySet_DestructM(&pset);
		return NULL;
	}
	
	Model_ConstructM(pModel);
	pModel->Vertices = vset;	/* Copy vertices in there,
										 * pModel becomes the owner. */

	/* Build the Model's BSP tree. */
	if (bQuick)
		pModel->pRoot = HPlane_ConstructTreeQuick(&pset, &(pModel->Vertices), &(pModel->Polygons));
	else
		pModel->pRoot = HPlane_ConstructTree(&pset, &(pModel->Vertices), &(pModel->Polygons));

		
	if (pModel->pRoot == NULL)
	{	/* Failed to build the BSP tree. */
		Polygon_DestructM(&pol);
		PolySet_DestructM(&pset);
		Model_DestructM(pModel);		/* This also destroys vset. */
		free(pModel);
		return NULL;
	}

	/* Build bounding sphere. */
	Model_CalcBoundingSphere(pModel);
	
	/* Build Hyperplane leaf count. */
	HPlane_CalculateLeafCount(pModel->pRoot);

	/* Clean up and return. */
	Polygon_DestructM(&pol);
	PolySet_DestructM(&pset);
	return pModel;
}

