/////////////////////////////////////////////////////////////////////////////
// IPCData.h
//                                                                        
// inter-process data stuff for RasterPlus95
// Copyright (C) 1995 Graphx, Inc.
//

#ifndef __IPCDATA_H__
#define __IPCDATA_H__

#define MAXDATAHANDLES 64

// common formats for creating events, etc...
#define IPCDATAFORMAT           "%s_%d"
#define IPCEVENT_PACK_APPEND    "_pack"
#define IPCEVENT_PNACK_APPEND   "_pnack"
#define IPCEVENT_CACK_APPEND    "_cack"
#define IPCEVENT_CNACK_APPEND   "_cnack"
#define IPC_CHILDSTARTARGS      "%s %s"

// this is the message structure that always sits
// in the first allocated mapped file.  it handles
// simple messages and data from and to the parent and child
typedef struct tagBASEIPCMESSAGE
   {
   int iParentMsg;
   int iParentData1;
   int iParentData2;
   int iParentRet;      
   int iChildMsg;
   int iChildData1;
   int iChildData2;
   int iChildRet;    
   BOOL PostOnly;     
   } BASEIPCMESSAGE, * PBASEIPCMESSAGE;

#define IDC_BASE_DATASLOT    0

#define pBaseMessage ((PBASEIPCMESSAGE)lpData[IDC_BASE_DATASLOT])

// IPC messages
#define IPC_MSG_CLOSEPROCESS    0x01
#define IPC_MSG_CREATEDATAMAP   0x02
#define IPC_MSG_DELETEDATAMAP   0x03

#endif