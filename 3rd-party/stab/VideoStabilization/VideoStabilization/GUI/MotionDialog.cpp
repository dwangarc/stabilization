#include "stdafx.h"
#include "MotionDialog.h"

IMPLEMENT_DYNAMIC(CMotionDlg, CDialog)

CMotionDlg::CMotionDlg(CWnd* pParent /*=NULL*/)
: CDialog(CMotionDlg::IDD, pParent)
{
}

CMotionDlg::~CMotionDlg()
{
}

void CMotionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_MOTION_ITER, updateIter);
	DDX_Text(pDX, IDC_MOTION_EPS, eps);
	DDX_Text(pDX, IDC_MOTION_MIN_OBJ, minObj);
	DDX_Text(pDX, IDC_MOTION_MIN_SPACE, minSpace);
	DDX_Text(pDX, IDC_MOTION_BASE_EPS, baseEps);
	DDX_Text(pDX, IDC_MOTION_DOWNSCALE, downscale);
	DDX_Text(pDX, IDC_MOTION_SMOOTH, smooth);
}


BEGIN_MESSAGE_MAP(CMotionDlg, CDialog)
END_MESSAGE_MAP()
