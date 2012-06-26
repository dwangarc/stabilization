// Capture.h : main header file for the Capture application
//
#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CCaptureApp:
// See Capture.cpp for the implementation of this class
//

class CStabilizeApp : public CWinApp
{
public:
	CStabilizeApp();
	~CStabilizeApp();

	// Overrides
public:
	virtual BOOL InitInstance();

	// Implementation
public:
	DECLARE_MESSAGE_MAP()
};

extern CStabilizeApp theApp;