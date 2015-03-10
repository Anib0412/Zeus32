// WebUpdate.cpp : implementation file
//

#include "stdafx.h"
#include "zeus.h"
#include "WebUpdate.h"
#include "profile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWebUpdate dialog


CWebUpdate::CWebUpdate(CWnd* pParent /*=NULL*/)
	: CDialog(CWebUpdate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWebUpdate)
	m_status = _T("");
	m_current_version = _T("");
	m_avail_version = _T("");
	//}}AFX_DATA_INIT
}


void CWebUpdate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebUpdate)
	DDX_Text(pDX, IDC_STATUS, m_status);
	DDX_Text(pDX, IDC_CURRENTVERSION, m_current_version);
	DDX_Text(pDX, IDC_AVAILVERSION, m_avail_version);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWebUpdate, CDialog)
	//{{AFX_MSG_MAP(CWebUpdate)
	ON_BN_CLICKED(IDC_CHECKUPDATE, OnCheckupdate)
	ON_BN_CLICKED(IDC_GETUPDATE, OnGetupdate)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebUpdate message handlers

void CWebUpdate::OnGetupdate() 
   {
   }

BOOL CWebUpdate::OnInitDialog() 
   {
   CDialog::OnInitDialog();   

   m_current_version.LoadString(IDS_RP95_BUILDVER);
   m_avail_version = "N/A";
   GetDlgItem(IDC_GETUPDATE)->EnableWindow(FALSE);

   UpdateData(FALSE);
	
   return TRUE;  	              
   }

void CWebUpdate::OnDestroy() 
   {
   CDialog::OnDestroy();
   }

void CWebUpdate::OnCheckupdate() 
   {
   CProfile Profile;
   CString cs;
   CInternetSession* pcis;         
   CString csDst;
   CString csSrc;

   GetDlgItem(IDOK)->EnableWindow(FALSE);
   GetDlgItem(IDC_CHECKUPDATE)->EnableWindow(FALSE);

   m_status = "Checking for update...";
   UpdateData(FALSE);

   // create the file names
   csSrc = "UPDATE.INI";
   csDst = Profile.GetStr(IDS_SERVER, IDS_APPDIR);   
   VERIFYPATH(csDst);
   csDst += csSrc;		   

   // make a new internet connection
   pcis = new CInternetSession;
   CFtpConnection* pFtp;

   m_status = "Connecting to Gaphx FTP server...";
   UpdateData(FALSE);
   
   // connect to the Graphx FTP update site
   try
	  {
	  pFtp = pcis->GetFtpConnection("ftp.graphx.com", "rpwupdate", 
	     "abcd1234");   
	  }
   catch(CInternetException* theException)
	  {
	  // to get rid of warning
	  int i = theException->m_dwError;
	  pcis->Close();      
      delete pcis;
	  theException->Delete();      
      m_status = "Can't connect to Gaphx FTP server!";
      UpdateData(FALSE);
	  GetDlgItem(IDOK)->EnableWindow(TRUE);
	  GetDlgItem(IDC_CHECKUPDATE)->EnableWindow(TRUE);
	  return;
	  }   

   // the 
   if (!pFtp->GetFile(csSrc, csDst))
	  {
	  m_status = "Can't get update information...";
	  UpdateData(FALSE);
	  }

   pFtp->Close();
   pcis->Close();
   delete pFtp;   
   delete pcis;

   GetDlgItem(IDOK)->EnableWindow(TRUE);
   GetDlgItem(IDC_CHECKUPDATE)->EnableWindow(TRUE);

   return;
   }

void CWebUpdate::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnClose();
}
