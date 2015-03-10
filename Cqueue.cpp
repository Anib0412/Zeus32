/////////////////////////////////////////////////////////////////////////////
// CQUEUE.CPP
//                                                                        
// define and manipulate files and Queues for RasterPlus
// Copyright (C) 1996-2000 Graphx, Inc.
//
// mods:
//
// 7-18-96  Build 40 - Added special hack to retrieve correct ViewPerspective from
//					      Targa file because of LEADTOOLS bug
// 10-02-96 Build 43 - Fixed PhotoShop PSD support
// 11-05-96 Build 45 - add server file flag to ExtraInfo
// 12-24-96 Build 47 - added new boundingbox logic to server file parsing
// 1-13-97  Build 47 - changed PSMAXTEXTLINES from 50 to 100 to account for 
//                        new WinSlide stuff
// 2-7-97   Build 48 - new bbox stuff and removed empirical pica check on mac files
// 8-7-97   Build 50 - pm 6.5 fixes - change NUMLINESTOTEST from 1000 to 2000, 
//                        nuke starting ctrl-d
// 8-18-97  Build 51 - put back in MAC hack with stricter checking
// 9-02-97  Build 51 - integrated lead 80 with FlashPix
// 9-19-97  Build 51 - added more tiff support
// 9-19-97  Build 51 - seek past EPS header to get PostScript info
// 10-02-97 Build 51 - get specual bbox stuff for pm 6.5 on non-server files
// 10-21-97 Build 52 - added new server stuff
// 11-12-97 Build 53 - wasn't checking for NumGot in GetServerFileInfo; files could
//                        be smaller than the line check (this bug might have been
//                        introduced in Build 52) - fixed.
// 11-16-97 Version 2.0 - changed logic of parsing device name - we now use 
//                           SERVERXFER (duh!)
// 11-20-97 Version 2.0 - some JPEGs reporting as Unknown with Lead 8.0 - fixed
// 11-23-97 Version 2.0 - don't wait until FoundEPSF goes true in GetPostScriptInfo
//                           this takes too long and is normally at the start of the
//                           file anyway
// 11-23-97 Version 2.0 - added methods for determining optimum orientation of files
// 11-23-97 Version 2.0 - added continue statements to parser to speed things up
// 12-09-97 Version 2.0.1 - account for blank line in MAC file server parsing
// 1-17-98  Version 2.0.1 - multiple devices with the same port would parse wrong - fixed
//                             in RPLNGMON (documented here)
// 1-26-98  Version 2.0.1 - printing to an alias would fail under NT - fixed
// 3-05-98  Version 2.0.2 - EPSF files from PhotoShop not rotating properly - fixed
// 6-17-98  Version 2.0.4 - increased NUMLINESTOTEST from 2000 to 4000 for Laserwriter 8 driver?
// 6-26-98  Version 2.0.4 - new StatusFlag stuff caused us to change
//                          if (StatusFlag == FILE_STATUS_INRIP)  to
//                          if ((StatusFlag & FILE_STATUS_INRIP) == FILE_STATUS_INRIP)
// 12-21-98 Version 2.0.6 - added QuarkXPress pagesize hack
// 1-15-99  Version 2.0.6 - added PDF
// 1-28-99  Version 2.0.6 - removed MAC PICT
// 5-18-99  Version 3.0.0 - support multi pages in bitmaps
// 10-05-99 Version 3.0.1 - if pServerXfer->pDriverName doesn't exist, use printer name
// 1-13-00  Version 3.0.3 - use the first bounding box we find in a server file parse
// 1-27-00  Version 3.0.3 - remove DoTargaHack which was incorrect and i think unecessary
//                          with the latest lead tools
// 4-13-00  Version 3.0.3 - possibly copying nul transform, color or device prefs because
//                          we weren't checking Prefs flag
// 7-18-00  Version 4.0.0   added SGI format
// 7-18-00  Version 4.0.0   weren't checking for "atend" in BoundingBox which caused 
//                             Win2000 File Print Jobs to have a 0 0 0 0 bounding box
// 10-12-00 Version 4.0.1 - Implement late-loading device strategy for faster queue loading
// 11-01-00 Version 4.0.1 - add support for other flashpix formats
// 11-07-00 Version 4.0.1 - increase NUMLINESTOTEST for Win2k PDF FilePrint problem
// 01-08-01 Version 4.0.2 - Fix Print Range processing ( Bug #137 )
// 04-02-01 Version 4.0.3 - Remove LineCount restrictions from PDF parsing
//                          Add support for hex and Unicode PDF Titles 
// 08-20-01 Version 5.0.0 - Add code to reject CMYK Tif files as Unsupported files
// 01-10-02 Version 5.0.1 - Fix FilePrining not adding files to a second queue for the same device (WindowsNT problem)
// 10-30-02 Version 5.1.0 - Fix Random orientation problem with PDF file uninitialized field - EPSF
// 02-14-03 Version 5.1.1 - Fix number of copies set incorrectly for file printing
// 02-27-03 Version 5.1.1 - Add look up for HiRes Bounding Box
//

#include "stdafx.h" 
#include "zeus.h"
#include "zeusdoc.h"
#include "cqueue.h"
#include <dos.h>    
#include <string.h>
#include <stdlib.h>
#include "fstream.h"
#include "servdefs.h"
#include "ftypes.h"

#include "..\CmnLibs\Current LeadTools\include\l_bitmap.h"
#include "..\CmnLibs\Current LeadTools\include\l_error.h"
#include "..\CmnLibs\Current LeadTools\include\Classlib\LtWrappr.h"

#include "nformat.h"
#include <ctype.h>
#include "ps_lookup.h"
#include "winspool.h"
#include "SpecialTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CQueueFile, CObject, 0)
//IMPLEMENT_SERIAL(CQueue, CObArray, 0)

//#define NUMLINESTOTEST  2000
//#define NUMLINESTOTEST  4000
//#define NUMLINESTOTEST  5000
#define NUMLINESTOTEST  10000
#define PDFLINESTOTEST  10000

CQueueFile::CQueueFile()	   
   {          
   // null out device        
   pCD = NULL;
   // null out transform
   pTransform = NULL;  
   // and color
   pCC = NULL;
   // defaults
   CATHeaderNeeded = 0;
   StartofValidPS = 0L;   
   BatchHeaderNotes = _T("");
   BatchHeaderOptions = 0;	  	  
   BatchHeaderCount = 0;
   ViewPerspective = TOP_LEFT;
   PageCount = 1;
   ExtraInfo = 0;   
   Status.LoadString(IDS_READY);
   PreviewStatus.LoadString(IDS_READY);
   StatusFlag = FILE_STATUS_HOLD;   
   Prefs = USE_PREFS_GLOBAL;         
   WasTouched = FALSE;
   Selected = FALSE;
   DragDelete = FALSE;
   IsComplete = TRUE; 
   iPrintToHold = QUEUESTYLE_PRINT;
   ctsElapsedTime = 0;  	  
   m_prange_edit = "";
   m_prange = FALSE;
   bDoSubPage = FALSE;
   iSubPage = 0;

   // Default this value to prevent incorrect rotation
   EPSF = -1;

   // get default owner
   DWORD dwSize = _MAX_PATH;
   GetComputerName(csOwner.GetBuffer(_MAX_PATH), &dwSize);
   csOwner.ReleaseBuffer();

   // default submitted time
   CTime t = CTime::GetCurrentTime();
   csSubmitted = t.Format( "%#I:%M:%S %p %x" );

   iJobID = ((CZeusApp*)AfxGetApp())->GetJobID();
   }

CQueueFile::InitBListFile()	   
   {
   // always blist for now
   ShortName.LoadString(IDS_FFORMAT_BATCHHEADER);
   Description.LoadString(IDS_FFORMAT_BATCHHEADER);
   LongName.LoadString(IDS_FFORMAT_BATCHHEADER);
   PS_JPGFileLongName.Empty();
   FileType = FTYPE_BLIST;   
   Status.LoadString(IDS_READY);
   PreviewStatus.LoadString(IDS_READY);
   StatusFlag = FILE_STATUS_HOLD;
   
   // bounding box stuff
   BoundingBoxStr = "0 0 792 528";
   BoundingBox.left = 0;
   BoundingBox.bottom = 0;
   BoundingBox.right = 792;
   BoundingBox.top = 528;      
   // a blist acts like an eps file
   EPSF = 1;   

   return TRUE; // always ok
   }

BOOL CQueueFile::InitServerFile(PSERVERXFER4 pServerXfer, int iPass, 
								   CDevice* pPassedDevice)
   {
   BOOL ret;
   HANDLE hFile;
   WIN32_FIND_DATA FileInfo;

   CString csError;

#if 0
	BITMAPHANDLE Bitmap;

	BOOL fIsBGRDevice = TRUE;

//	char szTempFileName[_MAX_PATH];
	char szFileName[_MAX_PATH];

	CString csTempPath;
	CString csError;
	CString csJPGFileName;

	FILEINFO ltFileInfo;
	LOADFILEOPTION   LoadFileOption;

	int iRet = 0;
	int iPageNumber = 0;

	TCHAR tcLTPDFDir[(_MAX_PATH + 1) * sizeof(TCHAR)];
	int iLTRet = 0;
#endif

   csError.Format("InitServerFile: Pass = %d", iPass);
   RPDEBUGSTRMFC(csError);

   // only do this on the second pass
   if (iPass == 2)
	  return GetServerFileInfo(pServerXfer, iPass, pPassedDevice);

   ret = FALSE;
   LongName = pServerXfer->FileName;
   MutexName = pServerXfer->MutexName;
   IsComplete = pServerXfer->IsComplete;

   csError.Format("pServerXfer->FileName: %s", LongName);
   RPDEBUGSTRMFC(csError);

   // new stuff
   PageCount = pServerXfer->TotalPages;
   Description = pServerXfer->pDocument;

   // check for existence
   if ((hFile = FindFirstFile(pServerXfer->FileName, 
								&FileInfo)) != INVALID_HANDLE_VALUE)
	  {      
      // short name             
      ShortName = FileInfo.cFileName;
	  // file time
	  ((CZeusApp*)AfxGetApp())->GetFileTime(&FileInfo, ctFileTime);
	  // file size (assuming size less than MAXDWORD)
	  RealFileSize = iFileSize = FileInfo.nFileSizeLow;
	  MakeCoolFileSizeStr();

      // get server file info stuff
      ret = GetServerFileInfo(pServerXfer, iPass, pPassedDevice);


#if 0
	  iLTRet = L_GetPDFInitDir(tcLTPDFDir, ((_MAX_PATH * 1) * 3));

	  memset(&ltFileInfo, 0, sizeof(FILEINFO));

	  L_GetDefaultLoadFileOption(&LoadFileOption, sizeof(LOADFILEOPTION)); 

	  memset(&LoadFileOption, 0, sizeof(LOADFILEOPTION));
	  LoadFileOption.uStructSize = sizeof(LOADFILEOPTION);
	  LoadFileOption.XResolution = 150;
	  LoadFileOption.YResolution = 150;
	  LoadFileOption.PageNumber = iPageNumber + 1;

      memset(&ltFileInfo, 0, sizeof(FILEINFO));
      ltFileInfo.uStructSize = sizeof(FILEINFO);
	  iRet = L_FileInfo(pServerXfer->FileName, &ltFileInfo, sizeof(FILEINFO), FILEINFO_TOTALPAGES, &LoadFileOption);
      if(iRet != SUCCESS)
	    	csError.Format("Error loading file info: %d", iRet);
	  else
	    	csError.Format("Total number of pages: %d", ltFileInfo.TotalPages);
	
#ifdef _DEBUG
//	  AfxMessageBox(csError, MB_OK);
#else
	  RPDEBUGSTRMFC(csError);
#endif

    /* Load the bitmap, keeping the bits per pixel of the file */
    iRet = L_LoadBitmap (pServerXfer->FileName, &Bitmap, sizeof(BITMAPHANDLE), 0, ORDER_BGR, &LoadFileOption, &ltFileInfo);

/*
	iRet = L_LoadFile(szFileName, &Bitmap, sizeof(BITMAPHANDLE), 0, fIsBGRDevice ? ORDER_BGRORGRAY : ORDER_RGBORGRAY, 
						LOADFILE_ALLOCATE | LOADFILE_STORE, NULL, NULL, &LoadFileOption, &ltFileInfo);
*/ 
	if(iRet == SUCCESS)
	{
//		csTempPath = GetSpoolFolder();
//		GetTempFileName(csTempPath, "img", 0, szTempFileName);   

//		csError.Format("L_LoadFile Filename = %s", szTempFileName);

#ifdef _DEBUG
//		AfxMessageBox(csError, MB_OK);
#else
		RPDEBUGSTRMFC(csError);
#endif

		// compress the image   
//			iRet = ::L_SaveFile (szTempFileName, &Bitmap, FILE_JFIF, 24, 
//		iRet = ::L_SaveFile (szTempFileName, &Bitmap, FILE_LEAD1JFIF_LAB, 24, 
//					NULL, NULL, NULL, NULL, NULL);

//		if ( iRet != SUCCESS )
//		{
//			csError.Format("L_SaveFile Return = %d", iRet);
#ifdef _DEBUG
//			AfxMessageBox(csError, MB_OK);
#else
//			RPDEBUGSTRMFC(csError);
#endif
//		}
//		else
//		{
//			csJPGFileName = szTempFileName;
//		}
	}
	else
	{
		csError.Format("L_LoadFile Return = %d", iRet);
#ifdef _DEBUG
		AfxMessageBox(csError, MB_OK);
#else
		RPDEBUGSTRMFC(csError);
#endif
	}
#endif
#ifdef LEADFILE_PS
	  // Convert PS to JPG File
	  PS_JPGFileLongName = ((CZeusApp*)AfxGetApp())->Convert_PS_to_JPG(LongName);
#endif	  
      // close the handle
      FindClose(hFile);   
	  
	  // set server file flag
	  ExtraInfo |= CQUEUE_EXTRA_SERVERFILE;

	  // these from servdefs
	  // convert submitted time from UTC
	  SYSTEMTIME LocalSystemTime;
	  FILETIME FileTime;
	  FILETIME LocalFileTime;
	  // convert to filetime
	  SystemTimeToFileTime(&pServerXfer->Submitted, &FileTime);
	  // get loca time
	  FileTimeToLocalFileTime(&FileTime, &LocalFileTime);
	  // convert back to systme time
	  FileTimeToSystemTime(&LocalFileTime, &LocalSystemTime);
      CTime t( LocalSystemTime ); 
      csSubmitted = t.Format( "%#I:%M:%S %p %x" );

	  csOwner = pServerXfer->pUserName;
	  Description = pServerXfer->pDocument;
	  }
   return ret;
   }


