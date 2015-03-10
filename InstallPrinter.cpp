/////////////////////////////////////////////////////////////////////////////
// InstallPrinter.cpp
//                                                                        
// install printer for RasterPlus
// Copyright (C) 1998-2002 Graphx, Inc.
//
// mods:
//
// 5-8-98   Version 2.0.3J - wasn't processng FILESET_LOCATION_THEIRS
//          properly - fixed
// 9-04-98  Version 2.0.5 - not copying PageMaker 6.5
// 10-05-98 Version 2.0.5 - missing regclosekey in GetPPD4Path
// 10-06-98 Version 2.0.5 - VERIFYPATH failed on NULL string - fixed
// 11-03-98 Version 2.0.5 - if PageMaker was not installed, we copied ppd file to 
//                          default path - fixed
// 11-18-98 Version 2.0.5 - Separate device install for 98
// 7-09-99  Version 3.0.0 - new pscript drivers for nt
//                          hack off read only bits on sys files
//                          only copy sys files if newer
//                          make mac ppd file CrossPlatform Dir
// 7-16-99  Version 3.0.0 - if install fails, try on LPT1: and set the port manually
// 7-22-99  Version 3.0.0 - added new CopySysFile logic
// 8-03-99  Version 3.0.0 - added SetDefaultNTPageSize to set the default NT page size
//                          in the app printer driver
// 9-22-99  Version 3.0.1 - disable spooler again (seems OK now)
// 11-04-99 Version 3.0.2 - NT and 2000 print sharing not working correctly - fixed
// 12-10-99 Version 3.0.2 - RE-ENABLE spooler again (sigh)
// 01-06-00 Version 3.0.3 - Assign maximum priority to installed printer to get around stalling
//                          problem where rip takes all the time and the RE-ENABLED spooler can't
//                          get anything done
// 04-11-00 Version 3.0.3 - Copy PPD files to CrossPlatform 2000 directory
// 08-09-00 Version 4.0.0 - Added ME CrossPlatform Support
// 12-05-00 Version 4.0.2 - record where ppd files are so we can delete them
// 05-15-01 Version 4.0.3 - Fix protection error in SetDefaultNTPageSize ( memcpy to uninitialized pointer )
// 08-20-01 Version 5.0.0 - Add to to create a separate PPD file for each Queue
// 10-15-01 Version 5.0.0 - Add XP Support
// 02-12-02 Version 5.0.4 - fixed page republish problem on Win NT/2000
// 03-07-02 Version 5.0.4 - Fix PPD creation problem for devices with '/' in the name see AGFA
// 03-13-02 Version 5.0.4 - Fix Printer Sharing of Multiple Queues
// 07-10-02 Version 5.0.6 - ME CrossPlatform not being deleted.
// 08-08-02 Version 5.0.7 - Change MSC crossplatform  file to use Friendly/Short name
//                          Change MakeSPDFile call to pass Short Name
// 10-08-02 Version 5.0.7 - Fix bug creating SPD Name for RP- devices
// 11-21-02 Version 5.1.1 - Delete the temp file created when creating a new queue
// 01-22-03 Version 5.1.1 - Fix Invalid extension for XP PPD file
// 12-15-03 Version 5.2.2 - Add support for Windows2003 Server
// 12-31-03 Version 5.2.3 - Fix PPD files not being deleted from System and 95 Crossplatform folders
//

#define _MSC_VER 1200
#define _WIN32_WINNT 0x0501

#include "stdafx.h"
// #include "WinBase.h"
#include "zeus.h"
#include "InstallPrinter.h"
#include "CabInstall.h"
#include "winspool.h"
#include "device.h"
#include "profile.h"
#include <stdlib.h>
#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// #define INSTALL_PORT_64 1
#define MAXSIZE_DEPFILE_BUF 32000      

CInstallPrinter::CInstallPrinter()
   {
   DWORD dwNeeded;

   csRPDebugStr.Format("CInstallPrinter = %s\n", "Constructor");
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr);

   // get printer install directory
   ::GetPrinterDriverDirectory(NULL, NULL, 
      1, (LPBYTE)csDevDir.GetBuffer(_MAX_PATH), _MAX_PATH, &dwNeeded);   
   csDevDir.ReleaseBuffer();
   VERIFYPATH(csDevDir);

   // get system directory
   csRPDebugStr.Format("CInstallPrinter ProcessorType = %d\n", ((CZeusApp*)AfxGetApp())->iProcessorType);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr);

   csRPDebugStr.Format("GetPrinterDriverDirectory = %s\n", csDevDir);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr);

   if ( ((CZeusApp*)AfxGetApp())->iProcessorType == PROCESSOR_64bit )
   {
   	   ::GetWindowsDirectory(csSysDir.GetBuffer(_MAX_PATH), _MAX_PATH);
	   csSysDir.ReleaseBuffer();
	   csSysDir += "\\Sysnative\\";
	   VERIFYPATH(csSysDir);
   }
   else
   {
	   ::GetSystemDirectory(csSysDir.GetBuffer(_MAX_PATH), _MAX_PATH);
	   csSysDir.ReleaseBuffer();
	   VERIFYPATH(csSysDir);
   }

	csRPDebugStr.Format("System Directory = %s\n", csSysDir);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr);

	return;
  }

CInstallPrinter::~CInstallPrinter()
{
}

