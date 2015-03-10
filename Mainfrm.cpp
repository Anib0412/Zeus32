////////////////////////////////////////////////////////////////////////////
// MAINFRM.CPP
//                                                                        
// main frame class for RasterPlus
// Copyright (C) 1996-2001 Graphx, Inc.
//
// mods:
//
// 7-11-96  Build 40 - don't allow close of mainfrm if printing
// 10-17-96 Build 45 - add Web address to Help
// 10-21-97 Build 52 - added new server stuff
// 11-03-97 Build 53 - reactivate last window in IsRipActive search
// 11-09-97 Build 53 - changed way we look for active rips
// 11-23-97 Version 2.0 - problem with active rip class destruction - fixed
// 02-20-98 Version 2.0.2 - added online registration and mnemonics for web menu item
// 06-24-98 Version 2.0.4 - move mutex handling from language monitor to RP95
// 10-20-98 Version 2.0.5 - can now distinguish connection on file print
// 12-01-99 Version 3.0.2 - added Variable Tool Support
// 12-01-99 Version 3.0.2 - only enable dongle utility if this is a dongle version
// 12-09-99 Version 3.0.2 - focus fix (see OnActivate)
// 02-07-00 Version 3.0.3 - Fixed problem with separators being added to the Tools menu when New printers are added(DJE)
// 04-18-00 Version 3.0.3 - Fixed Netscape WebUpdate Problem, also WebRegistration info not being passed
// 01-23-00 Version 4.0.2 - add timer code to check awake status of gis
// 03-21-01 Version 4.0.3 - move killtimer to destroy window code to get rid of assert
// 06-15-01 Version 4.0.3 - invalid TimerID might be getting kill'd, we now zero it out
// 06-15-01 Version 4.0.3 - add code to support Photogize PrintServer toggle
// 09-28-01 Version 5.0.0 - Added High_Defintion Preview support
// 10-10-01 Version 5.0.0 - implement an open queue counter
// 12-27-02 Version 5.1.1 - Add Remote Diagnostic capability
//                          Attempt to Launch browser in new browser window
// 07-31-03 Version 5.1.6 - Add Changes Log display on help menu
// 08-24-04 Version 5.2.6 - Add Help Desk to help menu

#include "stdafx.h"
#include "zeus.h"
#include "mainfrm.h"
#include "inidefs.h"
#include "prefsmsg.h"
#include "rip.h"
#include "prev.h"
#include "profile.h"
#include "GlobalProperties.h"
#include "zeusdoc.h"
#include "cqueue.h"
#include "SCSIExplorer.h"
#include "..\CmnSrc\scsi\scsi.h"
#include "OurWorkspace.h"
#include "SpecialTree.h"
#include "WebUpdate.h"
#include "objects.h"

// Required for QuickScan
#include "scandialog.h"

#include "..\CmnLibs\Current LeadTools\include\l_bitmap.h"
#include "..\CmnLibs\Current LeadTools\include\l_error.h"

DWORD _cdecl PGPRCThreadProc(LPVOID pParam);
DWORD _cdecl PFSThreadProc(LPVOID pParam);
DWORD WINAPI FileServerInstanceThread(LPVOID pParam);
DWORD WINAPI FileServerThread(LPVOID); 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// register private message       
const UINT NEAR wm_PrevSetInfo = RegisterWindowMessage( PREV_SETINFO );
const UINT NEAR wm_AddJobToQueue = RegisterWindowMessage( ADDSERVERFILEMSG );
const UINT NEAR wm_ServerFileEOF = RegisterWindowMessage( SERVERFILEEOFMSG );
const UINT NEAR wm_ResizePane = RegisterWindowMessage( "RPResize_Pane" );

#define QUEUETRANSFORM pDoc->QueueTransform
#define QUEUEDEVICE    pDoc->QueueDevice
#define QUEUECOLOR     pDoc->QueueColor

// up to 100 queues!  (should be enough)
#define QUEUE_ID_SEPARATOR	  42000
#define MIN_QUEUE_ID		  42001
#define MAX_QUEUE_ID		  42101

#define TOOL_ID_SEPARATOR	  42200
#define MIN_TOOL_ID		      42201
#define MAX_TOOL_ID		      42301

// Prototype for ENUMSOURCECALLBACK FUNCTION
L_INT FAR EXPORT EXT_CALLBACK EnumScanners( L_UCHAR FAR * pBuffer,
										 L_INT uBytes, L_VOID FAR *pUserData );

#define RP_SOCKET_TIMER_ID          399
#define RP_SOCKET_TIMER_INTERVAL    10000

#define BUFSIZE 4096

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, SECWorkbook)

BEGIN_MESSAGE_MAP(CMainFrame, SECWorkbook)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_REGISTERED_MESSAGE(wm_PrevSetInfo, OnPrevSetInfo)			
	ON_WM_CREATE()		
	ON_REGISTERED_MESSAGE(wm_AddJobToQueue, OnAddJobToQueue)
	ON_REGISTERED_MESSAGE(wm_ServerFileEOF, OnServerFileEOF)
	ON_COMMAND(ID_PREFERENCES_GLOBAL, OnPreferencesGlobal)
	ON_WM_QUERYENDSESSION()	
	ON_WM_CLOSE()
	ON_COMMAND(ID_TOOLS_EXPLORER, OnToolsExplorer)	
	ON_COMMAND(ID_WEB, OnWeb)
	ON_COMMAND(ID_WEB_UPDATE, CheckForUpdateASP)
	ON_COMMAND(ID_HELP_REMOTE_DIAGNOSTIC, OnRemoteDiagnostic)
	ON_COMMAND(ID_TOOLS_SCSI_EXPLORER, OnToolsScsiExplorer)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SCSI_EXPLORER, OnUpdateToolsScsiExplorer)
	ON_COMMAND(ID_REG, OnReg)
	ON_COMMAND(ID_FILE_NEW2, OnFileNew2)		
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_HELP_DESK, OnHelpDesk)
	ON_COMMAND(ID_TOOLS_SAVESTATE, OnToolsSavestate)
//	ON_COMMAND(ID_TOOLS_DONGLE_READER, OnToolsDongleReader)
//	ON_UPDATE_COMMAND_UI(ID_TOOLS_DONGLE_READER, OnUpdateToolsDongleReader)
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW2, OnUpdateFileNew2)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(MIN_QUEUE_ID, MAX_QUEUE_ID, OnQueueNames)	
	ON_UPDATE_COMMAND_UI_RANGE(MIN_QUEUE_ID, MAX_QUEUE_ID, OnUpdateQueueNames)	
	ON_COMMAND_RANGE(MIN_TOOL_ID, MAX_TOOL_ID, OnTools)	
	ON_UPDATE_COMMAND_UI_RANGE(MIN_TOOL_ID, MAX_TOOL_ID, OnUpdateTools)	
	//ON_COMMAND(ID_HELP_INDEX, SECWorkbook::OnHelpFinder)
    //ON_COMMAND(ID_HELP_USING, SECWorkbook::OnHelpUsing)
    //ON_COMMAND(ID_HELP, SECWorkbook::OnHelp)
	//ON_COMMAND(ID_CONTEXT_HELP, SECWorkbook::OnContextHelp)	
	ON_COMMAND(ID_HELP_INDEX, PDFHelp)
    ON_COMMAND(ID_HELP_USING, PDFHelp)	
    ON_COMMAND(ID_HELP, PDFHelp)
	ON_COMMAND(ID_CONTEXT_HELP, PDFHelp)	
	//ON_COMMAND(ID_DEFAULT_HELP, SECWorkbook::OnHelpIndex)		
	ON_COMMAND(ID_DEFAULT_HELP, PDFHelp)		
	ON_COMMAND(ID_WINDOW_CLOSE_ALL_PREVIEW, OnCloseAllPreview)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CLOSE_ALL_PREVIEW, OnUpdateOnCloseAllPreview)	
	//ON_COMMAND(ID_HELP, SECWorkbook::OnHelpFinder)	
	ON_COMMAND(ID_HELP, PDFHelp)	
	ON_COMMAND(ID_HELP_NEWINTHISVERSION, OnNewInThisVersion)
	ON_UPDATE_COMMAND_UI(ID_HELP_REMOTE_DIAGNOSTIC, OnUpdateHelpRemoteDiagnostics)

END_MESSAGE_MAP()                                     

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,           // status line indicator 1
	ID_SEPARATOR,           // status line indicator 2
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM	
};


static UINT fileButtons[] =
{
	ID_FILE_NEW,	   	
	ID_SEPARATOR,
	ID_EDIT_CUT,
	ID_EDIT_COPY,
	ID_EDIT_PASTE,
	ID_EDIT_ADDFILES,
	ID_SEPARATOR,
	ID_OUR_FILE_PRINT,
	ID_OUR_FILE_PRINT_SEL,
	ID_OUR_FILE_PRINT_INDEX,
	ID_OUR_FILE_PRINT_SEL_INDEX,
	ID_SEPARATOR,
	ID_OUR_FILE_PRINT_PREVIEW,
	ID_FILE_PRINT_PREVIEW_SEL,	// 32784
	ID_FILE_PRINT_PREVIEW_SEL_HD,
	ID_SEPARATOR,
	ID_PROCESS_CANCELIMAGING,	 // 32785
	ID_PROCESS_SKIPTONEXTFILE,
	ID_PROCESS_PAUSEIMAGING,
	ID_PROCESS_RESUMEIMAGING,
	ID_SEPARATOR,
	ID_PREFERENCES_QUEUE,
	ID_PREFERENCES_LOCAL,
	ID_SEPARATOR,
	ID_TOOLS_LOG
   };

/////////////////////////////////////////////////////////////////////////////
// Menu bar declarations

// IDs
#define ID_MENUBAR_FILE		0x80000001
#define ID_MENUBAR_FILE2	0x80000002
#define ID_MENUBAR_EDIT		0x80000003
#define ID_MENUBAR_VIEW		0x80000005
#define ID_MENUBAR_TOOLS	0x80000006
#define ID_MENUBAR_WINDOW	0x80000007
#define ID_MENUBAR_HELP		0x80000008

// View Bit Flags
#define BITFLAG_MAINFRAME	0x00000001
#define BITFLAG_EDITVIEW	0x00000002
#define BITFLAG_ALL			BITFLAG_MAINFRAME | BITFLAG_EDITVIEW

// Table mapping bitflag to menu resource
static SECMenuMap menuMap[] =
{
	{ IDR_MAINFRAME,	BITFLAG_MAINFRAME },
	{ IDR_ZEUSTYPE,	    BITFLAG_EDITVIEW  }
};

// Default menu bar layout
static UINT menuButtons[] =
{
	ID_MENUBAR_FILE,	   	// "File" submenu for edit view
	ID_MENUBAR_FILE2,		// "File" submenu for mainframe view
	ID_MENUBAR_EDIT,		
	ID_MENUBAR_VIEW,	
	ID_MENUBAR_TOOLS,	
	ID_MENUBAR_WINDOW,
	ID_MENUBAR_HELP
};