BOOL CQueueFile::Init(LPCSTR lpName, int& iTotalPages, int iPage)
   {
   BOOL ret;
   HANDLE hFile;
   WIN32_FIND_DATA FileInfo;
   FILEINFO ltFileInfo;

   ret = FALSE;
   LongName = lpName;	
   PS_JPGFileLongName.Empty();

   iTotalPages = 1;

	  csDebug.Format("Init LongName = %s\n", LongName);
	  RPDEBUGSTRMFC(csDebug);

   // check for existence
   if ((hFile = FindFirstFile((const char*)LongName,    
					      &FileInfo)) != INVALID_HANDLE_VALUE)
	  {      
      // short name             
      ShortName = FileInfo.cFileName;	  
	  // file time
	  ((CZeusApp*)AfxGetApp())->GetFileTime(&FileInfo, ctFileTime);
	  // file size (assuming size less than MAXDWORD)
	  RealFileSize = iFileSize = FileInfo.nFileSizeLow;
	  MakeCoolFileSizeStr();
      // get file type
      GetFileType();

#ifdef LEADFILE_PS
	  if ((FileType == FTYPE_PSCRIPT) || (FileType == FTYPE_PDF))
		  PS_JPGFileLongName = ((CZeusApp*)AfxGetApp())->Convert_PS_to_JPG(LongName, iPage);
#endif

	  memset(&ltFileInfo, 0, sizeof(FILEINFO));
	  ltFileInfo.uStructSize = sizeof(FILEINFO);
	  ::L_FileInfo((char*)(const char*)LongName, &ltFileInfo, sizeof(FILEINFO), FILEINFO_TOTALPAGES, NULL);

	  iTotalPages = ltFileInfo.TotalPages;

      // close the handle
      FindClose(hFile);   
	  ret = TRUE;
	  }
   else
   {
	  csDebug.Format("FindFirstFile = %s\n", "FAILED");
	  RPDEBUGSTRMFC(csDebug);
   }

   return ret;
   }

CQueueFile::~CQueueFile()   
   {  
   CTimeSpan* ctsp;

   // if using a local device, delete it
   if(Prefs & USE_LOCAL_DEV_PREFS)
      {
      if (pCD)
		  delete pCD;   
      }
   // if using a local transform, delete it
   if(Prefs & USE_LOCAL_TRN_PREFS)
      {
      if (pTransform)
		  delete pTransform;
      }
   // if using a local color transform, delete it
   if(Prefs & USE_LOCAL_COL_PREFS)
      {
      if (pCC)
		  delete pCC;
      }
   
   // and remove these
   for (int i = cdwaPageTimes.GetSize() - 1; i >= 0; i--)
	  {
	  // delete it from the array		 
	  ctsp = (CTimeSpan*)cdwaPageTimes.GetAt(i);
	  delete ctsp;
	  }  
   // remove the pointers
   cdwaPageTimes.RemoveAll();

   // remove the job id
   ((CZeusApp*)AfxGetApp())->DeleteJobID(iJobID);
   }

// serialization
void CQueueFile::Serialize(CArchive& ar)
{     
   WORD wVersion = ((CZeusApp*)AfxGetApp())->GetFileVersion();

   if (ar.IsStoring())
      {
	  // don't store in imaging state	  
	  if ((StatusFlag & FILE_STATUS_INRIP) == FILE_STATUS_INRIP)
	     {
	     Status.LoadString(IDS_READY);
         PreviewStatus.LoadString(IDS_READY);
         StatusFlag = FILE_STATUS_HOLD;   
		 }   
      ar << FileType << LongName << ShortName << Description << iFileSize << 
	  RealFileSize << csRealFileSize <<
	  ctFileTime << BoundingBoxStr << Status << 
	  PreviewStatus << EPSF << BoundingBox << 
	  ctsElapsedTime << StatusFlag << Prefs << CATHeaderNeeded << StartofValidPS <<
	  BatchHeaderNotes << BatchHeaderOptions << BatchHeaderCount << ViewPerspective <<
	  PageCount << ExtraInfo << QPos << IsComplete << MutexName <<
	  iPrintToHold << csOwner << csSubmitted << InQueue;

	  // if we have a device, serialize it
	  if(Prefs & USE_LOCAL_DEV_PREFS)
		  pCD->Serialize(ar);	  
	  // ditto for transform
	  if(Prefs & USE_LOCAL_TRN_PREFS)
		  pTransform->Serialize(ar);
	  // and for color
	  if(Prefs & USE_LOCAL_COL_PREFS)
		  pCC->Serialize(ar);

	  ar << PS_JPGFileLongName;
	  }      
   else                 
      {                                         
      ar >> FileType >> LongName >> ShortName >> Description >> iFileSize >> 
	  RealFileSize >> csRealFileSize >>
	  ctFileTime >> BoundingBoxStr >> Status >> 
	  PreviewStatus >> EPSF >> BoundingBox >> 
	  ctsElapsedTime >> StatusFlag >> Prefs >> CATHeaderNeeded >> StartofValidPS >>
	  BatchHeaderNotes >> BatchHeaderOptions >> BatchHeaderCount >> ViewPerspective >>
	  PageCount >> ExtraInfo >> QPos >> IsComplete >> MutexName >>
	  iPrintToHold >> csOwner >> csSubmitted >> InQueue;
	  // if we have a device, serialize it
	  if(Prefs & USE_LOCAL_DEV_PREFS) 
	     {                    
	     // make a new device
	     pCD = new CDevice;		 		 
	     // get it
	     pCD->Serialize(ar);	 
	     }
	  else
	     pCD = NULL;
	  // ditto for transform
	  if(Prefs & USE_LOCAL_TRN_PREFS) 
	     {                           
	     // make a new transform
	     pTransform = new CTransform;
	     // get it
	     pTransform->Serialize(ar);   
	     }	  
	  else
	     pTransform = NULL;
	  // and  for color
	  if(Prefs & USE_LOCAL_COL_PREFS) 
	     {                           
	     // make a new transform
	     pCC = new CColorTransform;
	     // get it
	     pCC->Serialize(ar);   
	     }	  
	  else
	     pCC = NULL;
	  // always zero out selection
	  Selected = FALSE;	  

	  switch ( wVersion )
	  {
	  case ZEUSDOC_VERSION_A:
	  case ZEUSDOC_VERSION_9:
	  case ZEUSDOC_VERSION_8:
		  break;
	  case ZEUSDOC_VERSION_7:
	  case ZEUSDOC_VERSION_6:
		  ar >> PS_JPGFileLongName;
	  case ZEUSDOC_NO_VERSION:
	  default:
			break;
	  }
	  }      
}   
           
#define MAXTESTSTRLEN 256  

