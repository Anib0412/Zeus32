/////////////////////////////////////////////////////////////////////////////
// RIPTHRD.H
//                                                                        
// threaded parent class of bitmap and ps rips for RasterPlus95
// Copyright (C) 1996-98 Graphx, Inc.
//
// Mods:
// 9-10-98  Version 2.0.5 - added new device rip loading code (disabled for now)
//

#ifndef __RIPALL_H__
#define __RIPALL_H__

#include "ripstruct.h"
#include "rip.h"
#include "device.h"
#include "ripmsg.h"
#include "RipProcess.h"

/////////////////////////////////////////////////////////////////////////////
// CRipThread thread

class CRipThread : public CObject
{
public:
	CRipThread();
	~CRipThread();

// Attributes
public:      
   BOOL GetRipStruct(PRIPSTRUCT pR, float& RealWPixelsPerInch, 
		float& RealHPixelsPerInch);
   virtual void SetRipStruct(PRIPSTRUCT) = 0;      
   virtual BOOL Init(CDevice* pCD, int ot) = 0;
   virtual BOOL GetDevError(CString& csErrStr);
   virtual BOOL GetDevErrorFromRip(CString& csErrStr) = 0;
   void Kill();   
   virtual int Process() = 0;
   virtual BOOL SuspendProcess();
   virtual BOOL ResumeProcess();
   CRipProcess* pCProcess;   

public:
   void DoMessage();
   void DoPSMessage(BYTE* pMsg, int len);
   void CancelProcess(BOOL bAll = TRUE);
   BOOL ProcessCanContinue();
   int DevProcessFlag;   

public:
   PJOBINFO pJobInfo;          

protected:   
   CDevice Device;
   CDevice* pDevice;
   CDevice PreviewDevice;
   int OutputType;      
   RIPMSGSTRUCT RipMsg;   
   PRIPSTRUCT pRipStruct;      
   CString DevErrorStr;

protected:
   short SetPreviewRipStruct(PRIPSTRUCT pRipStruct);   
   short SetHDPreviewRipStruct(PRIPSTRUCT pRipStruct);   
};

/////////////////////////////////////////////////////////////////////////////
#endif
