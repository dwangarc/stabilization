// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "VideoMosaic.h"

#include "MainFrm.h"
#include "Source.h"
#include "libjpeg\jpeglib.h"
#include "libjpeg\jpegutils.h"
#include <math.h>
#include "stabilization.h"
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void set_status(const char *szFormat, ...)
{
	CMainFrame *frm = (CMainFrame*)theApp.m_pMainWnd;

	va_list vl;
	va_start(vl, szFormat);

	CString s;
	s.FormatV(szFormat, vl);

	::WaitForSingleObject(frm->lock_status, INFINITE);
	frm->m_status = s;
	::SetEvent(frm->lock_status);

	frm->PostMessageA(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE, 0);

	va_end(vl);
}

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_MESSAGE(WM_SETMESSAGESTRING,		&CMainFrame::OnSetMessageString)
	
	ON_COMMAND(ID_SELECT, &CMainFrame::OnSelectSource)
	ON_COMMAND(ID_PLAY, &CMainFrame::OnPlay)
	ON_COMMAND(ID_NEXT, &CMainFrame::OnNext)
	ON_COMMAND(ID_STOP, &CMainFrame::OnStop)
	ON_COMMAND(ID_OPENFILE, &CMainFrame::OnOpenFile)

	ON_UPDATE_COMMAND_UI(ID_OPENFILE, &CMainFrame::OnUpdateOpen)
	ON_UPDATE_COMMAND_UI(ID_PLAY, &CMainFrame::OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(ID_NEXT, &CMainFrame::OnUpdateNext)
	ON_UPDATE_COMMAND_UI(ID_STOP, &CMainFrame::OnUpdateStop)
	ON_COMMAND(ID_REPLAY, &CMainFrame::OnReplay)
	ON_UPDATE_COMMAND_UI(ID_REPLAY, &CMainFrame::OnUpdateReplay)
	ON_COMMAND(ID_STABILIZATION_STARTSTABILIZATION, &CMainFrame::OnStabilization)
	ON_COMMAND(ID_STABILIZATION_STOPSTABILIZATION, &CMainFrame::OnStopStopstabilization)
	ON_COMMAND(ID_LOCALIZATION_LOCALIZATIONON, &CMainFrame::OnLocalizationOn)
	ON_COMMAND(ID_LOCALIZATION_LOCALIZATIONOFF, &CMainFrame::OnLocalizationOff)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{

	m_Source = 2;
	PlayStopButton = TRUE;
	NextButton = TRUE;

	Play = FALSE;
	Next = FALSE;
	NewFrame = FALSE;

	Stabilization = FALSE;
	Localization = FALSE;

	fr_width = 0;
	fr_height = 0;
	frame = NULL;

	File = _T("NULL");
	FilePath = _T("NULL");

	capture = FALSE;
	data = NULL;
	target.w = 0;
	target.h = 0;
	target.x = 0;
	target.y = 0;

	lock_frame =		::CreateEvent(NULL, FALSE, TRUE, NULL);
	libjpeg_thread =	::CreateEvent(NULL, FALSE, TRUE, NULL);

	main_thread = 		::CreateEvent(NULL, TRUE, FALSE, NULL);
	video_thread =		::CreateEvent(NULL, TRUE, TRUE, NULL);
	video_thread_run =  ::CreateEvent(NULL, TRUE, TRUE, NULL);
	ptz_thread =		::CreateEvent(NULL, TRUE, TRUE, NULL);

	jpeg_thread =		::CreateEvent(NULL, TRUE, TRUE, NULL);
	jpeg_thread_run =	::CreateThread(NULL, 0, CMainFrame::JPEGFunction, this, 0, NULL);


}

CMainFrame::~CMainFrame()
{
	::SetEvent(main_thread);
	::WaitForSingleObject(jpeg_thread, INFINITE);
	::WaitForSingleObject(video_thread, INFINITE);
	
	if (frame)		delete[] frame;
	if (data)		ml_track_release(data);
	
	if (lock_frame)			::CloseHandle(lock_frame);
	if (libjpeg_thread)		::CloseHandle(libjpeg_thread);
	if (main_thread)		::CloseHandle(main_thread);
	if (jpeg_thread)		::CloseHandle(jpeg_thread);
	if (jpeg_thread_run)	::CloseHandle(jpeg_thread_run);
	if (video_thread)		::CloseHandle(video_thread);
	if (video_thread_run)	::CloseHandle(video_thread_run);
	if (ptz_thread)			::CloseHandle(ptz_thread);

}

LRESULT CMainFrame::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
	SetWindowText(m_status);

	return 0;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	if(!m_wndToolBar.CreateEx(this, TBSTYLE_BUTTON, WS_CHILD | WS_VISIBLE | CBRS_TOP |
		CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect (0, 0, 0, 0)) ||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR1))
	{
		TRACE0("Failed to create Tool Bar\n");
		return -1;

	}


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnOpenFile()
{
	CFileDialog DlgOpen(TRUE);
	if(NextButton){
		DlgOpen.m_ofn.lpstrFilter = "Pictures (*.jpg)\0*.jpg\0";
		DlgOpen.m_ofn.lpstrDefExt = "jpg";
	}else {
		DlgOpen.m_ofn.lpstrFilter = "Video File (*.avi)\0*.avi\0";
		DlgOpen.m_ofn.lpstrDefExt = "avi";
	}
	int DlgResult = static_cast<int>(DlgOpen.DoModal());
	if(DlgResult == IDOK){
		File = DlgOpen.GetPathName();
		::WaitForSingleObject(lock_frame, INFINITE);
			capture = FALSE;
			target.y = 0;
			target.x = 0;
			target.w = 0;
			target.h = 0;
			ml_track_release(data);
			data = NULL;
		::SetEvent(lock_frame);
		Play = FALSE;
		Next = FALSE;
		NewFrame = TRUE;
	}
}


