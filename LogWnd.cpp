// LogWnd.cpp : implementation file
//

#include "stdafx.h"
#include "zeus.h"
#include "LogWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogWnd dialog


CLogWnd::CLogWnd()	
{
	//{{AFX_DATA_INIT(CLogWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CLogWnd::DoDataExchange(CDataExchange* pDX)
{
	CWnd::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogWnd)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLogWnd, CWnd)
	//{{AFX_MSG_MAP(CLogWnd)
	ON_WM_SIZE()
	ON_WM_CREATE()	
	ON_WM_DESTROY()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogWnd message handlers		 

void CLogWnd::Navigate(LPCSTR url)
   {
   COleVariant varEmpty; // Default is VT_EMPTY
   // need to make a modeless dialog here
	
   m_Log.Navigate(url, &varEmpty, &varEmpty,
	  &varEmpty, &varEmpty );
   }

void CLogWnd::OnSize(UINT nType, int cx, int cy) 
   {
   CWnd::OnSize(nType, cx, cy);

   m_Log.MoveWindow( 0, 0, cx, cy, FALSE );      
   }

int CLogWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if ( ! m_Log.Create( NULL, WS_CHILD | WS_VISIBLE, CRect(), this,
		 101) )
		return -1;
	
	return 0;
}


void CLogWnd::OnDestroy() 
{
	CWnd::OnDestroy();
	
	m_Log.DestroyWindow();	
	
}

void CLogWnd::Refresh()
   {
   if (m_Log)
	  {
      if (m_Log.IsWindowVisible())
         m_Log.Refresh();
	  }
   }

void CLogWnd::Print()
   {
   if (m_Log)
	  {
      LPDISPATCH lpDispatch = NULL;
      LPOLECOMMANDTARGET lpOleCommandTarget = NULL;

      lpDispatch = m_Log.GetDocument();
      ASSERT(lpDispatch);

      lpDispatch->QueryInterface(IID_IOleCommandTarget, (void**)&lpOleCommandTarget);
      ASSERT(lpOleCommandTarget);

      lpDispatch->Release();

      // Print contents of WebBrowser control.
      lpOleCommandTarget->Exec(NULL, OLECMDID_PRINT, 0, NULL,NULL);
      lpOleCommandTarget->Release();	
	  }
   }

void CLogWnd::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
   {
   CWnd::OnWindowPosChanged(lpwndpos);   

   // save the resized coords
   if (!IsIconic())
      GetWindowRect(pcrLogWinRect);
   }
