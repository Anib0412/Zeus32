/////////////////////////////////////////////////////////////////////////////
// RIPTHRD.CPP
//                                                                        
// threaded parent class of bitmap and ps rips for RasterPlus95
// Copyright (C) 1996-97 Graphx, Inc.
//
// mods: 
//
// 6-11-97  Build 50 - added stuff for non-square resolutions
// 9-09-97  Build 51 - improved accuracy of calculations by making sure we do a + .5 
//             before casting to an int
//  3-9-98  Version 2.0.2 - fixed None transforms for bitmaps
// 6-11-98  Version 2.0.4 - using PostMessage for rip messages could result
//                          in clobbered strings if at end of rip on a fast machine - 
//                          i.e.: the rip closes before the message can get processed
//                          we now revert back to SendMessage
// 09-28-01 Version 5.0.0 - Added High_Defintion Preview support
// 10-24-01 Version 5.0.0 - Fix Pix calculation for HD Preview
//

#include "stdafx.h"
#include "zeus.h"
#include "ripthrd.h"
#include "profile.h"
#include "inidefs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const UINT NEAR wm_RipSendMessage = RegisterWindowMessage(RIP_SENDMESSAGE);
const UINT NEAR wm_RipSendPSMessage = RegisterWindowMessage(RIP_SENDPSMESSAGE);

/////////////////////////////////////////////////////////////////////////////
// CRipThread

CRipThread::CRipThread()
   {
   }

CRipThread::~CRipThread()
   {
   }

void CRipThread::DoMessage()
   {          
   // send message to JobInfo window
   if (::IsWindow(pJobInfo->hFormWin))
      SendMessage(pJobInfo->hFormWin, wm_RipSendMessage, 0, (LPARAM)&RipMsg);   
   //PostMessage(pJobInfo->hFormWin, wm_RipSendMessage, 0, (LPARAM)&RipMsg);   
   }                           

void CRipThread::DoPSMessage(BYTE* pMsg, int len)
   {          
   // send message to JobInfo window
   if (::IsWindow(pJobInfo->hFormWin))
      SendMessage(pJobInfo->hFormWin, wm_RipSendPSMessage, (WPARAM)len, (LPARAM)pMsg);   
   //PostMessage(pJobInfo->hFormWin, wm_RipSendPSMessage, (WPARAM)len, (LPARAM)pMsg);   
   }                           

BOOL CRipThread::GetRipStruct(PRIPSTRUCT pR, float& RealWPixelsPerInch, 
							  float& RealHPixelsPerInch)
   {   
   // write the current device info
   pDevice->SetDevInfo(FALSE);

   switch (OutputType)
      {
      case RIPSTYLE_PREVIEW:
      	   // first, fill the RipStruct with stuff from the device
	       pDevice->FillRipStruct(pR);
		   // need these in case of None preview
		   RealWPixelsPerInch = pR->WPixelsPerInch;
		   RealHPixelsPerInch = pR->HPixelsPerInch;

	       // now reset the ripstruct for previewing
	       SetPreviewRipStruct(pR);
		   break;
	  case RIPSTYLE_OUTPUT:
	       // fill the RipStruct with stuff from the device
	       pDevice->FillRipStruct(pR);	  
		   // need these in case of None preview
		   RealWPixelsPerInch = pR->WPixelsPerInch;
		   RealHPixelsPerInch = pR->HPixelsPerInch;
		   break;
	  case RIPSTYLE_HD_PREVIEW:
      	   // first, fill the RipStruct with stuff from the device
	       pDevice->FillRipStruct(pR);
		   // need these in case of None preview
		   RealWPixelsPerInch = pR->WPixelsPerInch;
		   RealHPixelsPerInch = pR->HPixelsPerInch;

		   pR->MaxScaleHeight	= pR->PixHeight;
		   pR->MaxScaleWidth	= pR->PixWidth;

	       // now reset the ripstruct for previewing
	       SetHDPreviewRipStruct(pR);
		   break;
	  }	  
   // set the background color   
   pJobInfo->BackgroundColor = pDevice->pDevInfo->BackgroundColor;
   if (pDevice->pDevInfo->BackgroundColor == BACKGROUND_WHITE)
      pR->framefill = 0xff; // white
   else
      pR->framefill = 0x0;  // black
   return TRUE;
   }
   	
