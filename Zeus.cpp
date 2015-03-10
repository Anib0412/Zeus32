/////////////////////////////////////////////////////////////////////////////
// ZEUS.CPP
//                                                                        
// main module for RasterPlus
// Copyright (C) 1996-2001 Graphx, Inc.
//
// mods:
//
// 7-08-96  Build 40 - added 16/256 color splash screen
// 7-23-96  Build 40 - added DEMO code
// 8-08-96  Build 41 - checked Mnemonics and tab order on dialogs and menus
// 2-26-97  Build 48 - fixed device list in setup for MG1 and MG2
// 4-28-97  Build 49 - more device list fixes for MG1 and MG2
// 4-28-97  Build 49 - SETUP fixes - Back buttons all work properly, 100% == final
// 4-29-97  Build 49 - added Dongle code
// 5-06-97  Build 49 - RP95 help file was missing bitmaps - fixed
// 12-12-97 Version 2.0.1 - new dongle code for multiple dongles
// 2-6-98   Version 2.0.2 - fixed FJ2 installation for SCSI ports
// 4-16-98  Version 2.0.3 - fixed ASPI installer reboot problem
// 10-20-98 Version 2.0.5 - added device installation routines
// 11-10-98 Version 2.0.5 - our uninstaller not getting loaded - fixed
// 1-15-99  Version 3.0.0 - work begins
// 7-23-99  Version 3.0.0 - do the dongle and install printer reboot all at once
// 10-15-99 Version 3.0.1 - always load workspace, even on bInstallPrinterAndExit
// 3-07-00  Version 3.0.3 - automatically set farport key
// 3-14-00  Version 3.0.3 - added fuji logo to splash screens for fuji versions
// 3-20-00  Version 3.0.3 - don't allow creation of queue with dup name
// 11-05-00 Version 4.0.1 - if serverflag is set, start minimized
// 11-27-00 Version 4.0.2 - don't install printer if error return from makespd proc
// 08-20-01 Version 5.0.0 - Code to support separate PPD files for each Queue
// 08-23-01 Version 5.0.0 - Disable check dongle code - moved to individual drivers
// 09-18-01 Version 5.0.0 - list host IP in about box
// 10-10-01 Version 5.0.0 - rename workspace to	rasterplus_2.wsp
// 10-18-01 Version 5.0.0 - not properly deleting .pcf files
// 10-23-01 Version 5.0.0 - Check PCF filenames ... replace invalid characters
// 11-30-01 Version 5.0.1 - Remove Sentinel Dongle Code
// 01-10-02 Version 5.0.1 - Add Function to Return Device ID for a QName Correct NT problem that prevented
//                          file printing to a second queue for the same device
// 10-28-02 Version 5.1.0 - Add Function to allow the SplashScreen to be 'forced' to close
//

#include "stdafx.h"
#include "afxole.h"
#include "zeus.h"
#include "mainfrm.h"
#include "zeusdoc.h"
#include "RasterPlusView.h"
#include "holdtree.h"
#include "printtree.h"
#include "archivetree.h"
#include "sysmem.h"
#include "servdefs.h"
#include "QueueChild.h"
#include "profile.h"

#include "l_bitmap.h"
#include "l_error.h"

#include "..\pcgi\pcgi.h"
#include "..\Microsoft Platform SDK for Windows Server 2003 R2\Include\WinNT.h"

#ifndef _DEBUG
#define SHOWSPLASH 1
#endif
#include "InstallPrinter.h"
#include <string.h>
#ifdef DEMO
#include "DemoDlg.h"
#endif
#include "OurWorkspace.h"
#include "NewQueue.h"
#include <tlhelp32.h>
#include "..\CmnLibs\psapi\psapi.h"
#include "profile.h"
#include "winspool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);

/////////////////////////////////////////////////////////////////////////////
// CZeusApp

BEGIN_MESSAGE_MAP(CZeusApp, CWinApp)
	//{{AFX_MSG_MAP(CZeusApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)		
	ON_COMMAND(ID_FILE_NEW, OnMyFileNew)		
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based document commands		
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZeusApp construction

CZeusApp::CZeusApp()
   { 
   iGlobalJobID = 1;
   bClassRegistered = FALSE;
   }

int CZeusApp::CreateNewDoc(BOOL bFromInstaller, BOOL bAskOpenExisting, BOOL bAsk, LPCSTR csName, LPCSTR csDevice, 
							int iDevIdx, LPCSTR csPort, int iInstallPrinter)
   {
   CFileFind Finder;

   csRPDebugStr.Format("CreateNewDoc is %s\n", "Starting");
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr); 

   CInstallPrinter* pCins;
//   CInstallPrinter cins;
   CNewQueue nq;
   CString csErr;
   CMDIChildWnd* pCmChild;
   BOOL bMaximized;
   CZeusDoc* pDoc;
   BOOL NeedToReboot = FALSE;
   CDevice* pCD;   
   CColorTransform* pCC;
   CString csNewDocName;   
   BOOL bRet;
   CFile cf;
   CFileStatus cfs;
   CString csTest;
   CString csTemp;
   CString csTemp2;
   CString csPrompt;
   CString csTitle;

   pCins = NULL;
   pCD = NULL;
   pCC = NULL;

   // if we are launching from the installer, put it
   // on the bottom so we don't collide with
   // install from the web

   if (bFromInstaller)
	  {
	  nq.bOnBottom = TRUE;
	  // don't do copy thing
	  nq.bMakeCopyIfExist = FALSE;
	  }
   
   // do we want to ask about this?
   if (bAsk)
	  {
DOAGAIN:
	  if ((bRet = nq.DoModal()) == IDCANCEL)
		 return CREATENEWDOC_CANCEL;

	  if (OpenQueueExist(nq.m_name) && !bFromInstaller)
		 {
		 CString csMsg;
		 csMsg.Format("%s is already open", nq.m_name);
		 AfxMessageBox(csMsg);
		 goto DOAGAIN;
		 }
	  }
   else
	  {
	  // load the passed values
	  nq.csDevice = csDevice;
	  nq.m_name = csName;
	  nq.iDevIdx = iDevIdx;
	  nq.csPort = csPort;	  

	  // get Queue path
	  csNewDocName = GetAppPath();	  
	  // create new doc name	  
      csTitle = nq.m_name;
      // get rid of illegal chars
      NukeBadFnameChars(csTitle);
	  csNewDocName += csTitle;
	  }

   // if the user answered OK, OR we aren't in the mood to ask....
   if (bRet == IDOK || !bAsk)
	  {			
	  // always install a printer
//	  if (!cins.InstallPrinter(nq.csDevice, nq.m_name, nq.iDevIdx, nq.csPort, 
	  pCins = new CInstallPrinter;

	  if ( pCins )
	  {
		  if (!pCins->InstallPrinter(nq.csDevice, nq.m_name, nq.iDevIdx, nq.csPort, 
													csErr, NeedToReboot, FALSE, TRUE, TRUE))
			 {
   			 csTemp.LoadString(IDS_ERROR_INSTALL_PRINTER);
			 csPrompt.Format("%s\n%s", csTemp, csErr);
			 AfxMessageBox(csPrompt);

			 delete pCins;
			 return CREATENEWDOC_CANCEL;		
			 }

		  delete pCins;
	  }
	  else
	  {
		   csRPDebugStr.Format("CInstallPrinter %s\n", "Create Failed");
		   strcpy(cDebugStr, csRPDebugStr);
		   RPDEBUGSTRMFC(cDebugStr);
	  }


	  // get app path
	  csNewDocName = GetQueuePath();
	  // create new doc name	  
      csTitle = nq.m_name;
      // get rid of illegal chars
      NukeBadFnameChars(csTitle);
	  csNewDocName += csTitle;
	  csTest = csNewDocName;
	  csTest += ".que";

	  // does the queue already exist?
	  if (cf.GetStatus(csTest, cfs))
		 {
		 // should we ask if they want it?
		 if (bAskOpenExisting)
			{
			// then do it
		    csPrompt.LoadString(IDS_OPENEXISTING_QUEUE);
		    csTemp.Format(csPrompt, nq.m_name);
		    if (AfxMessageBox(csTemp, MB_YESNO | MB_SETFOREGROUND) == IDYES)
			   {
		       CWinApp::OpenDocumentFile(csTest);
			   // save our state
			   ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->SaveState();
			   return (NeedToReboot ? CREATENEWDOC_OK_REBOOT : CREATENEWDOC_OK);
			   }
			}
		 else
			{
			// otherwise, just do it anyway
			CWinApp::OpenDocumentFile(csTest);		
			// save our state
			((CMainFrame*)(AfxGetApp()->m_pMainWnd))->SaveState();
			return (NeedToReboot ? CREATENEWDOC_OK_REBOOT : CREATENEWDOC_OK);
			}
		 }

	  // create a new doc
	  CWinApp::OnFileNew();	

      pCmChild = 
		 (CMDIChildWnd*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->MDIGetActive(&bMaximized);
      if (pDoc = (CZeusDoc*)(pCmChild->GetActiveDocument()))      
		 {  		 
		 //pDoc->SetPathName(csNewDocName, FALSE);  
		 pDoc->SetTitle(nq.m_name);
		 // store it
		 pDoc->csTitle = nq.m_name;
         // create a new device
         pCD = new CDevice;	
         pCC = new CColorTransform;	   
         // load it
		 if (pCD->LoadDevice(nq.iDevIdx))
			{
			// set the friendly name
			pCD->csFriendlyName = nq.m_name;
			// map the connection
			//pCD->MapConnect(nq.csPort);
			// store the connection
			strcpy(pCD->pDevInfo->ConnectStr, nq.csPort);
			pCC->LoadDefaults();
			strncpy(pCC->CPInitStruct.Printer_Name , pCD->Name,MAX_PATH);			
			pCC->CPInitStruct.GlobalPrinterID = nq.iDevIdx;
			pCC->CPInitStruct.Printer_CSpace = pCD->pDevInfo->iColorSpace; 
			pCC->LoadDefaultProfiles();
			// set doc device to this one
	        pDoc->QueueDevice = *pCD;         
	        pDoc->QueueColor = *pCC;         
            pDoc->UpdateAllViews( NULL );                     
	        // save it right away for menu reasons			
            pDoc->OurSaveDocument();
			// kluge back the titles			
			CString csTemp;
			csTemp = GetQueuePath();
			csTemp2 = nq.m_name;
			NukeBadFnameChars(csTemp2);
			csTemp += csTemp2;
			csTemp += ".que";
			pDoc->SetPathName(csTemp);
			pDoc->SetTitle(nq.m_name);
			((CMainFrame*)(AfxGetApp()->m_pMainWnd))->AddDevNamesToMenu();
			}
		 delete pCD;
		 delete pCC;
		 }	  
	  }
   // save our state
   ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->SaveState();
   return (NeedToReboot ? CREATENEWDOC_OK_REBOOT : CREATENEWDOC_OK);
   }
   
