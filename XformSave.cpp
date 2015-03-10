/////////////////////////////////////////////////////////////////////////////
// XFormSave.cpp
//                                                                        
// Save a Custom Transform
// Copyright (C) 1996 Graphx, Inc.
//
// mods:
//
// 7-1-96  Build 40 - Only Allow OK if there is an entry

#include "stdafx.h"
#include "zeus.h"
#include "XformSave.h"
#include "Xfrmdefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXformSave dialog


CXformSave::CXformSave(CWnd* pParent /*=NULL*/)
	: CDialog(CXformSave::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXformSave)
	m_newxform = _T("");
	m_xformsel = _T("");
	//}}AFX_DATA_INIT
}


void CXformSave::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXformSave)
	DDX_Text(pDX, IDC_NEW_XFORM, m_newxform);
	DDX_LBString(pDX, IDC_XFORM_LIST, m_xformsel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CXformSave, CDialog)
	//{{AFX_MSG_MAP(CXformSave)
	ON_LBN_SELCHANGE(IDC_XFORM_LIST, OnSelchangeXformList)
	ON_EN_CHANGE(IDC_NEW_XFORM, OnChangeNewXform)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXformSave message handlers

BOOL CXformSave::OnInitDialog() 
   {
   int i, NumXforms;
   CTransform* pTransform;
   CListBox* pLB;		

   CDialog::OnInitDialog();

   NumXforms = ctl.LoadAll(FALSE);	
   pLB = (CListBox*)GetDlgItem(IDC_XFORM_LIST);   
   	   
   for (i = 0; i < NumXforms; i++)
       {    	   
	   // get transform
	   pTransform = (CTransform*)ctl.GetAt(i); 
	   // insert name into list
       //pLB->InsertString(-1, (const char*)pTransform->Name);
	   pLB->AddString(pTransform->Name);
       // insert pTransform into list  
       //pLB->SetItemDataPtr(pLB->GetCount() - 1, (void*)pTransform);
  	   pLB->SetItemDataPtr(pLB->FindString(-1, pTransform->Name), 
	      (void*)pTransform);  
       }    	    	    

   // select first item
   pLB->SetCurSel(0);  
   
   GetDlgItem(IDOK)->EnableWindow(FALSE); 

   return TRUE;  
   }

void CXformSave::OnSelchangeXformList() 
   {
   // update the edit box
   UpdateData(TRUE);
   m_newxform = m_xformsel;
   UpdateData(FALSE);

   if (m_newxform.GetLength() > 0)
      GetDlgItem(IDOK)->EnableWindow(TRUE);
   else
      GetDlgItem(IDOK)->EnableWindow(FALSE);
   }

void CXformSave::OnOK() 
   {
   int i;
   BOOL FoundMatch;      
   CString cs, csPrompt;
   CTransformList Allctl;	
   CTransform* pTransform;

   // check for existing
   UpdateData(TRUE);
   
   CListBox* pLB = (CListBox*)GetDlgItem(IDC_XFORM_LIST);   

   FoundMatch = FALSE;

   for (i = 0; i < pLB->GetCount(); i++)
      {
	  // check for match
	  pLB->GetText(i, cs);
	  if (m_newxform == cs)
	     {
		 FoundMatch = TRUE;		 
		 break;
	     }
	  }	 
   // load ALL transforms so we can compare against 
   // built-in transforms
   Allctl.LoadAll(TRUE);	   
   for (i = 0; i < CUSTOMTRANSFORM; i++)
      {
	  pTransform = (CTransform*)Allctl.GetAt(i); 
	  if (pTransform->Name == m_newxform)
	     {
		 // don't overwrite default transforms
		 AfxMessageBox(IDS_NO_OVERWRITE_DEFTRANSFORM);
		 return;
		 }
	   }
   if (FoundMatch)
      {	  	  
	  cs.LoadString(IDS_OVERWRITE_TRANSFORM);
	  csPrompt.Format(cs, (const char*)m_newxform);
	  if (AfxMessageBox(csPrompt, MB_YESNO) == IDYES)
	     CDialog::OnOK();		 
      }	    
   else
      CDialog::OnOK();
   }

void CXformSave::OnChangeNewXform() 
   {
   UpdateData(TRUE);

   if (m_newxform.GetLength() > 0)
      GetDlgItem(IDOK)->EnableWindow(TRUE);
   else
      GetDlgItem(IDOK)->EnableWindow(FALSE);
   }