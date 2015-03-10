// RasterPlusView.cpp : implementation file
//

#include "stdafx.h"
#include "zeus.h"
#include "RasterPlusView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRasterPlusView

IMPLEMENT_DYNCREATE(CRasterPlusView, CView)

CRasterPlusView::CRasterPlusView()
{
}

CRasterPlusView::~CRasterPlusView()
{
}


BEGIN_MESSAGE_MAP(CRasterPlusView, CView)
	//{{AFX_MSG_MAP(CRasterPlusView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRasterPlusView drawing

void CRasterPlusView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CRasterPlusView diagnostics

#ifdef _DEBUG
void CRasterPlusView::AssertValid() const
{
	CView::AssertValid();
}

void CRasterPlusView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


void CRasterPlusView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
//TRACE("On update\n");	// TODO: Add your specialized code here and/or call the base class
	
	
}
