// Stabilize.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Stabilize.h"
#include "Include/frames_reader.h"
//#include "stab_private.h"
#include "ImageProc/inc/pictureConverter.h"
#include "Mediator/mediator.h"
#include "Include/capture_log.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int, HWND &hWnd);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

Mediator* mediator;

// thread function
static DWORD WINAPI threadproc(LPVOID lpParam)
{
	HWND *m_hwnd = (HWND*)lpParam;
	
	while(1)
	{
		Sleep(40);
		RedrawWindow(*m_hwnd, 0, 0, RDW_INVALIDATE);
	}
	
	return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_STABILIZE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	HWND hwnd;
	MediatorParams params = Mediator::getParams("params.in");
	mediator = new Mediator("Snapshots", params);
	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow, hwnd))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_STABILIZE));

	// start new frame thread
	HANDLE m_thread = ::CreateThread(NULL, 0, threadproc, &hwnd, 0, 0);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_STABILIZE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_STABILIZE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND &hWnd)
{
   //HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	int width = mediator->getWidth();
	int height = mediator->getHeight();

	BITMAPINFOHEADER cbmi;
	memset(&cbmi, 0, sizeof(BITMAPINFOHEADER));
	int band = 0;
/*	cbmi.biWidth	= width - 2 * band;
	cbmi.biHeight	= height - 2 * band;*/
	cbmi.biPlanes	= 1;
	cbmi.biSize		= sizeof(BITMAPINFOHEADER);
	cbmi.biBitCount	= 24;
//	cbmi.
	static int col = 0;

	RECT r;
	::GetClientRect( hWnd, &r );

	int lx = (r.right - width + band) / 2;
	int ly = (r.bottom - height + band) / 2;
	unsigned char* resPic = 0;
	unsigned char* convPic = 0;


	Homography homo;
	Picture pic(0, 0);
/*	unsigned char* tmpImage = new unsigned char[width * height * 3];
	unsigned char const* img;*/
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			delete mediator;
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...

		col = ( col == 255 ? 0 : col + 1 );
		

		// get last frame :
		mediator->nextProjStep();
//		homo = mediator->getHomo();
//		homo.printLog();
/*		convPic = picConverter.applyHomo(mediator.getFrame(), homo);
		resPic = picConverter.convertGrey(convPic, 3);*/


//		stabilize_function( bitMap );
		//memset( bitMap->data, col, 640 * 480 * 3 );
		
		// stretch to window
/*		img = mediator.getFrame();
		for (int i = 0; i < width * height * 3; ++i)
		{
			tmpImage[i] = img[i];
		}**/
		pic = mediator->getFrame();
		cbmi.biWidth = pic.getWidth();
		cbmi.biHeight = pic.getHeight();
		::StretchDIBits( hdc, lx, ly, pic.getWidth(), pic.getHeight(), 0, 0, pic.getWidth(), pic.getHeight(), /*bitMap->data*//*gaussPic*//* resPic*/pic.getPictureChar(), (BITMAPINFO*)&cbmi/*pbmi*/, DIB_RGB_COLORS, SRCCOPY );
//		delete[] tmpImage;
		delete[] resPic;
		delete[] convPic;

		// release bitmap
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		delete mediator;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
