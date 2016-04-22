
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "vector.h"
#include "frame.h"
#include "plane.h"
#include "indexset.h"
#include "trans.h"
#include "vertxset.h"
#include "polygon.h"
#include "polyset.h"
#include "hplane.h"
#include "model.h"
#include "vpoint.h"
#include "actor.h"

struct Model *MakeCubeModel(void)
{	struct Model *pModel;
	struct Vertex vtx;
	struct Polygon pol;
	struct PolySet pset;

	Polygon_ConstructM(&pol);
	PolySet_ConstructM(&pset);

	pModel = (struct Model *)malloc(sizeof(struct Model));
	
	if (pModel != NULL)
	{
		Model_ConstructM(pModel);
		
		/* Build vertices. */
		vtx.Position.V[0] = -10;  vtx.Position.V[1] = -10;  vtx.Position.V[2] = -10;
		VertexSet_AddM(&(pModel->Vertices), &vtx);
		vtx.Position.V[0] = -10;  vtx.Position.V[1] = -10;  vtx.Position.V[2] = 10;
		VertexSet_AddM(&(pModel->Vertices), &vtx);
		vtx.Position.V[0] = -10;  vtx.Position.V[1] = 10;  vtx.Position.V[2] = -10;
		VertexSet_AddM(&(pModel->Vertices), &vtx);
		vtx.Position.V[0] = -10;  vtx.Position.V[1] = 10;  vtx.Position.V[2] = 10;
		VertexSet_AddM(&(pModel->Vertices), &vtx);
		vtx.Position.V[0] = 10;  vtx.Position.V[1] = -10;  vtx.Position.V[2] = -10;
		VertexSet_AddM(&(pModel->Vertices), &vtx);
		vtx.Position.V[0] = 10;  vtx.Position.V[1] = -10;  vtx.Position.V[2] = 10;
		VertexSet_AddM(&(pModel->Vertices), &vtx);
		vtx.Position.V[0] = 10;  vtx.Position.V[1] = 10;  vtx.Position.V[2] = -10;
		VertexSet_AddM(&(pModel->Vertices), &vtx);
		vtx.Position.V[0] = 10;  vtx.Position.V[1] = 10;  vtx.Position.V[2] = 10;
		VertexSet_AddM(&(pModel->Vertices), &vtx);

		/* Build polygons. */
		/* Front polygon. */
		pol.Vertices.nCount = 0;
		pol.ulRGB = '0';
		IndexSet_AddM(&(pol.Vertices), 0);
		IndexSet_AddM(&(pol.Vertices), 2);
		IndexSet_AddM(&(pol.Vertices), 6);
		IndexSet_AddM(&(pol.Vertices), 4);
		PolySet_AddM(&pset, &pol);
		/* Left Side Polygon. */
		pol.Vertices.nCount = 0;
		pol.ulRGB = '1';
		IndexSet_AddM(&(pol.Vertices), 0);
		IndexSet_AddM(&(pol.Vertices), 1);
		IndexSet_AddM(&(pol.Vertices), 3);
		IndexSet_AddM(&(pol.Vertices), 2);
		PolySet_AddM(&pset, &pol);
		/* Right Side Polygon. */
		pol.Vertices.nCount = 0;
		pol.ulRGB = '2';
		IndexSet_AddM(&(pol.Vertices), 4);
		IndexSet_AddM(&(pol.Vertices), 6);
		IndexSet_AddM(&(pol.Vertices), 7);
		IndexSet_AddM(&(pol.Vertices), 5);
		PolySet_AddM(&pset, &pol);
		/* Bottom Side Polygon. */
		pol.Vertices.nCount = 0;
		pol.ulRGB = '3';
		IndexSet_AddM(&(pol.Vertices), 2);
		IndexSet_AddM(&(pol.Vertices), 3);
		IndexSet_AddM(&(pol.Vertices), 7);
		IndexSet_AddM(&(pol.Vertices), 6);
		PolySet_AddM(&pset, &pol);
		/* Top Side Polygon. */
		pol.Vertices.nCount = 0;
		pol.ulRGB = '4';
		IndexSet_AddM(&(pol.Vertices), 0);
		IndexSet_AddM(&(pol.Vertices), 4);
		IndexSet_AddM(&(pol.Vertices), 5);
		IndexSet_AddM(&(pol.Vertices), 1);
		PolySet_AddM(&pset, &pol);
		/* Back Side Polygon. */
		pol.Vertices.nCount = 0;
		pol.ulRGB = '5';
		IndexSet_AddM(&(pol.Vertices), 1);
		IndexSet_AddM(&(pol.Vertices), 5);
		IndexSet_AddM(&(pol.Vertices), 7);
		IndexSet_AddM(&(pol.Vertices), 3);
		PolySet_AddM(&pset, &pol);

		Polygon_DestructM(&(pol));

		/* Now build a BSP Tree. */
		pModel->pRoot = HPlane_ConstructTree(&pset, &(pModel->Vertices), &(pModel->Polygons));

		/* Initialize LeafCount. */	
		printf("BSP Tree contains %d leafs.\n", HPlane_CalculateLeafCount(pModel->pRoot));
		printf("Finished HPlane construction.\n");

		/* And build a bounding sphere */
		Model_CalcBoundingSphere(pModel);
		
		printf("Finished bounding sphere construction [(%f, %f, %f), %f].\n",
				 pModel->Centerpoint.V[0], pModel->Centerpoint.V[1], pModel->Centerpoint.V[2], pModel->fRadius);

		PolySet_DestructM(&(pset));
		
		/* Done. */
		return pModel;
	}	
}

