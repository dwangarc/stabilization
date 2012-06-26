#pragma once

#include <afxwin.h>

class CRTPCameraDlg : public CDialog
{
	DECLARE_DYNAMIC(CRTPCameraDlg)

public:
	CRTPCameraDlg(CWnd* pParent = NULL);   // standard constructor

	int camPort;
	bool interlace;

	enum { IDD = IDD_RTP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

};
