// VideoAnaDoc.cpp : implementation of the CVideoAnaDoc class
//

#include "stdafx.h"
#include "VideoAna.h"
#include "VideoAnaDoc.h"
#include "VideoAnaView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

CVideoAnaDoc* pDoc = NULL; 

STDMETHODIMP SVideoProcessAdapter::BufferCB(double SampleTime, BYTE *pBuffer, long nBufferLen)
{
	return m_pVideoAnaDoc->ProcessFrame(SampleTime, pBuffer, nBufferLen);
}

// CVideoAnaDoc

IMPLEMENT_DYNCREATE(CVideoAnaDoc, CDocument)

BEGIN_MESSAGE_MAP(CVideoAnaDoc, CDocument)
	ON_UPDATE_COMMAND_UI(ID_ANA_START, &CVideoAnaDoc::OnUpdateAnaStart)
	ON_UPDATE_COMMAND_UI(ID_ANA_PAUSE, &CVideoAnaDoc::OnUpdateAnaPause)
	ON_UPDATE_COMMAND_UI(ID_ANA_STOP, &CVideoAnaDoc::OnUpdateAnaStop)
	ON_COMMAND(ID_ANA_START, &CVideoAnaDoc::OnAnaStart)
	ON_COMMAND(ID_ANA_PAUSE, &CVideoAnaDoc::OnAnaPause)
	ON_COMMAND(ID_ANA_STOP, &CVideoAnaDoc::OnAnaStop)
	ON_COMMAND(ID_ANA_ACTIONFINISHED, &CVideoAnaDoc::OnAnaActionFinished)
END_MESSAGE_MAP()


// CVideoAnaDoc construction/destruction

CVideoAnaDoc::CVideoAnaDoc()
: m_pGraph(NULL)
, m_pGrabberFilter(NULL)
, m_pGrabber(NULL)
, m_pMediaFilter(NULL)
, m_pControl(NULL)
, m_pEvent(NULL)
, m_GraphStatus(GRAPH_NONE)
, m_pSrcFilter(NULL)
, m_pNullRenderer(NULL)
, m_pSeeking(NULL)
, m_VideoProcessAdapter(this)
, m_nCurFrame(0)
{
	memset(&m_Bfh, 0, sizeof(m_Bfh));
	m_Bfh.bfType = 0x4d42;
	m_Bfh.bfOffBits = sizeof(m_Bfh) + sizeof(BITMAPINFOHEADER);

	m_hUpdateEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	memset(&m_Bih, 0, sizeof(m_Bih));

	pDoc = this;
}

CVideoAnaDoc::~CVideoAnaDoc()
{
}

BOOL CVideoAnaDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

// CVideoAnaDoc serialization

void CVideoAnaDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CVideoAnaDoc diagnostics

#ifdef _DEBUG
void CVideoAnaDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVideoAnaDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CVideoAnaDoc commands

BOOL CVideoAnaDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	m_GraphStatus = GRAPH_STOPPED;

	POSITION ViewPos = GetFirstViewPosition();
	m_pVideoAnaView = (CVideoAnaView *)GetNextView(ViewPos);
	m_pVideoAnaView->m_PreviewBox.SetDibBits(NULL, 0);

	m_pVideoAnaView->RedrawWindow();
	m_pVideoAnaView->ClearAll();

	return TRUE;
}

void CVideoAnaDoc::OnUpdateAnaStart(CCmdUI *pCmdUI)
{
	if(GRAPH_STOPPED == m_GraphStatus)
		pCmdUI->Enable();
	else
		pCmdUI->Enable(FALSE);
}

void CVideoAnaDoc::OnUpdateAnaPause(CCmdUI *pCmdUI)
{
	if(GRAPH_RUNNING == m_GraphStatus || GRAPH_PAUSED == m_GraphStatus)
		pCmdUI->Enable();
	else
		pCmdUI->Enable(FALSE);
}