void CQueueFile::GetFileType()
   {             
   CStdioFile csfCheckFile;

   FILEPDFOPTIONS PDFFileInfo;
   FILEINFO FileInfo;

   int iRet = 0;

//   LFile File;

   // default 
   FileType = FTYPE_UNKNOWN;           

   memset(&FileInfo, 0, sizeof(FILEINFO));
   FileInfo.uStructSize = sizeof(FILEINFO);
   iRet = ::L_FileInfo((char*)(const char*)LongName, &FileInfo, sizeof(FILEINFO), FILEINFO_TOTALPAGES, NULL);

   // open the file
   if (csfCheckFile.Open(LongName, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone))
      {
		  L_GetPDFOptions(&PDFFileInfo, sizeof(FILEPDFOPTIONS));

      // first, check for PostScript
      if (CheckForPostScript(csfCheckFile))
         {                      
         FileType = FTYPE_PSCRIPT;
		 ViewPerspective = TOP_LEFT;
         }          
      if (CheckForPDF(csfCheckFile))
         {                      
         FileType = FTYPE_PDF;
		 ViewPerspective = TOP_LEFT;
         }          
      // close file
      csfCheckFile.Close();

      // now check for non-postscript file
      if (FileType == FTYPE_UNKNOWN)
         {    

   csDebug.Format("CQueueFile::GetFileType(%s)\n", "UNKNOWN" );
   RPDEBUGSTRMFC(csDebug);

		 // block others from using the bitmap dll
         EnterCriticalSection(&BITMAPENGINEMUTEX);

   csDebug.Format("L_FileInfo for %s\n", LongName );
   RPDEBUGSTRMFC(csDebug);

		 // use lead tools
		 memset(&FileInfo, 0, sizeof(FILEINFO));
         FileInfo.uStructSize = sizeof(FILEINFO);
	     int iRet = ::L_FileInfo((char*)(const char*)LongName, &FileInfo, sizeof(FILEINFO), FILEINFO_TOTALPAGES, NULL);

   csDebug.Format("LFileInfo(%d)\n", iRet );
   RPDEBUGSTRMFC(csDebug);

	     if (iRet > 0)
            {                
			// load view persepective
            ViewPerspective = FileInfo.ViewPerspective;              
            // get bbox info                                         
            wsprintf(BoundingBoxStr.GetBuffer(_MAX_PATH), "%d %d %d %d",
               0, 0, FileInfo.Width, FileInfo.Height);
            BoundingBoxStr.ReleaseBuffer();    
            
            // set actual bounding box   
            BoundingBox.left = 0;
            BoundingBox.bottom = 0;
            BoundingBox.right = FileInfo.Width;
            BoundingBox.top = FileInfo.Height;
            // straight-up EPSF file
            EPSF = 1;               

			// set total pages
			PageCount = FileInfo.TotalPages;
            
            switch(FileInfo.Format)
               {
			   case FILE_PSD:     // PhotoShop
				    FileType = FTYPE_PSD; 
					break;
               case FILE_PCX:     // ZSoft PCX
                    FileType = FTYPE_PCX;
                    break;
               case FILE_GIF:     // CompuServe GIF
                    FileType = FTYPE_GIF;
                    break;               
               case FILE_TGA:     // Targa                   
               case FILE_TGA_RLE:
                    FileType = FTYPE_TARGA;
					// do special hack for targa files
					//DoTargaHack(csfCheckFile);
                    break;               
               case FILE_BMP:     // Windows BMP             
                    FileType = FTYPE_BMP;
                    break;
               case FILE_JFIF:    // Jpeg File Interchange Format               
               case FILE_LEAD1JFIF:               
               case FILE_LEAD2JFIF:
                    FileType = FTYPE_JFIF;
                    break;         
               case FILE_JTIF:    // Jpeg Tag Image File Format
			   case FILE_LEAD2JTIF:
			   case FILE_LEAD1JTIF:		               
                    FileType = FTYPE_JTIF;
                    break;                               
               case FILE_OS2:     // OS/2 BMP                 
                    FileType = FTYPE_OS2;
                    break;                
               case FILE_TIFLZW:  // Tagged Image File Formats
               case FILE_TIF_PACKBITS:
			   case FILE_TIF:     
			   case FILE_TIF_YCC: 
			   case FILE_TIFLZW_YCC:
			   case FILE_TIF_PACKBITS_YCC:
                    FileType = FTYPE_TIFF;
                    break;                               
               case FILE_TIF_CMYK:
               case FILE_TIF_PACKBITS_CMYK:
               case FILE_TIFLZW_CMYK:
				    FileType = FTYPE_UNSUPPORTED;
					break;
               case FILE_CALS:    // CCITT Group 4 CALS raster files (US Gov't)
                    FileType = FTYPE_CALS;
                    break;
               case FILE_MAC:     // MacPaint
                    FileType = FTYPE_MAC;
                    break;               
               case FILE_IMG:     // GEM Image
                    FileType = FTYPE_IMG;
                    break;               
               case FILE_MSP:     // Microsoft Paint
                    FileType = FTYPE_MSP;
                    break;               
               case FILE_WPG:     // WordPerfect Graphics raster files
                    FileType = FTYPE_WPG;
                    break;               
               case FILE_RAS:     // SUN Raster
                    FileType = FTYPE_RAS;
                    break;               
               //case FILE_PCT:     // Macintosh PICT
                    //FileType = FTYPE_PCT;
                    //break;
			   case FILE_PNG:
			        FileType = FTYPE_PNG;
                    break;
			   case FILE_FPX:
			   case FILE_FPX_SINGLE_COLOR:
			   case FILE_FPX_JPEG:
			   case FILE_FPX_JPEG_QFACTOR:
			        FileType = FTYPE_FPX;
                    break;
			   case FILE_SGI:
			   case FILE_SGI_RLE:
				    FileType = FTYPE_SGI;
				    break;
               case FILE_EXIF:
			   case FILE_EXIF_YCC:
			   case FILE_EXIF_JPEG:
               case FILE_EXIF_JPEG_411:
			   case FILE_JP2:
				    FileType = FTYPE_EXIF;
                    break;
               case FILE_PCD:     // Photo CD
                    FileType = FTYPE_PCD;
					// get supported resolutions
					PCDINFO pcdinfo;
					int i;
					int SupportedResolution = L_PCD_BASE;

					::L_GetPCDResolution((char*)(const char*)LongName, &pcdinfo);
					// get biggest resolution
					for (i = L_PCD_16BASE; i >= L_PCD_BASE_OVER_64; i--)
					   {
					   if (pcdinfo.resolution[i])
					      {
						  SupportedResolution = i;
						  break;
						  }
					   }
					::L_SetPCDResolution(SupportedResolution);
					// reset resolution based on max res
					memset(&FileInfo, 0, sizeof(FILEINFO));
					FileInfo.uStructSize = sizeof(FILEINFO);
					int ret = ::L_FileInfo((char*)(const char*)LongName, &FileInfo, sizeof(FILEINFO), FILEINFO_TOTALPAGES, NULL);

					wsprintf(BoundingBoxStr.GetBuffer(_MAX_PATH), "%d %d %d %d",
                       0, 0, FileInfo.Width, FileInfo.Height);
                    BoundingBoxStr.ReleaseBuffer();    
            
                    // set actual bounding box   
                    BoundingBox.left = 0;
                    BoundingBox.bottom = 0;
                    BoundingBox.right = FileInfo.Width;
                    BoundingBox.top = FileInfo.Height;            
                    break;
               }    
            //GetFileTypeName(Description);   			
            }
	     // free up the bitmap engine
         LeaveCriticalSection(&BITMAPENGINEMUTEX);

   csDebug.Format("FileTpe is %d\n", FileType );
   RPDEBUGSTRMFC(csDebug);
         }

   csDebug.Format("GetFileType ViewPerspective(%d)\n", ViewPerspective );
   RPDEBUGSTRMFC(csDebug);

      Description.TrimRight();
      if (Description.GetLength() <= 0)
		  Description = ShortName;
      }            
   }
   
int CQueueFile::GetPSLine(CStdioFile& csfCheckFile, char *buf, short buflen)
   {     
   short i, j;
   BYTE val;   
   BOOL HaveData;
   
   // get chars up to buf length or 0x0, 0x0d or 0x0a, 
   // whichever comes first
   for (i = 0, j = 0, HaveData = FALSE; i < buflen; i++)
      {
      // store char
      if (csfCheckFile.Read(&val, 1) != 1)      
         {         
         // oops!	  
	     return -1;
	     }
      else
	      buf[j] = val;
      
      // maxed out buffer!
      if (j == (buflen - 1)) break;      
      
      // check for whitespace or ctrl-d
      if (val == 0x0d || val == 0x0a || val == 0x0 || val == 0x04)
         {                   
         // if we have some data, break,
         // else set flag
         if (HaveData) 
            break;
         }      
      else     
         {
         HaveData = TRUE;               
         j++;
         }
      }                      
   // add a trailing 0x0
   buf[j] = 0x0;
   return(j);
   }

typedef struct tagEPSHEADER
   {
   BYTE signature;
   long PS_start,
	    PS_length,
	    META_start,
	    META_length,
	    TIFF_start,
	    TIFF_length;
   short Checksum;
   } EPSHEADER;	  

BOOL CQueueFile::CheckForPostScript(CStdioFile& csfCheckFile)
   {   
   BOOL ret;
   int NumRead;
   char CheckBuf[5];
   EPSHEADER EpsHeader;
   
   // default
   ret = FALSE;                            
   
   // hack off junk at begin of file   
   do
      {
      NumRead = csfCheckFile.Read(CheckBuf, 1);
      } while (((CheckBuf[0] == 0x04) || // ctrl-d
                (CheckBuf[0] == 0x20) || // space
                (CheckBuf[0] == 0x0d) || // cr                
                (CheckBuf[0] == 0x0a)) && // linefeed                 
                (NumRead > 0));   
   
   // go back one             
   csfCheckFile.Seek(-1L, CFile::current);

   // check first four chars for %!PS 
   if (csfCheckFile.Read(CheckBuf, 4) == 4)
      {
      if (strstr(CheckBuf, "%!PS") != NULL) // ps file
	     ret = TRUE;
	  else if (strstr(CheckBuf, "\xc5\xd0\xd3\xc6") != NULL) // eps file
	     {             
	     ret = TRUE;
	     // go to start
	     csfCheckFile.SeekToBegin();
	     // read the header
		 csfCheckFile.Read(&EpsHeader, sizeof(EPSHEADER));	     
	     // modify our input file size so we don't read past length
	     //GlobalImageSpecs.InputFileSize = EpsHeader.PS_start +
	     //   EpsHeader.PS_length;
	     // store the PS start position
		 RealFileSize = EpsHeader.PS_start + EpsHeader.PS_length;	     
		 MakeCoolFileSizeStr();
	     StartofValidPS = EpsHeader.PS_start;		 
	     }   
	  }      
   // get rest of stuff, if this is a valid PS file
   if (ret)               
      {                
      // go back to beginning
      //csfCheckFile.SeekToBegin();      	           
	  csfCheckFile.Seek(StartofValidPS, CFile::begin);	  
      GetPostScriptInfo(csfCheckFile);
      }                                   
   // go back to again
   csfCheckFile.SeekToBegin();      	        
   return(ret);
   }  

BOOL CQueueFile::CheckForPDF(CStdioFile& csfCheckFile)
   {   
   BOOL ret;
   int NumRead;
   char CheckBuf[5];
   
   // default
   ret = FALSE;                            
   
   // hack off junk at begin of file   
   do
      {
      NumRead = csfCheckFile.Read(CheckBuf, 1);
      } while (((CheckBuf[0] == 0x04) || // ctrl-d
                (CheckBuf[0] == 0x20) || // space
                (CheckBuf[0] == 0x0d) || // cr                
                (CheckBuf[0] == 0x0a)) && // linefeed                 
                (NumRead > 0));   
   
   // go back one             
   csfCheckFile.Seek(-1L, CFile::current);

   // check first four chars for %PDF 
   if (csfCheckFile.Read(CheckBuf, 4) == 4)
      {
      if (strstr(CheckBuf, "%PDF") != NULL) // ps file
	     ret = TRUE;
	  }      
   // get rest of stuff, if this is a valid PS file
   if (ret)               
      {                
      // go back to beginning
	  csfCheckFile.Seek(StartofValidPS, CFile::begin);	  
      GetPDFInfo(csfCheckFile);
      }                                   
   // go back to again
   csfCheckFile.SeekToBegin();      	        
   return(ret);
   }  

// maximum number of lines we'll search for stuff
#define PSMAXTEXTLINES 100
// bounding box searches are done only
// if a higher precendence box is not found
// 
#define BBOX_FOUND_BB   1
#define BBOX_FOUND_PBB  2
#define BBOX_FOUND_ADO  3

#define CATNUMBERFILEPOS 22
								   
