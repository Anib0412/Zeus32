/////////////////////////////////////////////////////////////////////////////
// DEVFACE.H
//                                                                        
// device interface defs for RasterPlus95
// Copyright (C) 1996-98 Graphx, Inc.
//
// mods:
//
// 7-16-96  Build 40 - moved FlipImage from pDevice->pDevInfo to JobInfo
// 12-24-96 Build 47 - added bounding box to devinfo
// 4-27-98  Version 2.0.3 - DEVICE_MEMUSED_MAX of 255000 set for Jaws 2002.3
//					      

#ifndef __DEVFACE_H__
#define __DEVFACE_H__

#include "ripstruct.h"
#include "rip.h"
#ifdef DEFAULT_GAMMA
#undef DEFAULT_GAMMA
#endif
#include "..\icc\initstruct.h"

#define UNUSED_LEN		 100
#define DEVINFOVERSION   0x0001
#define MAX_MEDIANAME    64
#define MAX_CONNECTNAME  _MAX_PATH
#define MAX_RESNAME      64 

// background colors
#define BACKGROUND_WHITE  1   
#define BACKGROUND_BLACK  0   

// line types
#define LINETYPE_RAW	 1
#define LINETYPE_RIPRLE	 2

// orientation
#define DEV_ORIENT_PORTRAIT    0
#define DEV_ORIENT_LANDSCAPE   1

// buffer to disk
#define DEV_BUFFERTODISK_DISABLE      0
#define DEV_BUFFERTODISK_ENABLE       1
#define DEV_BUFFERTODISK_ALWAYSALLOW  2

// prop box commands for 3.0
#define DEVPROPBOX_CREATE       0
#define DEVPROPBOX_DESTROY      1
#define DEVPROPBOX_SETACTIVE    2
#define DEVPROPBOX_KILLACTIVE	3

// media types
#define MEDIA_TYPE_NORMAL       0
#define MEDIA_TYPE_PACKAGE      0x01
#define MEDIA_TYPE_CUSTOM       0x02

// connection type (we only care about scsi right now)
#define CONNECT_TYPE_OTHER      0
#define CONNECT_TYPE_SCSI		1

// Quick Print FTypes
#define QUICK_PRINT_BMP			0x01
#define QUICK_PRINT_JPEG		0x02
#define QUICK_PRINT_TIFF		0x04
#define QUICK_PRINT_EXIF		0x10

// defines for getset proc
typedef struct tagGETDEVOPTIONSETTINGKEY
   {
   int iSetting;
   int iOption;
   } GETDEVOPTIONSETTINGKEY, *PGETDEVOPTIONSETTINGKEY;

#define DEVGETSET_GETDEVOPTIONNAME			0x01
#define DEVGETSET_GETDEVOPTIONSETTINGNAME	0x02
#define DEVGETSET_GETDEVOPTION				0x03
#define DEVGETSET_SETDEVOPTION				0x04

#define DEVGETSET_SETJOBID					0x1000 
#define DEVGETSET_GETBACKPRINT				0x1001
#define DEVGETSET_SETBACKPRINT				0x1002
#define DEVGETSET_GETPRINTONBACK			0x1003
#define DEVGETSET_SETPRINTONBACK			0x1004

// Structure for passing Backprint Information
/*
typedef struct tagDEVBACKPRINTDATA
   {
   int iBackprintLines;		// If this is 0 Backprint is not available
   int iNumKeys;
   int iMaxChars;
   char cBackprintData[_MAX_PATH];
   } DEVBACKPRINTDATA, *PDEVBACKPRINTDATA;
*/

typedef struct tagDEVBACKPRINTDATA
   {
   int iNumBackprintLines;         // If this is 0 Backprint is not available
   int iMaxLineLength;             // max length per line
   char cBackprintKeys[_MAX_PATH];
   BYTE* pDevInfo;        
   int iLineToSet;
   char cBackprintData[_MAX_PATH];
   } DEVBACKPRINTDATA, *PDEVBACKPRINTDATA;

