/////////////////////////////////////////////////////////////////////////////
// CANVAS.CPP
//                                                                        
// visual transform preview window for RasterPlus
// Copyright (C) 1996-2000 Graphx, Inc.
//
// mods:
//
// 8-08-96  Build 41 - added Kanji def stuff
// 6-09-97  Build 50 - added PicaHeight to account for non square pixels
// 9-26-97  Build 51 - separate preview window
//                     draw rect tracker even if must repaint is displayed
//                     call SetLastRepaintRect from SetSizingRect
//                     don't allow wild zoom swings - force repaint
// 3-3-98   Version 2.0.2 - changed all floats from 2 to 3 decimal places
// 7-24-00  Version 4.0.0 - now zoom from the center  
// 8-04-00  Version 4.0.0 - now specify height and width of image
// 8-04-00  Version 4.0.0 - fixed lots of squirrely bugs

#include "stdafx.h"
#include "zeus.h"
#include "canvas.h" 
#include "visualxf.h"
#include "prev.h"
#include "profile.h"
#include "prefsmsg.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAXZOOMFACTOR   10.F  

/////////////////////////////////////////////////////////////////////////////
// CanvasWnd

CanvasWnd::CanvasWnd()
   {                       
   RECT Rect;   

   // get rect    
   Rect.left = 10;
   Rect.top = 10;
   Rect.right = 10;
   Rect.bottom = 10;            
   
   DoPaint = FALSE;   
   
   m_must_repaint = TRUE;
   
   pRT = new NewRectTracker(&Rect, 
	   CRectTracker::solidLine | CRectTracker::resizeInside);	                            	      	   
   }

CanvasWnd::~CanvasWnd()
   {           
   delete pRT;
   }

int CanvasWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// store client rect
	GetClientRect(&pRT->ClientRect);
	// set snap (a reference!)
	pRT->m_pSnap = &m_snap;            
	// set retain_aspect (a reference!)
	pRT->m_pRetain_aspect = &m_retain_aspect;
	pRT->PicaWidth = PicaWidth;
	pRT->PicaHeight = PicaHeight;
	
	return 0;
}

BEGIN_MESSAGE_MAP(CanvasWnd, CWnd)
	//{{AFX_MSG_MAP(CanvasWnd)
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CanvasWnd message handlers

void CanvasWnd::OnPaint()
   {
   CBrush brush;         
   CBrush* pOldBrush;   
   CRect rect;
   	  
   CPaintDC dc(this); // keeps windows happy    

   if (m_must_repaint)
      {
 	  // set rect
	  pRT->fTrackRect.ToCRect(rect);
	  
	  // normalize the coords
	  rect.NormalizeRect();
	  pOldBrush = NULL;   	
	  // draw a yellow box
	  brush.CreateSolidBrush(RGB(255, 255, 255));
	  pOldBrush = dc.SelectObject(&brush);
	  dc.FillRect(rect, &brush);          
	  // cleanup DC
	  if (pOldBrush != NULL)
	     dc.SelectObject(pOldBrush);
	  brush.DeleteObject();         

	  // make a new font
      LOGFONT lf;
	  CFont* pDlgFont;
      memset(&lf, '\0', sizeof(LOGFONT));      
      lf.lfQuality = DEFAULT_QUALITY;
      lf.lfClipPrecision = CLIP_LH_ANGLES | CLIP_STROKE_PRECIS;
#ifdef USEKANJI	   
  	  lf.lfCharSet = DEFAULT_CHARSET;
      strcpy(lf.lfFaceName, "System");
	  lf.lfHeight = -MulDiv(6, dc.GetDeviceCaps(LOGPIXELSY), 72);      	  
#else
  	  lf.lfCharSet = ANSI_CHARSET;
      strcpy(lf.lfFaceName, "Arial");
	  lf.lfHeight = -MulDiv(8, dc.GetDeviceCaps(LOGPIXELSY), 72);      
#endif

      lf.lfPitchAndFamily = FF_SWISS ;   
      // Create the normal font
      lf.lfWeight = FW_NORMAL;
      pDlgFont = new CFont;
      pDlgFont->CreateFontIndirect(&lf);   
	  CFont* pOldFont = dc.SelectObject(pDlgFont);

	  dc.SetTextColor(RGB(0, 0, 0));
      dc.SetBkColor(RGB(255, 255, 255));
	  
	  CString cs;
	  cs.LoadString(IDS_VISUALXF_REPAINT);
	  dc.TextOut(10, 10, cs);
	  
	  dc.SelectObject(pOldFont);
	  delete pDlgFont;

      // draw tracker on outside
      pRT->Draw(&dc);	

	  return;
	  }	  

   // display the rendered image if DoPaint is set   
   if (DoPaint)
      {            
      // we need to re-display the image without re-ripping by using our
      // bitmap engine to approximate the transform
      // we pass our display engine an xoff, yoff which is computed
      // using the difference in this and the last tracker rect with the
      // zoom factoring in to the computations
      // we also pass a zoom which is really the difference between this 
      // and the last zoom      	 
      DisplayRect.left = rnd.Convert(fDisplayRect.left);
      DisplayRect.right = rnd.Convert(fDisplayRect.right);
      DisplayRect.top = rnd.Convert(fDisplayRect.top);
      DisplayRect.bottom = rnd.Convert(fDisplayRect.bottom);
	  cd.DisplayImage(&DisplayRect, dc);  	   	  	  
//	  TRACE("CANVAS.CPP: DisplayRect, left = %d, right = %d, top = %d, bottom = %d\n", 
//		 DisplayRect.left, DisplayRect.right, DisplayRect.top, DisplayRect.bottom);
      }    
   // draw tracker on outside
   pRT->Draw(&dc);	
   }

