#pragma once

#include "resource.h"
#include <afxwin.h>

class CModeDlg : public CDialog
{
	DECLARE_DYNAMIC(CModeDlg)

public:
	CModeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CModeDlg();

	// Dialog Data
	enum { IDD = IDD_MODE_DIALOG};

public:

	int	modeType;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnModeStabilization();
	afx_msg void OnModeUnStabilization();
	afx_msg void OnModeAnalyze();
	afx_msg void OnModeBase();
	afx_msg void OnModeMotion();
	afx_msg void OnModeTrack();
private:
	void OnModeChanged();
	void OnModeUnChanged();
};
