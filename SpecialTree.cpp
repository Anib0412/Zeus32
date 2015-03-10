/////////////////////////////////////////////////////////////////////////////
// SpecialTree.CPP
//                                                                        
// base tree display/op class for RasterPlus
// Copyright (C) 1999-2000 Graphx, Inc.
//
// mods:
//
// 9-16-99  Version 3.0.1 - fixed carriage return problem in rip status window
// 9-17-99  Version 3.0.1 - make sure that JobProps is visible
// 9-22-99  Version 3.0.1 - remove spinner when pasting an inrip file into other pane
// 9-22-99  Version 3.0.1 - don't allow paste of deleted file into hold or print queue
// 10-12-99 Version 3.0.1 - fix for custom queue props sizing crash
// 10-19-99 Version 3.0.1 - Queue Props was checking other Queue Panes for reset potential and
//                             resetting them (?) - fixed
// 10-19-99 Version 3.0.1 - server did not start polling until Queue Props was set
// 10-19-99 Version 3.0.1 - always apply job props to files that go to archive and only
//                          apply queue props to non-archive jobs 
// 10-29-99 Version 3.0.1 - extra check on Job Props positioning
// 11-29-99 Version 3.0.2 - not setting color stuff on Queue props sizing which caused crash - fixed
// 12-02-99 Version 3.0.2 - hot folders set to root were not working - fixed
// 12-02-99 Version 3.0.2 - don't delete server files if other instances exist
// 1-13-00  Version 3.0.3 - add files was adding in reverse order - fixed
// 2-07-00  Version 3.0.3 - couldn't disable hot folder - fixed again
// 2-16-00  Version 3.0.3 - weirdly dated files caused CFileStatus to ASSERT - 
//                          changed to FindFirstFile
// 2-17-00  Version 3.0.3 - pasting job prefs could cause crash - fixed
// 2-24-00  Version 3.0.3 - Changing Color Adjustment values sometimes did not stick - fixed
// 2-28-00	Version 3.0.3 - Do not do a KillTimmer on a No Default Printer error, allows Printing to continue 
//							for the remainder of the files in the print queue
// 3-17-00	Version 3.0.3 - Changed Queue Props logic
// 3-20-00	Version 3.0.3 - added Gang Print code
// 3-23-00  Version 3.0.3 - Added Rendering Intents updates and changes ResetProps function call to pass TRUE
// 3-28-00  Version 3.0.3 - Fix Compress Data from Bitmap not sticking - Polaroid bug 31 and BugDB Bug #48
// 4-04-00  Version 3.0.3 - call OnContinuePrinting from Print() to start rip faster
// 4-07-00  Version 3.0.3 - fixed crashing problem by removing unnecessary crPropsWinRect
// 4-13-00  Version 3.0.3 - make sure Prefs flag is reset when we delete transform, color or 
//                          device prefs because
// 5-04-00  Version 3.0.3 - Device changes do not effect Sizing props - fixed
// 6-08-00  Version 4.0 - Copies ( and other fields ) for last Job Properties change used to populate
//                          a new job when File Printing - fixed
// 7-10-00  Version 4.0 - log files are now unique for each queue, not device
// 7-17-00  Version 4.0 - fixed obscure crashing bug which sometimes occurred when queue props
//						    was reset and then made local again
// 9-07-00  Version 4.0 - Added code to Incr/Decr number of files in a PrintPane using CPrintPaneState functions
// 10-03-00 Version 4.0.1 - Set pTargetDevice before calling ResetWindow to prevent crash
// 10-10-00 Version 4.0.1 - FilePrint and GIS files not setting print pane state correctly - fixed
// 11-01-00 Version 4.0.1 - Sometimes the advanced dialog would go blank in job props - fixed
// 11-06-00 Version 4.0.1 - don't bring foreign devices into queue during paste file or paste atts
// 11-07-00 Version 4.0.1 - set tray title in SendFileToRip because if we instantiate a new
//                          queue, the doc title is not yet set in oninitialupdate
// 12-15-00 Version 4.0.2 - if you are adding many files, the buffer is sometimes not big enough,
//                          let's give it the max size
// 12-22-00 Version 4.0.2 - Destroy log and job props windows in DestroyWindow not destructor
//                          to prevent crash when user cancels install printer
// 04-27-01 Version 4.0.3 - Bug #174 - Is now clearing properly in Job Properties
// 05-23-01 Version 4.0.3 - Fix Focus problem related to previews ( Win95 )
// 06-11-01 Version 4.0.3 - Check Delete image on successful print - Do not display DeletedImage Icon if DeleteImage failed
// 06-19-01 Version 4.0.3 - Do not remove file from hot folder list if delete files ( e.g. File ReadOnly )
// 08-20-01 Version 5.0.0 - Code to supported Unsupported files and Separate PPD files for each Queue
// 09-28-01 Version 5.0.0 - Added High_Defintion Preview support
// 10-30-01 Version 5.0.0 - Add License Manager pick to Menu
// 11-06-01 Version 5.0.0 - reorganize temp path stuff
// 11-30-01 Version 5.0.1 - Remove Sentinel Dongle Code
// 01-29-02 Version 5.0.2 - New fields in Dev, RipStruct and JobInfo  - Future Expansion
// 03-14-02 Version 5.0.4 - Check Registry field to determine if driver needs to be re-installed
// 05-20-02 Version 5.0.6 - Fix removing entries from archive problem
// 09-09-02 Version 5.0.7 - Fix indeterminant state processing
// 10-16-02 Version 5.0.7 - Fix PrintPane not always initialized properly
// 10-28-02 Version 5.1.0 - Force SplashScreen to close if a device needs to request a reinstall, media has changed
// 01-16-03 Version 5.1.1 - Fix inconsistent sticky Queue Properties - Values being reset incorrectly
// 05-08-03 Version 5.1.2 - Fix inconsistent sticky Queue Properties when cancelling between Queue and Job Properties
// 09-11-03 Version 5.1.7 - Could not turn-off hotfolder once it was enabled - Fixed
//
 
#include "stdafx.h"
#include "zeus.h"
#include "SpecialTree.h"
#include "inidefs.h"
#include "zeusdoc.h"
#include "prefsmsg.h"
#include "ripmsg.h"
#include "CustomFileDialog.h"
#include "profile.h"
#include "InstallPrinter.h"
#include "winspool.h"
#include "dbt.h"
#include "LicenseManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// timer stuff
#define RP95_PRINT_TIMER_ID          199
#define RP95_SERVER_TIMER_ID         198
#define RP95_PRINT_TIMER_TIMEOUT     1000   // 1 sec
#define RP95_SERVER_TIMER_TIMEOUT    5000   // 5 sec

#define QUEUEDEVICE    PDOC->QueueDevice
#define QUEUETRANSFORM PDOC->QueueTransform
#define QUEUECOLOR     PDOC->QueueColor
#define CLIPARRAY      ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->ClipArray
#define PANEARRAY      ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pPaneArray

#define PSOUT_EDIT_WINDOW  (pCQueueChild->GetPSOutEdit())
#define PSOUT_HAVE_SELTEXT (pCQueueChild->DoesPSHaveSelText())
#define PSOUT_EMPTY        (pCQueueChild->IsPSOutEmpty())

const UINT NEAR wm_RipSendMsg = RegisterWindowMessage( RIP_SENDMESSAGE );
const UINT NEAR wm_RipSendPSMessage = RegisterWindowMessage(RIP_SENDPSMESSAGE);
const UINT NEAR wm_RipComplete = RegisterWindowMessage( RIP_COMPLETE );
const UINT NEAR wm_DevModeChange = RegisterWindowMessage( ID_DEVMODE_CHANGE );
const UINT NEAR wm_DevModeChangeDefault = RegisterWindowMessage( ID_DEVMODE_CHANGE_DEFAULT );
const UINT NEAR wm_AddToPane = RegisterWindowMessage( RIP_ADDTOPANE );
const UINT NEAR wm_StartPolling = RegisterWindowMessage( RIP_START_POLLING );
const UINT NEAR wm_StopPolling = RegisterWindowMessage( RIP_STOP_POLLING );
const UINT NEAR wm_DevInfoChange = RegisterWindowMessage( ID_DEVINFO_CHANGE );

const UINT NEAR wm_ResizePane = RegisterWindowMessage( "RPResize_Pane" );
const UINT NEAR wm_ResizeRPWindowMsg = RegisterWindowMessage( "ResizeRPWindow" );

/////////////////////////////////////////////////////////////////////////////
// CSpecialTree

IMPLEMENT_DYNCREATE(CSpecialTree, CRPTreeBase)

CSpecialTree::CSpecialTree()
   {
   CString cs;   

   // override the header height by pjt!   
   DrawHeaderPushCol = -1;   
   m_hItemDragged = NULL;

   // defaults
   InternalState = STATE_IDLE_NO_FILES;     	  
   bPrintServerStarted = FALSE;
   dwScrollStart = 0;

   // our clipboard format
   cs.LoadString(IDS_RP95_CBOARD_FMT);
   uiOurListClipFormat = ::RegisterClipboardFormat((const char*)cs);   

   // get pointer to fileinfo holder in CMainForm so we only do
   // single instance of fileinfo
   m_pPropFrame = ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_pPropFrame;
   //initialize the QUEUE properties modal dialog pointer
   m_pModalPropFrame = NULL;
   // get pointer to instance counter
   pInstanceCntr = 
      &(((CMainFrame*)(AfxGetApp()->m_pMainWnd))->CfDlgInstanceCntr);      
   // create the info dialog     
   CreateJobPropsDlg();     
   
   // set in OnInitialUpdate
   pPaneElement = NULL;
   pCPInitStruct = &QUEUECOLOR.CPInitStruct;
   // default sort order
   for (int i = 0; i < NUMQUEUECOLUMNS; i++)
      QueueSortOrderUp[i] = FALSE;   

   ServerPolling = FALSE;

   pCQueueChild= NULL;

   bSentStartToLog = FALSE;

   bResizeHack = FALSE;

   iViewStyle = -1;
   pPrintPaneState = NULL;

   iLastQueueStatus = IDS_QUEUESTATUS_IDLE;
   }

CSpecialTree::~CSpecialTree()
{
	if (pPaneElement)
		PANEARRAY.DeleteElement(pPaneElement);

	if (pPrintPaneState)
		delete pPrintPaneState;
}

BEGIN_MESSAGE_MAP(CSpecialTree, CRPTreeBase)
	//{{AFX_MSG_MAP(CSpecialTree)
    ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_COMMAND(ID_EDIT_ADDFILES, OnEditAddfiles)
	ON_NOTIFY_REFLECT(TVN_GETDISPINFO, OnGetdispinfoTree)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()		
	ON_WM_RBUTTONDOWN()
	ON_REGISTERED_MESSAGE(wm_RipComplete, OnRipComplete)
	ON_REGISTERED_MESSAGE(wm_AddToPane, OnAddToPane)	
	ON_REGISTERED_MESSAGE(wm_StartPolling, OnStartPolling)	
	ON_REGISTERED_MESSAGE(wm_StopPolling, OnStopPolling)	
	ON_REGISTERED_MESSAGE(wm_RipSendMsg, OnRipMessage)
	ON_REGISTERED_MESSAGE(wm_RipSendPSMessage, OnRipPSMessage)
	ON_REGISTERED_MESSAGE(wm_DevModeChange, OnDevModeChange)
	ON_REGISTERED_MESSAGE(wm_DevModeChangeDefault, OnDevModeChangeDefault)
	ON_REGISTERED_MESSAGE(wm_DevInfoChange, OnDevInfoChange)	
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnItemchanged)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_EDIT_COPYATTS, OnEditCopyatts)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYATTS, OnUpdateEditCopyatts)
	ON_COMMAND(ID_EDIT_PASTEATTS, OnEditPasteatts)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTEATTS, OnUpdateEditPasteatts)
	ON_COMMAND(ID_EDIT_RESETSTATUS, OnEditResetstatus)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RESETSTATUS, OnUpdateEditResetstatus)
	ON_COMMAND(ID_PREFERENCES_QUEUE, OnPreferencesQueue)	
	ON_WM_SETFOCUS()	
	ON_COMMAND(ID_TOOLS_LOG, OnToolsLog)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_LOG, OnUpdateToolsLog)
	ON_COMMAND(ID_TOOLS_PRINTLOG, OnToolsPrintlog)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_PRINTLOG, OnUpdateToolsPrintlog)	
	ON_COMMAND(ID_TOOLS_RESETLOG, OnToolsResetlog)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_RESETLOG, OnUpdateToolsResetlog)
	ON_COMMAND(ID_EDIT_SELECTALL, OnSelectall)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)	
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECTALL, OnUpdateEditSelectall)
	ON_COMMAND(ID_EDIT_ADDBATCHHEADER, OnEditInsertBatchHeader)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDBATCHHEADER, OnUpdateEditAddbatchheader)
	ON_COMMAND(ID_TOOLS_ARCHIVELOG, OnToolsArchivelog)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ARCHIVELOG, OnUpdateToolsArchivelog)
	ON_COMMAND(ID_EDIT_RESETPROPS, OnEditResetprops)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RESETPROPS, OnUpdateEditResetprops)
	ON_REGISTERED_MESSAGE(wm_ResizePane, OnResizePane)	
	ON_COMMAND(ID_PREFERENCES_LOCAL, OnPreferencesLocal)
	ON_UPDATE_COMMAND_UI(ID_PREFERENCES_LOCAL, OnUpdatePreferencesLocal)
	ON_UPDATE_COMMAND_UI(ID_PREFERENCES_QUEUE, OnUpdatePreferencesQueue)			
	ON_COMMAND(ID_FILE_REINSTALLPRINTER, OnFileReinstallprinter)
	ON_COMMAND(ID_FILE_APPDRIVER_CONFIG, OnFileAppdriverConfig)	
	ON_UPDATE_COMMAND_UI(ID_EDIT_JOBDESCRIPTION, OnUpdateEditJobdescription)
	ON_WM_DESTROY()
	ON_COMMAND(ID_TOOLS_LICENSE_MANAGER, OnToolsLicenseManager)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_LICENSE_MANAGER, OnUpdateToolsLicenseManager)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpecialTree drawing

void CSpecialTree::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CSpecialTree diagnostics

#ifdef _DEBUG
void CSpecialTree::AssertValid() const
{
	CRPTreeBase::AssertValid();
}

void CSpecialTree::Dump(CDumpContext& dc) const
{
	CRPTreeBase::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSpecialTree message handlers

void CSpecialTree::InsertString(int i, CQueueFile* pCQueueFile)
   {
   int InsertAfter = i - 1;
   TV_INSERTSTRUCT tvis;   

   memset(&tvis, 0, sizeof(TV_INSERTSTRUCT));	
   tvis.item.mask    = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
   tvis.item.pszText = LPSTR_TEXTCALLBACK;	      
   tvis.item.iImage = I_IMAGECALLBACK;
   tvis.item.iSelectedImage = I_IMAGECALLBACK;
   tvis.item.lParam = (LPARAM)pCQueueFile;
   tvis.hParent      = TVI_ROOT;
   if (InsertAfter < 0)
      tvis.hInsertAfter = TVI_FIRST;
   else
	  tvis.hInsertAfter = GetItemAt(InsertAfter);

   GetTreeCtrl().InsertItem(&tvis);
   }

void CSpecialTree::SetHeader()
   {
   CString cs;
   // set styles
   //GetTreeCtrl().ModifyTreeCtrlStyles(0, TVS_SHOWSELALWAYS | WS_CHILD | WS_VISIBLE | TVS_EDITLABELS,
   GetTreeCtrl().ModifyTreeCtrlStyles(0, TVS_SHOWSELALWAYS | WS_CHILD | WS_VISIBLE,
	   0, TVXS_MULTISEL | TVXS_FLYBYTOOLTIPS | TVXS_COLUMNHEADER, FALSE);
   GetListCtrl().ModifyListCtrlStyles(0, 0, 0, LVXS_HILIGHTSUBITEMS, TRUE);	      

   // create the hold list
   GetTreeCtrl().SetColumnWidth(QUEUECOL_DESCRIPTION, 
	  PDOC->iQueueColumnWidth[iViewStyle][QUEUECOL_DESCRIPTION]);
   cs.LoadString(IDS_DESCRIPTION_COLTITLE);
   GetTreeCtrl().SetColumnHeading(QUEUECOL_DESCRIPTION, cs);
   cs.LoadString(IDS_FILENAME_COLTITLE);
   GetTreeCtrl().InsertColumn(QUEUECOL_FILENAME, cs, LVCFMT_LEFT, 
	  PDOC->iQueueColumnWidth[iViewStyle][QUEUECOL_FILENAME]);
   cs.LoadString(IDS_OWNER_COLTITLE);
   GetTreeCtrl().InsertColumn(QUEUECOL_OWNER, cs, LVCFMT_LEFT, 
	  PDOC->iQueueColumnWidth[iViewStyle][QUEUECOL_OWNER]);
   cs.LoadString(IDS_SIZE_COLTITLE);   
   GetTreeCtrl().InsertColumn(QUEUECOL_SIZE, cs, LVCFMT_LEFT, 
	  PDOC->iQueueColumnWidth[iViewStyle][QUEUECOL_SIZE]);
   cs.LoadString(IDS_LOCALPROPS_COLTITLE);   
   GetTreeCtrl().InsertColumn(QUEUECOL_PROPS, cs, LVCFMT_LEFT, 
	  PDOC->iQueueColumnWidth[iViewStyle][QUEUECOL_PROPS]);
   cs.LoadString(IDS_SUBMITTED_COLTITLE);   
   GetTreeCtrl().InsertColumn(QUEUECOL_SUBMITTED, cs, LVCFMT_LEFT, 
	  PDOC->iQueueColumnWidth[iViewStyle][QUEUECOL_SUBMITTED]);
   cs.LoadString(IDS_STATUS_COLTITLE);   
   GetTreeCtrl().InsertColumn(QUEUECOL_STATUS, cs, LVCFMT_LEFT, 
	  PDOC->iQueueColumnWidth[iViewStyle][QUEUECOL_STATUS]);
   // Add a dummy column to the right of the last real column
   // This will allow the last column to be widened to the right
   //
   GetTreeCtrl().InsertColumn(QUEUECOL_STATUS+1, "", LVCFMT_LEFT, 0 );
   }

void CSpecialTree::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   CString cs;
   LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   CQueueFile* pCQueueFile;     
   char Props[5];
   int i = 0;
   
   // get queue file
   pCQueueFile = (CQueueFile*)pDispInfo->item.lParam;

   if (!pCQueueFile)
      {
	  *pResult = 0;
	  return;
	  }	

   if(pDispInfo->item.mask & TVIF_TEXT)
	  {
      switch(pDispInfo->item.iSubItem)
		 {
	     case QUEUECOL_FILENAME:
	          strcpy(pDispInfo->item.pszText, (const char*)pCQueueFile->ShortName);
		      break;
	     case QUEUECOL_OWNER:
	          strcpy(pDispInfo->item.pszText, (const char*)pCQueueFile->csOwner);
		      break;
	     case QUEUECOL_SIZE:
	          strcpy(pDispInfo->item.pszText, (const char*)pCQueueFile->csRealFileSize);
		      break;
	     case QUEUECOL_PROPS:
			  memset(Props, 0, 5);
			  // set prefs based on local vs queue based prefs			  
			  if (pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS)
				 Props[i++] = 'D';			  
			  if (pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS)
				 Props[i++] = 'S';												 			  
			  if (pCQueueFile->Prefs & USE_LOCAL_COL_PREFS)
				 Props[i++] = 'C';												 
	          strcpy(pDispInfo->item.pszText, (const char*)Props);
		      break;
	     case QUEUECOL_SUBMITTED:
	          strcpy(pDispInfo->item.pszText, (const char*)pCQueueFile->csSubmitted);
		      break;
	     case QUEUECOL_STATUS:
	          strcpy(pDispInfo->item.pszText, (const char*)pCQueueFile->Status);
	          // add time (if there is one)
			  if (pCQueueFile->ctsElapsedTime.GetTotalSeconds() > 0)
				 {
			     cs = pCQueueFile->ctsElapsedTime.Format(" (%H:%M:%S)");				 
			     strcat(pDispInfo->item.pszText, cs);
				 }
		      break;
		 } 
	  }
   if (pDispInfo->item.mask & TVIF_IMAGE)
	  {
      pDispInfo->item.iImage = MapStatusImageState(pCQueueFile);
	  }  
   *pResult = 0;
   }

void CSpecialTree::OnGetdispinfoTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* tvdi;
	CQueueFile* pCQueueFile;

	tvdi = (TV_DISPINFO*)pNMHDR;	   
	
	if(tvdi->item.mask & TVIF_TEXT)
	   {
	   pCQueueFile = (CQueueFile*)(tvdi->item.lParam);
	   if (pCQueueFile->Description.GetLength() > 0) 
		  strcpy(tvdi->item.pszText, pCQueueFile->Description);
	   }
	if (tvdi->item.mask & TVIF_IMAGE)
	   {	   
	   pCQueueFile = (CQueueFile*)(tvdi->item.lParam);
	   tvdi->item.iImage = MapStatusImageState(pCQueueFile);
	   }
	if (tvdi->item.mask & TVIF_SELECTEDIMAGE)
	   {
	   pCQueueFile = (CQueueFile*)(tvdi->item.lParam);
	   tvdi->item.iSelectedImage = MapStatusImageState(pCQueueFile);
	   }
	*pResult = 0;
}

void CSpecialTree::OnLButtonDown(UINT nFlags, CPoint point) 
   {
   UINT iHitFlags;
   int nHitIndex;
   int nHitCol;

   // there seems to be an SEC or MFC focus bug, so we do this to
   // force focus
   //((CSplitterWnd*)GetParent())->SetActivePane(iViewStyle, 0);	
   //TRACE("SetActivePane = %d\n", iViewStyle);

   // mark as modified in case we change column widths
   PDOC->SetModifiedFlag();

   // where is the mouse?
   nHitIndex = GetListCtrl().HitTest( point, &iHitFlags, nHitCol );

   if (iHitFlags & LVHT_COLUMNHEADING)
	  {
	  // force up
	  DrawHeaderPushCol = nHitIndex;
	  Invalidate();	  
	  }	
   CRPTreeBase::OnLButtonDown(nFlags, point);
   }

void CSpecialTree::OnLButtonUp(UINT nFlags, CPoint point) 
   {
   // force up
   if (DrawHeaderPushCol != -1)
	  {	   
	  // call the sort routines
	  SortCol(DrawHeaderPushCol);
	  DrawHeaderPushCol = -1;
	  Invalidate();	  
	  }
   CRPTreeBase::OnLButtonUp(nFlags, point);
   }

void CSpecialTree::OnLButtonDblClk(UINT nFlags, CPoint point) 
   {
   UINT iHitFlags;
   int nHitIndex;
   int nHitCol;

   // where is the mouse?
   nHitIndex = GetListCtrl().HitTest( point, &iHitFlags, nHitCol );   

   // if we are on an item...
   if (iHitFlags & LVHT_INSIDEITEMBOUNDS)	  
	  {
      // update the info      
      CDWordArray* pArrSelected;
      CQueueFile* pCQueueFile;

      // get selection array
      pArrSelected = GetSelectionArray();   

      // how many are selected?
      if (pArrSelected->GetSize() == 1)
		 {
	     // get the only entrant
	     pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(pArrSelected->GetAt(0)));

         // if this is a batch file, launch the edit dialog
         if (pCQueueFile->FileType == FTYPE_BLIST)
			{
	        EditExistingBatchHeader(pCQueueFile);
		    CRPTreeBase::OnLButtonDblClk(nFlags, point);
			return;
			}
		 }	  
      // if it's not visible and we have files
      if ((!m_pPropFrame->IsWindowVisible()) && (GetSelectedCount() > 0))
		 {
		 // we need to run the UpdateInfo machinery here if we are not visible 
		 // so that we do some initial setup to avoid crashing
		 if (!m_pPropFrame->m_pModelessPropSheet->IsWindowVisible()) 
			UpdateInfo();
		 // set our active page
		 m_pPropFrame->m_pModelessPropSheet->SetOurActivePage();		 
   	     // show it
   	     m_pPropFrame->ShowWindow(SW_SHOWNA);   	     	    
		 }            
      UpdateInfo();   
	  }
   CRPTreeBase::OnLButtonDblClk(nFlags, point);
   }

CEdit* CSpecialTree::EditLabel(HTREEITEM hti, int nCol)
   {
   if (nCol == 0)
	  return SEC_TREECLASS::EditLabel(hti, nCol);
   else
	  return NULL;
   }				

void CSpecialTree::RenumberList()
{
	int i;
	int iNumEntries;

	if ( iViewStyle == QUEUESTYLE_PRINT )
		return;

	CQueueFile* pCQueueFile;

	iNumEntries = GetCount();

	// renumber the list   
	for (i = 0; i < iNumEntries; i++)      
	{	        
		// get info	  	  
		pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(i));
		//TRACE("%s, old pos = %d, new pos = %d\n", pCQueueFile->ShortName, pCQueueFile->QPos, i);
		pCQueueFile->QPos = i;
	}
}			 

