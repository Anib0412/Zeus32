/////////////////////////////////////////////////////////////////////////////
// RPTreeBase.cpp
//                                                                        
// lower-level base tree display/op class for RasterPlus
// Copyright (C) 1999 Graphx, Inc.
//

#include "stdafx.h"
#include "zeus.h"
#include "RPTreeBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int cxVertScroll = ::GetSystemMetrics(SM_CXVSCROLL);
static const int cyHorzScroll = ::GetSystemMetrics(SM_CYHSCROLL);

/////////////////////////////////////////////////////////////////////////////
// CRPTreeBase

IMPLEMENT_DYNCREATE(CRPTreeBase, SECTreeView)

CRPTreeBase::CRPTreeBase()
   {
   }

CRPTreeBase::~CRPTreeBase()
   {
   }

BEGIN_MESSAGE_MAP(CRPTreeBase, SECTreeView)
	//{{AFX_MSG_MAP(CRPTreeBase)
    ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRPTreeBase message handlers

void CRPTreeBase::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   CMenu menu;                                                             
   CMenu *submenu;      
   POINT point;
   DWORD dwPos;

   dwPos = GetMessagePos();
 
   point.x = LOWORD (dwPos);
   point.y = HIWORD (dwPos);
 
   // load the menu  
   menu.LoadMenu(IDR_ZEUSTYPE);

   // get the edit popup menu                                                   
   submenu = menu.GetSubMenu(1);                                           

   // post the menu                                                        
   submenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,                  
                           point.x,                                        
                           point.y,                                        
                           AfxGetApp()->m_pMainWnd, NULL);                  
	
   *pResult = 0;
   }


