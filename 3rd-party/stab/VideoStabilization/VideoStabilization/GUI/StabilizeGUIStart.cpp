
#include "stdafx.h"

#include "MainFrm.h"
#include "StabilizeGUIStart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



BEGIN_MESSAGE_MAP(CStabilizeApp, CWinApp)
END_MESSAGE_MAP()



CStabilizeApp::CStabilizeApp()
{
}


CStabilizeApp theApp;


BOOL CStabilizeApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	CWinApp::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object

	//	pFrame->set_net_mediator(mediator);

	CMainFrame* pFrame = new CMainFrame(); 
	MediatorParams params = MediatorParams::getParams();
	Mediator* mediator = new Mediator(params);
	mediator->setFrame(pFrame);
	pFrame->setMediator(mediator);

	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;

	// create and load the frame with its resources
	pFrame->LoadFrame(IDC_STABILIZE, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);

	// force a title-bar icon (MFC Wizard doesn't set it up correctly) 
	HICON hIcon		= LoadIcon(IDI_STABILIZE); 
	HICON hPrevIcon	= pFrame->SetIcon(hIcon, TRUE); 
	if (hPrevIcon && hPrevIcon != hIcon) DestroyIcon(hPrevIcon);

	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_MAXIMIZE);
	pFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}


// CCaptureApp message handlers




// CAboutDlg dialog used for App About


CStabilizeApp::~CStabilizeApp()
{
}
