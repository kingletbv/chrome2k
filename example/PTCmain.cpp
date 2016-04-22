/********************************************************************
* FILE : PTCmain.cpp
* Purpose : Main file for OpenPTC
* Description : This is the main file for using Chrome to interface
*               with OpenPTC interface.
********************************************************************/

#define XRES 640
#define YRES 480

#include "tstworld.h"
#include <ptc.h>

bool						bForwardKey;			// If TRUE, key to go forward is down.
bool						bBackKey;				// If TRUE, key to go back is down.
bool						bLeftKey;				// If TRUE, key to turn left is down.
bool						bRightKey;				// If TRUE, key to turn right is down.
int						nState;					// State, used for keys 0..9, switchable.
int						nGauge;					// Count, used for + and -.
Surface* pSurface = NULL;


int32			_Colormap[256];			// Colormap entries, 0xRRGGBB encoded.

int SimLoop(void);	// Performs a single simulation loop,
							// including drawing.

int DrawLoop(int e, int i);
/********************************************************************
* Function : WinMain()
* Purpose : Mandatory Windows Init function.
********************************************************************/
int main(void)
{
	int n;
	// Create a PTC window
	Console* pConsole = new Console;
	if( !pConsole ) return -1;
	pConsole->open("Chrome test facility",XRES,YRES, Format(32,0xFF0000,0xFF00,0xFF,0x00) );

	// Set all key booleans to FALSE, assume no key is pressed.
	bForwardKey = false;
	bBackKey = false;
	bLeftKey = false;
	bRightKey = false;
	nState = 0;
	nGauge = 0;

//	RealTime = 0;	/* Start of using spacebar for frameflipping. */

	/* Initialize our test world. */
	if ( !InitWorld( XRES, YRES, (unsigned long*) _Colormap) )
	{	return -1;
	}

	// Create palette
	Palette palette;
	palette.load(_Colormap);
	pConsole->palette(palette);



	/* Instanciate our DirectDraw object */
	pSurface = new Surface( XRES, YRES, Format(8) );
	pSurface->palette(palette);
//	pSurface = new Surface( XRES , YRES , Format(32,0xFF0000,0xFF00,0xFF,0x00) );

	nState = 1;
	nGauge = 2;
	while (1)
	{
		if(pConsole->key())
		{	Key key = pConsole->read();
			bForwardKey = true;
			bBackKey = true;
			bLeftKey = true;
			bRightKey = true;
			switch( key.code() )
			{
				case Key::UP:
					bForwardKey = true;
					break;
				case Key::DOWN:
					bBackKey = true;
					break;
				case Key::LEFT:
					bLeftKey = true;
					break;
				case Key::RIGHT:
					bRightKey = true;
				case Key::SPACE:
					break;
				case Key::ESCAPE:
					return 0;
				default:
					break;

				case Key::F1:
				case Key::F2:
				case Key::F3:
				case Key::F4:
				case Key::F5:
				case Key::F6:
				case Key::F7:
				case Key::F8:
				case Key::F9:
				case Key::F10:
					nState = key.code() - Key::F1 + 1;
					break;
			}
		}
		SimLoop();
		pSurface->copy(*pConsole);
		pConsole->update();
	}
}