#define VACINITY_FACTOR 4
int CRPTreeBase::HitTest(CPoint pt, UINT* pFlags, int& nHitCol) const
{
	Item* pItem = 0;
	Column* pCol = 0;
	int xColLeft, xColRight;
	CRect rcItem;
	RECT rcInside, rcClient;
	int nTopIndex;
	int nHitItem;
	int iSubItem;
	int i;

	*pFlags = 0;
	nHitItem = -1;
	nHitCol = -1;

	SECGetClientRect(&rcClient);

    // check for a hit in the special header.  If hit, return
    // and do nothing
    if (pt.y < (rcClient.top + HeaderOffset))
	   {
	   *pFlags = LVHT_ABOVE;
       return -1;
	   }

	GetInsideRect(&rcInside);	

	// Hit outside of this window?
	if (pt.y < rcClient.top)
		*pFlags |= LVHT_ABOVE;
	if (pt.y > rcClient.bottom)
		*pFlags |= LVHT_BELOW;
	if (pt.x < rcClient.left)
		*pFlags |= LVHT_TOLEFT;
	if (pt.x > rcClient.right)
		*pFlags |= LVHT_TORIGHT;

	if (*pFlags != 0)
		return -1;

	nTopIndex = GetTopIndex();

	// At this point, the hit was definitely inside the window.
	// So, which column was hit?
	xColLeft = -m_ptViewportOrg.x;
	xColRight = xColLeft;

	if( GetColumnCount() != 0 )
	{
		if ( GetColumnCount() > 1)
		{
			for (i=0; i<GetColumnCount(); i++)
			{
				pCol = GetColumnAt(i);
				xColRight += GetColumnWidth(i);
				if (pt.x >= xColLeft && pt.x < xColRight)
				{
					nHitCol = i;
					break;
				}
				else
					xColLeft = xColLeft;
			}
		}
		else
		{
			pCol = GetColumnAt(0);
			nHitCol = 0;
		}
	}


	// Did the user click on a column header?
	if (pt.y >= rcClient.top && pt.y <= rcInside.top)
	{

		CRect rcCol;
		int nColCount;

		rcCol.top = rcClient.top;
		rcCol.left = -m_ptViewportOrg.x;
		rcCol.bottom = rcInside.top;
		rcCol.right = rcCol.left;

		nColCount = GetColumnCount();
		nColCount -= (GetListCtrlStyleEx()&LVXS_FITCOLUMNSONSIZE) ? 1 : 0;

		for (i=0; i<=nColCount; i++)
		{
			pCol = GetColumnAt(i);
			rcCol.right += GetColumnWidth(i);
			if (GetColumnCount() >= 1
				&& pt.x >= (rcCol.right - VACINITY_FACTOR)
				&& pt.x <= (rcCol.right + VACINITY_FACTOR))
			{
				*pFlags = LVHT_COLUMNSEP;
				nHitItem = i;
				break;
			}
			else if (rcCol.PtInRect(pt))
			{
				*pFlags = LVHT_COLUMNHEADING;
				nHitItem = i;
				break;
			}
			else
				rcCol.left = rcCol.right;
		}
	}
	else  // An item was definitely hit..  So which one?
	{

		rcItem.top = rcInside.top;
		rcItem.left = -m_ptViewportOrg.x;
		rcItem.right = rcInside.right;
		rcItem.bottom = rcItem.top;

		for (i=nTopIndex; rcItem.top < rcInside.bottom && i<GetItemCount(); i++)
		{
			pItem = GetListCtrl().GetItemAt(i);
			if (pItem->m_cy == LVXC_UNINITIALIZED)
				//const_cast<SEC_LISTCLASS*>(this)->MeasureItem(i);
				const_cast<CRPTreeBase*>(this)->MeasureItem(i);				

			rcItem.top += GetMargin();
			rcItem.bottom += pItem->m_cy + GetMargin();


			if (rcItem.PtInRect(pt))
			{
				nHitItem = i;
				iSubItem = pCol->iSubItem;
				*pFlags |= LVHT_INSIDEITEMBOUNDS;
				if (iSubItem == 0)
				{
					CRect rcText, rcIcon, rcStateIcon;

					// Text rect
					rcText.top = rcItem.top;
					rcText.bottom = rcItem.bottom;
					rcText.left = xColLeft + pItem->rcText.left;
					rcText.right = rcText.left + pItem->rcText.Width();

					// State icon rect
					rcStateIcon.top = rcItem.top + pItem->rcStateIcon.top;
					rcStateIcon.bottom = rcStateIcon.top + pItem->rcStateIcon.Height();
					rcStateIcon.left = xColLeft + pItem->rcStateIcon.left;
					rcStateIcon.right = rcStateIcon.left + pItem->rcStateIcon.Width();

					// Icon rect
					rcIcon.top = rcItem.top + pItem->rcIcon.top;
					rcIcon.bottom = rcIcon.top + pItem->rcIcon.Height();
					rcIcon.left = xColLeft + pItem->rcIcon.left;
					rcIcon.right = rcIcon.left + pItem->rcIcon.Width();

					if (rcText.PtInRect(pt))
						*pFlags |= LVHT_ONITEMLABEL;
					else if (rcIcon.PtInRect(pt))
						*pFlags |= LVHT_ONITEMICON;
					else if (rcStateIcon.PtInRect(pt))
						*pFlags |= LVHT_ONITEMSTATEICON;
					else if (pt.x < rcText.left)
						*pFlags |= LVHT_ONITEMLEFT;	
					else if (pt.x >= rcText.right)
						*pFlags |= LVHT_ONITEMRIGHT;
				}
				else
				   {
				   // this allows us to select anywhere and drag and drop
                   if (rcItem.PtInRect(pt))
				      *pFlags |= LVHT_ONITEMLABEL;

				   }
				break;
			}
			else
				rcItem.top = rcItem.bottom;
		}
	}

	if (*pFlags == 0)
		*pFlags = LVHT_NOWHERE;

	return nHitItem;
}

static COLORREF rgbBtnFace = ::GetSysColor(COLOR_BTNFACE);
static COLORREF rgbActiveHeader = ::GetSysColor(COLOR_ACTIVECAPTION);
static COLORREF rgbInactiveHeader = ::GetSysColor(COLOR_INACTIVECAPTION);

