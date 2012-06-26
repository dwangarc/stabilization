#pragma once

#include "resource.h"
#include <afxwin.h>

class CMotionDlg : public CDialog
{
	DECLARE_DYNAMIC(CMotionDlg)

public:
	CMotionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMotionDlg();

	// Dialog Data
	enum { IDD = IDD_MOTION_DIALOG};

public:

	int eps, updateIter;
	int minObj, minSpace;
	int baseEps, smooth, downscale;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