BOOL CInstallPrinter::InstallPrinter(CString& csDevName, CString& csFriendlyName, 
									 int iDevIdx, CString& csPortDefault, 
									 CString& csErr, BOOL& NeedToReboot, BOOL FindPrinter,
									 BOOL bQueueToo, BOOL bNewQueue)
   {   
   int i;   
   HANDLE hRet;
   BOOL ret;
   DWORD dwError;
   DRIVER_INFO_3 di;
//   DRIVER_INFO_8 di8;
   PRINTER_INFO_2 pi, piRetry;   
   BOOL BadCopy;
   HANDLE hPrinter;   
   cFileSetItem* cfsi;
   int CopyPos;   
   char* pDependentFiles;

   CString cs, csTemp;
   CString csRegSPDRegKey;
   CString csDevinstallDir;
   CString csSrc, csDst;
   CString csErrMsg;
   CString csCopyErr;
   CObArray cFileSetArray;
   CDevice* pCD;   
   CProfile Profile;
   CString csDefDataType, csPrintProcessor, csComment, csEnvironment;   
   CString csMonitor;   
   CString csPort;   
   CString csSrcPPDName, csDestPPDName, csDestPPDRoot;
   BOOL bPortMonitorReInstall = FALSE;
   BOOL bLangMonitorReInstall = FALSE;   
   CString csQueueMediaName;
   CStringArray csaFormNames;

   CString csShortName;

   CFileFind findFile;

   AfxGetApp()->DoWaitCursor(1);   

   // make a new device
   pCD = new CDevice;
   // load it
   if (!pCD->LoadDevice(iDevIdx)) 
      {
	  csErr.LoadString(IDS_DEVINSTALL_ERR_NOLOADDEV);
	  return FALSE;
	  }

   if (IS_OS_NT_FAMILY)
	  {
	  // create a string array of form names
	  if (pCD->GetDevInfo())
		 {
		 MEDIA Media;
		 if (pCD->pDevInfo)
			{
			// update the media list
			pCD->UpdateMediaList();	  
			for (i = 0; i < pCD->pDevInfo->TotalMedia; i++)
			   {	  	  	  
			   // get the media
			   pCD->GetMedia(i, &Media);	  
			   // add to the list
			   csaFormNames.Add(Media.Name);
			   }
			}	
		 }   
	  }

   // the default
   csPort = csPortDefault;

   // should we look for a printer   
   if (FindPrinter)	  
      {	  
	  if (bQueueToo)
	     csTemp.Format(IDS_DEVNOTFOUND_QUEUE, csDevName);
	  else
		 csTemp.Format(IDS_DEVNOTFOUND, csDevName);

	  // find the connection
	  int dummy;
	  if (!pCD->FindConnection(csPort.GetBuffer(_MAX_PATH), &dummy))
		 AfxMessageBox(csTemp, MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);
	  csPort.ReleaseBuffer();
	  }

   // first try to delete the printer driver in case
   // remnants exist      
   PRINTER_DEFAULTS pd;
   pd.pDatatype = NULL;
   pd.pDevMode = NULL;
   pd.DesiredAccess = PRINTER_ALL_ACCESS;

   int iDotUnique	= csFriendlyName.Find('.');
   int iSlashUnique	= csFriendlyName.Find('\\');
   if ( iSlashUnique < 0 )
	iSlashUnique	= csFriendlyName.Find('/');

   if ( iDotUnique < 0 && iSlashUnique < 0 )
		csShortName = csFriendlyName;
   else
   {
	   if ( iDotUnique < iSlashUnique && iDotUnique > 0 )
		   csShortName = csFriendlyName.Left(iDotUnique);
	   else
		   csShortName = csFriendlyName.Left(iSlashUnique);
   }

   if (::OpenPrinter((char*)(const char*)csFriendlyName, &hPrinter, &pd))
      {
	  if (IS_OS_NT_FAMILY)
		 {
		 // delete all existing forms
		 for (i = 0; i < csaFormNames.GetSize(); i++)
			{	  	  	  
			cs = csaFormNames.GetAt(i);
			DeleteForm(hPrinter, (char*)(const char*)cs);
			}
		 }
	  
   	  if (hPrinter) 	     
	  	 DeletePrinter(hPrinter);

	  if (hPrinter)
		  ::ClosePrinter(hPrinter);

	  // only delete if no one else is using the driver
	  if (!((CZeusApp*)AfxGetApp())->IsPrinterDriverStillUsed(csDevName))
		 {
		 if (!DeletePrinterDriver(NULL, NULL, (char*)(const char*)csDevName))		 
			{
			GetSysError(csErr, "Delete Printer Driver ");
			}		 
		 }

	  // delete ppd files and pcf files
	  csTemp = pCD->Name + "\\" + csShortName;
      ((CZeusApp*)AfxGetApp())->DeleteAllPPDPCFFiles(pCD->Name, csShortName, pCD->DevIdx);
      }	    															   


   // Generate a unique SPD Name
   CreateSPDNames(csDevName);

   csSrcPPDName = csDevDir;

   if (!IS_OS_NT_FAMILY)
       csSrcPPDName += csSPDName;
   else
	   csSrcPPDName += csSPDName_NT;

   // make it directly in system directory
   if (!pCD->MakeSPDFile(csSrcPPDName, csShortName))
      {
	  delete pCD;
	  AfxGetApp()->DoWaitCursor(-1);
	  csErr.LoadString(IDS_DEVINSTALL_ERR_NOMAKESPD);
	  return FALSE;
	  }

   // get the mac crossplatform dir
   csDestPPDName = Profile.GetStr(IDS_DEVICES, IDS_DEVICE_DEVINSTALLDIR_MAC);
   VERIFYPATH(csDestPPDName);

   // use the device name for the filename
   csTemp = csShortName;
   ((CZeusApp*)AfxGetApp())->NukeBadFnameChars(csTemp);
   csDestPPDName += csTemp;

   // now copy it
   HackOffReadOnlyFlags(csSrcPPDName);
   CopyFile(csSrcPPDName, csDestPPDName, FALSE);
   // record this file so we can delete it
   ((CZeusApp*)AfxGetApp())->RecordPPDFile(csDevName, csDestPPDName, 0);
      
   // get the pm 6.5 dir   
   if (GetPPD4Path(csDestPPDName))
	  {
      VERIFYPATH(csDestPPDName);
	  // put them together	
      csDestPPDName += csSPDName_NT;
	  // now copy it
      CopyFile(csSrcPPDName, csDestPPDName, FALSE);
      // record this file so we can delete it
	   csTemp = csDevName + "\\" + csShortName;
      ((CZeusApp*)AfxGetApp())->RecordPPDFile(csTemp, csDestPPDName, 1);
	  }


   // Update CrossPlatform Info for Windows 95
   UpdateCrossPlatformInfo(IDS_DEVICE_DEVINSTALLDIR, csDevName, csSrcPPDName, csShortName, csSPDName);

   // Update CrossPlatform Info for Windows 98
   UpdateCrossPlatformInfo(IDS_DEVICE_DEVINSTALLDIR_98, csDevName, csSrcPPDName, csShortName, csSPDName);

   // Update CrossPlatform Info for Windows ME
   UpdateCrossPlatformInfo(IDS_DEVICE_DEVINSTALLDIR_ME, csDevName, csSrcPPDName, csShortName, csSPDName);

   // Update CrossPlatform Info for Windows XP
   UpdateCrossPlatformInfo(IDS_DEVICE_DEVINSTALLDIR_XP, csDevName, csSrcPPDName, csShortName, csSPDName_NT);

   // Update CrossPlatform Info for Windows 2000
   UpdateCrossPlatformInfo(IDS_DEVICE_DEVINSTALLDIR_2000, csDevName, csSrcPPDName, csShortName, csSPDName_NT);

   // Update CrossPlatform Info for Windows 2003
   UpdateCrossPlatformInfo(IDS_DEVICE_DEVINSTALLDIR_2003, csDevName, csSrcPPDName, csShortName, csSPDName_NT);

   // Update CrossPlatform Info for Windows NT
   UpdateCrossPlatformInfo(IDS_DEVICE_DEVINSTALLDIR_NT, csDevName, csSrcPPDName, csShortName, csSPDName_NT);

   // Update CrossPlatform Info for Windows VISTA
   UpdateCrossPlatformInfo(IDS_DEVICE_DEVINSTALLDIR_VISTA, csDevName, csSrcPPDName, csShortName, csSPDName_NT);

   // Update CrossPlatform Info for Windows VISTA AMD64
   UpdateCrossPlatformInfo(IDS_DEVICE_DEVINSTALLDIR_VISTA_AMD64, csDevName, csSrcPPDName, csShortName, csSPDName_NT);

   // Update CrossPlatform Info for Window7
   UpdateCrossPlatformInfo(IDS_DEVICE_DEVINSTALLDIR_WINDOWS7, csDevName, csSrcPPDName, csShortName, csSPDName_NT);

   // Update CrossPlatform Info for Window7x64
   UpdateCrossPlatformInfo(IDS_DEVICE_DEVINSTALLDIR_WINDOWS7x64, csDevName, csSrcPPDName, csShortName, csSPDName_NT);

   // Update CrossPlatform Info for Windows8
   UpdateCrossPlatformInfo(IDS_DEVICE_DEVINSTALLDIR_WINDOWS8, csDevName, csSrcPPDName, csShortName, csSPDName_NT);

   // if it exists, then copy the ppd file there as well
   if (csDestPPDName.GetLength() > 1)
	  {
      // now copy it
      CopyFile(csSrcPPDName, csDestPPDName, FALSE);
      // record this file so we can delete it
	   csTemp = csDevName + "\\" + csShortName;
      ((CZeusApp*)AfxGetApp())->RecordPPDFile(csTemp, csDestPPDName, 8);
	  }

   ((CZeusApp*)AfxGetApp())->RecordPPDFile(csTemp, csSPDName, -1);
   ((CZeusApp*)AfxGetApp())->RecordPPDFile(csTemp, csSPDName_NT, -2);

   // now copy relevent data to the CrossPlatform inf files
   CString csMfg;
   CString csInfDev;
   CString csINFININame;
   CString csSPDINF1, csSPDINF2, csXPINFKey;

   // Windows 95
   csXPINFKey = "CrossPlatformINF";
   csMfg = "Graphx, Inc.";
   InfFileUpdate(csMfg, csShortName, csSPDName_NT, csXPINFKey);

   // Windows 98
   csXPINFKey = "CrossPlatformINF98";
   InfFileUpdate(csMfg, csShortName, csSPDName_NT, csXPINFKey);

   // Windows NT
   csXPINFKey = "CrossPlatformINFNT";
   InfFileUpdate(csMfg, csShortName, csSPDName_NT, csXPINFKey);

   // Windows Vista x32
   csXPINFKey = "CrossPlatformINFVISTA";
   InfFileUpdate(csMfg, csShortName, csSPDName_NT, csXPINFKey);

   // Windows VISTA64
   // [Graphx, Inc.]
   csXPINFKey = "CrossPlatformINFVISTAAMD64";
   InfFileUpdate(csMfg, csShortName, csSPDName_NT, csXPINFKey);

   // Windows 2000
   csXPINFKey = "CrossPlatformINF2000";
   InfFileUpdate(csMfg, csShortName, csSPDName_NT, csXPINFKey);

   // Windows ME
   csXPINFKey = "CrossPlatformINFME";
   InfFileUpdate(csMfg, csShortName, csSPDName_NT, csXPINFKey);

   // Windows XP
   csXPINFKey = "CrossPlatformINFXP";
   InfFileUpdate(csMfg, csShortName, csSPDName_NT, csXPINFKey);

   // Windows 2003 Server
   csXPINFKey = "CrossPlatformINF2003";
   InfFileUpdate(csMfg, csShortName, csSPDName_NT, csXPINFKey);

   // Windows7
   csXPINFKey = "CrossPlatformINFWindows7";
   InfFileUpdate(csMfg, csShortName, csSPDName_NT, csXPINFKey);

   // Windows7x64
   csXPINFKey = "CrossPlatformINFWindows7x64";
   InfFileUpdate(csMfg, csShortName, csSPDName_NT, csXPINFKey);

   // Windows8
   csXPINFKey = "CrossPlatformINFWindows8";
   InfFileUpdate(csMfg, csShortName, csSPDName_NT, csXPINFKey);

   csRPDebugStr.Format("Platform ID = %d\n", ((CZeusApp*)AfxGetApp())->OSVersion);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr);   

   csRPDebugStr.Format("Major Version = %d\nMinor Version = %d\n", ((CZeusApp*)AfxGetApp())->OSMajorVersion, ((CZeusApp*)AfxGetApp())->OSMinorVersion);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr);
   
   csRPDebugStr.Format("IS2 ProcessorType = %d\n", ((CZeusApp*)AfxGetApp())->iProcessorType);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr);

   // get the device installation dir   
   if (IS_OS_2000)
   {
	  csDevinstallDir = Profile.GetStr(IDS_DEVICES, IDS_DEVICE_DEVINSTALLDIR_2000);
   }
   else if (IS_OS_2003_SERVER)
   {
	  csDevinstallDir = Profile.GetStr(IDS_DEVICES, IDS_DEVICE_DEVINSTALLDIR_2003);
   }
   else if (IS_OS_NT)
   {
	  csDevinstallDir = Profile.GetStr(IDS_DEVICES, IDS_DEVICE_DEVINSTALLDIR_NT);
   }
   else if (IS_OS_98)
   {
      csDevinstallDir = Profile.GetStr(IDS_DEVICES, IDS_DEVICE_DEVINSTALLDIR_98);
   }
   else if (IS_OS_ME)   
   {
      csDevinstallDir = Profile.GetStr(IDS_DEVICES, IDS_DEVICE_DEVINSTALLDIR_ME);
   }
   else if (IS_OS_XP)
   {
	  csDevinstallDir = Profile.GetStr(IDS_DEVICES, IDS_DEVICE_DEVINSTALLDIR_XP);
   }
   else if (IS_OS_WINDOWS7)   
   {
	   csRPDebugStr.Format("IS2 ProcessorType = %d\n", ((CZeusApp*)AfxGetApp())->iProcessorType);
	   strcpy(cDebugStr, csRPDebugStr);
	   RPDEBUGSTRMFC(cDebugStr);

	   if ( ((CZeusApp*)AfxGetApp())->iProcessorType == PROCESSOR_64bit )
	   {
			csRPDebugStr.Format("Found OS = %s\n", "Windows7 AMD64");
			strcpy(cDebugStr, csRPDebugStr);
			RPDEBUGSTRMFC(cDebugStr);   

			csDevinstallDir = Profile.GetStr(IDS_DEVICES, IDS_DEVICE_DEVINSTALLDIR_VISTA_AMD64);
	   }
	   else
	   {
			csRPDebugStr.Format("Found OS = %s\n", "Windows7 x32");
			strcpy(cDebugStr, csRPDebugStr);
			RPDEBUGSTRMFC(cDebugStr);   

			csDevinstallDir = Profile.GetStr(IDS_DEVICES, IDS_DEVICE_DEVINSTALLDIR_VISTA);
	   }
   }
   else if (IS_OS_VISTA)   
   {
	   csRPDebugStr.Format("IS2 ProcessorType = %d\n", ((CZeusApp*)AfxGetApp())->iProcessorType);
	   strcpy(cDebugStr, csRPDebugStr);
	   RPDEBUGSTRMFC(cDebugStr);

	   if ( ((CZeusApp*)AfxGetApp())->iProcessorType == PROCESSOR_64bit )
	   {
			csRPDebugStr.Format("Found OS = %s\n", "Vista AMD64");
			strcpy(cDebugStr, csRPDebugStr);
			RPDEBUGSTRMFC(cDebugStr);   

			csDevinstallDir = Profile.GetStr(IDS_DEVICES, IDS_DEVICE_DEVINSTALLDIR_VISTA_AMD64);
	   }
	   else
	   {
			csRPDebugStr.Format("Found OS = %s\n", "Vista");
			strcpy(cDebugStr, csRPDebugStr);
			RPDEBUGSTRMFC(cDebugStr);   

			csDevinstallDir = Profile.GetStr(IDS_DEVICES, IDS_DEVICE_DEVINSTALLDIR_VISTA);
	   }
   }
   else if (IS_OS_2008_SERVER)
   {
	  csDevinstallDir = Profile.GetStr(IDS_DEVICES, IDS_DEVICE_DEVINSTALLDIR_2008);
   }
   else if (IS_OS_WINDOWS8)   
   {
	  csDevinstallDir = Profile.GetStr(IDS_DEVICES, IDS_DEVICE_DEVINSTALLDIR_WINDOWS8);
	csRPDebugStr.Format("Windows v8.1 csDevinstallDir = %s\n", csDevinstallDir);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr);   

   }
   else
   {
	  csDevinstallDir = Profile.GetStr(IDS_DEVICES, IDS_DEVICE_DEVINSTALLDIR);
   }

	csRPDebugStr.Format("csDevinstallDir = %s\n", csDevinstallDir);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr);   

   // verify path
   VERIFYPATH(csDevinstallDir);

   // make dependent file buf
   pDependentFiles = new char [MAXSIZE_DEPFILE_BUF];   
   memset(pDependentFiles, 0, MAXSIZE_DEPFILE_BUF);

   // get dependent files
   CString csFileSetTitle;   
   if (IS_OS_2000)
	  cs.LoadString(IDS_DEVICE_DEPENDENT_FILESET_2000);   
   else if (IS_OS_XP)
	  cs.LoadString(IDS_DEVICE_DEPENDENT_FILESET_2000);         
   else if (IS_OS_2003_SERVER)
	  cs.LoadString(IDS_DEVICE_DEPENDENT_FILESET_2003);         
   else if (IS_OS_NT)
	  cs.LoadString(IDS_DEVICE_DEPENDENT_FILESET_NT);         
   else if (IS_OS_WINDOWS7)
   {
	   csRPDebugStr.Format("FileSet ProcessorType = %d\n", ((CZeusApp*)AfxGetApp())->iProcessorType);
	   strcpy(cDebugStr, csRPDebugStr);
	   RPDEBUGSTRMFC(cDebugStr);
	   if ( ((CZeusApp*)AfxGetApp())->iProcessorType == PROCESSOR_64bit )
	   {
			cs.LoadString(IDS_DEVICE_DEPENDENT_FILESET_W7X64);
	   }
	   else
	   {
			cs.LoadString(IDS_DEVICE_DEPENDENT_FILESET_2003);
	   }
   }
   else if (IS_OS_VISTA)
   {
	   csRPDebugStr.Format("FileSet ProcessorType = %d\n", ((CZeusApp*)AfxGetApp())->iProcessorType);
	   strcpy(cDebugStr, csRPDebugStr);
	   RPDEBUGSTRMFC(cDebugStr);
	   if ( ((CZeusApp*)AfxGetApp())->iProcessorType == PROCESSOR_64bit )
	   {
			cs.LoadString(IDS_DEVICE_DEPENDENT_FILESET_AMD64);
	   }
	   else
	   {
			cs.LoadString(IDS_DEVICE_DEPENDENT_FILESET_2003);
	   }
   }
   else if (IS_OS_WINDOWS8)   
   {
	  cs.LoadString(IDS_DEVICE_DEPENDENT_FILESET_WINDOWS8);
   }
   else
   	  cs.LoadString(IDS_DEVICE_DEPENDENT_FILESET);

   	csRPDebugStr.Format("Profile.GetStr(%s, %s, _T(""))\n", csDevName, cs);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr); 
	  
   csFileSetTitle = Profile.GetStr(csDevName, cs, _T("")); 

	csRPDebugStr.Format("cs = %s\n", cs);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr); 

	csRPDebugStr.Format("csFileSetTitle = %s\n", csFileSetTitle);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr); 

	// get them

   	csRPDebugStr.Format("GetDependentFiles(%s, cFileSetArray)\n", csFileSetTitle);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr);

   if (GetDependentFiles(csFileSetTitle, cFileSetArray))
      {
	  CopyPos = 0;
	  BadCopy = FALSE;
	  for (i = 0; i < cFileSetArray.GetSize(); i++)
         {
	     cfsi = (cFileSetItem*)cFileSetArray.GetAt(i);
		 // copy based on location
	     switch(cfsi->Location)
	        {
		    case FILESET_LOCATION_OURS:
				 // copy the file from our devinstalldir to the system dir				 
				 csSrc = csDevinstallDir;
				 csSrc += cfsi->Name;			
				 if (FILESET_DESTINATION_SYS)
					{
				    csDst = csSysDir + cfsi->Name;				 

					csRPDebugStr.Format("csDst = %s, csSrc = %s\n",csDst, csSrc);
					strcpy(cDebugStr, csRPDebugStr);
					RPDEBUGSTRMFC(cDebugStr);

				    if (!CopySysFile(csSrc, csDst, csCopyErr, NeedToReboot))
						{
						csErrMsg.Empty();
						csErrMsg.Format(IDS_DEVINSTALL_ERR_BADCOPY, csCopyErr, csSrc, csDst);

						csRPDebugStr = "CopySysFile FAILED - FILESET_DESTINATION_SYS(OURS)";
						strcpy(cDebugStr, csRPDebugStr);
						RPDEBUGSTRMFC(cDebugStr);

				        BadCopy = TRUE;
						}
					}
				 if (!BadCopy && (IS_OS_NT_FAMILY))
					{
					csDst = csDevDir + cfsi->Name;				 
				    if (!CopySysFile(csSrc, csDst, csCopyErr, NeedToReboot))
						{
						csErrMsg.Empty();
						csErrMsg.Format(IDS_DEVINSTALL_ERR_BADCOPY, csCopyErr, csSrc, csDst);

						csRPDebugStr = "CopySysFile IS_OS_NT_FAMILY - FILESET_DESTINATION_SYS(OURS)";
						strcpy(cDebugStr, csRPDebugStr);
						RPDEBUGSTRMFC(cDebugStr);

				        BadCopy = TRUE;
						}
					}


		         // check port monitor installation
		         if (NeedToReboot && cfsi->Type == FILESET_TYPE_PMONITOR)
					bPortMonitorReInstall = TRUE;
		         else if (NeedToReboot && cfsi->Type == FILESET_TYPE_LMONITOR)
					bLangMonitorReInstall = TRUE;
				 break;
            case FILESET_LOCATION_THEIRS:
			     // we ship these files now
				 ret = TRUE;
				 if (ret)
				    {
				    // and copy it		            
					csSrc = csDevinstallDir;
				    csSrc += cfsi->Name;
				    if (FILESET_DESTINATION_SYS)
					   {
				       csDst = csSysDir + cfsi->Name;				 

					csRPDebugStr.Format("csDst = %s, csSrc = %s\n",csDst, csSrc);
					strcpy(cDebugStr, csRPDebugStr);
					RPDEBUGSTRMFC(cDebugStr);

				       if (!CopySysFile(csSrc, csDst, csCopyErr, NeedToReboot))
						{
						csErrMsg.Empty();
						csErrMsg.Format(IDS_DEVINSTALL_ERR_BADCOPY, csCopyErr, csSrc, csDst);

						csRPDebugStr = "CopySysFile FAILED - FILESET_DESTINATION_SYS(THEIRS)";
						strcpy(cDebugStr, csRPDebugStr);
						RPDEBUGSTRMFC(cDebugStr);

						BadCopy = TRUE;
						}
					   }
				    if (!BadCopy && (IS_OS_NT_FAMILY))
					   {
					   csDst = csDevDir + cfsi->Name;				 
				       if (!CopySysFile(csSrc, csDst, csCopyErr, NeedToReboot))
					   {
						  csErrMsg.Empty();
						  csErrMsg.Format(IDS_DEVINSTALL_ERR_BADCOPY, csCopyErr, csSrc, csDst);

						  csRPDebugStr = "IS_OS_NT_FAMILY - FILESET_DESTINATION_SYS(THEIRS)";
						  strcpy(cDebugStr, csRPDebugStr);
						  RPDEBUGSTRMFC(cDebugStr);

				          BadCopy = TRUE;
					   }
					   }
					}
				 else
				    BadCopy = TRUE;
				 break;
		    }

		 // install language monitor
         if (!BadCopy && cfsi->Type == FILESET_TYPE_LMONITOR)
		 {

			csRPDebugStr.Format("AddLanguageMonitor = %s\n",cfsi->Name);
			strcpy(cDebugStr, csRPDebugStr);
			RPDEBUGSTRMFC(cDebugStr);

			csErrMsg.Empty();

			BadCopy = (!AddPrintMonitor(cfsi->Name, IDS_DRIVER_OUR_LANG_MONITOR, 
														bPortMonitorReInstall, csErrMsg));

			if ( BadCopy )
			{
				csRPDebugStr.Format("AddLanguageMonitor = %s\n","BadCopy");
				strcpy(cDebugStr, csRPDebugStr);
				RPDEBUGSTRMFC(cDebugStr);
			}
		 }

		 // install port monitor
         if (!BadCopy && cfsi->Type == FILESET_TYPE_PMONITOR)
		 {

			csRPDebugStr.Format("AddPortMonitor = %s\n",cfsi->Name);
			strcpy(cDebugStr, csRPDebugStr);
			RPDEBUGSTRMFC(cDebugStr);

		    csErrMsg.Empty();
			BadCopy = (!AddPrintMonitor(cfsi->Name, IDS_DRIVER_OUR_PORT_MONITOR, 
														bLangMonitorReInstall, csErrMsg));

			if ( BadCopy )
			{
				csRPDebugStr.Format("AddPortMonitor = %s\n","BadCopy");
				strcpy(cDebugStr, csRPDebugStr);
				RPDEBUGSTRMFC(cDebugStr);
			}
		 }

		csRPDebugStr.Format("InstallPrinter = %s\n","Copy Dependent Files");
		strcpy(cDebugStr, csRPDebugStr);
		RPDEBUGSTRMFC(cDebugStr);

		 // add to dependent list
		 memcpy(pDependentFiles + CopyPos, (void*)(const char*)cfsi->Name, 
														cfsi->Name.GetLength());

		 // increment CopyPos
		 // goal is to have a string like: PSCRIPT.DRV\0PSCRIPT.HLP\0\0
		 CopyPos += cfsi->Name.GetLength() + 1;

		 // error?
		 if (BadCopy)
			 break;
         }     

		csRPDebugStr.Format("InstallPrinter = %s\n","Add Dependent File List");
		strcpy(cDebugStr, csRPDebugStr);
		RPDEBUGSTRMFC(cDebugStr);

  	  // add spd file to dependent file list	  
      
	  // get proper name
      if (!IS_OS_NT_FAMILY)
		  memcpy(pDependentFiles + CopyPos, (void*)(const char*)csSPDName, csSPDName.GetLength());
      else
		  memcpy(pDependentFiles + CopyPos, (void*)(const char*)csSPDName_NT, csSPDName.GetLength());
  
	  // delete the entry		 
      for (i = 0; i < cFileSetArray.GetSize(); i++)
         {
		 cfsi = (cFileSetItem*)cFileSetArray.GetAt(i);
		 delete cfsi;
		 }	  
	  // remove all pointers
      cFileSetArray.RemoveAll();     
	  }

   if (BadCopy) 
      {
      AfxGetApp()->DoWaitCursor(-1);	  
      delete pDependentFiles;

	  if ( csErrMsg.IsEmpty() )
		  csErr.LoadString(IDS_DEVINSTALL_ERR_BADCOPY);
	  else
		  csErr = csErrMsg;

      return FALSE;   
	  }   

