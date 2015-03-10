/////////////////////////////////////////////////////////////////////////////
// GeneralPrefsPage.cpp
//                                                                        
// general preferences for RasterPlus
// Copyright (C) 1996-2000 Graphx, Inc.
//
// mods:
//
// 11-16-96  Build 46 - preview edit box let you set a higher number than max preview
//			               - fixed
// 9-28-98  Version 2.0.5 - Changed OnOurOk PostMessage to SendMessage for VC6.0 compat
// 7-14-00  Version 4.0.0 - Added rulers and units

#include "stdafx.h"
#include "zeus.h"
#include "GeneralPrefsPage.h"
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
   IDC_PREVIEWSIZE_PRMPT, HIDC_PREVIEW_PREVIEWSIZE,
   IDC_PREVIEWSIZE, HIDC_PREVIEW_PREVIEWSIZE,   
   IDC_PAUSE, HIDC_PREVIEW_PAUSE,
   IDC_AUTOADVANCE, HIDC_AUTOADVANCE,
   IDC_ALWAYSOPEN, HIDC_ALWAYSOPEN,
   IDC_LARGETOOLBAR, HIDC_LARGETOOLBAR,
   IDC_ENABLEPRINTSERVER, HIDC_ENABLEPRINTSERVER,
   IDC_LEFTLOGO_PRMPT, HIDC_LOGO,
   IDC_LEFTLOGO_EDIT, HIDC_LOGO,
   IDC_LEFTLOGO_LOAD, HIDC_LOGO,
   IDC_RIGHTLOGO_PRMPT, HIDC_LOGO,
   IDC_RIGHTLOGO_EDIT, HIDC_LOGO,
   IDC_RIGHTLOGO_LOAD, HIDC_LOGO,
   IDOUROK, HIDC_OUROK,
   IDOURCANCEL, HIDC_OURCANCEL,
   0,0
   };

/////////////////////////////////////////////////////////////////////////////
// CGeneralPrefsPage property page

IMPLEMENT_DYNCREATE(CGeneralPrefsPage, CPropertyPage)

CGeneralPrefsPage::CGeneralPrefsPage() : CPropertyPage(CGeneralPrefsPage::IDD)
{
	//{{AFX_DATA_INIT(CGeneralPrefsPage)		
	m_prev_maxopen = -1;	
	m_prev_size = -1;
	m_pause = FALSE;
	m_largetoolbar = TRUE;
	m_always_open = FALSE;
	m_auto_advance = FALSE;
	m_use_rulers = FALSE;
	m_rulers_units = -1;
	m_enableprintserver = FALSE;
	//}}AFX_DATA_INIT
}

CGeneralPrefsPage::~CGeneralPrefsPage()
{
}

void CGeneralPrefsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);		   
	//{{AFX_DATA_MAP(CGeneralPrefsPage)
	DDX_Control(pDX, IDC_MAXPREVIEW_SPIN, m_maxpreview_spin);
	DDX_Text(pDX, IDC_MAXPREVIEW_EDIT, m_prev_maxopen);	
	DDV_MinMaxInt(pDX, m_prev_maxopen, 1, IDDM_MAX_PREVNUM);		
	DDX_CBIndex(pDX, IDC_PREVIEWSIZE, m_prev_size);
	DDX_Check(pDX, IDC_PAUSE, m_pause);
	DDX_Check(pDX, IDC_LARGETOOLBAR, m_largetoolbar);
	DDX_Check(pDX, IDC_ALWAYSOPEN, m_always_open);
	DDX_Check(pDX, IDC_AUTOADVANCE, m_auto_advance);
	DDX_Check(pDX, IDC_SHOWRULERS, m_use_rulers);
	DDX_CBIndex(pDX, IDC_UNITS, m_rulers_units);
	DDX_Check(pDX, IDC_ENABLEPRINTSERVER, m_enableprintserver);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGeneralPrefsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGeneralPrefsPage)
	ON_BN_CLICKED(IDOUROK, OnOurOK)
	ON_BN_CLICKED(IDOURCANCEL, OnOurcancel)
	ON_WM_CONTEXTMENU()	
	ON_WM_HELPINFO()	
	ON_BN_CLICKED(IDC_SHOWRULERS, OnShowrulers)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeneralPrefsPage message handlers

