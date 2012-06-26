#pragma once

#include <afxwin.h>

class CDistortionDlg : public CDialog
{
	DECLARE_DYNAMIC(CDistortionDlg)

public:
	CDistortionDlg(CWnd* pParent = NULL);   // standard constructor

	double params[8];
	bool compensateDistortions;

	enum { IDD = IDD_DISTORTION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

};