BOOL CZeusApp::OpenQueueExist(LPCSTR csName)
   {
   int i;
   CPaneElement* pRipElement;   
   CZeusDoc* pDoc;
   BOOL bRet = FALSE;

   for (i = 0; i < ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pPaneArray.GetSize(); i++)
	  {
	  pRipElement = (CPaneElement*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pPaneArray.GetAt(i);

	  if (!pRipElement) continue;

      // get the active document
      if (pDoc = (CZeusDoc*)(pRipElement->pDocPtr))
		 {
 		 // is it our device and our device alias and the proper queue style?			
		 if (pDoc->csTitle.Compare(csName) == 0)
			{
			bRet = TRUE;
			break;			
			}   
         }   
	  }
   return bRet;
   }

int CZeusApp::GetQueueDevice(LPCSTR csName)
   {
   int i;
   CPaneElement* pRipElement;   
   CZeusDoc* pDoc;
   int iDevNum = -1;;
   BOOL bRet = FALSE;

   for (i = 0; i < ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pPaneArray.GetSize(); i++)
	  {
	  pRipElement = (CPaneElement*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pPaneArray.GetAt(i);

	  if (!pRipElement) continue;

      // get the active document
      if (pDoc = (CZeusDoc*)(pRipElement->pDocPtr))
		 {
 		 // is it our device and our device alias and the proper queue style?			
		 if (pDoc->csTitle.Compare(csName) == 0)
			{
			iDevNum = pDoc->QueueDevice.DevIdx;
			break;			
			}   
         }   
	  }
   return iDevNum;
   }

/////////////////////////////////////////////////////////////////////////////
// The one and only CZeusApp object

//CZeusApp NEAR theApp;
CZeusApp theApp;

BOOL CZeusApp::InitApplication()
{
   // get os version
   OSVERSIONINFO os;
   os.dwOSVersionInfoSize = sizeof OSVERSIONINFO;
   GetVersionEx(&os);
   OSVersion = os.dwPlatformId;
   OSMinorVersion = os.dwMinorVersion;   
   OSMajorVersion = os.dwMajorVersion; 
//   wProductType = os.wProductType;

   csRPDebugStr.Format("dwPlatformId is %d\n", os.dwPlatformId);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr); 

   csRPDebugStr.Format("OSMajorVersion is %d\n", os.dwMajorVersion);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr); 

   csRPDebugStr.Format("OSMinorVersion is %d\n", os.dwMinorVersion);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr); 

   iProcessorType = GetProcessorType();

   csRPDebugStr.Format("iProcessorType is %d\n", iProcessorType);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr); 

   //Call base class.  Default version does nothing.
   CWinApp::InitApplication();     
   
   WNDCLASS wndcls;
   memset(&wndcls, 0, sizeof(WNDCLASS));   // start with NULL defaults

   // Get class information for default window class.
   //::GetClassInfo(AfxGetInstanceHandle(),"AfxFrameOrView",&wndcls);

   // in VC4, AfxFrameOrView doesn't exist so we must
   // manually create window class
   wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;   
   wndcls.hbrBackground = (HBRUSH)COLOR_WINDOW;
   wndcls.hInstance = AfxGetInstanceHandle();
   wndcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);   
   wndcls.lpfnWndProc = DefWindowProc;

   // Substitute unique class name for new class
   wndcls.lpszClassName = SERVERCLASS30;
   // add our own icon  
   wndcls.hIcon = CWinApp::LoadIcon(IDR_MAINFRAME);	

   if (AfxRegisterClass(&wndcls))
	  {
	  bClassRegistered = TRUE;
	  return TRUE;
	  }
   else
	  return FALSE;
   }
      
/////////////////////////////////////////////////////////////////////////////
// CPhotogizeLabApp initialization

BOOL CZeusApp::CheckActivation()
{
   BOOL bReturn = TRUE;
   CProfile Profile;

   PCG_INTERFACE  PCGInterface;
   int	 InterfaceStatus;
   CLicenseManager dlg;
   CString csPrompt;
   CString csAppTitle;

   int iActState = 0;

   CString csRPDebugStr;
   char cDebugStr[255];

   csAppTitle.LoadString(AFX_IDS_APP_TITLE);   

   // structure size must be set before calling interface functions
   PCGInterface.PCGI_Size = sizeof (PCG_INTERFACE);
   
   // get interface data
   InterfaceStatus = GetInterfaceData(&PCGInterface);
   
   // check return value
   if (InterfaceStatus != PCGI_STATUS_OK)
	  {	 
	  return FALSE;
	  }

   	csRPDebugStr.Format("%s, %s\n", "CheckActivation", "Starting");
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr); 

   dlg.CheckStatus(iActState);

   	csRPDebugStr.Format("%s, %d\n", "CheckStatus", iActState);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr); 

   	csRPDebugStr.Format("%s, %d\n", "PCGInterface.PCGI_ApplicationStatus = ", PCGInterface.PCGI_ApplicationStatus);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr); 

   if (!PCGInterface.PCGI_ApplicationStatus)
	  {	  
   	csRPDebugStr.Format("%s, %d\n", "PCGInterface.PCGI_DemoDaysLeft = ", PCGInterface.PCGI_DemoDaysLeft);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr); 

	  if (PCGInterface.PCGI_DemoDaysLeft > 0 && iActState != 2)	// iActState = 2 (Did Not Renew)
		 {
		 csPrompt.Format("RasterPlus will be locked in %d days.  Click OK to activate now or Cancel to continue", PCGInterface.PCGI_DemoDaysLeft);
		 if (::MessageBox(NULL, csPrompt, csAppTitle, MB_OKCANCEL | MB_SETFOREGROUND) == IDOK)
			{
			dlg.DoModal();			
			}
		 bReturn = TRUE;
		 }
	  else
		 {	
		 if (::MessageBox(NULL, "RasterPlus is locked.  Click OK to activate now or Cancel to exit", csAppTitle, MB_OKCANCEL | MB_SETFOREGROUND) == IDOK)
			{
			dlg.DoModal();

			// check again
			InterfaceStatus = GetInterfaceData(&PCGInterface);
			// check return value
			if (InterfaceStatus != PCGI_STATUS_OK)
			   bReturn = FALSE;
			else if (!PCGInterface.PCGI_ApplicationStatus)
			   bReturn = FALSE;
			}
		 else
			bReturn = FALSE;
		 }
	  }
   else
   {
	  switch ( iActState )
	  {
	  case 0:	// Everything OK
	  default:
		  bReturn = TRUE;
		  break;
	  case 1:	// Maintenance Past Due Warning
		  // Display a Dialog box advising of Past Due Invoices
		  csPrompt.Format("This RasterPlus Serial Number %s has Past Due Invoices.\r\n\r\nPlease Contact the Graphx Accounting Department.\r\nThank You.", Profile.GetStr("User Information", "User SerialNumber", ""));
		  AfxMessageBox(csPrompt, MB_OK | MB_SETFOREGROUND);
		  bReturn = TRUE;
		  break;
	  case 2:	// Remove and Lock License
		  dlg.AutoRemove();
		  bReturn = FALSE;
		  break;
	  }
   }

   	csRPDebugStr.Format("%s, %s\n", "CheckActivation", "Finished");
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr); 

	return bReturn;
} 

/////////////////////////////////////////////////////////////////////////////
// CZeusApp initialization

