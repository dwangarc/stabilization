#pragma once

#include <afxwin.h>

class CDeStabilizeDlg : public CDialog
{
	DECLARE_DYNAMIC(CDeStabilizeDlg)

public:
	CDeStabilizeDlg(CWnd* pParent = NULL);   // standard constructor

	double shiftX, shiftY, shiftAngle;

	enum { IDD = IDD_DESTAB_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

};
