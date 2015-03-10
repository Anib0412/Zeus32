////////////////////////////////////////////////////////////////////////////
// GISSERVER.CPP
//                                                                        
// Photogize.PrintServer class for RasterPlus
// Copyright (C) 2000-2002 Graphx, Inc.
//
// mods:
//
// 01-23-00 Version 4.0.2 - add code to re-awaken pgprc if it's dead
// 08-20-01 Version 5.0.0 - Add code to check for Unsupported file type
// 09-18-01 Version 5.0.0 - added code to allow user to specify separator logos in photogize
// 01-04-02 Version 5.0.1 - incorrect copies on montaged photos in index print - fixed
// 01-17-02 Version 5.0.2 - added putfile
// 02-20-02 Version 5.0.4 - fail if index logo file is not 24 bit
// 06-21-02 Version 5.0.6 - add support for PostScript files in system
//						    for some reason we were disallowing PSD files - fixed
// 02-26-04 Version 5.2.4 - Custom Sizes larger than the actual media size did not print - Fixed
//

#include "stdafx.h"
#include "zeus.h"
#include "zeusdoc.h"
#include "GISServer.h"
#include "cqueue.h"
#include "device.h"
#include "transfrm.h"
#include "SpecialTree.h"   
#include "jobseparator.h"   
#include "..\barcode\tecbarcode.h"
#include "AFXDB.H"
#include "sockimpl.H"
#include "profile.h"

IMPLEMENT_DYNAMIC(CSocketJob, CObject)

#define QUEUEDEVICE    pDoc->QueueDevice
#define PANEARRAY      ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pPaneArray

#define SWAPINTS(a,b) {int iTempSwap;iTempSwap=a;a=b;b=iTempSwap;}

/*
#ifdef RPDEBUGSTRMFC
#undef RPDEBUGSTRMFC
#define  RPDEBUGSTRMFC(a)
#endif
*/

#define MISFIT_PENALTY    100000

// socket job stuff
//
CSocketJob::CSocketJob()
   {
   bDoJobSeparator = FALSE;
   iPhotoCounter = 0;
   iTotalPages = 0;
   }

CSocketJob::~CSocketJob()
   {
   // clean up the photos when the job is deleted 
   DeleteAllPhotos();
   }			
//
//

CPGPRCServer::CPGPRCServer()
   {
   m_pListenSocket = NULL;   
   SocketPolling = FALSE;
   csLastGISError = "";
   bCheckAwake = FALSE;
   StartSocketPolling();   
   //bCheckAwake = TRUE;
   }

CPGPRCServer::~CPGPRCServer()
   {
   bCheckAwake = FALSE;
   // stop polling
   StopSocketPolling();   
   // destroy all socket jobs
   DestroyAllSocketJobs();
   }

void CPGPRCServer::StartSocketPolling()
   {
   // are we already doing it?
   if (SocketPolling) return;

   // init the socket layer
   AfxSocketInit();

   SocketPolling = StartSocketServer();
   }

void CPGPRCServer::StopSocketPolling()
   {   
   if (SocketPolling)
      {
	  SocketPolling = FALSE;
      if (m_pListenSocket) 
		 {
		 if (m_pListenSocket->m_hSocket != INVALID_SOCKET)
			{
		    if (m_pListenSocket->AsyncSelect() != 0)
			   delete m_pListenSocket;	 
			}
		 }
	  m_pListenSocket = NULL;	  
	  CloseAllSockets();
	  }
   }

void CPGPRCServer::ProcessPendingAccept()
   {
   CClientSocket* pSocket;
   CTimeSpan ctElapsedTime;

   // do a garbage collection on "dead" sockets
   POSITION pos, temp;
   for (pos = m_connectionList.GetHeadPosition(); pos != NULL;)
	  {			
	  temp = pos;
	  CClientSocket* pSock = (CClientSocket*)m_connectionList.GetNext(pos);
	  // delta minutes since last access
	  ctElapsedTime = CTime::GetCurrentTime() - pSock->ctLastAccessed;
	  // if we have exceeded our dead time on a socket, nuke it
	  if (ctElapsedTime.GetTotalMinutes() > PGPRC_SOCKET_DEADTIME)
		 {
		 RPDEBUGSTRMFC("GISServer: ctElapsedTime.GetTotalMinutes() > PGPRC_SOCKET_DEADTIME");
		 m_connectionList.RemoveAt(temp);
		 }
	  }   

   // create a client comm socket
   RPDEBUGSTRMFC("Server: pSocket = new CClientSocket");
   pSocket = new CClientSocket(this);
   
   // and accept the request
   if (m_pListenSocket->Accept(*pSocket))
	  {
	  // the init it
      RPDEBUGSTRMFC("Server: pSocket = Init Socket");
	  pSocket->Init();
      RPDEBUGSTRMFC("Server: pSocket = Init Socket That's done");
	  // add to array
	  m_connectionList.AddTail(pSocket);

      RPDEBUGSTRMFC("Server: pSocket = AddTail");

	  }	
   else
	  {
	  RPDEBUGSTRMFC("Server: delete pSocket");
	  delete pSocket;
	  }

   RPDEBUGSTRMFC("Server: pSocket = ProcessPendingAccept Complete");
   }

void CPGPRCServer::CloseSocket(CClientSocket* pSocket)
   {   
   RPDEBUGSTRMFC("GISServer: CloseSocket");
   pSocket->Close();
   
   POSITION pos, temp;
   for (pos = m_connectionList.GetHeadPosition(); pos != NULL;)
	  {			
	  temp = pos;
	  CClientSocket* pSock = (CClientSocket*)m_connectionList.GetNext(pos);
	  if (pSock == pSocket)
		 {
		 m_connectionList.RemoveAt(temp);
		 break;
		 }
	  }   
   RPDEBUGSTRMFC("Server: delete pSocket");
   delete pSocket;
   }

void CPGPRCServer::CloseAllSockets()
   {   
   POSITION pos, temp;
   CClientSocket* pSocket;

   RPDEBUGSTRMFC("GISServer: CloseAllSockets");

   for (pos = m_connectionList.GetHeadPosition(); pos != NULL;)
	  {	  
	  temp = pos;
	  pSocket = (CClientSocket*)m_connectionList.GetNext(pos);
	  if (pSocket->m_hSocket != INVALID_SOCKET)
		 {
	     if (pSocket->AsyncSelect() != 0)
			{
	        pSocket->Close();
			RPDEBUGSTRMFC("Server: delete pSocket");
	        delete pSocket;
			}
		 }
	  m_connectionList.RemoveAt(temp);
	  }   
   }

void CPGPRCServer::ProcessPendingRead(CClientSocket* pSocket)
   { 
   int dir, Cmd, arg1, arg2, arg3, len;         
   int iNumDevices;   
   PGPRC_RETURNVAL gRetVal;   

   // get the direction
   *(pSocket->m_pArchiveIn) >> dir;   
   // ReportByteCount(FALSE, sizeof dir);
   // get the command		 
   *(pSocket->m_pArchiveIn) >> Cmd;   
   // ReportByteCount(FALSE, sizeof Cmd);
   // and the first arg
   *(pSocket->m_pArchiveIn) >> arg1;
   // ReportByteCount(FALSE, sizeof arg1);
   // and the second arg
   *(pSocket->m_pArchiveIn) >> arg2;
   // ReportByteCount(FALSE, sizeof arg2);
   // and the third arg
   *(pSocket->m_pArchiveIn) >> arg3;
   // ReportByteCount(FALSE, sizeof arg3);
   // and the length
   *(pSocket->m_pArchiveIn) >> len;
   // ReportByteCount(FALSE, sizeof len);

   switch (Cmd)
	  {
	  case GISCMD_GETSVRVERSION:
		   RPDEBUGSTRMFC("GISServer: GISCMD_GETSVRVERSION");
		   // we don't care about the incoming data
		   DumpIncomingData(len, pSocket);
		   *(pSocket->m_pArchiveOut) << (sizeof (int));
		   // ReportByteCount(TRUE, sizeof (int));
		   *(pSocket->m_pArchiveOut) << (int)PGPRC_CURRENT_VERSION;
		   // ReportByteCount(TRUE, sizeof (int));
		   gRetVal = pgprc_returnok;
		   break;
	  case GISCMD_CLOSESOCKET:
		   RPDEBUGSTRMFC("GISServer: GISCMD_CLOSESOCKET");
		   pSocket->ShutDown();
		   CloseSocket(pSocket);
		   break;
      case GISCMD_GETNUMDEVICES:
		   RPDEBUGSTRMFC("GISServer: GISCMD_GETNUMDEVICES");
		   // we don't care about the incoming data
		   DumpIncomingData(len, pSocket);			  
		   // get the devices (which is actually the number of queues)
		   iNumDevices = PANEARRAY.GetSize() / 3;		   
		   *(pSocket->m_pArchiveOut) << (sizeof (int));
		   // ReportByteCount(TRUE, sizeof (int));
		   *(pSocket->m_pArchiveOut) << iNumDevices;
		   // ReportByteCount(TRUE, sizeof (int));
		   gRetVal = pgprc_returnok;
		   break;									
	  case GISCMD_GETDEVINFO:
		   RPDEBUGSTRMFC("GISServer: GISCMD_GETDEVINFO");
		   gRetVal = GetDevInfo(len, arg1, pSocket);
		   break;									
	  case GISCMD_GETCANVASINFO:
		   RPDEBUGSTRMFC("GISServer: GISCMD_GETCANVASINFO");
		   gRetVal = GetCanvasInfo(len, (HANDLE)arg1, arg2, pSocket);
		   break;
	  case GISCMD_OPENJOB:
		   RPDEBUGSTRMFC("GISServer: GISCMD_OPENJOB");
		   gRetVal = OpenJob(len, (HANDLE)arg1, pSocket);
		   break;
	  case GISCMD_GETLASTERRORSTR:
		   RPDEBUGSTRMFC("GISServer: GISCMD_GETLASTERRORSTR");
		   // we don't care about the incoming data
		   DumpIncomingData(len, pSocket);
		   *(pSocket->m_pArchiveOut) << (csLastGISError.GetLength() + 1);
		   // ReportByteCount(TRUE, sizeof (int));
  	       pSocket->m_pArchiveOut->Write(csLastGISError, csLastGISError.GetLength());
		   // ReportByteCount(TRUE, csLastGISError.GetLength() + 1);
		   // make sure it's null terminated
		   ReturnDummyInfo(1, pSocket);
		   gRetVal = pgprc_returnok;
		   break;
	  case GISCMD_ADDJOBSEPARATOR:
		   RPDEBUGSTRMFC("GISServer: GISCMD_ADDJOBSEPARATOR");
		   gRetVal = AddJobSeparator(len, (HANDLE)arg1, pSocket);
		   break;
	  case GISCMD_ADDPHOTO:
		   RPDEBUGSTRMFC("GISServer: GISCMD_ADDPHOTO");
		   gRetVal = AddPhoto(len, (HANDLE)arg1, pSocket, 1);
		   break;
	  case GISCMD_ADDPHOTO2:
		   RPDEBUGSTRMFC("GISServer: GISCMD_ADDPHOTO2");
		   gRetVal = AddPhoto(len, (HANDLE)arg1, pSocket, 2);
		   break;
	  case GISCMD_PUTFILE:
		   RPDEBUGSTRMFC("GISServer: GISCMD_PUTFILE");
		   gRetVal = PutFile(len, (HANDLE)arg1, pSocket);
		   break;
	  case GISCMD_GETFILE:
		   RPDEBUGSTRMFC("GISServer: GISCMD_GETFILE");
		   // not supported right now
		   gRetVal = pgprc_error_unsupported_cmd;
		   //gRetVal = GetFile(len, (HANDLE)arg1, pSocket);
		   break;
	  case GISCMD_PRINTJOB:
		   RPDEBUGSTRMFC("GISServer: GISCMD_PRINTJOB");
		   gRetVal = PrintJob(len, (HANDLE)arg1, pSocket);
		   break;
	  case GISCMD_GETJOBSTATUS:
		   RPDEBUGSTRMFC("GISServer: GISCMD_GETJOBSTATUS");
		   gRetVal = GetJobStatus(len, (HANDLE)arg1, pSocket);
		   break;
	  case GISCMD_GETPHOTOSTATUS:
		   RPDEBUGSTRMFC("GISServer: GISCMD_GETPHOTOSTATUS");
		   gRetVal = GetPhotoStatus(len, pSocket);
		   break;
	  case GISCMD_CLOSEJOB:	  
		   RPDEBUGSTRMFC("GISServer: GISCMD_CLOSEJOB");
		   gRetVal = CloseJob((HANDLE)arg1, pSocket);
		   break;
	  case GISCMD_SERVERSYSMSG:
		   RPDEBUGSTRMFC("GISServer: GISCMD_SERVERSYSMSG");
		   gRetVal = ServerSysMsg(len, arg1, pSocket);
		   break;
	  case GISCMD_GETDEVOPTIONNAME:
		   RPDEBUGSTRMFC("GISServer: GISCMD_GETDEVOPTIONNAME");
		   // we don't care about the incoming data
		   DumpIncomingData(len, pSocket);			  
		   gRetVal = GetDevOptionName(len, (HANDLE)arg1, arg2, pSocket);
		   break;
	  case GISCMD_GETDEVOPTIONSETTINGNAME:
		   RPDEBUGSTRMFC("GISServer: GISCMD_GETDEVOPTIONSETTINGNAME");
		   // we don't care about the incoming data
		   DumpIncomingData(len, pSocket);			  
		   gRetVal = GetDevOptionSettingName(len, (HANDLE)arg1, arg2, arg3, pSocket);
		   break;
	  case GISCMD_GETDEVOPTION:
		   RPDEBUGSTRMFC("GISServer: GISCMD_GETDEVOPTION");
		   // we don't care about the incoming data
		   DumpIncomingData(len, pSocket);			  
		   gRetVal = GetDevOption(len, (HANDLE)arg1, arg2, pSocket);
		   break;
	  case GISCMD_SETDEVOPTION:
		   RPDEBUGSTRMFC("GISServer: GISCMD_SETDEVOPTION");
		   // we don't care about the incoming data
		   DumpIncomingData(len, pSocket);			  
		   gRetVal = SetDevOption((HANDLE)arg1, arg2, arg3, pSocket);
		   break;
      default:				  
		   RPDEBUGSTRMFC("GISServer: Unsupported Command");
		   // get the rest of the bytes
		   if ((dir & GISCMD_SND) == GISCMD_SND)			  
		      DumpIncomingData(len, pSocket);			     			  
		   gRetVal = pgprc_error_unsupported_cmd;
		   break;		   
	  }
   // close socket is a special case
   if (Cmd != GISCMD_CLOSESOCKET)
	  {	  
	  *(pSocket->m_pArchiveOut) << (int)gRetVal;
	  // ReportByteCount(TRUE, sizeof (int));
	  pSocket->m_pArchiveOut->Flush();	  
	  RPDEBUGSTRMFC("Server: ProcessPendingRead, flush");
	  }
   }

BOOL CPGPRCServer::StartSocketServer() 
   {
   int iSocket;   

   if (m_pListenSocket) 
	  {
	  if (m_pListenSocket->m_hSocket != INVALID_SOCKET)
		 {
	     if (m_pListenSocket->AsyncSelect() != 0)
	        delete m_pListenSocket;
		 }
	  }

   for (iSocket = PGPRC_FIRST_SOCKET; iSocket <= PGPRC_LAST_SOCKET; iSocket++)
	  {	  
	  m_pListenSocket = new CListeningSocket(this);
	  if (m_pListenSocket) 
		 {
	     if (m_pListenSocket->Create(iSocket))
			{
		    if (m_pListenSocket->Listen())
			   return TRUE;
			}
	     else
			{
			DWORD dwErr = GetLastError();
			if (m_pListenSocket->m_hSocket != INVALID_SOCKET)
			   {
			   if (m_pListenSocket->AsyncSelect() != 0)
				  delete m_pListenSocket;			
			   }
		    m_pListenSocket = NULL;
			}
		 }
	  }
   return FALSE;
   }

PGPRC_RETURNVAL CPGPRCServer::GetDevInfo(int len, int idx, CClientSocket* pSocket)
   {
   CPaneElement* pRipElement;   
   CZeusDoc* pDoc;
   PGPRCDEVINFO PGPRCDevInfo;
   DEVBACKPRINTDATA DevBackPrintData;
   CString csDeviceName;
   CString csNA;
   PGPRC_RETURNVAL gRetVal = pgprc_error_getting_devinfo;

   // zero it out
   ZeroMemory(&PGPRCDevInfo, sizeof PGPRCDEVINFO);

   // we don't care about the incoming data
   DumpIncomingData(len, pSocket);      

   // range check
   // squirt the data back
   if (idx < 0 || idx >= (PANEARRAY.GetSize() / 3)) 
	  {													  
	  return gRetVal;
	  }   

   // load the n/a string
   csNA.LoadString(IDS_NOT_AVAIL);   
   
   csDeviceName = csNA;	
   
   // get the first pane in the queue
   pRipElement = (CPaneElement*)PANEARRAY.GetAt(idx * 3);
   if (pRipElement)
	  {
	  // get the active document
	  if (pDoc = (CZeusDoc*)(pRipElement->pDocPtr))
		 {	                		 
		 csDeviceName = pDoc->csTitle;
		 // the handle to the device is actually a pointer to the document
  		 PGPRCDevInfo.hDevice = (HANDLE)pDoc;
		 strcpy(PGPRCDevInfo.lpsLongDevName, csDeviceName);
		 strcpy(PGPRCDevInfo.lpsShortDevName, csDeviceName);
		 strcpy(PGPRCDevInfo.lpsDevLocation, csNA);
		 strcpy(PGPRCDevInfo.lpsSvrLocation, csNA);		   
		 PGPRCDevInfo.bIsAvailable = TRUE;
		 PGPRCDevInfo.iNumBackPrintLines = 0; // default
		 ZeroMemory(&DevBackPrintData, sizeof DEVBACKPRINTDATA);
		 // check device for actual backprint support
         if (QUEUEDEVICE.GetSetExtra(DEVGETSET_GETBACKPRINT, NULL,
			(BYTE*)&DevBackPrintData, 0, sizeof DEVBACKPRINTDATA))
			{
			PGPRCDevInfo.iNumBackPrintLines = DevBackPrintData.iNumBackprintLines;
			}
			
		 PGPRCDevInfo.bCanDoCustomCanvas = FALSE;	   		   
		 QUEUEDEVICE.pDevInfo->TotalMedia = QUEUEDEVICE.UpdateMediaList();
		 PGPRCDevInfo.iNumCanvases = QUEUEDEVICE.pDevInfo->TotalMedia;
		 PGPRCDevInfo.iCurrentCanvas = QUEUEDEVICE.pDevInfo->CurrentMediaIdx;
		 PGPRCDevInfo.iDevType = pgprc_devtype_photoprinter;
		 PGPRCDevInfo.bRollMedia = FALSE;		   		  		   
		 PGPRCDevInfo.iMaxLengthRollInches1000 = 0;		 
		 PGPRCDevInfo.iMaxWidthRollInches1000 = 0;		 
		 PGPRCDevInfo.iPrintsPerHour = 0;
		 // squirt the data back
		 *(pSocket->m_pArchiveOut) << (sizeof PGPRCDEVINFO);
		 // ReportByteCount(TRUE, sizeof (int));
		 pSocket->m_pArchiveOut->Write(&PGPRCDevInfo, (sizeof PGPRCDEVINFO));
		 // ReportByteCount(TRUE, sizeof PGPRCDEVINFO);
		 gRetVal = pgprc_returnok;
		 }		 
      }   
   return gRetVal;
   }

PGPRC_RETURNVAL CPGPRCServer::GetCanvasInfo(int len, HANDLE hDevice, int idx, CClientSocket* pSocket)
   {
   CZeusDoc* pDoc;      
   PGPRCCANVAS GISCanvas;
   CString csNA;
   MEDIA Media;
   
   // load the n/a string
   csNA.LoadString(IDS_NOT_AVAIL);
   
   // we don't care about the incoming data
   DumpIncomingData(len, pSocket);      
   
   // resolve the document
   pDoc = (CZeusDoc*)(hDevice);

   // sanity check
   if (!pDoc) 
	  {      
	  return pgprc_error_invalid_device_handle;
	  }
   if (!(pDoc->IsKindOf(RUNTIME_CLASS(CZeusDoc)))) 
	  {      
	  return pgprc_error_invalid_device_handle;
	  }

   // get the media
   QUEUEDEVICE.GetMedia(idx, &Media);

   // get the dims of the canvas
   if (QUEUEDEVICE.pDevInfo->Orientation == DEV_ORIENT_PORTRAIT)
	  {
      GISCanvas.iWidthInches1000 = abs(Media.Rect.right - Media.Rect.left) * 10;
      GISCanvas.iHeightInches1000 = abs(Media.Rect.top - Media.Rect.bottom) * 10;
	  }
   else
	  {
      GISCanvas.iHeightInches1000 = abs(Media.Rect.right - Media.Rect.left) * 10;
      GISCanvas.iWidthInches1000 = abs(Media.Rect.top - Media.Rect.bottom) * 10;
	  }							 
   
   // copy the data to the canvas struct
   strcpy(GISCanvas.lpsCanvasName, Media.Name);
   strcpy(GISCanvas.lpsCanvasDesc, Media.Description);
   GISCanvas.iStyle = PGPRC_CANVAS_NOSTYLE;

   // return the length
   *(pSocket->m_pArchiveOut) << (sizeof PGPRCCANVAS);
   // ReportByteCount(TRUE, sizeof (int));
   pSocket->m_pArchiveOut->Write(&GISCanvas, (sizeof PGPRCCANVAS));
   // ReportByteCount(TRUE, sizeof PGPRCCANVAS);

   return pgprc_returnok;
   }  

void CPGPRCServer::DestroyAllSocketJobs()
   {
   int i = 0;
   CSocketJob* pSocketJob;
   
   // destroy all the jobs
   while (i < coSocketJobArray.GetSize())
      {
	  pSocketJob = (CSocketJob*)coSocketJobArray.GetAt(i++);
	  // and delete the job
      delete pSocketJob;
      }     
   coSocketJobArray.RemoveAll();
   }

void CSocketJob::DeleteAllPhotos()
   { 
   POSITION pos, temp;
   POURPGPRCPHOTOSPECS pPhoto;   

   for (pos = Photos.GetHeadPosition(); pos != NULL;)
	  {	  
	  temp = pos;
	  pPhoto = (POURPGPRCPHOTOSPECS)Photos.GetNext(pos);
#if 0
	  // do we have a temp file?  if so, delete it
	  if (pPhoto->bUseTempFile)
		 ::DeleteFile(pPhoto->lpszTempDiskFile);	  
	  // delete the file if this flag is set
	  if (pPhoto->bDeleteWhenDone)
	     ::DeleteFile(pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile);	  
	  // leave the queuefiles?
	  CQueueFile* pCQueueFile;

	  // get the queuefile pointer
	  pCQueueFile = (CQueueFile*)pPhoto->hCqueueFile;
	  // sanity check 
	  if (pCQueueFile)
		 {
		 if (pCQueueFile->IsKindOf(RUNTIME_CLASS(CQueueFile))) 
			{
			delete pCQueueFile;
			pPhoto->hCqueueFile = NULL;
			pCQueueFile = NULL;
			}
		 }	  
#endif
	  // leave the queuefiles?
	  CQueueFile* pCQueueFile;

	  // if we haven't added the file to the queue, delete the queue file
	  if (!pPhoto->bAddedToQueue)
		 {
		 // get the queuefile pointer
		 pCQueueFile = (CQueueFile*)pPhoto->hCqueueFile;
		 // sanity check 
		 if (pCQueueFile)
			{
			if (pCQueueFile->IsKindOf(RUNTIME_CLASS(CQueueFile))) 
			   {
			   delete pCQueueFile;
			   pPhoto->hCqueueFile = NULL;
			   pCQueueFile = NULL;			   
			   }  
			}	  
		 }

	  // delete the object
	  delete pPhoto;
	  Photos.RemoveAt(temp);
	  }   

   iPhotoCounter = 0;
   }

