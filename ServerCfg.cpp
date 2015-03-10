/////////////////////////////////////////////////////////////////////////////
// ServerCfg.cpp
//                                                                        
// Server properties dialogs for RasterPlus95
// Copyright (C) 1996-98 Graphx, Inc.
//											  											  
// mods:
//
// 7-06-96  Build 40 - The transform selected in the Server cfg dialog was not 
//					      being loaded into Server.Transform
// 9-28-98  Version 2.0.5 - Changed OnOurOk PostMessage to SendMessage for VC6.0 compat

#include "stdafx.h"
#include "zeus.h"
#include "ServerCfg.h"
#include "rexdirdi.h"
#include "control.hm"
#include "profile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ENABLEDLGITEM(a,b) GetDlgItem(a)->EnableWindow(b)

// the control help-map
static DWORD dwHelpMap[] = 
   {
   IDC_STATUS, HIDC_SERVER_STATUS,
   IDC_STATUS_PRMPT, HIDC_SERVER_STATUS,
   IDC_TRANSFORM, HIDC_SERVER_TRANSFORM,
   IDC_TRANSFORM_PRMPT, HIDC_SERVER_TRANSFORM,
   IDC_DELETEFILES, HIDC_SERVER_DELETEFILES,
   IDC_ENABLE_POLLING, HIDC_SERVER_POLLING,
   IDC_POLLING_DIR, HIDC_SERVER_POLLING,
   IDC_POLLING_DIR_PRMPT, HIDC_SERVER_POLLING,
   IDC_DELAY_EDIT, HIDC_SERVER_DELAY,
   IDC_DELAY_PRMPT2, HIDC_SERVER_DELAY,
   IDC_DELAY_PRMPT, HIDC_SERVER_DELAY,
   IDC_DELAY_SPIN, HIDC_SERVER_DELAY,
   IDC_IMAGEONLY_NEW, HIDC_SERVER_IMAGEONLY_NEW,      
   IDC_POLL_DIR, HIDC_SERVER_POLL_DIR,
   IDC_DEFAULTS, HIDC_SERVER_DEFAULTS,
   IDOUROK, HIDC_OUROK,
   IDOURCANCEL, HIDC_OURCANCEL,   
   0,0
   };

/////////////////////////////////////////////////////////////////////////////
// CServerCfg property page

IMPLEMENT_DYNCREATE(CServerCfg, CPropertyPage)

CServerCfg::CServerCfg() : CPropertyPage(CServerCfg::IDD)
{
    //{{AFX_DATA_INIT(CServerCfg)
    m_polldir = _T("");
    m_enable_polling = FALSE;
	m_image_only_new = FALSE;
	m_delay_edit = 0;
	m_transform = _T("");	
	m_deletefiles = FALSE;
	m_status = -1;
	m_bSelect_By_Size = FALSE;
	m_hotfolder_dpi = 300;
	m_hotfolder_variance = 0.25f;
	//}}AFX_DATA_INIT
}

CServerCfg::~CServerCfg()
   {
   }

