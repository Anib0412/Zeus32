/////////////////////////////////////////////////////////////////////////////
// LoadTransform.cpp
//                                                                        
// Load a saved Transform into Custom Transform editor
// Copyright (C) 1996 Graphx, Inc.
//
// mods:
//
// 6-28-96  Build 40 - sort load list

#include "stdafx.h"
#include "zeus.h"
#include "loadtransform.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoadTransform dialog


CLoadTransform::CLoadTransform(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadTransform::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoadTransform)
	m_trans = _T("");
	//}}AFX_DATA_INIT
}


void CLoadTransform::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadTransform)
	DDX_LBString(pDX, IDC_TLIST, m_trans);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoadTransform, CDialog)
	//{{AFX_MSG_MAP(CLoadTransform)
	ON_LBN_SELCHANGE(IDC_TLIST, OnSelchangeTlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLoadTransform message handlers

BOOL CLoadTransform::OnInitDialog() 
   {
   int i, NumXforms;
   CTransform* pTransform;
   CListBox* pLB;

   CDialog::OnInitDialog();

   NumXforms = ctl.LoadAll(FALSE);	
   pLB = (CListBox*)GetDlgItem(IDC_TLIST);   
   	   
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

	pSelectedTransform = (CTransform*)pLB->GetItemDataPtr(0);

	return TRUE;  
	} 	              

void CLoadTransform::OnSelchangeTlist() 
   {
   int idx;

   CListBox* pLB = (CListBox*)GetDlgItem(IDC_TLIST);   

   idx = pLB->GetCurSel();

   if (idx != LB_ERR)
      pSelectedTransform = (CTransform*)pLB->GetItemDataPtr(idx);
   }


