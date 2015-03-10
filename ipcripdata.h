/////////////////////////////////////////////////////////////////////////////
// IPCRipData.h
//                                                                        
// rip child defines for RasterPlus95
// Copyright (C) 1996-98 Graphx, Inc.
//
// Mods:
// 9-10-98  Version 2.0.5 - added new device rip loading code (disabled for now)

#ifndef __IPCRIPDATA_H__
#define __IPCRIPDATA_H__

#include"ripmsg.h"
#include"ripstruct.h"

#define IPC_IDLE                  0

// rip-specific IPC messages (start at 0x100)
#define IPC_SETRIPINITPORT        0x101
#define IPC_SETRIPINFOPORT        0x102
#define IPC_SETRIPSCANPORT        0x103
#define IPC_SETRIPMSGPORT		  0x104
#define IPC_SCANLINEREADY	      0x105
#define IPC_RIPCOMPLETE           0x106
#define IPC_RIPERROR              0x107
#define IPC_CLOSERIP		      0x108
#define IPC_INITRIP		          0x109
#define IPC_SENDINITDATA          0x10A
#define IPC_SENDRIPINFO           0x10B 
#define IPC_RIPFILE               0x10C
#define IPC_PERCENTFILECOMPLETE	  0x10D
#define IPC_SUSPEND				  0x10E
#define IPC_RESUME				  0x10F
#define IPC_CANCEL				  0x111
#define IPC_RENDERTICK			  0x112
#define IPC_SENDPSMESSAGE         0x113
#define IPC_RENDERTITLE           0x114
#define IPC_ROTATEBMP			  0x116
#define IPC_TOTALFILECOMPLETE	  0x117
#define IPC_SENDABORTDEVICE       0x118
#define IPC_SCANLINEREADY_OFFSET  0x119

// new device stuff			   
#define IPC_SETDEVEXTRASLOT       0x200
#define IPC_DEVICE_OPEN		      0x201
#define IPC_DEVICE_CLOSE		  0x202
#define IPC_DEVICE_OUTPUTBYTES    0x203
#define IPC_DEVICE_ABORT		  0x204
#define IPC_DEVICE_CREATEPSINIT	  0x205

#define MAX_PSPATHS          4096
#define MAX_INITDATA         4096
#define PSMSG_MAXLEN		 _MAX_PATH

#define PSMSG_PSOUT          0
#define PSMSG_PSERR          1

typedef struct tagRIPIPCDATA
   {   
   char Paths[MAX_PSPATHS];
   char DLLName[_MAX_PATH];   	  
   char InitData[MAX_INITDATA];
   char BitmapDll[_MAX_PATH];
   } RIPIPCDATA, * PRIPIPCDATA;

// new device stuff			   
typedef struct IPCDEVEX
   {      
   int DevIdx;
   char pDevName[_MAX_PATH];
   char pDevDLLName[_MAX_PATH];   
   int iDevInfoLen;
   int DevProcessFlag;
   char pErrorMsg[RIPSTRUCT_MAX_ERRSTRLEN];
   RIPMSGSTRUCT RipMsg;
   PRIPMSGSTRUCT pHost_RipMsg;
   BYTE pDevInfo[1];
   } IPCDEVINFO, * PIPCDEVINFO;

#endif