BOOL CanvasWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
   {       
   if (pWnd == this && pRT->SetCursor(this, nHitTest))        
      return TRUE;
	
   return CWnd::OnSetCursor(pWnd, nHitTest, message);
   }

void CanvasWnd::OnLButtonDown(UINT nFlags, CPoint point)
   {      
   CRect rectSave, rectTrue;  
      
   // get last position
   pRT->GetTrueRect(rectSave);   

   if ((pRT->HitTest(point) >= 0) && pRT->Track(this, point))   
      {
      // invalidate last position   
      InvalidateRect(rectSave);	
	  // get new position	 	
      pRT->GetTrueRect(&rectTrue);  
      // invalidate new position   
      InvalidateRect(rectTrue);	  

	  fDisplayRect.left = 	
	    (float)rectTrue.left - 
	     (float)rectTrue.Width() / (float)rectSave.Width() * 
		 ((float)rectSave.left - fDisplayRect.left);
	  
  	  fDisplayRect.right = 	
	    (float)rectTrue.Width() / (float)rectSave.Width() * 
		 (fDisplayRect.right - (float)rectSave.right) + (float)rectTrue.right;
	  
	  fDisplayRect.top = 	
	    (float)rectTrue.top - 
	     (float)rectTrue.Height() / (float)rectSave.Height() * 
		 ((float)rectSave.top - fDisplayRect.top);
	  
  	  fDisplayRect.bottom = 	
	    (float)rectTrue.Height() / (float)rectSave.Height() * 
		 (fDisplayRect.bottom - (float)rectSave.bottom) + (float)rectTrue.bottom;	  
	  }   	
   	
   CWnd::OnLButtonDown(nFlags, point);
   }

void CanvasWnd::SetSizingRect(CRect* pCr, CRect* BoundRect, CRect* zr, POINT* ptR)
   {   
   pCr->NormalizeRect();
   pRT->m_rect = *pCr;   
   pRT->fTrackRect = pRT->m_rect;

   // store rects
   pRT->m_rectOriginal = pRT->m_rect;
   MaxRect = *BoundRect;
   ZoomRect = *zr;
      
   // store original zooms
   pRT->m_xOriginalZoom = pRT->m_xzoom;         
   pRT->m_yOriginalZoom = pRT->m_yzoom;                  
   
   // and widths
   pRT->m_OriginalWidth = pRT->m_width;
   pRT->m_OriginalHeight = pRT->m_height;
   
   pRT->ptRotateCorrect = *ptR;
   pRT->OriginalCorrect = pRT->ptRotateCorrect;

   SetLastRepaintRect();
   }        
   
