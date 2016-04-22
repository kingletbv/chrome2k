/********************************************************************
* FILE : tstworld.c
********************************************************************/

#define TSTWORLD_C

#include <stdlib.h>
#include <math.h>

#include <model.h>
#include <pmodel.h>
#include <trans.h>
#include <vpoint.h>
#include <actor.h>
#include <nffmodel.h>
#include <octree.h>
#include <lmap256.h>
#include <colormgr.h>
#include <texmap.h>

#include "tstworld.h"

#define PHI 3.141592654f

struct ColorManager colmgr;
struct Viewpoint Vpoint;
struct Model *pArena;	/* Arena Model, where the cars reside. */
struct Model *pBumper;	/* Bumper Model, the actual cars. */
struct Model *pGBumper;	/* Green Bumper Model, identical to pBumper except for
								 * it's color (which is green instead of red). */
struct Model *pCube;
struct Model *pCyl;
struct Actor AcArena;		/* Arena Actor. */

struct Actor Ac1;			/* First Actor, center cube. */
struct Actor Ac2;			/* Second Actor, comet cube. */
struct Actor Ac3;			/* Third Actor, comet orbitor cube. */

struct TextureMap	ChromeBmp;		/* Little Chrome texture, currently just blitted
									 * to the screen. */
struct TextureMap	OilSmudgeBmp;
struct TextureMap	ThemRoots;
struct TextureMap	TubularHaze;

#define ACS_COUNT	100	/* Number of actors in random box. */
#define ACS_DIMS 600.f	/* Dimensions of random box. */
struct Actor Acs[ACS_COUNT];	/* A lot of actors. */

float fAc1Angle;			/* Angle of rotation for Ac1. */

float	fSpeed;				/* Speed buildup for 'driver' */
float	fAngleSpeed;		/* Speed for Angle. */
float	fAngle;				/* Angle for 'driver' */

#define BC_COUNT	1000		/* 10 other cars. */
#define BC_XDIM	99.5f	/* X Dimensions. Defines playfield for cars. */
#define BC_ZDIM	149.5f/* Z Dimensions. Ditto. */
#define BC_MSPD	0.2f	/* Maximum speed. */
#define BC_MROT	0.07f	/* Maximum rotation. */
#define BC_DECI	30		/* Maximum next decision time. */
//#define RENDERMODE CHROME_VIEWPOINT_RENDERMODE_TRUECOLOR_32
#define RENDERMODE CHROME_VIEWPOINT_RENDERMODE_INDEXED_8

/* Small local structurure for describing the state of a single bumper car. */
struct BumperCarState
{
	struct Actor	Actor;
	float		fSpeed;			/* Speed. (a.k.a. inertia) */
	float		fAngleSpeed;	/* Speed of angular motion (rotation) */
	float		fAngle;			/* Current angle. */

	float		fAngleAccel;	/* Angle Acceleration. */
	float		fSpeedAccel;	/* Speed Acceleration. */

	int		nCount;			/* Nr. of frames to go before the AngleSpeed is
									 * re-evaluated. */
}	BumperCars[BC_COUNT];

/* Small function for BumperCar intelligence. */
static void AutoDrive(struct BumperCarState *pState);

/* Small function for handling driving characteristics.
 * Fills the specified transformation.
 * Drives into Z direction, pfAngleSpeed determines rotational
 * velocity, pfAngle determines current angle, pfSpeed determines
 * current driving speed. */
static void Drive(struct Transformation *pTrans, float *pfSpeed, float *pfAngleSpeed, float *pfAngle);