#define FBUFSIZ 32000
void CSpecialTree::OnEditAddfiles() 
   {   
   char* path;
   int idx;
   BOOL OneFile;
   CString cs;      
   CString csTitle;   
   char* szFilter;   
   CString csLastPath;
   CString csFileExt;

   CStringArray csaTemp;

   int i;
   
   DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | 
	  OFN_ALLOWMULTISELECT | OFN_NOCHANGEDIR | OFN_ENABLETEMPLATE | OFN_EXPLORER;

/*
   szFilter = 
       "PostScript (*.ps; *.prn; *.cps; *.eps)|*.ps; *.prn; *.cps; *.eps|"       
	   "Adobe PDF (*.pdf)|*.pdf|"
	   "JPEG (*.jpg; *.jff; *.jtf)|*.jpg;*.jff;*.jtf|"	   
       "TIFF (*.tif)|*.tif|Targa (*.tga; *.rnd)|*.tga;*.rnd|"       	   
	   "Kodak FlashPix (*.fpx)|*.fpx|"	   
	   "PCX (*.pcx)|*.pcx|GIF (*.gif)|*.gif|"       
	   "SGI graphics format (*.sgi)|*.sgi|"
       "Photo CD (*.pcd)|*.pcd|CALS (*.cal)|*.cal|SUN Raster (*.ras)|*.ras|"       
	   "Windows & OS/2 (*.bmp)|*.bmp|"       												
	   "PhotoShop 3.0 (*.psd)|*.psd|"
	   "Portable Network Graphics (*.png)|*.png|"	   
       "All Files (*.*)|*.*||";
*/
	szFilter = 
		"PostScript (*.ps; *.prn; *.cps; *.eps)|*.ps; *.prn; *.cps; *.eps|"       
		"Adobe PDF (*.pdf)|*.pdf|"
		"JPEG (*.jpg; *.jff; *.jtf)|*.jpg;*.jff;*.jtf|"    
		"TIFF (*.tif)|*.tif|Targa (*.tga; *.rnd)|*.tga;*.rnd|"
		"Kodak FlashPix (*.fpx)|*.fpx|"    
		"PCX (*.pcx)|*.pcx|GIF (*.gif)|*.gif|"       
		"SGI graphics format (*.sgi)|*.sgi|"
		"Photo CD (*.pcd)|*.pcd|CALS (*.cal)|*.cal|SUN Raster (*.ras)|*.ras|"       
		"Windows & OS/2 (*.bmp)|*.bmp|"

		"PhotoShop 3.0 (*.psd)|*.psd|"
		"Portable Network Graphics (*.png)|*.png|"    
		"All Image Files|*.ps;*.prn;*.cps;*.eps;*.pdf;*.jpg;*.jff;*.jtf;*.tif;*.tga;*.rnd;*.fpx;*.sgi;*.pcd;*.bmp;*.psd;*.png|"
		"All Files (*.*)|*.*||";

   // construct common dialog   
   CCustomFileDialog cfd(TRUE, NULL, NULL, dwFlags, szFilter, this);
   
   // get instance handle
   cfd.m_ofn.hInstance = AfxGetInstanceHandle();                
   
   // load the initial filter index
   cfd.m_ofn.nFilterIndex = PDOC->FilterIdx;
   
   // open file title
   csTitle.LoadString(IDS_ADDFILES_DLGPRMPT);
   cfd.m_ofn.lpstrTitle = csTitle;
   						 
   // set initial dir
   cfd.m_ofn.lpstrInitialDir = PDOC->InitialFileDir;

   cfd.pDefaultDevice = &QUEUEDEVICE;
   cfd.pDefaultTransform = &QUEUETRANSFORM;
   cfd.pCC = &QUEUECOLOR;

   // expand the buffer!
   DWORD MAXFILE = 2562; //2562 is the max
   cfd.m_ofn.nMaxFile = MAXFILE;
   char* pc = new char[MAXFILE];
   cfd.m_ofn.lpstrFile = pc;
   cfd.m_ofn.lpstrFile[0] = NULL;
   
   // do dialog
   if (cfd.DoModal() == IDOK)
      {  
      // load wait cursor
      AfxGetApp()->DoWaitCursor(1);

	  // get row to insert at   
	  idx = ((GetCount() == 0) ? 0 : ItemIndex(GetCaretItem()) + 1);

	  POSITION pos;
	  CString szPathName; 
	  OneFile = FALSE;
	  path = NULL;		 	  

	  // get last path
	  if (pos = cfd.GetStartPosition())
		 {
		 csLastPath = cfd.GetNextPathName(pos);
		 }

	  // get all the files
	  for (pos = cfd.GetStartPosition(); pos != NULL; )
		 {
		 szPathName = cfd.GetNextPathName(pos);

		 csaTemp.Add(szPathName);
		 }

	  if ( csaTemp.GetSize() == 1 )
	  {
		  OneFile = TRUE;
		  szPathName = cfd.csLastFile;
	  }

	  if ( OneFile == TRUE )
	  {
		 HANDLE hFile;
		 WIN32_FIND_DATA FileInfo;
		 if ((hFile = FindFirstFile(szPathName, &FileInfo)) != INVALID_HANDLE_VALUE)
			{      
			// close the handle
			FindClose(hFile);   
			}

		 if (FileInfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			AddFolder(szPathName, idx);
		 else
		 {
			InsertQueueFile(szPathName, idx);		
		 }
	  }
	  else
	  {
		  // add all the files
		  for (i = csaTemp.GetSize() - 1; i >= 0; i--)
			 {
			 szPathName = csaTemp.GetAt(i);

			 HANDLE hFile;
			 WIN32_FIND_DATA FileInfo;
			 if ((hFile = FindFirstFile(szPathName, &FileInfo)) != INVALID_HANDLE_VALUE)
				{      
				// close the handle
				FindClose(hFile);   
				}

			 if (FileInfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
				AddFolder(szPathName, idx);
			 else
			 {
				InsertQueueFile(szPathName, idx);		
			 }
			 }
		  csaTemp.RemoveAll();
	  }
													  
      // if nothing is selected, select the first item      
	  if ((GetSelectedCount() == 0)  && (GetCount() > 0))
         {         
		 SelectItem(GetItemAt(0));
         }         
      // reset state if we are currently idle
      if (InternalState == STATE_IDLE_NO_FILES)
		  SetState(STATE_IDLE_SOME_FILES);  
	  
      // our doc is modified
      PDOC->SetModifiedFlag();
      // if the info box is on, update it
	  UpdateInfo();						   

	  // seem to need this to force scroll bars	  
	  ReMeasureAllItems();	  
	  Invalidate();
	  
	  // reset initial file dir
	  CString FullPathString; 
	  CString EndPathString; 
	  if (csLastPath.GetLength() > 0)
	     _splitpath((const char *)csLastPath, FullPathString.GetBuffer(_MAX_DRIVE),
	     EndPathString.GetBuffer(_MAX_PATH), NULL, NULL);	  

	  //_splitpath(OneFile ? path : (const char *)cs, FullPathString.GetBuffer(_MAX_DRIVE),
	  //   EndPathString.GetBuffer(_MAX_PATH), NULL, NULL);	  
      FullPathString.ReleaseBuffer();	  
	  EndPathString.ReleaseBuffer();	        	     
	  PDOC->InitialFileDir = FullPathString + EndPathString;
	  // nuke trailing slash
	  if (PDOC->InitialFileDir.GetLength() > _MAX_DRIVE)
	     PDOC->InitialFileDir = 
	        PDOC->InitialFileDir.Left(PDOC->InitialFileDir.GetLength() - 1);	     
	  // reset cursor      
	  AfxGetApp()->DoWaitCursor(-1);
	  
 	  // save the initial filter index
      PDOC->FilterIdx = cfd.m_ofn.nFilterIndex;   
	  }

   delete pc;

   // clean up rip from custom file dialog
   if (cfd.cr.IsInit) 
	  {	  
	  cfd.cr.CancelProcess(TRUE);	  
	  cfd.cr.Close();
	  }
   if (cfd.pCQueueFile) delete cfd.pCQueueFile;

   	// renumber the list
	RenumberList();

   // start server if needed 
   if (!bPrintServerStarted) 
	  StartServer();
   else
	  // restart server?
      RestartServer();
   }

#define DRAGSCROLLDELAY 1000

DROPEFFECT CSpecialTree::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
   {	
   return OnDragOver(pDataObject, dwKeyState, point);
   }

void CSpecialTree::OnDragLeave() 
   {
   SelectDropTarget( NULL );		
   CRPTreeBase::OnDragLeave();
   }

DROPEFFECT CSpecialTree::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
   {
   int nItem, nHitCol;
   HTREEITEM hti;
   UINT      htflags;   

   // highlight this window   
   SetFocus();         
//   TRACE("CSpecialTree::OnDragOver: SetFocus()\r\n");

   DROPEFFECT de=DROPEFFECT_NONE;

   if( dwKeyState & MK_CONTROL )
      de=DROPEFFECT_COPY;      
   else
      de=DROPEFFECT_MOVE;

   //hit-test stuff
   ClientToScreen(&point);
   ScreenToClient(&point);       
   hti=GetTreeCtrl().HitTest( point, &htflags );
   nItem= GetListCtrl().HitTest( point, &htflags, nHitCol );
   
   if( hti )
	  {
	  int nLastFullyVisible = GetLastFullyVisibleItem();
	  int nTopIndex = GetTopIndex();

	 if (nItem > nLastFullyVisible && nLastFullyVisible != -1)
		{
	    if (dwScrollStart == 0)	  
	       dwScrollStart = GetTickCount();	  
        else if ((GetTickCount() - dwScrollStart) > DRAGSCROLLDELAY)	  	  
	       ScrollVert(nItem - nLastFullyVisible, TRUE);
		}		
	 else if (nItem < nTopIndex)
	    ScrollVert(nItem - nTopIndex);
	 else if ((nItem == nTopIndex) && (nTopIndex > 0))
		{
	    if (dwScrollStart == 0)	  
	       dwScrollStart = GetTickCount();	  
        else if ((GetTickCount() - dwScrollStart) > DRAGSCROLLDELAY)	  	  
	       ScrollVert(-1);
		}
	  }
   return de;
   }

void CSpecialTree::BeginDrag(NMHDR* pNMHDR, LRESULT* pResult)  
   {
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;	    
   int i, cnt, numChar;
   CQueueFile* pCQueueFile;
   COleDropSource OleDropSource;

   if (GetListCtrl().GetSelectedCount() <= 0) 
      {
	  *pResult = 0;
	  return;
	  }
   
   // mark for potential deletion
   for (i = 0; i < GetCount(); i++)
      {                         
      // get the queue item      
	  pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(i));	  
	  if (IsSelected(i))
	     {
		 // don't allow drag and drop on a file that's ripping		 
		 if ((pCQueueFile->StatusFlag & FILE_STATUS_INRIP) == FILE_STATUS_INRIP) return;
		 // set selected flag for when we drop
		 pCQueueFile->Selected = TRUE;	     
		 // if dragging from an archive, unset wastouched flag and don't delete it
         if (iViewStyle == QUEUESTYLE_ARCHIVE)
			{
			pCQueueFile->WasTouched = FALSE;
			pCQueueFile->DragDelete = FALSE;
			}
		 else
			pCQueueFile->DragDelete = TRUE;
		 }
	  else
	     {
		 // set selected flag for when we drop
		 pCQueueFile->Selected = FALSE;
	     pCQueueFile->DragDelete = FALSE;
		 }
	  }  
       
   CopyToClipboard(OURCLIP_FILE);
   CString csHolder;
   csHolder.LoadString(IDS_OLE_CBOARDHOLDER);

   // Get the text length
   numChar = csHolder.GetLength();
    
   // Create global memory for sharing dragdrop text
   HGLOBAL hgData=GlobalAlloc(GPTR,numChar+1);   
   ASSERT(hgData!=NULL);
    
   LPSTR   lpData=(LPSTR)GlobalLock(hgData);
   ASSERT(lpData!=NULL);
                                                 
   strcpy(lpData, (const char*)csHolder);
   
   COleDataSource OleDataSource; 
   // Cache the data, and initiate DragDrop   
   OleDataSource.CacheGlobalData(uiOurListClipFormat, hgData);              
    
   // Set drag/drop source rectangle (screen coord)
   //SetDragSourceRect();

   // renumber the list
   RenumberList();
    
   DROPEFFECT dropEffect =
      OleDataSource.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE,NULL);       
   
   // if a move, we must delete the files  
   if((dropEffect&DROPEFFECT_MOVE)==DROPEFFECT_MOVE)     
      {
	  cnt = GetCount();
	  // do it in reverse order so count stays the same
      for (i = cnt - 1; i >= 0; i--)
         {                         
         // get the queue item
         pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(i));
	     if (pCQueueFile->DragDelete)
	        {
			// don't delete files that are imaging!	  
			if ((pCQueueFile->StatusFlag & FILE_STATUS_INRIP) == FILE_STATUS_INRIP) continue;              
	        // delete the CQueue object
            delete pCQueueFile;                        
            // delete the string            
            DeleteItem(GetItemAt(i));
			// delete from the array
            pQueueArray->RemoveAt(i);	    			
		    }
		 }
	  
   	  // renumber the list
	  RenumberList();	  
	  ReMeasureAllItems();	  
	  Invalidate();
      }
   else if((dropEffect&DROPEFFECT_COPY)==DROPEFFECT_COPY)     
	  {
	  // if we are copying (not moving), we need to generate
	  // a new job id
	  cnt = GetCount();	     
      for (i = 0; i < cnt; i++)
		 {		
		 // get the queue item
		 pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(i));
		 if (pCQueueFile->Selected) 
			{
			pCQueueFile->iJobID = ((CZeusApp*)AfxGetApp())->GetJobID();
			}
		 }
	  }		   	

   // Clear the Data Source's cache
   OleDataSource.Empty();        
   *pResult = 0;
   }

void CSpecialTree::OnDropFiles(HDROP hDropInfo, WORD InQueue) 
   {
   int i, idx, NumDropped;
   CString csFile;   
   CFile cfTest;

   // get the number of files dropped
   NumDropped = DragQueryFile(hDropInfo, 0xffffffff, NULL, 0);

   // get row to insert at   	  
   idx = ((GetCount() == 0) ? 0 : ItemIndex(GetCaretItem()) + 1);

   // insert the dropped files
   for (i = 0; i < NumDropped; i++)
      {
	  // get file
	  DragQueryFile(hDropInfo, i, csFile.GetBuffer(_MAX_PATH), _MAX_PATH);
	  csFile.ReleaseBuffer();

	  // get status
	  HANDLE hFile;
	  WIN32_FIND_DATA FileInfo;
	  if ((hFile = FindFirstFile(csFile, &FileInfo)) != INVALID_HANDLE_VALUE)
		 {      
		 // close the handle
		 FindClose(hFile);   
		 }
	  if (FileInfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)	  
         AddFolder(csFile, idx);
	  else
		 InsertQueueFile(csFile, idx);

	  // force a reshow
	  ReMeasureAllItems();
	  Invalidate();

	  // show it
	  UpdateInfo();
	  }
   if (NumDropped > 0)
      {
 	  // reset state if we are currently idle
      if (InternalState == STATE_IDLE_NO_FILES) SetState(STATE_IDLE_SOME_FILES);                     
	  // if nothing is selected, select the first item
	  if ((GetSelectedCount() == 0)  && (GetCount() > 0))
         {         
		 SelectItem(GetItemAt(0));
         }         
	  // renumber the list
	  RenumberList();	  
	  ReMeasureAllItems();
	  Invalidate();
	  PDOC->SetModifiedFlag();
	  }
   CRPTreeBase::OnDropFiles(hDropInfo);

   // start server if needed 
   if (!bPrintServerStarted) 
	  StartServer();
   else
	  // restart server?
      RestartServer();
   }

void CSpecialTree::DeleteFiles() 
   {  
   CQueueFile* pCQueueFile;      
   CDWordArray *pArrSelected = NULL;
   DWORD dwIndex;	  
   int nArrIdx;

   if (GetListCtrl().GetSelectedCount() <= 0)
	  return;
   else
	  {	  
	  // get selection array
	  pArrSelected = GetSelectionArray();	

	  // delete all selected files
	  for (nArrIdx = pArrSelected->GetUpperBound(); nArrIdx >= 0; --nArrIdx)
		 {
		 // get the index
		 dwIndex = pArrSelected->GetAt(nArrIdx);
		 // get the item
		 pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(dwIndex));
		 // don't allow delete of files that are printing
		 if ((pCQueueFile->StatusFlag & FILE_STATUS_INRIP) == FILE_STATUS_INRIP)
		    continue;      		
         // delete the file if it's a temporary file!
		 if (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_SERVERFILE)
		    ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->DeleteLastServerFile(pCQueueFile);
		 // delete the preview if necessary
		 if (iViewStyle == QUEUESTYLE_HOLD)
			((CMainFrame*)(AfxGetApp()->m_pMainWnd))->Preview.DeletePreviewJob(pCQueueFile->iJobID, 0, TRUE);

		 // Delete the JPG associated with the PS File
		 if (!pCQueueFile->PS_JPGFileLongName.IsEmpty() )
			 ::DeleteFile(((const char*)pCQueueFile->PS_JPGFileLongName));
		 // delete the queue file
		 delete pCQueueFile;			
		 // delete the item from the list
         DeleteItem(GetItemAt(dwIndex));
		 // delete from the array
         pQueueArray->RemoveAt(dwIndex);
		 }
      // our doc is modified
      PDOC->SetModifiedFlag();

	  Invalidate();	  
       
      // if the info box is on, update it
      UpdateInfo();   

      // renumber the list
      RenumberList();

	  // force a reshow
	  ReMeasureAllItems();
	  Invalidate();

      // if nothing is selected, and we have items, select the first item
      if ((GetSelectedCount() == 0)  && (GetCount() > 0))
		 {         
	     SelectItem(GetItemAt(0));
		 }         
	  }
   }

void CSpecialTree::CopyToClipboard(int fmt)
   {        
   int i, j;      
   CQueueFile* pCQueueFile;   
   CQueueFile* pNewCQueueFile;            
   CString csSection, csEntry;
   int cnt = GetCount();

   if (cnt > 0)
	  {
	  // delete the current array if any
	  CLIPARRAY.Destroy();
	  // set the new type
	  CLIPARRAY.SetType(fmt);

	  for (i = cnt - 1, j=0; i >= 0; i--)
		 {         
	     if (IsSelected(i))
			{
            // get the queue item      
	        pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(i));
#if 0
	        // unset serverfile flag so it can't be deleted		 
		    if (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_SERVERFILE)
		       pCQueueFile->ExtraInfo -= CQUEUE_EXTRA_SERVERFILE;		
#endif
            // make a copy
            pNewCQueueFile = new CQueueFile;
            *pNewCQueueFile = *pCQueueFile; 
		    // insert into the clip array
	        CLIPARRAY.InsertAt(j++, pNewCQueueFile);
			}
		 }
	  }
   }  

BOOL CSpecialTree::OnDrop(COleDataObject* pDataObject, 
    DROPEFFECT dropEffect, CPoint point, WORD InQueue)
   {  
   HGLOBAL  hGlobal;
   LPCSTR   pData;
    
   // Get text data from COleDataObject   
   hGlobal=pDataObject->GetGlobalData(uiOurListClipFormat);   

   // if hGlobal is NULL, return FALSE and let our
   // non-OLE code do the work   
   if (!hGlobal) return FALSE;

   // Get pointer to data
   pData=(LPCSTR)GlobalLock(hGlobal);       

   ASSERT(pData!=NULL); 

   InsertDroppedQueueFiles(point, InQueue);	

   // Unlock memory
   GlobalUnlock(hGlobal);   

   return TRUE;
   }  

void CSpecialTree::InsertDroppedQueueFiles(CPoint& point, WORD InQueue)
   {
   int i, idx;            
   CQueueFile* pCQueueFile;   
   int cnt = GetCount();
   
   UINT iHitFlags;
   int nHitCol;
   idx = GetListCtrl().HitTest( point, &iHitFlags, nHitCol );

   // check out of bounds
   if (idx == -1)
	  {	  
   	  if (iHitFlags & LVHT_ABOVE)
	     idx = 0;	   
	  else
	     idx = GetCount() - 1;
	  }

   // available?
   if (CLIPARRAY.IsTypeAvail(OURCLIP_FILE))
      {
      // get all elements and insert them
      for (i = 0; i < CLIPARRAY.GetSize(); i++)
         {	                  
		 if (((((CQueueFile*)(CLIPARRAY.GetAt(i)))->StatusFlag & FILE_STATUS_CHECK) == 
			FILE_STATUS_OK_DELETED)	&& (iViewStyle != QUEUESTYLE_ARCHIVE))
			{
			AfxMessageBox(IDS_NOPASTE_DELETED, MB_SETFOREGROUND);
			continue;
			}
         // create a new queue file
         pCQueueFile = new CQueueFile;
         // copy the data
         *pCQueueFile = *((CQueueFile*)(CLIPARRAY.GetAt(i)));
	     // in what queue?
		 pCQueueFile->InQueue = InQueue;
		 // this is a new drop, so mark as not touched
         pCQueueFile->WasTouched = FALSE;			  

		 // lock in the local prefs
		 if (InQueue == QUEUESTYLE_ARCHIVE)
			{
            // if no local prefs, lock them in now   
            if (!(pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS))
			   {
		       pCQueueFile->pCD = new CDevice;
		       *(pCQueueFile->pCD) = QUEUEDEVICE;
		       pCQueueFile->Prefs |= USE_LOCAL_DEV_PREFS;        
			   }
	        if (!(pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS))
			   {
		       pCQueueFile->pTransform = new CTransform;
		       *(pCQueueFile->pTransform) = QUEUETRANSFORM;
		       pCQueueFile->Prefs |= USE_LOCAL_TRN_PREFS;        
			   }
	        if (!(pCQueueFile->Prefs & USE_LOCAL_COL_PREFS))
			   {
		       pCQueueFile->pCC = new CColorTransform;
		       *(pCQueueFile->pCC) = QUEUECOLOR;
		       pCQueueFile->Prefs |= USE_LOCAL_COL_PREFS;        
			   }
			}

		 // if dropping to the hold queue, reset the status
		 if (InQueue == QUEUESTYLE_HOLD)
			{
            pCQueueFile->Status.LoadString(IDS_READY);
            pCQueueFile->PreviewStatus.LoadString(IDS_READY);
            pCQueueFile->StatusFlag |= FILE_STATUS_HOLD;
			pCQueueFile->ctsElapsedTime = 0;
			}
		 
		 if (InQueue == QUEUESTYLE_PRINT)
		 {
		 	  // Add(1) to the Count of Print Pane Entries
			  pPrintPaneState->IncrPrintPaneEntries();
		 }

		 // if hit test is nowhere, assume that we are in "open
		 // space" so put it at end
		 if (iHitFlags & LVHT_NOWHERE)
			{
			pQueueArray->InsertAt(cnt, pCQueueFile);
			InsertString(idx + 1, pCQueueFile);
			}
		 else
			{
			pQueueArray->InsertAt(idx, pCQueueFile);	             
		    InsertString(idx, pCQueueFile);
			}
         }	   
      // reset state if we are currently idle
      if (InternalState == STATE_IDLE_NO_FILES) SetState(STATE_IDLE_SOME_FILES);   
      // our doc is modified
      PDOC->SetModifiedFlag();
	  Invalidate();	  
      // if the info box is on, update it
	  UpdateInfo();			  	      
	  }	
   // renumber the list
   RenumberList();  
   // force a reshow
   ReMeasureAllItems();
   Invalidate();   	   

   // start server if needed 
   if (!bPrintServerStarted) 
	  StartServer();
   else
      // restart server?
      RestartServer();
   }   

void CSpecialTree::InitialUpdate()
   {
	CProfile Profile;

   HICON hIcon;

   // create an image list
   m_ImageList.Create(16, 16, TRUE, 8, 0);

   // add status icons to the image list
   hIcon = AfxGetApp()->LoadIcon(IDI_STATUS_HOLD); 
   m_ImageList.Add(hIcon);
   hIcon = AfxGetApp()->LoadIcon(IDI_STATUS_WAIT1); 
   m_ImageList.Add(hIcon);
   hIcon = AfxGetApp()->LoadIcon(IDI_STATUS_OK); 
   m_ImageList.Add(hIcon);
   hIcon = AfxGetApp()->LoadIcon(IDI_STATUS_NOTOK); 
   m_ImageList.Add(hIcon);
   hIcon = AfxGetApp()->LoadIcon(IDI_STATUS_OK_DELETED); 
   m_ImageList.Add(hIcon);
   hIcon = AfxGetApp()->LoadIcon(IDI_STATUS_PROPS_DEVICE); 
   m_ImageList.Add(hIcon);					 
   hIcon = AfxGetApp()->LoadIcon(IDI_STATUS_PROPS_TRANSFORM); 
   m_ImageList.Add(hIcon);					 
   hIcon = AfxGetApp()->LoadIcon(IDI_STATUS_PROPS_COLOR); 
   m_ImageList.Add(hIcon);					    									 
   m_ImageList.SetBkColor(GetSysColor(COLOR_WINDOW));

   GetTreeCtrl().SetImageList(&m_ImageList, TVSIL_NORMAL);   

   // only need to do this once
   if (iViewStyle == QUEUESTYLE_ARCHIVE)
   {
      PDOC->SetTitle(PDOC->csTitle);
   }

   // get pointer to child frame
   pCQueueChild = (CQueueChild*)GetParentFrame();

   // set the state 
   if (GetCount() > 0)      
	  {
      if (InternalState == STATE_IDLE_NO_FILES) SetState(STATE_IDLE_SOME_FILES);   
	  // if we are in print queue and we have files, do it
      if (iViewStyle == QUEUESTYLE_PRINT)
         StartServer();
      }
   else      
	  SetState(STATE_IDLE_NO_FILES);     

   // create new pane element
   pPaneElement = new CPaneElement;

   if (pPaneElement) 
	  {
      // add pointer to document
      pPaneElement->pDocPtr = (void*)PDOC;
	  // add pointer to special tree
	  pPaneElement->pcstPtr = this;
	  // kind of view
	  pPaneElement->iViewStyle = iViewStyle;
      // set InRip flag
      pPaneElement->InRip = FALSE; 
      // add to array
      PANEARRAY.Add(pPaneElement);
	  }					
   // set bar states (do in archive pane)
   if (iViewStyle == QUEUESTYLE_ARCHIVE)
	  {
      CControlBar* pBar;
      pBar = pCQueueChild->GetControlBar(CG_ID_VIEW_PSOUT);
      if (pBar != NULL) 
         pCQueueChild->ShowControlBar(pBar, PDOC->bPSoutState, TRUE);
      pBar = pCQueueChild->GetControlBar(CG_ID_VIEW_QUEUESTATUS);
      if (pBar != NULL) 
        pCQueueChild->ShowControlBar(pBar, PDOC->bQStatusState, TRUE);		  
	  }   

   // add items to menu
   if (iViewStyle == QUEUESTYLE_ARCHIVE)
	  {
      // redo the menu
      ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->AddDevNamesToMenu();
	  ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->AddToolsToMenu();
	  }
   pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_IDLE);   
   iLastQueueStatus = IDS_QUEUESTATUS_IDLE;

   // create a print pane state object so devices
   // can find out how many jobs are in the print pane
   if (iViewStyle == QUEUESTYLE_PRINT)
	  {
	  if (pPrintPaneState = new CPrintPaneState)
		 {
		 // set the name
		 pPrintPaneState->SetName(PDOC->csTitle);
		 }
	  }
   else
	  pPrintPaneState = NULL;	

   ((CZeusApp*)AfxGetApp())->BackupRegistry();
   
   CString csFriendlyName = PDOC->csTitle;
   if ( Profile.GetInt(QUEUEDEVICE.Name, "ReInstallQueue") )
   {
	   // Force the Splash Screen to close
	   // Prevents Dialog Boxes from 'hiding' behind the Splash Screen
	   ((CZeusApp*)AfxGetApp())->RemoveSplashScreen();

	   Profile.WriteInt(QUEUEDEVICE.Name, "ReInstallQueue", 0 );
	   ReinstallAppDriver( TRUE );
   }

	// start server polling if it is enabled
   StartServerPolling(TRUE);
   }

// this method fixes a problem with the Stingray toolkit
// a right click in an empty list causes a crash
void CSpecialTree::OnRButtonDown(UINT nFlags, CPoint point)
{	
    //if(m_pToolTipWnd->IsWindowVisible()) // problem!!!!!
    if(m_pToolTipWnd->GetSafeHwnd() && m_pToolTipWnd->IsWindowVisible())
    {
        CWnd* pWndTop = AfxGetMainWnd();		
        SendMessage(WM_MOUSEACTIVATE, WPARAM(pWndTop->m_hWnd), MAKELPARAM(HTCLIENT, WM_RBUTTONDOWN));
    }	

    //if( GetFocus() != this )
	//	SetFocus();

	// lets see whats been hit
    UINT iHitFlags;
	int nHitIndex;
	int nHitCol;
    nHitIndex = HitTest( point, &iHitFlags, nHitCol );

	// Assume the click cannot be used to start dragging until we determine otherwise.
	m_bLookForBeginDrag = FALSE;

	BOOL bSelChange = FALSE;

	if (!(iHitFlags & LVHT_COLUMNSEP))
	{
		ListHitTestEx( point, nHitIndex, iHitFlags, m_bLookForBeginDrag, bSelChange );
	}

	// change the selection
	if (bSelChange)
	{
   		ProcessSelectChange(nHitIndex,iHitFlags,nFlags,ByMouse);
   		ExecuteSelectChange(nHitIndex, ByMouse);
	}


	// Remember lbutton down point for drag&drop logic in OnMouseMove
    m_ptButtonDown = point;

	// Redraw the tooltip
	m_pToolTipWnd->Refresh();
	
	// reset the dragging flag because if this is an OLE drag drop you
	// never get an OnLButtonUp
	if ( m_bDragging )
		m_bDragging = FALSE;	

	SEC_LISTBASE::OnRButtonDown(nFlags, point);
}

void CSpecialTree::SetNoneTouched()
   {
   int i;
   CQueueFile* pCQueueFile;   

   // renumber the list
   for (i = 0; i < GetCount(); i++)      
      {
	  pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(i));
	  pCQueueFile->WasTouched = FALSE;
	  }
   }

void CSpecialTree::Print()
   {
   // set the CancelFlag
   CancelFlag = FALSE;
   
   // set the PauseFlag
   PauseFlag = FALSE;   
   
   // set the Rip complete flag;
   RipComplete = TRUE;
   
   // get the queue start time
   PDOC->QueueStartTime = CTime::GetCurrentTime();

   // write to the log
   if (OutputType != RIPSTYLE_PREVIEW && OutputType != RIPSTYLE_HD_PREVIEW && !bSentStartToLog)
	  {
      CString csTimeString;
      csTimeString = PDOC->QueueStartTime.Format("Start Printing: %#I:%M:%S %p %x");   
      PDOC->cLog.AddText(csTimeString);
	  if (PDOC->LogWnd.m_hWnd)
		  WriteLog();
	  bSentStartToLog = TRUE;
	  }

   // in case we hose early
   PDOC->QueueElapsedTime = CTime::GetCurrentTime() - PDOC->QueueStartTime;

   // start timer   
   if (!SetTimer(RP95_PRINT_TIMER_ID, RP95_PRINT_TIMER_TIMEOUT, NULL))
      AfxMessageBox(IDS_SYS_TIMER_ERROR, MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);      
   else
      {
	  LastTimeCheck = GetTickCount();
      }         
   bPrintServerStarted = TRUE;

   // start this right away
   OnContinuePrinting();
   }


void CSpecialTree::SetScreenSaver(BOOL bRestore)
   {
   if (bRestore)
	  {
      // restore screen saver 
      SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, bScreenSaverState, NULL, SPIF_SENDCHANGE);
	  }
   else
	  {
      // get screen saver state
      bScreenSaverState = 
	     SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, NULL, SPIF_SENDCHANGE);
      // turn screen saver off
      SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, SPIF_SENDCHANGE);
	  }
   }

void CSpecialTree::StartServerPolling(BOOL FirstTime)
   {
   // only do this for print queue
//   if (iViewStyle != QUEUESTYLE_PRINT)
//	   return;

   // is the hot folder enabled?
   //if (!PDOC->crrrs.bEnableHotFolder) return;
   if (PDOC->crrrs.iHotFolderType == HOTFOLDER_TYPE_DISABLED)
	   return;

   // are we already doing it?
   if (ServerPolling)
	   return;

   // start server timer   
   if (!SetTimer(RP95_SERVER_TIMER_ID, RP95_SERVER_TIMER_TIMEOUT, NULL))
      AfxMessageBox(IDS_SYS_TIMER_ERROR, MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);      
   else
      {
	  if (FirstTime) PDOC->crrrs.InitPolling();
      ServerPolling = TRUE;
	  }
   }

void CSpecialTree::StopServerPolling()
   {
   if (ServerPolling)
      {
	  ServerPolling = FALSE;
      KillTimer(RP95_SERVER_TIMER_ID);
	  }
   }