void CanvasWnd::UpdateRectFromEdit(int idx, float fval)
   {                     
   float fDeltaZoom;   
   float fMidPoint;   
   float fMidPointOriginal;      

   switch(idx)
      {
      case TRANVAR_XZOOM:         	  
		 // get new track rect
		   pRT->fTrackRect.left =		   
              pRT->fTrackRect.left + 
              ((float)(ZoomRect.right - ZoomRect.left) * pRT->m_xzoom - 
			  (float)(ZoomRect.right - ZoomRect.left) * fval) / 2.;
		   pRT->fTrackRect.right =		   
              pRT->fTrackRect.right -               
              ((float)(ZoomRect.right - ZoomRect.left) * pRT->m_xzoom - 
			  (float)(ZoomRect.right - ZoomRect.left) * fval) / 2.;
		   
		   // store new zoom
		   pRT->m_xzoom = fval;     		   

		   // must update zoom as xoff might have changed due to rotation
		   pRT->UpdateEditVars(EXCLUDE_ANYZOOM);		   

		   // compute the change in zoom
		   fDeltaZoom = pRT->m_xzoom / LastXzoom;

		   // compute the mid points
		   fMidPoint = pRT->fTrackRect.left + 
			  (pRT->fTrackRect.right - pRT->fTrackRect.left) / 2.;		   		   
		   fMidPointOriginal = (LastRight - LastLeft) / 2 + LastLeft;

		   // compute new display rect
		   fDisplayRect.left = -fDeltaZoom * fMidPointOriginal + fMidPoint + fDeltaZoom * MaxRect.left;
		   fDisplayRect.right = fDeltaZoom * (MaxRect.right - fMidPointOriginal) + fMidPoint;

		   // don't allow wild zooming - force a repaint
		   if (fDisplayRect.Width() > (MaxRect.Width() * MAXZOOMFACTOR) ||
			   (fDisplayRect.Width() * MAXZOOMFACTOR) < MaxRect.Width())
			  m_must_repaint = TRUE;
           break;   
      case TRANVAR_YZOOM:
		   pRT->fTrackRect.top =
			  pRT->fTrackRect.top +
			  ((float)(ZoomRect.bottom - ZoomRect.top) * pRT->m_yzoom - 
			  (float)(ZoomRect.bottom - ZoomRect.top) * fval) / 2.;			  

		   pRT->fTrackRect.bottom =
			  pRT->fTrackRect.bottom -
			  ((float)(ZoomRect.bottom - ZoomRect.top) * pRT->m_yzoom - 
			  (float)(ZoomRect.bottom - ZoomRect.top) * fval) / 2.;			  

		   // store new zoom		   
		   pRT->m_yzoom = fval;           		   

		   // must update zoom as yoff might have changed due to rotation
		   pRT->UpdateEditVars(EXCLUDE_ANYZOOM);

		   // compute the change in zoom
		   fDeltaZoom = pRT->m_yzoom / LastYzoom;

		   // compute the mid points
		   fMidPoint = pRT->fTrackRect.top + 
			  (pRT->fTrackRect.bottom - pRT->fTrackRect.top) / 2.;		   		   
		   fMidPointOriginal = (LastBottom - LastTop) / 2 + LastTop;

		   // compute new display rect
		   fDisplayRect.top = -fDeltaZoom * fMidPointOriginal + fMidPoint + fDeltaZoom * MaxRect.top;
		   fDisplayRect.bottom = fDeltaZoom * (MaxRect.bottom - fMidPointOriginal) + fMidPoint;		   

		   // don't allow wild zooming - force a repaint
		   if (fDisplayRect.Height() > (MaxRect.Height() * MAXZOOMFACTOR) ||
			   (fDisplayRect.Height() * MAXZOOMFACTOR) < MaxRect.Height())
			  m_must_repaint = TRUE;
           break;  
      case TRANVAR_XOFF:
		   // compute new display rect
		   fDisplayRect.left += (fval - pRT->m_xoff) * (float)MaxRect.Width() / PicaWidth;
           fDisplayRect.right += (fval - pRT->m_xoff) * (float)MaxRect.Width() / PicaWidth;		   

		   // compute new track rect
		   pRT->fTrackRect.left += (fval - pRT->m_xoff) * (float)MaxRect.Width() / PicaWidth;
           pRT->fTrackRect.right += (fval - pRT->m_xoff) * (float)MaxRect.Width() / PicaWidth;		   

		   // store new xoff
		   pRT->m_xoff = fval;
           break;      
      case TRANVAR_YOFF:
		   // compute new display rect
		   fDisplayRect.top -= (fval - pRT->m_yoff) * (float)MaxRect.Height() / PicaHeight;
           fDisplayRect.bottom -= (fval - pRT->m_yoff) * (float)MaxRect.Height() / PicaHeight;		   

		   // compute new track rect
		   pRT->fTrackRect.top -= (fval - pRT->m_yoff) * (float)MaxRect.Height() / PicaHeight;
           pRT->fTrackRect.bottom -= (fval - pRT->m_yoff) * (float)MaxRect.Height() / PicaHeight;		   

		   // store new yoff
		   pRT->m_yoff = fval;
           break;   
      case TRANVAR_ROTATION:
		   // this changes everything - repaint
           m_must_repaint = TRUE;
           break;
      case TRANVAR_START:
		   // this changes everything - repaint
           m_must_repaint = TRUE;
           break;
      case TRANVAR_WIDTH:
		   // force a change in zoom		   
		   UpdateRectFromEdit(TRANVAR_XZOOM, fval / pRT->m_OriginalWidth * pRT->m_xOriginalZoom);
		   if (m_retain_aspect)
			  UpdateRectFromEdit(TRANVAR_YZOOM, fval / pRT->m_OriginalWidth * pRT->m_xOriginalZoom);
		   // and update all the vars
		   pRT->UpdateEditVars(EXCLUDE_NONE);		   
           break;
      case TRANVAR_HEIGHT:
		   // force a change in zoom		   
		   UpdateRectFromEdit(TRANVAR_YZOOM, fval / pRT->m_OriginalHeight * pRT->m_yOriginalZoom);
		   if (m_retain_aspect)
			  UpdateRectFromEdit(TRANVAR_XZOOM, fval / pRT->m_OriginalHeight * pRT->m_yOriginalZoom);
		   // and update all the vars
		   pRT->UpdateEditVars(EXCLUDE_NONE);
           break;
	  }
   pRT->fTrackRect.ToCRect(pRT->m_rect);

   Invalidate();      
   UpdateWindow();
   }
    