void CServerCfg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerCfg)
	DDX_Control(pDX, IDC_DELAY_SPIN, m_delay_spin);
	DDX_Text(pDX, IDC_POLLING_DIR, m_polldir);
	DDX_Check(pDX, IDC_ENABLE_POLLING, m_enable_polling);
	DDX_Check(pDX, IDC_IMAGEONLY_NEW, m_image_only_new);
	DDX_Text(pDX, IDC_DELAY_EDIT, m_delay_edit);
	DDV_MinMaxInt(pDX, m_delay_edit, 0, 60);
	DDX_CBString(pDX, IDC_TRANSFORM, m_transform);	
	DDX_Check(pDX, IDC_DELETEFILES, m_deletefiles);
	DDX_CBIndex(pDX, IDC_STATUS, m_status);
	DDX_Check(pDX, IDC_HF_SELECTBYSIZE, m_bSelect_By_Size);
	DDX_Text(pDX, IDC_HF_SELECTDPI, m_hotfolder_dpi);
	DDX_Text(pDX, IDC_HF_VARIANCE, m_hotfolder_variance);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServerCfg, CPropertyPage)
	//{{AFX_MSG_MAP(CServerCfg)
	ON_BN_CLICKED(IDC_POLL_DIR, OnPollDir)
	ON_BN_CLICKED(IDC_ENABLE_POLLING, OnEnablePolling)
	ON_BN_CLICKED(IDOURCANCEL, OnOurCancel)
	ON_BN_CLICKED(IDOUROK, OnOurOK)	
	ON_CBN_SELCHANGE(IDC_STATUS, OnSelchangeStatus)	
	ON_WM_CONTEXTMENU()	
	ON_WM_HELPINFO()	
	ON_BN_CLICKED(IDC_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerCfg message handlers

BOOL CServerCfg::OnInitDialog() 
   {
   int i, NumXforms;
   CTransform* pTransform;   

#ifdef USEKANJI	   
   // temp hacks!
   CString csTemp;
   ((CComboBox*)GetDlgItem(IDC_STATUS))->ResetContent();
   csTemp.LoadString(IDS_CBOX_SERVER_DISABLED);   
   ((CComboBox*)GetDlgItem(IDC_STATUS))->AddString(csTemp);
   csTemp.LoadString(IDS_CBOX_SERVER_ENABLED);   
   ((CComboBox*)GetDlgItem(IDC_STATUS))->AddString(csTemp);
   csTemp.LoadString(IDS_CBOX_SERVER_ACTIVE);   
   ((CComboBox*)GetDlgItem(IDC_STATUS))->AddString(csTemp);
#endif
      
   CPropertyPage::OnInitDialog();   

   // get the transform combo box
   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_TRANSFORM);	   

   // get all transforms
   NumXforms = ctl.LoadAll(TRUE);	

   // load them	   
   for (i = 0; i < NumXforms; i++)
       {    	   
	   // get transform
	   pTransform = (CTransform*)ctl.GetAt(i); 
	   // insert name into list
       pCB->InsertString(-1, pTransform->Name);
       }    

   // send to dialog	   
   UpdateData(FALSE);

   ENABLEDLGITEM(IDC_ENABLE_POLLING, m_status != STATUS_SERVER_DISABLED);   
   ENABLEDLGITEM(IDC_TRANSFORM, m_status != STATUS_SERVER_DISABLED);
   ENABLEDLGITEM(IDC_TRANSFORM_PRMPT, m_status != STATUS_SERVER_DISABLED);   
   ENABLEDLGITEM(IDC_DELETEFILES, m_status != STATUS_SERVER_DISABLED);      
   ENABLEDLGITEM(IDC_POLLING_GROUP, m_enable_polling && m_status != STATUS_SERVER_DISABLED);
   ENABLEDLGITEM(IDC_POLLING_DIR_PRMPT, m_enable_polling && m_status != STATUS_SERVER_DISABLED);
   ENABLEDLGITEM(IDC_POLLING_DIR, m_enable_polling && m_status != STATUS_SERVER_DISABLED);
   ENABLEDLGITEM(IDC_IMAGEONLY_NEW, m_enable_polling && m_status != STATUS_SERVER_DISABLED);
   ENABLEDLGITEM(IDC_DELAY_PRMPT, m_enable_polling && m_status != STATUS_SERVER_DISABLED);
   ENABLEDLGITEM(IDC_DELAY_EDIT, m_enable_polling && m_status != STATUS_SERVER_DISABLED);
   ENABLEDLGITEM(IDC_DELAY_SPIN, m_enable_polling && m_status != STATUS_SERVER_DISABLED);      
   ENABLEDLGITEM(IDC_POLL_DIR, m_enable_polling && m_status != STATUS_SERVER_DISABLED);

   m_delay_spin.SetRange(0, 60);

   return TRUE;  	              
   }

void CServerCfg::OnPollDir() 
   {
   CString cs;
   UpdateData(TRUE);

   cs.LoadString(IDS_SELECT_POLL_DIR);
   CRexDirDialog dlg((const char*)cs);      
   dlg.m_ofn.lpstrInitialDir = m_polldir;   
   dlg.m_ofn.Flags &= ~OFN_EXPLORER;   
   if (dlg.DoModal() == IDOK)            
      {
      m_polldir = dlg.GetPath(TRUE);
      UpdateData(FALSE);
      }   
   }

void CServerCfg::OnEnablePolling() 
   {
   UpdateData(TRUE);

   ENABLEDLGITEM(IDC_POLLING_GROUP, m_enable_polling && m_status != STATUS_SERVER_DISABLED);
   ENABLEDLGITEM(IDC_POLLING_DIR_PRMPT, m_enable_polling && m_status != STATUS_SERVER_DISABLED);
   ENABLEDLGITEM(IDC_POLLING_DIR, m_enable_polling && m_status != STATUS_SERVER_DISABLED);
   ENABLEDLGITEM(IDC_IMAGEONLY_NEW, m_enable_polling && m_status != STATUS_SERVER_DISABLED);
   ENABLEDLGITEM(IDC_DELAY_PRMPT, m_enable_polling && m_status != STATUS_SERVER_DISABLED);
   ENABLEDLGITEM(IDC_DELAY_EDIT, m_enable_polling && m_status != STATUS_SERVER_DISABLED);
   ENABLEDLGITEM(IDC_DELAY_SPIN, m_enable_polling && m_status != STATUS_SERVER_DISABLED);
   ENABLEDLGITEM(IDC_POLL_DIR, m_enable_polling && m_status != STATUS_SERVER_DISABLED);
   }

