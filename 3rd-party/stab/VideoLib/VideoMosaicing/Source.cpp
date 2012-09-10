// Source.cpp : implementation file
//

#include "stdafx.h"
#include "VideoMosaic.h"
#include "Source.h"


// CSource dialog

IMPLEMENT_DYNAMIC(CSource, CDialog)

CSource::CSource(CWnd* pParent /*=NULL*/)
	: CDialog(CSource::IDD, pParent)
{

}

CSource::~CSource()
{
}

void CSource::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CSource :: OnInitDialog(void)
{
	CDialog :: OnInitDialog();

	Video1 = static_cast<CButton*>(GetDlgItem(IDC_VIDEO));
	Video2 = static_cast<CButton*>(GetDlgItem(IDC_JPEG));
	Video3 = static_cast<CButton*>(GetDlgItem(IDC_PTZ));

	switch (m_Source)
	{
		case 1:
			Video1->SetCheck(1);
			break;

		case 2:
			Video2->SetCheck(1);
			break;

		case 3:
			Video3->SetCheck(1);
			break;
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CSource, CDialog)
	ON_BN_CLICKED(IDC_VIDEO, &CSource::OnVideo)
	ON_BN_CLICKED(IDC_JPEG, &CSource::OnJPEG)
	ON_BN_CLICKED(IDC_PTZ, &CSource::OnPTZ)
END_MESSAGE_MAP()

void CSource::OnVideo()
{
	m_Source = 1;
	PlayStopButton = TRUE;
	NextButton = FALSE;
	
	return;
}
void CSource::OnJPEG()
{
	m_Source = 2;
	PlayStopButton = TRUE;
	NextButton = TRUE;
	
	return;
}
void CSource::OnPTZ()
{
	m_Source = 3;
	PlayStopButton = FALSE;
	NextButton = FALSE;
	
	return;
}