void CRPTreeBase::DrawHeader(LvPaintContext* pPC)
{
	Column* pCol;
	CRect rcHeader, rcClient;
	CSize szLog;
	int i;		
	BOOL GotFocus;			 

	// do we have the focus?
	int row;
	((CSplitterWnd*)GetParent())->GetActivePane(&row);	
	if (row == iViewStyle) 
	   GotFocus = TRUE;
	else
	   GotFocus = FALSE;	 

	//TRACE("DrawHeader, iViewStyle = %d, ActivePane = %d\n", iViewStyle, row);

	// If there's no column header, this function shouldn't be called.
	ASSERT(IsHeaderCtrlEnabled() == TRUE);

	SECGetClientRect(rcClient);
	szLog = GetLogicalSize(pPC);

	// Compute Header Rect
	//rcHeader.top = 0;
	rcHeader.top = HeaderOffset;
	rcHeader.bottom = m_cyHeader;
	rcHeader.left = -m_ptViewportOrg.x;
	rcHeader.right = otmax(szLog.cx, rcClient.Width()+1);

	CRect rcHeaderAll = rcHeader;
	rcHeaderAll.top = 0;

	if (!pPC->pdc->RectVisible(rcHeaderAll))
		return;  // Early return if header isn't in need of repaint

	static CBrush br(rgbBtnFace);	
	CBrush* pOldBrush = pPC->pdc->SelectObject(&br);
	int oldMode = pPC->pdc->SetBkMode(TRANSPARENT);	

	// new    
	  // make a new font
      LOGFONT lf;
	  CFont* pDlgFont;
      memset(&lf, '\0', sizeof(LOGFONT));      
      lf.lfQuality = DEFAULT_QUALITY;
      lf.lfClipPrecision = CLIP_LH_ANGLES | CLIP_STROKE_PRECIS;
#ifdef USEKANJI	   
  	  lf.lfCharSet = DEFAULT_CHARSET;
      strcpy(lf.lfFaceName, "‚l‚r –¾’©");
	  lf.lfHeight = -MulDiv(10, pPC->pdc->GetDeviceCaps(LOGPIXELSY), 72);      	  
#else
  	  lf.lfCharSet = ANSI_CHARSET;
      strcpy(lf.lfFaceName, "Arial");
	  lf.lfHeight = -MulDiv(12, pPC->pdc->GetDeviceCaps(LOGPIXELSY), 72);      
#endif

      lf.lfPitchAndFamily = FF_SWISS ;   
      // Create the normal font
      //lf.lfWeight = FW_NORMAL;
	  lf.lfWeight = FW_BOLD;
      pDlgFont = new CFont;
      pDlgFont->CreateFontIndirect(&lf);   
	  CFont* pOldFont = pPC->pdc->SelectObject(pDlgFont);

	// DRAW TITLE BAR - hack by pjt
	CRect crTopOfHeader;
	crTopOfHeader = rcHeader;
	crTopOfHeader.top = 0;	
	crTopOfHeader.bottom = HeaderOffset;

	// if scroll is on, draw it further	
	if( m_dwScrollStyle & WS_VSCROLL )
	   crTopOfHeader.right += cxVertScroll; 

	if (GotFocus)
		pPC->pdc->FillSolidRect(&crTopOfHeader, GetSysColor(COLOR_ACTIVECAPTION));	    
	else
	   pPC->pdc->FillSolidRect(&crTopOfHeader, GetSysColor(COLOR_INACTIVECAPTION));	   

	if (GotFocus)
	   pPC->pdc->SetTextColor(GetSysColor(COLOR_CAPTIONTEXT));
	else
	   pPC->pdc->SetTextColor(GetSysColor(COLOR_INACTIVECAPTIONTEXT));

	// draw title text with count	
	CString csTemp;
	int nJobs = GetCount();
	if (nJobs == 0)
	   csTemp.Format("%s", csTreeTitle);
	else if (nJobs == 1)
	   csTemp.Format("%s - 1 Job", csTreeTitle);
	else
	   csTemp.Format("%s - %d Jobs", csTreeTitle, nJobs);
    pPC->pdc->TextOut(2, 0, csTemp);

	pPC->pdc->SetTextColor(RGB(0,0,0));

	// move here from above	
	pOldFont = pPC->pdc->SelectObject(GetHeaderFont());	 
	
	// Draw the Header Background
	pPC->pdc->FillRect(rcHeader, &br);

	// Draw the columns
	if (GetColumnCount() >= 1)
	{
		int nColCount;
		nColCount = GetColumnCount();
		for (i=0; i<nColCount; i++)
		{
			pCol = GetColumnAt(i);
			rcHeader.right = rcHeader.left + GetColumnWidth(i);
			// Drawing a frame control with zero width sometimes corrupts image, guard
			if(rcHeader.right>rcHeader.left)
			   {
			   if (i == DrawHeaderPushCol)			   
  			      pPC->pdc->DrawFrameControl(&rcHeader, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_PUSHED);
			   else
			      pPC->pdc->DrawFrameControl(&rcHeader, DFC_BUTTON, DFCS_BUTTONPUSH);
			   }
			rcHeader.left = rcHeader.right;
		}

		//fill in whats left over...
		//   make sure it always hangs over...just like the Win32 Header
		if( rcHeader.right < rcClient.right )
			rcHeader.right = rcClient.right + 2;
		else
			rcHeader.right += 2;

		pPC->pdc->DrawFrameControl(&rcHeader, DFC_BUTTON, DFCS_BUTTONPUSH );
	}

	UINT fAdditionalFlags(0);
	rcHeader.left = -m_ptViewportOrg.x;

	//use another rectangle because we might have to offset text on the 
	//		left, right or both, and we don't want to corrupt rcHeader...
	CRect rectText(0,0,0,0);

	// Draw the column header text
	for (i=0; i<GetColumnCount(); i++)
	{
		pCol = GetColumnAt(i);
		rcHeader.right = rcHeader.left + GetColumnWidth(i) - 1;

		rectText = rcHeader;
		rectText.left  += SEC_LEFT_OFFSET;
		rectText.right -= SEC_RIGHT_OFFSET;

		switch (pCol->fmt)
		{
		case LVCFMT_LEFT:
			fAdditionalFlags = DT_LEFT;
			break;
		case LVCFMT_RIGHT:
			fAdditionalFlags = DT_RIGHT;
			break;
		case LVCFMT_CENTER:
			fAdditionalFlags = DT_CENTER;
			break;
		default:
			// Invalid Format
			ASSERT(FALSE);
		}

		CSize size = pPC->pdc->GetTextExtent( pCol->strText );
		if( size.cx > rectText.Width() )
			fAdditionalFlags |= DT_END_ELLIPSIS;

		// show "pushed" text
		if (i == DrawHeaderPushCol)
		   {
		   rectText.top += 1;
		   rectText.left += 1;
		   rectText.right += 1;
		   rectText.bottom += 1;
		   }

		pPC->pdc->DrawText( pCol->strText, 
					   -1,
					   rectText,
					   DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX|DT_EXPANDTABS|fAdditionalFlags);

		rcHeader.left = rcHeader.right + 1;
	}


	pPC->pdc->SetBkMode(oldMode);
	pPC->pdc->SelectObject(pOldBrush);
	pPC->pdc->SelectObject(pOldFont);

	// new from pete
	delete pDlgFont;
}

