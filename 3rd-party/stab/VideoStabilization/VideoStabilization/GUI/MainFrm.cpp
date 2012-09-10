// MainFrm.cpp : implementation of the CMainFrame class
// enum

#include "stdafx.h"

#include <afxdlgs.h>
#include <WTypes.h>

#include "Resource.h"
#include "ModeDialog.h"
#include "ParamsDialog.h"
#include "DistortionDialog.h"
#include "DeStabilizeDialog.h"
#include "AnalyzeDialog.h"
#include "CameraDialog.h"
#include "SavingDialog.h"
#include "MotionDialog.h"
#include "RTP_Dialog.h"

#include "MainFrm.h"
#include "Include/capture_log.h"
#include "AboutDialog.h"
#include "Include/different_libs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


typedef			unsigned char		uchar;

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_UPDATEUISTATE()
	ON_WM_CLOSE()
	ON_COMMAND(IDM_OPENFILE, &CMainFrame::OnOpenFiles)
	ON_COMMAND(IDM_OPENURL, &CMainFrame::OnOpenURL)
	ON_COMMAND(ID_OPEN_RTP, &CMainFrame::OnOpenRTP)
	ON_COMMAND(IDM_OPENVIDEO, &CMainFrame::OnOpenVideo)
	ON_COMMAND(IDM_EXIT, &CMainFrame::OnClose)
	ON_COMMAND(IDM_ABOUT, &CMainFrame::OnAbout)
	ON_COMMAND(IDM_MODE, &CMainFrame::OnMode)
	ON_COMMAND(IDM_PARAMS, &CMainFrame::OnParams)
	ON_COMMAND(ID_TOOLS_RESTART, &CMainFrame::OnRestart)
	ON_COMMAND(ID_TOOLS_PAUSE, &CMainFrame::OnPause)
	ON_COMMAND(ID_TOOLS_DISTORTION, &CMainFrame::OnDistortion)
	ON_COMMAND(ID_TOOLS_DESTABILIZINGPARAMS, &CMainFrame::OnDestabParams)
	ON_COMMAND(ID_TOOLS_ANALIZATIONRESULTS, &CMainFrame::OnAnalyze)
	ON_COMMAND(ID_TOOLS_SAVINGVIDEOPARAMS, &CMainFrame::OnSavingVideoParams)
	ON_COMMAND(ID_TOOLS_MOTIONCAPTUREPARAMS, &CMainFrame::OnMotionParams)
//	ON_COMMAND(ID_TOOLS_START, &CMainFrame::OnStart)
//	ON_COMMAND(ID_TOOLS_STOP, &CMainFrame::OnStop)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	//	ID_INDICATOR_CAPS,
	//	ID_INDICATOR_NUM,
	//	ID_INDICATOR_SCRL,
};


CMainFrame::CMainFrame()
{
	mediator = 0;
}



