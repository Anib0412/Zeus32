/////////////////////////////////////////////////////////////////////////////
// ZEUS.H
//                                                                        
// header for main module for RasterPlus
// Copyright (C) 1996-99 Graphx, Inc.
//

#ifndef _ZEUS_H_
#define _ZEUS_H_

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "LicenseManager.h" // 
#include "ltwnd.h"

// very handy
#define VERIFYPATH(a) if (a.GetLength() > 0) { if (a[a.GetLength() - 1] != '\\' &&       \
a[a.GetLength() - 1] != ':') a += "\\"; }

// send a debug string to our debug window
#define RPDEBUGSTRMFC(a)                                  \
   {													  \
   HWND hWnd;											  \
   COPYDATASTRUCT cpds;						              \
   cpds.dwData = 0;										  \
   cpds.lpData = (PVOID)(char*)(const char*)a;	 		  \
   cpds.cbData = strlen(a) + 1;							  \
   if (hWnd = ::FindWindow("RPDebug_Class", NULL))		  \
   ::SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cpds);	  \
   }
#define RPDEBUGSTR(a)                                     \
   {													  \
   HWND hWnd;											  \
   COPYDATASTRUCT cpds;						              \
   cpds.dwData = 0;										  \
   cpds.lpData = (PVOID)a;								  \
   cpds.cbData = strlen(a) + 1;							  \
   if (hWnd = FindWindow("RPDebug_Class", NULL))		  \
      SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cpds);	  \
   }


#define PROCESSOR_ARCHITECTURE_INTEL            0
#define PROCESSOR_ARCHITECTURE_MIPS             1
#define PROCESSOR_ARCHITECTURE_ALPHA            2
#define PROCESSOR_ARCHITECTURE_PPC              3
#define PROCESSOR_ARCHITECTURE_SHX              4
#define PROCESSOR_ARCHITECTURE_ARM              5
#define PROCESSOR_ARCHITECTURE_IA64             6
#define PROCESSOR_ARCHITECTURE_ALPHA64          7
#define PROCESSOR_ARCHITECTURE_MSIL             8
#define PROCESSOR_ARCHITECTURE_AMD64            9
#define PROCESSOR_ARCHITECTURE_IA32_ON_WIN64    10

/////////////////////////////////////////////////////////////////////////////
// CSplashWnd window

class CSplashWnd : public CWnd
{
// Construction
public:
	CSplashWnd();

// Attributes
public:
	virtual BOOL Create();	
	
// Operations
public:

// Implementation
public:
	virtual ~CSplashWnd();

protected:
	CBitmap m_bmSplash;
	CBitmap m_bmSplash16;
	CFont* pSplashFont;
	int m_wndWidth;
	int m_wndHeight;
		
	// Generated message map functions
	//{{AFX_MSG(CSplashWnd)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CZeusApp:
// See zeus.cpp for the implementation of this class
//

class CJobID : public CObject
   {
public:
   CJobID(){}
   ~CJobID(){}

   int iJobID;   
   };

class CZeusApp : public CWinApp
{
public:     
	CZeusApp();	
	int CreateNewDoc(BOOL bFromInstaller = FALSE, BOOL bAskOpenExisting = TRUE, BOOL bAsk = TRUE, LPCSTR csName = NULL, 
	   LPCSTR csDevice = NULL, int iDevIdx = 0, LPCSTR csPort = NULL, int iInstallPrinter = FALSE);	
	CRITICAL_SECTION BitmapEngineMutex;

	int OSVersion;
	int OSMinorVersion;
	int OSMajorVersion;
	BYTE  wProductType;

	int iProcessorType;
	int HasDongle;
	int DongleKeyCode;	
    void NukeBadFnameChars(CString& csPath);
    CString GetWorkSpaceFileName();
    CString GetAppPath();
    CString GetQueuePath();
	BOOL GetAppDataFolder(CString& AppDataFolder);
	void OpenQueue(CString& csQueueFileName);
	BOOL IsPrinterDriverStillUsed(CString& csDriverName);	
	void ChangeSentinelKey();
	BOOL OpenQueueExist(LPCSTR csName);
	CString GetScratchFolder();
	void SetScratchFolder(CString& csScratchPath);
	CString GetSpoolFolder();
	void SetSpoolFolder(CString& csSpoolPath);
	void DeleteMutex();
	void BackupRegistry();
	void GetTodaysBackupFolder(CString& csRootBackupFolder, CString& csBackupFolder);
	int GetBackupFolderExcludeList(int iDays, CStringArray& csaExcludeFolders);
	void RecursiveDeleteAll(LPCSTR lpcstrPath, CStringArray* psaExclude, char* pStrCheck = NULL);

	BOOL bInstallPrinterAndExit;
	BOOL bClassRegistered;

	BOOL CheckActivation();

protected:
	CSplashWnd *m_pwndSplash;
	virtual BOOL OnIdle(LONG lCount);
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	DWORD m_dwSplashTime;		
	void CleanUpRipprocs();

    CString csRPDebugStr;
    char cDebugStr[255];

	CString csDebug;
	WORD wFileVersion;
	HANDLE hRPUniqueMutex;


public:
// Overrides
	virtual BOOL InitInstance();	
	virtual int ExitInstance();	
	virtual BOOL InitApplication();
	BOOL FirstInstance();	

