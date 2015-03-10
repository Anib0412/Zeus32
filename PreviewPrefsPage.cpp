/////////////////////////////////////////////////////////////////////////////
// PreviewPrefsPage.cpp
//                                                                        
// preview preferences for RasterPlus95
// Copyright (C) 1996-98 Graphx, Inc.
//
// mods:
//
// 11-16-96  Build 46 - preview edit box let you set a higher number than max preview
//			               - fixed
// 9-28-98  Version 2.0.5 - Changed OnOurOk PostMessage to SendMessage for VC6.0 compat

#include "stdafx.h"
#include "zeus.h"
#include "PreviewPrefsPage.h"
#include "inidefs.h"
#include "profile.h"
#include "control.hm"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// the control help-map
static DWORD dwHelpMap[] = 
   {
   IDC_MAXPREVIEW_SPIN, HIDC_PREVIEW_MAXPREVIEW,
   IDC_MAXPREVIEW_EDIT, HIDC_PREVIEW_MAXPREVIEW,
   IDC_MAXPREVIEW_PRMPT, HIDC_PREVIEW_MAXPREVIEW,
   IDC_ICONIC, HIDC_PREVIEW_ICONIC,
   IDC_COMPRESSION, HIDC_PREVIEW_COMPRESSION,
   IDC_PREVIEWSIZE, HIDC_PREVIEW_PREVIEWSIZE,
   IDC_PAUSE, HIDC_PREVIEW_PAUSE,
   IDOUROK, HIDC_OUROK,
   IDOURCANCEL, HIDC_OURCANCEL,
   0,0
   };

/////////////////////////////////////////////////////////////////////////////
// CPreviewPrefsPage property page

IMPLEMENT_DYNCREATE(CPreviewPrefsPage, CPropertyPage)

CPreviewPrefsPage::CPreviewPrefsPage() : CPropertyPage(CPreviewPrefsPage::IDD)
{
	//{{AFX_DATA_INIT(CPreviewPrefsPage)		
	m_prev_maxopen = -1;
	m_compression = FALSE;
	m_prev_size = -1;
	m_pause = FALSE;
	m_largetoolbar = TRUE;
	//}}AFX_DATA_INIT	
}

CPreviewPrefsPage::~CPreviewPrefsPage()
{
}

void CPreviewPrefsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);		   
	//{{AFX_DATA_MAP(CPreviewPrefsPage)
	DDX_Control(pDX, IDC_MAXPREVIEW_SPIN, m_maxpreview_spin);
	DDX_Text(pDX, IDC_MAXPREVIEW_EDIT, m_prev_maxopen);	
	DDV_MinMaxInt(pDX, m_prev_maxopen, 1, IDDM_MAX_PREVNUM);	
	DDX_Check(pDX, IDC_COMPRESSION, m_compression);
	DDX_CBIndex(pDX, IDC_PREVIEWSIZE, m_prev_size);
	DDX_Check(pDX, IDC_PAUSE, m_pause);
	DDX_Check(pDX, IDC_LARGETOOLBAR, m_largetoolbar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreviewPrefsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPreviewPrefsPage)
	ON_BN_CLICKED(IDOUROK, OnOurOK)
	ON_BN_CLICKED(IDOURCANCEL, OnOurcancel)
	ON_WM_CONTEXTMENU()	
	ON_WM_HELPINFO()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreviewPrefsPage message handlers

BOOL CPreviewPrefsPage::OnInitDialog()
   {           
   CProfile Profile;

#ifdef USEKANJI	   
   // temp hacks!
   CString csTemp;      
   ((CComboBox*)GetDlgItem(IDC_PREVIEWSIZE))->ResetContent();
   csTemp.LoadString(IDS_CBOX_PREVIEW_SMALL);   
   ((CComboBox*)GetDlgItem(IDC_PREVIEWSIZE))->AddString(csTemp);
   csTemp.LoadString(IDS_CBOX_PREVIEW_MEDIUM);   
   ((CComboBox*)GetDlgItem(IDC_PREVIEWSIZE))->AddString(csTemp);
   csTemp.LoadString(IDS_CBOX_PREVIEW_LARGE);   
   ((CComboBox*)GetDlgItem(IDC_PREVIEWSIZE))->AddString(csTemp);
#endif  
    
   CDialog::OnInitDialog();		

   // read dialog vars	
   m_prev_size = Profile.GetInt(IDS_PREVIEW, IDS_PREVIEW_SIZE, IDDM_PREVIEW_SIZE);
   m_prev_maxopen = Profile.GetInt(IDS_PREVIEW, IDS_PREVIEW_NUM, IDDM_PREVIEW_NUM);   
   m_compression = Profile.GetInt(IDS_PREVIEW, IDS_PREVIEW_COMPRESSION, IDDM_PREVIEW_COMPRESSION);	
   m_largetoolbar = Profile.GetInt(IDS_GENERALSECTION, "LargeToolBar", TRUE);	
   m_pause = Profile.GetInt(IDS_PREVIEW, IDS_PREVIEW_PAUSE, IDDM_PREVIEW_PAUSE);	   
   UpdateData(FALSE);        
											 
   // set range of spin control
   m_maxpreview_spin.SetRange(IDDM_MIN_PREVNUM, IDDM_MAX_PREVNUM);

   return TRUE;  // return TRUE  unless you set the focus to a control
   }

void CPreviewPrefsPage::OnOurOK() 
   {   
   GetParent()->SendMessage(PSM_PRESSBUTTON, PSBTN_OK);
   ((CPropertySheet*)GetParent())->EndDialog(IDOK);   
   }

void CPreviewPrefsPage::OnOurcancel() 
   {   
   GetParent()->SendMessage(PSM_PRESSBUTTON, PSBTN_CANCEL);
   ((CPropertySheet*)GetParent())->EndDialog(IDCANCEL);
   }

void CPreviewPrefsPage::OnOK() 
   {
   CProfile Profile;   
    
   // make sure that the data is updated
   UpdateData(TRUE);       

   // write dialog vars	
   Profile.WriteInt(IDS_PREVIEW, IDS_PREVIEW_SIZE, m_prev_size);
   Profile.WriteInt(IDS_PREVIEW, IDS_PREVIEW_NUM, m_prev_maxopen);      
   Profile.WriteInt(IDS_PREVIEW, IDS_PREVIEW_COMPRESSION, m_compression);   
   Profile.WriteInt(IDS_GENERALSECTION, "LargeToolBar", m_largetoolbar);   
   Profile.WriteInt(IDS_PREVIEW, IDS_PREVIEW_PAUSE, m_pause);

   CPropertyPage::OnOK();
   }

BOOL CPreviewPrefsPage::OnHelpInfo(HELPINFO* pHelpInfo) 
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
 
void CPreviewPrefsPage::OnContextMenu(CWnd* pWnd, CPoint point) 
   {   
   // call context help
   ::WinHelp(pWnd->m_hWnd, AfxGetApp()->m_pszHelpFilePath, 
      HELP_CONTEXTMENU,  (DWORD)(LPVOID)dwHelpMap);
   }
