#include "stdafx.h"

#include "resource.h"
#include "SavingDialog.h"
#include "Include/different_libs.h"



IMPLEMENT_DYNAMIC(CSavingVideoDlg, CDialog)

CSavingVideoDlg::CSavingVideoDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSavingVideoDlg::IDD, pParent)
{
}

void CSavingVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	int saveBase = saveBaseVideo;
	DDX_Check(pDX, IDC_SAVE_BASE_VIDEO, saveBase);
	saveBaseVideo = saveBase == 0 ? false : true;

	int saveRes = saveResVideo;
	DDX_Check(pDX, IDC_SAVE_RES_VIDEO, saveRes);
	saveResVideo = saveRes == 0 ? false : true;

	CString cBaseVideoPath = baseVideoPath.c_str();
	CString cResVideoPath = resVideoPath.c_str();

	DDX_Text(pDX, IDC_BASE_VIDEO_PATH, cBaseVideoPath);
	DDX_Text(pDX, IDC_RES_VIDEO_PATH, cResVideoPath);

	baseVideoPath = dif_lib::CStrToStr(cBaseVideoPath);
	resVideoPath = dif_lib::CStrToStr(cResVideoPath);
}


BEGIN_MESSAGE_MAP(CSavingVideoDlg, CDialog)
END_MESSAGE_MAP()