BOOL CZeusApp::InitInstance()
   {   
   BOOL bServerFlag = FALSE;      
   BOOL bHideIt = FALSE;   
   BOOL bInit = FALSE;

   CString csAppPath;   
   CString csAppPathShort;   

   TCHAR tcLTPDFDir[(_MAX_PATH * 1) * 3];
   int iLTRet = 0;
   CString csLTPSData;
   CProfile Profile;

#ifdef RP_6
//   HINSTANCE hPkgPlusLib;
#endif

   int iMustRebootForDevice = 0;
   int iRet;   

   bInstallPrinterAndExit = FALSE;   
   	
   AfxEnableControlContainer();	    
   
#ifdef USEKANJI
   // set resource handle to our language dll
   HINSTANCE hInst = ::LoadLibrary("rp_Kanji.dll");
   if (hInst != NULL)
      {
	  //m_pszAppName = "RasterPlus95";
	  m_pszAppName = _tcsdup("RasterPlus");
      AfxSetResourceHandle(hInst);
	  }	  
#endif

	  m_pszAppName = _tcsdup("RasterPlus");

/*
#ifdef RP_6
	  hPkgPlusLib = LoadLibrary("PkgPlus_DLL.dll");
	  if (hPkgPlusLib == NULL )
	  {
		LPVOID lpMessageBuffer; 

		DWORD dwErr = GetLastError();

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
						FORMAT_MESSAGE_FROM_SYSTEM,
						NULL,
						dwErr,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //The user default language
						(LPTSTR) &lpMessageBuffer,
						0,
						NULL );
	  }


#endif
*/

#ifdef DEMO
   // check date if demo copy         
   // system time must be between the times listed below
   CTime BeginTime( 1999, 5, 20, 22, 15, 0 );   // 10:15PM May 20, 1999
   CTime EndTime( 2001, 12, 30, 22, 15, 0 );   // 10:15PM December 30, 2001
   CTime NowTime = CTime::GetCurrentTime();
   if ((NowTime > EndTime) || (NowTime < BeginTime))
      {
	  AfxMessageBox(IDS_EVALEXPIRED);
	  return FALSE;
	  }   
#endif

   SetRegistryKey(_T("Graphx"));

   // create a bitmap engine mutex
   InitializeCriticalSection(&BitmapEngineMutex);

   if (!FirstInstance())      // If this isn't the first instance, return FALSE
      return FALSE;            // immediately.  FirstInstance() will have already

   // get queue path
   CString csQueuePath;
   csQueuePath = GetQueuePath();
   // create queue folder if it doesn't exist
   ::CreateDirectory(csQueuePath, NULL);

	// Initialize PackagePlus DLL - will add doc templates and make other class
	//   implementations available to this application
//	InitPackagePlusDLL();

   // get server flag	     
   if (m_lpCmdLine[0] != '\0')
	   {   
	   // special minimized operation
	   if (m_lpCmdLine[0] == '/' && m_lpCmdLine[1] == 'm')
	      {		  
	      bServerFlag = TRUE;
	      }
	   // install printer(s) if a /p is encountered	   
	   else if (m_lpCmdLine[0] == '/' && m_lpCmdLine[1] == 'p')
	      {		  			  		  
		  bInstallPrinterAndExit = TRUE;   		  
		  }
	   // hide the app if a /h is encountered	   
	   else if (m_lpCmdLine[0] == '/' && m_lpCmdLine[1] == 'h')
	      {		  			  		  
		  bHideIt = TRUE;   		  
		  }
	   }   

//#ifndef _DEBUG
   if ( !bInstallPrinterAndExit )
   {
	   if (!CheckActivation())
	   {
		   return FALSE;
	   }
   }
//#endif

   // init OLE	  
   bInit = AfxOleInit();

//   if (!AfxOleInit())
   if (bInit == FALSE)
      {
		 AfxMessageBox(_T("Ole Initialization Failed"));
         return FALSE;
      }				


#ifdef SHOWSPLASH     
   if (!bServerFlag && !bInstallPrinterAndExit && !bHideIt)
      {
#ifdef DEMO
	  CDemoDlg ddlg;

	  if (ddlg.DoModal() == IDCANCEL)
		 return FALSE;	  
#else
      ASSERT(NULL == m_pwndSplash);
	  m_pwndSplash = new CSplashWnd;
	  m_pwndSplash->Create();
	  m_pwndSplash->ShowWindow(SW_SHOW);
	  m_pwndSplash->UpdateWindow();
	  ASSERT(m_pwndSplash != NULL);
#endif
	  }
#endif
#ifdef LAB_STYLE
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

   // Standard initialization
   // If you are not using these features and wish to reduce the size
   //  of your final executable, you should remove from the following
   //  the specific initialization routines you do not need.
   LoadStdProfileSettings(0); // Load standard INI file options (NOT including MRU)		

   // Register the application's document templates.  Document templates
   //  serve as the connection between documents, frame windows and views.
#ifdef LAB_STYLE
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
      IDR_ZEUSTYPE,	  
	  RUNTIME_CLASS(CZeusDoc),
	  RUNTIME_CLASS(CQueueChild),        // standard SDI child frame      
	  RUNTIME_CLASS(CRasterPlusView));
	AddDocTemplate(pDocTemplate);
#else
   CMultiDocTemplate* pDocTemplate;

   pDocTemplate = new CMultiDocTemplate(

      IDR_ZEUSTYPE,	  
	  RUNTIME_CLASS(CZeusDoc),
	  RUNTIME_CLASS(CQueueChild),        // standard MDI child frame      
	  RUNTIME_CLASS(CRasterPlusView));
   AddDocTemplate(pDocTemplate);
#endif

   // create main MDI Frame window
   CMainFrame* pMainFrame = new CMainFrame;
   if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
      return FALSE;

   m_pMainWnd = pMainFrame;
   
#ifdef LAB_STYLE
	CMenu* pMenu = m_pMainWnd->GetMenu();
	if (pMenu)
		pMenu->DestroyMenu();
	HMENU hMenu = ((CMainFrame*) m_pMainWnd)->NewMenu();
	pMenu = CMenu::FromHandle( hMenu );
	m_pMainWnd->SetMenu(pMenu);
	((CMainFrame*)m_pMainWnd)->m_hMenuDefault = hMenu;	
#endif

   // load workspace
   COurWorkspace* pWsMgr = (COurWorkspace*)pMainFrame->GetWorkspaceMgrEx();	   
   if (pWsMgr)   
	  {
	  CString csWorkspaceFile;
	  csWorkspaceFile = ((CZeusApp*)AfxGetApp())->GetWorkSpaceFileName();
	  pWsMgr->OpenWorkspace(csWorkspaceFile, TRUE);
	  // if we load with no queues, the workspace screws up
	  // so we need to do this horrible hack

	  if (!((CMainFrame*)(AfxGetApp()->m_pMainWnd))->MDIGetActive())
		 pWsMgr->SWACP();
	  }		 

//   _tcscpy(tcLTPDFDir, TEXT("C:\\Program Files\\Graphx\\RasterPlus\\Leadtools\\PDF\\Lib;C:\\Program Files\\Graphx\\RasterPlus\\Leadtools\\PDF\\Fonts;C:\\Program Files\\Graphx\\RasterPlus\\Leadtools\\PDF\\Resources;"));

   csLTPSData = Profile.GetStr("LeadToolsPostScript", "Lib");
   _tcscpy(tcLTPDFDir, csLTPSData);
   _tcscat(tcLTPDFDir, ";");
   csLTPSData = Profile.GetStr("LeadToolsPostScript", "Font");
   _tcscat(tcLTPDFDir, csLTPSData);
   _tcscat(tcLTPDFDir, ";");
   csLTPSData = Profile.GetStr("LeadToolsPostScript", "Resource");
   _tcscat(tcLTPDFDir, csLTPSData);
   _tcscat(tcLTPDFDir, ";");
   
//   iLTRet = L_SetPDFInitDir(tcLTPDFDir);
   // if we are installing printers, don't show anything...
	if (bInstallPrinterAndExit)    	   
	   m_nCmdShow = SW_HIDE;
	else if (bHideIt)
	   m_nCmdShow = SW_HIDE;
	else if (bServerFlag) 
	   m_nCmdShow = SW_SHOWMINIMIZED;	
	else
	   {	   
       if (pWsMgr)
		  {
          // show maximized if flag is set
	      if (pWsMgr->bMainWndMaximized) 
			 m_nCmdShow = SW_SHOWMAXIMIZED;		  
		  }
	   }

   // we accept drag and drop files	
   m_pMainWnd->DragAcceptFiles(TRUE);			

   // we don't have true .que files so disable these
   //EnableShellOpen();
   //RegisterShellFileTypes();	

   // The main window has been initialized, so show and update it.
   pMainFrame->ShowWindow(m_nCmdShow);
   //pMainFrame->ShowWindow(SW_HIDE);
   
   pMainFrame->UpdateWindow();   
	
   m_dwSplashTime = ::GetCurrentTime();

   CleanUpRipprocs();

   // if this flag is set, install printers and exit
   int iNumPrintersInstalled = 0;
   if (bInstallPrinterAndExit)
	  {	  
	  do
		 {		 
		 iRet = CreateNewDoc(TRUE, FALSE);
		 if (iRet == CREATENEWDOC_OK_REBOOT) 
			{
			iNumPrintersInstalled++;
			iMustRebootForDevice++;
			}
		 else if (iRet == CREATENEWDOC_CANCEL) 
			break;
		 else
			iNumPrintersInstalled++;
		 } while (AfxMessageBox("Create another new Print Queue?", 
			MB_YESNO | MB_SETFOREGROUND | MB_DEFBUTTON2) == IDYES);
      COurWorkspace* pWsMgr = (COurWorkspace*)pMainFrame->GetWorkspaceMgrEx();

	  // save space only if we have installed printers
      if (pWsMgr && iNumPrintersInstalled > 0)	  
		 {
		 CString csWorkspaceFile;
		 csWorkspaceFile = ((CZeusApp*)AfxGetApp())->GetWorkSpaceFileName();
         ::DeleteFile(csWorkspaceFile);
         pWsMgr->SaveWorkspace(csWorkspaceFile);
		 }

	  // if we need to reboot because of the device installation
	  // inform now
	  if (iMustRebootForDevice)
		 AfxMessageBox(IDS_NEEDTOREBOOT);   

	  return FALSE;
	  }
   else
	  return TRUE;
   }

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
    
// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_organization;
	CString	m_username;
	CString	m_avail_virt;
	CString	m_installed_mem;
	CString	m_avail_phys;
	CString	m_version;
	CString m_productname;
	CString	m_serialnumber;
	CString	m_ip;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};       

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{   	
	//{{AFX_DATA_INIT(CAboutDlg)
	m_organization = _T("");
	m_username = _T("");
	m_avail_virt = _T("");
	m_installed_mem = _T("");
	m_avail_phys = _T("");
	m_version = _T("");
	m_productname = _T("");
	m_serialnumber = _T("");
	m_ip = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_ORGANIZATION, m_organization);
	DDX_Text(pDX, IDC_USERNAME, m_username);
	DDX_Text(pDX, IDC_FREE_VIRTUAL, m_avail_virt);
	DDX_Text(pDX, IDC_INSTALLED_MEM, m_installed_mem);
	DDX_Text(pDX, IDC_FREE_PHYSICAL, m_avail_phys);
	DDX_Text(pDX, IDC_VERSION, m_version);
	DDX_Text(pDX, IDC_PRODUCT_NAME, m_productname);
	DDX_Text(pDX, IDC_SERIALNUMBER, m_serialnumber);
	DDX_Text(pDX, IDC_IP, m_ip);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// App command to run the dialog
void CZeusApp::OnAppAbout()
   {
   CAboutDlg aboutDlg;   
   aboutDlg.DoModal();
   } 

BOOL CZeusApp::ExitInstance()
   {        
   // delete the mutex
   DeleteCriticalSection(&BitmapEngineMutex);

   // clean up any stray ripprocs
   CleanUpRipprocs();

   // delete all job ids
   DeleteAllJobIDs();

   if (bClassRegistered)
	  ::UnregisterClass(SERVERCLASS30, AfxGetInstanceHandle());

   return CWinApp::ExitInstance();
   }
   	
/////////////////////////////////////////////////////////////////////////////
// CZeusApp commands

BOOL CZeusApp::OnIdle(LONG lCount)
{
	// call base class idle first
	BOOL bResult = CWinApp::OnIdle(lCount);


#ifdef SHOWSPLASH
	// then do our work
	if (m_pwndSplash != NULL)
	{
		if (m_pwndSplash->m_hWnd != NULL)
		{
			if (::GetCurrentTime() - m_dwSplashTime > 2500)
			{
				// timeout expired, destroy the splash window
				m_pwndSplash->DestroyWindow();
				delete m_pwndSplash;
				m_pwndSplash = NULL;
				m_pMainWnd->UpdateWindow();
				// NOTE: don't set bResult to FALSE,
				//  CWinApp::OnIdle may have returned TRUE
			}
			else
			{
				// check again later...
				bResult = TRUE;
			}
		}
	}
#endif
	return bResult;
}			   

BOOL CZeusApp::PreTranslateMessage(MSG *pMsg)
{
#ifdef SHOWSPLASH
    if (m_pwndSplash != NULL)
    {
    	if (pMsg->message == WM_KEYDOWN ||
        	pMsg->message == WM_SYSKEYDOWN ||
        	pMsg->message == WM_LBUTTONDOWN ||
        	pMsg->message == WM_RBUTTONDOWN ||
        	pMsg->message == WM_MBUTTONDOWN ||
        	pMsg->message == WM_NCLBUTTONDOWN ||
        	pMsg->message == WM_NCRBUTTONDOWN ||
        	pMsg->message == WM_NCMBUTTONDOWN)
        {
        if (pMsg->hwnd == m_pwndSplash->m_hWnd) 
           m_dwSplashTime -= 2500;
		else
		   {
		   // timeout expired, destroy the splash window
		   m_pwndSplash->DestroyWindow();
		   delete m_pwndSplash;
		   m_pwndSplash = NULL;
		   m_pMainWnd->UpdateWindow();
		   }
		}
	}
#endif 	
 	return CWinApp::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CSplashWnd

CSplashWnd::CSplashWnd()
   {            
   pSplashFont = NULL;
   }

CSplashWnd::~CSplashWnd()
   {   
   m_bmSplash.DeleteObject();
   m_bmSplash16.DeleteObject();
   }

BOOL CSplashWnd::Create()
   {      
   // get seial number
   CProfile Profile;
   CString csSerial;
   csSerial = Profile.GetStr(IDS_USERINFO, IDS_SERIALNUMBER);

   // these versions are fuji versions
   if (csSerial.Mid(4, 5) == "52007" || csSerial.Mid(4, 5) == "52008" || 
	  csSerial.Mid(4, 5) == "52041")
	  {
      m_bmSplash.LoadBitmap(IDB_SPLASHWND_NEW_FUJI);
      // load the 16 color bitmap in case we
      // have a 16 color system (we don't know
      // this yet) but use the standard 256 color
      // bmp for sizing
      m_bmSplash16.LoadBitmap(IDB_SPLASHWND_256_NEW_FUJI);
	  }
   else
	  {
      m_bmSplash.LoadBitmap(IDB_SPLASHWND_VERSION6);
      m_bmSplash16.LoadBitmap(IDB_SPLASHWND_VERSION6);
//      m_bmSplash.LoadBitmap(IDB_SPLASHWND_NEW);
//      m_bmSplash16.LoadBitmap(IDB_SPLASHWND_256_NEW);
	  }	
	
   BITMAP bm;
   m_bmSplash.GetObject(sizeof(BITMAP), &bm);
	
   // Get the size of the splash window
   m_wndWidth = bm.bmWidth;
   m_wndHeight = bm.bmHeight;

   // Get the size of the screen
   int screenWidth = GetSystemMetrics(SM_CXSCREEN);
   int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	
   // get top/left coord to center the splash window
   int top = (screenHeight - m_wndHeight)/2;
   int left = (screenWidth - m_wndWidth)/2;
	
   return CWnd::CreateEx(WS_EX_TOPMOST, 
	   AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
       ::LoadCursor(NULL, IDC_ARROW)),       
	   _T(""), WS_POPUP | WS_VISIBLE,
      left, top, m_wndWidth, m_wndHeight, NULL, NULL);   	   
   }

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplashWnd message handlers

void CSplashWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CBitmap* pOld;

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	// load bitmap based on the number of colors displayed on the system
	if (dc.GetDeviceCaps(NUMCOLORS) > -1 && dc.GetDeviceCaps(NUMCOLORS) <= 256)
	   pOld = memDC.SelectObject(&m_bmSplash16);
	else
	   pOld = memDC.SelectObject(&m_bmSplash);
		
	if (pOld == NULL)
		return;     // destructors will clean up

	dc.BitBlt(0, 0, m_wndWidth, m_wndHeight, &memDC, 0, 0, SRCCOPY);

	// print build
	CString cs;
#ifdef DEMO
	cs.LoadString(IDS_RP95_BUILDVER_DEMO);	
#else
	cs.LoadString(IDS_RP95_BUILDVER);
#endif

	// append rlc string
	CString csRLCVer;
	csRLCVer.LoadString(IDS_RLC_VER);
	if (atoi(csRLCVer) > 0) 
	   {
	   cs += " RLC";
	   cs += csRLCVer;
	   }

	pSplashFont = new CFont;       
    int TypeHeight = -MulDiv(8, dc.GetDeviceCaps(LOGPIXELSY), 72);
    // create a font for the window
    pSplashFont->CreateFont(TypeHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,        
#ifdef USEKANJI
	   DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 	   
       DEFAULT_QUALITY, DEFAULT_PITCH, "System");
#else
	   ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
	   DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
#endif
	   	
	// select font
	CFont* pOldFont = (CFont*)dc.SelectObject(pSplashFont);	
	dc.SetBkMode(TRANSPARENT);
	//dc.ExtTextOut(250, m_wndHeight - 56, 0, NULL, (const char*)cs, cs.GetLength(), NULL);	
	dc.ExtTextOut(175, m_wndHeight - 56, 0, NULL, (const char*)cs, cs.GetLength(), NULL);	

	// deselect font
	dc.SelectObject(pOldFont);

	delete pSplashFont;   	
	
	memDC.SelectObject(pOld);

	// Do not call CWnd::OnPaint() for painting messages
}

void CZeusApp::DeleteMutex()
{
	if (hRPUniqueMutex)
	  {
      // unset mutex
      ReleaseMutex(hRPUniqueMutex);
      CloseHandle(hRPUniqueMutex);
      hRPUniqueMutex = NULL;
	  }
}