#if 0
   if (PortMonitorReboot || LangMonitorReboot)
	  {
	  AfxMessageBox("System files were installed\nYou must reboot the system and install the printer again", 
		 MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);
	  return FALSE;   
	  }
#endif

   // we cast a bunch of stuff from const char * to char *, 
   // but this is ok (i think) as AddPrinter... doesn't hammer vars
   // add printer driver
   di.cVersion = 0x400;
   di.pName = (char*)(const char*)csFriendlyName;

   	csRPDebugStr.Format("di.pName = %s\n", csFriendlyName);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr);

   if (!IS_OS_NT_FAMILY)
      csEnvironment.LoadString(IDS_DEVICE_ENVIRONMENT);
   else
   {
	  int iProcessorType = ((CZeusApp*)AfxGetApp())->iProcessorType;

   	  csRPDebugStr.Format("InstallPrinter ProcessorType = %d\n", iProcessorType);
	  strcpy(cDebugStr, csRPDebugStr);
	  RPDEBUGSTRMFC(cDebugStr);

	  if ( ((CZeusApp*)AfxGetApp())->iProcessorType == PROCESSOR_64bit )
		csEnvironment.LoadString(IDS_DEVICE_ENVIRONMENT_VISTA_AMD64);
	  else
		csEnvironment.LoadString(IDS_DEVICE_ENVIRONMENT_NT);
   }

   di.pEnvironment = (char*)(const char*)csEnvironment; 
   di.pDriverPath = (char*)(const char*)csDriver;

 	csRPDebugStr.Format("di.pEnvironment = %s\n", csEnvironment);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr);

   	csRPDebugStr.Format("di.pDriverPath = %s\n", csDriver);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr);

   if (!IS_OS_NT_FAMILY)
   {
      di.pConfigFile = (char*)(const char*)csDriver;   

   	  csRPDebugStr.Format("pConfigFile (NOT NT FAMILY) = %s\n", csDriver);
	  strcpy(cDebugStr, csRPDebugStr);
	  RPDEBUGSTRMFC(cDebugStr);
   }
   else
   {
	  di.pConfigFile = (char*)(const char*)csDriverUI;

   	  csRPDebugStr.Format("pConfigFile(NT FAMILY) = %s\n", csDriverUI);
	  strcpy(cDebugStr, csRPDebugStr);
	  RPDEBUGSTRMFC(cDebugStr);
   }
   
 	csRPDebugStr.Format("di.pConfigFile = %s\n", csDriverUI);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr);


   di.pHelpFile = (char*)(const char*)csHelpFile;
   
	csRPDebugStr.Format("di.pHelpFile = %s\n", csHelpFile);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr);

   di.pDependentFiles = pDependentFiles;

   csMonitor.LoadString(IDS_DEVICE_LANG_MONITOR_DESC);
   di.pMonitorName = (char*)(const char*)csMonitor;         
   csDefDataType.LoadString(IDS_DEVICE_DATATYPE);
   di.pDefaultDataType = (char*)(const char*)csDefDataType;
   
 	csRPDebugStr.Format("di.pMonitorName = %s\n", csMonitor);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr);

   	csRPDebugStr.Format("di.pDefaultDataType = %s\n", csDefDataType);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr);

   if (!IS_OS_NT_FAMILY)
   {
	   csTemp = csDevinstallDir + csShortName + "\\" + (char*)(const char*)csSPDName;

	   csRPDebugStr.Format("pDataFile(NOT NT FAMILY) = %s\n", csTemp);
	   strcpy(cDebugStr, csRPDebugStr);
	   RPDEBUGSTRMFC(cDebugStr);
   }
   else
   {
	   csTemp = csDevinstallDir + csShortName + "\\" + (char*)(const char*)csSPDName_NT;

	   csRPDebugStr.Format("pDataFile(NT FAMILY) = %s\n", csTemp);
	   strcpy(cDebugStr, csRPDebugStr);
	   RPDEBUGSTRMFC(cDebugStr);
   }
   di.pDataFile = (char*)(const char*)csTemp;


   ret = AddPrinterDriverEx(NULL, 3, (LPBYTE)&di, APD_COPY_NEW_FILES);
   dwError = GetLastError();

   // check return
   //  ERROR_MOD_NOT_FOUND(126) as OK
   if (!ret && dwError != ERROR_MOD_NOT_FOUND) 
     {

	   dwError = GetLastError();

	   if ( !IS_OS_NT_FAMILY )
	   {
	  		csRPDebugStr.Format("AddPrinterDriver - Failed(%d)\n", dwError);
			strcpy(cDebugStr, csRPDebugStr);
			RPDEBUGSTRMFC(cDebugStr);

			GetSysError(csErr, "AddPrinterDriver", dwError);
			csRPDebugStr.Format("System Failure %s\n", csErr);
			strcpy(cDebugStr, csRPDebugStr);
			RPDEBUGSTRMFC(cDebugStr);
	   }
	   else
	   {
	  		csRPDebugStr.Format("AddPrinterDriverEx - Failed(%d)\n", dwError);
			strcpy(cDebugStr, csRPDebugStr);
			RPDEBUGSTRMFC(cDebugStr);

			GetSysError(csErr, "AddPrinterDriverEx", dwError);
			csRPDebugStr.Format("System Failure %s\n", csErr);
			strcpy(cDebugStr, csRPDebugStr);
			RPDEBUGSTRMFC(cDebugStr);
	   }

       AfxGetApp()->DoWaitCursor(-1);	  
       delete pDependentFiles;   	  
	  
      return FALSE;   
	  }   		   

	csRPDebugStr.Format("AddPrinterDriverEx - Success(%d)\n", dwError);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(csRPDebugStr);

   DWORD dwSizeNeeded, dwNumPorts;
   LPBYTE lpbPorts;

   // enum our monitors to force reloading?
   // now big should the buffer be?
   ::EnumMonitors(NULL, 1, NULL, 0, &dwSizeNeeded, &dwNumPorts);

   // make the buffer
   lpbPorts = new BYTE[dwSizeNeeded];

   // get the ports
   ::EnumMonitors(NULL, 1, lpbPorts, dwSizeNeeded, &dwSizeNeeded, &dwNumPorts);

   // just delete it
   delete lpbPorts;	   					   

   // we need to enum our ports to force re-loading
   // of ports added by our port monitor
   // how big should the buffer be?   
   ::EnumPorts(NULL, 1, NULL, 0, &dwSizeNeeded, &dwNumPorts);   

   // make the buffer
   lpbPorts = new BYTE[dwSizeNeeded];

   // get the ports   
   ::EnumPorts(NULL, 1, lpbPorts, dwSizeNeeded, &dwSizeNeeded, &dwNumPorts);

   // do the same for monitors
   // just delete it
   delete lpbPorts;

   csRPDebugStr.Format("csPort ProcessorType = %d\n", ((CZeusApp*)AfxGetApp())->iProcessorType);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr);