void CVideoAnaDoc::OnUpdateAnaStop(CCmdUI *pCmdUI)
{
	if(GRAPH_RUNNING == m_GraphStatus || GRAPH_PAUSED == m_GraphStatus)
		pCmdUI->Enable();
	else
		pCmdUI->Enable(FALSE);
}

void CVideoAnaDoc::OnAnaStart()
{
	ASSERT(GRAPH_STOPPED == m_GraphStatus);

	// Create the graph builder
	HRESULT hr = ::CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
		IID_IGraphBuilder, reinterpret_cast<void**>(&m_pGraph));
	if (FAILED(hr))
	{
		AfxMessageBox("Failed creating DirectShow objects!");
		return;
	}

	// Create the Sample Grabber
	m_pGrabberFilter = NULL;
	m_pGrabber = NULL;
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, reinterpret_cast<void**>(&m_pGrabberFilter));
	hr = m_pGrabberFilter->QueryInterface(IID_ISampleGrabber,
		reinterpret_cast<void**>(&m_pGrabber));
	hr = m_pGraph->AddFilter(m_pGrabberFilter, L"SampleGrabber");

	// Set the media type
	AM_MEDIA_TYPE mt;
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.formattype = FORMAT_VideoInfo; 
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24;	// only accept 24-bit bitmaps
	hr = m_pGrabber->SetMediaType(&mt);

	// Create the src filter
	wchar_t strFilename[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, m_strPathName, -1, strFilename, MAX_PATH);
	hr = m_pGraph->AddSourceFilter(strFilename, L"Source", &m_pSrcFilter);
	if(FAILED(hr))
	{
		AfxMessageBox("Unsupported media type!");
		return;
	}

	// Connect the src and grabber
	hr = ConnectFilters(m_pGraph, m_pSrcFilter, m_pGrabberFilter);
	if(FAILED(hr))
	{
		SAFE_RELEASE(m_pSrcFilter);
		SAFE_RELEASE(m_pGrabber);
		SAFE_RELEASE(m_pGrabberFilter);
		SAFE_RELEASE(m_pGraph);
		AfxMessageBox("Unsupported media type!");
		return;
	}

	// Create the NULL renderer and connect
	m_pNullRenderer = NULL;
	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, reinterpret_cast<void**>(&m_pNullRenderer));
	hr = m_pGraph->AddFilter(m_pNullRenderer, L"NullRenderer");
	hr = ConnectFilters(m_pGraph, m_pGrabberFilter, m_pNullRenderer);

	m_nCurFrame = 0;

	// Set modes
	m_pGrabber->SetCallback(&m_VideoProcessAdapter, 1);
	m_pGrabber->SetBufferSamples(TRUE);

	// Necessary interfaces for controlling
	m_pGraph->QueryInterface(IID_IMediaFilter, reinterpret_cast<void**>(&m_pMediaFilter));
	m_pGraph->QueryInterface(IID_IMediaControl, reinterpret_cast<void**>(&m_pControl));
	m_pGraph->QueryInterface(IID_IMediaEvent, reinterpret_cast<void**>(&m_pEvent));
	m_pGraph->QueryInterface(IID_IMediaSeeking, reinterpret_cast<void**>(&m_pSeeking));

	// Turn off the sync clock for max speed
//	m_pMediaFilter->SetSyncSource(NULL);

	// Retrieve the actual media type
	ZeroMemory(&mt, sizeof(mt));

	hr = m_pGrabber->GetConnectedMediaType(&mt);

	VIDEOINFOHEADER *pVih;

	if (mt.formattype == FORMAT_VideoInfo) 
		pVih = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);
	else 
	{
		SAFE_RELEASE(m_pMediaFilter);
		SAFE_RELEASE(m_pControl);
		SAFE_RELEASE(m_pEvent);
		SAFE_RELEASE(m_pSrcFilter);
		SAFE_RELEASE(m_pNullRenderer);
		SAFE_RELEASE(m_pGrabber);
		SAFE_RELEASE(m_pGraph);
		AfxMessageBox("No video stream found!");
		return; // Something went wrong, perhaps not appropriate media type
	}

	m_lHeight = pVih->bmiHeader.biHeight;
	m_lWidth = pVih->bmiHeader.biWidth;

