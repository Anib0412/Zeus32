////////////////////////////////////////////////////////////////////////////
// RIP.H
//                                                                        
// Rip message defs and JOBINFO struct for RasterPlus
// Copyright (C) 1996-2000 Graphx, Inc.
//
// mods:
//
// 7-16-96  Build 40 - moved FlipImage from pDevice->pDevInfo to JobInfo
// 9-10-99  Version 3.0.0 - changed messages for Version 3
// 7-14-00  Version 4.0.0 - added crMediaRect, RipPixHeight et. al. for preview rulers
// 7-19-00  Version 4.0.0 - added print page selection
//					      

#ifndef __RIP_H__
#define __RIP_H__

#define RIP_SENDMESSAGE	    "RipMsg30_SendMessage"
#define RIP_SENDPSMESSAGE	"RipMsg30_SendPSMessage"
#define RIP_COMPLETE  	    "RipMsg30_Complete"
#define RIP_ADDTOPANE       "RipMsg30_AddToPane"
#define RIP_START_POLLING   "RipMsg30_StartPolling"
#define RIP_STOP_POLLING    "RipMsg30_StopPolling"
#define RIP_PRINTJOBID      "RipMsg30_PrintJobID"

// add to pane struct
typedef struct tagADDTOPANESTRUCT
   {
   int iViewStyle;   
   BOOL bDoSubPage;
   int iSubPage;
   } ADDTOPANESTRUCT, *PADDTOPANESTRUCT;

#define MAX_RIP_STR_LEN        _MAX_PATH
#define JOBINFO_OWNER_STRLEN	66
#define JOBINFO_PHOTOGIZE_ORDER	64
//#define JOBINFO_UNUSED_STRLEN	64
#define JOBINFO_UNUSED_STRLEN	60

typedef struct tagJOBINFO
   {
   char LongName[MAX_RIP_STR_LEN],
        ShortName[MAX_RIP_STR_LEN];
   int OutputType;
   int FileType;
   int RipType;
   int ViewPerspective;
   HWND hFormWin;
   int MultiTasking;      
   short CATHeaderNeeded;
   long StartofValidPS;	  
   HWND hwndPreviewOwner;   
   BOOL ResetDevice;
   DWORD RealFileSize;
   int BackgroundColor;
   CRITICAL_SECTION BitmapEngineMutex;
   int PCDResolution;
   BYTE AliasStyle;			// type of anti-aliasing
   BYTE AliasLevel;			// level of anti-aliasing
   BOOL ReverseLine;        // need to reverse line?
   BOOL FlipImage;		    // force device to flip image (only valid on devs that
							// can flip image - see DevInfo)
   BOOL IsComplete;
   char MutexName[MAX_RIP_STR_LEN];
   int iJobID;              // the job id
   UINT uiRipSessionID;     // the idea of this rip session
   int iPageNum;			// the page we're ripping
   RECT crMediaRect;
   int RipPixHeight;
   int RipPixWidth;
   int RipWPixelsPerInch;
   int RipHPixelsPerInch;   
   unsigned short usNumPagesToPrint;          // if 0, print all, otherwise, print this number of pages
   unsigned short usPageSelection[MAX_RIP_STR_LEN]; // filled with the pages to print 0 based
   //char Description[_MAX_PATH];
   char lpOwner[JOBINFO_OWNER_STRLEN];
   char lpSubmitted[JOBINFO_OWNER_STRLEN];
   char lpPhotogizeOrder[JOBINFO_PHOTOGIZE_ORDER];   
   int BitmapSample;
   char Unused[JOBINFO_UNUSED_STRLEN];
   BYTE QuickPrintImage;	// This will be set if the device can Scale and Size a bitmap to the appropriate output size
   BYTE JobSizing;
   BYTE UnusedByte2;
   BYTE UnusedByte3;
//   DWORD Unused1;
   DWORD Unused2;
   DWORD Unused3;
   DWORD Unused4;
   } JOBINFO, *PJOBINFO;

#define RIP_TYPE_PSCRIPT	   0
#define RIP_TYPE_BITMAP        1

// background colors
#define BACKGROUND_WHITE  1   
#define BACKGROUND_BLACK  0   

#define ANTIALIAS_STYLE_NONE      0
#define ANTIALIAS_STYLE_BOX       1
#define ANTIALIAS_STYLE_TRIANGLE  2

#define BITMAPSAMPLING_NEAREST    0
#define BITMAPSAMPLING_BILINEAR   1
#define BITMAPSAMPLING_BICUBIC    2

#endif
