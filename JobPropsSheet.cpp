// JobPropsSheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "JobPropsSheet.h"
#include "profile.h"
#include "zeus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJobPropsSheet
IMPLEMENT_DYNAMIC(CJobPropsSheet, CPropertySheet)

CJobPropsSheet::CJobPropsSheet(BOOL dp, CWnd* pWndParent, CString csTitle)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
{
	// Add all of the property pages here.  Note that
	// the order that they appear in here will be
	// the order they appear in on screen.  By default,
	// the first page of the set is the active one.
	// One way to make a different property page the 
	// active one is to call SetActivePage().

	AddPage(&m_general);
	AddPage(&m_device);
	AddPage(&m_size);
	AddPage(&m_cmmdialog);
	AddPage(&m_coloradjustdialog);	

	csDefaultTitle = csTitle;

	// default
	DoingDefaultProps = dp;

	// add memory page if doing default props
	if (DoingDefaultProps) AddPage(&m_memory);

   // set the active page
   CProfile Profile;
   CString csTabSection;
   CString csTabEntry;
   
   // use the general section
   csTabSection.LoadString(IDS_GENERALSECTION);
   
   // store the last tab we were working on
   if (DoingDefaultProps) 
      csTabEntry = "DefaultPropsTab";
   else
      csTabEntry = "JobPropsTab";				
   SetActivePage(Profile.GetInt(csTabSection, csTabEntry, 0));      
   // default
   pbDeleted = &bDeleted;
}

CJobPropsSheet::~CJobPropsSheet()
{
}


BEGIN_MESSAGE_MAP(CJobPropsSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CJobPropsSheet)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CJobPropsSheet message handlers

void CJobPropsSheet::PostNcDestroy()
{
	CPropertySheet::PostNcDestroy();
	// set this flag so we can check for deletion
	*pbDeleted = TRUE;
	// if we are not doing default props, this must
	// be a modeless sheet which we need to delete here	
	if (!DoingDefaultProps) delete this;
}

BOOL CJobPropsSheet::OnInitDialog() 
   {
   CPropertySheet::OnInitDialog();   

   // use this as the title for default props
   if (DoingDefaultProps) 
      SetWindowText(csDefaultTitle);
	
   return TRUE; 	             
   }

void CJobPropsSheet::OnDestroy() 
   {
   CProfile Profile;
   CString csTabSection;
   CString csTabEntry;

   // use the general section
   csTabSection.LoadString(IDS_GENERALSECTION);

   // store the last tab we were working on
   if (DoingDefaultProps) 
	  csTabEntry = "DefaultPropsTab";
   else
      csTabEntry = "JobPropsTab";

   Profile.WriteInt(csTabSection, csTabEntry, GetActiveIndex());

   if (DoingDefaultProps) 
		((CZeusApp*)AfxGetApp())->BackupRegistry();

   CPropertySheet::OnDestroy();	
   }

void CJobPropsSheet::SetOurActivePage()
   {
   // write the last page
   StoreOurActivePage();
   }

void CJobPropsSheet::StoreOurActivePage()
   {
   CProfile Profile;
   CString csTabSection;
   CString csTabEntry;

   // use the general section
   csTabSection.LoadString(IDS_GENERALSECTION);

   // store the last tab we were working on
   if (DoingDefaultProps) 
	  csTabEntry = "DefaultPropsTab";
   else
      csTabEntry = "JobPropsTab";

   Profile.WriteInt(csTabSection, csTabEntry, GetActiveIndex());
   }

void CJobPropsSheet::FrameShow(BOOL bShow)
   {
   // pass it here so we can hide/unhide the
   // previee window
   m_size.FrameShow(bShow);
   }

