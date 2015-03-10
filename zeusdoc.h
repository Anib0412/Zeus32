/////////////////////////////////////////////////////////////////////////////
// ZEUSDOC.H
//                                                                        
// document class for RasterPlus
// Copyright (C) 1996-99 Graphx, Inc.
//

#ifndef __ZEUSDOC_H__
#define __ZEUSDOC_H__   

/////////////////////////////////////////////////////////////////////////////
// CZeusDoc document

#include "cqueue.h"
#include "device.h"
#include "transfrm.h"
//#include "server.h"
#include "log.h"
#include "logwnd.h"
#include "specialtree.h"
#include "rpresource.h"

#define ZEUSDOC_NO_VERSION 0x0000
// Build 38!
//#define ZEUSDOC_VERSION 0xFFFE
// Build 40
//#define ZEUSDOC_VERSION 0xFFFD
// Build 53
//#define ZEUSDOC_VERSION 0xFFFC
// Version 3.0
//#define ZEUSDOC_VERSION 0xFFFB

// Version 3.0.3
#define ZEUSDOC_VERSION_A 0xFFFA
#define ZEUSDOC_VERSION_9 0xFFF9
#define ZEUSDOC_VERSION_8 0xFFF8
#define ZEUSDOC_VERSION_7 0xFFF7
#define ZEUSDOC_VERSION_6 0xFFF6

#define QUEUEFILEVERSION 1

// This CONSTANT is ALWAYS the Current Version
#define ZEUSDOC_VERSION 0xFFF6


#define PDOC ((CZeusDoc*)GetDocument())

class CZeusDoc : public CDocument
{              
   DECLARE_DYNCREATE(CZeusDoc) 
protected: // create from serialization only
	CZeusDoc();	

public:	
	//{{AFX_DATA(CZeusDoc)	
	//}}AFX_DATA         
	
// Attributes
public:         
   CObArray CQ;  
   CObArray CQ_Hold;  
   CObArray CQ_Archive;  
   CTime QueueStartTime;
   CTimeSpan QueueElapsedTime;     
   void DeleteContents();   
   CString csBaseTitle;  
   CString csTitle;     
   CLogWnd LogWnd;
   
   int HoldTreeHeight;
   int PrintTreeHeight;
   int ArchiveTreeHeight;
   BOOL SaveModified();
   DWORD FilterIdx;
   BOOL bQueueCloseOK;
   CString InitialFileDir;
   CRPResource crrrs;
   CLog cLog;
   CRect crLogWinRect;
   BOOL bPSoutState;
   BOOL bQStatusState;
   BOOL bDeleteOnClose;
   CString csDelFile;
   CString csDelDriver;

protected:
   int CheckDOCValid;  
   BOOL WasOnDisk; 
   BOOL bPathSet;
   void PopulateJobIDArrays();
					 
// Operations
public:    
// Implementation
public:
	virtual ~CZeusDoc();
	//BOOL IsServer();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(const char* pszPathName);	
	BOOL CanCloseFrame(CFrameWnd* cf);
	BOOL OurSaveDocument();
   
    CDevice QueueDevice;
    CTransform QueueTransform;	
	//CServer Server;
	CColorTransform QueueColor;
    int iQueueColumnWidth[NUMQUEUEPANES][NUMQUEUECOLUMNS];
	
	WORD GetFileVersion();
public:	
	void OnProperties();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
    BOOL InRip;

protected:
	virtual BOOL OnNewDocument();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	void OnCloseDocument();

    WORD wVerCheck;

// Generated message map functions
protected:	
	//{{AFX_MSG(CZeusDoc)
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();		
	afx_msg void OnFileClose();	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