void CSpecialTree::ProcessServerPolling()
   {
   BOOL bAddToQueue = TRUE;
   CStringArray csa;
   CString csFileName;
   int idx, i, NumFound;
   int iTotalPages = 0;
   int iSelectedMedia = 0;
   CQueueFile* pCQueueFile;
#ifdef LEADFILE_PS
   int iResetJobID;
   CQueueFile* pCQueueFile_Page;
#endif
   int iPage = 0;
   BOOL bContinue = TRUE;
   
   // iViewStyle should be equal to PDOC->crrrs.iHotFolderInPane
   int myval = PDOC->crrrs.iHotFolderInPane;
   if (iViewStyle != PDOC->crrrs.iHotFolderInPane) return;

   // get poll files	  
   NumFound = PDOC->crrrs.GetPollFiles(csa);   

   // get row to insert at   	  
   // insert at end
   idx = GetCount();

   // add to queue
   for (i = 0; i < NumFound; i++)
   {
	  iPage = 0;
      // try to insert it into the list		 
      if (pCQueueFile = new CQueueFile)
      {
		 csFileName = csa.GetAt(i);

		 if (pCQueueFile->Init(csFileName, iTotalPages, iPage))
		 {
			if ( PDOC->crrrs.m_bSelectBySize )
			{
				iSelectedMedia = CheckImageSize(pCQueueFile->BoundingBox);
				if ( iSelectedMedia >= 0 )
				{
					bAddToQueue = TRUE;
					pCQueueFile->pCD = new CDevice;
					*(pCQueueFile->pCD) = QUEUEDEVICE;
					pCQueueFile->pCD->pDevInfo->CurrentMediaIdx = iSelectedMedia;
					pCQueueFile->Prefs |= USE_LOCAL_DEV_PREFS;
				}
				else 
				{
					bAddToQueue = FALSE;
				}
			}
			else
			{
				bAddToQueue = TRUE;
			}

			if ( bAddToQueue )
			{
				// not touched     
				pCQueueFile->WasTouched = FALSE;
				// in what queue?
				pCQueueFile->InQueue = iViewStyle;
				// set this flag so we can intelligently erase
				pCQueueFile->ExtraInfo += CQUEUE_EXTRA_POLLEDFILE;
				// store it               
				pQueueArray->InsertAt(idx, pCQueueFile);	    
				// add index of new QueueFile to ListBox
				InsertString(idx++, pCQueueFile);
			}
			else
			{
				delete pCQueueFile;
			}
		 }
	  }
      else
	  {
	     // error
	     delete pCQueueFile;
	  }

#ifdef LEADFILE_PS
	  iPage++;

	  while ( iPage < iTotalPages && bContinue )
	  {
		if ( pCQueueFile_Page = new CQueueFile )
		{
			iResetJobID = pCQueueFile_Page->iJobID;
			*pCQueueFile_Page = *pCQueueFile;
			pCQueueFile_Page->iJobID = iResetJobID;

			pCQueueFile_Page->PS_JPGFileLongName = ((CZeusApp*)AfxGetApp())->Convert_PS_to_JPG(pCQueueFile_Page->LongName, iPage);

			pQueueArray->InsertAt(idx, pCQueueFile_Page);	    
			// add index of new QueueFile to ListBox
			InsertString(idx++, pCQueueFile_Page);
			iPage++;
		}
		else
		{
		    delete pCQueueFile_Page;
			bContinue = FALSE;
		}
	  }		// End While
#endif
   }
   if (NumFound > 0)
      {
      // reset state if we are currently idle
      if (InternalState == STATE_IDLE_NO_FILES) SetState(STATE_IDLE_SOME_FILES);               
      // our doc is modified
      PDOC->SetModifiedFlag();
      // if the info box is on, update it
	  UpdateInfo();		
	  // renumber the list
      RenumberList();  
	  // force a reshow
	  ReMeasureAllItems();
	  Invalidate();

      // start server if needed 
      if (!bPrintServerStarted) 
		 StartServer();
	  else
         // restart server
         RestartServer();
	  }
   }  

BOOL CSpecialTree::IsSelected(int idx)
   {
	return GetTreeCtrl().IsSelected(GetItemAt(idx));
   }

int CSpecialTree::SendFileToRIP(CQueueFile* pCQueueFile, BOOL bDumpGangPrints)
   {         
   UINT ret;               
   CString cs, csSection, csEntry;
   CDevice* pLocalDevice;
   CTransform* pLocalTransform;
   CColorTransform* pLocalColor;

   // set tray title
   pCQueueChild->SetTrayTitle(PDOC->csTitle);

   // if printing, lock in the prefs now
   if (OutputType == RIPSTYLE_OUTPUT)	
	  {								
	  // if no local prefs, lock them in now   
	  if (!(pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS))
		 {
		 pCQueueFile->pCD = new CDevice;
		 *(pCQueueFile->pCD) = QUEUEDEVICE;
		 pCQueueFile->Prefs |= USE_LOCAL_DEV_PREFS;        
		 }
	  if (!(pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS))
		 {
		 pCQueueFile->pTransform = new CTransform;
		 *(pCQueueFile->pTransform) = QUEUETRANSFORM;
		 pCQueueFile->Prefs |= USE_LOCAL_TRN_PREFS;        
		 }
	  if (!(pCQueueFile->Prefs & USE_LOCAL_COL_PREFS))
		 {
		 pCQueueFile->pCC = new CColorTransform;
		 *(pCQueueFile->pCC) = QUEUECOLOR;
		 pCQueueFile->Prefs |= USE_LOCAL_COL_PREFS;        
		 }
	  }

   // set proper devmode      
   if ((pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS) && pCQueueFile->pCD)
      pLocalDevice = pCQueueFile->pCD;
   else
      pLocalDevice = &QUEUEDEVICE;

   // set proper transform
   if ((pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS) && pCQueueFile->pTransform)
      pLocalTransform = pCQueueFile->pTransform;
   else
      pLocalTransform = &QUEUETRANSFORM;

   // set proper color transform
   if ((pCQueueFile->Prefs & USE_LOCAL_COL_PREFS) && pCQueueFile->pCC)
      pLocalColor = pCQueueFile->pCC;
   else
      pLocalColor = &QUEUECOLOR;

	pCPInitStruct = &pLocalColor->CPInitStruct;
    strncpy(pCPInitStruct->Printer_Name,pLocalDevice->Name,MAX_PATH);
 	pCPInitStruct->GlobalPrinterID = pLocalDevice->DevIdx; 
	pCPInitStruct->Printer_CSpace = pLocalDevice->pDevInfo->iColorSpace; 	
   
   // check file type
   if (pCQueueFile->FileType == FTYPE_UNKNOWN && !bDumpGangPrints)
   {
      return SYN_ERRMSG_BADFORMAT;
   }

   if ( pCQueueFile->FileType == FTYPE_UNSUPPORTED && !bDumpGangPrints)
      return SYN_ERRMSG_UNSUPPORTEDFORMAT;

   // in the rip
   PDOC->InRip = TRUE;
   // set element pointer
   if (pPaneElement) pPaneElement->InRip = TRUE;
  
   // set screen saver
   SetScreenSaver(FALSE);

   // stop server polling    
   StopServerPolling();

   // set the scratch path for the rip
   cr.csScratchPath = PDOC->crrrs.csScratchPath;

   // open the rip object
   cr.Open();

   // copy our resource settings into the device
   pLocalDevice->pDevInfo->MemUsed = QUEUEDEVICE.pDevInfo->MemUsed;
   pLocalDevice->pDevInfo->MemUsedIdx = QUEUEDEVICE.pDevInfo->MemUsedIdx;

   // reset rip times array   
   CTimeSpan* ctsp;
   for (int i = pCQueueFile->cdwaPageTimes.GetSize() - 1; i >= 0; i--)
	  {
	  // delete it from the array		 
	  ctsp = (CTimeSpan*)pCQueueFile->cdwaPageTimes.GetAt(i);
	  delete ctsp;
	  }  
   // remove the pointers
   pCQueueFile->cdwaPageTimes.RemoveAll();   

   pLocalDevice->csFriendlyName = PDOC->csTitle;

   // init RIP   
   ret = cr.Init(pLocalDevice, pLocalTransform, pLocalColor, pCQueueFile,
      OutputType, this, (CMainFrame*)(AfxGetApp()->m_pMainWnd), FALSE, bDumpGangPrints);	  

   LastPageTime = CTime::GetCurrentTime();
      
   // start RIP  
   if (ret == SYN_ERRMSG_ALLOK)
	   ret = cr.StartRip();   
   
   // tell everyone we're processing
   if (ret == SYN_ERRMSG_ALLOK)
      {            
      if (OutputType == RIPSTYLE_PREVIEW || OutputType == RIPSTYLE_HD_PREVIEW)      
         {
         pCQueueFile->PreviewStatus.LoadString(IDS_SYN_PSSTAT_PROCESSING);                  
         }
      else
         {   
		 if ( !bDumpGangPrints )
		 {
			 pCQueueChild->StartTrayControl();
			 pCQueueFile->Status.LoadString(IDS_SYN_PSSTAT_PROCESSING);
		 }
         }
      }
   
   // update data in prefs dlg   
   //pJopProps->m_dialog_general.UpdateData(FALSE);	

   // check file type
   if (pCQueueFile->FileType == FTYPE_UNKNOWN  && bDumpGangPrints)
   {
      return SYN_ERRMSG_BADFORMAT;
   }

   if ( pCQueueFile->FileType == FTYPE_UNSUPPORTED  && bDumpGangPrints)
   {
       return SYN_ERRMSG_UNSUPPORTEDFORMAT;
   }

   return ret;   
   }  

LRESULT CSpecialTree::OnRipComplete(WPARAM wParam, LPARAM lParam)
   {     
   int i, j;
   int nItems;
   int RipReturn;
   int iPaneJobCount = 0;
   int iGangPrintEntries = 0;

   CQueueFile* pCQueueFile; 
   CString csStatus;      
   CTimeSpan ctsElapsedTime;
   ASSERT(pActiveCQueueFile);

#if 0
   if (OutputType != RIPSTYLE_PREVIEW &&
		 OutputType != RIPSTYLE_HD_PREVIEW )
	{
	   iPaneJobCount = GetCount();
	   iGangPrintEntries = pPrintPaneState->GetGangPrintEntries();
   }
#endif

   // check for remaining gang prints if there is only one job
   // left in pane OR if we have met our gang print limit
   if (OutputType != RIPSTYLE_PREVIEW &&
		 OutputType != RIPSTYLE_HD_PREVIEW &&
	  pPrintPaneState->GetGangPrintEntries() > 0  && GetCount() == 1)
	  {	
	  // there are gang print entries and this is the last file
	  if (pPrintPaneState->GetGangPrintEntries() > 0)
		 {

	     // dump the remaining gang prints		 
		 RipReturn = SendFileToRIP(pActiveCQueueFile, TRUE);

		 // look up return                                                         
//		 cr.MapErrorReturn(RipReturn, pActiveCQueueFile->Status);                      	  
/*
			 // save elapsed time
			 pActiveCQueueFile->ctsElapsedTime = FileElapsedTime;
			 pCQueueChild->SetTimeString(csStatus, QUEUESTATUS_DOTIME_PRINT, TRUE);
			 pCQueueChild->StopTrayControl();
*/
/*
		 if ( RipReturn != SYN_ERRMSG_ALLOK )
		 {
			 pActiveCQueueFile->StatusFlag = FILE_STATUS_NOTOK;
		 }
		 else
		 {
			 pActiveCQueueFile->StatusFlag = FILE_STATUS_OK;
		 }

		 pActiveCQueueFile->StatusFlag |= FILE_STATUS_FINALGANG;

		 // our doc is modified
		 PDOC->SetModifiedFlag();

		 // redraw item      
		 GetListCtrl().Update(pActiveCQueueFile->QPos);

		 pCQueueChild->PSOut((BYTE*)(const char*)pActiveCQueueFile->Status, 
														pActiveCQueueFile->Status.GetLength());
		 // and add to log file
		 AddToLog(pActiveCQueueFile);
*/
		 return 1L;
		 }
	  else
		 {
		 // now clean up all the gang print files if they exist
		 if ((i = pPrintPaneState->GetGangPrintEntries()) > 0)
			{
			// now delete the entries 			
			for (j = 0; j < i; j++)
			   pPrintPaneState->DeleteGangPrintEntry(0);
			}
		 // and make sure we zero out the print pane entries
		 pPrintPaneState->SetPrintPaneEntries(0);
		 }										   		 
	  }

   // set completion flag  
   RipComplete = TRUE; 

   // load return status, wParam has image result                
   if (OutputType == RIPSTYLE_PREVIEW || OutputType == RIPSTYLE_HD_PREVIEW)           
      {
      // look up return                                                         
      cr.MapErrorReturn(wParam, pActiveCQueueFile->PreviewStatus);      
      // add time
      pActiveCQueueFile->PreviewStatus += FileElapsedTime.Format(" (%H:%M:%S)");
	  pCQueueChild->SetTimeString(csStatus /* unused */, QUEUESTATUS_DOTIME_HOLD, TRUE);
      }
   else
      {          
	      // look up return                                                         
		  cr.MapErrorReturn(wParam, pActiveCQueueFile->Status);                      	  
		  // save elapsed time
		  pActiveCQueueFile->ctsElapsedTime = FileElapsedTime;
		  pCQueueChild->SetTimeString(csStatus, QUEUESTATUS_DOTIME_PRINT, TRUE);
		  pCQueueChild->StopTrayControl();
      }

   // we know that it is now complete
   pActiveCQueueFile->IsComplete = TRUE;
      
	if (wParam == 0) 
	{      
		if (OutputType == RIPSTYLE_PREVIEW || OutputType == RIPSTYLE_HD_PREVIEW)
		{
			pActiveCQueueFile->StatusFlag -= FILE_STATUS_INRIP;
		}
		else
		{
			if ( (pActiveCQueueFile->StatusFlag & FILE_STATUS_FINALGANG) != FILE_STATUS_FINALGANG )
			{
				pActiveCQueueFile->StatusFlag = FILE_STATUS_OK;		 
			}
			else
			{
				pActiveCQueueFile->StatusFlag -= FILE_STATUS_FINALGANG;
			}
		}
	}
	else
	{	  
		if (OutputType == RIPSTYLE_PREVIEW || OutputType == RIPSTYLE_HD_PREVIEW)
		{
			pActiveCQueueFile->StatusFlag -= FILE_STATUS_INRIP;
		}
		else
		{
			if ( (pActiveCQueueFile->StatusFlag & FILE_STATUS_FINALGANG) != FILE_STATUS_FINALGANG )
			{
				pActiveCQueueFile->StatusFlag = FILE_STATUS_NOTOK;
			}
			else
			{
				pActiveCQueueFile->StatusFlag -= FILE_STATUS_FINALGANG;
			}
		}

		// need to force this because if we cancel preview
		// using the preview pause box, the flag doesn't get 
		// set here
		if (wParam == SYN_ERRMSG_CANCELALL)
			CancelFlag = TRUE;      
	}

   // our doc is modified
   PDOC->SetModifiedFlag();

   // redraw item      
   GetListCtrl().Update(pActiveCQueueFile->QPos);
   
   // nuke the prompts   
   ResetPrompts();

   // output result to PS window
   if (OutputType == RIPSTYLE_PREVIEW || OutputType == RIPSTYLE_HD_PREVIEW)           
      pCQueueChild->PSOut((BYTE*)(const char*)pActiveCQueueFile->PreviewStatus, 
	     pActiveCQueueFile->PreviewStatus.GetLength());
   else
	  {
      pCQueueChild->PSOut((BYTE*)(const char*)pActiveCQueueFile->Status, 
	     pActiveCQueueFile->Status.GetLength());
      // and add to log file
      AddToLog(pActiveCQueueFile);
	  }
   
   char CRbuf[4];
   strcpy(CRbuf, "\r\n");
   pCQueueChild->PSOut((BYTE*)CRbuf, 2);

   // if printed, move to archive
   if (iViewStyle == QUEUESTYLE_PRINT)
	  {	  
	  // make sure the numbering is straight
	  RenumberList();

	  // add it to archive	  
	  if (pCQueueChild)		 
		 {
		 ADDTOPANESTRUCT AddToPaneStruct;
		 AddToPaneStruct.iViewStyle = QUEUESTYLE_ARCHIVE;	  
         ::SendMessage(pCQueueChild->pArchiveWnd->GetSafeHwnd(), 
		    wm_AddToPane, (WPARAM)&AddToPaneStruct, (LPARAM)pActiveCQueueFile);   	  		 
		 }

	  // save these
	  csStatus = pActiveCQueueFile->Status; 
	  ctsElapsedTime = pActiveCQueueFile->ctsElapsedTime;

  	  // delete the item from the list
      DeleteItem(GetItemAt(pActiveCQueueFile->QPos));
	  
	  // delete from the array
      pQueueArray->RemoveAt(pActiveCQueueFile->QPos);

      // delete the CQueue object
      delete pActiveCQueueFile;

	  // make sure the numbering is straight
	  RenumberList();

	  // Decrease(1) to the Count of Print Pane Entries
	  if ( pPrintPaneState->GetPrintPaneEntries() > 0 )
		  pPrintPaneState->DecrPrintPaneEntries();

	  // force a reshow
	  ReMeasureAllItems();
	  Invalidate();

	  // if we cancelled, move all the rest of the files to the 
	  // archive queue with the cancel att set
	  if (CancelFlag)
		 {
		 nItems = GetCount();		 
         // get the selected items
         for (i = 0; i < nItems; i++)
			{                               
	        // get the NEXT item
	        // note: we always get the 0 based item because we are removing
	        // them from the top down
	        pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(0));  
			// all are cancelled
			pCQueueFile->Status = csStatus;
			pCQueueFile->ctsElapsedTime = ctsElapsedTime;
			pCQueueFile->StatusFlag = FILE_STATUS_NOTOK;
	        // add to archive queue
			if (pCQueueChild)
			   {
			   ADDTOPANESTRUCT AddToPaneStruct;
			   AddToPaneStruct.iViewStyle = QUEUESTYLE_ARCHIVE;	  
	           ::SendMessage(pCQueueChild->pArchiveWnd->GetSafeHwnd(), 
		          wm_AddToPane, (WPARAM)&AddToPaneStruct, (LPARAM)pCQueueFile);   	  			   
			   }
            // and add to log file
            AddToLog(pCQueueFile);
  	        // delete the item from the list
            DeleteItem(GetItemAt(0));
	        // delete from the array
            pQueueArray->RemoveAt(0);
			// and finally, delete the queue file
			delete pCQueueFile;
			}
 	     // make sure the numbering is straight
	     RenumberList();			 
	     // force a reshow
	     ReMeasureAllItems();
		 Invalidate();
		 
		 // our doc is modified   
         PDOC->SetModifiedFlag();
         // if the info box is on, update it
         UpdateInfo();			  
#if 0
		 // now clean up all the gang print files if they exist
	     if ((i = pPrintPaneState->GetGangPrintEntries()) > 0)
			{
			// now delete the entries 			
			for (j = 0; j < i; j++)
			   pPrintPaneState->DeleteGangPrintEntry(0);
			}
		 // and make sure we zero out the print pane entries
		 pPrintPaneState->SetPrintPaneEntries(0);
#endif
		 }	  		 
#if 0
	  else
		 {
		 // if we have no more print pane entries, make sure that we've
		 // cleaned up all gang print files
		 if (pPrintPaneState->GetPrintPaneEntries() == 0)
			{
		    if ((i = pPrintPaneState->GetGangPrintEntries()) > 0)
			   {
			   // now delete the entries 			
			   for (j = 0; j < i; j++)
				  pPrintPaneState->DeleteGangPrintEntry(0);
			   }
			}
		 }
#endif
	  }
   return 1L;
   }

LRESULT CSpecialTree::OnRipMessage(WPARAM WParam, LPARAM LParam)
   {                       
   PRIPMSGSTRUCT pMsgStruct;      
   
   // cast the LParam to a ripmsg pointer
   pMsgStruct = (PRIPMSGSTRUCT)LParam;

   // do message stuff based on msg type
   switch(pMsgStruct->MsgType)													 
      {
      case RIPMSG_READING:           
		   pCQueueChild->SetProcessStatus(StatusPromptID, StatusProgressID, STATUS_READING, pMsgStruct->PercentComplete);
           break;
      case RIPMSG_READING_TOTAL:           
		   pCQueueChild->SetProcessStatus(StatusPromptID, StatusProgressID, STATUS_READING_TOTAL, pMsgStruct->TotalComplete);
           break;
      case RIPMSG_SCANLINE:   
           pCQueueChild->SetProcessStatus(StatusPromptID, StatusProgressID, STATUS_SCANLINE, pMsgStruct->LineOn, 
		      pMsgStruct->LineTotal);           
           break;
      case RIPMSG_PRINTING:
		   pCQueueChild->SetProcessStatus(StatusPromptID, StatusProgressID, STATUS_PRINTING, pMsgStruct->LineOn, 
		      pMsgStruct->LineTotal, pMsgStruct->ColorOn);
           break;
      case RIPMSG_INFO:
		   pCQueueChild->SetStatus1(StatusTextID, pMsgStruct->Info);
           break;
      case RIPMSG_RENDERTITLE:
           pCQueueChild->SetProcessStatus(StatusPromptID, StatusProgressID, STATUS_RENDERING);
		   break;
      case RIPMSG_INFO2:
           // do the prompt		   
		   pCQueueChild->SetProcessStatus(StatusPromptID, StatusProgressID, pMsgStruct->Info);
           break;
      case RIPMSG_PRINTING_TP:
		   pCQueueChild->SetProcessStatus(StatusPromptID, StatusProgressID, STATUS_PRINTING_TP, pMsgStruct->LineOn, 
		      pMsgStruct->LineTotal, pMsgStruct->ColorOn, pMsgStruct->TotalComplete);
           break;
	  case RIPMSG_EJECT_PAGE:
		   // get elapsed time
		   CTimeSpan* ctsp = new CTimeSpan;
		   *ctsp = CTime::GetCurrentTime() - LastPageTime;
		   LastPageTime = CTime::GetCurrentTime();
		   // add to cqueue array of processing times		   
		   pActiveCQueueFile->cdwaPageTimes.Add((CObject*)ctsp);		   
		   break;
      }
   // tell the caller if we want to pause
   return (LRESULT)PauseFlag;
   }      

LRESULT CSpecialTree::OnRipPSMessage(WPARAM WParam, LPARAM LParam)
   {                       
   pCQueueChild->PSOut((BYTE*)LParam, (int)WParam);
   return 0L;
   }

void CSpecialTree::ResetPrompts()
   {
   pCQueueChild->ClearStatus(StatusTextID, StatusPromptID, StatusProgressID);
   //UpdateData(FALSE);
   }	

void CSpecialTree::UpdateInfo()
   {
   CString cs;
   CDWordArray* pArrSelected;
   CQueueFile* pCQueueFile;
   CDevice* pLocalDevice;
   CTransform* pLocalTransform;
   CColorTransform* pLocalColor;   
   CString csTitle;

   //We're not doing default props so let's flag it so
   DoingDefaultProps = FALSE;
   // get selection array
   pArrSelected = GetSelectionArray();   

   // how many are selected?
   int NumSelected = pArrSelected->GetSize();   

   // if we have no items, hide it
   if (NumSelected < 1) 
      {                 
	  // store the last page
      m_pPropFrame->m_pModelessPropSheet->StoreOurActivePage();
      if (m_pPropFrame) m_pPropFrame->ShowWindow(SW_HIDE);
      return;
      }        

   // get the first entrant
   pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(pArrSelected->GetAt(0)));

   if (NumSelected == 1)
	  {
	  // tell the preview machinery what file we're on
	  if (iViewStyle == QUEUESTYLE_HOLD)
	  {
		  if (RipComplete)
		  {
			((CMainFrame*)(AfxGetApp()->m_pMainWnd))->Preview.SetPreviewActiveJob(pCQueueFile->iJobID);
		  }
	  }
	  csTitle.Format("%s Properties", pCQueueFile->Description);

      // set proper devmode      
      if ((pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS) && pCQueueFile->pCD){
         pLocalDevice = pCQueueFile->pCD;
		 //Set flag so that we won't create a new object on a change.
		 m_pPropFrame->m_pModelessPropSheet->m_general.bIsUsingDefTarg = FALSE;
	  }
      else{
         pLocalDevice = &QUEUEDEVICE;
		 m_pPropFrame->m_pModelessPropSheet->m_general.bIsUsingDefTarg = TRUE;
	  }
   
	  //Make sure that the Devinfo structure is synched correctly
//	  pLocalDevice->SetDevInfo(pLocalDevice->pDevInfo, FALSE);

      // set proper transform
      if ((pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS) && pCQueueFile->pTransform)
         pLocalTransform = pCQueueFile->pTransform;
      else
         pLocalTransform = &QUEUETRANSFORM;

      // set proper transform
      if ((pCQueueFile->Prefs & USE_LOCAL_COL_PREFS) && pCQueueFile->pCC)
         pLocalColor = pCQueueFile->pCC;
      else
         pLocalColor = &QUEUECOLOR;		  	
	  }
   else
	  {
	  csTitle.LoadString(IDS_MULTIPLE_SELECTION);
	  pLocalDevice = &QUEUEDEVICE;
	  pLocalTransform = &QUEUETRANSFORM;
	  pLocalColor = &QUEUECOLOR;				 
	  }
   
   // load the title
   m_pPropFrame->SetWindowText(csTitle);   

   pCPInitStruct = &(pLocalColor->CPInitStruct);
   strncpy(pCPInitStruct->Printer_Name,pLocalDevice->Name,MAX_PATH);
   pCPInitStruct->GlobalPrinterID = pLocalDevice->DevIdx; 
   pCPInitStruct->Printer_CSpace = pLocalDevice->pDevInfo->iColorSpace; 	  

   // set current queue file
   m_pPropFrame->m_pModelessPropSheet->m_general.pTargetCQueue = 
	  pCQueueFile;

   // set device
   m_pPropFrame->m_pModelessPropSheet->m_general.pTargetDevice = pLocalDevice;	  
   //Set flag so that we will create a new object on a change.
   m_pPropFrame->m_pModelessPropSheet->m_general.bIsUsingDefTarg = TRUE;
   m_pPropFrame->m_pModelessPropSheet->m_general.pReportWnd = this;
   
   if (NumSelected > 1)
	  {
	  cs.LoadString(IDS_MULTIPLE_SELECTION);
	  m_pPropFrame->m_pModelessPropSheet->m_general.m_filename = cs;
	  cs.LoadString(IDS_NOT_AVAIL);            	      
	  m_pPropFrame->m_pModelessPropSheet->m_general.m_filesize =       
	  m_pPropFrame->m_pModelessPropSheet->m_general.m_filepages = 
	  m_pPropFrame->m_pModelessPropSheet->m_general.m_filedate = 
	  m_pPropFrame->m_pModelessPropSheet->m_general.m_filetime =
	  m_pPropFrame->m_pModelessPropSheet->m_general.m_filetype = 
	  m_pPropFrame->m_pModelessPropSheet->m_general.m_boundingbox = 
	  m_pPropFrame->m_pModelessPropSheet->m_general.m_prange_edit = cs;      
	  m_pPropFrame->m_pModelessPropSheet->m_general.m_prange = FALSE;
	  }
   
   // color profile stuff
   m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.pReportWnd = this;
   m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.SetInitStruct(pCPInitStruct);

   // color adjust stuff
   m_pPropFrame->m_pModelessPropSheet->m_coloradjustdialog.pReportWnd = this;
   m_pPropFrame->m_pModelessPropSheet->m_coloradjustdialog.SetInitStruct(pCPInitStruct);

   // set fields with multi-select indeterminence
   if (NumSelected > 1)
      SetMultiSelFields(pArrSelected);	  
   else
	  {
      // sizing stuff
	  m_pPropFrame->m_pModelessPropSheet->m_size.pTargetCQueue = pCQueueFile;
	  m_pPropFrame->m_pModelessPropSheet->m_size.pTargetDevice = pLocalDevice;
	  m_pPropFrame->m_pModelessPropSheet->m_size.csScratchPath = 
		 PDOC->crrrs.csScratchPath;
	  m_pPropFrame->m_pModelessPropSheet->m_size.pLocalColor = pLocalColor; 
	  m_pPropFrame->m_pModelessPropSheet->m_size.SetTransform(pLocalTransform);
	  m_pPropFrame->m_pModelessPropSheet->m_size.pReportWnd = this;	  
	  // show the job id
//	  TRACE("UpdateInfo, iJobID = %d\r\n", pCQueueFile->iJobID);
	  }

   // show it
   if(m_pPropFrame->m_pModelessPropSheet->m_general.IsInit)
	  m_pPropFrame->m_pModelessPropSheet->m_general.UpdateIt(NumSelected > 1);
   m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.UpdateIt(NumSelected > 1);
   m_pPropFrame->m_pModelessPropSheet->m_coloradjustdialog.UpdateIt(NumSelected > 1);
   m_pPropFrame->m_pModelessPropSheet->m_size.UpdateIt(NumSelected > 1);
   
   m_pPropFrame->m_pModelessPropSheet->m_device.pReportWnd = this;
   CDevice* pTempDevice = m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice;
   m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice = &QUEUEDEVICE;	  
//   if(m_pPropFrame->m_pModelessPropSheet->m_device.GetSafeHwnd())				  
//	  m_pPropFrame->m_pModelessPropSheet->m_device.ResetWindow(pTempDevice, &QUEUEDEVICE);      

   m_pPropFrame->m_pModelessPropSheet->m_device.ResetWindow(pTempDevice, &QUEUEDEVICE,
								m_pPropFrame->m_pModelessPropSheet->m_device.GetSafeHwnd());
   
   //Make sure that the Devinfo structure is synched correctly
   pLocalDevice->SetDevInfo(pLocalDevice->pDevInfo, FALSE);

   UpdateDeviceDlg();

   m_pPropFrame->m_pModelessPropSheet->m_device.ResetWindow(pTempDevice, &QUEUEDEVICE,
								m_pPropFrame->m_pModelessPropSheet->m_device.GetSafeHwnd());

   }

void CSpecialTree::CreateJobPropsDlg()
   {
   CProfile Profile;
   int x, y;								 

   // if the instance counter is zero, init the class 
   // and create the window                      
   if (*pInstanceCntr == 0)
      {	 	  
	  if (m_pPropFrame == NULL)
		 {
		 m_pPropFrame = new CPropertyFrame;

		 CRect rect(0, 0, 0, 0);
		 CString strTitle;
		 VERIFY(strTitle.LoadString(IDS_PROPSHT_CAPTION));		 

		 // create the property frame		 		 
		 if (!m_pPropFrame->Create(NULL, strTitle,
		    WS_POPUP | WS_CAPTION | WS_SYSMENU, rect, AfxGetMainWnd()))						
			{
			delete m_pPropFrame;
			m_pPropFrame = NULL;
			return;
			}

		 // get last window pos
		 x = Profile.GetInt(IDS_GENERALSECTION, "JobPropsPosX", -1);
		 y = Profile.GetInt(IDS_GENERALSECTION, "JobPropsPosY", -1);

		 // sanity check
         int screenWidth = GetSystemMetrics(SM_CXSCREEN);
         int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		 // position the window
		 // if no known position, or a bad position, put in lower right
		 if (x == -1 || y == -1 || x > screenWidth || y > screenHeight)					 
		    m_pPropFrame->PositionWindow();			
		 else
		    m_pPropFrame->SetWindowPos(&wndTop,			
	           x, y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE);

         // store pointer in main frame
         ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_pPropFrame = m_pPropFrame;		 

		 // set the deletion pointer
		 m_pPropFrame->m_pModelessPropSheet->pbDeleted = &m_pPropFrame->bDeleted;
		 }
	  }
   (*pInstanceCntr)++;   
   }

