/////////////////////////////////////////////////////////////////////////////
// Server.cpp
//                                                                        
// server class for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#include "stdafx.h"
#include "zeus.h"
#include "server.h"
#include "profile.h"
#include <string.h>
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CServer, CObject, 1)
                           
CServer::CServer()
   { 
   // get default settings
   CProfile Profile;

   // defaults
   m_transform.LoadString(IDS_DEFSERVER_XFORM_ENTRY);
   // get the polldir from the installation default in Server section
   m_polldir = Profile.GetStr(IDS_SERVER, IDS_POLLDIR);
   m_enable_polling = FALSE;
   m_image_only_new = TRUE;
   m_delay_edit = 0;
   m_deletefiles = TRUE;
   m_status = STATUS_SERVER_DISABLED;
   m_bSelect_By_Size = FALSE;
   m_hotfolder_dpi = 300;
   m_hotfolder_variance = 0.25f;
   }

CServer::~CServer()
   { 
   // empty CObArrays
   EmptyCheckedFiles();
   EmptyFilesToDo();
   }

void CServer::EmptyCheckedFiles()
   {
   int i = 0;

   // remove contents 
   while (i < CheckedFiles.GetSize())
      {
      delete CheckedFiles.GetAt(i++);
      }     
   CheckedFiles.RemoveAll();
   }

void CServer::EmptyFilesToDo()
   {
   int i = 0;

   // remove contents 
   while (i < FilesToDo.GetSize())
      {
      delete FilesToDo.GetAt(i++);
      }     
   FilesToDo.RemoveAll();
   }

void CServer::Serialize(CArchive& ar)
   {    
   if (ar.IsStoring())
      {
	  // force disabled!	  
	  //m_status = STATUS_SERVER_DISABLED;
	  // if we have a non-disabled queue, prompt for 
	  // retaining status
#if 0
	  if (m_status == STATUS_SERVER_ENABLED)
	     {
		 if (AfxMessageBox(IDS_DISABLE_ENABLED_QUEUE, MB_YESNO) == IDYES)
		    m_status = STATUS_SERVER_DISABLED;
		 }
	  else if (m_status == STATUS_SERVER_ACTIVE)
	     {
		 if (AfxMessageBox(IDS_DISABLE_ACTIVE_QUEUE, MB_YESNO) == IDYES)
		    m_status = STATUS_SERVER_DISABLED;
		 }
#endif
      ar << m_transform << m_polldir << m_enable_polling << 
         m_image_only_new << m_delay_edit << m_deletefiles
		 << m_status;

	  ar << m_bSelect_By_Size << m_hotfolder_dpi << m_hotfolder_variance;   

	  Transform.Serialize(ar);	  
      }      
   else                 
      {                                         
	  int nVersion = ar.GetObjectSchema();

      ar >> m_transform >> m_polldir >> m_enable_polling >> 
         m_image_only_new >> m_delay_edit >> m_deletefiles
		 >> m_status; 

	  if ( nVersion >= 1 )
	  {
  		ar << m_bSelect_By_Size << m_hotfolder_dpi << m_hotfolder_variance;   
	  }

	  Transform.Serialize(ar);	  
      }   
   }   

const CServer& CServer::operator=(const CServer& ctSrc) // assignment	  
   {        
   if (this != &ctSrc)
      {        
      // these assignments will delete themselves
      m_transform = ctSrc.m_transform;
      m_polldir = ctSrc.m_polldir;
      m_enable_polling = ctSrc.m_enable_polling;
      m_image_only_new = ctSrc.m_image_only_new;      
      m_delay_edit = ctSrc.m_delay_edit;
	  m_deletefiles = ctSrc.m_deletefiles;
	  m_status = ctSrc.m_status;
	  Transform = ctSrc.Transform;	  
	  m_bSelect_By_Size = ctSrc.m_bSelect_By_Size;      
      m_hotfolder_dpi = ctSrc.m_hotfolder_dpi;
	  m_hotfolder_variance = ctSrc.m_hotfolder_variance;
      }
   return *this;
   }   
      
BOOL CServer::operator==(const CServer& dm)
   {        
   return IsSame(dm);
   }

BOOL CServer::operator!=(const CServer& dm)
   {        
   return (!IsSame(dm));
   }
   
BOOL CServer::IsSame(const CServer& dm)
   {        
   if ((m_transform == dm.m_transform) &&      
       (m_polldir == dm.m_polldir) &&  
       (m_enable_polling == dm.m_enable_polling) &&
       (m_image_only_new == dm.m_image_only_new) &&       
       (m_delay_edit == dm.m_delay_edit) &&
	   (m_deletefiles == dm.m_deletefiles) &&
	   (m_status == dm.m_status) && 
	   (Transform == dm.Transform) && 
	   (m_bSelect_By_Size == dm.m_bSelect_By_Size) &&
	   ( m_hotfolder_dpi == dm.m_hotfolder_dpi) &&
	   ( m_hotfolder_variance == dm.m_hotfolder_variance) )	   
      return TRUE;
   else
      return FALSE;    
   }   

int CServer::InsertFiles(BOOL CheckedOnly)
   {
   int i;   
   HANDLE hFile;
   WIN32_FIND_DATA FileInfo;   
   CString csSearch, csDir, csFile;
   CServerFile* pSF;
   BOOL IsRepeat;
   int NumAdded = 0;
   int TotalChecked;
   BOOL Added;

   csDir = m_polldir;

   // make sure the slash is there
   VERIFYPATH(csDir);
   
   // make search string
   csSearch = csDir;
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
		 csFile = csDir;
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
	     if (!IsRepeat && pSF->HasReadWritePriv()) 
		    {
			// wait delay time and check again
			Sleep(m_delay_edit * 1000);
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
   return NumAdded;
   }

int CServer::GetPollFiles(CStringArray& csa)
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

void CServer::InitPolling()
   {
   // empty CObArrays
   EmptyCheckedFiles();
   EmptyFilesToDo();

   // if image only new is set, store the files
   // currently in the poll dir in the checked array
   if (m_image_only_new)
      InsertFiles(TRUE);
   }
      
// 
// CServerFile stuff
//
CServerFile::CServerFile(CString& csFileName)
   {
   CFileStatus fStatus;

   // store the name
   csName = csFileName;
   CFile::GetStatus(csFileName, fStatus);
   m_mtime = fStatus.m_mtime;
   m_size = fStatus.m_size;
   }
   
BOOL CServerFile::HasChanged()
   {
   CFileStatus fStatus;

   // get the status again
   CFile::GetStatus(csName, fStatus);
   if (m_mtime == fStatus.m_mtime && m_size == fStatus.m_size)
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

