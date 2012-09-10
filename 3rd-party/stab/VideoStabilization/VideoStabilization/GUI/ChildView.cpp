// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"

#include "ChildView.h"
#include "StabilizeGUIStart.h"
#include "MainFrm.h"
#include "ImageProc/inc/PicturePainter.h"
#include "Include/capture_log.h"
#include "Include/RegistryParamsSaver.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView():fillPic(100, 100, 3)
{
	fillPic.fill(50);
	brozing = false;
	koeff = 6;
	motionMode = MOTION_MODE_ALL;
	pbmi = (BITMAPINFO*) malloc(sizeof(BITMAPINFOHEADER) + 256 * 4);
	for (int k = 0; k < 256; k++) {
		pbmi->bmiColors[k].rgbRed		= k;
		pbmi->bmiColors[k].rgbGreen		= k;
		pbmi->bmiColors[k].rgbBlue		= k;
		pbmi->bmiColors[k].rgbReserved	= 0;
	}
	memset(&pbmi->bmiHeader, 0, sizeof(BITMAPINFOHEADER));
	pbmi->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biBitCount		= 24;
	pbmi->bmiHeader.biPlanes		= 1;
	pbmi->bmiHeader.biClrUsed		= 256;
	pbmi->bmiHeader.biClrImportant	= 256;
	RegistryParamsSaver saver;
	bgr = saver.loadInt(_T("CChildView"), _T("BGR"), false) == 0 ? false : true;

}

CChildView::~CChildView()
{
	if (pbmi) free(pbmi);
	if (hGLRC) 
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hGLRC);
	}
}




BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CHAR()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CChildView message handlers

void CChildView::setBGR(bool val)
{
	bgr = val;
	RegistryParamsSaver saver;
	saver.saveInt(_T("CChildView"), _T("BGR"), val);
}

bool CChildView::getBGR() const
{
	return bgr;
}



void CChildView::OnPaint() 
{
	PAINTSTRUCT ps;

	::BeginPaint(m_hWnd, &ps);

	CMainFrame *frm = (CMainFrame*)AfxGetMainWnd();

	int cx = rc_view.right	- rc_view.left;
	int cy = rc_view.bottom	- rc_view.top;

	Mediator* mediator = frm->getMediator();
	Picture pic = mediator->getFrame();

	glPainter.startPaint();
	if (pic.getWidth() > 0) 
	{
		if (mediator->getModeType() != MOTION_DETECTION)
		{
			if (mediator->getModeType() == TRACK && brozing)
			{
				PicturePainter painter;
				Color color(0);
				color.g() = 255;
				painter.drawRectangleCross(pic, p1, mouse, color);
			}
			if (mediator->getModeType() == STABILIZE)
			{
				int offsetY = cy / koeff;
				glPainter.drawPic(pic, 0, offsetY, cx / 2, cy - offsetY, bgr);
				glPainter.drawPic(mediator->getOrigin(), cx / 2, offsetY, cx, cy - offsetY, bgr);
			}
			else
			{
				glPainter.drawPic(pic, 0, 0, cx, cy, bgr);
			}
		}
		else
		{
			sh_ptr_mtdtc motionDetector = mediator->getMotionDetector();
			switch (motionMode)
			{
			case MOTION_MODE_ALL:
				glPainter.drawPic(mediator->getOrigin(),		0,		cy / 2,	cx / 2,	cy,		bgr);
				glPainter.drawPic(motionDetector->getMoved(),	cx / 2, 0,		cx,		cy / 2, bgr);
				glPainter.drawPic(motionDetector->getStayed(),	0,		0,		cx / 2,	cy / 2, bgr);
				glPainter.drawPic(motionDetector->getBase(),	cx / 2,	cy / 2,	cx,		cy,		bgr);
			
				break;
			case MOTION_MODE_MOVING:
				glPainter.drawPic(motionDetector->getMoved(), 0, 0, cx, cy, bgr);
				break;
			case MOTION_MODE_STAYED:
				glPainter.drawPic(motionDetector->getStayed(), 0, 0, cx, cy, bgr);
				break;
			case MOTION_MODE_BASE:
				glPainter.drawPic(motionDetector->getBase(), 0, 0, cx, cy, bgr);
				break;
			case MOTION_MODE_ORIGIN:
				glPainter.drawPic(mediator->getOrigin(), 0, 0, cx, cy, bgr);
				break;
			}

		}
	}
	else
	{
		glPainter.drawPic(fillPic, 0, 0, cx, cy, bgr);
	}

	glPainter.finishPaint();
	::EndPaint(m_hWnd, &ps);
}

void CChildView::paintPicture(CDC& dc_mem, Picture pic, int left, int top, int right, int bottom)
{
	if (pic.getWidth() == 0)
	{
		dc_mem.FillSolidRect(left, top, right - left, bottom - top, RGB(0, 0, 0));
		return;
	}
	int frame_width		= pic.getWidth();
	int frame_height	= pic.getHeight();
	pbmi->bmiHeader.biWidth		= frame_width;
	pbmi->bmiHeader.biHeight	= frame_height;

	PictureConverter pc;
	sh_ptr_uch picData = pc.createBitMapData(pic);
//	bitmap* btm = pc.createBitMap(pic);
	StretchDIBits(dc_mem.m_hDC, left, top, right - left, bottom - top, 0, 0, frame_width, frame_height, /*pic.getPictureChar()*/picData.get(), pbmi, DIB_RGB_COLORS, SRCCOPY);
//	bitmap_release(btm);
}



