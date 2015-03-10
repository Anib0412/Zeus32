/////////////////////////////////////////////////////////////////////////////
// RPResource.cpp
//                                                                        
// resource settings (including hot folder polling) for RasterPlus
// Copyright (C) 1999-2001 Graphx, Inc.
//
// mods:
//
// 10-18-99 Version 3.0.1 - hot folder could not pick up read-only files - fixed
// 10-19-99 Version 3.0.1 - if hot-folder file was deleted, the file should be removed
//                          from the checked list - fixed
// 02-07-00 Version 3.0.3 - alphabetize poll files before inserting them
// 2-16-00  Version 3.0.3 - weirdly dated files caused CFileStatus to ASSERT - 
//                          changed to FindFirstFile
// 11-06-01 Version 5.0.0 - reorganize temp path stuff
// 09-11-03 Version 5.1.7 - Change Server to delay 5 seconds before checking Hot Folder file
// 11-14-03 Version 5.2.2 - Allow the user to set the Hot Folder delay from 0 to 10 seconds
// 11-14-03 Version 5.2.2 - Change the Archive default to delete after 100 files


#include "stdafx.h"
#include "zeus.h"
#include "zeusdoc.h"
#include "RPResource.h"
#include "Profile.h"
#include "Servdefs.h"
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/locking.h>
#include <share.h>
#include <fcntl.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define DELAY_EDIT  5000
//#define DELAY_EDIT  0

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRPResource::CRPResource()
   {   
   CProfile Profile;

   // default for scratch dir   
   csScratchPath = ((CZeusApp*)AfxGetApp())->GetScratchFolder();
   // default for spool dir
   csSpoolDir = ((CZeusApp*)AfxGetApp())->GetSpoolFolder();
   // default hot folder
   csHotFolderDir = Profile.GetStr(SERVERSECTION, IDS_HOTFOLDER, csScratchPath);

   iListJobsResourceSelector = LISTJOBS_KEEP_MAX;
   iDiskFilesResourceSelector = DISKFILES_DELETE_AFTER_IMAGING;   
   iHotFolderResourceSelector = HOTFOLDER_DELETE_AFTER_IMAGING;
   iListJobsMax = DISKFILES_LISTJOBS_DEFCNT;

   //bEnableHotFolder = FALSE;
   iHotFolderType = HOTFOLDER_TYPE_DISABLED;
   bImageOnlyNew = TRUE;
   iHotFolderDelay = 0;
   iHotFolderInPane = 0;
   m_bSelectBySize = FALSE;
   iHotFolderDPI = 300;
   iHotFolderVariance = 0.25f;
   }

CRPResource::~CRPResource()
   {
   CProfile Profile;

   // write the default spool dir
   Profile.WriteStr(SERVERSECTION, IDS_HOTFOLDER, csHotFolderDir);	

   // empty CObArrays
   EmptyCheckedFiles();
   EmptyFilesToDo();
   }

void CRPResource::Serialize(CArchive& ar, WORD wVerCheck)
   {
   if (ar.IsStoring())
      {
      ar << csSpoolDir << csHotFolderDir << csScratchPath << 
      iDiskFilesResourceSelector << iListJobsResourceSelector <<
	  iHotFolderResourceSelector << iListJobsMax <<
	  iHotFolderInPane << iHotFolderType << bImageOnlyNew << iHotFolderDelay;
      ar << m_bSelectBySize << iHotFolderDPI  << iHotFolderVariance;

	  }
   else
	{
		switch ( wVerCheck )
		{
		case ZEUSDOC_VERSION_A:
		case ZEUSDOC_VERSION_9:
		case ZEUSDOC_VERSION_8:
			ar >> csSpoolDir >> csHotFolderDir >> csScratchPath >>
			iDiskFilesResourceSelector >> iListJobsResourceSelector >>
			iHotFolderResourceSelector >> iListJobsMax >>
			iHotFolderInPane >> iHotFolderType >> bImageOnlyNew;
			break;
		// default is always Current Version
		case ZEUSDOC_VERSION_7:
			ar >> csSpoolDir >> csHotFolderDir >> csScratchPath >>
			iDiskFilesResourceSelector >> iListJobsResourceSelector >>
			iHotFolderResourceSelector >> iListJobsMax >>
			iHotFolderInPane >> iHotFolderType >> bImageOnlyNew >> iHotFolderDelay;
			break;
		case ZEUSDOC_VERSION_6:
			ar >> csSpoolDir >> csHotFolderDir >> csScratchPath >>
			iDiskFilesResourceSelector >> iListJobsResourceSelector >>
			iHotFolderResourceSelector >> iListJobsMax >>
			iHotFolderInPane >> iHotFolderType >> bImageOnlyNew >> iHotFolderDelay;
			ar >> m_bSelectBySize >> iHotFolderDPI >> iHotFolderVariance;
			break;
		case ZEUSDOC_NO_VERSION:
		default:
			ar >> csSpoolDir >> csHotFolderDir >> csScratchPath >>
			iDiskFilesResourceSelector >> iListJobsResourceSelector >>
			iHotFolderResourceSelector >> iListJobsMax >>
			iHotFolderInPane >> iHotFolderType >> bImageOnlyNew >> iHotFolderDelay;
			ar >> m_bSelectBySize >> iHotFolderDPI >> iHotFolderVariance;
			break;
		}
   }
}   

