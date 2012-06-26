#include "stdafx.h"
#include "ModeDialog.h"
#include "Include/ModeTypes.h"

IMPLEMENT_DYNAMIC(CModeDlg, CDialog)

CModeDlg::CModeDlg(CWnd* pParent /*=NULL*/)
: CDialog(CModeDlg::IDD, pParent)
{
	modeType = STABILIZE;
}

CModeDlg::~CModeDlg()
{
}

void CModeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	switch (modeType)
	{
	case STABILIZE:
		OnModeStabilization();
		break;
	case UN_STABILIZE:
		OnModeUnStabilization();
		break;
	case ANALYZE :
		OnModeAnalyze();
		break;
	case BASE :
		OnModeBase();
		break;
	case MOTION_DETECTION :
		OnModeMotion();
		break;
	case TRACK :
		OnModeTrack();
		break;
	default :
		OnModeUnChanged();
	}

}


BEGIN_MESSAGE_MAP(CModeDlg, CDialog)
	ON_BN_CLICKED(IDC_STABILIZE, &CModeDlg::OnModeStabilization)
	ON_BN_CLICKED(IDC_UNSTABILIZE, &CModeDlg::OnModeUnStabilization)
	ON_BN_CLICKED(IDC_ANALYZE, &CModeDlg::OnModeAnalyze)
	ON_BN_CLICKED(IDC_BASE, &CModeDlg::OnModeBase)
	ON_BN_CLICKED(IDC_MOTION, &CModeDlg::OnModeMotion)
	ON_BN_CLICKED(IDC_TRACK, &CModeDlg::OnModeTrack)
END_MESSAGE_MAP()


void CModeDlg::OnModeStabilization()
{
	OnModeUnChanged();
	::SendMessage(::GetDlgItem(m_hWnd, IDC_STABILIZE),	BM_SETCHECK, 1, 0);
	modeType = STABILIZE;
	OnModeChanged();
}

void CModeDlg::OnModeUnStabilization()
{
	OnModeUnChanged();
	::SendMessage(::GetDlgItem(m_hWnd, IDC_UNSTABILIZE),	BM_SETCHECK, 1, 0);
	modeType = UN_STABILIZE;
	OnModeChanged();
}

void CModeDlg::OnModeAnalyze()
{
	OnModeUnChanged();
	::SendMessage(::GetDlgItem(m_hWnd, IDC_ANALYZE),	BM_SETCHECK, 1, 0);
	modeType = ANALYZE;
	OnModeChanged();
}

void CModeDlg::OnModeBase()
{
	OnModeUnChanged();
	::SendMessage(::GetDlgItem(m_hWnd, IDC_BASE),	BM_SETCHECK, 1, 0);
	modeType = BASE;
	OnModeChanged();
}

void CModeDlg::OnModeMotion()
{
	OnModeUnChanged();
	::SendMessage(::GetDlgItem(m_hWnd, IDC_MOTION),	BM_SETCHECK, 1, 0);
	modeType = MOTION_DETECTION;
	OnModeChanged();
}

void CModeDlg::OnModeTrack()
{
	OnModeUnChanged();
	::SendMessage(::GetDlgItem(m_hWnd, IDC_TRACK),	BM_SETCHECK, 1, 0);
	modeType = TRACK;
	OnModeChanged();
}

void CModeDlg::OnModeChanged()
{
#define CTL_ENABLE(idx)		::EnableWindow(::GetDlgItem(m_hWnd, idx), TRUE);
#define CTL_DISABLE(idx)	::EnableWindow(::GetDlgItem(m_hWnd, idx), FALSE);

#undef CTL_ENABLE
#undef CTL_DISABLE
}

void CModeDlg::OnModeUnChanged() 
{
	::SendMessage(::GetDlgItem(m_hWnd, IDC_STABILIZE),	BM_SETCHECK, 0, 0);
	::SendMessage(::GetDlgItem(m_hWnd, IDC_UNSTABILIZE),BM_SETCHECK, 0, 0);
	::SendMessage(::GetDlgItem(m_hWnd, IDC_ANALYZE),	BM_SETCHECK, 0, 0);
	::SendMessage(::GetDlgItem(m_hWnd, IDC_BASE),		BM_SETCHECK, 0, 0);
	::SendMessage(::GetDlgItem(m_hWnd, IDC_MOTION),		BM_SETCHECK, 0, 0);
	::SendMessage(::GetDlgItem(m_hWnd, IDC_TRACK),		BM_SETCHECK, 0, 0);
}
