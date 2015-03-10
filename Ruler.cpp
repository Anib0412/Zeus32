/////////////////////////////////////////////////////////////////////////////
// RULER.CPP
//                                                                        
// ruler class for visual transform dialog for RasterPlus95
// Copyright (C) 1997-98 Graphx, Inc.
//
// 4-7-98   Version 2.0.3 - changed ruler font for WinJ to Arial
// 5-31-98  Version 2.0.4 - Rulers were not displaying small ints properly - 
//                             fixed
//

#include "stdafx.h"
#include "zeus.h"
#include "Ruler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRuler

CRuler::CRuler()
   {
   CDC dc;
   // default style
   style = RULER_STYLE_NONE;

   dc.Attach(::GetDC(NULL));

   // make a new font for drawing text
   LOGFONT lf;   
   memset(&lf, '\0', sizeof(LOGFONT));      
   lf.lfQuality = DEFAULT_QUALITY;
   lf.lfClipPrecision = CLIP_CHARACTER_PRECIS;

   lf.lfCharSet = ANSI_CHARSET;
   strcpy(lf.lfFaceName, "Arial");
   //lf.lfHeight = -MulDiv(8, dc.GetDeviceCaps(LOGPIXELSY), 72);      
   lf.lfHeight = -MulDiv(7, dc.GetDeviceCaps(LOGPIXELSY), 72);      

   lf.lfPitchAndFamily = FF_SWISS ;   
   // Create the normal font
   //lf.lfWeight = FW_NORMAL;   
   lf.lfWeight = 700;   
   pDlgFont = new CFont;
   pDlgFont->CreateFontIndirect(&lf);   

   ::ReleaseDC(NULL, dc.Detach());

   // defaults
   LoWater = 0;
   HiWater = 1100;
   UpTick = 100;
   TickDiv = 8;
   }

CRuler::~CRuler()
   {
   delete pDlgFont;
   }

void CRuler::SetStyle(int st)
   {
   style = st;
   }

void CRuler::SetRange(int lo, int hi, int td, int ut)
   {
   LoWater = lo;
   HiWater = hi;
   TickDiv = td;
   UpTick = ut;
   }

BEGIN_MESSAGE_MAP(CRuler, CWnd)
	//{{AFX_MSG_MAP(CRuler)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRuler message handlers

void CRuler::OnPaint() 
   {
   CPaintDC dc(this); 
   
   CFont* pOldFont = dc.SelectObject(pDlgFont);

   switch(style)
	  {
	  case RULER_STYLE_NONE:
      case RULER_STYLE_CORNER:
		   // do nothing
		   break; 
	  case RULER_STYLE_TOP:
		   DrawTicksTop(dc);
		   break;
	  case RULER_STYLE_LEFT:
		   DrawTicksLeft(dc);
		   break;
	  }			 
   // deselect font
   dc.SelectObject(pOldFont);   
   }

