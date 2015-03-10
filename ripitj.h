/////////////////////////////////////////////////////////////////////////////
// RIPITJ.H
//                                                                        
// the encapsulation class for the JAWS RIP Process
// Copyright (C) 1996-98 Graphx, Inc.
//
// Mods:
// 9-10-98  Version 2.0.5 - added new device rip loading code (disabled for now)

#ifndef __RIPITJ_H__
#define __RIPITJ_H__

#include "ripthrd.h"

class CRipItJ : public CRipThread
   {
DECLARE_DYNAMIC(CRipItJ)
public:
   CRipItJ();
   ~CRipItJ();

public:   
   BOOL Init(CDevice* pCD, int ot);
   void SetRipStruct(PRIPSTRUCT pR);            
   int Process();
   BOOL SuspendProcess();
   BOOL ResumeProcess();
   int SendInitInfo();
   BOOL GetDevErrorFromRip(CString& csErrStr);

protected:   
   BOOL CancelFlag;   
   };

#endif