PGPRC_RETURNVAL CPGPRCServer::OpenJob(int len, HANDLE hDevice, CClientSocket* pSocket)
   {
   CSocketJob* pJob;   
   LPSTR lpStr;

   // create a new job
   if (!(pJob = new CSocketJob))
	  {
	  DumpIncomingData(len, pSocket);
	  return pgprc_error_outofmemory;   
	  }

   // if the name is too big, skeedaddle
   if (len > PGPRC_MAXSTRLEN)
	  {
	  DumpIncomingData(len, pSocket);
	  return pgprc_server_bad_datalen;
	  }

   // capture the name of the job
   lpStr = pJob->csName.GetBuffer(PGPRC_MAXSTRLEN);   
   ZeroMemory(lpStr, PGPRC_MAXSTRLEN);
   pSocket->m_pArchiveIn->Read(lpStr, len);
   // ReportByteCount(FALSE, len);
	  
   pJob->csName.ReleaseBuffer();

   // save the device
   pJob->hDevice = hDevice;
   coSocketJobArray.Add(pJob);

   // we're sending back a pointer to the job object which serves as the handle
   *(pSocket->m_pArchiveOut) << (sizeof (int));
   // ReportByteCount(TRUE, sizeof (int));
   *(pSocket->m_pArchiveOut) << (int)pJob;   
   // ReportByteCount(TRUE, sizeof (int));

   return pgprc_returnok;
   }

PGPRC_RETURNVAL CPGPRCServer::CloseJob(HANDLE hJob, CClientSocket* pSocket)
   {
   int i = 0;
   CSocketJob* pJob;
   CSocketJob* pSocketJob;

   // resolve the job
   pJob = (CSocketJob*)hJob;

   // sanity check
   if (!pJob) return pgprc_error_invalid_job_handle;
   if (!(pJob->IsKindOf(RUNTIME_CLASS(CSocketJob)))) 
	  return pgprc_error_invalid_job_handle;

   // find the job and destroy it
   while (i < coSocketJobArray.GetSize())
      {
	  pSocketJob = (CSocketJob*)coSocketJobArray.GetAt(i);
	  if (pSocketJob == pJob)
		 {
		 coSocketJobArray.RemoveAt(i);		 
         delete pSocketJob;
		 break;
		 }
	  i++;
      }     
   return pgprc_returnok;
   }

PGPRC_RETURNVAL CPGPRCServer::ServerSysMsg(int len, int iType, CClientSocket* pSocket)
   {
   LPSTR lpStr;
   CString csPrompt;
   int iRet;

   // if the name is too big, skeedaddle
   if (len > PGPRC_MAXSTRLEN)
	  {
	  DumpIncomingData(len, pSocket);
	  return pgprc_server_bad_datalen;
	  }

   // capture the text
   lpStr = csPrompt.GetBuffer(PGPRC_MAXSTRLEN);   
   ZeroMemory(lpStr, PGPRC_MAXSTRLEN);
   pSocket->m_pArchiveIn->Read(lpStr, len);
   // ReportByteCount(FALSE, len);
	  
   csPrompt.ReleaseBuffer();

   iRet = AfxMessageBox(csPrompt, iType | MB_SETFOREGROUND);
   
   // we're sending back a pointer to the job object which serves as the handle
   *(pSocket->m_pArchiveOut) << (sizeof (int));
   // ReportByteCount(TRUE, sizeof (int));
   *(pSocket->m_pArchiveOut) << (int)iRet;   
   // ReportByteCount(TRUE, sizeof (int));

   return pgprc_returnok;
   }

void CPGPRCServer::DumpIncomingData(int len, CClientSocket* pSocket)
   {
   int i;
   BYTE bData;         

   // if this happens, we're really hosed
   if (len < 0) return;

   for (i = 0; i < len; i++)			  
	  *(pSocket->m_pArchiveIn) >> bData;		   		   
   // ReportByteCount(FALSE, len);
   }

PGPRC_RETURNVAL CPGPRCServer::AddJobSeparator(int len, HANDLE hJob, CClientSocket* pSocket)
   {
   CSocketJob* pJob;   

   // resolve the job
   pJob = (CSocketJob*)hJob;

   // sanity check
   if (!pJob) 
	  {	  
	  DumpIncomingData(len, pSocket);
	  return pgprc_error_invalid_job_handle;
	  }
   if (!(pJob->IsKindOf(RUNTIME_CLASS(CSocketJob)))) 
	  {	  
	  DumpIncomingData(len, pSocket);
	  return pgprc_error_invalid_job_handle;
	  }
   if (len != (sizeof JOBSEPARATOR))
	  {	  
	  DumpIncomingData(len, pSocket);
	  return pgprc_server_bad_datalen;
	  }
   // copy the separator information
   pSocket->m_pArchiveIn->Read(&pJob->JobSeparator, sizeof JOBSEPARATOR);
   // ReportByteCount(FALSE, sizeof JOBSEPARATOR);

   // copy the separator back with the assigned handle
   pJob->JobSeparator.hPhoto = (HANDLE)&pJob->JobSeparator;
   *(pSocket->m_pArchiveOut) << (sizeof (JOBSEPARATOR));
   // ReportByteCount(TRUE, sizeof (int));
   pSocket->m_pArchiveOut->Write(&pJob->JobSeparator, sizeof JOBSEPARATOR);
   // ReportByteCount(TRUE, sizeof JOBSEPARATOR);

   // set this flag
   pJob->bDoJobSeparator = TRUE;

   // and assign these
   switch(pJob->JobSeparator.iStyle)
	  {
	  case PGPRC_JOBSEPARATOR_STYLE_1:		   
		   PGPRC_CONTACTIMAGE_WIDTH = 100 * 4;
		   PGPRC_CONTACTIMAGE_HEIGHT = 66 * 4;
		   PGPRC_CONTACTIMAGE_SPACINGX = 30 * 4;
		   PGPRC_CONTACTIMAGE_SPACINGY = 45 * 4;
		   PGPRC_CONTACTIMAGE_NUMACCROSS = 5;
		   PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_1 = 2;
		   PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_N = 4;
		   PGPRC_SEPARATOR_BBOX_WIDTH = 700;
		   PGPRC_SEPARATOR_BBOX_HEIGHT = 500;
		   PGPRC_SEPARATOR_BBOX_WIDTHF = 700.F;
		   PGPRC_SEPARATOR_BBOX_HEIGHTF = 500.F;
		   break;
	  default:
	  case PGPRC_JOBSEPARATOR_STYLE_0:  
		   PGPRC_CONTACTIMAGE_WIDTH = 200 * 2;
		   PGPRC_CONTACTIMAGE_HEIGHT = 133 * 2;
		   PGPRC_CONTACTIMAGE_SPACINGX = 5 * 2;
		   PGPRC_CONTACTIMAGE_SPACINGY = 5 * 2;
		   PGPRC_CONTACTIMAGE_NUMACCROSS = 7;
		   PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_1 = 2;
		   PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_N = 6;
		   PGPRC_SEPARATOR_BBOX_WIDTH = 600;
		   PGPRC_SEPARATOR_BBOX_HEIGHT = 400;
		   PGPRC_SEPARATOR_BBOX_WIDTHF = 600.F;
		   PGPRC_SEPARATOR_BBOX_HEIGHTF = 400.F;
		   break;
	  }

   return pgprc_returnok;
   }

#define PHOTOSENDBUFSIZE 32000