void TESTVPoint(void)
{	struct Model *pModel;
	struct Viewpoint Vpoint;
	struct Actor Actr;
	struct Actor Actr2;
	static unsigned char pBitmap[79*23];
	int n, m;
	
	/* Make a test cube model */
	pModel = MakeCubeModel();
	
	if (pModel != NULL)
	{
		/* Got a cube, dump it's BSP tree to stdout. */
#ifdef DEBUG
		HPlane_DebugDump(pModel->pRoot, 0);
	
		printf("Finished HPlane debug dump.\n");
#endif

		/* Build an Actor. */
		Actor_ConstructM(&Actr);
		Actor_ConstructM(&Actr2);
		
		printf("Finished Actor construction.\n");
		
		/* Set the Actor's Model. */
		Actor_SetModel(&Actr, pModel);
		Actor_SetModel(&Actr2, pModel);

		/* Put the actor about 20 meters away from us. */
		Actr.ActorFrame.TransformationToParent.Translation.V[0] = 20.f;
		Actr.ActorFrame.TransformationToParent.Translation.V[1] = 20.f;
		Actr.ActorFrame.TransformationToParent.Translation.V[2] = 35.f;
		/* Put the other actor about 50 meters away from us. */
		Actr2.ActorFrame.TransformationToParent.Translation.V[0] = 0.f;
		Actr2.ActorFrame.TransformationToParent.Translation.V[1] = 0.f;
		Actr2.ActorFrame.TransformationToParent.Translation.V[2] = 45.f;

		/* Rotate around the Z axis of the second actor to some degree. */
		/* Shear the Y component of the X axis. */
		Actr2.ActorFrame.TransformationToParent.Rotation[1][0] = 1.f;
		/* Shear the Z component of the X axis. */
		Actr2.ActorFrame.TransformationToParent.Rotation[2][0] = 0.5f;
//		/* And force the shear into a rotation. */
//		Transformation_ForceRotation(&Actr2.ActorFrame.TransformationToParent);

		/* Link the actors together. */
		Actr.pNext = &Actr2;
		
		/* Build a viewpoint. */
		Viewpoint_ConstructM(&(Vpoint));
		printf("Finished Viewpoint construction.\n");
		/* Set some width and height, 79x23 keeps it easy. */
		Vpoint.nWidth = 79;
		Vpoint.nHeight = 23;
		Vpoint.nPixelRow = 79;
		Vpoint.pBitmap = pBitmap;
		for (n = 0; n < (79 * 23); n++)
			pBitmap[n] = '.';
		EdgeTable_AtLeast(&(Vpoint.PolyEdgeTable), 23);
		/* Initialize multipliers. */
//		Viewpoint_PrecalcM(&(Vpoint));
		/* Initialize frustrum planes. */
		Viewpoint_PrecalcFrustrum(&Vpoint);

		printf("Finished Viewpoint multiplier initialization.\n");
		
		/* Get ready for some heavy stuff. 
		 * Prepare the actors for drawing. */
		if (Viewpoint_PrepActorsForDraw(&Vpoint, &Actr))
		{
			/* Succesfully prepared. */
			/* Dump to output. */
			printf("Successfully prepared!\n");
		} else
		{	printf("Failed to prep...\n");
		}
#ifdef DEBUG
		printf("First Actor Info.\n");
		Actor_DumpScreenVertices(&Actr);
		Actor_DumpScreenPolygons(&Actr);
		printf("Second Actor Info.\n");
		Actor_DumpScreenVertices(&Actr2);
		Actor_DumpScreenPolygons(&Actr2);
#endif

		/* Draw to the bitmap. */
		if (Viewpoint_Draw(&Vpoint))
		{
			printf("Succesfully rendered the viewpoint!\n");
			/* And display the bitmap. */
			for (n = 0; n < 23; n++)
			{	for (m = 0; m < 79; m++)
				{	printf("%c", pBitmap[n * 79 + m]);
				}
				printf("\n");
			}
		}
		
		/* Done. */
		Viewpoint_DestructM(&Vpoint);
		Actor_DestructM(&Actr);
		Actor_DestructM(&Actr2);
		Model_DestructM(pModel);
		free(pModel);
	}
}

