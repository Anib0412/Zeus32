/////////////////////////////////////////////////////////////////////////////
// BHEADER.CPP
//                                                                        
// BatchHeader class for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#include "stdafx.h"
#include "zeus.h"
#include "bheader.h"
#include "control.hm"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// the control help-map
static DWORD dwHelpMap[] = 
   {
   IDC_IMAGECORNERS, HIDC_BLIST_IMAGECORNERS,
   IDC_DATE, HIDC_BLIST_DATE,
   IDC_TIME, HIDC_BLIST_TIME,
   IDC_SHOW_IMAGECOUNT, HIDC_BLIST_SHOW_IMAGECOUNT,
   IDC_IMAGECOUNT_TITLE, HIDC_BLIST_IMAGECOUNTNUM,
   IDC_IMAGECOUNT_TITLE, HIDC_BLIST_IMAGECOUNTNUM,
   IDC_IMAGECOUNT_EDIT, HIDC_BLIST_IMAGECOUNTNUM,
   IDC_SPIN_CONTROL, HIDC_BLIST_IMAGECOUNTNUM,
   IDC_BACKGROUND, HIDC_BLIST_BACKGROUND,
   IDC_BACKGROUND_PRMPT, HIDC_BLIST_BACKGROUND,
   IDC_NOTES_PROMPT, HIDC_BLIST_NOTES,
   IDC_NOTES1, HIDC_BLIST_NOTES,
   IDC_NOTES2, HIDC_BLIST_NOTES,
   IDOK, HIDC_OUROK,
   IDCANCEL, HIDC_OURCANCEL,   
   0,0
   }; 

/////////////////////////////////////////////////////////////////////////////
// bheader dialog


CBheader::CBheader(CWnd* pParent /*=NULL*/)
	: CDialog(CBheader::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBheader)
	m_date = TRUE;
	m_time = TRUE;
	m_notes1 = _T("");
	m_notes2 = _T("");
	m_edit_imagecount = _T("");
	m_show_imagecount = TRUE;
	m_imagecorners = TRUE;	
	m_background = 0;
	//}}AFX_DATA_INIT
}

void CBheader::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBheader)
	DDX_Check(pDX, IDC_DATE, m_date);
	DDX_Check(pDX, IDC_TIME, m_time);
	DDX_Text(pDX, IDC_NOTES1, m_notes1);
	DDX_Text(pDX, IDC_NOTES2, m_notes2);
	DDX_Text(pDX, IDC_IMAGECOUNT_EDIT, m_edit_imagecount);
	DDX_Check(pDX, IDC_SHOW_IMAGECOUNT, m_show_imagecount);
	DDX_Check(pDX, IDC_IMAGECORNERS, m_imagecorners);	
	DDX_CBIndex(pDX, IDC_BACKGROUND, m_background);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBheader, CDialog)
	//{{AFX_MSG_MAP(CBheader)
	ON_WM_CONTEXTMENU()	
	ON_WM_HELPINFO()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBheader message handlers

BOOL CBheader::OnInitDialog()
   {   	
#ifdef USEKANJI	   
   // temp hacks!
   CString csTemp;
   ((CComboBox*)GetDlgItem(IDC_BACKGROUND))->ResetContent();
   csTemp.LoadString(IDS_CBOX_BATCH_BLACK);
   ((CComboBox*)GetDlgItem(IDC_BACKGROUND))->AddString(csTemp);
   csTemp.LoadString(IDS_CBOX_BATCH_WHITE);
   ((CComboBox*)GetDlgItem(IDC_BACKGROUND))->AddString(csTemp);
   csTemp.LoadString(IDS_CBOX_BATCH_NONE);
   ((CComboBox*)GetDlgItem(IDC_BACKGROUND))->AddString(csTemp);
#endif   

   CDialog::OnInitDialog();
   CenterWindow();   

   // set range of spin control
   ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_CONTROL))->SetRange(1, 10000);   
   
   return TRUE;  // return TRUE  unless you set the focus to a control
   }   

void CBheader::OnContextMenu(CWnd* pWnd, CPoint point) 
   {   
   // call context help
   ::WinHelp(pWnd->m_hWnd, AfxGetApp()->m_pszHelpFilePath, 
      HELP_CONTEXTMENU,  (DWORD)(LPVOID)dwHelpMap);
   }  

BOOL CBheader::OnHelpInfo(HELPINFO* pHelpInfo) 
   {
   if (pHelpInfo->iContextType == HELPINFO_WINDOW)   // must be for a control
      {
      ::WinHelp ((HWND)pHelpInfo->hItemHandle,
         AfxGetApp()->m_pszHelpFilePath, HELP_WM_HELP, 
		 (DWORD)(LPVOID)dwHelpMap);
	  return TRUE;
	  }      
   else
   	  return CDialog::OnHelpInfo(pHelpInfo);
   }

void CBheader::OnOK() 
   {
   UpdateData(TRUE);
   CDialog::OnOK();
   }
