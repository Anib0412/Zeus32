/////////////////////////////////////////////////////////////////////////////
// CRipProcess.cpp
//                                                                        
// rip child process for RasterPlus
// Copyright (C) 1996-99 Graphx, Inc.
//
// Mods:
// 9-10-98  Version 2.0.5 - added new device rip loading code (disabled for now)
// 12-13-99 Version 3.0.2 - additional checks for bad process pointers

#include "RipProcess.h"
#include <string.h>

CRipProcess::CRipProcess()
   {
   iRipInfoSlot = NODATAHANDLE;
   iRipInitSlot = NODATAHANDLE;
   iScanLineSlot = NODATAHANDLE;
   iRipMsgSlot = NODATAHANDLE;
   iDevInfoSlot = NODATAHANDLE;
   }

CRipProcess::~CRipProcess()
   {
   FreeRipInitSlot();
   FreeRipInfoSlot();
   FreeScanLineSlot();
   FreeRipMsgSlot();
   FreeDevInfoSlot();
   }

int CRipProcess::AllocRipInitSlot()
   {
   // get a slot
   iRipInitSlot = GetNextAvailDataSlot();
   // alloc the data
   AllocDataSlot(iRipInitSlot, sizeof RIPIPCDATA, TRUE);
   // tell the child
   return SendIPCMessage(IPC_SETRIPINITPORT, iRipInitSlot);
   }

int CRipProcess::InitRIP(CString& csDLLName, CString& csInitPaths)
   {
   // alloc the init slot
   AllocRipInitSlot();   

   // copy the dll name
   strcpy(((PRIPIPCDATA)lpData[iRipInitSlot])->DLLName, (const char*)csDLLName);

   // copy the init paths
   strcpy(((PRIPIPCDATA)lpData[iRipInitSlot])->Paths, (const char*)csInitPaths);
   
   return SendIPCMessage(IPC_INITRIP);
   }

void CRipProcess::SendDevAbort()
   {      
   // tell 'em
   SendIPCMessage(IPC_SENDABORTDEVICE);
   }

BOOL CRipProcess::SendInitData(int len)
   {      
   // tell 'em
   SendIPCMessage(IPC_SENDINITDATA, len);

   return TRUE;
   }

void CRipProcess::CloseRIP()
   {
   SendIPCMessage(IPC_CLOSERIP);
   FreeRipInitSlot();
   }

void CRipProcess::FreeRipInitSlot()
   {
   if (iRipInitSlot != NODATAHANDLE)
      FreeDataSlot(iRipInitSlot, TRUE);
   }

int CRipProcess::AllocRipInfoSlot(int Size)
   {
   // get a slot
   iRipInfoSlot = GetNextAvailDataSlot();
   // alloc the data
   AllocDataSlot(iRipInfoSlot, Size, TRUE);
   // tell the child
   return SendIPCMessage(IPC_SETRIPINFOPORT, iRipInfoSlot);
   }

void CRipProcess::FreeRipInfoSlot()
   {
   if (iRipInfoSlot != NODATAHANDLE)
      FreeDataSlot(iRipInfoSlot, TRUE);
   }

BYTE* CRipProcess::GetRipInfoPtr()
   {
   // resolve the pointer
   if (iRipInfoSlot == NODATAHANDLE)
	  return NULL;
   else
	  return (BYTE*)(lpData[iRipInfoSlot]);
   }

BYTE* CRipProcess::GetRipInitPtr()
   {
   // resolve the pointer
   if (iRipInitSlot == NODATAHANDLE)
	  return NULL;
   else
      return (BYTE*)(lpData[iRipInitSlot]);
   }

int CRipProcess::SendRipInfo()
   {   
   return SendIPCMessage(IPC_SENDRIPINFO);
   }

int CRipProcess::OpenDevice()
   {   
   return SendIPCMessage(IPC_DEVICE_OPEN);
   }

int CRipProcess::AllocScanLineSlot(int Size)
   {
   // get a slot
   iScanLineSlot = GetNextAvailDataSlot();
   // alloc the data
   AllocDataSlot(iScanLineSlot, Size, TRUE);
   // tell the child
   return SendIPCMessage(IPC_SETRIPSCANPORT, iScanLineSlot);
   }

void CRipProcess::FreeScanLineSlot()
   {
   if (iScanLineSlot != NODATAHANDLE)
      FreeDataSlot(iScanLineSlot, TRUE);
   }

int CRipProcess::AllocRipMsgSlot(int Size)
   {
   // get a slot
   iRipMsgSlot = GetNextAvailDataSlot();
   // alloc the data
   AllocDataSlot(iRipMsgSlot, Size, TRUE);
   // tell the child
   return SendIPCMessage(IPC_SETRIPMSGPORT, iRipMsgSlot);
   }

int CRipProcess::AllocDevExtraSlot(int Size)
   {
   // get a slot
   iDevInfoSlot = GetNextAvailDataSlot();
   // alloc the data
   AllocDataSlot(iDevInfoSlot, Size, TRUE);
   // tell the child
   return SendIPCMessage(IPC_SETDEVEXTRASLOT, iDevInfoSlot);
   }

void CRipProcess::FreeDevInfoSlot()
   {
   if (iDevInfoSlot != NODATAHANDLE)
      FreeDataSlot(iDevInfoSlot, TRUE);
   }						  

void CRipProcess::FreeRipMsgSlot()
   {
   if (iRipMsgSlot != NODATAHANDLE)
      FreeDataSlot(iRipMsgSlot, TRUE);
   }						  

BYTE* CRipProcess::GetScanLinePtr()
   {
   // resolve the pointer
   if (iScanLineSlot == NODATAHANDLE)
	  return NULL;
   else   
      return (BYTE*)(lpData[iScanLineSlot]);
   }

BYTE* CRipProcess::GetRipMsgPtr()
   {
   // resolve the pointer
   if (iRipMsgSlot == NODATAHANDLE)
	  return NULL;
   else   
      return (BYTE*)(lpData[iRipMsgSlot]);
   }

BYTE* CRipProcess::GetDevInfoPtr()
   {
   // resolve the pointer
   if (iDevInfoSlot	== NODATAHANDLE)
	  return NULL;
   else
	  return (BYTE*)(lpData[iDevInfoSlot]);
   }

void CRipProcess::RipFile()
   {   
   SendIPCMessage(IPC_RIPFILE);   
   }

void CRipProcess::SuspendProcess()
   {
   SendIPCMessage(IPC_SUSPEND);
   }

void CRipProcess::ResumeProcess()
   {
   SendIPCMessage(IPC_RESUME);
   }
   
void CRipProcess::CancelProcess()
   {   
   SendIPCMessage(IPC_CANCEL);   
   }