NewRectTracker::NewRectTracker()
   {
   }	
	
NewRectTracker::NewRectTracker(LPCRECT lpSrcRect, UINT nStyle)
   :CRectTracker(lpSrcRect, nStyle)
   {                                      	
   }	
	
NewRectTracker::~NewRectTracker()
   {
   }  
   
#define SNAP_LEN   10       
void NewRectTracker::AdjustRect(int nHandle, LPRECT lpRect)
   {   
   int dim;              
   
   // retain aspect   
   if (*m_pRetain_aspect)
      {      
      if (nHandle == hitTopLeft || nHandle == hitBottomLeft)
         {
         dim = MulDiv(lpRect->bottom - lpRect->top, m_rectOriginal.Width(),
                      m_rectOriginal.Height());
         lpRect->left = lpRect->right - dim;
         UpdateEditVars();            
         }
      else if (nHandle == hitTopRight || nHandle == hitBottomRight)   
         {
         dim = MulDiv(lpRect->bottom - lpRect->top, m_rectOriginal.Width(),
                      m_rectOriginal.Height());
         lpRect->right = lpRect->left + dim;
         UpdateEditVars();            
         }      
      }            
   // snap   
   if (*m_pSnap)
      {
      // Snap it, note that if our handle is hitMiddle, we are moving 
      // so we should retain rect width/height
      if (abs(lpRect->top - ClientRect.top) < SNAP_LEN)
         {                                              
         if ((nHandle == hitTopLeft) || (nHandle == hitTop) || 
            (nHandle == hitTopRight) || (nHandle == hitMiddle))
            {
            dim = lpRect->bottom - lpRect->top;
            lpRect->top = ClientRect.top;
            if (nHandle == hitMiddle)
               lpRect->bottom = lpRect->top + dim;
            UpdateEditVars();
            }
         }
      if (abs(lpRect->bottom - ClientRect.bottom) < SNAP_LEN) 
         {                                 
         if ((nHandle == hitBottomLeft) || (nHandle == hitBottom) || 
            (nHandle == hitBottomRight) || (nHandle == hitMiddle))
            {
            dim = lpRect->bottom - lpRect->top;
            lpRect->bottom = ClientRect.bottom;
            if (nHandle == hitMiddle)
               lpRect->top = lpRect->bottom - dim;
            UpdateEditVars();         
            }
         }   
      if (abs(lpRect->left - ClientRect.left) < SNAP_LEN) 
         {
         if ((nHandle == hitTopLeft) || (nHandle == hitLeft) || 
            (nHandle == hitBottomLeft) || (nHandle == hitMiddle))
            {
            dim = lpRect->right - lpRect->left;
            lpRect->left = ClientRect.left;
            if (nHandle == hitMiddle)
               lpRect->right = lpRect->left + dim;
            UpdateEditVars();         
            }
         }
      if (abs(lpRect->right - ClientRect.right) < SNAP_LEN) 
         {   
         if ((nHandle == hitTopRight) || (nHandle == hitRight) || 
            (nHandle == hitBottomRight) || (nHandle == hitMiddle))
            {
            dim = lpRect->right - lpRect->left;
            lpRect->right = ClientRect.right; 
            if (nHandle == hitMiddle)         
               lpRect->left = lpRect->right - dim;         
            UpdateEditVars();         
            }
         }       
      }   
   }