void CQueueFile::GetPostScriptInfo(CStdioFile& csfCheckFile)
   {                 
   int NumGot;                             
   int LineCount;
   char TestStr[MAXTESTSTRLEN];
   char *p;
   BOOL FoundDesc, FoundEPSF;
   BOOL bFoundPageBoundingBox;
   int FoundBBox;
   BOOL CheckForCAT;
   BOOL Adobe_20_Found = FALSE;
   BOOL MacFilmHack = FALSE;
   CString Token;
   CString Value;
   FoundDesc = FoundEPSF = FALSE;
   CheckForCAT = FALSE;
   FoundBBox = 0;
   LineCount = 0;
   
   csDebug.Format("%s GetPostScriptInfo\n", "Starting");
   RPDEBUGSTRMFC(csDebug);

   do
      {
	  // get a line
	  NumGot = GetPSLine(csfCheckFile, TestStr, MAXTESTSTRLEN);
	  if (NumGot == -1) break;
	  LineCount++;
	     
	  if (strncmp(TestStr, "%%", 2) == 0)
	  {
		csDebug.Format("PSInfo - TestStr(%d) = %s\n", LineCount, TestStr );
		RPDEBUGSTRMFC(csDebug);
	  }

	  // look for title
	  if (p = strstr(TestStr, PS_Token[PSLOOKUP_TITLE]))	     
	     {
	     // extract Description
	     Description = p + lstrlen(PS_Token[PSLOOKUP_TITLE]) + 1;
	     FoundDesc = TRUE;		 

		 csDebug.Format("PSLOOKUP_TITLE = %s\n", Description );
		 RPDEBUGSTRMFC(csDebug);
	     } 
	  else if (p = strstr(TestStr, PS_Token[PSLOOKUP_RP95_ADOBE_30]))	
	     {     
		 SetPages(csfCheckFile);		 
	     if (strstr(TestStr, "EPSF-3.0"))
	        {
	        FoundEPSF = TRUE;
	        EPSF = 2;       
	        }				

		 csDebug.Format("PSLOOKUP_RP95_ADOBE_30 = %s\n", "" );
		 RPDEBUGSTRMFC(csDebug);
		 }	  
	  else if (p = strstr(TestStr, PS_Token[PSLOOKUP_RP95_ADOBE_20]))	
	     {     
		 Adobe_20_Found = TRUE;
	     if (strstr(TestStr, "EPSF-1.0"))
	        {
	        FoundEPSF = TRUE;
	        EPSF = 2;       
	        }
	     else if (strstr(TestStr, "EPSF"))
	        {
	        FoundEPSF = TRUE;
	        EPSF = 1;	    
	        }		 

		 csDebug.Format("PSLOOKUP_RP95_ADOBE_20 = %s\n", "" );
		 RPDEBUGSTRMFC(csDebug);
	  }	  
	  else if (p = strstr(TestStr, PS_Token[PSLOOKUP_RP95_MACFILM_HACK]))		  
		 {
		 MacFilmHack = TRUE;

		 csDebug.Format("PSLOOKUP_RP95_MACFILM_HACK = %s\n", "" );
		 RPDEBUGSTRMFC(csDebug);
		 continue;
		 }
	  // look for winslide
	  else if (p = strstr(TestStr, PS_Token[PSLOOKUP_WINSLIDE_CREATOR] ))	     
	     {
	     ExtraInfo |= CQUEUE_EXTRA_WINSLIDE;
		 //%%Creator: WinSlide Driver V8->M2       (CAT!)
		 // get page count for new WinSlide files
		 if ((TestStr[31] == 'M') &&
		     (isdigit(TestStr[32])))
			 {
			 PageCount = atoi(TestStr + 32);
			 // can winsplit!
			 ExtraInfo |= CQUEUE_EXTRA_WINSPLIT;
			 }		 

		 csDebug.Format("PSLOOKUP_WINSLIDE_CREATOR = %s\n", "" );
		 RPDEBUGSTRMFC(csDebug);
	     } 
      // look for ADO BoundingBox
	  else if (p = strstr(TestStr, PS_Token[PSLOOKUP_ADOIAREA]))
	     {
	     if ((!strstr(TestStr, PS_Token[PSLOOKUP_QUESTION])) &&
	        (!strstr(TestStr, PS_Token[PSLOOKUP_ATEND])) &&
	        (FoundBBox < BBOX_FOUND_ADO))
	        {
	        // extract bounding box
	        BoundingBoxStr = p + lstrlen(PS_Token[PSLOOKUP_ADOIAREA]) + 1;
	        FoundBBox = BBOX_FOUND_ADO;
	        break;
	        }

		 csDebug.Format("PSLOOKUP_ADOIAREA BoundingBoxStr = %s\n", BoundingBoxStr);
		 RPDEBUGSTRMFC(csDebug);
		 continue;
	    }		     
      // look for PageBoundingBox 
	  else if (p = strstr(TestStr, PS_Token[PSLOOKUP_PAGEBOUNDINGBOX]))
	     {             
	     // is it real?
	     if ((!strstr(TestStr, PS_Token[PSLOOKUP_QUESTION])) &&
	        (!strstr(TestStr, PS_Token[PSLOOKUP_ATEND])) &&
	        (FoundBBox < BBOX_FOUND_PBB))
	        {                                   
	        // extract bounding box
	        BoundingBoxStr = p + lstrlen(PS_Token[PSLOOKUP_PAGEBOUNDINGBOX]) + 1;
	        FoundBBox = BBOX_FOUND_PBB;
	        }

		 csDebug.Format("PSLOOKUP_PAGEBOUNDINGBOX BoundingBoxStr = %s\n", BoundingBoxStr);
		 RPDEBUGSTRMFC(csDebug);
		 continue;
	     }
      // look for regular bounding box
	  else if (p = strstr(TestStr, PS_Token[PSLOOKUP_BOUNDINGBOX]))
	     {                         
	     // is it real?
	     if ((!strstr(TestStr, PS_Token[PSLOOKUP_QUESTION])) &&
	        (!strstr(TestStr, PS_Token[PSLOOKUP_ATEND])) &&
	        (FoundBBox < BBOX_FOUND_BB))
	        {                      
	        // extract bounding box
	        BoundingBoxStr = p + lstrlen(PS_Token[PSLOOKUP_BOUNDINGBOX]) + 1;
	        FoundBBox = BBOX_FOUND_BB;
	        }    

		 csDebug.Format("PSLOOKUP_BOUNDINGBOX BoundingBoxStr = %s\n", BoundingBoxStr);
		 RPDEBUGSTRMFC(csDebug);
		 continue;
	     }	        	  	     
      // look for HiRes bounding box
	  else if (p = strstr(TestStr, PS_Token[PSLOOKUP_HIRES_BOUNDINGBOX]))
	     {                         
	     // is it real?
	     if ((!strstr(TestStr, PS_Token[PSLOOKUP_QUESTION])) &&
	        (!strstr(TestStr, PS_Token[PSLOOKUP_ATEND])) &&
	        (FoundBBox < BBOX_FOUND_BB))
	        {                      
	        // extract bounding box
	        BoundingBoxStr = p + lstrlen(PS_Token[PSLOOKUP_HIRES_BOUNDINGBOX]) + 1;
	        FoundBBox = BBOX_FOUND_BB;
	        }    

		 csDebug.Format("PSLOOKUP_HIRES_BOUNDINGBOX BoundingBoxStr = %s\n", BoundingBoxStr);
		 RPDEBUGSTRMFC(csDebug);
		 continue;
	     }	        	  	     
	  // look for EPSF string
	  else if (p = strstr(TestStr, PS_Token[PSLOOKUP_EPSF] ))
	     {
	     if (strstr(TestStr, "EPSF-1.0"))
	        {
	        FoundEPSF = TRUE;
	        EPSF = 2;       
	        }
	     else if (strstr(TestStr, "EPSF"))
	        {
	        FoundEPSF = TRUE;
	        EPSF = 1;	    
	        }

		 csDebug.Format("PSLOOKUP_EPSF = %s\n", "" );
		 RPDEBUGSTRMFC(csDebug);
	     }
	  // look for non-EPSF string
	  // Note: this must be after the previous "if" statement
	  else if (p = strstr(TestStr, PS_Token[PSLOOKUP_RP95_ADOBE_ANY] ))
	     {
	     FoundEPSF = TRUE;
	     EPSF = 0;

		 csDebug.Format("PSLOOKUP_RP95_ADOBE_ANY = %s\n", "" );
		 RPDEBUGSTRMFC(csDebug);
	     }	     
	  // need to flip win 2k files
	  else if (p = strstr(TestStr, PS_Token[PSLOOKUP_WIN2000_CREATOR] ))
	     {
	     ExtraInfo |= CQUEUE_EXTRA_WINSLIDE;

		 csDebug.Format("PSLOOKUP_WIN2000_CREATOR = %s\n", "" );
		 RPDEBUGSTRMFC(csDebug);		 
	     }	      
	  // look for bounding box feature for pm 6.5
	  else if (ServerTokenMatch(TestStr, PSLOOKUP_RP95_BEGIN_FEATURE))	  
	     {
		 p = TestStr;
		 // get token
	     Token = p + lstrlen(PS_Token[PSLOOKUP_RP95_BEGIN_FEATURE]) + 1;     		 		 

		 csDebug.Format("PSLOOKUP_RP95_BEGIN_FEATURE = %s\n", TestStr );
		 RPDEBUGSTRMFC(csDebug);

		 // get value
		 if (GetPSLine(csfCheckFile, TestStr, MAXTESTSTRLEN) > 0)
		    Value = TestStr;
		 LineCount++;

		 csDebug.Format("PSLOOKUP_RP95(%d) - TestStr() = %s\n", LineCount, TestStr );
		 RPDEBUGSTRMFC(csDebug);

		 // blank line!
		 if (ServerTokenMatch(TestStr, PSLOOKUP_RP95_END_FEATURE))
			 continue;

		 // keep getting lines until an EndFeature is encountered
		 do
		    {
			NumGot = GetPSLine(csfCheckFile, TestStr, MAXTESTSTRLEN);
			if (NumGot == -1)
				break;

			csDebug.Format("Begin(do - %d) - TestStr() = %s\n", LineCount, TestStr );
			RPDEBUGSTRMFC(csDebug);

			// we may have a bounding box in here
			p = TestStr;
			bFoundPageBoundingBox = ServerTokenMatch(p, PSLOOKUP_PAGEBOUNDINGBOX);

			if ( bFoundPageBoundingBox == FALSE )
			{
			    p = TestStr + 1;
				bFoundPageBoundingBox = ServerTokenMatch(TestStr, PSLOOKUP_PAGEBOUNDINGBOX);
			}

			if ( bFoundPageBoundingBox == TRUE )
			   {
//			   p = TestStr;
			   ExtractBoundingBox((const char*)(p + lstrlen(PS_Token[PSLOOKUP_PAGEBOUNDINGBOX]) + 1));
//			   ExtractBoundingBox((const char*)(TestStr + lstrlen(PS_Token[PSLOOKUP_PAGEBOUNDINGBOX]) + 1));
   			   FoundBBox = TRUE;

			   csDebug.Format("PSLOOKUP_PAGEBOUNDINGBOX = top(%d), left(%d), right(%d), bottom(%d)\n", BoundingBox.top, BoundingBox.left, BoundingBox.right, BoundingBox.bottom );
			   RPDEBUGSTRMFC(csDebug);
			   }
			// append TestStr into Value
			LineCount++;
			} while (!ServerTokenMatch(TestStr, PSLOOKUP_RP95_END_FEATURE) && 
			          //LineCount < PSMAXTEXTLINES);		 
					  LineCount < NUMLINESTOTEST);		 					  
		 continue;
		 }		 	  

	  // check for CPS file
	  if (LineCount == 9)
	     {
	     if (strstr(TestStr, PS_Token[PSLOOKUP_WINSPLIT_DICT]))
	        {	     
	        // get dict number   
            CATHeaderNeeded = atoi(TestStr + CATNUMBERFILEPOS);        
            // get pos of start read
            StartofValidPS = csfCheckFile.GetPosition() + 2L;			
			}
		 CheckForCAT = TRUE;

		 csDebug.Format("PSLOOKUP_WINSPLIT_DICT = %s\n", "" );
		 RPDEBUGSTRMFC(csDebug);
         }	     	     
	  //} while (LineCount < PSMAXTEXTLINES && NumGot != -1 && 
	  } while (LineCount < NUMLINESTOTEST && NumGot != -1 && 
	     //(!FoundBBox || !FoundDesc || !FoundEPSF || !CheckForCAT));
		 (!FoundBBox || !FoundDesc || !CheckForCAT));
      
   if (!FoundBBox)
      {     
      // not found!
      BoundingBoxStr.LoadString(IDS_NOT_AVAIL);
      BoundingBox.left = BoundingBox.right = 
      BoundingBox.top = BoundingBox.bottom = 0;
	  
	  csDebug.Format("!FoundBBox BoundingBoxStr = %s\n", BoundingBoxStr );
	  RPDEBUGSTRMFC(csDebug);
      } 
   else
      {
	  csDebug.Format("FoundBBox BoundingBoxStr = %s\n", BoundingBoxStr );
	  RPDEBUGSTRMFC(csDebug);

	  ExtractBoundingBox(BoundingBoxStr);

	  csDebug.Format("FoundBBox = top(%d), left(%d), right(%d), bottom(%d)\n", BoundingBox.top, BoundingBox.left, BoundingBox.right, BoundingBox.bottom );
	  RPDEBUGSTRMFC(csDebug);

	  // look for special stuff indicating a MacFilm 2.0 file and then
	  // check and correct for weird bounding box stuff
	  if (FoundBBox == BBOX_FOUND_PBB && Adobe_20_Found && MacFilmHack)	  
         {
		  csDebug.Format("FoundBBox == BBOX_FOUND_PBB && Adobe_20_Found && MacFilmHack  %s\n", "");
		  RPDEBUGSTRMFC(csDebug);
         /* if the width or height is GREATER than 24", then we must be
            using dots instead of PICA (purely emprical!) */
         if (abs(BoundingBox.Width()) > 1728 || abs(BoundingBox.Height()) > 1728)
            {
            BoundingBox.left = MulDiv(BoundingBox.left, 72, 300);
            BoundingBox.right = MulDiv(BoundingBox.right, 72, 300);
            BoundingBox.top = MulDiv(BoundingBox.top, 72, 300);
            BoundingBox.bottom = MulDiv(BoundingBox.bottom, 72, 300);
	  csDebug.Format(">1728 = top(%d), left(%d), right(%d), bottom(%d)\n", BoundingBox.top, BoundingBox.left, BoundingBox.right, BoundingBox.bottom );
	  RPDEBUGSTRMFC(csDebug);
            }   
		 // force image to flip
		 ExtraInfo |= CQUEUE_EXTRA_WINSLIDE;
		 }
      }	           
	                        
   if (!FoundDesc)
      {     
      // not found!
      //Description = "N/A";
      Description = ShortName;
      }           
   if (!FoundEPSF)
      {     
      // not found!
      EPSF = 0;
      }                 
   }

