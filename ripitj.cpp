////////////////////////////////////////////////////////////////////////////
// RIPITJ.CPP
//                                                                        
// the encapsulation class for the JAWS RIP Process
// Copyright (C) 1996-2000 Graphx, Inc.
//
// mods:
//
// 7-16-96  Build 40 - moved FlipImage from pDevice->pDevInfo to JobInfo
// 5-27-97  Build 50 - put SendInitInfo after SendRipInfo for color correction
//                        (and other) reasons
// 6-01-97  Build 50 - wasn't processing Ripproc,exe processing errors - we now process 
//                        data from the IPC_RIPCOMPLETE call.
// 9-03-97  Build 51 - save first error so that the cancel that we force does not 
//                     overwrite it resulting in "User cancelled" errors instead of
//                     proper errors
// 10-27-97 Build 53 - added byte total reading style for print on the fly stuff
// 9-10-98  Version 2.0.5 - added new device rip loading code (disabled for now)
// 10-06-99 Version 3.0.1 - LoadFromRip now defaults to 2 in setup
// 12-01-99 Version 3.0.2 - more canceling fixes
// 12-13-99 Version 3.0.2 - yet more canceling fixes
//  3-03-00 Version 3.0.3 - Stop print if an error is returned from SendRipInfo call
// 09-28-01 Version 5.0.0 - Added High_Defintion Preview support

#include "stdafx.h"
#include <stdio.h>
#include "zeus.h"
#include "ripstruct.h"
#include "ripitj.h"
#include "inidefs.h"
#include "profile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif	

#define MAXCOMPRESS_FACTOR          2
#define SOME_SLOP					32

IMPLEMENT_DYNAMIC(CRipItJ, CObject)

CRipItJ::CRipItJ()
   {   
   }

BOOL CRipItJ::Init(CDevice* pCD, int ot)
   {
   OutputType = ot;

   // init the device locally in this thread so if the
   // device is used in another thread we don't get hammered   
   if (*pCD != Device) Device = *pCD;   
   pDevice = &Device;   

   return TRUE;
   }

CRipItJ::~CRipItJ()
   {
   }

void CRipItJ::SetRipStruct(PRIPSTRUCT pR)
   {
   // set ripstruct
   pRipStruct = pR;

   // special for preview   
   if (OutputType == RIPSTYLE_PREVIEW || OutputType == RIPSTYLE_HD_PREVIEW)
      {   	  	     
      PreviewDevice.LoadDevice(PREVIEW_DEVICE_IDX);
	  pDevice = &PreviewDevice;
	  }  				
   }

