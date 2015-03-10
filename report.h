/////////////////////////////////////////////////////////////////////////////
// REPORT.H
//                                                                        
// report handling stuff for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __REPORT_H__
#define __REPORT_H__

#include <afxext.h>
#include "resource.h"
#include "device.h"
#include "plist.h"

// report fonts
#define REPORT_FONT_TITLE       0
#define REPORT_FONT_SUBTITLE    1
#define REPORT_FONT_DEVICE      2
#define REPORT_FONT_ENTRY		3
#define NUM_REPORT_FONTS		4

// report font styles
#define OUR_TEXT_NORMAL      0x00
#define OUR_TEXT_BOLD        0x01
#define OUR_TEXT_ITALIC      0x02

class CReport : public CObject
   {
public:
   CReport(){}
   ~CReport(){}

public:
   void BeginPrinting(CDC* pDC, CPrintInfo* pInfo); 
   void EndPrinting(CDC* pDC, CPrintInfo* pInfo);
   void Print(CDC* pDC, CPrintInfo* pInfo, CPseudoListBox* pLB, 
      CTime& QueueStartTime, CTimeSpan& QueueElapsedTime, CDevice& GlobalDevice);   
   void PrepareDC(CDC* pDC, CPrintInfo* pInfo);
   void SetPages(CDC* pDC, CPseudoListBox* pLB, CPrintInfo* pInfo, CDevice& GlobalDevice);

protected:
   void LoadLogFont(PLOGFONT pLogFont, int iPointSize, int Style, 
      CString csFaceName);   
   void PrintEntry(CDC* pDC, int TextPos, int ClipPos, const char *str);
   void PrintEntry(CDC* pDC, int TextPos, int ClipPos, CString& cs);

   int NextLine, PageCount;
   int cxChar;  
   CPoint point;
   int ReportTabCol[6];
   CFont* pReportFont[NUM_REPORT_FONTS];
   int nReportTextHeight[NUM_REPORT_FONTS];      
   };
#endif
