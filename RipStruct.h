////////////////////////////////////////////////////////////////////////////
// RIPSTRUCT.H
//                                                                        
// RipStruct definition for RasterPlus95
// Copyright (C) 1996-98 Graphx, Inc.
//
// mods:
//
// 7-09-96  Build 40 - added MonoFlag to RIPSTRUCT
// 4-29-97  Build 49 - added Dongle code and demo code to RIPSTRUCT
// 5-26-97  Build 50 - added color correction stuff
// 9-22-97  Build 51 - changed dpi to floats for more accurate previewing
//                     added FilePasses to RipStruct for multiple band/copy handling
// 9-10-98  Version 2.0.5 - added new device rip loading code (disabled for now)
// 3-30-99  Version 2.0.6 - added MaxScaleWidth/Height for bitmaps
// 2-25-00	Version 3.0.3 - Added DefaultPrinterProfile field 

#ifndef __RIPSTRUCT_H__
#define __RIPSTRUCT_H__

#ifdef DEFAULT_GAMMA
#undef DEFAULT_GAMMA
#endif
#include "..\icc\initstruct.h"

#define UNUSED_LEN 100

// the state of the device
#define DEVICE_NORMAL_PROCESS		0
#define DEVICE_CANCEL_PROCESS		1	
#define DEVICE_PAUSE_PROCESS		2
#define DEVICE_CANCELALL_PROCESS	3

// type of rip
#define RIPSTRUCT_RIPTYPE_FILEBANDC     0
#define RIPSTRUCT_RIPTYPE_FILEBANDS     1
#define RIPSTRUCT_RIPTYPE_FILEPAGE      2
#define RIPSTRUCT_RIPTYPE_FILEBAND      3
#define RIPSTRUCT_RIPTYPE_FILEBANDC_SEP 4

// type of output proc
#define RIPSTRUCT_OUTPUTSTYLE_PLANAR    0
#define RIPSTRUCT_OUTPUTSTYLE_ILEAVE 	1

// color stuff
#define MAX_COLORCORRECT_PLANES         4
#define MAX_COLORCORRECT_COLORS         256

// bitmap color correction stuff
#define DEVICE_COLOR_MODEL_RGB          0
#define DEVICE_COLOR_MODEL_CMY          1
#define DEVICE_COLOR_MODEL_CMYK         2

typedef struct tagCTM_ARRAY
   {
   float val[6];
   } CTM_ARRAY;

typedef struct tagPACKAGEDETAILS
   {
   BOOL IsDoingPackage;
   char PackageININame[_MAX_PATH];
   char LongFileName[_MAX_PATH];
   int iPackageIdx;
   int iPixDevPageWidth;
   int iPixDevPageHeight;         
   float WPixelsPerInch;
   float HPixelsPerInch;
   int iPreviewSize;
   int iCropStyle;
   } PACKAGEDETAILS; 

typedef  struct tagRIPSTRUCT
   {
   int RipType;
   int OutputStyle;   
   int FramePasses;
   int PixWidth;
   int PixHeight;
   float WPixelsPerInch;
   float HPixelsPerInch;
   int NumBands;
   DWORD MemUsed;
   CTM_ARRAY InitMatrix;
   BYTE framefill;   
   BOOL PrepScanData;
   char PrepDLLName[_MAX_PATH];
   char PrepProcName[_MAX_PATH];
   char ScratchFolder[_MAX_PATH];
   int PrepOffset;
   int Rotation;
   BOOL MonoFlag;
   int QuickPrintImage;
   int DongleKeyCode;
   BOOL DemoVersion;
   BOOL DoColorTransfer;
   int DeviceColorModel;
   char DefaultPrinterProfile[_MAX_PATH];
   CPINITSTRUCT CPInitStruct;
   BOOL LoadDevFromRip;
   int MaxScaleHeight;	// the maximum height of the scaled image
   int MaxScaleWidth;		// the maximum width of the scaled image
   PACKAGEDETAILS PackageDetails;   
   int iScanLineOffset;
   BOOL bDumpGangPrints;
   char SharedMemoryName[_MAX_PATH];
   char unused [UNUSED_LEN];
   DWORD Unused1;
   DWORD Unused2;
   DWORD Unused3;
   DWORD Unused4;
   } RIPSTRUCT, *PRIPSTRUCT;

#define RIPSTYLE_PREVIEW	1
#define RIPSTYLE_OUTPUT		2
#define RIPSTYLE_HD_PREVIEW	3

#define SYN_ERRMSG_ALLOK            0  /* No Errors during rip */
#define SYN_ERRMSG_CANCELALL        3  /* User aborted all images */
#define SYN_ERRMSG_CANCELIMAGE      6  /* User aborted this image */
#define SYN_ERRMSG_NOPREVIEW        7  /* Error loading preview */
#define SYN_ERRMSG_UNDEFINED        8  /* we dont know what happened */

// Pete's errors                                                       
#define SYN_ERRMSG_RIPNOTFOUND        9 // can't find rip
#define SYN_ERRMSG_RIPDLLNOTFOUND    10 // can't find rip dll
#define SYN_ERRMSG_DEVICEERROR       11 // device error
#define SYN_ERRMSG_BADFORMAT         12 // bad file format
#define SYN_ERRMSG_RIPTHRDBAD        13 // can't load rip thread
#define SYN_ERRMSG_FILENOTFOUND		 14 // can't find file
#define SYN_ERRMSG_FILEWASDELETED    15 // file was deleted 
#define SYN_ERRMSG_NOROTATE_BMP      16 // can't rotate bitmap file
#define SYN_ERRMSG_RIPDEVICEERROR    17 // device error (from rip)
#define SYN_ERRMSG_NODEFAULTPROFILE  18 // Default Printer Profile not set
#define SYN_ERRMSG_INVALIDPROFILE    19 // ICC Profile Missing or Invalid
#define SYN_ERRMSG_DEVINIT           20 // device init error 
#define SYN_ERRMSG_UNSUPPORTEDFORMAT 21 // File format not supported
#define RIPSTRUCT_MAXPIXWIDTH       6

#define RIPSTRUCT_MAX_ERRSTRLEN     256
#endif