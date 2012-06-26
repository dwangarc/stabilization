#include "stdafx.h"

#include "resource.h"
#include "ParamsDialog.h"


IMPLEMENT_DYNAMIC(CParamsDlg, CDialog)

CParamsDlg::CParamsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CParamsDlg::IDD, pParent)
{
}

void CParamsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_ITERATIONS, iterNum[0]);
	DDX_Text(pDX, IDC_EDIT_ITERATIONS1, iterNum[1]);
	DDX_Text(pDX, IDC_EDIT_ITERATIONS2, iterNum[2]);
	DDX_Text(pDX, IDC_EDIT_PYR_SIZE, pyrSize);
	DDX_Text(pDX, IDC_EDIT_ALPHA, alpha);
	DDX_Text(pDX, IDC_EDIT_DOWNSCALE, downscale);
	DDX_Text(pDX, IDC_EDIT_BAND_WIDTH, bandWidth);
	DDX_Text(pDX, IDC_EDIT_CORNER_DISTANCE, cornerDistance);
	DDX_Text(pDX, IDC_EDIT_SAFE_SHIFT, safeShift);
	DDX_Text(pDX, IDC_EDIT_SHIFT_PARAM, shiftParam);
	DDX_Check(pDX, IDC_EDIT_BGR, bgr);
}


BEGIN_MESSAGE_MAP(CParamsDlg, CDialog)
END_MESSAGE_MAP()


