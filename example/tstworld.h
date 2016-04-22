/********************************************************************
* FILE : tstworld.h
* Purpose : Header file for Chrome testing routines.
*           Routines here are called from the file that sets things
*           up for a platform. (Which is "dxmain.cpp" for
*           Windows' DirectDraw).
********************************************************************/
#ifdef __cplusplus
	#define C_STYLE extern "C"
#else
	#define C_STYLE
#endif

#ifndef TSTWORLD_H
#define TSTWORLD_H

/* InitWorld(),
 * Initializes the world.
 * Screen resolution MUST be specified!
 * If it returns 0, a failure occured. */
C_STYLE int InitWorld( int nSWidth, int nSHeight, unsigned long *pColormap);

/* StepWorld(),
 * Performs one iteration in the simulation loop.
 * This moves some objects & draws them to the screen.
 * The pointer to the screen MUST be specified!
 * If it returns 0, a failure occured.
 */
C_STYLE int StepWorld(char bForward, char bBack, char bLeft, char bRight, int nState, int nGauge);

/* PrepDrawWorld(),
 * Prepares rendering the world. This doesn't write to any
 * bitmap and therefore allows hardware blitters to run during
 * this step.
 * If it returns 0, a failure occured and DrawWorld should NOT
 * be called. */
C_STYLE int PrepDrawWorld(void);

/* DrawWorld(),
 * Renders the world, assumes the bitmap (pScreen) to be clear.
 * pScreen MUST be specified!
 */
C_STYLE void DrawWorld(unsigned char *pScreen, int nBytesPerRow);

/* EndWorld(),
 * This shuts down the world, freeing all allocated
 * memory. */
C_STYLE void EndWorld(void);

#endif