/********************************************************************
* Function : InitTexs() & PrepTexs()
* Purpose : Helper function for InitWorld to initialize textures.
********************************************************************/
int InitTexs(struct ColorManager *pColMgr)
{
	FILE *fp;

	/* Load Chrome texturemap. */
	fp = fopen("Chrome.tex", "rb");

	if (fp != NULL)
	{
		if (0 != TextureMap_ReadTex(&ChromeBmp, fp))
		{
			fclose(fp);
			/* Succesfully read Chrome Bmp.
			 * Link it to the color manager. */
			TextureMap_AttachToColorMgr(&ChromeBmp, pColMgr);

			/* Load OilSmudge Texturemap. */
			fp = fopen("OilSmudgeTile.tex", "rb");

			if (fp != NULL)
			{
				if (0 != TextureMap_ReadTex(&OilSmudgeBmp, fp))
				{
					fclose(fp);
					/* Succesfully read OilSmudge Bmp.
					 * Link it to the color manager. */
					TextureMap_AttachToColorMgr(&OilSmudgeBmp, pColMgr);

					/* Load ThemRoots Texturemap. */
					fp = fopen("ThemRoots.tex", "rb");

					if (fp != NULL)
					{
						if (0 != TextureMap_ReadTex(&ThemRoots, fp))
						{
							fclose(fp);
							/* Succesfully read ThemRoots Bmp.
							 * Link it to the color manager. */
							TextureMap_AttachToColorMgr(&ThemRoots, pColMgr);
	
							/* Load TubularHaze Texturemap. */
							fp = fopen("TubularHaze.tex", "rb");

							if (fp != NULL)
							{
								if (0 != TextureMap_ReadTex(&TubularHaze, fp))
								{
									fclose(fp);
									/* Succesfully read ThemRoots Bmp.
									 * Link it to the color manager. */
									TextureMap_AttachToColorMgr(&TubularHaze, pColMgr);

									return 1;
								} else
									fclose(fp);
							}
						} else
							fclose(fp);
					}
				} else
					fclose(fp);
			}
		} else
			fclose(fp);
	}

	return 0;	/* Failure */
}

int PrepTexs(void)
{
	TextureMap_Prepare(&ChromeBmp);
	TextureMap_Prepare(&OilSmudgeBmp);
	TextureMap_Prepare(&ThemRoots);
	TextureMap_Prepare(&TubularHaze);
	return 1;
}

