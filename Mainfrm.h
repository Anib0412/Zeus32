/////////////////////////////////////////////////////////////////////////////
// MAINFRM.H
//                                                                        
// main frame class for RasterPlus
// Copyright (C) 1996-2001 Graphx, Inc.
//

#ifndef _MAINFRM_H_
#define _MAINFRM_H_

#include "device.h"
#include "transfrm.h"
#include "NewPreview.h"
#include "inidefs.h"
#include "prev.h"
#include "PropertyFrame.h"
#include "servdefs.h"

//#include "GisRPCServer.h"
#include "GISServer.h"

#include "rippane.h"
#include "QueueCount.h"
#include "WebServicesASP.h"
#include "MyInternetSession.h"

#ifdef _VERSION_6
#include "..\pcgi\pcgi.h"
#include "..\device\loaddevid.h"
#include "TimeCheck.h"
#endif

#include <afxmt.h>

// max number of rip windows - should be enough!
#define MAX_IPCSLOTS   256

#ifdef _VERSION_6
typedef BOOL (WINAPIV *GetRPW01Interface)(PPCG_INTERFACE, LPCSTR);
typedef int (WINAPIV *GetRPW02Interface)(int);
#endif


class CClipArray : public CObArray
   {
public:   
   void Destroy();
   void SetType(int t);
   BOOL IsTypeAvail(int t);
protected:
   int Type;
   };


class CMainFrame : public SECWorkbook
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	BOOL PreCreateWindow(CREATESTRUCT& cs);			
	void AddDevNamesToMenu();
	
	int GetAppVersion();
	BOOL CheckForUpdateASP();
	BOOL GetHTTPFile(CString& csFilePath, CString& csTempFileName, int iSize, CWnd* pWnd);

// Attributes
public: 

// Operations
public:   
   // array of mdi preview windows          
   CPreviewManager Preview;   
 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	protected:
	//}}AFX_VIRTUAL

public:
	char rpString[255];
	CString csDebugStr;

   HANDLE hPipe;
   HANDLE hThread;

   CPropertyFrame* m_pPropFrame;
   int CfDlgInstanceCntr;   
   CClipArray ClipArray;   
   CPaneArray pPaneArray;
   int GetAvailIPCSlot();   
   void FreeIPCSlot(int Slot);  
   void NotifyViewClose();   
   CStringArray csaQueueFileNames;
   CStringArray csat;
   void FindQueueFiles();
   void AddToolsToMenu();
   int NumSaveQueueFileInstances(CQueueFile* pCQueueFile);
   BOOL DeleteLastServerFile(CQueueFile* pCQueueFile);
   
   BOOL IsPackagePlusInstalled();
   CString csPackagePlusExe;
   int iPackagePlusToolID;

   BOOL IsColorProfilerInstalled();
   CString csColorProfilerExe;
   int iColorProfilerToolID;

   int iScannerToolID;
   int iSelectSCSIToolID;
//   int IsScannerAvailable();
//   void AcquireScanImage();

   void StartAsyncPipe();

#ifdef LAB_STYLE
	HMENU NewMenu();    
	CMenu m_menu;
#endif

   void PDFHelp();
   void SaveState();   
   CQueueCount QueueCount;

#ifdef _VERSION_6
#ifndef _DEBUG
	BOOL	bRPWCheckGuard;
	BOOL	bInterfaceActive;
	BOOL	bRPWInterface;

	HMODULE hPCGI;
	HMODULE hRPWI;

	PCG_INTERFACE  PCGInterface;
	RPW_INTERFACE  RPWInterface;

	GetPCGInterface lpGetInterface;

	CTimeCheck tcTimeCheck;

	DWORD CheckGuard();
#endif
#endif

protected:   
   BOOL WinASPIExist;
   HANDLE hIsCompleteMutex; 
   
// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif 		
	
    // the status bar
    SECStatusBar  m_wndStatusBar;                          
	
//	CGISRPCServer* pGisRPC_server;
    CPGPRCServer* pgis_server;

	CWinThread* pPGPRCThread;   
	CEvent m_KillPhotogizeServer;

	CWinThread* pPFSThread;   

	
protected:  
	void LaunchURLInNewWindow(HWND hWndParent, LPCSTR lpcstrWebUpload);
	void StartPhotogizeServer();
	void StopPhotogizeServer();
	int TimerID;

    // the toolbar
	SECToolBar m_wndToolBar;    			

	BOOL IPCSlotTaken[MAX_IPCSLOTS];	
	BOOL GetServerStrVal(LPTSTR Entry, LPTSTR Value, LPTSTR Default, DWORD MaxSize);	    
	CPaneElement* FindServerDoc(CQueueFile* pCQueueFile, LPCSTR lpPrinterName);
// Attributes
public:


private:
// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)             
    afx_msg LRESULT OnPrevSetInfo(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);			
	afx_msg LRESULT OnAddJobToQueue(WPARAM, LPARAM);	
	afx_msg LRESULT OnServerFileEOF(WPARAM, LPARAM);
	afx_msg void OnPreferencesGlobal();
	afx_msg BOOL OnQueryEndSession();	
	afx_msg void OnClose();
	afx_msg void OnToolsExplorer();	
	afx_msg void OnCloseAllPreview();
	afx_msg void OnUpdateOnCloseAllPreview(CCmdUI* pCmdUI);
	afx_msg void OnWeb();
	afx_msg void OnWebUpdate();
	afx_msg void OnHelpDesk();
	afx_msg void OnRemoteDiagnostic();
	afx_msg void OnToolsScsiExplorer();
	afx_msg void OnUpdateToolsScsiExplorer(CCmdUI* pCmdUI);
	afx_msg void OnReg();		
	afx_msg void OnFileNew2();	
	afx_msg void OnDestroy();	
	afx_msg void OnSize(UINT nType, int cx, int cy);	
	afx_msg void OnToolsSavestate();	
//	afx_msg void OnToolsDongleReader();
//	afx_msg void OnUpdateToolsDongleReader(CCmdUI* pCmdUI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdateFileNew2(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnQueueNames(UINT nID);	
	afx_msg void OnUpdateQueueNames(CCmdUI* pCmdUI);
	afx_msg void OnTools(UINT nID);
	afx_msg void OnUpdateTools(CCmdUI* pCmdUI);
	afx_msg void OnNewInThisVersion();
	afx_msg void OnUpdateHelpRemoteDiagnostics(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

// our clipboard styles
#define OURCLIP_NONE             0
#define OURCLIP_FILE             1
#define OURCLIP_ATTS             2

/////////////////////////////////////////////////////////////////////////////
#endif