void CSpecialTree::DestroyJobPropsDlg()
   {
   CProfile Profile;

   // accounts for inexplicable crashing when we try to destroy it while
   // it's showing
   if (m_pPropFrame) 
	  {
	  if (m_pPropFrame->GetSafeHwnd())
	     m_pPropFrame->ShowWindow(SW_HIDE);
	  }

   (*pInstanceCntr)--;

   if (*pInstanceCntr == 0)
	  {
	  if (m_pPropFrame->GetSafeHwnd())
		 {
		 // save the last position
		 RECT rc;
		 m_pPropFrame->GetWindowRect(&rc);
		 Profile.WriteInt(IDS_GENERALSECTION, "JobPropsPosX", rc.left);
		 // make sure that the top of JobPropsPosY is on the screen
		 Profile.WriteInt(IDS_GENERALSECTION, "JobPropsPosY", max(rc.top, 0));
		 // kill it
		 m_pPropFrame->DestroyWindow();
	     //delete m_pPropFrame;	  
		 //m_pPropFrame = NULL;
		 ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_pPropFrame = m_pPropFrame = NULL;
		 }
	  }
   }

void CSpecialTree::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   // update the jobinfo dialog
   UpdateInfo();   

   // reset the focus
   // THIS SOLVES THE 'D' random appearance problem
   // The JobProps box got the focus and keystrokes would go to 
   // the default control, which may have been a combo box.  The keystroke
   // would be fed to the combo box and the resulting change would get
   // fed back to the Queue.....
   //SetFocus();
//   TRACE("SpecialTree: OnItemchanged: SetFocus\r\n");
   }

void CSpecialTree::SetState(int State)
   {                  
   InternalState = State;

   // special if no rip is loaded
   //if (!cr.RipLoaded) State = STATE_IDLE_RIP_NOTAVAIL;
      
   switch(State)
      {
	  case STATE_IDLE_RIP_NOTAVAIL:
      case STATE_IDLE_NO_FILES:                      
           CmdCancelActive = FALSE;
           CmdAddFiles = TRUE;
		   CmdQueuePreferences = TRUE;
           CmdSkip = FALSE;
           CmdPause = FALSE;
           CmdResume = FALSE;
           CmdFilePrint = FALSE;
           CmdFilePrintSel = FALSE;
           CmdFilePrintPreview = FALSE;
           CmdFilePrintPreviewSel = FALSE;
           CmdFilePrintHDPreview = FALSE;
           CmdFilePrintHDPreviewSel = FALSE;
           CmdSelectAll = FALSE;
           CmdDelete = FALSE;
           break;
      case STATE_IDLE_SOME_FILES:         
           CmdCancelActive = FALSE;
           CmdAddFiles = TRUE;           
		   CmdQueuePreferences = TRUE;           
           CmdSkip = FALSE;
           CmdPause = FALSE;
           CmdResume = FALSE;
           CmdFilePrint = TRUE;
           CmdFilePrintSel = TRUE;
           CmdFilePrintPreview = TRUE;
           CmdFilePrintPreviewSel = TRUE;
           CmdFilePrintHDPreview = TRUE;
           CmdFilePrintHDPreviewSel = TRUE;
           CmdSelectAll = TRUE;
           CmdDelete = TRUE;
           break;
      case STATE_PREVIEWING:      
           CmdCancelActive = TRUE;
           CmdAddFiles = FALSE;
		   CmdQueuePreferences = FALSE;
           CmdSkip = TRUE;
           CmdPause = TRUE;
           CmdResume = FALSE;
           CmdFilePrint = FALSE;
           CmdFilePrintSel = FALSE;
           CmdFilePrintPreview = FALSE;
           CmdFilePrintPreviewSel = FALSE;
           CmdFilePrintHDPreview = FALSE;
           CmdFilePrintHDPreviewSel = FALSE;
           CmdSelectAll = FALSE;
           CmdDelete = FALSE;
           break;
      case STATE_PRINTING:      
           //CmdCancelActive = FALSE;
		   CmdCancelActive = TRUE;
           CmdAddFiles = FALSE;
		   CmdQueuePreferences = FALSE;
           CmdSkip = TRUE;
           CmdPause = TRUE;
           CmdResume = FALSE;
           CmdFilePrint = FALSE;
           CmdFilePrintSel = FALSE;
           CmdFilePrintPreview = FALSE;
           CmdFilePrintPreviewSel = FALSE;
           CmdFilePrintHDPreview = FALSE;
           CmdFilePrintHDPreviewSel = FALSE;
           CmdSelectAll = FALSE;
           CmdDelete = FALSE;
           break;
      case STATE_PAUSED:
           CmdCancelActive = FALSE;
           CmdAddFiles = TRUE;
		   CmdQueuePreferences = TRUE;
           CmdSkip = FALSE;
           CmdPause = FALSE;
           CmdResume = TRUE;
           CmdFilePrint = FALSE;
           CmdFilePrintSel = FALSE;
           CmdFilePrintPreview = FALSE;
           CmdFilePrintPreviewSel = FALSE;
           CmdFilePrintHDPreview = FALSE;
           CmdFilePrintHDPreviewSel = FALSE;
           CmdSelectAll = TRUE;
           CmdDelete = TRUE;
		   break;
      }     
   }   

void CSpecialTree::OnContinuePrinting()
   {   
   CQueueFile* pCQueueFile;   
   int RipReturn;
   int i;
   
   // check to see if rip is busy
   if (!RipComplete) 
      {
	  // do timer every second
	  if ((GetTickCount() - LastTimeCheck) > 1)
	     {
		 LastTimeCheck = GetTickCount();
         // get the file elapsed time
         FileElapsedTime = CTime::GetCurrentTime() - RipStartTime;   

         // format the time string
         TimeString = FileElapsedTime.Format("%H:%M:%S / ");   
         // get the queue elapsed time
         PDOC->QueueElapsedTime = CTime::GetCurrentTime() - 
           PDOC->QueueStartTime;   
         // add the new time string
         TimeString += PDOC->QueueElapsedTime.Format("%H:%M:%S");            	  
		 if (iViewStyle == QUEUESTYLE_HOLD)
	        pCQueueChild->SetTimeString(TimeString, QUEUESTATUS_DOTIME_HOLD);		 
		 else
			pCQueueChild->SetTimeString(TimeString, QUEUESTATUS_DOTIME_PRINT);

         // get the queue elapsed time
         PDOC->QueueElapsedTime = CTime::GetCurrentTime() - 
           PDOC->QueueStartTime;   
		 }
	  return;
      }

   // nothing in the queue
   pActiveCQueueFile = NULL;   
   
   // if CancelFlag is not set, look for next available file in list
   if (!CancelFlag)
      {      
      for (i = 0; i < GetCount(); i++)      
         {
	     pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(i));
		 // found one!
		 if (!pCQueueFile->WasTouched)
		    {
			// if selected or if QueueType == QUEUETYPE_ALL, make it active		    
			if (IsSelected(i) || QueueType == QUEUETYPE_ALL)
			   {
	           pActiveCQueueFile = pCQueueFile;

			   pActiveCQueueFile->QPos = i;
			   break;
			   }
			}
	     }
	  }
   
   // is an item available?
   if (pActiveCQueueFile)
      {                                        
      // make sure it's visible      
	  //SetFirstVisible(GetItemAt(pActiveCQueueFile->QPos));
	  GetListCtrl().EnsureVisible(pActiveCQueueFile->QPos, FALSE);

	  // we had our chance...
	  pActiveCQueueFile->WasTouched = TRUE;
#if 0
 	  // check for delete status, if deleted, beep and return
	  // note that we put this after wastouched so we don't 
	  // do it again      
	  if ((pCQueueFile->StatusFlag & FILE_STATUS_CHECK) == FILE_STATUS_OK_DELETED)
	     {
		 ::MessageBeep(MB_ICONEXCLAMATION);
         return;
		 }
#endif
      // set flag  
      RipComplete = FALSE;
      
      // set status
      //pActiveCQueueFile->StatusFlag = FILE_STATUS_INRIP;      
	  pActiveCQueueFile->StatusFlag |= FILE_STATUS_INRIP;      

	  // redraw it	  
	  GetListCtrl().Update(pActiveCQueueFile->QPos);
      
      // close the rip object
      if (cr.IsInit) 
         {
         cr.Close();         

		 // set queue status
	     pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_IDLE);
	     iLastQueueStatus = IDS_QUEUESTATUS_IDLE;
	
		 // out of the rip
		 PDOC->InRip = FALSE;
         // set element pointer
         if (pPaneElement) pPaneElement->InRip = FALSE;
		 
		 // set screen saver
		 SetScreenSaver(TRUE);

		 // start potentially polling
		 StartServerPolling();
         }
	  // set queue status
	  if ((OutputType == RIPSTYLE_OUTPUT) && 
		  (QueueType == QUEUETYPE_SELECTED))
		 {
	     pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_PRINT_SELECT);
		 iLastQueueStatus = IDS_QUEUESTATUS_PRINT_SELECT;
		 }
	  else if ((OutputType == RIPSTYLE_OUTPUT) && 
		  (QueueType == QUEUETYPE_ALL))
		 {
	     pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_PRINT_ALL);
		 iLastQueueStatus = IDS_QUEUESTATUS_PRINT_ALL;
		 }
	  else if ((OutputType == RIPSTYLE_PREVIEW || OutputType == RIPSTYLE_HD_PREVIEW) && 
		  (QueueType == QUEUETYPE_SELECTED))
		 {
	     pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_PREVIEW_SELECT);
		 iLastQueueStatus = IDS_QUEUESTATUS_PREVIEW_SELECT;
		 }
	  else if ((OutputType == RIPSTYLE_PREVIEW || OutputType == RIPSTYLE_HD_PREVIEW) && 
		  (QueueType == QUEUETYPE_ALL))
		 {
	     pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_PREVIEW_ALL);
		 iLastQueueStatus = IDS_QUEUESTATUS_PREVIEW_ALL;
		 }

      // get start time
      RipStartTime = CTime::GetCurrentTime();      
      
      // set buttons
	  if (OutputType == RIPSTYLE_PREVIEW || OutputType == RIPSTYLE_HD_PREVIEW)
         SetState(STATE_PREVIEWING);
	  else
		 SetState(STATE_PRINTING);

      // send file to RIP
      if ((RipReturn = SendFileToRIP(pActiveCQueueFile)) != SYN_ERRMSG_ALLOK)
         {                                       
		 // make sure that we have the elapsed time
         FileElapsedTime = CTime::GetCurrentTime() - RipStartTime;   
      
         // process return         
         OnRipComplete(RipReturn, 0L);

		 // problem! Cancel the rest!!
		 if ( RipReturn == SYN_ERRMSG_RIPNOTFOUND ||
				RipReturn == SYN_ERRMSG_RIPDLLNOTFOUND ||
					RipReturn == SYN_ERRMSG_RIPTHRDBAD ||
						RipReturn == SYN_ERRMSG_DEVINIT )
			KillTimer(RP95_PRINT_TIMER_ID);

		 // set queue status
	     pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_IDLE);		 
		 iLastQueueStatus = IDS_QUEUESTATUS_IDLE;

         // close the rip object
         if (GetCount() <= 0)
		 {
			 if (cr.IsInit) 
				{
				cr.Close();                     

				// set queue status
				pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_IDLE);
				iLastQueueStatus = IDS_QUEUESTATUS_IDLE;

				// out of the rip
				PDOC->InRip = FALSE;
				// set element pointer
				if (pPaneElement) pPaneElement->InRip = FALSE;

				// set screen saver
				SetScreenSaver(TRUE);

				// start potentially polling
				StartServerPolling();
				}
		 }
         // set buttons
         if (GetCount() > 0)
		    {
			SetState(STATE_IDLE_SOME_FILES);
			}
         else        
		    {                       
            SetState(STATE_IDLE_NO_FILES);   
			}
         }         
      // no elapsed time
      FileElapsedTime = 0;                  
      }   
   else
      {                
      // write to the log
	  if (OutputType != RIPSTYLE_PREVIEW && OutputType != RIPSTYLE_HD_PREVIEW)
		 {
         CString csTimeString;
	     if (PDOC->QueueElapsedTime.GetHours() > 0)
            csTimeString = PDOC->QueueElapsedTime.Format("Total Queue Print Time: %H hours %M minutes %S seconds");   
	     else if (PDOC->QueueElapsedTime.GetMinutes() > 0)
            csTimeString = PDOC->QueueElapsedTime.Format("Total Queue Print Time: %M minutes %S seconds");   
	     else
            csTimeString = PDOC->QueueElapsedTime.Format("Total Queue Print Time: %S seconds");   

		 PDOC->cLog.AddText(csTimeString);
	     if (PDOC->LogWnd.m_hWnd)
		     WriteLog();
		 bSentStartToLog = FALSE;
		 }

      KillTimer(RP95_PRINT_TIMER_ID);

	  // set queue status
	  pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_IDLE);		 
	  iLastQueueStatus = IDS_QUEUESTATUS_IDLE;

      // close the rip object
      if (cr.IsInit)
         {
         cr.Close();         

 		 // set queue status
	     pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_IDLE);
		 iLastQueueStatus = IDS_QUEUESTATUS_IDLE;

		 // out of the rip
		 PDOC->InRip = FALSE;         
         // set element pointer
         if (pPaneElement) pPaneElement->InRip = FALSE;

		 // set screen saver
		 SetScreenSaver(TRUE);

		 // start potentially polling
		 StartServerPolling();
         }

      // nuke the prompts	  
      ResetPrompts();

	  // set buttons
      if (GetCount() > 0)
	     {
		 SetState(STATE_IDLE_SOME_FILES);
		 }
      else
	     {
	     SetState(STATE_IDLE_NO_FILES);  
		 }
      }      
   // if the info box is on, update it
   UpdateInfo();               
   }

void CSpecialTree::OnTimer(UINT nIDEvent) 
   {
   // CListView::OnTimer nukes our timer! (bug in VC 4.0?)
   // so only go to the base class impl. if not our timer
   if (nIDEvent == RP95_PRINT_TIMER_ID)
      {
	  // call print continue proc
      OnContinuePrinting();    	  
	  }
   else if (nIDEvent == RP95_SERVER_TIMER_ID)
      {
	  // process polling timer tick
	  ProcessServerPolling();
	  }
   else	      
      CRPTreeBase::OnTimer(nIDEvent);
   }

void CSpecialTree::RestartServer()
   {
   // only do this for print queue
   if (iViewStyle != QUEUESTYLE_PRINT) return;
   
   // if so, see if there are files an start rip
   if (InternalState == STATE_IDLE_SOME_FILES)
      {	  
      // rip ALL
      QueueType = QUEUETYPE_ALL;
   
      // do it
      Print();				
	  }    
   }

int CSpecialTree::MapStatusImageState(CQueueFile* pCQueueFile)
   {
   int iRet = 0;

   if ((pCQueueFile->StatusFlag & FILE_STATUS_INRIP) == FILE_STATUS_INRIP)	  		 
      iRet = 1;	  
   else
	  {
      switch (pCQueueFile->StatusFlag & FILE_STATUS_CHECK)
		 {
         case FILE_STATUS_HOLD:
	 	      iRet = 0;
		      break;
         case FILE_STATUS_OK:
	          iRet = 2;
		      break;
         case FILE_STATUS_NOTOK:
  		      iRet = 3;
		      break;
         case FILE_STATUS_OK_DELETED:
	          iRet = 4;
	          break;
	     default:
	          iRet = 0;
	          break;		
		 }
	  }
   return iRet;
   }

LRESULT CSpecialTree::OnStartPolling(WPARAM wParam, LPARAM lParam){
	StartServerPolling((BOOL)lParam);
	return 0L;
}

LRESULT CSpecialTree::OnStopPolling(WPARAM wParam, LPARAM lParam){
	StopServerPolling();
	return 0L;
}

LRESULT CSpecialTree::OnAddToPane(WPARAM wParam, LPARAM lParam)
   {
   CQueueFile* pCQueueFile;
   CQueueFile* pCQueueFileToDelete;
   int idx;
   ADDTOPANESTRUCT AddToPaneStruct;

   // copy over the struct
   memcpy(&AddToPaneStruct, (void*)wParam, sizeof ADDTOPANESTRUCT);

   if (AddToPaneStruct.iViewStyle == QUEUESTYLE_ARCHIVE)
	  {
	  // this should always be true
	  ASSERT (iViewStyle == QUEUESTYLE_ARCHIVE);

      // instantiate a new queue item
      if (!(pCQueueFile = new CQueueFile))
		  return 1L;

      // copy the queue item
      *pCQueueFile = *((CQueueFile*)lParam);

      // if no local prefs, lock them in now   
      if (!(pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS))
		 {
		 pCQueueFile->pCD = new CDevice;
		 *(pCQueueFile->pCD) = QUEUEDEVICE;
		 pCQueueFile->Prefs |= USE_LOCAL_DEV_PREFS;        
		 }
	  if (!(pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS))
		 {
		 pCQueueFile->pTransform = new CTransform;
		 *(pCQueueFile->pTransform) = QUEUETRANSFORM;
		 pCQueueFile->Prefs |= USE_LOCAL_TRN_PREFS;        
		 }
	  if (!(pCQueueFile->Prefs & USE_LOCAL_COL_PREFS))
		 {
		 pCQueueFile->pCC = new CColorTransform;
		 *(pCQueueFile->pCC) = QUEUECOLOR;
		 pCQueueFile->Prefs |= USE_LOCAL_COL_PREFS;        
		 }

	  // if imaging was OK, and this is a server file,
	  // then make a delete decision...
	  if (((pCQueueFile->StatusFlag & FILE_STATUS_CHECK) == FILE_STATUS_OK) &&		  
		   (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_SERVERFILE))
		 {
		 if (PDOC->crrrs.iDiskFilesResourceSelector == DISKFILES_DELETE_AFTER_IMAGING)
			{
			// nuke it if it's the last file		    
			if ( ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->DeleteLastServerFile(pCQueueFile) )
			{
				// change the status
				pCQueueFile->StatusFlag |= FILE_STATUS_OK_DELETED;
				// add deleted prompt
				CString cs;
				cs.LoadString(IDS_DELETED_ADDENDUM);
				pCQueueFile->Status += cs;         
			}
			}
		 }	 
  	  // if imaging was OK, and this is a server file,
	  // then make a delete decision...
	  else if (((pCQueueFile->StatusFlag & FILE_STATUS_CHECK) == FILE_STATUS_OK) &&		  
		   (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_POLLEDFILE))
		 {
		 if (PDOC->crrrs.iHotFolderResourceSelector == HOTFOLDER_DELETE_AFTER_IMAGING)
			{
			// nuke it
		    if ( ::DeleteFile(pCQueueFile->LongName) )
				{
				// tell the poller about it
				PDOC->crrrs.DeleteFromCheckedFilesArray(pCQueueFile->LongName);   
				// change the status
				pCQueueFile->StatusFlag |= FILE_STATUS_OK_DELETED;
				// add deleted prompt
				CString cs;
				cs.LoadString(IDS_DELETED_ADDENDUM);
				pCQueueFile->Status += cs;
				}
			}
		 }	 						 
	  else if (((pCQueueFile->StatusFlag & FILE_STATUS_CHECK) == FILE_STATUS_OK) &&		  
		   (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_INDEXFILE))
	  {

		  // nuke it
		  if ( ::DeleteFile(pCQueueFile->LongName) )
		  {
			// tell the poller about it
			PDOC->crrrs.DeleteFromCheckedFilesArray(pCQueueFile->LongName);   
			// change the status
			pCQueueFile->StatusFlag |= FILE_STATUS_OK_DELETED;
			// add deleted prompt
			CString cs;
			cs.LoadString(IDS_DELETED_ADDENDUM);
			pCQueueFile->Status += cs;
		  }
	  }

	  // have we hit our archive max?
	  if (PDOC->crrrs.iListJobsResourceSelector == LISTJOBS_KEEP_MAX)
		 {
		  int iArrayCount = GetCount();

		 // do we have more than enough files?
		 // if so, delete the last one
		 if (iArrayCount >= PDOC->crrrs.iListJobsMax)
			{	  
		    // get the item		       
			pCQueueFileToDelete = (CQueueFile*)GetItemData(GetItemAt(iArrayCount - 1));
            // is this a server file?
			if (pCQueueFileToDelete->ExtraInfo & CQUEUE_EXTRA_SERVERFILE)
			   {
			   // nuke it
			   ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->DeleteLastServerFile(pCQueueFileToDelete);
			   }
            // is this a polled file?
			if (pCQueueFileToDelete->ExtraInfo & CQUEUE_EXTRA_POLLEDFILE && 
			   PDOC->crrrs.iHotFolderResourceSelector != HOTFOLDER_NEVER_DELETE)
			   {			   
			   // nuke it
			   ::DeleteFile(pCQueueFileToDelete->LongName);	  
			   }

		    // delete the queue file
		    delete pCQueueFileToDelete;
		    pCQueueFileToDelete = NULL;

		    // delete the item from the list
            DeleteItem(GetItemAt(iArrayCount - 1));
		    // delete from the array
            pQueueArray->RemoveAt(iArrayCount - 1);			
			}
		 }	 
   
      PDOC->CQ_Archive.InsertAt(0, pCQueueFile);	             
      InsertString(0, pCQueueFile);   

      // make sure it's visible
      GetListCtrl().Update(0);	     
      Invalidate();

      // if the info box is on, update it
      UpdateInfo();

      // renumber the list
      RenumberList();  
	  // force a reshow
	  ReMeasureAllItems();
	  Invalidate();
	  }
   else if (AddToPaneStruct.iViewStyle == QUEUESTYLE_PRINT)
	  {
	  // this should always be true
	  ASSERT (iViewStyle == QUEUESTYLE_PRINT);

      idx = GetCount();	  

      // instantiate a new queue item
      if (!(pCQueueFile = new CQueueFile))
		  return 1L;

      // copy the queue item
      *pCQueueFile = *((CQueueFile*)lParam);

	  // mark it as untouched
	  pCQueueFile->WasTouched = FALSE;
	  
	  // set these
	  pCQueueFile->bDoSubPage = AddToPaneStruct.bDoSubPage;
	  pCQueueFile->iSubPage = AddToPaneStruct.iSubPage;
   
      // insert it into the print array
      PDOC->CQ.InsertAt(idx, pCQueueFile);	             
      InsertString(idx, pCQueueFile);   

      // make sure it's visible
      GetListCtrl().Update(idx);
	  Invalidate();         

      // reset state if we are currently idle
      if (InternalState == STATE_IDLE_NO_FILES) SetState(STATE_IDLE_SOME_FILES);
      // our doc is modified
      PDOC->SetModifiedFlag();

	  // Add(1) to the Count of Print Pane Entries
	  pPrintPaneState->IncrPrintPaneEntries();

      // if the info box is on, update it
      UpdateInfo();			  
      // renumber the list
      RenumberList();  
	  // force a reshow
	  ReMeasureAllItems();
	  Invalidate();
      // start server if needed 
      if (!bPrintServerStarted)		  
		 StartServer();
	  else
         // restart server
         RestartServer();
	  }
/*
#ifdef _PHZ_PRINT_SERVER
   else if (AddToPaneStruct.iViewStyle == QUEUESTYLE_HOLD)
	  {
	  // this should always be true
	  ASSERT (iViewStyle == QUEUESTYLE_HOLD);

      idx = GetCount();	  

      // instantiate a new queue item
      if (!(pCQueueFile = new CQueueFile))	return 1L;

      // copy the queue item
      *pCQueueFile = *((CQueueFile*)lParam);

	  // mark it as untouched
	  pCQueueFile->WasTouched = FALSE;
	  
	  // set these
	  pCQueueFile->bDoSubPage = AddToPaneStruct.bDoSubPage;
	  pCQueueFile->iSubPage = AddToPaneStruct.iSubPage;
   
      // insert it into the print array
      PDOC->CQ_Hold.InsertAt(idx, pCQueueFile);	             
      InsertString(idx, pCQueueFile);   

      // make sure it's visible
      GetListCtrl().Update(idx);
	  Invalidate();         

      // reset state if we are currently idle
      if (InternalState == STATE_IDLE_NO_FILES)
		  SetState(STATE_IDLE_SOME_FILES);

      // our doc is modified
      PDOC->SetModifiedFlag();

      // if the info box is on, update it
      UpdateInfo();
	  
      // renumber the list
      RenumberList();  
	  // force a reshow
	  ReMeasureAllItems();

	  Invalidate();

      // start server if needed 
      if (!bPrintServerStarted)		  
		 StartServer();
	  else
         // restart server
         RestartServer();
	  }
#endif
*/
   return 0L;
   }

LRESULT CSpecialTree::OnDevInfoChange(WPARAM WParam, LPARAM LParam){
	/*
	*	When a notification of a change in the device info structure occurs
	*	we will need to get the DevInfo Structure for each job selected
	*	and pass them to the device as an array of PDEVINFO*'s
	*/
	PDEVINFO* pData = NULL;
	CQueueFile* pCQueueFile;
	CDWordArray *pArrSelected = NULL;
	DWORD dwIndex;
	int nArrIdx, nArraySize;
	
	//Get selection array
    pArrSelected = GetSelectionArray();

	//Are we doing default properties or job properties?
	if(DoingDefaultProps && m_pModalPropFrame)
	{
		pData = new PDEVINFO[1];
		pData[0] = m_pModalPropFrame->m_device.pTargetDevice->pDevInfo;
		m_pModalPropFrame->m_device.pTargetDevice->UpdateDevInfo(pData, 1, (int) LParam);
	}
	else if(m_pPropFrame)
	{
		//Allocate space for the pData array
		pData = new PDEVINFO[pArrSelected->GetSize()];
		nArraySize = 0;
		//Walk through and gather all of the selected DEVINFO structs
		for(nArrIdx = pArrSelected->GetUpperBound(); nArrIdx >= 0; --nArrIdx)
		{		
			//Get the index
			dwIndex = pArrSelected->GetAt(nArrIdx);
			//Get the item
			pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(pArrSelected->GetAt(nArrIdx)));

			if((pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS) && pCQueueFile->pCD)
			{
				pData[nArraySize++] = pCQueueFile->pCD->pDevInfo;
			}
			else
			{
				/*	
				*	We're currently using the QUEUEDEVICE DevInfo struct.
				*	We must now use our own
				*/	
				
			  if ( pCQueueFile->pCD )
			  {
				  csRPDebugStr.Format("DevInfoChange New Device %s\n", "Should we delete old device");
			  }
			  else
			  {
				  csRPDebugStr.Format("DevInfoChange New pData %s\n", "OK Old Device is NULL");
			  }

				//Make new devmode
				pCQueueFile->pCD = new CDevice;
				//Store devmode
				//This does the instantiation
				*pCQueueFile->pCD = *m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice;
				//memcpy(pCQueueFile->pCD, m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice->pDevInfo, sizeof CDevice);
				//Change status
				pCQueueFile->Prefs |= USE_LOCAL_DEV_PREFS;
				//Now add to our array
				pData[nArraySize++] = pCQueueFile->pCD->pDevInfo;
			}
		}

		m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice->UpdateDevInfo(pData, nArraySize, (int) LParam);

		PDOC->SetModifiedFlag();

		if ( nArrIdx >= 0 )
		{
			GetListCtrl().Update(pCQueueFile->QPos);
		}
	}

	//Clean up and bail
	if(pData)
		delete pData;

	return 0L;
}

/*
*	OnDevModeChangeDefault() is called whenever a change has occured in the QUEUE properties dialog
*	so that all other tabs within the QUEUE properties dialog can update thier respective devInfo 
*	Structures accordingly.
*/
LRESULT CSpecialTree::OnDevModeChangeDefault(WPARAM WParam, LPARAM LParam)
   {
   PDEVCHANGE pDevChange = (PDEVCHANGE)LParam;

   if (WParam & PREFS_CHANGE_DEVICE)
	  {
	  if (!DoingDefaultProps && m_pPropFrame)
		 {
		 // make sure that this is our device
		 if (m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice &&
			m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice->DevIdx == 
			pDevChange->pCD->DevIdx) 
		    m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice->SetDevInfo(pDevChange->pCD->pDevInfo, FALSE);
		    // make sure the media is correct
		    if ( m_pPropFrame->m_pModelessPropSheet->m_general.pTargetDevice )
				m_pPropFrame->m_pModelessPropSheet->m_general.pTargetDevice->SetDevInfo(pDevChange->pCD->pDevInfo, FALSE);
		    m_pPropFrame->m_pModelessPropSheet->m_general.UpdateMediaDims();

			if (m_pPropFrame->m_pModelessPropSheet->m_size.pTargetDevice)
			   {
			   if (m_pPropFrame->m_pModelessPropSheet->m_size.pTargetDevice->pDevInfo)
				  {
			      m_pPropFrame->m_pModelessPropSheet->m_size.pTargetDevice->SetDevInfo(pDevChange->pCD->pDevInfo, FALSE);
			      m_pPropFrame->m_pModelessPropSheet->m_size.UpdateIt();		  
				  }
			   }
		 }
      else if(m_pModalPropFrame)
		 {
	     if (m_pModalPropFrame->m_device.pTargetDevice &&
			m_pModalPropFrame->m_device.pTargetDevice->DevIdx == 
			pDevChange->pCD->DevIdx) 				 
	        m_pModalPropFrame->m_device.pTargetDevice->SetDevInfo(pDevChange->pCD->pDevInfo, TRUE);
		    // make sure the media is correct
		    m_pModalPropFrame->m_general.pTargetDevice->SetDevInfo(pDevChange->pCD->pDevInfo, TRUE);
		    m_pModalPropFrame->m_general.UpdateMediaDims();
			if (m_pModalPropFrame->m_size.pTargetDevice)
			   {
			   if (m_pModalPropFrame->m_size.pTargetDevice->pDevInfo)
				  {
	              m_pModalPropFrame->m_size.pTargetDevice->SetDevInfo(pDevChange->pCD->pDevInfo, TRUE);
	              m_pModalPropFrame->m_size.UpdateIt();		  
				  }
			   }
		 }
	  }
   return 0L;
   }