/////////////////////////////////////////////////////////////////////////////
// Define the button map
BEGIN_BUTTON_MAP(btnMap)
	MENU_BUTTON_EX(ID_MENUBAR_FILE, SEC_TBBS_NODISABLE, 
	   MAKEINTRESOURCE(IDR_ZEUSTYPE), 0, _T("&Printer"), BITFLAG_EDITVIEW)
	MENU_BUTTON_EX(ID_MENUBAR_FILE2, SEC_TBBS_NODISABLE, 
	   MAKEINTRESOURCE(IDR_MAINFRAME), 0, _T("&Printer"), BITFLAG_MAINFRAME)
	MENU_BUTTON_EX(ID_MENUBAR_EDIT, SEC_TBBS_NODISABLE, 
	   MAKEINTRESOURCE(IDR_ZEUSTYPE), 1, _T("&Edit"), BITFLAG_EDITVIEW)
	MENU_BUTTON_EX(ID_MENUBAR_VIEW, SEC_TBBS_NODISABLE, 
	   MAKEINTRESOURCE(IDR_ZEUSTYPE), 2, _T("&View"), BITFLAG_ALL)
	MENU_BUTTON_EX(ID_MENUBAR_TOOLS, SEC_TBBS_NODISABLE, 
	   MAKEINTRESOURCE(IDR_ZEUSTYPE), 3, _T("&Tools"), BITFLAG_ALL)
	MENU_BUTTON_EX(ID_MENUBAR_WINDOW, SEC_TBBS_NODISABLE, 
	   MAKEINTRESOURCE(IDR_ZEUSTYPE), 4, _T("&Window"), BITFLAG_EDITVIEW)
	MENU_BUTTON_EX(ID_MENUBAR_HELP, SEC_TBBS_NODISABLE, 
	   MAKEINTRESOURCE(IDR_ZEUSTYPE), 5, _T("&Help"), BITFLAG_ALL)
END_BUTTON_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
   {      
   CString cs;
   int i;   

   // zero out fileinfo instance counter
   CfDlgInstanceCntr = 0;     
   
   // set open queues to zero
   QueueCount.SetQueueCount(0);   

   m_pPropFrame = NULL;   
   
   // reset clip array
   ClipArray.Destroy();

   // reset pPaneArray
   pPaneArray.Destroy();

   // zero out the ipc slots
   for (i = 0; i < MAX_IPCSLOTS; i++)
      IPCSlotTaken[i] = FALSE;

   hIsCompleteMutex = NULL;

#ifdef LAB_STYLE
   m_pControlBarManager = NULL;
   m_pMenuBar = NULL;
#else
   // new SEC GUI stuff
   m_pMenuBar = new SECMDIMenuBar;					   	
   m_pControlBarManager = new SECToolBarManager(this);
#endif

   // Enable bitmap menu support.
   EnableBmpMenus();   

   m_nMaxTabWidth = 180;

   m_bShowIcons = FALSE;

   pgis_server = NULL;
   TimerID = 0;

   // get digital now version if we're not just installing and skeedaddling
   if (!((CZeusApp*)AfxGetApp())->bInstallPrinterAndExit)
	  {
      CProfile Profile;	   
	  // if the print server flag is set, start the server
      if (Profile.GetInt(IDS_PHOTOGIZE, IDS_PRINTSERVER, FALSE))   
		 pgis_server = new CPGPRCServer;		 
	  // if the digitalnow flag is set, start the server
      else if (Profile.GetInt(IDS_GENERALSECTION, "DigitalNow", FALSE))   
		 {
		 // write the new style profile
		 Profile.WriteInt(IDS_PHOTOGIZE, IDS_PRINTSERVER, TRUE);
		 // turn off the digital now setting
		 Profile.WriteInt(IDS_GENERALSECTION, "DigitalNow", FALSE);
		 // and start the server
		 pgis_server = new CPGPRCServer;		 
		 }
	  }

	StartAsyncPipe();

   }

CMainFrame::~CMainFrame()
   {   
   // new SEC GUI stuff
   delete m_pMenuBar;
   delete m_pControlBarManager;
   if (pgis_server)
	  {	  
      delete pgis_server;
	  }
   }

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
  { 
  // override the main frame class name
  cs.lpszClass = SERVERCLASS30;
#ifdef LAB_STYLE
  return TRUE;
#else
  return SECWorkbook::PreCreateWindow(cs);
#endif
  }

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{   
    CProfile Profile;	

#ifdef LAB_STYLE
   if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR_MAIN))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
#else	
	if (SECWorkbook::OnCreate(lpCreateStruct) == -1)
		return -1;	
	
	// Load the master bitmap for ALL toolbars administrated by the
	// toolbar manager (and the large bitmap counterpart). All toolbars
	// (now and future) will be indices into this bitmap.
	SECToolBarManager* pToolBarMgr=(SECToolBarManager *)m_pControlBarManager;	
	// hack from Friedrich Brunzema [brunzefb@sciex.com]
	// doesn't seem to work!
	pToolBarMgr->GetBmpMgr()->SetLargeSize(24, 23);	
	VERIFY(pToolBarMgr->LoadToolBarResource(MAKEINTRESOURCE(IDR_TOOLBAR_MAIN_SMALL), 	   
	   MAKEINTRESOURCE(IDR_TOOLBAR_MAIN)));	   
    // configure the button map for drawing non-standard buttons
	pToolBarMgr->SetButtonMap(btnMap);
	pToolBarMgr->SetMenuMap(menuMap, NUMELEMENTS(menuMap));

	// establish the default toolbar groupings, these button groupings
	// may be radically different once a workspace is loaded 
	pToolBarMgr->DefineDefaultToolBar(AFX_IDW_TOOLBAR,_T("File"),
											NUMELEMENTS(fileButtons),fileButtons,
													CBRS_ALIGN_ANY, AFX_IDW_DOCKBAR_TOP);

	EnableDocking(CBRS_ALIGN_ANY);

	// Initialize menubar support
	pToolBarMgr->SetMenuInfo(2, IDR_MAINFRAME, IDR_ZEUSTYPE);
	pToolBarMgr->LayoutMenuBar(NUMELEMENTS(menuButtons), menuButtons);			
	
	pToolBarMgr->EnableLargeBtns();		

	pToolBarMgr->EnableCoolLook(TRUE);

	pToolBarMgr->SetDefaultDockState();	
#endif

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
//		TRACE("Failed to create status bar\n");
		return -1;      // fail to create
	}    


	InitWorkspaceMgrEx(_T("Software\\Graphx\\RasterPlus"),FALSE,
	   RUNTIME_CLASS(COurWorkspace));

	BOOL m_largetoolbar = Profile.GetInt(IDS_GENERALSECTION, "LargeToolBar", TRUE);	
	pToolBarMgr->EnableLargeBtns(m_largetoolbar);		

	// enable the workbook
	SetWorkbookMode(TRUE);		
	if (pgis_server)
		TimerID = SetTimer(RP_SOCKET_TIMER_ID, RP_SOCKET_TIMER_INTERVAL, NULL);         

	return 0;
}                                            

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	SECWorkbook::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	SECWorkbook::Dump(dc);
}

#endif //_DEBUG

void CMainFrame::OnPreferencesGlobal()
   {   	
   // make new
   CGlobalProperties* cg = new CGlobalProperties(this);
   // remove apply now
   cg->m_psh.dwFlags |= PSH_NOAPPLYNOW;
   // do the dialog
   if (cg->DoModal())
	  {
	  // apply the button setting now
      SECToolBarManager* pToolBarMgr=(SECToolBarManager *)m_pControlBarManager;	
      pToolBarMgr->EnableLargeBtns(cg->m_GeneralPrefsPage.m_largetoolbar);	

	  // and check for toggling of printserver
	  if (cg->m_GeneralPrefsPage.m_enableprintserver)
		 {
		 // if it's not already running, launch it
		 if (!pgis_server)
			{
			pgis_server = new CPGPRCServer;
			if (TimerID == 0) 
			   TimerID = SetTimer(RP_SOCKET_TIMER_ID, RP_SOCKET_TIMER_INTERVAL, NULL);
			}
		 }
	  else
		 {		 
		 // if it is running, turn it off
		 if (pgis_server)
			{
			delete pgis_server;
			pgis_server = NULL;
			if (TimerID) 
			   {
			   KillTimer(TimerID);
			   TimerID = 0;
			   }
			}
		 }

	  }
   delete cg;
   }

BOOL CMainFrame::OnQueryEndSession()
   {
//   TRACE("MAINFRM QueryEndSession\n");
	
   if (!SECWorkbook::OnQueryEndSession())
      return FALSE;	
   return TRUE;
   }

/////////////////////////////////////////////////////////////////////////////
// CPreviewMDI message handlers

LRESULT CMainFrame::OnPrevSetInfo(WPARAM wParam, LPARAM lParam)
   {                
   // pass on to preview array if it's ours
   return Preview.PrevSetInfo(wParam, lParam);   
   }    

void CMainFrame::OnClose() 
   {
   if (pPaneArray.AnyRipActive())      
	  AfxMessageBox(IDS_NOCLOSEAPP_WHILEACTIVE);
   else
      {
	  // tell the views that we are closing
	  NotifyViewClose();
      // delete the current clip array if it exists
      ClipArray.Destroy();	  
      // reset pPaneArray
      pPaneArray.Destroy();	  

      COurWorkspace* pWsMgr=(COurWorkspace*)GetWorkspaceMgrEx();

	  if (pWsMgr)
		 {
	     // set maximized flag
	     pWsMgr->bMainWndMaximized = IsZoomed();		 

		 // write workspace
		 CString csWorkspaceFile;
		 csWorkspaceFile = ((CZeusApp*)AfxGetApp())->GetWorkSpaceFileName();
         ::DeleteFile(csWorkspaceFile);
         pWsMgr->SaveWorkspace(csWorkspaceFile);
		 }

	  // default
   	  SECWorkbook::OnClose();	  
	  }   
   }

void CMainFrame::OnToolsExplorer() 
   {
   ::WinExec("Explorer", SW_NORMAL);
   }

void CMainFrame::OnCloseAllPreview() 
   {
   Preview.CloseAll();
   }

void CMainFrame::OnUpdateOnCloseAllPreview(CCmdUI* pCmdUI)
   {  
   if (Preview.NumActive() > 0)
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
   }

void CClipArray::Destroy()
   {
   int i = 0;

   while (i < GetSize())
      {
      delete GetAt(i++);
      }     
   RemoveAll();

   Type = OURCLIP_NONE;
   }

void CClipArray::SetType(int t)
   {
   Type = t;
   }

BOOL CClipArray::IsTypeAvail(int t)
   {
   if (Type == t && GetSize() > 0)
      return TRUE;
   else
      return FALSE;
   }

int CMainFrame::GetAvailIPCSlot()
   {   
   int i;
   int iAvailSlot;

   // get next avail IPC slot
   iAvailSlot = -1;
   for (i = 0; i < MAX_IPCSLOTS; i++)
      {
	  if (!IPCSlotTaken[i])
	     {
		 iAvailSlot = i;
		 IPCSlotTaken[i] = TRUE;
		 break;
		 }
	  }
   return iAvailSlot;
   }

void CMainFrame::FreeIPCSlot(int Slot)
   {
   if (Slot != -1) IPCSlotTaken[Slot] = FALSE;
   }

						   
void CMainFrame::OnWeb() 
   { 
	CString csAddress;

	csAddress = "http://www.graphx.com";
//   ::ShellExecute(GetSafeHwnd(), NULL, 
//	  "http://www.graphx.com/index.htm", "", "", SW_SHOW);
	LaunchURLInNewWindow(GetSafeHwnd(), csAddress ); 
   }

void CMainFrame::OnHelpDesk() 
   { 
	CString csAddress;

	csAddress = "http://www.photogize.com/helpdesk/index.asp";
//   ::ShellExecute(GetSafeHwnd(), NULL, 
//	  "http://www.graphx.com/index.htm", "", "", SW_SHOW);
	LaunchURLInNewWindow(GetSafeHwnd(), csAddress ); 
   }