#ifdef INSTALL_PORT_64
   if ( ((CZeusApp*)AfxGetApp())->iProcessorType == PROCESSOR_64bit )
   {
	   csPort = "LPT1:";
   }
#endif
	csRPDebugStr.Format("csPort = %s\n", csPort);
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr);

   // add printer
   pi.pServerName = NULL;   
   pi.pPrinterName = (char*)(const char*)csFriendlyName;   
   pi.pShareName = NULL;   
   pi.pPortName = (char*)(const char*)csPort;
   pi.pDriverName = (char*)(const char*)csFriendlyName;
   csComment.LoadString(IDS_DEVICE_COMMENT);
   pi.pComment = (char*)(const char*)csComment;
   pi.pLocation = _T("");   
   pi.pDevMode = NULL;
   pi.pSepFile = NULL;
   csPrintProcessor.LoadString(IDS_DEVICE_PRINTPROCESSOR);
   pi.pPrintProcessor = (char*)(const char*)csPrintProcessor;
   csDefDataType.LoadString(IDS_DEVICE_DATATYPE);
   pi.pDatatype = (char*)(const char*)csDefDataType;
   pi.pParameters = NULL;
   pi.pSecurityDescriptor = NULL;

   pi.Attributes = PRINTER_ATTRIBUTE_ENABLE_BIDI | PRINTER_ATTRIBUTE_LOCAL;

   pi.Priority = MAX_PRIORITY;
   pi.DefaultPriority = MAX_PRIORITY;
   pi.StartTime = 0;
   pi.UntilTime = 0;

   // copy in case we want to retry
   memcpy(&piRetry, &pi, sizeof PRINTER_INFO_2);

   csRPDebugStr.Format("AddPrinter = %s\n", csFriendlyName);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr);
   
   hRet = AddPrinter(NULL, 2, (LPBYTE)&pi);   
   
   // get error here
   if (!hRet)
   {
	  DWORD dwLastError;

	  dwLastError = GetLastError();

	  GetSysError(csErr, "AddPrinter");

	  csRPDebugStr.Format("AddPrinter Error = %s (%d)\n", csErr, dwLastError);
	  strcpy(cDebugStr, csRPDebugStr);
      RPDEBUGSTRMFC(cDebugStr);
   }
   else
   {
      ClosePrinter(hRet);

  	  csRPDebugStr.Format("Printer Closed = %s\n", csFriendlyName);
	  strcpy(cDebugStr, csRPDebugStr);
      RPDEBUGSTRMFC(cDebugStr);
   }

   AfxGetApp()->DoWaitCursor(-1);

   delete pDependentFiles;   

   // if ok, and we are in NT, set the proper page size (NT botches this)
   if (hRet && pCD && IS_OS_NT_FAMILY)
	  {
	  pCD->GetCurrentMediaName(csQueueMediaName);
      SetDefaultNTPageSize(csFriendlyName, csPort, csQueueMediaName);
	  }

   // delete the device
   if (pCD)
	   delete pCD;

   // always prompt to reboot on win 95
   if (IS_OS_95)
	   NeedToReboot = TRUE;

   if (!hRet) 
      return FALSE;	  
   else
      return TRUE;
   }

