////////////////////////////////////////////////////////////////////////////
// GISRPCSERVER.CPP
//                                                                        
// Photogize.PrintServer class for RasterPlus
// Copyright (C) 2005 Graphx, Inc.
//
// Created: April 6, 2005 by dje
//
// Mods:
//

#include "stdafx.h"

#include "zeus.h"
#include "zeusdoc.h"
#include "cqueue.h"
#include "queuechild.h"
#include "device.h"
#include "transfrm.h"
#include "jobseparator.h"   
#include "..\barcode\tecbarcode.h"
#include "AFXDB.H"
//#include "sockimpl.H"
#include "profile.h"

#include "Rpc.h"
#include "Rpcndr.h"

#include "GisRPCServer.h"


IMPLEMENT_DYNAMIC(CRPCJob, CObject)

#define QUEUEDEVICE    pDoc->QueueDevice
#define PANEARRAY      ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pPaneArray

#define SWAPINTS(a,b) {int iTempSwap;iTempSwap=a;a=b;b=iTempSwap;}

const UINT NEAR wm_AddToPane = RegisterWindowMessage( RIP_ADDTOPANE );

#define MISFIT_PENALTY    100000
#define PHOTOSENDBUFSIZE 32000
#define PGPRC_PS_BUF 32000

#define JOBSEP_MAXCOLS  12
#define JOBSEP_COLWIDTH 25

char cRetString[255];

typedef struct
{
	void* pDoc;
} GISRPC_ZEUSDOC;

typedef struct
{
	void* pDevice;
} GISRPC_DEVICE;

typedef struct
{
	void* pJob;
} GISRPC_JOB;

CObArray coDeviceArray;
CObArray coJobArray;


// RPC Job Stuff
//
CRPCJob::CRPCJob()
{
	bDoJobSeparator	= FALSE;
	iPhotoCounter	= 0;
	iTotalPages		= 0;
}

CRPCJob::~CRPCJob()
{
	// clean up the photos when the job is deleted 
	DeleteAllPhotos();
}			