void CMainFrame::OnSelectSource()
{
	CSource Dlg(this);

	Dlg.m_Source = m_Source;
	Dlg.PlayStopButton = PlayStopButton;
	Dlg.NextButton = NextButton;

	int DlgResult = static_cast<int>(Dlg.DoModal());

	if(DlgResult == IDOK)
	{
		m_Source = Dlg.m_Source;
		PlayStopButton = Dlg.PlayStopButton;
		NextButton = Dlg.NextButton;
	}
	
	switch (m_Source)
	{
		case 1:
			::SetEvent(ptz_thread);
			if(::WaitForSingleObject(video_thread, 0) != WAIT_OBJECT_0)
				break;
			::CloseHandle(video_thread);
			video_thread = ::CreateThread(NULL, 0, CMainFrame::VideoFunction, this, 0, NULL);
			break;
		case 2:
			::SetEvent(ptz_thread);
			if(::WaitForSingleObject(jpeg_thread, 0) != WAIT_OBJECT_0)
				break;
			::CloseHandle(jpeg_thread);
			jpeg_thread = ::CreateThread(NULL, 0, CMainFrame::JPEGFunction, this, 0, NULL);
			break;
		case 3:
			::ResetEvent(ptz_thread);
			break;
	}
	return afx_msg void();
}

void CMainFrame::OnPlay()
{
	Play = TRUE;
	return afx_msg void();
}
void CMainFrame::OnNext()
{
	Next = TRUE;
	return afx_msg void();
}
void CMainFrame::OnStop()
{
	Play = FALSE;
	return afx_msg void();
}

void CMainFrame::OnUpdatePlay(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(PlayStopButton);
}

void CMainFrame::OnUpdateNext(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(NextButton);
}
void CMainFrame::OnUpdateStop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(PlayStopButton);
}
void CMainFrame::OnUpdateOpen(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(PlayStopButton);
}

