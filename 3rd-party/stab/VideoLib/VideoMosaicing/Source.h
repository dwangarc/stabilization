#pragma once


// CSource dialog

class CSource : public CDialog
{
	DECLARE_DYNAMIC(CSource)

public:
	CSource(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSource();

// Dialog Data
	enum { IDD = IDD_SEL_SOURCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	private:
	CButton			*Video1;
	CButton			*Video2;
	CButton			*Video3;

public:
	int m_Source;
	
	BOOL PlayStopButton;
	BOOL NextButton;
	
protected:
	virtual BOOL OnInitDialog(void);
	
	afx_msg void OnVideo();
	afx_msg void OnJPEG();
	afx_msg void OnPTZ();
	
	DECLARE_MESSAGE_MAP()


};
