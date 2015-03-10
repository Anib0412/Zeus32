/////////////////////////////////////////////////////////////////////////////
// ProcessManager.cpp
//                                                                        
// manage child processes for RasterPlus
// Copyright (C) 1996-99 Graphx, Inc.
//
// mods:
//
// 12-02-98 Version 2.0.5 - shorten ripproc path to get around NT bug
// 12-13-99 Version 3.0.2 - process close not being called properly (timeout not being used - fixed)
// 12-13-99 Version 3.0.2 - put a timeout on data slot allocs and frees

#include "ProcessManager.h"
#include <string.h>	   

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// wait 30 seconds for process to close
#define PROCESS_CLOSE_TIMEOUT 30 * 1000

// wait 10 seconds to respond to data slot allocs and frees
#define DATA_SLOT_TIMEOUT     10 * 1000

// this module controls the creation and communication between 
// a main process and a child process.  Here's what we do:
//
// 1. The main app creates a memory-mapped file for interprocess
//    communications
// 2. The main app creates (or loads) the child process and sets
//    the process inheritence flag to true.  the child also receives
//    an instance-specific name on the command line which is used as
//    a Window class name and a file mapping name for data transfer

CProcessManager::CProcessManager()
   {
   lpData[0] = NULL;
   hFileMap[0] = NULL;   
   // start index at IDC_BASE_DATASLOT + 1
   iDataIndex = IDC_BASE_DATASLOT + 1;
   hWndProc = NULL;
   hParentACK = NULL;
   hParentNACK = NULL;
   hChildACK = NULL;
   hChildNACK = NULL;
   hProcess = NULL;
   InitializeCriticalSection(&cSection);
   }

CProcessManager::~CProcessManager()
   {
   // close the process if open
   if (hWndProc) CloseProcess();

   // close events
   if (hParentACK) ::CloseHandle(hParentACK);
   if (hParentNACK) ::CloseHandle(hParentNACK);
   if (hChildACK) ::CloseHandle(hChildACK);
   if (hChildNACK) ::CloseHandle(hChildNACK);
   // free the base data slot 
   FreeDataSlot(IDC_BASE_DATASLOT, FALSE);

   // delete the mutex
   DeleteCriticalSection(&cSection);  
   }

BOOL CProcessManager::OpenProcess(CString& csBN, CString& csAppName, BOOL HideWindow)
   {
   
   BOOL ret = TRUE;
   SECURITY_ATTRIBUTES Security;   
   STARTUPINFO StartupInfo;
   PROCESS_INFORMATION ProcessInfo;            
   char CommandLine[_MAX_PATH];      
   CString csEventName;

   DWORD WFII_Ret;
   DWORD dwLastError = 0;

   memset(&ProcessInfo, 0, sizeof PROCESS_INFORMATION);

   // set inheritence flag for all handles
   memset(&Security, 0, sizeof SECURITY_ATTRIBUTES);
   Security.nLength = sizeof(SECURITY_ATTRIBUTES);
   Security.lpSecurityDescriptor = NULL; 
   Security.bInheritHandle = TRUE;          

   // save base name
   csBaseName = csBN;

   // create events
   csEventName = csBaseName;
   csEventName += IPCEVENT_PACK_APPEND;
   hParentACK = CreateEvent(&Security, FALSE, FALSE, csEventName);       
   if (!hParentACK) return FALSE;     

   csEventName = csBaseName;
   csEventName += IPCEVENT_PNACK_APPEND;
   hParentNACK = CreateEvent(&Security, FALSE, FALSE, csEventName);       
   if (!hParentNACK) return FALSE;     

   csEventName = csBaseName;
   csEventName += IPCEVENT_CACK_APPEND;
   hChildACK = CreateEvent(&Security, FALSE, FALSE, csEventName);       
   if (!hChildACK) return FALSE;     

   csEventName = csBaseName;
   csEventName += IPCEVENT_CNACK_APPEND;
   hChildNACK = CreateEvent(&Security, FALSE, FALSE, csEventName);       
   if (!hChildNACK) return FALSE;     

   // alloc base message slot
   if (AllocDataSlot(IDC_BASE_DATASLOT, sizeof BASEIPCMESSAGE, FALSE) == NODATAHANDLE)
      return FALSE;

   // try to launch the app
   // NULL the StartupInfo struct
   memset(&StartupInfo, 0, sizeof(STARTUPINFO));
   // the size
   StartupInfo.cb = sizeof(STARTUPINFO);
   // hide the window?
   if (HideWindow)
      {
	  StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
      StartupInfo.wShowWindow = SW_HIDE;
	  }

   // shorten path to get around NT bug
   char pShortAppName[_MAX_PATH];
   GetShortPathName(csAppName, pShortAppName, _MAX_PATH);	     
   wsprintf(CommandLine, IPC_CHILDSTARTARGS,  pShortAppName, 
      (const char*)csBaseName);	     

   // launch it											  
   ret = CreateProcess(NULL, CommandLine, &Security, NULL, FALSE, 
	  NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &ProcessInfo);	  
   
   // wait for child to start
   if (ret)
      {
 	  // default
	  ret = FALSE;

	  hProcess = ProcessInfo.hProcess;
	 
	  // wait for it to start
	  WFII_Ret = WaitForInputIdle(hProcess, WAITFORPROCSTART);
	  if ( WFII_Ret != 0 )
	  {
		  // Get Error
		  dwLastError = GetLastError();
		  ret = FALSE;
	  }

	  // look for window
	  if ((hWndProc = ::FindWindow(csBaseName, (LPCTSTR)NULL)) != NULL)   	     
		 ret = TRUE;

	  }
   else 
      hProcess = NULL;
   return ret;
   }