typedef struct tagDEVPRINTONBACKDATA
   {
   BYTE* pDevInfo;        
   BOOL bPrintOnBack;         // If this is 0 Backprint is not available
   } DEVPRINTONBACKDATA, *PDEVPRINTONBACKDATA;

#pragma pack(1)
// the media
typedef struct tagMEDIA
   {   
   char Name[MAX_MEDIANAME];   
   char PPDName[MAX_MEDIANAME];   
   char Description[_MAX_PATH];   
   RECT Rect;
   int iType;
   } MEDIA, *PMEDIA;

#define DEVICE_VERSION_3000		3000
#define CURRENT_DEVICE_VERSION	5220

// the device information struct
typedef struct tagDEVINFO
   {
   WORD Version;            // version of this device
   WORD StructSize;         // size of structure, including extra info
   WORD BackgroundColor;    // the background color
   WORD Copies;   		    // number of copies to image
   WORD Orientation;   	    // landspace, portrait
   WORD CurrentMediaIdx;    // current media index
   WORD TotalMedia;         // total number of media slots
   WORD CurrentBinIdx;	    // current bin index
   WORD TotalBin;           // total number of media bins      
   WORD CurrentResIdx;      // current resolution index
   WORD TotalRes;		    // total number of resolution choices   
   char ConnectStr[_MAX_PATH]; // actual connection string
   WORD TotalConnect;	    // total number of connection choices   
   WORD CanFlipImage;	    // can the device flip an image?   
   DWORD MinScaleHeight;	// the minimum height of the scaled image
   DWORD MinScaleWidth;		// the minimum width of the scaled image
   DWORD MemUsed;			// how much memory to use for image processing
   BYTE MemUsedIdx;			// Auto or Custom mem alloc type
   BYTE EnableBufferToDisk;	// should we enable buffer to disk in AdvancedDevice
   BYTE UseCompression;		// compress data for pscript?   
   BYTE UseCompressionBmp;	// compress data for bitmap?   
   BYTE CanScaleImage;		// can the device internally scale the image?
   BYTE ScaleImage;         // tell the device to scale images
   BYTE CanDoOrientation;       // can the device set orientation of data output?
   BYTE CanOptimizeOrientation;	// can it optimize the orientation?
   BYTE OptimizeOrientation;	// should it optimize the orientation?
   BYTE CanDoCompression;	// can the device do compression?
   BYTE CanDoAntiAliasing;  // can the device do anti-aliasing?
   BYTE BufferToDisk;   	// buffer the image to disk before printing?
   BYTE AliasStyle;			// type of anti-aliasing
   BYTE AliasLevel;			// level of anti-aliasing
   DWORD BandSize;
   WORD NumBands;
   WORD BandWidth;
   short GotBoundingBox;   // make these short ints so devinfo is the same size as in 3.0.0
   BYTE bExtra;
   RECT BoundingBox;
   int iColorSpace;    // color space of the device       
   int CustomPage_Width;       // width in inches x 1000
   int CustomPage_Height;      // height in inches x 1000
   int CustomPage_XOffset;     // xoffset in inches x 1000
   int CustomPage_YOffset;     // yoffset in inches x 1000
   int CustomPage_Min_XOffset; // min xoffset in inches x 1000
   int CustomPage_Min_YOffset; // min yoffset in inches x 1000
   int CustomPage_MediaWidth;  // media width in inches x 1000   
   int BitmapSample;
   char Unused[UNUSED_LEN];
   BYTE CanCropImage;
   BYTE QuickPrintFType;
   BYTE UnusedByte3;
   BYTE UnusedByte4;
   DWORD Unused2;
   DWORD Unused3;
   DWORD Unused4;
   BYTE Private[1];		    // data private to device
   } DEVINFO, *PDEVINFO;
#pragma pack()