BOOL CInstallPrinter::GetDependentFiles(CString& csFileSetTitle, CObArray& cFileSetArray)
   {   
   BOOL ret;
   CProfile Profile;
   CString csEntry;
   CString csTemp;
   cFileSetItem* cfsi;
   char pTempBuf[_MAX_PATH];

   // remove everything
   cFileSetArray.RemoveAll();
   ret = FALSE;

	csRPDebugStr.Format("GetDependentFiles %s\n", "Starting");
	strcpy(cDebugStr, csRPDebugStr);
	RPDEBUGSTRMFC(cDebugStr);

   csRPDebugStr.Format("GetPrinterDriverDirectory = %s\n", csDevDir);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr);


   // get driver
   csEntry.LoadString(IDS_DRIVER_DRIVER);   
   csTemp = Profile.GetStr(csFileSetTitle, csEntry, "");   

   csRPDebugStr.Format("csEntry = %s\n", csEntry);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr);

   csRPDebugStr.Format("csFileSetTitle = %s\n", csFileSetTitle);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr);

   csRPDebugStr.Format("csTemp = %s\n", csTemp);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr);

   if (csTemp.GetLength())
      {
	  strcpy(pTempBuf, csTemp);
      cfsi = new cFileSetItem;
      cfsi->Name = strtok(pTempBuf, ",");
	  csDriver = csDevDir + cfsi->Name;
      cfsi->Location = atoi(strtok(NULL, ","));  
	  cfsi->Destination = atoi(strtok(NULL, ","));  
      cfsi->Type = FILESET_TYPE_DRIVER;	  
	  // add it
	  cFileSetArray.Add(cfsi);       
	  }

   // get language monitor file
   csEntry.LoadString(IDS_DRIVER_LANGUAGE_MONITOR);   
   csTemp = Profile.GetStr(csFileSetTitle, csEntry, "");
   if (csTemp.GetLength())
      {
      strcpy(pTempBuf, csTemp);
      cfsi = new cFileSetItem;	  
      cfsi->Name = strtok(pTempBuf, ",");
      cfsi->Location = atoi(strtok(NULL, ","));  
	  cfsi->Destination = atoi(strtok(NULL, ","));  
      cfsi->Type = FILESET_TYPE_LMONITOR;
	  // add it
	  cFileSetArray.Add(cfsi);       
	  }

   // get port monitor file
   csEntry.LoadString(IDS_DRIVER_PORT_MONITOR);   
   csTemp = Profile.GetStr(csFileSetTitle, csEntry, "");
   if (csTemp.GetLength())
      {
      strcpy(pTempBuf, csTemp);
      cfsi = new cFileSetItem;	  
      cfsi->Name = strtok(pTempBuf, ",");
      cfsi->Location = atoi(strtok(NULL, ","));  
	  cfsi->Destination = atoi(strtok(NULL, ","));  
      cfsi->Type = FILESET_TYPE_PMONITOR;
	  // add it
	  cFileSetArray.Add(cfsi);       
	  }

   // get help file
   csEntry.LoadString(IDS_DRIVER_HELP);   
   csTemp = Profile.GetStr(csFileSetTitle, csEntry, "");
   if (csTemp.GetLength())
      {
      strcpy(pTempBuf, csTemp);
      cfsi = new cFileSetItem;	  
      cfsi->Name = strtok(pTempBuf, ",");
      csHelpFile = csSysDir + cfsi->Name;
      cfsi->Location = atoi(strtok(NULL, ","));  
	  cfsi->Destination = atoi(strtok(NULL, ","));  
      cfsi->Type = FILESET_TYPE_HELP;
	  // add it
	  cFileSetArray.Add(cfsi);       
	  }
   if (!IS_OS_NT_FAMILY)
	  {
      // get font file
      csEntry.LoadString(IDS_DRIVER_FONT);   
      csTemp = Profile.GetStr(csFileSetTitle, csEntry, "");

	   csRPDebugStr.Format("Font(NOT NT FAMILY) = %s\n", csTemp);
	   strcpy(cDebugStr, csRPDebugStr);
	   RPDEBUGSTRMFC(cDebugStr);

      if (csTemp.GetLength())
         {
         strcpy(pTempBuf, csTemp);
         cfsi = new cFileSetItem;	  
         cfsi->Name = strtok(pTempBuf, ",");
         cfsi->Location = atoi(strtok(NULL, ","));  
		 cfsi->Destination = atoi(strtok(NULL, ","));  
         cfsi->Type = FILESET_TYPE_FONTS;
	     // add it
	     cFileSetArray.Add(cfsi);       
		 }
	  
      // get font file
      csEntry.LoadString(IDS_DRIVER_ICONLIB);   
      csTemp = Profile.GetStr(csFileSetTitle, csEntry, "");
      if (csTemp.GetLength())
         {
         strcpy(pTempBuf, csTemp);
         cfsi = new cFileSetItem;	  
         cfsi->Name = strtok(pTempBuf, _T(","));
         cfsi->Location = atoi(strtok(NULL, _T(","))); 
		 cfsi->Destination = atoi(strtok(NULL, ","));  
         cfsi->Type = FILESET_TYPE_ICONLIB;
	     // add it
	     cFileSetArray.Add(cfsi);       
		 }
	  }
   else
	  {
      // get driver ui file
      csEntry.LoadString(IDS_DRIVER_UI);   
      csTemp = Profile.GetStr(csFileSetTitle, csEntry, "");

	   csRPDebugStr.Format("Font(NT FAMILY) = %s\n", csTemp);
	   strcpy(cDebugStr, csRPDebugStr);
	   RPDEBUGSTRMFC(cDebugStr);

      if (csTemp.GetLength())
         {
         strcpy(pTempBuf, csTemp);
         cfsi = new cFileSetItem;	  
         cfsi->Name = strtok(pTempBuf, _T(","));
		 csDriverUI = csDevDir + cfsi->Name;
         cfsi->Location = atoi(strtok(NULL, _T(","))); 
		 cfsi->Destination = atoi(strtok(NULL, ","));  
         cfsi->Type = FILESET_TYPE_DRIVERUI;
	     // add it
	     cFileSetArray.Add(cfsi);       
		 }
	  }

   return TRUE;
   }

// this method will check for existence before copying a system file
// copy a system file, it will load set [rename] in the WININIT.INI 
// file if necessary in 95/98 and do a MoveFileEx in NT
BOOL CInstallPrinter::CopySysFile(CString& csSrc, CString& csDst, CString& csCopyErr, BOOL& NeedToReboot)
   {
   BOOL ret;
   CFile cf;	   
   CFileStatus cfsDst;   
   CFileStatus cfsSrc;            
   ret = FALSE;   

   // hack off read only flags just in case
   HackOffReadOnlyFlags(csSrc);

   // check for existence
   if (!cf.GetStatus(csDst, cfsDst))
      {	  
	  // doesn't exist, do a straight copy
		csRPDebugStr.Format("CopySysFile Src = %s", csSrc);
		strcpy(cDebugStr, csRPDebugStr);
		RPDEBUGSTRMFC(cDebugStr);

		csRPDebugStr.Format("Dest = %s", csDst);
		strcpy(cDebugStr, csRPDebugStr);
		RPDEBUGSTRMFC(cDebugStr);

	    ret = CopyFile((const char*)csSrc, (const char*)csDst, TRUE);

		if ( ret == FALSE )
		{
			  GetSysError(csCopyErr);
		}

		csRPDebugStr.Format("CopyFile Return = %d", ret);
		strcpy(cDebugStr, csRPDebugStr);
		RPDEBUGSTRMFC(cDebugStr);
	  }
   else
      {	  
	  // get src status
	  cf.GetStatus(csSrc, cfsSrc);	  
	  // is the existing one the same or newer?
	  // if so, return true
	  if (cfsSrc.m_mtime <= cfsDst.m_mtime)	  
	     return TRUE;

		csRPDebugStr.Format("CopySysFile(Newer) Src = %s", csSrc);
		strcpy(cDebugStr, csRPDebugStr);
		RPDEBUGSTRMFC(cDebugStr);

		csRPDebugStr.Format("Dest = %s", csDst);
		strcpy(cDebugStr, csRPDebugStr);
		RPDEBUGSTRMFC(cDebugStr);

	  // hack off read only flags
	  HackOffReadOnlyFlags(csDst);

      // first try to simply copy it
	  if (!CopyFile(csSrc, csDst, FALSE))
		 {
		 // need to copy and reboot
         ret = ReplaceFileOnReboot (csDst, csSrc);
		 NeedToReboot = TRUE;
		 }
	  else 
		 ret = TRUE;      
	  }
   return ret;
   }	  

BOOL CInstallPrinter::GetPPD4Path(CString& csPPD4Path)
   {
   HKEY hKey;
   DWORD lDataSize = _MAX_PATH;
   char PPD4Path[_MAX_PATH];   
   DWORD dwType = REG_SZ;
   char *pEntry = "ppd4";
   BOOL bRet = FALSE;

   // default
   csPPD4Path = "";
   
   if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Adobe\\PageMaker65", 0, 
	  KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
      {  	  
      // get value
	  if (RegQueryValueEx(hKey, pEntry, NULL, &dwType, (LPBYTE)PPD4Path, &lDataSize) == ERROR_SUCCESS)
		 {
		 csPPD4Path = PPD4Path;
		 bRet = TRUE;
		 }
	  RegCloseKey(hKey);
	  }
   return bRet;
   }

void CInstallPrinter::GetSysError(CString& csSysErr, CString csModule, DWORD dwError)
   {
    char cErrorStr[255];
	CString csErrorMsg;
   
   if ( dwError == 0 )
	   dwError = GetLastError();

   if ( FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError, 0, cErrorStr,
																sizeof(cErrorStr), NULL ) == 0 )
   {
	   csErrorMsg = cErrorStr;
   }
   else
   {
	   csErrorMsg = "Unknown";
   }

   csSysErr = csModule;
   csSysErr += " - ";
   csSysErr += csErrorMsg;
   }

BOOL CInstallPrinter::AddPrintMonitor(CString& csMonitor, int iKey, BOOL bReinstallMonitor, CString& csErrMsg)   
   {
   BOOL ret;
   
   CString csEnvironment;
   CString csMonitorName;
   CString csErr;

   CProfile Profile;
   
   DWORD dwErr;   

   MONITOR_INFO_2 mi2;

   CString csRPDebugStr;
   char cDebugStr[255];

   csMonitorName.LoadString(iKey);   

   csRPDebugStr.Format("AddPrintMonitor (%s)\n", "Start");
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr);

   csRPDebugStr.Format("ProcessorType = %d\n", ((CZeusApp*)AfxGetApp())->iProcessorType);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr);

#ifdef INSTALL_PORT_64
   if ( ((CZeusApp*)AfxGetApp())->iProcessorType == PROCESSOR_64bit && 
				iKey == IDS_DRIVER_OUR_PORT_MONITOR)
   {
	   return TRUE;
   }
#endif

   if (!IS_OS_NT_FAMILY)
      csEnvironment.LoadString(IDS_DEVICE_ENVIRONMENT);
   else
   {
	  int iProcessorType = ((CZeusApp*)AfxGetApp())->iProcessorType;

   	  csRPDebugStr.Format("InstallPrinter ProcessorType = %d\n", iProcessorType);
	  strcpy(cDebugStr, csRPDebugStr);
	  RPDEBUGSTRMFC(cDebugStr);

	  if ( ((CZeusApp*)AfxGetApp())->iProcessorType == PROCESSOR_64bit )
		csEnvironment.LoadString(IDS_DEVICE_ENVIRONMENT_VISTA_AMD64);
	  else
		csEnvironment.LoadString(IDS_DEVICE_ENVIRONMENT_NT);
   }
    
   mi2.pName = (char*)(const char*)csMonitorName;

   if ( IS_OS_VISTA || IS_OS_WINDOWS7 || IS_OS_WINDOWS8)
   {
	   csRPDebugStr.Format("csEnvironment ProcessorType = %d\n", ((CZeusApp*)AfxGetApp())->iProcessorType);
	   strcpy(cDebugStr, csRPDebugStr);
	   RPDEBUGSTRMFC(cDebugStr);

	   if ( ((CZeusApp*)AfxGetApp())->iProcessorType == PROCESSOR_64bit )
	   {
		   csEnvironment.LoadString(IDS_DEVICE_ENVIRONMENT_VISTA_AMD64);

		   csRPDebugStr.Format("AddMonitor Environment(%s)\n", csEnvironment);
		   strcpy(cDebugStr, csRPDebugStr);
		   RPDEBUGSTRMFC(cDebugStr);
	   }

	   mi2.pEnvironment = (char*)(const char*)csEnvironment;
   }
   else
   {
	   csRPDebugStr.Format("AddMonitor Environment(%s)\n", csEnvironment);
	   strcpy(cDebugStr, csRPDebugStr);
	   RPDEBUGSTRMFC(cDebugStr);
	   mi2.pEnvironment = (char*)(const char*)csEnvironment;
   }

   csRPDebugStr.Format("AddPrintMonitor Environment = %s\n", csEnvironment);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr);

   mi2.pDLLName = (char*)(const char*)csMonitor;

   // if we're installing a new monitor, delete the old one first
   if (bReinstallMonitor)
	  {
		  csRPDebugStr.Format("DeleteMonitor  %s\n", "Deleting");
		  strcpy(cDebugStr, csRPDebugStr);
		  RPDEBUGSTRMFC(cDebugStr);

		  if ( !DeleteMonitor(NULL, mi2.pEnvironment, mi2.pName) )
		  {
			  dwErr = GetLastError();

			  GetSysError(csErr, "ReInstall Monitor");

			  csRPDebugStr.Format("DeleteMonitor Error(%d) = %s\n", dwErr, csErr);
			  strcpy(cDebugStr, csRPDebugStr);
			  RPDEBUGSTRMFC(cDebugStr);
		  }
	  }
   
   // add the monitor
   ret = AddMonitor(NULL, 2, (LPBYTE)&mi2);

   // if return is FALSE, check error, if the error
   // indicates that the monitor already exists,
   // force a TRUE return
   if (!ret)
      {
	  dwErr = GetLastError();
	  GetSysError(csErr, "Add Monitor");

	  if (dwErr == ERROR_ALREADY_EXISTS || dwErr == ERROR_PRINT_MONITOR_ALREADY_INSTALLED)
	     ret = TRUE;

	  if ( !ret )
	  {
		  if ( iKey == IDS_DRIVER_OUR_LANG_MONITOR )
			  csErrMsg.Format("Add %s(%s) Failed(%d)\n\n%s\n", csMonitorName, csMonitor, dwErr, csErr);
		  else
			  csErrMsg.Format("Add Port Monitor(%s) Failed(%d)\n%s\n", csMonitorName, dwErr, csErr);

		  strcpy(cDebugStr, csErrMsg);
		  RPDEBUGSTRMFC(cDebugStr);
	  }
	  else
	  {
		  if ( iKey == IDS_DRIVER_OUR_LANG_MONITOR )
			  csErrMsg.Format("Add %s(%s) Success(%d)\n\n%s\n", csMonitorName, csMonitor, dwErr, csErr);
		  else
			  csErrMsg.Format("Add Port Success(%s) Failed(%d)\n%s\n", csMonitorName, dwErr, csErr);

		   strcpy(cDebugStr, csRPDebugStr);
		   RPDEBUGSTRMFC(cDebugStr);
	  }

	  }
   return ret;
   }

