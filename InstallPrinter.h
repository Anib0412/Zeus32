/////////////////////////////////////////////////////////////////////////////
// InstallPrinter.h
//                                                                        
// install printer for RasterPlus
// Copyright (C) 1998-99 Graphx, Inc.
//
//

#ifndef __INSTALLPRINTER_H__
#define __INSTALLPRINTER_H__

class CInstallPrinter : public CObject
{	
public:
	CInstallPrinter();
	~CInstallPrinter();
	BOOL InstallPrinter(CString& csDevName, CString& csFriendlyName, int iDevIdx, 
	   CString& csPortDefault, 
	   CString& csErr, BOOL& NeedToReboot, BOOL FindPrinter = FALSE, BOOL bQueueToo = FALSE, BOOL bNewQueue = FALSE);
protected:
   BOOL GetDependentFiles(CString& csFileSetTitle, CObArray& cFileSetArray);
   BOOL CopySysFile(CString& csSrc, CString& csDst, CString& csErrMsg, BOOL& NeedToReboot);
   BOOL GetPPD4Path(CString& csPPD4Path);
   void GetSysError(CString& csSysErr, CString csModule = "Install Printer", DWORD dwError = 0);   
   BOOL AddPrintMonitor(CString& csMonitor, int iKey, BOOL bReinstallMonitor, CString& csErrMsg);
   BOOL GetWinInstallFile(CString& csFileName, CString& csCopyDir);
   BOOL GetWinInstallFileNT(CString& csFileName, CString& csCopyDir);
   BOOL ExpandFile(CString& csSrc, CString& csDst);
   void HackOffReadOnlyFlags(CString& csFile);   
   void SetOurPort(CString& csDev, CString& csPort);
   BOOL ReplaceFileOnReboot (LPCTSTR pszExisting, LPCTSTR pszNew);
   BOOL MakeTempRebootFile(LPCTSTR csSrc, char* pFileName);
   void SetDefaultNTPageSize(CString& csName, CString& csPort, CString& csQueueMediaName);

   void UpdateCrossPlatformInfo(int iInstallDir, CString csDevName, CString csSrcPPDName, CString csShortName, CString csSPDName);
   void InfFileUpdate(CString& csMfg, CString& csShortName, CString& csSPDName_NT, CString& csXPINFKey);

   BOOL CreateSPDNames(CString& csDevName);

   CString csDevDir;
   CString csSysDir;
   CString csHelpFile, csDriver, csDriverUI;   
   BOOL ExtractCABFile(CString& csCABFile, CString& csCopyDir, 
      CString& csFile, int FileSize, BOOL ChainSearch);

   CString csRPDebugStr;
   char cDebugStr[255];

   CString csSPDName;
   CString csSPDName_NT;
};

#ifndef __FILESETITEM_H__
#define __FILESETITEM_H__

// need this def for now until we take out old tab dialog
class cFileSetItem : public CObject
   {
public:
   cFileSetItem(){}
   ~cFileSetItem(){}
   CString Name;
   int Location;
   int Destination;
   int Type;
   };

#define FILESET_LOCATION_OURS    0
#define FILESET_LOCATION_THEIRS  1
#define FILESET_DESTINATION_DRV  0
#define FILESET_DESTINATION_SYS  1
#define FILESET_TYPE_DRIVER      0
#define FILESET_TYPE_HELP        1 
#define FILESET_TYPE_LMONITOR	 2
#define FILESET_TYPE_PMONITOR	 3
#define FILESET_TYPE_FONTS  	 4
#define FILESET_TYPE_ICONLIB 	 5
#define FILESET_TYPE_DRIVERUI    6
#endif


#endif
