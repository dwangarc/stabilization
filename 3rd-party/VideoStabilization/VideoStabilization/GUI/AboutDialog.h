#pragma once

#include "Resource.h"
#include <afxwin.h>

class CAboutDlg : public CDialog
{
public:
	CAboutDlg(CWnd* parent = 0);

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