BOOL CZeusApp::FirstInstance()
{
  CWnd *PrevCWnd, *ChildCWnd;
  
  // Determine if another window with our class name exists.
  if (PrevCWnd = CWnd::FindWindow(SERVERCLASS30, NULL))
  {                          
    PrevCWnd = CWnd::FindWindow(SERVERCLASS30, NULL);

	ChildCWnd=PrevCWnd->GetLastActivePopup(); // if so, does it have any popups?
	PrevCWnd->BringWindowToTop();             // Bring the main window to the top
	if (PrevCWnd->IsIconic()) 
	   PrevCWnd->ShowWindow(SW_RESTORE);      // If iconic, restore the main window
	if (PrevCWnd != ChildCWnd) 
	   ChildCWnd->BringWindowToTop();         // If there are popups, bring them along too!

	return FALSE;                             // Return FALSE.  This isn't the first instance
					      // and we are done activating the previous one.
  }
  
  CString csMutexName;
  csMutexName.Format("Global%s%s", "\\", SERVERMUTEX);

  hRPUniqueMutex = CreateMutex(NULL, TRUE, csMutexName);

  return TRUE;                              // First instance. Proceed as normal.
}
             
BOOL CAboutDlg::OnInitDialog()
{
	CString cs;	
	SysMem sm;  
	CProfile Profile;	

    CDialog::OnInitDialog();
	
	// get username and organization
	m_username = Profile.GetStr(IDS_USERINFO, IDS_USERNAME);
	m_organization = Profile.GetStr(IDS_USERINFO, IDS_ORGANIZATION);
	m_serialnumber = Profile.GetStr(IDS_USERINFO, IDS_SERIALNUMBER);
	((CZeusApp*)AfxGetApp())->GetHostIP(m_ip);

	// version
#ifdef DEMO
	m_version.LoadString(IDS_RP95_BUILDVER_DEMO);	
#else
	m_version.LoadString(IDS_RP95_BUILDVER);
#endif
	
	m_productname.LoadString(IDS_RASTERPLUS);

	// append rlc string
	CString csRLCVer;
	csRLCVer.LoadString(IDS_RLC_VER);
	if (atoi(csRLCVer) > 0) 
	   {
	   m_version += " RLC";
	   m_version += csRLCVer;
	   }
	
	// memory strings
	m_installed_mem = sm.csInstalledMemory;
	m_avail_virt = sm.csFreeVirtualMemory;
	m_avail_phys = sm.csFreePhysicalMemory;
	
	// tell the dialog about the data
	UpdateData(FALSE);
		
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CZeusApp::OnMyFileNew() 
   {
   int iRet;

   csRPDebugStr.Format("OnMyFileNew is %s\n", "was Clicked");
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr); 

   // create a new (empty) document
   if ((iRet = CreateNewDoc(FALSE)) == CREATENEWDOC_OK_REBOOT)
	  AfxMessageBox(IDS_NEEDTOREBOOT);   
   }

// win 95 procs
HANDLE (WINAPI *lpfn_CreateToolhelp32Snapshot)(DWORD, DWORD); 
BOOL (WINAPI *lpfn_Process32First)(HANDLE, LPPROCESSENTRY32); 
BOOL (WINAPI *lpfn_Process32Next)(HANDLE, LPPROCESSENTRY32); 
// win nt procs
BOOL (WINAPI *lpfn_EnumProcesses)(DWORD*, DWORD, DWORD*); 
BOOL (WINAPI *lpfn_EnumProcessModules)(HANDLE, HMODULE*, DWORD, LPDWORD); 
DWORD (WINAPI *lpfn_GetModuleBaseName)(HANDLE, HMODULE, LPTSTR, DWORD); 
							 
void CZeusApp::CleanUpRipprocs()
   {
   HANDLE hSnapshot;
   PROCESSENTRY32 pe32;      
   HANDLE hProcess; 
   char szProcessName[MAX_PATH] = "unknown";  
   BOOL bProcessClosed;
   HINSTANCE hLib;

   if (!IS_OS_NT_FAMILY)
	  {
	  if ((hLib = LoadLibrary("KERNEL32.DLL")) == NULL)
		 return;

	  // get helper procs from lib...we must do this 
	  // because these procs cannot be resolved using static
	  // linking in NT4
	  lpfn_CreateToolhelp32Snapshot = 		 
		 (HANDLE (WINAPI *)(DWORD, DWORD))::GetProcAddress(hLib, 
		 "CreateToolhelp32Snapshot");
	  lpfn_Process32First = 
		 (BOOL (WINAPI *)(HANDLE, LPPROCESSENTRY32))::GetProcAddress(hLib, 
		 "Process32First");
	  lpfn_Process32Next = 
		 (BOOL (WINAPI *)(HANDLE, LPPROCESSENTRY32))::GetProcAddress(hLib, 
		 "Process32Next");

	  if ((hSnapshot = lpfn_CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) != (HANDLE)-1)
		 {
		 pe32.dwSize = sizeof(PROCESSENTRY32); 
		 if (lpfn_Process32First(hSnapshot, &pe32))
			{
			do
			   {
               // Get the actual priority class. 
               hProcess = OpenProcess (PROCESS_ALL_ACCESS, 
                  FALSE, pe32.th32ProcessID);                                     

			   // if this is a ripproc, nuke it
			   // otherwise, just close the process
			   strupr(pe32.szExeFile);
               if (strstr(pe32.szExeFile, "RIPPROC.EXE"))
                  TerminateProcess(hProcess, 0);                                    					 
			   else
			      CloseHandle (hProcess); 
			   
			   } while (lpfn_Process32Next(hSnapshot, &pe32));
			}
		 CloseHandle(hSnapshot);
		 }					   		 
	  ::FreeLibrary(hLib);
	  }
   else
	  {				
	  if ((hLib = LoadLibrary("PSAPI.DLL")) == NULL)
		 return;

	  // get helper procs from lib...we must do this 
	  // because these procs cannot be resolved using static
	  // linking in 95
	  lpfn_EnumProcesses = 		 
		 (BOOL (WINAPI *)(DWORD*, DWORD, DWORD*))::GetProcAddress(hLib, 
		 "EnumProcesses");
	  lpfn_EnumProcessModules = 
		 (BOOL (WINAPI *)(HANDLE, HMODULE*, DWORD, LPDWORD))::GetProcAddress(hLib, 
		 "EnumProcessModules");
	  lpfn_GetModuleBaseName = 
		 (DWORD (WINAPI *)(HANDLE, HMODULE, LPTSTR, DWORD))::GetProcAddress(hLib, 
		 "GetModuleBaseNameA");

	  DWORD aProcesses[1024], cbNeeded, cProcesses;
      unsigned int i;

      if ( !lpfn_EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
         return;

      // Calculate how many process identifiers were returned.
	  cProcesses = cbNeeded / sizeof(DWORD);

      // Print the name and process identifier for each process.
      for ( i = 0; i < cProcesses; i++ )
		 {
         hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, aProcesses[i]);

         // Get the process name.
		 if (hProcess)
			{
            HMODULE hMod;
            DWORD cbNeeded;

			bProcessClosed = FALSE;

            if (lpfn_EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
               &cbNeeded))
			   {				  
               lpfn_GetModuleBaseName(hProcess, hMod, szProcessName, 
                  sizeof(szProcessName) );
			   // if this is a ripproc, nuke it
			   // otherwise, just close the process
			   strupr(szProcessName);
               if (strstr(szProcessName, "RIPPROC.EXE"))
				  {				  
                  TerminateProcess(hProcess, 0);					 
				  bProcessClosed = TRUE;
				  }
			   }
			if (!bProcessClosed) CloseHandle(hProcess); 
			}
		 }
      ::FreeLibrary(hLib);
	  }
   }

void CZeusApp::NukeBadFnameChars(CString& csPath)
   {
   csPath.Replace('\\', '_');
   csPath.Replace('/', '_');
   csPath.Replace('*', '_');
   csPath.Replace('?', '_');
   csPath.Replace(':', '_');
   csPath.Replace(';', '_');
   csPath.Replace('[', '_');
   csPath.Replace(']', '_');
   csPath.Replace('=', '_');
   csPath.Replace('"', '_');   
   csPath.Replace(',', '_');
   }

CString CZeusApp::GetWorkSpaceFileName()
   {
   BOOL bFoundFile;
   CProfile Profile;
   CString csWorkspace;

   CFileFind Finder;
   CString csSource;
   CString csDest;

   // get appdir
   csWorkspace = Profile.GetStr(IDS_SERVER, IDS_APPDIR);
   VERIFYPATH(csWorkspace);

	// Copy the RasterPlus wsp file, moving it to queue for convenience
	//
	bFoundFile = Finder.FindFile(csWorkspace + "*.wsp");

	while ( bFoundFile )
	{
		bFoundFile = Finder.FindNextFile();

		if ( Finder.IsDots() )
			continue;

		csSource = csWorkspace + Finder.GetFileName();
		csDest = csWorkspace + "Queue\\" + Finder.GetFileName();

		if ( CopyFile(csSource, csDest, FALSE ) == 0 )
		{
			DWORD dwErr = GetLastError();
		}

		DeleteFile(csSource);
	}

   csWorkspace += "Queue\\rasterplus_2.wsp";

   return csWorkspace;
   }

CString CZeusApp::GetAppPath()
   {
   CProfile Profile;
   CString csAppPath;

   csAppPath = Profile.GetStr(IDS_SERVER, IDS_APPDIR);
   VERIFYPATH(csAppPath);

   return csAppPath;
   }

CString CZeusApp::GetQueuePath()
   {
   CString csQueuePath;
   
   // try to start with the app data folder
   if (!GetAppDataFolder(csQueuePath))
      csQueuePath = GetAppPath();
   csQueuePath += "Queue\\";

   return csQueuePath;
   }