void CMainFrame::OnNewInThisVersion() 
{ 
	CString csChangeLog;
	CFileStatus cFileStat;

#if _DEBUG
	csChangeLog = "D:\\Source\\Zeus32\\Manual\\ChangeLog.Txt";
#else
	csChangeLog = ((CZeusApp*)AfxGetApp())->GetAppPath();
	csChangeLog += "ChangeLog.Txt";
#endif

	if  ( !( CFile::GetStatus(csChangeLog, cFileStat )) )
	{
		AfxMessageBox(IDS_MISSING_CHANGE_LOG, MB_OK );
	}
	else
	{
		LaunchURLInNewWindow(GetSafeHwnd(), csChangeLog ); 
	}
}

void CMainFrame::OnRemoteDiagnostic()
{
	if (AfxMessageBox("Have you established telephone contact with a Graphx Technical Representative?", MB_YESNO) == IDYES)
		LaunchURLInNewWindow(GetSafeHwnd(), "https://www.fastsupport.com");	  
}

BOOL CMainFrame::GetServerStrVal(LPTSTR Entry, LPTSTR Value, LPTSTR Default, DWORD MaxSize)
   {
   HKEY hKey;   
   DWORD dwType = REG_SZ;
   BOOL ret = FALSE;
   LONG lRet;

   if ((lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, RP30_SERVER_KEY, 0, KEY_READ, &hKey)) == ERROR_SUCCESS)
      {  	  
      // get value
	  if ((lRet = RegQueryValueEx(hKey, Entry, NULL, &dwType, (LPBYTE)Value, &MaxSize)) == ERROR_SUCCESS)
	     ret = TRUE;
	  RegCloseKey(hKey);
	  }
   // error, use default
   if (!ret)
	   _tcscpy(Value, Default);

   return ret;
   }

void CMainFrame::OnToolsScsiExplorer() 
   {
   CSCSIExplorer se;    
   // show the scsi explorer
   se.DoModal();   
   }

void CMainFrame::OnUpdateToolsScsiExplorer(CCmdUI* pCmdUI) 
   {
   CSCSI csi; 

   if (csi.DoesWinASPIExist())   
	  pCmdUI->Enable(TRUE);
   else
	  pCmdUI->Enable(FALSE);	
   }

void CMainFrame::OnUpdateHelpRemoteDiagnostics(CCmdUI* pCmdUI) 
   {
	CProfile Profile;
	CString csKey;
	CString csValue;
	CString csAddress;

	int iRegOption;

	iRegOption = Profile.GetInt("Server", "Register", 0);

	switch ( iRegOption )
	{
	case 0:
	default:
		pCmdUI->Enable(TRUE);

		break;
	case 1:
	    pCmdUI->Enable(FALSE);

		break;
	}
   }

LRESULT CMainFrame::OnAddJobToQueue(WPARAM wParam, LPARAM lParam)
   {          
   CZeusDoc* pDoc;
   CString csFileName;
   CFile csIPCFile;
   CString csDevice;
   CString csPort;
   int iDevIdx;
   SERVERXFER OldServerXFer;
   SERVERXFER2 OldServerXFer2;
   SERVERXFER3 OldServerXFer3;
   SERVERXFER4 ServerXFer;
   BOOL GoodIPCFile = FALSE;
   CQueueFile* pCQueueFile;   
   CMDIChildWnd* pCmChild;
   BOOL bMaximized;
   CSpecialTree* pcst;
   CPaneElement* cire;
   int iRet;
   CFileException cFileException;

   csDebugStr.Format("AddJobToQueue = %s", "Starting" );  
   RPDEBUGSTRMFC(csDebugStr);

   if (!GetServerStrVal(SERVERIPCDATAFILE, csFileName.GetBuffer(_MAX_PATH), 
														SERVERIPCDEFAULT, _MAX_PATH)) 
	  {
      csDebugStr.Format("GetServerStrVal Filename = %s", csFileName );  
      RPDEBUGSTRMFC(csDebugStr);

	  csFileName.ReleaseBuffer();
	  return 0;
	  }
   else
	  csFileName.ReleaseBuffer();  
   
   csDebugStr.Format("csIPCFile.%s filename = %s", "Open", csFileName );  
   RPDEBUGSTRMFC(csDebugStr);

   // open file
   if (csIPCFile.Open(csFileName, CFile::modeRead | CFile::typeBinary, &cFileException))
      {
   csDebugStr.Format("csIPCFile.%s filename = %s", "Read", csFileName );  
   RPDEBUGSTRMFC(csDebugStr);

	  if (csIPCFile.Read(&ServerXFer, sizeof(SERVERXFER4)) == sizeof(SERVERXFER4))	  
		 {
		 if (!ServerXFer.IsComplete)
		   hIsCompleteMutex = CreateMutex(NULL, TRUE, ServerXFer.MutexName);
		 GoodIPCFile = TRUE;		 
		 }
	  else
		 {
   csDebugStr.Format("csIPCFile.%s filename = %s", "Seek", csFileName );  
   RPDEBUGSTRMFC(csDebugStr);

		 csIPCFile.SeekToBegin();
	     if (csIPCFile.Read(&OldServerXFer3, sizeof(SERVERXFER3)) == sizeof(SERVERXFER3))	  
			{
		    // if not complete, create a mutex for eof checking
		    // note that we must do this here instead of in the 
		    // language monitor because of NT permission problems
		    // with MAC->NTS printing.
		    if (!OldServerXFer3.IsComplete)
		       hIsCompleteMutex = CreateMutex(NULL, TRUE, OldServerXFer3.MutexName);
		    GoodIPCFile = TRUE;		 
			}
	     else
			{
		    csIPCFile.SeekToBegin();
	        if (csIPCFile.Read(&OldServerXFer2, sizeof(SERVERXFER2)) == sizeof(SERVERXFER2))	  
			   {
		       // old language monitor, so we can't do new mutex stuff
		       GoodIPCFile = TRUE;		 
			   }
		    else
			   {
			   // reset
		       csIPCFile.SeekToBegin();
		       // might be old server
	           if (csIPCFile.Read(&OldServerXFer, sizeof(SERVERXFER)) == sizeof(SERVERXFER))	  
				  {
			      memcpy((void*)&ServerXFer, (void*)&OldServerXFer, sizeof(SERVERXFER));
		          ServerXFer.IsComplete = TRUE;
                  ServerXFer.MutexName[0] = 0;
			      GoodIPCFile = TRUE;		 
				  }
			   }
			}
		 }
	  csIPCFile.Close();
	  }
   else
   {
    csDebugStr.Format("GetServerStrVal File Open = %s", "FAILED" );  
    RPDEBUGSTRMFC(csDebugStr);

    csDebugStr.Format("GetServerStrVal File Open Error = %s", cFileException.m_cause );  
    RPDEBUGSTRMFC(csDebugStr);
   }

   // bad read
   if (!GoodIPCFile)
   {
	   csDebugStr.Format("csIPCFile.%s %s", "Read", "Failed" );  
	   RPDEBUGSTRMFC(csDebugStr);

	   return 0L;
   }
   else
   {
	   csDebugStr.Format("csIPCFile.%s %s", "Read", "Success" );  
	   RPDEBUGSTRMFC(csDebugStr);
   }

   // instantiate a queuefile here so we can check the
   // device type, etc...,
   if ((pCQueueFile = new CQueueFile) == NULL)
	   return 0L;

   // init various server things
   if (!pCQueueFile->InitServerFile(&ServerXFer, 1, NULL))
      {	  
	  delete pCQueueFile;
	  return 0L;
	  }

   // look for a server doc
   cire = FindServerDoc(pCQueueFile, ServerXFer.PrinterName);

   csDebugStr.Format("FindServerDoc PrinterName = %s", ServerXFer.PrinterName );
   RPDEBUGSTRMFC(csDebugStr);
      
   // no server doc found, so create one
   if (!cire)
      {      	  
	  // get device particulars
	  csDevice = pCQueueFile->pCD->Name;	  
	  iDevIdx = pCQueueFile->pCD->DevIdx;
	  csPort = ServerXFer.PortName;

	  csDebugStr.Format("csDevice = %s, iDevIdx = %d, csPort = %s", csDevice, iDevIdx, csPort );
      RPDEBUGSTRMFC(csDebugStr);

      // create a new doc
      if ((iRet = ((CZeusApp*)AfxGetApp())->CreateNewDoc(FALSE, FALSE, FALSE, csDevice, csDevice, 
		 iDevIdx, csPort, FALSE)) == CREATENEWDOC_OK_REBOOT)
		 AfxMessageBox(IDS_NEEDTOREBOOT);   
      // get the active doc and set the server flag
      pCmChild = MDIGetActive(&bMaximized);
      if (pDoc = (CZeusDoc*)(pCmChild->GetActiveDocument()))      
         {         
		 // this is not necessarily intuitive for the user
#if 0
		 // no other files so set the default Queue device
		 // to the device of the first passed file
		 pDoc->QueueDevice = *(pCQueueFile->pCD);
#endif
#if 0
		 pDoc->QueueColor = *(pCQueueFile->pCC);
#endif
	     pDoc->SetModifiedFlag();
         pDoc->UpdateAllViews( NULL );                     		 
		 }
      }         

   // look again
   cire = FindServerDoc(pCQueueFile, ServerXFer.PrinterName);
   
   // add file to queue
   if (cire) 
      {    
	   
	  CDevice* pDefaultDevice;
	  pDefaultDevice = pCQueueFile->pCD;

	  // this is the doc
	  pDoc = (CZeusDoc*)(cire->pDocPtr);
	  // this is special tree
	  pcst = (CSpecialTree*)(cire->pcstPtr);

#if 0
      // set the transform to the queue transform
      pCQueueFile->pTransform = new CTransform;
      pCQueueFile->Prefs |= USE_LOCAL_TRN_PREFS;
      *pCQueueFile->pTransform = QUEUETRANSFORM;
#endif
	  // for now, we always use the queue sizing
	  pCQueueFile->pTransform = NULL;

	  // do it again but first load the queuedevice
      *(pCQueueFile->pCD) = QUEUEDEVICE;
	  // force these to be the same (they may change because
	  // of the new load
      pCQueueFile->pCD->pDevInfo->MemUsed = QUEUEDEVICE.pDevInfo->MemUsed;
      pCQueueFile->pCD->pDevInfo->MemUsedIdx = QUEUEDEVICE.pDevInfo->MemUsedIdx;

	  pCQueueFile->pCD->pDevInfo->TotalMedia = pCQueueFile->pCD->UpdateMediaList();
	  pCQueueFile->pCD->SetDevInfo(TRUE);
	  pCQueueFile->InitServerFile(&ServerXFer, 2, pCQueueFile->pCD);
	  pDefaultDevice->SetDevInfo(TRUE);

	  // let's simplify - if the local queue device is the same
	  // as the Queue device, force the Queuefile to use the 
	  // queue device
	  if (*pCQueueFile->pCD == QUEUEDEVICE)
		 {
		 // delete the existing devmode
         delete pCQueueFile->pCD;               
         pCQueueFile->pCD = NULL; 
         if (pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS)
            pCQueueFile->Prefs -= USE_LOCAL_DEV_PREFS;
		 }
#if 0
	  if (*pCQueueFile->pCC == QUEUECOLOR)
		 {
		 // delete the existing devmode
         delete pCQueueFile->pCC;               
         pCQueueFile->pCC = NULL; 
         if (pCQueueFile->Prefs & USE_LOCAL_COL_PREFS)
            pCQueueFile->Prefs -= USE_LOCAL_COL_PREFS;
		 }
#endif
      // add the file
      pcst->AddToQueue(pCQueueFile);
      }         
   return 0L;
   }