void TESTHPlaneCube(void)
{
	struct PolySet pset;
	struct PolySet NwPset;
	struct Polygon pol;
	struct VertexSet vset;
	struct Vertex vtx;
	struct HPlane *pHPlane;
	int n;

	printf("HPlane Cube testing (convex polyhedron BSP tree).\n");

	/* Construction of variables. */
	PolySet_ConstructM(&NwPset);
	PolySet_ConstructM(&pset);
	Polygon_ConstructM(&pol);
	VertexSet_ConstructM(&vset);
	Vertex_ConstructM(&vtx);

	/* Build vertices. */
	vtx.Position.V[0] = -10;  vtx.Position.V[1] = -10;  vtx.Position.V[2] = -10;
	VertexSet_AddM(&vset, &vtx);
	vtx.Position.V[0] = -10;  vtx.Position.V[1] = -10;  vtx.Position.V[2] = 10;
	VertexSet_AddM(&vset, &vtx);
	vtx.Position.V[0] = -10;  vtx.Position.V[1] = 10;  vtx.Position.V[2] = -10;
	VertexSet_AddM(&vset, &vtx);
	vtx.Position.V[0] = -10;  vtx.Position.V[1] = 10;  vtx.Position.V[2] = 10;
	VertexSet_AddM(&vset, &vtx);
	vtx.Position.V[0] = 10;  vtx.Position.V[1] = -10;  vtx.Position.V[2] = -10;
	VertexSet_AddM(&vset, &vtx);
	vtx.Position.V[0] = 10;  vtx.Position.V[1] = -10;  vtx.Position.V[2] = 10;
	VertexSet_AddM(&vset, &vtx);
	vtx.Position.V[0] = 10;  vtx.Position.V[1] = 10;  vtx.Position.V[2] = -10;
	VertexSet_AddM(&vset, &vtx);
	vtx.Position.V[0] = 10;  vtx.Position.V[1] = 10;  vtx.Position.V[2] = 10;
	VertexSet_AddM(&vset, &vtx);
	
	/* Build polygons. */
	/* Front polygon. */
	pol.Vertices.nCount = 0;
	IndexSet_AddM(&(pol.Vertices), 0);
	IndexSet_AddM(&(pol.Vertices), 2);
	IndexSet_AddM(&(pol.Vertices), 6);
	IndexSet_AddM(&(pol.Vertices), 4);
	PolySet_AddM(&pset, &pol);
	/* Left Side Polygon. */
	pol.Vertices.nCount = 0;
	IndexSet_AddM(&(pol.Vertices), 0);
	IndexSet_AddM(&(pol.Vertices), 1);
	IndexSet_AddM(&(pol.Vertices), 3);
	IndexSet_AddM(&(pol.Vertices), 2);
	PolySet_AddM(&pset, &pol);
	/* Right Side Polygon. */
	pol.Vertices.nCount = 0;
	IndexSet_AddM(&(pol.Vertices), 4);
	IndexSet_AddM(&(pol.Vertices), 6);
	IndexSet_AddM(&(pol.Vertices), 7);
	IndexSet_AddM(&(pol.Vertices), 5);
	PolySet_AddM(&pset, &pol);
	/* Bottom Side Polygon. */
	pol.Vertices.nCount = 0;
	IndexSet_AddM(&(pol.Vertices), 2);
	IndexSet_AddM(&(pol.Vertices), 3);
	IndexSet_AddM(&(pol.Vertices), 7);
	IndexSet_AddM(&(pol.Vertices), 6);
	PolySet_AddM(&pset, &pol);
	/* Top Side Polygon. */
	pol.Vertices.nCount = 0;
	IndexSet_AddM(&(pol.Vertices), 0);
	IndexSet_AddM(&(pol.Vertices), 4);
	IndexSet_AddM(&(pol.Vertices), 5);
	IndexSet_AddM(&(pol.Vertices), 1);
	PolySet_AddM(&pset, &pol);
	/* Back Side Polygon. */
	pol.Vertices.nCount = 0;
	IndexSet_AddM(&(pol.Vertices), 1);
	IndexSet_AddM(&(pol.Vertices), 5);
	IndexSet_AddM(&(pol.Vertices), 7);
	IndexSet_AddM(&(pol.Vertices), 3);
	PolySet_AddM(&pset, &pol);

	/* Now build a BSP Tree. */
	pHPlane = HPlane_ConstructTree(&pset, &vset, &NwPset);

#ifdef DEBUG
	/* And dump it to stdout. */
	HPlane_DebugDump(pHPlane, 0);
#endif
	
	/* Clean up. */
	PolySet_DestructM(&NwPset);
	PolySet_DestructM(&pset);
	Polygon_DestructM(&pol);
	VertexSet_DestructM(&vset);
}




