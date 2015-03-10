/////////////////////////////////////////////////////////////////////////////
// RPResource.h
//                                                                        
// resource settings (including hot folder polling) for RasterPlus
// Copyright (C) 1999 Graphx, Inc.
//

#if !defined(AFX_RPRESOURCE_H__080EB965_38D4_11D3_8E67_00C04F7975DC__INCLUDED_)
#define AFX_RPRESOURCE_H__080EB965_38D4_11D3_8E67_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DISKFILES_DELETE_AFTER_IMAGING     0
#define DISKFILES_KEEP_MAX                 1
#define LISTJOBS_UNTIL_DELETED             0
#define LISTJOBS_KEEP_MAX                  1
#define HOTFOLDER_DELETE_AFTER_IMAGING     0
#define HOTFOLDER_KEEP_MAX                 1
#define HOTFOLDER_NEVER_DELETE             2
#define LISTJOBS_KEEP_MAX                  1
#define DISKFILES_LISTJOBS_DEFCNT          20

#define HOTFOLDER_TYPE_DISABLED            0
#define HOTFOLDER_TYPE_STD				   1
#define HOTFOLDER_TYPE_XCONNECT			   2

class CServerFile : public CObject
   {
public:
   CServerFile(CString& csFileName);
   ~CServerFile(){}  
   BOOL HasChanged();
   BOOL HasReadWritePriv();
   BOOL FullReadPrivileges(int iFileSize);
   BOOL operator==(const CServerFile& dm); // compare      
   BOOL operator!=(const CServerFile& dm); // compare         
   CString csName;   
   LONG m_size;
   CTime m_mtime;
protected:
   BOOL IsSame(const CServerFile& dm);
   };   

class CRPResource : public CObject  
   {
public:
   CRPResource();
   virtual ~CRPResource();
public:
   void Serialize(CArchive& ar, WORD verCheck );
   void InitPolling();
   void EmptyCheckedFiles();
   void EmptyFilesToDo();
   int InsertFiles(BOOL CheckedOnly = FALSE);
   int GetPollFiles(CStringArray& csa);
   void DeleteFromCheckedFilesArray(CString& csFileName);

   CObArray CheckedFiles;
   CObArray FilesToDo;

   CString csSpoolDir;
   CString csHotFolderDir;
   CString csScratchPath;   
   int iDiskFilesResourceSelector;
   int iListJobsResourceSelector;
   int iHotFolderResourceSelector;   
   int iListJobsMax;  
   //BOOL bEnableHotFolder;
   int iHotFolderType;
   int iHotFolderInPane;
   BOOL bImageOnlyNew;
   int iHotFolderDelay;
   BOOL m_bSelectBySize;
   int iHotFolderDPI;
   float iHotFolderVariance;
protected:
   void SortToDoFiles();
   BOOL CompareAndSwap(int pos);

   int iDelayEdit;
   };

#endif // !defined(AFX_RPRESOURCE_H__080EB965_38D4_11D3_8E67_00C04F7975DC__INCLUDED_)