//	m_pVideoAnaView->ResizeToFitVideo(m_lWidth,m_lHeight);


	// Save the video info header
	memcpy(&m_Bih, &pVih->bmiHeader, sizeof(m_Bih));
	m_Bfh.bfSize = sizeof(m_Bfh) + sizeof(BITMAPINFOHEADER) + m_Bih.biSizeImage;
	m_pVideoAnaView->m_PreviewBox.SetBih(&m_Bih);
	m_lTimeperFrame = pVih->AvgTimePerFrame;
	// Free the media type
	if (mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);

		// Strictly unnecessary but tidier
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL) {
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}

//	CreateArrays();

	// Get video info
	m_pSeeking->GetDuration(&m_lDuration);
	if(FAILED(m_pSeeking->SetTimeFormat(&TIME_FORMAT_FRAME)))
		m_nTotalFrames = m_lDuration / m_lTimeperFrame;
	else
		m_pSeeking->GetDuration(&m_nTotalFrames);

	m_pVideoAnaView->ClearAll();
	m_pVideoAnaView->m_AnaProgress.SetRange32(0, (int)m_nTotalFrames);

	// Setup the view
	m_pVideoAnaView->SetTimer(ID_TIMER_EVENT_UPDATE, 500, NULL);
	SetEvent(m_hUpdateEvent);

	m_GraphStatus = GRAPH_RUNNING;
	m_pControl->Run(); // Run the graph to start the analyzing process!

	AfxBeginThread((AFX_THREADPROC)WaitProc, this);	// Begin to wait
}

void CVideoAnaDoc::OnAnaPause()
{
	// TODO: Add your command handler code here
	ASSERT(GRAPH_RUNNING == m_GraphStatus || GRAPH_PAUSED == m_GraphStatus);
	if(GRAPH_RUNNING == m_GraphStatus)
	{
		m_pControl->Pause();
		m_GraphStatus = GRAPH_PAUSEPENDING;
	}
	else
	{
		m_pControl->Run();
		m_GraphStatus = GRAPH_RUNNING;
		AfxBeginThread((AFX_THREADPROC)WaitProc, this);
	}

}

void CVideoAnaDoc::OnAnaStop()
{
	// TODO: Add your command handler code here
	ASSERT(GRAPH_RUNNING == m_GraphStatus || GRAPH_PAUSED == m_GraphStatus);
	if(GRAPH_RUNNING == m_GraphStatus)
	{
		m_pControl->Stop();
		m_pControl->Pause();
		m_GraphStatus = GRAPH_STOPPENDING;
	}
	else	// GRAPH_PAUSED == m_GraphStatus
	{
		m_pControl->Stop();
		m_GraphStatus = GRAPH_STOPPENDING;

		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_ANA_ACTIONFINISHED);	// Simulate a stop event
	}

//	CleanupArrays();
}


// Wait until analyzing ends or pauses
UINT __cdecl CVideoAnaDoc::WaitProc(CVideoAnaDoc * pThis)
{
	long evCode;
	pThis->m_pEvent->WaitForCompletion(INFINITE, &evCode); // Wait until the graph stops
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_ANA_ACTIONFINISHED);
	return 0;
}

