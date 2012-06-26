#pragma once

#include <afxwin.h>
#include <string>


class CSavingVideoDlg : public CDialog
{
	DECLARE_DYNAMIC(CSavingVideoDlg)

public:
	CSavingVideoDlg(CWnd* pParent = NULL);   // standard constructor

	bool saveBaseVideo, saveResVideo;
	std::string baseVideoPath, resVideoPath;

	enum { IDD = IDD_SAVING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

};