LRESULT CSpecialTree::OnDevModeChange(WPARAM WParam, LPARAM LParam)
   {                       
   CQueueFile* pCQueueFile;                  
   CDWordArray *pArrSelected = NULL;
   CDevice* pPassedDevice;
   CTransform* pPassedTransform;   
   PCPINITSTRUCT pPassedInitStruct;   
   int NumSelected;
   int nArrIdx;
   DWORD dwIndex;
   PDEVCHANGE pDevChange;
   PCMMCHANGE pCMMChange;
   PXFORMCHANGE pXformChange;

   if (WParam & PREFS_CHANGE_DESCRIPTION)
	  {	
      // get passed dev change structure
	  pDevChange = (PDEVCHANGE)LParam;

      // get selection array
      pArrSelected = GetSelectionArray();
	  // how many are selected?
      NumSelected = pArrSelected->GetSize();   
      // get the selected items
      for (nArrIdx = pArrSelected->GetUpperBound(); nArrIdx >= 0; --nArrIdx)
         {                               
		 // get the index
		 dwIndex = pArrSelected->GetAt(nArrIdx);
		 // get the item
		 pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(dwIndex));
		 // just changing the description!
		 UpdateSpecificDeviceItem(NULL, pCQueueFile, pDevChange);
         // redraw it            
		 GetListCtrl().Update(pCQueueFile->QPos);
		 PDOC->SetModifiedFlag();
		 }
	  }
   else if (WParam & PREFS_CHANGE_PRANGE)
	  {
      // get passed dev change structure
	  pDevChange = (PDEVCHANGE)LParam;

      // get selection array
      pArrSelected = GetSelectionArray();
	  // how many are selected?
      NumSelected = pArrSelected->GetSize();   
      // get the selected items
      for (nArrIdx = pArrSelected->GetUpperBound(); nArrIdx >= 0; --nArrIdx)
         {                               
		 // get the index
		 dwIndex = pArrSelected->GetAt(nArrIdx);
		 // get the item
		 pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(dwIndex));
		 // just changing the pint range
		 UpdateSpecificDeviceItem(NULL, pCQueueFile, pDevChange);
         // redraw it            
		 GetListCtrl().Update(pCQueueFile->QPos);
		 PDOC->SetModifiedFlag();
		 }
	  }
   else if (WParam & PREFS_CHANGE_TRANSFORM)
      {                        
      // get passed dev change structure
	  pXformChange = (PXFORMCHANGE)LParam;	  

      // get passed transform
      pPassedTransform = pXformChange->pCT;

      // get selection array
      pArrSelected = GetSelectionArray();
	  // how many are selected?
      NumSelected = pArrSelected->GetSize();   
      // get the selected items
      for (nArrIdx = pArrSelected->GetUpperBound(); nArrIdx >= 0; --nArrIdx)
         {                               
		 // get the index
		 dwIndex = pArrSelected->GetAt(nArrIdx);
		 // get the item
		 pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(dwIndex));
         // do we have a transform now?
         if (pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS)
            {                
            // if it's the same, then do nothing
            if (*(pCQueueFile->pTransform) == *(pPassedTransform))
               continue;                                     
            else
               {                                                        
               // if its the same as global reset the prefs
               if (*(pPassedTransform) == QUEUETRANSFORM)
                  {                                              
			      // delete the existing transform
                  delete pCQueueFile->pTransform;            
                  pCQueueFile->pTransform = NULL; 
                  if (pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS)
                     pCQueueFile->Prefs -= USE_LOCAL_TRN_PREFS;
                  }
               else   
                  {                              
				  // overwrite transform			      
			      UpdateSpecificXFormItem(pPassedTransform, pCQueueFile->pTransform, 
					 pXformChange->ItemChanged);			   

                  // set status based on transform
                  pCQueueFile->Prefs |= USE_LOCAL_TRN_PREFS;
                  }
			   PDOC->SetModifiedFlag();
               }
            }   
         else
            {            
            // no transform, so let's compare it to global
            // if the same, do nothing
            if (*(pPassedTransform) == QUEUETRANSFORM)
               {               
               if (pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS)
                  pCQueueFile->Prefs -= USE_LOCAL_TRN_PREFS;            
               continue;                  
               }
            else   
               {               
               // make new transform
               pCQueueFile->pTransform = new CTransform;
               // store transform
               //*(pCQueueFile->pTransform) = *(pPassedTransform);
		       
			   // this does the instantiation			   
			   *(pCQueueFile->pTransform) = QUEUETRANSFORM;			   
			   // now update the changed items
			   UpdateSpecificXFormItem(pPassedTransform, pCQueueFile->pTransform, 
			      pXformChange->ItemChanged);			   

               // change status 
               pCQueueFile->Prefs |= USE_LOCAL_TRN_PREFS;        
               }
            }                           
         // redraw it            
		 GetListCtrl().Update(pCQueueFile->QPos);
		 PDOC->SetModifiedFlag();
         }     
      }
   else if (WParam & PREFS_CHANGE_DEVICE)
      {            
      // get passed dev change structure
	  pDevChange = (PDEVCHANGE)LParam;
	  
	  // get passed device
  	  pPassedDevice = pDevChange->pCD;

      // get selection array
      pArrSelected = GetSelectionArray();
	  // how many are selected?
      NumSelected = pArrSelected->GetSize();   
      // get the selected items
      for (nArrIdx = pArrSelected->GetUpperBound(); nArrIdx >= 0; --nArrIdx)
         {                               
		 // get the index
		 dwIndex = pArrSelected->GetAt(nArrIdx);
		 // get the item
		 pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(dwIndex));
         // do we have a devmode now?         
		 if (pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS)
            {                
            // if it's the same, then do nothing
            if (*(pCQueueFile->pCD) == *(pPassedDevice))
               continue;                                     
            else
               {                                                        
               // if its the same as global reset the prefs
               if (*(pPassedDevice) == QUEUEDEVICE)                  
                  {                                              
				  // delete the existing devmode
                  delete pCQueueFile->pCD;               
                  pCQueueFile->pCD = NULL; 
                  if (pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS)
                     pCQueueFile->Prefs -= USE_LOCAL_DEV_PREFS;
				  // and reset these!
				  m_pPropFrame->m_pModelessPropSheet->m_general.pTargetDevice = &QUEUEDEVICE;
				  m_pPropFrame->m_pModelessPropSheet->m_general.bIsUsingDefTarg = TRUE;
				  m_pPropFrame->m_pModelessPropSheet->m_size.pTargetDevice = &QUEUEDEVICE;
			  	  m_pPropFrame->m_pModelessPropSheet->m_size.csScratchPath = 
					 PDOC->crrrs.csScratchPath;
				  m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice = &QUEUEDEVICE;
                  }
               else   
                  {                                      
                  // overwrite devmode
                  //*(pCQueueFile->pCD) = *(pPassedDevice);
				  UpdateSpecificDeviceItem(pPassedDevice, pCQueueFile, 
					 pDevChange);
				  // set status based on devmode
                  pCQueueFile->Prefs |= USE_LOCAL_DEV_PREFS;
                  }
			   PDOC->SetModifiedFlag();
               }
            }   
         else
            {            
            // no devmode, so let's compare it to global
            // if the same, do nothing
            if (*(pPassedDevice) == QUEUEDEVICE)
               {
               if (pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS)
                  pCQueueFile->Prefs -= USE_LOCAL_DEV_PREFS;            
               continue;                  
               }
            else   
               {               
               // make new devmode
               pCQueueFile->pCD = new CDevice;
               // store devmode
               //*(pCQueueFile->pCD) = *(pPassedDevice);
			   // this does the instantiation			   
			   *(pCQueueFile->pCD) = QUEUEDEVICE;			   
			   // now update the changed items
			   UpdateSpecificDeviceItem(pPassedDevice, pCQueueFile, pDevChange);			   
               // change status 
               pCQueueFile->Prefs |= USE_LOCAL_DEV_PREFS;        
			   // and point other prop pages to this device
			   m_pPropFrame->m_pModelessPropSheet->m_general.pTargetDevice = pCQueueFile->pCD;
			   m_pPropFrame->m_pModelessPropSheet->m_general.bIsUsingDefTarg = TRUE;
			   m_pPropFrame->m_pModelessPropSheet->m_size.pTargetDevice = pCQueueFile->pCD;
		   	   m_pPropFrame->m_pModelessPropSheet->m_size.csScratchPath = 
				  PDOC->crrrs.csScratchPath;
			   m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice = pCQueueFile->pCD;

			   m_pPropFrame->m_pModelessPropSheet->m_general.pTargetDevice->SetDevInfo(pCQueueFile->pCD->pDevInfo, FALSE);
			   //m_pPropFrame->m_pModelessPropSheet->m_general.pTargetDevice->GetDevInfo();
  			   m_pPropFrame->m_pModelessPropSheet->m_size.pTargetDevice->SetDevInfo(pCQueueFile->pCD->pDevInfo, FALSE);
			   //m_pPropFrame->m_pModelessPropSheet->m_size.pTargetDevice->GetDevInfo();
			   m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice->SetDevInfo(pCQueueFile->pCD->pDevInfo, FALSE);
			   //m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice->GetDevInfo();
               }
			PDOC->SetModifiedFlag();
            }                           
         // redraw it            
		 GetListCtrl().Update(pCQueueFile->QPos);
         }     				  
      }
   else if (WParam & PREFS_CHANGE_COLOR)
      {            
      // get passed CMM initstruct
	  pCMMChange = (PCMMCHANGE)LParam;
	  pPassedInitStruct = (PCPINITSTRUCT)pCMMChange->pChangedStruct;
	  int ChangedThing = pCMMChange->ItemChanged;

      // get selection array
      pArrSelected = GetSelectionArray();
	  // how many are selected?
      NumSelected = pArrSelected->GetSize();   
      // get the selected items
      for (nArrIdx = pArrSelected->GetUpperBound(); nArrIdx >= 0; --nArrIdx)
         {                               
		 // get the index
		 dwIndex = pArrSelected->GetAt(nArrIdx);
		 // get the item
		 pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(dwIndex));
         // do we have color prefs now
         if (pCQueueFile->Prefs & USE_LOCAL_COL_PREFS)
            {                
            // if it's the same, then do nothing
            if ((pCQueueFile->pCC->IsSame(*(pPassedInitStruct))))
               continue;                                     
            else
               {                                                        
               // if its the same as global reset the prefs
				if ((QUEUECOLOR.IsSame(*(pPassedInitStruct))))
                  {                                              
				  // delete the existing devmode
                  delete pCQueueFile->pCC;               
                  pCQueueFile->pCC = NULL; 
                  if (pCQueueFile->Prefs & USE_LOCAL_COL_PREFS)
                     pCQueueFile->Prefs -= USE_LOCAL_COL_PREFS;
				  // and reset this!				  
				  m_pPropFrame->m_pModelessPropSheet->m_size.pLocalColor = &QUEUECOLOR;				  
                  }
               else   
                  {                                      
                  // overwrite devmode
                  //*(pCQueueFile->pCD) = *(pPassedDevice);
				  UpdateSpecificColorItem(pPassedInitStruct, &pCQueueFile->pCC->CPInitStruct, pCMMChange->ItemChanged);
				  // set status based on devmode
                  pCQueueFile->Prefs |= USE_LOCAL_COL_PREFS;
                  }
			   PDOC->SetModifiedFlag();
               }
            }   
         else
            {            
            // no devmode, so let's compare it to global
            // if the same, do nothing
            if ((QUEUECOLOR.IsSame(*(pPassedInitStruct))))
               {               
               if (pCQueueFile->Prefs & USE_LOCAL_COL_PREFS)
                  pCQueueFile->Prefs -= USE_LOCAL_COL_PREFS;            
               continue;                  
               }
            else   
               {               
               // make new devmode
               pCQueueFile->pCC = new CColorTransform;
               // store devmode
               //*(pCQueueFile->pCD) = *(pPassedDevice);			   

			   // this does the instantiation			   
			   *(pCQueueFile->pCC) = QUEUECOLOR;			   
			   // now update the changed items
				  UpdateSpecificColorItem(pPassedInitStruct, &pCQueueFile->pCC->CPInitStruct, pCMMChange->ItemChanged);
               // change status 
               pCQueueFile->Prefs |= USE_LOCAL_COL_PREFS;        
               }
			PDOC->SetModifiedFlag();
            }                           
         // redraw it            
		 GetListCtrl().Update(pCQueueFile->QPos);
         }     				  
      } 
   return 0L;
   }

void CSpecialTree::OnSize(UINT nType, int cx, int cy) 
   {
   CRPTreeBase::OnSize(nType, cx, cy);

   // we want to save the size stuff in the doc so say that 
   // the document is modified so that a save will be forced	
   PDOC->SetModifiedFlag();
   }

void CSpecialTree::UpdateDeviceDlg(){
	PDEVINFO* pData = NULL;
	CQueueFile* pCQueueFile;
	CDWordArray *pArrSelected = NULL;
	DWORD dwIndex;
	int nArrIdx, nArraySize;

	if(!DoingDefaultProps && m_pPropFrame){
		//Get selection array
		pArrSelected = GetSelectionArray();
		//Find out how many are selected
		pData = new PDEVINFO[pArrSelected->GetSize()];
		nArraySize = 0;

		for(nArrIdx = pArrSelected->GetUpperBound(); nArrIdx >= 0; --nArrIdx){		
			//Get the index
			dwIndex = pArrSelected->GetAt(nArrIdx);
			//Get the item
			pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(pArrSelected->GetAt(nArrIdx)));

			if(pCQueueFile->pCD){
				if(pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS)
					pData[nArraySize++] = pCQueueFile->pCD->pDevInfo;
				else{
					pData[nArraySize++] = m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice->pDevInfo;
				}
			}
			else{
				pData[nArraySize++] = m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice->pDevInfo;
			}
		}
		//Call device.dll callback function
		m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice->UpdateDlgFromQueue(pData, nArraySize);
	}
	else if(m_pModalPropFrame){
		pData = new PDEVINFO[1];
		pData[0] = m_pModalPropFrame->m_device.pTargetDevice->pDevInfo;
		m_pModalPropFrame->m_device.pTargetDevice->UpdateDlgFromQueue(pData, 1);
	}

	//Clean up and bail
	if(pData)
		delete pData;
}

void CSpecialTree::SetMultiSelFields(CDWordArray *pArrSelected)
   {
   CQueueFile* pCQueueFileFirst;
   CQueueFile* pCQueueFile;
   CDevice* pFirstDevice;
   CDevice* pThisDevice;
   CColorTransform* pFirstColor;
   CColorTransform* pThisColor;
   CTransform* pFirstTransform;
   CTransform* pThisTransform;   
   DWORD dwIndex;	  
   int nArrIdx;
   BOOL bIsSame = TRUE;
   BOOL bBreakOut = FALSE;

   // get the first queue item
   pCQueueFileFirst = (CQueueFile*)GetItemData(GetItemAt(pArrSelected->GetAt(0)));

   // get the device
   if ((pCQueueFileFirst->Prefs & USE_LOCAL_DEV_PREFS) && pCQueueFileFirst->pCD)
      pFirstDevice = pCQueueFileFirst->pCD;
   else
      pFirstDevice = &QUEUEDEVICE; 

   // set proper transform
   if ((pCQueueFileFirst->Prefs & USE_LOCAL_TRN_PREFS) && pCQueueFileFirst->pTransform)
      pFirstTransform = pCQueueFileFirst->pTransform;
   else
      pFirstTransform = &QUEUETRANSFORM;

   // set proper color transform
   if ((pCQueueFileFirst->Prefs & USE_LOCAL_COL_PREFS) && pCQueueFileFirst->pCC)
      pFirstColor = pCQueueFileFirst->pCC;
   else
      pFirstColor = &QUEUECOLOR;

   pCPInitStruct = &(pFirstColor->CPInitStruct);
   strncpy(pCPInitStruct->Printer_Name,pFirstDevice->Name,MAX_PATH);
   pCPInitStruct->GlobalPrinterID = pFirstDevice->DevIdx; 
   pCPInitStruct->Printer_CSpace = pFirstDevice->pDevInfo->iColorSpace;    

   // device defaults
   m_pPropFrame->m_pModelessPropSheet->m_general.m_resolution = 
	  pFirstDevice->pDevInfo->CurrentResIdx;
   m_pPropFrame->m_pModelessPropSheet->m_general.m_copies = 
      pFirstDevice->pDevInfo->Copies;
   m_pPropFrame->m_pModelessPropSheet->m_general.m_scalebitmap = 
      pFirstDevice->pDevInfo->ScaleImage;
   // this will always be the same for each device
   m_pPropFrame->m_pModelessPropSheet->m_general.m_canscale_bitmap = 
	  pFirstDevice->pDevInfo->CanScaleImage;
   // this will always be the same for each device
   m_pPropFrame->m_pModelessPropSheet->m_general.m_cancompress = 
	  pFirstDevice->pDevInfo->CanDoCompression;
   // this will always be the same for each device
   m_pPropFrame->m_pModelessPropSheet->m_general.m_canaalias = 
	  pFirstDevice->pDevInfo->CanDoAntiAliasing;
   m_pPropFrame->m_pModelessPropSheet->m_general.m_aatype = 
	  (pFirstDevice->pDevInfo->AliasStyle == ANTIALIAS_STYLE_BOX);
   m_pPropFrame->m_pModelessPropSheet->m_general.m_aaoversample = 
	  max((int)pFirstDevice->pDevInfo->AliasLevel - 2, 0);
   m_pPropFrame->m_pModelessPropSheet->m_general.m_buffertodisk = 
	  pFirstDevice->pDevInfo->BufferToDisk;
   m_pPropFrame->m_pModelessPropSheet->m_general.m_bitmapsampling = 
	  pFirstDevice->pDevInfo->BitmapSample;
   m_pPropFrame->m_pModelessPropSheet->m_general.m_compress = 
	  pFirstDevice->pDevInfo->UseCompression;
   m_pPropFrame->m_pModelessPropSheet->m_general.m_compressbitmap = 
	  pFirstDevice->pDevInfo->UseCompressionBmp;   
   m_pPropFrame->m_pModelessPropSheet->m_general.m_mediasize = 
	  pFirstDevice->pDevInfo->CurrentMediaIdx;

   // sizing defaults
   m_pPropFrame->m_pModelessPropSheet->m_size.pTargetCQueue = 
	  pCQueueFileFirst;
   m_pPropFrame->m_pModelessPropSheet->m_size.pTargetDevice = 
	  pFirstDevice;
   m_pPropFrame->m_pModelessPropSheet->m_size.csScratchPath = 
	  PDOC->crrrs.csScratchPath;

   m_pPropFrame->m_pModelessPropSheet->m_size.SetTransform(pFirstTransform);	  
   m_pPropFrame->m_pModelessPropSheet->m_size.pReportWnd = 
	  this;
   m_pPropFrame->m_pModelessPropSheet->m_size.m_xzoom_is_indeterminate = FALSE;
   m_pPropFrame->m_pModelessPropSheet->m_size.m_yzoom_is_indeterminate = FALSE;
   m_pPropFrame->m_pModelessPropSheet->m_size.m_xoff_is_indeterminate = FALSE;
   m_pPropFrame->m_pModelessPropSheet->m_size.m_yoff_is_indeterminate = FALSE;
   m_pPropFrame->m_pModelessPropSheet->m_size.m_rotation_is_indeterminate = FALSE;

   //color defaults
   m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.SetInitStruct(&pFirstColor->CPInitStruct);
   m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.UpdateIt();
   m_pPropFrame->m_pModelessPropSheet->m_coloradjustdialog.SetInitStruct(&pFirstColor->CPInitStruct);
   m_pPropFrame->m_pModelessPropSheet->m_coloradjustdialog.UpdateIt();

   // go through all the files except the first
   for (nArrIdx = pArrSelected->GetUpperBound(); nArrIdx > 0; --nArrIdx)
	  {
	  // get the index
 	  dwIndex = pArrSelected->GetAt(nArrIdx);
	  // get the item
	  pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(dwIndex));
      // get the device
      if ((pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS) && pCQueueFile->pCD)
         pThisDevice = pCQueueFile->pCD;
      else
         pThisDevice = &QUEUEDEVICE; 
      // get the transform
      if ((pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS) && pCQueueFile->pTransform)
         pThisTransform = pCQueueFile->pTransform;
      else
         pThisTransform = &QUEUETRANSFORM; 

      // get the transform
      if ((pCQueueFile->Prefs & USE_LOCAL_COL_PREFS) && pCQueueFile->pCC)
         pThisColor = pCQueueFile->pCC;
      else
         pThisColor = &QUEUECOLOR; 

	  // resolution is different
	  if (pFirstDevice->pDevInfo->CurrentResIdx != pThisDevice->pDevInfo->CurrentResIdx)
		 m_pPropFrame->m_pModelessPropSheet->m_general.m_resolution = -1;

	  // copies are different
	  if (pFirstDevice->pDevInfo->Copies != pThisDevice->pDevInfo->Copies)
	     m_pPropFrame->m_pModelessPropSheet->m_general.m_copies = -1;

	  // scale bitmap is different
	  // set indeterminate
	  if (pFirstDevice->pDevInfo->ScaleImage != pThisDevice->pDevInfo->ScaleImage)
	     m_pPropFrame->m_pModelessPropSheet->m_general.m_scalebitmap = 2;

	  // AliasStyle is different
	  if (pFirstDevice->pDevInfo->AliasStyle != pThisDevice->pDevInfo->AliasStyle)
         m_pPropFrame->m_pModelessPropSheet->m_general.m_aatype = 2;	     

	  // AliasLevel is different
	  if (pFirstDevice->pDevInfo->AliasLevel != pThisDevice->pDevInfo->AliasLevel)
         m_pPropFrame->m_pModelessPropSheet->m_general.m_aaoversample = -1;	     

	  // BitmapSample is different
	  if (pFirstDevice->pDevInfo->BitmapSample != pThisDevice->pDevInfo->BitmapSample)
         m_pPropFrame->m_pModelessPropSheet->m_general.m_bitmapsampling = -1;	     

	  // BufferToDisk is different
	  // set indeterminate
	  if (pFirstDevice->pDevInfo->BufferToDisk != pThisDevice->pDevInfo->BufferToDisk)
         m_pPropFrame->m_pModelessPropSheet->m_general.m_buffertodisk = 2;

	  // UseCompression is different
	  // set indeterminate
	  if (pFirstDevice->pDevInfo->UseCompression != pThisDevice->pDevInfo->UseCompression)
         m_pPropFrame->m_pModelessPropSheet->m_general.m_compress = 2;

 	  // UseCompressionBmp is different
	  // set indeterminate
	  if (pFirstDevice->pDevInfo->UseCompressionBmp != pThisDevice->pDevInfo->UseCompressionBmp)
         m_pPropFrame->m_pModelessPropSheet->m_general.m_compressbitmap = 2;

 	  // CurrentMediaIdx is different
	  if (pFirstDevice->pDevInfo->CurrentMediaIdx != pThisDevice->pDevInfo->CurrentMediaIdx)
         m_pPropFrame->m_pModelessPropSheet->m_general.m_mediasize = -1;	     
	  
	  //
	  // transform stuff
	  //

	  // Name is different
	  if (pFirstTransform->Name != pThisTransform->Name)
         m_pPropFrame->m_pModelessPropSheet->m_size.m_name = -1;	     

	  // StartType is different
	  if (pFirstTransform->StartType != pThisTransform->StartType)
         m_pPropFrame->m_pModelessPropSheet->m_size.m_start_type = "";	     

	  // Description is different
	  if (pFirstTransform->Description != pThisTransform->Description)
         m_pPropFrame->m_pModelessPropSheet->m_size.m_description = -1;	     

	  // Rotation is different
	  if (pFirstTransform->Rotate != pThisTransform->Rotate)
         m_pPropFrame->m_pModelessPropSheet->m_size.m_rotation_is_indeterminate = TRUE;	     

	  // XZoom is different
	  if (pFirstTransform->XZoom != pThisTransform->XZoom)
         m_pPropFrame->m_pModelessPropSheet->m_size.m_xzoom_is_indeterminate = TRUE;
	  
	  // YZoom is different
	  if (pFirstTransform->YZoom != pThisTransform->YZoom)
         m_pPropFrame->m_pModelessPropSheet->m_size.m_yzoom_is_indeterminate = TRUE;
	  
	  // XOff is different
	  if (pFirstTransform->XOff != pThisTransform->XOff)
         m_pPropFrame->m_pModelessPropSheet->m_size.m_xoff_is_indeterminate = TRUE;
	  
	  // YOff is different
	  if (pFirstTransform->YOff != pThisTransform->YOff)
         m_pPropFrame->m_pModelessPropSheet->m_size.m_yoff_is_indeterminate = TRUE;
	  }

	  //
	  // color stuff
	  //

	  // input profile Name is different
	  if (strcmp(pFirstColor->CPInitStruct.RGB_In_Name,pThisColor->CPInitStruct.RGB_In_Name))
         strncpy(m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.RGB_In_Name,"",MAX_PATH);	     
	  // CMYK_In_Name profile Name is different
	  if (strcmp(pFirstColor->CPInitStruct.CMYK_In_Name,pThisColor->CPInitStruct.CMYK_In_Name))
         strncpy(m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.CMYK_In_Name,"",MAX_PATH);	     
	  // Monitor_Name profile Name is different
	  if (strcmp(pFirstColor->CPInitStruct.Monitor_Name,pThisColor->CPInitStruct.Monitor_Name))
         strncpy(m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.Monitor_Name,"",MAX_PATH);	     
	  // Press_Name profile Name is different
	  if (strcmp(pFirstColor->CPInitStruct.Press_Name,pThisColor->CPInitStruct.Press_Name))
         strncpy(m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.Press_Name,"",MAX_PATH);	     
	  // Printer_Name profile Name is different
	  if (strcmp(pFirstColor->CPInitStruct.Printer_Name,pThisColor->CPInitStruct.Printer_Name))
         strncpy(m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.Printer_Name,"",MAX_PATH);	     
	  // Printer_Profile_Name profile Name is different
	  if (strcmp(pFirstColor->CPInitStruct.Printer_Profile_Name,pThisColor->CPInitStruct.Printer_Profile_Name))
         strncpy(m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.Printer_Profile_Name,"",MAX_PATH);	     

	  if (pFirstColor->CPInitStruct.MaxBlack != pThisColor->CPInitStruct.MaxBlack)
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.MaxBlack = -1;
	  if (pFirstColor->CPInitStruct.MaxInk != pThisColor->CPInitStruct.MaxInk)
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.MaxInk = -1;
	  if (pFirstColor->CPInitStruct.GCR_UCR_Value != pThisColor->CPInitStruct.GCR_UCR_Value)
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.GCR_UCR_Value = -1;
	  if (pFirstColor->CPInitStruct.Brightness != pThisColor->CPInitStruct.Brightness)
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.Brightness = -101;
	  if (pFirstColor->CPInitStruct.Contrast != pThisColor->CPInitStruct.Contrast)
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.Contrast = -101;
	  if (pFirstColor->CPInitStruct.ColorLevels[RED_COLR] != pThisColor->CPInitStruct.ColorLevels[RED_COLR])
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.ColorLevels[RED_COLR] = -101;
	  if (pFirstColor->CPInitStruct.ColorLevels[GREEN_COLR] != pThisColor->CPInitStruct.ColorLevels[GREEN_COLR])
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.ColorLevels[GREEN_COLR] = -101;
	  if (pFirstColor->CPInitStruct.ColorLevels[BLUE_COLR] != pThisColor->CPInitStruct.ColorLevels[BLUE_COLR])
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.ColorLevels[BLUE_COLR] = -101;
	  if (pFirstColor->CPInitStruct.ColorLevels[BLACK_COLR] != pThisColor->CPInitStruct.ColorLevels[BLACK_COLR])
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.ColorLevels[BLACK_COLR] = -101;
	  if (pFirstColor->CPInitStruct.TransType != pThisColor->CPInitStruct.TransType)
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.TransType = -1;
	  if (pFirstColor->CPInitStruct.Printer_CSpace != pThisColor->CPInitStruct.Printer_CSpace)
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.Printer_CSpace = -1;
	  if (pFirstColor->CPInitStruct.Input_CSpace != pThisColor->CPInitStruct.Input_CSpace)
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.Input_CSpace = -1;
	  if (pFirstColor->CPInitStruct.GlobalPrinterID != pThisColor->CPInitStruct.GlobalPrinterID)
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.GlobalPrinterID = -1;
	  if (pFirstColor->CPInitStruct.Preview != pThisColor->CPInitStruct.Preview)
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.Preview = -1;
	  if (pFirstColor->CPInitStruct.Prepress != pThisColor->CPInitStruct.Prepress)
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.Prepress = -1;
	  if (pFirstColor->CPInitStruct.WhiteIsWhite != pThisColor->CPInitStruct.WhiteIsWhite)
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.WhiteIsWhite = -1;
	  if (pFirstColor->CPInitStruct.BlackIsBlack != pThisColor->CPInitStruct.BlackIsBlack)
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.BlackIsBlack = -1;
	  if (pFirstColor->CPInitStruct.PreserveLuminosity != pThisColor->CPInitStruct.PreserveLuminosity)
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.PreserveLuminosity = -1;
	  if (pFirstColor->CPInitStruct.GCR_UCR_Flag != pThisColor->CPInitStruct.GCR_UCR_Flag)
         m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.GCR_UCR_Flag = -1;
		for (int i = 0; i < 7; i++)
		{
		  if (pFirstColor->CPInitStruct.gamma[i] != pThisColor->CPInitStruct.gamma[i])
			 m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct.gamma[i] = -1;
		}
		//make sure both dialogs have the same initstruct
		pFirstColor->CopyInitStruct(&m_pPropFrame->m_pModelessPropSheet->m_coloradjustdialog.LocalInitStruct,
			&m_pPropFrame->m_pModelessPropSheet->m_cmmdialog.LocalInitStruct);
  }

