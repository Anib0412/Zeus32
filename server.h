/////////////////////////////////////////////////////////////////////////////
// Server.h
//                                                                        
// server class for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __SERVER_H__
#define __SERVER_H__

#include "transfrm.h"

class CServerFile : public CObject
   {
public:
   CServerFile(CString& csFileName);
   ~CServerFile(){}  
   BOOL HasChanged();
   BOOL HasReadWritePriv();
   BOOL operator==(const CServerFile& dm); // compare      
   BOOL operator!=(const CServerFile& dm); // compare         
   CString csName;   
   LONG m_size;
   CTime m_mtime;
protected:
   BOOL IsSame(const CServerFile& dm);
   };   
                        
class CServer : public CObject
   { 
DECLARE_SERIAL(CServer);
public:   
   CServer();
   ~CServer();

   CString m_transform;
   CString m_polldir;
   WORD m_enable_polling;
   WORD m_image_only_new;
   WORD m_delay_edit;
   WORD m_deletefiles;
   WORD m_bSelect_By_Size;
   WORD m_hotfolder_dpi;
   float m_hotfolder_variance;
   CTransform Transform;   
   WORD m_status;
   CObArray CheckedFiles;
   CObArray FilesToDo;
   void EmptyCheckedFiles();
   void EmptyFilesToDo();
   int InsertFiles(BOOL CheckedOnly = FALSE);
   int GetPollFiles(CStringArray& csa);
   void InitPolling();

   virtual void Serialize(CArchive& ar);	
   const CServer& operator=(const CServer& cdmSrc); // assignment
   BOOL operator==(const CServer& dm); // compare      
   BOOL operator!=(const CServer& dm); // compare         
protected:
   BOOL IsSame(const CServer& dm);
   };

#define STATUS_SERVER_DISABLED   0
#define STATUS_SERVER_ENABLED    1
#define STATUS_SERVER_ACTIVE     2   

#endif
   