LRESULT CMainFrame::OnServerFileEOF(WPARAM wParam, LPARAM lParam)
   {
   if (hIsCompleteMutex)
	  {
      // unset mutex
      ReleaseMutex(hIsCompleteMutex);
      CloseHandle(hIsCompleteMutex);
      hIsCompleteMutex = NULL;
	  }
   return 0L;
   }

void CMainFrame::OnReg() 
   {
	CProfile Profile;
	CString csKey;
	CString csValue;
	CString csAddress;

	int		iRegOption;
	int		nDefDevice;
	char	NumStr[10];

	//Create Address String
	// This code is for the Japanese Polaroid ONLY version
	
	iRegOption = Profile.GetInt("Server", "Register", 0);

	switch ( iRegOption )
	{
	case 0:
	default:
		csAddress = "http://www.graphx.com/register_rp.asp?";
		break;
	case 1:
		csAddress = "http://www.poladigital.co.jp//support//rpreg.html?";
		break;
	}

	//User name
	csAddress += "USERNAME=";
	csValue = Profile.GetStr(IDS_USERINFO, IDS_USERNAME);
	csValue.TrimLeft();
	csValue.TrimRight();
	//Make sure spaces get included properly
	csValue.Replace(" ", "%20");	
	csAddress += csValue;
	//Company name
	csAddress += "&COMPANYNAME=";
	csValue	= Profile.GetStr(IDS_USERINFO, IDS_ORGANIZATION);
	csValue.TrimLeft();
	csValue.TrimRight();
	//Make sure spaces get included properly
	csValue.Replace(" ", "%20");
	csAddress += csValue;
	//Serial Number
	csAddress += "&SERIALNUMBER=";
	csAddress += Profile.GetStr(IDS_USERINFO, IDS_SERIALNUMBER);
	//Get the default device name
	csAddress += "&DEVICEUSED=";
	nDefDevice = Profile.GetInt(IDS_DEVICES, IDS_DEVICE_DEFAULT, TRUE);
	csAddress += itoa(nDefDevice, NumStr, 10);

	//Version Number
#ifdef DEMO
	csValue.LoadString(IDS_RP95_BUILDVER_DEMO);
#else
	csValue.LoadString(IDS_RP95_BUILDVER);
#endif

	csAddress += "&VERSION=";
	csValue.TrimLeft();
	csValue.TrimRight();
	//Make sure spaces get included properly
	csValue.Replace(" ", "%20");
	csAddress += csValue;

	//Make sure that we don't have any commas in out string or else our URL will not work correctly.
	csAddress.Remove(',');
	//Launch Browser so that it points to our asp page.

	LaunchURLInNewWindow(GetSafeHwnd(), csAddress ); 
   }

void CMainFrame::OnFileNew2() 
   {
   int iRet;

   if ((iRet = ((CZeusApp*)AfxGetApp())->CreateNewDoc(FALSE)) == CREATENEWDOC_OK_REBOOT)
	  AfxMessageBox(IDS_NEEDTOREBOOT);   
   }

void CMainFrame::OnDestroy() 
   {
   if (TimerID) KillTimer(TimerID);
   SECWorkbook::OnDestroy();	
   }

void CMainFrame::OnWebUpdate() 
   {
	CProfile Profile;
	CString csVersion;
	CString csAddress;
	CString csRLCVer;

#ifdef DEMO
	csVersion.LoadString(IDS_RP95_BUILDVER_DEMO);
#else
	csVersion.LoadString(IDS_RP95_BUILDVER);
#endif

	//Get RLC Version number
	csRLCVer.LoadString(IDS_RLC_VER);
	
	//Create Address String
	csAddress = "http://www.graphx.com/UpdateRP.asp?";

	csAddress += "SERIALNUMBER=";
	csAddress += Profile.GetStr(IDS_USERINFO, IDS_SERIALNUMBER);

	csAddress += "VERSION=";

	csVersion.TrimLeft();
	csVersion.TrimRight();
	//Make sure spaces get included properly
	csVersion.Replace(" ", "%20");
	csAddress += csVersion;
	
	csAddress += "RLC=";
	csAddress += csRLCVer;

	csAddress.Remove(',');

	//Launch Browser so that it points to our asp page.
	LaunchURLInNewWindow(GetSafeHwnd(), csAddress ); 
//	::ShellExecute(GetSafeHwnd(), NULL, csAddress, "", "", SW_SHOW);  	
   }

CPaneElement* CMainFrame::FindServerDoc(CQueueFile* pCQueueFile, LPCSTR lpPrinterName)
   {
   int i;
   CPaneElement* pRipElement;   
   CZeusDoc* pDoc;
   CPaneElement* pcst = NULL;

   for (i = 0; i < pPaneArray.GetSize(); i++)
	  {
	  pRipElement = (CPaneElement*)pPaneArray.GetAt(i);

	  if (!pRipElement) continue;

      // get the active document
      if (pDoc = (CZeusDoc*)(pRipElement->pDocPtr))
		 {
 		 // is it our device and our device alias and the proper queue style?			
		 if ((pDoc->QueueDevice.DevIdx == pCQueueFile->pCD->DevIdx) &&
		    (pDoc->csTitle.Compare(lpPrinterName) == 0) &&
		    (pRipElement->iViewStyle == pCQueueFile->iPrintToHold))
			{
			// set return pointer
			pcst = pRipElement;
			break;			
			}   
         }   
	  }
   return pcst;
   }

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
   {   
   int i;
   CPaneElement* pRipElement;         

   // do this first
   SECWorkbook::OnSize(nType, cx, cy);

   // evenly resize all the panes in all the queues
   for (i = 0; i < pPaneArray.GetSize(); i++)
	  {
	  pRipElement = (CPaneElement*)pPaneArray.GetAt(i);

	  if (pRipElement)
         ::SendMessage(((CWnd*)pRipElement->pcstPtr)->GetSafeHwnd(), 
		 wm_ResizePane, 0, 0);   	  
	  }	 	
   }

void CMainFrame::NotifyViewClose()
   {
   int i;
   CPaneElement* pRipElement;         

   // tell all the views that we are closing
   for (i = 0; i < pPaneArray.GetSize(); i++)
	  {
	  pRipElement = (CPaneElement*)pPaneArray.GetAt(i);

	  if (pRipElement)
	     ((CSpecialTree*)pRipElement->pcstPtr)->NotifyViewClose();	  
	  }	 	
   }

void CMainFrame::AddDevNamesToMenu()
   {
   HMENU hMenu;
   HMENU hSubMenu;
   UINT nMenuID;
   int i;  

   // find all the queue files
   FindQueueFiles();
   
   // get menu bar
   nMenuID = GetMenuBar()->GetCurMenuID();
   if(nMenuID != 0)
	  {
	  // get the attached menu
	  if (hMenu = MenuFromResource(nMenuID))	  
		 {
		 // get the "Printer" menu
		 if (hSubMenu = ::GetSubMenu(hMenu, 0))
			{
			// delete menu items
			for (i = MIN_QUEUE_ID; i <= MAX_QUEUE_ID; i++)			   	
			   ::DeleteMenu(hSubMenu, i, MF_BYCOMMAND);			   
			::DeleteMenu(hSubMenu, QUEUE_ID_SEPARATOR, MF_BYCOMMAND);
			if (csaQueueFileNames.GetSize() > 0)
			   {			   
			   for (i = 0; i < csaQueueFileNames.GetSize(); i++)			   	
			      ::InsertMenu(hSubMenu, ID_APP_EXIT, MF_BYCOMMAND | MF_STRING, i + MIN_QUEUE_ID, csaQueueFileNames.GetAt(i));
			   ::InsertMenu(hSubMenu, ID_APP_EXIT, MF_BYCOMMAND | MF_SEPARATOR, QUEUE_ID_SEPARATOR, NULL);
			   }
			::DrawMenuBar(this->GetSafeHwnd());
			}
		 }
	  }   
   }

void CMainFrame::OnQueueNames(UINT nID)
   {
   CString csQueueFileName;
   CString csQueueName;

   csQueueFileName = ((CZeusApp*)AfxGetApp())->GetQueuePath();

   csQueueName =  csaQueueFileNames.GetAt(nID - MIN_QUEUE_ID);
   csQueueName += ".que";

   csQueueFileName += csQueueName;
   ((CZeusApp*)AfxGetApp())->OpenQueue(csQueueFileName);
   }

void CMainFrame::OnUpdateQueueNames(CCmdUI* pCmdUI)
   {  
   // get the index
   int idx = pCmdUI->m_nID - MIN_QUEUE_ID;
   // within our range?
   if (idx <= csaQueueFileNames.GetUpperBound())   
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
   }

