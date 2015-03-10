// OurSplitter.cpp : implementation file
//

#include "stdafx.h"
#include "zeus.h"
#include "OurSplitter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COurSplitter

COurSplitter::COurSplitter()
{
}

COurSplitter::~COurSplitter()
{
}


BEGIN_MESSAGE_MAP(COurSplitter, CSplitterWnd)
	//{{AFX_MSG_MAP(COurSplitter)
	ON_WM_DROPFILES()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COurSplitter message handlers

void COurSplitter::OnDropFiles(HDROP hDropInfo) 
   {
   // send this on to the active pane
   GetActivePane()->SendMessage(WM_DROPFILES, (WPARAM)hDropInfo);
   }