short CRipThread::SetPreviewRipStruct(PRIPSTRUCT pR)
   {
   int PreviewSize, DevWidth, DevHeight;
   int PreviewHeight, PreviewWidth;
   CProfile Profile;      
   float WPixelsPerInch, HPixelsPerInch;
   double devAspectRatio;

   // store the dev vars
   DevWidth = pR->PixWidth;
   DevHeight = pR->PixHeight;
   WPixelsPerInch = pR->WPixelsPerInch;
   HPixelsPerInch = pR->HPixelsPerInch;

   devAspectRatio =
      (double)pR->PixHeight / (double)pR->PixWidth *
      pR->WPixelsPerInch / pR->HPixelsPerInch;  

   // defaults
   pR->RipType = RIPSTRUCT_RIPTYPE_FILEPAGE;
   pR->OutputStyle = RIPSTRUCT_OUTPUTSTYLE_ILEAVE;
   pR->FramePasses = 1;
   pR->PrepScanData = FALSE;   
   
   // get the preview size
   PreviewSize = Profile.GetInt(IDS_PREVIEW, IDS_PREVIEW_SIZE, IDDM_PREVIEW_SIZE);   
   
   //if (DevWidth > DevHeight)
   if (devAspectRatio < 1.0)
      {
   	  // set width
      switch(PreviewSize)
	     {
	     case 0:
	          PreviewWidth = (int)((float)GetSystemMetrics(SM_CXSCREEN) / 4.F + .5F);
	          break;
	     case 1:
	          PreviewWidth = (int)((float)GetSystemMetrics(SM_CXSCREEN) / 2.F + .5F);
	          break;
	     case 2:
	          PreviewWidth = (int)((float)GetSystemMetrics(SM_CXSCREEN) * 3.F / 4.F + .5F);
	         break;
	     }    	  
      // set ripstruct pix width
      pR->PixWidth = PreviewWidth;        
      pR->PixHeight = PreviewHeight = (int)
          ((float)PreviewWidth * ((float)DevHeight / HPixelsPerInch) /  
		  ((float)DevWidth / WPixelsPerInch) + .5F);
      }
   else
      {
   	  // set height
      switch(PreviewSize)
	     {
	     case 0:
	          PreviewHeight = (int)((float)GetSystemMetrics(SM_CYSCREEN) / 4.F + .5F);
	          break;
	     case 1:
	          PreviewHeight = (int)((float)GetSystemMetrics(SM_CYSCREEN) / 2.F + .5F);
	          break;
	     case 2:
	          PreviewHeight = (int)((float)GetSystemMetrics(SM_CYSCREEN) * 3.F / 4.F + .5F);
	         break;
	     }    	  

      // set ripstruct pix width
	  pR->PixHeight = PreviewHeight;
      pR->PixWidth = PreviewWidth = (int)      
          ((float)PreviewHeight * ((float)DevWidth / WPixelsPerInch) /  
		  ((float)DevHeight / HPixelsPerInch) + .5F);
	  }

   pR->WPixelsPerInch = (float)PreviewWidth / ((float)DevWidth / WPixelsPerInch);
   pR->HPixelsPerInch = (float)PreviewHeight / ((float)DevHeight / HPixelsPerInch);

   pR->PackageDetails.iPreviewSize = PreviewSize; 
   
   return TRUE;
   }

short CRipThread::SetHDPreviewRipStruct(PRIPSTRUCT pR)
{
	int PreviewSize, DevWidth, DevHeight;
	int PreviewHeight, PreviewWidth;

	CProfile Profile;      
	
	float WPixelsPerInch, HPixelsPerInch;
	
	double devAspectRatio;

	// store the dev vars
	DevWidth = pR->PixWidth;
	DevHeight = pR->PixHeight;
	WPixelsPerInch = pR->WPixelsPerInch;
	HPixelsPerInch = pR->HPixelsPerInch;

	devAspectRatio = (double)pR->PixHeight / (double)pR->PixWidth * pR->WPixelsPerInch / pR->HPixelsPerInch;  

	// defaults
	pR->RipType			= RIPSTRUCT_RIPTYPE_FILEPAGE;
	pR->OutputStyle		= RIPSTRUCT_OUTPUTSTYLE_ILEAVE;
	pR->FramePasses		= 1;
	pR->PrepScanData	= FALSE;   

	// get the preview size
	PreviewSize = Profile.GetInt(IDS_PREVIEW, IDS_PREVIEW_SIZE, IDDM_PREVIEW_SIZE);   

	//if (DevWidth > DevHeight)
	if (devAspectRatio < 1.0)
	{
		PreviewWidth = (int)((float)GetSystemMetrics(SM_CXSCREEN) / 2.F + .5F);

		// set ripstruct pix width
		pR->PixWidth	= ((float)PreviewWidth / 100) * WPixelsPerInch;
		PreviewHeight	= (int)((float)PreviewWidth * ((float)DevHeight / HPixelsPerInch) /  
														((float)DevWidth / WPixelsPerInch) + .5F);
		pR->PixHeight	= ((float)PreviewHeight / 100) * WPixelsPerInch;
	}
	else
	{
		PreviewHeight = (int)((float)GetSystemMetrics(SM_CYSCREEN) / 2.F + .5F);

		// set ripstruct pix width
		pR->PixHeight	= ((float)PreviewHeight / 100) * HPixelsPerInch;
		PreviewWidth	= (int)((float)PreviewHeight * ((float)DevWidth / WPixelsPerInch) /  
													((float)DevHeight / HPixelsPerInch) + .5F);
		pR->PixWidth	= ((float)PreviewWidth / 100) * WPixelsPerInch;
	}

	pR->PackageDetails.iPreviewSize = PreviewSize; 

	return TRUE;
}

BOOL CRipThread::GetDevError(CString& csErrStr)
   {   
   BOOL ret;

   ret = pDevice->GetDevError(csErrStr.GetBuffer(RIPSTRUCT_MAX_ERRSTRLEN));
   csErrStr.ReleaseBuffer(); 

   return ret;
   }   

void CRipThread::Kill()
   {
//   TRACE("Deleteing thread\n");
   delete this;
   }			 

BOOL CRipThread::SuspendProcess()
   {
   // can't suspend by default
   return FALSE;
   }

BOOL CRipThread::ResumeProcess()
   {
   // can't resume by default
   return FALSE;
   }

void CRipThread::CancelProcess(BOOL bAll)
   {
   CString cs;

   cs.LoadString(IDS_CANCELLING);
   strcpy(RipMsg.Info, (const char*)cs);
   RipMsg.MsgType = RIPMSG_INFO2;
   RipMsg.DoTime = TRUE;   
   DoMessage();              
   if (bAll)
      DevProcessFlag = DEVICE_CANCELALL_PROCESS;
   else
      DevProcessFlag = DEVICE_CANCEL_PROCESS;   
   pCProcess->CancelProcess();
   }   

BOOL CRipThread::ProcessCanContinue()
   {
   if (DevProcessFlag == DEVICE_CANCEL_PROCESS ||
       DevProcessFlag == DEVICE_CANCELALL_PROCESS)
      return FALSE;
   else
      return TRUE;
   }