void NewRectTracker::OnChangedRect(const CRect& rectOld)
   {       
   fTrackRect = m_rect;   

   UpdateEditVars();
   }     
      
void NewRectTracker::UpdateEditVars(int exclude)
   {            
   char buf[20];      

   ((CJobSizeProps*)pWnd)->m_should_repaint = TRUE;

   if (m_rectOriginal.Width())
      {  
      if (exclude != EXCLUDE_WIDTH)
		 {
         m_width = m_OriginalWidth * fTrackRect.Width() / 
            (float)m_rectOriginal.Width();            
		 ((CJobSizeProps*)pWnd)->m_width = ((CJobSizeProps*)pWnd)->PicaNumToDisplay(m_width);
		 }
	  if (exclude != EXCLUDE_ANYZOOM)
	     {
         m_xzoom = m_xOriginalZoom * fTrackRect.Width() / 
            (float)m_rectOriginal.Width();            
         sprintf(buf, "%.3f", m_xzoom);                     
         ((CJobSizeProps*)pWnd)->m_xzoom = buf;	  
		 }
	  ptRotateCorrect.x = (int)((float)OriginalCorrect.x * m_xzoom / m_xOriginalZoom + .5F);

//	  TRACE("SET ptRotateCorrect.x = %d\n", ptRotateCorrect.x);
	  }
   if (m_rectOriginal.Height())
      {	  
      if (exclude != EXCLUDE_HEIGHT)
		 {
         m_height = m_OriginalHeight * fTrackRect.Height() / 
            (float)m_rectOriginal.Height();            
		 ((CJobSizeProps*)pWnd)->m_height = ((CJobSizeProps*)pWnd)->PicaNumToDisplay(m_height);
		 }
	  if (exclude != EXCLUDE_ANYZOOM)
	     {
         m_yzoom = m_yOriginalZoom * fTrackRect.Height() / 
            (float)m_rectOriginal.Height();            
         sprintf(buf, "%.3f", m_yzoom);                     
         ((CJobSizeProps*)pWnd)->m_yzoom = buf;
		 }
	  ptRotateCorrect.y = (int)((float)OriginalCorrect.y * m_yzoom / m_yOriginalZoom + .5F);
//	  TRACE("SET ptRotateCorrect.y = %d\n", ptRotateCorrect.y);
      }
   // get new sizing vars relative to original rect
   // convert to strings and update the display   
   if (BoundingRect.Width()) 
      {      
	  if (exclude != EXCLUDE_XOFF)
	     {
		 m_xoff = (fTrackRect.left - (float)ptRotateCorrect.x) * PicaWidth / 
            (float)BoundingRect.Width();
         ((CJobSizeProps*)pWnd)->m_xoff = ((CJobSizeProps*)pWnd)->PicaNumToDisplay(m_xoff);         
		 }
      if (exclude != EXCLUDE_YOFF)
	     {
	     m_yoff = ((float)BoundingRect.Height() - fTrackRect.bottom + 
		    (float)ptRotateCorrect.y) * PicaHeight /          	  
            (float)BoundingRect.Height();         
         ((CJobSizeProps*)pWnd)->m_yoff = ((CJobSizeProps*)pWnd)->PicaNumToDisplay(m_yoff);
		 }
      }   
   ((CJobSizeProps*)pWnd)->UpdateData(FALSE);
   ((CJobSizeProps*)pWnd)->ReportChange(XFORMCHANGE_ALL);
   }
   