void CServerCfg::OnOurCancel()
   {
   GetParent()->SendMessage(PSM_PRESSBUTTON, PSBTN_CANCEL);
   ((CPropertySheet*)GetParent())->EndDialog(IDCANCEL);
   }

void CServerCfg::OnOurOK()
   {                    
   CProfile Profile;

   // make this polldir the default for future servers
   Profile.WriteStr(IDS_SERVER, IDS_POLLDIR, m_polldir);

   // tell parent that we've selected OK
   // it will handle saving data    
   GetParent()->SendMessage(PSM_PRESSBUTTON, PSBTN_OK);
   ((CPropertySheet*)GetParent())->EndDialog(IDOK);   
   }

void CServerCfg::OnOK()
   {
   UpdateData(TRUE);
   }

void CServerCfg::SetServer(CServer& Server)
   {
   // write default data to INI file
   m_transform = Server.m_transform;
   m_polldir = Server.m_polldir;
   m_enable_polling = Server.m_enable_polling;
   m_image_only_new = Server.m_image_only_new;   
   m_delay_edit = Server.m_delay_edit;
   m_deletefiles = (BOOL)Server.m_deletefiles;
   m_status = (BOOL)Server.m_status;
   m_bSelect_By_Size = Server.m_bSelect_By_Size;
   m_hotfolder_dpi = Server.m_hotfolder_dpi;
   m_hotfolder_variance = Server.m_hotfolder_variance;
   }

 void CServerCfg::GetServer(CServer& Server)
   {
   // write default data to passed Server
   Server.m_transform = m_transform;
   // use the xform name to load a real xform
   Server.Transform.LoadTransform(Server.m_transform);
   Server.m_polldir = m_polldir;
   Server.m_enable_polling = m_enable_polling;
   Server.m_image_only_new = m_image_only_new;   
   Server.m_delay_edit = m_delay_edit;
   Server.m_deletefiles = (WORD)m_deletefiles;
   Server.m_status = (WORD)m_status;
   Server.m_bSelect_By_Size = m_bSelect_By_Size;
   Server.m_hotfolder_dpi = m_hotfolder_dpi;
   Server.m_hotfolder_variance = m_hotfolder_variance;
   }

void CServerCfg::OnDefaults() 
   {
   CProfile Profile;

   if (AfxMessageBox(IDS_RESET_SERVER_DEFAULTS, MB_YESNO) == IDYES)
      {
      // defaults
      m_transform.LoadString(IDS_DEFSERVER_XFORM_ENTRY);
      // get the polldir from the installation default in Server section
      m_polldir = Profile.GetStr(IDS_SERVER, IDS_POLLDIR);
      m_enable_polling = FALSE;
      m_image_only_new = TRUE;
      m_delay_edit = 0;
      m_deletefiles = TRUE;
      m_status = STATUS_SERVER_DISABLED;
	  m_bSelect_By_Size = FALSE;
	  m_hotfolder_dpi = 300;
	  m_hotfolder_variance = 0.25f;
	  UpdateData(FALSE);
	  }
   }

void CServerCfg::OnSelchangeStatus() 
   {
   OnEnablePolling();
   ENABLEDLGITEM(IDC_ENABLE_POLLING, m_status != STATUS_SERVER_DISABLED);   
   ENABLEDLGITEM(IDC_TRANSFORM, m_status != STATUS_SERVER_DISABLED);
   ENABLEDLGITEM(IDC_TRANSFORM_PRMPT, m_status != STATUS_SERVER_DISABLED);   
   ENABLEDLGITEM(IDC_DELETEFILES, m_status != STATUS_SERVER_DISABLED);         
   }

void CServerCfg::OnContextMenu(CWnd* pWnd, CPoint point) 
   {   
   // call context help
   ::WinHelp(pWnd->m_hWnd, AfxGetApp()->m_pszHelpFilePath, 
      HELP_CONTEXTMENU,  (DWORD)(LPVOID)dwHelpMap);
   }  

BOOL CServerCfg::OnHelpInfo(HELPINFO* pHelpInfo) 
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


