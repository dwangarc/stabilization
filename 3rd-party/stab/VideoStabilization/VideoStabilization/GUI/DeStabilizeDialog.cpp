#include "stdafx.h"

#include "resource.h"
#include "DeStabilizeDialog.h"


IMPLEMENT_DYNAMIC(CDeStabilizeDlg, CDialog)

CDeStabilizeDlg::CDeStabilizeDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDeStabilizeDlg::IDD, pParent)
{
}

void CDeStabilizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_DESTAB_SHIFT_X, shiftX);
	DDX_Text(pDX, IDC_DESTAB_SHIFT_Y, shiftY);
	DDX_Text(pDX, IDC_DESTAB_SHIFT_ANGLE, shiftAngle);
}


BEGIN_MESSAGE_MAP(CDeStabilizeDlg, CDialog)
END_MESSAGE_MAP()


