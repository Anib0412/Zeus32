////////////////////////////////////////////////////////////////////////////
// PHZSERVER.H
//                                                                        
// Photogize.PrintServer class for RasterPlus
// Copyright (C) 2000-2001 Graphx, Inc.
//

#ifndef __GISRPC_SERVER_H__
#define __GISRPC_SERVER_H__ 

#include "GisRPCInterface.h"
#include "GisRPCIntfDefs.h"

#include "..\GisRpc\GisRPCRetVal.h"

#ifdef LEADTOOLS_13
#include "..\CmnLibs\Current LeadTools\include\l_bitmap.h"
#include "..\CmnLibs\Current LeadTools\include\l_error.h"
#else
#include "l_bitmap.h"
#include "l_error.h"
#endif

#define GISRPC_CURRENT_VERSION	402103


class CRPCJob : public CObject
{	  
DECLARE_DYNAMIC(CRPCJob)
public:
	CRPCJob();
	~CRPCJob();

	void DeleteAllPhotos();

	CString csName;
	GISRPC_PRINTSPECS PGPRCPrintSpecs;	// print specs

	GISRPC_JOBSEPARATOR JobSeparator;	// the job separator
	BOOL bDoJobSeparator;
	CString csJobSeparatorFile;
	int iNumSeparatorPages;

	HANDLE hDevice;						// device to print to
	
	CPtrList Photos;					// an array of PGPRCPHOTOSPECS from client   
	int iPhotoCounter;
	int iTotalPages;
};


class CGISRPCServer : public CObject
{
public:
	CGISRPCServer();
	~CGISRPCServer();

	void ProcessPendingRead();
	void ProcessPendingAccept();   
	void StartRPCPolling();
	void StopRPCPolling();   
	BOOL RPCPolling;   
	void StartRPCServer();

	void FinalCleanUp();

	// New Added Data Fields
	
	int iCurrentVersion;

	RPC_STATUS	rpcStatus;
	RPC_BINDING_VECTOR*	rpcBindingVector;

//	PGPRC_RETURNVAL gRetVal;   

	// New Added Data Fields
//	CObArray coJobArray;  

	// New Added Functions

protected:
	CString csRPDebugStr;
	char cDebugStr[255];

	void CloseRPC();   
	void CloseAllRPC();
};
#endif