DWORD WINAPI CMainFrame::JPEGFunction( LPVOID lpParam )
{
	CMainFrame *frm = (CMainFrame*) lpParam;
	::ResetEvent(frm->jpeg_thread);
	::WaitForSingleObject(frm->video_thread, INFINITE);
	
	double fps = 0;
	do{
		if(::WaitForSingleObject(frm->main_thread, 0) == WAIT_OBJECT_0)
			break;
		if(::WaitForSingleObject(frm->video_thread, 0) != WAIT_OBJECT_0)
			break;
		if(::WaitForSingleObject(frm->ptz_thread, 0) != WAIT_OBJECT_0)
			break;
////////////////////////////////////////////////////////////////////////////////
		if(frm->NewFrame){
			frm->FilePath = frm->File;
			frm->Next = TRUE;
			frm->Play = FALSE;
			//frm->NewFrame = FALSE;
		}

//Open File
		FILE* infile = NULL;
		if(!strcmp(frm->FilePath.GetBuffer(), "NULL"))
			continue;
		
		if(fopen_s(&infile, (char*)frm->FilePath.GetBuffer(), "rb")){
			::WaitForSingleObject(frm->lock_frame, INFINITE);
				frm->NewFrame = TRUE;
				frm->capture = FALSE;
				frm->target.x = 0;
				frm->target.y = 0;
				frm->target.w = 0;
				frm->target.h = 0;
				ml_track_release(frm->data);
				frm->data = NULL;
			::SetEvent(frm->lock_frame);
			continue;
		}

//Decompress File
		::WaitForSingleObject(frm->libjpeg_thread, INFINITE);

			frm->cinfo = new jpeg_decompress_struct;
			frm->jerr = new jpeg_error_mgr;
			frm->cinfo->err = jpeg_std_error(frm->jerr);
			jpeg_create_decompress(frm->cinfo);

			jpeg_stdio_src(frm->cinfo, infile);
			jpeg_read_header(frm->cinfo, TRUE);
			
			jpeg_start_decompress(frm->cinfo);

			int width = frm->cinfo->output_width;
			int height = frm->cinfo->output_height;
			int num_comp = frm->cinfo->output_components;
			
			unsigned char* frame = new unsigned char[width * height * num_comp];
			JSAMPROW row_buffer = (JSAMPROW)frame;
			int row_stride	= width * num_comp;

			while(frm->cinfo->output_scanline < frm->cinfo->output_height){
				jpeg_read_scanlines(frm->cinfo, &row_buffer, 1);
				row_buffer += row_stride;
			}
			
			jpeg_finish_decompress(frm->cinfo);

			if (frm->cinfo)		{jpeg_destroy_decompress(frm->cinfo); delete frm->cinfo;}
			if (frm->jerr)		delete frm->jerr;
			frm->cinfo = 0;
			frm->jerr = 0;
			fclose(infile);
		::SetEvent(frm->libjpeg_thread);

		if (::IsWindow(frm->m_wndView.m_hWnd))
			frm->m_wndView.Invalidate();
		
//Check whether need to update
		if(!frm->Play && !frm->Next){
			if(frame) delete[] frame;
			continue;
		}

		
//Close File, take the picture
	::WaitForSingleObject(frm->lock_frame, INFINITE);
		if(!frm->NewFrame && (frm->fr_width != width || frm->fr_height != height)){
			AfxMessageBox("Different resolution of frames. Stop tracking.");
			frm->capture = FALSE;
			frm->target.x = 0;
			frm->target.y = 0;
			frm->target.w = 0;
			frm->target.h = 0;
			ml_track_release(frm->data);
			frm->data = NULL;
		}
		
		frm->fr_width	= width;
		frm->fr_height	= height;
		if(frm->frame) delete[] frm->frame;
		frm->frame = new unsigned char[width * height * num_comp];

		unsigned char *src = frame;
		unsigned char *dst = frm->frame;
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				dst[0] = src[2];
				dst[1] = src[1];
				dst[2] = src[0];
				dst += 3;
				src += 3;
			}
		}
		if(frm->NewFrame){
			frm->data = ml_track_create(frm->fr_width, frm->fr_height);
			ml_track_init(frm->data, frm->frame, 3 * width, &frm->target);
			frm->NewFrame = FALSE;
		}
		if(frm->Next || frm->Play){
			clock_t start, finish;
			start = clock();
			double rho = ml_track_track(frm->data, frm->frame, 3 * width, &frm->target);
			finish = clock();
			double duration = (double)(finish - start)/CLOCKS_PER_SEC;
			if(duration)
				duration = 1 / duration;
			fps = (23 * fps + duration) / 24.0;
			rho *= 100;
			set_status("TRANSAS TeleAvtomat is working with FPS %.3lf (diff > 3 for %f%% )", fps, rho);
			//check
		}
	::SetEvent(frm->lock_frame);

		if(frame) delete[] frame;
		frm->Next = FALSE;
		