int CRipItJ::Process()
   {   
   BOOL BreakOut = FALSE;
   BOOL AbortFlag = FALSE;   
   int Msg, Data1, Data2;   
   BYTE *pScanLine;   
   int oret;   
   int First_oret;
   int retcode = 0;
   PIPCDEVINFO pip;

   DevProcessFlag = DEVICE_NORMAL_PROCESS;   

   // alloc a scanline slot         
   if (ProcessCanContinue())
      pCProcess->AllocScanLineSlot(pRipStruct->PixWidth * 
         RIPSTRUCT_MAXPIXWIDTH * MAXCOMPRESS_FACTOR + SOME_SLOP);	   

   // alloc a rip info slot   
   if (ProcessCanContinue())
      pCProcess->AllocRipInfoSlot(sizeof RIPSTRUCT + sizeof JOBINFO);   

   // alloc a rip msg slot   
   if (ProcessCanContinue())
      pCProcess->AllocRipMsgSlot(PSMSG_MAXLEN);   

   // get the scan line pointer   
   pScanLine = pCProcess->GetScanLinePtr();   

   // don't flip this!
   pJobInfo->FlipImage = FALSE;
   
   // type of rip
   pJobInfo->RipType = RIP_TYPE_PSCRIPT;   

   // load from rip if multiple processors
   SYSTEM_INFO SystemInfo;
   ::GetSystemInfo(&SystemInfo);	   

   int iTest;
   CProfile Profile;
   iTest = Profile.GetInt(IDS_NEWRIPSECTION, "LoadFromRip", 0);
   switch(iTest)
	  {
	  case 0:
		   // default - load from rip depending on processors
           if (SystemInfo.dwNumberOfProcessors > 1)
		   {
	          pRipStruct->LoadDevFromRip = TRUE;
		   }
           else
		   {
	          pRipStruct->LoadDevFromRip = FALSE;
		   }
		   break;
	  case 1:
		   // option 1 - always
           pRipStruct->LoadDevFromRip = TRUE;
		   break;
	  case 2:
		   // option 2 - never
           pRipStruct->LoadDevFromRip = FALSE;
		   break;							  
	  }

#ifdef _DEBUG
   // disable this for debug
   pRipStruct->LoadDevFromRip = FALSE;         
#endif

   // open the device (proc may modify RipStruct)
   if (!pDevice->OpenDevice(pRipStruct, pJobInfo, &DevProcessFlag))
      {
      // close the device 
      pDevice->CloseDevice();

      // free info slot
	  pCProcess->FreeRipInfoSlot();

      // free msg slot
	  pCProcess->FreeRipMsgSlot();

      // free scan line data slot
	  pCProcess->FreeScanLineSlot();	
	  
      return SYN_ERRMSG_DEVINIT;
	  }								   

   // copy our rip info      
   if (pCProcess->GetRipInfoPtr())
	  memcpy(pCProcess->GetRipInfoPtr(), pRipStruct, sizeof RIPSTRUCT);
   if (pCProcess->GetRipInfoPtr())
	  memcpy(pCProcess->GetRipInfoPtr() + sizeof RIPSTRUCT, pJobInfo, sizeof JOBINFO);   
	  
   // send it   
   if (ProcessCanContinue())
   {
      retcode = pCProcess->SendRipInfo();
	  if ( retcode != 0 )
	  {
	  // close the device 
      pDevice->CloseDevice();

      // free info slot
	  pCProcess->FreeRipInfoSlot();

      // free msg slot
	  pCProcess->FreeRipMsgSlot();

      // free scan line data slot
	  pCProcess->FreeScanLineSlot();	
	  
      return retcode;
	  }
   }

   // send init info   
   if (ProcessCanContinue())
	  {
      if (!pRipStruct->LoadDevFromRip)
         SendInitInfo();      
	  }

   // special devinfo stuff...
   if (ProcessCanContinue())
	  {
      IPCDEVINFO IpcDevExtra;

	  // Set the Error Msg to Empty
	  memset(IpcDevExtra.pErrorMsg, NULL, RIPSTRUCT_MAX_ERRSTRLEN );

      // get devinfo size
      int iDevInfoSize = pDevice->GetDevInfoSize();
      // add space for IPCDEVINFO and alloc
      pCProcess->AllocDevExtraSlot(iDevInfoSize + sizeof IPCDEVINFO);   
      // copy the device number
      IpcDevExtra.DevIdx = pDevice->DevIdx;
      // copy the device name
      strcpy(IpcDevExtra.pDevName, pDevice->Name);
      // copy the dll name
      strcpy(IpcDevExtra.pDevDLLName, pDevice->DLLName);
      // all systems normal
      IpcDevExtra.DevProcessFlag = DEVICE_NORMAL_PROCESS;
      // point to the ripmsg in the host memory space   
	  if (pip = (PIPCDEVINFO)pCProcess->GetDevInfoPtr())
		 {												         
         IpcDevExtra.pHost_RipMsg = &(pip->RipMsg);
		 // devinfo size
         IpcDevExtra.iDevInfoLen = iDevInfoSize;   
         // get the info
         pDevice->GetDevInfo();
         // copy the structure into shared memory
         memcpy(pCProcess->GetDevInfoPtr(), &IpcDevExtra, sizeof IPCDEVINFO);   
         // copy devinfo at the end of the structure
         memcpy(pCProcess->GetDevInfoPtr() + sizeof IPCDEVINFO, 
	        pDevice->pDevInfo, iDevInfoSize);   
		 }
	  }	

   // check for early cancel
   if (!ProcessCanContinue())
   {
	  goto CLEANUP;
   }

   // rip file
   pCProcess->RipFile();   

   // default
   oret = SYN_ERRMSG_ALLOK;
   First_oret = SYN_ERRMSG_ALLOK;
   
   // call the rip and process messages
   do
      {	  
	  // Note: we must wait for rip to break out, we can't
	  // otherwise our messaging would get screwed up
	  if (pCProcess->GetChildMessage(Msg, Data1, Data2))
	     {		 		 
		 // process
		 switch(Msg)	
		    {			
		    case IPC_SCANLINEREADY:		        							     
	             if (!AbortFlag && ProcessCanContinue())
					oret = pDevice->OutputBytes(pScanLine, Data1, Data2);
				 break;
		    case IPC_PERCENTFILECOMPLETE:			     
		         RipMsg.MsgType = RIPMSG_READING;
                 RipMsg.DoTime = TRUE;             
                 RipMsg.PercentComplete = Data1;				 
				 if (ProcessCanContinue()) DoMessage();
				 break;
		    case IPC_TOTALFILECOMPLETE:			     
		         RipMsg.MsgType = RIPMSG_READING_TOTAL;
                 RipMsg.DoTime = TRUE;             
                 RipMsg.TotalComplete = Data1;
   				 if (ProcessCanContinue()) DoMessage();
				 break;
		    case IPC_RIPCOMPLETE:				     
			     oret = Data1;
		         BreakOut = TRUE;
		         break;
			case IPC_RENDERTITLE:			     
   	             RipMsg.MsgType = RIPMSG_RENDERTITLE;
                 RipMsg.DoTime = TRUE;                              
				 if (ProcessCanContinue()) DoMessage();
				 break;
			case IPC_RENDERTICK:			     
   	             RipMsg.MsgType = RIPMSG_SPINONLY;
                 RipMsg.DoTime = TRUE;                              
				 if (ProcessCanContinue()) DoMessage();
				 break;
			case IPC_SENDPSMESSAGE:			     
				 // type is in Data2, len in Data1
				 // check Data1 len!				 
				 if (!AbortFlag && ProcessCanContinue())
				    DoPSMessage(pCProcess->GetRipMsgPtr(), max(min(Data1, PSMSG_MAXLEN), 0));
				 break;				 				 
		    }
		 // nack it		 
		 pCProcess->ClearChildMessage(0);		 

		 // if error, request cancel and set abort flag
		 if (oret != SYN_ERRMSG_ALLOK) 
		    {
			// save first error
			if (First_oret == SYN_ERRMSG_ALLOK) 
			   First_oret = oret;
			// cancel process			
			pCProcess->CancelProcess();   					
			AbortFlag = TRUE;
			}										  
		 }
	  //if (!ProcessCanContinue()) BreakOut = TRUE;
	  } while (!BreakOut);

   // if cancel or abort, abort device
   if (!ProcessCanContinue() || AbortFlag)
	  {
	  if (pRipStruct->LoadDevFromRip)
		 pCProcess->SendDevAbort();
	  else
		 pDevice->Abort();
	  }

CLEANUP:

   // close the device 
   pDevice->CloseDevice();
   
   // free info slot
   pCProcess->FreeRipInfoSlot();

   // free msg slot
   pCProcess->FreeRipMsgSlot();

   // free scan line data slot
   pCProcess->FreeScanLineSlot();	

   // get and store any errors before we free this
   if (pip = (PIPCDEVINFO)pCProcess->GetDevInfoPtr())
	  {
	  if (strlen(pip->pErrorMsg) > 0) DevErrorStr = pip->pErrorMsg;
	  }
   
   // free devinfo slot
   pCProcess->FreeDevInfoSlot();

   if (DevProcessFlag == DEVICE_CANCELALL_PROCESS)
      return SYN_ERRMSG_CANCELALL;
   else if (DevProcessFlag == DEVICE_CANCEL_PROCESS)
      return SYN_ERRMSG_CANCELIMAGE;
   else
	  {
	  // if first error has been loaded, use it
	  if (First_oret != SYN_ERRMSG_ALLOK) 
		 oret = First_oret;
      return oret;
	  }
   }

BOOL CRipItJ::SuspendProcess()
   {   
   DevProcessFlag = DEVICE_PAUSE_PROCESS;
   pCProcess->SuspendProcess();
   return TRUE; // so caller doesn't try to suspend thread
   }

BOOL CRipItJ::ResumeProcess()
   {
   DevProcessFlag = DEVICE_NORMAL_PROCESS;
   pCProcess->ResumeProcess();
   return TRUE; // so caller doesn't try to suspend thread
   }

int CRipItJ::SendInitInfo()
   {
   int len;
   BYTE* pRipData;

   // get pointer to initdata
   pRipData = (BYTE*)(((PRIPIPCDATA)pCProcess->GetRipInitPtr())->InitData);

   len = pDevice->CreatePSInit(pRipData);
   
   // make sure of the length
   ASSERT (len < MAX_INITDATA);

   return pCProcess->SendInitData(len);
   }

BOOL CRipItJ::GetDevErrorFromRip(CString& csErrStr)
   {
   csErrStr = DevErrorStr;

   return TRUE;
   }

