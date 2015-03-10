/////////////////////////////////////////////////////////////////////////////
// GENERAL.CPP
//                                                                        
// general config class for RasterPlus95
// Copyright (C) 1996-98 Graphx, Inc.
//
// 9-28-98  Version 2.0.5 - Changed OnOurOk PostMessage to SendMessage for VC6.0 compat

#include "stdafx.h"
#include "zeus.h"
#include "general.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeneralPrefs dialog

CGeneralPrefs::CGeneralPrefs() : CPropertyPage(CGeneralPrefs::IDD)
{
	//{{AFX_DATA_INIT(CGeneralPrefs)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT	
}


void CGeneralPrefs::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralPrefs)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGeneralPrefs, CPropertyPage)
	//{{AFX_MSG_MAP(CGeneralPrefs)    
	ON_BN_CLICKED(IDOURCANCEL, OnOurCancel)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGeneralPrefs message handlers

BOOL CGeneralPrefs::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGeneralPrefs::OnOurCancel() 
   {
   GetParent()->SendMessage(PSM_PRESSBUTTON, PSBTN_CANCEL);
   ((CPropertySheet*)GetParent())->EndDialog(IDCANCEL);   
   }