PGPRC_RETURNVAL CPGPRCServer::AddPhoto(int len, HANDLE hJob, CClientSocket* pSocket, 
   int iStyle)
   {
   CString csDebug;
   
   int i;
   CSocketJob* pJob;
   POURPGPRCPHOTOSPECS pGisPhotoSpecs;
   int iTotalCopies;
   POURPGPRCPHOTOSPECS pNewGisPhotoSpecs;
   CString csTempPath;
   CFile cf;
   BYTE* pFileBuf;
   int iSizeToWrite;

   // resolve the job
   pJob = (CSocketJob*)hJob;

   // sanity check
   if (!pJob) 
	  {								
	  DumpIncomingData(len, pSocket);
	  ReturnDummyInfo(len, pSocket);
	  return pgprc_error_invalid_job_handle;
	  }
   if (!(pJob->IsKindOf(RUNTIME_CLASS(CSocketJob)))) 
	  {	  
	  DumpIncomingData(len, pSocket);
	  ReturnDummyInfo(len, pSocket);
	  return pgprc_error_invalid_job_handle;
	  }
   // resolve the device to get the temp path
   CZeusDoc* pDoc;
   if (!(pDoc = (CZeusDoc*)(pJob->hDevice)))
	  {	  	  
	  DumpIncomingData(len, pSocket);
	  ReturnDummyInfo(len, pSocket);
	  return pgprc_error_invalid_job_handle;
	  }
   if (iStyle == 1)
	  {
	  if (len != (sizeof PGPRCPHOTOSPECS))
		 {								
		 DumpIncomingData(len, pSocket);
		 ReturnDummyInfo(len, pSocket);
		 return pgprc_server_bad_datalen;
		 }
	  }
   else
	  {
	  if (len != (sizeof PGPRCPHOTOSPECS2))
		 {								
		 DumpIncomingData(len, pSocket);
		 ReturnDummyInfo(len, pSocket);
		 return pgprc_server_bad_datalen;
		 }
	  }	  

   // create a new photo spec
   if (!(pGisPhotoSpecs = new OURPGPRCPHOTOSPECS)) 
	  {	  
	  DumpIncomingData(len, pSocket);
	  ReturnDummyInfo(len, pSocket);
	  return pgprc_error_outofmemory;
	  }

   csTempPath = pDoc->crrrs.csScratchPath;

   ZeroMemory(pGisPhotoSpecs, sizeof OURPGPRCPHOTOSPECS);   

   // do different things based on the style of add_photo
   if (iStyle == 1)
      pSocket->m_pArchiveIn->Read(&pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs, 
	  sizeof PGPRCPHOTOSPECS);
   else
	  pSocket->m_pArchiveIn->Read(&pGisPhotoSpecs->PhotoSpecs, sizeof PGPRCPHOTOSPECS2);   

   // get the actual data if bSendActualData is set
   if (pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.bSendActualData)
	  {
	  if (pFileBuf = new BYTE[PHOTOSENDBUFSIZE])
		 {
		 // make a temp file
		 GetTempFileName(csTempPath, "pgprc", 0, pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.lpszDiskFile);
		 // open it and write the data
		 if (cf.Open(pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.lpszDiskFile,
			CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
			{
			iSizeToWrite = pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.iPhotoSize; 
			do
			   {
			   if (iSizeToWrite > 0)
				  {
			      pSocket->m_pArchiveIn->Read(pFileBuf, min(iSizeToWrite, PHOTOSENDBUFSIZE));
				  // ReportByteCount(FALSE, min(iSizeToWrite, PHOTOSENDBUFSIZE));
				  cf.Write(pFileBuf, min(iSizeToWrite, PHOTOSENDBUFSIZE));
				  }
			   iSizeToWrite -= min(iSizeToWrite, PHOTOSENDBUFSIZE);
			   } while (iSizeToWrite > 0);
			cf.Close();
			}
		 delete pFileBuf;
		 }
	  }

   // set the reported copy count to be the same as the passed copy count
   pGisPhotoSpecs->iReportedCopyCount = pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.iCopies;

   // apply a filter if necessary
   if (ApplyFilter(pJob, pGisPhotoSpecs) != pgprc_returnok)
	  {
	  delete pGisPhotoSpecs;	  	  
	  return pgprc_error_cant_apply_filter;
	  }

   // get file type
   FILEINFO FileInfo;
   memset(&FileInfo, 0, sizeof(FILEINFO));
   FileInfo.uStructSize = sizeof(FILEINFO);
   if (::L_FileInfo(pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.lpszDiskFile,
				  &FileInfo, sizeof(FILEINFO), FILEINFO_TOTALPAGES, NULL) > 0)
   {
	  pGisPhotoSpecs->FileType = FileInfo.Format;
   }

   // increment photo counter
   pJob->iPhotoCounter++;

   pGisPhotoSpecs->iPhotoCount = pJob->iPhotoCounter;   

   pGisPhotoSpecs->bIsSeparator = FALSE;
	
   // put in the thumbnails
   pGisPhotoSpecs->bPutInThumbnails = TRUE;   

   // add add to the array
   pJob->Photos.AddTail(pGisPhotoSpecs);

   // if this is a montage print AND the page size is the same,
   // change to fit sizing
   if (pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.pgprcSizing == pgprc_montage_crop ||
	   pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.pgprcSizing == pgprc_montage_fit)
	  {
	  MEDIA Media;	  
	  CZeusDoc* pDoc;
	  
	  // resolve the document
      if (pDoc = (CZeusDoc*)(pJob->hDevice))
		 {
		 QUEUEDEVICE.GetMedia(pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.iCanvasToUse, &Media);

		 if (min(pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000, 
			pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000) ==
			min(abs(Media.Rect.right - Media.Rect.left) * 10,
			    abs(Media.Rect.top - Media.Rect.bottom) * 10))
			{
            if (pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.pgprcSizing == pgprc_montage_crop)
			   pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.pgprcSizing = pgprc_crop;
			else
			   pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.pgprcSizing = pgprc_fit;
			}			
		 }
	  }

   // copy the photospecs back with the assigned handle
   if (iStyle == 1)
	  {   
      pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.hPhoto = (HANDLE)pGisPhotoSpecs;
      *(pSocket->m_pArchiveOut) << (sizeof (PGPRCPHOTOSPECS));
	  pSocket->m_pArchiveOut->Write(&pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs, sizeof PGPRCPHOTOSPECS);
	  }
   else
	  {   
      pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.hPhoto = (HANDLE)pGisPhotoSpecs;
      *(pSocket->m_pArchiveOut) << (sizeof (PGPRCPHOTOSPECS2));
	  pSocket->m_pArchiveOut->Write(&pGisPhotoSpecs->PhotoSpecs, sizeof PGPRCPHOTOSPECS2);
	  }
	  
   // if we are montaging this photo AND we are doing copies,
   // add the photo to the queue again
   if ((pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.pgprcSizing == pgprc_montage_crop ||
	    pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.pgprcSizing == pgprc_montage_fit) && 
	    pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.iCopies > 1)
	  {
	  // save this
	  iTotalCopies = pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.iCopies;
	  // force this
	  pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.iCopies = 1;
	  for (i = 1; i < iTotalCopies; i++)
		 {
		 if (!(pNewGisPhotoSpecs = new OURPGPRCPHOTOSPECS)) 
			{
			delete pGisPhotoSpecs;			
			return pgprc_error_outofmemory;
			}
		 else
			{
			// copy over
			memcpy(pNewGisPhotoSpecs, pGisPhotoSpecs, sizeof OURPGPRCPHOTOSPECS);			
			// increment photo counter
			pJob->iPhotoCounter++;
			pNewGisPhotoSpecs->iPhotoCount = pJob->iPhotoCounter;   
			// don't put the copies in the thumbnails
			pNewGisPhotoSpecs->bPutInThumbnails = FALSE;
			// add add to the array
			pJob->Photos.AddTail(pNewGisPhotoSpecs);
			}
		 }
	  }

   return pgprc_returnok;
   }

PGPRC_RETURNVAL CPGPRCServer::PutFile(int len, HANDLE hJob, CClientSocket* pSocket)
   {
   CSocketJob* pJob;
   PPGPRCSENDFILEINFO pgprcSendFileInfo;   
   CString csFolder;
   CString csFileName;
   CFile cf;
   BYTE* pFileBuf;
   int iSizeToWrite;
   CProfile Profile;

   // resolve the job
   pJob = (CSocketJob*)hJob;

   // sanity check
   if (!pJob) 
	  {									  
	  DumpIncomingData(len, pSocket);
	  return pgprc_error_invalid_job_handle;
	  }
   if (!(pJob->IsKindOf(RUNTIME_CLASS(CSocketJob)))) 
	  {	  
	  DumpIncomingData(len, pSocket);
	  return pgprc_error_invalid_job_handle;
	  }
   if (len != (sizeof PGPRCSENDFILEINFO))
	  {									  
	  DumpIncomingData(len, pSocket);
	  return pgprc_server_bad_datalen;
	  }

   // create a new file spec
   if (!(pgprcSendFileInfo = new PGPRCSENDFILEINFO)) 
	  {	  
	  DumpIncomingData(len, pSocket);
	  return pgprc_error_outofmemory;
	  }

   // get app path
   csFolder = ((CZeusApp*)AfxGetApp())->GetAppPath();

   // create a Photogize subdirectory
   csFolder += "Photogize\\";

   // make sure it exists
   ::CreateDirectory(csFolder, NULL);

   ZeroMemory(pgprcSendFileInfo, sizeof PGPRCSENDFILEINFO);   
   pSocket->m_pArchiveIn->Read(pgprcSendFileInfo, sizeof PGPRCSENDFILEINFO);

   // now act on the file type
   switch(pgprcSendFileInfo->pgprcFileType)
	  {
	  case pgprc_filetype_leftindex_logo:
		   csFileName = csFolder;
		   csFileName += pgprcSendFileInfo->lpszFileName;
		   // modify this item in the registry
		   Profile.WriteStr(IDS_PHOTOGIZE, "LeftLogo", csFileName);
		   break;
	  case pgprc_filetype_rightindex_logo:
		   csFileName = csFolder;
		   csFileName += pgprcSendFileInfo->lpszFileName;
		   // modify this item in the registry
		   Profile.WriteStr(IDS_PHOTOGIZE, "RightLogo", csFileName);
		   break;
	  case pgprc_filetype_other:
		   csFileName = csFolder;
		   csFileName += pgprcSendFileInfo->lpszFileName;
		   break;
	  }
   
   // get the actual data
   if (pFileBuf = new BYTE[PHOTOSENDBUFSIZE])
	  {
	  // open it and write the data
	  if (cf.Open(csFileName,
		 CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
		 {
		 iSizeToWrite = pgprcSendFileInfo->iFileSize;
		 do
			{
			if (iSizeToWrite > 0)
			   {
			   pSocket->m_pArchiveIn->Read(pFileBuf, min(iSizeToWrite, PHOTOSENDBUFSIZE));
			   cf.Write(pFileBuf, min(iSizeToWrite, PHOTOSENDBUFSIZE));
			   }
			iSizeToWrite -= min(iSizeToWrite, PHOTOSENDBUFSIZE);
			} while (iSizeToWrite > 0);
		 cf.Close();
		 }
	  delete pFileBuf;
	  }  
   
   delete pgprcSendFileInfo;
   return pgprc_returnok;
   }

PGPRC_RETURNVAL CPGPRCServer::PrintJob(int len, HANDLE hJob, CClientSocket* pSocket)
   {
   CSocketJob* pJob;
   POURPGPRCPHOTOSPECS pPhoto;   
   CQueueFile* pCQueueFile;   
   int iPage;
   PGPRC_RETURNVAL gRetVal = pgprc_returnok;
   int iMontageCanvasToUse;
   int iNumToMontage = 0;
   int iTotalPages = 0;

   // resolve the job
   pJob = (CSocketJob*)hJob;

   // sanity check
   if (!pJob) 
	  {
	  DumpIncomingData(len, pSocket);
	  return pgprc_error_invalid_job_handle;
	  }
   if (!(pJob->IsKindOf(RUNTIME_CLASS(CSocketJob)))) 
	  {
	  DumpIncomingData(len, pSocket);
	  return pgprc_error_invalid_job_handle;
	  }
   if (len != sizeof PGPRCPRINTSPECS)
	  {
	  DumpIncomingData(len, pSocket);
	  return pgprc_server_bad_datalen;
	  }
					 
   // copy the print specs
   pSocket->m_pArchiveIn->Read(&pJob->PGPRCPrintSpecs, len);
   // ReportByteCount(FALSE, len);

   // create the queuefiles
   gRetVal = CreateQueueFiles(pJob);
   if (gRetVal != pgprc_returnok)
	  {
	   switch (gRetVal)
	   {
	   case pgprc_error_outofmemory:
		   RPDEBUGSTRMFC("GISServer: CreateQueueFiles(Out of Memory");
		   break;
	   case pgprc_cant_find_file:
		   RPDEBUGSTRMFC("GISServer: CreateQueueFiles(Can't Find File");
		   break;
	   case pgprc_error_unsupported_filetype:
		   RPDEBUGSTRMFC("GISServer: CreateQueueFiles(Invalid File Type");
		   break;
	   default:
		   RPDEBUGSTRMFC("GISServer: Can't create queue files");
		   break;
	   }
	  return gRetVal;
	  }

   // do we need to add a separator?
   if (pJob->bDoJobSeparator)
	  {	  
	  int iStart, iEnd, iTotalPhotos;	  

	  // total number of photos
	  //iTotalPhotos = pJob->Photos.GetCount();
	  iTotalPhotos = GetRealPhotoCount(pJob);

	  iStart = 0; 
	  iEnd = iStart + PGPRC_PERPAGE_TOTALPHOTOS_PAGE_1;
	  // get pages
	  GetTotalSeparatorPages(pJob);
	  // current page
	  iPage = 0;

	  do
		 {	 
		 switch(pJob->JobSeparator.iStyle)
			{
			case PGPRC_JOBSEPARATOR_STYLE_1:		   
			     // we need to add iPage to iStart and iEnd because we've added a separator
			     // photo to the start of the photo list!
			     if (!CreateJobSeparatorFiles_1(pJob, iStart + iPage, iEnd + iPage, iPage))
				    {
				    return pgprc_error_cantcreate_jobseparator;
				    }
			     break;
			default:
			case PGPRC_JOBSEPARATOR_STYLE_0:  
 			     if (!CreateJobSeparatorFiles_0(pJob, iStart + iPage, iEnd + iPage, iPage))
				    {
				    return pgprc_error_cantcreate_jobseparator;
				    }
			     break;
			}	   	

		 if (iStart == 0)
			{
			iStart += PGPRC_PERPAGE_TOTALPHOTOS_PAGE_1;
		    iTotalPhotos -= PGPRC_PERPAGE_TOTALPHOTOS_PAGE_1;
			}
		 else
			{
			iStart += PGPRC_PERPAGE_TOTALPHOTOS_PAGE_N;
			iTotalPhotos -= PGPRC_PERPAGE_TOTALPHOTOS_PAGE_N;		 
			}
		 iEnd = iStart + PGPRC_PERPAGE_TOTALPHOTOS_PAGE_N;
		 iPage++;

		 // now insert the separator into the job list	  
  		 if (pPhoto = new OURPGPRCPHOTOSPECS)
			{
			ZeroMemory(pPhoto, sizeof (OURPGPRCPHOTOSPECS));		 		 
			pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcImageFilter.iNumFiltersToUse = 0;			 
			pPhoto->iJobID = 0;
			pPhoto->iReportedCopyCount = 1;
			pPhoto->PhotoSpecs.BasePhotoSpecs.iCopies = 1;
			pPhoto->bUseTempFile = FALSE;		 
			pPhoto->FileType = FILE_EPS;
			pPhoto->bIsSeparator = TRUE;
			strcpy(pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile, pJob->csJobSeparatorFile);
			// don't put in the thumbnails
			pPhoto->bPutInThumbnails = FALSE;
			pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcSizing = pJob->JobSeparator.pgprcSizing;
			pPhoto->PhotoSpecs.BasePhotoSpecs.iCanvasToUse = pJob->JobSeparator.iCanvasToUse;
			pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000 = 
			   pJob->JobSeparator.iMontageWidthInches1000;
			pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000 = 
			   pJob->JobSeparator.iMontageHeightInches1000;
			pPhoto->PhotoSpecs.BasePhotoSpecs.iMontagePhotoSpacingXInches1000  = 
			   pJob->JobSeparator.iMontagePhotoSpacingXInches1000;
			pPhoto->PhotoSpecs.BasePhotoSpecs.iMontagePhotoSpacingYInches1000 = 
			   pJob->JobSeparator.iMontagePhotoSpacingYInches1000;

			pPhoto->bAddedToQueue = FALSE;
			
			// insert into photo list					 
			// make a queue file if we are not montaging it
			if (pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcSizing != pgprc_montage_crop &&
			    pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcSizing != pgprc_montage_fit)
			   {
			   // instantiate a queuefile 
			   if ((pCQueueFile = new CQueueFile) == NULL)
				  {
				  delete pPhoto;	  
				  return pgprc_error_outofmemory;		 
				  }

			   // save the pointer
			   pPhoto->hCqueueFile = pCQueueFile;   

			   // and the job id
			   pPhoto->iJobID = pCQueueFile->iJobID;

			   // init the queue file
			   if (!pCQueueFile->Init(pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile, iTotalPages, 0))
				  {
				  delete pCQueueFile;
				  pPhoto->hCqueueFile = NULL;
				  pCQueueFile = NULL;
				  delete pPhoto;	 
				  pPhoto = NULL;
				  return pgprc_cant_find_file;		 
				  }

			   // set this flag
			   pCQueueFile->ExtraInfo = CQUEUE_EXTRA_PGPRCFILE;

			   // mark this for deletion
			   pCQueueFile->ExtraInfo |= CQUEUE_EXTRA_SERVERFILE;

			   // add a description
			   pCQueueFile->Description.Format("%s Photogize Index Print with %d Photos", 
				  pJob->csName, pJob->iPhotoCounter);   
			   }		
			// add add to the beginning of the photo array
			POSITION pos;
			int j;
			pos = pJob->Photos.GetHeadPosition();
			// insert after last
			for (j = 1; j < iPage; j++)
			   {
			   pJob->Photos.GetNext(pos);
			   }
			pJob->Photos.InsertBefore(pos, pPhoto);		
			// decrement these to account for index print
			iStart--;iEnd--;
			}													 
	  	 } while (iTotalPhotos > 0);							 
	  }

   // we need to determine the optimal orientation of the 
   // printspecs width and height

   // reset montage flags
   ResetMontageFlags(pJob);

   // now loop through the photos and create the appropriate montages
   for(;;)
	  {	  
	  // get montage set
	  iNumToMontage = CheckNextMontageSet(pJob, iMontageCanvasToUse);

	  // all done?
	  if (iNumToMontage < 1) break;

	  // compute offsets and pages for montage
	  int iPageStd = 1;
	  int iPageSwapped = 1;	  

	  // first start with the pPhoto->iMontageWidthInches1000 and 
	  // pPhoto->iMontageHeightInches1000 as passed   
	  gRetVal = ComputeMontagePhotosVars(pJob, iPageStd, iMontageCanvasToUse);
	  // now swap and try again   
	  SwapCustomPageDims(pJob);
	  // compute   
	  gRetVal = (gRetVal == pgprc_returnok) ? 
		 ComputeMontagePhotosVars(pJob, iPageSwapped, iMontageCanvasToUse) : gRetVal;
	  // swap back in case of error
	  SwapCustomPageDims(pJob);
	  // process error
	  if (gRetVal != pgprc_returnok)
		 {
		 break;
		 }				  	  
	  
	  // neither way fits!
	  if (iPageSwapped > MISFIT_PENALTY && iPageStd > MISFIT_PENALTY)
		 {
		 // Recompute and set to Media Size(TRUE)
		 iPageStd = 1;
		 gRetVal = ComputeMontagePhotosVars(pJob, iPageStd, iMontageCanvasToUse, TRUE);
//		 gRetVal = pgprc_error_cant_create_montage;
//		 break;
		 }
	  else if (iPageSwapped < iPageStd) // go with the original
		 SwapCustomPageDims(pJob);
	  else
		 {
		 // otherwise, recompute!
		 iPageStd = 1;
		 gRetVal = ComputeMontagePhotosVars(pJob, iPageStd, iMontageCanvasToUse);
		 }   
	  
	  if (gRetVal == pgprc_returnok) 
		 gRetVal = MontagePhotos(pJob);	  
	  
	  if (gRetVal != pgprc_returnok) 
		 {
		 break;
		 }
	  } 
   
   if (gRetVal != pgprc_returnok)
	  {
      return gRetVal;
	  }

   // insert the jobs into the queue
   InsertJobs(pJob);

   return pgprc_returnok;
   }

PGPRC_RETURNVAL CPGPRCServer::InsertJobs(CSocketJob* pJob)
   {
   POSITION pos;
   POURPGPRCPHOTOSPECS pPhoto;
   PGPRC_RETURNVAL gRetVal = pgprc_returnok;
   CZeusDoc* pDoc;
   CPaneElement* pRipElement = NULL;
   CQueueFile* pCQueueFile;
   CSpecialTree* pcstPtr = NULL;
   DEVBACKPRINTDATA DevBackPrintData;
   DEVPRINTONBACKDATA DevPrintOnBackData;

   // loop through the photos
   for (pos = pJob->Photos.GetHeadPosition(); pos != NULL;)
	  {	  	  
	  // get the photo
	  pPhoto = (POURPGPRCPHOTOSPECS)pJob->Photos.GetNext(pos);

	  // get the queuefile pointer
	  pCQueueFile = (CQueueFile*)pPhoto->hCqueueFile;

	  // do we have a queue file?
	  if (!pCQueueFile) 
		 continue;
      if (!(pCQueueFile->IsKindOf(RUNTIME_CLASS(CQueueFile)))) 
		 return pgprc_error_invalid_photo_handle;

      // resolve the document
      pDoc = (CZeusDoc*)(pJob->hDevice);

      // sanity check
      if (!pDoc) return pgprc_error_invalid_device_handle;
      if (!(pDoc->IsKindOf(RUNTIME_CLASS(CZeusDoc)))) 
	     return pgprc_error_invalid_device_handle;

	  // load the appropriate transform
	  pCQueueFile->pTransform = new CTransform;
	  pCQueueFile->Prefs |= USE_LOCAL_TRN_PREFS;

	  // always smart fit separators
	  if (pPhoto->bIsSeparator)
		 pCQueueFile->pTransform->LoadTransform("SmartFit");
	  else
		 {	   	  
		 switch(pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcSizing)
			{
			case pgprc_fit:	
			     pCQueueFile->pTransform->LoadTransform("SmartFit");
			     break;
			case pgprc_crop:
			     pCQueueFile->pTransform->LoadTransform("SmartCrop");
			     break;		 
			default:			  
			     pCQueueFile->pTransform->LoadTransform("SmartFit");
			     break;
			}
		 }	  	  

	  // start with the queue device
	  pCQueueFile->pCD = new CDevice;

	  *(pCQueueFile->pCD) = QUEUEDEVICE;
	  pCQueueFile->Prefs |= USE_LOCAL_DEV_PREFS;

	  // set the copies
	  pCQueueFile->pCD->pDevInfo->Copies = pPhoto->PhotoSpecs.BasePhotoSpecs.iCopies;

	  // force this off
	  pCQueueFile->pCD->pDevInfo->OptimizeOrientation = FALSE;

	  // set the canvas
	  pCQueueFile->pCD->pDevInfo->CurrentMediaIdx = pPhoto->PhotoSpecs.BasePhotoSpecs.iCanvasToUse;
	  
	  // set the backprint if it exists
	  if (pPhoto->PhotoSpecs.cBackPrint1[0] != 0)
		 {
		 ZeroMemory(&DevBackPrintData, sizeof DEVBACKPRINTDATA);
		 DevBackPrintData.pDevInfo = (BYTE*)(pCQueueFile->pCD->pDevInfo);
		 strcpy(DevBackPrintData.cBackprintData, pPhoto->PhotoSpecs.cBackPrint1);
		 DevBackPrintData.iLineToSet = 0;
		 QUEUEDEVICE.GetSetExtra(DEVGETSET_SETBACKPRINT, (BYTE*)&DevBackPrintData, NULL, 
			sizeof DEVBACKPRINTDATA, 0);
		 }
	  if (pPhoto->PhotoSpecs.cBackPrint2[0] != 0)
		 {
		 ZeroMemory(&DevBackPrintData, sizeof DEVBACKPRINTDATA);
		 DevBackPrintData.pDevInfo = (BYTE*)(pCQueueFile->pCD->pDevInfo);
		 strcpy(DevBackPrintData.cBackprintData, pPhoto->PhotoSpecs.cBackPrint2);
		 DevBackPrintData.iLineToSet = 1;
		 QUEUEDEVICE.GetSetExtra(DEVGETSET_SETBACKPRINT, (BYTE*)&DevBackPrintData, NULL, 
			sizeof DEVBACKPRINTDATA, 0);
		 }

	  ZeroMemory(&DevPrintOnBackData, sizeof DEVPRINTONBACKDATA);
	  DevPrintOnBackData.pDevInfo = (BYTE*)(pCQueueFile->pCD->pDevInfo);
	  DevPrintOnBackData.bPrintOnBack = pPhoto->PhotoSpecs.bPrintOnBack;
	  QUEUEDEVICE.GetSetExtra(DEVGETSET_SETPRINTONBACK, (BYTE*)&DevPrintOnBackData, NULL, 
			sizeof DEVPRINTONBACKDATA, 0);

	  // print to hold?
	  pCQueueFile->iPrintToHold = pJob->PGPRCPrintSpecs.bPrintToHold;

      // set the owner to be the orderid
	  pCQueueFile->csOwner = pJob->PGPRCPrintSpecs.OrderIDString;

	  // get the proper pane
	  if (pCQueueFile->iPrintToHold)
	     pRipElement = GetRipPane(pJob, QUEUESTYLE_HOLD);
	  else
		 pRipElement = GetRipPane(pJob, QUEUESTYLE_PRINT);

	  // get special tree
	  if (pRipElement)
		 pcstPtr = (CSpecialTree*)pRipElement->pcstPtr;

	  // add the file
	  if (pcstPtr)		 
		 {
		 // add if it is a separator, or if it is a photo AND bPrintJobSeparatorOnly
		 // is not set
		 if (pPhoto->bIsSeparator || 
			 (!pPhoto->bIsSeparator && !pJob->PGPRCPrintSpecs.bPrintJobSeparatorOnly))
			{
		    pcstPtr->AddToQueue(pCQueueFile);	  
			pPhoto->bAddedToQueue = TRUE;
		    pJob->iTotalPages++;
			}
		 }
	  else
		 return pgprc_error_cantfind_device_queue;
	  }	
	  
   return pgprc_returnok;
   }

PGPRC_RETURNVAL CPGPRCServer::ApplyFilter(CSocketJob* pJob, POURPGPRCPHOTOSPECS pPhoto)
   {
   int i;
   BITMAPHANDLE LeadBitmap;
   CString csTempPath;
   char pTempFileName[_MAX_PATH];
   FILEINFO FileInfo;
   BOOL bMustLoadSave = FALSE;

   // get file info to see if we need to convert the file to 24 bit color
   memset(&FileInfo, 0, sizeof(FILEINFO));
   FileInfo.uStructSize = sizeof(FILEINFO);
   if (L_FileInfo(pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile, &FileInfo, sizeof(FILEINFO), FILEINFO_TOTALPAGES, NULL) > 0)
	  {
	  // if we don't have a 24 bit file AND we are not applying filters, force a load
	  // and save 
	  if (FileInfo.BitsPerPixel != 24 && pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcImageFilter.iNumFiltersToUse == 0)
		 {
		 bMustLoadSave = TRUE;
		 // make sure the filters are off!
		 for (i = 0; i < PGPRC_MAXFILTERS; i++)
			{
			pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcImageFilter.pgprcImageFilterType[i] = pgprc_imagefilter_none;
			}
		 }
	  }

   if (pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcImageFilter.iNumFiltersToUse > 0 || bMustLoadSave)
	  {
	  if (L_LoadBitmap(pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile, &LeadBitmap, sizeof(BITMAPHANDLE), 0, ORDER_RGB, NULL, NULL) != SUCCESS)
		 {
		 return pgprc_error_cant_apply_filter;		 
		 }
	  else
		 {
		 for (i = 0; i < PGPRC_MAXFILTERS; i++)
			{
			// what kind of filter?
			switch(pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcImageFilter.pgprcImageFilterType[i])
			   {
			   case pgprc_imagefilter_sharpen:
				    L_SharpenBitmap(&LeadBitmap, pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcImageFilter.iVar[i]);
				    break;
			   case pgprc_imagefilter_contrast:
				    L_ChangeBitmapContrast(&LeadBitmap, pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcImageFilter.iVar[i]);
				    break;
			   case pgprc_imagefilter_intensity:
				    L_ChangeBitmapIntensity(&LeadBitmap, pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcImageFilter.iVar[i]);
				    break;
			   case pgprc_imagefilter_saturation:
				    L_ChangeBitmapIntensity(&LeadBitmap, pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcImageFilter.iVar[i]);
				    break;
			   case pgprc_imagefilter_gamma:
				    L_GammaCorrectBitmap(&LeadBitmap, pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcImageFilter.iVar[i]);
				    break;
			   }
			}
		 // resolve the device to get the temp path
		 CZeusDoc* pDoc;
		 if (!(pDoc = (CZeusDoc*)(pJob->hDevice)))
			return pgprc_error_invalid_job_handle;
		 csTempPath = pDoc->crrrs.csScratchPath;

		 // make a temp file
		 GetTempFileName(csTempPath, "pgprc", 0, pTempFileName);
		 // now write out the file to a temp file
         L_SaveBitmap(pTempFileName, &LeadBitmap, FILE_TIF, 24, 0, NULL);
		 // Free the temporary bitmap
		 L_FreeBitmap(&LeadBitmap);
		 // we're now using a temp file
		 pPhoto->bUseTempFile = TRUE;
		 // store the name
		 strcpy(pPhoto->lpszTempDiskFile, pTempFileName);
		 }	  
	  }
   return pgprc_returnok;
   }

CPaneElement* CPGPRCServer::GetRipPane(CSocketJob* pJob, int iViewStyle)
   {
   int i;
   CPaneElement* pRipElement;
   CPaneElement* pRipElementRet = NULL;   
   CZeusDoc* pDoc;
   CZeusDoc* pPassedDoc;

   // resolve the document
   pPassedDoc = (CZeusDoc*)(pJob->hDevice);   

   // sanity check
   if (!pPassedDoc) return NULL;
   if (!(pPassedDoc->IsKindOf(RUNTIME_CLASS(CZeusDoc)))) 
	  return NULL;

   for (i = 0; i < PANEARRAY.GetSize(); i++)
	  {
	  pRipElement = (CPaneElement*)PANEARRAY.GetAt(i);

	  if (!pRipElement) continue;

      // get the active document
      if (pDoc = (CZeusDoc*)(pRipElement->pDocPtr))
		 {
		 // this is the one
		 if ((pPassedDoc == pDoc) && (iViewStyle == pRipElement->iViewStyle))
			{
			pRipElementRet = pRipElement;
			break;
			}
		 }
	  }
   return pRipElementRet;
   }

PGPRC_RETURNVAL CPGPRCServer::GetJobStatus(int len, HANDLE hJob, CClientSocket* pSocket)
   {
   int i = 0;      
   PGPRCJOBSTATUSSTRUCT PGPRCJobStatusStruct;
   CSocketJob* pJob;
   POSITION pos;
   POURPGPRCPHOTOSPECS pPhoto;   
   PGPRC_RETURNVAL gRetVal = pgprc_returnok;

   // we don't care about the incoming data
   DumpIncomingData(len, pSocket);      

   // default
   PGPRCJobStatusStruct.pgprcJobStatus = pgprc_jobstatus_ok;   

   // resolve the job
   pJob = (CSocketJob*)hJob;

   // sanity check
   if (!pJob) 
	  {
	  return pgprc_error_invalid_job_handle;
	  }
   if (!(pJob->IsKindOf(RUNTIME_CLASS(CSocketJob)))) 
	  {
	  return pgprc_error_invalid_job_handle;
	  }

   // init these
   PGPRCJobStatusStruct.iHoldCount = 0;
   PGPRCJobStatusStruct.iPrintCount = 0;
   PGPRCJobStatusStruct.iArchiveCountGood = 0;
   PGPRCJobStatusStruct.iArchiveCountBad = 0;
   PGPRCJobStatusStruct.FirstError[0] = '\0';
   PGPRCJobStatusStruct.iTotalPhotos = pJob->iPhotoCounter;
   PGPRCJobStatusStruct.iTotalPages = pJob->iTotalPages;

   // loop through the photos to get status
   for (pos = pJob->Photos.GetHeadPosition(); pos != NULL;)
	  {	  	
	  pPhoto = (POURPGPRCPHOTOSPECS)pJob->Photos.GetNext(pos);
      gRetVal = GetInternalPhotoStatus(pJob, pPhoto, &PGPRCJobStatusStruct);
	  if (gRetVal != pgprc_returnok) 
		 {
		 break;
		 }
	  }
   
   // copy the job status over
   *(pSocket->m_pArchiveOut) << (sizeof (PGPRCJOBSTATUSSTRUCT));
   // ReportByteCount(TRUE, sizeof (int));
   pSocket->m_pArchiveOut->Write(&PGPRCJobStatusStruct, sizeof PGPRCJOBSTATUSSTRUCT);
   // ReportByteCount(TRUE, sizeof PGPRCJOBSTATUSSTRUCT);

#ifdef RPDEBUGSTRMFC
	  {				
	  CString csDebug;
	  char* p;
	  csDebug.Format("PGPRCServer: GetJobStatus, returnval = %d", gRetVal);
	  p = (char*)(const char*)csDebug;
      RPDEBUGSTRMFC(p);
	  }
#endif
   return gRetVal;
   }			 

PGPRC_RETURNVAL CPGPRCServer::GetInternalPhotoStatus(CSocketJob* pJob, POURPGPRCPHOTOSPECS pPhoto, 
										 PPGPRCJOBSTATUSSTRUCT pPgprcJobStatusStruct)
   {
   CPaneElement* pRipElement;   
   CZeusDoc* pDoc;   
   CZeusDoc* pPassedDoc;
   int i, j;
   int iCnt = 0;   
   CQueueFile* pThisCQueueFile;

   // resolve the document
   pPassedDoc = (CZeusDoc*)(pJob->hDevice);  

   // sanity check
   if (!pPassedDoc) return pgprc_error_invalid_device_handle;
   if (!(pPassedDoc->IsKindOf(RUNTIME_CLASS(CZeusDoc)))) 
	  return pgprc_error_invalid_device_handle;

   // search all the rip arrays for files which match our photo id
   // and generate the status
   for (i = 0; i < PANEARRAY.GetSize(); i++)
	  {
	  pRipElement = (CPaneElement*)PANEARRAY.GetAt(i);

	  if (!pRipElement) continue;

      // get the active document
      if (pDoc = (CZeusDoc*)(pRipElement->pDocPtr))
		 {
		 // doesn't match our doc so continue;
		 if (pPassedDoc != pDoc) continue;
		 // is this a hold queue?
		 if (pRipElement->iViewStyle == QUEUESTYLE_HOLD)
			{
		    // get the hold queue files
            for (j = 0; j < pDoc->CQ_Hold.GetSize(); j++)
			   {
			   // get file
			   pThisCQueueFile = (CQueueFile*)pDoc->CQ_Hold.GetAt(j);
			   // a match!
			   if (pPhoto->iJobID == pThisCQueueFile->iJobID)
				  pPgprcJobStatusStruct->iHoldCount++;
			   }
			}
		 else if (pRipElement->iViewStyle == QUEUESTYLE_PRINT)
			{
		    // get the print queue files
            for (j = 0; j < pDoc->CQ.GetSize(); j++)
			   {
		       // get file
			   pThisCQueueFile = (CQueueFile*)pDoc->CQ.GetAt(j);
			   // a match!
			   if (pPhoto->iJobID == pThisCQueueFile->iJobID)
				  pPgprcJobStatusStruct->iPrintCount++;
			   }
			}
		 else if (pRipElement->iViewStyle == QUEUESTYLE_ARCHIVE)
			{												   
		    // get the archive queue files
            for (j = 0; j < pDoc->CQ_Archive.GetSize(); j++)
			   {
			   // get file
			   pThisCQueueFile = (CQueueFile*)pDoc->CQ_Archive.GetAt(j);
			   // a match!
			   if (pPhoto->iJobID == pThisCQueueFile->iJobID)
				  {
				  // now check the status for a possible error
				  if ((pThisCQueueFile->StatusFlag & FILE_STATUS_CHECK) == FILE_STATUS_NOTOK)
					 {
					 pPgprcJobStatusStruct->iArchiveCountBad++;
					 // now get the error string
					 strcpy(pPgprcJobStatusStruct->FirstError, pThisCQueueFile->Status);
					 }
				  else
					 {
					 pPgprcJobStatusStruct->iArchiveCountGood++;
					 }
				  }
			   }
			}
         }   
	  }
   return pgprc_returnok;
   }

BOOL CPGPRCServer::CreateJobSeparatorFiles_0(CSocketJob* pJob, int iStart, int iEnd, int iPage)
   {   
   CString csTemp;
   CString csTempPath;			 
   char pTempFileName[_MAX_PATH];
   CStdioFile cf;
   int i;
   POSITION pos;
   POURPGPRCPHOTOSPECS pPhoto;   
   char* lpFileName;
   BITMAPHANDLE TileBitmap;
   BITMAPHANDLE ImageBitmap;   
   BITMAPHANDLE LeftLogoBitmap;
   BITMAPHANDLE RightLogoBitmap;
   BITMAPHANDLE BarCodeBitmap;
   int NewWidth, NewHeight;
   int xdest, ydest;
   int xidx, yidx;
   float fARi, fARt;
   BOOL bFlip;   
   L_INT lRet;
   BYTE* pData;
   UINT uiOpenFlags;
   BOOL bRet = FALSE;    
   CString csTempBarCodeFileName;
   int iNewWidth;
   int iNewHeight;
   CProfile Profile;

   // create a new bitmap for tiling the contact prints
   NewWidth = PGPRC_CONTACTIMAGE_WIDTH * PGPRC_CONTACTIMAGE_NUMACCROSS +
	  (PGPRC_CONTACTIMAGE_NUMACCROSS - 1) * PGPRC_CONTACTIMAGE_SPACINGX;

   if (iPage == 0)
      NewHeight = PGPRC_CONTACTIMAGE_HEIGHT * PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_1 +
		         (PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_1 - 1) * PGPRC_CONTACTIMAGE_SPACINGY;	  
   else
      NewHeight = PGPRC_CONTACTIMAGE_HEIGHT * PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_N +
		         (PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_N - 1) * PGPRC_CONTACTIMAGE_SPACINGY;	  

   if (L_CreateBitmap(&TileBitmap, sizeof(BITMAPHANDLE), TYPE_CONV, NewWidth, NewHeight, 24, ORDER_RGB,
						NULL, TOP_LEFT, NULL, 0) != SUCCESS)
		return FALSE;

   L_FillBitmap(&TileBitmap, RGB(255, 255, 255));

   // load the logo file, don't totally fail if it's not there   
   if (pJob->JobSeparator.bDoLeftLogo && iPage == 0)
	  {
	  // if the logo file name is empty, use the one set in options
	  if (strlen(pJob->JobSeparator.lpszLeftLogoFile) < 1)
		 {
		 csTemp = Profile.GetStr(IDS_PHOTOGIZE, "LeftLogo", "");		 
		 if (csTemp.GetLength() > 0)
		    strcpy(pJob->JobSeparator.lpszLeftLogoFile, csTemp);
		 }
	  if (L_LoadBitmap(pJob->JobSeparator.lpszLeftLogoFile, 
				&LeftLogoBitmap, sizeof(BITMAPHANDLE), 0, ORDER_RGB, NULL, NULL) != SUCCESS)
		 pJob->JobSeparator.bDoLeftLogo = FALSE;	 		 
	  else
		 {
		 if (LeftLogoBitmap.BitsPerPixel != 24)
			{
			L_FreeBitmap(&LeftLogoBitmap);
			pJob->JobSeparator.bDoLeftLogo = FALSE;
			}
		 }
	  }	 	  

   if (pJob->JobSeparator.bDoRightLogo && iPage == 0)
	  {
	  // if the logo file name is empty, use the one set in options
	  if (strlen(pJob->JobSeparator.lpszRightLogoFile) < 1)
		 {
		 csTemp = Profile.GetStr(IDS_PHOTOGIZE, "RightLogo", "");		 
		 if (csTemp.GetLength() > 0)
		    strcpy(pJob->JobSeparator.lpszRightLogoFile, csTemp);
		 }											   
	  if (L_LoadBitmap(pJob->JobSeparator.lpszRightLogoFile, 
						&RightLogoBitmap, sizeof(BITMAPHANDLE), 0, ORDER_RGB, NULL, NULL) != SUCCESS)
		 pJob->JobSeparator.bDoRightLogo = FALSE;	 		 
	  else
		 {
		 if (RightLogoBitmap.BitsPerPixel != 24)
			{
			L_FreeBitmap(&RightLogoBitmap);
			pJob->JobSeparator.bDoRightLogo = FALSE;
			}
		 }
	  }	 	  

   /// do we want a barcode?
   if (pJob->JobSeparator.bDoBarCode && iPage == 0 && 
	  strlen(pJob->PGPRCPrintSpecs.OrderIDString) > 0)
	  {
	  bRet = DoBarCode(pJob, csTempBarCodeFileName);
	  if (!bRet)		 
		 pJob->JobSeparator.bDoBarCode = FALSE;		 
	  else
		 {
	     if (L_LoadBitmap((char*)(const char*)csTempBarCodeFileName, 
							&BarCodeBitmap, sizeof(BITMAPHANDLE), 0, ORDER_RGB, NULL, NULL) != SUCCESS)
		    pJob->JobSeparator.bDoBarCode = FALSE;		 
		 // and blow it away - we don't need it anymore
		 ::DeleteFile(csTempBarCodeFileName);
		 }
	  }
   else
	  pJob->JobSeparator.bDoBarCode = FALSE;

   if (pJob->JobSeparator.bDoThumbnails)
	  {	  
	  // impose index prints into the Tile Bitmap
	  xidx = 0; yidx = 0;
	  xdest = 0; ydest = 0;
	  int idx;
	  //for (pos = pJob->Photos.GetHeadPosition(), idx = 0; pos != NULL; idx++)
	  for (pos = pJob->Photos.GetHeadPosition(), idx = 0; pos != NULL;)
		 {	  
		 // get the photo
		 pPhoto = (POURPGPRCPHOTOSPECS)pJob->Photos.GetNext(pos);		 
		 // check bounds
		 //if (idx < iStart || idx > iEnd) continue;
		 
		 if (!pPhoto->bPutInThumbnails) continue;

		 if (idx < iStart || idx > (iEnd - 1)) 
			{
			idx++;
			continue;
			}

		 idx++;

		 // which file to use?
		 if (pPhoto->bUseTempFile)
			lpFileName = pPhoto->lpszTempDiskFile;
		 else
			lpFileName = pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile;

		 // resize the images
		 if (L_LoadBitmap(lpFileName, &ImageBitmap, sizeof(BITMAPHANDLE), 0, ORDER_RGB, NULL, NULL) == SUCCESS)
			{
			// get aspect ratios
			fARi = (float)ImageBitmap.Height / (float)ImageBitmap.Width;
			fARt = (float)(PGPRC_CONTACTIMAGE_HEIGHT) / (float)(PGPRC_CONTACTIMAGE_WIDTH);		 

			// do we need to flip the image?
			if (fARi > 1.0)
			   {
			   fARi = 1.0 / fARi;
			   bFlip = TRUE;
			   }
			else			
			   bFlip = FALSE;			

			// get new height and width
			if (fARi < fARt)
			   {
			   NewWidth = PGPRC_CONTACTIMAGE_WIDTH;
			   NewHeight = (int)((float)(PGPRC_CONTACTIMAGE_WIDTH) * fARi + .5F);
			   }
			else
			   {
			   NewWidth = (int)((float)(PGPRC_CONTACTIMAGE_HEIGHT) / fARi + .5F);
			   NewHeight = PGPRC_CONTACTIMAGE_HEIGHT;					
			   }
			// resize it
			if (bFlip)
			   lRet = L_SizeBitmap(&ImageBitmap, NewHeight, NewWidth, SIZE_BICUBIC);
			else
			   lRet = L_SizeBitmap(&ImageBitmap, NewWidth, NewHeight, SIZE_BICUBIC);

			if (lRet != SUCCESS)
			   {
			   L_FreeBitmap(&ImageBitmap);
			   L_FreeBitmap(&TileBitmap);
			   if (pJob->JobSeparator.bDoLeftLogo && iPage == 0)
				  L_FreeBitmap(&LeftLogoBitmap);
			   if (pJob->JobSeparator.bDoRightLogo && iPage == 0)
				  L_FreeBitmap(&RightLogoBitmap);
			   if (pJob->JobSeparator.bDoBarCode && iPage == 0)
				  L_FreeBitmap(&BarCodeBitmap);
			   return FALSE;
			   }
			// flip it?
			if (bFlip)
			   {
			   // rotate it
			   if (L_RotateBitmap(&ImageBitmap, 9000, ROTATE_RESIZE,  RGB(255, 255, 255)) != SUCCESS)
				  {
				  L_FreeBitmap(&ImageBitmap);
				  L_FreeBitmap(&TileBitmap);
				  if (pJob->JobSeparator.bDoLeftLogo && iPage == 0)
					 L_FreeBitmap(&LeftLogoBitmap);
				  if (pJob->JobSeparator.bDoRightLogo && iPage == 0)
					 L_FreeBitmap(&RightLogoBitmap);
			      if (pJob->JobSeparator.bDoBarCode && iPage == 0)
				     L_FreeBitmap(&BarCodeBitmap);
				  return FALSE;
				  }			
			   }			
			// get the index of the index
			xidx %= PGPRC_CONTACTIMAGE_NUMACCROSS;
			if (iPage == 0)
			   yidx %= PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_1;
			else
			   yidx %= PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_N;

			xdest = xidx * PGPRC_CONTACTIMAGE_WIDTH + xidx * PGPRC_CONTACTIMAGE_SPACINGX;
			ydest = yidx * PGPRC_CONTACTIMAGE_HEIGHT + yidx * PGPRC_CONTACTIMAGE_SPACINGY;

			// offsets?
			xdest += (PGPRC_CONTACTIMAGE_WIDTH - ImageBitmap.Width) / 2;
			ydest += (PGPRC_CONTACTIMAGE_HEIGHT - ImageBitmap.Height) / 2;

			// now impose it into the tile bitmap		 
			if (L_CombineBitmap(&TileBitmap, xdest, ydest, ImageBitmap.Width, ImageBitmap.Height,
			   &ImageBitmap, 0, 0, CB_OP_ADD | CB_DST_0) != SUCCESS)
			   {
			   L_FreeBitmap(&ImageBitmap);
			   L_FreeBitmap(&TileBitmap);
			   if (pJob->JobSeparator.bDoLeftLogo && iPage == 0)
				  L_FreeBitmap(&LeftLogoBitmap);
			   if (pJob->JobSeparator.bDoRightLogo && iPage == 0)
				  L_FreeBitmap(&RightLogoBitmap);
			   if (pJob->JobSeparator.bDoBarCode && iPage == 0)
				  L_FreeBitmap(&BarCodeBitmap);
			   return FALSE;
			   }
			xidx++;

			// carriage return
			if (xidx == PGPRC_CONTACTIMAGE_NUMACCROSS)
			   {
			   xidx = 0;
			   yidx++;
			   }
			
			// Free the temporary bitmap
			L_FreeBitmap(&ImageBitmap);
			}
		 }
      }

   // resolve the device to get the temp path
   CZeusDoc* pDoc;
   if (!(pDoc = (CZeusDoc*)(pJob->hDevice)))
	  return pgprc_error_invalid_job_handle;
   csTempPath = pDoc->crrrs.csScratchPath;

   // make a temp file		 
   GetTempFileName(csTempPath, "pgprc", 0, pTempFileName);

   // save the name
   pJob->csJobSeparatorFile = pTempFileName;		 

   uiOpenFlags = CFile::modeCreate | CFile::modeWrite | CFile::typeBinary;

   // create it
   if (cf.Open(pJob->csJobSeparatorFile, uiOpenFlags))
	  {
	  if (iPage == 0)
		 {
	     // write the prolog	     
		 WritePSStart(cf, PGPRC_SEPARATOR_BBOX_WIDTH, PGPRC_SEPARATOR_BBOX_HEIGHT, FALSE);
	     // write the address	  	 
		 for (i = 0, ydest = 150; i < PGPRC_NUM_ADDRESS_LINES; i++, ydest -= 15)
			{
		    fprintf(cf.m_pStream, "240 %d M\r\n", ydest);
		    fprintf(cf.m_pStream, "(%s) S\r\n", 
			   CStringToPSString(pJob->JobSeparator.lpszAddress[i]));
			}
		 // now do order info		 
		 fprintf(cf.m_pStream, "/Courier findfont [8 0 0 8 0 0] makefont setfont\r\n");
 		 for (i = 0, ydest = 180; i < PGPRC_NUM_ORDERINFO_LINES; i++, ydest -= 10)
			{
		    fprintf(cf.m_pStream, "15 %d M\r\n", ydest);
		    fprintf(cf.m_pStream, "(%s) S\r\n", 
			   CStringToPSString(pJob->JobSeparator.lpszOrderInfo[i]));
			} 
		 }
	  else
		 {
		 cf.SeekToEnd(); // go to the end
	     // write the prolog	     
		 WritePSStartN(cf, PGPRC_SEPARATOR_BBOX_WIDTH, PGPRC_SEPARATOR_BBOX_HEIGHT);
		 }	
	  // write page number	  
	  fprintf(cf.m_pStream, "/Courier findfont [10 0 0 10 0 0] makefont setfont\r\n");
	  fprintf(cf.m_pStream, "300 5 M\r\n");
	  fprintf(cf.m_pStream, "(%d of %d) S\r\n", iPage + 1, pJob->iNumSeparatorPages);		 	  

      // write the bitmap to the eps file
      fprintf(cf.m_pStream, "GS /picstr %d string def\r\n", 
		 TileBitmap.Width * 3);

	  if (pJob->JobSeparator.bDoThumbnails)
		 {	  
		 if (iPage == 0)
			fprintf(cf.m_pStream, "5 282 translate %d %d scale\r\n", 	  
			   PGPRC_SEPARATOR_BBOX_WIDTH - 10, (int)((float)TileBitmap.Height / ((float)TileBitmap.Width / (PGPRC_SEPARATOR_BBOX_WIDTHF - 10.F)) + .5F));
		 else
			fprintf(cf.m_pStream, "5 51 translate %d %d scale\r\n", 	  
			   PGPRC_SEPARATOR_BBOX_WIDTH - 10, (int)((float)TileBitmap.Height / ((float)TileBitmap.Width / (PGPRC_SEPARATOR_BBOX_WIDTHF - 10.F)) + .5F));
		 fprintf(cf.m_pStream, "%d %d 8 [%d 0 0 -%d 0 %d]\r\n", 
			TileBitmap.Width, TileBitmap.Height, TileBitmap.Width, 
			TileBitmap.Height, TileBitmap.Height);
		 fprintf(cf.m_pStream, "{currentfile picstr readstring pop} false 3 colorimage\r\n");
		 
		 // lock the tile bitmap
		 L_AccessBitmap(&TileBitmap);

		 // write the data
		 if (pData = new BYTE[TileBitmap.Width * 3])
			{
			for (i = 0; i < TileBitmap.Height; i++)
			   {
			   L_GetBitmapRow(&TileBitmap, pData, i, TileBitmap.Width * 3);
			   cf.Write(pData, TileBitmap.Width * 3);
			   }
			cf.Write("\r\nGR\r\n", 6);
			delete pData;
			}

		 // release the bitmap
		 L_ReleaseBitmap(&TileBitmap);
		 }

	  // now do the left logo on the first page
	  if (pJob->JobSeparator.bDoLeftLogo && iPage == 0)
		 {
		 // best fit
		 if (((float)LeftLogoBitmap.Height / (float)LeftLogoBitmap.Width) < (82./300.F))
			{
			iNewWidth = 300;
			iNewHeight = (int)((float)LeftLogoBitmap.Height / ((float)LeftLogoBitmap.Width / 300.F));
			}
		 else
			{
			iNewWidth = (int)((float)LeftLogoBitmap.Width / (float)LeftLogoBitmap.Height * 82.F + .5F);
			iNewHeight = 82;
			}		   
		 // write the bitmap to the eps file
		 fprintf(cf.m_pStream, "GS /picstr %d string def\r\n", 
			LeftLogoBitmap.Width * 3);
		 fprintf(cf.m_pStream, "0 195 translate %d %d scale\r\n", iNewWidth, iNewHeight);	  
		 fprintf(cf.m_pStream, "%d %d 8 [%d 0 0 -%d 0 %d]\r\n", 
			LeftLogoBitmap.Width, LeftLogoBitmap.Height, LeftLogoBitmap.Width, 
			LeftLogoBitmap.Height, LeftLogoBitmap.Height);
		 fprintf(cf.m_pStream, "{currentfile picstr readstring pop} false 3 colorimage\r\n");
		 
		 // lock the tile bitmap
		 L_AccessBitmap(&LeftLogoBitmap);

		 // write the data
		 if (pData = new BYTE[LeftLogoBitmap.Width * 3])
			{
			// should we flip?
			if (LeftLogoBitmap.ViewPerspective == BOTTOM_LEFT)
			   {
			   for (i = LeftLogoBitmap.Height - 1; i >= 0; i--)
				  {
			      L_GetBitmapRow(&LeftLogoBitmap, pData, i, LeftLogoBitmap.Width * 3);
			      cf.Write(pData, LeftLogoBitmap.Width * 3);
				  }
			   }
			else
			   {
			   for (i = 0; i < LeftLogoBitmap.Height; i++)
				  {
			      L_GetBitmapRow(&LeftLogoBitmap, pData, i, LeftLogoBitmap.Width * 3);
			      cf.Write(pData, LeftLogoBitmap.Width * 3);
				  }
			   }
			cf.Write("\r\nGR\r\n", 6);
			delete pData;
			}

		 // release the bitmap
		 L_ReleaseBitmap(&LeftLogoBitmap);
		 }
	  // now do the right logo on the first page
	  if (pJob->JobSeparator.bDoRightLogo && iPage == 0)
		 {
		 // best fit
		 if (((float)RightLogoBitmap.Height / (float)RightLogoBitmap.Width) < (82./300.F))
			{
			iNewWidth = 300;
			iNewHeight = (int)((float)RightLogoBitmap.Height / ((float)RightLogoBitmap.Width / 300.F));
			}
		 else
			{
			iNewWidth = (int)((float)RightLogoBitmap.Width / (float)RightLogoBitmap.Height * 82.F + .5F);
			iNewHeight = 82;
			}		   

		 // write the bitmap to the eps file
		 fprintf(cf.m_pStream, "GS /picstr %d string def\r\n", 
			RightLogoBitmap.Width * 3);
		 fprintf(cf.m_pStream, "300 195 translate %d %d scale\r\n", iNewWidth, iNewHeight);
		 fprintf(cf.m_pStream, "%d %d 8 [%d 0 0 -%d 0 %d]\r\n", 
			RightLogoBitmap.Width, RightLogoBitmap.Height, RightLogoBitmap.Width, 
			RightLogoBitmap.Height, RightLogoBitmap.Height);
		 fprintf(cf.m_pStream, "{currentfile picstr readstring pop} false 3 colorimage\r\n");
		 
		 // lock the tile bitmap
		 L_AccessBitmap(&RightLogoBitmap);

		 // write the data
		 if (pData = new BYTE[RightLogoBitmap.Width * 3])
			{
			// should we flip?
			if (RightLogoBitmap.ViewPerspective == BOTTOM_LEFT)
			   {
			   for (i = RightLogoBitmap.Height - 1; i >= 0; i--)
				  {
			      L_GetBitmapRow(&RightLogoBitmap, pData, i, RightLogoBitmap.Width * 3);
			      cf.Write(pData, RightLogoBitmap.Width * 3);
				  }
			   }
			else
			   {
			   for (i = 0; i < RightLogoBitmap.Height; i++)
				  {
			      L_GetBitmapRow(&RightLogoBitmap, pData, i, RightLogoBitmap.Width * 3);
			      cf.Write(pData, RightLogoBitmap.Width * 3);
				  }
			   }
			cf.Write("\r\nGR\r\n", 6);
			delete pData;
			}

		 // release the bitmap
		 L_ReleaseBitmap(&RightLogoBitmap);
		 }
	  // now do the barcode on the first page
	  // remember that this is a 
	  if (pJob->JobSeparator.bDoBarCode && iPage == 0)	  
		 {
		 // write the bitmap to the eps file
		 fprintf(cf.m_pStream, "GS /picstr %d string def\r\n", 
			BarCodeBitmap.BytesPerLine);		 		 
		 fprintf(cf.m_pStream, "375 10 translate %d %d scale\r\n", 	  
			200, (int)((float)BarCodeBitmap.Height / ((float)BarCodeBitmap.Width / 200.F) + .5F));
		 fprintf(cf.m_pStream, "%d %d 1 [%d 0 0 -%d 0 %d]\r\n", 
			BarCodeBitmap.Width, BarCodeBitmap.Height, BarCodeBitmap.Width, 
			BarCodeBitmap.Height, BarCodeBitmap.Height);
		 fprintf(cf.m_pStream, "{currentfile picstr readstring pop} false 1 colorimage\r\n");
		 
		 // lock the tile bitmap
		 L_AccessBitmap(&BarCodeBitmap);

		 // write the data
		 if (pData = new BYTE[BarCodeBitmap.BytesPerLine])
			{
			for (i = 0; i < BarCodeBitmap.Height; i++)
			   {
			   L_GetBitmapRow(&BarCodeBitmap, pData, i, BarCodeBitmap.BytesPerLine);
			   cf.Write(pData, BarCodeBitmap.BytesPerLine);
			   }		  			   
			cf.Write("\r\nGR\r\n", 6);
			delete pData;
			}			  
		 // release the bitmap
		 L_ReleaseBitmap(&BarCodeBitmap);
		 }				  

	  // write the end
      fprintf(cf.m_pStream, "gsave showpage grestore\r\n");
		 		 
	  cf.Close();
	  bRet = TRUE;
	  }			  
   // free the tiling bitmap
   L_FreeBitmap(&TileBitmap);
   // free the logo bitmaps
   if (pJob->JobSeparator.bDoLeftLogo && iPage == 0)
	  L_FreeBitmap(&LeftLogoBitmap);
   if (pJob->JobSeparator.bDoRightLogo && iPage == 0)
	  L_FreeBitmap(&RightLogoBitmap);
   if (pJob->JobSeparator.bDoBarCode && iPage == 0)
  	  L_FreeBitmap(&BarCodeBitmap);

   return bRet;
   } 

BOOL CPGPRCServer::CreateJobSeparatorFiles_1(CSocketJob* pJob, int iStart, int iEnd, int iPage)
   {   
   CString csTemp;
   CString csTempPath;			 
   char pTempFileName[_MAX_PATH];
   CStdioFile cf;
   int i;
   POSITION pos;
   POURPGPRCPHOTOSPECS pPhoto;   
   char* lpFileName;
   BITMAPHANDLE TileBitmap;
   BITMAPHANDLE ImageBitmap;   
   BITMAPHANDLE LeftLogoBitmap;
   BITMAPHANDLE RightLogoBitmap;
   BITMAPHANDLE BarCodeBitmap;
   int NewWidth, NewHeight;
   int xdest, ydest;
   int xidx, yidx;
   float fARi, fARt;
   BOOL bFlip;   
   L_INT lRet;
   BYTE* pData;
   UINT uiOpenFlags;
   BOOL bRet = FALSE;    
   CString csTempBarCodeFileName;
   CString cs;
   int iNewWidth;
   int iNewHeight;
   CProfile Profile;

   // create a new bitmap for tiling the contact prints
   NewWidth = PGPRC_CONTACTIMAGE_WIDTH * PGPRC_CONTACTIMAGE_NUMACCROSS +
	  (PGPRC_CONTACTIMAGE_NUMACCROSS - 1) * PGPRC_CONTACTIMAGE_SPACINGX + PGPRC_CONTACTIMAGE_SPACINGX;

   if (iPage == 0)
      NewHeight = PGPRC_CONTACTIMAGE_HEIGHT * PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_1 +
		         (PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_1 - 1) * PGPRC_CONTACTIMAGE_SPACINGY;	  
   else
      NewHeight = PGPRC_CONTACTIMAGE_HEIGHT * PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_N +
		         (PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_N - 1) * PGPRC_CONTACTIMAGE_SPACINGY;	  

   if (L_CreateBitmap(&TileBitmap, sizeof(BITMAPHANDLE), TYPE_CONV, NewWidth, NewHeight, 24, ORDER_RGB,
						  NULL, TOP_LEFT, NULL, 0) != SUCCESS)
	  return FALSE;

   L_FillBitmap(&TileBitmap, RGB(255, 255, 255));

   // load the logo file, don't totally fail if it's not there   
   if (pJob->JobSeparator.bDoLeftLogo && iPage == 0)
	  {
	  // if the logo file name is empty, use the one set in options
	  if (strlen(pJob->JobSeparator.lpszLeftLogoFile) < 1)
		 {
		 csTemp = Profile.GetStr(IDS_PHOTOGIZE, "LeftLogo", "");		 
		 if (csTemp.GetLength() > 0)
		    strcpy(pJob->JobSeparator.lpszLeftLogoFile, csTemp);
		 }
	  if (L_LoadBitmap(pJob->JobSeparator.lpszLeftLogoFile, 
						&LeftLogoBitmap, sizeof(BITMAPHANDLE), 0, ORDER_RGB, NULL, NULL) != SUCCESS)
		 pJob->JobSeparator.bDoLeftLogo = FALSE;	 	
	  else
		 {
		 if (LeftLogoBitmap.BitsPerPixel != 24)
			{
			L_FreeBitmap(&LeftLogoBitmap);
			pJob->JobSeparator.bDoLeftLogo = FALSE;
			}
		 }
	  
	  }	 	  

   if (pJob->JobSeparator.bDoRightLogo && iPage == 0)
	  {
	  // if the logo file name is empty, use the one set in options
	  if (strlen(pJob->JobSeparator.lpszRightLogoFile) < 1)
		 {
		 csTemp = Profile.GetStr(IDS_PHOTOGIZE, "RightLogo", "");		 
		 if (csTemp.GetLength() > 0)
		    strcpy(pJob->JobSeparator.lpszRightLogoFile, csTemp);
		 }											   
	  if (L_LoadBitmap(pJob->JobSeparator.lpszRightLogoFile, 
						&RightLogoBitmap, sizeof(BITMAPHANDLE), 0, ORDER_RGB, NULL, NULL) != SUCCESS)
		 pJob->JobSeparator.bDoRightLogo = FALSE;	 	
	  else
		 {
		 if (RightLogoBitmap.BitsPerPixel != 24)
			{
			L_FreeBitmap(&RightLogoBitmap);
			pJob->JobSeparator.bDoRightLogo = FALSE;
			}
		 }	  
	  }	 	  

   /// do we want a barcode?
   if (pJob->JobSeparator.bDoBarCode && 
	  strlen(pJob->PGPRCPrintSpecs.OrderIDString) > 0)
	  {
	  bRet = DoBarCode(pJob, csTempBarCodeFileName);
	  if (!bRet)		 
		 pJob->JobSeparator.bDoBarCode = FALSE;		 
	  else
		 {
	     if (L_LoadBitmap((char*)(const char*)csTempBarCodeFileName, 
							&BarCodeBitmap, sizeof(BITMAPHANDLE), 0, ORDER_RGB, NULL, NULL) != SUCCESS)
		    pJob->JobSeparator.bDoBarCode = FALSE;		 
		 // and blow it away - we don't need it anymore
		 ::DeleteFile(csTempBarCodeFileName);
		 }
	  }
   else
	  pJob->JobSeparator.bDoBarCode = FALSE;

   if (pJob->JobSeparator.bDoThumbnails)
	  {	  
	  // impose index prints into the Tile Bitmap
	  xidx = 0; yidx = 0;
	  xdest = 0; ydest = 0;
	  int idx;
	  //for (pos = pJob->Photos.GetHeadPosition(), idx = 0; pos != NULL; idx++)
	  for (pos = pJob->Photos.GetHeadPosition(), idx = 0; pos != NULL;)
		 {	  
		 // get the photo
		 pPhoto = (POURPGPRCPHOTOSPECS)pJob->Photos.GetNext(pos);		 
		 // check bounds
		 //if (idx < iStart || idx > iEnd) continue;

		 if (!pPhoto->bPutInThumbnails) continue;

		 if (idx < iStart || idx > (iEnd - 1)) 
			{
			idx++;
			continue;
			}

		 idx++;

		 // which file to use?
		 if (pPhoto->bUseTempFile)
			lpFileName = pPhoto->lpszTempDiskFile;
		 else
			lpFileName = pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile;

		 // resize the images
		 if (L_LoadBitmap(lpFileName, &ImageBitmap, sizeof(BITMAPHANDLE), 0, ORDER_RGB, NULL, NULL) == SUCCESS)
			{
			// get aspect ratios
			fARi = (float)ImageBitmap.Height / (float)ImageBitmap.Width;
			fARt = (float)(PGPRC_CONTACTIMAGE_HEIGHT) / (float)(PGPRC_CONTACTIMAGE_WIDTH);		 

			// do we need to flip the image?
			if (fARi > 1.0)
			   {
			   fARi = 1.0 / fARi;
			   bFlip = TRUE;
			   }
			else			
			   bFlip = FALSE;			

			// get new height and width
			if (fARi < fARt)
			   {
			   NewWidth = PGPRC_CONTACTIMAGE_WIDTH;
			   NewHeight = (int)((float)(PGPRC_CONTACTIMAGE_WIDTH) * fARi + .5F);
			   }
			else
			   {
			   NewWidth = (int)((float)(PGPRC_CONTACTIMAGE_HEIGHT) / fARi + .5F);
			   NewHeight = PGPRC_CONTACTIMAGE_HEIGHT;					
			   }
			// resize it
			if (bFlip)
			   lRet = L_SizeBitmap(&ImageBitmap, NewHeight, NewWidth, SIZE_BICUBIC);
			else
			   lRet = L_SizeBitmap(&ImageBitmap, NewWidth, NewHeight, SIZE_BICUBIC);

			if (lRet != SUCCESS)
			   {
			   L_FreeBitmap(&ImageBitmap);
			   L_FreeBitmap(&TileBitmap);
			   if (pJob->JobSeparator.bDoLeftLogo && iPage == 0)
				  L_FreeBitmap(&LeftLogoBitmap);
			   if (pJob->JobSeparator.bDoRightLogo && iPage == 0)
				  L_FreeBitmap(&RightLogoBitmap);
			   if (pJob->JobSeparator.bDoBarCode)
				  L_FreeBitmap(&BarCodeBitmap);
			   return FALSE;
			   }
			// flip it?
			if (bFlip)
			   {
			   // rotate it
			   if (L_RotateBitmap(&ImageBitmap, 9000, ROTATE_RESIZE,  RGB(255, 255, 255)) != SUCCESS)
				  {
				  L_FreeBitmap(&ImageBitmap);
				  L_FreeBitmap(&TileBitmap);
				  if (pJob->JobSeparator.bDoLeftLogo && iPage == 0)
					 L_FreeBitmap(&LeftLogoBitmap);
				  if (pJob->JobSeparator.bDoRightLogo && iPage == 0)
					 L_FreeBitmap(&RightLogoBitmap);
			      if (pJob->JobSeparator.bDoBarCode)
				     L_FreeBitmap(&BarCodeBitmap);
				  return FALSE;
				  }			
			   }			
			// get the index of the index
			xidx %= PGPRC_CONTACTIMAGE_NUMACCROSS;
			if (iPage == 0)
			   yidx %= PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_1;
			else
			   yidx %= PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_N;

			xdest = xidx * PGPRC_CONTACTIMAGE_WIDTH + xidx * PGPRC_CONTACTIMAGE_SPACINGX + PGPRC_CONTACTIMAGE_SPACINGX;
			ydest = yidx * PGPRC_CONTACTIMAGE_HEIGHT + yidx * PGPRC_CONTACTIMAGE_SPACINGY;

			// offsets?
			xdest += (PGPRC_CONTACTIMAGE_WIDTH - ImageBitmap.Width) / 2;
			ydest += (PGPRC_CONTACTIMAGE_HEIGHT - ImageBitmap.Height) / 2;

			// now impose it into the tile bitmap		 
			if (L_CombineBitmap(&TileBitmap, xdest, ydest, ImageBitmap.Width, ImageBitmap.Height,
			   &ImageBitmap, 0, 0, CB_OP_ADD | CB_DST_0) != SUCCESS)
			   {
			   L_FreeBitmap(&ImageBitmap);
			   L_FreeBitmap(&TileBitmap);
			   if (pJob->JobSeparator.bDoLeftLogo && iPage == 0)
				  L_FreeBitmap(&LeftLogoBitmap);
			   if (pJob->JobSeparator.bDoRightLogo && iPage == 0)
				  L_FreeBitmap(&RightLogoBitmap);
			   if (pJob->JobSeparator.bDoBarCode)
				  L_FreeBitmap(&BarCodeBitmap);
			   return FALSE;
			   }
			xidx++;

			// carriage return
			if (xidx == PGPRC_CONTACTIMAGE_NUMACCROSS)
			   {
			   xidx = 0;
			   yidx++;
			   }
			
			// Free the temporary bitmap
			L_FreeBitmap(&ImageBitmap);
			}
		 }
      }

   // resolve the device to get the temp path
   CZeusDoc* pDoc;
   if (!(pDoc = (CZeusDoc*)(pJob->hDevice)))
	  return pgprc_error_invalid_job_handle;
   csTempPath = pDoc->crrrs.csScratchPath;

   // make a temp file		 
   GetTempFileName(csTempPath, "pgprc", 0, pTempFileName);

   // save the name
   pJob->csJobSeparatorFile = pTempFileName;		 

   uiOpenFlags = CFile::modeCreate | CFile::modeWrite | CFile::typeBinary;

   // create it
   if (cf.Open(pJob->csJobSeparatorFile, uiOpenFlags))
	  {
	  if (iPage == 0)
		 {
		 // do the separator stuff
         DoSeparatorText(pJob, cf);
		 }
	  else
		 {
		 cf.SeekToEnd(); // go to the end
	     // write the prolog	     
		 WritePSStartN(cf, PGPRC_SEPARATOR_BBOX_WIDTH, PGPRC_SEPARATOR_BBOX_HEIGHT);
		 }	
      // write the bitmap to the eps file
      fprintf(cf.m_pStream, "GS /picstr %d string def\r\n", 
		 TileBitmap.Width * 3);

	  if (pJob->JobSeparator.bDoThumbnails)
		 {	  
		 if (iPage == 0)
			fprintf(cf.m_pStream, "0 260 translate %d %d scale\r\n", 	  
			   PGPRC_SEPARATOR_BBOX_WIDTH - 50, (int)((float)TileBitmap.Height / 
			   ((float)TileBitmap.Width / (PGPRC_SEPARATOR_BBOX_WIDTHF - 50.F)) + .5F));
		 else
			fprintf(cf.m_pStream, "0 51 translate %d %d scale\r\n", 	  
			   PGPRC_SEPARATOR_BBOX_WIDTH - 50, (int)((float)TileBitmap.Height / 
			   ((float)TileBitmap.Width / (PGPRC_SEPARATOR_BBOX_WIDTHF - 50.F)) + .5F));
		 fprintf(cf.m_pStream, "%d %d 8 [%d 0 0 -%d 0 %d]\r\n", 
			TileBitmap.Width, TileBitmap.Height, TileBitmap.Width, 
			TileBitmap.Height, TileBitmap.Height);
		 fprintf(cf.m_pStream, "{currentfile picstr readstring pop} false 3 colorimage\r\n");
		 
		 // lock the tile bitmap
		 L_AccessBitmap(&TileBitmap);

		 // write the data
		 if (pData = new BYTE[TileBitmap.Width * 3])
			{
			for (i = 0; i < TileBitmap.Height; i++)
			   {
			   L_GetBitmapRow(&TileBitmap, pData, i, TileBitmap.Width * 3);
			   cf.Write(pData, TileBitmap.Width * 3);
			   }
			cf.Write("\r\nGR\r\n", 6);
			delete pData;
			}

		 // release the bitmap
		 L_ReleaseBitmap(&TileBitmap);

		 // tag each photo
		 TagPhotos(pJob, cf, iStart, iEnd, iPage); 
		 }

	  // now do the left logo on the first page
	  if (pJob->JobSeparator.bDoLeftLogo && iPage == 0)
		 {
		 // best fit
		 if (((float)LeftLogoBitmap.Height / (float)LeftLogoBitmap.Width) < (36./350.F))
			{
			iNewWidth = 350;
			iNewHeight = (int)((float)LeftLogoBitmap.Height / 
			   ((float)LeftLogoBitmap.Width / 350.F));
			}
		 else
			{
			iNewWidth = (int)((float)LeftLogoBitmap.Width / 
			   (float)LeftLogoBitmap.Height * 36.F + .5F);
			iNewHeight = 36;
			}		   
		 // write the bitmap to the eps file
		 fprintf(cf.m_pStream, "GS /picstr %d string def\r\n", 
			LeftLogoBitmap.Width * 3);
		 fprintf(cf.m_pStream, "0 185 translate %d %d scale\r\n", iNewWidth, iNewHeight);	  
		 fprintf(cf.m_pStream, "%d %d 8 [%d 0 0 -%d 0 %d]\r\n", 
			LeftLogoBitmap.Width, LeftLogoBitmap.Height, LeftLogoBitmap.Width, 
			LeftLogoBitmap.Height, LeftLogoBitmap.Height);
		 fprintf(cf.m_pStream, "{currentfile picstr readstring pop} false 3 colorimage\r\n");
		 
		 // lock the tile bitmap
		 L_AccessBitmap(&LeftLogoBitmap);

		 // write the data
		 if (pData = new BYTE[LeftLogoBitmap.Width * 3])
			{
			// should we flip?
			if (LeftLogoBitmap.ViewPerspective == BOTTOM_LEFT)
			   {
			   for (i = LeftLogoBitmap.Height - 1; i >= 0; i--)
				  {
			      L_GetBitmapRow(&LeftLogoBitmap, pData, i, LeftLogoBitmap.Width * 3);
			      cf.Write(pData, LeftLogoBitmap.Width * 3);
				  }
			   }
			else
			   {
			   for (i = 0; i < LeftLogoBitmap.Height; i++)
				  {
			      L_GetBitmapRow(&LeftLogoBitmap, pData, i, LeftLogoBitmap.Width * 3);
			      cf.Write(pData, LeftLogoBitmap.Width * 3);
				  }
			   }
			cf.Write("\r\nGR\r\n", 6);
			delete pData;
			}

		 // release the bitmap
		 L_ReleaseBitmap(&LeftLogoBitmap);
		 }
	  // now do the right logo on the first page
	  if (pJob->JobSeparator.bDoRightLogo && iPage == 0)
		 {
		 // best fit
		 if (((float)RightLogoBitmap.Height / (float)RightLogoBitmap.Width) < (36./350.F))
			{
			iNewWidth = 350;
			iNewHeight = (int)((float)RightLogoBitmap.Height / ((float)RightLogoBitmap.Width / 350.F));
			}
		 else
			{
			iNewWidth = (int)((float)RightLogoBitmap.Width / (float)RightLogoBitmap.Height * 36.F + .5F);
			iNewHeight = 36;
			}		   

		 // write the bitmap to the eps file
		 fprintf(cf.m_pStream, "GS /picstr %d string def\r\n", 
			RightLogoBitmap.Width * 3);
		 fprintf(cf.m_pStream, "350 185 translate %d %d scale\r\n", iNewWidth, iNewHeight);
		 fprintf(cf.m_pStream, "%d %d 8 [%d 0 0 -%d 0 %d]\r\n", 
			RightLogoBitmap.Width, RightLogoBitmap.Height, RightLogoBitmap.Width, 
			RightLogoBitmap.Height, RightLogoBitmap.Height);
		 fprintf(cf.m_pStream, "{currentfile picstr readstring pop} false 3 colorimage\r\n");
		 
		 // lock the tile bitmap
		 L_AccessBitmap(&RightLogoBitmap);

		 // write the data
		 if (pData = new BYTE[RightLogoBitmap.Width * 3])
			{
			// should we flip?
			if (RightLogoBitmap.ViewPerspective == BOTTOM_LEFT)
			   {
			   for (i = RightLogoBitmap.Height - 1; i >= 0; i--)
				  {
			      L_GetBitmapRow(&RightLogoBitmap, pData, i, RightLogoBitmap.Width * 3);
			      cf.Write(pData, RightLogoBitmap.Width * 3);
				  }
			   }
			else
			   {
			   for (i = 0; i < RightLogoBitmap.Height; i++)
				  {
			      L_GetBitmapRow(&RightLogoBitmap, pData, i, RightLogoBitmap.Width * 3);
			      cf.Write(pData, RightLogoBitmap.Width * 3);
				  }
			   }
			cf.Write("\r\nGR\r\n", 6);
			delete pData;
			}

		 // release the bitmap
		 L_ReleaseBitmap(&RightLogoBitmap);
		 }
	  // now do the barcode on the first page
	  // remember that this is a 
	  if (pJob->JobSeparator.bDoBarCode)	  
		 {
		 // always do the top barcode
		 // write the bitmap to the eps file
		 fprintf(cf.m_pStream, "GS /picstr %d string def\r\n", 
			BarCodeBitmap.BytesPerLine);		 		 		 
		 
		 //fprintf(cf.m_pStream, "25 450 translate %d %d scale\r\n", 	  
		 fprintf(cf.m_pStream, "200 460 translate %d %d scale\r\n", 	  
//			96, (int)((float)BarCodeBitmap.Height / ((float)BarCodeBitmap.Width / 96.F) + .5F));
			300, 25);
		 fprintf(cf.m_pStream, "%d %d 1 [%d 0 0 -%d 0 %d]\r\n", 
			BarCodeBitmap.Width, BarCodeBitmap.Height, BarCodeBitmap.Width, 
			BarCodeBitmap.Height, BarCodeBitmap.Height);
		 fprintf(cf.m_pStream, "{currentfile picstr readstring pop} false 1 colorimage\r\n");
		 
		 // lock the tile bitmap
		 L_AccessBitmap(&BarCodeBitmap);

		 // write the data
		 if (pData = new BYTE[BarCodeBitmap.BytesPerLine])
			{
			for (i = 0; i < BarCodeBitmap.Height; i++)
			   {
			   L_GetBitmapRow(&BarCodeBitmap, pData, i, BarCodeBitmap.BytesPerLine);
			   cf.Write(pData, BarCodeBitmap.BytesPerLine);
			   }		  			   
			cf.Write("\r\nGR\r\n", 6);
			delete pData;
			}			  
		 // release the bitmap
		 L_ReleaseBitmap(&BarCodeBitmap);
#if 0
		 // only do this one on page 0
		 if (iPage == 0)
			{
			// write the bitmap to the eps file
			fprintf(cf.m_pStream, "GS /picstr %d string def\r\n", 
			   BarCodeBitmap.BytesPerLine);		 		 
			
			// the bottom barcode
			//fprintf(cf.m_pStream, "420 63 translate %d %d scale\r\n", 	  
			fprintf(cf.m_pStream, "340 55 translate %d %d scale\r\n", 	  
			   96, (int)((float)BarCodeBitmap.Height / ((float)BarCodeBitmap.Width / 96.F) + .5F));
			fprintf(cf.m_pStream, "%d %d 1 [%d 0 0 -%d 0 %d]\r\n", 
			   BarCodeBitmap.Width, BarCodeBitmap.Height, BarCodeBitmap.Width, 
			   BarCodeBitmap.Height, BarCodeBitmap.Height);
			fprintf(cf.m_pStream, "{currentfile picstr readstring pop} false 1 colorimage\r\n");
			
			// lock the tile bitmap
			L_AccessBitmap(&BarCodeBitmap);

			// write the data
			if (pData = new BYTE[BarCodeBitmap.BytesPerLine])
			   {
			   for (i = 0; i < BarCodeBitmap.Height; i++)
				  {
				  L_GetBitmapRow(&BarCodeBitmap, pData, i, BarCodeBitmap.BytesPerLine);
				  cf.Write(pData, BarCodeBitmap.BytesPerLine);
				  }		  			   
			   cf.Write("\r\nGR\r\n", 6);
			   delete pData;
			   }			  
			// release the bitmap
			L_ReleaseBitmap(&BarCodeBitmap); 			   
			}
#endif
		 }				  

	  // write the index page, etc..	  
	  fprintf(cf.m_pStream, "/Helvetica-Bold findfont [14 0 0 14 0 0] makefont setfont\r\n");
      fprintf(cf.m_pStream, "596 475 M\r\n");
	  GetTotalSeparatorPages(pJob);
	  cs.Format("Index %d of %d", iPage + 1, pJob->iNumSeparatorPages);
	  fprintf(cf.m_pStream, "(%s) S\r\n", CStringToPSString(cs));
	  // write time
	  if (pJob->JobSeparator.TimeStamp[0] == 0)
		 {
	     CTime t = CTime::GetCurrentTime();
	     cs = t.Format( "%m/%d/%y" );
		 }
	  else
		 cs = pJob->JobSeparator.TimeStamp;
	  fprintf(cf.m_pStream, "596 460 M\r\n");
	  fprintf(cf.m_pStream, "(%s) S\r\n", CStringToPSString(cs));

	  // write order number
	  fprintf(cf.m_pStream, "/Helvetica findfont [14 0 0 14 0 0] makefont setfont\r\n");
	  fprintf(cf.m_pStream, "25 475 M\r\n");
	  fprintf(cf.m_pStream, "(Order #%s) S\r\n", pJob->PGPRCPrintSpecs.OrderIDString);	  
	  fprintf(cf.m_pStream, "25 460 M\r\n");
	  fprintf(cf.m_pStream, "(Store ID: %s) S\r\n", pJob->JobSeparator.lpszStoreID);	  

	  // write the end
      fprintf(cf.m_pStream, "gsave showpage grestore\r\n");
		 		 
	  cf.Close();
	  bRet = TRUE;
	  }			  
   // free the tiling bitmap
   L_FreeBitmap(&TileBitmap);
   // free the logo bitmaps
   if (pJob->JobSeparator.bDoLeftLogo && iPage == 0)
	  L_FreeBitmap(&LeftLogoBitmap);
   if (pJob->JobSeparator.bDoRightLogo && iPage == 0)
	  L_FreeBitmap(&RightLogoBitmap);
   if (pJob->JobSeparator.bDoBarCode)
  	  L_FreeBitmap(&BarCodeBitmap);

   return bRet;
   }

BOOL CPGPRCServer::DoBarCode(CSocketJob* pJob, CString& csTempBarCodeFileName)
   {
   ERRCODE eCode = S_OK;
   t_BarCode* m_pBarCode;
   CString csTempPath;
   char pTempFileName[_MAX_PATH];      
   int iHeight;

   switch(pJob->JobSeparator.iStyle)
	  {
	  case PGPRC_JOBSEPARATOR_STYLE_1:		              
		   //iHeight = 208;
		   //iHeight = 250;
		   iHeight = 384;
		   break;
	  default:
	  case PGPRC_JOBSEPARATOR_STYLE_0:             
		   iHeight = 384;
		   break;
	  }	      

   // resolve the device to get the temp path
   CZeusDoc* pDoc;
   if (!(pDoc = (CZeusDoc*)(pJob->hDevice)))
	  return pgprc_error_invalid_job_handle;
   csTempPath = pDoc->crrrs.csScratchPath;

   // make a temp file
   GetTempFileName(csTempPath, "pgprc", 0, pTempFileName);

   // do the license
   BCLicenseMe ("Graphx, Woburn", (e_licKind)3,
	  1, "D1F438BA", (e_licProduct)5);		
   // Allocate and initialize bar code structure
   if (BCAlloc (&m_pBarCode) == S_OK)
	  {
	  // set the text height
      LOGFONT* plfFont;
	  plfFont = BCGetLogFont (m_pBarCode);
	  plfFont->lfHeight = -50;
	  BCSetLogFont (m_pBarCode, plfFont);	

	  // set the bar code text
	  eCode = (eCode == S_OK) ? BCSetText(m_pBarCode, pJob->PGPRCPrintSpecs.OrderIDString, 
		 strlen(pJob->PGPRCPrintSpecs.OrderIDString)) : eCode;
	  // set the type to code 39 ascii
	  eCode = (eCode == S_OK) ? BCSetBCType(m_pBarCode, 
		 (tag_BarCType)pJob->JobSeparator.pgprcBarCodeType) : eCode;
	  // check digit method
	  eCode = (eCode == S_OK) ? BCSetCDMethod(m_pBarCode, eCDStandard) : eCode;
	  eCode = (eCode == S_OK) ? BCSetPrintText(m_pBarCode, pJob->JobSeparator.bTextUnderBarCode, 
		 FALSE) : eCode;
      eCode = (eCode == S_OK) ? BCSetRotation(m_pBarCode, deg0) : eCode;

	  // check it
	  eCode = (eCode == S_OK) ? BCCheck	(m_pBarCode) : eCode;
	  // check the cd type
	  eCode = (eCode == S_OK) ? BCCalcCD	(m_pBarCode) : eCode;
	  
	  // create it
	  eCode = (eCode == S_OK) ? BCCreate	(m_pBarCode) : eCode;
	  // save it
      eCode = (eCode == S_OK) ? BCSaveImage(m_pBarCode, pTempFileName, 
		 eIMTif, 1536, iHeight, 128, 128) : eCode;

	  BCFree (m_pBarCode);
	  }
   // error check
   if (eCode != S_OK)	  
	  return FALSE;	  
   else
	  {
	  csTempBarCodeFileName = pTempFileName;
	  return TRUE;
	  }
   }

CString CPGPRCServer::CStringToPSString(LPCSTR lpStrIn)
   {
   int i;
   CString cs = "";
   char c;   

   for (i = 0; i < strlen(lpStrIn); i++)
	  {
	  c = lpStrIn[i];

	  switch(c)
		 {
		 case '\\':
		      cs += "\\\\";			  
			  break;
		 case '(':
			  cs += "\\(";
			  break;
		 case ')':
			  cs += "\\)";
			  break;
		 default:
			  cs += c;
			  break;
		 }
	  }
   return cs;
   }

void CPGPRCServer::GetTotalSeparatorPages(CSocketJob* pJob)
   {
   int iPhotosAfterFirstPage;
   
   pJob->iNumSeparatorPages = 0;

   if (pJob->iPhotoCounter <= PGPRC_PERPAGE_TOTALPHOTOS_PAGE_1)
	  pJob->iNumSeparatorPages = 1;
   else
	  {
	  // get the number of pages after the first page
	  //iPhotosAfterFirstPage = pJob->iPhotoCounter - PGPRC_PERPAGE_TOTALPHOTOS_PAGE_1;		 
	  iPhotosAfterFirstPage = GetRealPhotoCount(pJob) - PGPRC_PERPAGE_TOTALPHOTOS_PAGE_1;		 	  

	  // divide the photos left by the number per page
	  pJob->iNumSeparatorPages = iPhotosAfterFirstPage / PGPRC_PERPAGE_TOTALPHOTOS_PAGE_N + 1;

	  // look for a remainder
	  if (iPhotosAfterFirstPage % PGPRC_PERPAGE_TOTALPHOTOS_PAGE_N)
		 pJob->iNumSeparatorPages++;
	  }   
   }

PGPRC_RETURNVAL CPGPRCServer::CreateQueueFiles(CSocketJob* pJob)
   {
   POSITION pos;
   POURPGPRCPHOTOSPECS pPhoto;
   PGPRC_RETURNVAL gRetVal = pgprc_returnok;
   CQueueFile* pCQueueFile;
   char* lpFileName;
   int iTotalPages = 0;
	  
   for (pos = pJob->Photos.GetHeadPosition(); pos != NULL;)
	  {	  	  
	  pPhoto = (POURPGPRCPHOTOSPECS)pJob->Photos.GetNext(pos);

	  // a single photo on a new page
	  if (pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcSizing != pgprc_montage_crop &&
		  pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcSizing != pgprc_montage_fit)
		 {							 
		 // instantiate a queuefile 
		 if ((pCQueueFile = new CQueueFile) == NULL)						
			{
			gRetVal = pgprc_error_outofmemory;		 			
			break;
			}

		 // save the pointer
		 pPhoto->hCqueueFile = pCQueueFile;   

		 // and the job id
		 pPhoto->iJobID = pCQueueFile->iJobID;

		 // init the queue file
		 if (pPhoto->bUseTempFile)
			lpFileName = pPhoto->lpszTempDiskFile;
		 else
			lpFileName = pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile;

		 if (!pCQueueFile->Init(lpFileName, iTotalPages, 0))
			{
			delete pCQueueFile;
			pPhoto->hCqueueFile = NULL;
			pCQueueFile = NULL;
			gRetVal = pgprc_cant_find_file;		 
			break;
			}
									   
		 // set this flag
		 pCQueueFile->ExtraInfo = CQUEUE_EXTRA_PGPRCFILE;

		 // if we're using a temp file, or other flags are set, 
		 // set this flag so that the file is marked for deletion
		 if (pPhoto->bUseTempFile || pPhoto->PhotoSpecs.BasePhotoSpecs.bSendActualData 
			|| pPhoto->PhotoSpecs.BasePhotoSpecs.bDeleteWhenDone)
			pCQueueFile->ExtraInfo |= CQUEUE_EXTRA_SERVERFILE;

		 // check the file type
#if 0
		 if (pCQueueFile->FileType == FTYPE_UNKNOWN || pCQueueFile->FileType == FTYPE_PSD ||
			 pCQueueFile->FileType == FTYPE_PSCRIPT || pCQueueFile->FileType == FTYPE_EPS ||
			 pCQueueFile->FileType == FTYPE_UNSUPPORTED)
#endif
		 if (pCQueueFile->FileType == FTYPE_UNKNOWN || 
			 pCQueueFile->FileType == FTYPE_UNSUPPORTED)
			{
			delete pCQueueFile;			
			pPhoto->hCqueueFile = NULL;
			pCQueueFile = NULL;
			gRetVal = pgprc_error_unsupported_filetype;		 
			break;
			}	  
		 // add a description   
		 pCQueueFile->Description.Format("%s Photogize Photo #%d", pJob->csName, pPhoto->iPhotoCount);   
		 }
	  }

   return gRetVal;
   }

PGPRC_RETURNVAL CPGPRCServer::MontagePhotos(CSocketJob* pJob)
   {
   POSITION pos;
   POURPGPRCPHOTOSPECS pPhoto;
   PGPRC_RETURNVAL gRetVal = pgprc_returnok;
   CPtrArray coPhotosToMontage;
   int iHarvestedPage = 1;
   int iMontageCount;

   do
	  {
	  // first, go through the photo list and pick out all files
	  // on the same page
	  coPhotosToMontage.RemoveAll();	  
	  iMontageCount = 0;
	  for (pos = pJob->Photos.GetHeadPosition(); pos != NULL;)
		 {	  	  				 
		 pPhoto = (POURPGPRCPHOTOSPECS)pJob->Photos.GetNext(pos);
	     
		 // should we?
	     if (pPhoto->bMontageThis)
			{
			// this is the one we want
			if (iHarvestedPage == pPhoto->iPage)
			   { 
			   // add if it is a separator, or if it is a photo AND bPrintJobSeparatorOnly
			   // is not set
			   if (pPhoto->bIsSeparator || 
				   (!pPhoto->bIsSeparator && !pJob->PGPRCPrintSpecs.bPrintJobSeparatorOnly))
				  {
				  coPhotosToMontage.Add(pPhoto);					   
				  pPhoto->bMontageDone = TRUE;
			      iMontageCount++;
				  }
			   }
			}		 
		 }
      // now montage these
      gRetVal = CreateMontagePhoto(pJob, coPhotosToMontage);
	  // break on error
	  if (gRetVal != pgprc_returnok) break;
	  iHarvestedPage++;
	  } while (iMontageCount > 0);

   return gRetVal;
   }

PGPRC_RETURNVAL CPGPRCServer::ComputeMontagePhotosVars(CSocketJob* pJob, int& iPage, 
											   int iMontageCanvasToUse, BOOL bOverrideMisFit)
   {
   POSITION pos;
   POURPGPRCPHOTOSPECS pPhoto;
   PGPRC_RETURNVAL gRetVal = pgprc_returnok;
   CPtrArray coPhotosToMontage;
   PGPRCCANVAS PGPRCCanvas;   
   int iRunningWidthInches1000 = 0;
   int iRunningHeightInches1000 = 0;
   int iMaxSwathHeight = 0;  
   int Save_iWidthInches1000	= 0;
   int Save_iHeightInches1000	= 0;
   
   CZeusDoc* pDoc;
   MEDIA Media;

   // resolve the document
   pDoc = (CZeusDoc*)(pJob->hDevice);
   
   // sanity check
   if (!pDoc) return pgprc_error_invalid_device_handle;
   if (!(pDoc->IsKindOf(RUNTIME_CLASS(CZeusDoc)))) 
	  return pgprc_error_invalid_device_handle;

   // look for all montageable photos
   for (pos = pJob->Photos.GetHeadPosition(); pos != NULL;)
	  {	  	  		 
	  // get the photo
	  pPhoto = (POURPGPRCPHOTOSPECS)pJob->Photos.GetNext(pos);

	  // should we?
	  if (!pPhoto->bMontageThis) continue;	  

	  // get the media
	  QUEUEDEVICE.GetMedia(iMontageCanvasToUse, &Media);

	  // get the dims of the canvas
	  if (QUEUEDEVICE.pDevInfo->Orientation == DEV_ORIENT_PORTRAIT)
		 {      
		 PGPRCCanvas.iWidthInches1000 = abs(Media.Rect.right - Media.Rect.left) * 10;
		 PGPRCCanvas.iHeightInches1000 = abs(Media.Rect.top - Media.Rect.bottom) * 10;   
		 }
	  else
		 {      
		 PGPRCCanvas.iHeightInches1000 = abs(Media.Rect.right - Media.Rect.left) * 10;
		 PGPRCCanvas.iWidthInches1000 = abs(Media.Rect.top - Media.Rect.bottom) * 10;   
		 }	   					

	  if ( bOverrideMisFit )
	  {
		// Save Original Size
	    Save_iWidthInches1000	= pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000;
	    Save_iHeightInches1000	= pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000;

		// Set Sise to Media Size
		pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000	= PGPRCCanvas.iWidthInches1000;
		pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000	= PGPRCCanvas.iHeightInches1000;
	  }

TRYAGAIN:

	  // the photo is bigger than the canvas!
	  if (pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000 > PGPRCCanvas.iWidthInches1000 ||
		  pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000 > PGPRCCanvas.iHeightInches1000)			 
		 {
		 // assign a penalty for a misfit so we don't mistakenly choose this
		 // one because of fewer pages
		 iPage += MISFIT_PENALTY;
		 continue;
		 }		
	  // can we fit this on the canvas?
	  if (((iRunningWidthInches1000 + pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000) <= 
		  PGPRCCanvas.iWidthInches1000) &&
		 (iRunningHeightInches1000 + pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000) <=
		 PGPRCCanvas.iHeightInches1000)
		 {
		 // set the x offset
		 pPhoto->iXoffInches1000 = iRunningWidthInches1000;
		 // set the y offset
		 pPhoto->iYoffInches1000 = iRunningHeightInches1000;
		 // set the page
		 pPhoto->iPage = iPage;
		 // new running width
		 iRunningWidthInches1000 = 
			iRunningWidthInches1000 + 
			pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000 + 
			//pJob->PGPRCPrintSpecs.iMontagePhotoSpacingXInches1000;
			pPhoto->PhotoSpecs.BasePhotoSpecs.iMontagePhotoSpacingXInches1000;
		 // new max swath height
		 iMaxSwathHeight = max(iMaxSwathHeight, pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000);
		 }
	  else
		 {
		 // we need a new page
		 if ((iRunningHeightInches1000 + pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000) >
			  PGPRCCanvas.iHeightInches1000)
			 {
			 // reset these
			 iRunningWidthInches1000 = 0;
			 iRunningHeightInches1000 = 0;
			 iMaxSwathHeight = 0;
			 // increment the page
			 iPage++;
			 goto TRYAGAIN;
			 }
		  // we need a new swath
		 else if ((iRunningWidthInches1000 + pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000) >
			  PGPRCCanvas.iWidthInches1000)
			 {
			 // reset these
			 iRunningWidthInches1000 = 0;			    
			 // compute new running height
			 iRunningHeightInches1000 = 
			    iRunningHeightInches1000 +
			    //iMaxSwathHeight + pJob->PGPRCPrintSpecs.iMontagePhotoSpacingYInches1000;
				iMaxSwathHeight + pPhoto->PhotoSpecs.BasePhotoSpecs.iMontagePhotoSpacingYInches1000;				
			 goto TRYAGAIN;
			 }  					  
		 } 		 	
	  }

   if ( bOverrideMisFit )
   {
	  // Restore Original Size
	  pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000		= Save_iWidthInches1000;
	  pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000	= Save_iHeightInches1000;
   }

   return gRetVal;
   }			  

PGPRC_RETURNVAL CPGPRCServer::CreateMontagePhoto(CSocketJob* pJob, CPtrArray& coPhotosToMontage)
   {
   int i;
   PGPRC_RETURNVAL gRetVal = pgprc_returnok;
   POURPGPRCPHOTOSPECS pPhoto;      
   RIPSTRUCT RipStruct;   
   BITMAPHANDLE PhotoBitmap;
   MEDIA Media;
   CDevice* pcdTemp;
   CZeusDoc* pDoc;
   CString csTempPath;
   char pTempFileName[_MAX_PATH];
   CQueueFile* pCQueueFile;
   int iMediaWidthPica;
   int iMediaHeightPica;
   int iDestWidthPica;
   int iDestHeightPica;
   int iDestXOffPica;
   int iDestYOffPica;
   CStdioFile cf;    
   float hDots;
   float fEnlarge;   
   int iNewWidth;
   int iNewHeight;
   int iTotalPages = 0;

   // got milk?
   if (coPhotosToMontage.GetSize() < 1) 
	  {	  
	  return gRetVal;	  
	  }

   // get the first photo;
   // assume that all the photos are using the same device settings
   pPhoto = (POURPGPRCPHOTOSPECS)coPhotosToMontage.GetAt(0);

   // resolve the document
   pDoc = (CZeusDoc*)(pJob->hDevice);

   // make a temp device for this
   pcdTemp = new CDevice;

   // seed it with the QUEUEDEVICE
   *(pcdTemp) = QUEUEDEVICE;
   
   // get the media, all photos in this method should 
   // use the same media
   pcdTemp->GetMedia(pPhoto->PhotoSpecs.BasePhotoSpecs.iCanvasToUse, &Media);   

   // compute the media size
   if (pcdTemp->pDevInfo->Orientation == DEV_ORIENT_PORTRAIT)
	  {
      iMediaWidthPica = (int)((float)abs(Media.Rect.right - Media.Rect.left) / 100.F * 72. + .5F);
      iMediaHeightPica = (int)((float)abs(Media.Rect.top - Media.Rect.bottom) / 100.F * 72. + .5F);
	  }
   else
	  {
      iMediaHeightPica = (int)((float)abs(Media.Rect.right - Media.Rect.left) / 100.F * 72. + .5F);
      iMediaWidthPica = (int)((float)abs(Media.Rect.top - Media.Rect.bottom) / 100.F * 72. + .5F);
	  }

   // get the temp path
   csTempPath = pDoc->crrrs.csScratchPath;

   // make a temp file
   GetTempFileName(csTempPath, "pgprc", 0, pTempFileName);

   if (!cf.Open(pTempFileName, 
	  CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	  {
      // we're done with the device
      delete pcdTemp;				   
	  return pgprc_error_cant_create_montage;
	  }

   // write the beginning of the PS file
   gRetVal = WritePSStart(cf, iMediaWidthPica, iMediaHeightPica, TRUE);

   if (gRetVal != pgprc_returnok) 
	  {
	  cf.Close();
      // we're done with the device
	  delete pcdTemp;				   
	  return gRetVal;
	  }			 

   // now blit all the photos into the montage bitmap
   for (i = 0; i < coPhotosToMontage.GetSize(); i++)
	  {
	  // get the photo
	  pPhoto = (POURPGPRCPHOTOSPECS)coPhotosToMontage.GetAt(i); 
		 
	  // load the photo
	  if (pPhoto->FileType != FILE_EPS)
		 {
	     if (L_LoadBitmap(pPhoto->bUseTempFile ? pPhoto->lpszTempDiskFile : 
							pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile,
							    &PhotoBitmap, sizeof(BITMAPHANDLE), 0, ORDER_RGB, NULL, NULL) != SUCCESS)
			{
		    gRetVal = pgprc_error_cant_create_montage;
		    break;
			}	  			
		 if (!ResizeMontageBitmap(&PhotoBitmap, pPhoto))
			{
			gRetVal = pgprc_error_cant_create_montage;
			break;
			}		  
	 	 // check bitmap sizing setting.  If bicubic or blinear, we need to 
		 // resize the bitmap manually - we can't rely on the PS rip
		 // to size properly
		 // note: because of offsets and the fact that we only use xdpi, this is
	 	 // an approximation, but it is probably good enough	 	 
		 if (pcdTemp->pDevInfo->BitmapSample == BITMAPSAMPLING_BICUBIC ||
			 pcdTemp->pDevInfo->BitmapSample == BITMAPSAMPLING_BILINEAR)
			{
			// get the actual frame buffer size
			pcdTemp->FillRipStruct(&RipStruct);

			// get the number of pixels in the horizontal frame
			hDots = (float)pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000 * 
			   RipStruct.WPixelsPerInch / 1000.;
			// get the enlargement factor
			fEnlarge = hDots / (float)PhotoBitmap.Width;
			// calculate the new width and height
			iNewWidth = (int)(fEnlarge * (float)PhotoBitmap.Width + .5);
			iNewHeight = (int)(fEnlarge * (float)PhotoBitmap.Height + .5);
			// resize it
			if (pcdTemp->pDevInfo->BitmapSample == BITMAPSAMPLING_BICUBIC)
			   ::L_SizeBitmap(&PhotoBitmap, iNewWidth, iNewHeight, SIZE_BICUBIC);
			else
			   ::L_SizeBitmap(&PhotoBitmap, iNewWidth, iNewHeight, BITMAPSAMPLING_BILINEAR);
			}
		 }			  

	  // positioning!
      iDestWidthPica = (int)((float)pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000 * 72. / 1000.F + .5F);
	  iDestHeightPica = (int)((float)pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000 * 72. / 1000.F + .5F);
      iDestXOffPica = (int)((float)pPhoto->iXoffInches1000 * 72. / 1000.F + .5F);
	  iDestYOffPica = (int)((float)pPhoto->iYoffInches1000 * 72. / 1000.F + .5F);
	  
	  // integreate the image differently if it is an eps file
	  if (pPhoto->FileType == FILE_EPS)
         gRetVal = WritePSMontagePS(cf, iDestWidthPica, iDestHeightPica, 
		    iDestXOffPica, iDestYOffPica, pPhoto);
	  else
         gRetVal = WritePSMontageBitmap(cf, iDestWidthPica, iDestHeightPica, 
		    iDestXOffPica, iDestYOffPica, PhotoBitmap);	  

	  if (gRetVal != pgprc_returnok)
		 {
		 break;
		 }

	  // free the photo bitmap
	  if (pPhoto->FileType != FILE_EPS)
	     L_FreeBitmap(&PhotoBitmap);	 	  
	  }																	   
   
   // write the end of the ps file
   WritePSMontageEnd(cf);

   // close the file
   cf.Close();

   // instantiate a queuefile 
   if ((pCQueueFile = new CQueueFile) == NULL)							  
	  {
      // we're done with the device
      delete pcdTemp;				   
	  return pgprc_error_outofmemory;	
	  }

   // init the queue file
   if (!pCQueueFile->Init(pTempFileName, iTotalPages, 0))
	  {
	  delete pCQueueFile;			
	  pCQueueFile = NULL;
	  ::DeleteFile(pTempFileName);
	  // we're done with the device
	  delete pcdTemp;				   
	  return pgprc_cant_find_file;		 	
	  }
								 
   // set this flag
   pCQueueFile->ExtraInfo = CQUEUE_EXTRA_PGPRCFILE;   

   // mark this for deletion
	pCQueueFile->ExtraInfo |= CQUEUE_EXTRA_SERVERFILE;

   // add a description   
   pCQueueFile->Description.Format("%s Photogize Montage with %d Photos", pJob->csName,
	  coPhotosToMontage.GetSize());   

   // now create a new photo and add it to the list
   if (pPhoto = new OURPGPRCPHOTOSPECS)
	  {	  
	  // copy over the photo specs for the first photo
	  memcpy(pPhoto, (POURPGPRCPHOTOSPECS)coPhotosToMontage.GetAt(0), sizeof OURPGPRCPHOTOSPECS);
	  pPhoto->bUseTempFile = FALSE;
	  strcpy(pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile, pTempFileName);
	  pPhoto->lpszTempDiskFile[0] = 0;	  
	  // reset this
	  pPhoto->hCqueueFile = pCQueueFile;   
	  // and this
	  pPhoto->iJobID = pCQueueFile->iJobID;
	  // don't put in the thumbnails
	  pPhoto->bPutInThumbnails = FALSE;
	  // add separators at the beginning
	  if (pPhoto->bIsSeparator)
		 {
		 // add add to the beginning of the photo array
		 POSITION pos;		 
		 pos = pJob->Photos.GetHeadPosition();
		 pJob->Photos.InsertBefore(pos, pPhoto);		 		 
		 }
	  else
	     // otherwise, add to the photo array
	     pJob->Photos.AddTail(pPhoto);	   	  

	  pPhoto->FileType = FILE_EPS;
	  //pPhoto->PhotoSpecs.iCanvasToUse = pJob->PGPRCPrintSpecs.iMontageCanvasToUse;
	  }
   else
	  {	  
	  delete pCQueueFile;   
	  pPhoto->hCqueueFile = NULL;
	  pCQueueFile = NULL;
	  }

   // we're done with the device
   delete pcdTemp;				
   
   return gRetVal;
   }

PGPRC_RETURNVAL CPGPRCServer::WritePSStart(CStdioFile& cf, int iMediaWidthPica, 
										  int iMediaHeightPica, BOOL bDoEPSFStuff)
   {
   // do the starting PS stuff
   fprintf(cf.m_pStream, "%%!PS-Adobe-2.0 EPSF-2.0\r\n");
   fprintf(cf.m_pStream, "%%%%Title: PGPRC Montage\r\n");
   fprintf(cf.m_pStream, "%%%%Creator: PGPRC\r\n");
   fprintf(cf.m_pStream, "%%%%BoundingBox: 0 0 %d %d\r\n", iMediaWidthPica, iMediaHeightPica);
   fprintf(cf.m_pStream, "%%%%EndComments\r\n");
   fprintf(cf.m_pStream, "%%%%BeginSetup\r\n");
   fprintf(cf.m_pStream, "%%%%EndSetup\r\n");
   fprintf(cf.m_pStream, "/BD {bind def} bind def\r\n");
   fprintf(cf.m_pStream, "/LD {load def} BD\r\n");
   fprintf(cf.m_pStream, "/GS /gsave LD\r\n");
   fprintf(cf.m_pStream, "/GR /grestore LD\r\n");
   fprintf(cf.m_pStream, "/N /newpath LD\r\n");
   fprintf(cf.m_pStream, "/M /moveto LD\r\n");
   fprintf(cf.m_pStream, "/L /lineto LD\r\n");   
   fprintf(cf.m_pStream, "/S /show LD\r\n");
   fprintf(cf.m_pStream, "/SL /setlinewidth LD\r\n");
   fprintf(cf.m_pStream, "/FF {gsave eofill grestore} def\r\n");
   fprintf(cf.m_pStream, "/EO /eoclip LD\r\n");
   fprintf(cf.m_pStream, "/SF {gsave stroke grestore} def\r\n");
   fprintf(cf.m_pStream, "/Helvetica-Bold findfont [15 0 0 15 0 0] makefont setfont\r\n");
   fprintf(cf.m_pStream, "/cstring\r\n");
   fprintf(cf.m_pStream, "   { dup stringwidth pop 2 div currentpoint\r\n");
   fprintf(cf.m_pStream, "     3 -2 roll exch sub exch moveto } def\r\n");
   fprintf(cf.m_pStream, "/rstring\r\n");
   fprintf(cf.m_pStream, "   { dup stringwidth pop currentpoint\r\n");
   fprintf(cf.m_pStream, "     3 -2 roll exch sub exch moveto } def\r\n");
   fprintf(cf.m_pStream, "\r\n");
   if (bDoEPSFStuff)
	  {
	  fprintf(cf.m_pStream, "/BeginEPSF {\r\n");
	  fprintf(cf.m_pStream, "/b4_Inc_state save def\r\n");
	  fprintf(cf.m_pStream, "/dict_count countdictstack def\r\n");
	  fprintf(cf.m_pStream, "/op_count count 1 sub def\r\n");
	  fprintf(cf.m_pStream, "userdict begin\r\n");
	  fprintf(cf.m_pStream, "/showpage {} def\r\n");
	  fprintf(cf.m_pStream, "0 setgray 0 setlinecap\r\n");
	  fprintf(cf.m_pStream, "1 setlinewidth 0 setlinejoin\r\n");
	  fprintf(cf.m_pStream, "10 setmiterlimit [] 0 setdash newpath\r\n");
	  fprintf(cf.m_pStream, "/languagelevel where\r\n");
	  fprintf(cf.m_pStream, "{pop languagelevel\r\n");
	  fprintf(cf.m_pStream, "1 ne\r\n");
	  fprintf(cf.m_pStream, "  {false setstrokeadjust false setoverprint\r\n");
	  fprintf(cf.m_pStream, "  } if\r\n");
	  fprintf(cf.m_pStream, "} if\r\n");
	  fprintf(cf.m_pStream, "} bind def\r\n");
	  fprintf(cf.m_pStream, "\r\n");
	  fprintf(cf.m_pStream, "/EndEPSF {\r\n");
	  fprintf(cf.m_pStream, "count op_count sub {pop} repeat\r\n");
	  fprintf(cf.m_pStream, "countdictstack dict_count sub {end} repeat\r\n");
	  fprintf(cf.m_pStream, "b4_Inc_state restore\r\n");	  
	  fprintf(cf.m_pStream, "} bind def\r\n");
	  }
   fprintf(cf.m_pStream, "\r\n");   
   fprintf(cf.m_pStream, "GS N\r\n");
   fprintf(cf.m_pStream, "0 0 M\r\n");
   fprintf(cf.m_pStream, "%d 0 L\r\n", iMediaWidthPica);
   fprintf(cf.m_pStream, "%d %d L\r\n", iMediaWidthPica, iMediaHeightPica);
   fprintf(cf.m_pStream, "0 %d L\r\n", iMediaHeightPica);
   fprintf(cf.m_pStream, ".999 .999 .999 setrgbcolor\r\n");
   
   fprintf(cf.m_pStream, "FF GR\r\n");

   // for everything else
   fprintf(cf.m_pStream, ".0 .0 .0 setrgbcolor\r\n");   

   return pgprc_returnok;
   }

PGPRC_RETURNVAL CPGPRCServer::WritePSStartN(CStdioFile& cf, int iMediaWidthPica, 
									int iMediaHeightPica)
   {
   fprintf(cf.m_pStream, "%%!PS-Adobe-2.0 EPSF-2.0\r\n");
   fprintf(cf.m_pStream, "%%%%Title: PGPRC Montage\r\n");
   fprintf(cf.m_pStream, "%%%%Creator: PGPRC\r\n");
   fprintf(cf.m_pStream, "%%%%BoundingBox: 0 0 %d %d\r\n", iMediaWidthPica, iMediaHeightPica);
   fprintf(cf.m_pStream, "%%%%EndComments\r\n");
   fprintf(cf.m_pStream, "%%%%BeginSetup\r\n");
   fprintf(cf.m_pStream, "%%%%EndSetup\r\n");
   fprintf(cf.m_pStream, "/BD {bind def} bind def\r\n");
   fprintf(cf.m_pStream, "/LD {load def} BD\r\n");
   fprintf(cf.m_pStream, "/GS /gsave LD\r\n");
   fprintf(cf.m_pStream, "/GR /grestore LD\r\n");
   fprintf(cf.m_pStream, "/N /newpath LD\r\n");
   fprintf(cf.m_pStream, "/M /moveto LD\r\n");
   fprintf(cf.m_pStream, "/L /lineto LD\r\n");   
   fprintf(cf.m_pStream, "/S /show LD\r\n");
   fprintf(cf.m_pStream, "/SL /setlinewidth LD\r\n");
   fprintf(cf.m_pStream, "/FF {gsave eofill grestore} def\r\n");
   fprintf(cf.m_pStream, "/EO /eoclip LD\r\n");
   fprintf(cf.m_pStream, "/SF {gsave stroke grestore} def\r\n");
   fprintf(cf.m_pStream, "/Helvetica-Bold findfont [15 0 0 15 0 0] makefont setfont\r\n");
   fprintf(cf.m_pStream, "/cstring\r\n");
   fprintf(cf.m_pStream, "   { dup stringwidth pop 2 div currentpoint\r\n");
   fprintf(cf.m_pStream, "     3 -2 roll exch sub exch moveto } def\r\n");
   fprintf(cf.m_pStream, "/rstring\r\n");
   fprintf(cf.m_pStream, "   { dup stringwidth pop currentpoint\r\n");
   fprintf(cf.m_pStream, "     3 -2 roll exch sub exch moveto } def\r\n");
   fprintf(cf.m_pStream, "\r\n");   
   fprintf(cf.m_pStream, "GS N\r\n");
   fprintf(cf.m_pStream, "0 0 M\r\n");
   fprintf(cf.m_pStream, "%d 0 L\r\n", iMediaWidthPica);
   fprintf(cf.m_pStream, "%d %d L\r\n", iMediaWidthPica, iMediaHeightPica);
   fprintf(cf.m_pStream, "0 %d L\r\n", iMediaHeightPica);
   fprintf(cf.m_pStream, ".999 .999 .999 setrgbcolor\r\n");   

   fprintf(cf.m_pStream, "FF GR\r\n");

   // for everything else
   fprintf(cf.m_pStream, ".0 .0 .0 setrgbcolor\r\n");   

   return pgprc_returnok;				  
   };


PGPRC_RETURNVAL CPGPRCServer::WritePSMontageBitmap(CStdioFile& cf, int iDestWidthPica, int iDestHeightPica, 
										   int iDestXOffPica, int iDestYOffPica, 
										   BITMAPHANDLE PhotoBitmap) 
   {
   int i;
   BYTE* pData; 
   PGPRC_RETURNVAL gRetVal = pgprc_returnok;

   // write the bitmap to the eps file
   fprintf(cf.m_pStream, "GS /picstr %d string def\r\n", 
	  PhotoBitmap.Width * 3);	   
   fprintf(cf.m_pStream, "%d %d translate %d %d scale\r\n", 	  
	  iDestXOffPica, iDestYOffPica, iDestWidthPica, iDestHeightPica);
   fprintf(cf.m_pStream, "%d %d 8 [%d 0 0 -%d 0 %d]\r\n", 
	  PhotoBitmap.Width, PhotoBitmap.Height, PhotoBitmap.Width, 
	  PhotoBitmap.Height, PhotoBitmap.Height);
   fprintf(cf.m_pStream, "{currentfile picstr readstring pop} false 3 colorimage\r\n");
		 
   // lock the tile bitmap
   L_AccessBitmap(&PhotoBitmap);
	
   // write the data
   if (pData = new BYTE[PhotoBitmap.Width * 3])
	  {
	  for (i = 0; i < PhotoBitmap.Height; i++)
		 {	
		 L_GetBitmapRow(&PhotoBitmap, pData, i, PhotoBitmap.Width * 3);
		 cf.Write(pData, PhotoBitmap.Width * 3);
		 }
	  cf.Write("\r\nGR\r\n", 6);
	  delete pData;
	  }
   else
	  {
	  gRetVal = pgprc_error_outofmemory;
	  }

   // release the bitmap
   L_ReleaseBitmap(&PhotoBitmap);

   return gRetVal;
   }

#define PGPRC_PS_BUF 32000

PGPRC_RETURNVAL CPGPRCServer::WritePSMontagePS(CStdioFile& cf, int iDestWidthPica, int iDestHeightPica, 
										   int iDestXOffPica, int iDestYOffPica, 
										   POURPGPRCPHOTOSPECS pPhoto)
   {
   CFile cfIn;
   PGPRC_RETURNVAL gRetVal = pgprc_returnok;
   char* lpFileName;				  
   BYTE* pBuf;
   int iNumRead;
   float fxScale;
   float fyScale;

   // get scale factors
   fxScale = (float)iDestWidthPica / PGPRC_SEPARATOR_BBOX_WIDTHF;
   fyScale = (float)iDestHeightPica / PGPRC_SEPARATOR_BBOX_HEIGHTF;

   // get name
   if (pPhoto->bUseTempFile)
	  lpFileName = pPhoto->lpszTempDiskFile;
   else
	  lpFileName = pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile;

   // make a buf
   if (!(pBuf = new BYTE [PGPRC_PS_BUF]))
	  {
	  return pgprc_error_cant_create_montage;
	  }

   // put the job separator file into the bigger file
   try
	  {
      if (cfIn.Open(lpFileName, CFile::modeRead | CFile::typeBinary))
		 {	  
		 // write the start
         fprintf(cf.m_pStream, "\r\nBeginEPSF\r\n");         
		 
		 // if this is a portrait frame, rotate the image
		 if (((float)iDestHeightPica / (float)iDestWidthPica) > 1.0)
			{		    
			//fprintf(cf.m_pStream, "%d 0 translate\r\n", iDestHeightPica / 2);        			
			fprintf(cf.m_pStream, "%d 0 translate\r\n", iDestWidthPica);        			
		    fprintf(cf.m_pStream, "90 rotate\r\n");     
            fxScale = (float)iDestWidthPica / PGPRC_SEPARATOR_BBOX_HEIGHTF;
            fyScale = (float)iDestHeightPica / PGPRC_SEPARATOR_BBOX_WIDTHF;
			SWAPINTS(iDestXOffPica, iDestYOffPica);

			iDestYOffPica = -iDestYOffPica;
			}

		 // and the scaline
         fprintf(cf.m_pStream, "%d %d translate %f %f scale\r\n", 	  
	        iDestXOffPica, iDestYOffPica, fxScale, fyScale);
		 // put in the file
		 do
			{
			// read the file
	        iNumRead = cfIn.Read(pBuf, PGPRC_PS_BUF); 
			// and write it
			if (iNumRead > 0)
			   cf.Write(pBuf, iNumRead);
			} while (iNumRead > 0); 	  

		 // write the end
         fprintf(cf.m_pStream, "\r\nEndEPSF\r\n");

	     cfIn.Close();
		 }	  
      else
		 {
	     gRetVal = pgprc_error_cant_create_montage;
		 }

      // now delete the job file because we're montaged it
      ::DeleteFile(lpFileName);
	  }

   catch(CFileException* e)	  
	  {
	  e->Delete();
	  gRetVal = pgprc_error_cant_create_montage;
	  }										 	  

   // delete the buffer
   delete pBuf;

   return gRetVal;
   }

PGPRC_RETURNVAL CPGPRCServer::WritePSMontageEnd(CStdioFile& cf)
   {
   // write the end
   fprintf(cf.m_pStream, "gsave showpage grestore\r\n");   
   
   return pgprc_returnok;
   }

PGPRC_RETURNVAL CPGPRCServer::GetDevOptionName(int len, HANDLE hDevice, int iOption, 
									   CClientSocket* pSocket)
   {
   PGPRC_RETURNVAL gRetVal = pgprc_returnok;
   char pTemp[PGPRC_MAXSTRLEN];
   GETDEVOPTIONSETTINGKEY GetDevOptionSettingKey;
   CZeusDoc* pDoc;

   GetDevOptionSettingKey.iOption = iOption;

   // resolve the document
   pDoc = (CZeusDoc*)(hDevice);

   // sanity check
   if (!pDoc) 
	  {	  
	  return pgprc_error_invalid_device_handle;
	  }
   if (!(pDoc->IsKindOf(RUNTIME_CLASS(CZeusDoc)))) 
	  {	  
	  return pgprc_error_invalid_device_handle;
	  }

   if (!QUEUEDEVICE.GetSetExtra(DEVGETSET_GETDEVOPTIONNAME, (BYTE*)&GetDevOptionSettingKey, 
	  (BYTE*)pTemp, sizeof(int), PGPRC_MAXSTRLEN))	  
	  gRetVal = pgprc_error_nodevsettingoroption;
	  	
   // copy the dev option name or junk if a problem
   *(pSocket->m_pArchiveOut) << PGPRC_MAXSTRLEN;
   // ReportByteCount(TRUE, sizeof (int));
   pSocket->m_pArchiveOut->Write(pTemp, PGPRC_MAXSTRLEN);
   // ReportByteCount(TRUE, sizeof PGPRC_MAXSTRLEN);

   return gRetVal;
   }

PGPRC_RETURNVAL CPGPRCServer::GetDevOptionSettingName(int len, HANDLE hDevice, int iOption, 
											  int iSetting, CClientSocket* pSocket)
   {
   PGPRC_RETURNVAL gRetVal = pgprc_returnok;
   char pTemp[PGPRC_MAXSTRLEN];
   GETDEVOPTIONSETTINGKEY GetDevOptionSettingKey;
   CZeusDoc* pDoc;

   // load the option and the setting
   GetDevOptionSettingKey.iOption = iOption;
   GetDevOptionSettingKey.iSetting = iSetting;

   // resolve the document
   pDoc = (CZeusDoc*)(hDevice);

   // sanity check
   if (!pDoc)
	  {	  
	  return pgprc_error_invalid_device_handle;
	  }
   if (!(pDoc->IsKindOf(RUNTIME_CLASS(CZeusDoc)))) 
	  {	  
	  return pgprc_error_invalid_device_handle;
	  }

   // call the device proc
   if (!QUEUEDEVICE.GetSetExtra(DEVGETSET_GETDEVOPTIONSETTINGNAME, (BYTE*)&GetDevOptionSettingKey, 
	  (BYTE*)pTemp, sizeof GETDEVOPTIONSETTINGKEY, PGPRC_MAXSTRLEN))	   
	  gRetVal = pgprc_error_nodevsettingoroption;
	  
   // copy the dev setting name
   *(pSocket->m_pArchiveOut) << PGPRC_MAXSTRLEN;
   // ReportByteCount(TRUE, sizeof (int));
   pSocket->m_pArchiveOut->Write(pTemp, PGPRC_MAXSTRLEN);
   // ReportByteCount(TRUE, sizeof PGPRC_MAXSTRLEN);

   return gRetVal;
   }

PGPRC_RETURNVAL CPGPRCServer::GetDevOption(int len, HANDLE hDevice, int iOption, 
								   CClientSocket* pSocket)
   {
   PGPRC_RETURNVAL gRetVal = pgprc_returnok;
   GETDEVOPTIONSETTINGKEY GetDevOptionSettingKey;   
   CZeusDoc* pDoc;

   // load the option and the setting
   GetDevOptionSettingKey.iOption = iOption;
   GetDevOptionSettingKey.iSetting = 0;

   // resolve the document
   pDoc = (CZeusDoc*)(hDevice);

   // sanity check
   if (!pDoc) 
	  {	  
	  return pgprc_error_invalid_device_handle;
	  }
   if (!(pDoc->IsKindOf(RUNTIME_CLASS(CZeusDoc)))) 
	  {	  
	  return pgprc_error_invalid_device_handle;
	  }

   if (!QUEUEDEVICE.GetSetExtra(DEVGETSET_GETDEVOPTION, (BYTE*)&GetDevOptionSettingKey, NULL, 
	  sizeof GETDEVOPTIONSETTINGKEY, 0))	  
	  gRetVal = pgprc_error_nodevsettingoroption;	  

   // copy the dev setting name
   *(pSocket->m_pArchiveOut) << (sizeof (int));
   // ReportByteCount(TRUE, sizeof (int));
   *(pSocket->m_pArchiveOut) << GetDevOptionSettingKey.iSetting;	  
   // ReportByteCount(TRUE, sizeof (int));

   return gRetVal;
   }

PGPRC_RETURNVAL CPGPRCServer::SetDevOption(HANDLE hDevice, int iOption, int iSetting, 
								   CClientSocket* pSocket)
   {
   PGPRC_RETURNVAL gRetVal = pgprc_returnok;
   GETDEVOPTIONSETTINGKEY GetDevOptionSettingKey;   
   CZeusDoc* pDoc;
   CString csToken;
   CString csValue;
   PDEVINFO* pData = NULL;

   GetDevOptionSettingKey.iOption = iOption;
   GetDevOptionSettingKey.iSetting = iSetting;

   // resolve the document
   pDoc = (CZeusDoc*)(hDevice);

   QUEUEDEVICE.SetDevInfo(QUEUEDEVICE.pDevInfo, TRUE);

   pData = new PDEVINFO[1];
   pData[0] = QUEUEDEVICE.pDevInfo;
   QUEUEDEVICE.UpdateDlgFromQueue(pData, 1);
   QUEUEDEVICE.GetDevInfo();

   // sanity check
   if (!pDoc)
	   return pgprc_error_invalid_device_handle;

   if (!(pDoc->IsKindOf(RUNTIME_CLASS(CZeusDoc)))) 
	  return pgprc_error_invalid_device_handle;

   if (!QUEUEDEVICE.GetSetExtra(DEVGETSET_SETDEVOPTION, (BYTE*)&GetDevOptionSettingKey, NULL, 
	  sizeof GETDEVOPTIONSETTINGKEY, 0))
		gRetVal = pgprc_error_nodevsettingoroption;	  
   else
	  // reload our options
		QUEUEDEVICE.GetDevInfo();

   return gRetVal;					
   }

BOOL CPGPRCServer::ResizeMontageBitmap(pBITMAPHANDLE pPhotoBitmap, 
   POURPGPRCPHOTOSPECS pPhoto)
   {
   BOOL bRet = TRUE;
   float fDevAspect;
   float fBitmapAspect;
   BOOL bLandscapeDevice;
   BOOL bLandscapeBitmap;
   int iNewWidth, iNewHeight;   
	
   // get aspect ratios
   fDevAspect = (float)pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000 / 
	  (float)pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000;
   fBitmapAspect = (float)pPhotoBitmap->Height / (float)pPhotoBitmap->Width;

   // landscape device?
   if (fDevAspect < 1.0)
      bLandscapeDevice = TRUE;
   else
      bLandscapeDevice = FALSE;
   
   // landscape bitmap?
   if (fBitmapAspect < 1.0)
      bLandscapeBitmap = TRUE;
   else
      bLandscapeBitmap = FALSE;

   // rotate it if necessary
   if (bLandscapeBitmap != bLandscapeDevice)
	  {
	  if (L_RotateBitmap(pPhotoBitmap, 9000, ROTATE_RESIZE,  RGB(255, 255, 255)) != SUCCESS)
		 return FALSE;
	  // recompute
	  fBitmapAspect = (float)pPhotoBitmap->Height / (float)pPhotoBitmap->Width;
	  }
   
   if (pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcSizing == pgprc_montage_crop)    
	  {
	  // now compute the trim for cropping
	  if (fDevAspect < fBitmapAspect)
		 {
		 iNewHeight = (int)((float)pPhotoBitmap->Width * fDevAspect + .5F);
		 if (L_TrimBitmap(pPhotoBitmap, 0, (pPhotoBitmap->Height - iNewHeight) / 2,
			pPhotoBitmap->Width, iNewHeight) != SUCCESS)
			return FALSE;															   
		 }
	  else if (fDevAspect > fBitmapAspect)
		 {
		 iNewWidth = (int)((float)pPhotoBitmap->Height / fDevAspect + .5F);
		 if (L_TrimBitmap(pPhotoBitmap, (pPhotoBitmap->Width - iNewWidth) / 2, 0,
			iNewWidth, pPhotoBitmap->Height) != SUCCESS)
			return FALSE;															   
		 }
	  }
   else
	  {
	  // now compute the extra spacing we need to fitting
	  if (fDevAspect < fBitmapAspect)
		 {
		 pPhoto->iXoffInches1000 += (int)
		    (((float)pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000 - 
		    (float)pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000 / fBitmapAspect) / 2. + .5); 		  
		 pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000 -= (int)
		    (((float)pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000 - 
		    (float)pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000 / fBitmapAspect) + .5);
		 }
	  else if (fDevAspect > fBitmapAspect)
		 {		 
		 pPhoto->iYoffInches1000 += (int)
		    (((float)pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000 - 
		    (float)pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000 * fBitmapAspect) / 2. + .5);
		 pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000 -= (int)
		    (((float)pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000 - 
		    (float)pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000 * fBitmapAspect) + .5);
		 }
	  }
   
   return bRet;
   }

PGPRC_RETURNVAL CPGPRCServer::GetPhotoStatus(int len, CClientSocket* pSocket)
   {
   CPaneElement* pRipElement;   
   CZeusDoc* pDoc;   
   CZeusDoc* pPassedDoc;
   int i, j;
   int iCnt = 0;   
   CQueueFile* pThisCQueueFile;
   CSocketJob* pJob;
   POSITION pos;
   POURPGPRCPHOTOSPECS pPhoto;   
   PPGPRCPHOTOSTATUSSTRUCT pGisPhotoStatusStruct;
   BYTE* pTemp;
   CObArray* pCQ;

   if (len != (sizeof PGPRCPHOTOSTATUSSTRUCT))
	  {
	  DumpIncomingData(len, pSocket);
	  return pgprc_server_bad_datalen;
	  }

   // create a new photo status struct
   if (!(pGisPhotoStatusStruct = new PGPRCPHOTOSTATUSSTRUCT)) 
	  {      
	  DumpIncomingData(len, pSocket);
	  delete pGisPhotoStatusStruct;
	  return pgprc_error_outofmemory;
	  }

   // copy the data
   pTemp = (BYTE*)pGisPhotoStatusStruct;
   pSocket->m_pArchiveIn->Read(pTemp, len);
   // ReportByteCount(FALSE, len);

   // resolve the job
   pJob = (CSocketJob*)pGisPhotoStatusStruct->hJob;
									   	  
   // default
   pGisPhotoStatusStruct->pgprcJobStatus = pgprc_photo_notfound;

   // sanity check
   if (!pJob) 
	  {      
	  delete pGisPhotoStatusStruct;
	  return pgprc_error_invalid_job_handle;
	  }
   if (!(pJob->IsKindOf(RUNTIME_CLASS(CSocketJob)))) 
	  {      
	  delete pGisPhotoStatusStruct;
	  return pgprc_error_invalid_job_handle;
	  }

   // resolve the document
   pPassedDoc = (CZeusDoc*)(pJob->hDevice);  

   // sanity check
   if (!pPassedDoc) 
	  {      
	  delete pGisPhotoStatusStruct;
	  return pgprc_error_invalid_device_handle;
	  }
   if (!(pPassedDoc->IsKindOf(RUNTIME_CLASS(CZeusDoc)))) 
	  {      
	  delete pGisPhotoStatusStruct;
	  return pgprc_error_invalid_device_handle;
	  }

   // search all the rip arrays for files which match our photo id
   // and handle, then get the status
   for (pos = pJob->Photos.GetHeadPosition(); pos != NULL;)
	  {	  
	  pPhoto = (POURPGPRCPHOTOSPECS)pJob->Photos.GetNext(pos);
	  
	  // not the one!
	  if (((POURPGPRCPHOTOSPECS)pGisPhotoStatusStruct->hPhoto != pPhoto) && 
		  ((PJOBSEPARATOR)pGisPhotoStatusStruct->hPhoto != &pJob->JobSeparator))
		  continue;

	  // default
	  pGisPhotoStatusStruct->pgprcJobStatus = pgprc_photo_notfound;
	
	  // look for it
	  for (i = 0; i < PANEARRAY.GetSize(); i++)
		 {
		 pRipElement = (CPaneElement*)PANEARRAY.GetAt(i);

		 if (!pRipElement) continue;

		 // get the active document
		 if (pDoc = (CZeusDoc*)(pRipElement->pDocPtr))
			{
			// doesn't match our doc so continue;
			if (pPassedDoc != pDoc) continue;

			// copy the pane location
			pGisPhotoStatusStruct->iPaneLocation = pRipElement->iViewStyle;

			// what kind of pane?
			switch(pRipElement->iViewStyle)
			   {
			   case QUEUESTYLE_HOLD:
				    pCQ = &pDoc->CQ_Hold;
				    break;
			   case QUEUESTYLE_PRINT:
				    pCQ = &pDoc->CQ;
				    break;
			   case QUEUESTYLE_ARCHIVE:
				    pCQ = &pDoc->CQ_Archive;
				    break;
			   }

			// go through the files
			for (j = 0; j < pCQ->GetSize(); j++)
			   {
			   // get file
			   pThisCQueueFile = (CQueueFile*)pCQ->GetAt(j);
			   // a match!
			   if (pPhoto->iJobID == pThisCQueueFile->iJobID)
				  {
				  // get the status
				  if ((pThisCQueueFile->StatusFlag & FILE_STATUS_INRIP) == FILE_STATUS_INRIP)						
					 pGisPhotoStatusStruct->pgprcJobStatus = pgprc_photo_printing;
				  else if ((pThisCQueueFile->StatusFlag & FILE_STATUS_CHECK) == FILE_STATUS_OK)
					 pGisPhotoStatusStruct->pgprcJobStatus = pgprc_photo_printed_ok;
				  else if ((pThisCQueueFile->StatusFlag & FILE_STATUS_CHECK) == FILE_STATUS_OK_DELETED)
					 pGisPhotoStatusStruct->pgprcJobStatus = pgprc_photo_printed_ok;
				  else if ((pThisCQueueFile->StatusFlag & FILE_STATUS_CHECK) == FILE_STATUS_HOLD)
					 pGisPhotoStatusStruct->pgprcJobStatus = pgprc_photo_idle;
				  else if ((pThisCQueueFile->StatusFlag & FILE_STATUS_CHECK) == FILE_STATUS_NOTOK)
					 pGisPhotoStatusStruct->pgprcJobStatus = pgprc_photo_error_printing;				 						
				  // and copy the string
				  strcpy(pGisPhotoStatusStruct->StatusStr, pThisCQueueFile->Status);
				  }
			   }
			}			
         }   
	  }

   // copy the job status over
   pTemp = (BYTE*)pGisPhotoStatusStruct;
   *(pSocket->m_pArchiveOut) << (sizeof (PGPRCPHOTOSTATUSSTRUCT));
   // ReportByteCount(TRUE, sizeof (int));
   pSocket->m_pArchiveOut->Write(pTemp, len);
   // ReportByteCount(TRUE, sizeof PGPRCPHOTOSTATUSSTRUCT);
   delete pGisPhotoStatusStruct;
   return pgprc_returnok;
   }

void CPGPRCServer::ReturnDummyInfo(int len, CClientSocket* pSocket)
   {
   int i;

   for (i = 0; i < len; i++)		  
	  *(pSocket->m_pArchiveOut) << 0;

   // ReportByteCount(TRUE, len);
   }

void CPGPRCServer::SwapCustomPageDims(CSocketJob* pJob)
   {
   POSITION pos;
   POURPGPRCPHOTOSPECS pPhoto;
   PGPRC_RETURNVAL gRetVal = pgprc_returnok;

   // look for all montageable photos
   for (pos = pJob->Photos.GetHeadPosition(); pos != NULL;)
	  {	  	  		 
	  // get the photo
	  pPhoto = (POURPGPRCPHOTOSPECS)pJob->Photos.GetNext(pos);

	  SWAPINTS(pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000, 
		 pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000);
	  }
   }

void CPGPRCServer::ResetMontageFlags(CSocketJob* pJob)
   {
   POSITION pos;
   POURPGPRCPHOTOSPECS pPhoto;

   for (pos = pJob->Photos.GetHeadPosition(); pos != NULL;)
	  {	  	  				 
	  pPhoto = (POURPGPRCPHOTOSPECS)pJob->Photos.GetNext(pos);
	  pPhoto->bMontageThis = FALSE;
	  pPhoto->bMontageDone = FALSE;
	  }
   }

int CPGPRCServer::CheckNextMontageSet(CSocketJob* pJob, int& iMontageCanvasToUse)
   {
   POSITION pos;
   POURPGPRCPHOTOSPECS pPhoto;
   int cnt = 0;   
   
   for (pos = pJob->Photos.GetHeadPosition(); pos != NULL;)
	  {	  	  				 
	  pPhoto = (POURPGPRCPHOTOSPECS)pJob->Photos.GetNext(pos);

	  // if the file is to be montaged AND has not been montaged...
	  if ((pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcSizing == pgprc_montage_crop ||
		   pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcSizing == pgprc_montage_fit) && !pPhoto->bMontageDone)
		 {
		 // set the canvas on the first file
		 if (cnt == 0)
			{
			iMontageCanvasToUse = pPhoto->PhotoSpecs.BasePhotoSpecs.iCanvasToUse;	
			// if not a separator, and bPrintJobSeparatorOnly is set, don't montage
			if (!pPhoto->bIsSeparator && pJob->PGPRCPrintSpecs.bPrintJobSeparatorOnly)
			   pPhoto->bMontageThis = FALSE;
			else
			   {
			   pPhoto->bMontageThis = TRUE;
			   cnt++;
			   }
			}
		 else
			{
			// after that, check to see if this uses the same canvas
			if (iMontageCanvasToUse == pPhoto->PhotoSpecs.BasePhotoSpecs.iCanvasToUse)
			   {
			   // if not a separator, and bPrintJobSeparatorOnly is set, don't montage
			   if (!pPhoto->bIsSeparator && pJob->PGPRCPrintSpecs.bPrintJobSeparatorOnly)
				  pPhoto->bMontageThis = FALSE;
			   else
				  {
				  pPhoto->bMontageThis = TRUE;
			      cnt++;
				  }
			   }
			}	 
		 }		 
	  else
		 pPhoto->bMontageThis = FALSE;
	  }
   return cnt;
   }

#define JOBSEP_MAXCOLS  12
#define JOBSEP_COLWIDTH 25

void CPGPRCServer::DoSeparatorText(CSocketJob* pJob, CStdioFile& cf)
   {
   int i;
   int k;
   int xdest, ydest;
   char pTempBuf[PGPRC_MAXSTRLEN];
   char pTextCol[JOBSEP_MAXCOLS][PGPRC_MAXSTRLEN];
   int iCol;
   int iMaxCol;

   // defaults
   if (pJob->JobSeparator.iFontSize[0] == 0)
	  pJob->JobSeparator.iFontSize[0] = 20;
   if (pJob->JobSeparator.iFontSize[1] == 0)
	  pJob->JobSeparator.iFontSize[1] = 18;
   if (pJob->JobSeparator.iFontSize[2] == 0)
	  pJob->JobSeparator.iFontSize[2] = 14;

   // write the prolog	     
   WritePSStart(cf, PGPRC_SEPARATOR_BBOX_WIDTH, PGPRC_SEPARATOR_BBOX_HEIGHT, FALSE);
   // write the address   
   // the first line is bold
   fprintf(cf.m_pStream, "/Helvetica-Bold findfont [%d 0 0 %d 0 0] makefont setfont\r\n",
	  pJob->JobSeparator.iFontSize[0], pJob->JobSeparator.iFontSize[0]);
	  //fprintf(cf.m_pStream, "420 148 M\r\n");
      fprintf(cf.m_pStream, "340 148 M\r\n");
	  fprintf(cf.m_pStream, "(%s) S\r\n", 
		 CStringToPSString(pJob->JobSeparator.lpszAddress[0]));
   // next lines
   fprintf(cf.m_pStream, "/Helvetica findfont [%d 0 0 %d 0 0] makefont setfont\r\n",
	  pJob->JobSeparator.iFontSize[1], pJob->JobSeparator.iFontSize[1]);
   for (i = 1, ydest = 148 - pJob->JobSeparator.iFontSize[0]; 
        i < PGPRC_NUM_ADDRESS_LINES; i++, ydest -= (pJob->JobSeparator.iFontSize[1] + 2))
	  {
	  //fprintf(cf.m_pStream, "420 %d M\r\n", ydest);
	  fprintf(cf.m_pStream, "340 %d M\r\n", ydest);
	  fprintf(cf.m_pStream, "(%s) S\r\n", 
		 CStringToPSString(pJob->JobSeparator.lpszAddress[i]));
	  }
   // now do order info
   fprintf(cf.m_pStream, "/Helvetica-Bold findfont [%d 0 0 %d 0 0] makefont setfont\r\n",
	  pJob->JobSeparator.iFontSize[2], pJob->JobSeparator.iFontSize[2]);
   fprintf(cf.m_pStream, "100 160 M\r\n");
   fprintf(cf.m_pStream, "(Order Summary) S\r\n"); 
   
   fprintf(cf.m_pStream, "/Helvetica findfont [%d 0 0 %d 0 0] makefont setfont\r\n",
	  pJob->JobSeparator.iFontSize[2], pJob->JobSeparator.iFontSize[2]);
   for (iCol = 0; iCol < JOBSEP_MAXCOLS; iCol++)
	  {
	  for (i = 0, ydest = 160 - (pJob->JobSeparator.iFontSize[2] + 6); 
	  i < PGPRC_NUM_ORDERINFO_LINES; i++, ydest -= (pJob->JobSeparator.iFontSize[2] + 1))
		 {
		 for (k = 0; k < JOBSEP_MAXCOLS; k++)
		    ZeroMemory(pTextCol[k], PGPRC_MAXSTRLEN);

		 strcpy(pTempBuf, pJob->JobSeparator.lpszOrderInfo[i]);

		 int slen;
		 int iCnt;
		 for (slen = 0, k = 0, iCnt = 0; slen < strlen(pTempBuf); slen++)
			{
			if (pTempBuf[slen] == '\t') 			   
			   {			   
			   k++;
			   iCnt = 0;
			   }
			else
			   pTextCol[k][iCnt++] = pTempBuf[slen];
			}
		 // get the max number of columns
		 iMaxCol = k + 1;

		 // something here?
		 if (strlen(pTextCol[iCol]) > 0)
			{
		    xdest = 30 + iCol * JOBSEP_COLWIDTH;
		    fprintf(cf.m_pStream, "%d %d M\r\n", xdest, ydest);
			// right justify the last column
			if (iMaxCol == (iCol + 1))
			   {
			   fprintf(cf.m_pStream, "292 %d M\r\n", ydest);
			   fprintf(cf.m_pStream, "(%s) rstring\r\n", CStringToPSString(pTextCol[iCol]));
		       fprintf(cf.m_pStream, "(%s) S\r\n", CStringToPSString(pTextCol[iCol]));
			   }
			else
			   fprintf(cf.m_pStream, "(%s) S\r\n", CStringToPSString(pTextCol[iCol]));
			}
		 } 
	  }

   // draw ShipMethod
   //fprintf(cf.m_pStream, "670 63 M\r\n");
   //fprintf(cf.m_pStream, "(%s) rstring\r\n", CStringToPSString(pJob->JobSeparator.lpszShipMethod));
   //fprintf(cf.m_pStream, "(%s) S\r\n", CStringToPSString(pJob->JobSeparator.lpszShipMethod));
   fprintf(cf.m_pStream, "340 63 M\r\n");   
   fprintf(cf.m_pStream, "(%s) S\r\n", CStringToPSString(pJob->JobSeparator.lpszShipMethod));

   // draw boxes
   fprintf(cf.m_pStream, ".5 SL\r\n");
   // order box
   fprintf(cf.m_pStream, "25 50 M\r\n");
   fprintf(cf.m_pStream, "300 50 L\r\n");
   fprintf(cf.m_pStream, "300 178 L\r\n");
   fprintf(cf.m_pStream, "25 178 L\r\n");
   fprintf(cf.m_pStream, "25 50 L\r\n");
   fprintf(cf.m_pStream, "SF\r\n");

   // address box
   fprintf(cf.m_pStream, "322 50 M\r\n");
   fprintf(cf.m_pStream, "675 50 L\r\n");
   fprintf(cf.m_pStream, "675 178 L\r\n");
   fprintf(cf.m_pStream, "322 178 L\r\n");
   fprintf(cf.m_pStream, "322 50 L\r\n");
   fprintf(cf.m_pStream, "SF\r\n");
   }

void CPGPRCServer::TagPhotos(CSocketJob* pJob, CStdioFile& cf, int iStart, int iEnd, int iPage)
   {
   int i;
   int ydest;
   POSITION pos;
   POURPGPRCPHOTOSPECS pPhoto;
   int idx;
   CDevice* pcdTemp;
   MEDIA Media;
   int iLastMedia;
   CString csMedia;
   CString csTag;
   CZeusDoc* pDoc;
   CStringArray csaPhotoTags;
   int cnt;
   CString csMediaName;
   CString csTemp;

   // resolve the document
   pDoc = (CZeusDoc*)(pJob->hDevice);

   // for sanity
   ZeroMemory(&Media, sizeof (MEDIA));

   // set font   
   fprintf(cf.m_pStream, "/Helvetica findfont [14 0 0 14 0 0] makefont setfont\r\n");

   // make a temp device for this
   pcdTemp = new CDevice;

   // seed it with the QUEUEDEVICE
   *(pcdTemp) = QUEUEDEVICE;

   // nothing yet
   iLastMedia = -1;
   
   //for (pos = pJob->Photos.GetHeadPosition(), idx = 0; pos != NULL; idx++)
   for (pos = pJob->Photos.GetHeadPosition(), idx = 0; pos != NULL;)
	  {	  
	  // get the photo
	  pPhoto = (POURPGPRCPHOTOSPECS)pJob->Photos.GetNext(pos);		 
	  // check bounds
	  //if (idx < iStart || idx > iEnd) continue;
	  
	  if (!pPhoto->bPutInThumbnails) continue;

	  if (idx < iStart || idx > (iEnd - 1)) 
		 {
		 idx++;
		 continue;
		 }

	  idx++;

	  if (iLastMedia != pPhoto->PhotoSpecs.BasePhotoSpecs.iCanvasToUse)	 
         pcdTemp->GetMedia(pPhoto->PhotoSpecs.BasePhotoSpecs.iCanvasToUse, &Media);

	  csMediaName = pPhoto->PhotoSpecs.BasePhotoSpecs.lpszProductName;

	  //csTag.Format("%d - %s", pPhoto->PhotoSpecs.iCopies, csMediaName);
	  csTag.Format("%d - %s", pPhoto->iReportedCopyCount, csMediaName);	  

	  // add to the array, we should be writing tags
	  // same order
	  csaPhotoTags.Add(csTag);
	  						 
	  // store the last canvas idx
	  iLastMedia = pPhoto->PhotoSpecs.BasePhotoSpecs.iCanvasToUse;
	  }

   cnt = 0;

   // do the first two rows
   for (i = 0, ydest = 80; i < 5; i++, ydest += 130)
	  {
	  if (cnt < csaPhotoTags.GetSize())
		 {
	     csTag = csaPhotoTags.GetAt(cnt++);	  
	     fprintf(cf.m_pStream, "%d 355 M\r\n", ydest);
	     fprintf(cf.m_pStream, "(%s) cstring\r\n", CStringToPSString(csTag));   
	     fprintf(cf.m_pStream, "(%s) S\r\n", CStringToPSString(csTag));   
		 }
	  }
   for (i = 0, ydest = 80; i < 5; i++, ydest += 130)
	  {
	  if (cnt < csaPhotoTags.GetSize())
		 {
	     csTag = csaPhotoTags.GetAt(cnt++);	  	
		 fprintf(cf.m_pStream, "%d 245 M\r\n", ydest);
		 fprintf(cf.m_pStream, "(%s) cstring\r\n", CStringToPSString(csTag));   
		 fprintf(cf.m_pStream, "(%s) S\r\n", CStringToPSString(csTag));   
		 }
	  }

   if (iPage > 0)
	  {
	  for (i = 0, ydest = 80; i < 5; i++, ydest += 130)
		 {
		 if (cnt < csaPhotoTags.GetSize())
			{
			csTag = csaPhotoTags.GetAt(cnt++);	
			fprintf(cf.m_pStream, "%d 135 M\r\n", ydest);
			fprintf(cf.m_pStream, "(%s) cstring\r\n", CStringToPSString(csTag));   
			fprintf(cf.m_pStream, "(%s) S\r\n", CStringToPSString(csTag));   
			}
		 }
	  for (i = 0, ydest = 80; i < 5; i++, ydest += 130)
		 {
		 if (cnt < csaPhotoTags.GetSize())
			{
			csTag = csaPhotoTags.GetAt(cnt++);	 
			fprintf(cf.m_pStream, "%d 25 M\r\n", ydest);
			fprintf(cf.m_pStream, "(%s) cstring\r\n", CStringToPSString(csTag));   
			fprintf(cf.m_pStream, "(%s) S\r\n", CStringToPSString(csTag));   
			}
		 }
	  }
   csaPhotoTags.RemoveAll();
   delete pcdTemp;
   }

// this appears to hose the socket so we're not using it
void CPGPRCServer::CheckAwake()
   {
   int OptionValue;
   int OptionLen = sizeof OptionValue;
#ifdef RPDEBUGSTRMFC
   CString csTrace;
   char* p;
#endif

   // should we check this?
   if (!bCheckAwake) return;

   if (m_pListenSocket)
	  {
      m_pListenSocket->GetSockOpt(SO_ERROR, (void*)&OptionValue, 
	     &OptionLen);
#ifdef RPDEBUGSTRMFC
      csTrace.Format("CheckAwake = %d", OptionValue);
      p = (char*)(const char*)csTrace;								   
      RPDEBUGSTRMFC(p);
#endif
	  }
   else
	  OptionValue = -1;

   if (OptionValue != 0)
	  {
	  StopSocketPolling();
	  AfxSocketTerm();
	  // need this hack to make sure everythig is shut down
      _AFX_SOCK_STATE* pState = _afxSockState.GetData();
	  if (pState)
		 {
		 pState->m_pfnSockTerm = 0;
		 }
	  StartSocketPolling();
	  }
   }

void CPGPRCServer::ReportByteCount(BOOL bWrite, int iNum)
   {
#ifdef RPDEBUGSTRMFC
   CString cs;

   if (bWrite)	  
	  cs.Format("Server: Writing %d bytes", iNum);
   else
	  cs.Format("Server: Reading %d bytes", iNum);
   RPDEBUGSTRMFC(cs);
#endif
   }

int CPGPRCServer::GetRealPhotoCount(CSocketJob* pJob)
   {   
   int iCnt = 0;
   POSITION pos;
   POURPGPRCPHOTOSPECS pPhoto;

   for (pos = pJob->Photos.GetHeadPosition(); pos != NULL;)
	  {	  
	  // get the photo
	  pPhoto = (POURPGPRCPHOTOSPECS)pJob->Photos.GetNext(pos);		 
	  
	  if (!pPhoto->bPutInThumbnails) continue;

	  iCnt++;
	  }

   return iCnt;
   }

PGPRC_RETURNVAL CPGPRCServer::SetBackPrintLine(int len, HANDLE hDevice, int iLine, 
								   CClientSocket* pSocket)
   {
   CString csBackStr;
   LPSTR lpStr;
   PGPRC_RETURNVAL gRetVal = pgprc_returnok;
   CZeusDoc* pDoc;
   DEVBACKPRINTDATA DevBackPrintData;

   // capture the text
   lpStr = csBackStr.GetBuffer(PGPRC_MAXSTRLEN);   
   ZeroMemory(lpStr, PGPRC_MAXSTRLEN);
   pSocket->m_pArchiveIn->Read(lpStr, len);	  
   csBackStr.ReleaseBuffer();			   

   // resolve the document
   pDoc = (CZeusDoc*)(hDevice);

   // sanity check
   if (!pDoc)
	   return pgprc_error_invalid_device_handle;

   if (!(pDoc->IsKindOf(RUNTIME_CLASS(CZeusDoc)))) 
	   return pgprc_error_invalid_device_handle;

   ZeroMemory(&DevBackPrintData, sizeof DEVBACKPRINTDATA);
   strcpy(DevBackPrintData.cBackprintData, csBackStr);
   DevBackPrintData.iLineToSet = iLine;

   if (!QUEUEDEVICE.GetSetExtra(DEVGETSET_SETBACKPRINT, (BYTE*)&DevBackPrintData, NULL, 
	  sizeof DEVBACKPRINTDATA, 0))
		  gRetVal = pgprc_error_nodevsettingoroption;	  

   return gRetVal;					
   }