// Note: a lot of the stuff below is guesswork...
BOOL CInstallPrinter::GetWinInstallFile(CString& csFileName, CString& csCopyDir)
   {
   int i;
   BOOL ret;     
   char pTemp[_MAX_PATH];   
   CFile cf;   
   CFileStatus cfs, cfsDst;
   int FileSize;      
   CString csLayoutFile;
   CString csTemp;   
   CString csSection, csEntry;   
   CString csDiskTitle, csCABFileName;
   CString csCABSrcStart, csCABSrc, csSrc, csDst;
   CString csPrompt1, csPrompt2;   
   CCabInstall ciDlg;
   CString MRUList;
   CStringArray csaInstallLocs;
   CString csNewInstallPath;   

   // first check to see if the file was already copied 
   // to the devinst directory
   csTemp = csCopyDir;
   csTemp += csFileName;
   // if it exists, return TRUE immediately
   if(cf.GetStatus(csTemp, cfs))
	   return TRUE;

   // get layout INI file name
   GetWindowsDirectory(csLayoutFile.GetBuffer(_MAX_PATH), _MAX_PATH);
   csLayoutFile.ReleaseBuffer();
   csTemp.LoadString(IDS_DEVICE_LAYOUT_DIR);
   csLayoutFile += csTemp;
   csTemp.LoadString(IDS_DEVICE_LAYOUT_FILE);
   csLayoutFile += csTemp;

   // get section
   // [SourceDisksFiles]
   csSection.LoadString(IDS_DEVICE_LAYOUT_SDF);
   csEntry = csFileName;
   // get the file info line
   // ex: pscript.drv=10,,393200,c09a
   //csTemp = Profile.GetStr(csLayoutFile, csSection, csEntry, "");
   ::GetPrivateProfileString(csSection, csEntry, "", 
	  csTemp.GetBuffer(2048), 2048, csLayoutFile);
   csTemp.ReleaseBuffer();
   strncpy(pTemp, (const char*)csTemp, _MAX_PATH);
   // get section
   // [SourceDisksName]
   csSection.LoadString(IDS_DEVICE_LAYOUT_SDN);
   // entry is disk id from previous profile call
   //strtok(pTemp, "=,");
   csEntry = strtok(pTemp, "=,");
   FileSize = atoi(strtok(NULL, "=,"));
   // get the cabinet info line
   // ex: 9="Windows 95 CD-ROM","win95_08.cab",0
   //csTemp = Profile.GetStr(csLayoutFile, csSection, csEntry, "");
   ::GetPrivateProfileString(csSection, csEntry, "", 
	  csTemp.GetBuffer(2048), 2048, csLayoutFile);
   csTemp.ReleaseBuffer();

   strncpy(pTemp, (const char*)csTemp, _MAX_PATH);
   // get disk name
   csDiskTitle = strtok(pTemp, "=\",");
   // get cab file name
   csCABFileName = strtok(NULL, "=\",");
   
   // first the simple case...the file exists on the MRU install directory
   //
   // get MRU install dir   
   csSection.LoadString(IDS_DEVICE_INSTALL_LOCS_MRU);   
   DWORD dwType;
   DWORD dwLength;
   HKEY hKey;

   RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const char*)csSection, 0, KEY_ALL_ACCESS, &hKey);	
   // 
   // get MRUList
   //
   csTemp.LoadString(IDS_DEVICE_INSTALL_LOCS_MRU_LIST);
   // get size of buffer
   RegQueryValueEx(hKey, csTemp, NULL, &dwType, NULL, &dwLength);
   // get value
   RegQueryValueEx(hKey, csTemp, NULL, &dwType, 
      (LPBYTE)MRUList.GetBuffer(dwLength), &dwLength);
   MRUList.ReleaseBuffer();	     

   // parse through the MRUList to develop a 
   // string array of install locations
   csaInstallLocs.RemoveAll();
   for (i = 0; i < MRUList.GetLength(); i++)
      {
	  pTemp[0] = MRUList[i];
	  pTemp[1] = '\0';
	  // get size of buffer
      RegQueryValueEx(hKey, pTemp, NULL, &dwType, NULL, &dwLength);
      // get value
      RegQueryValueEx(hKey, pTemp, NULL, &dwType, 
         (LPBYTE)csTemp.GetBuffer(dwLength), &dwLength);
      csTemp.ReleaseBuffer();	     
	  csaInstallLocs.Add(csTemp);
	  }
   
   // no install locations! - add a null one
   if (csaInstallLocs.GetSize() <= 0)
      csaInstallLocs.Add("");

   // load cabinet install prompts
   // The %s file on %s could not be found.
   csTemp.LoadString(IDS_CABINSTALL_PROMPT1);
   wsprintf(csPrompt1.GetBuffer(_MAX_PATH * 4), 
      (const char*)csTemp, (const char*)csFileName, (const char*)csDiskTitle);
   csPrompt1.ReleaseBuffer();
   //csTemp.LoadString(IDS_CABINSTALL_PROMPT2);
   //wsprintf(csPrompt2.GetBuffer(_MAX_PATH * 4), 
   //	     (const char*)csTemp, (const char*)csDiskTitle);
   //csPrompt2.ReleaseBuffer();	  
   csPrompt2.LoadString(IDS_CABINSTALL_PROMPT3);   

   // make source filename
   // use first install location
   csSrc = csaInstallLocs.GetAt(0);
   csSrc += csFileName;
   // make source cabinet
   // use first install location
   csCABSrc = csaInstallLocs.GetAt(0);
   csCABSrc += csCABFileName;   
   csCABSrcStart = csaInstallLocs.GetAt(0);
   // load starting cab file for extended search
   csTemp.LoadString(IDS_CABFILE_INSTALL_START);
   csCABSrcStart += csTemp;      

   // default is crap
   ret = FALSE;      

   // keep putting up a dialog prompting for path if
   // neither the raw file nor the cab file exists
   while(!cf.GetStatus(csSrc, cfs) && !cf.GetStatus(csCABSrc, cfs))
      {
	  ciDlg.m_prompt1 = csPrompt1;
	  ciDlg.m_prompt2 = csPrompt2;
	  ciDlg.pcsaInstallLocs = &csaInstallLocs;
	  // undo the wait cursor
	  AfxGetApp()->DoWaitCursor(-1);
	  // if cancel, quick break
	  if (ciDlg.DoModal() == IDCANCEL)     		 
	     {
		 RegCloseKey(hKey);
	     return FALSE;		 
		 }

	  // redo the wait cursor
	  AfxGetApp()->DoWaitCursor(1);
		 
	  // otherwise, load new path
	  csSrc = ciDlg.m_path;
      csSrc += csFileName;
      csCABSrc = ciDlg.m_path;
      csCABSrc += csCABFileName;   
	  csNewInstallPath = ciDlg.m_path;	  
	  // load starting cab file for extended search
      csTemp.LoadString(IDS_CABFILE_INSTALL_START);
	  csCABSrcStart = csNewInstallPath;
      csCABSrcStart += csTemp;      
      }	   

   // does the source file exist?
   if (cf.GetStatus(csSrc, cfs))
      {
	  // make the dest filename
	  csDst = csCopyDir;
	  csDst += csFileName;
	  // if the same, don't copy
	  if (cf.GetStatus(csDst, cfsDst) &&
	     (cfs.m_mtime == cfsDst.m_mtime) &&
	  	 (cfs.m_size == cfsDst.m_size))
	     ret = TRUE;
	  else
	     {
		 // copy it then
	     if (CopyFile((const char*)csSrc, (const char*)csDst, TRUE))
	        ret = TRUE;
		 }
	  }      			
   // does the cabinet exist there?      
   else if (cf.GetStatus(csCABSrc, cfs))
      {
	  csDst = csCopyDir;
	  csDst += csFileName;

	  // do a non-chain extract
	  if ((ret = ExtractCABFile(csCABSrc, csCopyDir, csFileName, FileSize, FALSE)) == FALSE)
	     {
		 // didn't work, try chain
		 ret = ExtractCABFile(csCABSrcStart, csCopyDir, csFileName, FileSize, TRUE);
		 }
	  }
   // do we need to modify the install path MRU List?
   if (csNewInstallPath.GetLength())
      {	  
	  char cNewMRULetter[2];	  
	  BOOL HaveIt = FALSE;

	  // do we already have it?
	  for (i = 0; i < csaInstallLocs.GetSize(); i++)
	     {	
	     // no-case compare  						
		 if (csNewInstallPath.CompareNoCase(csaInstallLocs.GetAt(i)) == 0)
		    {
			// re-do MRUList
			int j;
			int cnt = 0;
			strncpy(pTemp, MRUList, _MAX_PATH);			
			pTemp[cnt++] = MRUList[i];
			for (j = 0; j < i; j++)		
			   {			   
			   pTemp[cnt++] = MRUList[j];
			   }
			for (j = i + 1; j < csaInstallLocs.GetSize(); j++)		
			   {			   
			   pTemp[cnt++] = MRUList[j];
			   }			   
			HaveIt = TRUE;
			// set new MRU list
			csTemp.LoadString(IDS_DEVICE_INSTALL_LOCS_MRU_LIST);	        
	        RegSetValueEx(hKey, csTemp, NULL, REG_SZ, 
	           (CONST BYTE*)pTemp, strlen(pTemp));
	     							  
			break;
			}
		 }
	  if (!HaveIt)
	     {
		 // new letter
	     cNewMRULetter[0] = 'a' + MRUList.GetLength();
	     cNewMRULetter[1] = '\0';
	     // add it to list
	     csSrc = (char*)cNewMRULetter;
	     csSrc += MRUList;		 
	     // set new MRU list
		 csTemp.LoadString(IDS_DEVICE_INSTALL_LOCS_MRU_LIST);
	     RegSetValueEx(hKey, csTemp, NULL, REG_SZ, 
	        (CONST BYTE*)(const char*)csSrc, csSrc.GetLength());		 
	     // set new path value	  
	     RegSetValueEx(hKey, cNewMRULetter, NULL, REG_SZ, 
	        (CONST BYTE*)(const char*)csNewInstallPath, 
	        csNewInstallPath.GetLength());
		 }
      }
   RegCloseKey(hKey);
   return ret;
   }

