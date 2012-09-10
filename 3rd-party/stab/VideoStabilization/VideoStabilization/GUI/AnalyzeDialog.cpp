#include "stdafx.h"

#include "resource.h"
#include "AnalyzeDialog.h"


IMPLEMENT_DYNAMIC(CAnalyzeDlg, CDialog)

CAnalyzeDlg::CAnalyzeDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAnalyzeDlg::IDD, pParent)
{
}

void CAnalyzeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_ANALYZE_SHIFT_X, shX);
	DDX_Text(pDX, IDC_ANALYZE_SHIFT_Y, shY);
	DDX_Text(pDX, IDC_ANALYZE_SHIFT_ANGLE, shAngle);
}


BEGIN_MESSAGE_MAP(CAnalyzeDlg, CDialog)
END_MESSAGE_MAP()


