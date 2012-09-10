// VideoAnaView.cpp : implementation of the CVideoAnaView class
//

#include "stdafx.h"
#include "VideoAna.h"

#include "VideoAnaDoc.h"
#include "VideoAnaView.h"
#include "stdio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVideoAnaView

IMPLEMENT_DYNCREATE(CVideoAnaView, CFormView)

BEGIN_MESSAGE_MAP(CVideoAnaView, CFormView)
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CVideoAnaView construction/destruction

CVideoAnaView::CVideoAnaView()
	: CFormView(CVideoAnaView::IDD)
	, m_strAnaStatus(_T("Not analyzing. Open a video file and push \"start\"."))
{
	// TODO: add construction code here

}

CVideoAnaView::~CVideoAnaView()
{
}

void CVideoAnaView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PREVIEWBOX, m_PreviewBox);
	DDX_Text(pDX, IDC_ANA_STATUS, m_strAnaStatus);
	DDX_Control(pDX, IDC_ANA_PROGRESS, m_AnaProgress);
}

BOOL CVideoAnaView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CVideoAnaView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	GetParentFrame()->RecalcLayout();
//	ResizeParentToFit();
}


// CVideoAnaView diagnostics

#ifdef _DEBUG
void CVideoAnaView::AssertValid() const
{
	CFormView::AssertValid();
}

void CVideoAnaView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CVideoAnaDoc* CVideoAnaView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVideoAnaDoc)));
	return (CVideoAnaDoc*)m_pDocument;
}
#endif //_DEBUG


// CVideoAnaView message handlers

LRESULT CVideoAnaView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
	case WM_USER_PREVIEW_FRAME:
		{
			// Preview image
			m_PreviewBox.SetDibBits((LPBYTE)wParam, (int)lParam);
			m_PreviewBox.PaintDIB();
			delete[] ((BYTE *)wParam);

			return 0;
			break;
		}
	case WM_USER_UPDATE_PROGRESS:
		{
			// Ana status
			m_AnaProgress.SetPos((int)wParam);
			m_strAnaStatus.Format("Analyzing... Frame: %d/%d", wParam, GetDocument()->m_nTotalFrames);
			UpdateData(FALSE);
		}
	default:
		return CFormView::WindowProc(message, wParam, lParam);
	    break;
	}

}

void CVideoAnaView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(ID_TIMER_EVENT_UPDATE == nIDEvent)
		SetEvent(GetDocument()->m_hUpdateEvent);

	CFormView::OnTimer(nIDEvent);
}

void CVideoAnaView::ClearAll()
{
	m_AnaProgress.SetPos(0);
	m_PreviewBox.SetDibBits(NULL, 0);

	m_strAnaStatus = "Not analyzing. Open a video file and push \"start\".";

	UpdateData(FALSE);
}

void CVideoAnaView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	CRect l_formRect; 
	GetClientRect(&l_formRect);

	//Resizing Preview Box
	SPicBox* pPicBoxCtrl;
	pPicBoxCtrl = (SPicBox *)GetDlgItem(IDC_PREVIEWBOX);

	if(pPicBoxCtrl->GetSafeHwnd())
	{
		pPicBoxCtrl->MoveWindow(l_formRect.TopLeft().x+5, l_formRect.TopLeft().y+5, l_formRect.BottomRight().x-5,
				l_formRect.BottomRight().y-50, TRUE);
	}

	//Resizing Status CStatic
	CStatic* pAnaStatus;
	pAnaStatus = (CStatic *)GetDlgItem(IDC_ANA_STATUS);

	if(pAnaStatus->GetSafeHwnd())
	{
		pAnaStatus->MoveWindow(l_formRect.TopLeft().x+5, l_formRect.BottomRight().y-50+10,
			l_formRect.BottomRight().x-5, 20, TRUE);
	}

	//Resizing Status CStatic
	CProgressCtrl* pAnaProgress;
	pAnaProgress = (CProgressCtrl *)GetDlgItem(IDC_ANA_PROGRESS);

	if(pAnaProgress->GetSafeHwnd())
	{
		pAnaProgress->MoveWindow(l_formRect.TopLeft().x+5, l_formRect.BottomRight().y-17, l_formRect.BottomRight().x-5,
				10, TRUE);

		pAnaProgress->RedrawWindow();
	}
	
}
/*
void CVideoAnaView::ResizeToFitVideo(LONG nWidth, LONG nHeigth)
{
	CRect rect = CRect(0,0,nWidth,nHeigth);
	ClientToScreen(rect);
	MoveWindow(rect,TRUE);

	CRect l_Rect; 
	GetClientRect(&l_Rect);

	//Resizing Preview Box
	SPicBox* pPicBoxCtrl;
	pPicBoxCtrl = (SPicBox *)GetDlgItem(IDC_PREVIEWBOX);

	if(pPicBoxCtrl->GetSafeHwnd())
	{
		pPicBoxCtrl->MoveWindow(l_Rect.TopLeft().x+5, l_Rect.TopLeft().y+5, nWidth, nHeigth, TRUE);
	}

	//Resizing Status CStatic
	CStatic* pAnaStatus;
	pAnaStatus = (CStatic *)GetDlgItem(IDC_ANA_STATUS);

	if(pAnaStatus->GetSafeHwnd())
	{
		pAnaStatus->MoveWindow(l_Rect.TopLeft().x+5, nHeigth + 10,
			nWidth, 20, TRUE);
	}

	//Resizing Status CStatic
	CProgressCtrl* pAnaProgress;
	pAnaProgress = (CProgressCtrl *)GetDlgItem(IDC_ANA_PROGRESS);

	if(pAnaProgress->GetSafeHwnd())
	{
		pAnaProgress->MoveWindow(l_Rect.TopLeft().x+5, nHeigth + 33, nWidth,
				10, TRUE);

		pAnaProgress->RedrawWindow();
	}
	
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);
}
*/