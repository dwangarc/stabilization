#pragma once

#include <afxwin.h>

class CParamsDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamsDlg)

public:
	CParamsDlg(CWnd* pParent = NULL);   // standard constructor

	int iterNum[3];
	int pyrSize, downscale, bandWidth;
	int cornerDistance;
	int shiftParam, safeShift;
	int bgr;
	double alpha;

	enum { IDD = IDD_PARAMS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

};