CMainFrame::~CMainFrame()
{
	
	if (mediator) delete mediator;
	delete Capture_Log::getInstance();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
//	m_wndView.set_main_frame(this);
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	return 0;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


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
/*	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
*/
	// otherwise, do default handling

	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnClose()
{

	CFrameWnd::OnClose();
}

void CMainFrame::OnUpdateUIState(UINT nAction, UINT nUIElement)
{
	if (nAction != 0x4321 || nUIElement != 0x1234) return;	// check for magick value


}

bool CMainFrame::isFrameWindow()
{
	return ::IsWindow(m_wndView.m_hWnd) == 0 ? false : true;
}

void CMainFrame::FrmPostMessage(__in UINT Msg,	__in WPARAM wParam,	__in LPARAM lParam)
{
	::PostMessage(m_hWnd, Msg, wParam, lParam);
}

void CMainFrame::repaintFrame()
{
	m_wndView.InvalidateRect(&m_wndView.rc_view);
}

void CMainFrame::OnOpenVideo()
{
	mediator->stop();
	CFileDialog fdlg(TRUE, _T("Video"), _T("*.*"), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		_T("Video (*.avi)|*.avi|All Files (*.*)|*.*||"), this);

	if (fdlg.DoModal() != IDOK)
	{
		return;
	}
	mediator->getFramesGetter().openVideoFile(dif_lib::CStrToStr(fdlg.GetPathName()));
	mediator->getFramesGetter().useVideoData();
	mediator->restart();
}


void CMainFrame::OnOpenFiles()
{
	mediator->stop();
	CFileDialog fdlg(TRUE, _T("Photos"), _T("*.*"), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT,
		_T("Photo files (*.jpeg,*.jpg,*.bmp,*.png)|*.jpeg;*.jpg;*.bmp;*.png|All Files (*.*)|*.*||"), this);
	

	CString fileName;
	fdlg.GetOFN().lpstrFile = fileName.GetBuffer(100000);
	fdlg.GetOFN().nMaxFile = 5000;

	int res = fdlg.DoModal();
	if (res != IDOK)
	{
		return;
	}

	mediator->getFramesGetter().clearSelectedFrameNames();
	POSITION position = fdlg.GetStartPosition();
	//TODO
	fdlg.GetNextPathName(position); // Because there is a problem with including file under cursor
									// at the first position.
	while (position != NULL)
	{
		CT2CA pszConvertedAnsiString (fdlg.GetNextPathName(position));
		std::string name (pszConvertedAnsiString);
		mediator->getFramesGetter().addSelectedFrame(name);
	}
	fileName.ReleaseBuffer();
	mediator->getFramesGetter().useFileData();
	mediator->restart();
//	CString path_database = fdlg.GetPathName();
//		AfxMessageBox("Can't load calibration file");

}

void CMainFrame::OnOpenURL()
{
	CCameraDlg dlg;
	dlg.camIp = mediator->getFramesGetter().getCamIp();
	dlg.panasonic = mediator->getFramesGetter().isPanasonic();
	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	mediator->getFramesGetter().useCamData();
	mediator->getFramesGetter().setCamIp(dlg.camIp);
	mediator->getFramesGetter().setPanasonic(dlg.panasonic);
	mediator->restart();
}

void CMainFrame::OnOpenRTP()
{
	CRTPCameraDlg dlg;
	dlg.camPort = mediator->getFramesGetter().getCamRTPPort();
	dlg.interlace = mediator->getFramesGetter().interlaced();
	if (dlg.DoModal() != IDOK)
	{
		return;
	}
	mediator->getFramesGetter().useRTPData();
	mediator->getFramesGetter().setCamRTPPort(dlg.camPort);
	mediator->getFramesGetter().setInterlaced(dlg.interlace);
	mediator->restart();
}


void CMainFrame::setMediator(Mediator* mediator)
{
	this->mediator = mediator;
}

Mediator* CMainFrame::getMediator()
{
	return mediator;
}

void CMainFrame::OnAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CMainFrame::OnMode()
{
	CModeDlg dlg;
	dlg.modeType = mediator->getModeType();

	if (dlg.DoModal() != IDOK)
	{
		return;
	}
	mediator->setModeType(ModeType(dlg.modeType));
}

void CMainFrame::OnRestart()
{
	mediator->restart();
}


void CMainFrame::OnParams()
{
	CParamsDlg dlg;

	dlg.alpha = mediator->getParams().alpha;
	for (int i = 0; i < 3; ++i)
	{
		dlg.iterNum[i] = mediator->getParams().pyrIter.get()[i];
	}
	dlg.pyrSize = mediator->getParams().pyrSize;
	dlg.bandWidth = mediator->getParams().bandWidth;
	dlg.downscale = mediator->getParams().downscale;
	dlg.cornerDistance = mediator->getParams().maxCornerDistance;
	dlg.shiftParam = mediator->getParams().shiftParam;
	dlg.safeShift = mediator->getParams().safeShift;
	dlg.bgr = m_wndView.getBGR();

	if (!dlg.DoModal())
	{
		return;
	}

	m_wndView.setBGR(dlg.bgr == 0 ? false : true);
	if (dlg.pyrSize <= 0) return;
	sh_ptr_int pyrIter = sh_ptr_int(new int[std::max(dlg.pyrSize, 3)]);
	for (int i = 0; i < std::max(3, dlg.pyrSize); ++i)
	{
		pyrIter.get()[i] = dlg.iterNum[std::min(i, 2)];
	}
	bool ifRestart = mediator->getParams().setData(dlg.alpha, dlg.pyrSize, pyrIter, dlg.downscale, dlg.cornerDistance, dlg.shiftParam, dlg.safeShift);
	if (dlg.bandWidth >= 0 && dlg.bandWidth < 200)	mediator->getParams().bandWidth = dlg.bandWidth;
	mediator->getParams().maxCornerDistance = dlg.cornerDistance;
	if (ifRestart)
	{
		mediator->restart();
	}
}

void CMainFrame::OnPause()
{
	mediator->pause();
}

void CMainFrame::OnDistortion()
{
	CDistortionDlg dlg;

	sh_ptr_db params = mediator->getDistortionParams();
	for (int i = 0; i < 8; ++i)
	{
		dlg.params[i] = params.get()[i];
	}
	dlg.compensateDistortions = mediator->ifCompensateDistortions();

	if (!dlg.DoModal())
	{
		return;
	}

	for (int i = 0; i < 8; ++i)
	{
		params.get()[i] = dlg.params[i];
	}
	mediator->setDistortionParams(params.get());
	mediator->setCompensateDistortions(dlg.compensateDistortions);
}

void CMainFrame::OnDestabParams()
{
	CDeStabilizeDlg dlg;
	UnStabilizator* unStab = mediator->getUnstabilizator();
	dlg.shiftX = unStab->getShiftX();
	dlg.shiftY = unStab->getShiftY();
	dlg.shiftAngle = unStab->getShiftAngle();

	if (!dlg.DoModal())
	{
		return;
	}
	unStab->setShiftXYAngle(dlg.shiftX, dlg.shiftY, dlg.shiftAngle);
}

void CMainFrame::OnAnalyze()
{
	CAnalyzeDlg dlg;
	dlg.shX = mediator->getProjectiveAnalyzer().getMedX();
	dlg.shY = mediator->getProjectiveAnalyzer().getMedY();
	dlg.shAngle = mediator->getProjectiveAnalyzer().getMedAngle();
	if (!dlg.DoModal())
	{
		return;
	}
}

void CMainFrame::OnSavingVideoParams()
{
	CSavingVideoDlg dlg;
	dlg.saveResVideo = mediator->getResVideoSaver().ifSaveVideo();
	dlg.saveBaseVideo = mediator->getFramesGetter().getVideoSaver().ifSaveVideo();
	dlg.resVideoPath = mediator->getResVideoSaver().getOutputPath();
	dlg.baseVideoPath = mediator->getFramesGetter().getVideoSaver().getOutputPath();

	if (!dlg.DoModal())
	{
		return;
	}
	mediator->getResVideoSaver().setSaveVideo(dlg.saveResVideo);
	mediator->getResVideoSaver().setOutputPath(dlg.resVideoPath);
	mediator->getFramesGetter().getVideoSaver().setSaveVideo(dlg.saveBaseVideo);
	mediator->getFramesGetter().getVideoSaver().setOutputPath(dlg.baseVideoPath);
}

void CMainFrame::OnMotionParams()
{
	CMotionDlg dlg;
	sh_ptr_mtdtc motionDetector = mediator->getMotionDetector();
	
	dlg.eps = motionDetector->getEPS();
	dlg.updateIter = motionDetector->getUpdateIter();
	dlg.minObj = motionDetector->getMinObjectSize();
	dlg.minSpace = motionDetector->getMinFreeSpaceSize();
	dlg.baseEps = motionDetector->getBaseEps();
	dlg.downscale = motionDetector->getDownscale();
	dlg.smooth = motionDetector->getSmoothParam();

	if (!dlg.DoModal())
	{
		return;
	}

	motionDetector->setEPS(dlg.eps);
	motionDetector->setBaseEps(dlg.baseEps);
	motionDetector->setDownscale(dlg.downscale);
	motionDetector->setSmoothParam(dlg.smooth);
	motionDetector->setUpdateIter(dlg.updateIter);
	motionDetector->setMinFreeSpaceSize(dlg.minSpace);
	motionDetector->setMinObjectSize(dlg.minObj);
}

