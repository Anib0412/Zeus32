/////////////////////////////////////////////////////////////////////////////
// GlobalProperties.cpp
//                                                                        
// Global program properties for RasterPlus
// Copyright (C) 1996-02Graphx, Inc.
//
// mods:
//
// 10-17-01 Version 5.0.0 - Add tab for print index options
// 01-08-02 Version 5.0.1 - not saving last tab - fixed
//

#include "stdafx.h"
#include "zeus.h"
#include "GlobalProperties.h"
#include "profile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGlobalProperties

IMPLEMENT_DYNAMIC(CGlobalProperties, CPropertySheet)

CGlobalProperties::CGlobalProperties(CWnd* pWndParent, CString csTitle)
	 : CPropertySheet(IDS_GLOBALPROPS_TITLE, pWndParent)
   {
   CProfile Profile;

   //AddPage(&m_DevListPage);	
   AddPage(&m_GeneralPrefsPage);

   AddPage(&m_FontManagerPage);
   AddPage(&m_PrintIndexPage);

   // get the last tab we were working on
   int idx = Profile.GetInt(IDS_GENERALSECTION, IDS_LAST_OPTION_TAB, 0);   
   SetActivePage(idx);
   }

CGlobalProperties::~CGlobalProperties()
   {
   }							

BEGIN_MESSAGE_MAP(CGlobalProperties, CPropertySheet)
	//{{AFX_MSG_MAP(CGlobalProperties)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGlobalProperties message handlers

void CGlobalProperties::OnDestroy() 
   {
   CProfile Profile;

   // store the last tab we were working on
   Profile.WriteInt(IDS_GENERALSECTION, IDS_LAST_OPTION_TAB, GetActiveIndex());   

   CPropertySheet::OnDestroy();
   }
