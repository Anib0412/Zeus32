/////////////////////////////////////////////////////////////////////////////
// PREFSMSG.H
//                                                                        
// Properties change notification message defs
// Copyright (C) 1996-99 Graphx, Inc.
//

#ifndef __PREFSMSG_H__
#define __PREFSMSG_H__

#define ID_DEVMODE_CHANGE "DevMode_Change30"
#define ID_DEVMODE_CHANGE_DEFAULT "DevMode_Change_Default30"
#define ID_DEVINFO_CHANGE "DevInfo_Change"

#define PREFS_CHANGE_DEVICE           0x01 
#define PREFS_CHANGE_TRANSFORM        0x02
#define PREFS_CHANGE_COLOR            0x04
#define PREFS_CHANGE_NOTES            0x08
#define PREFS_CHANGE_DESCRIPTION      0x10
#define PREFS_CHANGE_PRANGE           0x20

//Messages recieved from the device
#define DEVICE_CHANGE_DEVINFO         0x01	
//Messages recieved from the device

#define PREFS_CHANGE_REPORT_YES       0x10
#define PREFS_CHANGE_REPORT_NO        0

#define DEVCHANGE_RESIDX               0x0001
#define DEVCHANGE_COPIES			   0x0002
#define DEVCHANGE_SCALEIMAGE		   0x0004
#define DEVCHANGE_ALIASSTYLE		   0x0008
#define DEVCHANGE_ALIASLEVEL           0x0010
#define DEVCHANGE_BUFFERTODISK		   0x0020
#define DEVCHANGE_USECOMPRESSION	   0x0040
#define DEVCHANGE_USECOMPRESSIONBMP	   0x0080
#define DEVCHANGE_MEDIAIDX			   0x0100
#define DEVCHANGE_CONNECTIDX		   0x0200
#define DEVCHANGE_MEMUSED			   0x0400
#define DEVCHANGE_ORIENTATION		   0x0800
#define DEVCHANGE_ORIENTATION_OPT	   0x1000
#define DEVCHANGE_DESCRIPTION          0x2000 
#define DEVCHANGE_CUSTOMPAGE           0x4000 
#define DEVCHANGE_PRANGE               0x8000
#define DEVCHANGE_BITMAPSAMPLING	   0x10000

#define XFORMCHANGE_STARTTYPE          0x0001
#define XFORMCHANGE_DESCRIPTION        0x0002
#define XFORMCHANGE_ROTATE			   0x0004
#define XFORMCHANGE_XZOOM			   0x0008
#define XFORMCHANGE_YZOOM			   0x0010
#define XFORMCHANGE_XOFF			   0x0020
#define XFORMCHANGE_YOFF			   0x0040

#define XFORMCHANGE_ALL  (XFORMCHANGE_STARTTYPE | XFORMCHANGE_DESCRIPTION | \
                          XFORMCHANGE_ROTATE | XFORMCHANGE_XZOOM |          \
						  XFORMCHANGE_YZOOM | XFORMCHANGE_XOFF | 			\
						  XFORMCHANGE_YOFF)             
#endif

#define CMMCHANGE_MAXBLACK				0x00001
#define CMMCHANGE_MAXINK				0x00002
#define CMMCHANGE_GCRUCR				0x00004
#define CMMCHANGE_BRIGHTNESS			0x00008
#define CMMCHANGE_CONTRAST				0x00010
#define CMMCHANGE_COLORLEVELS			0x00020
//#define CMMCHANGE_GRAPHLINES			0x00040
#define CMMCHANGE_GRAPHLINES_ADJ		0x00040
#define CMMCHANGE_ADJLOCK				0x00041
#define CMMCHANGE_GAMMA					0x00080
#define CMMCHANGE_TRANSTYPE				0x00100
#define CMMCHANGE_PRINTERCSPACE			0x00200
#define CMMCHANGE_INPUTCSPACE			0x00400
#define CMMCHANGE_PREVIEW				0x00800
#define CMMCHANGE_PREPRESS				0x01000
#define CMMCHANGE_WHITEISWHITE			0x02000
#define CMMCHANGE_BLACKISBLACK			0x04000
#define CMMCHANGE_RGBINPUTNAME			0x08000
#define CMMCHANGE_CMYKINPUTNAME			0x10000
#define	CMMCHANGE_MONITORNAME			0x20000
#define CMMCHANGE_PRESSNAME				0x40000
#define CMMCHANGE_PRINTERNAME			0x80000
#define CMMCHANGE_GRAPHLINES_TRN		0x80001
#define CMMCHANGE_TRNSLOCK				0x80002
#define CMMCHANGE_IMAGE_RENDINTENT		0x80004
#define CMMCHANGE_TEXT_RENDINTENT		0x80008

#define CMMCHANGE_ALL					0xFFFFF