void CSpecialTree::UpdateSpecificDeviceItem(CDevice* cdFrom, CQueueFile* pCQueueFile, 
											DEVCHANGE* pDevChange)
   {
   switch(pDevChange->ItemChanged)
	  {
	  case DEVCHANGE_CONNECTIDX:
		   strcpy(pCQueueFile->pCD->pDevInfo->ConnectStr, cdFrom->pDevInfo->ConnectStr);
		   break;
      case DEVCHANGE_RESIDX:
		   pCQueueFile->pCD->pDevInfo->CurrentResIdx = cdFrom->pDevInfo->CurrentResIdx;
		   break;
      case DEVCHANGE_COPIES: 
		   pCQueueFile->pCD->pDevInfo->Copies = cdFrom->pDevInfo->Copies;
		   break;
      case DEVCHANGE_SCALEIMAGE:
		   pCQueueFile->pCD->pDevInfo->ScaleImage = cdFrom->pDevInfo->ScaleImage;
		   break;
      case DEVCHANGE_ALIASSTYLE:
		   pCQueueFile->pCD->pDevInfo->AliasStyle = cdFrom->pDevInfo->AliasStyle;
		   break;
      case DEVCHANGE_ALIASLEVEL:
		   pCQueueFile->pCD->pDevInfo->AliasLevel = cdFrom->pDevInfo->AliasLevel;
		   break;													 		   
      case DEVCHANGE_BUFFERTODISK:
		   pCQueueFile->pCD->pDevInfo->BufferToDisk = cdFrom->pDevInfo->BufferToDisk;
		   break;
      case DEVCHANGE_USECOMPRESSION:
		   pCQueueFile->pCD->pDevInfo->UseCompression = cdFrom->pDevInfo->UseCompression;
		   break;
      case DEVCHANGE_USECOMPRESSIONBMP:
		   pCQueueFile->pCD->pDevInfo->UseCompressionBmp = cdFrom->pDevInfo->UseCompressionBmp;
		   break;
	  case DEVCHANGE_DESCRIPTION:
		   pCQueueFile->Description = pDevChange->csText;
		   break;
      case DEVCHANGE_MEDIAIDX:
		   pCQueueFile->pCD->pDevInfo->CurrentMediaIdx = cdFrom->pDevInfo->CurrentMediaIdx;
		   break;
      case DEVCHANGE_ORIENTATION:
		   pCQueueFile->pCD->pDevInfo->Orientation = cdFrom->pDevInfo->Orientation;
		   break;		   
      case DEVCHANGE_ORIENTATION_OPT:
		   pCQueueFile->pCD->pDevInfo->OptimizeOrientation = cdFrom->pDevInfo->OptimizeOrientation;
		   break;		   
      case DEVCHANGE_BITMAPSAMPLING:
		   pCQueueFile->pCD->pDevInfo->BitmapSample = cdFrom->pDevInfo->BitmapSample;
		   break;		   
	  case DEVCHANGE_MEMUSED:
		   pCQueueFile->pCD->pDevInfo->MemUsed = cdFrom->pDevInfo->MemUsed;
		   break;
      case DEVCHANGE_CUSTOMPAGE:
	       pCQueueFile->pCD->pDevInfo->CustomPage_Width = cdFrom->pDevInfo->CustomPage_Width;
		   pCQueueFile->pCD->pDevInfo->CustomPage_Height = cdFrom->pDevInfo->CustomPage_Height;
	       pCQueueFile->pCD->pDevInfo->CustomPage_XOffset = cdFrom->pDevInfo->CustomPage_XOffset;
	       pCQueueFile->pCD->pDevInfo->CustomPage_YOffset = cdFrom->pDevInfo->CustomPage_YOffset;
		   break;
	  case DEVCHANGE_PRANGE:
		   pCQueueFile->m_prange_edit = pDevChange->csText;
		   if (pDevChange->csText.GetLength())
		      pCQueueFile->m_prange = pDevChange->arg1;
		   else
			  pCQueueFile->m_prange = pDevChange->arg1 = FALSE;
		   break;
	  }
   }
 
void CSpecialTree::UpdateSpecificXFormItem(CTransform* ctFrom, CTransform* ctTo, int Item)
   {
   switch(Item)
	  {
      case XFORMCHANGE_STARTTYPE:
		   ctTo->Name = ctFrom->Name;
		   break;
      case XFORMCHANGE_DESCRIPTION: 
		   ctTo->Description = ctFrom->Description;
		   break;
      case XFORMCHANGE_ROTATE:
		   ctTo->Rotate = ctFrom->Rotate;
		   break;
      case XFORMCHANGE_XZOOM:
		   ctTo->XZoom = ctFrom->XZoom;
		   break;
      case XFORMCHANGE_YZOOM:
		   ctTo->YZoom = ctFrom->YZoom;
		   break;													 		   
      case XFORMCHANGE_XOFF:
		   ctTo->XOff = ctFrom->XOff;
		   break;
      case XFORMCHANGE_YOFF:
		   ctTo->YOff = ctFrom->YOff;
		   break;
      case XFORMCHANGE_ALL:
		   *ctTo = *ctFrom;
		   break;
	  }
   }
void CSpecialTree::UpdateSpecificColorItem(PCPINITSTRUCT ctFrom, PCPINITSTRUCT ctTo, int Item)
   {
	int i = 0;

   switch(Item)
	  {
      case CMMCHANGE_MAXBLACK:
		   ctTo->MaxBlack = ctFrom->MaxBlack;
		   break;
      case CMMCHANGE_MAXINK: 
		   ctTo->MaxInk = ctFrom->MaxInk;
		   break;
      case CMMCHANGE_GCRUCR:
		   ctTo->GCR_UCR_Value = ctFrom->GCR_UCR_Value;
		   break;
      case CMMCHANGE_BRIGHTNESS:
		   ctTo->Brightness = ctFrom->Brightness;
           ctTo->bColorAdjustment = ctFrom->bColorAdjustment;
		   break;
      case CMMCHANGE_CONTRAST:
		   ctTo->Contrast = ctFrom->Contrast;
           ctTo->bColorAdjustment = ctFrom->bColorAdjustment;
		   break;													 		   
      case CMMCHANGE_COLORLEVELS:
		  for(i = 0; i < 4; i++)
		  {
		   ctTo->ColorLevels[i] = ctFrom->ColorLevels[i];
		  }
           ctTo->bColorAdjustment = ctFrom->bColorAdjustment;
		   break;
#if 0
      case CMMCHANGE_GRAPHLINES:
		  for(i = 0; i < DEFAULT_LUT_SIZE * 4; i++)
		  {
		   ctTo->ColorAdjTable[i] = ctFrom->ColorAdjTable[i];
		   ctTo->ColorTrnsfrmTable[i] = ctFrom->ColorTrnsfrmTable[i];
		  }
		   break;
#endif
      case CMMCHANGE_GRAPHLINES_ADJ:
		  for(i = 0; i < DEFAULT_LUT_SIZE * 4; i++)
		  {
		   ctTo->ColorAdjTable[i] = ctFrom->ColorAdjTable[i];		   
		  }
		   break;
      case CMMCHANGE_ADJLOCK:
		   ctTo->bLockAdjustments = ctFrom->bLockAdjustments;
		   break;

      case CMMCHANGE_GRAPHLINES_TRN:
		  for(i = 0; i < DEFAULT_LUT_SIZE * 4; i++)
		  {		   
		   ctTo->ColorTrnsfrmTable[i] = ctFrom->ColorTrnsfrmTable[i];
		  }
		   break;
      case CMMCHANGE_TRNSLOCK:
		   ctTo->bLockTransformGraph = ctFrom->bLockTransformGraph;
		   break;

      case CMMCHANGE_GAMMA:
		  for(i = 0; i < DEFAULT_LUT_SIZE * 4; i++)
		  {
		   ctTo->gamma[i] = ctFrom->gamma[i];
		  }
		   break;
      case CMMCHANGE_TRANSTYPE: 
		   ctTo->TransType = ctFrom->TransType;
		   break;
      case CMMCHANGE_PRINTERCSPACE:
		   ctTo->Printer_CSpace = ctFrom->Printer_CSpace;
		   break;
      case CMMCHANGE_INPUTCSPACE:
		   ctTo->Input_CSpace = ctFrom->Input_CSpace;
		   break;
      case CMMCHANGE_PREVIEW:
		   ctTo->Preview = ctFrom->Preview;
		   break;													 		   
      case CMMCHANGE_PREPRESS:
		   ctTo->Prepress = ctFrom->Prepress;
		   break;
      case CMMCHANGE_WHITEISWHITE:
		   ctTo->WhiteIsWhite = ctFrom->WhiteIsWhite;
           ctTo->bColorAdjustment = ctFrom->bColorAdjustment;
		   break;
      case CMMCHANGE_BLACKISBLACK:
		   ctTo->BlackIsBlack = ctFrom->BlackIsBlack;
           ctTo->bColorAdjustment = ctFrom->bColorAdjustment;
		   break;
      case CMMCHANGE_RGBINPUTNAME:
		   strncpy(ctTo->RGB_In_Name,ctFrom->RGB_In_Name,MAX_PATH);
		   break;
      case CMMCHANGE_CMYKINPUTNAME: 
		   strncpy(ctTo->CMYK_In_Name,ctFrom->CMYK_In_Name,MAX_PATH);
		   break;
      case CMMCHANGE_MONITORNAME:
		   strncpy(ctTo->Monitor_Name,ctFrom->Monitor_Name,MAX_PATH);
		   break;
      case CMMCHANGE_PRESSNAME:
		   strncpy(ctTo->Press_Name,ctFrom->Press_Name,MAX_PATH);
		   break;
      case CMMCHANGE_PRINTERNAME:
		   strncpy(ctTo->Printer_Profile_Name,ctFrom->Printer_Profile_Name,MAX_PATH);
		   break;
	  case CMMCHANGE_IMAGE_RENDINTENT:
		   ctTo->Image_Rendering_Intent = ctFrom->Image_Rendering_Intent;
		   break;
	  case CMMCHANGE_TEXT_RENDINTENT:
		   ctTo->Text_Rendering_Intent = ctFrom->Text_Rendering_Intent;
		   break;
      case CMMCHANGE_ALL:
		   *ctTo = *ctFrom;
		   break;
	  }
   }

void CSpecialTree::OnEditCopyatts() 
   {
   // only do a copy atts if one file is selected
   if (GetSelectedCount() == 1)      
	  CopyToClipboard(OURCLIP_ATTS);	
   }

void CSpecialTree::OnUpdateEditCopyatts(CCmdUI* pCmdUI) 
   {   
   pCmdUI->Enable((GetSelectedCount() == 1) ? TRUE : FALSE);
   }

void CSpecialTree::OnUpdateEditPasteatts(CCmdUI* pCmdUI) 
   {
   if (CLIPARRAY.IsTypeAvail(OURCLIP_ATTS))
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
   }

void CSpecialTree::OnEditPasteatts() 
   {
   int nArrIdx, NumSelected;   
   CQueueFile* pCQueueFile;
   CQueueFile* pSrcCQueueFile;   
   CDWordArray *pArrSelected = NULL;
   DWORD dwIndex;   

   // get selection array
   pArrSelected = GetSelectionArray();

   // how many are selected?
   NumSelected = pArrSelected->GetSize();   
    
   // if we have no items, return
   if (NumSelected < 1) return;              
   
   pSrcCQueueFile = NULL;

   if (CLIPARRAY.IsTypeAvail(OURCLIP_ATTS))      
      {      
	  // create a new queue file
	  pSrcCQueueFile = new CQueueFile;	  
	  // only get first item
	  *pSrcCQueueFile = *((CQueueFile*)(CLIPARRAY.GetAt(0)));
	  }
   
   // nothing!
   if (!pSrcCQueueFile) return;

   // get the selected items
   for (nArrIdx = pArrSelected->GetUpperBound(); nArrIdx >= 0; --nArrIdx)
      {                               
	  // get the index
	  dwIndex = pArrSelected->GetAt(nArrIdx);
	  // get the item
	  pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(dwIndex));

	  // copy device and transform stuff
	  if (pSrcCQueueFile->pCD && (pSrcCQueueFile->Prefs & USE_LOCAL_DEV_PREFS))  
	     {
		 // make sure that we are copying atts from the same device
		 if (pSrcCQueueFile->pCD->DevIdx == QUEUEDEVICE.DevIdx)
			{
		    // alloc new device
		    if (!(pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS))
		       pCQueueFile->pCD = new CDevice;
		    *(pCQueueFile->pCD) = *(pSrcCQueueFile->pCD);
			}
		 else
			{
			// not the same device, so force the QUEUEDEVICE
			if ((pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS) && pCQueueFile->pCD)
			   {
			   pCQueueFile->Prefs -= USE_LOCAL_DEV_PREFS;
			   delete pCQueueFile->pCD;
			   }
			}			 
		 }
	  else
	     {
		 // no device, so delete this one
		 if (pCQueueFile->pCD && (pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS))
			{
			pCQueueFile->Prefs -= USE_LOCAL_DEV_PREFS;
		    delete pCQueueFile->pCD;
			}
		 }
	  // transform
	  if (pSrcCQueueFile->pTransform && (pSrcCQueueFile->Prefs & USE_LOCAL_TRN_PREFS))
	     {
		 // alloc new transform
		 if (!(pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS))		 
		    pCQueueFile->pTransform = new CTransform;
	     *(pCQueueFile->pTransform) = *(pSrcCQueueFile->pTransform);
		 }
	  else
	     {
		 // no transform, so delete this one
		 if (pCQueueFile->pTransform && (pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS))
			{
			pCQueueFile->Prefs -= USE_LOCAL_TRN_PREFS;
		    delete pCQueueFile->pTransform;
			}
		 }

	  // color
	  if (pSrcCQueueFile->pCC && (pSrcCQueueFile->Prefs & USE_LOCAL_COL_PREFS))
	     {
		 // make sure that we are copying atts from the same device
		 if (pSrcCQueueFile->pCC->CPInitStruct.GlobalPrinterID == QUEUEDEVICE.DevIdx)
			{
		    // alloc new color
		    if (!(pCQueueFile->Prefs & USE_LOCAL_COL_PREFS))		 
		       pCQueueFile->pCC = new CColorTransform;
	        *(pCQueueFile->pCC) = *(pSrcCQueueFile->pCC);
			}
		 else
			{
		    // not the same device, so turn off color
		    if ((pCQueueFile->Prefs & USE_LOCAL_COL_PREFS) && pCQueueFile->pCC)
		       delete pCQueueFile->pCC;
			pCQueueFile->pCC = NULL;	        
			}
		 }
	  else
	     {
		 // no color, so delete this one
		 if (pCQueueFile->pCC && (pCQueueFile->Prefs & USE_LOCAL_COL_PREFS))
			{
			pCQueueFile->Prefs -= USE_LOCAL_COL_PREFS;
		    delete pCQueueFile->pCC;
			}
		 }
	
	  // copy prefs
	  pCQueueFile->Prefs = pSrcCQueueFile->Prefs;

	  // we've already forced this to the QUEUEDEVICE if the wrond device
	  if (pSrcCQueueFile->pCD)
		 {
	     if (pSrcCQueueFile->pCD->DevIdx != QUEUEDEVICE.DevIdx)
			{
		    if (pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS)
		       pCQueueFile->Prefs -= USE_LOCAL_DEV_PREFS;
			}
		 }
	  // we've already forced this to no color if the wrong device
	  if (pSrcCQueueFile->pCC)
		 {
	     if (pSrcCQueueFile->pCC->CPInitStruct.GlobalPrinterID != QUEUEDEVICE.DevIdx)
			{
		    if (pCQueueFile->Prefs & USE_LOCAL_COL_PREFS)
		       pCQueueFile->Prefs -= USE_LOCAL_COL_PREFS;
			}
		 }

      // redraw it   
      GetListCtrl().Update(nArrIdx);
	  }

   // force a reshow
   ReMeasureAllItems();
   Invalidate();   	   
   // our doc is modified   
   PDOC->SetModifiedFlag();
   // if the info box is on, update it
   UpdateInfo();			  

   if (pSrcCQueueFile) delete pSrcCQueueFile;
   }   

void CSpecialTree::OnUpdateEditResetstatus(CCmdUI* pCmdUI) 
   {
   pCmdUI->Enable((GetSelectedCount() > 0) ? TRUE : FALSE);
   }

void CSpecialTree::OnEditResetstatus() 
   {
   int nArrIdx, NumSelected;   
   CQueueFile* pCQueueFile;
   CDWordArray *pArrSelected = NULL;
   DWORD dwIndex;   

   // get selection array
   pArrSelected = GetSelectionArray();

   // how many are selected?
   NumSelected = pArrSelected->GetSize();   
    
   // if we have no items, return
   if (NumSelected < 1) return;

   // get the items selected   
   for (nArrIdx = pArrSelected->GetUpperBound(); nArrIdx >= 0; --nArrIdx)
      {  
      // get the index
	  dwIndex = pArrSelected->GetAt(nArrIdx);
	  // get the item
	  pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(dwIndex));

      // load the default strings
      pCQueueFile->Status.LoadString(IDS_READY);
      pCQueueFile->PreviewStatus.LoadString(IDS_READY);
      //pCQueueFile->StatusFlag = FILE_STATUS_HOLD;      
	  pCQueueFile->StatusFlag |= FILE_STATUS_HOLD;      
	  pCQueueFile->WasTouched = FALSE;
	  pCQueueFile->ctsElapsedTime = 0;
	  // redraw the item
	  GetListCtrl().Update(nArrIdx);
      }
   // update the file info box     
   UpdateInfo();     

   // our doc is modified
   PDOC->SetModifiedFlag();
   }
  
void CSpecialTree::OnPreferencesQueue() 
   {   
   CString csTitle;
   BOOL PollingWasHalted;
   BOOL bReleventChanges;

   CProfile Profile;

   // hide the local prefs window
   if (m_pPropFrame) m_pPropFrame->ShowWindow(SW_HIDE);
   
   // stop polling
   //if (PDOC->crrrs.bEnableHotFolder)
   if (PDOC->crrrs.iHotFolderType != HOTFOLDER_TYPE_DISABLED)
      {
	   ::SendMessage(pCQueueChild->pHoldWnd->GetSafeHwnd(), wm_StopPolling, 0, 0); 
	   ::SendMessage(pCQueueChild->pPrintWnd->GetSafeHwnd(), wm_StopPolling, 0, 0); 
	  //StopServerPolling();
	  PollingWasHalted = TRUE;
	  }
   else
      PollingWasHalted = FALSE;

   // make title
   csTitle = QUEUEDEVICE.Name;   
   csTitle += " Properties";

   // instantiate
   CJobPropsSheet m_ModalPropFrame(TRUE, NULL, csTitle);
   m_pModalPropFrame = &m_ModalPropFrame;

   m_ModalPropFrame.TargetDevice = QUEUEDEVICE;
   // set current queue file
   m_ModalPropFrame.m_general.pTargetCQueue = NULL;

   // no apply button
   m_ModalPropFrame.m_psh.dwFlags |= PSH_NOAPPLYNOW;
 
   // set device   
   m_ModalPropFrame.m_general.pTargetDevice = &m_ModalPropFrame.TargetDevice;   
   //Set flag so that we won't create a new object on a change.
   m_pPropFrame->m_pModelessPropSheet->m_general.bIsUsingDefTarg = FALSE;	  

   // send messages to this window
   m_ModalPropFrame.m_general.pReportWnd = this;
      
   m_ModalPropFrame.m_general.m_filename = 
   m_ModalPropFrame.m_general.m_filesize =       
   m_ModalPropFrame.m_general.m_filepages = 
   m_ModalPropFrame.m_general.m_filedate = 
   m_ModalPropFrame.m_general.m_filetime =
   m_ModalPropFrame.m_general.m_filetype = 
   m_ModalPropFrame.m_general.m_boundingbox = 
   m_ModalPropFrame.m_general.m_description = 
   m_ModalPropFrame.m_general.m_prange_edit = "";
   m_ModalPropFrame.m_general.m_prange = FALSE;
   m_ModalPropFrame.m_general.DoingDefaultProps = TRUE;

   // sizing stuff
   // if no files exist in pane, set this to null
   m_ModalPropFrame.m_size.pTargetCQueue = NULL;
   // if we have files but non are selected, select the first one
   if (GetCount() > 0)
	  m_ModalPropFrame.m_size.pTargetCQueue = (CQueueFile*)GetItemData(GetItemAt(0));
   // if we have files and at least one is selected, set this
   for (int i = 0; i < GetCount(); i++)      
	  {
	  if (IsSelected(i))
		 {
		 m_ModalPropFrame.m_size.pTargetCQueue = (CQueueFile*)GetItemData(GetItemAt(i));
		 break;
		 }
	  }		
   m_ModalPropFrame.m_size.pTargetDevice = &QUEUEDEVICE;
   m_ModalPropFrame.m_size.csScratchPath = PDOC->crrrs.csScratchPath;

   m_ModalPropFrame.m_size.SetTransform(&QUEUETRANSFORM);
   m_ModalPropFrame.m_size.pReportWnd = this;
   m_ModalPropFrame.m_size.DoingDefaultProps = TRUE;
   m_ModalPropFrame.m_size.pLocalColor = &QUEUECOLOR;
   
   // device stuff	  
   //m_ModalPropFrame.m_device.pTargetDevice = &QUEUEDEVICE;
   m_ModalPropFrame.m_device.pTargetDevice = &m_ModalPropFrame.TargetDevice;
   m_ModalPropFrame.m_device.DoingDefaultProps = TRUE;
   m_ModalPropFrame.m_device.pReportWnd = this;
   DoingDefaultProps = TRUE;
   UpdateDeviceDlg();
   
   // color stuff
   strncpy(QUEUECOLOR.CPInitStruct.Printer_Name, 
	  m_ModalPropFrame.m_device.pTargetDevice->Name, MAX_PATH);
   QUEUECOLOR.CPInitStruct.GlobalPrinterID = 
	  m_ModalPropFrame.m_device.pTargetDevice->DevIdx; 
   QUEUECOLOR.CPInitStruct.Printer_CSpace = 
	  m_ModalPropFrame.m_device.pTargetDevice->pDevInfo->iColorSpace;    

   m_ModalPropFrame.m_cmmdialog.pReportWnd = this;
   m_ModalPropFrame.m_cmmdialog.SetInitStruct(&QUEUECOLOR.CPInitStruct); 
   m_ModalPropFrame.m_cmmdialog.DoingDefaultProps = TRUE;
   m_ModalPropFrame.m_coloradjustdialog.pReportWnd = this;
   m_ModalPropFrame.m_coloradjustdialog.SetInitStruct(&QUEUECOLOR.CPInitStruct);    
   m_ModalPropFrame.m_coloradjustdialog.DoingDefaultProps = TRUE;

   // set indeterminancy
   m_ModalPropFrame.m_size.m_xzoom_is_indeterminate = FALSE;
   m_ModalPropFrame.m_size.m_yzoom_is_indeterminate = FALSE;
   m_ModalPropFrame.m_size.m_xoff_is_indeterminate = FALSE;
   m_ModalPropFrame.m_size.m_yoff_is_indeterminate = FALSE;
   m_ModalPropFrame.m_size.m_rotation_is_indeterminate = FALSE;

   // memory stuff   
   m_ModalPropFrame.m_memory.m_imagememory_num = QUEUEDEVICE.pDevInfo->BandSize / 1024 * 
	  QUEUEDEVICE.pDevInfo->BandWidth;
   m_ModalPropFrame.m_memory.m_spool = PDOC->crrrs.csSpoolDir;
   m_ModalPropFrame.m_memory.m_scratch = PDOC->crrrs.csScratchPath;   
   m_ModalPropFrame.m_memory.m_diskfiles = PDOC->crrrs.iDiskFilesResourceSelector;
   m_ModalPropFrame.m_memory.m_listjobs = PDOC->crrrs.iListJobsResourceSelector;      
   m_ModalPropFrame.m_memory.m_listjobs_max = PDOC->crrrs.iListJobsMax;   
   //m_ModalPropFrame.m_memory.m_enable_hotfolder = PDOC->crrrs.bEnableHotFolder;
   if (PDOC->crrrs.iHotFolderType != HOTFOLDER_TYPE_DISABLED)
      m_ModalPropFrame.m_memory.m_enable_hotfolder = TRUE;
   else
	  m_ModalPropFrame.m_memory.m_enable_hotfolder = FALSE;
   m_ModalPropFrame.m_memory.m_hotfolder = PDOC->crrrs.iHotFolderResourceSelector;   
   m_ModalPropFrame.m_memory.m_hotfolder_dir = PDOC->crrrs.csHotFolderDir;   
   m_ModalPropFrame.m_memory.m_hotfolder_inpane = PDOC->crrrs.iHotFolderInPane;   
   m_ModalPropFrame.m_memory.m_image_only_new = PDOC->crrrs.bImageOnlyNew;
   m_ModalPropFrame.m_memory.m_hotfolder_delay = PDOC->crrrs.iHotFolderDelay;   
   m_ModalPropFrame.m_memory.m_bHotFolderReset = FALSE;
   m_ModalPropFrame.m_memory.m_memused = QUEUEDEVICE.pDevInfo->MemUsed;
   m_ModalPropFrame.m_memory.m_memalloc_style = QUEUEDEVICE.pDevInfo->MemUsedIdx;
   m_ModalPropFrame.m_memory.pTargetDevice = &m_ModalPropFrame.TargetDevice;
   m_ModalPropFrame.m_memory.m_bSelect_By_Size = PDOC->crrrs.m_bSelectBySize;
   m_ModalPropFrame.m_memory.m_hotfolder_dpi = PDOC->crrrs.iHotFolderDPI;   
   m_ModalPropFrame.m_memory.m_hotfolder_variance = PDOC->crrrs.iHotFolderVariance;   

   m_ModalPropFrame.m_device.pTargetDevice->SetDevInfo(m_ModalPropFrame.m_device.pTargetDevice->pDevInfo, TRUE);

   // on ok, get back the data
   if (m_ModalPropFrame.DoModal() == IDOK)
	  {	  
	  // first see if relevent changes were made
	  if (QUEUEDEVICE != m_ModalPropFrame.TargetDevice ||
	      QUEUETRANSFORM != m_ModalPropFrame.m_size.GetDefTransform() ||
		  !(QUEUECOLOR.IsSame(m_ModalPropFrame.m_cmmdialog.LocalInitStruct)) ||
		  !(QUEUECOLOR.IsSame(m_ModalPropFrame.m_coloradjustdialog.LocalInitStruct)))
		 bReleventChanges = TRUE;
	  else
	     bReleventChanges = FALSE;

	  QUEUEDEVICE = m_ModalPropFrame.TargetDevice;
	  // save as the default
	  QUEUEDEVICE.SetDevInfo(TRUE);
	  QUEUETRANSFORM = m_ModalPropFrame.m_size.GetDefTransform();
	  QUEUECOLOR.CopyCMMDlg(&QUEUECOLOR.CPInitStruct,&m_ModalPropFrame.m_cmmdialog.LocalInitStruct);
	  QUEUECOLOR.CopyClrAdjDlg(&QUEUECOLOR.CPInitStruct,&m_ModalPropFrame.m_coloradjustdialog.LocalInitStruct);

      PDOC->crrrs.csSpoolDir = m_ModalPropFrame.m_memory.m_spool;
	  // write the default spool folder
	  ((CZeusApp*)AfxGetApp())->SetSpoolFolder(PDOC->crrrs.csSpoolDir);
      PDOC->crrrs.csScratchPath = m_ModalPropFrame.m_memory.m_scratch;   
	  // write the default scratch folder
	  ((CZeusApp*)AfxGetApp())->SetScratchFolder(PDOC->crrrs.csScratchPath);
      PDOC->crrrs.iDiskFilesResourceSelector = m_ModalPropFrame.m_memory.m_diskfiles;
      PDOC->crrrs.iListJobsResourceSelector = m_ModalPropFrame.m_memory.m_listjobs;      
      PDOC->crrrs.iListJobsMax = m_ModalPropFrame.m_memory.m_listjobs_max;
	  PDOC->crrrs.csHotFolderDir = m_ModalPropFrame.m_memory.m_hotfolder_dir;
	  PDOC->crrrs.iHotFolderDelay = m_ModalPropFrame.m_memory.m_hotfolder_delay;
      PDOC->crrrs.m_bSelectBySize = m_ModalPropFrame.m_memory.m_bSelect_By_Size;
      PDOC->crrrs.iHotFolderDPI = m_ModalPropFrame.m_memory.m_hotfolder_dpi;   
      PDOC->crrrs.iHotFolderVariance = m_ModalPropFrame.m_memory.m_hotfolder_variance;
	  
	  //Handle the hot folder option
	  if(m_ModalPropFrame.m_memory.m_enable_hotfolder){
		  //Does the specified hot folder exist?
		  CString			csMsg;
		  DWORD				LastError;
		  BOOL				bTryToCreateHotFolder = FALSE;
		  CFileFind			Finder;
		  CString			strFolderName;
		 
		  //Add a backslash if we need to so that this is designated as a folder and not a file
		  if(m_ModalPropFrame.m_memory.m_hotfolder_dir.GetAt(m_ModalPropFrame.m_memory.m_hotfolder_dir.GetLength()-1) != '\\')
			  strFolderName = m_ModalPropFrame.m_memory.m_hotfolder_dir += '\\';
		  
		  //Add the wildcard so that we can search successfully
		  strFolderName = m_ModalPropFrame.m_memory.m_hotfolder_dir + "*.*";
		  //Try and find the folder
		  if(!Finder.FindFile(strFolderName)){
			  //Find out why we failed
			  LastError = GetLastError();
			  if(LastError == ERROR_FILE_NOT_FOUND || LastError == ERROR_PATH_NOT_FOUND)
				  bTryToCreateHotFolder = TRUE;
			  else
				  csMsg.Format("Unable to create the hot folder %s please check to make sure that path is valid.", m_ModalPropFrame.m_memory.m_hotfolder_dir);
		  }

		  //The folder didn't exist so let's see if we can and should create one.
		  if(bTryToCreateHotFolder){
			  //Format message string
			  csMsg.Format("The specified hot folder %s does not exist. Would you like to create it?", m_ModalPropFrame.m_memory.m_hotfolder_dir);
			  //Post the message to the user
			  if(AfxMessageBox(csMsg, MB_OKCANCEL | MB_SETFOREGROUND) == IDOK){
				  //Create the hot folder and set all of the remaining values associated with the hot folder
				  if(::CreateDirectory(m_ModalPropFrame.m_memory.m_hotfolder_dir, NULL)){
					  PDOC->crrrs.csHotFolderDir = m_ModalPropFrame.m_memory.m_hotfolder_dir;

					  //PDOC->crrrs.bEnableHotFolder = m_ModalPropFrame.m_memory.m_enable_hotfolder;
					  if (m_ModalPropFrame.m_memory.m_enable_hotfolder)					  
						 PDOC->crrrs.iHotFolderType = HOTFOLDER_TYPE_STD;
					  else
						 PDOC->crrrs.iHotFolderType = HOTFOLDER_TYPE_DISABLED;

					  PDOC->crrrs.iHotFolderResourceSelector = m_ModalPropFrame.m_memory.m_hotfolder;      
					  int myval = PDOC->crrrs.iHotFolderInPane = m_ModalPropFrame.m_memory.m_hotfolder_inpane;
				  }
				  else{
					  csMsg.Format("Unable to create the hot folder %s please check to make sure that path is valid.", m_ModalPropFrame.m_memory.m_hotfolder_dir);
					  //The hot folder doesn't exist so let's disable it
					  m_ModalPropFrame.m_memory.m_enable_hotfolder = !m_ModalPropFrame.m_memory.m_enable_hotfolder;
				  }
			  }
			  else{
				  //The hot folder doesn't exist so let's disable it
				  m_ModalPropFrame.m_memory.m_enable_hotfolder = !m_ModalPropFrame.m_memory.m_enable_hotfolder;
			  }
		  }
		  //It existed, let's set specify it as a hot folder now
		  else{
			  PDOC->crrrs.csHotFolderDir = m_ModalPropFrame.m_memory.m_hotfolder_dir;
			  //PDOC->crrrs.bEnableHotFolder = m_ModalPropFrame.m_memory.m_enable_hotfolder;
			  if (m_ModalPropFrame.m_memory.m_enable_hotfolder)					  
				 PDOC->crrrs.iHotFolderType = HOTFOLDER_TYPE_STD;
			  else
				 PDOC->crrrs.iHotFolderType = HOTFOLDER_TYPE_DISABLED;

			  PDOC->crrrs.iHotFolderResourceSelector = m_ModalPropFrame.m_memory.m_hotfolder;      
			  int myval = PDOC->crrrs.iHotFolderInPane = m_ModalPropFrame.m_memory.m_hotfolder_inpane;
		  }
	  }

	  //End handling the hot folder option
	  PDOC->crrrs.bImageOnlyNew = m_ModalPropFrame.m_memory.m_image_only_new;

      QUEUEDEVICE.pDevInfo->MemUsed = m_ModalPropFrame.m_memory.m_memused;
      QUEUEDEVICE.pDevInfo->MemUsedIdx = m_ModalPropFrame.m_memory.m_memalloc_style;
	  //PDOC->crrrs.bEnableHotFolder = m_ModalPropFrame.m_memory.m_enable_hotfolder;
	  if (m_ModalPropFrame.m_memory.m_enable_hotfolder)					  
		 PDOC->crrrs.iHotFolderType = HOTFOLDER_TYPE_STD;
	  else
		 PDOC->crrrs.iHotFolderType = HOTFOLDER_TYPE_DISABLED;

      // our doc is modified
      PDOC->SetModifiedFlag(); 	  

#if 0
	  // any relevent changes?
	  if (bReleventChanges && iViewStyle != QUEUESTYLE_ARCHIVE && iViewStyle != QUEUESTYLE_PRINT)
		 {
	     // apply these settings to all Jobs?
		 int iAnyJobProps = 0;
	     iAnyJobProps += AnyJobProps();			  
#if 0
	     switch(iViewStyle)
			{
	        case QUEUESTYLE_HOLD:
			     iAnyJobProps += ((CSpecialTree*)pCQueueChild->pPrintWnd)->AnyJobProps();			     
			     break;
		    case QUEUESTYLE_PRINT:	 
			     iAnyJobProps += ((CSpecialTree*)pCQueueChild->pHoldWnd)->AnyJobProps();			     
			     break;
			}		   
#endif		 
	     if (iAnyJobProps)
			{	     
	        if (AfxMessageBox(IDS_APPLYDEFAULTS, MB_YESNO | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_DEFBUTTON2) == IDYES)
			   {
			   ResetProps(TRUE);
#if 0
			   // do for all other panes
			   switch(iViewStyle)
				  {
		          case QUEUESTYLE_HOLD:
			           ((CSpecialTree*)pCQueueChild->pPrintWnd)->ResetProps();			           
			           break;
		          case QUEUESTYLE_PRINT:	 
			           ((CSpecialTree*)pCQueueChild->pHoldWnd)->ResetProps();			           
			           break;
				  }
#endif
			   }		    
			}
		 }
#endif
	  // any relevent changes?
	  if (bReleventChanges)
		 {
		 // any individual job props in hold queue
		 int iAnyJobProps = ((CSpecialTree*)pCQueueChild->pHoldWnd)->AnyJobProps();
		 // if so, ask to apply queue props to items in hold queue
	     if (iAnyJobProps)
			{	     
	        if (AfxMessageBox(IDS_APPLYDEFAULTS, MB_YESNO | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_DEFBUTTON2) == IDYES)
			   {
			   ((CSpecialTree*)pCQueueChild->pHoldWnd)->ResetProps( TRUE );			           
			   }
			else
			{
			  QUEUEDEVICE.SetDevInfo(TRUE);
			}

			}

		 }
//	  }
	   // the jobprops advanced device sometimes gets hammered after queueprops,
	   // so force this

	   if (m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice)
		  {
		  if (m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice->PropBoxCreated)
			 {
			 m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice->DoPropBox(DEVPROPBOX_DESTROY, this);
			 m_pPropFrame->m_pModelessPropSheet->m_device.pTargetDevice->PropBoxCreated = FALSE;
			 }
		  }

	   }

	  // Before we restart POLLING Clear the Hot Folders list IF it has been RESET
	  if ( m_ModalPropFrame.m_memory.m_bHotFolderReset )
	  {
			PDOC->crrrs.EmptyCheckedFiles();
			PDOC->crrrs.EmptyFilesToDo();
	  }

	  //Start polling again for the print and hold Q's, we do a complete re-init if PollingWasHalted
	  //StartServerPolling(!PollingWasHalted);
	  ::SendMessage(pCQueueChild->pHoldWnd->GetSafeHwnd(), wm_StartPolling, 0, !PollingWasHalted); 
	  ::SendMessage(pCQueueChild->pPrintWnd->GetSafeHwnd(), wm_StartPolling, 0, !PollingWasHalted); 
}													 