BOOL CInstallPrinter::GetWinInstallFileNT(CString& csFileName, CString& csCopyDir)
   {   
   BOOL ret;        
   CFile cf;   
   CFileStatus cfs, cfsDst;   
   CString csTemp;      
   CString csSrc, csDst, csSrcExpanded;
   CString csPrompt1, csPrompt2;   
   CCabInstall ciDlg;      
   CStringArray csaInstallLocs;
   CProfile Profile;

   // load install prompts
   // The %s file on %s could not be found.
   csTemp.LoadString(IDS_NTINSTALL_PROMPT1);
   csPrompt1.Format(csTemp, (const char*)csFileName);
   csPrompt2.LoadString(IDS_NTINSTALL_PROMPT3);   

   // first check to see if the file was already copied 
   // to the devinst directory
   csTemp = csCopyDir;
   csTemp += csFileName;
   // if it exists, return TRUE immediately
   if(cf.GetStatus(csTemp, cfs)) return TRUE;

   // make source compressed filename   
   csTemp.LoadString(IDS_MRU_INSTALL_NT_DEF);
   csSrc = Profile.GetStr(IDS_DEVICES, IDS_MRU_INSTALL_NT, csTemp);
   csaInstallLocs.RemoveAll();
   csaInstallLocs.Add(csSrc);
   csSrcExpanded = csSrc;
   csSrcExpanded += csFileName;
   csSrc += csFileName.Left(csFileName.GetLength() - 1);
   csSrc += "_";

   // default is crap
   ret = FALSE;      

   // keep putting up a dialog prompting for path if
   // the compressed file is not there
   while(!cf.GetStatus(csSrc, cfs) && !cf.GetStatus(csSrcExpanded, cfs))	  
      {
	  ciDlg.m_prompt1 = csPrompt1;
	  ciDlg.m_prompt2 = csPrompt2;	  
	  ciDlg.pcsaInstallLocs = &csaInstallLocs;
	  // undo the wait cursor
	  AfxGetApp()->DoWaitCursor(-1);
	  // if cancel, quick break
	  if (ciDlg.DoModal() == IDCANCEL)     		 
	     {		 
	     return FALSE;		 
		 }

	  // redo the wait cursor
	  AfxGetApp()->DoWaitCursor(1);
		 
	  // otherwise, load new path
	  csSrc = ciDlg.m_path;
	  csSrcExpanded = csSrc;
	  csSrcExpanded += csFileName;
      csSrc += csFileName.Left(csFileName.GetLength() - 1);
	  csSrc += "_";
	  
	  // remember this
	  Profile.WriteStr(IDS_DEVICES, IDS_MRU_INSTALL_NT, ciDlg.m_path);
	  }      			   

   // just copy it
   if (cf.GetStatus(csSrcExpanded, cfs))
      {
	  csDst = csCopyDir;
	  csDst += csFileName;

	  // copy the file
	  ret = CopyFile(csSrcExpanded, csDst, FALSE);
	  }      
   // or expand it
   else if (cf.GetStatus(csSrc, cfs))
      {
	  csDst = csCopyDir;
	  csDst += csFileName;

	  // expand the file
	  ret = ExpandFile(csSrc, csDst);
	  }      
   return ret;
   }

#define WAITFORCOPYTOCOMPLETE 2 * 60 * 1000

BOOL CInstallPrinter::ExpandFile(CString& csSrc, CString& csDst)
   {
   STARTUPINFO StartupInfo;
   PROCESS_INFORMATION ProcessInfo;   
   char pTemp[_MAX_PATH];      
   char pTemp2[_MAX_PATH];         
   char pTemp3[_MAX_PATH];         
   BOOL ret = FALSE;
   CString csExpandExe, csExpandPif;   
   DWORD dwExitCode;
   CString csTemp;   
   CFile cf;
   CFileStatus cfs;
   
   // NULL the StartupInfo struct
   memset(&StartupInfo, 0, sizeof(STARTUPINFO));
   // the size
   StartupInfo.cb = sizeof(STARTUPINFO);
   // show the window
   StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
   StartupInfo.wShowWindow = SW_HIDE;
   // get the EXTRACT PIF file
   csExpandPif.LoadString(IDS_EXPAND_PIF);   
   // convert long paths to short!   
   GetShortPathName((const char*)csExpandPif, pTemp, _MAX_PATH);
   GetShortPathName((const char*)csSrc, pTemp2, _MAX_PATH);
   GetShortPathName((const char*)csDst, pTemp3, _MAX_PATH);
   csExpandExe.Format("%s %s %s", (const char*)pTemp, (const char*)pTemp2, (const char*)pTemp3);
   // make the extractor launch string
   strncpy(pTemp, (const char*)csExpandExe, _MAX_PATH);
   
   // start the extractor
   if (CreateProcess(NULL, pTemp, NULL, NULL, FALSE, 
      NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &ProcessInfo))
  	  {
	  HANDLE hProcess = ProcessInfo.hProcess;

	  // wait up to two minutes for completion
	  WaitForSingleObject(hProcess, WAITFORCOPYTOCOMPLETE);
   
	  // get exit code
	  GetExitCodeProcess(ProcessInfo.hProcess, &dwExitCode);
	  // still going?
      if (dwExitCode == STILL_ACTIVE)
	     {
	     // nuke it
	     TerminateProcess(ProcessInfo.hProcess, 0);
	     }
	  else
	     ret = TRUE;
	  // close the handles				
	  CloseHandle(ProcessInfo.hThread);
      CloseHandle(hProcess);	  
	  }
   if (ret)
      {
	  // only error checking we have right now is to 
	  // check for file existence 
	  if (cf.GetStatus(csDst, cfs))
	     ret = TRUE;
	  else
	     ret = FALSE;
	  }
   return ret;
   }

BOOL CInstallPrinter::ExtractCABFile(CString& csCABFile, CString& csCopyDir, 
   CString& csFile, int FileSize, BOOL ChainSearch)
   {
   STARTUPINFO StartupInfo;
   PROCESS_INFORMATION ProcessInfo;   
   char pTemp[_MAX_PATH];      
   char pTemp2[_MAX_PATH];      
   char pTemp3[_MAX_PATH];      
   BOOL ret = FALSE;
   CString csExtractExe, csExtractPif;   
   DWORD dwExitCode;
   CString csTemp;
   CProfile Profile;
   CFile cf;
   CFileStatus cfs;
   CString csDst;

   // NULL the StartupInfo struct
   memset(&StartupInfo, 0, sizeof(STARTUPINFO));
   // the size
   StartupInfo.cb = sizeof(STARTUPINFO);
   // show the window
   StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
   StartupInfo.wShowWindow = SW_HIDE;
   // get the EXTRACT PIF file
   csExtractPif = Profile.GetStr(IDS_DEVICES, IDS_EXTRACT_PIF);
   // get the extract wsprintf line
   csTemp.LoadString(ChainSearch ? IDS_DEVICE_EXTRACTEXE_CHAIN : IDS_DEVICE_EXTRACTEXE);
   // create the string
   //csExtractExe.Format((const char*)csTemp, (const char*)csExtractPif, 
   //	     (const char*)csCopyDir, (const char*)csCABFile, (const char*)csFile);	  
   // convert long paths to short!   
   GetShortPathName((const char*)csExtractPif, pTemp, _MAX_PATH);
   GetShortPathName((const char*)csCopyDir, pTemp2, _MAX_PATH);
   GetShortPathName((const char*)csCABFile, pTemp3, _MAX_PATH);
   csExtractExe.Format((const char*)csTemp, pTemp, pTemp2, pTemp3, (const char*)csFile);	  
   // make the extractor launch string
   strncpy(pTemp, (const char*)csExtractExe, _MAX_PATH);
   
   // start the extractor
   if (CreateProcess(NULL, pTemp, NULL, NULL, FALSE, 
      NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &ProcessInfo))
  	  {
	  HANDLE hProcess = ProcessInfo.hProcess;

	  // wait up to two minutes for completion
	  WaitForSingleObject(hProcess, WAITFORCOPYTOCOMPLETE);
   
	  // get exit code
	  GetExitCodeProcess(ProcessInfo.hProcess, &dwExitCode);
	  // still going?
      if (dwExitCode == STILL_ACTIVE)
	     {
	     // nuke it
	     TerminateProcess(ProcessInfo.hProcess, 0);
	     }
	  else
	     ret = TRUE;
	  // close the handles				
	  CloseHandle(ProcessInfo.hThread);
      CloseHandle(hProcess);	  
	  }
   if (ret)
      {
	  // only error checking we have right now is to 
	  // check for file existence and size
	  csDst = csCopyDir;
	  csDst += csFile;
	  if (cf.GetStatus(csDst, cfs) && (cfs.m_size == (LONG)FileSize))
	     ret = TRUE;
	  else
	     ret = FALSE;
	  }
   return ret;
   } 

void CInstallPrinter::HackOffReadOnlyFlags(CString& csFile)
   {
   CFile cf;
   CFileStatus cfs;   

   // get status
   cf.GetStatus(csFile, cfs);
   if (cfs.m_attribute & CFile::readOnly)
	  {
      // hack off att
      cfs.m_attribute -= CFile::readOnly;
      // set it
      cf.SetStatus(csFile, cfs);
	  }
   }

void CInstallPrinter::SetOurPort(CString& csDev, CString& csPort)
   {      
   HANDLE hPrinter;   
   PRINTER_INFO_2* pi2;
   DWORD cbNeeded;
   LPBYTE lpBuf;   

   PRINTER_DEFAULTS pd;
   pd.pDatatype = NULL;
   pd.pDevMode = NULL;
   pd.DesiredAccess = PRINTER_ALL_ACCESS;
   
   // must tell windows!!!
   if (::OpenPrinter((char*)(const char*)csDev, &hPrinter, &pd))
      {
   	  if (hPrinter) 
	     {
	  	 ::GetPrinter(hPrinter, 2, NULL, 0, &cbNeeded);
		 // make the buffer equal to the old plus the new port info
		 lpBuf = (LPBYTE)(new char[cbNeeded + csPort.GetLength() + 1]);
		 if (::GetPrinter(hPrinter, 2, lpBuf, cbNeeded, &cbNeeded))
		    {
		    // copy the new port
		    strcpy((char*)(lpBuf + cbNeeded), (const char*)csPort);
		    // point to the buffer
		    pi2 = (PRINTER_INFO_2*)lpBuf;
		    // set the new port	
		    pi2->pPortName = (LPSTR)(lpBuf + cbNeeded);
		    // do it
		    ::SetPrinter(hPrinter, 2, lpBuf, 0);
		    }
		 delete lpBuf;
		 ::ClosePrinter(hPrinter);
		 }	  
      }
   }

