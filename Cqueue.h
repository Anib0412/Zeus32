/////////////////////////////////////////////////////////////////////////////
// CQUEUE.H
//                                                                        
// define and manipulate files and Queues for RasterPlus95
// Copyright (C) 1996-99 Graphx, Inc.
//

#ifndef __CQUEUE_H__
#define __CQUEUE_H__

#include "device.h"
#include "transfrm.h"
#include "DeviceList.h"
#include "servdefs.h"
#include "colortransform.h"

// extra info for queue
#define CQUEUE_EXTRA_WINSLIDE   0x0001
#define CQUEUE_EXTRA_WINSPLIT   0x0002
#define CQUEUE_EXTRA_INDEXFILE  0x0004
#define CQUEUE_EXTRA_SERVERFILE 0x0020
#define CQUEUE_EXTRA_POLLEDFILE 0x0040
#define CQUEUE_EXTRA_PGPRCFILE  0x0080

class CQueueFile; // forward reference

class CQueueFile : public CObject
{                           
DECLARE_SERIAL(CQueueFile)   
public:                                  
      CQueueFile();       	  
	  BOOL Init(LPCSTR lpName, int& iTotalPages, int iPage = 0);
	  BOOL InitBListFile();	  
      BOOL InitServerFile(PSERVERXFER4 pServerXfer, int iPass, 
		 CDevice* pPassedDevice);

	  ~CQueueFile();        	  	  
	  void GetFileTypeName(CString& cs);
	  void GetFileTypeName(LPSTR lpstr);
	  void GetSizeString(CString& csSize);
	  void GetDateString(CString& csDate);
	  void GetTimeString(CString& csTime);
	  void Serialize(CArchive& ar, WORD verCheck );
	  const CQueueFile& operator=(const CQueueFile& cqfSrc); // assignment	           	  
      BOOL operator==(const CQueueFile& cq);              // compare      
      BOOL operator!=(const CQueueFile& cq);              // compare      
	  BOOL IsSame(const CQueueFile& cq);
	  CRect GetSetOptimalBoundingBox(int Width, int Height, int* pPCDResolution);

	  WORD FileType;
	  WORD Selected;	  	  
	  CTransform* pTransform;
	  WORD Prefs;
	  CDevice* pCD;
	  CColorTransform* pCC;
	  CRect BoundingBox;	  
	  CString LongName;
	  CString PS_JPGFileLongName;
	  CString ShortName;
	  CString Description;
	  CTime ctFileTime;
      DWORD iFileSize;
	  DWORD RealFileSize;
	  CString csRealFileSize;
	  WORD QPos;
	  CString BoundingBoxStr;
	  CString PDFRotate;
	  CString Status;
	  CString PreviewStatus;	  
	  CTimeSpan ctsElapsedTime;
	  WORD EPSF;      
	  WORD StatusFlag;	  
	  WORD CATHeaderNeeded;   
	  DWORD StartofValidPS;
	  // batch header options
	  CString BatchHeaderNotes;	  
	  WORD BatchHeaderOptions;	  
	  WORD BatchHeaderCount;
	  WORD ViewPerspective;
	  WORD PageCount;
	  WORD ExtraInfo;
	  CObArray CZeusServerTokenArray;
	  BOOL WasTouched;	  
	  BOOL DragDelete;
	  int AltDevNum;
	  WORD IsComplete;
	  CString MutexName;	  
	  // new stuff for rp3
	  int iPrintToHold;
	  CString csOwner;
	  CString csSubmitted;	  
	  WORD InQueue;
	  CObArray cdwaPageTimes;
	  int iJobID;     // unique job identifier
	  int iOldJobID;  // in case
	  // page range stuff
	  CString m_prange_edit;
	  BOOL m_prange;
	  BOOL bDoSubPage;
	  int iSubPage;	  

      virtual void Serialize(CArchive& ar);	
	  
	  CString csDebug;

public:
	  BOOL CheckForPostScript(CStdioFile& csfCheckFile);
	  BOOL CheckForPDF(CStdioFile& csfCheckFile);
	  void ConvertDescFromHex(CString& csIn);
	  int GetHexVal(char c);

protected:
      BOOL MacOrientOptimize;
      BOOL G95OrientOptimize;
      BOOL CorelOrientOptimize;
      BOOL CreatedByNT;
	  BOOL CreatedByNT2000;
      int OrientOptimize;
      CRect OriginalBoundingBox;
      
protected:      
      void GetFileType();
      void GetPostScriptInfo(CStdioFile& csfCheckFile);	  
	  void GetPDFInfo(CStdioFile& csfCheckFile);
	  void GetCATInfoOnly(CStdioFile& csfCheckFile);
	  int GetPSLine(CStdioFile& csfCheckFile, char *buf, short buflen);	  	  
      BOOL GetServerFileInfo(PSERVERXFER4 pServerXfer, int iPass, 
								   CDevice* pPassedDevice);
	  BOOL ServerTokenMatch(char *Token, int TokenIdx);	  
	  BOOL ServerTokenMatch(CString& csToken, int TokenIdx);
	  void ParseToken(CString& Token, CString& Value, CDeviceList& cdl);
	  void SetConnect(char* Port);
	  void SetPages(CStdioFile& csfCheckFile);
	  void DoTargaHack(CStdioFile& csfCheckFile);
	  void ExtractBoundingBox(const char* pin, BOOL bRotate = FALSE);
	  void MakeCoolFileSizeStr();
};

#define USE_PREFS_GLOBAL       0
#define USE_LOCAL_DEV_PREFS    0x01
#define USE_LOCAL_TRN_PREFS    0x02
#define USE_LOCAL_COL_PREFS    0x04

// an array of tokens extracted from a server PS file
class CZeusServerToken : public CObject
   {
DECLARE_SERIAL(CZeusServerToken)   
public:
   CString Token;
   CString Value;
   void Serialize(CArchive& ar);	  
   };	   

// file status 
#define FILE_STATUS_OK           0x0
#define FILE_STATUS_NOTOK		 0x1
#define FILE_STATUS_OK_DELETED	 0x2
#define FILE_STATUS_HOLD         0x3
//#define FILE_STATUS_INRIP		 0x4
#define FILE_STATUS_CHECK        0xF 
#define FILE_STATUS_INRIP		 0x10
#define FILE_STATUS_FINALGANG	 0x20
#endif     