void CQueueFile::GetCATInfoOnly(CStdioFile& csfCheckFile)
   {                 
   int NumGot;                             
   int LineCount;
   static char TestStr[MAXTESTSTRLEN];      
   BOOL GotCAT;
   
   GotCAT = FALSE;
   LineCount = 0;
   
   do
      {
	  // get a line
	  NumGot = GetPSLine(csfCheckFile, TestStr, MAXTESTSTRLEN);
	  if (NumGot == -1) break;
	  LineCount++;
	     
	  // check for CPS file
	  if ((LineCount == 9) && strstr(TestStr, PS_Token[PSLOOKUP_WINSPLIT_DICT]))
	     {	     
	     // get dict number   
         CATHeaderNeeded = atoi(TestStr + CATNUMBERFILEPOS);        
         // get pos of start read
         StartofValidPS = csfCheckFile.GetPosition() + 2L;
         GotCAT = TRUE;
         }	     	     
	  } while (LineCount < PSMAXTEXTLINES && NumGot != -1 && !GotCAT);	  
   }

BOOL CQueueFile::operator!=(const CQueueFile& cq)
   {
   // is same thing?
   return (!IsSame(cq));
   }

BOOL CQueueFile::operator==(const CQueueFile& cq)
   {
   // is same thing?
   return (IsSame(cq));
   }

BOOL CQueueFile::IsSame(const CQueueFile& cq)
   {                                                           
   if (FileType != cq.FileType) return FALSE;
   if (Prefs != cq.Prefs) return FALSE;
   if (BoundingBox != cq.BoundingBox) return FALSE;
   if (LongName != cq.LongName) return FALSE;
   if (PS_JPGFileLongName != cq.PS_JPGFileLongName) return FALSE;
   if (ShortName != cq.ShortName) return FALSE;
   if (Description != cq.Description) return FALSE;
   if (BoundingBoxStr != cq.BoundingBoxStr) return FALSE;
   if (Status != cq.Status) return FALSE;
   if (PreviewStatus != cq.PreviewStatus) return FALSE;      
   if (ctsElapsedTime != cq.ctsElapsedTime) return FALSE;
   if (iFileSize != cq.iFileSize) return FALSE;
   if (RealFileSize != cq.RealFileSize) return FALSE;
   if (csRealFileSize != cq.csRealFileSize) return FALSE;
   if (ctFileTime != cq.ctFileTime) return FALSE;            
   if (EPSF != cq.EPSF) return FALSE;
   if (CATHeaderNeeded != cq.CATHeaderNeeded) return FALSE;
   if (StartofValidPS != cq.StartofValidPS) return FALSE;
   if (BatchHeaderNotes != cq.BatchHeaderNotes) return FALSE;      
   if (BatchHeaderOptions != cq.BatchHeaderOptions) return FALSE;
   if (BatchHeaderCount != cq.BatchHeaderCount) return FALSE;
   if (ViewPerspective != cq.ViewPerspective) return FALSE;
   if (StatusFlag != cq.StatusFlag) return FALSE;	  
   if (WasTouched != cq.WasTouched) return FALSE;
   if (ExtraInfo != cq.ExtraInfo) return FALSE;
   if (QPos != cq.QPos) return FALSE;
   if (IsComplete != cq.IsComplete) return FALSE;
   if (MutexName != cq.MutexName) return FALSE;
   if (PageCount != cq.PageCount) return FALSE;
   if (iPrintToHold != cq.iPrintToHold) return FALSE;
   if (csOwner != cq.csOwner) return FALSE;
   if (csSubmitted != cq.csSubmitted) return FALSE;	  

   // well, I guess they are the same 
   return TRUE;
   }

const CQueueFile& CQueueFile::operator=(const CQueueFile& cqfSrc) 
   {                                                              
   // assignment	  
   if (this != &cqfSrc)
      {        
      FileType = cqfSrc.FileType;
      Selected = cqfSrc.Selected;
      Prefs = cqfSrc.Prefs;
      BoundingBox = cqfSrc.BoundingBox;
      if (cqfSrc.pCD && cqfSrc.Prefs & USE_LOCAL_DEV_PREFS)
         {
         // create a new device
         pCD = new CDevice;  
         // copy it
         *pCD = *(cqfSrc.pCD);
         }
      if (cqfSrc.pTransform && cqfSrc.Prefs & USE_LOCAL_TRN_PREFS)
         {
         // create a new transform
         pTransform = new CTransform;
         // copy it
         *pTransform = *(cqfSrc.pTransform);
         }
      if (cqfSrc.pCC && cqfSrc.Prefs & USE_LOCAL_COL_PREFS)
         {
         // create a new color transform
         pCC = new CColorTransform;
         // copy it
         *pCC = *(cqfSrc.pCC);
         }
      if (cqfSrc.LongName.GetLength()) LongName = cqfSrc.LongName;
      if (cqfSrc.PS_JPGFileLongName.GetLength()) PS_JPGFileLongName = cqfSrc.PS_JPGFileLongName;
      if (cqfSrc.ShortName.GetLength()) ShortName = cqfSrc.ShortName;
      if (cqfSrc.Description.GetLength()) Description = cqfSrc.Description;
      if (cqfSrc.BoundingBoxStr.GetLength()) BoundingBoxStr = cqfSrc.BoundingBoxStr;
      if (cqfSrc.Status.GetLength()) Status = cqfSrc.Status;
      if (cqfSrc.PreviewStatus.GetLength()) PreviewStatus = cqfSrc.PreviewStatus;      
      ctsElapsedTime = cqfSrc.ctsElapsedTime;
	  iFileSize = cqfSrc.iFileSize;
	  RealFileSize = cqfSrc.RealFileSize;
	  csRealFileSize = cqfSrc.csRealFileSize;
      ctFileTime = cqfSrc.ctFileTime;            
      EPSF = cqfSrc.EPSF;
      CATHeaderNeeded = cqfSrc.CATHeaderNeeded;
      StartofValidPS = cqfSrc.StartofValidPS;
      if (cqfSrc.BatchHeaderNotes.GetLength()) BatchHeaderNotes = cqfSrc.BatchHeaderNotes;      
      BatchHeaderOptions = cqfSrc.BatchHeaderOptions;
      BatchHeaderCount = cqfSrc.BatchHeaderCount;
	  ViewPerspective = cqfSrc.ViewPerspective;
      StatusFlag = cqfSrc.StatusFlag;	  
	  WasTouched = cqfSrc.WasTouched;
	  ExtraInfo = cqfSrc.ExtraInfo;
	  QPos = cqfSrc.QPos;
	  IsComplete = cqfSrc.IsComplete;
	  MutexName = cqfSrc.MutexName;
	  PageCount = cqfSrc.PageCount;
      iPrintToHold = cqfSrc.iPrintToHold;
	  csOwner = cqfSrc.csOwner;
	  csSubmitted = cqfSrc.csSubmitted;
	  InQueue = cqfSrc.InQueue;
	  iJobID = cqfSrc.iJobID;
	  m_prange_edit = cqfSrc.m_prange_edit;
	  m_prange = cqfSrc.m_prange;
      }                
   return *this;   
   }   	    

void CQueueFile::GetFileTypeName(LPSTR lpstr)
   {
   CString cs;

   // get the file type
   GetFileTypeName(cs);

   // copy to our buffer
   strcpy(lpstr, (const char*)cs);
   }

void CQueueFile::GetFileTypeName(CString& cs)
   {             
   switch(FileType)
      {
      case FTYPE_PSCRIPT:
           cs.LoadString(IDS_FFORMAT_POSTSCRIPT);
           break;
      case FTYPE_BLIST:
           cs.LoadString(IDS_FFORMAT_BATCHHEADER);
           break;                  
	  case FTYPE_PSD:
		   cs.LoadString(IDS_FFORMAT_PSD);
		   break;
      case FTYPE_PCX:     
           cs.LoadString(IDS_FFORMAT_PCX);
           break;
      case FTYPE_GIF:     
	       cs.LoadString(IDS_FFORMAT_GIF);
		   break;               
      case FTYPE_TIFF:
	       cs.LoadString(IDS_FFORMAT_TIFF);
           break;
      case FTYPE_TARGA:
	       cs.LoadString(IDS_FFORMAT_TARGA);
           break;               
      case FTYPE_BMP:
	       cs.LoadString(IDS_FFORMAT_BMP);
           break;
      case FTYPE_JFIF:
	       cs.LoadString(IDS_FFORMAT_JFIF);
		   break;
      case FTYPE_JTIF:
	       cs.LoadString(IDS_FFORMAT_JTIF);
           break;
      case FTYPE_OS2:
	       cs.LoadString(IDS_FFORMAT_OS2);
           break;
      case FTYPE_CALS:
	       cs.LoadString(IDS_FFORMAT_CALS);
           break;
      case FTYPE_MAC:
	       cs.LoadString(IDS_FFORMAT_MACPAINT);
           break;
      case FTYPE_IMG:
	       cs.LoadString(IDS_FFORMAT_IMG);           
           break;
      case FILE_MSP:     
	       cs.LoadString(IDS_FFORMAT_MSP);            
           break;
      case FTYPE_WPG:
	       cs.LoadString(IDS_FFORMAT_WPG);           
           break;
      case FTYPE_RAS:
	       cs.LoadString(IDS_FFORMAT_RAS);           
           break;
      case FTYPE_PCT:
	       cs.LoadString(IDS_FFORMAT_PCT);           
           break;
      case FTYPE_PCD:
	       cs.LoadString(IDS_FFORMAT_PCD);           
           break;           
      case FTYPE_PNG:
	       cs.LoadString(IDS_FFORMAT_PNG);           
           break;           
      case FTYPE_FPX:
		   cs.LoadString(IDS_FFORMAT_FPX);
           break;
      case FTYPE_PDF:
		   cs.LoadString(IDS_FFORMAT_PDF);
           break;
      case FTYPE_EXIF:
		   cs.LoadString(IDS_FFORMAT_EXIF);
           break;
	  case FTYPE_SGI:
		   cs.LoadString(IDS_FFORMAT_SGI);
		   break;
	  case FTYPE_UNSUPPORTED:
		   cs.LoadString(IDS_FFORMAT_UNSUPPORTED);
		   break;
      default:
	       cs.LoadString(IDS_FFORMAT_UNKNOWN);           
           break;
      }   
   }               
                                   