void CVideoAnaDoc::OnAnaActionFinished()
{
	ASSERT(GRAPH_STOPPENDING == m_GraphStatus || GRAPH_PAUSEPENDING == m_GraphStatus || GRAPH_RUNNING == m_GraphStatus);

	if(GRAPH_PAUSEPENDING == m_GraphStatus)
		m_GraphStatus = GRAPH_PAUSED;
	else	// (GRAPH_STOPPENDING == m_GraphStatus || GRAPH_RUNNING == m_GraphStatus)
	{

		m_GraphStatus = GRAPH_STOPPED;
		// Free DirectShow resources when finished analyzing
		SAFE_RELEASE(m_pMediaFilter);
		SAFE_RELEASE(m_pSeeking);
		SAFE_RELEASE(m_pControl);
		SAFE_RELEASE(m_pEvent);
		SAFE_RELEASE(m_pSrcFilter);
		SAFE_RELEASE(m_pNullRenderer);
		SAFE_RELEASE(m_pGrabber);
		SAFE_RELEASE(m_pGraph);

		m_pVideoAnaView->KillTimer(ID_TIMER_EVENT_UPDATE);

		CString strMsg;
		strMsg.Format("Process finished. %d frames total.", m_nCurFrame);
		AfxMessageBox(strMsg);
	}
}

// Helper functions:
HRESULT CVideoAnaDoc::GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin)
{
	IEnumPins  *pEnum;
	IPin       *pPin;
	pFilter->EnumPins(&pEnum);
	while(pEnum->Next(1, &pPin, 0) == S_OK)
	{
		PIN_DIRECTION PinDirThis;
		pPin->QueryDirection(&PinDirThis);
		if (PinDir == PinDirThis)
		{
			pEnum->Release();
			*ppPin = pPin;
			return S_OK;
		}
		pPin->Release();
	}
	pEnum->Release();
	return E_FAIL;  
}

HRESULT CVideoAnaDoc::ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pFirst, IBaseFilter *pSecond)
{
	IPin *pOut = NULL, *pIn = NULL;
	HRESULT hr = GetPin(pSecond, PINDIR_INPUT, &pIn);
	if (FAILED(hr)) return hr;
	// The previous filter may have multiple outputs, so try each one!
	IEnumPins  *pEnum;
	pFirst->EnumPins(&pEnum);
	while(pEnum->Next(1, &pOut, 0) == S_OK)
	{
		PIN_DIRECTION PinDirThis;
		pOut->QueryDirection(&PinDirThis);
		if (PINDIR_OUTPUT == PinDirThis)
		{
			hr = pGraph->Connect(pOut, pIn);
			if(!FAILED(hr))
			{
				break;
			}
		}
		pOut->Release();
	}
	pEnum->Release();
	pIn->Release();
	pOut->Release();
	return hr;
}

HRESULT CVideoAnaDoc::ProcessFrame(double SampleTime, BYTE *pInBuffer, long nBufferLen)
{
	if(GRAPH_RUNNING != m_GraphStatus)
		return S_OK;

	RECT rect;
	::GetWindowRect(m_pVideoAnaView->m_PreviewBox.m_hWnd, &rect);
	int cx = rect.right - rect.left;
	int cy = rect.bottom - rect.top;

	HDC hdc = ::GetDC(m_pVideoAnaView->m_PreviewBox.m_hWnd);
	
	BITMAPINFO p_bmpi;
	memcpy( &p_bmpi.bmiHeader, &m_Bih, sizeof(BITMAPINFOHEADER) );

	::StretchDIBits(hdc, 0, 0, cx, cy, 0, 0, m_lWidth, m_lHeight, pInBuffer, &p_bmpi, DIB_RGB_COLORS, SRCCOPY);

	::ReleaseDC(m_pVideoAnaView->m_PreviewBox.m_hWnd, hdc);

	m_pVideoAnaView->PostMessage(WM_USER_UPDATE_PROGRESS, (WPARAM)m_nCurFrame++);
	
	// Incorrect thread
	//m_pVideoAnaView->PostMessage(WM_USER_PREVIEW_FRAME, (WPARAM)pInBuffer, nBufferLen);

	return S_OK;
}
