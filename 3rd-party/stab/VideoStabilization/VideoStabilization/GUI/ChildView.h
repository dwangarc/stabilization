// ChildView.h : interface of the CChildView class
//

#pragma once

#include <afxwin.h>
#include "ImageProc/inc/Picture.h"
#include "GlWrap/inc/OpenGlWrap.h"

class CMainFrame;

class CChildView : public CWnd
{
	BITMAPINFO*	pbmi;

	int koeff;
	bool bgr;
	
	Point2D mouse;

	OpenGlWrap glPainter;
	
	bool brozing;
	Point2D p1, p2;

	HGLRC hGLRC;
	HDC hDC;

	enum MotionMode
	{
		MOTION_MODE_ALL = 0,
		MOTION_MODE_MOVING = 1,
		MOTION_MODE_STAYED = 2,
		MOTION_MODE_BASE = 3,
		MOTION_MODE_ORIGIN = 4
	};

	MotionMode motionMode;

	Point2D convertToPicCoord(CPoint point) const;
	Picture fillPic;

public:
	CChildView();

	// Overrides
protected:

	// Implementation
public:
	virtual ~CChildView();
	CEdit		m_edtInfo;
	RECT		rc_view;

	void setBGR(bool val);
	bool getBGR() const;

	// Generated message map functions
protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
	
	void paintPicture(CDC& dcMem, Picture pic, int left, int top, int right, int bottom);

	int SetWindowPixelFormat();
};

