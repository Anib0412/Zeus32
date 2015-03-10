/////////////////////////////////////////////////////////////////////////////
// CRIP.CPP
//                                                                        
// RIP class for RasterPlus
// Copyright (C) 1996-2002 Graphx, Inc.
//
// mods:
//
// 7-09-96  Build 40 - ViewPerspective was getting set After its use in
//					      XFORM stuff - fixed
// 7-09-96  Build 40 - GetSetOptimalBoundingBox gets PCDres idx so we can put
//				          it into JobInfo for passing to bitmap section of RIPPROC
// 7-09-96  Build 40 - always do alteration of transform based on ViewPerspective 
//				          even on non-preview; old stuff caused output to device to
//						  be transformed incorrectly
// 7-17-96  Build 40 - moved RipStruct.Rotation setting 
// 7-25-96  Build 40 - put a limit on the timeout waiting for the rip thread to
//                        close in CRip::Close(), if the timeout is hit, we terminate
//                        the thread with extreme prejudice.  (might be some kind of 
//                        messaging deadlock
// 11-25-96 Build 46 - changed RemoveFile to DeleteFile
// 6-09-97  Build 50 - added PicaHeight to account for non square pixels
// 6-11-97  Build 50 - added other stuff for non-square resolutions
// 9-17-97  Build 51 - always force RipStruct.DoColorTransfer to FALSE on preview
// 10-22-97 Build 53 - check for existence and proper file size of each file we rip
// 11-20-97 Version 2.0 - new size and existence checking clobbered BatchHeader 
//                           printing - fixed
//                        bug in VC 5.0 caused transform rotation problems - applied vc
//                           patch 2 - fixed
// 1-22-98  Version 2.0.1 - added NT-specific DLL specs
// 3-9-98   Version 2.0.2 - fixed None transforms for bitmaps
// 5-8-98   Version 2.0.3J - we treated EPSF-2 files differently during auto
//                           rotate for reasons which I don't remember.  Removing
//                           this seems to work fine with all EPSF files tested
// 9-10-98  Version 2.0.5 - added new device rip loading code (disabled for now)
// 11-23-98 Version 2.0.5 - if background in bheader is black, use white text and lines
// 1-13-99  Version 2.0.6 - dynamically get rip temp path
// 4-12-99  Version 2.0.6 - new OptimizeBitmapOutput logic
// 7-09-99  Version 3.0.0 - selectable scratch disk
// 8-08-99  Version 3.0.0 - optimize orientation on CanDoOrientation devices
// 9-16-99  Version 3.0.1 - fixed carriage return problem in rip status window
// 10-18-99 Version 3.0.1 - optimize orientation not working properly - fixed
// 10-18-99 Version 3.0.1 - not processing PDF files properly - fixed
// 10-29-99 Version 3.0.1 - increased time-out delays for closing rip
// 11-30-99 Version 3.0.2 - smoother cancelling
// 12-01-99 Version 3.0.2 - more canceling fixes
// 12-08-99 Version 3.0.2 - faster/more efficient rip close
// 1-12-00  Version 3.0.3 - disable preview ICC if printing
// 2-16-00  Version 3.0.3 - weirdly dated files caused CFileStatus to ASSERT - 
//                          changed to FindFirstFile
// 2-25-00  Version 3.0.3 - Added code to support a Default ICC Printer PProfile
// 3-03-00  Version 3.0.3 - Added invalid profile to the ErrMsgLookup table
// 3-24-00  Version 3.0.3 - Added check for TranTyep to determine ColorTransfer for a Preview
// 5-31-00  Version 4.0.0 - Fix Preview output - Force Printer psce to RGB for a Preview
// 10-20-00 Version 4.0.1 - only check for CanOptimizeOrientation, not CanDoOrientation
// 11-02-00 Version 4.0.1 - rulers on package preview not displaying properly - fixed
// 04-26-01 Version 4.0.3 - Don't force package orientation to Portrait
// 08-20-01 Version 5.0.0 - Add code for Unsupported file message
// 08-23-01 Version 5.0.0 - Disable check dongle code - moved to individual drivers
// 08-31-01 Version 5.0.0 - Fix Demo requesting Dongle - force HasDongle to TRUE
// 09-28-01 Version 5.0.0 - Changes to support High_Defintion Preview
//                          Support for Photogize Order as part of JobInfo
// 11-06-01 Version 5.0.0 - reorganize temp path stuff
// 11-30-01 Version 5.0.1 - Remove Sentinel Dongle Code
// 01-11-01 Version 5.0.1 - assign a default bitmap sample
// 01-29-02 Version 5.0.2 - Pass the SharedMemoryName in the RipStruct
//                          Initialize new files in JobInfo
// 06-11-02 Version 5.0.6 - Change Close() to minimize 5 second delay between images ( Improves Throughput)
// 08-14-02 Version 5.0.7 - Verify that the device will support QuickPrintImage and set appropraite value
// 08-15-02 Version 5.0.7 - Verify Job FileType is acceptable as a QuickPrint File by the device
// 10-10-02 Version 5.0.7 - Add EXIF to valid Quick Print file types
// 03-17-03 Version 5.1.1 - Force Transform to use the Crop Style of the largest frame when printing a package
// 05-23-03 Version 5.1.7 - Devices without a default Printer Profile would not use the specified profile - Fixed
// 01-09-04 Version 5.2.3 - RasterPlus hanging when processing multiple queues simultaneously - Fixed
//

#include"stdafx.h"
#include"zeus.h"
#include"stdlib.h"
#include"math.h"
#include"string.h"
#include "crip.h"            
#include "xfrmdefs.h"
#include "inidefs.h"
#include "mainfrm.h"
#include "batch.h"
#include "bheader.h"
#include "profile.h"
#include "ripitb.h"
#include "ripitj.h"

#include "userfile.h"
//#include "..\CmnLibs\Current LeadTools\include\Classlib\LtWrappr.h"

const UINT NEAR wm_RipComplete = RegisterWindowMessage( RIP_COMPLETE );

// the rip thread
DWORD _cdecl RipThreadProc(LPVOID pParam);

// Used to determine if the image size is within a range for
// printing directly to a Noritsu Device
#define ROUNDING_FACTOR 15

#define CRIP_FILETYPE_NONE      0
#define CRIP_FILETYPE_PSCRIPT   1
#define CRIP_FILETYPE_BITMAP    2

CRip::CRip()
   {
   pCQueueFile = NULL;   
   ThreadStart = FALSE;    
   IsInit = FALSE;   
   pRip = NULL;
   LastFileType = CRIP_FILETYPE_NONE;
   RipLoaded = FALSE;
   JawsTempPathCreated = FALSE;
   iIPCSlot = -1;
   pRipThread = NULL;

   // get the default value from the registry
   // this might get changed later
   csScratchPath = ((CZeusApp*)AfxGetApp())->GetScratchFolder();
   }

void CRip::LaunchRip()
   {
   CString csBase;
   CString csTemp;   
   CProfile Profile;
   CString csFontPath;
   CString csResPath, csLibPath, csPathFormat, csPaths;	     

   // create a new rip process
   pCProcess = new CRipProcess;

   // get next available IPC slot
   iIPCSlot = ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetAvailIPCSlot();
   
   if (iIPCSlot != -1)
      {
	  CString csApp, csDLL;

	  // make base name
      csTemp.LoadString(IDS_IPCBASENAME);
      csBase.Format("%s%d", (const char*)csTemp, iIPCSlot);	   
      // start it	  
#ifdef _DEBUG           
	  csApp = Profile.GetStr("Debug", "Root", "f:\\source\\");	  
	  if (!IS_OS_NT_FAMILY)
         csApp += Profile.GetStr(IDS_NEWRIPSECTION, IDS_RIPPROCAPP_DEBUG);
	  else
		 csApp += Profile.GetStr(IDS_NEWRIPSECTION, IDS_RIPPROCAPP_DEBUG_NT);

      csApp = Profile.GetStr(IDS_NEWRIPSECTION, IDS_RIPPROCAPP);

#else
      csApp = Profile.GetStr(IDS_NEWRIPSECTION, IDS_RIPPROCAPP);
#endif
	  csDLL = Profile.GetStr(IDS_NEWRIPSECTION, IDS_RIP_DLL);

	  // get paths
	  csFontPath = Profile.GetStr(IDS_NEWRIPSECTION, IDS_NEWRIP_FONTPATH);
	  csResPath = Profile.GetStr(IDS_NEWRIPSECTION, IDS_NEWRIP_RESPATH);
	  csLibPath = Profile.GetStr(IDS_NEWRIPSECTION, IDS_NEWRIP_LIBPATH);
	  csPathFormat.LoadString(IDS_NEWRIP_PFORMAT);

	  // create a directory off the temp directory using the ripproc id	  
	  // so that jaws temp files don't collide?
	  csTempJawsScrathPath = csScratchPath;
	  csTempJawsScrathPath += csBase;
	  csTempJawsScrathPath += "\\";
	  ::CreateDirectory(csTempJawsScrathPath, NULL);
	  JawsTempPathCreated = TRUE;
	   
	  // make path string
	  csPaths.Format((LPCSTR)csPathFormat, 
	     (LPCSTR)csFontPath, (LPCSTR)csResPath,
		 (LPCSTR)csLibPath, (LPCSTR)csTempJawsScrathPath);	  

#ifdef _DEBUG
      if (pCProcess->OpenProcess(csBase, csApp, FALSE /* don't hide window */))
#else
      if (pCProcess->OpenProcess(csBase, csApp, TRUE))
#endif
	  {         
	     if (pCProcess->InitRIP(csDLL, csPaths))
		 {
		   RipLoaded = TRUE;
		 }

//	   RipLoaded = TRUE;
	  }
	  }
   }