int CanvasWnd::OnPrevSetInfo(WPARAM wParam, LPARAM lParam)            
   {   
   PPREVIEWINFO pPreviewInfo;
   
   // get the pointer  
   pPreviewInfo = (PPREVIEWINFO)lParam;     

   // create a new display image
   if (cd.CreateImage(pPreviewInfo))
      {
      SetLastRepaintRect();
      DoPaint = TRUE;                     
      }
   // force a repaint   
   InvalidateRect(NULL);

   // force a cancel after first image
   return SYN_ERRMSG_CANCELIMAGE;
   }         
   
void CanvasWnd::SetLastRepaintRect()
   {
   // is equal to the entire canvas because we are re-ripping
   DisplayRect = MaxRect;

   // create a floating point version of the display rect
   DisplayRect.NormalizeRect();
   fDisplayRect.left = (float)DisplayRect.left;
   fDisplayRect.right = (float)DisplayRect.right;
   fDisplayRect.top = (float)DisplayRect.top;
   fDisplayRect.bottom = (float)DisplayRect.bottom;   

   // store these variables for later computation
   LastXzoom = pRT->m_xzoom;
   LastYzoom = pRT->m_yzoom;   					    
   LastLeft = pRT->fTrackRect.left;
   LastRight = pRT->fTrackRect.right;
   LastTop = pRT->fTrackRect.top;
   LastBottom = pRT->fTrackRect.bottom;
   }

/////////////////////////////////////////////////////////////////////////////
// CanvasParentWnd message handlers
/////////////////////////////////////////////////////////////////////////////
// CanvasParentWnd

CanvasParentWnd::CanvasParentWnd()
{
}

CanvasParentWnd::~CanvasParentWnd()
{
}

BEGIN_MESSAGE_MAP(CanvasParentWnd, CWnd)
	//{{AFX_MSG_MAP(CanvasParentWnd)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CanvasParentWnd message handlers

void CanvasParentWnd::OnClose() 
   {
   Exists = FALSE;
   
   CWnd::OnClose();
   }

void CanvasParentWnd::OnDestroy() 
   {
   CProfile Profile;
   RECT Rect;
   GetWindowRect(&Rect);

   // write the last position of the window
   Profile.WriteInt(IDS_PREVIEW, IDS_LASTWINDOWPOSY, Rect.top);
   Profile.WriteInt(IDS_PREVIEW, IDS_LASTWINDOWPOSX, Rect.left);

   CWnd::OnDestroy();
   }