void CRPResource::InitPolling()
   {
   // empty CObArrays
   EmptyCheckedFiles();
   EmptyFilesToDo();

   // if image only new is set, store the files
   // currently in the poll dir in the checked array
   if (bImageOnlyNew)
      InsertFiles(TRUE);
   }

void CRPResource::EmptyCheckedFiles()
   {
   int i = 0;

   // remove contents 
   while (i < CheckedFiles.GetSize())
      {
      delete CheckedFiles.GetAt(i++);
      }     
   CheckedFiles.RemoveAll();
   }

void CRPResource::EmptyFilesToDo()
   {
   int i = 0;

   // remove contents 
   while (i < FilesToDo.GetSize())
      {
      delete FilesToDo.GetAt(i++);
      }     
   FilesToDo.RemoveAll();
   }

int CRPResource::InsertFiles(BOOL CheckedOnly)
   {
   int i;   
   HANDLE hFile;
   WIN32_FIND_DATA FileInfo;

   CString csSearch, csFile;
   CServerFile* pSF;
   BOOL IsRepeat;
   int NumAdded = 0;
   int TotalChecked;
   BOOL Added;

   if ( CheckedOnly )
	   iDelayEdit = 0;
   else
//#if HOTFOLDER_DELAY
	   iDelayEdit = iHotFolderDelay * 1000;
//#else
//	   iDelayEdit = DELAY_EDIT;
//#endif

   // make sure the slash is there
   VERIFYPATH(csHotFolderDir);
   
   // make search string
   csSearch = csHotFolderDir;
   csSearch += "*.*";

   TotalChecked = CheckedFiles.GetSize();

   // check for existence
   memset(&FileInfo, 0, sizeof WIN32_FIND_DATA);
   if ((hFile = FindFirstFile(csSearch, &FileInfo)) != INVALID_HANDLE_VALUE)
	  {      
	  do
	     {		 
		 // don't do these
		 if ((FileInfo.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ||
		     (FileInfo.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ||
			 (FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||			 
			 (FileInfo.nFileSizeLow == 0 && FileInfo.nFileSizeHigh == 0))
			 continue;
		 IsRepeat = FALSE;
		 // create file name
		 csFile = csHotFolderDir;
		 csFile += FileInfo.cFileName;

	     // create a new CServerFile
	     pSF = new CServerFile(csFile);
		 Added = FALSE;		 
		 // have we already done this?
		 for (i = 0; i < TotalChecked; i++)
		    {
			// duplicate!
			if (*pSF == *((CServerFile*)CheckedFiles.GetAt(i)))
			   {
			   IsRepeat = TRUE;			   
			   break;
			   }
			}
	     // add to the FilesToDo and CheckedFiles arrays
		 // if we have access
	     //if (!IsRepeat && pSF->HasReadWritePriv()) 
		 if (!IsRepeat && pSF->FullReadPrivileges(FileInfo.nFileSizeLow)) 		 		 
		    {
			// wait delay time and check again
			Sleep(iDelayEdit);
			if (!pSF->HasChanged())
			   {
			   // add to checked list
	           CheckedFiles.Add(pSF);
//			   TRACE("Adding %s to CheckedFiles\n", (const char*)pSF->csName);
			   // if flag is not set, add to ToDo
			   if (!CheckedOnly)
  			      {			   
				  CServerFile* pToDo = new CServerFile(csFile);
			      FilesToDo.Add(pToDo);
//				  TRACE("Adding %s to FilesToDo\n", (const char*)pToDo->csName);
			      }
			   NumAdded++;
			   Added = TRUE;
			   }
			}
		 memset(&FileInfo, 0, sizeof WIN32_FIND_DATA);
		 // not added!
		 if (!Added) delete pSF;
		 } while (FindNextFile(hFile, &FileInfo));	  
      // close the handle
      FindClose(hFile);   	  
	  }
   // sort the added files
   if (NumAdded) SortToDoFiles();
   return NumAdded;
   }

int CRPResource::GetPollFiles(CStringArray& csa)
   {
   CServerFile* csf;
   int i = 0;
   
   // poll for files
   InsertFiles();	   

   while (i < FilesToDo.GetSize())
      {
	  // get file
      csf = (CServerFile*)FilesToDo.GetAt(i++);
	  // add name to string array
	  csa.Add(csf->csName);	  
//	  TRACE("Adding %s to csa\n", (const char*)csf->csName);
	  // delete it
	  delete csf;	  
	  }     
   FilesToDo.RemoveAll();
   return i;
   }		  

// CServerFile stuff
//
CServerFile::CServerFile(CString& csFileName)
   {
   // store the name
   csName = csFileName;

   HANDLE hFile;
   WIN32_FIND_DATA FileInfo;
   if ((hFile = FindFirstFile(csFileName, &FileInfo)) != INVALID_HANDLE_VALUE)
	  {      			
	  // close the handle
	  FindClose(hFile);         
	  // file time
	  ((CZeusApp*)AfxGetApp())->GetFileTime(&FileInfo, m_mtime);
      m_size = FileInfo.nFileSizeLow;
	  }				 	
   }
   
BOOL CServerFile::HasChanged()
   {
   HANDLE hFile;
   WIN32_FIND_DATA FileInfo;
   CTime m_newmtime;
   LONG m_newsize;
   
   if ((hFile = FindFirstFile(csName, &FileInfo)) != INVALID_HANDLE_VALUE)
	  {      			
	  // close the handle
	  FindClose(hFile);   
	  // file time
	  ((CZeusApp*)AfxGetApp())->GetFileTime(&FileInfo, m_newmtime);
      m_newsize = FileInfo.nFileSizeLow;
	  }				 	
   if (m_mtime == m_newmtime && m_size == m_newsize)
      return FALSE;
   else
      return TRUE;
   }
   
BOOL CServerFile::HasReadWritePriv()
   {
   if (_access((const char*)csName, 06) == 0)
      return TRUE;
   else
      return FALSE;
   }
   
BOOL CServerFile::FullReadPrivileges(int iFileSize)
   {
   int fh;
   BOOL bRet = FALSE;

   if ((fh = _open(csName, _O_RDONLY, 0)) != -1)
	  {
	  // try to lock all of it
	  if (_locking(fh, _LK_NBLCK, iFileSize) != -1)
		 {		 
		 bRet = TRUE;
		 // unlock it
		 _locking(fh, _LK_UNLCK, iFileSize);		 
		 }
	  _close(fh);
	  }
   return bRet;
   }
   
BOOL CServerFile::operator==(const CServerFile& dm)
   {        
   return IsSame(dm);
   }

BOOL CServerFile::operator!=(const CServerFile& dm)
   {        
   return (!IsSame(dm));
   }   

BOOL CServerFile::IsSame(const CServerFile& dm)
   {    
   if (csName == dm.csName && m_mtime == dm.m_mtime && m_size == dm.m_size)
      return TRUE;
   else
      return FALSE;
   }   

void CRPResource::DeleteFromCheckedFilesArray(CString& csFileName)
   {
   int i;
   CServerFile* pSF;

   // look at all checked files
   for (i = 0; i < CheckedFiles.GetSize(); i++)
      {
	  pSF = (CServerFile*)CheckedFiles.GetAt(i);
	  // if the same, remove it
      if (pSF->csName == csFileName)
		 {
		 delete pSF;
		 CheckedFiles.RemoveAt(i);
		 break;
		 }
      }        
   }

void CRPResource::SortToDoFiles()
   {
   BOOL bNotDone = TRUE;
   int pos;

   if (FilesToDo.GetSize() < 1) return;
 
   // sort until done
   while (bNotDone)
      {
      bNotDone = FALSE;
      for (pos = 0; pos < FilesToDo.GetSize() - 1; pos++)
         bNotDone |= CompareAndSwap(pos);
      }   
   }

BOOL CRPResource::CompareAndSwap(int pos)
   {   
   int posFirst = pos;
   int posNext = pos + 1;
   CServerFile* pFileFirst;
   CServerFile* pFileNext;
   int ret = -1;   

   // get the queue files
   pFileFirst = (CServerFile*)FilesToDo.GetAt(posFirst);
   pFileNext = (CServerFile*)FilesToDo.GetAt(posNext);

   // compare  
   ret = pFileFirst->csName.CompareNoCase(pFileNext->csName);       

   if (ret > 0)
      {
      // swap them
	  FilesToDo.SetAt(posFirst, pFileNext);
	  FilesToDo.SetAt(posNext, pFileFirst);
      return TRUE;
      }
   else
      return FALSE;
   }