/********************************************************************
* Function : InitWorld()
* Purpose : Initializes our world.
********************************************************************/
int InitWorld(int nSWidth, int nSHeight, unsigned long *pColormap)
{
	struct Vector Vec;
	struct Plane TPlane;
	struct Octree Oct;
	struct Lightmap256 *pLmap0, *pLmap1, *pLmap2, *pLmap3;
	int n;
	int b;
	FILE *fp;

	/* Initialize Viewpoint 'driving' logic. */
	fSpeed = 0.f;
	fAngle = 0.f;
	fAngleSpeed = 0.f;

	/* Initialize the ColorManager */
	ColorManager_Construct(&colmgr);

	/* Initialize the octree. */
	Octree_ConstructM(&Oct);

	/* Build a new viewpoint. */
	Viewpoint_ConstructM(&Vpoint);

	Viewpoint_SetRendermode( &Vpoint, RENDERMODE );

	/* Construct texturemap. */
	TextureMap_ConstructM(&ChromeBmp);

	/* Set the width and height of this viewpoint. */
	Vpoint.nWidth = nSWidth;
	Vpoint.nHeight = nSHeight;
	/* Assume that the number of bytes on a single row is
	 * identical to the width.
	 * This assumption means we assume taking the whole width
	 * of the screen. */
	Vpoint.nPixelRow = nSWidth;

	/* Make sure our polygons span the whole height. */
	if (!EdgeTable_AtLeast(&(Vpoint.PolyEdgeTable), nSHeight))
		return 0;

	Vpoint.fXFOV = (20.f / 180.f) * PHI;
	Vpoint.fYFOV = (16.f / 180.f) * PHI;

	/* Initialize multipliers. */
	Viewpoint_PrecalcM(&Vpoint);

	/* Initialize frustrum planes. */
	Viewpoint_PrecalcFrustrum(&Vpoint);

	/* Let the viewpoint hover just above the (0,0,0) point. */
	Vpoint.VpointFrame.TransformationToParent.Translation.V[1] = -0.4f;
	/* Let the viewpoint look down on the scene. */
	Vector_ConstructM(&Vec);
	Vec.V[0] = 1.f;
	/*Transformation_MakeRotateArbitrary(&(Vpoint.VpointFrame.TransformationToParent),
												  &Vec, -0.6f);
*/
	/* Next, build a model for the viewpoint to see. */
	/* Make it a cube, with radius 10. */
	/* pCube = MakeCubeModel();*/
	/* pCube = Model_NewCube(20.f); */
	/* pCube = Model_LoadNFF("d:\\models\\van.nff", 0);*/
	/* pCube = Model_LoadNFF("\\Chrome\\bumper.nff", 0);*/

	/* Load Chrome Model.. */
	pCube = Model_LoadNFF("chrome.nff", 0);
	if (pCube != NULL)
	{	/* Link cube Model up to color manager. */
		Model_LinkToColorManager(pCube, &colmgr);

		/* Load Bumper car Model. */
		pBumper = Model_LoadNFF("bumper.nff", 0);
		if (pBumper != NULL)
		{
			/* Link bumper car up to color manager. */
			Model_LinkToColorManager(pBumper, &colmgr);
			
			/* Load Green Bumper car Model. */
			pGBumper = Model_LoadNFF("bumpergr.nff", 0);
			if (pGBumper != NULL)
			{	
				/* Link bumper car up to color manager. */
				Model_LinkToColorManager(pGBumper, &colmgr);
				
				/* Load Arena Model. */
				pArena = Model_LoadNFF("arena.nff", 0);
				if (pArena != NULL)
				{
					/* Link Arena up to color manager. */
					Model_LinkToColorManager(pArena, &colmgr);
					
					/* Initialize textures. */
					if (InitTexs(&colmgr))
					{
						/* Build the palette. */
						ColorManager_AssignColors(&colmgr);
						/* Fill the colormap we got passed so our screen actually uses
						 * the right colors. */
						for (n = 0; n < 256; n++)
						{	pColormap[n] = colmgr.Colormap[n];
						}
						pColormap[255] = 0xFFFFFF;	/* Force 255 to white as it is used
															 * as background color. */
						/* Build texturemaps. */
						PrepTexs();

						/* Now that we have defined all models,
						 * build some actors. */

						/* AcArena, contains the arena. */
						Actor_ConstructM(&AcArena);
						
						/* Next Actor will be Ac1. */
						AcArena.pNext = &Ac2;

						/* Make it use the Arena model. */
						if (Actor_SetModel(&AcArena, pArena))
						{
							/* Ac1, a bumper car. */
							Actor_ConstructM(&Ac1);
							
							/* Position Ac1 in the world;
							 * specify it's frame. */
							Ac1.ActorFrame.TransformationToParent.Translation.V[0] = 0.f;
							Ac1.ActorFrame.TransformationToParent.Translation.V[1] = 0.0f;
							Ac1.ActorFrame.TransformationToParent.Translation.V[2] = 50.f;

							/* Next Actor will be Ac2. */
							Ac1.pNext = &Ac3;
							
							/* Set Ac1's Model. */
							if (Actor_SetModel(&Ac1, pGBumper))
							{
								/* Ac2, a Chrome logo for scenery. */
								Actor_ConstructM(&Ac2);

								/* Position Ac2 in the world;
								 * specify it's frame, in this case on the ground... */
								Ac2.ActorFrame.TransformationToParent.Translation.V[0] = 0.f;
								Ac2.ActorFrame.TransformationToParent.Translation.V[1] = 0.f;
								Ac2.ActorFrame.TransformationToParent.Translation.V[2] = 0.f;
								
								/* Set next model. */
								Ac2.pNext = &Ac1;

								/* Set it's Model. */
								if (Actor_SetModel(&Ac2, pCube))
								{
									/* Ac3, another Chrome logo for scenery. */
									Actor_ConstructM(&Ac3);

									/* Position Ac3 in the world;
									 * specify it's frame, in this case on the ground... */
									Ac3.ActorFrame.TransformationToParent.Translation.V[0] = 0.f;
									Ac3.ActorFrame.TransformationToParent.Translation.V[1] = 0.f;
									Ac3.ActorFrame.TransformationToParent.Translation.V[2] = 150.f;
									Ac3.ActorFrame.TransformationToParent.Rotation[0][0] = 1.f;
									Ac3.ActorFrame.TransformationToParent.Rotation[1][0] = 0.f;
									Ac3.ActorFrame.TransformationToParent.Rotation[2][0] = 0.f;
									Ac3.ActorFrame.TransformationToParent.Rotation[0][1] = 0.f;
									Ac3.ActorFrame.TransformationToParent.Rotation[1][1] = 0.f;
									Ac3.ActorFrame.TransformationToParent.Rotation[2][1] = 1.f;
									Ac3.ActorFrame.TransformationToParent.Rotation[0][2] = 0.f;
									Ac3.ActorFrame.TransformationToParent.Rotation[1][2] = -1.f;
									Ac3.ActorFrame.TransformationToParent.Rotation[2][2] = 0.f;
									
									/* Set next model. */
									Ac3.pNext = &(BumperCars[0].Actor);
									
									if (Actor_SetModel(&Ac3, pCube))
									{	/* Define all other bumpercars. */
										for (n = 0; n < BC_COUNT; n++)
										{
											Actor_ConstructM(&(BumperCars[n].Actor));
											/* force init by setting nCount to -1. */
											BumperCars[n].nCount = -1;
											BumperCars[n].Actor.ActorFrame.TransformationToParent.Translation.V[0] = (float)((rand() * BC_XDIM * 2.f) / RAND_MAX) - BC_XDIM;
											BumperCars[n].Actor.ActorFrame.TransformationToParent.Translation.V[1] = 0.f;
											BumperCars[n].Actor.ActorFrame.TransformationToParent.Translation.V[2] = (float)((rand() * BC_ZDIM * 2.f) / RAND_MAX) - BC_ZDIM;

											/* Set bumper car's model. */
											if (!Actor_SetModel(&(BumperCars[n].Actor), pBumper))
												return 0;		/* Failure. */

											/* Set bumper car's Next actor pointer. (forget about last ptr.) */
											if (n < (BC_COUNT - 1))
											{	BumperCars[n].Actor.pNext = &(BumperCars[n + 1].Actor);
											}
										}
										/* Done. */
										return 1;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}


#ifdef NEVERDEFINED
	if (pCube != NULL)
	{
		Model_LinkToColorManager(pCube, &colmgr);
		/*pLmap0 = ColorManager_GetLightmap256(&colmgr, 0xFFFFFF, 0xFF0000);
		pLmap1 = ColorManager_GetLightmap256(&colmgr, 0xFFFFFF, 0xFFFF00);
		pLmap2 = ColorManager_GetLightmap256(&colmgr, 0xFFFFFF, 0x0000FF);
		pLmap3 = ColorManager_GetLightmap256(&colmgr, 0xFFFFFF, 0xFF00FF);
		*/
		ColorManager_AssignColors(&colmgr);
		/* Model_UpdateColorIndices(pCube, &(colmgr.ColOctree)); */
		/* Fill the colormap we got passed so our screen actually uses
		 * the right colors. */
		for (n = 0; n < 256; n++)
		{	pColormap[n] = colmgr.Colormap[n];
		}
		pColormap[255] = 0xFFFFFF;	/* Force 255 to white. */

		/*pCyl = Model_NewCylinder(10.f, 20.f, 5);
		if (pCyl != NULL)*/
		{
			/* Now build an actor to actually ADD the cube to
			 * the world. */
			Actor_ConstructM(&Ac1);

			/* Specify it's frame. */
			Ac1.ActorFrame.TransformationToParent.Translation.V[0] = 0.f;
			Ac1.ActorFrame.TransformationToParent.Translation.V[1] = 0.2f;
			Ac1.ActorFrame.TransformationToParent.Translation.V[2] = 50.f;
		
			Ac1.pNext = &Ac2;
			
			/* And set the actor's model. */
			if (Actor_SetModel(&Ac1, pCube))
			{
				/* Initialize Ac1's motion. */
				fAc1Angle = 0.f;

				/* Construct a second actor. */
				Actor_ConstructM(&Ac2);

				/* Specify it's frame. */
				Ac2.ActorFrame.TransformationToParent.Translation.V[0] = 0.f;
				Ac2.ActorFrame.TransformationToParent.Translation.V[1] = 0.f;
				Ac2.ActorFrame.TransformationToParent.Translation.V[2] = 230.f;

				/* And set the actor's model. */
				if (Actor_SetModel(&Ac2, pCube))
				{
					/* Specify Ac1 as the parent for it's
					 * transformation. */
					Ac2.ActorFrame.pParent = &Ac1.ActorFrame;
					Ac2.pNext = &Ac3;

					/* Construct a third actor. */
					Actor_Construct(&Ac3);
					
					/* Specify it's frame, above Ac2. */
					Ac3.ActorFrame.TransformationToParent.Translation.V[0] = 0.f;
					Ac3.ActorFrame.TransformationToParent.Translation.V[1] = -230.f;
					Ac3.ActorFrame.TransformationToParent.Translation.V[2] = 0.f;

					/* Set the actor's model. */
					if (Actor_SetModel(&Ac3, pCube))
					{
						/* Specify Ac2 as the parent for it's
						 * transformation. */
						Ac3.ActorFrame.pParent = &Ac2.ActorFrame;
						//Ac3.pNext = &(Acs[0]);
						Ac3.pNext = NULL;

						b = 1;
						for (n = 0; (n < ACS_COUNT) && b; n++)
						{
							Actor_Construct(&(Acs[n]));
							if (n != (ACS_COUNT - 1))
								Acs[n].pNext = &(Acs[n+1]);
							Acs[n].ActorFrame.TransformationToParent.Translation.V[0] = (ACS_DIMS / 2.f) - (float)((rand() * ACS_DIMS) / RAND_MAX);
							Acs[n].ActorFrame.TransformationToParent.Translation.V[1] = (ACS_DIMS / 2.f) - (float)((rand() * ACS_DIMS) / RAND_MAX);
							Acs[n].ActorFrame.TransformationToParent.Translation.V[2] = (ACS_DIMS / 2.f) - (float)((rand() * ACS_DIMS) / RAND_MAX);
							Acs[n].ActorFrame.pParent = &Ac3.ActorFrame;

							if (!Actor_SetModel(&(Acs[n]), pCube))
								b = 0;
						}

						/* We're all done! */
						if (b)
						{	Octree_Destruct(&Oct);
							return 1;
						}
					}
				}
				/* Actor_SetModel() failed. */
				Actor_DestructM(&Ac1);
			}
			
			/* Uh-o, something went wrong (mem failure). */
			Actor_DestructM(&Ac1);

			/*Model_DestructM(pCyl);*/
		}

		Model_DestructM(pCube);
		free((void *)pCyl);
	}

	Viewpoint_DestructM(&Vpoint);
	Octree_Destruct(&Oct);
	ColorManager_Destruct(&colmgr);

	return 0;
}
#endif

/********************************************************************
* Function : StepWorld()
* Purpose : Performs a single iteration in our world's simulation.
********************************************************************/
int StepWorld(char bForward, char bBack, char bLeft, char bRight, int nState, int nGauge)
{
	struct Vector Vec;
	struct Actor *pActor;
	int n;

	/* Rotate around the Z axis of the second actor to some degree. */
	/* Shear the Y component of the X axis. */
	/* Ac1.ActorFrame.TransformationToParent.Rotation[1][0] += 0.2f; */
	/* Shear the Z component of the X axis. */
	/* Ac1.ActorFrame.TransformationToParent.Rotation[2][0] -= 0.1f; */
	/* And force the shear into a rotation. */
	/*Transformation_ForceRotation(&Ac1.ActorFrame.TransformationToParent); */

	/* Handle key input.
	 * Forward means we have to move forward. */
	if (bForward) fSpeed += 0.2f;
	if (bBack) fSpeed -= 0.2f;
	/* Calculate new viewpoint angle. */
	if (bLeft) fAngleSpeed -= 0.01f;
	if (bRight) fAngleSpeed += 0.01f;
	
	/* And call driving routine. */
	Drive(&(Ac1.ActorFrame.TransformationToParent), &fSpeed, &fAngleSpeed, &fAngle);

	/* Perform logic for computer controlled bumper cars. */
	for (n = 0; n < BC_COUNT; n++)
	{	AutoDrive(&(BumperCars[n]));
	}

	/* Determine currently active Actor (by looking at nGauge). */
	if ((nGauge % (BC_COUNT + 1)) == 0)
	{	/* nGauge is % 0, put it in the player's car. */
		pActor = &Ac1;
	} else
	{	/* nGauge is not 0, put it in one of the computer cars. */
		pActor = &(BumperCars[(nGauge % (BC_COUNT + 1)) - 1].Actor);
	}

	/* Determine where to put the Viewpoint. */
	switch (nState)
	{
		case 1 :
			/* F1, Viewport is in bumper car. */
			Transformation_Construct(&Vpoint.VpointFrame.TransformationToParent);
			Vpoint.VpointFrame.pParent = &(pActor->ActorFrame);
			Vpoint.VpointFrame.TransformationToParent.Translation.V[0] = 0.f;
			Vpoint.VpointFrame.TransformationToParent.Translation.V[1] = -0.41f;
			Vpoint.VpointFrame.TransformationToParent.Translation.V[2] = 0.f;
			break;
		case 2 :
			/* F2, Viewport is somewhere. */
			Transformation_Construct(&Vpoint.VpointFrame.TransformationToParent);
			Vpoint.VpointFrame.pParent = NULL;
			Vpoint.VpointFrame.TransformationToParent.Translation.V[0] = 0.f;
			Vpoint.VpointFrame.TransformationToParent.Translation.V[1] = -1.f;
			Vpoint.VpointFrame.TransformationToParent.Translation.V[2] = 0.f;
			break;
		case 3 :
			/* F3, Viewport is behind bumper car. */
			Transformation_Construct(&Vpoint.VpointFrame.TransformationToParent);
			Vpoint.VpointFrame.pParent = &(pActor->ActorFrame);
			Vpoint.VpointFrame.TransformationToParent.Translation.V[0] = 0.f;
			Vpoint.VpointFrame.TransformationToParent.Translation.V[1] = -0.5f;
			Vpoint.VpointFrame.TransformationToParent.Translation.V[2] = -2.f;
			break;
		case 4 :
			/* F4, Viewport is somewhere (as with F2), but now it tracks
			 * the bumper car. */
			Transformation_Construct(&Vpoint.VpointFrame.TransformationToParent);
			Vpoint.VpointFrame.pParent = NULL;
			Vpoint.VpointFrame.TransformationToParent.Translation.V[0] = 0.f;
			Vpoint.VpointFrame.TransformationToParent.Translation.V[1] = -1.f;
			Vpoint.VpointFrame.TransformationToParent.Translation.V[2] = 0.f;
			/* Let Z vector point into direction of bumper car,
			 * Let Y vector point down so we always remain vertical. */
			/* Z vector aiming is first. */
			Vpoint.VpointFrame.TransformationToParent.Rotation[0][2] =
				pActor->ActorFrame.TransformationToParent.Translation.V[0] -
				Vpoint.VpointFrame.TransformationToParent.Translation.V[0];
			Vpoint.VpointFrame.TransformationToParent.Rotation[1][2] =
				pActor->ActorFrame.TransformationToParent.Translation.V[1] -
				Vpoint.VpointFrame.TransformationToParent.Translation.V[1];
			Vpoint.VpointFrame.TransformationToParent.Rotation[2][2] =
				pActor->ActorFrame.TransformationToParent.Translation.V[2] -
				Vpoint.VpointFrame.TransformationToParent.Translation.V[2];
			/* Then the Y vector. */
			Vpoint.VpointFrame.TransformationToParent.Rotation[0][1] = 0.f;
			Vpoint.VpointFrame.TransformationToParent.Rotation[1][1] = 1.f;
			Vpoint.VpointFrame.TransformationToParent.Rotation[2][1] = 0.f;
			/* And build a transformation from this 3x3 matrix. */
			Transformation_ForceRotationZY(&(Vpoint.VpointFrame.TransformationToParent));
			break;
		case 5 :
			/* F5, Viewport tracks bumpercar (as with F4), but now it is in the corner
			 * of the track, high up. */
			Transformation_Construct(&Vpoint.VpointFrame.TransformationToParent);
			Vpoint.VpointFrame.pParent = NULL;
			Vpoint.VpointFrame.TransformationToParent.Translation.V[0] = -89.9f;
			Vpoint.VpointFrame.TransformationToParent.Translation.V[1] = -19.9f;
			Vpoint.VpointFrame.TransformationToParent.Translation.V[2] = -89.9f;
			/* Let Z vector point into direction of bumper car,
			 * Let Y vector point down so we always remain vertical. */
			/* Z vector aiming is first. */
			Vpoint.VpointFrame.TransformationToParent.Rotation[0][2] =
				pActor->ActorFrame.TransformationToParent.Translation.V[0] -
				Vpoint.VpointFrame.TransformationToParent.Translation.V[0];
			Vpoint.VpointFrame.TransformationToParent.Rotation[1][2] =
				pActor->ActorFrame.TransformationToParent.Translation.V[1] -
				Vpoint.VpointFrame.TransformationToParent.Translation.V[1];
			Vpoint.VpointFrame.TransformationToParent.Rotation[2][2] =
				pActor->ActorFrame.TransformationToParent.Translation.V[2] -
				Vpoint.VpointFrame.TransformationToParent.Translation.V[2];
			/* Then the Y vector. */
			Vpoint.VpointFrame.TransformationToParent.Rotation[0][1] = 0.f;
			Vpoint.VpointFrame.TransformationToParent.Rotation[1][1] = 1.f;
			Vpoint.VpointFrame.TransformationToParent.Rotation[2][1] = 0.f;
			/* And build a transformation from this 3x3 matrix. */
			Transformation_ForceRotationZY(&(Vpoint.VpointFrame.TransformationToParent));
			break;
		case 6 :
			/* F6, Heli view of the bumper car (similair to F3 but now more distant).
			*/
			Transformation_Construct(&Vpoint.VpointFrame.TransformationToParent);
			Vpoint.VpointFrame.pParent = &(pActor->ActorFrame);
			Vpoint.VpointFrame.TransformationToParent.Translation.V[0] = 0.f;
			Vpoint.VpointFrame.TransformationToParent.Translation.V[1] = -0.5f * 15;
			Vpoint.VpointFrame.TransformationToParent.Translation.V[2] = -2.f * 15;
			break;

	}

	return 1;
}

/********************************************************************
* Function : PrepDrawWorld()
* Purpose : Prepares drawing the world without actually drawing
*           anything. During this (CPU intensive) step, some blitters
*           can run and clear the bitmap.
********************************************************************/
int PrepDrawWorld(void)
{
	/* Everything is linked to the Arena, so submit that object. */
	return Viewpoint_PrepActorsForDraw(&Vpoint, &AcArena);
}

/********************************************************************
* Function : DrawWorld()
* Purpose : Performs actual drawing to a screen.
********************************************************************/
void DrawWorld(unsigned char *pScreen, int nBytesPerRow)
{
	Vpoint.pBitmap = pScreen;
	Vpoint.nPixelRow = nBytesPerRow;
	Viewpoint_Draw(&Vpoint);

	/* Draw Chrome texture. */
#ifdef UNDEFINED
	TextureMap_Draw(&ChromeBmp, 27,47, 203, 63, nBytesPerRow-203, 0, pScreen, nBytesPerRow);
	TextureMap_Draw(&ChromeBmp, 0, 0, 256, 256, 0, 0, pScreen, nBytesPerRow);
	TextureMap_Draw(&ThemRoots, 0, 0, 256, 256, 256, 0, pScreen, nBytesPerRow);
	TextureMap_Draw(&TubularHaze, 0, 0, 640-512, 256, 512, 0, pScreen, nBytesPerRow);
#endif
}

/********************************************************************
* Function : EndWorld()
* Purpose : Cleans up the world.
********************************************************************/
void EndWorld(void)
{	int n;
	Viewpoint_DestructM(&Vpoint);
	Actor_DestructM(&AcArena);
	Actor_DestructM(&Ac1);
	Actor_DestructM(&Ac2);
	/* Actor_DestructM(&Ac3);
	for (n = 0; n < ACS_COUNT; n++)
		Actor_Destruct(&(Acs[n]));
	*/
	Model_DestructM(pCube);
	free((void *)pCube);
	Model_DestructM(pBumper);
	free((void *)pBumper);
	Model_DestructM(pArena);
	free((void *)pArena);
	ColorManager_Destruct(&colmgr);
}

/* Small 'Drive' function for driving. */
void Drive(struct Transformation *pTrans, float *pfSpeed, float *pfAngleSpeed, float *pfAngle)
{
	/* A strong rotational speed tilts the transformation around
	 * the Z axis slightly. */
	float fZRot;
	struct Vector vec;
	struct Transformation trans;

	/* Do some characteristics of driving. */
	*pfSpeed *= 0.9f;
	*pfAngle += *pfAngleSpeed;
	*pfAngleSpeed *= 0.7f;

	fZRot = *pfAngleSpeed * 5.f * *pfSpeed;

	/* Build a transformation for this Z angle.
	 * This initializes the rotation matrix. */
	vec.V[0] = 0.f;
	vec.V[1] = 0.f;
	vec.V[2] = 1.f;
	Transformation_MakeRotateArbitrary(pTrans, &vec, -fZRot);

	/* Next we also need to rotate around the Y axis for the direction
	 * in which the transformation is moving. */
	Transformation_Construct(&trans);
	vec.V[0] = 0.f;
	vec.V[1] = 1.f;
	vec.V[2] = 0.f;
	Transformation_MakeRotateArbitrary(&trans, &vec, *pfAngle);

	/* Concatenate the two transformations to yield a third, final,
	 * transformation. */
	Transformation_ConcatenateRotation(&trans, pTrans, pTrans);

	/* Now update the new position. */
	pTrans->Translation.V[0] += *pfSpeed * trans.Rotation[0][2];
	/* pTrans->Translation.V[1] += *pfSpeed * trans.Rotation[1][2];*/
	pTrans->Translation.V[2] += *pfSpeed * trans.Rotation[2][2];

	/* Done. */
}



/* Small automatic bumpercar handling function. */
void AutoDrive(struct BumperCarState *pState)
{
	if (pState->nCount < 0)
	{	/* Init. */
		pState->fSpeed = 0.f;
		pState->fAngle = 0.f;
		pState->fAngleSpeed = 0.f;
	}

	if (pState->nCount <= 0)
	{	/* New action required. */

		/* Define new Random speed. */
		/* Forward is Maximum speed, Backward is 50% speed. */
		// pState->fSpeed = (float)((rand() * BC_MSPD * 1.5f) / RAND_MAX) - BC_MSPD * 0.5f;
		// pState->fAngleSpeed = (float)((rand() * BC_MROT * 2.f) / RAND_MAX) - BC_MROT;

		pState->fSpeedAccel = (float)((rand() * BC_MSPD * 1.5f) / RAND_MAX) - BC_MSPD * 0.5f;
		pState->fAngleAccel = (float)((rand() * BC_MROT * 2.f) / RAND_MAX) - BC_MROT;

		/* Max. # of frames to wait for new decision. */
		pState->nCount = rand() / ( RAND_MAX / BC_DECI );
	}

	// Check bounds.
	if (pState->Actor.ActorFrame.TransformationToParent.Translation.V[0] <= (-BC_XDIM))
	{	pState->Actor.ActorFrame.TransformationToParent.Translation.V[0] = -BC_XDIM;
//		pState->nCount = 0;
	}
	if (pState->Actor.ActorFrame.TransformationToParent.Translation.V[0] >= BC_XDIM)
	{	pState->Actor.ActorFrame.TransformationToParent.Translation.V[0] = BC_XDIM;
//		pState->nCount = 0;
	}
	if (pState->Actor.ActorFrame.TransformationToParent.Translation.V[2] <= (-BC_ZDIM))
	{	pState->Actor.ActorFrame.TransformationToParent.Translation.V[2] = -BC_ZDIM;
//		pState->nCount = 0;
	}
	if (pState->Actor.ActorFrame.TransformationToParent.Translation.V[2] >= BC_ZDIM)
	{	pState->Actor.ActorFrame.TransformationToParent.Translation.V[2] = BC_ZDIM;
//		pState->nCount = 0;
	}

	if (pState->nCount != 0)
	{
		pState->fSpeed += pState->fSpeedAccel;
		pState->fAngle += pState->fAngleAccel;

		pState->nCount--;

		Drive(&(pState->Actor.ActorFrame.TransformationToParent), &(pState->fSpeed), &(pState->fAngleSpeed), &(pState->fAngle));
	}
}