void CRuler::DrawTicksTop(CPaintDC& dc) 
   {
   int i;
   int j;												
   CString cs;
   CSize sz; 
   int xpos;
   int tpos;
   BOOL AtStart;
   
   penWindowText.DeleteObject();
   penWindowText.CreatePen(PS_SOLID, 0, GetSysColor(COLOR_WINDOWTEXT));
   dc.SetBkMode(TRANSPARENT);
   CPen* pOldPen = dc.SelectObject(&penWindowText);   

   CRect cr;										 
   GetClientRect(&cr);

   dc.SetTextAlign(TA_CENTER | TA_TOP);   

   for (i = LoWater, AtStart = TRUE; i < HiWater; i++)
	  {
	  if (i % UpTick) continue;
	  // if we are at the beginning, write backward ticks to make
	  // sure that we cover start
	  if (AtStart)
		 {
	     for (j = 0; j < TickDiv; j++)
		    {
		    tpos = (int)((float)cr.left + (float)(i * cr.Width()) / (float)(HiWater - LoWater) +
			   (float)(-j * cr.Width()) / ((float)TickDiv * (float)(HiWater - LoWater) / UpTick) + .5F);
 	        // draw the minor tick
  	        dc.MoveTo(tpos, MINOR_TICK_WIDTH);
	        dc.LineTo(tpos, RULER_HEIGHT - 1);
		    }
		 AtStart = FALSE;
		 }
	  // draw the minor ticks
	  for (j = 0; j < TickDiv; j++)
		 {
		 tpos = (int)((float)cr.left + (float)(i * cr.Width()) / (float)(HiWater - LoWater) +
			(float)(j * cr.Width()) / ((float)TickDiv * (float)(HiWater - LoWater) / UpTick) + .5F);
 	     // draw the minor tick
  	     dc.MoveTo(tpos, MINOR_TICK_WIDTH);
	     dc.LineTo(tpos, RULER_HEIGHT - 1);
		 }
	  // draw the major tick
  	  xpos = (int)((float)cr.left + (float)(i * cr.Width()) / (float)(HiWater - LoWater) + .5F);
	  // draw the major tick
  	  dc.MoveTo(xpos, MAJOR_TICK_WIDTH);
	  dc.LineTo(xpos, RULER_HEIGHT - 1);
	  // draw the number
	  //cs.Format("%d", i / 100);
	  cs.Format("%d", (int)((float)i / 100.F + .5F));
	  sz = dc.GetTextExtent(cs);
	  //dc.ExtTextOut(xpos, RULER_HEIGHT * 1 / 4 - sz.cy / 2, 0, NULL, 
	  //dc.ExtTextOut(xpos, RULER_HEIGHT * 3 / 4 - sz.cy / 2, 0, NULL, 
	  dc.ExtTextOut(xpos, RULER_HEIGHT * 1 / 2 - sz.cy / 2, 0, NULL, 
	     (const char*)cs, cs.GetLength(), NULL);	
	  }
   dc.SelectObject(pOldPen);
   }													

void CRuler::DrawTicksLeft(CPaintDC& dc) 
   {
   int i;
   int j;
   CString cs;
   CSize sz; 
   int ypos;
   int tpos;
   BOOL AtStart;
   
   penWindowText.DeleteObject();
   penWindowText.CreatePen(PS_SOLID, 0, GetSysColor(COLOR_WINDOWTEXT));
   dc.SetBkMode(TRANSPARENT);
   CPen* pOldPen = dc.SelectObject(&penWindowText);
   
   CRect cr;
   GetClientRect(&cr);

   dc.SetTextAlign(TA_CENTER | TA_TOP);

   for (i = LoWater, AtStart = TRUE; i < HiWater; i++)
	  {
	  if (i % UpTick) continue;
	  // if we are at the beginning, write backward ticks to make
	  // sure that we cover start
	  if (AtStart)
		 {
	     for (j = 0; j < TickDiv; j++)
		    {
		    tpos = (int)((float)cr.top + (float)(i * cr.Height()) / (float)(HiWater - LoWater) +
			   (float)(-j * cr.Height()) / ((float)TickDiv * (float)(HiWater - LoWater) / UpTick) + .5F);
 	        // draw the minor tick
  	        dc.MoveTo(MINOR_TICK_WIDTH, tpos);
	        dc.LineTo(RULER_HEIGHT - 1, tpos);
		    }
		 AtStart = FALSE;
		 }
	  // draw minor ticks
	  for (j = 0; j < TickDiv; j++)
		 {
		 tpos = (int)((float)cr.top + (float)(i * cr.Height()) / (float)(HiWater - LoWater) +
			(float)(j * cr.Height()) / ((float)TickDiv * (float)(HiWater - LoWater) / UpTick) + .5F);
 	     // draw the minor tick
  	     dc.MoveTo(MINOR_TICK_WIDTH, tpos);
	     dc.LineTo(RULER_HEIGHT - 1, tpos);
		 }
	  // draw the major tick
  	  ypos = (int)((float)cr.top + (float)(i * cr.Height()) / (float)(HiWater - LoWater) + .5F);
	  // draw the major tick
  	  dc.MoveTo(MAJOR_TICK_WIDTH, ypos);
	  dc.LineTo(RULER_HEIGHT - 1, ypos);
	  // draw the number
      //cs.Format("%d", i / 100);
	  cs.Format("%d", (int)((float)i / 100.F + .5F));
	  sz = dc.GetTextExtent(cs);
	  dc.ExtTextOut(MAJOR_TICK_WIDTH - sz.cx / 2, ypos - sz.cy / 2, 0, NULL, 
         (const char*)cs, cs.GetLength(), NULL);		
	  }
   dc.SelectObject(pOldPen);
   }