void CMainFrame::FindQueueFiles()
   {
   WIN32_FIND_DATA FileInfo;
   CString csFileName;
   CString csSearchPath;
   CString csSearchWild;
   HANDLE hFile;
   CFile cf;   
   WORD wVersion;   
   CString csQueueName;
   CString csTemp;

   // reset
   csaQueueFileNames.RemoveAll();
   
   // set path
   csSearchPath = csSearchWild = ((CZeusApp*)AfxGetApp())->GetQueuePath();
   csSearchWild += "*.que";
   
   // check for existence
   if ((hFile = FindFirstFile(csSearchWild, &FileInfo)) != INVALID_HANDLE_VALUE)
	  {  
      // add path    
      csFileName = csSearchPath;
      // add file name
      csFileName += FileInfo.cFileName;

	  if ((FileInfo.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ||
	      (FileInfo.dwFileAttributes & FILE_ATTRIBUTE_NORMAL))
		 {
	     if (cf.Open(csFileName, CFile::modeRead))
			{
		    try
			   {
               cf.Read(&wVersion, sizeof WORD);
			   if (wVersion == ZEUSDOC_VERSION)
				  {
				  csTemp = FileInfo.cFileName;
				  csQueueName = csTemp.SpanExcluding(".");
				  if (csQueueName.GetLength() > 0)
			         csaQueueFileNames.Add(csQueueName);
				  }
			   }
            catch(CException* theException)
			   {      
		       // oops
		       theException->Delete();
			   }
		    cf.Close();
			}			
		 }
	  while (FindNextFile(hFile, &FileInfo))
		 {		       
	     if ((FileInfo.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ||
	        (FileInfo.dwFileAttributes & FILE_ATTRIBUTE_NORMAL))
			{
	        if (cf.Open(csFileName, CFile::modeRead))
			   {
		       try
				  {
                  cf.Read(&wVersion, sizeof WORD);
			      if (wVersion == ZEUSDOC_VERSION)
					 {
					 csTemp = FileInfo.cFileName;
					 csQueueName = csTemp.SpanExcluding(".");
					 if (csQueueName.GetLength() > 0)
			            csaQueueFileNames.Add(csQueueName);
					 }
				  }
               catch(CException* theException)
				  {      
		          // oops
		          theException->Delete();
				  }
		       cf.Close();
			   }			
			}
		 }		 	     	  
       // close the handle
       FindClose(hFile);   	  
	  }	     
   }

void CMainFrame::OnToolsSavestate() 
   {
   int i;
   CPaneElement* pRipElement;   
   CZeusDoc* pDoc;
   CPaneElement* pcst = NULL;

   for (i = 0; i < pPaneArray.GetSize(); i++)
	  {
	  pRipElement = (CPaneElement*)pPaneArray.GetAt(i);

	  if (!pRipElement) continue;

	  // update some info on the view positions
	  ((CSpecialTree*)pRipElement->pcstPtr)->NotifyViewClose();	  

	  // only do on archive
	  if (pRipElement->iViewStyle != QUEUESTYLE_ARCHIVE) continue;

      // get the active document
      if (pDoc = (CZeusDoc*)(pRipElement->pDocPtr))
		 {
		 // force it to be modifed
		 pDoc->SetModifiedFlag();
		 // save title and paths
		 CString csTempTitle;
		 CString csTempPath;
		 csTempTitle = pDoc->GetTitle();
		 csTempPath = pDoc->GetPathName();
		 // save it
		 pDoc->OurSaveDocument(); 		 
		 // kluge back the titles (the order is important)		 
		 pDoc->SetPathName(csTempPath);
   	     pDoc->SetTitle(csTempTitle);  		 
         }   
	  }

   // get our workspace
   COurWorkspace* pWsMgr=(COurWorkspace*)GetWorkspaceMgrEx();

   if (pWsMgr)
	  {
	  // set maximized flag
	  pWsMgr->bMainWndMaximized = IsZoomed();		 

	  // write workspace
	  CString csWorkspaceFile;
	  csWorkspaceFile = ((CZeusApp*)AfxGetApp())->GetWorkSpaceFileName();
      ::DeleteFile(csWorkspaceFile);
      pWsMgr->SaveWorkspace(csWorkspaceFile);
	  }
   }

void CMainFrame::AddToolsToMenu()
{
   BOOL bAddPackagePlus;
   BOOL bAddColorProfiler = FALSE;
   int iAddQuickScan;		// Use an int because we also return the number of TWAIN sources found

   int iToolID = 0;

   HMENU hMenu;
   HMENU hSubMenu;
   UINT nMenuID;
   int i;         
   CProfile Profile;   

#if 0
   iAddQuickScan = IsScannerAvailable();
#endif
   iAddQuickScan = FALSE;

   bAddPackagePlus = IsPackagePlusInstalled();

//   bAddColorProfiler = IsColorProfilerInstalled();

   // add packageplus

   if ( bAddPackagePlus || bAddColorProfiler || iAddQuickScan )
   {
	   csat.RemoveAll();
	   
	   if ( iAddQuickScan )
	   {
		   csat.Add("Ac&quire Image");
		   iScannerToolID = iToolID + MIN_TOOL_ID;
		   iToolID++;

		   if ( iAddQuickScan > 1 )
		   {
			   csat.Add("Select T&wain Device");
			   iSelectSCSIToolID = iToolID + MIN_TOOL_ID;
			   iToolID++;
		   }
	   }

	   if ( bAddPackagePlus )
	   {
		   csat.Add("&PackagePlus");
	   
		   iPackagePlusToolID = iToolID + MIN_TOOL_ID;
		   iToolID++;
	   }

	   if ( bAddColorProfiler )
	   {
		   csat.Add("&TrueGraphx Color Profiler");

		   iColorProfilerToolID = iToolID + MIN_TOOL_ID;
		   iToolID++;
	   }
   }

   if ( bAddPackagePlus || bAddColorProfiler || iAddQuickScan )
   {
	   // get menu bar
	   nMenuID = GetMenuBar()->GetCurMenuID();
	   if(nMenuID != 0)
		  {
		  // get the attached menu
		  if (hMenu = MenuFromResource(nMenuID))	  
			 {
			 // get the "Tools" menu in mainfrm
			 if (hSubMenu = ::GetSubMenu(hMenu, 3))
				{
				// delete menu items
				::DeleteMenu(hSubMenu, ID_TOOLS_SCANJOB, MF_BYCOMMAND);			   			
				::DeleteMenu(hSubMenu, ID_TOOLS_SELECTSCANNER, MF_BYCOMMAND);			   			

				// Note: A DeleteMenu() call must be done for each Separator
				//    
				::DeleteMenu(hSubMenu, TOOL_ID_SEPARATOR, MF_BYCOMMAND);
				::DeleteMenu(hSubMenu, TOOL_ID_SEPARATOR, MF_BYCOMMAND);
				::DeleteMenu(hSubMenu, TOOL_ID_SEPARATOR, MF_BYCOMMAND);

				for (i = MIN_TOOL_ID; i <= MAX_TOOL_ID; i++)			   	
				   ::DeleteMenu(hSubMenu, i, MF_BYCOMMAND);			   			

				if (csat.GetSize() > 0)
				   {			   
					i = 0;
					if ( iAddQuickScan > 0 )
					{
						::InsertMenu(hSubMenu, 0xFFFFFFFF, 
							MF_BYPOSITION | MF_SEPARATOR, TOOL_ID_SEPARATOR, NULL);

						::InsertMenu(hSubMenu, 0xFFFFFFFF, 
								MF_BYPOSITION | MF_STRING,
									i + MIN_TOOL_ID, csat.GetAt(i));
						i++;

						if ( iAddQuickScan > 1 )
						{
							::InsertMenu(hSubMenu, 0xFFFFFFFF, 
								MF_BYPOSITION | MF_STRING,
									i + MIN_TOOL_ID, csat.GetAt(i));
							i++;
						}
					}

					if ( bAddPackagePlus )
					{
					   ::InsertMenu(hSubMenu, 0xFFFFFFFF, 
						  MF_BYPOSITION | MF_SEPARATOR, TOOL_ID_SEPARATOR, NULL);

					   ::InsertMenu(hSubMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, 
						  i + MIN_TOOL_ID, csat.GetAt(i));
					   i++;
					}

					if ( bAddColorProfiler )
					{
					   ::InsertMenu(hSubMenu, 0xFFFFFFFF, 
						  MF_BYPOSITION | MF_SEPARATOR, TOOL_ID_SEPARATOR, NULL);

					   ::InsertMenu(hSubMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, 
						  i + MIN_TOOL_ID, csat.GetAt(i));
					   i++;
					}

				   }
				::DrawMenuBar(this->GetSafeHwnd());
				}
			 }
		  }   
   }

}

void CMainFrame::PDFHelp()
{
	HINSTANCE hShellInst = 0;
	CString csHelpFilePDF;
	CString csAcrReaderPath;

	// get help file
	csHelpFilePDF = ((CZeusApp*)AfxGetApp())->GetAppPath();
	csHelpFilePDF += "RPMANUAL.PDF";

	// read it with acrobat
	hShellInst = ::ShellExecute(GetSafeHwnd(), NULL, csHelpFilePDF, "", "", SW_SHOW);

	if ((int)hShellInst == SE_ERR_NOASSOC )
	{
		if ( AfxMessageBox("The RasterPlus Manual requires Acrobat Reader\n\nDo you want to install the Acrobat Reader now?", MB_YESNO) == IDYES)
		{
			// get help file
			csAcrReaderPath = ((CZeusApp*)AfxGetApp())->GetAppPath();
			csAcrReaderPath += "Acrobat\\AdbeRdr60_enu.exe";

			::ShellExecute(GetSafeHwnd(), NULL, csAcrReaderPath, "", "", SW_SHOW);
		}
	}
}
/*
void CMainFrame::OnToolsDongleReader() 
   {
   CString csDongleExplorer;

   csDongleExplorer = ((CZeusApp*)AfxGetApp())->GetAppPath();
   csDongleExplorer += "DongleRead.exe";
   ::WinExec(csDongleExplorer, SW_NORMAL);		
   }
*/
void CMainFrame::SaveState()
   {
   OnToolsSavestate();
   }		   

BOOL CMainFrame::IsPackagePlusInstalled()
   {
   HKEY hKey;
   BOOL bRet = FALSE;
   DWORD lDataSize = _MAX_PATH;   
   DWORD dwType = REG_SZ;
   char *pEntry = "App";

   csPackagePlusExe = "";

   if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Graphx\\PackagePlus", 0, 
	  KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
      {  	 	  
      // get value
	  if (RegQueryValueEx(hKey, pEntry, NULL, &dwType, 
		 (LPBYTE)(csPackagePlusExe.GetBuffer(lDataSize)), &lDataSize) == ERROR_SUCCESS)
		 {		 
		 bRet = TRUE;
		 }
	  csPackagePlusExe.ReleaseBuffer();	  
	  RegCloseKey(hKey);
	  }

/*
//   HKEY hKey;
   BOOL bRet = TRUE;
   CString csAppPath;
   CPhotogizeUtils utils;

//   DWORD lDataSize = _MAX_PATH;   
//   DWORD dwType = REG_SZ;
//   char *pEntry = "App";

   utils.GetAppPath(csAppPath);
   csPackagePlusExe = csAppPath + "PackagePlus.Exe";


   if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Graphx\\PackagePlus", 0, 
	  KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
      {  	 	  
      // get value
	  if (RegQueryValueEx(hKey, pEntry, NULL, &dwType, 
		 (LPBYTE)(csPackagePlusExe.GetBuffer(lDataSize)), &lDataSize) == ERROR_SUCCESS)
		 {		 
		 bRet = TRUE;
		 }
	  csPackagePlusExe.ReleaseBuffer();	  
	  RegCloseKey(hKey);
	  }

   bRet = TRUE;
*/
   return bRet;
   }

BOOL CMainFrame::IsColorProfilerInstalled()
   {
   HKEY hKey;
   BOOL bRet = FALSE;
   DWORD lDataSize = _MAX_PATH;   
   DWORD dwType = REG_SZ;
   char *pEntry = "App";

   csColorProfilerExe = "";

   if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Graphx\\TrueGraphx", 0, 
	  KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
      {  	 	  
      // get value
	  if (RegQueryValueEx(hKey, pEntry, NULL, &dwType, 
		 (LPBYTE)(csColorProfilerExe.GetBuffer(lDataSize)), &lDataSize) == ERROR_SUCCESS)
		 {		 
		 bRet = TRUE;
		 }
	  csColorProfilerExe.ReleaseBuffer();	  
	  RegCloseKey(hKey);
	  }
   return bRet;
   }
/*
int CMainFrame::IsScannerAvailable()
{
	ENUMSOURCECALLBACK pfnEnumScanners;

	int iRetVal;

	int *pTwainSourceCnt = new int;

	HWND hWnd = GetSafeHwnd();

	if ( L_IsTwainAvailable( hWnd ) )
	{
		*pTwainSourceCnt = 0;

		pfnEnumScanners = (ENUMSOURCECALLBACK) MakeProcInstance((FARPROC) EnumScanners, hInstance );

		L_TwainEnumSources( hWnd, pfnEnumScanners, (L_VOID FAR *)pTwainSourceCnt );

		FreeProcInstance(( FARPROC ) pfnEnumScanners );

		if ( *pTwainSourceCnt == 0 )
			iRetVal = -1;
		else
			iRetVal = *pTwainSourceCnt;

	}
	else
	{
		iRetVal = -1;
	}

	delete pTwainSourceCnt;
	
	return iRetVal;
}
*/
void CMainFrame::OnTools(UINT nID)
{
	CString csCmdLine;

   csCmdLine = csPackagePlusExe;

   csCmdLine += " /m2";

//   csCmdLine += " /d";
//   csCmdLine += pTargetDevice->Name;

//   csCmdLine += " /p";
//   pTargetDevice->GetCurrentMedia(Media);
//   if (Media.Name[0] == '?')
//   {
//	   csCmdLine += Media.Name + 1;
//   }
//   else
//   {
//	   csCmdLine += Media.Name;
//   }

   csCmdLine += " /iRasterGize";

/*
	if ( nID == iPackagePlusToolID )
	{
	   // start packageplus
	   STARTUPINFO StartupInfo;
	   PROCESS_INFORMATION ProcessInfo;      
	   HWND hPackagePlusWnd = NULL;
   
	   // NULL the StartupInfo struct
	   memset(&StartupInfo, 0, sizeof(STARTUPINFO));
	   // the size
	   StartupInfo.cb = sizeof(STARTUPINFO);
	   // show the window
	   StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	   StartupInfo.wShowWindow = SW_SHOW;
	   // start PackagePlus
//	   if (CreateProcess(NULL, (char*)(const char*)csPackagePlusExe, NULL, NULL, FALSE, 
	   if (CreateProcess(NULL, (char*)(const char*)csCmdLine, NULL, NULL, FALSE, 
		  NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &ProcessInfo))
  		  {
	      // look for PackagePlus for 20 seconds
	      WaitForInputIdle(ProcessInfo.hProcess, 20000);
	      // now look for the DIALOG that says PackagePlus
	      if (hPackagePlusWnd = ::FindWindow("#32770", "PackagePlus"))
		     ::BringWindowToTop(hPackagePlusWnd);
		  // close the handles				
		  CloseHandle(ProcessInfo.hThread);
		  CloseHandle(ProcessInfo.hProcess);	  
		  }
	}
*/
	if ( nID == iPackagePlusToolID )
	{
	   // start packageplus
	   STARTUPINFO StartupInfo;
	   PROCESS_INFORMATION ProcessInfo;      
	   HWND hPackagePlusWnd = NULL;
   
	   // NULL the StartupInfo struct
	   memset(&StartupInfo, 0, sizeof(STARTUPINFO));
	   // the size
	   StartupInfo.cb = sizeof(STARTUPINFO);
	   // show the window
	   StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	   StartupInfo.wShowWindow = SW_SHOW;
	   // start PackagePlus
	   if (CreateProcess(NULL, (char*)(const char*)csPackagePlusExe, NULL, NULL, FALSE, 

		  NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &ProcessInfo))
  		  {
	      // look for PackagePlus for 20 seconds
	      WaitForInputIdle(ProcessInfo.hProcess, 20000);
	      // now look for the DIALOG that says PackagePlus
	      if (hPackagePlusWnd = ::FindWindow("#32770", "PackagePlus"))
		     ::BringWindowToTop(hPackagePlusWnd);
		  // close the handles				
		  CloseHandle(ProcessInfo.hThread);
		  CloseHandle(ProcessInfo.hProcess);	  
		  }
	}
/*
	else if ( nID == iScannerToolID )
	{
		AcquireScanImage();
	}
	else if ( nID == iSelectSCSIToolID )
	{
		L_TwainSelect(GetSafeHwnd());
	}
*/
	else if ( nID == iColorProfilerToolID )
	{
	   // start packageplus
	   STARTUPINFO StartupInfo;
	   PROCESS_INFORMATION ProcessInfo;      
	   HWND hTrueGraphxWnd = NULL;
   
	   // NULL the StartupInfo struct
	   memset(&StartupInfo, 0, sizeof(STARTUPINFO));
	   // the size
	   StartupInfo.cb = sizeof(STARTUPINFO);
	   // show the window
	   StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	   StartupInfo.wShowWindow = SW_SHOW;
	   // start PackagePlus
	   if (CreateProcess(NULL, (char*)(const char*)csColorProfilerExe, NULL, NULL, FALSE, 
		  NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &ProcessInfo))
  		  {
	      // look for TrueGraphx Color Profiler for 30 seconds
	      WaitForInputIdle(ProcessInfo.hProcess, 30000);
	      // now look for the DIALOG that says TrueGraphx Color Profiler
	      if (hTrueGraphxWnd = ::FindWindow("#32770", "TrueGraphx Color Profiler"))
		     ::BringWindowToTop(hTrueGraphxWnd);
		  // close the handles				
		  CloseHandle(ProcessInfo.hThread);
		  CloseHandle(ProcessInfo.hProcess);	  
		  }
	}

}

void CMainFrame::OnUpdateTools(CCmdUI* pCmdUI)
   {  
   // get the index
   int idx = pCmdUI->m_nID - MIN_TOOL_ID;
   // within our range?
   if (idx <= csat.GetUpperBound())   
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
   }
/*
void CMainFrame::OnUpdateToolsDongleReader(CCmdUI* pCmdUI) 
   {
//#ifdef USEDONGLE
   pCmdUI->Enable(TRUE);
//#else
//   pCmdUI->Enable(FALSE);
//#endif
   }
*/
L_INT EXPORT EXT_CALLBACK EnumScanners( L_UCHAR FAR * pBuffer, L_INT uBytes, L_VOID FAR *pUserData )
{
	L_CHAR FAR *pTwainSources = (L_CHAR FAR *) pUserData;

	*pTwainSources = *pTwainSources + 1;
	
	return SUCCESS;
}

int CMainFrame::NumSaveQueueFileInstances(CQueueFile* pCQueueFile)
   {
   int i, j;
   int iCnt = 0;
   CPaneElement* pRipElement;   
   CQueueFile* pThisCQueueFile;
   CZeusDoc* pDoc;   

   for (i = 0; i < pPaneArray.GetSize(); i++)
	  {
	  pRipElement = (CPaneElement*)pPaneArray.GetAt(i);

	  if (!pRipElement) continue;

      // get the active document
      if (pDoc = (CZeusDoc*)(pRipElement->pDocPtr))
		 {
		 if (pRipElement->iViewStyle == QUEUESTYLE_HOLD)
			{
		    // get the hold queue files
            for (j = 0; j < pDoc->CQ_Hold.GetSize(); j++)
			   {
			   // get file
			   pThisCQueueFile = (CQueueFile*)pDoc->CQ_Hold.GetAt(j);
			   // if these are the same, increment count
			   if (pThisCQueueFile->LongName == pCQueueFile->LongName) iCnt++;
			   }
			}
		 else if (pRipElement->iViewStyle == QUEUESTYLE_PRINT)
			{
		    // get the print queue files
            for (j = 0; j < pDoc->CQ.GetSize(); j++)
			   {
		       // get file
			   pThisCQueueFile = (CQueueFile*)pDoc->CQ.GetAt(j);
			   // if these are the same, increment count
			   if (pThisCQueueFile->LongName == pCQueueFile->LongName) iCnt++;
			   }
			}
		 else if (pRipElement->iViewStyle == QUEUESTYLE_ARCHIVE)
			{												   
		    // get the archive queue files
            for (j = 0; j < pDoc->CQ_Archive.GetSize(); j++)
			   {
			   // get file
			   pThisCQueueFile = (CQueueFile*)pDoc->CQ_Archive.GetAt(j);
			   // if these are the same, increment count
			   if (pThisCQueueFile->LongName == pCQueueFile->LongName) iCnt++;
			   }
			}
         }   
	  }
   return iCnt;
   }

BOOL CMainFrame::DeleteLastServerFile(CQueueFile* pCQueueFile)
   {
   BOOL bRet = FALSE;
   // is this a server file
   if (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_SERVERFILE)
	  {
	  // is there only one instance?  if so, then delete it
	  if (NumSaveQueueFileInstances(pCQueueFile) == 1)
		 {
	     ::DeleteFile(pCQueueFile->LongName);			 
		 bRet = TRUE;
		 }
	  }				
   return bRet;
   }
 	
void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
   {
   SECWorkbook::OnActivate(nState, pWndOther, bMinimized);

   // we need the following code to fix a bug where, if the app
   // loses focus, AND we select a pane on return to app which is
   // DIFFERENT than the original focus pane, two panes would have
   // highlighted headers.  We now force an top-down invalidate anytime the app
   // gets activated
   int i;
   CPaneElement* pRipElement;   
   CSpecialTree* pst;   
	
   for (i = 0; i < pPaneArray.GetSize(); i++)
	  {
	  pRipElement = (CPaneElement*)pPaneArray.GetAt(i);

	  if (!pRipElement) continue;

	  pst = (CSpecialTree*)(pRipElement->pcstPtr);

      pst->Invalidate();
	  }	
   }

/*
void CMainFrame::AcquireScanImage()
{
	BOOL bMaximized;

	CQueueFile* pCQueueFile;
	CString csFileName;

	CMDIChildWnd* pCmChild;
	CZeusDoc* pDoc;
	
	int idx = 0;
	int i;
	CPaneElement* pRipElement;   
	CSpecialTree* pcst = NULL;
	
	CScanDialog scanDlg;

	pCmChild = MDIGetActive(&bMaximized);
	pDoc = (CZeusDoc*)(pCmChild->GetActiveDocument());
	
	scanDlg.pCDev = &pDoc->QueueDevice;

	if ( scanDlg.DoModal() == IDOK )
	{
		// Put the new file in the Print or Hold Queue
		//
		csFileName	= scanDlg.fileName;

		for (i = 0; i < pPaneArray.GetSize(); i++)
		{
			pRipElement = (CPaneElement*)pPaneArray.GetAt(i);

			if (!pRipElement)
				continue;

			// get the active document
			if (pDoc = (CZeusDoc*)(pRipElement->pDocPtr))
			{
				// is it our device and our device alias and the proper queue style?			
				if ((pDoc->QueueDevice.DevIdx == scanDlg.pCDev->DevIdx) &&
					(pRipElement->iViewStyle == scanDlg.iQueue))
				{
				// set return pointer
				pcst = (CSpecialTree*)(pRipElement->pcstPtr);
				break;			
				}   
			}   
		}

		if (pCQueueFile = new CQueueFile)
		{
			pCQueueFile->Init(csFileName);

			if (pCQueueFile->Init(csFileName))
			{
				// not touched     
				pCQueueFile->WasTouched = FALSE;

				pCQueueFile->InQueue = scanDlg.iQueue;
				
				pCQueueFile->ExtraInfo |= CQUEUE_EXTRA_SERVERFILE;

				if ( scanDlg.iMediaIdx != scanDlg.pCDev->pDevInfo->CurrentMediaIdx )
				{
					pCQueueFile->pCD = new CDevice;

					pCQueueFile->pCD->LoadDevice(scanDlg.pCDev->DevIdx);

					pCQueueFile->pCD->pDevInfo->CurrentMediaIdx = scanDlg.iMediaIdx;

					pCQueueFile->Prefs |= USE_LOCAL_DEV_PREFS;
				}
				
				pcst->AddToQueue(pCQueueFile);
			}
		}
		else
		{
			delete pCQueueFile;
		}
	}
}
*/
void CMainFrame::OnTimer(UINT nIDEvent) 
   {	
   // socket timer
   if (nIDEvent == RP_SOCKET_TIMER_ID)
	  {
	  pgis_server->CheckAwake();
	  }	
   SECWorkbook::OnTimer(nIDEvent);
   }

void CMainFrame::OnUpdateFileNew2(CCmdUI* pCmdUI) 
   {
   // have we exceeded the maximum number of queues?
   if (QueueCount.GetQueueCount() < MAX_OPEN_QUEUES)
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
   }

void CMainFrame::StartPhotogizeServer()
   {
   // start the server
   pPGPRCThread = AfxBeginThread((AFX_THREADPROC)PGPRCThreadProc, (LPVOID)this);   
   }

void CMainFrame::StopPhotogizeServer()
   {
   if (pgis_server)
	  {
	  delete pgis_server;
	  pgis_server = NULL;
	  }
   }

void CMainFrame::LaunchURLInNewWindow(HWND hWndParent, LPCSTR lpcstrWebUpload)
{
	char szBuffer[_MAX_PATH];
	CString csFileName;
	CFile cf;   
	CString csAppPath;
	CString csTemp;

	csTemp = "<HTML>test</HTML>";

	csAppPath = ((CZeusApp*)AfxGetApp())->GetAppPath();      

	// make filename   
	csFileName.Format("%s~test.htm", csAppPath);    

	// open the cache and read the contents
	if (cf.Open(csFileName, CFile::modeCreate | CFile::modeWrite))
	{
		try
		{
			cf.Write(csTemp, csTemp.GetLength());         
		}
		catch(CException* theException)
		{      
			// oops
			theException->Delete();
		}
		cf.Close();
	}       
 
	// open in a new window by doing FindExecutable and using csWebUpload on the
	// command line     
	if (::FindExecutable(csFileName, NULL, szBuffer) > (HINSTANCE)32)
		::ShellExecute(hWndParent, NULL, szBuffer, lpcstrWebUpload, "", SW_SHOW);
	else 
		::ShellExecute(hWndParent, NULL, lpcstrWebUpload, "", "", SW_SHOW); // last resort   
}

// version 6.0.0 release
//#define CURRENTVERSION 9600
// version 6.0.1 release
//#define CURRENTVERSION 9601
// version 6.0.2 release
//#define CURRENTVERSION 9602
// version 6.0.3 release
//#define CURRENTVERSION 9603
// version 6.0.4 release
//#define CURRENTVERSION 9604
// version 6.0.5 release
//#define CURRENTVERSION 9605
// version 6.0.6 release
//#define CURRENTVERSION 9606
// version 6.0.7 release
//#define CURRENTVERSION 9607
// version 6.0.8 release
//#define CURRENTVERSION 9608
// version 6.0.9 release
//#define CURRENTVERSION 9609
// version 6.0.10 release
//#define CURRENTVERSION 9610
// version 6.0.11 release
//#define CURRENTVERSION 9611
// version 6.0.12 release
//#define CURRENTVERSION 9612
// version 6.0.13 release
//#define CURRENTVERSION 9613
// version 6.0.14 release
//#define CURRENTVERSION 9614
// version 6.0.15 release
//#define CURRENTVERSION 9615
// version 6.0.16 release
//#define CURRENTVERSION 9616
// version 6.0.17 release
//#define CURRENTVERSION 9617
// version 6.0.18 release
//#define CURRENTVERSION 9618
// version 6.1.0 release
//#define CURRENTVERSION 9619
// version 6.1.1 release
//#define CURRENTVERSION 9620
// version 6.1.2 release
//#define CURRENTVERSION 9621
// version 6.1.3 release
#define CURRENTVERSION 9622

int CMainFrame::GetAppVersion()
{
	return CURRENTVERSION;
}   

BOOL CMainFrame::CheckForUpdateASP()
{
   CString csError;
   CString csURL;
   CStringArray csaResp;	  
   CString csToGet;
   CString csOnlineFileName;
   int iSize;
   BOOL bRet = FALSE;   
   CString csTemp;

   // call the check for update web service   
   CWebServicesASP cweb("www.photogize.com", "/update/chkupdate.asp");		 	  
   cweb.AddFormItem("prd", "RP6");
   cweb.AddFormItem("iver", GetAppVersion());
   cweb.PostFormData(TRUE);

   // if yes, ask to download
   if (cweb.GetMultipleResponse(csaResp) == WEBSERVICESASP_RESP_YES)
	  {
	  // sanity
	  if (csaResp.GetSize() == 3)
		 {
		 csTemp.LoadString(IDS_NEWVERSIONAVAIL);
		 if (::MessageBox(NULL, csTemp, "RasterPlus", MB_YESNO | MB_SETFOREGROUND) == IDYES)
			{			   
			AfxGetApp()->DoWaitCursor(1);
			
			csOnlineFileName = csaResp.GetAt(0);
			csOnlineFileName.MakeLower();
			// if http is in the filename, then it is a complete URL, 
			// otherwise, prepend http://www.photogize.com/update/downloads
			if ((csOnlineFileName.Find("http:") >= 0) || (csOnlineFileName.Find("https:") >= 0))
				csURL = csOnlineFileName;
			else
			{
				// the filename is first
				csURL.Format("http://www.photogize.com/update/downloads/%s", csaResp.GetAt(0));
			}

			// the size is next
			iSize = atoi(csaResp.GetAt(1));
			csToGet = ((CZeusApp*)AfxGetApp())->GetAppPath();
			csToGet += "RasterPlusV6Installer.exe";			
			if (GetHTTPFile(csURL, csToGet, iSize, this))			
			   {
			   bRet = TRUE;
			   STARTUPINFO StartupInfo;
			   PROCESS_INFORMATION ProcessInfo;      
			   // NULL the StartupInfo struct
			   memset(&StartupInfo, 0, sizeof(STARTUPINFO));
			   // the size
			   StartupInfo.cb = sizeof(STARTUPINFO);
			   // show the window
			   StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
			   StartupInfo.wShowWindow = SW_SHOW;
			   if (CreateProcess(NULL, (char*)(const char*)"RasterPlusV6Installer.exe", NULL, NULL, FALSE, 
				  NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &ProcessInfo))
				  {
				  PostMessage(WM_CLOSE);
				  }
				  else
				  {
		    csError.Format("CreateProcess  - FAILED, GLE=%d.\n", GetLastError()); 
				  }
			   }
			else
			{
		csError.Format("GetHTTPFile = %s.\n", "Failed");
			}
			AfxGetApp()->DoWaitCursor(-1);      
			}
		 }
	  }
   else
	  {
	  csTemp.LoadString(IDS_NONEWVERSION);
	  ::MessageBox(NULL, csTemp, "RasterPlus", MB_OK | MB_SETFOREGROUND);
	  }

   cweb.Reset();

   return bRet;
}

BOOL CMainFrame::GetHTTPFile(CString& csFilePath, CString& csTempFileName, int iSize, CWnd* pWnd)
{
   BOOL bRet = TRUE;
   
//   CMyInternetSession mySession("Photogize Lab\r\n", 0, "", "", 0);
   CMyInternetSession mySession("RasterPlus\r\n", 0, "", "", 0);
   
   // break up the URL
   DWORD dwServiceType;
   CString strServer;
   CString strObject;
   INTERNET_PORT nPort;
   AfxParseURL(csFilePath, dwServiceType, strServer, strObject, nPort);	      
   
   // get the file
//   if (mySession.GetWebFile2("Photogize Lab\r\n", strServer, nPort, strObject, 
   if (mySession.GetWebFile2("RasterPlus\r\n", strServer, nPort, strObject, 
	  csTempFileName, iSize, pWnd) == HTTP_STATUS_OK)	  
	  bRet = TRUE;
   else
	  bRet = FALSE;
   
   return bRet;
}  

void CMainFrame::StartAsyncPipe()
{
   // start the server
//   pPFSThread = AfxBeginThread((AFX_THREADPROC)PFSThreadProc, (LPVOID)this);   

   CString csPipeName;
   CString csError;

#if 0
   BOOL fConnected; 
#endif

   DWORD dwThreadId; 

   csPipeName = TEXT("\\\\.\\pipe\\");
   csPipeName += SERVERPIPE;
 
	// Vista
	hPipe = CreateNamedPipe( 
				csPipeName,
				PIPE_ACCESS_INBOUND,      // read access 
//				PIPE_ACCESS_DUPLEX,      // read access 
				PIPE_TYPE_MESSAGE | PIPE_WAIT,    // message type pipe 
																			// message-read mode 
																			// NON - blocking mode 
				PIPE_UNLIMITED_INSTANCES, // max. instances  
				BUFSIZE,                  // output buffer size 
				BUFSIZE,                  // input buffer size 
				0,                        // client time-out 
				NULL);                    // default security attribute 

	if (hPipe == INVALID_HANDLE_VALUE) 
	{
		DWORD dwLastError = GetLastError();
		_tprintf(TEXT("CreateNamedPipe failed, GLE=%d.\n"), GetLastError()); 

		return;
	}
 

#if 0
	// Wait for the client to connect; if it succeeds, 
	// the function returns a nonzero value. If the function
	// returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 
	fConnected = ConnectNamedPipe(hPipe, NULL) ? 
					TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 

	if (fConnected) 
	{ 
		strcpy(rpString, "ConnectNamedPipe - Success" );
	}
	else
	{ 
		_tprintf(TEXT("ConnectNamedPipe failed, GLE=%d.\n"), GetLastError()); 

//		CloseHandle(hPipe);
//		hPipe = NULL;

//		return;
	}
#endif

	printf("Client connected, creating a processing thread.\n"); 

	// Create a thread for this client. 
	hThread = CreateThread( 
					NULL,              // no security attribute 
					0,                 // default stack size 
					FileServerThread,  // thread proc
					(LPVOID)hPipe,    // thread parameter 
					0,                 // not suspended 
					&dwThreadId);      // returns thread ID 

	if (hThread == NULL) 
	{
		_tprintf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError()); 

		return;
	}
}



#ifdef _VERSION_6
#ifndef  _DEBUG
DWORD CMainFrame::CheckGuard()
{
	int iTestDevIdx;

	CString csPathName;
	CString csDLLName;
	CString csError;

	CString csModule = "Load_SUPPORTPLUS";

	CProfile Profile;

	DWORD dwElapsedSecs;
	DWORD dwLastSecs;
	DWORD dwLoadTime;
	DWORD dwLastError = 0;

	GetRPW02Interface lpRPW02Interface;
	GetRPW01Interface lpRPW01Interface;

	SYSTEMTIME timeDest;

	BOOL bRPWCheckGuard = TRUE;

	// If the TimeCheck was already created then
	// check the elapsed tim otherwise do the Protection Check
	if ( !tcTimeCheck.SetCheck("Load_SUPPORTPLUS") )
	{
		tcTimeCheck.GetTotalElapsed(dwElapsedSecs);
		tcTimeCheck.GetLastSet(dwLastSecs);

		if ( dwLastSecs != 0 )
		{
			if ( (dwElapsedSecs / SECS_IN_DAY) == 0 )
			{
				return dwLastError;
			}
		}
	}

	CString csLibrary;

	csLibrary = Profile.GetStr( "Server", "AppDir", "" );
	csLibrary += "\\";
	csLibrary += "LoadSprt.dll";

	hRPWI = ::LoadLibrary(csLibrary);

	if (hRPWI == NULL)
	{
		dwLastError = GetLastError();
		if ( hRPWI != NULL )
		{
			::FreeLibrary(hRPWI);
			hRPWI = NULL;
		}

		bRPWCheckGuard = FALSE;
		return dwLastError;
	}

	// OK. PCGW32.DLL is already loaded by protection code.
	// Get pointer to interface function
	lpRPW01Interface = (GetRPW01Interface)GetProcAddress(hRPWI, "RPW01");

	if ( !lpRPW01Interface )
	{
		bRPWCheckGuard = FALSE;
		if ( hRPWI != NULL )
		{
			::FreeLibrary(hRPWI);
			hRPWI = NULL;
		}

		dwLastError = 001;
		return dwLastError;
	}

	dwLoadTime = lpRPW01Interface(&PCGInterface, "Load_SUPPORTPLUS");

	tcTimeCheck.GetLastSet(dwLastSecs);
	if ( (dwLoadTime - dwLastSecs) != IDD_LOAD_DEVICE_SUPPORTPLUS || PCGInterface.PCGI_SiteCode != IDD_VERSION_NUMBER)
	{
		// Structure was not initialized by protection code!
		// It is possible that someone is debugging the program!//
		bRPWCheckGuard = FALSE;
		if ( hRPWI != NULL )
		{
			::FreeLibrary(hRPWI);
			hRPWI = NULL;
		}
		return PCGInterface.PCGI_SiteCode;
	}
/***********************************************************************
*
*	Structure is valid. We can now do the HandShake Test
*
***********************************************************************/

	// OK. PCGW32.DLL is already loaded by protection code.
	// Get pointer to interface function
	lpRPW02Interface = (GetRPW02Interface)GetProcAddress(hRPWI, "RPW02");

	if ( !lpRPW02Interface )
	{
		bRPWCheckGuard = FALSE;
		if ( hRPWI != NULL )
		{
			::FreeLibrary(hRPWI);
			hRPWI = NULL;
		}
		dwLastError = 002;
		return dwLastError;
	}

	RPWInterface.dwSiteCode = PCGInterface.PCGI_SiteCode;

	CTime time(CTime::GetCurrentTime());
	time.GetAsSystemTime(timeDest);

	if (( timeDest.wDayOfWeek % 2 ) == 0 )
		RPWInterface.iCustomLoad = CUSTOMLOAD_EVEN(timeDest.wMonth, timeDest.wDay, timeDest.wDayOfWeek) + IDD_LOAD_DEVICE_SUPPORTPLUS;
	else
		RPWInterface.iCustomLoad = CUSTOMLOAD_ODD(timeDest.wMonth, timeDest.wDay, timeDest.wDayOfWeek) + IDD_LOAD_DEVICE_SUPPORTPLUS;

	// Get interface structure
	RPWInterface.iRetCustomLoad = lpRPW02Interface(RPWInterface.iCustomLoad);

	CTime newTime(CTime::GetCurrentTime());
	newTime.GetAsSystemTime(timeDest);

	if (( timeDest.wDayOfWeek % 2 ) == 0 )
	{
		iTestDevIdx = RPWInterface.iRetCustomLoad - CUSTOMLOAD_ODD(timeDest.wMonth, timeDest.wDay, timeDest.wDayOfWeek);
	}
	else
	{
		iTestDevIdx = RPWInterface.iRetCustomLoad - CUSTOMLOAD_EVEN(timeDest.wMonth, timeDest.wDay, timeDest.wDayOfWeek);
	}

	if ( iTestDevIdx != IDD_LOAD_DEVICE_SUPPORTPLUS )
	{
		bRPWCheckGuard = FALSE;
		if ( hRPWI != NULL )
		{
			::FreeLibrary(hRPWI);
			hRPWI = NULL;
		}
		dwLastError = 003;
		return dwLastError;
	}

	RPWInterface.iAppStatus = PCGInterface.PCGI_ApplicationStatus;
	RPWInterface.iDemoActive = PCGInterface.PCGI_DemoModeActive;

	// This is the last thing we do because
	// we only want to reset if it works
	// Stamp the last time this was checked
	tcTimeCheck.ResetElapsed();

	if ( hRPWI != NULL )
	{
		::FreeLibrary(hRPWI);
		hRPWI = NULL;
	}

	return dwLastError;
}
#endif
#endif   // _Version_6
 

DWORD _cdecl PGPRCThreadProc(LPVOID pParam)
   {
   // cast the pointer to our mainframe
   CMainFrame* pMainframe = (CMainFrame*)pParam;

   // launch the server
   pMainframe->pgis_server = new CPGPRCServer;

   do
	  {
	  } while (1);

   return 0;
   }


// This routine is a thread processing function to read from and reply to a client
// via the open pipe connection passed from the main loop. Note this allows
// the main loop to continue executing, potentially creating more threads of
// of this procedure to run concurrently, depending on the number of incoming
// client connections.
DWORD _cdecl PFSThreadProc(LPVOID pParam)
{
	CString csPipeName;
	CString csError;

	BOOL fConnected; 
	DWORD dwThreadId; 

	HANDLE hPipe = NULL;
	HANDLE hThread = NULL;

	csPipeName = TEXT("\\\\.\\pipe\\");
	csPipeName += SERVERPIPE;

	// Vista
	hPipe = CreateNamedPipe( 
				csPipeName,
				PIPE_ACCESS_INBOUND,      // read access 
				PIPE_TYPE_MESSAGE | PIPE_WAIT,    // message type pipe 
																			// message-read mode 
																			// NON - blocking mode 
				PIPE_UNLIMITED_INSTANCES, // max. instances  
				BUFSIZE,                  // output buffer size 
				BUFSIZE,                  // input buffer size 
				0,                        // client time-out 
				NULL);                    // default security attribute 

	if (hPipe == INVALID_HANDLE_VALUE) 
	{
		DWORD dwLastError = GetLastError();
		_tprintf(TEXT("CreateNamedPipe failed, GLE=%d.\n"), GetLastError()); 

		return 1;
	}
 
	for (;;)
	{
		// Wait for the client to connect; if it succeeds, 
		// the function returns a nonzero value. If the function
		// returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 

//		fConnected = ConnectNamedPipe(hPipe, NULL); 
		fConnected = ConnectNamedPipe(hPipe, NULL) ? 
						TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 

		if (fConnected) 
		{ 
			printf("Client connected, creating a processing thread.\n"); 

			// Create a thread for this client. 
			hThread = CreateThread( 
							NULL,              // no security attribute 
							0,                 // default stack size 
							FileServerInstanceThread,    // thread proc
							(LPVOID) hPipe,    // thread parameter 
							0,                 // not suspended 
							&dwThreadId);      // returns thread ID 

			if (hThread == NULL) 
			{
				_tprintf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError()); 

				return -1;
			}
		} 
		else
		{
			csError.Format("PFSThreadProc - ConnectNamedPipe failed, GLE=%d.\n", GetLastError()); 
		}

		DisconnectNamedPipe(hPipe);
	}

	return 0; 
}

