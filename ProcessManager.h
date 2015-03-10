/////////////////////////////////////////////////////////////////////////////
// ProcessManager.h
//                                                                        
// manage child processes for RasterPlus
// Copyright (C) 1996-99 Graphx, Inc.
//

#ifndef __PROCESSMANAGER_H__
#define __PROCESSMANAGER_H__

#include <afxext.h>
#include "IPCData.h"

#define NODATAHANDLE   -1

class CProcessManager : public CObject
{
// construction/destruction
public:
    CProcessManager();
    ~CProcessManager();
    BOOL OpenProcess(CString& csBN, CString& csAppName, BOOL HideWindow = TRUE);	
	void CloseProcess();
	int AllocDataSlot(int index, int Size, BOOL Inform = TRUE);
	void FreeDataSlot(int index, BOOL Inform = TRUE);
	int SendIPCMessage(int Msg, int Data1 = 0, int Data2 = 0, DWORD Timeout = INFINITE);
	int GetIPCMessage(int& Msg, int& Data1, int& Data2);	
	BOOL GetChildMessage(int& Msg, int& Data1, int& Data2);
	void ClearChildMessage(int ret);

protected:
    HWND hWndProc;
    HANDLE hFileMap[MAXDATAHANDLES];			
	LPVOID lpData[MAXDATAHANDLES];
	CString csBaseName;
	int iDataIndex;	
	HANDLE hParentACK;
    HANDLE hParentNACK;
    HANDLE hChildACK;
    HANDLE hChildNACK;
	HANDLE hProcess;
	DWORD IPCReturn;
	int GetNextAvailDataSlot();
	CRITICAL_SECTION cSection;	
};

#define WAITFORPROCSTART     60 * 1000     // wait one minute for child to start

#endif