void CQueueFile::GetDateString(CString& csDate)
   {
   // convert date
   if (FileType == FTYPE_BLIST)
      csDate.LoadString(IDS_NOT_AVAIL);      
   else
      csDate = ctFileTime.Format("%b %d, %Y");      
   }

void CQueueFile::GetTimeString(CString& csTime)
   {
   // convert date
   if (FileType == FTYPE_BLIST)
      csTime.LoadString(IDS_NOT_AVAIL);
   else
      csTime = ctFileTime.Format("%I:%M%p");
   }

void CQueueFile::GetSizeString(CString& csSize)
   {
   NumberFormat nm;
   // convert size to a string
   nm.ConvertToComma(iFileSize, csSize);
   }

// get the RasterPlus95 server file info
// on pass 1, we instantiate a new device and gather info
// then we delete this device and load the queue device, 
// and reload the options.  We do this so that we don't
// have differently instantiated devices which could cause
// an irrelevent difference in the device (e.g. Memory alloc)
// which would force a Job Props to show up and confuse the user
BOOL CQueueFile::GetServerFileInfo(PSERVERXFER4 pServerXfer, int iPass, 
								   CDevice* pPassedDevice)
   {
   int i;
   BOOL ret;
   BOOL bFoundPageBoundingBox;
   int NumLines, NumGot;   
   char TestStr[MAXTESTSTRLEN];   
   char* p;     
   CStdioFile csfCheckFile;
   BOOL EndDocSetup;
   CString Token;
   CString Value;
   CDeviceList cdl;
   int DevNum;
   CString cs;
   CString csRPDEBUGMsg;

   // only look for the first bounding box on a file print
   BOOL bFoundBBox = FALSE;

   csRPDEBUGMsg.Format("GetServerFileInfo: Pass = %d", iPass);
   RPDEBUGSTRMFC(csRPDEBUGMsg);

   if (iPass == 2)
	   goto PARSING_ONLY;

   // init these
   MacOrientOptimize = FALSE;
   G95OrientOptimize = FALSE;
   CorelOrientOptimize = FALSE;
   CreatedByNT = FALSE;   
   CreatedByNT2000 = FALSE;   
   OrientOptimize;
   OriginalBoundingBox;

   // default for alternate device strategy for pm 6.5
   AltDevNum = -1;

   // get all devices
   cdl.LoadAll();      

   // open the file   
   if (!csfCheckFile.Open(LongName, 
	  CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone))
		return FALSE;	  
   
   // basic check for ps file server
   if (GetPSLine(csfCheckFile, TestStr, MAXTESTSTRLEN) > 0)
      {
	  if (!ServerTokenMatch(TestStr, PSLOOKUP_RP95_ADOBE_30))
	     {
		 csfCheckFile.Close(); 
	     return FALSE;
		 }
	  }   	  

   // defaults
   FileType = FTYPE_PSCRIPT;
   ViewPerspective = TOP_LEFT;
   EPSF = 0; 
   Prefs = USE_PREFS_GLOBAL;

   // load the device from the serverxfer struct
   cs = pServerXfer->pDriverName;

   // if no driver, then use printer name
   if (cs.GetLength() < 1)
	  cs = pServerXfer->PrinterName;

   DevNum = cdl.LookupName(cs);

   if ( DevNum == -1 )
   {
	   //Try getting the Device Based on the QName
	   DevNum = ((CZeusApp*)AfxGetApp())->GetQueueDevice(cs);
   }

   if (DevNum != -1)
	  {	  
#if 0
	  Prefs |= USE_LOCAL_COL_PREFS;
#endif
	  // load it
	  Prefs |= USE_LOCAL_DEV_PREFS;
	  pCD = new CDevice;
	  pCD->LoadDevice(DevNum);
#if 0
	  pCC = new CColorTransform;
#endif												 
	  // for now, we are always using the queue color
      pCC = NULL; 
#if 0	  
	  pCC->CPInitStruct.GlobalPrinterID = DevNum;
	  pCC->CPInitStruct.Printer_CSpace = pCD->pDevInfo->iColorSpace;
	  strncpy(pCC->CPInitStruct.Printer_Name,cs,MAX_PATH);
	  pCC->LoadDefaultProfiles();
#endif
	  }

   NumLines = 1;      
   EndDocSetup = FALSE;

   csDebug.Format("%s GetServerFileInfo\n", "Starting");
   RPDEBUGSTRMFC(csDebug);

   do
      {
	  NumGot = GetPSLine(csfCheckFile, TestStr, MAXTESTSTRLEN);
	  if (NumGot == -1) break;
	  NumLines++;
	  p = TestStr;         
	  
	  if (strncmp(TestStr, "%%", 2) == 0)
	  {
		csDebug.Format("TestStr(%d) = %s\n", NumLines, TestStr );
		RPDEBUGSTRMFC(csDebug);
	  }

	  // look for regular bounding box
	  if (ServerTokenMatch(TestStr, PSLOOKUP_BOUNDINGBOX) && !bFoundBBox)	  
	     {	     
	     if (!strstr(TestStr, PS_Token[PSLOOKUP_QUESTION]) &&
	         !strstr(TestStr, PS_Token[PSLOOKUP_ATEND]))	        
			{
		    ExtractBoundingBox((const char*)(p + lstrlen(PS_Token[PSLOOKUP_BOUNDINGBOX]) + 1));
		    OriginalBoundingBox = BoundingBox;
		    bFoundBBox = TRUE;

			csDebug.Format("PSLOOKUP_BOUNDINGBOX = top(%d), left(%d) , right(%d), bottom(%d)\n", BoundingBox.top, BoundingBox.left, BoundingBox.right, BoundingBox.bottom );
			RPDEBUGSTRMFC(csDebug);

	        continue;
			}
		 }
      // look for ADO BoundingBox
	  else if (ServerTokenMatch(TestStr, PSLOOKUP_ADOIAREA) && !bFoundBBox)	  	  
	     {
	     if (!strstr(TestStr, PS_Token[PSLOOKUP_QUESTION]) &&
	         !strstr(TestStr, PS_Token[PSLOOKUP_ATEND]))	        
	        {
			ExtractBoundingBox((const char*)(TestStr + lstrlen(PS_Token[PSLOOKUP_ADOIAREA]) + 1));
			bFoundBBox = TRUE;

			csDebug.Format("PSLOOKUP_ADOIAREA = top(%d), left(%d) , right(%d), bottom(%d)\n", BoundingBox.top, BoundingBox.left, BoundingBox.right, BoundingBox.bottom );
			RPDEBUGSTRMFC(csDebug);

			continue;
	        }
	    }		     
      // look for PageBoundingBox 
	  else if (ServerTokenMatch(TestStr, PSLOOKUP_PAGEBOUNDINGBOX) && !bFoundBBox)	  	  	  
	     {             
	     // is it real?
	     if (!strstr(TestStr, PS_Token[PSLOOKUP_QUESTION]) &&
	         !strstr(TestStr, PS_Token[PSLOOKUP_ATEND]))	        
	        {                                   
			ExtractBoundingBox((const char*)(TestStr + lstrlen(PS_Token[PSLOOKUP_PAGEBOUNDINGBOX]) + 1));
			bFoundBBox = TRUE;

			csDebug.Format("PSLOOKUP_PAGEBOUNDINGBOX = top(%d), left(%d) , right(%d), bottom(%d)\n", BoundingBox.top, BoundingBox.left, BoundingBox.right, BoundingBox.bottom );
			RPDEBUGSTRMFC(csDebug);

			continue;
	        }
	     }	 
	  // look for HiRes bounding box
	  else if (ServerTokenMatch(TestStr, PSLOOKUP_HIRES_BOUNDINGBOX) && !bFoundBBox)	  
	     {	     
	     if (!strstr(TestStr, PS_Token[PSLOOKUP_QUESTION]) &&
	         !strstr(TestStr, PS_Token[PSLOOKUP_ATEND]))	        
			{
		    ExtractBoundingBox((const char*)(p + lstrlen(PS_Token[PSLOOKUP_HIRES_BOUNDINGBOX]) + 1));
		    OriginalBoundingBox = BoundingBox;
		    bFoundBBox = TRUE;

			csDebug.Format("PSLOOKUP_HIRES_BOUNDINGBOX = top(%d), left(%d) , right(%d), bottom(%d)\n", BoundingBox.top, BoundingBox.left, BoundingBox.right, BoundingBox.bottom );
			RPDEBUGSTRMFC(csDebug);

	        continue;
			}
		 }
	  // look for begin setup
      else if (ServerTokenMatch(TestStr, PSLOOKUP_RP95_BEGIN_SETUP))	  
	     {		 

		 csDebug.Format("PSLOOKUP_RP95_BEGIN_SETUP = %s)\n", "" );
		 RPDEBUGSTRMFC(csDebug);

		 continue;
		 }		 	  
      else if (ServerTokenMatch(TestStr, PSLOOKUP_RP95_END_SETUP))	  
	     {
		 EndDocSetup = TRUE;

		 csDebug.Format("PSLOOKUP_RP95_END_SETUP = %s)\n", "" );
		 RPDEBUGSTRMFC(csDebug);

		 continue;
		 }		 	  
	  // special mac orient optimizations
	  else if (ServerTokenMatch(TestStr, PSLOOKUP_RP95_MAC_ORIENT_LANDSCAPE))
		 {
         MacOrientOptimize = TRUE;
         OrientOptimize = DEV_ORIENT_LANDSCAPE;

		 csDebug.Format("PSLOOKUP_RP95_MAC_ORIENT_LANDSCAPE = %s)\n", "" );
		 RPDEBUGSTRMFC(csDebug);

		 continue;
		 }
	  else if (ServerTokenMatch(TestStr, PSLOOKUP_RP95_MAC_ORIENT_PORTRAIT))
		 {
         MacOrientOptimize = TRUE;
         OrientOptimize = DEV_ORIENT_PORTRAIT;

		 csDebug.Format("PSLOOKUP_RP95_MAC_ORIENT_PORTRAIT = %s)\n", "" );
		 RPDEBUGSTRMFC(csDebug);

		 continue;
		 }
	  // generic 95 ps driver orientations
	  else if (ServerTokenMatch(TestStr, PSLOOKUP_RP95_95_ORIENT_PORTRAIT))
		 {
		 G95OrientOptimize = TRUE;
		 OrientOptimize = DEV_ORIENT_PORTRAIT;

		 csDebug.Format("PSLOOKUP_RP95_95_ORIENT_PORTRAIT = %s)\n", "" );
		 RPDEBUGSTRMFC(csDebug);

		 continue;
		 }
	  else if (ServerTokenMatch(TestStr, PSLOOKUP_RP95_95_ORIENT_LANDSCAPE))
		 {
		 G95OrientOptimize = TRUE;
		 OrientOptimize = DEV_ORIENT_LANDSCAPE;

		 csDebug.Format("PSLOOKUP_RP95_95_ORIENT_LANDSCAPE = %s)\n", "" );
		 RPDEBUGSTRMFC(csDebug);

		 continue;
		 }
	  else if (ServerTokenMatch(TestStr, PSLOOKUP_COREL_ORIENT_LANDSCAPE))
		 {
		 CorelOrientOptimize = TRUE;
		 OrientOptimize = DEV_ORIENT_LANDSCAPE;

		 csDebug.Format("PSLOOKUP_COREL_ORIENT_LANDSCAPE = %s)\n", "" );
		 RPDEBUGSTRMFC(csDebug);

		 continue;
		 }
	  else if (ServerTokenMatch(TestStr, PSLOOKUP_WINNT_CREATOR))
		 {
		 CreatedByNT = TRUE;

		 csDebug.Format("PSLOOKUP_WINNT_CREATOR = %s)\n", "" );
		 RPDEBUGSTRMFC(csDebug);

		 continue;
		 }
	  else if (ServerTokenMatch(TestStr, PSLOOKUP_WIN2000_CREATOR))
		 {
		 CreatedByNT2000 = TRUE;

		 csDebug.Format("PSLOOKUP_WIN2000_CREATOR = %s)\n", "" );
		 RPDEBUGSTRMFC(csDebug);

		 continue;
		 }
	  // Special HACK for Quark on the MAC.  Seems that if you leave
	  // all print options at DEFAULT, the page size doesn't come through
	  // and gets "set" here.  Since we have configured JAWS to ignore this
	  // we must parse it here
	  else if (ServerTokenMatch(TestStr, PSLOOKUP_QUARK_PAGE_SIZE))
		 {		 
		 // set token
		 Token = "*PageRegion ";		 
		 // get value
	     Value = p + lstrlen(PS_Token[PSLOOKUP_QUARK_PAGE_SIZE]) - 2;
		 Token += (p + lstrlen(PS_Token[PSLOOKUP_QUARK_PAGE_SIZE]));
			 
		 csDebug.Format("PSLOOKUP_QUARK_PAGE_SIZE = %s)\n", "" );
		 RPDEBUGSTRMFC(csDebug);
	 
		 (Token, Value, cdl);
	  }
	  else if (ServerTokenMatch(TestStr, PSLOOKUP_RP95_BEGIN_FEATURE))	  
	     {
		 // get token
	     Token = p + lstrlen(PS_Token[PSLOOKUP_RP95_BEGIN_FEATURE]) + 1;     		 		 
		 
		 csDebug.Format("PSLOOKUP_RP95_BEGIN_FEATURE = %s)\n", "" );
		 RPDEBUGSTRMFC(csDebug);

		 // get value
		 if (GetPSLine(csfCheckFile, TestStr, MAXTESTSTRLEN) > 0)
		    Value = TestStr;
		 NumLines++;

		 csDebug.Format("   BeginFeature Data = %s)\n", TestStr );
		 RPDEBUGSTRMFC(csDebug);

		 // blank line!
		 if (ServerTokenMatch(TestStr, PSLOOKUP_RP95_END_FEATURE)) continue;

		 // we may have a print to hold token here
		 if (ServerTokenMatch(Token, PSLOOKUP_PRINTTOHOLD))
			{
			if (strstr(Value, "Yes") != NULL)
			   iPrintToHold = QUEUESTYLE_HOLD;
		 
			 csDebug.Format("PSLOOKUP_PRINTTOHOLD = %s)\n", "" );
			 RPDEBUGSTRMFC(csDebug);

			}			
	  	 else if (ServerTokenMatch(Token, PSLOOKUP_PRINTTOHOLD_2))
			{
			if (strstr(Value, "Yes") != NULL)
			   iPrintToHold = QUEUESTYLE_HOLD;
		 
			 csDebug.Format("PSLOOKUP_PRINTTOHOLD_2 = %s)\n", "" );
			 RPDEBUGSTRMFC(csDebug);
			}

		 // keep getting lines until an EndFeature is encountered
		 do
		    {
			NumGot = GetPSLine(csfCheckFile, TestStr, MAXTESTSTRLEN);
			if (NumGot == -1)
				break;

			 csDebug.Format("   BeginFeature DataLoop = %s)\n", TestStr );
			 RPDEBUGSTRMFC(csDebug);

			// we may have a bounding box in here
			p = TestStr;
			bFoundPageBoundingBox = ServerTokenMatch(p, PSLOOKUP_PAGEBOUNDINGBOX);

			if ( bFoundPageBoundingBox == FALSE )
			{
			    p = TestStr + 1;
				bFoundPageBoundingBox = ServerTokenMatch(p, PSLOOKUP_PAGEBOUNDINGBOX);
			}
			if ( bFoundPageBoundingBox == TRUE  && !bFoundBBox)
//			if (ServerTokenMatch(TestStr, PSLOOKUP_PAGEBOUNDINGBOX) && !bFoundBBox)
			   {
			   ExtractBoundingBox((const char*)(p + lstrlen(PS_Token[PSLOOKUP_PAGEBOUNDINGBOX]) + 1));
//			   ExtractBoundingBox((const char*)(TestStr + lstrlen(PS_Token[PSLOOKUP_PAGEBOUNDINGBOX]) + 1));
			   bFoundBBox = TRUE;
		 
			   csDebug.Format("PSLOOKUP_PAGEBOUNDINGBOX = top(%d), left(%d) , right(%d), bottom(%d)\n", BoundingBox.top, BoundingBox.left, BoundingBox.right, BoundingBox.bottom );
			   RPDEBUGSTRMFC(csDebug);

			   }
			// append TestStr into Value
			NumLines++;	 			
			} while (!ServerTokenMatch(TestStr, PSLOOKUP_RP95_END_FEATURE) && 
			          NumLines < NUMLINESTOTEST);
		 ParseToken(Token, Value, cdl);
		 continue;
		 }		 	  
	  // keep going until we have checked all our test lines or we 
	  // have reached the end of the doc setup
	  } while (NumLines < NUMLINESTOTEST && !EndDocSetup && NumGot != -1); 

   // close file
   csfCheckFile.Close(); 

   // if device was not found conventionally, check our alternate
   // device
   if (!pCD)
	  {	  	  	  
	  if (AltDevNum != -1)
	     {
		 Prefs |= USE_LOCAL_DEV_PREFS;
#if 0
		 Prefs |= USE_LOCAL_COL_PREFS;
#endif
		 // load it
		 pCD = new CDevice;
		 pCD->LoadDevice(AltDevNum);
#if 0
  	     pCC = new CColorTransform;
		 pCC->CPInitStruct.GlobalPrinterID = AltDevNum;
		 pCC->CPInitStruct.Printer_CSpace = pCD->pDevInfo->iColorSpace;
		 strncpy(pCC->CPInitStruct.Printer_Name,cs,MAX_PATH);
		 pCC->LoadDefaultProfiles();
#endif
		 pCC = NULL;
		 }
	  }

PARSING_ONLY:
   csDebug.Format("%s PARSING\n", "Starting");
   RPDEBUGSTRMFC(csDebug);

   // use the passed device on the second cut
   if (iPass == 2)
	   pCD = pPassedDevice;
   
   // if we found a device, parse the setup
   if (pCD)
      {	  
	  // don't have bounding box 
	  pCD->pDevInfo->GotBoundingBox = FALSE;
	  CZeusServerToken* pzst;
      for (i = 0; i < CZeusServerTokenArray.GetSize(); i++)
         {
		 // get the token
		 pzst = (CZeusServerToken*)CZeusServerTokenArray.GetAt(i);
		 // send it to the device
		 pCD->ParseServerTokens(pzst->Token, pzst->Value);
		 }  
	  // reload our options
	  // note that memory option will get set to default which
	  // is automatic

//      if (iPass == 1)
	 	  pCD->GetDevInfo();

	  if (pCC) 
		 {
		 strncpy(pCC->CPInitStruct.Printer_Name,pCD->Name,MAX_PATH);
         pCC->CPInitStruct.GlobalPrinterID = pCD->DevIdx; 
	     pCC->CPInitStruct.Printer_CSpace = pCD->pDevInfo->iColorSpace; 
		 pCC->LoadDefaultProfiles();
		 }
	  // if we got a new bounding box, reset the box
	  if (pCD->pDevInfo->GotBoundingBox)
		 {
		 BoundingBox = pCD->pDevInfo->BoundingBox;
		 BoundingBoxStr.Format("%d %d %d %d", 
			BoundingBox.left, BoundingBox.bottom, BoundingBox.right, BoundingBox.top);
		 }
      // set the connection
      SetConnect(pServerXfer->PortName);

	  pCD->pDevInfo->Copies = 1;

	  // to get rid of flipping
	  if (CreatedByNT2000) ExtraInfo |= CQUEUE_EXTRA_WINSLIDE;

	  // if the device has variable orientation, set optimum if possible
	  if (pCD->pDevInfo->CanDoOrientation && pCD->pDevInfo->CanOptimizeOrientation &&
		 pCD->pDevInfo->OptimizeOrientation)
		 {
		 if (MacOrientOptimize)
			{
			pCD->pDevInfo->Orientation = OrientOptimize;
			}
		 else if (G95OrientOptimize)
			{
			pCD->pDevInfo->Orientation = OrientOptimize;
			}
		 else if (CorelOrientOptimize)
			{
			pCD->pDevInfo->Orientation = OrientOptimize;
			}
		 else if (CreatedByNT || CreatedByNT2000)
			{
		    if (abs(OriginalBoundingBox.Width()) > abs(OriginalBoundingBox.Height()))
			   pCD->pDevInfo->Orientation = DEV_ORIENT_LANDSCAPE;
		    else
			   pCD->pDevInfo->Orientation = DEV_ORIENT_PORTRAIT;		    
			}
		 }

	  // get the 
	  ret = TRUE;
	  }
   else
   {
      ret = FALSE;   
	}

   // delete the token array (we might have this even if we
   // don't have a device)
   if (iPass == 2)
	  {
      for (i = 0; i < CZeusServerTokenArray.GetSize(); i++)
		 {
         // delete it from the array		 
         delete CZeusServerTokenArray.GetAt(i);
		 }  
      // remove the pointers
      CZeusServerTokenArray.RemoveAll();      
	  }

   return ret;
   }