BOOL CSpecialTree::InsertQueueFile(CString& csFile, int& idx)
   {
   CQueueFile* pCQueueFile;
   BOOL bRet = FALSE;
   BOOL bContinue = TRUE;
   int iPage = 0;
   int iTotalPages = 0;
#ifdef LEADFILE_PS
   int iResetJobID;
   CQueueFile* pCQueueFile_Page;
#endif

   // try to insert it into the list		 
   if (pCQueueFile = new CQueueFile)
   {
	  if (pCQueueFile->Init(csFile, iTotalPages, iPage))
	  {
		 // not touched     
		 pCQueueFile->WasTouched = FALSE;
		 // in what queue
		 pCQueueFile->InQueue = iViewStyle;
		 pCQueueFile->iJobID = ((CZeusApp*)AfxGetApp())->GetJobID();
		 // if dropping to the hold queue, reset the status
		 if (iViewStyle == QUEUESTYLE_HOLD)
			{
			pCQueueFile->Status.LoadString(IDS_READY);
			pCQueueFile->PreviewStatus.LoadString(IDS_READY);
			pCQueueFile->StatusFlag |= FILE_STATUS_HOLD;
			pCQueueFile->ctsElapsedTime = 0;
			}
		 // we must lock these in if this is an archive
		 if (iViewStyle == QUEUESTYLE_ARCHIVE)
			{
			// if no local prefs, lock them in now   
			if (!(pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS))
			   {
			   pCQueueFile->pCD = new CDevice;
			   *(pCQueueFile->pCD) = QUEUEDEVICE;
			   pCQueueFile->Prefs |= USE_LOCAL_DEV_PREFS;        
			   }
			if (!(pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS))
			   {
			   pCQueueFile->pTransform = new CTransform;
			   *(pCQueueFile->pTransform) = QUEUETRANSFORM;
			   pCQueueFile->Prefs |= USE_LOCAL_TRN_PREFS;        
			   }
			if (!(pCQueueFile->Prefs & USE_LOCAL_COL_PREFS))
			   {
			   pCQueueFile->pCC = new CColorTransform;
			   *(pCQueueFile->pCC) = QUEUECOLOR;
			   pCQueueFile->Prefs |= USE_LOCAL_COL_PREFS;        
			   }
			}

			// store it            
			pQueueArray->InsertAt(idx, pCQueueFile);	    
			// add index of new QueueFile to ListBox
			InsertString(idx++, pCQueueFile);
			bRet = TRUE;
	  }
	  else
	  {
		  bContinue = FALSE;
	  }

#ifdef LEADFILE_PS
	 iPage++;

	 while ( iPage < iTotalPages && bContinue )
	 {
		if ( pCQueueFile_Page = new CQueueFile )
		{
			iResetJobID = pCQueueFile_Page->iJobID;
			*pCQueueFile_Page = *pCQueueFile;
			pCQueueFile_Page->iJobID = iResetJobID;

			pCQueueFile_Page->PS_JPGFileLongName = ((CZeusApp*)AfxGetApp())->Convert_PS_to_JPG(pCQueueFile_Page->LongName, iPage);

			pQueueArray->InsertAt(idx, pCQueueFile_Page);	    
			// add index of new QueueFile to ListBox
			InsertString(idx++, pCQueueFile_Page);
			iPage++;
			bRet = TRUE;
		}
		else
		{
		    delete pCQueueFile_Page;
			bContinue = FALSE;
		}
	 }		// End While
#endif
   }
   else
   {
	  // error
	  delete pCQueueFile;
   }
   
   return bRet;
   }

void CSpecialTree::AddFolder(CString& csFile, int& idx)
   {
   WIN32_FIND_DATA FileInfo;
   CString csFileName;
   CString csSearchPath;
   HANDLE hFile;
	
   csFile += "\\";
   csSearchPath = csFile;
   csSearchPath += "*.*";
   
   // check for existence
   if ((hFile = FindFirstFile(csSearchPath, &FileInfo)) != INVALID_HANDLE_VALUE)
	  {  	        
	  if ((FileInfo.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ||
	      (FileInfo.dwFileAttributes & FILE_ATTRIBUTE_NORMAL))
		 {
	     // add path    
	     csFileName = csFile;
         // add file name
         csFileName += FileInfo.cFileName;
	     // insert the file
		 InsertQueueFile(csFileName, idx);
		 }
	  else if (FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		 {			 
		 // recursive
		 if (FileInfo.cFileName[0] != '.')
			{
		    csFile += FileInfo.cFileName;
		    AddFolder(csFile, idx);
			}
		 }
			
	  while (FindNextFile(hFile, &FileInfo))
		 {		       
	     if ((FileInfo.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ||
	         (FileInfo.dwFileAttributes & FILE_ATTRIBUTE_NORMAL)) 	           
			{
		    // add path    
	        csFileName = csFile;
            // add file name
            csFileName += FileInfo.cFileName;
	        // delete the file
		    InsertQueueFile(csFileName, idx);
			}
	      else if (FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			 {
		     // recursive
			 if (FileInfo.cFileName[0] != '.')
				{
			    csFile += FileInfo.cFileName;
		        AddFolder(csFile, idx);
				}
			 }
		 }			  		 	     	  
       // close the handle
       FindClose(hFile);   	  
	  }	     
   }

void CSpecialTree::OnSetFocus(CWnd* pOldWnd) 
{				
	CProfile Profile;
	CString csFriendlyName;

//	TRACE("SpecialTree: OnSetFocus\n");

	CRPTreeBase::OnSetFocus(pOldWnd);

	// set this pane active if we have the focus
	((CSplitterWnd*)GetParent())->SetActivePane(iViewStyle, 0);

	if ( Profile.GetInt(QUEUEDEVICE.Name, "ReInstallQueue") )
	{
		Profile.WriteInt(QUEUEDEVICE.Name, "ReInstallQueue", 0 );
		ReinstallAppDriver( TRUE );
	}
}			   

void CSpecialTree::SortCol(int DrawHeaderPushCol)
   {
   BOOL bNotDone = TRUE;
   int pos;

   if (GetCount() < 1) return;
 
   // sort until done
   while (bNotDone)
      {
      bNotDone = FALSE;
      for (pos = 0; pos < GetCount() - 1; pos++)
         bNotDone |= CompareAndSwap(pos, DrawHeaderPushCol);
      }
   QueueSortOrderUp[DrawHeaderPushCol] = !(QueueSortOrderUp[DrawHeaderPushCol]);
   }

BOOL CSpecialTree::CompareAndSwap(int pos, int DrawHeaderPushCol)
   {   
   int posFirst = pos;
   int posNext = pos + 1;
   CQueueFile* pCQueueFileFirst;
   CQueueFile* pCQueueFileNext;
   int ret = -1;   

   // get the queue files
   pCQueueFileFirst = (CQueueFile*)GetItemData(GetItemAt(posFirst));
   pCQueueFileNext = (CQueueFile*)GetItemData(GetItemAt(posNext));

   // compare  
   switch (DrawHeaderPushCol)
	  {
      case QUEUECOL_DESCRIPTION:
		   ret = pCQueueFileFirst->Description.CompareNoCase(pCQueueFileNext->Description);       
		   break;
      case QUEUECOL_FILENAME:
		   ret = pCQueueFileFirst->ShortName.CompareNoCase(pCQueueFileNext->ShortName);           
		   break;
      case QUEUECOL_OWNER:
		   ret = pCQueueFileFirst->csOwner.CompareNoCase(pCQueueFileNext->csOwner);           
		   break;
      case QUEUECOL_SIZE:
           if (pCQueueFileFirst->RealFileSize > pCQueueFileNext->RealFileSize)
	          ret = 1;
           else if (pCQueueFileFirst->RealFileSize < pCQueueFileNext->RealFileSize)
	          ret = -1;
           else 
	          ret = 0;
		   break;
      case QUEUECOL_PROPS:
           if (pCQueueFileFirst->Prefs > pCQueueFileNext->Prefs)
	          ret = 1;
           else if (pCQueueFileFirst->Prefs < pCQueueFileNext->Prefs)
	          ret = -1;
           else 
	          ret = 0;
		   break;
      case QUEUECOL_SUBMITTED:
		   ret = pCQueueFileFirst->csSubmitted.CompareNoCase(pCQueueFileNext->csSubmitted);           
		   break;
      case QUEUECOL_STATUS:
		   ret = pCQueueFileFirst->Status.CompareNoCase(pCQueueFileNext->Status);           
		   break;
	  }
   if (QueueSortOrderUp[DrawHeaderPushCol]) ret = -ret;

   if (ret > 0)
      {
      // store it               
	  pQueueArray->InsertAt(posFirst, pCQueueFileNext);	    
      // add index of new QueueFile to ListBox
      InsertString(posFirst, pCQueueFileNext);
 	  // delete the item from the list
      DeleteItem(GetItemAt(posNext + 1));
	  // delete from the array
      pQueueArray->RemoveAt(posNext + 1);	
      return TRUE;
      }
   else
      return FALSE;
   }

#ifdef _PHZ_PRINT_SERVER_TEMP
long CSpecialTree::RPCAddToQueue(PRPCADDSTRUCT pRpcAddStruct, CQueueFile* pRPCCQueueFile)
{
	int idx;

	CQueueFile* pCQueueFile = NULL;

	idx = GetCount();	  

	// instantiate a new queue item
	if (!(pCQueueFile = new CQueueFile))
		return 1L;

	*pCQueueFile = *pCQueueFile;

	// mark it as untouched
	pCQueueFile->WasTouched = FALSE;

	// insert it into the print array
	PDOC->CQ.InsertAt(idx, pCQueueFile);	             
	InsertString(idx, pCQueueFile);   

	// make sure it's visible
	GetListCtrl().Update(idx);

	Invalidate();         

	// reset state if we are currently idle
	if (InternalState == STATE_IDLE_NO_FILES)
		SetState(STATE_IDLE_SOME_FILES);

	// our doc is modified
	PDOC->SetModifiedFlag();

	// Add(1) to the Count of Print Pane Entries
	pPrintPaneState->IncrPrintPaneEntries();

	// if the info box is on, update it
	UpdateInfo();

	// renumber the list
	RenumberList();

	// force a reshow
	ReMeasureAllItems();

	Invalidate();

	return 0l;
}

CQueueChild* CSpecialTree::GetQueueChild()
{
	CQueueChild* pTestCQueueChild = NULL;

	pTestCQueueChild = (CQueueChild*)GetParentFrame();

	return pCQueueChild;
}


CQueueFile* CSpecialTree::GetRPCQueueFile()
{
	CQueueFile* pCQueueFile = NULL;

	pCQueueFile = new CQueueFile;

	pCQueueFile->pTransform = new CTransform;
	pCQueueFile->pCD		= new CDevice;
	*(pCQueueFile->pCD)		= QUEUEDEVICE;

	return pCQueueFile;
}
#endif
void CSpecialTree::AddToQueue(CQueueFile* pCQueueFile)
   {   
   int idx;

   // add at end
   idx = GetCount();

   // mark as un-selected
   pCQueueFile->Selected = FALSE;       
   // not touched     
   pCQueueFile->WasTouched = FALSE;               
   // store it               
   pQueueArray->InsertAt(idx, pCQueueFile);	    
   // add index of new QueueFile to ListBox
   InsertString(idx, pCQueueFile);
   // reset state if we are currently idle
   if (InternalState == STATE_IDLE_NO_FILES) SetState(STATE_IDLE_SOME_FILES);
   // our doc is modified
   PDOC->SetModifiedFlag();                          
   // if the info box is on, update it
   UpdateInfo();			  
   // renumber the list
   RenumberList();  
   // force a reshow
   ReMeasureAllItems();
   Invalidate();
	
   if (iViewStyle == QUEUESTYLE_PRINT)
	  {
	  // Add(1) to the Count of Print Pane Entries
	  pPrintPaneState->IncrPrintPaneEntries();
	  }

   // start server if needed 
   if (!bPrintServerStarted) 
	  StartServer();
   else
      // restart server
      RestartServer();
   }   

void CSpecialTree::StartServer()
   {                       
   // only do this for print queue
   if (iViewStyle != QUEUESTYLE_PRINT) return;

   if ( !pPrintPaneState->IspData() )
	   pPrintPaneState->SetName(PDOC->csTitle);

   // set output type
   OutputType = RIPSTYLE_OUTPUT;
	  
   // rip ALL
   QueueType = QUEUETYPE_ALL;
   
   // do it
   Print();				
   }

void CSpecialTree::AddToLog(CQueueFile* pCQueueFile)
   {             
   CDevice* pLocalDevice;
   CTransform* pLocalTransform;
   CColorTransform* pLocalColor;
   
   // set proper devmode      
   if ((pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS) && pCQueueFile->pCD)	  
      pLocalDevice = pCQueueFile->pCD;	  
   else	  
      pLocalDevice = &QUEUEDEVICE;		  
   
   // set proper transform
   if ((pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS) && pCQueueFile->pTransform)
      pLocalTransform = pCQueueFile->pTransform;
   else
      pLocalTransform = &QUEUETRANSFORM;

   // set proper transform
   if ((pCQueueFile->Prefs & USE_LOCAL_COL_PREFS) && pCQueueFile->pCC)
      pLocalColor = pCQueueFile->pCC;
   else
      pLocalColor = &QUEUECOLOR;

   // add to the log
   PDOC->cLog.AddEntry(pCQueueFile, pLocalDevice, pLocalTransform, pLocalColor);
   if (PDOC->LogWnd.m_hWnd)
	  WriteLog();
   }		  

void CSpecialTree::OnToolsLog() 
   {   
   CProfile Profile;
   CString csLogFile;
   CFileStatus fStatus;
   CString csTitle;

   // make title
   csTitle.LoadString(IDS_RPLOG_TITLE);
   csTitle += " ";
   //csTitle += QUEUEDEVICE.Name;   
   csTitle += PDOC->csTitle;

   // get log file name   
   csLogFile = GetLogFileName();

   // set the device
   PDOC->cLog.csDevice = PDOC->csTitle;

   // write the log                  
   PDOC->cLog.WriteLog(csLogFile);

   // does the file exist?
   if (!CFile::GetStatus(csLogFile, fStatus)) return;

   // create the log window if it doesn't exist         
   if (!PDOC->LogWnd.m_hWnd)
	  {
	  // get screen dims
      PDOC->LogWnd.CreateEx(0,         
		 AfxRegisterWndClass(CS_PARENTDC,       
         ::LoadCursor(NULL, IDC_ARROW),       	            
		 NULL,
         AfxGetApp()->LoadIcon(IDI_PSOUT)),
         csTitle,         
	     WS_OVERLAPPEDWINDOW   | WS_VISIBLE | WS_MINIMIZEBOX | WS_CAPTION,	  		 
	     PDOC->crLogWinRect.left, PDOC->crLogWinRect.top,
		 PDOC->crLogWinRect.Width(), PDOC->crLogWinRect.Height(),
         AfxGetMainWnd()->GetSafeHwnd(), NULL);
	  }   

   if (PDOC->LogWnd.m_hWnd)
	  {
	  // show it
      PDOC->LogWnd.SetWindowPos(&wndTop,			
	           0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	  PDOC->LogWnd.ShowWindow(SW_SHOWNA);   	    	     	     	  
	  
      // with the url to the log
      PDOC->LogWnd.Navigate(csLogFile);
      // force our doc to be modified in case we change
	  // our window position
      PDOC->SetModifiedFlag();
	  }
   }

void CSpecialTree::OnUpdateToolsLog(CCmdUI* pCmdUI) 
   {
   // are there entries?
   if (PDOC->cLog.GetNumEntries() > 0)
	  pCmdUI->Enable(TRUE);
   else
	  pCmdUI->Enable(FALSE);	  
   }

CString CSpecialTree::GetLogFileName()
   {
   CString csRet;
   CString csDevice;      

   // get app path
   csRet = ((CZeusApp*)AfxGetApp())->GetAppPath();

   // app queue to it
   csRet += "Log\\";

   // make sure it exist
   ::CreateDirectory(csRet, NULL);

   // get device name
   //csDevice = QUEUEDEVICE.Name;
   csDevice = PDOC->csTitle;

   // make sure it has a good name
   ((CZeusApp*)AfxGetApp())->NukeBadFnameChars(csDevice);

   // add the device name to it
   csRet += csDevice;

   // and finally, add a .htm to it
   csRet += ".htm";

   return csRet;
   }

void CSpecialTree::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
   {
   BOOL bAlreadyActive = FALSE;

   // update the info      
   if (bActivate) 
   {
	  UpdateInfo();
	  if ( pActivateView == pDeactiveView ) 
		  bAlreadyActive = TRUE;
   }

   // reset the queue status   
   if (pCQueueChild)
	  {
	  if (pCQueueChild->GetSafeHwnd())
         pCQueueChild->SetQueueStatus(iLastQueueStatus);   
	  }

   if ( !bAlreadyActive )
	   CRPTreeBase:: OnActivateView(bActivate, pActivateView, pDeactiveView);
   }

void CSpecialTree::OnToolsResetlog() 
   {
   if (AfxMessageBox(IDS_RESETLOG, MB_YESNO | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_DEFBUTTON2) != IDYES)
	  return;
	  
   // delete all entries
   PDOC->cLog.DeleteAll();
   // write it
   WriteLog();
   }

void CSpecialTree::OnUpdateToolsResetlog(CCmdUI* pCmdUI) 
   {
   // are there entries?
   if (PDOC->cLog.GetNumEntries() > 0)
	  pCmdUI->Enable(TRUE);
   else
	  pCmdUI->Enable(FALSE);	  
   }

void CSpecialTree::OnEditCopy()
   {   
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW)
      PSOUT_EDIT_WINDOW->Copy();
   else
      {
	  if (GetSelectedCount() > 0)
         CopyToClipboard(OURCLIP_FILE);
	  }
   }

void CSpecialTree::OnEditPaste()
   {	
    int i, idx;            
    CQueueFile* pCQueueFile;  
	
	if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) return; 
    
	// get row to insert at   	  
	idx = ((GetCount() == 0) ? 0 : ItemIndex(GetCaretItem()) + 1);
	
   // available?
   if (CLIPARRAY.IsTypeAvail(OURCLIP_FILE))
      {
      // get all elements and insert them
      for (i = 0; i < CLIPARRAY.GetSize(); i++)
         {	                  
		 if (((((CQueueFile*)(CLIPARRAY.GetAt(i)))->StatusFlag & FILE_STATUS_CHECK) == 
			FILE_STATUS_OK_DELETED)	&& (iViewStyle != QUEUESTYLE_ARCHIVE))
			{
			AfxMessageBox(IDS_NOPASTE_DELETED, MB_SETFOREGROUND);
			continue;
			}
         // create a new queue file
         pCQueueFile = new CQueueFile;
         // copy the data
         *pCQueueFile = *((CQueueFile*)(CLIPARRAY.GetAt(i)));

 		 // don't bring over a foreign device
		 if (pCQueueFile->pCD)
			{
			if (pCQueueFile->pCD->DevIdx != QUEUEDEVICE.DevIdx)
			   {
			   // don't do device
			   if (pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS)
				  {
				  delete pCQueueFile->pCD;
				  pCQueueFile->pCD = NULL;
				  pCQueueFile->Prefs -= USE_LOCAL_DEV_PREFS;        
				  }
			   }
			}

		 // don't bring over foreign color
		 if (pCQueueFile->pCC)
			{
			// don't do color
			if (pCQueueFile->Prefs & USE_LOCAL_COL_PREFS)
			   {			
			   if (pCQueueFile->pCC->CPInitStruct.GlobalPrinterID != QUEUEDEVICE.DevIdx)
				  {
				  delete pCQueueFile->pCC;
				  pCQueueFile->pCC = NULL;
				  pCQueueFile->Prefs -= USE_LOCAL_COL_PREFS;        
				  }
			   }
			}

	     // in what queue?
		 pCQueueFile->InQueue = iViewStyle;
		 // this is a new drop, so mark as not touched
         pCQueueFile->WasTouched = FALSE;			  

		 // remove the inrip flag if it's there
		 if ((pCQueueFile->StatusFlag & FILE_STATUS_INRIP) == FILE_STATUS_INRIP) 
			pCQueueFile->StatusFlag -= FILE_STATUS_INRIP;

		 // if dropping to the hold queue, reset the status
		 if (iViewStyle == QUEUESTYLE_HOLD)
			{
            pCQueueFile->Status.LoadString(IDS_READY);
            pCQueueFile->PreviewStatus.LoadString(IDS_READY);
            pCQueueFile->StatusFlag |= FILE_STATUS_HOLD;
			pCQueueFile->ctsElapsedTime = 0;
			}		 
		 // if dropping to the archive, lock in the atts
		 else if (iViewStyle == QUEUESTYLE_ARCHIVE)
			{
			// if no local prefs, lock them in now   
			if (!(pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS))
			   {
			   pCQueueFile->pCD = new CDevice;
			   *(pCQueueFile->pCD) = QUEUEDEVICE;
			   pCQueueFile->Prefs |= USE_LOCAL_DEV_PREFS;        
			   }
			if (!(pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS))
			   {
			   pCQueueFile->pTransform = new CTransform;
			   *(pCQueueFile->pTransform) = QUEUETRANSFORM;
			   pCQueueFile->Prefs |= USE_LOCAL_TRN_PREFS;        
			   }
			if (!(pCQueueFile->Prefs & USE_LOCAL_COL_PREFS))
			   {
			   pCQueueFile->pCC = new CColorTransform;
			   *(pCQueueFile->pCC) = QUEUECOLOR;
			   pCQueueFile->Prefs |= USE_LOCAL_COL_PREFS;        
			   }
			}
		 // save this
		 pCQueueFile->iOldJobID = pCQueueFile->iJobID;
		 // and generate a new one in case the user is making a copy
		 pCQueueFile->iJobID = ((CZeusApp*)AfxGetApp())->GetJobID();
		 
		 pQueueArray->InsertAt(idx, pCQueueFile);	             
		 InsertString(idx, pCQueueFile);			
         }	   
      // reset state if we are currently idle
      if (InternalState == STATE_IDLE_NO_FILES) SetState(STATE_IDLE_SOME_FILES);   
      // our doc is modified
      PDOC->SetModifiedFlag();
	  Invalidate();	  
      // if the info box is on, update it
	  UpdateInfo();			  	      
	  }


   // renumber the list
   RenumberList();

   // force a reshow
   ReMeasureAllItems();
   Invalidate();

   // start server if needed 
   if (!bPrintServerStarted) 
	  StartServer();
   else
      // restart server?
      RestartServer();
   }

void CSpecialTree::OnEditDelete() 
   {
   // call the base class implementation
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW)
      {
	  PSOUT_EDIT_WINDOW->SetReadOnly(FALSE);
	  PSOUT_EDIT_WINDOW->Clear();	  
	  PSOUT_EDIT_WINDOW->SetReadOnly(TRUE);
	  }
   else
      DeleteFiles();	
   }

void CSpecialTree::OnEditCut()
   {   
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW)
      {	  
	  PSOUT_EDIT_WINDOW->SetReadOnly(FALSE);
	  PSOUT_EDIT_WINDOW->Cut();	  
	  PSOUT_EDIT_WINDOW->SetReadOnly(TRUE);
	  }
   else
      {
	  // first copy the files
      OnEditCopy();          
      // now delete the files
      DeleteFiles();
	  // restore the old jobids
	  RestoreOldJobIDs();
	  }
   }

void CSpecialTree::OnSelectall()
   {   
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW)
      PSOUT_EDIT_WINDOW->SetSel(0, -1);
   else
      {
	  // if no files, return
      if (GetCount() <= 0) return;   
    
      // select all items      
	  GetListCtrl().SelectItemRange(0, GetCount() - 1, TRUE);
    
      // update the data
      UpdateInfo();		
	  }
   }                               