#define DEVICE_EXTRA_CANDOPACKAGES        0x01
#define DEVICE_EXTRA_CANDOCUSTOM          0x02
#define DEVICE_EXTRA_CANDOCUSTOMOFFSET    0x04

#define ANTIALIAS_STYLE_NONE      0
#define ANTIALIAS_STYLE_BOX       1
#define ANTIALIAS_STYLE_TRIANGLE  2

#define DEVICE_MEMUSED_MIN		4096      // at least 4M
#define DEVICE_MEMUSED_MAX		250000    // max in Jaws 2002.3!!!
#define RIPMEM_ALLOC_WORKSTATION  0
//#define RIPMEM_ALLOC_SERVER_1D    1
//#define RIPMEM_ALLOC_SERVER_2D    2
//#define RIPMEM_ALLOC_SERVER_3D    3
//#define RIPMEM_ALLOC_SERVER_4D    4
//#define RIPMEM_ALLOC_CUSTOM       5
#define RIPMEM_ALLOC_SERVER    1
#define RIPMEM_ALLOC_CUSTOM    2

// mode changes
#define DEVMODE_CHANGE_MEDIA          1
#define DEVMODE_CHANGE_MEDIA_PRMPT    2

// allocate device for doing stuff
// returns a handle (actually a pointer to a class)

#define DLLEXPORT __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#endif

// allocate the device (get the C++ class pointer)
DWORD DLLEXPORT AllocDevice(char *DevName, int DevIdx);
// free our device
void DLLEXPORT FreeDevice(DWORD dwDevHandle);
// get/set device information that is useful to callers
// call with pd = NULL for size of struct
int DLLEXPORT GetDevInfo(DWORD dwDevHandle, PDEVINFO pd);
// if bRegister is TRUE, commit to registry
void DLLEXPORT SetDevInfo(DWORD dwDevHandle, PDEVINFO pd, BOOL bRegister);
// get media name and size
BOOL DLLEXPORT GetMedia(DWORD dwDevHandle, int idx, PMEDIA pMedia);
// get bin name
BOOL DLLEXPORT GetMediaBin(DWORD dwDevHandle, int idx, char Name[MAX_MEDIANAME]);
// get connect name and size
BOOL DLLEXPORT GetConnect(DWORD dwDevHandle, int idx, char Name[MAX_CONNECTNAME], int* pConnectType);
// parse server tokens
void DLLEXPORT ParseServerTokens(DWORD dwDevHandle, const char* csToken, const char* csValue);
// make spd file
BOOL DLLEXPORT MakeSPDFile(DWORD dwDevHandle, const char* csSPDName, const char* csShortName);
// get res name and size
BOOL DLLEXPORT GetRes(DWORD dwDevHandle, int idx, char Name[MAX_RESNAME]);
// prop box stuff
BOOL DLLEXPORT DoPropBox(DWORD dwDevHandle, int cmd, CWnd* pCw);
BOOL DLLEXPORT GetSetExtra(DWORD dwDevHandle, int cmd, BYTE* pIn, BYTE* pOut, 
						   int iLenIn, int iLenOut);

// printing stuff
BOOL DLLEXPORT OpenDevice(DWORD dwDevHandle, PRIPSTRUCT pRipStruct, PJOBINFO pJobInfo, int* daf);
void DLLEXPORT CloseDevice(DWORD dwDevHandle);
void DLLEXPORT Abort(DWORD dwDevHandle);
BOOL DLLEXPORT GetDevError(DWORD dwDevHandle, char *pError);
BOOL DLLEXPORT DoDialog(DWORD dwDevHandle, HWND hWnd);
BOOL DLLEXPORT FillRipStruct(DWORD dwDevHandle, PRIPSTRUCT pRipStruct);
int DLLEXPORT OutputBytes(DWORD dwDevHandle, BYTE *buf, int len, int y);

#ifdef __cplusplus
}
#endif

#endif