void CRPTreeBase::DoPaint(CDC* pDC)
{
	LvPaintContext* pPC;
	CRect rectInside(0,0,0,0),	//Inside Rectangle  = "client rect" - "width of vertical scroll" - "height of horizontal scroll" - "height of header"
		  rectClient(0,0,0,0),  //Complete client area
		  rectFill(0,0,0,0),    //Area to fill with background color.
		  rectErase(0,0,0,0);   //small corner intersection of vertical and horizontal scroll bar ( lower right hand corner )

	GetInsideRect(rectInside);
	SECGetClientRect(rectClient);
	
	GetParent()->UpdateWindow();

	if ( pDC->IsPrinting() )
	{
		// Initialize Paint Context
		pPC = CreatePaintContext(pDC);
		pPC->da = DrawEntire;
		pPC->pdc = pDC;
		pPC->dwViewStyle = GetListCtrlStyle() & LVS_TYPEMASK;


		//Draw the header control...
		if( (GetListCtrlStyle() & LVS_TYPEMASK) == LVS_REPORT 
			&& IsHeaderCtrlEnabled())
		{			    
			DrawHeader( pPC );
		}


		DrawInvalidItems(pPC);

	}
	else	// not printing
	{
		CDC* pTempDC = NULL; // scratch pad dc
		
		BOOL bDelete = TRUE;
		BOOL bHScroll = !!(m_dwScrollStyle & WS_HSCROLL);
		BOOL bVScroll = !!(m_dwScrollStyle & WS_VSCROLL);

		//We only fill in the item area...normally we would want to fill the header as well,
		// but since the header can extend off the screen we don't want to confine it to our normal clipping rect...
		rectFill = rectInside;
		rectFill.top = rectClient.top;

		//If I don't have a Horizontal scroll bar, I want my rectFill to extend all
		// the way to the bottom
		if( !bHScroll )
		{
			rectFill.bottom = rectClient.bottom;
		}

		//If I don't have a Vertical scroll bar, I want my rectFill to extend all
		// the way to the right
		if( !bVScroll )
		{
			rectFill.right = rectClient.right;
		}		

		// If I have both a hscroll and vscroll, setup the rectangular area 
		//  where the scrollbars meet.
		if( bHScroll && bVScroll )
		{
			rectErase.left = rectInside.right + 1;
			rectErase.right = rectClient.right;
			rectErase.top = rectInside.bottom + 1;
			rectErase.bottom = rectClient.bottom;
		}

		//Create the memdc SEC_LISTBASEd on the fill rectangle.  That way there will be no flicker behind
		//  the scroll bars when they redraw ( hopefully :-)
		bDelete = CreateMemDC( pDC, pTempDC, rectFill );

		pTempDC->FillSolidRect( rectFill, GetBkColor() );

		// Initialize Paint Context
		pPC = CreatePaintContext(pTempDC);
		pPC->da = DrawEntire;
		pPC->pdc = pTempDC;
		pPC->dwViewStyle = GetListCtrlStyle() & LVS_TYPEMASK;

		//Draw the header control...
		if( (GetListCtrlStyle() & LVS_TYPEMASK) == LVS_REPORT 
			&& IsHeaderCtrlEnabled())
		{			    
			DrawHeader( pPC );		    
		}

		// We have to do this so drawn text doesn't overwrite the
		// vertical scrollbar which makes it flicker.
		pTempDC->IntersectClipRect(rectInside);

		// And finally, draw the contents of the list control
		DrawInvalidItems(pPC);

		if( bDelete ) 
			delete pTempDC;

		//All non-memdc drawing goes here...

		// Erase the rectangular area where the horizontal and vertical
		// scrollbars meet.  Must Erase with the screen dc and not mem dc because mem dc is created
		// SEC_LISTBASEd on rectFill which may not include this area.
		if ( bHScroll && bVScroll )
		{
			pDC->FillSolidRect(rectErase, rgbBtnFace);
		}

	// do we have the focus?
	BOOL GotFocus;				
	int row;
	((CSplitterWnd*)GetParent())->GetActivePane(&row);	
	if (row == iViewStyle) 
	   GotFocus = TRUE;
	else
	   GotFocus = FALSE;

	// now, we must draw the "end" of the special header
	if (bVScroll)
	   {
	   CRect rectEnd(0,0,0,0);   
	   rectEnd.left = rectInside.right;	   
	   rectEnd.right = rectClient.right;
	   rectEnd.top = rectClient.top;	   
	   rectEnd.bottom = rectInside.top - SECLISTCTRL_HDRHEIGHT_CLISTCTRL;

	   pDC->FillSolidRect(rectEnd, 
		  GetSysColor(GotFocus ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION));
	   }
	}
	// Delete the paint context
	delete pPC;
}

