// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#include "ChildView.h"
#include "stabilization.h"

struct jpeg_decompress_struct;
struct jpeg_error_mgr;

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:
	HANDLE	lock_status;
	CString	m_status;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CChildView  m_wndView;
	CToolBar	m_wndToolBar;


// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	HANDLE lock_frame;
	HANDLE main_thread;
	HANDLE jpeg_thread;
	HANDLE jpeg_thread_run;
	HANDLE video_thread;
	HANDLE video_thread_run;
	HANDLE ptz_thread;
	HANDLE libjpeg_thread;
	
	int m_Source;
	BOOL PlayStopButton;
	BOOL NextButton;

	BOOL Play;
	BOOL Next;
	BOOL NewFrame;

	BOOL Stabilization;
	BOOL Localization;

	int fr_width;
	int fr_height;
	unsigned char* frame;
	unsigned char* frame_grayscale;

	CString File;
	CString FilePath;

	jpeg_decompress_struct* cinfo;
	jpeg_error_mgr* jerr;

	BOOL capture;
	ml_track_data* data;
	track_target target;

//Functions
	afx_msg void OnSelectSource();
	afx_msg void OnPlay();
	afx_msg void OnNext();
	afx_msg void OnStop();
	afx_msg void OnOpenFile();

	afx_msg void OnUpdateOpen(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlay(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNext(CCmdUI *pCmdUI);
	afx_msg void OnUpdateStop(CCmdUI *pCmdUI);

	static DWORD WINAPI JPEGFunction( LPVOID lpParam );
	static DWORD WINAPI VideoFunction( LPVOID lpParam );
	afx_msg void OnReplay();
	afx_msg void OnUpdateReplay(CCmdUI *pCmdUI);
	afx_msg void OnStabilization();
	afx_msg void OnStopStopstabilization();
	afx_msg void OnLocalizationOn();
	afx_msg void OnLocalizationOff();
};


