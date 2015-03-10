/////////////////////////////////////////////////////////////////////////////
// OurPropertySheet.cpp
//                                                                        
// property sheet derivation for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//
// Note: Microsoft says to leave Apply Now at bottom of sheet, etc...
// but in their Office products they ignore this stupid advice as
// we do here.  This derivation allows us to remove the OK, Cancel
// Help and Apply Now buttons
//
// mods:
//
// 8-15-96  Build 41 - Changed code to only hide but not disable OK et. al. buttons
//					      so that the mechanics of ESC et. al. are preserved

#include "stdafx.h"
#include "zeus.h"
#include "OurPropertySheet.h"
#include "profile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COurPropertySheet

IMPLEMENT_DYNAMIC(COurPropertySheet, CPropertySheet)

COurPropertySheet::COurPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)	
   {
   // get last tab (must always call this to init the csTabEnry, et. al.   
   int LastTab = ReadLastTab(nIDCaption);
   // if not overridden, use it
   if (iSelectPage == -1) iSelectPage = LastTab;
   // call base class
   Construct(nIDCaption, pParentWnd, iSelectPage);
   // store it
   LastPage = iSelectPage;
   }

COurPropertySheet::COurPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)	
   {
      // get last tab (must always call this to init the csTabEnry, et. al.   
   int LastTab = ReadLastTab(pszCaption);
   // if not overridden, use it
   if (iSelectPage == -1) iSelectPage = LastTab;
   // call base class
   Construct(pszCaption, pParentWnd, iSelectPage);
   // store it
   LastPage = iSelectPage;   
   }

COurPropertySheet::~COurPropertySheet()
   {
   }

BEGIN_MESSAGE_MAP(COurPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(COurPropertySheet)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COurPropertySheet message handlers

static int rgiButtons[] = { IDOK, IDCANCEL, ID_APPLY_NOW, IDHELP };

BOOL COurPropertySheet::OnInitDialog() 
   {
   BOOL bResult = CPropertySheet::OnInitDialog();	

   // layout property sheet so button area is not accounted for		
   CRect rectWnd;
   GetWindowRect(rectWnd);
   CRect rectButton;
   HWND hWnd = ::GetDlgItem(m_hWnd, IDOK);
   ASSERT(hWnd != NULL);
   ::GetWindowRect(hWnd, rectButton);
   SetWindowPos(NULL, 0, 0,
      rectWnd.Width(), rectButton.top - rectWnd.top,
   SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

   // hide but don't disable standard buttons   
   for (int i = 0; i < 4; i++)
      {
	  HWND hWnd = ::GetDlgItem(m_hWnd, rgiButtons[i]);
	  if (hWnd != NULL)
	  	 {
		 ::ShowWindow(hWnd, SW_HIDE);
		 //::EnableWindow(hWnd, FALSE);
		 }
      }

   CenterWindow();

   return bResult;
   }

 void COurPropertySheet::WriteLastTab()
    {
	CProfile Profile;
		 
	int i = GetActiveIndex();
	// store the last tab we were working on
    Profile.WriteInt(csTabSection, csTabEntry, GetActiveIndex());
	}

int COurPropertySheet::ReadLastTab(UINT nIDCaption)
   {
   CString cs;

   cs.LoadString(nIDCaption);
   return ReadLastTab(cs);
   }

int COurPropertySheet::ReadLastTab(LPCTSTR pszCaption)
   {
   CString cs;
   CProfile Profile;
   int DefaultTab;
		
   // put in general section
   csTabSection.LoadString(IDS_GENERALSECTION);
   // create entry based on caption name and save it
   csTabEntry = pszCaption;
   cs.LoadString(IDS_LASTTAB_PROFILE);
   csTabEntry += cs;

   // get the last tab we were working on
   DefaultTab = Profile.GetInt(csTabSection, csTabEntry, 0);

   // return it
   return DefaultTab;
   }

void COurPropertySheet::OnDestroy() 
   {
   WriteLastTab();
	
   CPropertySheet::OnDestroy();
   }