void CRPTreeBase::RecalcLayout()
{
	CScrollBar* pHorzSB = GetScrollBarCtrl(SB_HORZ);
	CScrollBar* pVertSB = GetScrollBarCtrl(SB_VERT);

	// give the hint for the maximum number of HWNDs
	if (pHorzSB || pVertSB)
	{
		CRect rectInside;
		CRect rectClient;

		HDWP hDWP = ::BeginDeferWindowPos(2);

		GetInsideRect(rectInside);
		SECGetClientRect(rectClient);

		// reposition scroll bars
		if (pHorzSB)
		{
			::DeferWindowPos(hDWP, pHorzSB->m_hWnd, NULL, rectInside.left,
				rectInside.bottom, rectInside.Width()+1, cyHorzScroll, 0);
			pHorzSB->Invalidate();			
		}

		if (pVertSB)
		{
			int cy = rectInside.Height()+1;
			
			if ( IsHeaderCtrlEnabled() )				
				cy += (m_cyHeader - HeaderOffset);

			::DeferWindowPos(hDWP, pVertSB->m_hWnd, NULL, rectInside.right,
			   //rectClient.top, cxVertScroll, cy, 0);
			   // and adding this
				rectClient.top + HeaderOffset, cxVertScroll, cy, 0);

			pVertSB->Invalidate();			
		}

		// move and resize all the windows at once!
		if (hDWP == NULL || !::EndDeferWindowPos(hDWP))
			TRACE0("Warning: DeferWindowPos failed - low system resources.\n");
	}
}

void CRPTreeBase::OnLButtonDown(UINT nFlags, CPoint point)
{
	SECTreeView::OnLButtonDown(nFlags, point);
	return;
}