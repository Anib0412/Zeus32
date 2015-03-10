/////////////////////////////////////////////////////////////////////////////
// PropertyFrame.cpp
//                                                                        
// parent class of job props
// Copyright (C) 1999 Graphx, Inc.
//
// mods:
//
// 9-17-99  Version 3.0.1 - make sure that PositionWindows doesn't move window off-screen

#include "stdafx.h"
#include "resource.h"
#include "PropertyFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyFrame

IMPLEMENT_DYNCREATE(CPropertyFrame, CMiniFrameWnd)

CPropertyFrame::CPropertyFrame()
{
	m_pModelessPropSheet = NULL;
	bDeleted = FALSE;
}

CPropertyFrame::~CPropertyFrame()
{
//delete m_pModelessPropSheet;
}


BEGIN_MESSAGE_MAP(CPropertyFrame, CMiniFrameWnd)
	//{{AFX_MSG_MAP(CPropertyFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_ACTIVATE()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyFrame message handlers

int CPropertyFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMiniFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pModelessPropSheet = new CJobPropsSheet(FALSE, this);
	if (!m_pModelessPropSheet->Create(this, 
		WS_CHILD | WS_VISIBLE, 0))
	{
		delete m_pModelessPropSheet;
		m_pModelessPropSheet = NULL;
		return -1;
	}

	// Resize the mini frame so that it fits around the child property
	// sheet.
	CRect rectClient, rectWindow;
	m_pModelessPropSheet->GetWindowRect(rectClient);
	rectWindow = rectClient;

	// CMiniFrameWnd::CalcWindowRect adds the extra width and height
	// needed from the mini frame.
	CalcWindowRect(rectWindow);
	SetWindowPos(NULL, rectWindow.left, rectWindow.top,
		rectWindow.Width(), rectWindow.Height(),
		SWP_NOZORDER | SWP_NOACTIVATE);
	m_pModelessPropSheet->SetWindowPos(NULL, 0, 0,
		rectClient.Width(), rectClient.Height(),
		SWP_NOZORDER | SWP_NOACTIVATE);

	return 0;
}

void CPropertyFrame::OnClose() 
{
	// Instead of closing the modeless property sheet, just hide it.
	ShowWindow(SW_HIDE);
}

void CPropertyFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CMiniFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	// Forward any WM_ACTIVATEs to the property sheet...
	// Like the dialog manager itself, it needs them to save/restore the focus.
	ASSERT_VALID(m_pModelessPropSheet);

	// Use GetCurrentMessage to get unmodified message data.
	const MSG* pMsg = GetCurrentMessage();
	ASSERT(pMsg->message == WM_ACTIVATE);
	m_pModelessPropSheet->SendMessage(pMsg->message, pMsg->wParam,
		pMsg->lParam);
}

void CPropertyFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
   {
   CMiniFrameWnd::OnShowWindow(bShow, nStatus);
   // notify the prop sheet about the visual state of this
   // frame
   m_pModelessPropSheet->FrameShow(bShow);
   }

void CPropertyFrame::PositionWindow()
   {
   CPoint   Point;
   CRect    OurRect;
   CRect    ParentRect;   
   int      nWidth;
   int      nHeight;   

   // Get the size of the dialog box.
   GetWindowRect(OurRect);
   
   // get working area on the desktop
   RECT rc;
   SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
   ParentRect = rc;					 
      
   // Calculate the height and width for MoveWindow().
   nWidth = OurRect.Width();
   nHeight = OurRect.Height();   

   // Find the lower right point
   Point.x = ParentRect.Width();
   Point.y = ParentRect.Height();
   
   // Calculate the new X, Y starting point.
   Point.x -= nWidth;
   Point.y -= nHeight;

   // Move the window.
   // make sure that the window is on-screen
   MoveWindow(Point.x, max(Point.y, 0), nWidth, nHeight, FALSE);
   }
