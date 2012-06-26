#pragma once

#include "ChildView.h"
#include "Mediator/mediator.h"
#include "Include/IFrame.h"

class CMainFrame : public CFrameWnd, public IFrame
{
	Mediator* mediator;
	bool paused;

public:
	CMainFrame();
	virtual ~CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CChildView		m_wndView;

	// Generated message map functions
protected:

	// OK
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg LRESULT OnSetProgress(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAbout();
	afx_msg void OnMode();
	afx_msg void OnParams();
	afx_msg void OnDistortion();
	afx_msg void OnRestart();
	afx_msg void OnPause();
	afx_msg void OnDestabParams();
	afx_msg void OnAnalyze();
	afx_msg void OnSavingVideoParams();
	afx_msg void OnMotionParams();


	afx_msg void OnOpenFiles();
	afx_msg void OnOpenVideo();
	afx_msg void OnOpenURL();
	afx_msg void OnOpenRTP();

	//	afx_msg void OnChar(UINT Char, UINT Count, UINT Flags);

	// OK
	DECLARE_MESSAGE_MAP()

public:
	// OK
	afx_msg void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
	afx_msg void OnClose();

	void FrmPostMessage(__in UINT Msg,	__in WPARAM wParam,	__in LPARAM lParam);

	bool isFrameWindow();
	void repaintFrame();

	void setMediator(Mediator* mediator);
	Mediator* getMediator();
};