BOOL CGeneralPrefsPage::OnInitDialog()
   {           
   CProfile Profile;
   CString csTemp;

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
   m_auto_advance = Profile.GetInt(IDS_PREVIEW, IDS_AUTOADVANCE, TRUE);
   m_always_open = Profile.GetInt(IDS_PREVIEW, IDS_ALWAYS_OPEN, FALSE);
   m_prev_size = Profile.GetInt(IDS_PREVIEW, IDS_PREVIEW_SIZE, IDDM_PREVIEW_SIZE);
   m_prev_maxopen = Profile.GetInt(IDS_PREVIEW, IDS_PREVIEW_NUM, IDDM_PREVIEW_NUM);      
   m_largetoolbar = Profile.GetInt(IDS_GENERALSECTION, IDS_LARGETOOLBAR, TRUE);	
   m_pause = Profile.GetInt(IDS_PREVIEW, IDS_PREVIEW_PAUSE, IDDM_PREVIEW_PAUSE);	   
   m_use_rulers = Profile.GetInt(IDS_PREVIEW, IDS_USERULERS, IDDM_PREVIEW_USERULERS);	   
   m_rulers_units = Profile.GetInt(IDS_PREVIEW, IDS_RULERSUNITS, IDDM_PREVIEW_RULERSUNITS);	
   m_enableprintserver = Profile.GetInt(IDS_PHOTOGIZE, IDS_PRINTSERVER, FALSE);		 

   UpdateData(FALSE);        
											 
   // set range of spin control
   m_maxpreview_spin.SetRange(IDDM_MIN_PREVNUM, IDDM_MAX_PREVNUM);

   GetDlgItem(IDC_UNITS_PROMPT)->EnableWindow(m_use_rulers);   
   GetDlgItem(IDC_UNITS)->EnableWindow(m_use_rulers);   

   return TRUE;  // return TRUE  unless you set the focus to a control
   }

void CGeneralPrefsPage::OnOurOK() 
   {   
   GetParent()->SendMessage(PSM_PRESSBUTTON, PSBTN_OK);
   ((CPropertySheet*)GetParent())->EndDialog(IDOK);   
   }

void CGeneralPrefsPage::OnOurcancel() 
   {   
   GetParent()->SendMessage(PSM_PRESSBUTTON, PSBTN_CANCEL);
   ((CPropertySheet*)GetParent())->EndDialog(IDCANCEL);
   }

void CGeneralPrefsPage::OnOK() 
   {
   CProfile Profile;   
    
   // make sure that the data is updated
   UpdateData(TRUE);       

   // write dialog vars	
   Profile.WriteInt(IDS_PREVIEW, IDS_PREVIEW_SIZE, m_prev_size);
   Profile.WriteInt(IDS_PREVIEW, IDS_ALWAYS_OPEN, m_always_open);
   Profile.WriteInt(IDS_PREVIEW, IDS_AUTOADVANCE, m_auto_advance);
   Profile.WriteInt(IDS_PREVIEW, IDS_PREVIEW_NUM, m_prev_maxopen);         
   Profile.WriteInt(IDS_GENERALSECTION, IDS_LARGETOOLBAR, m_largetoolbar);   
   Profile.WriteInt(IDS_PREVIEW, IDS_PREVIEW_PAUSE, m_pause);
   Profile.WriteInt(IDS_PREVIEW, IDS_USERULERS, m_use_rulers);
   Profile.WriteInt(IDS_PREVIEW, IDS_RULERSUNITS, m_rulers_units);
   Profile.WriteInt(IDS_PHOTOGIZE, IDS_PRINTSERVER, m_enableprintserver);

   CPropertyPage::OnOK();
   }

BOOL CGeneralPrefsPage::OnHelpInfo(HELPINFO* pHelpInfo) 
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
 
void CGeneralPrefsPage::OnContextMenu(CWnd* pWnd, CPoint point) 
   {   
   // call context help
   ::WinHelp(pWnd->m_hWnd, AfxGetApp()->m_pszHelpFilePath, 
      HELP_CONTEXTMENU,  (DWORD)(LPVOID)dwHelpMap);   
   }

void CGeneralPrefsPage::OnShowrulers() 
   {
   // get data from dialog
   UpdateData(TRUE);		

   GetDlgItem(IDC_UNITS_PROMPT)->EnableWindow(m_use_rulers);   
   GetDlgItem(IDC_UNITS)->EnableWindow(m_use_rulers);   
   }