	int iGlobalJobID;
    int GetJobID();
    int GetQueueDevice(LPCSTR csPrinterName);
	void PopulateJobIDArray(int iJobID);
	void DeleteJobID(int iJobID);
	void DeleteAllJobIDs();
	CObArray coJobIDArray;
	void GetFileTime(WIN32_FIND_DATA* pFileInfo, CTime& ctFileTime);
	void RecordPPDFile(LPCSTR csDevName, LPCSTR csPPDName, int idx);
	void DeleteAllPPDPCFFiles(LPCSTR csName, LPCSTR csFriendlyName, int iDevIdx);
	BOOL GetHostIP(CString& csIP);

	void SetFileVersion(WORD);
	WORD GetFileVersion();
	int GetProcessorType();
	void RemoveSplashScreen();
#ifdef LEADFILE_PS
	CString Convert_PS_to_JPG(CString& csPSFileName, int iPageNumber = 0);
#endif

// Implementation

	//{{AFX_MSG(CZeusApp)
	afx_msg void OnAppAbout();		
	afx_msg void OnMyFileNew();		
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define BITMAPENGINEMUTEX (((CZeusApp*)AfxGetApp())->BitmapEngineMutex)
#define IS_OS_ME          ((((CZeusApp*)AfxGetApp())->OSVersion == VER_PLATFORM_WIN32_WINDOWS) \
														&& (((CZeusApp*)AfxGetApp())->OSMajorVersion == 4) \
														&& (((CZeusApp*)AfxGetApp())->OSMinorVersion == 90))
#define IS_OS_98          ((((CZeusApp*)AfxGetApp())->OSVersion == VER_PLATFORM_WIN32_WINDOWS) \
														&& (((CZeusApp*)AfxGetApp())->OSMajorVersion == 4) \
														&& (((CZeusApp*)AfxGetApp())->OSMinorVersion == 10))
#define IS_OS_95          ((((CZeusApp*)AfxGetApp())->OSVersion == VER_PLATFORM_WIN32_WINDOWS) \
														&& (((CZeusApp*)AfxGetApp())->OSMajorVersion == 4) \
														&& (((CZeusApp*)AfxGetApp())->OSMinorVersion == 0))

#define IS_OS_NT_FAMILY   (((CZeusApp*)AfxGetApp())->OSVersion == VER_PLATFORM_WIN32_NT)
#define IS_OS_NT          (((CZeusApp*)AfxGetApp())->OSVersion == VER_PLATFORM_WIN32_NT) \
														&& (((CZeusApp*)AfxGetApp())->OSMajorVersion <= 4)
#define IS_OS_2000        ((((CZeusApp*)AfxGetApp())->OSVersion == VER_PLATFORM_WIN32_NT) \
														&& (((CZeusApp*)AfxGetApp())->OSMajorVersion == 5) \
														&& (((CZeusApp*)AfxGetApp())->OSMinorVersion == 0))
#define IS_OS_XP          ((((CZeusApp*)AfxGetApp())->OSVersion == VER_PLATFORM_WIN32_NT) \
														&& (((CZeusApp*)AfxGetApp())->OSMajorVersion == 5) \
														&& (((CZeusApp*)AfxGetApp())->OSMinorVersion == 1))
#define IS_OS_2003_SERVER ((((CZeusApp*)AfxGetApp())->OSVersion == VER_PLATFORM_WIN32_NT) \
														&& (((CZeusApp*)AfxGetApp())->OSMajorVersion == 5) \
														&& (((CZeusApp*)AfxGetApp())->OSMinorVersion == 2))
#define IS_OS_VISTA ((((CZeusApp*)AfxGetApp())->OSVersion == VER_PLATFORM_WIN32_NT) \
														&& (((CZeusApp*)AfxGetApp())->OSMajorVersion == 6) \
														&& (((CZeusApp*)AfxGetApp())->OSMinorVersion == 0))
#define IS_OS_WINDOWS7 ((((CZeusApp*)AfxGetApp())->OSVersion == VER_PLATFORM_WIN32_NT) \
														&& (((CZeusApp*)AfxGetApp())->OSMajorVersion == 6) \
														&& (((CZeusApp*)AfxGetApp())->OSMinorVersion == 1))
#define IS_OS_2008_SERVER ((((CZeusApp*)AfxGetApp())->OSVersion == VER_PLATFORM_WIN32_NT) \
														&& (((CZeusApp*)AfxGetApp())->OSMajorVersion == 6) \
														&& (((CZeusApp*)AfxGetApp())->OSMinorVersion == 0))
#define IS_OS_WINDOWS8 ((((CZeusApp*)AfxGetApp())->OSVersion == VER_PLATFORM_WIN32_NT) \
														&& (((CZeusApp*)AfxGetApp())->OSMajorVersion == 6) \
														&& (((CZeusApp*)AfxGetApp())->OSMinorVersion > 1))

// Processor Types
#define PROCESSOR_UNKNOWN	0
#define PROCESSOR_32bit		1
#define PROCESSOR_64bit		2

// Create new doc returns
#define CREATENEWDOC_OK	         0
#define CREATENEWDOC_OK_REBOOT	 1
#define CREATENEWDOC_CANCEL		 2

#define MAX_PPDDELFILES          15 

/////////////////////////////////////////////////////////////////////////////

#endif
