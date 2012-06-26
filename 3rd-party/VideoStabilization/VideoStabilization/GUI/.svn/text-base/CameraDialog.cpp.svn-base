#include "stdafx.h"

#include "resource.h"
#include "CameraDialog.h"


IMPLEMENT_DYNAMIC(CCameraDlg, CDialog)

CCameraDlg::CCameraDlg(CWnd* pParent /*=NULL*/)
: CDialog(CCameraDlg::IDD, pParent)
{
}

void CCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	CString ip = camIp.c_str();
	DDX_Text(pDX, IDC_EDIT_CAM_IP, ip);

	CT2CA pszConvertedAnsiString (ip);
	camIp = std::string(pszConvertedAnsiString);
}


BEGIN_MESSAGE_MAP(CCameraDlg, CDialog)
END_MESSAGE_MAP()