int CProcessManager::GetNextAvailDataSlot()
   {
   int i;
   int iToUse;
   
   iToUse = iDataIndex;

   for (i = IDC_BASE_DATASLOT + 1; i < iDataIndex; i++)
      {
      if (hFileMap[i] == NULL)
         {
	     iToUse = i;
		 break;
		 }
  	  }

   // too many handles!
   if (iToUse >= MAXDATAHANDLES) 
      return NODATAHANDLE;   
   else    
      return iToUse;
   }

int CProcessManager::AllocDataSlot(int index, int Size, BOOL Inform)
   {
   CString csMapName;   
   int ret;
   
   // create file map name based on index and basename
   csMapName.Format(IPCDATAFORMAT, csBaseName, index); 
   
   // Create an in-memory memory-mapped file 
   hFileMap[index] = ::CreateFileMapping((HANDLE) 0xFFFFFFFF, 
     NULL, PAGE_READWRITE, 0, Size, csMapName);

   // if handle exists, try to close it in a loop 
   // for PROCESS_CLOSE_TIMEOUT ms.  If it doesn't free, we return
   // an error which prevents rip usage by the caller
   if ((hFileMap[index] != NULL) && (GetLastError() == ERROR_ALREADY_EXISTS))
      {	 
	  DWORD TickStart = ::GetTickCount(); 
      do
         {
//		 TRACE("Waiting for hFileMap[index] to free\n");
		 // wait 10 ms
		 ::Sleep(10);	
 	     // unmap the file
         ::UnmapViewOfFile(lpData[index]);
         // close the handle
         ::CloseHandle(hFileMap[index]);	 
		 // attempt to create it again
	     hFileMap[index] = ::CreateFileMapping((HANDLE) 0xFFFFFFFF, 
            NULL, PAGE_READWRITE, 0, Size, csMapName);
		 } while ((hFileMap[index] != NULL) && (GetLastError() == ERROR_ALREADY_EXISTS)
		          && ((::GetTickCount() - TickStart) < PROCESS_CLOSE_TIMEOUT));
	  }	    
					 
   if (hFileMap[index] != NULL) 
      {
	  if (GetLastError() == ERROR_ALREADY_EXISTS)
	     {
		 // still a problem, forget it         
         ret = NODATAHANDLE;		  
		 }
	  else 
	     {
		 // File mapping created successfully.
         // Map a view of the file 
         // into the address space.
         lpData[index] = MapViewOfFile(hFileMap[index],
            FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

         if (lpData[index] == NULL) 		    			
		    ret = NODATAHANDLE;
		 else
		    ret = index;
		 }	  
	  }
   else
      ret = NODATAHANDLE;

   if (ret != NODATAHANDLE)
      {
	  // tell proc
	  if (Inform) SendIPCMessage(IPC_MSG_CREATEDATAMAP, index, Size, DATA_SLOT_TIMEOUT);
	  // increment index if needed
	  if (index == iDataIndex)
	     {
	     iDataIndex++;
		 hFileMap[iDataIndex] = NULL;
		 lpData[iDataIndex] = NULL;
		 }
	  }
   return ret;
   }
   
void CProcessManager::FreeDataSlot(int index, BOOL Inform)
   {
   if (index == NODATAHANDLE) return;

   if (hFileMap[index])
      {
	  // unmap the file
      ::UnmapViewOfFile(lpData[index]);
      // close the handle
      ::CloseHandle(hFileMap[index]);
      // null these out so we can re-use them
      lpData[index] = NULL;
      hFileMap[index] = NULL;
      // tell the proc
	  if (Inform) SendIPCMessage(IPC_MSG_DELETEDATAMAP, index, 0, DATA_SLOT_TIMEOUT);
	  }
   }

void CProcessManager::CloseProcess()
   {   
   int i;

   // close all data slots (except base)
   for (i = IDC_BASE_DATASLOT + 1; i < iDataIndex; i++) 
      FreeDataSlot(i);
   
   // close process
   // wait ten seconds for return, 
   SendIPCMessage(IPC_MSG_CLOSEPROCESS, 0, 0, PROCESS_CLOSE_TIMEOUT);   

   hWndProc = NULL;

   // if we timed out, terminate with extreme prejudice
   if (IPCReturn == WAIT_TIMEOUT)
      ::TerminateProcess(hProcess, 0);
   else
      {
	  // make sure that the process has closed
      DWORD dwExitCode;
      DWORD TickStart = ::GetTickCount();
      do
         {
	     ::GetExitCodeProcess(hProcess, &dwExitCode);
	     //Sleep(0);			  
	     } while ((dwExitCode == STILL_ACTIVE) && 
	        ((::GetTickCount() - TickStart) < PROCESS_CLOSE_TIMEOUT));
      // still active? terminate it!
      if (dwExitCode == STILL_ACTIVE) ::TerminateProcess(hProcess, 0);      
      }
   }
   
int CProcessManager::SendIPCMessage(int Msg, int Data1, int Data2, DWORD Timeout)
   {      
   int ret;

   // block others from sending messages until we get our response
   EnterCriticalSection(&cSection);   

   // load message
   pBaseMessage->iParentMsg = Msg;
   // and data
   pBaseMessage->iParentData1 = Data1;
   pBaseMessage->iParentData2 = Data2;
   // reset nack   
   ResetEvent(hParentNACK);
   // set ack event to tell child about message
   SetEvent(hParentACK);
   // wait for nack
   IPCReturn = WaitForSingleObject(hParentNACK, Timeout);
   //
   // check dwRet against WAIT_OBJECT_0, and    WAIT_TIMEOUT	?
   //
   // get return 
   ret = pBaseMessage->iParentRet;

   // free up the IPC messaging
   LeaveCriticalSection(&cSection);

   return ret;
   }

BOOL CProcessManager::GetChildMessage(int& Msg, int& Data1, int& Data2)
   {   
   // check for incoming events from child
   if (WaitForSingleObject(hChildACK, 0) == WAIT_OBJECT_0)
      {	  
	  Msg = pBaseMessage->iChildMsg;
	  Data1 = pBaseMessage->iChildData1;
	  Data2 = pBaseMessage->iChildData2;
	  return TRUE;
	  }
   else
      {
	  Sleep(0);	  
      return FALSE;
	  }
   }

void CProcessManager::ClearChildMessage(int ret)
   {   
   // set return
   pBaseMessage->iChildRet = ret;
   // tell the child we got the message
   SetEvent(hChildNACK);
   }
	 