BOOL CQueueFile::ServerTokenMatch(char *Token, int TokenIdx)
   {
   if (strncmp(Token, PS_Token[TokenIdx], strlen(PS_Token[TokenIdx])) == 0)
      return TRUE;
   else
      return FALSE;
   }

BOOL CQueueFile::ServerTokenMatch(CString& csToken, int TokenIdx)
   {
   if (strncmp((const char*)csToken, PS_Token[TokenIdx], strlen(PS_Token[TokenIdx])) == 0)
      return TRUE;
   else
      return FALSE;
   }

void CQueueFile::ParseToken(CString& Token, CString& Value, CDeviceList& cdl)
   {        
   CString csVersion;
   CZeusServerToken* pzst;   
   char NewValue[MAX_PATH];   
   char* p;

   // get device
   if (Token.Compare(PS_Token[PSLOOKUP_RP95_SERVER_DEVNAME]) == 0)
      {
	  // extract the value without the %%
	  strcpy(NewValue, Value);
	  p = strtok(NewValue, "%%()");
	  strcpy(NewValue, p);
	  Value = NewValue;		 
      AltDevNum = cdl.LookupName(Value);
	  }	 
   else
      {	  
	  // is it already in here?	  
      for (int i = 0; i < CZeusServerTokenArray.GetSize(); i++)
		 {
		 pzst = (CZeusServerToken*)CZeusServerTokenArray.GetAt(i);	     
		 if (pzst->Token == Token)
			return;
		 }

	  // load the token and value
	  pzst = new CZeusServerToken;
	  pzst->Token = Token;	        
	  pzst->Value = Value;			
	  // add to CObArray	  
      CZeusServerTokenArray.Add(pzst);
	  }      
   }

