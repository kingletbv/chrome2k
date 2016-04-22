/********************************************************************
* FILE : dxmain.cpp
* Purpose : Main file for DirectX (DirectDraw).
* Description : This is the main file for using Chrome to interface
*               with Microsoft's DirectDraw interface.
********************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include <ddraw.h>
#include <stdlib.h>

#define CLEARBCKGRND	1		/* 1 = Clearbackground,
								 * 0 = Assume full 3D coverage of background. */

#define XRES 640
#define YRES 400

/* Include our Testworld. */
extern "C" {
	#include "tstworld.h"
}

HFONT						AppFont = NULL;		// Font used for all text in apps.
LPDIRECTDRAW			lpDirectDrawObject;	// DD object
LPDIRECTDRAWSURFACE	lpPrimary;				// DD primary surface
LPDIRECTDRAWSURFACE	lpBackbuffer;			// DD back buffer surface
LPDIRECTDRAWPALETTE	lpPalette;				// DD palette.
BOOL						ActiveApp;				// Is this program active ?
BOOL						RealTime;				// Should we run RealTime ?

BOOL						bForwardKey;			// If TRUE, key to go forward is down.
BOOL						bBackKey;				// If TRUE, key to go back is down.
BOOL						bLeftKey;				// If TRUE, key to turn left is down.
BOOL						bRightKey;				// If TRUE, key to turn right is down.
int						nState;					// State, used for keys 0..9, switchable.
int						nGauge;					// Count, used for + and -.

unsigned long			Colormap[256];			// Colormap entries, 0xRRGGBB encoded.
PALETTEENTRY			WinColormap[256];		// Colormap entries, now in PALETTEENTRY
														// windows format.
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
						 LPSTR lpCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hwnd, UINT message,
									WPARAM wParam, LPARAM lParam);
int SimLoop(void);	// Performs a single simulation loop,
							// including drawing.