/*
	}
	else
	{ 
		_tprintf(TEXT("ConnectNamedPipe failed, GLE=%d.\n"), GetLastError()); 

		return;
	}

	printf("Client connected, creating a processing thread.\n"); 

	// Create a thread for this client. 
	hThread = CreateThread( 
					NULL,              // no security attribute 
					0,                 // default stack size 
					FileServerThread,  // thread proc
					(LPVOID)hPipe,    // thread parameter 
					0,                 // not suspended 
					&dwThreadId);      // returns thread ID 

	if (hThread == NULL) 
	{
		_tprintf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError()); 

		return;
	}

	CloseHandle(hThread);
	hThread = NULL;
	do
	{
	} while (1);

	return 0;
}
*/


DWORD WINAPI FileServerThread(LPVOID lpvParam)
{ 
   BOOL fConnected;
   CString csError;

   TCHAR tcRequest[_MAX_PATH];

   DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0; 
   BOOL fSuccess = FALSE;
   HANDLE hPipe  = NULL;
   HWND hWndServer = NULL;

// Do some extra error checking in a compound statement, this could be
// made more specific for different recovery routines if more robust
// recovery is needed. For this example we simply quit.
   if ( lpvParam == NULL )
   {
       printf( "\nERROR - Pipe Server Failure: FileServerThread has an unexpected NULL value.\n");
       return 0;
   }

// The thread's parameter is a handle to a pipe object instance. 
   hPipe = (HANDLE) lpvParam; 

   for (;;)
   {
		// Wait for the client to connect; if it succeeds, 
		// the function returns a nonzero value. If the function
		// returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 
		fConnected = ConnectNamedPipe(hPipe, NULL) ? 
				TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 

		if (fConnected) 
		{ 
//		RPDEBUGSTRMFC("ConnectNamedPipe - Success");
		}
		else
		{ 
		_tprintf(TEXT("ConnectNamedPipe failed, GLE=%d.\n"), GetLastError()); 
		}

// Loop until done reading
	   while (1) 
	   { 
	   // Read client requests from the pipe. 
		  fSuccess = ReadFile( 
			 hPipe,					// handle to pipe 
			 tcRequest,
			 _MAX_PATH * sizeof(TCHAR), // size of buffer 
			 &cbBytesRead,			// number of bytes read 
			 NULL);					// not overlapped I/O 

		  if ( fSuccess && cbBytesRead > 0 )
		  {
			  hWndServer = FindWindow((LPTSTR)SERVERCLASS30, (LPTSTR)NULL);
			  if ( cbBytesRead > 5 )
			  {
				  // Add File Server Job to Queue
				  SendMessage(hWndServer, wm_AddJobToQueue, 0, 0);	  
			  }
			  else
			  {
				// Tell End of File
				SendMessage(hWndServer, wm_ServerFileEOF, 0, 0);
			  }
		  }

			DisconnectNamedPipe(hPipe);
			fConnected = ConnectNamedPipe(hPipe, NULL) ? 
				TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 

			if (fConnected) 
			{ 
//				RPDEBUGSTRMFC("ConnectNamedPipe - Success");
			}
			else
			{ 
				_tprintf(TEXT("ConnectNamedPipe failed, GLE=%d.\n"), GetLastError()); 
			}
	   }
   }
  
   // Flush the pipe to allow the client to read the pipe's contents 
   // before disconnecting. Then disconnect the pipe, and close the 
   // handle to this pipe instance. 
   FlushFileBuffers(hPipe); 
   DisconnectNamedPipe(hPipe); 
   CloseHandle(hPipe); 

   return 1;
}

