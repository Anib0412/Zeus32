/////////////////////////////////////////////////////////////////////////////
// CRipProcess.h
//                                                                        
// rip child process for RasterPlus95
// Copyright (C) 1996-98 Graphx, Inc.
//
// Mods:
// 9-10-98  Version 2.0.5 - added new device rip loading code (disabled for now)
//

#ifndef __CRIPPROCESS_H__
#define __CRIPPROCESS_H__

#include "ProcessManager.h"
#include "IPCRipData.h"

class CRipProcess : public CProcessManager
{
// construction/destruction
public:
    CRipProcess();
    ~CRipProcess();
	int AllocRipInitSlot();	
	void FreeRipInitSlot();	
	int AllocScanLineSlot(int Size);
	void FreeScanLineSlot();	
	int AllocRipInfoSlot(int Size);
	void FreeRipInfoSlot();	
	int AllocRipMsgSlot(int Size);
	void FreeRipMsgSlot();	
	int SendRipInfo();
	BYTE* GetScanLinePtr();
	BYTE* GetRipInfoPtr();
	BYTE* GetRipInitPtr();
	BYTE* GetRipMsgPtr();
	int InitRIP(CString& csDLLName, CString& csInitPaths);
	void CloseRIP();	
	BOOL SendInitData(int len);
	void SendDevAbort();
	void RipFile();
	void SuspendProcess();
	void ResumeProcess();
	void CancelProcess();	
	BOOL* pbIsActive;
    BYTE* GetDevInfoPtr();
	void FreeDevInfoSlot();
	int AllocDevExtraSlot(int Size);	

	int OpenDevice();

protected:
    int iRipInitSlot;
	int iScanLineSlot;
	int iRipInfoSlot;
	int iRipMsgSlot;
	int iDevInfoSlot;
};

#endif    