// from MSDN, however, it had a major bug in it
// i also added code to make sure we were using 8.3 fnames
BOOL CInstallPrinter::ReplaceFileOnReboot (LPCTSTR pszExisting, LPCTSTR pszNew) 
   {
   char szRename[1024];   
   char pszNewShort[_MAX_PATH];  
   char pszExistingShort[_MAX_PATH];  

   // make a temp filename
   if (!MakeTempRebootFile(pszNew, szRename)) return FALSE;

   // First, attempt to use the MoveFileEx function.
   BOOL fOk = MoveFileEx(pszExisting, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
   if (fOk) fOk = MoveFileEx(szRename, pszExisting, MOVEFILE_DELAY_UNTIL_REBOOT);
   if (fOk) return(fOk);

   // If MoveFileEx failed, we are running on Windows 95 and need to add
   // entries to the WININIT.INI file (an ANSI file).
   
   // convert to short path name   
   GetShortPathName(szRename, pszNewShort, _MAX_PATH);
   GetShortPathName(pszExisting, pszExistingShort, _MAX_PATH);

   int cchRenameLine = wsprintfA(szRename, 
      "%hs=%hs\r\n", 
	  //(pszNew == NULL) ? "NUL" : pszNewShort, pszExisting);  bug!!!
      (pszExisting == NULL) ? "NUL" : pszExistingShort, pszNewShort);
      char szRenameSec[] = "[Rename]\r\n";
      int cchRenameSec = sizeof(szRenameSec) - 1;
      HANDLE hfile, hfilemap;
      DWORD dwFileSize, dwRenameLinePos;
      TCHAR szPathnameWinInit[_MAX_PATH];

      // Construct the full pathname of the WININIT.INI file.
      GetWindowsDirectory(szPathnameWinInit, _MAX_PATH);
      lstrcat(szPathnameWinInit, "\\WinInit.Ini");

      // Open/Create the WININIT.INI file.
      hfile = CreateFile(szPathnameWinInit,      
         GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 
         FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

      if (hfile == INVALID_HANDLE_VALUE) 
         return(fOk);

      // Create a file mapping object that is the current size of 
      // the WININIT.INI file plus the length of the additional string
      // that we're about to insert into it plus the length of the section
      // header (which we might have to add).
      dwFileSize = GetFileSize(hfile, NULL);
      hfilemap = CreateFileMapping(hfile, NULL, PAGE_READWRITE, 0, 
         dwFileSize + cchRenameLine + cchRenameSec, NULL);

      if (hfilemap != NULL) {

         // Map the WININIT.INI file into memory.  Note: The contents 
         // of WININIT.INI are always ANSI; never Unicode.
         LPSTR pszWinInit = (LPSTR) MapViewOfFile(hfilemap, 
            FILE_MAP_WRITE, 0, 0, 0);

         if (pszWinInit != NULL) {

            // Search for the [Rename] section in the file.
            LPSTR pszRenameSecInFile = strstr(pszWinInit, szRenameSec);

            if (pszRenameSecInFile == NULL) {

               // There is no [Rename] section in the WININIT.INI file.
               // We must add the section too.
               dwFileSize += wsprintfA(&pszWinInit[dwFileSize], "%s", szRenameSec);
               dwRenameLinePos = dwFileSize;

            } else {

               // We found the [Rename] section, shift all the lines down
               PSTR pszFirstRenameLine = strchr(pszRenameSecInFile, '\n');
               // Shift the contents of the file down to make room for 
               // the newly added line.  The new line is always added 
               // to the top of the list.
               pszFirstRenameLine++;   // 1st char on the next line
               memmove(pszFirstRenameLine + cchRenameLine, pszFirstRenameLine, 
                  pszWinInit + dwFileSize - pszFirstRenameLine);                  
               dwRenameLinePos = pszFirstRenameLine - pszWinInit;
            }

            // Insert the new line
            memcpy(&pszWinInit[dwRenameLinePos], szRename,cchRenameLine);

            UnmapViewOfFile(pszWinInit);

            // Calculate the true, new size of the file.
            dwFileSize += cchRenameLine;

            // Everything was successful.
            fOk = TRUE; 
         }
         CloseHandle(hfilemap);
      }

      // Force the end of the file to be the calculated, new size.
      SetFilePointer(hfile, dwFileSize, NULL, FILE_BEGIN);
      SetEndOfFile(hfile);

      CloseHandle(hfile);
   

   return(fOk);
   }

BOOL CInstallPrinter::MakeTempRebootFile(LPCTSTR csSrc, char* pFileName) 
   {
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   CString csNewPath;
   
   // get path to original file
   _splitpath(csSrc, drive, dir, NULL, NULL);

   // use to construct temp filename
   csNewPath = drive;
   csNewPath += dir;

   if (GetTempFileName(csNewPath, "RPI", 0, pFileName))
	  {
	  // copy the file
	  CopyFile(csSrc, pFileName, FALSE);
	  return TRUE;
	  }
   else
	  return FALSE;
   }

// a hack for NT which doesn't seem to set the default page size
// in the app print driver (note that this uses a DEVMODE field
// which is only supported in NT
void CInstallPrinter::SetDefaultNTPageSize(CString& csName, CString& csPort, 
										 CString& csQueueMediaName)
   {   
   HANDLE hPrinter;      
   DEVMODE* pDevMode;
   int iDevSize = 0;
   PRINTER_DEFAULTS pd;
   pd.pDatatype = NULL;
   pd.pDevMode = NULL;
   pd.DesiredAccess = PRINTER_ALL_ACCESS;
   PRINTER_INFO_2* pi2;
   DWORD cbNeeded;
   LPBYTE lpBuf;  
   
   // open the printer
   if (::OpenPrinter((char*)(const char*)csName, &hPrinter, &pd))
	  {
      if (hPrinter) 
	     {
		 // get size
		 iDevSize = ::DocumentProperties(AfxGetMainWnd()->GetSafeHwnd(), hPrinter, 
			(char*)(const char*)csName, NULL, NULL, 0);	
		 
		 if (iDevSize > 0)
			{
			// do properties
			if (pDevMode = (PDEVMODE)(new BYTE [iDevSize]))
			   {
			   // get the current devmode
			   ::DocumentProperties(AfxGetMainWnd()->GetSafeHwnd(), hPrinter, (char*)(const char*)csName, 
			      pDevMode, NULL, DM_OUT_BUFFER);
			   // now set the default paper size
			   pDevMode->dmFields = DM_FORMNAME;

			   strcpy((char*)pDevMode->dmFormName, csQueueMediaName);
			   // set the new devmode
			   ::DocumentProperties(AfxGetMainWnd()->GetSafeHwnd(), hPrinter, (char*)(const char*)csName, 
			      NULL, pDevMode, DM_IN_BUFFER);
 			   
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
					 pi2->pDevMode = pDevMode;
					 // do it
					 ::SetPrinter(hPrinter, 2, lpBuf, 0);
					 }
				  delete lpBuf;
			   }
   		       delete pDevMode;			
			   }
			}
		 }
	  if (hPrinter) ::ClosePrinter(hPrinter);	
	  }  												
   }

BOOL CInstallPrinter::CreateSPDNames(CString& csDevName)
{
	BOOL bFileFound = FALSE;

	char pTempFileName[_MAX_PATH];

	CFileFind fileFind;

	CProfile Profile;
   
	CString csTemp;
	CString csTestSPDName;
	CString csUniqueSPDName;
	CString csUniqueSPDName_NT;

	int iDotLoc;
	int iDotUnique;
	int iSlashUnique;

	// Get the default names from the Registry
	//
	if ( csDevName.Left(3) == "RP-" )
	{
		csSPDName		= "RP-.spd";
		csSPDName_NT	= "RP-.ppd";
	}
	else
	{
		csTemp.LoadString(IDS_SPD_DLLNAME);   
		csSPDName = Profile.GetStr(csDevName, csTemp);

		csTemp.LoadString(IDS_SPD_DLLNAME_NT);   
		csSPDName_NT = Profile.GetStr(csDevName, csTemp);
	}

   csRPDebugStr.Format("csSPDName(NOT NT FAMILY) = %s\n", csSPDName);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr);

   csRPDebugStr.Format("csSPDName_NT(NT FAMILY) = %s\n", csSPDName_NT);
   strcpy(cDebugStr, csRPDebugStr);
   RPDEBUGSTRMFC(cDebugStr);

	return TRUE;

	if (!IS_OS_NT_FAMILY)
	{
		bFileFound = fileFind.FindFile(csDevDir + csSPDName);
		csTestSPDName = csSPDName;
	}
	else
	{
		bFileFound = fileFind.FindFile(csDevDir + csSPDName_NT);
		csTestSPDName = csSPDName_NT;
	}

	if ( GetTempFileName(csDevDir, csTestSPDName.Left(3), 0, pTempFileName) )
	  ::DeleteFile(pTempFileName);

	csUniqueSPDName = pTempFileName;

	iDotUnique		= csUniqueSPDName.ReverseFind('.');
	iSlashUnique	= csUniqueSPDName.ReverseFind('\\');

	iDotLoc	= csSPDName.ReverseFind('.');
	csSPDName = csUniqueSPDName.Mid(iSlashUnique + 1, iDotUnique - iSlashUnique) +
										csSPDName.Right(csSPDName.GetLength() - (iDotLoc+1));

	iDotLoc = csSPDName_NT.ReverseFind('.');
	csSPDName_NT = csUniqueSPDName.Mid(iSlashUnique + 1, iDotUnique - iSlashUnique) +
										csSPDName_NT.Right(csSPDName_NT.GetLength() - (iDotLoc+1));

	return TRUE;
}

void CInstallPrinter::UpdateCrossPlatformInfo(int iInstallDir, CString csDevName, CString csSrcPPDName, CString csShortName, CString csSPDName)
{
	CProfile Profile;

	CString csDestPPDName;
	CString csTemp;

	// get the devinst directory for 95
	csDestPPDName = Profile.GetStr(IDS_DEVICES, iInstallDir);
	VERIFYPATH(csDestPPDName);
	csDestPPDName += csSPDName;

	CopyFile(csSrcPPDName, csDestPPDName, FALSE);

	csDestPPDName = Profile.GetStr(IDS_DEVICES, iInstallDir) + "\\" + csShortName;
	VERIFYPATH(csDestPPDName);

	CreateDirectory( csDestPPDName, NULL );

	// put them together	
	csDestPPDName += csSPDName;

	// now copy the spd file to the devinst dir   
	CopyFile(csSrcPPDName, csDestPPDName, FALSE);
	// record this file so we can delete it
	csTemp = csDevName + "\\" + csShortName;
	((CZeusApp*)AfxGetApp())->RecordPPDFile(csTemp, csDestPPDName, 2);
}

void CInstallPrinter::InfFileUpdate(CString& csMfg, CString& csShortName, CString& csSPDName_NT, CString& csXPINFKey)
{
	CProfile Profile;

	CString csINFININame;
	CString csInfDev;
	CString csSPDINF1;
	CString csSPDINF2;
	CString csCopyDataSection;
	CString csCopyData;

	csINFININame = Profile.GetStr(IDS_DEVICES, csXPINFKey);   
	csInfDev = "\"";
	csInfDev += csShortName;
	csInfDev += "\"";
	// note that csSPDName is the NT version here
	::WritePrivateProfileString(csMfg, csInfDev, csSPDName_NT, csINFININame);		       

	csCopyDataSection = csShortName;
	csCopyDataSection += "_DATA";

	csSPDINF1 = csSPDName_NT;
	csSPDINF2.Format("@%s,PSCRIPT", csSPDINF1);   
	::WritePrivateProfileString(csSPDName_NT, "CopyFiles", csSPDINF2, csINFININame);		       
	::WritePrivateProfileString(csSPDName_NT, "DataSection", "PSCRIPT_DATA", csINFININame);
}