/********************************************************************
* Function : WinMain()
* Purpose : Mandatory Windows Init function.
********************************************************************/
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
						 LPSTR lpCmdLine, int nCmdShow)
{
	MSG		msg;
	HWND		hwnd;
	WNDCLASS	wc;
	static char ClassName[] = "ChromeTestingFacility";
	DDSURFACEDESC	ddsd;
	DDSCAPS			ddscaps;
	HRESULT			ddreturn;
	int n;

	// Set all key booleans to FALSE, assume no key is pressed.
	bForwardKey = FALSE;
	bBackKey = FALSE;
	bLeftKey = FALSE;
	bRightKey = FALSE;
	nState = 0;
	nGauge = 0;

	lpCmdLine = lpCmdLine;
	hPrevInstance = hPrevInstance;
	RealTime = 0;	/* Start of using spacebar for frameflipping. */

	/* Register and realize our display window */
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = ClassName;
	wc.lpszClassName = ClassName;
	RegisterClass(&wc);

	/* Initialize our test world. */
	if (!InitWorld(XRES, YRES, Colormap))
	{	return FALSE;
	}

	/* Convert the Chrome colormap to a windows colormap. */
	for (n = 0; n < 256; n++)
	{
		WinColormap[n].peRed = (unsigned char)((Colormap[n] & 0xFF0000) >> 16);
		WinColormap[n].peGreen = (unsigned char)((Colormap[n] & 0xFF00) >> 8);
		WinColormap[n].peBlue = (unsigned char)((Colormap[n] & 0xFF));
		WinColormap[n].peFlags = 0;
	}
	/* Create a full screen window so that GDI won't ever be
	 * called. */
	hwnd = CreateWindowEx(WS_EX_TOPMOST,
								 ClassName,
								 ClassName,
								 WS_POPUP,
								 0,
								 0,
								 GetSystemMetrics(SM_CXSCREEN),
								 GetSystemMetrics(SM_CYSCREEN),
								 NULL,
								 NULL,
								 hInstance,
								 NULL);
	if (hwnd == NULL)
		return FALSE;
	
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	SetFocus(hwnd);
	ShowCursor(FALSE);		/* Remove cursor to prevent GDI from writing. */

	/* Instanciate our DirectDraw object */
	ddreturn = DirectDrawCreate(NULL, &lpDirectDrawObject, NULL);
	if (ddreturn != DD_OK)
	{
		DestroyWindow(hwnd);
		return FALSE;
	}

	ddreturn = lpDirectDrawObject->SetCooperativeLevel(hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN| DDSCL_ALLOWMODEX);
	if (ddreturn != DD_OK)
	{
		DestroyWindow(hwnd);
		return FALSE;
	}

	/* Create a palette for the surfaces. */
	ddreturn = lpDirectDrawObject->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256 | DDPCAPS_INITIALIZE,
																(LPPALETTEENTRY)WinColormap,
																&lpPalette,
																NULL);
	if (ddreturn != DD_OK)
	{	DestroyWindow(hwnd);
		return FALSE;
	}

	/* Set the video mode to XRESxYRESx8. */
	ddreturn = lpDirectDrawObject->SetDisplayMode(XRES, YRES, 8);
	if (ddreturn != DD_OK)
	{	DestroyWindow(hwnd);
		return FALSE;
	}

	/* Create a default font for the application. */
	AppFont = CreateFont(11,
								0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
								ANSI_CHARSET,
								OUT_DEFAULT_PRECIS,
								CLIP_DEFAULT_PRECIS,
								NONANTIALIASED_QUALITY,
								VARIABLE_PITCH,
								"Comic Sans MS");

	/* Create the primary surface and one back buffer surface */
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
								 DDSCAPS_FLIP |
								 DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount = 1;
	ddreturn = lpDirectDrawObject->CreateSurface(&ddsd, &lpPrimary, NULL);

	if (ddreturn != DD_OK)
	{	DestroyWindow(hwnd);
		return FALSE;
	}

	ddreturn = lpPrimary->SetPalette(lpPalette);
	if (ddreturn != DD_OK)
	{	DestroyWindow(hwnd);
		return FALSE;
	}

	/* Get a surface pointer to our back buffer. */
	ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
	ddreturn = lpPrimary->GetAttachedSurface(&ddscaps, &lpBackbuffer);

	if (ddreturn != DD_OK)
	{	DestroyWindow(hwnd);
		return FALSE;
	}

/*	ddreturn = lpBackbuffer->SetPalette(lpPalette);
	if (ddreturn != DD_OK)
	{	DestroyWindow(hwnd);
		return FALSE;
	}
*/
	{	/* Clear the background once for both buffers so we don't get anoying flicker effect. */
		DDBLTFX	BltFx;
		BltFx.dwSize = sizeof(BltFx);
		BltFx.dwFillColor = 255;
		ddreturn = lpBackbuffer->Blt(NULL,
											  NULL,
											  NULL,
											  DDBLT_COLORFILL | DDBLT_WAIT,
											  &BltFx);
		BltFx.dwSize = sizeof(BltFx);
		BltFx.dwFillColor = 255;
		ddreturn = lpPrimary->Blt(NULL,
											  NULL,
											  NULL,
											  DDBLT_COLORFILL | DDBLT_WAIT,
											  &BltFx);
	}

	while (1)
	{	if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{	if (!GetMessage(&msg, NULL, 0, 0))
				return msg.wParam;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else
		{	if (ActiveApp && RealTime)
			{	// Simulation Iteration should go here.
				// Only do this when running Realtime.
				SimLoop();
			} else
			{	WaitMessage();
			}
		}
	}
}

/********************************************************************
* Function : WindowProc()
* Purpose : Receive and handle windows messages.
********************************************************************/
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