#ifdef jsjhdujahfckjdhkjhskjfhjksdhfkjhsdjfkhjdskhfjk
/********************************************************************
* Function : WindowProc()
* Purpose : Receive and handle windows messages.
*********************************************************************/
long FAR PASCAL WindowProc(HWND hwnd, UINT message,
									WPARAM wParam, LPARAM lParam)
{
	DDSURFACEDESC ddsd;
	DDBLTFX	BltFx;
	HRESULT ddreturn;

	switch (message)
	{
		case WM_ACTIVATEAPP :
			ActiveApp = wParam;
			break;

		case WM_CREATE :
			break;

		case WM_KEYDOWN :
			switch (wParam)
			{
				case VK_PAUSE :	/* Start or Stop realtime. */
					RealTime = !RealTime;
					break;
				case VK_SPACE :	/* Flip a single frame. */
					SimLoop();
					break;
				case VK_ESCAPE :
					DestroyWindow(hwnd);
					break;
				case VK_LEFT :
					bLeftKey = TRUE;
					break;
				case VK_RIGHT :
					bRightKey = TRUE;
					break;
				case VK_UP :
					bForwardKey = TRUE;
					break;
				case VK_DOWN :
					bBackKey = TRUE;
					break;
				case VK_ADD :
					nGauge++;
					break;
				case VK_SUBTRACT :
					nGauge--;
					break;
				case VK_HOME :
					nGauge = 0;
					break;
			}
			break;

		case WM_KEYUP :
			switch (wParam)
			{
				case VK_F1 :
					nState = 1;
					break;
				case VK_F2 :
					nState = 2;
					break;
				case VK_F3 :
					nState = 3;
					break;
				case VK_F4 :
					nState = 4;
					break;
				case VK_F5 :
					nState = 5;
					break;
				case VK_F6 :
					nState = 6;
					break;
				case VK_F7 :
					nState = 7;
					break;
				case VK_F8 :
					nState = 8;
					break;
				case VK_F9 :
					nState = 9;
					break;
				case VK_F10 :
					nState = 0;
					break;
				case VK_LEFT :
					bLeftKey = FALSE;
					break;
				case VK_RIGHT :
					bRightKey = FALSE;
					break;
				case VK_UP :
					bForwardKey = FALSE;
					break;
				case VK_DOWN :
					bBackKey = FALSE;
					break;
				case 'S' :
					// Screen Snapshot.
					// This is not done very reliably, infact, it's more of a hack so
					// I can show some Chrome snapshots on my website.
					// Open a file named "Snapshot.tga" and write the current content
					// of the screen as a targa file.
					HANDLE hFile;
					hFile = CreateFile("Snapshot.tga", GENERIC_WRITE, 0, NULL, 
										CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
										NULL);
					if (hFile != NULL)
					{
						// Write the Targa header, as appropriate for the current
						// screen type.
						DWORD dwWritten;
					   /* Targa File Header : (Little Endian / Intel)
						* Offset Length  Description
						*      0      1  ID field length
						*      1      1  Color map type.
						*      2      1  Image type.
						*      3      2  First color map entry.
						*      5      2  Color map length.
						*      7      1  Color map entry size.
						*      8      2  Image X origin.
						*     10      2  Image Y Origin.
						*     12      2  Image Width.
						*     14      2  Image Height.
						*     16      1  Bits per pixel.
						*     17      1  Image descriptor bits.
						* (from : "Graphics File Formats", David C. Kay and John R. Levine)
						*/
#						define OutB(b)\
						{	unsigned char b2;\
							b2 = b;\
							WriteFile(hFile, &b2, sizeof(b2), &dwWritten, NULL);\
						}
#						define OutW(w)\
						{	unsigned short w2;\
							w2 = w;\
							WriteFile(hFile, &w2, sizeof(w2), &dwWritten, NULL);\
						}
						OutB(0);		// No message ID field length.
						OutB(0);		// Colormap type = 0; true color.
						OutB(2);		// Image type = 2, True color, uncompressed.
						OutW(0);		// First colormap entry is 0, (Because Cmap type = 0).
						OutW(0);		// Colormap length = 0, (CMap type = 0).
						OutB(0);		// CMap entry size is 0 (because CMap type = 0)
						OutW(0);		// OriginX = 0;
						OutW(0);		// OriginY = 0;
						OutW(XRES);		// Image Width is the value of the XRES constant.
						OutW(YRES);		// Image Height is the value of the YRES constant.
						OutB(24);		// Bits per pixel.
						OutB(0x20);		// Image Descriptor Bits (0x20 = left to right, top to bottom).

						// Each pixel is written as a Blue Green Red triple.

						// Lock the surface & write each byte.
						DDSURFACEDESC ddsd;
						memset(&ddsd, 0, sizeof(DDSURFACEDESC));
						ddsd.dwSize = sizeof(ddsd);
						ddreturn = lpBackbuffer->Lock(NULL, &ddsd, DDLOCK_READONLY | DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
						if (ddreturn == DD_OK)
						{
							//DrawWorld((unsigned char *)ddsd.lpSurface, (int)ddsd.lPitch);
							int nX, nY;
							for (nY = 0; nY < YRES; nY++)
							{
								for (nX = 0; nX < XRES; nX++)
								{
									unsigned char ucByte;
									ucByte = *(((unsigned char *)ddsd.lpSurface) + ddsd.lPitch * nY + nX);

									// Index the colormap & write the BGR triple.
									OutB(WinColormap[ucByte].peBlue);
									OutB(WinColormap[ucByte].peGreen);
									OutB(WinColormap[ucByte].peRed);
								}
							}
							lpBackbuffer->Unlock(NULL);
						}
						// Close the file, we're done...
						CloseHandle(hFile);
					}
			}
			break;

		case WM_DESTROY :
			if (lpDirectDrawObject != NULL)
			{
				if (lpBackbuffer != NULL)
					lpBackbuffer->Release();
				if (lpPrimary != NULL)
					lpPrimary->Release();
				if (lpPalette != NULL)
					lpPalette->Release();
				lpDirectDrawObject->Release();
			}
			/* Free the world. */
			EndWorld();

			/* Free the font. */
			if (AppFont != NULL)
				DeleteObject(AppFont);

			ShowCursor(TRUE);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0L;
}
#endif



/********************************************************************
* Function : SimLoop()
* Purpose : Performs a single Simulation Loop iteration. Includes
*           drawing.
********************************************************************/
int SimLoop(void)
{
	static int nFramesPerSecond = 0;
	static int nFramesSinceLastTick;
	static int LastTicks = 0;
	unsigned char* buffer;
	int Ticks;
	char s[80];
	int slen;
	/* Perform a single step in our world. */
	if (StepWorld(bForwardKey, bBackKey, bLeftKey, bRightKey, nState, nGauge))
	{
//		pSurface->clear( Color(0,0,0) );
		if (PrepDrawWorld())
		{
			buffer = reinterpret_cast<unsigned char*> (pSurface->lock());
			DrawWorld(buffer, XRES);

					int nX, nY;
					static unsigned char dummy;
					unsigned char ni;
					ni = 0;
					for (nY = 0; nY < 16; nY++)
						for (nX = 0; nX < 16; nX++)
						{
							/* Draw a small block at (nX * 3, nY * 3) */
//							buffer[(nY * 3 * XRES) + (nX * 3)] = ni;
//							buffer[(nY * 3 * XRES) + (nX * 3 + 1)] = ni;
//							buffer[((nY * 3 + 1) * XRES) + (nX * 3)] = ni;
//							buffer[((nY * 3 + 1) * XRES) + (nX * 3 + 1)] = ni;
							ni++;
						}
	
			pSurface->unlock();
		}
		return 1;
	}
	return 0;
}