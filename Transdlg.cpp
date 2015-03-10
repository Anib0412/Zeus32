/////////////////////////////////////////////////////////////////////////////
// TRANSDLG.CPP
//                                                                        
// transform dialog for RasterPlus95
// Copyright (C) 1996-98 Graphx, Inc.
//
// mods:
//
// 7-06-96  Build 40 - A multi-selection of queue items with the same transform
//					      would result in no transform selected - fixed
// 8-05-97  Build 50 - remove ReportDevChange
// 9-28-98  Version 2.0.5 - Changed OnOurOk PostMessage to SendMessage for VC6.0 compat

#include "stdafx.h"
#include "zeus.h"
#include "transdlg.h"
#include "xfrmdefs.h"
#include "visualxf.h"
#include "prefsmsg.h"
#include <string.h>
#include <stdlib.h>
#include "mainfrm.h"
#include "profile.h"
#include "control.hm"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT NEAR wm_DevModeChange = RegisterWindowMessage( ID_DEVMODE_CHANGE );

// the control help-map
static DWORD dwHelpMap[] = 
   {
   IDC_TRANSFORM, HIDC_TRANSFORM_TRANSFORM,
   IDC_TRANSFORM_GRP, HIDC_TRANSFORM_TRANSFORM,
   IDC_DESCRIPTION_GRP, HIDC_TRANSFORM_DESCRIPTION,
   IDC_EDIT, HIDC_TRANSFORM_EDIT,
   IDC_RESET, HIDC_TRANSFORM_RESET,
   IDOUROK, HIDC_OUROK,
   IDOURCANCEL, HIDC_OURCANCEL,
   ID_CLOSE, HIDC_CLOSE,   
   0,0
   };

/////////////////////////////////////////////////////////////////////////////
// CDialogTransSetup dialog

CDialogTransSetup::CDialogTransSetup() : CPropertyPage(CDialogTransSetup::IDD)
{
	//{{AFX_DATA_INIT(CDialogTransSetup)
	m_description = "";
	//}}AFX_DATA_INIT
	// get the default transform		
	DefaultTransform.LoadDefaultTransform();        	
    DelayTransformFlag = FALSE;
}   

CDialogTransSetup::~CDialogTransSetup()
   { 
   }

void CDialogTransSetup::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogTransSetup)
	DDX_Text(pDX, IDC_DESCRIPTION, m_description);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDialogTransSetup, CDialog)
	//{{AFX_MSG_MAP(CDialogTransSetup)
	ON_CBN_SELCHANGE(IDC_TRANSFORM, OnSelchangeTransform)	
	ON_BN_CLICKED(IDC_EDIT, OnEdit)	
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDOUROK, OnOurOK)
	ON_BN_CLICKED(IDOURCANCEL, OnOurCancel)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_RESET, OnReset)	
	ON_EN_CHANGE(IDC_DESCRIPTION, OnChangeDescription)
	ON_WM_CONTEXTMENU()	
	ON_WM_HELPINFO()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDialogTransSetup::OnInitDialog()
   {       
   int i, NumXforms;
   CComboBox* pCB;                        
   CTransform* pTransform;
   int DefTransformIdx = -1;  
   
   csCustom.LoadString(IDS_CUSTOMXFORMNAME);
    
   // get the transform combo box
   pCB = (CComboBox*)GetDlgItem(IDC_TRANSFORM);	   
	
   // get all transforms
   NumXforms = ctl.LoadAll(TRUE);	
   	   
   for (i = 0; i < NumXforms; i++)
       {    	   
	   // get transform
	   pTransform = (CTransform*)ctl.GetAt(i); 
	   // if this is the default, store the index
       if (DefaultTransform.Name == pTransform->Name)
          DefTransformIdx = pCB->GetCount();
       // insert name into list
       //pCB->InsertString(-1, (const char*)pTransform->Name);
	   pCB->AddString(pTransform->Name);
       // insert pTransform into list  
       //pCB->SetItemDataPtr(pCB->GetCount() - 1, (void*)pTransform);
 	   pCB->SetItemDataPtr(pCB->FindString(-1, pTransform->Name), 
	      (void*)pTransform);	  
       }    	    
    
    // select the default transform
    //if (DefTransformIdx != -1) pCB->SetCurSel(DefTransformIdx);	
	if ((DefTransformIdx = pCB->FindString(-1, DefaultTransform.Name)) != CB_ERR) 
	   pCB->SetCurSel(DefTransformIdx);

	// disable edit box if no custom    
	if (DefaultTransform.Name != csCustom)
       GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
    else   
       GetDlgItem(IDC_EDIT)->EnableWindow(TRUE);	               

	// set description    
	m_description = DefaultTransform.Description;
    
	// make sure that data is in list
	UpdateData(FALSE);	
	
	CPropertyPage::OnInitDialog();	

	// show buttons depending on state
   if (ShowOKFlag)
      {
      (CWnd*)GetDlgItem(IDOUROK)->ShowWindow(SW_SHOW);
      (CWnd*)GetDlgItem(IDOURCANCEL)->ShowWindow(SW_SHOW);
      (CWnd*)GetDlgItem(IDC_CLOSE)->ShowWindow(SW_HIDE);
      (CWnd*)GetDlgItem(IDC_RESET)->ShowWindow(SW_HIDE);
      }
   else
      {
      (CWnd*)GetDlgItem(IDOUROK)->ShowWindow(SW_HIDE);    
      (CWnd*)GetDlgItem(IDOURCANCEL)->ShowWindow(SW_HIDE);
      (CWnd*)GetDlgItem(IDC_CLOSE)->ShowWindow(SW_SHOW);
      (CWnd*)GetDlgItem(IDC_RESET)->ShowWindow(SW_SHOW);
      }   

   // if we have delayed setting the transform, do it now
   if (DelayTransformFlag)
      {
	  SetDefaultTransform(DelayTransform);
	  DelayTransformFlag = FALSE;
	  }       
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDialogTransSetup::OnOurCancel()
   {
   GetParent()->SendMessage(PSM_PRESSBUTTON, PSBTN_CANCEL);
   ((CPropertySheet*)GetParent())->EndDialog(IDCANCEL);
   }

void CDialogTransSetup::OnOurOK()
   {                    
   // tell parent that we've selected OK
   // it will handle saving data    
   GetParent()->SendMessage(PSM_PRESSBUTTON, PSBTN_OK);
   ((CPropertySheet*)GetParent())->EndDialog(IDOK);   
   }

/////////////////////////////////////////////////////////////////////////////
// CDialogTransSetup message handlers
                 
                 
void CDialogTransSetup::OnSelchangeTransform()
   {
   // get the transform combo box
   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_TRANSFORM);

   CTransform* pTransform = (CTransform*)pCB->GetItemDataPtr(pCB->GetCurSel());	   

   // read from ini
   pTransform->ReadFromIni();

   // disable edit box if no custom   
   if (pTransform->Name != csCustom)
      GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
   else   
      GetDlgItem(IDC_EDIT)->EnableWindow(TRUE);	               

   // set description    
   m_description = pTransform->Description;       
   // put it in dialog
   UpdateData(FALSE);
    
   // make it the default and inform the queue
   ApplyToTransform();	      
   }