void CZeusApp::OpenQueue(CString& csQueueFileName)
   {
   CWinApp::OpenDocumentFile(csQueueFileName);
   }

BOOL CZeusApp::GetAppDataFolder(CString& AppDataFolder)
   {
   // can't get this working
   return FALSE;
#if 0
   char buf[_MAX_PATH];
   BOOL bRet = FALSE;

   // note that this is only supported in Win98/2000 or 95/NT4 with Explorer 4
   AppDataFolder = "";

   // get common folder
   if (SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, buf))
	  {
	  AppDataFolder = buf;
	  VERIFYPATH(AppDataFolder);
	  bRet = TRUE;
	  }
   return bRet;
#endif
   }

BOOL CZeusApp::IsPrinterDriverStillUsed(CString& csDriverName) 
   {
   LPPRINTER_INFO_2 pi;      
   DWORD dwSizeNeeded, dwNumPrinters;
   DWORD i;   
   BOOL bRet = FALSE;
   CString csDebug;
   
   // get space needed for printers
   ::EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 2, NULL, 
      0, &dwSizeNeeded, &dwNumPrinters);

   // allocate space for all printers
   // simple error checking per Win NT sample
   if (!(pi = (LPPRINTER_INFO_2)new char[dwSizeNeeded]))
      return 0;

   // get all printers
   if (!::EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 2, (LPBYTE)pi, 
      dwSizeNeeded, &dwSizeNeeded, &dwNumPrinters))
      {
      delete pi;
      return 0;
      }	  
   
   // get all printers
   for (i = 0; i < dwNumPrinters; i++)
      {	
      // get printer driver name
	  csDebug = (pi + i)->pDriverName;
	  if (csDriverName.Compare((pi + i)->pDriverName) == 0)
	     {
		 bRet = TRUE;
		 break;
		 }
	  }

   delete pi;

   // delete the printer buffer   
   return bRet;
   }

void CZeusApp::PopulateJobIDArray(int iJobID)
   {
   CJobID* pCJobID;

   // create a new jobid
   pCJobID = new CJobID;
   // put in the passed id
   pCJobID->iJobID = iJobID;
   // and add it to the array
   coJobIDArray.Add(pCJobID); 
   // make sure that the globaljob id is current
   iGlobalJobID = max(iGlobalJobID, iJobID + 1);
   }

int CZeusApp::GetJobID()
   {
   CJobID* pCJobID;
   int iJobID;
   BOOL bGotOne = FALSE;		

   // create a new jobid
   pCJobID = new CJobID;
   iJobID = pCJobID->iJobID = iGlobalJobID++;
   coJobIDArray.Add(pCJobID);      

   // assume that by the time we reach this number 
   // the jobs starting at 0 are long gone
   if (iGlobalJobID > 4000000000)
	  iGlobalJobID = 1;			
	  
   return iJobID;
   }

void CZeusApp::DeleteJobID(int iJobID)
   {
   int i;
   CJobID* pCJobID;

   // look for the job id and delete it from the array
   for (i = 0; i < coJobIDArray.GetSize(); i++)
	  {
	  pCJobID = (CJobID*)coJobIDArray.GetAt(i);
	  if (iJobID == pCJobID->iJobID)
		 {
		 delete pCJobID;
		 coJobIDArray.RemoveAt(i);
		 break;
		 }	   
	  }							  
   }

void CZeusApp::DeleteAllJobIDs()
   {
   int i;
   CJobID* pCJobID;

   // look for the job id and delete it from the array
   for (i = coJobIDArray.GetSize() - 1; i >= 0; i--)
	  {
	  pCJobID = (CJobID*)coJobIDArray.GetAt(i);
	  delete pCJobID;
	  coJobIDArray.RemoveAt(i);		 	
	  }							  
   }

// need this proc because some bad time/dates cause an MFC assert!
void CZeusApp::GetFileTime(WIN32_FIND_DATA* pFileInfo, CTime& ctFileTime)
   {
   SYSTEMTIME SystemTime;
   BOOL bRet;
   
   // convert
   bRet = FileTimeToSystemTime(&pFileInfo->ftLastWriteTime, &SystemTime);

   // bad time!
   if (!bRet || SystemTime.wYear < 1970)
	  ctFileTime = CTime::GetCurrentTime();
   else
	  ctFileTime = pFileInfo->ftLastWriteTime;
   }

void CZeusApp::ChangeSentinelKey()
   {
   // does farport key exist?
   HKEY hKey;		 
   if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
	  "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\FarPort", 
	  0, KEY_READ, &hKey) == ERROR_SUCCESS)
	  {
	  RegCloseKey(hKey);
	  // set sentinel key for farport
	  char* pDepend = "FARPORT";
	  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
		 "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\Sentinel", 
		 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		 {		  
		 RegSetValueEx(hKey, "DependOnService", NULL, REG_BINARY,
			(LPBYTE)pDepend, strlen(pDepend) + 1);
		 }
	  }
   }

CString CZeusApp::GetScratchFolder()
   {
   CString csTempPath;
   CString csScratchPath;   
   CProfile Profile;  
   char pTempFileName[_MAX_PATH];

   // get temp path
   GetTempPath(_MAX_PATH, csTempPath.GetBuffer(_MAX_PATH));
   csTempPath.ReleaseBuffer();

   // check validity by trying to make a temp file
   if (GetTempFileName(csTempPath, "tmp", 0, pTempFileName))
	  ::DeleteFile(pTempFileName);
   else
	  {
	  // otherwise, try windows path
      GetWindowsDirectory(csTempPath.GetBuffer(_MAX_PATH), _MAX_PATH);
	  csTempPath.ReleaseBuffer();
	  if (GetTempFileName(csTempPath, "tmp", 0, pTempFileName))
		 ::DeleteFile(pTempFileName);
	  else
		 {
		 // assume that current directory is OK
         GetCurrentDirectory(_MAX_PATH, csTempPath.GetBuffer(_MAX_PATH));
	     csTempPath.ReleaseBuffer();
		 }      
	  }	  

   // get scratch path
   csScratchPath = Profile.GetStr(SERVERSECTION, IDS_SCRATCHDISK, csTempPath);

   //check validity
   if (GetTempFileName(csScratchPath, "tmp", 0, pTempFileName))
	  {
	  ::DeleteFile(pTempFileName);
	  // make sure that it ends with a slash
	  VERIFYPATH(csScratchPath);
	  return csScratchPath;
	  }
   else
	  {
	  // make sure that it ends with a slash
	  VERIFYPATH(csTempPath);
	  return csTempPath;
	  }
   }

void CZeusApp::SetScratchFolder(CString& csScratchPath)
   {
   CProfile Profile;

   // write the default scratch dir
   Profile.WriteStr(SERVERSECTION, IDS_SCRATCHDISK, csScratchPath);	
   }

CString CZeusApp::GetSpoolFolder()
   {
   CString csTempPath;
   CString csScratchPath;   
   CProfile Profile;  
   char pTempFileName[_MAX_PATH];

   // get temp path
   GetTempPath(_MAX_PATH, csTempPath.GetBuffer(_MAX_PATH));
   csTempPath.ReleaseBuffer();

   // check validity by trying to make a temp file
   if (GetTempFileName(csTempPath, "tmp", 0, pTempFileName))
	  ::DeleteFile(pTempFileName);
   else
	  {
	  // otherwise, try windows path
      GetWindowsDirectory(csTempPath.GetBuffer(_MAX_PATH), _MAX_PATH);
	  csTempPath.ReleaseBuffer();
	  if (GetTempFileName(csTempPath, "tmp", 0, pTempFileName))
		 ::DeleteFile(pTempFileName);
	  else
		 {
		 // assume that current directory is OK
         GetCurrentDirectory(_MAX_PATH, csTempPath.GetBuffer(_MAX_PATH));
	     csTempPath.ReleaseBuffer();
		 }      
	  }	  

   // get scratch path
   csScratchPath = Profile.GetStr(SERVERSECTION, SERVERSPOOLFOLDER, csTempPath);

   //check validity
   if (GetTempFileName(csScratchPath, "tmp", 0, pTempFileName))
	  {
	  ::DeleteFile(pTempFileName);
	  // make sure that it ends with a slash
	  VERIFYPATH(csScratchPath);
	  return csScratchPath;
	  }
   else
	  {
	  // make sure that it ends with a slash
	  VERIFYPATH(csTempPath);
	  return csTempPath;
	  }
   }

void CZeusApp::SetSpoolFolder(CString& csSpoolPath)
   {
   CProfile Profile;

   // write the default scratch dir
   Profile.WriteStr(SERVERSECTION, SERVERSPOOLFOLDER, csSpoolPath);	
   }

void CZeusApp::RecordPPDFile(LPCSTR csDevName, LPCSTR csPPDName, int idx)
   {
   CProfile Profile;
   CString csTemp;

   if ( idx < 0 )
   {
	   if ( idx == -1 )
		csTemp = "PPDDelFileName";
	   else
		csTemp = "PPDDelFileName_NT";
   }
   else
	   csTemp.Format("PPDDelFile%d", idx);
   Profile.WriteStr(csDevName, csTemp, csPPDName);
   }