int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	RECT rect;
	GetClientRect(&rect);

	m_edtInfo.Create(WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY, rect, this, 0);
	::SendMessage(m_edtInfo.m_hWnd, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), FALSE);

	hDC = ::GetDC( m_hWnd );

	SetWindowPixelFormat(); 

	hGLRC = wglCreateContext(hDC);

	wglMakeCurrent(hDC, hGLRC);


	return 0;
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	const int tab_height = 0;
	const int info_width = 1;
	CWnd::OnSize(nType, cx, cy);

	if (::IsWindow(m_edtInfo.m_hWnd))	m_edtInfo.MoveWindow(0, 0, info_width, cy);

	rc_view.left	= info_width;
	rc_view.right	= cx;
	rc_view.top		= tab_height;
	rc_view.bottom	= cy;
	glPainter.resize(rc_view.left, rc_view.top, cx -  rc_view.left, cy - rc_view.top);
}

void CChildView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 27)
	{
		motionMode = MOTION_MODE_ALL;
		CMainFrame *frm = (CMainFrame*)AfxGetMainWnd();
		Mediator* mediator = frm->getMediator();
		mediator->disableTracker();
	}
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMainFrame *frm = (CMainFrame*)AfxGetMainWnd();
	Mediator* mediator = frm->getMediator();
	if (mediator->getModeType() != TRACK) return;
	mediator->disableTracker();
	p1 = convertToPicCoord(point);
	brozing = true;
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CMainFrame *frm = (CMainFrame*)AfxGetMainWnd();
	Mediator* mediator = frm->getMediator();
	if (mediator->getModeType() != TRACK) return;
	p2 = convertToPicCoord(point);
	brozing = false;
	mediator->setTarget(p1, p2);	
}

Point2D CChildView::convertToPicCoord(CPoint point) const
{
	int cx = rc_view.right	- rc_view.left;
	int cy = rc_view.bottom	- rc_view.top;
	double x = point.x - rc_view.left;
	double y = point.y - rc_view.top;
	CMainFrame *frm = (CMainFrame*)AfxGetMainWnd();
	Mediator* mediator = frm->getMediator();
	if (mediator->getModeType() == STABILIZE)
	{
		if (x > cx / 2)
		{
			x -= cx / 2;
		}
		x /= cx;
		x *= 2;
		int offsetY = cy / koeff;
		if (y < offsetY || y > cy - offsetY) return Point2D(0, 0);
		y -= offsetY;
		y /= cy;
		y /= 1.0 - (2.0 / (double)koeff);
	}
	else
	{
		x /= cx;
		y /= cy;
	}

	y = 1.0 - y;
	Picture origin = mediator->getOrigin();
	x *= origin.getWidth();
	y *= origin.getHeight();
	Point2D target(x, y);

	return target;
}


void CChildView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CMainFrame *frm = (CMainFrame*)AfxGetMainWnd();
	Mediator* mediator = frm->getMediator();
	if (mediator->getModeType() == MOTION_DETECTION)
	{
		int cx = rc_view.right	- rc_view.left;
		int cy = rc_view.bottom	- rc_view.top;
		double x = point.x - rc_view.left;
		double y = point.y - rc_view.top;
		if (motionMode != MOTION_MODE_ALL) 
		{
			return;
		}
		x -= cx / 2;
		y -= cy / 2;

		if (x < 0 && y < 0)	motionMode = MOTION_MODE_ORIGIN;
		if (x > 0 && y < 0)	motionMode = MOTION_MODE_BASE;
		if (x < 0 && y > 0)	motionMode = MOTION_MODE_STAYED;
		if (x > 0 && y > 0)	motionMode = MOTION_MODE_MOVING;
	}
	else
	{
		Point2D target = convertToPicCoord(point);
		double width = 20;
		Point2D tp1(target.x - width, target.y - width);
		Point2D tp2(target.x + width, target.y + width);
		mediator->setTarget(tp1, tp2);
	}
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	mouse = convertToPicCoord(point);
	CWnd::OnMouseMove(nFlags, point);
}


int CChildView::SetWindowPixelFormat()
{
	int m_GLPixelIndex;
	PIXELFORMATDESCRIPTOR pfd;


	pfd.nSize       = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion    = 1;

	pfd.dwFlags   = PFD_DRAW_TO_WINDOW | 
		PFD_SUPPORT_OPENGL | 
		PFD_DOUBLEBUFFER;

	pfd.iPixelType     = PFD_TYPE_RGBA;
	pfd.cColorBits     = 32;
	pfd.cRedBits       = 8;
	pfd.cRedShift      = 16;
	pfd.cGreenBits     = 8;
	pfd.cGreenShift    = 8;
	pfd.cBlueBits      = 8;
	pfd.cBlueShift     = 0;
	pfd.cAlphaBits     = 0;
	pfd.cAlphaShift    = 0;
	pfd.cAccumBits     = 64;    
	pfd.cAccumRedBits  = 16;
	pfd.cAccumGreenBits   = 16;
	pfd.cAccumBlueBits    = 16;
	pfd.cAccumAlphaBits   = 0;
	pfd.cDepthBits        = 32;
	pfd.cStencilBits      = 8;
	pfd.cAuxBuffers       = 0;
	pfd.iLayerType        = PFD_MAIN_PLANE;
	pfd.bReserved         = 0;
	pfd.dwLayerMask       = 0;
	pfd.dwVisibleMask     = 0;
	pfd.dwDamageMask      = 0;


	m_GLPixelIndex = ChoosePixelFormat( hDC, &pfd);
	if(m_GLPixelIndex==0) // Let's choose a default index.
	{
		m_GLPixelIndex = 1;    
		if(DescribePixelFormat(hDC,m_GLPixelIndex,sizeof(PIXELFORMATDESCRIPTOR),&pfd)==0)
			return 0;
	}


	if (SetPixelFormat( hDC, m_GLPixelIndex, &pfd)==FALSE)
		return 0;


	return 1;
}