/********************************************************************
* Function : SimLoop()
* Purpose : Performs a single Simulation Loop iteration. Includes
*           drawing.
********************************************************************/
int SimLoop(void)
{
	static int nFramesPerSecond = 0;
	static int nFramesSinceLastTick;
	static DWORD LastTicks = 0;
	DWORD Ticks;
	HDC hDC;
	HFONT hOldFont;
	char s[80];
	int slen;

	DDSURFACEDESC ddsd;
	DDBLTFX	BltFx;
	HRESULT ddreturn;

	/* Perform a single step in our world. */
	if (StepWorld(bForwardKey, bBackKey, bLeftKey, bRightKey, nState, nGauge))
	{
		if (lpPrimary->IsLost() == DDERR_SURFACELOST)
			lpPrimary->Restore();

		/* Clear the backbuffer. */
#if CLEARBCKGRND
		BltFx.dwSize = sizeof(BltFx);
		BltFx.dwFillColor = 255;
		ddreturn = lpBackbuffer->Blt(NULL,
											  NULL,
											  NULL,
											  DDBLT_COLORFILL | DDBLT_WAIT,
											  &BltFx);
#else
		ddreturn = DD_OK;
#endif
		if (ddreturn == DD_OK)
		{	/* While this is running, prepare
			 * the drawing. */
			if (PrepDrawWorld())
			{
				/* Lock the surface. */
				memset(&ddsd, 0, sizeof(DDSURFACEDESC));
				ddsd.dwSize = sizeof(ddsd);
				ddreturn = lpBackbuffer->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
				if (ddreturn == DD_OK)
				{
					DrawWorld((unsigned char *)ddsd.lpSurface, (int)ddsd.lPitch);

					int nX, nY;
					static unsigned char dummy;
					unsigned char ni;
					ni = 0;
					for (nY = 0; nY < 16; nY++)
						for (nX = 0; nX < 16; nX++)
						{
							/* Draw a small block at (nX * 3, nY * 3) */
							((unsigned char *)ddsd.lpSurface)[(nY * 3 * ddsd.lPitch) + (nX * 3)] = ni;
							((unsigned char *)ddsd.lpSurface)[(nY * 3 * ddsd.lPitch) + (nX * 3 + 1)] = ni;
							((unsigned char *)ddsd.lpSurface)[((nY * 3 + 1) * ddsd.lPitch) + (nX * 3)] = ni;
							((unsigned char *)ddsd.lpSurface)[((nY * 3 + 1) * ddsd.lPitch) + (nX * 3 + 1)] = ni;
							ni++;
						}
					lpBackbuffer->Unlock(NULL);

					/* And now write Frames per second. */
					/* Increment Frame counter. */
					nFramesSinceLastTick++;
					/* Get system tick count. */
					Ticks = GetTickCount();
					/* Update fps value every second. */
					if (Ticks > (LastTicks + 1000))
					{	nFramesPerSecond = nFramesSinceLastTick;
						nFramesSinceLastTick = 0;
						LastTicks = Ticks;
					}

					/* Get a DC to the buffer & write count. */
					if (DD_OK == lpBackbuffer->GetDC(&hDC))
					{	
						SetBkMode(hDC, TRANSPARENT);
						hOldFont = SelectObject(hDC, AppFont);
						/* Build a string for display. */
						slen = wsprintf(s, "FPS : %d", nFramesPerSecond);
						/* And draw the text. */
						SetTextColor(hDC, RGB(0,0,0));
						SIZE sz;
						GetTextExtentPoint32(hDC, s, slen, &sz);
						RECT rc;
						rc.top = 0;
						rc.left = 16 * 3;
						rc.right = 16 * 3 + sz.cx + 10;
						rc.bottom = sz.cy + 10;
						DrawFrameControl(hDC, &rc, DFC_BUTTON, DFCS_BUTTONPUSH);
						TextOut(hDC, 16*3 + 5, 5, s, slen);
						SelectObject(hDC, hOldFont);
						lpBackbuffer->ReleaseDC(hDC);
					}
					/* Perform required pageflipping to make the surface
					 * we drawed visible. */
					ddreturn = lpPrimary->Flip(NULL, DDFLIP_WAIT);
					if (ddreturn == DD_OK)
					{
						return 1;
					}
				} 
			}
		}
	}

	return 0;
}