void CZeusApp::DeleteAllPPDPCFFiles(LPCSTR csName, LPCSTR csFriendlyName, int iDevIdx)
{
	CProfile Profile;
	CString csTemp;
	CString csDestPPDName;
	CString csDestPPDShortName;
	CString csDestPPDDir;
	CString csPCFFileName;
	CString csDevName;
	CString csPrinterDir;
	CString csSystemDir;
	CString csPPDName;
	CString csPPDName_NT;
	CString	csPCFName;

	DWORD dwNeeded;

	int i;

	// put in a CString
	csDevName = csName;
	csDevName += "\\";
	csDevName += csFriendlyName;

	// try to delete the ppd files and the .pcf files
	if (csDevName.GetLength() > 1)
	{
		csPCFName = csName;

		csPCFName.Replace('\\', '_' );
		csPCFName.Replace('/', '_' );
		csPCFName.Replace(':', '_' );
		csPCFName.Replace('*', '_' );
		csPCFName.Replace('?', '_' );
		csPCFName.Replace('<', '_' );
		csPCFName.Replace('>', '_' );
		csPCFName.Replace('|', '_' );

		csPCFFileName = ((CZeusApp*)AfxGetApp())->GetQueuePath();
		csPCFFileName += csPCFName;
		csPCFFileName += ".pcf";
		::DeleteFile(csPCFFileName);
	}

	for (i = 0; i < MAX_PPDDELFILES; i++)
	{
		csTemp.Format("PPDDelFile%d", i);
		csDestPPDName = Profile.GetStr(csDevName, csTemp);
		if (csDestPPDName.GetLength() > 0)
		{
			::DeleteFile(csDestPPDName);
			csDestPPDDir = csDestPPDName.Left(csDestPPDName.ReverseFind('\\'));
			::RemoveDirectory(csDestPPDDir);
		}
		Profile.RemoveRegValue(csDevName, csTemp);
	}

	// Remove the Directory created for this Printer
	//
	// get printer install directory
	::GetPrinterDriverDirectory(NULL, NULL, 1,
				(LPBYTE)csPrinterDir.GetBuffer(_MAX_PATH), _MAX_PATH, &dwNeeded);   
	csPrinterDir.ReleaseBuffer();
	VERIFYPATH(csPrinterDir);

	// get system directory
	::GetSystemDirectory(csSystemDir.GetBuffer(_MAX_PATH), _MAX_PATH);
	csSystemDir.ReleaseBuffer();
	VERIFYPATH(csSystemDir);

	csPPDName = Profile.GetStr(csDevName, "PPDDelFileName");
	csPPDName_NT = Profile.GetStr(csDevName, "PPDDelFileName_NT");

	if ( csPPDName.GetLength() > 0 )
	{
		csTemp = csPrinterDir + csPPDName;
		::DeleteFile(csTemp);
		csTemp = csPrinterDir + "3\\" + csPPDName;
		::DeleteFile(csTemp);

		csTemp = csSystemDir + csPPDName;
		::DeleteFile(csTemp);

		// Remove the BPD file too
		//
		csTemp = csPrinterDir + "3\\" + csPPDName.Left(csPPDName.ReverseFind('.')) + ".BPD";
		::DeleteFile(csTemp);
	}
	Profile.RemoveRegValue(csDevName, "PPDDelFileName");

	if ( csPPDName_NT.GetLength() > 0 )
	{
		csTemp = csPrinterDir + csPPDName_NT;
		::DeleteFile(csTemp);
		csTemp = csPrinterDir + "3\\" + csPPDName_NT;
		::DeleteFile(csTemp);

		csTemp = csSystemDir + csPPDName_NT;
		::DeleteFile(csTemp);

	}
	Profile.RemoveRegValue(csDevName, "PPDDelFileName_NT");

	// This should cleanup the Registry
	//
	Profile.RemoveSubKey(csName, csFriendlyName );
}

BOOL CZeusApp::GetHostIP(CString& csIP)
   {
   BOOL bRet = FALSE;
   char szHostName[128];
   
   csIP = "";
   
   if (gethostname(szHostName, 128) == 0 )
	  {				 
	  // Get host adresses
	  struct hostent * pHost;
	  int i;
	  UINT ipb;
	  pHost = gethostbyname(szHostName); 
	  for (i = 0; pHost!= NULL && pHost->h_addr_list[i]!= NULL; i++ )
		 {
		 int j;
		 csIP = "";
		 for( j = 0; j < pHost->h_length; j++ )
			{
			CString addr;
			if( j > 0 )	csIP += ".";
			ipb = (unsigned int)((unsigned char*)pHost->h_addr_list[i])[j];
			addr.Format("%u", ipb );
			csIP += addr;
			}
		 }	  
	  bRet = TRUE;
	  }
   return bRet;
   }

void CZeusApp::RemoveSplashScreen()
{
	if (m_pwndSplash != NULL)
	{
		if (m_pwndSplash->m_hWnd != NULL)
		{
			// timeout expired, destroy the splash window
			m_pwndSplash->DestroyWindow();
			delete m_pwndSplash;
			m_pwndSplash = NULL;
			m_pMainWnd->UpdateWindow();
			// NOTE: don't set bResult to FALSE,
			//  CWinApp::OnIdle may have returned TRUE
		}
	}

	return;
}

void CZeusApp::SetFileVersion(WORD wVersion)
{
	wFileVersion = wVersion;
	return;
}

WORD CZeusApp::GetFileVersion()
{
	return wFileVersion;
}


int CZeusApp::GetProcessorType()
{
   CString strValue;
   DWORD dwCount = 10;
   DWORD dwType = REG_SZ;

   int iProcessorType = PROCESSOR_UNKNOWN;

   if ( IS_OS_VISTA )
   {
	   csRPDebugStr.Format("System OS is %s\n", "VISTA");
	   strcpy(cDebugStr, csRPDebugStr);
	   RPDEBUGSTRMFC(cDebugStr);
   }

   if ( IS_OS_WINDOWS7 )
   {
	   csRPDebugStr.Format("System OS is %s\n", "Windows7");
	   strcpy(cDebugStr, csRPDebugStr);
	   RPDEBUGSTRMFC(cDebugStr);
   }

   if ( IS_OS_WINDOWS8 )
   {
	   csRPDebugStr.Format("System OS is %s\n", "Windows8");
	   strcpy(cDebugStr, csRPDebugStr);
	   RPDEBUGSTRMFC(cDebugStr);
   }

   if ( IS_OS_VISTA || IS_OS_WINDOWS7 || IS_OS_WINDOWS8)
   {
	   SYSTEM_INFO sysInfo;
	   PGNSI pGNSI;

	   ZeroMemory(&sysInfo, sizeof(SYSTEM_INFO));

	   // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.

	   pGNSI = (PGNSI) GetProcAddress(
					GetModuleHandle(TEXT("kernel32.dll")), 
						"GetNativeSystemInfo");

	   if(NULL != pGNSI)
		   pGNSI(&sysInfo);
	   else
		   GetSystemInfo(&sysInfo);

	   csRPDebugStr.Format("PROCESSOR_ARCHITECTURE is %d\n", sysInfo.wProcessorArchitecture);
	   strcpy(cDebugStr, csRPDebugStr);
	   RPDEBUGSTRMFC(cDebugStr); 

	   switch ( sysInfo.wProcessorArchitecture )
	   {
	   case PROCESSOR_ARCHITECTURE_AMD64:
	   case PROCESSOR_ARCHITECTURE_IA64:
		   iProcessorType = PROCESSOR_64bit;
		   break;
	   case PROCESSOR_ARCHITECTURE_INTEL:
		   iProcessorType = PROCESSOR_32bit;
		   break;
	   default:
		   iProcessorType = PROCESSOR_UNKNOWN;
		   break;
	   }

	   if ( sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64  ||
				sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
	   {
			iProcessorType = PROCESSOR_64bit;
	   }
	   else
	   {
		   if ( sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL )
		   {
			    iProcessorType = PROCESSOR_32bit;
		   }
	   }
   }
   else
   {
	   csRPDebugStr.Format("PROCESSOR_ARCHITECTURE is %s\n", "Not Valid Vista or Windows7");
	   strcpy(cDebugStr, csRPDebugStr);
	   RPDEBUGSTRMFC(cDebugStr); 
   }

   return iProcessorType;
}


// Backup Registry Function
BOOL SetPrivilege(
    HANDLE hToken,          // access token handle
    LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
    BOOL bEnablePrivilege   // to enable or disable privilege
    ) 
{
    TOKEN_PRIVILEGES tp;
    LUID luid;

    if ( !LookupPrivilegeValue( 
            NULL,            // lookup privilege on local system
            lpszPrivilege,   // privilege to lookup 
            &luid ) )        // receives LUID of privilege
    {
        printf("LookupPrivilegeValue error: %u\n", GetLastError() ); 
        return FALSE; 
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    // Enable the privilege or disable all privileges.

    if ( !AdjustTokenPrivileges(
           hToken, 
           FALSE, 
           &tp, 
           sizeof(TOKEN_PRIVILEGES), 
           (PTOKEN_PRIVILEGES) NULL, 
           (PDWORD) NULL) )
    { 
          printf("AdjustTokenPrivileges error: %u\n", GetLastError() ); 
          return FALSE; 
    } 

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

    {
          printf("The token does not have the specified privilege. \n");
          return FALSE;
    } 

    return TRUE;
}


void CZeusApp::BackupRegistry()
{
	CString csRootBackupFolder;
	CString csBackupFolder;
	CStringArray csaExcludeFolders;

	// backup the registry
	CString csRegBackup;

	GetTodaysBackupFolder(csRootBackupFolder, csBackupFolder);

	if (GetBackupFolderExcludeList(7, csaExcludeFolders) > 0)
	{		 
		GetTodaysBackupFolder(csRootBackupFolder, csBackupFolder);

		RecursiveDeleteAll(csRootBackupFolder, &csaExcludeFolders, "backup");
	}	  

	csRegBackup = csBackupFolder;
	csRegBackup += "RasterV6_REG.txt"; 			
	HKEY hKey=NULL;
	LONG lRet;					  
	HANDLE ProcessToken;
			
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &ProcessToken)) 
	{			  				
		SetPrivilege(ProcessToken, SE_BACKUP_NAME, TRUE);

		if ((lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Graphx\\rasterplus", 0, KEY_READ, &hKey)) == ERROR_SUCCESS)
		{
			// need this as this method cannot overwrite a file
			::DeleteFile(csRegBackup);
			lRet = RegSaveKey(hKey, csRegBackup, NULL); 
			RegCloseKey(hKey);  
		}	
	}
	

}

void CZeusApp::GetTodaysBackupFolder(CString& csRootBackupFolder, CString& csBackupFolder)
   {
   CTime ctCurrent;
   CString csTime;

   ctCurrent = CTime::GetCurrentTime();
   csTime = ctCurrent.Format("%m-%d-%y");

   // create backup folder if it doesn't exist
   csBackupFolder = GetAppPath();
   csBackupFolder += "backup\\";
   ::CreateDirectory(csBackupFolder, NULL);		          
   csRootBackupFolder = csBackupFolder;
   csBackupFolder += csTime;   
   ::CreateDirectory(csBackupFolder, NULL);		          
   csBackupFolder += "\\";

   csRPDebugStr.Format("Backup Folder for TODAY is %s\n", csBackupFolder);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr); 
   }

int CZeusApp::GetBackupFolderExcludeList(int iDays, CStringArray& csaExcludeFolders)
   {
   CString csBackupFolder;
   CTime ctCurrent;
   CString csTime;
   CTimeSpan ctsOneDay(1, 0, 0, 0);
   int i;

   csaExcludeFolders.RemoveAll();

   // start by excluding today
   ctCurrent = CTime::GetCurrentTime();	  

   for (i = 0; i < iDays; i++)
	  {	  
	  // create folder name and add to the array
	  csTime = ctCurrent.Format("%m-%d-%y");
	  
	  csBackupFolder = AfxGetApp()->GetProfileString("General", "BackupFolder", "");

	  if ( csBackupFolder.IsEmpty() )
	  {
		  csBackupFolder = GetAppPath();
		  csBackupFolder += "backup";
		  csBackupFolder += "\\";
		  AfxGetApp()->WriteProfileString("General", "BackupFolder", csBackupFolder );
	  }
	  else
	  {
	      int iFldLen = csBackupFolder.GetLength();
		  if ( csBackupFolder.GetAt(iFldLen - 1) != _T('\\') )
				 csBackupFolder += "\\";
	  }

      csBackupFolder += csTime;   
      csBackupFolder += "\\";
	  csaExcludeFolders.Add(csBackupFolder);

   csRPDebugStr.Format("Exclude This folder %s\n", csBackupFolder);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr); 

	  ctCurrent -= ctsOneDay;
	  }
   return csaExcludeFolders.GetSize();
   }



