#include "stdafx.h"

#include "resource.h"
#include "DistortionDialog.h"



IMPLEMENT_DYNAMIC(CDistortionDlg, CDialog)

CDistortionDlg::CDistortionDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDistortionDlg::IDD, pParent)
{
}

void CDistortionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	int compensate = compensateDistortions;
	DDX_Check(pDX, IDC_DISTORTION_COMPENSATE, compensate);
	compensateDistortions = compensate == 0 ? false : true;

	DDX_Text(pDX, IDC_DISTORTION_FX, params[0]);
	DDX_Text(pDX, IDC_DISTORTION_FY, params[1]);
	DDX_Text(pDX, IDC_DISTORTION_CX, params[2]);
	DDX_Text(pDX, IDC_DISTORTION_CY, params[3]);
	DDX_Text(pDX, IDC_DISTORTION_K1, params[4]);
	DDX_Text(pDX, IDC_DISTORTION_K2, params[5]);
	DDX_Text(pDX, IDC_DISTORTION_P1, params[6]);
	DDX_Text(pDX, IDC_DISTORTION_P2, params[7]);
}


BEGIN_MESSAGE_MAP(CDistortionDlg, CDialog)
END_MESSAGE_MAP()


