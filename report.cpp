/////////////////////////////////////////////////////////////////////////////
// REPORT.CPP
//                                                                        
// report handling stuff for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//
// mods:
//
// 12-04-98 Version 2.0.5 - added UpdateMediaList
// 2-20-00  Version 3.0.3 - added UpdateConnectList


#include "report.h"
#include "cqueue.h"
#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REPORT_YMARGIN  1440  // 1 inch
#define REPORT_XMARGIN  1440  // 1 inch

void CReport::Print(CDC* pDC, CPrintInfo* pInfo, CPseudoListBox* pLB, 
   CTime& QueueStartTime, CTimeSpan& QueueElapsedTime, CDevice& GlobalDevice)
    {   	
	int          i;    	
	CString      csOutput;	
	CString      csTimeString, csFmtStr;
	CDevice*     pThisDevMode;
	CDevice      LastDevMode;
	MEDIA 		 Media;
	CQueueFile*  pCQueueFile;   		

	// set twips map mode
    pDC->SetMapMode(MM_TWIPS);

	// we're on the first page!!!
	if (pInfo->m_nCurPage == 1)  
	   {	   
	   // we want our margins to be 1" around
	   //point.x = max(1440 - pInfo->m_rectDraw.left, 1440);
	   //point.y = min(-1440 - pInfo->m_rectDraw.top, -1440);
	   // reset point.y to y margin
	   point.x = REPORT_XMARGIN;
	   point.y = -REPORT_YMARGIN;
	   // do blue for the first few lines
	   pDC->SetTextColor(RGB(0, 0, 255));    	
	   // select the title font
	   pDC->SelectObject(pReportFont[REPORT_FONT_TITLE]);
	   // get the title
	   csOutput.LoadString(IDS_REPORT_TITLE);	   
	   // write it
       pDC->ExtTextOut(point.x, point.y, 0, NULL, (const char*)csOutput, 
          csOutput.GetLength(), NULL);	       
	   // decrement our point 
	   point.y -= nReportTextHeight[REPORT_FONT_TITLE];

	   // select the subtitle font
	   pDC->SelectObject(pReportFont[REPORT_FONT_SUBTITLE]);	   	   
	   // get the numimages format string
	   csFmtStr.LoadString(IDS_REPORT_SUBTITLE);
	   // make the text
	   csOutput.Format((const char*)csFmtStr, pLB->GetCount());
	   // write it
       pDC->ExtTextOut(point.x, point.y, 0, NULL, (const char*)csOutput, 
          csOutput.GetLength(), NULL);	       
	   // decrement our point 	   
	   point.y -= nReportTextHeight[REPORT_FONT_SUBTITLE];

	   	// get the start time
	   if (QueueStartTime != 0)	     
	      csTimeString = 
	      QueueStartTime.Format("%I:%M:%S%p on %B %d, %Y");
	   else
	      csTimeString.LoadString(IDS_NOT_AVAIL);
	   // select the subtitle font
	   pDC->SelectObject(pReportFont[REPORT_FONT_SUBTITLE]);	   	   
	   // get the session start format string
	   csFmtStr.LoadString(IDS_REPORT_SESSION_START);
	   // make the text
	   csOutput.Format((const char*)csFmtStr, csTimeString);
	   // write it
       pDC->ExtTextOut(point.x, point.y, 0, NULL, (const char*)csOutput, 
          csOutput.GetLength(), NULL);	       
	   // decrement our point 	   
	   point.y -= nReportTextHeight[REPORT_FONT_SUBTITLE];

	   // get lines per page and reset line count
	   // this is not right! - need to account for larger headers
	   //LinesPerPage = 
	   //   abs((pInfo->m_rectDraw.Height() + 2880) / nReportTextHeight[REPORT_FONT_ENTRY]);              
	   NextLine = 0;	   
       PageCount = 0;       	   
	   }
	
	// by default, we're finished
    pInfo->m_bContinuePrinting = FALSE;   
    
	// do black for other stuff
	pDC->SetTextColor(RGB(0, 0, 0));    	

	// do all the remaining queue files
	for (i = NextLine; i < pLB->GetCount(); i++)
	   {
	   // get queue file
       pLB->GetText(i, (LPSTR)&pCQueueFile);
	   // get the device
	   if ((pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS) && pCQueueFile->pCD)
          pThisDevMode = pCQueueFile->pCD;
       else
          pThisDevMode = &GlobalDevice;
	   
	   // if we have a new device, make sure that there is enough room to print device
	   // name and entry, else just look for enough to print entry.  if not go on
	   // to the next page
	   if ((abs(point.y) + REPORT_YMARGIN + 	        
			((LastDevMode != *pThisDevMode) ? 
	        nReportTextHeight[REPORT_FONT_DEVICE] + 
			nReportTextHeight[REPORT_FONT_ENTRY] :
			nReportTextHeight[REPORT_FONT_ENTRY]))
	        > abs(pInfo->m_rectDraw.Height()))
	      {
	      PageCount++;
		  // reset point.y to y margin
	      point.y = -REPORT_YMARGIN;	   
	      pInfo->m_bContinuePrinting = TRUE;
	      NextLine = i;
	      break;
	      }	      	   
	   // if a different device, show it (first time through should always work!)
	   // Note: this will force true at the top of any new page
	   if (LastDevMode != *pThisDevMode)
	      {
	      // same now
	      LastDevMode = *pThisDevMode;
	  
	      // select the device font
	      pDC->SelectObject(pReportFont[REPORT_FONT_DEVICE]);	   	   
	      // get the device format string
	      csFmtStr.LoadString(IDS_REPORT_DEVICE);
		  // get the media
		  pThisDevMode->TotalMedia = pThisDevMode->UpdateMediaList();
		  pThisDevMode->GetMedia(pThisDevMode->pDevInfo->CurrentMediaIdx, &Media);	  
	      // make the text
	      csOutput.Format((const char*)csFmtStr, 
   	         (const char*)pThisDevMode->Name,  // name		 
	         (const char*)Media.Name);         // media
          // write it
          pDC->ExtTextOut(point.x, point.y, 0, NULL, (const char*)csOutput, 
             csOutput.GetLength(), NULL);	       
	      // decrement our point 	      
	      point.y -= nReportTextHeight[REPORT_FONT_DEVICE];
		  }
       // set entry font
	   pDC->SelectObject(pReportFont[REPORT_FONT_ENTRY]);	   
	   // print the queue number
	   csOutput.Format("%d", i + 1);
	   PrintEntry(pDC, 0, 1, csOutput);
	   // print the filename		  
	   PrintEntry(pDC, 1, 2, pCQueueFile->ShortName);
	   // print the description
	   PrintEntry(pDC, 2, 3, pCQueueFile->Description);
	   // print the result	   
	   PrintEntry(pDC, 3, 5, pCQueueFile->Status);
	   // move down
	   point.y -= nReportTextHeight[REPORT_FONT_ENTRY];		  	  
	   }
   }  