void CZeusApp::RecursiveDeleteAll(LPCSTR lpcstrPath, CStringArray* psaExclude, char* pStrCheck)
   {
   CFileFind ff;   
   CString csTest1;
   CString csTest2;

   // sanity
   if (strlen(lpcstrPath) < 2) return;
   
   CString path = lpcstrPath;
   
   if (path.Right(1) != "\\") path += "\\";
   path += "*.*";
   BOOL res = ff.FindFile(path);
   
   while(res)
	  {
	  res = ff.FindNextFile();
	  if (ff.IsDots())
		 continue;
	  if (ff.IsDirectory())
		 {
		 // check exclude
		 if (psaExclude)
			{
			int i;
			BOOL bExcludeThis = FALSE;

			for (i = 0; i < psaExclude->GetSize(); i++)
			   {
			   csTest1 = psaExclude->GetAt(i);
			   VERIFYPATH(csTest1);			
			   csTest2 = ff.GetFilePath();
			   VERIFYPATH(csTest2);			
			   if (csTest1.CompareNoCase(csTest2) == 0)
				  {
				  bExcludeThis = TRUE;
				  break;
				  }
			   }
			if (bExcludeThis)
				continue;
			}

		 path = ff.GetFilePath();
		 // sanity check?
		 if (pStrCheck)
			{
			csTest1 = pStrCheck;
			csTest1.MakeUpper();
			csTest2 = path;
			csTest2.MakeUpper();
			// if the path includes our sanity check string, delete it
			if (csTest2.Find(csTest1) != -1)
			   {
   csRPDebugStr.Format("Remove this path: %s\n", path);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr); 
			   RecursiveDeleteAll(path, psaExclude);
			   RemoveDirectory(path);
			   }
			}
		 else
			{
   csRPDebugStr.Format("(2)Remove this path: %s\n", path);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr); 
			RecursiveDeleteAll(path, psaExclude);
			RemoveDirectory(path);
			}
		 }
	  else
		 {
		 // make sure that we don't have a read only file
		 csTest1 = ff.GetFilePath();
   csRPDebugStr.Format("Delete this file: %s\n", csTest1);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr); 
		 ::SetFileAttributes(csTest1, FILE_ATTRIBUTE_ARCHIVE);
		 // if the file is read-only, reset the atts and try to delete again
		 ::DeleteFile(csTest1);
/*
		 ::SetFileAttributes(ff.GetFilePath(), FILE_ATTRIBUTE_ARCHIVE);
		 // if the file is read-only, reset the atts and try to delete again
		 ::DeleteFile(ff.GetFilePath());
*/
		 }
	  }
   }


#ifdef LEADFILE_PS
CString CZeusApp::Convert_PS_to_JPG(CString& csPSFileName, int iPageNumber)
{
	BITMAPHANDLE Bitmap;

	BOOL fIsBGRDevice = TRUE;

	char szTempFileName[_MAX_PATH];
	char szFileName[_MAX_PATH];

	CString csTempPath;
	CString csError;
	CString csJPGFileName;

	FILEINFO FileInfo;
	memset(&FileInfo, 0, sizeof(FILEINFO));
	FileInfo.uStructSize = sizeof(FILEINFO);

	int iRet = 0;
	TCHAR tcLTPDFDir[(_MAX_PATH + 1) * sizeof(TCHAR)];
	int iLTRet = 0;

	iLTRet = L_GetPDFInitDir(tcLTPDFDir, ((_MAX_PATH * 1) * 3));

	LOADFILEOPTION   LoadFileOption;

	csJPGFileName.Empty();
	strcpy(szFileName, csPSFileName);

	memset(&FileInfo, 0, sizeof(FILEINFO));
	FileInfo.uStructSize = sizeof(FILEINFO);

	L_GetDefaultLoadFileOption(&LoadFileOption, sizeof(LOADFILEOPTION)); 

	memset(&LoadFileOption, 0, sizeof(LOADFILEOPTION));
	LoadFileOption.uStructSize = sizeof(LOADFILEOPTION);
	LoadFileOption.XResolution = 150;
	LoadFileOption.YResolution = 150;
	LoadFileOption.PageNumber = iPageNumber + 1;

	iRet = L_FileInfo(szFileName, &FileInfo, sizeof(FILEINFO), FILEINFO_TOTALPAGES, &LoadFileOption);
    if(iRet != SUCCESS)
		csError.Format("Error loading file info: %d", iRet);
	else
		csError.Format("Total number of pages: %d", FileInfo.TotalPages);
	
    /* Load the bitmap, keeping the bits per pixel of the file */
    iRet = L_LoadBitmap (szFileName, &Bitmap, sizeof(BITMAPHANDLE), 0, ORDER_BGR, &LoadFileOption, &FileInfo);

/*
	iRet = L_LoadFile(szFileName, &Bitmap, sizeof(BITMAPHANDLE), 0, fIsBGRDevice ? ORDER_BGRORGRAY : ORDER_RGBORGRAY, 
						LOADFILE_ALLOCATE | LOADFILE_STORE, NULL, NULL, &LoadFileOption, &FileInfo);
*/ 
	if(iRet == SUCCESS)
	{
		csTempPath = GetSpoolFolder();
		GetTempFileName(csTempPath, "img", 0, szTempFileName);   

		csError.Format("L_LoadFile Filename = %s", szTempFileName);

		// compress the image   
//			iRet = ::L_SaveFile (szTempFileName, &Bitmap, FILE_JFIF, 24, 
		iRet = ::L_SaveFile (szTempFileName, &Bitmap, FILE_LEAD1JFIF_LAB, 24, 
					NULL, NULL, NULL, NULL, NULL);

		if ( iRet != SUCCESS )
		{
			csError.Format("L_SaveFile Return = %d", iRet);
		}
		else
		{
			csJPGFileName = szTempFileName;
		}
	}
	else
	{
		csError.Format("L_LoadFile Return = %d", iRet);
	}

	return csJPGFileName;
}
#endif