//Take new number		
		frm->FilePath.Delete(frm->FilePath.GetLength() - 4, 4);
		
		int num_size = 0;
		for(int i = frm->FilePath.GetLength() - 1; i >= 0; i--)
			if(isdigit(frm->FilePath.GetBuffer()[i]))
				num_size++;
			else
				break;
		
		char* buffer = new char[num_size + 1];
		strcpy_s(buffer, num_size + 1, 
			frm->FilePath.GetBuffer() + frm->FilePath.GetLength() - num_size);
		
		int frame_num = atoi(buffer);
		
		if(frame_num > pow(10.0, num_size) - 1){
			frm->FilePath = frm->File;
			continue;
		}

		frame_num++;
		_itoa_s(frame_num, buffer, num_size + 1, 10);

		int new_num_size = (int)strlen(buffer);
		
		frm->FilePath.Delete(frm->FilePath.GetLength() - num_size, num_size);
		for(int i = 0; i < num_size - new_num_size; i++)
			frm->FilePath += '0';

		frm->FilePath += buffer;
		frm->FilePath += ".jpg";

		if(buffer) delete[] buffer;

/////////////////////////////////////////////////////////////////////////////////
	}while(1);
	
	::WaitForSingleObject(frm->lock_frame, INFINITE);
		frm->fr_width = 0;
		frm->fr_height = 0;
		ml_track_release(frm->data);
		frm->data = NULL;
		if(frm->frame) {delete [] frm->frame, frm->frame = NULL;}
		frm->File = _T("NULL");
		frm->FilePath = _T("NULL");
		frm->Play = FALSE;
		frm->Next = FALSE;
		
		frm->capture = FALSE;
		frm->target.x = 0;
		frm->target.y = 0;
		frm->target.w = 0;
		frm->target.h = 0;
	::SetEvent(frm->lock_frame);

	if (::IsWindow(frm->m_wndView.m_hWnd))
		frm->m_wndView.Invalidate();

	::SetEvent(frm->jpeg_thread);
	return 0;
}
DWORD WINAPI CMainFrame::VideoFunction( LPVOID lpParam )
{
	CMainFrame *frm = (CMainFrame*) lpParam;
	::ResetEvent(frm->video_thread);
	::WaitForSingleObject(frm->jpeg_thread, INFINITE);
	
	do{
		if (::IsWindow(frm->m_wndView.m_hWnd))
			frm->m_wndView.Invalidate();

		if(::WaitForSingleObject(frm->main_thread, 0) == WAIT_OBJECT_0)
			break;
		if(::WaitForSingleObject(frm->jpeg_thread, 0) != WAIT_OBJECT_0)
			break;
		if(::WaitForSingleObject(frm->ptz_thread, 0) != WAIT_OBJECT_0)
			break;

	}while(1);
	
	::WaitForSingleObject(frm->lock_frame, INFINITE);
		frm->fr_width = 0;
		frm->fr_height = 0;
		if(frm->frame) {free(frm->frame), frm->frame = NULL;}
		frm->File = _T("NULL");
		frm->FilePath = _T("NULL");
		frm->Play = FALSE;
		frm->Next = FALSE;

		frm->capture = FALSE;
		frm->target.x = 0;
		frm->target.y = 0;
		frm->target.w = 0;
		frm->target.h = 0;
		ml_track_release(frm->data);
		frm->data = NULL;
	::SetEvent(frm->lock_frame);

	if (::IsWindow(frm->m_wndView.m_hWnd))
		frm->m_wndView.Invalidate();

	::SetEvent(frm->video_thread);
	return 0;
}
void CMainFrame::OnReplay()
{
	::WaitForSingleObject(lock_frame, INFINITE);
		NewFrame = TRUE;
		capture = FALSE;
		target.x = 0;
		target.y = 0;
		target.w = 0;
		target.h = 0;
		ml_track_release(data);
		data = NULL;
	::SetEvent(lock_frame);
}

void CMainFrame::OnUpdateReplay(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(PlayStopButton);
}
void CMainFrame::OnStabilization()
{
	Stabilization = TRUE;
}

void CMainFrame::OnStopStopstabilization()
{
	Stabilization = FALSE;
}

void CMainFrame::OnLocalizationOn()
{
	Localization = TRUE;
}

void CMainFrame::OnLocalizationOff()
{
	Localization = FALSE;
}