void CReport::PrepareDC(CDC* pDC, CPrintInfo* pInfo)
   {
   pDC->SetMapMode(MM_TWIPS);
   }
   
void CReport::BeginPrinting(CDC* pDC, CPrintInfo* pInfo)
   {   
   CString csFontName;
   LOGFONT LogFont;
   TEXTMETRIC Print_tm;  
   int cxCaps;	
   int i;      

   // get the font name
   csFontName.LoadString(IDS_REPORT_FONT);	

   // Note: this is the Japanese string for
   // MS Gothic that we load from the string resource
   //csFontName = "‚l‚r –¾’©";						 

   for (i = REPORT_FONT_TITLE; i <= REPORT_FONT_ENTRY; i++)
      {
      // make the font
      if (pReportFont[i] = new CFont)
	     {
		 switch(i)
		    {
			// special stuff
			case REPORT_FONT_TITLE:
				 LoadLogFont(&LogFont, 14, OUR_TEXT_BOLD, csFontName);
				 break;
			case REPORT_FONT_SUBTITLE:
				 LoadLogFont(&LogFont, 12, OUR_TEXT_BOLD, csFontName);
				 break;
			case REPORT_FONT_DEVICE:
#ifdef USEKANJI
			     LoadLogFont(&LogFont, 10, OUR_TEXT_NORMAL, csFontName);				 
#else
				 LoadLogFont(&LogFont, 10, OUR_TEXT_ITALIC, csFontName);
#endif
				 break;
			case REPORT_FONT_ENTRY:
				 LoadLogFont(&LogFont, 10, OUR_TEXT_NORMAL, csFontName);
				 break;
			}
         pReportFont[i]->CreateFontIndirect((const LOGFONT*)&LogFont);   
         // select the font
         pDC->SelectObject(pReportFont[i]);   
         // get the metrics
         pDC->GetTextMetrics(&Print_tm);
         // get the char height
		 nReportTextHeight[i] = Print_tm.tmHeight + Print_tm.tmExternalLeading; 
		 }
	  else
	     {
		 // TEST THIS!!!!!
	     pInfo->m_bContinuePrinting = FALSE;
		 break;
		 }
	 
	  cxChar = Print_tm.tmAveCharWidth;
      cxCaps = (Print_tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;    
      ReportTabCol[0] = 0;                
      ReportTabCol[1] = 5 * cxCaps;	 
      ReportTabCol[2] = 20 * cxCaps;	
      ReportTabCol[3] = 45 * cxCaps;	
      ReportTabCol[4] = 55 * cxCaps;	   
	  ReportTabCol[5] = 79 * cxCaps;	   
	  }
   }   
   
void CReport::EndPrinting(CDC* pDC, CPrintInfo* pInfo)   
   {               
   int i;

   // select any font out of the DC by selecting a stock font   
   pDC->SelectStockObject(SYSTEM_FONT);     
      			 
   // delete our fonts             
   for (i = REPORT_FONT_TITLE; i <= REPORT_FONT_ENTRY; i++)
      {
	  if (pReportFont[i])
	     {
		 pReportFont[i]->DeleteObject();
	     delete pReportFont[i];
		 }
	  }
   }

void CReport::LoadLogFont(PLOGFONT lpLogFont, int iPointSize, 
   int Style, CString csFaceName)
   {
   // make a log font   
   // convert points to twips
   // 1/20 pt == 1/1440 inch == 1 logical unit (twip)
   lpLogFont->lfHeight = -iPointSize * 20;
   lpLogFont->lfWidth = 0;
   lpLogFont->lfEscapement = 0;
   lpLogFont->lfOrientation = 0;
   lpLogFont->lfWeight = ((Style & OUR_TEXT_BOLD) ? FW_BOLD : FW_NORMAL);
   lpLogFont->lfItalic = ((Style & OUR_TEXT_ITALIC) ? TRUE : FALSE);
   lpLogFont->lfUnderline = FALSE;
   lpLogFont->lfStrikeOut = FALSE;
#ifdef USEKANJI
   lpLogFont->lfCharSet = DEFAULT_CHARSET;
#else
   lpLogFont->lfCharSet = ANSI_CHARSET;
#endif   
   lpLogFont->lfOutPrecision = OUT_DEFAULT_PRECIS;
   lpLogFont->lfClipPrecision = CLIP_DEFAULT_PRECIS;
   lpLogFont->lfQuality = DEFAULT_QUALITY;
   lpLogFont->lfPitchAndFamily = FF_SWISS;
   strncpy(lpLogFont->lfFaceName, (const char*)csFaceName, LF_FACESIZE);
   }		  
   
void CReport::PrintEntry(CDC* pDC, int TextPos, int ClipPos, const char *str)
   {
   CRect cr;
   
   // make clip rect
   cr.left = ReportTabCol[TextPos] + point.x; 
   cr.right = ReportTabCol[ClipPos] - cxChar + point.x;
   cr.top = point.y; cr.bottom = point.y - nReportTextHeight[REPORT_FONT_ENTRY];              
   // output the text
   pDC->ExtTextOut(cr.left, point.y, ETO_CLIPPED, cr, str, strlen(str), NULL);	
   }

void CReport::PrintEntry(CDC* pDC, int TextPos, int ClipPos, CString& cs)
   {
   PrintEntry(pDC, TextPos, ClipPos, (const char*)cs);
   }
   
void CReport::SetPages(CDC* pDC, CPseudoListBox* pLB, CPrintInfo* pInfo, CDevice& GlobalDevice)
   {
   int i;
   int NextLine;
   int PageCount;
   CDevice*     pThisDevMode;
   CDevice      LastDevMode;
   CQueueFile*  pCQueueFile;   		
   BOOL m_bContinuePrinting;

   // must do this in order to set vars
   BeginPrinting(pDC, pInfo);
   
   NextLine = 0;
   PageCount = 1;
   point.y = -REPORT_YMARGIN;
   point.y -= nReportTextHeight[REPORT_FONT_TITLE];
   point.y -= nReportTextHeight[REPORT_FONT_SUBTITLE];
   point.y -= nReportTextHeight[REPORT_FONT_SUBTITLE];

   // by default, we're finished
   m_bContinuePrinting = FALSE;   
   do
      {
	  for (i = NextLine; i < pLB->GetCount(); i++)
	     {
	     // get queue file
         pLB->GetText(i, (LPSTR)&pCQueueFile);
	     // get the device
	     if ((pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS) && pCQueueFile->pCD)
            pThisDevMode = pCQueueFile->pCD;
         else
            pThisDevMode = &GlobalDevice;
	   
	     // if we have a new device, make sure that there is enough room to print device
	     // name and entry, else just look for enough to print entry.  if not go on
	     // to the next page
	     if ((abs(point.y) + REPORT_YMARGIN + 	        
		  	((LastDevMode != *pThisDevMode) ? 
	        nReportTextHeight[REPORT_FONT_DEVICE] + 
			nReportTextHeight[REPORT_FONT_ENTRY] :
			nReportTextHeight[REPORT_FONT_ENTRY]))
	        > abs(pInfo->m_rectDraw.Height()))
	        {
	        PageCount++;
  		    // reset point.y to y margin
	        point.y = -REPORT_YMARGIN;	   
	        m_bContinuePrinting = TRUE;
	        NextLine = i;
	        break;
	        }	      	   
	     // if a different device, show it (first time through should always work!)
	     // Note: this will force true at the top of any new page
	     if (LastDevMode != *pThisDevMode)
	        {
	        // same now
	        LastDevMode = *pThisDevMode;	  
	        point.y -= nReportTextHeight[REPORT_FONT_DEVICE];
		    }
         point.y -= nReportTextHeight[REPORT_FONT_ENTRY];		  	  
	     }
	  } while(m_bContinuePrinting); 

   pInfo->SetMinPage(0);
   pInfo->SetMaxPage(PageCount);   

   // must do this in order to reset vars
   EndPrinting(pDC, pInfo);
   }  