void CSpecialTree::OnUpdateEditCut(CCmdUI* pCmdUI)
   {   	
   // sneak this hack in here because it doesn't work anywhere else
   // doing it here assures us that it will happen after just about every 
   // UI element is drawn
   if (pCQueueChild && !bResizeHack) 
	  {
	  pCQueueChild->SendMessage(wm_ResizeRPWindowMsg, 0, 0);
	  bResizeHack = TRUE;
	  }

   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW)
      pCmdUI->Enable(PSOUT_HAVE_SELTEXT);
   else
      pCmdUI->Enable(GetSelectedCount() > 0 ? TRUE : FALSE);		
   }

void CSpecialTree::OnUpdateEditCopy(CCmdUI* pCmdUI)
   {   	
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW)
      pCmdUI->Enable(PSOUT_HAVE_SELTEXT);
   else
      pCmdUI->Enable(GetSelectedCount() > 0 ? TRUE : FALSE);	
   }

void CSpecialTree::OnUpdateEditPaste(CCmdUI* pCmdUI)
   {
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      {	  
      pCmdUI->Enable(FALSE);
	  return;
	  }

   if (CLIPARRAY.IsTypeAvail(OURCLIP_FILE))
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
   }

void CSpecialTree::OnUpdateEditSelectall(CCmdUI* pCmdUI)
   {
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW)
      pCmdUI->Enable(!PSOUT_EMPTY);
   else
      pCmdUI->Enable(GetCount() > 0 ? TRUE : FALSE);		
   }

int CSpecialTree::GetAvailHeight()
   {
   int iTotalHeight;
   CRect crStatusRect;
   CWnd* pStatusWindow;

   GetParent()->GetClientRect(crStatusRect);
   iTotalHeight = crStatusRect.Height();

   pStatusWindow = AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);   	  
   if (pStatusWindow != NULL)
	  {
      pStatusWindow->GetClientRect(crStatusRect);         
      iTotalHeight -= crStatusRect.Height();
	  }    

   return iTotalHeight;
   }


void CSpecialTree::EditExistingBatchHeader(CQueueFile* pCQueueFile)
   {           
   CBheader cb;                 
   
   // set existing options      
   cb.m_date = (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_DATE) ? TRUE : FALSE;
   cb.m_time = (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_TIME) ? TRUE : FALSE;
   cb.m_imagecorners = (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_CORNERS) ? TRUE : FALSE;
   cb.m_show_imagecount = (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_COUNT) ? TRUE : FALSE;
   if (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_BACKWHITE)
      cb.m_background = BATCHHEADER_BACKGROUND_WHITE;
   else if (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_BACKBLACK)
      cb.m_background = BATCHHEADER_BACKGROUND_BLACK;
   else
      cb.m_background = BATCHHEADER_BACKGROUND_NONE;
           
   // get notes
   if(pCQueueFile->BatchHeaderNotes.GetLength()) 
      {                                            
      // if there is a tab, extract the first part                                             
      int TabPos;
      if ((TabPos = pCQueueFile->BatchHeaderNotes.Find("\t")) > 0)
         {
         cb.m_notes1 = pCQueueFile->BatchHeaderNotes.Left(TabPos);
         cb.m_notes2 = 
            pCQueueFile->BatchHeaderNotes.Right(pCQueueFile->BatchHeaderNotes.GetLength() - TabPos - 1);
         }
      else
         {   
         cb.m_notes1 = pCQueueFile->BatchHeaderNotes;
         cb.m_notes2 = _T("");
         }
      }   

   // do the dialog stuff   
   DoBatchHeaderDialog(cb, pCQueueFile->BatchHeaderCount, pCQueueFile);

   PDOC->SetModifiedFlag();
   }   

void CSpecialTree::OnEditInsertBatchHeader()
   {
   int i, idx;
   CQueueFile* pCQueueFile;
   int ImageCount;
   CBheader cb;

   // get row to insert at   	  
   idx = ((GetCount() == 0) ? 0 : ItemIndex(GetCaretItem()) + 1);
      
   // set the number of images from here   
   if (idx == 1)
      ImageCount = GetCount();
   else
	  ImageCount = GetCount() - idx;

   // get next bheader
   for (i = idx; i < GetCount(); i++)
      {
      pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(i));
      // the next blist is here so the count is up to this
      if (pCQueueFile->FileType == FTYPE_BLIST)
         {
         ImageCount = i;
         break;
         }
      }   
   
   // create a new queue file object
   if (!(pCQueueFile = new CQueueFile)) return;      
   if (!(pCQueueFile->InitBListFile())) return;         
   
   // if user cancels, delete, else
   // if OK, add index of new QueueFile to ListBox
   if (DoBatchHeaderDialog(cb, ImageCount, pCQueueFile) == IDOK)
      {
	  pQueueArray->InsertAt(idx, pCQueueFile);	             
	  InsertString(idx, pCQueueFile);
	  // not touched     
	  pCQueueFile->WasTouched = FALSE;               
      // mark as selected
	  SelectItem(GetItemAt(idx));      
      // if the info box is on, update it
      UpdateInfo();			  
	  // reset state if we are currently idle
      if (InternalState == STATE_IDLE_NO_FILES) SetState(STATE_IDLE_SOME_FILES);
	  // renumber the list
	  RenumberList();  
      }
   else   
      delete pCQueueFile;
   
   // restart server?
   RestartServer();
   
   PDOC->SetModifiedFlag();
   }      
   
int CSpecialTree::DoBatchHeaderDialog(CBheader& cb, int ImageCount, 
   CQueueFile* pCQueueFile)
   {              
   CProfile Profile;
   char NumStr[10];
   
   // default image count is queue size
   cb.m_edit_imagecount = itoa(ImageCount, NumStr, 10);

   // get default background
   cb.m_background = Profile.GetInt(IDS_SERVER, "BatchHeaderBackground", 
	  BATCHHEADER_BACKGROUND_BLACK);
   
   // show the dialog
   if (cb.DoModal() == IDCANCEL) return (IDCANCEL);   
   
   // start with nothing, in case we are re-editing
   pCQueueFile->BatchHeaderOptions = 0;
   
   // get new settings
   if (cb.m_date)
      pCQueueFile->BatchHeaderOptions |= OPTIONS_BATCHHEADER_DATE;
   if (cb.m_time)
      pCQueueFile->BatchHeaderOptions |= OPTIONS_BATCHHEADER_TIME;
   if (cb.m_imagecorners)
      pCQueueFile->BatchHeaderOptions |= OPTIONS_BATCHHEADER_CORNERS;
   if (cb.m_show_imagecount)
      pCQueueFile->BatchHeaderOptions |= OPTIONS_BATCHHEADER_COUNT;
   if (cb.m_notes1.GetLength() > 0)
	  {
	  pCQueueFile->BatchHeaderNotes = cb.m_notes1;    
      pCQueueFile->Description = cb.m_notes1;    
	  }
   if (cb.m_notes2.GetLength() > 0)
      {
      pCQueueFile->BatchHeaderNotes += "\t"; // add a tab
      pCQueueFile->BatchHeaderNotes += cb.m_notes2;    
      }
   pCQueueFile->BatchHeaderCount = atoi(cb.m_edit_imagecount);
   
   // get background
   switch(cb.m_background)
      {	   
      case BATCHHEADER_BACKGROUND_WHITE:
           pCQueueFile->BatchHeaderOptions |= OPTIONS_BATCHHEADER_BACKWHITE;
           break;
      case BATCHHEADER_BACKGROUND_BLACK:
           pCQueueFile->BatchHeaderOptions |= OPTIONS_BATCHHEADER_BACKBLACK;
           break;
      case BATCHHEADER_BACKGROUND_NONE:
           pCQueueFile->BatchHeaderOptions |= OPTIONS_BATCHHEADER_BACKNONE;
           break;
      }              

   // write default background
   Profile.WriteInt(IDS_SERVER, "BatchHeaderBackground", cb.m_background);
   
   return(IDOK);   
   }							 								 

void CSpecialTree::OnUpdateEditAddbatchheader(CCmdUI* pCmdUI) 
   {
   // don't allow this in archive
   if (iViewStyle == QUEUESTYLE_ARCHIVE)
      pCmdUI->Enable(FALSE);
   else
      pCmdUI->Enable(TRUE);
   }

void CSpecialTree::OnToolsArchivelog() 
   {
   CString csArchive;
   CString csLog;
   CString szFilter;
   CString csPath;
   CString csTitle;
   CProfile Profile;

   // construct a file save dialog
   szFilter =        
	   "HTML (*.htm)|*.htm|"
	   "All Files (*.*)|*.*||";						   
   CFileDialog cfd(FALSE, "htm", NULL, 
	  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);

   // get instance handle
   cfd.m_ofn.hInstance = AfxGetInstanceHandle();                
   
   // open file title
   csTitle.LoadString(IDS_ARCHIVELOG_DLGPRMPT);
   cfd.m_ofn.lpstrTitle = csTitle;

   // get app path
   csPath = Profile.GetStr(IDS_SERVER, IDS_APPDIR);
   VERIFYPATH(csPath);
   // app queue to it
   csPath += "Log\\";
   						 
   // set initial dir
   cfd.m_ofn.lpstrInitialDir = csPath;

   // show the dialog
   if (cfd.DoModal() != IDOK) return;   

   // get the filenames
   csLog = GetLogFileName();
   csArchive = cfd.GetPathName();
   // copy it
   if (CopyFile(csLog, csArchive, TRUE))
      AfxMessageBox(IDS_LOG_ARCHIVE_OK, MB_SETFOREGROUND);
   else
	  {
	  AfxMessageBox(IDS_LOG_ARCHIVE_ERROR, MB_SETFOREGROUND);
	  return;
	  }

   if (AfxMessageBox(IDS_RESETLOG, MB_YESNO | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_DEFBUTTON2) != IDYES)
	  return;
	  
   // delete all entries
   PDOC->cLog.DeleteAll();
   
   // write it
   WriteLog();
   }

void CSpecialTree::OnUpdateEditResetprops(CCmdUI* pCmdUI) 
   {
   if (iViewStyle != QUEUESTYLE_ARCHIVE && GetSelectedCount() > 0)	  
	  pCmdUI->Enable(TRUE);
   else
	  pCmdUI->Enable(FALSE);	  
   }

void CSpecialTree::OnEditResetprops()
   {
   ResetProps(FALSE);
   }

void CSpecialTree::ResetProps(BOOL bDoAll)
   {
   int nArrIdx, NumSelected;   
   CQueueFile* pCQueueFile;   
   CDWordArray *pArrSelected = NULL;
   DWORD dwIndex;   
   int iStartIndex;

   if (bDoAll)
	  {	  
	  iStartIndex = GetCount() - 1;
	  }
   else
	  {
      // get selection array
      pArrSelected = GetSelectionArray();

      // how many are selected?
      NumSelected = pArrSelected->GetSize();   
    
      // if we have no items, return
      if (NumSelected < 1) return;

	  iStartIndex = pArrSelected->GetUpperBound();
	  }
   
   // get the items
   for (nArrIdx = iStartIndex; nArrIdx >= 0; --nArrIdx)
      {  
	  // get the index
	  if (bDoAll)
		 dwIndex = nArrIdx;
	  else
		 dwIndex = pArrSelected->GetAt(nArrIdx);

	  pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(dwIndex));

	  // can't reset if it's being ripped
	  if ((pCQueueFile->StatusFlag & FILE_STATUS_INRIP) == FILE_STATUS_INRIP)
		 continue;

	  // if we are using local dev prefs, reset them
	  if (pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS)
		 {
         delete pCQueueFile->pCD;               
         pCQueueFile->pCD = NULL; 
         pCQueueFile->Prefs -= USE_LOCAL_DEV_PREFS;
		 }										   
	  // transform
	  if (pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS)
	     {
		 // alloc new transform
		 delete pCQueueFile->pTransform;
		 pCQueueFile->pTransform = NULL;
		 pCQueueFile->Prefs -= USE_LOCAL_TRN_PREFS;
		 }
	  // color
	  if (pCQueueFile->Prefs & USE_LOCAL_COL_PREFS)
	     {
		 delete pCQueueFile->pCC;
		 pCQueueFile->pCC = NULL;
	     pCQueueFile->Prefs -= USE_LOCAL_COL_PREFS;
		 }      
      // redraw it   
      GetListCtrl().Update(nArrIdx);
	  }

   // force a reshow
   ReMeasureAllItems();
   Invalidate();   	   
   // our doc is modified   
   PDOC->SetModifiedFlag();
   // if the info box is on, update it
   UpdateInfo();			  
   }

LRESULT CSpecialTree::OnResizePane(WPARAM wParam, LPARAM lParam)
   {
   int iTotalHeight = GetAvailHeight();
   
   // auto size it
   if (iTotalHeight > 0)
	  {	  
	  ((CSplitterWnd*)GetParent())->SetRowInfo(iViewStyle, iTotalHeight / 3, 0);
	  ((CSplitterWnd*)GetParent())->RecalcLayout();
	  PDOC->SetModifiedFlag();	  	  
	  }
   return 0;
   }

void CSpecialTree::OnUpdatePreferencesLocal(CCmdUI* pCmdUI) 
   {					   
   pCmdUI->Enable(GetSelectedCount() > 0);	
   }

void CSpecialTree::OnUpdatePreferencesQueue(CCmdUI* pCmdUI) 
   {					   
   //pCmdUI->Enable(CmdQueuePreferences);	
   pCmdUI->Enable(TRUE);	
   }

void CSpecialTree::OnPreferencesLocal() 
   {	
   // if it's not visible and we have files
   if ((!m_pPropFrame->IsWindowVisible()) && (GetSelectedCount() > 0))
      {
	  // we need to run the UpdateInfo machinery here if we are not visible 
	  // so that we do some initial setup to avoid crashing
	  if (!m_pPropFrame->m_pModelessPropSheet->IsWindowVisible()) 
		 UpdateInfo();
	  // set our active page
	  m_pPropFrame->m_pModelessPropSheet->SetOurActivePage();
      // show it
   	  m_pPropFrame->ShowWindow(SW_SHOWNA);
   	  }            
   // update the info      
   UpdateInfo();
   }

void CSpecialTree::OnUpdateToolsPrintlog(CCmdUI* pCmdUI) 
   {
   // are there entries?
   if (PDOC->cLog.GetNumEntries() > 0 && PDOC->LogWnd.m_hWnd)
	  pCmdUI->Enable(TRUE);
   else
	  pCmdUI->Enable(FALSE);	  
   }

void CSpecialTree::OnUpdateToolsLicenseManager(CCmdUI* pCmdUI) 
   {
   // are there entries?
   if (PDOC)
	  pCmdUI->Enable(TRUE);
   else
	  pCmdUI->Enable(FALSE);	  
   }

void CSpecialTree::OnToolsPrintlog()
   {   
   PDOC->LogWnd.Print();
   }

void CSpecialTree::OnToolsLicenseManager() 
{
	CLicenseManager licenseDlg;

	licenseDlg.DoModal();
}

void CSpecialTree::OnUpdateToolsArchivelog(CCmdUI* pCmdUI) 
   {
   // are there entries?
   if (PDOC->cLog.GetNumEntries() > 0 && PDOC->LogWnd.m_hWnd)
	  pCmdUI->Enable(TRUE);
   else
	  pCmdUI->Enable(FALSE);	  
   }

void CSpecialTree::NotifyViewClose()
   {
   // if archive, get bar states
   if (iViewStyle == QUEUESTYLE_ARCHIVE)
	  {
      CQueueChild* pCQueueChild = (CQueueChild*)GetParentFrame();

	  // get bar states
      CControlBar* pBar;
      pBar = pCQueueChild->GetControlBar(CG_ID_VIEW_PSOUT);
      if (pBar != NULL) PDOC->bPSoutState = (pBar->GetStyle() & WS_VISIBLE);
      pBar = pCQueueChild->GetControlBar(CG_ID_VIEW_QUEUESTATUS);
      if (pBar != NULL) PDOC->bQStatusState = (pBar->GetStyle() & WS_VISIBLE);
	  // make sure we save these
	  PDOC->SetModifiedFlag();
	  }
   }

int CSpecialTree::AnyJobProps()
   {
   int nArrIdx;   
   CQueueFile* pCQueueFile;         
   BOOL iRet = 0;

   // get the items
   for (nArrIdx = GetCount() - 1; nArrIdx >= 0; --nArrIdx)
      {  
	  // get the job
	  pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(nArrIdx));

	  // if we are using local dev prefs, set flag
	  if (pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS ||
		  pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS ||
		  pCQueueFile->Prefs & USE_LOCAL_COL_PREFS)
		 {
		 iRet = 1;
		 break;
		 }										   
	  }
   return iRet;
   }

void CSpecialTree::OnFileReinstallprinter() 
   {
   ReinstallAppDriver();
   }

void CSpecialTree::OnFileAppdriverConfig() 
   {
   HANDLE hPrinter;   
   CString csName;
   DEVMODE* pDevMode;
   int iDevSize = 0;
   PRINTER_DEFAULTS pd;
   pd.pDatatype = NULL;
   pd.pDevMode = NULL;
   pd.DesiredAccess = PRINTER_ALL_ACCESS;
   PRINTER_INFO_2* pi2;
   DWORD cbNeeded;
   LPBYTE lpBuf;   

   // get device name
   csName = PDOC->GetTitle();
   
   // open the printer
   if (::OpenPrinter((char*)(const char*)csName, &hPrinter, &pd))
	  {
      if (hPrinter) 
	     {
		 // get size
		 iDevSize = ::DocumentProperties(GetSafeHwnd(), hPrinter, 
			(char*)(const char*)csName, NULL, NULL, 0);			
		 if (iDevSize > 0)
			{
			// do properties
			if (pDevMode = (PDEVMODE)(new BYTE [iDevSize]))
			   {
			   // prompt for new values and get the current devmode
			   if (::DocumentProperties(GetSafeHwnd(), hPrinter, (char*)(const char*)csName, 
			      pDevMode, NULL, DM_IN_PROMPT | DM_OUT_BUFFER) == IDOK)
				  {
				  // write the new devmode
			      ::DocumentProperties(GetSafeHwnd(), hPrinter, (char*)(const char*)csName, 
			         pDevMode, pDevMode, DM_IN_BUFFER | DM_OUT_BUFFER | DM_UPDATE);
				  
				  // get the printer to update it globally
				  ::GetPrinter(hPrinter, 2, NULL, 0, &cbNeeded);
		          // make the buffer equal to the old plus the new port info
		          if (lpBuf = (LPBYTE)(new char[cbNeeded]))
					 {
		             if (::GetPrinter(hPrinter, 2, lpBuf, cbNeeded, &cbNeeded))
						{
		                // point to the buffer
		                pi2 = (PRINTER_INFO_2*)lpBuf;
		                // set the new port	
		                memcpy(pi2->pDevMode, pDevMode, iDevSize);
		                // do it
		                ::SetPrinter(hPrinter, 2, lpBuf, 0);
						}
					 delete lpBuf;
					 }
			      delete pDevMode;
				  }
			   }
			}		 		 
		 }
	  if (hPrinter) ::ClosePrinter(hPrinter);	
	  }  												
   }

void CSpecialTree::WriteLog()
   {
   CString csLog;
   
   // get log file name
   csLog = GetLogFileName();

   // set the device
   PDOC->cLog.csDevice = PDOC->csTitle;

   // write it
   PDOC->cLog.WriteLog(csLog);	

   // if valid window and it is visible, refresh it
   if (PDOC->LogWnd.m_hWnd)
	  {
      if (PDOC->LogWnd.IsWindowVisible())
		 {         
    	 // refresh if necessary
         PDOC->LogWnd.Refresh();
		 }
	  }
   }

void CSpecialTree::OnUpdateEditJobdescription(CCmdUI* pCmdUI) 
   {
   pCmdUI->Enable((GetSelectedCount() >= 1) ? TRUE : FALSE);
   }

#if 0
BOOL CSpecialTree::OnDeviceChange(UINT nEventType, DWORD dwData)
   {
   PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)dwData;
   char szMsg[180];
      
   if (nEventType == DBT_DEVICEREMOVECOMPLETE)
	  {
	  // See if a CD-ROM was removed from a drive.
	  if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
		 {
		 PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
		 if (lpdbv->dbcv_flags & DBTF_MEDIA)
			{
			wsprintf (szMsg, "Drive %c: was removed\n",
			   FirstDriveFromMask(lpdbv->dbcv_unitmask));
			AfxMessageBox (szMsg);
			}
		 }
	  }
   return TRUE;
   }

char CSpecialTree::FirstDriveFromMask(ULONG unitmask)
   {
   char i;
   
   for (i = 0; i < 26; ++i)
      {
	  if (unitmask & 0x1)
		 break;
	  unitmask = unitmask >> 1;
      }
   return (i + 'A');
   }
#endif

void CSpecialTree::RestoreOldJobIDs()
   {
   int i;
   CQueueFile* pCQueueFile;   

   // renumber the list   
   for (i = 0; i < GetCount(); i++)      
      {	        
	  // get info	  	  
	  pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(i));
	  // restore the old job id
	  pCQueueFile->iJobID = pCQueueFile->iOldJobID;
	  }
   }

void CSpecialTree::OnDestroy() 
   {
   // in installandexit, we have already deleted the modeless prop sheet
   // let's check
   if (m_pPropFrame->GetSafeHwnd())
	  {
      if (!m_pPropFrame->bDeleted)
		 {
		 // destroy the window
         DestroyJobPropsDlg();		 		 
		 }
	  }

   // if the log was created and we own it, delete it
   if (PDOC->LogWnd.m_hWnd)
	  {
	  // nuke it
	  PDOC->LogWnd.DestroyWindow();
	  // make sure
	  PDOC->LogWnd.m_hWnd = NULL;	  
	  }
   CRPTreeBase::OnDestroy();	
   }


void CSpecialTree::ReinstallAppDriver( BOOL bAutoPrompt ) 
{
	BOOL NeedToReboot = FALSE;

	CInstallPrinter cins;

	CString csErr;
	CString csFriendly;
	CString csPort;
	CString csPrompt;
	CString csTemp;

	// get the friendly name
	csFriendly = PDOC->GetTitle();

	if ( bAutoPrompt )
		csPrompt.Format(IDS_MEDIA_CHANGE_REINSTALL, csFriendly);
	else
		csPrompt.Format(IDS_REINSTALL_PROMPT, csFriendly);

	if (AfxMessageBox(csPrompt, MB_YESNO | MB_SETFOREGROUND | MB_DEFBUTTON2) == IDNO)
		return;

	AfxGetApp()->DoWaitCursor(1);

	// first, delete the current set of ppd and pcf files
	((CZeusApp*)AfxGetApp())->DeleteAllPPDPCFFiles(QUEUEDEVICE.Name, csFriendly, QUEUEDEVICE.DevIdx);

	// get the connection
	csPort = QUEUEDEVICE.pDevInfo->ConnectStr;

	// re-install the printer
	if (!cins.InstallPrinter(QUEUEDEVICE.Name, csFriendly, QUEUEDEVICE.DevIdx, csPort, 
								csErr, NeedToReboot, FALSE, FALSE, FALSE))
	{
		AfxGetApp()->DoWaitCursor(-1);
		// whoops
		csTemp.LoadString(IDS_ERROR_INSTALL_PRINTER);
		csPrompt.Format("%s\n%s", csTemp, csErr);
		AfxMessageBox(csPrompt, MB_SETFOREGROUND);
	}
	else
		AfxGetApp()->DoWaitCursor(-1);

	// do we need to reboot?
	if (NeedToReboot)
		AfxMessageBox(IDS_NEEDTOREBOOT, MB_SETFOREGROUND);
}

int CSpecialTree::CheckImageSize(RECT rImageRect)
{
	BOOL bFoundMedia = FALSE;

	CDevice* pDefaultDevice;

	float fMediaWidth;
	float fMediaHeight;

	float fMediaWidthHigh;
	float fMediaWidthLow;
	float fMediaHeightHigh;
	float fMediaHeightLow;

	float fImageWidth;
	float fImageHeight;

	int iSelectedMedia;

	// Calculate Image Height
	if ( rImageRect.top > rImageRect.bottom )
		fImageHeight = (float(rImageRect.top - rImageRect.bottom) / PDOC->crrrs.iHotFolderDPI);
	else
		fImageHeight = (float(rImageRect.bottom - rImageRect.top) / PDOC->crrrs.iHotFolderDPI);

	// Calculate Media Width
	if ( rImageRect.left > rImageRect.right )
		fImageWidth = (float(rImageRect.top - rImageRect.right) / PDOC->crrrs.iHotFolderDPI);
	else
		fImageWidth = (float(rImageRect.right - rImageRect.left) / PDOC->crrrs.iHotFolderDPI);

	MEDIA Media;	 
	pDefaultDevice = &QUEUEDEVICE;

	iSelectedMedia = 0;
	while ( bFoundMedia == FALSE && iSelectedMedia <= pDefaultDevice->pDevInfo->TotalMedia )
	{
		pDefaultDevice->GetMedia(iSelectedMedia, &Media);

		// Calculate Media Height
		if ( Media.Rect.top > Media.Rect.bottom )
			fMediaHeight = ((float)Media.Rect.top - (float)Media.Rect.bottom) / 100.0f;
		else
			fMediaHeight = ((float)Media.Rect.bottom - (float)Media.Rect.top) / 100.0f;

		// Calculate Media Width
		if ( Media.Rect.left > Media.Rect.right )
			fMediaWidth = ((float)Media.Rect.top - (float)Media.Rect.right) / 100.0f;
		else
			fMediaWidth = ((float)Media.Rect.right - (float)Media.Rect.left) / 100.0f;

		fMediaWidthHigh		= fMediaWidth + PDOC->crrrs.iHotFolderVariance;
		fMediaWidthLow		= fMediaWidth - PDOC->crrrs.iHotFolderVariance;
		fMediaHeightHigh	= fMediaHeight + PDOC->crrrs.iHotFolderVariance;
		fMediaHeightLow		= fMediaHeight - PDOC->crrrs.iHotFolderVariance;

		if ( fMediaWidth > fMediaHeight )
		{
			if ( fImageWidth > fImageHeight )
			{
				if ( ((fImageWidth <= fMediaWidthHigh) && (fImageWidth >= fMediaWidthLow)) &&
						((fImageHeight <= fMediaHeightHigh) && (fImageHeight >= fMediaHeightLow)) )
					bFoundMedia = TRUE;
				else
					bFoundMedia = FALSE;
			}
			else
			{
				if ( ((fImageWidth <= fMediaHeightHigh) && (fImageWidth >= fMediaHeightLow)) &&
						((fImageHeight <= fMediaWidthHigh) && (fImageHeight >= fMediaWidthLow)) )
					bFoundMedia = TRUE;
				else
					bFoundMedia = FALSE;
			}
		}
		else
		{
			if ( fImageWidth < fImageHeight )
			{
				if ( ((fImageWidth <= fMediaWidthHigh) && (fImageWidth >= fMediaWidthLow)) &&
						((fImageHeight <= fMediaHeightHigh) && (fImageHeight >= fMediaHeightLow)) )
					bFoundMedia = TRUE;
				else
					bFoundMedia = FALSE;
			}
			else
			{
				if ( ((fImageWidth <= fMediaHeightHigh) && (fImageWidth >= fMediaHeightLow)) &&
						((fImageHeight <= fMediaWidthHigh) && (fImageHeight >= fMediaWidthLow)) )
					bFoundMedia = TRUE;
				else
					bFoundMedia = FALSE;
			}
		}

		if ( bFoundMedia == FALSE )
			iSelectedMedia++;
	}

	if ( bFoundMedia == FALSE )
		iSelectedMedia = -1;
	return iSelectedMedia;
}