CRip::~CRip()
   {    
	if (pRip)
	{
	   delete pRip;
	}
   
   // close rip
   if (RipLoaded) 
      {
	  pCProcess->CloseRIP();
      // close process    
      pCProcess->CloseProcess();

      // delete the process object
      delete pCProcess;  
	  
      // delete files in the temp directory
	  if (JawsTempPathCreated)
		 {
         WIN32_FIND_DATA FileInfo;
         CString csFileName;
         CString csSearchPath;
         HANDLE hFile;

         csSearchPath = csTempJawsScrathPath;
         csSearchPath += "*.*";
   
         // check for existence
         if ((hFile = FindFirstFile(csSearchPath, &FileInfo)) != INVALID_HANDLE_VALUE)
			{  
	        // look at archive files only (we know that JAWS files are these
	        if (FileInfo.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
			   {
	           // add path    
	           csFileName = csTempJawsScrathPath;
               // add file name
               csFileName += FileInfo.cFileName;
	           // delete the file
		       ::DeleteFile(csFileName);		 
			   }
	        while (FindNextFile(hFile, &FileInfo))
			   {
		       // look at archive files only (we know that JAWS files are these
 	           if (FileInfo.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
				  {
			      // add path    
	              csFileName = csTempJawsScrathPath;
                  // add file name
                  csFileName += FileInfo.cFileName;
	              // delete the file
			      ::DeleteFile(csFileName);
				  }
			   }
		 	     	  
            // close the handle
            FindClose(hFile);   	  
			}
		 ::RemoveDirectory(csTempJawsScrathPath);
		 }
	  }

   // free the slot
   if (iIPCSlot != -1)
      ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->FreeIPCSlot(iIPCSlot);
   
#ifdef LEADFILE_PS
//   	LBase::UnloadLibraries(LT_ALL_LEADLIB);
#endif
   }                 
   
void CRip::Open()
{   
	// launch if necessary
	if (!RipLoaded)
	{
		LaunchRip();
	}

	pCQueueFile = NULL;   
	ThreadStart = FALSE; 
	IsInit = TRUE;      
}

void CRip::Close()
{
	DWORD dwRet = 0;
	unsigned long ulLastError = 0;

/*
	// delete our temp file if a blist rip
	if (pCQueueFile)
	{
		if (pCQueueFile->FileType == FTYPE_BLIST)     
			::DeleteFile(pCQueueFile->LongName);
	}     
*/
	if ( !csBatchHdrFilename.IsEmpty() )
		::DeleteFile(pCQueueFile->LongName);

	// are we in thread?           
	if(ThreadStart)
	{
		BOOL bExitCode;
		BOOL bGotMsg = FALSE;

		int LastTickCount;

		MSG msg;

		unsigned long iExitCode = 0;

		// Check for an ExitCode
		// If this fails the Thread is probably Closed (Invalid Handle)
		// Success = NonZero
		bExitCode = GetExitCodeThread(pRipThread->m_hThread, &iExitCode);

		// Check Invalid Handle (6) - Thread is gone
		// Check iExitCode (0) - Successful end
		if ( bExitCode == 0 )
		{
			ulLastError = GetLastError();
		}

		if ( (bExitCode == 0 && (ulLastError != 6 && ulLastError != 0)) ||
					( bExitCode != 0 && (iExitCode != 0 && iExitCode != STATUS_PENDING)))
		{
			dwRet = WaitForSingleObject(pRipThread->m_hThread, 0);
			if (dwRet != WAIT_OBJECT_0)	  
//			if (WaitForSingleObject(pRipThread->m_hThread, 0) != WAIT_OBJECT_0)	  
			{
				// Retry for 2 seconds to find rip complete message
				LastTickCount = ::GetTickCount();
				do
				{
					if (PeekMessage (&msg, JobInfo.hFormWin, 0, 0, PM_REMOVE)) 
					{
						// got it
						if (msg.message == wm_RipComplete) 				  
						{
//							TRACE("Peeked at wm_RipComplete message\n");
							bGotMsg = TRUE;
						}
						TranslateMessage (&msg);
						DispatchMessage (&msg);
					}

					Sleep(500);

				} while (((::GetTickCount() -  LastTickCount) < 3000) && !bGotMsg);	

				// still not there - terminate with extreme prejudice
				if (!bGotMsg)
				{
					// one last check!
					dwRet = WaitForSingleObject(pRipThread->m_hThread, 1000);

					if (dwRet == WAIT_TIMEOUT)	  
					{
						if ( TerminateThread(pRipThread->m_hThread, 0) != 0)
						{
							TRACE("**** Terminating Rip Thread - Successful\n");
						}
						else
						{
							// Indicates the Thread was probably already gone.
							TRACE("**** Terminating Rip Thread - Failed\n");
						}
					}
				}
			}
		}
	}

	IsInit = FALSE;
}  
  
int CRip::Init(CDevice* pDevice, CTransform* pct, CColorTransform* pCC,
   CQueueFile* pCQ, int OutputType, CWnd* cwReport, CWnd* cwPreview, 
   BOOL ForceOffPackage, BOOL bDumpGangPrints)
   {           
   CProfile Profile;   
   CColorTransform* pColorTemp;
   int iPassCnt = 0;
   int iRetCode = SYN_ERRMSG_ALLOK;
   int iOurDeviceColorModel;
   L_INT iRet = 0;

   CString csError;

ATSTART:
   iPassCnt++;

#ifdef LEADFILE_PS
   return LeadOnlyInit(pDevice, pct, pCC, pCQ, OutputType, cwReport, cwPreview, 
					ForceOffPackage, bDumpGangPrints);
#endif

   // alloc the appropriate rip object
   if ((pCQ->FileType == FTYPE_PSCRIPT) || (pCQ->FileType == FTYPE_BLIST) || 
	  (pCQ->FileType == FTYPE_PDF))
      {	  
	  switch(LastFileType)
	     {
		 case CRIP_FILETYPE_PSCRIPT:
		      // do nothing
			 if ( pRip )
			 {
			  csRPDebugStr.Format("Alloc Rip Object = %s\n", "Should delete?");
			 }
		      break;	     
	     case CRIP_FILETYPE_BITMAP:
		      // last one was a bitmap, delete it
			 if ( pRip )
			 {
			  csRPDebugStr.Format("Alloc Rip Object = %s\n", "delete");
			 }
		      delete pRip;	  
			  // drop through			  
	     case CRIP_FILETYPE_NONE:
		      // allocate a new one
			 if ( pRip )
			 {
			  csRPDebugStr.Format("Alloc Rip Object = %s\n", "A new RipItJ");
			 }
			  pRip = new CRipItJ;
			  LastFileType = CRIP_FILETYPE_PSCRIPT;
			  break;
		 }	  
	  }
   else
      {
 	  switch(LastFileType)
	     {
		 case CRIP_FILETYPE_BITMAP:		 
		      // do nothing
			 if ( pRip )
			 {
			  csRPDebugStr.Format("Alloc Rip Object Bitmmap= %s\n", "Should delete?");
			 }
		      break;	     
	     case CRIP_FILETYPE_PSCRIPT:
		      // last one was a pscript, delete it
			 if ( pRip )
			 {
			  csRPDebugStr.Format("Alloc Rip Object Bitmmap= %s\n", "delete");
			 }
		      delete pRip;	  
			  // drop through			  
	     case CRIP_FILETYPE_NONE:
		      // allocate a new one
			  //pRip = new CBitmapRip;
			 if ( pRip )
			 {
			  csRPDebugStr.Format("Alloc Rip Object Bitmap = %s\n", "A New RipItB");
			 }
			  pRip = new CRipItB;
			  LastFileType = CRIP_FILETYPE_BITMAP;
			  break;
		 }
	  }

   // store pointer to process
   pRip->pCProcess = pCProcess;

   // check for the existence of the file if not a BatchHeader
   if (pCQ->FileType != FTYPE_BLIST)
	  {
	  HANDLE hFile;
	  WIN32_FIND_DATA FileInfo;
      if ((hFile = FindFirstFile(pCQ->LongName, 
         &FileInfo)) != INVALID_HANDLE_VALUE)
		 {      
		 // close the handle
		 FindClose(hFile);   
		 }
	  else
		 {
	     pCQ->RealFileSize = pCQ->iFileSize = 0;
	     return SYN_ERRMSG_FILENOTFOUND;
		 }

      // update the file size if if pCQ->RealFileSize == pCQ->iFileSize   
      if (pCQ->RealFileSize == pCQ->iFileSize)
		 {
	     pCQ->RealFileSize = pCQ->iFileSize = FileInfo.nFileSizeLow;
		 }
      // if pCQ->RealFileSize != pCQ->iFileSize and iFileSize has changed then 
      // we have (had) an EPS file and we need to reget everything
      else if (pCQ->RealFileSize != pCQ->iFileSize && pCQ->iFileSize != FileInfo.nFileSizeLow)
		 {
         CStdioFile csfCheckFile;

         // open the file
         if (csfCheckFile.Open(pCQ->LongName, 
		    CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone))
			{
            pCQ->CheckForPostScript(csfCheckFile);
            csfCheckFile.Close();
			}
		 }
	  }

   // load initial params                                          
   pCQueueFile = pCQ;
   //BoundingBox = pCQueueFile->BoundingBox;   
   EPSF = pCQueueFile->EPSF;
   ExtraInfo = pCQueueFile->ExtraInfo;
   Transform = *pct;       

   // store pointer to the jobinfo
   pRip->pJobInfo = &JobInfo;
   
   // init the rip
   if (!pRip->Init(pDevice, OutputType))
      return(SYN_ERRMSG_RIPNOTFOUND);

   // zero this out
   memset(&RipStruct, 0, sizeof RIPSTRUCT);

   strcpy(RipStruct.SharedMemoryName, pDevice->csFriendlyName);

   // ripstruct with bounding box in case it is needed for
   // packagizing
   if (pCQueueFile)
	  {
	  RipStruct.PixWidth = abs(pCQueueFile->BoundingBox.Width());
      RipStruct.PixHeight = abs(pCQueueFile->BoundingBox.Height());
	  }

   // This will be used to determine the correct size for the image
   // ripped for a package
   strcpy(RipStruct.PackageDetails.LongFileName, pCQueueFile->LongName );

   // get the ripstruct
   pRip->GetRipStruct(&RipStruct, RealWPixelsPerInch, RealHPixelsPerInch);

   strcpy(RipStruct.ScratchFolder, csScratchPath);

   if ( RipStruct.PackageDetails.IsDoingPackage )
   {
	   // Use the crop style specified for the largest frame within the package
	   if ( RipStruct.PackageDetails.iCropStyle == 0 )
		   Transform.LoadTransform("SmartFit");
	   else
		   Transform.LoadTransform("SmartCrop");
   }

   // get the appropriate color transform
   if (pCQueueFile->Prefs & USE_LOCAL_COL_PREFS)
      pColorTemp = pCQueueFile->pCC;
   else
      pColorTemp = pCC;
/*
   // copy it to the ripstruct
   memcpy(&RipStruct.CPInitStruct, 
	  &pColorTemp->CPInitStruct, sizeof CPINITSTRUCT);
*/   
   RipStruct.CPInitStruct = pColorTemp->CPInitStruct;

   // save this!
   iOurDeviceColorModel = RipStruct.DeviceColorModel;

   // set this
   RipStruct.bDumpGangPrints = bDumpGangPrints;
/*
   // special stuff for preview
   if (OutputType == RIPSTYLE_PREVIEW)	  	  	  
	  {
	  // if the preview flag is set, do a tranform to RGB space
	  // regardless of the device space, otherwise, force
	  // color correction off
	  if (pColorTemp->CPInitStruct.Preview && pColorTemp->CPInitStruct.TransType == PROFILE_TRANS )
		 {
		 RipStruct.DoColorTransfer = TRUE;	  
		 RipStruct.DeviceColorModel = DEVICE_COLOR_MODEL_RGB;
		 RipStruct.CPInitStruct.Printer_CSpace = RGB_CSPACE;
		 }
	  else
	     RipStruct.DoColorTransfer = FALSE;	  
	  }
   else
	  {
*/
	  /* we need to run the color engine in the following cases:
	     1. We are doing any type of UCR/GCR
		 2. We are doing ICC color matching
		 3. We have some type of color adjustment off the default
		 */
      if (RipStruct.CPInitStruct.TransType == PROFILE_TRANS ||
		  RipStruct.CPInitStruct.TransType == GCR_UCR_TRANS ||		 
		  RipStruct.CPInitStruct.bColorAdjustment           ||
		  RipStruct.CPInitStruct.WhiteIsWhite               ||
		  RipStruct.CPInitStruct.BlackIsBlack )
		 {
		 // force this off
//		 RipStruct.CPInitStruct.Preview = FALSE;
	     RipStruct.DoColorTransfer = TRUE;	  
		 if (OutputType == RIPSTYLE_PREVIEW || OutputType == RIPSTYLE_HD_PREVIEW)
		 {
			 RipStruct.DeviceColorModel = DEVICE_COLOR_MODEL_RGB;
			 RipStruct.CPInitStruct.Printer_CSpace = RGB_CSPACE;
		 }
		 }
      else
	     RipStruct.DoColorTransfer = FALSE;	  	  
/*
	  }
*/
   // FOR NOW!!!!!
   //RipStruct.DoColorTransfer = FALSE;	 

   // force off for this type of rip
   if (ForceOffPackage)
      RipStruct.PackageDetails.IsDoingPackage = FALSE;

   // mod the ripstruct if we are doing anti-aliasing so we
   // get proper sizing vals
   if (pDevice->pDevInfo)
      {
      if (pDevice->pDevInfo->AliasStyle != ANTIALIAS_STYLE_NONE && 
		 OutputType != RIPSTYLE_PREVIEW &&
		 OutputType != RIPSTYLE_HD_PREVIEW &&
		 LastFileType != CRIP_FILETYPE_BITMAP)
         {
	     RipStruct.PixWidth *= pDevice->pDevInfo->AliasLevel;
	     RipStruct.PixHeight *= pDevice->pDevInfo->AliasLevel;	  
	     RipStruct.WPixelsPerInch *= (float)pDevice->pDevInfo->AliasLevel;
         RipStruct.HPixelsPerInch *= (float)pDevice->pDevInfo->AliasLevel;
	     }
	  }

   if ( RipStruct.CPInitStruct.TransType == PROFILE_TRANS && RipStruct.DoColorTransfer )
   {
	   // Check the PrinterProfile
	   // If it is the "Default Profile" ask the device what the 
	   // Default profile is:

	   // check for existence of printprofile.  if it doesn't exist, try to
	   // use the default profile
	   // note that we gracefully exit out later if we don't have a valid profile
	   // but since the cmmdialog shows << Default Profile >> when the file is missing
	   // we might as well insert it here
	   BOOL bProfileExist = FALSE;
	   HANDLE hFile;
       WIN32_FIND_DATA FileInfo;
	   if ((hFile = FindFirstFile(RipStruct.CPInitStruct.Printer_Profile_Name, 
		  &FileInfo)) != INVALID_HANDLE_VALUE)
		  {      		
		  bProfileExist = TRUE;
		  // close the handle
		  FindClose(hFile);   
		  } 			 

	   CString csDefaultProfile;
	   csDefaultProfile.LoadString(IDS_DEFAULTPROFILE);

	   // do we have a default profile?
	   if (strlen(RipStruct.DefaultPrinterProfile) > 0)
		  {
	      // strip off path junk if it's there
	      char fname[_MAX_FNAME];	   
	      _splitpath(RipStruct.CPInitStruct.Printer_Profile_Name, NULL, NULL, fname, NULL);

		  // if we have are supposed to use the default profile or
		  // if it is simply missing, use it
	      if ((csDefaultProfile.Compare(fname) == 0) || !bProfileExist)
			 {
		     if ( strlen(RipStruct.DefaultPrinterProfile ) != 0 )
				{
			    strcpy( RipStruct.CPInitStruct.Printer_Profile_Name, 
				   RipStruct.DefaultPrinterProfile );
				}
		     else
				{
			    iRetCode = SYN_ERRMSG_NODEFAULTPROFILE;
				}
			 }
		  }

	   bProfileExist = FALSE;
	   if ((hFile = FindFirstFile(RipStruct.CPInitStruct.Printer_Profile_Name, 
												&FileInfo)) != INVALID_HANDLE_VALUE)
	   {      		
		  bProfileExist = TRUE;
		  // close the handle
		  FindClose(hFile);   
	   } 			 


	   if ( iRetCode == SYN_ERRMSG_NODEFAULTPROFILE || !bProfileExist )
		  {
		  iRetCode = SYN_ERRMSG_ALLOK;

		  // no specific default profile, so load the default default
		  CString csTemp;

		  switch(iOurDeviceColorModel)
			 {
			 case DEVICE_COLOR_MODEL_RGB:
				  csTemp = Profile.GetStr(IDS_TRUEGRAPHX, "DefaultRGBDevICC");
				  break;
			 case DEVICE_COLOR_MODEL_CMY:
				  // not defined?
				  csTemp = "";				  
				  break;
			 case DEVICE_COLOR_MODEL_CMYK:
				  csTemp = Profile.GetStr(IDS_TRUEGRAPHX, "DefaultCMYKDevICC");
				  break;
			 }

		  if (csTemp.GetLength() < 1) 
			 iRetCode = SYN_ERRMSG_NODEFAULTPROFILE;
		  else
		  {
			if ( !csTemp.IsEmpty() )
			{
				strcpy( RipStruct.CPInitStruct.Printer_Profile_Name, csTemp );
			}
			else
			{
				iRetCode = SYN_ERRMSG_NODEFAULTPROFILE;
			}
		  }
	   }
   }

#ifdef DEMO
   RipStruct.DemoVersion = TRUE;
#else
   RipStruct.DemoVersion = FALSE;
#endif

   // get a bounding box which is optimal for this device
   // generally, this only is necessary for Kodak Photo CD files
   // which have variable resolutions
   BoundingBox = pCQueueFile->GetSetOptimalBoundingBox(RipStruct.PixWidth, 
      RipStruct.PixHeight, &JobInfo.PCDResolution);   
   
   // view perspective
   JobInfo.ViewPerspective = pCQueueFile->ViewPerspective;   

   // owner and submitted strings
   ZeroMemory(JobInfo.lpOwner, JOBINFO_OWNER_STRLEN);
   strncpy(JobInfo.lpOwner, pCQueueFile->csOwner, JOBINFO_OWNER_STRLEN - 1);
   ZeroMemory(JobInfo.lpSubmitted, JOBINFO_OWNER_STRLEN);
   strncpy(JobInfo.lpSubmitted, pCQueueFile->csSubmitted, JOBINFO_OWNER_STRLEN - 1);

   // if this is a photogize file, set the order string to the owner
   if (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_PGPRCFILE)
   {
	  strcpy(JobInfo.lpPhotogizeOrder, pCQueueFile->csOwner);
   }
   else
   {
	  ZeroMemory(JobInfo.lpPhotogizeOrder, JOBINFO_PHOTOGIZE_ORDER );
   }

   // Initialize Xtra Fields
   ZeroMemory(JobInfo.Unused, JOBINFO_UNUSED_STRLEN );
//   JobInfo.Unused1 = NULL;
//   JobInfo.UnusedByte1 = NULL;
   JobInfo.UnusedByte2 = NULL;
   JobInfo.UnusedByte3 = NULL;
   JobInfo.Unused2 = NULL;
   JobInfo.Unused3 = NULL;
   JobInfo.Unused4 = NULL;

   // load output type
   JobInfo.OutputType = OutputType;   

   // store jobid
   JobInfo.iJobID = pCQ->iJobID;

   // default
   JobInfo.BitmapSample = BITMAPSAMPLING_NEAREST;

   // set the RipSession ID
   JobInfo.uiRipSessionID = uiRipSessionID++;

   // start this at 0
   JobInfo.iPageNum = 0;
   
   if (pCQ->m_prange)
	  {
	  CWordArray cw;
	  unsigned short i;

	  // get page selection
	  JobInfo.usNumPagesToPrint = ParsePageRangeSelection(cw);	  

	  // load into array
	  if (JobInfo.usNumPagesToPrint > 0)
		 {
		 for (i = 0; i < JobInfo.usNumPagesToPrint; i++)
			{
			// max array check
			if (i >= (_MAX_PATH - 1)) break;
			// load it
			JobInfo.usPageSelection[i] = (unsigned short)cw.GetAt(i);
			}		 
		 }
	  }
   else
	  {
	  // if sub page set, do one page
	  if (pCQueueFile->bDoSubPage)
		 {
		 JobInfo.usNumPagesToPrint = 1;
		 JobInfo.usPageSelection[0] = (unsigned short)pCQueueFile->iSubPage;
		 }				
	  else
		 {
	     // print all pages
	     JobInfo.usNumPagesToPrint = 0;
		 }
	  }

   // store these for previews
   if (RipStruct.PackageDetails.IsDoingPackage)
	  {
	  JobInfo.crMediaRect.top = 0;
	  JobInfo.crMediaRect.bottom = 
		 (int)((float)RipStruct.PackageDetails.iPixDevPageHeight / 
		 (float)RipStruct.PackageDetails.HPixelsPerInch * 100.F + .5F);
	  JobInfo.crMediaRect.left = 0;
	  JobInfo.crMediaRect.right = 
		 (int)((float)RipStruct.PackageDetails.iPixDevPageWidth / 
		 (float)RipStruct.PackageDetails.WPixelsPerInch * 100.F + .5F);

	  JobInfo.RipPixHeight = RipStruct.PackageDetails.iPixDevPageHeight;
	  JobInfo.RipPixWidth = RipStruct.PackageDetails.iPixDevPageWidth;
	  JobInfo.RipWPixelsPerInch = RipStruct.PackageDetails.WPixelsPerInch;
	  JobInfo.RipHPixelsPerInch = RipStruct.PackageDetails.HPixelsPerInch;
	  }
   else
	  {
	  CRect cMediaRect;
	  pDevice->GetCurrentMediaRect(cMediaRect);	   
	  JobInfo.crMediaRect.top = cMediaRect.top;
	  JobInfo.crMediaRect.bottom = cMediaRect.bottom;
	  JobInfo.crMediaRect.left = cMediaRect.left;
	  JobInfo.crMediaRect.right = cMediaRect.right;
	  JobInfo.RipPixHeight = RipStruct.PixHeight;
	  JobInfo.RipPixWidth = RipStruct.PixWidth;
	  JobInfo.RipWPixelsPerInch = RipStruct.WPixelsPerInch;
	  JobInfo.RipHPixelsPerInch = RipStruct.HPixelsPerInch;
	  }
 
   // assume that by the time we reach this number 
   // the jobs starting at 0 are long gone
   if (uiRipSessionID > 4000000000)
	  uiRipSessionID = 0;			

   // mutex stuff
   JobInfo.IsComplete = pCQueueFile->IsComplete;
   strcpy(JobInfo.MutexName, pCQueueFile->MutexName);
   
   // set ripstruct and sizing params
   SetSizing();

   // if we can optimize, but we're told not to, OR we are
   // doing a package, just default to portrait
   if ((pDevice->pDevInfo->CanOptimizeOrientation && 
	  !pDevice->pDevInfo->OptimizeOrientation))
	  {
	  pDevice->pDevInfo->Orientation = DEV_ORIENT_PORTRAIT;
	  pDevice->SetDevInfo(FALSE);
	  }
   // if this device CanOptimizeOrientation and we have a rotation, and
   // we are not doing a package (which must always be portrait), retry sizing 
   // with a new orientation
   else if (pDevice->pDevInfo->CanOptimizeOrientation && 
	  pDevice->pDevInfo->OptimizeOrientation)	  
	  {
	  // try portrait
	  //pDevice->pDevInfo->Orientation = DEV_ORIENT_PORTRAIT;
  	  // for bitmaps, we have a flag
	  if (LastFileType == CRIP_FILETYPE_BITMAP)
		 {
		 if ((RipStruct.Rotation != 0) && (RipStruct.Rotation != 180))
			{
		    if (pDevice->pDevInfo->Orientation == DEV_ORIENT_LANDSCAPE)
			   pDevice->pDevInfo->Orientation = DEV_ORIENT_PORTRAIT;
		    else
			   pDevice->pDevInfo->Orientation = DEV_ORIENT_LANDSCAPE;
			// swap sizing coords and redo
			pDevice->SetDevInfo(FALSE);						
			// a check - just in case 
			if (iPassCnt < 2)
			   goto ATSTART;
			}
		 }
	  else
		 {
		 // for PostScript, this is more of a kluge
	     if ((RipStruct.InitMatrix.val[1] != 0) && (RipStruct.InitMatrix.val[1] != 180))
			{			
		    if (pDevice->pDevInfo->Orientation == DEV_ORIENT_LANDSCAPE)
			   pDevice->pDevInfo->Orientation = DEV_ORIENT_PORTRAIT;
		    else
			   pDevice->pDevInfo->Orientation = DEV_ORIENT_LANDSCAPE;
			pDevice->SetDevInfo(FALSE);			
			// swap sizing coords and redo
			pDevice->SetDevInfo(FALSE);			
			// a check - just in case 
			if (iPassCnt < 2)
			   goto ATSTART;
			}
		 }		 
	  }
   
   // if a device can scale a bitmap, find the optimal minimum resolution
   // for this rip
   if (pDevice->pDevInfo)
   {
   	  if (LastFileType == CRIP_FILETYPE_BITMAP &&  
         pDevice->pDevInfo->AliasStyle == ANTIALIAS_STYLE_NONE)
         {
	     if (pDevice->pDevInfo->CanScaleImage && pDevice->pDevInfo->ScaleImage &&
				!pDevice->pDevInfo->CanCropImage )
	        {
		    // make optimial bitmap output params
			if (OutputType != RIPSTYLE_PREVIEW && OutputType != RIPSTYLE_HD_PREVIEW)
		       OptimizeBitmapOutput(BoundingBox, pDevice);
		    // reset ripstruct and sizing params
            SetSizing();
		    }
	     }	 
   }

   // reset the ripstruct if we are doing anti-aliasing
   if (pDevice->pDevInfo)
      {
      if (pDevice->pDevInfo->AliasStyle != ANTIALIAS_STYLE_NONE && 
		 OutputType != RIPSTYLE_PREVIEW &&
		 OutputType != RIPSTYLE_HD_PREVIEW &&
		 LastFileType != CRIP_FILETYPE_BITMAP)
         {
	     RipStruct.PixWidth /= pDevice->pDevInfo->AliasLevel;
	     RipStruct.PixHeight /= pDevice->pDevInfo->AliasLevel;	  
	     RipStruct.WPixelsPerInch /= (float)pDevice->pDevInfo->AliasLevel;
         RipStruct.HPixelsPerInch /= (float)pDevice->pDevInfo->AliasLevel;
	     }
	  }

   // if the filetype is BLIST, make the file now
   if (pCQueueFile->FileType == FTYPE_BLIST)
   {
      MakeBatchHeaderFile();
	  csBatchHdrFilename = pCQueueFile->LongName;
   }
   else
   {
	  if ( !csBatchHdrFilename.IsEmpty() )
	     csBatchHdrFilename.Empty();
   }
   
   //
   // load JobInfo stuff
   //   
   
   // file name   
   strncpy(JobInfo.LongName, 
      (const char *)pCQueueFile->LongName, MAX_RIP_STR_LEN);

   // set short name
   strncpy(JobInfo.ShortName,
      (const char *)pCQueueFile->ShortName, MAX_RIP_STR_LEN); 
   
   // add ps specific stuff   
   JobInfo.CATHeaderNeeded = pCQueueFile->CATHeaderNeeded;
   JobInfo.StartofValidPS = pCQueueFile->StartofValidPS; 
   JobInfo.RealFileSize = pCQueueFile->RealFileSize;
   
   // who owns the preview?   
   JobInfo.hwndPreviewOwner = cwPreview->m_hWnd;
   
   // who gets reporting messages?
   JobInfo.hFormWin = cwReport->m_hWnd;         
   
   // file type
   JobInfo.FileType = pCQueueFile->FileType; 

   // other stuff
   JobInfo.BitmapEngineMutex = BITMAPENGINEMUTEX;
   
   if (pDevice->pDevInfo && OutputType != RIPSTYLE_PREVIEW && OutputType != RIPSTYLE_HD_PREVIEW)
      {
	  JobInfo.AliasStyle = pDevice->pDevInfo->AliasStyle;
      JobInfo.AliasLevel = pDevice->pDevInfo->AliasLevel;
	  }
   else
      {
   	  JobInfo.AliasStyle = ANTIALIAS_STYLE_NONE;
      JobInfo.AliasLevel = 1;
	  }

   // Quick Print Option for Bitmaps
   JobInfo.QuickPrintImage = FALSE;		// Default
   StartTransformIndex = Transform.StartIndex();   

   JobInfo.JobSizing = SMARTCROPTRANSFORM;
   if ( StartTransformIndex == SMARTFITTRANSFORM || StartTransformIndex == SMARTCROPTRANSFORM )
   {
	   JobInfo.JobSizing = StartTransformIndex;
   }


   if ( pDevice->pDevInfo )
   {
	   // This is a Noritsu Feature BEWARE if you attempt to use this with any other device
	   //
	   if (pDevice->pDevInfo->ScaleImage && pDevice->pDevInfo->CanCropImage > 0 &&
					( OutputType != RIPSTYLE_PREVIEW && OutputType != RIPSTYLE_HD_PREVIEW ) &&
					( StartTransformIndex == SMARTFITTRANSFORM || StartTransformIndex == SMARTCROPTRANSFORM ) )
	   {
		  switch(JobInfo.FileType)
			{
			   case FTYPE_BMP:
				   if ( pDevice->pDevInfo->QuickPrintFType & QUICK_PRINT_BMP )
						JobInfo.QuickPrintImage = TRUE;
				   break;
			   case FTYPE_JFIF:
			   case FTYPE_JTIF:
			   case FTYPE_FPX:
				   if ( pDevice->pDevInfo->QuickPrintFType & QUICK_PRINT_JPEG )
						JobInfo.QuickPrintImage = TRUE;
				   break;
			   case FTYPE_TIFF:
				   if ( pDevice->pDevInfo->QuickPrintFType & QUICK_PRINT_TIFF )
						JobInfo.QuickPrintImage = TRUE;
				   break;
			   case FTYPE_EXIF:
				   if ( pDevice->pDevInfo->QuickPrintFType & QUICK_PRINT_EXIF )
						JobInfo.QuickPrintImage = TRUE;
				   break;

			   default:
				   JobInfo.QuickPrintImage = FALSE;
				   break;
			}

		  // This is a Noritsu Feature BEWARE if you attempt to use this with any other device
		  //
		  if ( pDevice->pDevInfo->ScaleImage && pDevice->pDevInfo->CanCropImage == 2 &&
				JobInfo.QuickPrintImage == TRUE)
		  {
			  // Check the Image Size
			  // Only Quick Print if Size is equal to the Selected Media

			  // Allow for rounding by adjusting +/- .2 inches
			  int iMinHeight	= (int)(((JobInfo.crMediaRect.bottom - ROUNDING_FACTOR) * JobInfo.RipHPixelsPerInch) / 100);
			  int iMaxHeight	= (int)(((JobInfo.crMediaRect.bottom + ROUNDING_FACTOR) * JobInfo.RipHPixelsPerInch) / 100);
			  int iMinWidth		= (int)(((JobInfo.crMediaRect.right - ROUNDING_FACTOR) * JobInfo.RipWPixelsPerInch) / 100);
			  int iMaxWidth		= (int)(((JobInfo.crMediaRect.right + ROUNDING_FACTOR) * JobInfo.RipWPixelsPerInch) / 100);

			  if (( BoundingBox.top >= iMinHeight) &&
					( BoundingBox.top <= iMaxHeight) &&
						( BoundingBox.right >= iMinWidth) &&
							( BoundingBox.right <= iMaxWidth))
			  {
					JobInfo.QuickPrintImage = TRUE;
			  }
			  else
			  {
					JobInfo.QuickPrintImage = FALSE;
			  }
		  }
	   }
   }

   return(iRetCode);
   }           

#ifdef LEADFILE_PS
int CRip::LeadOnlyInit(CDevice* pDevice, CTransform* pct, CColorTransform* pCC,
   CQueueFile* pCQ, int OutputType, CWnd* cwReport, CWnd* cwPreview, 
   BOOL ForceOffPackage, BOOL bDumpGangPrints)
{
	BOOL fIsBGRDevice = TRUE;
	BOOL bFileExist;

	CColorTransform* pColorTemp;
	CProfile Profile;   
	CString csTempPath;
	CString csError;

	char cFileName[_MAX_PATH];

	FILEINFO FileInfo;
	WIN32_FIND_DATA FileFindInfo;

	HANDLE hFile;

	int iRet = 0;
	int iPassCnt = 0;
	int iOurDeviceColorModel;

	LOADFILEOPTION   LoadFileOption;

ATSTART:
   iPassCnt++;

	memset(&FileInfo, 0, sizeof(FILEINFO) );
    FileInfo.uStructSize = sizeof(FILEINFO);
	memset(&LoadFileOption, 0, sizeof(LOADFILEOPTION) );

	strcpy(cFileName, pCQ->LongName);

	L_FileInfo(cFileName, &FileInfo, sizeof(FILEINFO), NULL, NULL);

	if ( FileInfo.Format == FILE_UNKNOWN_FORMAT )
	{
		ZeroMemory(&FileInfo, sizeof(FILEINFO));
		if ((pCQ->FileType == FTYPE_PSCRIPT) || (pCQ->FileType == FTYPE_PDF))
		{
			if (pCQ->FileType == FTYPE_PSCRIPT)
				FileInfo.Format = FILE_POSTSCRIPT;
			else
				FileInfo.Format = FILE_RAS_PDF;

			FileInfo.Flags = FILEINFO_FORMATVALID;
		}
	}

	if ( FileInfo.Format == FILE_POSTSCRIPT ||
			FileInfo.Format == FILE_RAS_PDF )
	{
		bFileExist = FALSE;
		if ((hFile = FindFirstFile(pCQ->LongName, &FileFindInfo)) != INVALID_HANDLE_VALUE)
		{      		
			bFileExist = TRUE;
			// close the handle
			FindClose(hFile);   
		} 			 

//		if ( pCQ->PS_JPGFileLongName.IsEmpty() || bFileExist == FALSE)
//			pCQ->PS_JPGFileLongName = ((CZeusApp*)AfxGetApp())->Convert_PS_to_JPG(pCQ->LongName);
	}

    iRet = SYN_ERRMSG_ALLOK;
	if ( !pRip )
	{
		pRip = new CRipItB;
	}

   // Always Processing a BitMap
   //
   LastFileType = CRIP_FILETYPE_BITMAP;

   // store pointer to process
   pRip->pCProcess = pCProcess;

/*
   // check for the existence of the file if not a BatchHeader
   if (pCQ->FileType != FTYPE_BLIST)
	  {
	  HANDLE hFile;
	  WIN32_FIND_DATA FileInfo;
      if ((hFile = FindFirstFile(pCQ->LongName, 
         &FileInfo)) != INVALID_HANDLE_VALUE)
		 {      
		 // close the handle
		 FindClose(hFile);   
		 }
	  else
		 {
	     pCQ->RealFileSize = pCQ->iFileSize = 0;
	     return SYN_ERRMSG_FILENOTFOUND;
		 }

      // update the file size if if pCQ->RealFileSize == pCQ->iFileSize   
      if (pCQ->RealFileSize == pCQ->iFileSize)
		 {
	     pCQ->RealFileSize = pCQ->iFileSize = FileInfo.nFileSizeLow;
		 }
      // if pCQ->RealFileSize != pCQ->iFileSize and iFileSize has changed then 
      // we have (had) an EPS file and we need to reget everything
      else if (pCQ->RealFileSize != pCQ->iFileSize && pCQ->iFileSize != FileInfo.nFileSizeLow)
		 {
         CStdioFile csfCheckFile;

         // open the file
         if (csfCheckFile.Open(pCQ->LongName, 
		    CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone))
			{
            pCQ->CheckForPostScript(csfCheckFile);
            csfCheckFile.Close();
			}
		 }
	  }
*/

   // load initial params                                          
   pCQueueFile = pCQ;
   //BoundingBox = pCQueueFile->BoundingBox;   
   EPSF = pCQueueFile->EPSF;
   ExtraInfo = pCQueueFile->ExtraInfo;
   Transform = *pct;       

   // store pointer to the jobinfo
   pRip->pJobInfo = &JobInfo;
   
   // init the rip
   if (!pRip->Init(pDevice, OutputType))
      return(SYN_ERRMSG_RIPNOTFOUND);

   // zero this out
   memset(&RipStruct, 0, sizeof RIPSTRUCT);

   strcpy(RipStruct.SharedMemoryName, pDevice->csFriendlyName);

   // ripstruct with bounding box in case it is needed for
   // packagizing
   if (pCQueueFile)
	  {
	  RipStruct.PixWidth = abs(pCQueueFile->BoundingBox.Width());
      RipStruct.PixHeight = abs(pCQueueFile->BoundingBox.Height());
	  }

   // This will be used to determine the correct size for the image
   // ripped for a package
   if ( pCQueueFile->PS_JPGFileLongName.IsEmpty() )
		strcpy(RipStruct.PackageDetails.LongFileName, pCQueueFile->LongName );
   else
		strcpy(RipStruct.PackageDetails.LongFileName, pCQueueFile->PS_JPGFileLongName );

   // get the ripstruct
   pRip->GetRipStruct(&RipStruct, RealWPixelsPerInch, RealHPixelsPerInch);

   strcpy(RipStruct.ScratchFolder, csScratchPath);

   if ( RipStruct.PackageDetails.IsDoingPackage )
   {
	   // Use the crop style specified for the largest frame within the package
	   if ( RipStruct.PackageDetails.iCropStyle == 0 )
		   Transform.LoadTransform("SmartFit");
	   else
		   Transform.LoadTransform("SmartCrop");
   }

   // get the appropriate color transform
   if (pCQueueFile->Prefs & USE_LOCAL_COL_PREFS)
      pColorTemp = pCQueueFile->pCC;
   else
      pColorTemp = pCC;
/*
   // copy it to the ripstruct
   memcpy(&RipStruct.CPInitStruct, 
	  &pColorTemp->CPInitStruct, sizeof CPINITSTRUCT);
*/   
   RipStruct.CPInitStruct = pColorTemp->CPInitStruct;

   // save this!
   iOurDeviceColorModel = RipStruct.DeviceColorModel;

   // set this
   RipStruct.bDumpGangPrints = bDumpGangPrints;
/*
   // special stuff for preview
   if (OutputType == RIPSTYLE_PREVIEW)	  	  	  
	  {
	  // if the preview flag is set, do a tranform to RGB space
	  // regardless of the device space, otherwise, force
	  // color correction off
	  if (pColorTemp->CPInitStruct.Preview && pColorTemp->CPInitStruct.TransType == PROFILE_TRANS )
		 {
		 RipStruct.DoColorTransfer = TRUE;	  
		 RipStruct.DeviceColorModel = DEVICE_COLOR_MODEL_RGB;
		 RipStruct.CPInitStruct.Printer_CSpace = RGB_CSPACE;
		 }
	  else
	     RipStruct.DoColorTransfer = FALSE;	  
	  }
   else
	  {
*/
	  /* we need to run the color engine in the following cases:
	     1. We are doing any type of UCR/GCR
		 2. We are doing ICC color matching
		 3. We have some type of color adjustment off the default
		 */
      if (RipStruct.CPInitStruct.TransType == PROFILE_TRANS ||
		  RipStruct.CPInitStruct.TransType == GCR_UCR_TRANS ||		 
		  RipStruct.CPInitStruct.bColorAdjustment           ||
		  RipStruct.CPInitStruct.WhiteIsWhite               ||
		  RipStruct.CPInitStruct.BlackIsBlack )
		 {
		 // force this off
//		 RipStruct.CPInitStruct.Preview = FALSE;
	     RipStruct.DoColorTransfer = TRUE;	  
		 if (OutputType == RIPSTYLE_PREVIEW || OutputType == RIPSTYLE_HD_PREVIEW)
		 {
			 RipStruct.DeviceColorModel = DEVICE_COLOR_MODEL_RGB;
			 RipStruct.CPInitStruct.Printer_CSpace = RGB_CSPACE;
		 }
		 }
      else
	     RipStruct.DoColorTransfer = FALSE;	  	  
/*
	  }
*/
   // FOR NOW!!!!!
   //RipStruct.DoColorTransfer = FALSE;	 

   // force off for this type of rip
   if (ForceOffPackage)
      RipStruct.PackageDetails.IsDoingPackage = FALSE;

   // mod the ripstruct if we are doing anti-aliasing so we
   // get proper sizing vals
   if (pDevice->pDevInfo)
      {
      if (pDevice->pDevInfo->AliasStyle != ANTIALIAS_STYLE_NONE && 
		 OutputType != RIPSTYLE_PREVIEW &&
		 OutputType != RIPSTYLE_HD_PREVIEW &&
		 LastFileType != CRIP_FILETYPE_BITMAP)
         {
	     RipStruct.PixWidth *= pDevice->pDevInfo->AliasLevel;
	     RipStruct.PixHeight *= pDevice->pDevInfo->AliasLevel;	  
	     RipStruct.WPixelsPerInch *= (float)pDevice->pDevInfo->AliasLevel;
         RipStruct.HPixelsPerInch *= (float)pDevice->pDevInfo->AliasLevel;
	     }
	  }

   if ( RipStruct.CPInitStruct.TransType == PROFILE_TRANS && RipStruct.DoColorTransfer )
   {
	   // Check the PrinterProfile
	   // If it is the "Default Profile" ask the device what the 
	   // Default profile is:

	   // check for existence of printprofile.  if it doesn't exist, try to
	   // use the default profile
	   // note that we gracefully exit out later if we don't have a valid profile
	   // but since the cmmdialog shows << Default Profile >> when the file is missing
	   // we might as well insert it here
	   BOOL bProfileExist = FALSE;
	   HANDLE hFile;
       WIN32_FIND_DATA FileInfo;
	   if ((hFile = FindFirstFile(RipStruct.CPInitStruct.Printer_Profile_Name, 
		  &FileInfo)) != INVALID_HANDLE_VALUE)
		  {      		
		  bProfileExist = TRUE;
		  // close the handle
		  FindClose(hFile);   
		  } 			 

	   CString csDefaultProfile;
	   csDefaultProfile.LoadString(IDS_DEFAULTPROFILE);

	   // do we have a default profile?
	   if (strlen(RipStruct.DefaultPrinterProfile) > 0)
		  {
	      // strip off path junk if it's there
	      char fname[_MAX_FNAME];	   
	      _splitpath(RipStruct.CPInitStruct.Printer_Profile_Name, NULL, NULL, fname, NULL);

		  // if we have are supposed to use the default profile or
		  // if it is simply missing, use it
	      if ((csDefaultProfile.Compare(fname) == 0) || !bProfileExist)
			 {
		     if ( strlen(RipStruct.DefaultPrinterProfile ) != 0 )
				{
			    strcpy( RipStruct.CPInitStruct.Printer_Profile_Name, 
				   RipStruct.DefaultPrinterProfile );
				}
		     else
				{
			    iRet = SYN_ERRMSG_NODEFAULTPROFILE;
//			    iRetCode = SYN_ERRMSG_NODEFAULTPROFILE;
				}
			 }

			csError.Format("strlen(RipStruct.DefaultPrinterProfile = %d", iRet);
		  }

	   bProfileExist = FALSE;
	   if ((hFile = FindFirstFile(RipStruct.CPInitStruct.Printer_Profile_Name, 
												&FileInfo)) != INVALID_HANDLE_VALUE)
	   {      		
		  bProfileExist = TRUE;
		  // close the handle
		  FindClose(hFile);   
	   } 			 


	   if ( iRet == SYN_ERRMSG_NODEFAULTPROFILE || !bProfileExist )
//	   if ( iRetCode == SYN_ERRMSG_NODEFAULTPROFILE || !bProfileExist )
		  {
		  iRet = SYN_ERRMSG_ALLOK;
//		  iRetCode = SYN_ERRMSG_ALLOK;

		  // no specific default profile, so load the default default
		  CString csTemp;

		  switch(iOurDeviceColorModel)
			 {
			 case DEVICE_COLOR_MODEL_RGB:
				  csTemp = Profile.GetStr(IDS_TRUEGRAPHX, "DefaultRGBDevICC");
				  break;
			 case DEVICE_COLOR_MODEL_CMY:
				  // not defined?
				  csTemp = "";				  
				  break;
			 case DEVICE_COLOR_MODEL_CMYK:
				  csTemp = Profile.GetStr(IDS_TRUEGRAPHX, "DefaultCMYKDevICC");
				  break;
			 }

		  if (csTemp.GetLength() < 1) 
		  {
			 iRet = SYN_ERRMSG_NODEFAULTPROFILE;
//			 iRetCode = SYN_ERRMSG_NODEFAULTPROFILE;
		  }
		  else
		  {
			if ( !csTemp.IsEmpty() )
			{
				strcpy( RipStruct.CPInitStruct.Printer_Profile_Name, csTemp );
			}
			else
			{
				iRet = SYN_ERRMSG_NODEFAULTPROFILE;
//				iRetCode = SYN_ERRMSG_NODEFAULTPROFILE;
			}
		  }
		csError.Format("csTemp.GetLength() = %d", iRet);
	   }
   }

#ifdef DEMO
   RipStruct.DemoVersion = TRUE;
#else
   RipStruct.DemoVersion = FALSE;
#endif

   // get a bounding box which is optimal for this device
   // generally, this only is necessary for Kodak Photo CD files
   // which have variable resolutions
   BoundingBox = pCQueueFile->GetSetOptimalBoundingBox(RipStruct.PixWidth, 
										RipStruct.PixHeight, &JobInfo.PCDResolution);   
   
   // view perspective
   JobInfo.ViewPerspective = pCQueueFile->ViewPerspective;   
   // owner and submitted strings
   ZeroMemory(JobInfo.lpOwner, JOBINFO_OWNER_STRLEN);
   strncpy(JobInfo.lpOwner, pCQueueFile->csOwner, JOBINFO_OWNER_STRLEN - 1);
   ZeroMemory(JobInfo.lpSubmitted, JOBINFO_OWNER_STRLEN);
   strncpy(JobInfo.lpSubmitted, pCQueueFile->csSubmitted, JOBINFO_OWNER_STRLEN - 1);

   // if this is a photogize file, set the order string to the owner
   if (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_PGPRCFILE)
	  strcpy(JobInfo.lpPhotogizeOrder, pCQueueFile->csOwner);
   else												   
	  ZeroMemory(JobInfo.lpPhotogizeOrder, JOBINFO_PHOTOGIZE_ORDER );

   // Initialize Xtra Fields
   ZeroMemory(JobInfo.Unused, JOBINFO_UNUSED_STRLEN );
//   JobInfo.Unused1 = NULL;
   JobInfo.UnusedByte1 = NULL;
   JobInfo.UnusedByte2 = NULL;
   JobInfo.UnusedByte3 = NULL;
   JobInfo.Unused2 = NULL;
   JobInfo.Unused3 = NULL;
   JobInfo.Unused4 = NULL;

   // load output type
   JobInfo.OutputType = OutputType;   

   // store jobid
   JobInfo.iJobID = pCQ->iJobID;

   // default
   JobInfo.BitmapSample = BITMAPSAMPLING_NEAREST;

   // set the RipSession ID
   JobInfo.uiRipSessionID = uiRipSessionID++;

   // start this at 0
   JobInfo.iPageNum = 0;
   
   if (pCQ->m_prange)
	  {
	  CWordArray cw;
	  unsigned short i;

	  // get page selection
	  JobInfo.usNumPagesToPrint = ParsePageRangeSelection(cw);	  

	  // load into array
	  if (JobInfo.usNumPagesToPrint > 0)
		 {
		 for (i = 0; i < JobInfo.usNumPagesToPrint; i++)
			{
			// max array check
			if (i >= (_MAX_PATH - 1)) break;
			// load it
			JobInfo.usPageSelection[i] = (unsigned short)cw.GetAt(i);
			}		 
		 }
	  }
   else
	  {
	  // if sub page set, do one page
	  if (pCQueueFile->bDoSubPage)
		 {
		 JobInfo.usNumPagesToPrint = 1;
		 JobInfo.usPageSelection[0] = (unsigned short)pCQueueFile->iSubPage;
		 }				
	  else
		 {
	     // print all pages
	     JobInfo.usNumPagesToPrint = 0;
		 }
	  }

   // store these for previews
   if (RipStruct.PackageDetails.IsDoingPackage)
	  {
	  JobInfo.crMediaRect.top = 0;
	  JobInfo.crMediaRect.bottom = 
		 (int)((float)RipStruct.PackageDetails.iPixDevPageHeight / 
		 (float)RipStruct.PackageDetails.HPixelsPerInch * 100.F + .5F);
	  JobInfo.crMediaRect.left = 0;
	  JobInfo.crMediaRect.right = 
		 (int)((float)RipStruct.PackageDetails.iPixDevPageWidth / 
		 (float)RipStruct.PackageDetails.WPixelsPerInch * 100.F + .5F);

	  JobInfo.RipPixHeight = RipStruct.PackageDetails.iPixDevPageHeight;
	  JobInfo.RipPixWidth = RipStruct.PackageDetails.iPixDevPageWidth;
	  JobInfo.RipWPixelsPerInch = RipStruct.PackageDetails.WPixelsPerInch;
	  JobInfo.RipHPixelsPerInch = RipStruct.PackageDetails.HPixelsPerInch;
	  }
   else
	  {
	  CRect cMediaRect;
	  pDevice->GetCurrentMediaRect(cMediaRect);	   
	  JobInfo.crMediaRect.top = cMediaRect.top;
	  JobInfo.crMediaRect.bottom = cMediaRect.bottom;
	  JobInfo.crMediaRect.left = cMediaRect.left;
	  JobInfo.crMediaRect.right = cMediaRect.right;
	  JobInfo.RipPixHeight = RipStruct.PixHeight;
	  JobInfo.RipPixWidth = RipStruct.PixWidth;
	  JobInfo.RipWPixelsPerInch = RipStruct.WPixelsPerInch;
	  JobInfo.RipHPixelsPerInch = RipStruct.HPixelsPerInch;
	  }
 
   // assume that by the time we reach this number 
   // the jobs starting at 0 are long gone
   if (uiRipSessionID > 4000000000)
	  uiRipSessionID = 0;			

   // mutex stuff
   JobInfo.IsComplete = pCQueueFile->IsComplete;
   strcpy(JobInfo.MutexName, pCQueueFile->MutexName);
   
   // set ripstruct and sizing params
   SetSizing();

   // if we can optimize, but we're told not to, OR we are
   // doing a package, just default to portrait
   if ((pDevice->pDevInfo->CanOptimizeOrientation && 
	  !pDevice->pDevInfo->OptimizeOrientation))
	  {
	  pDevice->pDevInfo->Orientation = DEV_ORIENT_PORTRAIT;
	  pDevice->SetDevInfo(FALSE);
	  }
	   // if this device CanOptimizeOrientation and we have a rotation, and
	   // we are not doing a package (which must always be portrait), retry sizing 
	   // with a new orientation
   else if (pDevice->pDevInfo->CanOptimizeOrientation && 
	  pDevice->pDevInfo->OptimizeOrientation)	  
	  {
	  // try portrait
	  //pDevice->pDevInfo->Orientation = DEV_ORIENT_PORTRAIT;
  	  // for bitmaps, we have a flag
	  if (LastFileType == CRIP_FILETYPE_BITMAP)
		 {
		 if ((RipStruct.Rotation != 0) && (RipStruct.Rotation != 180))
			{
		    if (pDevice->pDevInfo->Orientation == DEV_ORIENT_LANDSCAPE)
			   pDevice->pDevInfo->Orientation = DEV_ORIENT_PORTRAIT;
		    else
			   pDevice->pDevInfo->Orientation = DEV_ORIENT_LANDSCAPE;
			// swap sizing coords and redo
			pDevice->SetDevInfo(FALSE);						
			// a check - just in case 
			if (iPassCnt < 2)
			   goto ATSTART;
			}
		 }
	  else
		 {
		 // for PostScript, this is more of a kluge
	     if ((RipStruct.InitMatrix.val[1] != 0) && (RipStruct.InitMatrix.val[1] != 180))
			{			
		    if (pDevice->pDevInfo->Orientation == DEV_ORIENT_LANDSCAPE)
			   pDevice->pDevInfo->Orientation = DEV_ORIENT_PORTRAIT;
		    else
			   pDevice->pDevInfo->Orientation = DEV_ORIENT_LANDSCAPE;
			pDevice->SetDevInfo(FALSE);			
			// swap sizing coords and redo
			pDevice->SetDevInfo(FALSE);			
			// a check - just in case 
			if (iPassCnt < 2)
			   goto ATSTART;
			}
		 }		 
	  }
   
   // if a device can scale a bitmap, find the optimal minimum resolution
   // for this rip
   if (pDevice->pDevInfo)
   {
   	  if (LastFileType == CRIP_FILETYPE_BITMAP &&  
         pDevice->pDevInfo->AliasStyle == ANTIALIAS_STYLE_NONE)
         {
	     if (pDevice->pDevInfo->CanScaleImage && pDevice->pDevInfo->ScaleImage &&
				!pDevice->pDevInfo->CanCropImage )
	        {
		    // make optimial bitmap output params
			if (OutputType != RIPSTYLE_PREVIEW && OutputType != RIPSTYLE_HD_PREVIEW)
		       OptimizeBitmapOutput(BoundingBox, pDevice);
		    // reset ripstruct and sizing params
            SetSizing();
		    }
	     }	 
   }

   // reset the ripstruct if we are doing anti-aliasing
   if (pDevice->pDevInfo)
      {
      if (pDevice->pDevInfo->AliasStyle != ANTIALIAS_STYLE_NONE && 
		 OutputType != RIPSTYLE_PREVIEW &&
		 OutputType != RIPSTYLE_HD_PREVIEW &&
		 LastFileType != CRIP_FILETYPE_BITMAP)
         {
	     RipStruct.PixWidth /= pDevice->pDevInfo->AliasLevel;
	     RipStruct.PixHeight /= pDevice->pDevInfo->AliasLevel;	  
	     RipStruct.WPixelsPerInch /= (float)pDevice->pDevInfo->AliasLevel;
         RipStruct.HPixelsPerInch /= (float)pDevice->pDevInfo->AliasLevel;
	     }
	  }

/*
   // if the filetype is BLIST, make the file now
   if (pCQueueFile->FileType == FTYPE_BLIST)
   {
      MakeBatchHeaderFile();
	  csBatchHdrFilename = pCQueueFile->LongName;
   }
   else
   {
*/
	  if ( !csBatchHdrFilename.IsEmpty() )
	     csBatchHdrFilename.Empty();
/*
   }
*/
	  
   //
   // load JobInfo stuff
   //   
   
   // file name   
   if ( pCQueueFile->PS_JPGFileLongName.IsEmpty() )
   {
	   strncpy(JobInfo.LongName, 
		  (const char *)pCQueueFile->LongName, MAX_RIP_STR_LEN);
   }
   else
   {
	   strncpy(JobInfo.LongName, 
		  (const char *)pCQueueFile->PS_JPGFileLongName, MAX_RIP_STR_LEN);
   }

   // set short name
   strncpy(JobInfo.ShortName,
      (const char *)pCQueueFile->ShortName, MAX_RIP_STR_LEN); 
   
   // add ps specific stuff   
   JobInfo.CATHeaderNeeded = pCQueueFile->CATHeaderNeeded;
   JobInfo.StartofValidPS = pCQueueFile->StartofValidPS; 
   JobInfo.RealFileSize = pCQueueFile->RealFileSize;
   
   // who owns the preview?   
   JobInfo.hwndPreviewOwner = cwPreview->m_hWnd;
   
   // who gets reporting messages?
   JobInfo.hFormWin = cwReport->m_hWnd;         
   
   // file type
   JobInfo.FileType = pCQueueFile->FileType; 

   // other stuff
   JobInfo.BitmapEngineMutex = BITMAPENGINEMUTEX;
   
   if (pDevice->pDevInfo && OutputType != RIPSTYLE_PREVIEW && OutputType != RIPSTYLE_HD_PREVIEW)
      {
	  JobInfo.AliasStyle = pDevice->pDevInfo->AliasStyle;
      JobInfo.AliasLevel = pDevice->pDevInfo->AliasLevel;
	  }
   else
      {
   	  JobInfo.AliasStyle = ANTIALIAS_STYLE_NONE;
      JobInfo.AliasLevel = 1;
	  }

   // Quick Print Option for Bitmaps
   JobInfo.QuickPrintImage = FALSE;		// Default
   StartTransformIndex = Transform.StartIndex();   

   if ( pDevice->pDevInfo )
   {
	   // This is a Noritsu Feature BEWARE if you attempt to use this with any other device
	   //
	   if (pDevice->pDevInfo->ScaleImage && pDevice->pDevInfo->CanCropImage > 0 &&
					( OutputType != RIPSTYLE_PREVIEW && OutputType != RIPSTYLE_HD_PREVIEW ) &&
					( StartTransformIndex == SMARTFITTRANSFORM || StartTransformIndex == SMARTCROPTRANSFORM ) )
	   {
		  switch(JobInfo.FileType)
			{
			   case FTYPE_BMP:
				   if ( pDevice->pDevInfo->QuickPrintFType & QUICK_PRINT_BMP )
						JobInfo.QuickPrintImage = TRUE;
				   break;
			   case FTYPE_JFIF:
			   case FTYPE_JTIF:
			   case FTYPE_FPX:
				   if ( pDevice->pDevInfo->QuickPrintFType & QUICK_PRINT_JPEG )
						JobInfo.QuickPrintImage = TRUE;
				   break;
			   case FTYPE_TIFF:
				   if ( pDevice->pDevInfo->QuickPrintFType & QUICK_PRINT_TIFF )
						JobInfo.QuickPrintImage = TRUE;
				   break;
			   case FTYPE_EXIF:
				   if ( pDevice->pDevInfo->QuickPrintFType & QUICK_PRINT_EXIF )
						JobInfo.QuickPrintImage = TRUE;
				   break;

			   default:
				   JobInfo.QuickPrintImage = FALSE;
				   break;
			}

		  // This is a Noritsu Feature BEWARE if you attempt to use this with any other device
		  //
		  if ( pDevice->pDevInfo->ScaleImage && pDevice->pDevInfo->CanCropImage == 2 &&
				JobInfo.QuickPrintImage == TRUE)
		  {
			  // Check the Image Size
			  // Only Quick Print if Size is equal to the Selected Media

			  // Allow for rounding by adjusting +/- .2 inches
			  int iMinHeight	= (int)(((JobInfo.crMediaRect.bottom - ROUNDING_FACTOR) * JobInfo.RipHPixelsPerInch) / 100);
			  int iMaxHeight	= (int)(((JobInfo.crMediaRect.bottom + ROUNDING_FACTOR) * JobInfo.RipHPixelsPerInch) / 100);
			  int iMinWidth		= (int)(((JobInfo.crMediaRect.right - ROUNDING_FACTOR) * JobInfo.RipWPixelsPerInch) / 100);
			  int iMaxWidth		= (int)(((JobInfo.crMediaRect.right + ROUNDING_FACTOR) * JobInfo.RipWPixelsPerInch) / 100);

			  if (( BoundingBox.top >= iMinHeight) &&
					( BoundingBox.top <= iMaxHeight) &&
						( BoundingBox.right >= iMinWidth) &&
							( BoundingBox.right <= iMaxWidth))
			  {
					JobInfo.QuickPrintImage = TRUE;
			  }
			  else
			  {
					JobInfo.QuickPrintImage = FALSE;
			  }
		  }
	   }
   }

	csError.Format("Final Return = %d", iRet);
	return iRet;
}
#endif

void CRip::SetSizing()
   {
   // compute device aspect ratio
   devAspectRatio =
      (double)RipStruct.PixHeight / 
      (double)RipStruct.PixWidth *
      RipStruct.WPixelsPerInch / 
      RipStruct.HPixelsPerInch;

   // if transform is NONE, and it is a bitmap, we must use the actual 
   // pixels of the print device so previews don't fo awry
   StartTransformIndex = Transform.StartIndex();   
   if (StartTransformIndex == NOTRANSFORM && LastFileType == CRIP_FILETYPE_BITMAP)
	  {		 
      PicaWidth = (double)RipStruct.PixWidth * RealWPixelsPerInch /
		          (double)RipStruct.WPixelsPerInch;
      PicaHeight = (double)RipStruct.PixHeight * RealHPixelsPerInch /
		          (double)RipStruct.HPixelsPerInch;
	  }
   else
	  {
      // compute PicaWidth, TrueHeight, and TrueWidth
      PicaWidth = (double)RipStruct.PixWidth * 72. /
	   	       RipStruct.WPixelsPerInch;
      PicaHeight = (double)RipStruct.PixHeight * 72. /
	   	       RipStruct.HPixelsPerInch;
	  }

   TrueWidth = (double)RipStruct.PixWidth / PicaWidth;   
   TrueHeight = (double)RipStruct.PixHeight / PicaHeight;   
   
   // get the ctm array
   GetCTMArray();
   }

void CRip::RipParamsOverride(int PixWidth, int PixHeight, 
   float WPixelsPerInch, float HPixelsPerInch, HWND hwndPreviewOwner, CTransform* pct, BOOL bIndexPrint)
   {
   RipStruct.PixWidth = PixWidth;
   RipStruct.PixHeight = PixHeight;
   RipStruct.WPixelsPerInch = WPixelsPerInch;
   RipStruct.HPixelsPerInch = HPixelsPerInch;

   if ( bIndexPrint )
   {
	  JobInfo.RipPixHeight = RipStruct.PixHeight;
	  JobInfo.RipPixWidth = RipStruct.PixWidth;
	  JobInfo.RipWPixelsPerInch = RipStruct.WPixelsPerInch;
	  JobInfo.RipHPixelsPerInch = RipStruct.HPixelsPerInch;
   }
   
   // re-compute device aspect ratio
   devAspectRatio =
      (double)RipStruct.PixHeight / 
      (double)RipStruct.PixWidth *
      RipStruct.WPixelsPerInch / 
      RipStruct.HPixelsPerInch;

   // compute PicaWidth, TrueHeight, and TrueWidth
   //
   // if transform is NONE, and it is a bitmap, we must use the actual 
   // pixels of the print device so previews don't fo awry
   StartTransformIndex = Transform.StartIndex();
   if (StartTransformIndex == NOTRANSFORM && LastFileType == CRIP_FILETYPE_BITMAP)
	  {		 
      PicaWidth = (double)RipStruct.PixWidth * RealWPixelsPerInch /
		          (double)RipStruct.WPixelsPerInch;
      PicaHeight = (double)RipStruct.PixHeight * RealHPixelsPerInch /
		          (double)RipStruct.HPixelsPerInch;
	  }
   else
	  {
      PicaWidth = (double)RipStruct.PixWidth * 72. /
		          (double)RipStruct.WPixelsPerInch;
      PicaHeight = (double)RipStruct.PixHeight * 72. /
		          (double)RipStruct.HPixelsPerInch;
	  }

   TrueWidth = (double)RipStruct.PixWidth / PicaWidth;   
   TrueHeight = (double)RipStruct.PixHeight / PicaHeight;   
   
   // get the new transform   
   Transform = *pct;       
   
   // re-get the ctm array
   GetCTMArray();   

   JobInfo.hwndPreviewOwner = hwndPreviewOwner;
   }   

int CRip::StartRip()
   {
   int ret;
   
   // set the jobinfo
   pRip->pJobInfo = &JobInfo;

   // get start time
   StartTime = ::GetTickCount();      
   
   // set the rip struct
   pRip->SetRipStruct(&RipStruct); 

   // get end time in case we fail right away   
   CTimeSpan ct(0, 0, 0, 0);
   pCQueueFile->ctsElapsedTime = ct;

   // set our flag
   ThreadStart = TRUE;
   
   // start the rip       
   pRipThread = AfxBeginThread((AFX_THREADPROC)RipThreadProc, 
      (LPVOID)this, 
	  THREAD_PRIORITY_BELOW_NORMAL,	  	  	  	  
	  32000); 
   
//   TRACE("CRip::StartRip() AfxBeginThread ThreadID = %d\n", pRipThread->m_nThreadID);

   // ok for now
   ret = SYN_ERRMSG_ALLOK;

   return(ret);
   }

int CRip::EndRip()
{
	if ( ThreadStart )
	{
//		TRACE("CRip::EndRip() AfxEndThread ThreadID = %d\n", pRipThread->m_nThreadID);

		AfxEndThread(0);

		ThreadStart = FALSE;  
	}

	return 0;
}
   
void CRip::GetCTMArray()													
{      
	CTM_ARRAY BeginCTM, AppendCTM, DeviceCTM;
	int i;

	// get the start index
	StartTransformIndex = Transform.StartIndex();

	// if starting with SmartFit or SmartCrop, do transform 
	// based on bounding box
	if ((StartTransformIndex == SMARTCROPTRANSFORM) || 
					(StartTransformIndex == SMARTFITTRANSFORM))
	{                              
		// convert xform to a ctm    
		XFORMtoCTM(&AppendCTM);

		if (!GetBBoxCTM(&BeginCTM))
		{
			// start with rotation if PS and a landscape device
			if (((pCQueueFile->FileType == FTYPE_PSCRIPT) || 
					(pCQueueFile->FileType == FTYPE_BLIST) || 
						(pCQueueFile->FileType == FTYPE_PDF)) && (devAspectRatio < 1.0))		    
			{
				BeginCTM.val[0] = 0.0F; BeginCTM.val[1] = 1.0F;
				BeginCTM.val[2] = -1.0F; BeginCTM.val[3] = 0.0F;
				BeginCTM.val[4] = (float)PicaWidth; BeginCTM.val[5] = 0.0F;
			}
			else
			{
				BeginCTM.val[0] = 1.0F; BeginCTM.val[1] = 0.0F;
				BeginCTM.val[2] = 0.0F; BeginCTM.val[3] = 1.0F;
				BeginCTM.val[4] = 0.0F; BeginCTM.val[5] = 0.0F;
			}
		}
	}
   else
	{	  
		// start with rotation if PS and a landscape device
		//if (!EPSF && (RipStruct.PixHeight < RipStruct.PixWidth))		    	
		if (!EPSF && (devAspectRatio < 1.0))
		{
			BeginCTM.val[0] = 0.0F; BeginCTM.val[1] = 1.0F;
			BeginCTM.val[2] = -1.0F; BeginCTM.val[3] = 0.0F;
			BeginCTM.val[4] = (float)PicaWidth; BeginCTM.val[5] = 0.0F;
		}
		else
		{
			BeginCTM.val[0] = 1.0F; BeginCTM.val[1] = 0.0F;
			BeginCTM.val[2] = 0.0F; BeginCTM.val[3] = 1.0F;
			BeginCTM.val[4] = 0.0F; BeginCTM.val[5] = 0.0F;
		}

		XFORMtoCTM(&AppendCTM);
	}   

	// multiply matricies
	GetMatrixProduct(&BeginCTM, &AppendCTM, &DeviceCTM);

	AppendCTM.val[0] = (float)TrueWidth;
	AppendCTM.val[1] = 0.0F;
	AppendCTM.val[2] = 0.0F;
	AppendCTM.val[3] = (float)TrueHeight;
	AppendCTM.val[4] = 0.0F;
	AppendCTM.val[5] = 0.0F;

	GetMatrixProduct(&DeviceCTM, &AppendCTM, &RipCTM);

	// load CTM into RipStruct   
	for (i = 0; i < 6; i++)
		RipStruct.InitMatrix.val[i] = RipCTM.val[i];   
}      

/***************************************************************************/
/* GetBBoxCTM                                                              */
/*                                                                         */
/* get CTM based on bounding box                                           */
/***************************************************************************/

   /* the bounding box reflects the height and width of the image
      after it has been placed on the device.  And an !EPSF file
      usually assumes a PORTRAIT device, so:

      if we have a !EPSF image
	 if we have a landscape DEVICE
	    if the BBOX is landscape
	       rotate the image
	    else if the BBOX is portrait
	       don't rotate the image
	 else if we have a portrait DEVICE
	    if the BBOX is landscape
	       don't rotate the image
	    else if the BBOX is portrait
	       rotate the image

       after this is done, we need to fit !EPSF and
       EPSF images to the page.
    */

BOOL CRip::GetBBoxCTM(CTM_ARRAY* OutputArray)
   {   
   CTM_ARRAY BeginCTM, ArrayInput, ArrayInput2, ArrayInput3;   
   double ScaleFactor, xoff, yoff;
   double angle;
   short LandscapeBBOX, LandscapeDevice;
   
   // if no bounding box, return 
   if (BoundingBox.Height() == 0 || BoundingBox.Width() == 0) 
      return FALSE;

   /* set orientation of device */
   //if (RipStruct.PixHeight < RipStruct.PixWidth)
   if (devAspectRatio < 1.0)
   {
      LandscapeDevice = TRUE;
   }
   else
   {
      LandscapeDevice = FALSE;
   }
 
   /* set orientation of BBOX */
   if (abs(BoundingBox.Height()) > abs(BoundingBox.Width()))
   {
      LandscapeBBOX = TRUE;
	  }
   else
   {
      LandscapeBBOX = FALSE;
   }

#ifdef LEADFILE_PS
      /* begin with normal coords */
      ArrayInput2.val[0] = 1.0F; ArrayInput2.val[1] = 0.0F;
      ArrayInput2.val[2] = 0.0F; ArrayInput2.val[3] = 1.0F;
      ArrayInput2.val[4] = 0.0F; ArrayInput2.val[5] = 0.0F;

      /* un-offset the image */
      ArrayInput.val[0] = 1.0F; ArrayInput.val[1] = 0.0F;
      ArrayInput.val[2] = 0.0F; ArrayInput.val[3] = 1.0F;
      ArrayInput.val[4] = -(float)(BoundingBox.left);
      ArrayInput.val[5] = -(float)(BoundingBox.bottom);

      /* get matrix product */
      GetMatrixProduct(&ArrayInput2, &ArrayInput, &ArrayInput3);

      /* for EPS files, see if we have to rotate to get best fit */
      if (min(1.0 / (double)abs(BoundingBox.Height()), devAspectRatio / (double)abs(BoundingBox.Width())) >
//	     min(1.0 / (double)abs(BoundingBox.Width()), devAspectRatio / (double)abs(BoundingBox.Height())) &&	      
//		  (EPSF != 2))		  		  
	     min(1.0 / (double)abs(BoundingBox.Width()), devAspectRatio / (double)abs(BoundingBox.Height())))		  
	     {
			 // for bitmaps!
			 RipStruct.Rotation += 90;   	  

			 /* first, rotate counter-clockwise 90 degrees */
			 angle = 90.0 * PIVALUE / 180.0;
			 ArrayInput.val[0] = (float)(cos(angle)); 
			 ArrayInput.val[1] = (float)(sin(angle));
			 ArrayInput.val[2] = (float)(-sin(angle)); 
			 ArrayInput.val[3] = (float)(cos(angle));
			 ArrayInput.val[4] = 0.0F; 
			 ArrayInput.val[5] = 0.0F;

			 /* get matrix product */
			 GetMatrixProduct(&ArrayInput3, &ArrayInput, &ArrayInput2);

			 /* now move it right by image height */
			 ArrayInput.val[0] = 1.0F; ArrayInput.val[1] = 0.0F;
			 ArrayInput.val[2] = 0.0F; ArrayInput.val[3] = 1.0F;
			 ArrayInput.val[4] = (float)abs(BoundingBox.Height());
			 ArrayInput.val[5] = 0.0F;

			 /* get matrix product */
			 GetMatrixProduct(&ArrayInput2, &ArrayInput, &BeginCTM);

			 /* get scale factor */
			 if (StartTransformIndex == SMARTFITTRANSFORM)
				ScaleFactor = min(PicaWidth * devAspectRatio / (double)abs(BoundingBox.Width()),
   				   PicaWidth / (double)abs(BoundingBox.Height()));
   			 else
   				ScaleFactor = max(PicaWidth * devAspectRatio / (double)abs(BoundingBox.Width()),
				   PicaWidth / (double)abs(BoundingBox.Height()));

			 /* don't do microscopic scaling */
			 if (fabs(ScaleFactor - 1.0) < .001) ScaleFactor = 1.0;

			 /* get offsets */
			 xoff = (PicaWidth - ScaleFactor * (double)abs(BoundingBox.Height())) / 2.;
			 yoff = (PicaWidth * devAspectRatio - ScaleFactor * 
				(double)abs(BoundingBox.Width())) / 2.;
	     }
      else
	     {
			 /* no-rotate matrix */
			 ArrayInput.val[0] = 1.0F; ArrayInput.val[1] = 0.0F;
			 ArrayInput.val[2] = 0.0F; ArrayInput.val[3] = 1.0F;
			 ArrayInput.val[4] = 0.0F; ArrayInput.val[5] = 0.0F;

			 /* get matrix product */
			 GetMatrixProduct(&ArrayInput3, &ArrayInput, &BeginCTM);

			 /* get scale factor */
			 if (StartTransformIndex == SMARTFITTRANSFORM)
				ScaleFactor = min(PicaWidth * devAspectRatio / (double)abs(BoundingBox.Height()),
				   PicaWidth / (double)abs(BoundingBox.Width()));
			 else
				ScaleFactor = max(PicaWidth * devAspectRatio / (double)abs(BoundingBox.Height()),
				   PicaWidth / (double)abs(BoundingBox.Width()));

			 /* don't do microscopic scaling */
			 if (fabs(ScaleFactor - 1.0) < .001) ScaleFactor = 1.0;

			 /* get offsets */
			 xoff = (PicaWidth - ScaleFactor * (double)abs(BoundingBox.Width())) / 2.;
			 yoff = (PicaWidth * devAspectRatio - ScaleFactor *
				(double)abs(BoundingBox.Height())) / 2.;
	     }
#else
   /* if !EPSF, rotate the image based on the algorithm outlined
      above */
   if (!EPSF)
      {
      /* always assume that the image is NOT Landscape, if not, force it */
      if (LandscapeBBOX)
	     {
	     SwapBBoxCoords(BoundingBox);

	     LandscapeBBOX = FALSE;
	     }

      /* start with normal coords */
      ArrayInput2.val[0] = 1.0F; ArrayInput2.val[1] = 0.0F;
      ArrayInput2.val[2] = 0.0F; ArrayInput2.val[3] = 1.0F;
      ArrayInput2.val[4] = 0.0F; ArrayInput2.val[5] = 0.0F;

      /* un-offset the image */
      ArrayInput.val[0] = 1.0F; ArrayInput.val[1] = 0.0F;
      ArrayInput.val[2] = 0.0F; ArrayInput.val[3] = 1.0F;
      ArrayInput.val[4] = -(float)(BoundingBox.bottom);
      ArrayInput.val[5] = -(float)(BoundingBox.left);

      /* get matrix product */
      GetMatrixProduct(&ArrayInput2, &ArrayInput, &ArrayInput3);

      /* rotate the image if constrainsts are met */
      if (LandscapeDevice != LandscapeBBOX)
	     {
	     /* we have found that, empirically, we should rotate one
	        way if we see EPSF, and another way if we don't 
			non-WinSlide should rotate the same way as an EPSF */
	     //if (EPSF || !(ExtraInfo & CQUEUE_EXTRA_WINSLIDE))		 
		 if (1)		 
	        {		 
	        ArrayInput.val[0] = 0.0F; ArrayInput.val[1] = -1.0F;
  	        ArrayInput.val[2] = 1.0F; ArrayInput.val[3] = 0.0F;
     	    ArrayInput.val[4] = 0.0F; ArrayInput.val[5] = (float)abs(BoundingBox.Height());
	        }
	     else
	        {
	        ArrayInput.val[0] = 0.0F; ArrayInput.val[1] = 1.0F;
	        ArrayInput.val[2] = -1.0F; ArrayInput.val[3] = 0.0F;
	        ArrayInput.val[4] = (float)abs(BoundingBox.Width()); 
	        ArrayInput.val[5] = 0.0F;
	        }
	     /* get matrix product */
	     GetMatrixProduct(&ArrayInput3, &ArrayInput, &BeginCTM);
  
    	 /* get scale factor */
    	 if (StartTransformIndex == SMARTFITTRANSFORM)
	        ScaleFactor = min(PicaWidth * devAspectRatio / (double)abs(BoundingBox.Height()),
		       PicaWidth / (double)abs(BoundingBox.Width()));
		 else        
	        ScaleFactor = max(PicaWidth * devAspectRatio / (double)abs(BoundingBox.Height()),
		       PicaWidth / (double)abs(BoundingBox.Width()));		  

    	 /* don't do microscopic scaling */
	     if (fabs(ScaleFactor - 1.0) < .001)
			 ScaleFactor = 1.0;

	     /* get offsets */
	     xoff = (PicaWidth - ScaleFactor * (double)abs(BoundingBox.Width())) / 2.;
	     yoff = (PicaWidth * devAspectRatio -
					ScaleFactor * (double)abs(BoundingBox.Height())) / 2.;
	     }
      else
	     {
	     /* no-rotate matrix */
	     ArrayInput.val[0] = 1.0F; ArrayInput.val[1] = 0.0F;
	     ArrayInput.val[2] = 0.0F; ArrayInput.val[3] = 1.0F;
	     ArrayInput.val[4] = 0.0F; ArrayInput.val[5] = 0.0F;

	     /* get matrix product */
	     GetMatrixProduct(&ArrayInput3, &ArrayInput, &BeginCTM);

	     /* get scale factor */
	     if (StartTransformIndex == SMARTFITTRANSFORM)
 	        ScaleFactor = min(PicaWidth * devAspectRatio / (double)abs(BoundingBox.Width()), 	        
			   PicaWidth / (double)abs(BoundingBox.Height()));
		 else
		    ScaleFactor = max(PicaWidth * devAspectRatio / (double)abs(BoundingBox.Width()),
			   PicaWidth / (double)abs(BoundingBox.Height()));

	     /* don't do microscopic scaling */
	     if (fabs(ScaleFactor - 1.0) < .001)
			 ScaleFactor = 1.0;

	     /* get offsets */
	     xoff = (PicaWidth - ScaleFactor * (double)abs(BoundingBox.Height())) / 2.;
	     yoff = (PicaWidth * devAspectRatio -
	         ScaleFactor * (double)abs(BoundingBox.Width())) / 2.;
     	 }
      }
   else
      {
      /* begin with normal coords */
      ArrayInput2.val[0] = 1.0F; ArrayInput2.val[1] = 0.0F;
      ArrayInput2.val[2] = 0.0F; ArrayInput2.val[3] = 1.0F;
      ArrayInput2.val[4] = 0.0F; ArrayInput2.val[5] = 0.0F;

      /* un-offset the image */
      ArrayInput.val[0] = 1.0F; ArrayInput.val[1] = 0.0F;
      ArrayInput.val[2] = 0.0F; ArrayInput.val[3] = 1.0F;
      ArrayInput.val[4] = -(float)(BoundingBox.left);
      ArrayInput.val[5] = -(float)(BoundingBox.bottom);

      /* get matrix product */
      GetMatrixProduct(&ArrayInput2, &ArrayInput, &ArrayInput3);

      /* for EPS files, see if we have to rotate to get best fit */
      if (min(1.0 / (double)abs(BoundingBox.Height()), devAspectRatio / (double)abs(BoundingBox.Width())) >
//	     min(1.0 / (double)abs(BoundingBox.Width()), devAspectRatio / (double)abs(BoundingBox.Height())) &&	      
//		  (EPSF != 2))		  		  
	     min(1.0 / (double)abs(BoundingBox.Width()), devAspectRatio / (double)abs(BoundingBox.Height())))		  
	     {
			 // for bitmaps!
			 RipStruct.Rotation += 90;   	  

			 /* first, rotate counter-clockwise 90 degrees */
			 angle = 90.0 * PIVALUE / 180.0;
			 ArrayInput.val[0] = (float)(cos(angle)); 
			 ArrayInput.val[1] = (float)(sin(angle));
			 ArrayInput.val[2] = (float)(-sin(angle)); 
			 ArrayInput.val[3] = (float)(cos(angle));
			 ArrayInput.val[4] = 0.0F; 
			 ArrayInput.val[5] = 0.0F;

			 /* get matrix product */
			 GetMatrixProduct(&ArrayInput3, &ArrayInput, &ArrayInput2);

			 /* now move it right by image height */
			 ArrayInput.val[0] = 1.0F; ArrayInput.val[1] = 0.0F;
			 ArrayInput.val[2] = 0.0F; ArrayInput.val[3] = 1.0F;
			 ArrayInput.val[4] = (float)abs(BoundingBox.Height());
			 ArrayInput.val[5] = 0.0F;

			 /* get matrix product */
			 GetMatrixProduct(&ArrayInput2, &ArrayInput, &BeginCTM);

			 /* get scale factor */
			 if (StartTransformIndex == SMARTFITTRANSFORM)
				ScaleFactor = min(PicaWidth * devAspectRatio / (double)abs(BoundingBox.Width()),
   				   PicaWidth / (double)abs(BoundingBox.Height()));
   			 else
   				ScaleFactor = max(PicaWidth * devAspectRatio / (double)abs(BoundingBox.Width()),
				   PicaWidth / (double)abs(BoundingBox.Height()));

			 /* don't do microscopic scaling */
			 if (fabs(ScaleFactor - 1.0) < .001) ScaleFactor = 1.0;

			 /* get offsets */
			 xoff = (PicaWidth - ScaleFactor * (double)abs(BoundingBox.Height())) / 2.;
			 yoff = (PicaWidth * devAspectRatio - ScaleFactor * 
				(double)abs(BoundingBox.Width())) / 2.;
	     }
      else
	     {
	     /* no-rotate matrix */
	     ArrayInput.val[0] = 1.0F; ArrayInput.val[1] = 0.0F;
	     ArrayInput.val[2] = 0.0F; ArrayInput.val[3] = 1.0F;
	     ArrayInput.val[4] = 0.0F; ArrayInput.val[5] = 0.0F;

	     /* get matrix product */
	     GetMatrixProduct(&ArrayInput3, &ArrayInput, &BeginCTM);

	     /* get scale factor */
	     if (StartTransformIndex == SMARTFITTRANSFORM)
            ScaleFactor = min(PicaWidth * devAspectRatio / (double)abs(BoundingBox.Height()),
               PicaWidth / (double)abs(BoundingBox.Width()));
         else
            ScaleFactor = max(PicaWidth * devAspectRatio / (double)abs(BoundingBox.Height()),
               PicaWidth / (double)abs(BoundingBox.Width()));

	     /* don't do microscopic scaling */
	     if (fabs(ScaleFactor - 1.0) < .001) ScaleFactor = 1.0;

	     /* get offsets */
	     xoff = (PicaWidth - ScaleFactor * (double)abs(BoundingBox.Width())) / 2.;
	     yoff = (PicaWidth * devAspectRatio - ScaleFactor *
	        (double)abs(BoundingBox.Height())) / 2.;
	     }
#if 0
			 /* no-rotate matrix */
			 ArrayInput.val[0] = 1.0F; ArrayInput.val[1] = 0.0F;
			 ArrayInput.val[2] = 0.0F; ArrayInput.val[3] = 1.0F;
			 ArrayInput.val[4] = 0.0F; ArrayInput.val[5] = 0.0F;

			 /* get matrix product */
			 GetMatrixProduct(&ArrayInput3, &ArrayInput, &BeginCTM);

			 /* get scale factor */
			 if (StartTransformIndex == SMARTFITTRANSFORM)
				ScaleFactor = min(PicaWidth * devAspectRatio / (double)abs(BoundingBox.Height()),
				   PicaWidth / (double)abs(BoundingBox.Width()));
			 else
				ScaleFactor = max(PicaWidth * devAspectRatio / (double)abs(BoundingBox.Height()),
				   PicaWidth / (double)abs(BoundingBox.Width()));

			 /* don't do microscopic scaling */
			 if (fabs(ScaleFactor - 1.0) < .001) ScaleFactor = 1.0;

			 /* get offsets */
			 xoff = (PicaWidth - ScaleFactor * (double)abs(BoundingBox.Width())) / 2.;
			 yoff = (PicaWidth * devAspectRatio - ScaleFactor *
				(double)abs(BoundingBox.Height())) / 2.;

	     }
#endif
      }
#endif
   /* now scale it */
   ArrayInput.val[0] = (float)ScaleFactor;
   ArrayInput.val[1] = 0.0F; ArrayInput.val[2] = 0.0F;
   ArrayInput.val[3] = ArrayInput.val[0];
   ArrayInput.val[4] = 0.0F; ArrayInput.val[5] = 0.0F;

   /* get matrix product */
   GetMatrixProduct(&BeginCTM, &ArrayInput, &ArrayInput3);

   /* now center the image */
   ArrayInput.val[0] = 1.0F; 
   ArrayInput.val[1] = 0.0F;
   ArrayInput.val[2] = 0.0F; 
   ArrayInput.val[3] = 1.0F;
   ArrayInput.val[4] = (float)xoff;
   ArrayInput.val[5] = (float)yoff;

   /* get matrix product */
   GetMatrixProduct(&ArrayInput3, &ArrayInput, OutputArray);

   return TRUE;
   }

void CRip::SwapBBoxCoords(CRect& bbox)
   {
   CRect BoxTemp;

   BoxTemp = bbox;

   bbox.left = BoxTemp.bottom;
   bbox.bottom = BoxTemp.left;
   bbox.right = BoxTemp.top;
   bbox.top = BoxTemp.right;
   }

/***************************************************************************/
/*                           GetMatrixProduct                              */
/*                                                                         */
/* multiply two matricies and store product in third matrix                */
/*                                                                         */
/* | a    b    0 |        | a1   b1   0 |                                  */
/* | c    d    0 |    X   | c1   d1   0 |                                  */
/* | tx   ty   1 |        | tx1  ty1  1 |     results in:                  */
/*                                                                         */
/* | a * a1 + b * c1          a * b1 + b * d1          0 |                 */
/* | c * a1 + d * c1          c * b1 + d * d1          0 |                 */
/* | tx * a1 + ty * c1 + tx1  tx * b1 + ty * d1 + ty1  1 |                 */
/*                                                                         */
/***************************************************************************/

void CRip::GetMatrixProduct(CTM_ARRAY *first, CTM_ARRAY *second, CTM_ARRAY *product)
   {
   product->val[0] =  first->val[0] * second->val[0] +
		      first->val[1] * second->val[2];

   product->val[1] =  first->val[0] * second->val[1] +
		      first->val[1] * second->val[3];

   product->val[2] =  first->val[2] * second->val[0] +
		      first->val[3] * second->val[2];

   product->val[3] =  first->val[2] * second->val[1] +
		      first->val[3] * second->val[3];

   product->val[4] =  first->val[4] * second->val[0] +
		      first->val[5] * second->val[2] +
		      second->val[4];

   product->val[5] =  first->val[4] * second->val[1] +
		      first->val[5] * second->val[3] +
		      second->val[5];
   }

BOOL CRip::GetDeviceError(CString& ErrStr)
   {
   // get device error
   return pRip->GetDevError(ErrStr);
   }      
      
BOOL CRip::GetDevErrorFromRip(CString& ErrStr)
   {
   // get device error
   return pRip->GetDevErrorFromRip(ErrStr);
   }      
      
/***************************************************************************/
/* XFORMtoCTM                                                              */
/*                                                                         */
/* convert RasterPlus95 XFORM data to a PostScript CTM array			   */
/***************************************************************************/

#define PIVALUE  3.141592654

void CRip::XFORMtoCTM(CTM_ARRAY *Array)
   {
   CTM_ARRAY first, second, Product;
   double angle;   
   double iHeight, iWidth;
   double xorg, yorg, xnew, ynew, radius;
   float xoff, yoff;

   // convert to radians
   angle = atof(Transform.Rotate) * PIVALUE / 180.0;   

   // special for bitmaps
   RipStruct.Rotation = atoi((const char*)Transform.Rotate);     
   
   // height and width of page   
   if (StartTransformIndex == NOTRANSFORM && LastFileType == CRIP_FILETYPE_BITMAP)
	  {		 
      iWidth = (double)RipStruct.PixWidth * RealWPixelsPerInch /
         RipStruct.WPixelsPerInch;
      iHeight = (double)RipStruct.PixHeight * RealHPixelsPerInch /
         RipStruct.HPixelsPerInch;
	  xoff = ((float)atof(Transform.XOff)) * RealWPixelsPerInch / 72.F;   
	  yoff = ((float)atof(Transform.YOff)) * RealHPixelsPerInch / 72.F;   
	  }
   else
	  {		 
      iWidth = (double)RipStruct.PixWidth * 72. /
         RipStruct.WPixelsPerInch;
      iHeight = (double)RipStruct.PixHeight * 72. /
         RipStruct.HPixelsPerInch;
	  xoff = (float)atof(Transform.XOff);
	  yoff = (float)atof(Transform.YOff);   
	  }
   // center point of page
   xorg = iWidth / 2;
   yorg = iHeight / 2;
   // distance from center point to origin
   radius = sqrt(yorg * yorg + xorg * xorg);

   // compute where the center will be after we rotate it
   xnew = cos(angle + asin(yorg / radius)) * radius;
   ynew = sin(angle + asin(yorg / radius)) * radius;

   // array #1 is the rotation
   first.val[0] = (float)cos(angle);
   first.val[1] = (float)sin(angle);
   first.val[2] = (float)(-sin(angle));
   first.val[3] = (float)cos(angle);
   first.val[4] = 0.0F;
   first.val[5] = 0.0F;

   // array #2 moves the center of the image back to the 
   // original point
   second.val[0] = 1.0F;
   second.val[1] = 0.0F;
   second.val[2] = 0.0F;
   second.val[3] = 1.0F;
   second.val[4] = (float)(xorg - xnew);
   second.val[5] = (float)(yorg - ynew);
   
   GetMatrixProduct(&first, &second, &Product);   
   first = Product;				   

   // now add zooms and offsets
   second.val[0] = (float)atof(Transform.XZoom);
   second.val[1] = 0.0F;
   second.val[2] = 0.0F;
   second.val[3] = (float)atof(Transform.YZoom);
   second.val[4] = xoff;
   second.val[5] = yoff;

   GetMatrixProduct(&first, &second, Array);   
   }

int CRip::MakeBatchHeaderFile()
   {
   int i;   
   CString cs1, cs2, TimeString;
   CStdioFile cf;                     
   CTime TimeNow;
   CProfile Profile;
   
   // get notes
   if(pCQueueFile->BatchHeaderNotes.GetLength()) 
      {                                            
      // if there is a tab, extract the first part                                             
      int TabPos;
      if ((TabPos = pCQueueFile->BatchHeaderNotes.Find("\t")) > 0)      
         {
         cs1 = pCQueueFile->BatchHeaderNotes.Left(TabPos);
         cs2 = 
            pCQueueFile->BatchHeaderNotes.Right(pCQueueFile->BatchHeaderNotes.GetLength() - TabPos - 1);
         }
      else
         {   
         cs1 = pCQueueFile->BatchHeaderNotes;
         cs2 = _T("");
         }
      }           
   
   // get temp filename
   // if the temp path has not been set yet, get it from
   // the registry
   GetTempFileName(csScratchPath, "img", 0, 
            pCQueueFile->LongName.GetBuffer(_MAX_PATH));      
   pCQueueFile->LongName.ReleaseBuffer();

   // open the temp file and write the batch header
   if (cf.Open(pCQueueFile->LongName, CFile::modeWrite | CFile::typeBinary))
      {
      for (i = 0; i < BATCHHEADER_NUMLINES; i++)
	     {
	     switch(i)
	        { 
	        // special cases for info
	        case BATCHHEADER_LINE1:	             
	             if (cs1.GetLength())
	                {
	                StrToPSStr(cs1);	             	        
		            fprintf(cf.m_pStream, "%s%s", (const char *)cs1, 
		               BatchHeaderString1);
		            }   
		         break;
	        case BATCHHEADER_LINE1 + 1:
	             if (cs1.GetLength())
	                {
	                fprintf(cf.m_pStream, "%s%s", (const char *)cs1, 
		               BatchHeaderString2);
		            }   
		         break;
	        case BATCHHEADER_LINE2:                
	             if (cs2.GetLength())
	                {   	           
	                StrToPSStr(cs2);
		            fprintf(cf.m_pStream, "%s%s", (const char *)cs2, 
		               BatchHeaderString1);
		            }   
		         break;
	        case BATCHHEADER_LINE2 + 1:	             
	             if (cs2.GetLength())
	                {	           
		            fprintf(cf.m_pStream, "%s%s", (const char *)cs2, 
		               BatchHeaderString2);
		            }   
		         break;
	        case BATCHHEADER_LINE3:  
	             if (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_COUNT)
		            fprintf(cf.m_pStream, "(%d Files in Job)%s",
		               pCQueueFile->BatchHeaderCount, BatchHeaderString1);
	  	         break;
	        case BATCHHEADER_LINE3 + 1:
	             if (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_COUNT)
		            fprintf(cf.m_pStream, "(%d Files in Job)%s",
		               pCQueueFile->BatchHeaderCount, BatchHeaderString2);		         
		         break;
	        case BATCHHEADER_LINE4:
		         // get date, time
		         if (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_DATE)
		            {
		            TimeNow = CTime::GetCurrentTime();
                    TimeString = TimeNow.Format("%A, %B %d, %Y");   
                    fprintf(cf.m_pStream, "(%s)%s", (const char*)TimeString,
		               BatchHeaderString1);
		            }   
		         break;
	        case BATCHHEADER_LINE4 + 1:
	             if (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_DATE)
	                {
		            fprintf(cf.m_pStream, "(%s)%s", (const char*)TimeString,
		               BatchHeaderString2);
		            }   
		         break;
	        case BATCHHEADER_LINE5:
		         // get date, time
		         if (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_TIME)
		            {
		            TimeNow = CTime::GetCurrentTime();
                    TimeString = TimeNow.Format("%H:%M:%S");   
                    fprintf(cf.m_pStream, "(%s)%s", (const char*)TimeString,
		               BatchHeaderString1);
		            }   
		         break;
	        case BATCHHEADER_BACKCOLOR:
		         // background color
		         if (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_BACKWHITE)
		            fprintf(cf.m_pStream, "%s", BatchHeaderBackWhite);
		         else if (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_BACKBLACK)
		            fprintf(cf.m_pStream, "%s", BatchHeaderBackBlack);
		         break;
            case BATCHHEADER_TEXTCOLOR:
			     // if a black background, use white text
		         if (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_BACKBLACK)
		            fprintf(cf.m_pStream, "%s", BatchHeaderTextWhite);
				 else
					fprintf(cf.m_pStream, "%s", BatchHeaderTextRed);
			     break;
	        case BATCHHEADER_LINE5 + 1:
	             if (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_TIME)
	                {
	                fprintf(cf.m_pStream, "(%s)%s", (const char*)TimeString,
		               BatchHeaderString2);
		            }   
		         break;
	        default:
	             // don't output corner stuff if this flag is not set
	             if (!(pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_CORNERS))
	                {
	                if (i >= BATCHHEADER_STARTCORNER && i <=
                        BATCHHEADER_ENDCORNER)
                        break;
                    }
		         // don't output background if flag is set
	             if (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_BACKNONE)
	                {
	                if (i >= BATCHHEADER_STARTBACK && i <= BATCHHEADER_ENDBACK)
                        break;
                    }
		         fprintf(cf.m_pStream, "%s", BatchHeaderText[i]);
		         break;
	        }
	     // carriage return
	     fprintf(cf.m_pStream, "\r\n");
	     }
      cf.Close();

	  // get file size, time etc...
      HANDLE hFile;
      WIN32_FIND_DATA FileInfo;
      if ((hFile = FindFirstFile((const char*)pCQueueFile->LongName,
         &FileInfo)) != INVALID_HANDLE_VALUE)
	     {      
	     // file time
	     ((CZeusApp*)AfxGetApp())->GetFileTime(&FileInfo, pCQueueFile->ctFileTime);
	     // file size (assuming size less than MAXDWORD)
	     pCQueueFile->RealFileSize = pCQueueFile->iFileSize = FileInfo.nFileSizeLow;
         // close the handle
         FindClose(hFile);   
	     }	  
      return TRUE;
      }
   else
      return FALSE;
   }   

void CRip::StrToPSStr(CString& cs)
   {
   int j;            
   char* pTempString;
   char* pThisString;
   char TempChar;    
   
   // must have length!
   if (cs.GetLength() <= 0) return;
   
   // this string could be 2x long + leading ( and trailing )
   pThisString = pTempString = new char[cs.GetLength() * 2 + 2];   
   
   *pTempString++ = '(';   
   for (j = 0; j < cs.GetLength(); j++)   
      {
      *pTempString = cs[j];
      // special chars
      if ((*pTempString == '(') || (*pTempString == ')') || (*pTempString == '\\'))
	     {
	     TempChar = *pTempString;
	     *pTempString++ = '\\';
	     *pTempString = TempChar;
	     }           
	  pTempString++;         	  
      }    
   
   *pTempString++ = ')';   
   *pTempString = '\0';
   
   // make new string
   cs = pThisString;
   
   // delete the buffer
   delete pThisString;
   }
   
void CRip::CancelProcess(BOOL bAll)
   {
   // set cancel flag now
   if (bAll)
	  pRip->DevProcessFlag = DEVICE_CANCELALL_PROCESS;
   else
	  pRip->DevProcessFlag = DEVICE_CANCEL_PROCESS;
   // cancel it
   pRip->CancelProcess(bAll);

   // give up some time to receive a ripclose
   int LastTickCount;
   MSG msg;
   BOOL bGotMsg = FALSE;
   // now wait 2 seconds for rip complete message
   LastTickCount = ::GetTickCount();
   do 
	  {
	  if (PeekMessage (&msg, JobInfo.hFormWin, 0, 0, PM_REMOVE)) 
		 {
		 // got it
	     if (msg.message == wm_RipComplete) 				  
			{
//			TRACE("Peeked at wm_RipComplete message\n");
			bGotMsg = TRUE;
			}
	     TranslateMessage (&msg);
	     DispatchMessage (&msg);
		 }
	  } while (((::GetTickCount() -  LastTickCount) < 2000) && !bGotMsg);
   }

void CRip::PauseProcess()
   {
   // try suspending, if return is 
   // false, it is a thread that needs
   // to be suspended directly
   if (!pRip->SuspendProcess())
      pRipThread->SuspendThread();
   }

void CRip::ResumeProcess()
   {
   // try resuming, if return is 
   // false, it is a thread that needs
   // to be suspended directly
   if (!pRip->ResumeProcess())
      pRipThread->ResumeThread();
   }
				   
static UINT ErrMsgLookup[] = 
   {
   SYN_ERRMSG_ALLOK, IDS_SYN_ERRMSG_ALLOK, 
   SYN_ERRMSG_CANCELALL, IDS_SYN_ERRMSG_CANCELIMAGE,   
   SYN_ERRMSG_CANCELIMAGE, IDS_SYN_ERRMSG_CANCELIMAGE,   
   SYN_ERRMSG_RIPNOTFOUND, IDS_SYN_ERRMSG_RIPNOTFOUND,
   SYN_ERRMSG_RIPDLLNOTFOUND, IDS_SYN_ERRMSG_RIPDLLNOTFOUND,   
   SYN_ERRMSG_BADFORMAT, IDS_SYN_ERRMSG_BADFORMAT,
   SYN_ERRMSG_RIPTHRDBAD, IDS_SYN_ERRMSG_RIPTHRDBAD,   
   SYN_ERRMSG_FILENOTFOUND, IDS_SYN_ERRMSG_FILENOTFOUND,
   SYN_ERRMSG_FILEWASDELETED, IDS_SYN_ERRMSG_FILEWASDELETED,
   SYN_ERRMSG_NOROTATE_BMP, IDS_SYN_ERRMSG_NOROTATE_BMP,
   SYN_ERRMSG_NODEFAULTPROFILE, IDS_SYN_ERRMSG_NODEFAULTPROFILE,
   SYN_ERRMSG_INVALIDPROFILE, IDS_SYN_ERRMSG_INVALIDPROFILE,
   SYN_ERRMSG_DEVINIT, IDS_SYN_ERRMSG_DEVINIT,
   SYN_ERRMSG_UNSUPPORTEDFORMAT, IDS_SYN_ERRMSG_UNSUPPORTEDFORMAT,
   SYN_ERRMSG_UNDEFINED, IDS_SYN_ERRMSG_UNDEFINED
   };
                                  
void CRip::MapErrorReturn(UINT val, CString& cs)
   {  
   int i;           

   // default
   cs.LoadString(IDS_SYN_ERRMSG_UNDEFINED);

   // check for device errors     
   if (val == SYN_ERRMSG_DEVICEERROR)
   {
      // get the device error from the crip class      
      GetDeviceError(cs);
      }
   // check for device errors from rip
   else if (val == SYN_ERRMSG_RIPDEVICEERROR)
      {   
      // get the device error from the crip class      
      GetDevErrorFromRip(cs);
      }
   else       
      { 
	  // look-up the error string
	  i = 0;
	  while (ErrMsgLookup[i] != SYN_ERRMSG_UNDEFINED)
	     {
		 if (val == ErrMsgLookup[i])
		    {
			cs.LoadString(ErrMsgLookup[i + 1]);			
			break;
			}
		 i += 2;
		 }      
      }
   }

DWORD _cdecl RipThreadProc(LPVOID pParam)
   {
   int ret;   
   CString cs;

   // cast the pointer to our rip
   CRip* cRip = (CRip*)pParam;

   // check validity
   if (cRip->pRip == NULL)
	   return SYN_ERRMSG_RIPTHRDBAD;

   // rip it
   ret = cRip->pRip->Process(); 
   
   // we're done
   // note: we can get this return code by using ::GetExitCodeThread
   PostMessage(cRip->pRip->pJobInfo->hFormWin, wm_RipComplete, ret, 0L);

   return (DWORD)ret;   
   }

void CRip::OptimizeBitmapOutput(CRect& BoundingBox, CDevice* pDevice)
   {
   int BitHeight, BitWidth;
   int MaxHeight, MaxWidth;
   int OrigWidth, OrigHeight;
   double bitAspectRatio;
   double devAspectRatio;

   OrigWidth = RipStruct.PixWidth;
   OrigHeight = RipStruct.PixHeight;

   if (RipStruct.Rotation == 0 || RipStruct.Rotation == 180)
      {
	  BitHeight = abs(BoundingBox.Height());
	  BitWidth = abs(BoundingBox.Width());
	  }
   else
      {
      BitHeight = abs(BoundingBox.Width());
      BitWidth = abs(BoundingBox.Height());
	  }		 
   MaxHeight = RipStruct.MaxScaleHeight;
   MaxWidth = RipStruct.MaxScaleWidth;

   /* get input bitmap aspect ratio */
   bitAspectRatio = (double)(BitHeight) / (double)(BitWidth);

   /* get output bitmap (device) aspect ratio */
   devAspectRatio = (double)(RipStruct.PixHeight) / (double)(RipStruct.PixWidth);

   if (bitAspectRatio > devAspectRatio)
      {
	  RipStruct.PixHeight = min(max(BitHeight, (int)pDevice->pDevInfo->MinScaleHeight),
	     MaxHeight);
	  RipStruct.PixWidth = (short)((double)RipStruct.PixHeight / devAspectRatio);
	  /* make sure that RipStruct.PixWidth is within bounds */
	  if (RipStruct.PixWidth < (int)pDevice->pDevInfo->MinScaleWidth)
	     {
	     RipStruct.PixWidth = (int)pDevice->pDevInfo->MinScaleWidth;
	     RipStruct.PixHeight = (short)((double)RipStruct.PixWidth * devAspectRatio);
	     }
	  if (RipStruct.PixWidth > MaxWidth)
	     {
	     RipStruct.PixWidth = MaxWidth;
	     RipStruct.PixHeight = (short)((double)RipStruct.PixWidth * devAspectRatio);
	     }
	  }
   else
      {
	  RipStruct.PixWidth = min(max(BitWidth, (int)pDevice->pDevInfo->MinScaleWidth),
	     MaxWidth);
	  RipStruct.PixHeight = (short)((double)RipStruct.PixWidth * devAspectRatio);
	  /* make sure that RipStruct.PixHeight is within bounds */
	  if (RipStruct.PixHeight < (int)pDevice->pDevInfo->MinScaleHeight)
	     {
	     RipStruct.PixHeight = (int)pDevice->pDevInfo->MinScaleHeight;
	     RipStruct.PixWidth = (short)((double)RipStruct.PixHeight / devAspectRatio);
	     }
	  if (RipStruct.PixHeight > MaxHeight)
	     {
	     RipStruct.PixHeight = MaxHeight;
	     RipStruct.PixWidth = (short)((double)RipStruct.PixHeight / devAspectRatio);
	     }
	  }
   RipStruct.WPixelsPerInch = (float)RipStruct.PixWidth * RipStruct.WPixelsPerInch / (float)OrigWidth;
   RipStruct.HPixelsPerInch = (float)RipStruct.PixHeight * RipStruct.HPixelsPerInch / (float)OrigHeight;
   }

#define PRANGE_LASTCHAR_NONE    0
#define PRANGE_LASTCHAR_SPACE   1
#define PRANGE_LASTCHAR_COMMA   2
#define PRANGE_LASTCHAR_DASH    3
#define PRANGE_LASTCHAR_NUMBER  4

int CRip::ParsePageRangeSelection(CWordArray& cw)
   {   
   int i, j;
   char pTempBuf[_MAX_PATH];
   char c;
   char *p;   
   CStringArray csa;
   CString cs;
   int iStartPage, iEndPage;
   int iPageCount = 0;

   // reset the string array
   csa.RemoveAll();

   // zero out the temp buf
   ZeroMemory(pTempBuf, _MAX_PATH);

   // make sure that we have all valid chars: 0-9, comma, dash
   for (i = 0; i < min(_MAX_PATH, pCQueueFile->m_prange_edit.GetLength()); i++)
	  {
	  c = pCQueueFile->m_prange_edit[i];

	  if ((c >= '0' && c <= '9') || c == ',' || c == '-' || c == 0)
		 pTempBuf[i] = c;
	  }

   // nothing left?
   if (strlen(pTempBuf) < 1) return 0;

   // now look for commas
   p = strtok(pTempBuf, ",");

   do
	  {
      // nothing left?
      if (strlen(p) < 1) 
	     break;
      else
		 {
	     cs = p;
	     csa.Add(cs);
		 }
	  } while (p = strtok(NULL, ","));

   // nothing added?
   if (csa.GetSize() < 1) return 0;

   // now process the comma tokens
   for (i = 0; i < csa.GetSize(); i++)
	  {
	  cs = csa.GetAt(i);
	  strcpy(pTempBuf, cs);

      // and look for dashes
      p = strtok(pTempBuf, "-");

	  if (p)
		 {
		 iStartPage = atoi(p);	  
		 if (iStartPage > 0) 
			{
			// add this page
			// if the sub page flag is set, only do the specified sub page
			if (!pCQueueFile->bDoSubPage || 
			   (pCQueueFile->bDoSubPage && (pCQueueFile->iSubPage == iPageCount)))
			   cw.Add(iStartPage - 1); // 0 based
			iPageCount++;
			// to?
			p = strtok(NULL, "-");
			if (p)
			   {
			   iEndPage = atoi(p);
			   if (iEndPage > 0)
				  {
				  for (j = iStartPage + 1; j <= iEndPage; j++)
					 {		 
					 if (!pCQueueFile->bDoSubPage || 
						(pCQueueFile->bDoSubPage && (pCQueueFile->iSubPage == iPageCount)))
						cw.Add(j - 1);
					 iPageCount++;
					 }
				  }
			   }
			}   
		 }
	  }
   csa.RemoveAll();
   return cw.GetSize();
   }

