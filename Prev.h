/////////////////////////////////////////////////////////////////////////////
// PREV.H
//                                                                        
// preview engine defs for RasterPlus95
// Copyright (C) 1996-2000 Graphx, Inc.
//
// mods:
//
//  9-21-98  Version 2.0.5 - no matching pragma pack()
//  7-12-00  Version 4.0   - add job id and remove pragma pack
//  9-20-01  Version 5.0.0 - Add preview type to support High Definition preview

#ifndef __PREV_H__
#define __PREV_H__
#define PREV_SETINFO       "PrevMsg30_SetInfo"

typedef struct tagPREVIEWINFO
   {
   long Height;
   long Width;   
   long ImageSize;
   char Title[_MAX_PATH]; 
   BYTE *ImageBuffer;
   HWND hWndParent;
   int iJobID;              // the job id
   UINT uiRipSessionID;     // the idea of this rip session
   int iPageNum;			// the page we're ripping
   RECT crMediaRect;
   int RipPixHeight;
   int RipPixWidth;
   int RipWPixelsPerInch;
   int RipHPixelsPerInch;
   char LongName[_MAX_PATH]; 
   int iPreviewType;		// 1=Normal, 3=HighDefinition
   int iZoomValue;
   int iInitialZoomValue;
   } PREVIEWINFO;

typedef PREVIEWINFO * PPREVIEWINFO;   
#endif                             