IMPLEMENT_SERIAL(CZeusServerToken, CObject, 0)

void CZeusServerToken::Serialize(CArchive& ar)
   {
   if (ar.IsStoring())
      {
      ar << Token << Value;
	  }
   else
      {
      ar >> Token >> Value;
	  }
   }   

static int MaxPCDResolution[] = { 96, 192, 384, 768, 1536, 3072 };

CRect CQueueFile::GetSetOptimalBoundingBox(int Width, int Height, int* pPCDResolution)
   {
   int i, MaxDim;
   PCDINFO pcdinfo;
   CRect NewBoundingBox;
   FILEINFO FileInfo;

   if (FileType != FTYPE_PCD)
      return BoundingBox;
   else
      {
	  // block others from using the bitmap dll
      EnterCriticalSection(&BITMAPENGINEMUTEX);
   
	  //default
	  NewBoundingBox = BoundingBox;
	  // get maximum dimension
	  MaxDim = max(Width, Height);	  

	  // get all supported resolutions
      ::L_GetPCDResolution((char*)(const char*)LongName, &pcdinfo);

	  // get max needed resolution
	  for (i = L_PCD_16BASE; i >= L_PCD_BASE_OVER_64; i--)
	     {
		 // supported?
		 if (pcdinfo.resolution[i])
		    {
			// do we need it?
			if (MaxDim >= MaxPCDResolution[i])
			   {
			   ::L_SetPCDResolution(i);
			   // store it
			   *pPCDResolution = i;
			   // reset resolution based on this res

			   if ( !PS_JPGFileLongName.IsEmpty() )
			   {
				   csDebug.Format("BoundingBox::PS_JPGFileLongName %s\n", PS_JPGFileLongName );
				   RPDEBUGSTRMFC(csDebug);

				   memset(&FileInfo, 0, sizeof(FILEINFO));
				   FileInfo.uStructSize = sizeof(FILEINFO);
				   ::L_FileInfo((char*)(const char*)PS_JPGFileLongName, &FileInfo, sizeof(FILEINFO), FILEINFO_TOTALPAGES, NULL);
			   }
			   else
			   {
				   csDebug.Format("BoundingBox::LongName %s\n", LongName );
				   RPDEBUGSTRMFC(csDebug);

				   memset(&FileInfo, 0, sizeof(FILEINFO));
				   FileInfo.uStructSize = sizeof(FILEINFO);
				   ::L_FileInfo((char*)(const char*)LongName, &FileInfo, sizeof(FILEINFO), FILEINFO_TOTALPAGES, NULL);
			   }

			   // set actual bounding box   
               NewBoundingBox.left = 0;
               NewBoundingBox.bottom = 0;
               NewBoundingBox.right = FileInfo.Width;
               NewBoundingBox.top = FileInfo.Height;            
			   break;
			   }
			}
		 }
	  LeaveCriticalSection(&BITMAPENGINEMUTEX);
	  return NewBoundingBox;
	  }
   }					

void CQueueFile::SetConnect(char* Port)
   {   
   strcpy(pCD->pDevInfo->ConnectStr, Port);
   }

void CQueueFile::SetPages(CStdioFile& csfCheckFile)
   {
   char buf[256];
   char *p;
   DWORD cPos;

   cPos = csfCheckFile.GetPosition();

   // go to near the end of the file
   csfCheckFile.Seek(-256, CFile::end);

   // read in the last 256 bytes
   csfCheckFile.Read(buf, 256);

   // look for pages
   p = strstr(buf, "%%Pages:");

   // extract the page count
   if (p)
      {
	  PageCount = atoi(p + 9);
	  if (PageCount <= 0) PageCount = 1;
	  }
   csfCheckFile.Seek(cPos, CFile::begin);
   }

#pragma pack(1)
typedef struct tagOURTGAHEADER
   {
   BYTE  idLength,
	     mapType,
	     imageType;
   short mapOrigin,
         mapLength;
   BYTE  mapWidth;
   short xOrigin,
   	     yOrigin,
	     imageWidth,
	     imageHeight;
   BYTE  pixelDepth,
	     imageDesc;
   } OURTGAHEADER;
#pragma pack()

void CQueueFile::DoTargaHack(CStdioFile& csfCheckFile)
{
   OURTGAHEADER TgaHeader;

   csDebug.Format("Start DoTargaHack(%s)\n", LongName );
   RPDEBUGSTRMFC(csDebug);

   // default
   ViewPerspective = BOTTOM_LEFT;

   // open the file
   if (csfCheckFile.Open(LongName, 
      CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone))
      {
	  if (csfCheckFile.Read(&TgaHeader, sizeof(TgaHeader)) == sizeof(TgaHeader))      
	     {
	     if (TgaHeader.imageDesc & 0x30)
			ViewPerspective = TOP_RIGHT;
		 else if (TgaHeader.imageDesc & 0x20)
			ViewPerspective = TOP_LEFT;
		 else if (TgaHeader.imageDesc & 0x10)
			ViewPerspective = BOTTOM_RIGHT;
      	 }
      // close file
      csfCheckFile.Close();	
	  }

   csDebug.Format("DoTargaHack ViewPerspective(%d)\n", ViewPerspective );
   RPDEBUGSTRMFC(csDebug);
}

void CQueueFile::ExtractBoundingBox(const char* pin, BOOL bRotate)
   {
   char BBoxStr[MAXTESTSTRLEN];
   char* p;

   // defaults
   BoundingBox.left = 0;
   BoundingBox.bottom = 0;
   BoundingBox.right = 0;
   BoundingBox.top = 0;

   // copy to a working buffer
   lstrcpy(BBoxStr,  pin);

   // go thru this poop so we don't crash atoi with a NULL
   // add bracket for pdf parsing
   p = strtok(BBoxStr, " \t[");
   if (p) 
	  BoundingBox.left = atoi(p);
   else
	  goto ATEND;
   p = strtok(NULL, " \t");
   if (p) 
	  BoundingBox.bottom = atoi(p);
   else
	  goto ATEND;
   p = strtok(NULL, " \t");
   if (p)
   {
	   if ( bRotate )
			BoundingBox.top = atoi(p);
	   else
			BoundingBox.right = atoi(p);
   }
   else
	  goto ATEND;
   p = strtok(NULL, " \t");
   if (p) 
   {
	   if ( bRotate )
			BoundingBox.right = atoi(p);
	   else
			BoundingBox.top = atoi(p);
   }
   else
	  goto ATEND;

ATEND:
   // redo the string to make it nicer
   BoundingBoxStr.Format("%d %d %d %d", 
      BoundingBox.left, BoundingBox.bottom, 
	  BoundingBox.right, BoundingBox.top);

   csDebug.Format("::ExtractBoundingBox BoundingBox = top(%d), left(%d), right(%d), bottom(%d)\n", BoundingBox.top, BoundingBox.left, BoundingBox.right, BoundingBox.bottom );
   RPDEBUGSTRMFC(csDebug);

   csDebug.Format("::ExtractBoundingBox BoundingBoxStr = %d\n", BoundingBoxStr );
   RPDEBUGSTRMFC(csDebug);
   }

void CQueueFile::GetPDFInfo(CStdioFile& csfCheckFile)
   {                 
	BOOL bRotate = FALSE;
   int NumGot;                             
   int LineCount = 0;
   char TestStr[MAXTESTSTRLEN];
   char *p;   
   BOOL FoundBBox = FALSE;   
   BOOL FoundDescription = FALSE;   
   BOOL bFoundRotate = FALSE;

   do
      {
	  // get a line
	  NumGot = GetPSLine(csfCheckFile, TestStr, MAXTESTSTRLEN);
	  if (NumGot == -1) break;
	  LineCount++;
	     
      // look for Drop Box
	  if (p = strstr(TestStr, PS_Token[PSLOOKUP_PDF_CROP_BOX]))
	     {             
	     // extract bounding box
	     BoundingBoxStr = p + lstrlen(PS_Token[PSLOOKUP_PDF_CROP_BOX]) + 1;
	     FoundBBox = BBOX_FOUND_PBB;	     
		 continue;
	     }
      // look for regular bounding box
	  else if (p = strstr(TestStr, PS_Token[PSLOOKUP_PDF_MEDIA_BOX]))
	     {                         
	     // extract bounding box
	     BoundingBoxStr = p + lstrlen(PS_Token[PSLOOKUP_PDF_MEDIA_BOX]) + 1;
	     FoundBBox = BBOX_FOUND_BB;	     
		 continue;
	     }	        	  	     
	  else if (p = strstr(TestStr, PS_Token[PSLOOKUP_PDF_TITLE]))
	     {                         
	     // extract bounding box
	     Description = p + lstrlen(PS_Token[PSLOOKUP_PDF_TITLE]) + 1;
		 // this may be unicode, so convert it
		 ConvertDescFromHex(Description);
	     FoundDescription = TRUE;
		 continue;
	     }	        	  	     
	  else if (p = strstr(TestStr, PS_Token[PSLOOKUP_PDF_ROTATE]))
	     {                         
	     // extract bounding box
		 if ( !bFoundRotate )
		 {
			 PDFRotate = p + lstrlen(PS_Token[PSLOOKUP_PDF_ROTATE]) + 1;
			 if ( PDFRotate == "90 " || PDFRotate == "270 " )
				 bRotate = TRUE;
			 bFoundRotate = TRUE;
		 }
		 continue;
	     }	        	  	     
	  //} while ((LineCount < PDFLINESTOTEST && NumGot != -1) &&
	  } while ((NumGot != -1) &&
		 (!FoundBBox || !FoundDescription));
	     
   if (!FoundBBox)
      {     
      // not found!
      BoundingBoxStr.LoadString(IDS_NOT_AVAIL);
      BoundingBox.left = BoundingBox.right = 
      BoundingBox.top = BoundingBox.bottom = 0;         
      } 
   else
      {
	   ExtractBoundingBox(BoundingBoxStr, bRotate);
      }	           	       
   }

void CQueueFile::MakeCoolFileSizeStr()
   {
   if (RealFileSize > (1024 * 1024))
	  csRealFileSize.Format("%dM", RealFileSize / (1024 * 1024));
   else if (RealFileSize > (1024))
	  csRealFileSize.Format("%dK", RealFileSize / 1024);
   else 
	  csRealFileSize.Format("%d", RealFileSize);
   }

static char HexMap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
'A', 'B', 'C', 'D', 'E', 'F' };

int CQueueFile::GetHexVal(char c)
   {
   BYTE iRet = 0;

   for (int i = 0; i < 16; i++)
	  {
	  if (c == HexMap[i])
		 {
		 iRet = i;
		 break;
		 }
	  }
   return iRet;
   }

void CQueueFile::ConvertDescFromHex(CString& csIn)
   {
   const char* p;
   CString csRet; 
   int i;
   char c;   

   // start with nothing
   csRet = "";

   // hex encoding?
   if (csIn[0] != '<') return;
   
   // unicode?
   if (csIn.Left(5) == "<FEFF")
	  {
	  // point past the start
	  p = csIn;
	  p += 5;

	  do
		 {
		 p += 2;		 
		 c = *p++;
		 i = GetHexVal(c) * 16;		 
		 c = *p++;
		 i += GetHexVal(c);
		 csRet += (BYTE)i;
		 } while (*p != '>');
	  csIn = csRet;
	  }   
   }