void CDialogTransSetup::OnEdit()
   {                
   CJobSizeProps cx;   
   CTransform* pTransform;   
   CComboBox* pCB;
   
   // make sure that the data is updated
   UpdateData(TRUE);              
      
   // get the combo box
   pCB = (CComboBox*)GetDlgItem(IDC_TRANSFORM);

   // get the current transform
   pTransform = (CTransform*)pCB->GetItemDataPtr(pCB->GetCurSel());

   // set it	
   //cx.SetDefTransform(*pTransform);	
   
   // set queue file
   cx.pTargetCQueue = pTargetCQueue;   
   // set Target devmode
   cx.pTargetDevice = pTargetDevice;
   
   // call the dialog   
   if (cx.DoModal() == IDOK)
      {
	  int i, NumXforms;
	  int DefTransformIdx = -1;   

	  // reload xforms
	  ctl.Clear();
	  // get all transforms
      NumXforms = ctl.LoadAll(TRUE);	
	  // need to reget this?
	  pCB = (CComboBox*)GetDlgItem(IDC_TRANSFORM);
	  // reset the combo box
	  pCB->ResetContent();
	  // get the selected xform
	  DefaultTransform = cx.GetDefTransform();
	  // reload the xforms in list
      for (i = 0; i < NumXforms; i++)
		  {    	   
	      // get transform
	      pTransform = (CTransform*)ctl.GetAt(i); 
	      // if this is the default, store the index
          if (DefaultTransform.Name == pTransform->Name)
		     {
             DefTransformIdx = pCB->GetCount();
			 // if this is custom, overwrite existing			 			 
			 if (DefaultTransform.Name == csCustom)			    
				*pTransform = DefaultTransform;
			 }

          // insert name into list
          //pCB->InsertString(-1, (const char*)pTransform->Name);
		  pCB->AddString(pTransform->Name);
          // insert pTransform into list  
          //pCB->SetItemDataPtr(pCB->GetCount() - 1, (void*)pTransform);		  
		  pCB->SetItemDataPtr(pCB->FindString(-1, pTransform->Name), 
		     (void*)pTransform);	  
          }    	          
    
	  // need to reget this 
	  pCB = (CComboBox*)GetDlgItem(IDC_TRANSFORM);

	  // select the default transform
      //if (DefTransformIdx != -1) pCB->SetCurSel(DefTransformIdx);
  	  if ((DefTransformIdx = pCB->FindString(-1, DefaultTransform.Name)) != CB_ERR) 
	     pCB->SetCurSel(DefTransformIdx);
	  else
		 return;

	  // disable edit box if no custom      
	  if (DefaultTransform.Name != csCustom)
         GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
      else   
         GetDlgItem(IDC_EDIT)->EnableWindow(TRUE);	               
		  
      m_description = DefaultTransform.Description;      
      // make it the default and inform the queue
      ApplyToTransform();	   
      UpdateData(FALSE);
      }       
   }