void CRPCJob::DeleteAllPhotos()
{ 
	POSITION pos, temp;
	GISRPC_OURPGPRCPHOTOSPECS* pPhoto;   

	for (pos = Photos.GetHeadPosition(); pos != NULL;)
	{	  
		temp = pos;
		pPhoto = (GISRPC_OURPGPRCPHOTOSPECS*)Photos.GetNext(pos);

		// leave the queuefiles?
		CQueueFile* pCQueueFile = NULL;

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


CGISRPCServer::CGISRPCServer()
{
	char* pszProtocolSeq;
	
	pszProtocolSeq = new char[14];
	strcpy(pszProtocolSeq, "ncacn_ip_tcp");

	iCurrentVersion = 100;

	RPCPolling	= FALSE;

	RpcServerRegisterIf(GISRPCInterface_v1_0_s_ifspec, NULL, NULL );

	rpcStatus = RpcServerUseProtseq((unsigned char *)pszProtocolSeq, RPC_C_PROTSEQ_MAX_REQS_DEFAULT, NULL);

	switch ( rpcStatus )
	{
	case RPC_S_OK:
		break;
	case RPC_S_NO_PROTSEQS:
		break;
	case RPC_S_OUT_OF_MEMORY:
		break;
	case RPC_S_INVALID_SECURITY_DESC:
		break;
	}

	rpcStatus = RpcServerInqBindings(&rpcBindingVector);

	switch ( rpcStatus )
	{
	case RPC_S_OK:
		break;
	case RPC_S_NO_BINDINGS:
		break;
	}

	rpcStatus = RpcEpRegister(GISRPCInterface_v1_0_s_ifspec, rpcBindingVector, NULL, NULL);

	switch ( rpcStatus )
	{
	case RPC_S_OK:
		AfxMessageBox("RpcEpRegister = OK", MB_OK | MB_ICONEXCLAMATION);      
		break;
	case RPC_S_NO_BINDINGS:
		AfxMessageBox("RpcEpRegister = No Bindings", MB_OK | MB_ICONEXCLAMATION);      
		break;
	case RPC_S_INVALID_BINDING:
		AfxMessageBox("RpcEpRegister = Invalid Binding", MB_OK | MB_ICONEXCLAMATION);      
		break;
	case RPC_S_WRONG_KIND_OF_BINDING:
		AfxMessageBox("RpcEpRegister = Wrong Bindings", MB_OK | MB_ICONEXCLAMATION);      
		break;
	}

	delete pszProtocolSeq;
}

CGISRPCServer::~CGISRPCServer()
{
	// stop polling
	StopRPCPolling();
	
	FinalCleanUp();
}

void CGISRPCServer::StartRPCPolling()
{
	// are we already doing it?
	if (RPCPolling)
	   return;

	RPCPolling = TRUE;

	StartRPCServer();
}

void CGISRPCServer::StopRPCPolling()
{   
	RpcMgmtStopServerListening(rpcBindingVector);

	RPCPolling = FALSE;
}

void CGISRPCServer::StartRPCServer()
{   
	rpcStatus = RpcServerListen(1, 20, 0);

	return;
}

void CGISRPCServer::FinalCleanUp()
{
	// Check all arrays and delete as necessary
	//
	return;
}

void CGISRPCServer::CloseRPC()
{   
	return;
}

void CGISRPCServer::CloseAllRPC()
{   
	return;
}

extern "C"
CPaneElement* GISRPCGetRipPane(CRPCJob* pJob, int iViewStyle)
{
	int i;

	CPaneElement* pRipElement		= NULL;
	CPaneElement* pRipElementRet	= NULL;   

	CZeusDoc*		pDoc		= NULL;	
	CZeusDoc*		pPassedDoc	= NULL;	
	GISRPC_ZEUSDOC* pZeusDoc	= NULL;

	// resolve the document
	pZeusDoc	= (GISRPC_ZEUSDOC*)(pJob->hDevice);;
	pPassedDoc	= (CZeusDoc*)(pZeusDoc->pDoc);

	// sanity check
	if (!pPassedDoc)
		return NULL;

	if (!(pPassedDoc->IsKindOf(RUNTIME_CLASS(CZeusDoc)))) 
		return NULL;

	for (i = 0; i < PANEARRAY.GetSize(); i++)
	{
		pRipElement = (CPaneElement*)PANEARRAY.GetAt(i);

		if (!pRipElement)
			continue;

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


extern "C"
PGPRC_RETURNVAL GISRPCCreateQueueFiles(CRPCJob* pJob)
{
	POSITION pos;
	GISRPC_OURPGPRCPHOTOSPECS* pPhoto;
	PGPRC_RETURNVAL gRetVal = pgprc_returnok;
	CQueueFile* pCQueueFile;

	CString csFileName;

	CPaneElement* pRipElement;
	CSpecialTree* pcstPtr = NULL;

	for (pos = pJob->Photos.GetHeadPosition(); pos != NULL;)
	{	  	  
		pPhoto = (GISRPC_OURPGPRCPHOTOSPECS*)pJob->Photos.GetNext(pos);

		// a single photo on a new page
		if (pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcSizing != GISRPC_montage_crop &&
							pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcSizing != GISRPC_montage_fit)
		{					
			if (pJob->PGPRCPrintSpecs.bPrintToHold)
				pRipElement = GISRPCGetRipPane(pJob, QUEUESTYLE_HOLD);
			else
				pRipElement = GISRPCGetRipPane(pJob, QUEUESTYLE_PRINT);

			RPDEBUGSTRMFC("CreateQueueFiles = Get the proper Pane");

			// get special tree
			if (pRipElement)
				pcstPtr = (CSpecialTree*)pRipElement->pcstPtr;

			RPDEBUGSTRMFC("GISRPCCreateQueueFiles = Get the Queue File");

			// Get the Queue File
			if (pcstPtr)		 
			{
				pCQueueFile = pcstPtr->GetRPCQueueFile();	  
			}
			else
			{
				gRetVal = pgprc_error_outofmemory;		 			
				break;
			}

			// instantiate a queuefile 
			if (pCQueueFile == NULL)						
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
				csFileName = pPhoto->lpszTempDiskFile;
			else
				csFileName = pPhoto->PhotoSpecs.BasePhotoSpecs.lpszOutFile;

			if (!pCQueueFile->Init(csFileName))
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




extern "C"
PGPRC_RETURNVAL InsertJobs(CRPCJob* pJob)
{

	POSITION pos;

	PGPRC_RETURNVAL gRetVal = pgprc_returnok;

	GISRPC_OURPGPRCPHOTOSPECS* pPhoto = NULL;
	GISRPC_ZEUSDOC* pZeusDoc	= NULL;
	CZeusDoc* pDoc				= NULL;
	CPaneElement* pRipElement	= NULL;
	CQueueFile* pCQueueFile		= NULL;
	CSpecialTree* pcstPtr		= NULL;

	DEVBACKPRINTDATA DevBackPrintData;

	RPDEBUGSTRMFC("Insert Jobs");

	// loop through the photos
	for (pos = pJob->Photos.GetHeadPosition(); pos != NULL;)
	{	  	  
		// get the photo
		pPhoto = (GISRPC_OURPGPRCPHOTOSPECS*)pJob->Photos.GetNext(pos);

		RPDEBUGSTRMFC("Insert Jobs - Photos.GetNext(pos)");

		// get the queuefile pointer
		pCQueueFile = (CQueueFile*)pPhoto->hCqueueFile;

		// do we have a queue file?
		if (!pCQueueFile) 
			continue;

		if (!(pCQueueFile->IsKindOf(RUNTIME_CLASS(CQueueFile)))) 
			return pgprc_error_invalid_photo_handle;

		// resolve the document
		pZeusDoc = (GISRPC_ZEUSDOC*)pJob->hDevice;
		pDoc = (CZeusDoc*)(pZeusDoc->pDoc);

		RPDEBUGSTRMFC("Insert Jobs - Doc Resolved");

		// sanity check
		if (!pDoc)
			return pgprc_error_invalid_device_handle;
		if (!(pDoc->IsKindOf(RUNTIME_CLASS(CZeusDoc)))) 
			return pgprc_error_invalid_device_handle;

		// load the appropriate transform
		if ( !pCQueueFile->pTransform )
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

		RPDEBUGSTRMFC("Insert Jobs - New Queue Device");

		// start with the queue device
		if ( !pCQueueFile->pCD )
		{
			pCQueueFile->pCD = new CDevice;
			*(pCQueueFile->pCD) = QUEUEDEVICE;
		}

		pCQueueFile->Prefs |= USE_LOCAL_DEV_PREFS;

		// set the copies
		pCQueueFile->pCD->pDevInfo->Copies = pPhoto->PhotoSpecs.BasePhotoSpecs.iCopies;

		// force this off
		pCQueueFile->pCD->pDevInfo->OptimizeOrientation = FALSE;

		// set the canvas
		pCQueueFile->pCD->pDevInfo->CurrentMediaIdx = pPhoto->PhotoSpecs.BasePhotoSpecs.iCanvasToUse;

		RPDEBUGSTRMFC("Insert Jobs = Set Backprint");

		// set the backprint if it exists
		if (pPhoto->PhotoSpecs.cBackPrint1[0] != 0)
		{
			ZeroMemory(&DevBackPrintData, sizeof DEVBACKPRINTDATA);
			DevBackPrintData.pDevInfo = (BYTE*)(pCQueueFile->pCD->pDevInfo);
			strcpy(DevBackPrintData.cBackprintData, (char*)pPhoto->PhotoSpecs.cBackPrint1);
			DevBackPrintData.iLineToSet = 0;
			QUEUEDEVICE.GetSetExtra(DEVGETSET_SETBACKPRINT, (BYTE*)&DevBackPrintData, NULL, 
			sizeof DEVBACKPRINTDATA, 0);
		}

		if (pPhoto->PhotoSpecs.cBackPrint2[0] != 0)
		{
			ZeroMemory(&DevBackPrintData, sizeof DEVBACKPRINTDATA);
			DevBackPrintData.pDevInfo = (BYTE*)(pCQueueFile->pCD->pDevInfo);
			strcpy(DevBackPrintData.cBackprintData, (char*)pPhoto->PhotoSpecs.cBackPrint2);
			DevBackPrintData.iLineToSet = 1;
			QUEUEDEVICE.GetSetExtra(DEVGETSET_SETBACKPRINT, (BYTE*)&DevBackPrintData, NULL, 
			sizeof DEVBACKPRINTDATA, 0);
		}

		// print to hold?
		pCQueueFile->iPrintToHold = pJob->PGPRCPrintSpecs.bPrintToHold;

		// set the owner to be the orderid
		pCQueueFile->csOwner = pJob->PGPRCPrintSpecs.OrderIDString;

		// get the proper pane
		if (pCQueueFile->iPrintToHold)
			pRipElement = GISRPCGetRipPane(pJob, QUEUESTYLE_HOLD);
		else
			pRipElement = GISRPCGetRipPane(pJob, QUEUESTYLE_PRINT);

		RPDEBUGSTRMFC("Insert Jobs = Get the proper Pane");

		// get special tree
		if (pRipElement)
			pcstPtr = (CSpecialTree*)pRipElement->pcstPtr;

		RPDEBUGSTRMFC("Insert Jobs = Add the file");

		// add the file
		if (pcstPtr)		 
		{
			// add if it is a separator, or if it is a photo AND bPrintJobSeparatorOnly
			// is not set
			if (pPhoto->bIsSeparator || 
						(!pPhoto->bIsSeparator && !pJob->PGPRCPrintSpecs.bPrintJobSeparatorOnly))
			{
				ADDTOPANESTRUCT AddToPaneStruct;
				CQueueChild* pCQueueChild = NULL;
				pCQueueChild = pcstPtr->pCQueueChild;

				AddToPaneStruct.bDoSubPage	= FALSE;
				AddToPaneStruct.iSubPage	= 0;

				if ( pCQueueChild )
				{
					if (pCQueueFile->iPrintToHold)
					{
						AddToPaneStruct.iViewStyle = QUEUESTYLE_HOLD;
						::SendMessage(pCQueueChild->pHoldWnd->GetSafeHwnd(), 
									wm_AddToPane, (WPARAM)&AddToPaneStruct, (LPARAM)pCQueueFile);
					}
					else
					{
						AddToPaneStruct.iViewStyle = QUEUESTYLE_PRINT;
						::SendMessage(pCQueueChild->pPrintWnd->GetSafeHwnd(), 
									wm_AddToPane, (WPARAM)&AddToPaneStruct, (LPARAM)pCQueueFile);
					}
					pPhoto->bAddedToQueue = TRUE;
					pJob->iTotalPages++;
				}
			}
		}
		else
			return pgprc_error_cantfind_device_queue;
	}	

	RPDEBUGSTRMFC("Insert Jobs = All Done");

	return pgprc_returnok;
}

extern "C"
long GISRPCGetNumDevices(int* iNumDevices)
{ 
	// get the devices (which is actually the number of queues)
	*iNumDevices = PANEARRAY.GetSize() / 3;		   

	return 0;
}

extern "C"
long GISRPCGetDevInfo(int* iIdx, GISRPC_DEVINFO* pDevInfo, PCONTEXT_DOC_HANDLE* pphDevice)
{
	CPaneElement* pRipElement	= NULL;   
	CZeusDoc* pDoc				= NULL;
	GISRPC_ZEUSDOC* pZeusDoc	= NULL;

	DEVBACKPRINTDATA DevBackPrintData;
	
	CString csDeviceName;
	CString csNA;

	CString csRPDebugStr;
	char cDebugStr[255];

	// zero it out
	ZeroMemory(pDevInfo, sizeof GISRPC_DEVINFO);

	RPDEBUGSTRMFC("GetDevInfo - Start");

	// range check
	// squirt the data back
	if (*iIdx < 0 || *iIdx >= (PANEARRAY.GetSize() / 3)) 
	{				
		return gisrpc_error_nodevices;
	}   

	RPDEBUGSTRMFC("GetDevInfo - Found Some Devices");

	// load the n/a string
	csNA.LoadString(IDS_NOT_AVAIL);   

	csDeviceName = csNA;	

	// get the first pane in the queue
	pRipElement = (CPaneElement*)PANEARRAY.GetAt(*iIdx * 3);
	if (pRipElement)
	{
		RPDEBUGSTRMFC("GetDevInfo - Have a RipElement");

		// get the active document
		if (pDoc = (CZeusDoc*)(pRipElement->pDocPtr))
		{	                		 
			RPDEBUGSTRMFC("GetDevInfo - Have a Doc");

			pZeusDoc = (GISRPC_ZEUSDOC*)midl_user_allocate(sizeof(GISRPC_ZEUSDOC)); 
			pZeusDoc->pDoc = pDoc;
			*pphDevice = (PCONTEXT_DOC_HANDLE) pZeusDoc;

			csDeviceName = pDoc->csTitle;

			csRPDebugStr.Format("GevDevInfo DeviceName = %s\n", csDeviceName);
			strcpy(cDebugStr, csRPDebugStr);
			RPDEBUGSTRMFC(cDebugStr);

			strcpy((char*)pDevInfo->lpsLongDevName, csDeviceName);
			strcpy((char*)pDevInfo->lpsShortDevName, csDeviceName);
			strcpy((char*)pDevInfo->lpsDevLocation, csNA);
			strcpy((char*)pDevInfo->lpsSvrLocation, csNA);		   
			
			pDevInfo->bIsAvailable			= TRUE;
			pDevInfo->iNumBackPrintLines	= 0; // default

			ZeroMemory(&DevBackPrintData, sizeof DEVBACKPRINTDATA);

			csRPDebugStr.Format("GevDevInfo Backprint Lines = %d\n", pDevInfo->iNumBackPrintLines);
			strcpy(cDebugStr, csRPDebugStr);
			RPDEBUGSTRMFC(cDebugStr);

			csRPDebugStr.Format("GevDevInfo Orientation = %d\n", QUEUEDEVICE.pDevInfo->Orientation);
			strcpy(cDebugStr, csRPDebugStr);
			RPDEBUGSTRMFC(cDebugStr);

			// check device for actual backprint support
			if (QUEUEDEVICE.GetSetExtra(DEVGETSET_GETBACKPRINT, NULL,
							(BYTE*)&DevBackPrintData, 0, sizeof DEVBACKPRINTDATA))
			{
				csRPDebugStr.Format("GevDevInfo Backprint Lines = %d\n", pDevInfo->iNumBackPrintLines);
				strcpy(cDebugStr, csRPDebugStr);
				RPDEBUGSTRMFC(cDebugStr);

				pDevInfo->iNumBackPrintLines = DevBackPrintData.iNumBackprintLines;
			}

			csRPDebugStr.Format("GevDevInfo Backprint Lines = %d\n", pDevInfo->iNumBackPrintLines);
			strcpy(cDebugStr, csRPDebugStr);
			RPDEBUGSTRMFC(cDebugStr);

			pDevInfo->bCanDoCustomCanvas		= FALSE;	   		   
			QUEUEDEVICE.pDevInfo->TotalMedia	= QUEUEDEVICE.UpdateMediaList();
			pDevInfo->iNumCanvases				= QUEUEDEVICE.pDevInfo->TotalMedia;
			pDevInfo->iCurrentCanvas			= QUEUEDEVICE.pDevInfo->CurrentMediaIdx;
			pDevInfo->iDevType					= GISRPC_devtype_photoprinter;
			pDevInfo->bRollMedia				= FALSE;		   		  		   
			pDevInfo->iMaxLengthRollInches1000	= 0;		 
			pDevInfo->iMaxWidthRollInches1000	= 0;		 
			pDevInfo->iPrintsPerHour			= 0;
		}		 
	}   

	RPDEBUGSTRMFC("GetDevInfo - All Done");

	return gisrpc_returnok;
}



extern "C"
long GISRPCGetServerVersion(int* iVersion)
{
//	int iCurrentVersion = GISRPC_CURRENT_VERSION;

	*iVersion = GISRPC_CURRENT_VERSION;

	return 0;
}

extern "C"
long GISRPCGetCanvasInfo(PCONTEXT_DOC_HANDLE phDevice, int* idx, GISRPC_CANVAS* GISCanvas)
{
	CZeusDoc* pDoc;      
	CString csNA;
	MEDIA Media;

	GISRPC_ZEUSDOC* pZeusDoc;

	// load the n/a string
	csNA.LoadString(IDS_NOT_AVAIL);

	// resolve the document
	pZeusDoc = (GISRPC_ZEUSDOC*)phDevice;
	pDoc = (CZeusDoc*)(pZeusDoc->pDoc);

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
	QUEUEDEVICE.GetMedia(*idx, &Media);

	// get the dims of the canvas
	if (QUEUEDEVICE.pDevInfo->Orientation == DEV_ORIENT_PORTRAIT)
	{
		GISCanvas->iWidthInches1000		= abs(Media.Rect.right - Media.Rect.left) * 10;
		GISCanvas->iHeightInches1000	= abs(Media.Rect.top - Media.Rect.bottom) * 10;
	}
	else
	{
		GISCanvas->iHeightInches1000	= abs(Media.Rect.right - Media.Rect.left) * 10;
		GISCanvas->iWidthInches1000		= abs(Media.Rect.top - Media.Rect.bottom) * 10;
	}							 

	// copy the data to the canvas struct
	strcpy((char*)GISCanvas->lpsCanvasName, Media.Name);
	strcpy((char*)GISCanvas->lpsCanvasDesc, Media.Description);

	GISCanvas->iStyle = PGPRC_CANVAS_NOSTYLE;

//	ReportByteCount(TRUE, sizeof PGPRCCANVAS);
	
	return pgprc_returnok;
}

extern "C"
long GISRPCSetBackPrintLine(PCONTEXT_DOC_HANDLE phDevice, int* iLine )
{
	CString csBackStr;
	LPSTR lpStr;
	
	PGPRC_RETURNVAL gRetVal = pgprc_returnok;

	CZeusDoc* pDoc;
	GISRPC_ZEUSDOC* pZeusDoc;

	DEVBACKPRINTDATA DevBackPrintData;

	// capture the text
	lpStr = csBackStr.GetBuffer(PGPRC_MAXSTRLEN);   
	ZeroMemory(lpStr, PGPRC_MAXSTRLEN);
	csBackStr.ReleaseBuffer();			   

	// resolve the document
	pZeusDoc = (GISRPC_ZEUSDOC*)phDevice;
	pDoc = (CZeusDoc*)(pZeusDoc->pDoc);

	// sanity check
	if (!pDoc)
		return pgprc_error_invalid_device_handle;

	if (!(pDoc->IsKindOf(RUNTIME_CLASS(CZeusDoc)))) 
		return pgprc_error_invalid_device_handle;

	ZeroMemory(&DevBackPrintData, sizeof DEVBACKPRINTDATA);
	strcpy(DevBackPrintData.cBackprintData, csBackStr);
	DevBackPrintData.iLineToSet = *iLine;

	if (!QUEUEDEVICE.GetSetExtra(DEVGETSET_SETBACKPRINT, (BYTE*)&DevBackPrintData, NULL, 
						sizeof DEVBACKPRINTDATA, 0))
		gRetVal = pgprc_error_nodevsettingoroption;	  

	return gRetVal;					
}

extern "C"
long GISRPCGetDevOptionName(PCONTEXT_DOC_HANDLE phDevice, int* iOption, GISRPC_DEVSTRING* pOptionName)
{
	PGPRC_RETURNVAL gRetVal = pgprc_returnok;
	char pTemp[PGPRC_MAXSTRLEN];
	GETDEVOPTIONSETTINGKEY GetDevOptionSettingKey;

	CZeusDoc* pDoc;
	GISRPC_ZEUSDOC* pZeusDoc;

	GetDevOptionSettingKey.iOption = *iOption;

	// resolve the document
	pZeusDoc = (GISRPC_ZEUSDOC*)phDevice;
	pDoc = (CZeusDoc*)(pZeusDoc->pDoc);

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
	{
		gRetVal = pgprc_error_nodevsettingoroption;
	}
	else
	{
		strcpy((char*)pOptionName, pTemp);
	}

	return gRetVal;
}


extern "C"
long GISRPCGetDevOption(PCONTEXT_DOC_HANDLE phDevice, int* iOption, int* iSetting)
{
	PGPRC_RETURNVAL gRetVal = pgprc_returnok;
	GETDEVOPTIONSETTINGKEY GetDevOptionSettingKey;   

	CZeusDoc* pDoc;
	GISRPC_ZEUSDOC* pZeusDoc;

	// load the option and the setting
	GetDevOptionSettingKey.iOption = *iOption;
	GetDevOptionSettingKey.iSetting = 0;

	// resolve the document
	pZeusDoc = (GISRPC_ZEUSDOC*)phDevice;
	pDoc = (CZeusDoc*)(pZeusDoc->pDoc);

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
	{
		gRetVal = pgprc_error_nodevsettingoroption;	  
	}

	return gRetVal;
}

extern "C"
long GISRPCSetDevOption(PCONTEXT_DOC_HANDLE phDevice, int* iOption, int* iSetting)
{
	PGPRC_RETURNVAL gRetVal = pgprc_returnok;
	GETDEVOPTIONSETTINGKEY GetDevOptionSettingKey;   

	CZeusDoc* pDoc;
	GISRPC_ZEUSDOC* pZeusDoc;

	CString csToken;
	CString csValue;

	GetDevOptionSettingKey.iOption = *iOption;
	GetDevOptionSettingKey.iSetting = *iSetting;

	// resolve the document
	pZeusDoc = (GISRPC_ZEUSDOC*)phDevice;
	pDoc = (CZeusDoc*)(pZeusDoc->pDoc);

	// sanity check
	if (!pDoc)
		return pgprc_error_invalid_device_handle;

	if (!QUEUEDEVICE.GetSetExtra(DEVGETSET_SETDEVOPTION, (BYTE*)&GetDevOptionSettingKey, NULL, 
																	sizeof GETDEVOPTIONSETTINGKEY, 0))
	{
		gRetVal = pgprc_error_nodevsettingoroption;
	}
	else
	{
		// reload our options
		QUEUEDEVICE.GetDevInfo();
	}

	return gRetVal;					
}

extern "C"
long GISRPCGetDevOptionSettingName(PCONTEXT_DOC_HANDLE phDevice, int* iOption, int* iSetting, GISRPC_DEVSTRING* pSettingName)
{
	PGPRC_RETURNVAL gRetVal = pgprc_returnok;
	char pTemp[PGPRC_MAXSTRLEN];
	GETDEVOPTIONSETTINGKEY GetDevOptionSettingKey;
	CZeusDoc* pDoc;
	GISRPC_ZEUSDOC* pZeusDoc;

	// load the option and the setting
	GetDevOptionSettingKey.iOption = *iOption;
	GetDevOptionSettingKey.iSetting = *iSetting;

	// resolve the document
	pZeusDoc = (GISRPC_ZEUSDOC*)phDevice;
	pDoc = (CZeusDoc*)(pZeusDoc->pDoc);

	// sanity check
	if (!pZeusDoc)
	{	  
		return pgprc_error_invalid_device_handle;
	}

	// call the device proc
	if (!QUEUEDEVICE.GetSetExtra(DEVGETSET_GETDEVOPTIONSETTINGNAME, (BYTE*)&GetDevOptionSettingKey, 
												(BYTE*)pTemp, sizeof GETDEVOPTIONSETTINGKEY, PGPRC_MAXSTRLEN))
	{
		gRetVal = pgprc_error_nodevsettingoroption;
	}
	else
	{
		strcpy((char*)pSettingName, pTemp);
	}


	return gRetVal;
}


extern "C"
long GISRPCOpenJob(PCONTEXT_DOC_HANDLE phDevice, unsigned char* jobName, PCONTEXT_JOB_HANDLE* phJob)
{
	GISRPC_JOB* pZeusJob;

	CRPCJob* pJob;   
	LPSTR lpStr;

	// create a new job
	if (!(pJob = new CRPCJob))
	{
		return pgprc_error_outofmemory;   
	}

	// capture the name of the job
	lpStr = pJob->csName.GetBuffer(PGPRC_MAXSTRLEN);   
	ZeroMemory(lpStr, PGPRC_MAXSTRLEN);
	strcpy((char*)lpStr, (const char*)jobName);

	pJob->csName.ReleaseBuffer();

	RPDEBUGSTRMFC("Open Job");

	// save the device
	pJob->hDevice = phDevice;
	coJobArray.Add(pJob);

	pZeusJob = (GISRPC_JOB*)midl_user_allocate(sizeof(GISRPC_JOB)); 
	pZeusJob->pJob = pJob;
	*phJob = (PCONTEXT_JOB_HANDLE) pZeusJob;

	return pgprc_returnok;
}


extern "C"
long GISRPCCloseJob(PCONTEXT_JOB_HANDLE hJob)
{
	GISRPC_JOB* pZeusJob;

	int i = 0;

	CRPCJob* pJob;
	CRPCJob* pArrayJob;

	// resolve the job
	pZeusJob = (GISRPC_JOB*)hJob;
	pJob = (CRPCJob*)(pZeusJob->pJob);

	// sanity check
	if (!pJob)
		return pgprc_error_invalid_job_handle;

	if (!(pJob->IsKindOf(RUNTIME_CLASS(CRPCJob)))) 
	{	  
		return pgprc_error_invalid_job_handle;
	}

	// find the job and destroy it
	while (i < coJobArray.GetSize())
	{
		pArrayJob = (CRPCJob*)coJobArray.GetAt(i);
		if (pArrayJob == pJob)
		{
			coJobArray.RemoveAt(i);
			midl_user_free((GISRPC_JOB*)pZeusJob);
			delete pArrayJob;
			break;
		}
		i++;
	}

	return pgprc_returnok;
}



extern "C"
long GISRPCComputeMontagePhotosVars(CRPCJob* pJob, int& iPage, 
											   int iMontageCanvasToUse, BOOL bOverrideMisFit)
{
	POSITION pos;
	GISRPC_OURPGPRCPHOTOSPECS* pPhoto;
	PGPRC_RETURNVAL gRetVal = pgprc_returnok;
	CPtrArray coPhotosToMontage;
	PGPRCCANVAS PGPRCCanvas;   
	int iRunningWidthInches1000 = 0;
	int iRunningHeightInches1000 = 0;
	int iMaxSwathHeight = 0;  
	int Save_iWidthInches1000	= 0;
	int Save_iHeightInches1000	= 0;

	CZeusDoc* pDoc;
	GISRPC_ZEUSDOC* pZeusDoc;

	MEDIA Media;

	// resolve the document
	pZeusDoc = (GISRPC_ZEUSDOC*)pJob->hDevice;
	pDoc = (CZeusDoc*)(pZeusDoc->pDoc);

	// sanity check
	if (!pDoc)
		return pgprc_error_invalid_device_handle;

	if (!(pDoc->IsKindOf(RUNTIME_CLASS(CZeusDoc)))) 
		return pgprc_error_invalid_device_handle;

	// look for all montageable photos
	for (pos = pJob->Photos.GetHeadPosition(); pos != NULL;)
	{	  	  		 
		// get the photo
		pPhoto = (GISRPC_OURPGPRCPHOTOSPECS*)pJob->Photos.GetNext(pos);

		// should we?
		if (!pPhoto->bMontageThis)
			continue;	  

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
			else
			{
				if ((iRunningWidthInches1000 + pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000) >
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
	}

	if ( bOverrideMisFit )
	{
		// Restore Original Size
		pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000		= Save_iWidthInches1000;
		pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000	= Save_iHeightInches1000;
	}

	return gRetVal;
}			  



extern "C"
long GISRPCSwapCustomPageDims(CRPCJob* pJob)
{
	POSITION pos;
	GISRPC_OURPGPRCPHOTOSPECS* pPhoto;
	PGPRC_RETURNVAL gRetVal = pgprc_returnok;

	// look for all montageable photos
	for (pos = pJob->Photos.GetHeadPosition(); pos != NULL;)
	{	  	  		 
		// get the photo
		pPhoto = (GISRPC_OURPGPRCPHOTOSPECS*)pJob->Photos.GetNext(pos);

		SWAPINTS(pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000, 
		pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000);
	}

	return gRetVal;
}



extern "C"
long GISRPCResetMontageFlags(CRPCJob* pJob)
{
	POSITION pos;
	GISRPC_OURPGPRCPHOTOSPECS* pPhoto;
	PGPRC_RETURNVAL gRetVal = pgprc_returnok;

	for (pos = pJob->Photos.GetHeadPosition(); pos != NULL;)
	{	  	  				 
		pPhoto = (GISRPC_OURPGPRCPHOTOSPECS*)pJob->Photos.GetNext(pos);
		pPhoto->bMontageThis = FALSE;
		pPhoto->bMontageDone = FALSE;
	}

	return gRetVal;
}


extern "C"
long GISRPCCheckNextMontageSet(CRPCJob* pJob, int& iMontageCanvasToUse)
{
	POSITION pos;
	GISRPC_OURPGPRCPHOTOSPECS* pPhoto;
	int cnt = 0;   

	for (pos = pJob->Photos.GetHeadPosition(); pos != NULL;)
	{	  	  				 
		pPhoto = (GISRPC_OURPGPRCPHOTOSPECS*)pJob->Photos.GetNext(pos);

		// if the file is to be montaged AND has not been montaged...
		if ((pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcSizing == GISRPC_montage_crop ||
				pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcSizing == GISRPC_montage_fit) && !pPhoto->bMontageDone)
		{
			// set the canvas on the first file
			if (cnt == 0)
			{
				iMontageCanvasToUse = pPhoto->PhotoSpecs.BasePhotoSpecs.iCanvasToUse;	
				// if not a separator, and bPrintJobSeparatorOnly is set, don't montage
				if (!pPhoto->bIsSeparator && pJob->PGPRCPrintSpecs.bPrintJobSeparatorOnly)
				{
					pPhoto->bMontageThis = FALSE;
				}
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
					{
						pPhoto->bMontageThis = FALSE;
					}
					else
					{
						pPhoto->bMontageThis = TRUE;
						cnt++;
					}
				}
			}	 
		}		 
		else
		{
			pPhoto->bMontageThis = FALSE;
		}
	}
	return cnt;
}


extern "C"
long GISRPCWritePSStart(CStdioFile& cf, int iMediaWidthPica, 
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





extern "C"
long GISRPCWritePSStartN(CStdioFile& cf, int iMediaWidthPica, int iMediaHeightPica)
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
}


extern "C"
long GISRPCResizeMontageBitmap(pBITMAPHANDLE pPhotoBitmap, GISRPC_OURPGPRCPHOTOSPECS* pPhoto)
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

	if (pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcSizing == GISRPC_montage_crop)    
	{
		// now compute the trim for cropping
		if (fDevAspect < fBitmapAspect)
		{
			iNewHeight = (int)((float)pPhotoBitmap->Width * fDevAspect + .5F);
			if (L_TrimBitmap(pPhotoBitmap, 0, (pPhotoBitmap->Height - iNewHeight) / 2,
													pPhotoBitmap->Width, iNewHeight) != SUCCESS)
				return FALSE;															   
		}
		else
		{
			if (fDevAspect > fBitmapAspect)
			{
				iNewWidth = (int)((float)pPhotoBitmap->Height / fDevAspect + .5F);
				if (L_TrimBitmap(pPhotoBitmap, (pPhotoBitmap->Width - iNewWidth) / 2, 0,
				iNewWidth, pPhotoBitmap->Height) != SUCCESS)
				return FALSE;															   
			}
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
		else
		{
			if (fDevAspect > fBitmapAspect)
			{		 
			pPhoto->iYoffInches1000 += (int)
					(((float)pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000 - 
						(float)pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000 * fBitmapAspect) / 2. + .5);
			pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000 -= (int)
					(((float)pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000 - 
						(float)pPhoto->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000 * fBitmapAspect) + .5);
			}
		}
	}

	return bRet;
}


extern "C"
long GISRPCWritePSMontagePS(CStdioFile& cf, CRPCJob* pJob, int iDestWidthPica, int iDestHeightPica, 
										   int iDestXOffPica, int iDestYOffPica, 
										   GISRPC_OURPGPRCPHOTOSPECS* pPhoto)
{
	CFile cfIn
		;
	PGPRC_RETURNVAL gRetVal = pgprc_returnok;

	char* lpFileName	= NULL;				  
	BYTE* pBuf			= NULL;

	int iNumRead	= 0;
	float fxScale	= 0;
	float fyScale	= 0;

	// get scale factors
//	fxScale = (float)iDestWidthPica / fSeparatorBBox_Width;
//	fyScale = (float)iDestHeightPica / fSeparatorBBox_Height;

	// get name
	if (pPhoto->bUseTempFile)
		lpFileName = (char*)pPhoto->lpszTempDiskFile;
	else
		lpFileName = (char*)pPhoto->PhotoSpecs.BasePhotoSpecs.lpszOutFile;

	// make a buf
	if (!(pBuf = new BYTE [PGPRC_PS_BUF]))
	{
		RPDEBUGSTRMFC("PGPRCServer: pBuf = new BYTE [PGPRC_PS_BUF]) bad");
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
//				fxScale = (float)iDestWidthPica / fSeparatorBBox_Height;
//				fyScale = (float)iDestHeightPica / fSeparatorBBox_Width;
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





extern "C"
long GISRPCWritePSMontageBitmap(CStdioFile& cf, CRPCJob* pJob, int iDestWidthPica, int iDestHeightPica, 
									int iDestXOffPica, int iDestYOffPica, BITMAPHANDLE PhotoBitmap)
{
	int i;
	BYTE* pData; 
	PGPRC_RETURNVAL gRetVal = pgprc_returnok;

	// write the bitmap to the eps file
	fprintf(cf.m_pStream, "GS /picstr %d string def\r\n", PhotoBitmap.Width * 3);	   
	fprintf(cf.m_pStream, "%d %d translate %d %d scale\r\n", iDestXOffPica, iDestYOffPica, iDestWidthPica, iDestHeightPica);
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
		RPDEBUGSTRMFC("PGPRCServer: WritePSMontageBitmap, pgprc_error_outofmemory");
		gRetVal = pgprc_error_outofmemory;
	}

	// release the bitmap
	L_ReleaseBitmap(&PhotoBitmap);

	return gRetVal;
}


extern "C"
long GISRPCWritePSMontageEnd(CStdioFile& cf)
{
	// write the end
	fprintf(cf.m_pStream, "gsave showpage grestore\r\n");   

	return pgprc_returnok;
}



extern "C"
long GISRPCCreateMontagePhoto(CRPCJob* pJob, CPtrArray& coPhotosToMontage)
{
	int i;
	PGPRC_RETURNVAL gRetVal = pgprc_returnok;
	GISRPC_OURPGPRCPHOTOSPECS* pPhoto;      
	RIPSTRUCT RipStruct;   
	BITMAPHANDLE PhotoBitmap;
	MEDIA Media;
	CDevice* pcdTemp;
	
	CZeusDoc* pDoc;
	GISRPC_ZEUSDOC* pZeusDoc;

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

	// got milk?
	if (coPhotosToMontage.GetSize() < 1) 
	{	  
		return gRetVal;	  
	}

	// get the first photo;
	// assume that all the photos are using the same device settings
	pPhoto = (GISRPC_OURPGPRCPHOTOSPECS*)coPhotosToMontage.GetAt(0);

	// resolve the document
	pZeusDoc = (GISRPC_ZEUSDOC*)(pJob->hDevice);
	pDoc = (CZeusDoc*)(pZeusDoc->pDoc);

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
		RPDEBUGSTRMFC("PGPRCServer: can't open pTempFileName");
		return pgprc_error_cant_create_montage;
	}

	// write the beginning of the PS file
	gRetVal = (PGPRC_RETURNVAL)GISRPCWritePSStart(cf, iMediaWidthPica, iMediaHeightPica, TRUE);

	if (gRetVal != pgprc_returnok) 
	{
		cf.Close();
		// we're done with the device
		delete pcdTemp;				   
		RPDEBUGSTRMFC("PGPRCServer: can't write WritePSStart");
		return gRetVal;
	}			 

	// now blit all the photos into the montage bitmap
	for (i = 0; i < coPhotosToMontage.GetSize(); i++)
	{
		// get the photo
		pPhoto = (GISRPC_OURPGPRCPHOTOSPECS*)coPhotosToMontage.GetAt(i); 

		// load the photo
		if (pPhoto->FileType != FILE_EPS)
		{
			if (L_LoadBitmap(pPhoto->bUseTempFile ? (char*)pPhoto->lpszTempDiskFile : 
							(char*)pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile,
								&PhotoBitmap, 0, ORDER_RGB, NULL, NULL) != SUCCESS)
			{
				gRetVal = pgprc_error_cant_create_montage;
				break;
			}	  			

			if (!GISRPCResizeMontageBitmap(&PhotoBitmap, pPhoto))
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
			gRetVal = (PGPRC_RETURNVAL)GISRPCWritePSMontagePS(cf, pJob, iDestWidthPica, iDestHeightPica, iDestXOffPica, iDestYOffPica, pPhoto);
		else
			gRetVal = (PGPRC_RETURNVAL)GISRPCWritePSMontageBitmap(cf, pJob, iDestWidthPica, iDestHeightPica, iDestXOffPica, iDestYOffPica, PhotoBitmap);	  

		if (gRetVal != pgprc_returnok)
		{
			RPDEBUGSTRMFC("PGPRCServer: can't WritePSMontagePS or WritePSMontageBitmap");
			break;
		}

		// free the photo bitmap
		if (pPhoto->FileType != FILE_EPS)
			L_FreeBitmap(&PhotoBitmap);	 	  
	}																	   

	// write the end of the ps file
	GISRPCWritePSMontageEnd(cf);

	// close the file
	cf.Close();

	// instantiate a queuefile 
	if ((pCQueueFile = new CQueueFile) == NULL)							  
	{
		// we're done with the device
		delete pcdTemp;				   
		RPDEBUGSTRMFC("PGPRCServer: can't pCQueueFile = new CQueueFile");
		return pgprc_error_outofmemory;	
	}

	// init the queue file
	if (!pCQueueFile->Init(pTempFileName))
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
	pCQueueFile->Description.Format("%s Photogize Montage with %d Photos", pJob->csName, coPhotosToMontage.GetSize());   

	// now create a new photo and add it to the list
	if (pPhoto = new GISRPC_OURPGPRCPHOTOSPECS)
	{
		// copy over the photo specs for the first photo
		memcpy(pPhoto, (GISRPC_OURPGPRCPHOTOSPECS*)coPhotosToMontage.GetAt(0), sizeof GISRPC_OURPGPRCPHOTOSPECS);
		pPhoto->bUseTempFile = FALSE;
		strcpy((char*)pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile, pTempFileName);
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
		{
			// otherwise, add to the photo array
			pJob->Photos.AddTail(pPhoto);
		}

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


extern "C"
long GISRPCMontagePhotos(CRPCJob* pJob)
{
	POSITION pos;
	GISRPC_OURPGPRCPHOTOSPECS* pPhoto;
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
				pPhoto = (GISRPC_OURPGPRCPHOTOSPECS*)pJob->Photos.GetNext(pos);

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
			gRetVal = (PGPRC_RETURNVAL)GISRPCCreateMontagePhoto(pJob, coPhotosToMontage);
			// break on error
			if (gRetVal != pgprc_returnok)
				break;

			iHarvestedPage++;

		} while (iMontageCount > 0);

	return gRetVal;
}


extern "C"
long GISRPCGetRealPhotoCount(CRPCJob* pJob)
{   
	int iCnt = 0;
	POSITION pos;
	GISRPC_OURPGPRCPHOTOSPECS* pPhoto;

	for (pos = pJob->Photos.GetHeadPosition(); pos != NULL;)
	{	  
		// get the photo
		pPhoto = (GISRPC_OURPGPRCPHOTOSPECS*)pJob->Photos.GetNext(pos);		 

		if (!pPhoto->bPutInThumbnails)
			continue;

		iCnt++;
	}

	return iCnt;
}







extern "C"
void GISRPCGetTotalSeparatorPages(CRPCJob* pJob)
{
	int iPhotosAfterFirstPage;

	pJob->iNumSeparatorPages = 0;

	if (pJob->iPhotoCounter <= (pJob->JobSeparator.iContactImage_NumAccross * pJob->JobSeparator.iContactImage_NumDown_Page_1))
		pJob->iNumSeparatorPages = 1;
	else
	{
		// get the number of pages after the first page
		//iPhotosAfterFirstPage = pJob->iPhotoCounter - pJob->JobSeparator.iContactImage_NumAccross * pJob->JobSeparator.iContactImage_NumDown_Page_1;		 
		iPhotosAfterFirstPage = GISRPCGetRealPhotoCount(pJob) - (pJob->JobSeparator.iContactImage_NumAccross * pJob->JobSeparator.iContactImage_NumDown_Page_1);		 	  

		// divide the photos left by the number per page
		pJob->iNumSeparatorPages = iPhotosAfterFirstPage / (pJob->JobSeparator.iContactImage_NumAccross * pJob->JobSeparator.iContactImage_NumDown_Page_N);

		// look for a remainder
		if (iPhotosAfterFirstPage % pJob->JobSeparator.iContactImage_NumDown_Page_N)
			pJob->iNumSeparatorPages++;
	}   
}


extern "C"
char* GISRPCCStringToPSString(LPCSTR lpStrIn)
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

	strcpy((char*)cRetString, cs);
	return cRetString;
}


extern "C"
void GISRPCDoSeparatorText(CRPCJob* pJob, CStdioFile& cf)
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
	GISRPCWritePSStart(cf, pJob->JobSeparator.iSeparatorBBox_Width, pJob->JobSeparator.iSeparatorBBox_Height, FALSE);

	// write the address   
	// the first line is bold
	fprintf(cf.m_pStream, "/Helvetica-Bold findfont [%d 0 0 %d 0 0] makefont setfont\r\n",
								pJob->JobSeparator.iFontSize[0], pJob->JobSeparator.iFontSize[0]);

	//fprintf(cf.m_pStream, "420 148 M\r\n");
	fprintf(cf.m_pStream, "340 148 M\r\n");
	fprintf(cf.m_pStream, "(%s) S\r\n", 
	GISRPCCStringToPSString((char*)pJob->JobSeparator.lpszAddress[0]));
	
	// next lines
	fprintf(cf.m_pStream, "/Helvetica findfont [%d 0 0 %d 0 0] makefont setfont\r\n",
								pJob->JobSeparator.iFontSize[1], pJob->JobSeparator.iFontSize[1]);

	for (i = 1, ydest = 148 - pJob->JobSeparator.iFontSize[0]; 
			i < PGPRC_NUM_ADDRESS_LINES; i++, ydest -= (pJob->JobSeparator.iFontSize[1] + 2))
	{
		//fprintf(cf.m_pStream, "420 %d M\r\n", ydest);
		fprintf(cf.m_pStream, "340 %d M\r\n", ydest);
		fprintf(cf.m_pStream, "(%s) S\r\n", 
		GISRPCCStringToPSString((char*)pJob->JobSeparator.lpszAddress[i]));
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

			strcpy(pTempBuf, (char*)pJob->JobSeparator.lpszOrderInfo[i]);

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
					fprintf(cf.m_pStream, "(%s) rstring\r\n", GISRPCCStringToPSString(pTextCol[iCol]));
					fprintf(cf.m_pStream, "(%s) S\r\n", GISRPCCStringToPSString(pTextCol[iCol]));
				}
				else
					fprintf(cf.m_pStream, "(%s) S\r\n", GISRPCCStringToPSString(pTextCol[iCol]));
			}
		} 
	}

	// draw ShipMethod
	//fprintf(cf.m_pStream, "670 63 M\r\n");
	//fprintf(cf.m_pStream, "(%s) rstring\r\n", GISRPCCStringToPSString(pJob->JobSeparator.lpszShipMethod));
	//fprintf(cf.m_pStream, "(%s) S\r\n", GISRPCCStringToPSString(pJob->JobSeparator.lpszShipMethod));
	fprintf(cf.m_pStream, "340 63 M\r\n");   
	fprintf(cf.m_pStream, "(%s) S\r\n", GISRPCCStringToPSString((char*)pJob->JobSeparator.lpszShipMethod));

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





extern "C"
void GISRPCTagPhotos(CRPCJob* pJob, CStdioFile& cf, int iStart, int iEnd, int iPage)
{
	int i			= 0;
	int ydest		= 0;
	int idx			= 0;
	int iLastMedia	= 0;
	int cnt			= 0;

	POSITION pos	= NULL;
	
	GISRPC_OURPGPRCPHOTOSPECS* pPhoto	= NULL;
	GISRPC_ZEUSDOC* pZeusDoc			= NULL;

	CDevice* pcdTemp	= NULL;
	CZeusDoc* pDoc		= NULL;
	
	MEDIA Media;
	
	CString csMedia;
	CString csTag;
	CString csMediaName;
	CString csTemp;

	CStringArray csaPhotoTags;

	// resolve the document
	pZeusDoc	= (GISRPC_ZEUSDOC*)(pJob->hDevice);;
	pDoc = (CZeusDoc*)(pZeusDoc->pDoc);

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
		pPhoto = (GISRPC_OURPGPRCPHOTOSPECS*)pJob->Photos.GetNext(pos);		 

		if (!pPhoto->bPutInThumbnails)
			continue;

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
			fprintf(cf.m_pStream, "(%s) cstring\r\n", GISRPCCStringToPSString(csTag));   
			fprintf(cf.m_pStream, "(%s) S\r\n", GISRPCCStringToPSString(csTag));   
		}
	}

	for (i = 0, ydest = 80; i < 5; i++, ydest += 130)
	{
		if (cnt < csaPhotoTags.GetSize())
		{
			csTag = csaPhotoTags.GetAt(cnt++);	  	
			fprintf(cf.m_pStream, "%d 245 M\r\n", ydest);
			fprintf(cf.m_pStream, "(%s) cstring\r\n", GISRPCCStringToPSString(csTag));   
			fprintf(cf.m_pStream, "(%s) S\r\n", GISRPCCStringToPSString(csTag));   
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
				fprintf(cf.m_pStream, "(%s) cstring\r\n", GISRPCCStringToPSString(csTag));   
				fprintf(cf.m_pStream, "(%s) S\r\n", GISRPCCStringToPSString(csTag));   
			}
		}

		for (i = 0, ydest = 80; i < 5; i++, ydest += 130)
		{
			if (cnt < csaPhotoTags.GetSize())
			{
				csTag = csaPhotoTags.GetAt(cnt++);	 
				fprintf(cf.m_pStream, "%d 25 M\r\n", ydest);
				fprintf(cf.m_pStream, "(%s) cstring\r\n", GISRPCCStringToPSString(csTag));   
				fprintf(cf.m_pStream, "(%s) S\r\n", GISRPCCStringToPSString(csTag));   
			}
		}
	}

	csaPhotoTags.RemoveAll();
	delete pcdTemp;
}


extern "C"
long GISRPCDoBarCode(CRPCJob* pJob, CString& csTempBarCodeFileName)
{
	ERRCODE eCode = S_OK;
	t_BarCode* m_pBarCode;
	CString csTempPath;
	char pTempFileName[_MAX_PATH];      
	int iHeight;

	CZeusDoc* pDoc;
	GISRPC_ZEUSDOC* pZeusDoc;
			
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
	// resolve the document
	pZeusDoc = (GISRPC_ZEUSDOC*)pJob->hDevice;
	pDoc = (CZeusDoc*)(pZeusDoc->pDoc);

	csTempPath = pDoc->crrrs.csScratchPath;

	// make a temp file
	GetTempFileName(csTempPath, "pgprc", 0, pTempFileName);

	// do the license
	BCLicenseMe ("Graphx, Woburn", (e_licKind)3, 1, "D1F438BA", (e_licProduct)5);

	// Allocate and initialize bar code structure
	if (BCAlloc (&m_pBarCode) == S_OK)
	{
		// set the text height
		LOGFONT* plfFont;
		plfFont = BCGetLogFont (m_pBarCode);
		plfFont->lfHeight = -50;
		BCSetLogFont (m_pBarCode, plfFont);	

		// set the bar code text
		eCode = (eCode == S_OK) ? BCSetText(m_pBarCode, (char*)pJob->PGPRCPrintSpecs.OrderIDString, 
				strlen((char*)pJob->PGPRCPrintSpecs.OrderIDString)) : eCode;
		// set the type to code 39 ascii
		eCode = (eCode == S_OK) ? BCSetBCType(m_pBarCode, 
				(tag_BarCType)pJob->JobSeparator.pgprcBarCodeType) : eCode;
		// check digit method
		eCode = (eCode == S_OK) ? BCSetCDMethod(m_pBarCode, eCDStandard) : eCode;
		eCode = (eCode == S_OK) ? BCSetPrintText(m_pBarCode, pJob->JobSeparator.bTextUnderBarCode, FALSE) : eCode;
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



extern "C"
long GISRPCCreateJobSeparatorFiles_0(CRPCJob* pJob, int iStart, int iEnd, int iPage)
{   
	CString csTemp;
	CString csTempPath;			 
	char pTempFileName[_MAX_PATH];
	CStdioFile cf;
	int i;
	POSITION pos;
	GISRPC_OURPGPRCPHOTOSPECS* pPhoto;   
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

	CZeusDoc* pDoc;
	GISRPC_ZEUSDOC* pZeusDoc;
			
	// create a new bitmap for tiling the contact prints
	NewWidth = pJob->JobSeparator.iContactImage_Width * pJob->JobSeparator.iContactImage_NumAccross +
			  (pJob->JobSeparator.iContactImage_NumAccross - 1) * pJob->JobSeparator.iContactImage_SpacingX;

	if (iPage == 0)
		NewHeight = pJob->JobSeparator.iContactImage_Height * pJob->JobSeparator.iContactImage_NumDown_Page_1 +
				(pJob->JobSeparator.iContactImage_NumDown_Page_1 - 1) * pJob->JobSeparator.iContactImage_SpacingY;	  
	else
		NewHeight = pJob->JobSeparator.iContactImage_Height * pJob->JobSeparator.iContactImage_NumDown_Page_N +
					 (pJob->JobSeparator.iContactImage_NumDown_Page_N - 1) * pJob->JobSeparator.iContactImage_SpacingY;	  

	if (L_CreateBitmap(&TileBitmap, TYPE_CONV, NewWidth, NewHeight, 24, ORDER_RGB,
														NULL, TOP_LEFT, NULL, 0) != SUCCESS)
	return FALSE;

	L_FillBitmap(&TileBitmap, RGB(255, 255, 255));

	// load the logo file, don't totally fail if it's not there   
	if (pJob->JobSeparator.bDoLeftLogo && iPage == 0)
	{
		// if the logo file name is empty, use the one set in options
		if (strlen((char*)pJob->JobSeparator.lpszLeftLogoFile) < 1)
		{
			csTemp = Profile.GetStr(IDS_PHOTOGIZE, "LeftLogo", "");		 
			if (csTemp.GetLength() > 0)
				strcpy((char*)pJob->JobSeparator.lpszLeftLogoFile, csTemp);
		}

		if (L_LoadBitmap((char*)pJob->JobSeparator.lpszLeftLogoFile, 
				&LeftLogoBitmap, 0, ORDER_RGB, NULL, NULL) != SUCCESS)
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
		if (strlen((char*)pJob->JobSeparator.lpszRightLogoFile) < 1)
		{
			csTemp = Profile.GetStr(IDS_PHOTOGIZE, "RightLogo", "");		 
			if (csTemp.GetLength() > 0)
				strcpy((char*)pJob->JobSeparator.lpszRightLogoFile, csTemp);
		}											   

		if (L_LoadBitmap((char*)pJob->JobSeparator.lpszRightLogoFile, 
					&RightLogoBitmap, 0, ORDER_RGB, NULL, NULL) != SUCCESS)
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
			strlen((char*)pJob->PGPRCPrintSpecs.OrderIDString) > 0)
	{
		bRet = GISRPCDoBarCode(pJob, csTempBarCodeFileName);
		if (!bRet)		 
			pJob->JobSeparator.bDoBarCode = FALSE;		 
		else
		{
			if (L_LoadBitmap((char*)(const char*)csTempBarCodeFileName, 
						&BarCodeBitmap, 0, ORDER_RGB, NULL, NULL) != SUCCESS)
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
			pPhoto = (GISRPC_OURPGPRCPHOTOSPECS*)pJob->Photos.GetNext(pos);		 
			// check bounds
			if (!pPhoto->bPutInThumbnails)
				continue;

			if (idx < iStart || idx > (iEnd - 1)) 
			{
				idx++;
				continue;
			}

			idx++;

			// which file to use?
			if (pPhoto->bUseTempFile)
				lpFileName = (char*)pPhoto->lpszTempDiskFile;
			else
				lpFileName = (char*)pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile;

			// resize the images
			if (L_LoadBitmap(lpFileName, &ImageBitmap, 0, ORDER_RGB, NULL, NULL) == SUCCESS)
			{
				// get aspect ratios
				fARi = (float)ImageBitmap.Height / (float)ImageBitmap.Width;
				fARt = (float)(pJob->JobSeparator.iContactImage_Height) / (float)(pJob->JobSeparator.iContactImage_Width);		 

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
					NewWidth = pJob->JobSeparator.iContactImage_Width;
					NewHeight = (int)((float)(pJob->JobSeparator.iContactImage_Width) * fARi + .5F);
				}
				else
				{
					NewWidth = (int)((float)(pJob->JobSeparator.iContactImage_Height) / fARi + .5F);
					NewHeight = pJob->JobSeparator.iContactImage_Height;
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
				xidx %= pJob->JobSeparator.iContactImage_NumAccross;
				
				if (iPage == 0)
					yidx %= pJob->JobSeparator.iContactImage_NumDown_Page_1;
				else
					yidx %= pJob->JobSeparator.iContactImage_NumDown_Page_N;

				xdest = xidx * pJob->JobSeparator.iContactImage_Width + xidx * pJob->JobSeparator.iContactImage_SpacingX;
				ydest = yidx * pJob->JobSeparator.iContactImage_Height + yidx * pJob->JobSeparator.iContactImage_SpacingY;

				// offsets?
				xdest += (pJob->JobSeparator.iContactImage_Width - ImageBitmap.Width) / 2;
				ydest += (pJob->JobSeparator.iContactImage_Height - ImageBitmap.Height) / 2;

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
				if (xidx == pJob->JobSeparator.iContactImage_NumAccross)
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
	// resolve the document
	pZeusDoc = (GISRPC_ZEUSDOC*)(pJob->hDevice);
	pDoc = (CZeusDoc*)(pZeusDoc->pDoc);

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
			GISRPCWritePSStart(cf, pJob->JobSeparator.fSeparatorBBox_Width, pJob->JobSeparator.fSeparatorBBox_Height, FALSE);
		
			// write the address	  	 
			for (i = 0, ydest = 150; i < PGPRC_NUM_ADDRESS_LINES; i++, ydest -= 15)
			{
				fprintf(cf.m_pStream, "240 %d M\r\n", ydest);
				fprintf(cf.m_pStream, "(%s) S\r\n", 
				GISRPCCStringToPSString((char*)pJob->JobSeparator.lpszAddress[i]));
			}

			// now do order info		 
			fprintf(cf.m_pStream, "/Courier findfont [8 0 0 8 0 0] makefont setfont\r\n");
			for (i = 0, ydest = 180; i < PGPRC_NUM_ORDERINFO_LINES; i++, ydest -= 10)
			{
				fprintf(cf.m_pStream, "15 %d M\r\n", ydest);
				fprintf(cf.m_pStream, "(%s) S\r\n", 
				GISRPCCStringToPSString((char*)pJob->JobSeparator.lpszOrderInfo[i]));
			} 
		}
		else
		{
			cf.SeekToEnd(); // go to the end
			// write the prolog	     
			GISRPCWritePSStartN(cf, pJob->JobSeparator.fSeparatorBBox_Width, pJob->JobSeparator.fSeparatorBBox_Height);
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
					pJob->JobSeparator.fSeparatorBBox_Width - 10, (int)((float)TileBitmap.Height /
						((float)TileBitmap.Width /
							(pJob->JobSeparator.fSeparatorBBox_Width - 10.F)) + .5F));
			else
				fprintf(cf.m_pStream, "5 51 translate %d %d scale\r\n",
					pJob->JobSeparator.fSeparatorBBox_Width - 10, (int)((float)TileBitmap.Height / ((float)TileBitmap.Width / (pJob->JobSeparator.fSeparatorBBox_Width - 10.F)) + .5F));

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




extern "C"
long GISRPCCreateJobSeparatorFiles_1(CRPCJob* pJob, int iStart, int iEnd, int iPage)
{   
	CString csTemp;
	CString csTempPath;			 
	char pTempFileName[_MAX_PATH];
	CStdioFile cf;
	int i;
	POSITION pos;
	GISRPC_OURPGPRCPHOTOSPECS* pPhoto;   
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

	CZeusDoc* pDoc;
	GISRPC_ZEUSDOC* pZeusDoc;
			
	// create a new bitmap for tiling the contact prints
	NewWidth = pJob->JobSeparator.iContactImage_Width * pJob->JobSeparator.iContactImage_NumAccross +
					(pJob->JobSeparator.iContactImage_NumAccross - 1) * pJob->JobSeparator.iContactImage_SpacingX + pJob->JobSeparator.iContactImage_SpacingX;

	if (iPage == 0)
		NewHeight = pJob->JobSeparator.iContactImage_Height * pJob->JobSeparator.iContactImage_NumDown_Page_1 +
			(pJob->JobSeparator.iContactImage_NumDown_Page_1 - 1) * pJob->JobSeparator.iContactImage_SpacingY;	  
	else
		NewHeight = pJob->JobSeparator.iContactImage_Height * pJob->JobSeparator.iContactImage_NumDown_Page_N +
			(pJob->JobSeparator.iContactImage_NumDown_Page_N - 1) * pJob->JobSeparator.iContactImage_SpacingY;	  

	if (L_CreateBitmap(&TileBitmap, TYPE_CONV, NewWidth, NewHeight, 24, ORDER_RGB,
															NULL, TOP_LEFT, NULL, 0) != SUCCESS)
		return FALSE;

	L_FillBitmap(&TileBitmap, RGB(255, 255, 255));

	// load the logo file, don't totally fail if it's not there   
	if (pJob->JobSeparator.bDoLeftLogo && iPage == 0)
	{
		// if the logo file name is empty, use the one set in options
		if (strlen((char*)pJob->JobSeparator.lpszLeftLogoFile) < 1)
		{
			csTemp = Profile.GetStr(IDS_PHOTOGIZE, "LeftLogo", "");		 
			if (csTemp.GetLength() > 0)
				strcpy((char*)pJob->JobSeparator.lpszLeftLogoFile, csTemp);
		}

		if (L_LoadBitmap((char*)pJob->JobSeparator.lpszLeftLogoFile, 
					&LeftLogoBitmap, 0, ORDER_RGB, NULL, NULL) != SUCCESS)
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
		if (strlen((char*)pJob->JobSeparator.lpszRightLogoFile) < 1)
		{
			csTemp = Profile.GetStr(IDS_PHOTOGIZE, "RightLogo", "");		 
			if (csTemp.GetLength() > 0)
				strcpy((char*)pJob->JobSeparator.lpszRightLogoFile, csTemp);
		}											   

		if (L_LoadBitmap((char*)pJob->JobSeparator.lpszRightLogoFile, 
						&RightLogoBitmap, 0, ORDER_RGB, NULL, NULL) != SUCCESS)
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
			strlen((char*)pJob->PGPRCPrintSpecs.OrderIDString) > 0)
	{
		bRet = GISRPCDoBarCode(pJob, csTempBarCodeFileName);
		if (!bRet)		 
			pJob->JobSeparator.bDoBarCode = FALSE;		 
		else
		{
			if (L_LoadBitmap((char*)(const char*)csTempBarCodeFileName, 
						&BarCodeBitmap, 0, ORDER_RGB, NULL, NULL) != SUCCESS)
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
			pPhoto = (GISRPC_OURPGPRCPHOTOSPECS*)pJob->Photos.GetNext(pos);		 
			// check bounds
			if (!pPhoto->bPutInThumbnails)
				continue;

			if (idx < iStart || idx > (iEnd - 1)) 
			{
				idx++;
				continue;
			}

			idx++;

			// which file to use?
			if (pPhoto->bUseTempFile)
				lpFileName = (char*)pPhoto->lpszTempDiskFile;
			else
				lpFileName = (char*)pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile;

			// resize the images
			if (L_LoadBitmap(lpFileName, &ImageBitmap, 0, ORDER_RGB, NULL, NULL) == SUCCESS)
			{
				// get aspect ratios
				fARi = (float)ImageBitmap.Height / (float)ImageBitmap.Width;
				fARt = (float)(pJob->JobSeparator.iContactImage_Height) / (float)(pJob->JobSeparator.iContactImage_Width);		 

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
					NewWidth = pJob->JobSeparator.iContactImage_Width;
					NewHeight = (int)((float)(pJob->JobSeparator.iContactImage_Width) * fARi + .5F);
				}
				else
				{
					NewWidth = (int)((float)(pJob->JobSeparator.iContactImage_Height) / fARi + .5F);
					NewHeight = pJob->JobSeparator.iContactImage_Height;					
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
				xidx %= pJob->JobSeparator.iContactImage_NumAccross;
				if (iPage == 0)
					yidx %= pJob->JobSeparator.iContactImage_NumDown_Page_1;
				else
					yidx %= pJob->JobSeparator.iContactImage_NumDown_Page_N;

				xdest = xidx * pJob->JobSeparator.iContactImage_Width + xidx * pJob->JobSeparator.iContactImage_SpacingX + pJob->JobSeparator.iContactImage_SpacingX;
				ydest = yidx * pJob->JobSeparator.iContactImage_Height + yidx * pJob->JobSeparator.iContactImage_SpacingY;

				// offsets?
				xdest += (pJob->JobSeparator.iContactImage_Width - ImageBitmap.Width) / 2;
				ydest += (pJob->JobSeparator.iContactImage_Height - ImageBitmap.Height) / 2;

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
				if (xidx == pJob->JobSeparator.iContactImage_NumAccross)
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
	// resolve the document
	pZeusDoc = (GISRPC_ZEUSDOC*)(pJob->hDevice);
	pDoc = (CZeusDoc*)(pZeusDoc->pDoc);

//	if (!(pDoc = (CZeusDoc*)(pJob->hDevice)))
//		return pgprc_error_invalid_job_handle;
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
			GISRPCDoSeparatorText(pJob, cf);
		}
		else
		{
			cf.SeekToEnd(); // go to the end
			// write the prolog	     
			GISRPCWritePSStartN(cf, pJob->JobSeparator.fSeparatorBBox_Width, pJob->JobSeparator.fSeparatorBBox_Height);
		}	

		// write the bitmap to the eps file
		fprintf(cf.m_pStream, "GS /picstr %d string def\r\n", 
		TileBitmap.Width * 3);

		if (pJob->JobSeparator.bDoThumbnails)
		{	  
			if (iPage == 0)
				fprintf(cf.m_pStream, "0 260 translate %d %d scale\r\n", 	  
					pJob->JobSeparator.fSeparatorBBox_Width - 50, (int)((float)TileBitmap.Height / 
						((float)TileBitmap.Width / (pJob->JobSeparator.fSeparatorBBox_Width - 50.F)) + .5F));
			else
				fprintf(cf.m_pStream, "0 51 translate %d %d scale\r\n", 	  
					pJob->JobSeparator.fSeparatorBBox_Width - 50, (int)((float)TileBitmap.Height / 
						((float)TileBitmap.Width / (pJob->JobSeparator.fSeparatorBBox_Width - 50.F)) + .5F));

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
			GISRPCTagPhotos(pJob, cf, iStart, iEnd, iPage); 
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
		}				  

		// write the index page, etc..	  
		fprintf(cf.m_pStream, "/Helvetica-Bold findfont [14 0 0 14 0 0] makefont setfont\r\n");
		fprintf(cf.m_pStream, "596 475 M\r\n");
		GISRPCGetTotalSeparatorPages(pJob);
		cs.Format("Index %d of %d", iPage + 1, pJob->iNumSeparatorPages);
		fprintf(cf.m_pStream, "(%s) S\r\n", GISRPCCStringToPSString(cs));
		// write time
		if (pJob->JobSeparator.TimeStamp[0] == 0)
		{
			CTime t = CTime::GetCurrentTime();
			cs = t.Format( "%m/%d/%y" );
		}
		else
			cs = pJob->JobSeparator.TimeStamp;

		fprintf(cf.m_pStream, "596 460 M\r\n");
		fprintf(cf.m_pStream, "(%s) S\r\n", GISRPCCStringToPSString(cs));

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





extern "C"
long GISPRCApplyFilter(CRPCJob* pJob, PGISRPC_OURPGPRCPHOTOSPECS pPhoto, PCONTEXT_DOC_HANDLE* phDevice)
{
	int i;
	BITMAPHANDLE LeadBitmap;
	CString csTempPath;
	char pTempFileName[_MAX_PATH];
	FILEINFO FileInfo;
	BOOL bMustLoadSave = FALSE;

	CZeusDoc* pDoc;
	GISRPC_ZEUSDOC* pZeusDoc;
			
	// get file info to see if we need to convert the file to 24 bit color
    memset(&FileInfo, 0, sizeof(FILEINFO));
    FileInfo.uStructSize = sizeof(FILEINFO);
	if (L_FileInfo((char*)pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile, &FileInfo, FILEINFO_TOTALPAGES, NULL) > 0)
	{
		// if we don't have a 24 bit file AND we are not applying filters, force a load
		// and save 
		if (FileInfo.BitsPerPixel != 24 && pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcImageFilter.iNumFiltersToUse == 0)
		{
			bMustLoadSave = TRUE;
			// make sure the filters are off!
			for (i = 0; i < PGPRC_MAXFILTERS; i++)
			{
				pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcImageFilter.pgprcImageFilterType[i] = GISRPC_imagefilter_none;
			}
		}
	}

	if (pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcImageFilter.iNumFiltersToUse > 0 || bMustLoadSave)
	{
		if (L_LoadBitmap((char*)pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile, &LeadBitmap, 0, ORDER_RGB, NULL, NULL) != SUCCESS)
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
			pZeusDoc = (GISRPC_ZEUSDOC*)phDevice;
			pDoc = (CZeusDoc*)(pZeusDoc->pDoc);

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

			strcpy((char*)pPhoto->lpszTempDiskFile, pTempFileName);
		}	  
	}
	return pgprc_returnok;
}




extern "C"
long GISRPCPrintJob(PCONTEXT_JOB_HANDLE hJob, GISRPC_PRINTSPECS* pPrintSpecs)
{
//	CZeusDoc* pDoc;
//	GISRPC_ZEUSDOC* pZeusDoc;

	GISRPC_JOB* pZeusJob;

	CRPCJob* pJob;
	GISRPC_OURPGPRCPHOTOSPECS* pPhoto;   
	CQueueFile* pCQueueFile;   
	int iPage;
	PGPRC_RETURNVAL gRetVal = pgprc_returnok;
	int iMontageCanvasToUse;
	int iNumToMontage = 0;

	// resolve the job
	pZeusJob = (GISRPC_JOB*)hJob;
	pJob = (CRPCJob*)(pZeusJob->pJob);

	// sanity check
	if (!pJob) 
	{
		return pgprc_error_invalid_job_handle;
	}

	if (!(pJob->IsKindOf(RUNTIME_CLASS(CRPCJob)))) 
	{
		return pgprc_error_invalid_job_handle;
	}

	pJob->PGPRCPrintSpecs.pgprcQuality				= pPrintSpecs->pgprcQuality;
	pJob->PGPRCPrintSpecs.bPrintToHold				= pPrintSpecs->bPrintToHold;
	pJob->PGPRCPrintSpecs.bPromptBeforePrint		= pPrintSpecs->bPromptBeforePrint;
	pJob->PGPRCPrintSpecs.pgprcPriority				= pPrintSpecs->pgprcPriority;
	pJob->PGPRCPrintSpecs.isecStartDelay			= pPrintSpecs->isecStartDelay;
	pJob->PGPRCPrintSpecs.bPrintJobSeparatorOnly	= pPrintSpecs->bPrintJobSeparatorOnly;
	pJob->PGPRCPrintSpecs.OrderID					= pPrintSpecs->OrderID;

	strcpy((char*)pJob->PGPRCPrintSpecs.Owner, (const char*)pPrintSpecs->Owner);
	strcpy((char*)pJob->PGPRCPrintSpecs.OrderIDString, (const char*)pPrintSpecs->OrderIDString);

	// create the queuefiles
	gRetVal = GISRPCCreateQueueFiles(pJob);
	if (gRetVal != pgprc_returnok)
	{
		RPDEBUGSTRMFC("GISServer: Can't create queue files");
		return gRetVal;
	}

	// do we need to add a separator?
	if (pJob->bDoJobSeparator)
	{	  
		int iStart, iEnd, iTotalPhotos;	  

		// total number of photos
		//iTotalPhotos = pJob->Photos.GetCount();
		iTotalPhotos = GISRPCGetRealPhotoCount(pJob);

		iStart = 0; 
		iEnd = iStart + pJob->JobSeparator.iContactImage_NumAccross * pJob->JobSeparator.iContactImage_NumDown_Page_1;
		// get pages
		GISRPCGetTotalSeparatorPages(pJob);
		// current page
		iPage = 0;

		do
		{	 
			switch(pJob->JobSeparator.iStyle)
			{
			case PGPRC_JOBSEPARATOR_STYLE_1:		   
				// we need to add iPage to iStart and iEnd because we've added a separator
				// photo to the start of the photo list!
				if (!GISRPCCreateJobSeparatorFiles_1(pJob, iStart + iPage, iEnd + iPage, iPage))
				{
					RPDEBUGSTRMFC("GISServer: pgprc_error_cantcreate_jobseparator");
					return pgprc_error_cantcreate_jobseparator;
				}
				break;
			default:
			case PGPRC_JOBSEPARATOR_STYLE_0:  
				if (!GISRPCCreateJobSeparatorFiles_0(pJob, iStart + iPage, iEnd + iPage, iPage))
				{
					RPDEBUGSTRMFC("GISServer: pgprc_error_cantcreate_jobseparator");
					return pgprc_error_cantcreate_jobseparator;
				}
			break;
			}	   	

			if (iStart == 0)
			{
				iStart += pJob->JobSeparator.iContactImage_NumAccross * pJob->JobSeparator.iContactImage_NumDown_Page_1;
				iTotalPhotos -= pJob->JobSeparator.iContactImage_NumAccross * pJob->JobSeparator.iContactImage_NumDown_Page_1;
			}
			else
			{
				iStart += pJob->JobSeparator.iContactImage_NumAccross * pJob->JobSeparator.iContactImage_NumDown_Page_N;
				iTotalPhotos -= pJob->JobSeparator.iContactImage_NumAccross * pJob->JobSeparator.iContactImage_NumDown_Page_N;		 
			}

			iEnd = iStart + pJob->JobSeparator.iContactImage_NumAccross * pJob->JobSeparator.iContactImage_NumDown_Page_N;
			iPage++;

			// now insert the separator into the job list	  
			if (pPhoto = new GISRPC_OURPGPRCPHOTOSPECS)
			{
				ZeroMemory(pPhoto, sizeof (GISRPC_OURPGPRCPHOTOSPECS));		 		 
				pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcImageFilter.iNumFiltersToUse = 0;			 
				pPhoto->iJobID = 0;
				pPhoto->iReportedCopyCount = 1;
				pPhoto->PhotoSpecs.BasePhotoSpecs.iCopies = 1;
				pPhoto->bUseTempFile = FALSE;		 
				pPhoto->FileType = FILE_EPS;
				pPhoto->bIsSeparator = TRUE;
				strcpy((char*)pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile, pJob->csJobSeparatorFile);
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
				if (pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcSizing != GISRPC_montage_crop &&
							pPhoto->PhotoSpecs.BasePhotoSpecs.pgprcSizing != GISRPC_montage_fit)
				{
					// instantiate a queuefile 
					if ((pCQueueFile = new CQueueFile) == NULL)
					{
						delete pPhoto;	  
						RPDEBUGSTRMFC("GISServer: pgprc_error_outofmemory");
						return pgprc_error_outofmemory;		 
					}

					// save the pointer
					pPhoto->hCqueueFile = pCQueueFile;   

					// and the job id
					pPhoto->iJobID = pCQueueFile->iJobID;

					// init the queue file
					if (!pCQueueFile->Init((char*)pPhoto->PhotoSpecs.BasePhotoSpecs.lpszDiskFile))
					{
						delete pCQueueFile;
						pPhoto->hCqueueFile = NULL;
						pCQueueFile = NULL;
						delete pPhoto;	 
						pPhoto = NULL;
						RPDEBUGSTRMFC("GISServer: pgprc_cant_find_file");
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
				iStart--;
				iEnd--;
			}													 
		} while (iTotalPhotos > 0);							 
	}


	// we need to determine the optimal orientation of the 
	// printspecs width and height

	// reset montage flags
	GISRPCResetMontageFlags(pJob);

	// now loop through the photos and create the appropriate montages
	for(;;)
	{	  
		// get montage set
		iNumToMontage = GISRPCCheckNextMontageSet(pJob, iMontageCanvasToUse);

		// all done?
		if (iNumToMontage < 1)
			break;

		// compute offsets and pages for montage
		int iPageStd = 1;
		int iPageSwapped = 1;	  

		// first start with the pPhoto->iMontageWidthInches1000 and 
		// pPhoto->iMontageHeightInches1000 as passed   
		gRetVal = (PGPRC_RETURNVAL)GISRPCComputeMontagePhotosVars(pJob, iPageStd, iMontageCanvasToUse, FALSE);

		// now swap and try again   
		GISRPCSwapCustomPageDims(pJob);
		
		// compute   
		gRetVal = (gRetVal == pgprc_returnok) ? 
					(PGPRC_RETURNVAL)GISRPCComputeMontagePhotosVars(pJob, iPageSwapped, iMontageCanvasToUse, FALSE) : gRetVal;
		
		// swap back in case of error
		GISRPCSwapCustomPageDims(pJob);
		
		// process error
		if (gRetVal != pgprc_returnok)
		{
			RPDEBUGSTRMFC("GISServer: ComputeMontagePhotosVars bad");
			break;
		}				  	  

		// neither way fits!
		if (iPageSwapped > MISFIT_PENALTY && iPageStd > MISFIT_PENALTY)
		{
			// Recompute and set to Media Size(TRUE)
			iPageStd = 1;
			gRetVal = (PGPRC_RETURNVAL)GISRPCComputeMontagePhotosVars(pJob, iPageStd, iMontageCanvasToUse, TRUE);
			//		 gRetVal = pgprc_error_cant_create_montage;
			//		 break;
		}
		else
		{
			if (iPageSwapped < iPageStd) // go with the original
			{
				GISRPCSwapCustomPageDims(pJob);
			}
			else
			{
				// otherwise, recompute!
				iPageStd = 1;
				gRetVal = (PGPRC_RETURNVAL)GISRPCComputeMontagePhotosVars(pJob, iPageStd, iMontageCanvasToUse, FALSE);
			}
		}

		if (gRetVal == pgprc_returnok) 
			gRetVal = (PGPRC_RETURNVAL)GISRPCMontagePhotos(pJob);	  

		if (gRetVal != pgprc_returnok) 
		{
			RPDEBUGSTRMFC("GISServer: MontagePhotosVars bad");
			break;
		}
	} 

	if (gRetVal != pgprc_returnok)
	{
		RPDEBUGSTRMFC("GISServer: General PrintJob bad");
		return gRetVal;
	}

	// insert the jobs into the queue
	InsertJobs(pJob);
//	InsertJobs2(pJob);

	return pgprc_returnok;
}




extern "C"
long GISRPCAddJobSeparator(PCONTEXT_JOB_HANDLE hJob, GISRPC_JOBSEPARATOR* pJobSepinfo)
{
	int i = 0;

	GISRPC_JOB* pZeusJob;

	CRPCJob* pJob;   

	// resolve the job
	pZeusJob = (GISRPC_JOB*)hJob;
	pJob = (CRPCJob*)(pZeusJob->pJob);

	// sanity check
	if (!pJob) 
	{	  
		return pgprc_error_invalid_job_handle;
	}

	if (!(pJob->IsKindOf(RUNTIME_CLASS(CRPCJob)))) 
	{	  
		return pgprc_error_invalid_job_handle;
	}

//	pJob->JobSeparator.hPhoto = (unsigned int)&pJob->JobSeparator;
	pJob->JobSeparator.iStyle							= pJobSepinfo->iStyle;
	pJob->JobSeparator.hPhoto							= pJobSepinfo->hPhoto;
	pJob->JobSeparator.bDoThumbnails					= pJobSepinfo->bDoThumbnails;
	pJob->JobSeparator.bDoBarCode						= pJobSepinfo->bDoBarCode;
	pJob->JobSeparator.pgprcBarCodeType					= pJobSepinfo->pgprcBarCodeType;
	pJob->JobSeparator.bTextUnderBarCode				= pJobSepinfo->bTextUnderBarCode;
	pJob->JobSeparator.bDoLeftLogo						= pJobSepinfo->bDoLeftLogo;
	pJob->JobSeparator.bDoRightLogo						= pJobSepinfo->bDoRightLogo;
	pJob->JobSeparator.pgprcSizing						= pJobSepinfo->pgprcSizing;
	pJob->JobSeparator.iCanvasToUse						= pJobSepinfo->iCanvasToUse;
	pJob->JobSeparator.iMontageWidthInches1000			= pJobSepinfo->iMontageWidthInches1000;
	pJob->JobSeparator.iMontageHeightInches1000			= pJobSepinfo->iMontageHeightInches1000;
	pJob->JobSeparator.iMontagePhotoSpacingXInches1000	= pJobSepinfo->iMontagePhotoSpacingXInches1000;
	pJob->JobSeparator.iMontagePhotoSpacingYInches1000	= pJobSepinfo->iMontagePhotoSpacingYInches1000;

	strcpy((char*)pJob->JobSeparator.lpszShipMethod, (const char*)pJobSepinfo->lpszShipMethod );
	strcpy((char*)pJob->JobSeparator.lpszStoreID, (const char*)pJobSepinfo->lpszStoreID );
	strcpy((char*)pJob->JobSeparator.lpszLeftLogoFile, (const char*)pJobSepinfo->lpszLeftLogoFile );
	strcpy((char*)pJob->JobSeparator.lpszRightLogoFile, (const char*)pJobSepinfo->lpszRightLogoFile );
	strcpy((char*)pJob->JobSeparator.TimeStamp, (const char*)pJobSepinfo->TimeStamp );

	for ( i = 0; i < PGPRC_NUM_ADDRESS_LINES; i++)
	{
		strcpy((char*)pJob->JobSeparator.lpszAddress[i], (const char*)pJobSepinfo->lpszAddress[i] );
	}

	for ( i = 0; i < PGPRC_NUM_ORDERINFO_LINES; i++)
	{
		strcpy((char*)pJob->JobSeparator.lpszOrderInfo[i], (const char*)pJobSepinfo->lpszOrderInfo[i] );
	}


	// set this flag
	pJob->bDoJobSeparator = TRUE;

	// and assign these
	switch(pJob->JobSeparator.iStyle)
	{
	case PGPRC_JOBSEPARATOR_STYLE_1:
		pJob->JobSeparator.iContactImage_Width			= 100 * 4;
		pJob->JobSeparator.iContactImage_Height			= 66 * 4;
		pJob->JobSeparator.iContactImage_SpacingX		= 30 * 4;
		pJob->JobSeparator.iContactImage_SpacingY		= 45 * 4;
		pJob->JobSeparator.iContactImage_NumAccross		= 5;
		pJob->JobSeparator.iContactImage_NumDown_Page_1	= 2;
		pJob->JobSeparator.iContactImage_NumDown_Page_N	= 4;
		pJob->JobSeparator.iSeparatorBBox_Width			= 700;
		pJob->JobSeparator.iSeparatorBBox_Height		= 500;
		pJob->JobSeparator.fSeparatorBBox_Width			= 700.F;
		pJob->JobSeparator.fSeparatorBBox_Height		= 500.F;
		break;
	default:
	case PGPRC_JOBSEPARATOR_STYLE_0:  
		pJob->JobSeparator.iContactImage_Width			= 200 * 2;
		pJob->JobSeparator.iContactImage_Height			= 133 * 2;
		pJob->JobSeparator.iContactImage_SpacingX		= 5 * 2;
		pJob->JobSeparator.iContactImage_SpacingY		= 5 * 2;
		pJob->JobSeparator.iContactImage_NumAccross		= 7;
		pJob->JobSeparator.iContactImage_NumDown_Page_1	= 2;
		pJob->JobSeparator.iContactImage_NumDown_Page_N	= 6;
		pJob->JobSeparator.iSeparatorBBox_Width			= 600;
		pJob->JobSeparator.iSeparatorBBox_Height		= 400;
		pJob->JobSeparator.fSeparatorBBox_Width			= 600.F;
		pJob->JobSeparator.fSeparatorBBox_Height		= 400.F;
		break;
	}

	return pgprc_returnok;
}

extern "C"
long GISRPCPutFile(PCONTEXT_JOB_HANDLE hJob, GISRPC_SENDFILEINFO* pSendFileInfo)
{
	CRPCJob* pJob;
	PPGPRCSENDFILEINFO pgprcSendFileInfo;   
	CString csFolder;
	CString csFileName;
	CFile cf;
	BYTE* pFileBuf;
	int iSizeToWrite;
	CProfile Profile;

//	GISRPC_ZEUSDOC* pZeusDoc;
	GISRPC_JOB* pZeusJob;

	// resolve the job
	pZeusJob = (GISRPC_JOB*)hJob;
	pJob = (CRPCJob*)(pZeusJob->pJob);

	// sanity check
	if (!pJob) 
	{									  
		return pgprc_error_invalid_job_handle;
	}

	if (!(pJob->IsKindOf(RUNTIME_CLASS(CRPCJob)))) 
	{	  
		return pgprc_error_invalid_job_handle;
	}

	// create a new file spec
	if (!(pgprcSendFileInfo = new PGPRCSENDFILEINFO)) 
	{	  
		return pgprc_error_outofmemory;
	}

	// get app path
	csFolder = ((CZeusApp*)AfxGetApp())->GetAppPath();

	// create a Photogize subdirectory
	csFolder += "Photogize\\";

	// make sure it exists
	::CreateDirectory(csFolder, NULL);

	ZeroMemory(pgprcSendFileInfo, sizeof PGPRCSENDFILEINFO);   

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
		if (cf.Open(csFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
		{
			iSizeToWrite = pgprcSendFileInfo->iFileSize;
			do
			{
				if (iSizeToWrite > 0)
				{
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

extern "C"
long GISRPCAddPhoto(PCONTEXT_JOB_HANDLE hJob, GISRPC_PHOTOSPECS* pPhotoSpecs, GISRPC_DEVSTRING* pOutFileName)
{
	BOOL bNewFile = FALSE;

	int i;
	int iTotalCopies;
	
	CFile cFileOut;

	CRPCJob* pJob;
	GISRPC_OURPGPRCPHOTOSPECS* pGisPhotoSpecs;
	GISRPC_OURPGPRCPHOTOSPECS* pNewGisPhotoSpecs;
	
	CString csTempPath;
	int iSizeToWrite;

	CZeusDoc* pDoc;
	GISRPC_ZEUSDOC* pZeusDoc;

	GISRPC_JOB* pZeusJob;

	// resolve the job
	pZeusJob = (GISRPC_JOB*)hJob;
	pJob = (CRPCJob*)(pZeusJob->pJob);

	// sanity check
	if (!pJob) 
	{								
		RPDEBUGSTRMFC("Add Photo Bad #1");
		return pgprc_error_invalid_job_handle;
	}

	if (!(pJob->IsKindOf(RUNTIME_CLASS(CRPCJob)))) 
	{	  
		RPDEBUGSTRMFC("Add Photo Bad #2");
		return pgprc_error_invalid_job_handle;
	}
	
	// resolve the device to get the temp path
	pZeusDoc = (GISRPC_ZEUSDOC*)pJob->hDevice;
	pDoc = (CZeusDoc*)(pZeusDoc->pDoc);

	// create a new photo spec
	if (!(pGisPhotoSpecs = new GISRPC_OURPGPRCPHOTOSPECS)) 
	{
		RPDEBUGSTRMFC("Add Photo Bad #4");
		return pgprc_error_outofmemory;
	}

	ZeroMemory(pGisPhotoSpecs, sizeof GISRPC_OURPGPRCPHOTOSPECS);   

	csTempPath = pDoc->crrrs.csScratchPath;

	// do different things based on the style of add_photo
	pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs = (GISRPC_PHOTOSPECS)*pPhotoSpecs;

	// get the actual data if bSendActualData is set
	if (pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.bSendActualData)
	{
		if ( strlen((const char*)pPhotoSpecs->lpszOutFile) == 0 )
		{
			// make a temp file
			GetTempFileName(csTempPath, "pgprc", 0, (char*)pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.lpszOutFile);
			strcpy((char*)pOutFileName, (char*)pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.lpszOutFile);
			bNewFile = TRUE;
		}
		else
		{
			strcpy((char*)pOutFileName, (char*)pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.lpszOutFile);
		}

		if ( pPhotoSpecs->iPhotoBytesCopied > 0 )
		{
			// open it and write the data
			if (cFileOut.Open((char*)pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.lpszOutFile,
				CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeBinary))
			{
				if ( !bNewFile )
				{
					cFileOut.SeekToEnd();
				}

				iSizeToWrite = pPhotoSpecs->iPhotoBytesCopied;
				do
				{
					if (iSizeToWrite > 0)
					{
						cFileOut.Write(pPhotoSpecs->PhotoData, min(iSizeToWrite, PHOTOSENDBUFSIZE));
					}

					iSizeToWrite -= min(iSizeToWrite, PHOTOSENDBUFSIZE);
				} while (iSizeToWrite > 0);
			}
		}
	}

	// Only want to do the following processing when the file is complete
	// Indicated by pPhotoSpecs->iPhotoBytesCopied == 0
	if ( pPhotoSpecs->iPhotoBytesCopied > 0 )
		return pgprc_returnok;

	// set the reported copy count to be the same as the passed copy count
	pGisPhotoSpecs->iReportedCopyCount = pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.iCopies;

	// get file type
	FILEINFO FileInfo;
    memset(&FileInfo, 0, sizeof(FILEINFO));
    FileInfo.uStructSize = sizeof(FILEINFO);
	if (::L_FileInfo((char*)pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.lpszOutFile, 
													&FileInfo, FILEINFO_TOTALPAGES, NULL) > 0)	  
		pGisPhotoSpecs->FileType = FileInfo.Format;

	// increment photo counter
	pJob->iPhotoCounter++;

	pGisPhotoSpecs->iPhotoCount = pJob->iPhotoCounter;   

	pGisPhotoSpecs->bIsSeparator = FALSE;

	// put in the thumbnails
	pGisPhotoSpecs->bPutInThumbnails = TRUE;   

	// add to the array
	pJob->Photos.AddTail(pGisPhotoSpecs);

	// if this is a montage print AND the page size is the same,
	// change to fit sizing
	if (pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.pgprcSizing == GISRPC_montage_crop ||
								pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.pgprcSizing == GISRPC_montage_fit)
	{
		MEDIA Media;	  

		if (pDoc = (CZeusDoc*)(pJob->hDevice))
		{
			QUEUEDEVICE.GetMedia(pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.iCanvasToUse, &Media);

			if (min(pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.iMontageHeightInches1000, 
							pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.iMontageWidthInches1000) ==
					min(abs(Media.Rect.right - Media.Rect.left) * 10,
							abs(Media.Rect.top - Media.Rect.bottom) * 10))
			{
			if (pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.pgprcSizing == GISRPC_montage_crop)
				pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.pgprcSizing = GISRPC_crop;
			else
				pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.pgprcSizing = GISRPC_fit;
			}			
		}
	}

	// copy the photospecs back with the assigned handle
//	pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.hPhoto = (PCONTEXT_PHOTO_HANDLE)pGisPhotoSpecs;

	// if we are montaging this photo AND we are doing copies,
	// add the photo to the queue again
	if ((pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.pgprcSizing == GISRPC_montage_crop ||
							pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.pgprcSizing == GISRPC_montage_fit) && 
								pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.iCopies > 1)
	{
		// save this
		iTotalCopies = pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.iCopies;
		// force this
		pGisPhotoSpecs->PhotoSpecs.BasePhotoSpecs.iCopies = 1;
		for (i = 1; i < iTotalCopies; i++)
		{
			if (!(pNewGisPhotoSpecs = new GISRPC_OURPGPRCPHOTOSPECS)) 
			{
				delete pGisPhotoSpecs;			
				return pgprc_error_outofmemory;
			}
			else
			{
				// copy over
				memcpy(pNewGisPhotoSpecs, pGisPhotoSpecs, sizeof GISRPC_OURPGPRCPHOTOSPECS);			
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

extern "C"
long GISRPCAddPhoto2(PCONTEXT_JOB_HANDLE hJob, GISRPC_PHOTOSPECS2* pPhotoSpecs)
{
	return pgprc_returnok;
}

/*
extern "C"
long GISRPCGetJobStatus(int len, HANDLE hJob, CClientSocket* pSocket)
   {
   int i = 0;      
   PGPRCJOBSTATUSSTRUCT PGPRCJobStatusStruct;
   CRPCJob* pJob;
   POSITION pos;
   GISRPC_OURPGPRCPHOTOSPECS* pPhoto;   
   PGPRC_RETURNVAL gRetVal = pgprc_returnok;

   // we don't care about the incoming data
   DumpIncomingData(len, pSocket);      

   RPDEBUGSTRMFC("PGPRCServer: GetJobStatus, data dumped");

   // default
   PGPRCJobStatusStruct.pgprcJobStatus = pgprc_jobstatus_ok;   

	// resolve the job
	pZeusJob = (GISRPC_JOB*)hJob;
	pJob = (CRPCJob*)(pZeusJob->pJob);

   // sanity check
   if (!pJob) 
	  {
	  RPDEBUGSTRMFC("PGPRCServer: GetJobStatus, Bad pJob");	  
	  return pgprc_error_invalid_job_handle;
	  }
   if (!(pJob->IsKindOf(RUNTIME_CLASS(CRPCJob)))) 
	  {
	  RPDEBUGSTRMFC("PGPRCServer: GetJobStatus, Bad CRPCJob class");	  
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
	  pPhoto = (GISRPC_OURPGPRCPHOTOSPECS*)pJob->Photos.GetNext(pos);
      gRetVal = GetInternalPhotoStatus(pJob, pPhoto, &PGPRCJobStatusStruct);
	  if (gRetVal != pgprc_returnok) 
		 {
		 RPDEBUGSTRMFC("PGPRCServer: GetJobStatus, GetInternalPhotoStatus NOTOK");
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
*/

/*
extern "C"
long GISRPCGetInternalPhotoStatus(CRPCJob* pJob, GISRPC_OURPGPRCPHOTOSPECS* pPhoto, 
										 PPGPRCJOBSTATUSSTRUCT pPgprcJobStatusStruct)
   {
   CPaneElement* pRipElement;   
   CZeusDoc* pDoc;   
   CZeusDoc* pPassedDoc;
   int i, j;
   int iCnt = 0;   
   CQueueFile* pThisCQueueFile;

   // resolve the document
	// resolve the document
	pZeusDoc = (GISRPC_ZEUSDOC*)phDevice;
	pDoc = (CZeusDoc*)(pZeusDoc->pDevice);
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
*/


/*
extern "C"
long GISRPCGetPhotoStatus(int len, CClientSocket* pSocket)
{
   CPaneElement* pRipElement;   
   CZeusDoc* pDoc;   
   CZeusDoc* pPassedDoc;
   int i, j;
   int iCnt = 0;   
   CQueueFile* pThisCQueueFile;
   CRPCJob* pJob;
   POSITION pos;
   GISRPC_OURPGPRCPHOTOSPECS* pPhoto;   
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
	pZeusJob = (GISRPC_JOB*)hJob;
	pJob = (CRPCJob*)(pZeusJob->pJob);
//   pJob = (CRPCJob*)pGisPhotoStatusStruct->hJob;
									   	  
   // default
   pGisPhotoStatusStruct->pgprcJobStatus = pgprc_photo_notfound;

   // sanity check
   if (!pJob) 
	  {      
	  delete pGisPhotoStatusStruct;
	  return pgprc_error_invalid_job_handle;
	  }
   if (!(pJob->IsKindOf(RUNTIME_CLASS(CRPCJob)))) 
	  {      
	  delete pGisPhotoStatusStruct;
	  return pgprc_error_invalid_job_handle;
	  }

   // resolve the document
	// resolve the document
	pZeusDoc = (GISRPC_ZEUSDOC*)phDevice;
	pDoc = (CZeusDoc*)(pZeusDoc->pDevice);
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
	  pPhoto = (GISRPC_OURPGPRCPHOTOSPECS*)pJob->Photos.GetNext(pos);
	  
	  // not the one!
	  if (((GISRPC_OURPGPRCPHOTOSPECS*)pGisPhotoStatusStruct->hPhoto != pPhoto) && 
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
*/

/*
extern "C"
long GISRPCReturnDummyInfo(int len, CClientSocket* pSocket)
   {
   int i;

   RPDEBUGSTRMFC("Return Dummy Info");

   for (i = 0; i < len; i++)		  
	  *(pSocket->m_pArchiveOut) << 0;

   // ReportByteCount(TRUE, len);
   }
*/


/*
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

#ifdef RPDEBUGSTRMFC
      RPDEBUGSTRMFC("Check Awake");
#endif

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
*/

/*
extern "C"
long GISRPCReportByteCount(BOOL bWrite, int iNum)
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
*/


//////////////////// MIDL_user_allocate() ////////////////////
extern "C"
void __RPC_FAR* __RPC_API MIDL_user_allocate(size_t len)
{
	return (new(unsigned char [len]));
}

//////////////////// MIDL_user_free() ////////////////////
extern "C"
void __RPC_API MIDL_user_free(void __RPC_FAR * ptr)
{
	if (NULL != ptr)
		delete (ptr);
}

void __RPC_USER PCONTEXT_DOC_HANDLE_rundown( PCONTEXT_DOC_HANDLE )
{
	return;
}

void __RPC_USER PCONTEXT_JOB_HANDLE_rundown( PCONTEXT_JOB_HANDLE )
{
	return;
}


