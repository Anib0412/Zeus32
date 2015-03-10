/////////////////////////////////////////////////////////////////////////////
// QPREFS.CPP
//                                                                        
// Queue properties dialogs for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//
// mods:
//
// 8-08-96  Build 41 - load "Queue Properties" from string table
// 9-28-98  Version 2.0.5 - Changed OnOurOk PostMessage to SendMessage for VC6.0 compat

#include "stdafx.h"
#include "zeus.h"    
#include "qprefs.h"
#include "inidefs.h"
#include "profile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT NEAR wm_GlobalChange = RegisterWindowMessage( GLOBAL_PREFS_HAS_CHANGED );

/////////////////////////////////////////////////////////////////////////////
// CQueuePrefs dialog

CQueuePrefs::CQueuePrefs(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:COurPropertySheet(nIDCaption, pParentWnd, iSelectPage)
   {
   InitVars();	
   }

CQueuePrefs::CQueuePrefs(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:COurPropertySheet(pszCaption, pParentWnd, iSelectPage)
   {
   InitVars();			
   }

CQueuePrefs::~CQueuePrefs()
   {
   }
   		              
BEGIN_MESSAGE_MAP(CQueuePrefs, COurPropertySheet)
	//{{AFX_MSG_MAP(CQueuePrefs)       		
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQueuePrefs message handlers 

void CQueuePrefs::InitVars()
   {   		   
   AddPage(&m_dialog_dev_setup);
   AddPage(&m_dialog_trans_setup);
   //AddPage(&m_dialog_rip_setup);
   AddPage(&m_dialog_server_setup);
   }                               

BOOL CQueuePrefs::OnInitDialog() 
   {
   CString csTemp, csTemp2;

   COurPropertySheet::OnInitDialog();  

   // make all pages active so as to force OnInits to 
   // be called 
   SetActivePage(&m_dialog_dev_setup);   
   SetActivePage(&m_dialog_trans_setup);
   //SetActivePage(&m_dialog_rip_setup);
   SetActivePage(&m_dialog_server_setup);   
   
   // now set it to the page we used last
   SetActivePage(LastPage);
   
   // set title
   csTemp.LoadString(IDS_QUEUEPROPS_TITLE);
   SetWindowText(csTemp);

   return TRUE;  	             
   }

/////////////////////////////////////////////////////////////////////////////
// CGPrefDlgRip dialog

CGPrefDlgRip::CGPrefDlgRip() : CPropertyPage(CGPrefDlgRip::IDD)
{   	
	//{{AFX_DATA_INIT(CGPrefDlgRip)	
	m_atmpath = _T("");
	m_ttpath = _T("");	
	//}}AFX_DATA_INIT	
}

void CGPrefDlgRip::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGPrefDlgRip)	
	DDX_Text(pDX, IDC_ATMPATH, m_atmpath);
	DDX_Text(pDX, IDC_TTPATH, m_ttpath);	
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGPrefDlgRip, CPropertyPage)
	//{{AFX_MSG_MAP(CGPrefDlgRip)	
	ON_BN_CLICKED(IDOUROK, OnOurOK)
	ON_BN_CLICKED(IDOURCANCEL, OnOurCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGPrefDlgRip message handlers

BOOL CGPrefDlgRip::OnInitDialog()
   {
   CPropertyPage::OnInitDialog();
   CProfile Profile;
   
   // get the rip data
   m_atmpath = Profile.GetStr(IDS_NEWRIPSECTION, IDS_NEWRIP_ATMPATH);
   m_ttpath = Profile.GetStr(IDS_NEWRIPSECTION, IDS_NEWRIP_TTPATH);   
   	
   // update the dialog
   UpdateData(FALSE);  
	
   return TRUE;  // return TRUE  unless you set the focus to a control	
   }

void CGPrefDlgRip::OnOurCancel()
   {
   // send cancel to parent     
   GetParent()->SendMessage(PSM_PRESSBUTTON, PSBTN_CANCEL);
   ((CPropertySheet*)GetParent())->EndDialog(IDCANCEL);   
   }

void CGPrefDlgRip::OnOurOK()
   {                      
   // tell parent that we've selected OK
   // it will handle saving data    
   GetParent()->SendMessage(PSM_PRESSBUTTON, PSBTN_OK);
   ((CPropertySheet*)GetParent())->EndDialog(IDOK);   
   }

void CGPrefDlgRip::OnOK()
   {   
   CProfile Profile;

   // make sure that the data is updated
   UpdateData(TRUE);          
   
   // write the rip data
   Profile.WriteStr(IDS_NEWRIPSECTION, IDS_NEWRIP_ATMPATH, m_atmpath);
   Profile.WriteStr(IDS_NEWRIPSECTION, IDS_NEWRIP_TTPATH, m_ttpath);      
   }  