void CDialogTransSetup::OnOK()
   {
   CProfile Profile;
   CTransform* pTransform;
    
   // make sure that the data is updated
   UpdateData(TRUE);       
   
   // get the transform combo box
   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_TRANSFORM);
   
   // get the transform
   pTransform = (CTransform*)pCB->GetItemDataPtr(pCB->GetCurSel());

   // write it as the default
   Profile.WriteStr(IDS_RIPSECTION, IDS_TRANSFORM_DEFAULT, 
      pTransform->Name);
   }

void CDialogTransSetup::ApplyToTransform()
   {           
   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_TRANSFORM);
   DefaultTransform = *((CTransform*)pCB->GetItemDataPtr(pCB->GetCurSel()));	      

   // inform the queue	
   pWnd->SendMessage(wm_DevModeChange, PREFS_CHANGE_TRANSFORM | ReportChange, 
      (LONG)&DefaultTransform);	
   }				

void CDialogTransSetup::OnClose()
   {
   // hide it
   GetParent()->ShowWindow(SW_HIDE);
   }

void CDialogTransSetup::ShowOK(BOOL flag)
   {
   ShowOKFlag = flag;
   }		

void CDialogTransSetup::SetMultiSelFields(CObArray& cod)
   {   
   CString cs;
   CTransform* ct;
   CTransform* ctFirst;
   int i;
   BOOL TransformsDiffer;

   // get first transform
   ctFirst = (CTransform*)cod.GetAt(0);

   TransformsDiffer = FALSE;

   // see if they are all the same
   for (i = 1; i < cod.GetSize(); i++)
      {      
	  // get device
	  ct = (CTransform*)cod.GetAt(i);
	  if (*ct != *ctFirst) 
	     {
		 TransformsDiffer = TRUE;
		 break;
		 }
	  }

   // get the transform combo box
   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_TRANSFORM);
   
   if (TransformsDiffer)
      {
	  // no selection!
      pCB->SetCurSel(-1);                       
      // disable name window
      GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);   
      // set description
      m_description = "";
      // make sure that data is in list
      UpdateData(FALSE);	
      // disable edit box
      GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);      
	  }
   else
      {
	  // set to the first
	  CTransform ThisTransform;
	  ThisTransform = *((CTransform*)cod.GetAt(0));
      SetDefaultTransform(ThisTransform);
	  }
   }   

void CDialogTransSetup::DelayedSetDefaultTransform(CTransform& Transform)
   {
   DelayTransform = Transform;
   DelayTransformFlag = TRUE;
   }
   
void CDialogTransSetup::SetDefaultTransform(CTransform& Transform)
   {
   int idx; 
         
   // get the transform combo box
   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_TRANSFORM);
   
   // find the transform
   idx = pCB->FindStringExact(0, Transform.Name);
   
   // check it!
   if (idx == -1) idx = 0;
   
   // copy the passed transform over the current transform
   // the assignent operator should take care of the delete issues
   CTransform* pCT = (CTransform*)pCB->GetItemDataPtr(idx);	
   *pCT = Transform;
   
   // the default transform is now this one   
   DefaultTransform = *pCT;
     
   // select it
   pCB->SetCurSel(idx);   
   
   // highlight the edit window if necessary      
   m_description = pCT->Description;       

   // disable edit box if no custom    
	if (DefaultTransform.Name != csCustom)
       GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
    else   
       GetDlgItem(IDC_EDIT)->EnableWindow(TRUE);	               	
   }
   
CTransform& CDialogTransSetup::GetDefaultTransform()
   {     
   // return the default transform   
   return (DefaultTransform);
   }

void CDialogTransSetup::OnDestroy()
   {   
   CPropertyPage::OnDestroy();
   }
      
void CDialogTransSetup::OnReset()
   {
   // inform the caller      
   pWnd->SendMessage(wm_DevModeChange, PREFS_CHANGE_TRANSFORM | ReportChange, 0L);            
   }   

void CDialogTransSetup::OnChangeDescription()
   {
   // get the data
   UpdateData(TRUE);
   
   // load the description   
   DefaultTransform.Description = m_description;
   }

void CDialogTransSetup::OnContextMenu(CWnd* pWnd, CPoint point) 
   {   
   // call context help
   ::WinHelp(pWnd->m_hWnd, AfxGetApp()->m_pszHelpFilePath, 
      HELP_CONTEXTMENU,  (DWORD)(LPVOID)dwHelpMap);
   }  

BOOL CDialogTransSetup::OnHelpInfo(HELPINFO* pHelpInfo) 
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
