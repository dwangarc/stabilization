// Tracking 1.0.h : main header file for the Tracking 1.0 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CTrackingApp:
// See Tracking 1.0.cpp for the implementation of this class
//

class CTrackingApp : public CWinApp
{
public:
	CTrackingApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CTrackingApp theApp;