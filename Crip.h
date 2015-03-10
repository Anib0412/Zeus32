/////////////////////////////////////////////////////////////////////////////
// CRIP.H
//                                                                        
// RIP class for RasterPlus
// Copyright (C) 1996-2001 Graphx, Inc.
//
// Mods:
// 9-10-98  Version 2.0.5 - added new device rip loading code (disabled for now)
// 9-10-98  Version 2.0.5 - added new device rip loading code (disabled for now)
// 11-06-01 Version 5.0.0 - reorganize temp path stuff

#ifndef __CRIP_H__
#define __CRIP_H__

#include "ftypes.h"
#include "device.h"
#include "transfrm.h"
#include "cqueue.h"
#include "rip.h"   
#include "ripstruct.h"
#include "ripthrd.h"
#include "RipProcess.h"

#define PIVALUE  3.141592654

class CRip : public CObject
   {
public:         
   CRip();
   ~CRip();      
   void Open();
   void Close();
   int Init(CDevice*, CTransform*, CColorTransform* pCC, CQueueFile*, int, 
	  CWnd*, CWnd*, BOOL ForceOffPackage=FALSE, BOOL bDumpGangPrints=FALSE);
#ifdef LEADFILE_PS
   int LeadOnlyInit(CDevice* pDevice, CTransform* pct, CColorTransform* pCC,
				   CQueueFile* pCQ, int OutputType, CWnd* cwReport, CWnd* cwPreview, 
											BOOL ForceOffPackage, BOOL bDumpGangPrints);
#endif
   void RipParamsOverride(int, int, float, float, HWND, CTransform*, BOOL bIndexPrint = FALSE);
   void SetSizing();
   int MakeBatchHeaderFile();
   int StartRip();               
   int EndRip();               
   BOOL GetDeviceError(CString&);   
   BOOL GetDevErrorFromRip(CString& ErrStr);
   void CancelProcess(BOOL bAll);
   void PauseProcess();
   void ResumeProcess();
   void MapErrorReturn(UINT val, CString& cs);
   int StartTransformIndex;
   BOOL IsInit;
   BOOL RipLoaded;   
   CRipThread* pRip;
   BOOL JawsTempPathCreated;   
   CString csScratchPath;
   CString csTempJawsScrathPath;
   CString csBatchHdrFilename;

protected:                                                    
   void LaunchRip();
   void GetCTMArray();
   void GetMatrixProduct(CTM_ARRAY*, CTM_ARRAY*, CTM_ARRAY*);  
   void SwapBBoxCoords(CRect&); 
   BOOL GetBBoxCTM(CTM_ARRAY*);       
   BOOL ThreadStart;   
   CRipProcess* pCProcess;
   int iIPCSlot;      
   float RealHPixelsPerInch, RealWPixelsPerInch;
   UINT uiRipSessionID;
   int ParsePageRangeSelection(CWordArray& cw); 
   
private:
	CString csRPDebugStr;
	char cDebugStr[255];

public:      
   RIPSTRUCT RipStruct;
   CTM_ARRAY RipCTM;
   CRect BoundingBox;
   int FileType;      
   CTransform Transform;
   WORD EPSF;
   WORD ExtraInfo;   
   CQueueFile* pCQueueFile;
   JOBINFO JobInfo; 
   CWinThread* pRipThread;   
     
protected:
   void XFORMtoCTM(CTM_ARRAY *);
   void StrToPSStr(CString& cs);
   double devAspectRatio;       
   double PicaWidth, TrueHeight, TrueWidth, PicaHeight; 
   DWORD StartTime;
   int LastFileType;
   void OptimizeBitmapOutput(CRect& BoundingBox, CDevice* pDevice);
   };

#endif
   
         
