/////////////////////////////////////////////////////////////////////////////
// TransformMgrPage.cpp
//                                                                        
// manage transforms for RasterPlus95
// Copyright (C) 1996-98 Graphx, Inc.
//
// 4-30-98  Version 2.0.3 - wrong help for ID_CLOSE
// 9-28-98  Version 2.0.5 - Changed OnOurOk PostMessage to SendMessage for VC6.0 compat

#include "stdafx.h"
#include "zeus.h"
#include "TransformMgrPage.h"
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
   IDC_XFORMLIST, HIDC_XFORMLIST_XFORMLIST,
   IDC_DELETE, HIDC_XFORMLIST_DELETE,
   //IDOUROK, HIDC_OUROK,
   IDOUROK, HIDC_CLOSE,
   IDOURCANCEL, HIDC_OURCANCEL,
   ID_CLOSE, HIDC_CLOSE,
   0,0
   };

/////////////////////////////////////////////////////////////////////////////
// CTransformMgrPage property page

IMPLEMENT_DYNCREATE(CTransformMgrPage, CPropertyPage)

CTransformMgrPage::CTransformMgrPage() : CPropertyPage(CTransformMgrPage::IDD)
{
	//{{AFX_DATA_INIT(CTransformMgrPage)
	m_xform = _T("");
	//}}AFX_DATA_INIT
}

CTransformMgrPage::~CTransformMgrPage()
{
}

void CTransformMgrPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransformMgrPage)
	DDX_LBString(pDX, IDC_XFORMLIST, m_xform);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransformMgrPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTransformMgrPage)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDOUROK, OnOurok)
	ON_WM_CONTEXTMENU()	
	ON_WM_HELPINFO()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransformMgrPage message handlers

void CTransformMgrPage::ResetList()
   {
   CTransform* pTransform;
   int i, NumXforms;   
   CString DefServerTransform;

   // load name of default server transform
   DefServerTransform.LoadString(IDS_DEFSERVER_XFORM_ENTRY);

   // get the transform combo box
   CListBox* pCL = (CListBox*)GetDlgItem(IDC_XFORMLIST);	   

   // reset the list box
   pCL->ResetContent();	
	
   // get all transforms
   NumXforms = ctl.LoadAll(FALSE);	

   if (NumXforms > 0)
      {
	  for (i = 0; i < NumXforms; i++)
         {    	   
	     // get transform
	     pTransform = (CTransform*)ctl.GetAt(i); 
	     // insert name into list
         //pCL->InsertString(-1, (const char*)pTransform->Name);
		 // don't allow delete of server transform
		 if (pTransform->Name != DefServerTransform)
		    pCL->AddString(pTransform->Name);
         }    	    
	  // select the first one
      pCL->SetCurSel(0);
	  GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);	  
	  }
   else
      GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);	  
   }
   

BOOL CTransformMgrPage::OnInitDialog() 
   {
   CPropertyPage::OnInitDialog();
	
   ResetList();
	
   return TRUE;                   
   }

void CTransformMgrPage::OnDelete() 
   {   
   CString csTemp, csPrompt, csSection;   
   CProfile Profile;

   // get the selection
   UpdateData(TRUE);

   // make the prompt
   csTemp.LoadString(IDS_DELETEXFORM);
   csPrompt.Format(csTemp, (const char*)m_xform);

   if (AfxMessageBox(csPrompt, MB_YESNO) == IDYES)
      {
	  // delete the entry
      csSection.LoadString(IDS_CUSTOMXFORMS);
      Profile.WriteStr(csSection, m_xform, NULL);	

      //reload the xforms
      ctl.Clear();
      // get all transforms
      ctl.LoadAll(TRUE);	
   
      ResetList();
	  }			  
   }

void CTransformMgrPage::OnOurok() 
   {
   GetParent()->SendMessage(PSM_PRESSBUTTON, PSBTN_OK);
   ((CPropertySheet*)GetParent())->EndDialog(IDOK);   
   }

BOOL CTransformMgrPage::OnHelpInfo(HELPINFO* pHelpInfo) 
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
 
void CTransformMgrPage::OnContextMenu(CWnd* pWnd, CPoint point) 
   {   
   // call context help
   ::WinHelp(pWnd->m_hWnd, AfxGetApp()->m_pszHelpFilePath, 
      HELP_CONTEXTMENU,  (DWORD)(LPVOID)dwHelpMap);
   }