DWORD WINAPI FileServerInstanceThread(LPVOID lpvParam)
{

   TCHAR tcRequest[_MAX_PATH];

   DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0; 
   BOOL fSuccess = FALSE;
   HANDLE hPipe  = NULL;
   HWND hWndServer = NULL;

// Do some extra error checking in a compound statement, this could be
// made more specific for different recovery routines if more robust
// recovery is needed. For this example we simply quit.
   if ( lpvParam == NULL )
   {
       printf( "\nERROR - Pipe Server Failure: FileServerThread has an unexpected NULL value.\n");
       return 0;
   }
   else
   {
       printf( "\nERROR - Pipe Server Failure: FileServerThread has an unexpected NULL value.\n");
   }


// The thread's parameter is a handle to a pipe object instance. 
   hPipe = (HANDLE) lpvParam; 

// Loop until done reading
   while (1) 
   { 
   // Read client requests from the pipe. 
      fSuccess = ReadFile( 
         hPipe,							// handle to pipe 
		 tcRequest,
         _MAX_PATH * sizeof(TCHAR),		// size of buffer 
         &cbBytesRead,					// number of bytes read 
         NULL);							// not overlapped I/O 

     if (! fSuccess || cbBytesRead == 0) 
         break; 

	  hWndServer = FindWindow((LPTSTR)SERVERCLASS30, (LPTSTR)NULL);
	  if ( cbBytesRead > 5 )
	  {
		  // Add File Server Job to Queue
		  SendMessage(hWndServer, wm_AddJobToQueue, 0, 0);	  
	  }
	  else
	  {
		// Tell End of File
		SendMessage(hWndServer, wm_ServerFileEOF, 0, 0);
	  }
   } 
  
   // Flush the pipe to allow the client to read the pipe's contents 
   // before disconnecting. Then disconnect the pipe, and close the 
   // handle to this pipe instance. 
   FlushFileBuffers(hPipe); 
   DisconnectNamedPipe(hPipe); 
   CloseHandle(hPipe); 

   return 1;
}

   
