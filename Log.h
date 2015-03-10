/////////////////////////////////////////////////////////////////////////////
// log.h
//                                                                        
// log class for RasterPlus
// Copyright (C) 1999 Graphx, Inc.
//

#if !defined(AFX_LOG_H__ADD923A4_0EB3_11D3_8E40_00C04F7975DC__INCLUDED_)
#define AFX_LOG_H__ADD923A4_0EB3_11D3_8E40_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "zeus.h"
#include "cqueue.h"

// log types
#define LOGENTRY_TYPE_JOB    0
#define LOGENTRY_TYPE_TEXT   1  

class CLogEntry : public CObject
   {
DECLARE_SERIAL(CLogEntry)   
public:
   CLogEntry();
   ~CLogEntry();
   virtual void Serialize(CArchive& ar, WORD verCheck = 0 );
   virtual void Serialize(CArchive& ar );

   int iType;
   CString csText;
   CString csName;
   CString csType;
   CString csDescription;
   CString csScaling;
   CString csDevice;
   CString csResolution;
   CString csMedia;
   CString csCopies;
   CString csPages;   
   CString csResult;   
   CString csOwner;
   CString csSubmitted;
   CString csElapsedTime;
   int iStatusFlag;
   };			   

class CLog : public CObject 
   {
DECLARE_SERIAL(CLog)   
public:
   CLog();
   ~CLog();  
   CString csDevice;
   BOOL AddEntry(CQueueFile* pCQueueFile,
					CDevice* pLocalDevice, 
					CTransform* pLocalTransform, 
					CColorTransform* pLocalColor);
   BOOL AddText(CString& csText);
   void DeleteAll();
   CObArray caLogArray;
   void Serialize(CArchive& ar, WORD verCheck );
   void WriteLog(CString& csFileName);
   int GetNumEntries();
   BOOL IsValidEntry(CLogEntry* pLe);
protected:
   int iMaxLogEntries;
   };

#endif // !defined(AFX_LOG_H__ADD923A4_0EB3_11D3_8E40_00C04F7975DC__INCLUDED_)