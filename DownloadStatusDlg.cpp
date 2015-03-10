// DownloadStatusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "zeus.h"
#include "DownloadStatusDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDownloadStatusDlg dialog


CDownloadStatusDlg::CDownloadStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDownloadStatusDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDownloadStatusDlg)
	m_status = _T("");
	//}}AFX_DATA_INIT	  
	bCancel = FALSE;
}


void CDownloadStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDownloadStatusDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Text(pDX, IDC_STATUS, m_status);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDownloadStatusDlg, CDialog)
	//{{AFX_MSG_MAP(CDownloadStatusDlg)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDownloadStatusDlg message handlers

void CDownloadStatusDlg::OnCancel() 
   {
   bCancel = TRUE;	
   }

BOOL CDownloadStatusDlg::OnInitDialog() 
   {
   CDialog::OnInitDialog();
   CenterWindow();   	

   return TRUE;
   }