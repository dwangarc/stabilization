// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "VideoMosaic.h"
#include "ChildView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SEARCH_SIZE 40
// CChildView

CChildView::CChildView()
{
	target = FALSE; 
	
	m_iPointX = 0;
	m_iPointY = 0;
	mouse_X = 0;
	mouse_Y = 0;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	//cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
	//	::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CMainFrame *frm = (CMainFrame*)AfxGetMainWnd();

	RECT rect;
	GetClientRect(&rect);
	int cx = rect.right - rect.left;
	int cy = rect.bottom - rect.top;

	CBitmap bm_mem, *pbm_mem;
	CDC dc_mem;
	bm_mem.CreateCompatibleBitmap(&dc, cx, cy);
	dc_mem.CreateCompatibleDC(&dc);
	pbm_mem = dc_mem.SelectObject(&bm_mem);

	::WaitForSingleObject(frm->lock_frame, INFINITE);

		int width	= frm->fr_width;
		int height	= frm->fr_height;

		if (frm->frame && width && height) {
			BITMAPINFOHEADER bmih;
			memset(&bmih, 0, sizeof(BITMAPINFOHEADER));
			
			bmih.biSize		= sizeof(BITMAPINFOHEADER);
			bmih.biWidth	= frm->fr_width;
			bmih.biHeight	= -frm->fr_height;
			bmih.biPlanes	= 1;
			bmih.biBitCount	= 24;

			SetStretchBltMode(dc_mem.m_hDC, COLORONCOLOR);
			StretchDIBits(dc_mem.m_hDC, 0, 0, cx, cy, 0, 0, width, height, frm->frame, (BITMAPINFO*)&bmih, DIB_RGB_COLORS, SRCCOPY);
		
			/*if(target){
				RECT rect0;
				rect0.left		= __min(m_iPointX, mouse_X);
				rect0.right		= __max(m_iPointX, mouse_X);
				rect0.top		= __min(m_iPointY, mouse_Y);
				rect0.bottom	= __max(m_iPointY, mouse_Y);

				HPEN	pen		= ::CreatePen(PS_SOLID, 4, RGB(100, 100, 255));
				HBRUSH	brush0	= (HBRUSH)::SelectObject(dc_mem.m_hDC, GetStockObject(NULL_BRUSH));
				HPEN	pen0	= (HPEN)::SelectObject(dc_mem.m_hDC, pen);
				dc_mem.SetROP2(R2_COPYPEN);
				dc_mem.Rectangle(&rect0);
				DeleteObject(pen);
			}*/
			if(frm->capture){
				RECT rect1;
				rect1.left		= ( frm->target.x - frm->target.w/2 ) * cx / frm->fr_width;
				rect1.right		= ( frm->target.x + frm->target.w/2 ) * cx / frm->fr_width;
				rect1.top		= ( frm->target.y - frm->target.h/2 ) * cy / frm->fr_height;
				rect1.bottom	= ( frm->target.y + frm->target.h/2 ) * cy / frm->fr_height;
				HPEN	pen		= ::CreatePen(PS_SOLID, 4, RGB(100, 255, 100));
				HBRUSH	brush1	= (HBRUSH)::SelectObject(dc_mem.m_hDC, GetStockObject(NULL_BRUSH));
				HPEN	pen1	= (HPEN)::SelectObject(dc_mem.m_hDC, pen);
				dc_mem.SetROP2(R2_COPYPEN);
				dc_mem.Rectangle(&rect1);
				DeleteObject(pen);
			}

		} else
			dc_mem.FillSolidRect(0, 0, cx, cy, RGB(0, 0, 0));
	
	::SetEvent(frm->lock_frame);

	dc.BitBlt(0, 0, cx, cy, &dc_mem, 0, 0, SRCCOPY);
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	mouse_X = point.x;
	mouse_Y = point.y;



	CWnd::OnMouseMove(nFlags, point);
}
void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMainFrame *frm = (CMainFrame*)AfxGetMainWnd();
	
	m_iPointX = point.x;
	m_iPointY = point.y;

	if(frm->fr_width && frm->fr_height){
		RECT rect;
		GetClientRect(&rect);
		int cx = rect.right - rect.left;
		int cy = rect.bottom - rect.top;
			
		::WaitForSingleObject(frm->lock_frame, INFINITE);	
			frm->target.x = point.x * frm->fr_width / cx;
			frm->target.y = point.y * frm->fr_height / cy;
			frm->target.w = SEARCH_SIZE * frm->fr_width / cx;
			frm->target.h = SEARCH_SIZE * frm->fr_height / cy;
			//EndTrackData(&frm->data, frm->m_Method);
			//frm->data = CreateTrack(frm->fr_width, frm->fr_height, frm->m_Method);
			//if(!InitTrack(frm->data, frm->frame, frm->fr_width * 3, &frm->target, frm->m_Method))
			//	frm->capture = FALSE;
			//else
				frm->capture = TRUE;
		::SetEvent(frm->lock_frame);
	}


	CWnd::OnLButtonDown(nFlags, point);
}
void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	
	
	CWnd::OnLButtonUp(nFlags, point);
}
void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMainFrame *frm = (CMainFrame*)AfxGetMainWnd();
	
	::WaitForSingleObject(frm->lock_frame, INFINITE);
		frm->capture = FALSE;
		frm->target.y = 0;
		frm->target.x = 0;
		frm->target.w = 0;
		frm->target.h = 0;
		//ml_track_release(frm->data);
		//frm->data = NULL;
	::SetEvent(frm->lock_frame);

}