main()
{
	struct Polygon pol;
	struct VertexSet vset;
	struct Vertex vtx;
	struct Vector tnormal;

	/* Prevent nasty buffering from preventing all output when we hang. */
	if (setvbuf(stdout, NULL, _IONBF, 0))
		printf("Failed to halt output buffering.\n");
	
	printf("Chrome testing facility\n");
	
	printf("Normal vector from polygon test.\n");
	
	Polygon_ConstructM(&pol);
	VertexSet_ConstructM(&vset);
	Vertex_ConstructM(&vtx);
	
	/* Now build a triangle in the polygon & vertexset. */
	vtx.Position.V[0] = 0.f;
	vtx.Position.V[1] = 0.f;
	vtx.Position.V[2] = 0.f;
	VertexSet_Add(&vset, &vtx);
	
	vtx.Position.V[0] = 10.f;
	vtx.Position.V[1] = 0.f;
	vtx.Position.V[2] = 0.f;
	VertexSet_Add(&vset, &vtx);
	
	vtx.Position.V[0] = 0.f;
	vtx.Position.V[1] = 10.f;
	vtx.Position.V[2] = 0.f;
	VertexSet_Add(&vset, &vtx);
	
	IndexSet_Add(&(pol.Vertices), 0);
	IndexSet_Add(&(pol.Vertices), 1);
	IndexSet_Add(&(pol.Vertices), 2);
	
	Polygon_ExtractNormal(&pol, &vset, &tnormal);
	
	printf("Computed normal : %f, %f, %f\n", tnormal.V[0], tnormal.V[1], tnormal.V[2]);
	
	/* Test BSPTree logic. */
	/* TESTHPlaneCube(); */
	
	/* Test viewpoint logic. */
	printf("VIEWPOINT LOGIC\n");
	TESTVPoint();
}	
