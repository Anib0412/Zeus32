// OrderForm.cpp : implementation of the CPropertyPane class
//

#include "stdafx.h"
#include "propertypane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyPane

IMPLEMENT_DYNCREATE(CPropertyPane, CFormView)

BEGIN_MESSAGE_MAP(CPropertyPane, CFormView)
	//{{AFX_MSG_MAP(CPropertyPane)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, DoPrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, DoPrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPane construction/destruction

CPropertyPane::CPropertyPane()
	: CFormView(CPropertyPane::IDD)
{
	//{{AFX_DATA_INIT(CPropertyPane)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here	
//    m_autoprintinvoice = FALSE;
}

CPropertyPane::~CPropertyPane()
{
}

void CPropertyPane::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyPane)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CPropertyPane::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyPane printing

BOOL CPropertyPane::OnPreparePrinting(CPrintInfo* pInfo)
{
	return TRUE;
}

void CPropertyPane::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
}				

void CPropertyPane::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	return;
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyPane diagnostics

#ifdef _DEBUG
void CPropertyPane::AssertValid() const
{
	CFormView::AssertValid();
}

void CPropertyPane::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

/*
CPhotogizeLabDoc* CPropertyPane::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPhotogizeLabDoc)));
	return (CPhotogizeLabDoc*)m_pDocument;
}							
*/
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPropertyPane message handlers

void CPropertyPane::DoPrintPreview()
   {
   CFormView::OnFilePrintPreview();
   }

void CPropertyPane::DoPrint()
   {
   CFormView::OnFilePrint();
   }

void CPropertyPane::OnInitialUpdate() 
{
	static	BOOL FirstTime=TRUE;
    CMainFrame* pFrame;
    CRect rectFrame, rectView;

    CFormView::OnInitialUpdate();
	UpdateData(FALSE);

	if(FirstTime==TRUE)
	{
		FirstTime=FALSE;
		// Resize parent to fit dialog template exactly    
		// while not resizing beyond size of screen
		VERIFY( pFrame = (CMainFrame*)GetParentFrame() );
		pFrame->GetClientRect( rectFrame );
		GetClientRect( rectView );
		if ( rectFrame.Width() > rectView.Width()
			 || rectFrame.Height() < rectView.Height() )
		{        
			// Resize so can be refit to any template that fits the screen
			pFrame->MoveWindow( 0, 0, GetSystemMetrics(SM_CXSCREEN), 
							GetSystemMetrics(SM_CYSCREEN), FALSE ); 
		}
		pFrame->RecalcLayout();
		ResizeParentToFit(TRUE);    // Shrink to fit template
		pFrame->CenterWindow();
	}	
}

void CPropertyPane::OnPaint() 
{
	CMainFrame* pFrame;
	CPaintDC dc(this); // device context for painting
	VERIFY( pFrame = (CMainFrame*)GetParentFrame() );
	CString s;
	s.LoadString(AFX_IDS_APP_TITLE);
	
	pFrame->SetWindowText(s);
}


int CPropertyPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
//    GetDocument()->m_OrderForm = this;		

	return 0;
}