#include "stdafx.h"
#include "resource.h"
#include "RTP_Dialog.h"


IMPLEMENT_DYNAMIC(CRTPCameraDlg, CDialog)

CRTPCameraDlg::CRTPCameraDlg(CWnd* pParent /*=NULL*/)
: CDialog(CRTPCameraDlg::IDD, pParent)
{
}

void CRTPCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_CAM_PORT, camPort);
	int val = interlace;
	DDX_Check(pDX, IDC_DEINTERLACE, val);
	interlace = val == 0 ? false : true;
}


BEGIN_MESSAGE_MAP(CRTPCameraDlg, CDialog)
END_MESSAGE_MAP()


