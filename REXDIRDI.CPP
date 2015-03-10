// rexdirdi.cpp : implementation file
//

#include "stdafx.h"
#include "rexdirdi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRexDirDialog dialog
CRexDirDialog::CRexDirDialog(
	const CString& cstringPrompt, const CString& cstringTitle, CWnd* pParent /*=NULL*/)
	: m_cstringPrompt(cstringPrompt), m_cstringTitle(cstringTitle), CFileDialog(TRUE)
{
	//{{AFX_DATA_INIT(CRexDirDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CRexDirDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRexDirDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRexDirDialog, CFileDialog)
	//{{AFX_MSG_MAP(CRexDirDialog)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRexDirDialog message handlers

BOOL CRexDirDialog::OnInitDialog()
{
	GetDlgItem(IDC_ST_FILENAME)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_ED_FILENAME)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_LB_FILENAME)->ShowWindow(SW_HIDE);
    
    GetDlgItem(IDC_ST_FILETYPE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_CL_FILETYPE)->ShowWindow(SW_HIDE);
 
 	GetDlgItem(IDC_LB_DIRS)->SetFocus();
 	ResizeWindow();
 	SetDlgItemText(IDC_ED_FILENAME, "None");
 	
 	if (!m_cstringPrompt.IsEmpty())
 		SetDlgItemText(IDC_ST_DIRS, m_cstringPrompt);
 	if (!m_cstringTitle.IsEmpty())
 		SetWindowText( m_cstringTitle );
 	 	 	
	CFileDialog::OnInitDialog();
	m_bInitialUpdate = TRUE;
	return FALSE;  // return TRUE  unless you set the focus to a control
}

void CRexDirDialog::SetTitle(CString csTitle)
{
	m_cstringTitle = csTitle;
	return;
}

void CRexDirDialog::SetPrompt(CString csPrompt)
{
	m_cstringPrompt = csPrompt;
	return;
}


void CRexDirDialog::ResizeWindow()
{
	/*-------------------------------------------------------------------*
	*                          LOCAL VARIABLES                           *
	*-------------------------------------------------------------------*/
	CRect crectOK;
	CRect crectWnd;
	CRect crectDirs;
	/*-------------------------------------------------------------------*
	*                      VARIABLE INITIALIZATION                       *
	*-------------------------------------------------------------------*/
	GetDlgItem(IDOK)->GetWindowRect(crectOK);
	GetDlgItem(IDC_LB_DIRS)->GetWindowRect(crectDirs);
	GetWindowRect(crectWnd);
	/*********************************************************************
	*                           FUNCTION BODY                            *
	*********************************************************************/
	int intGap = crectWnd.right - crectOK.right;
	int intScroll = crectWnd.left - crectDirs.left + intGap;
	crectWnd.left = crectDirs.left - intGap;
	
	MoveWindow( crectWnd.left, crectWnd.top, crectWnd.Width(), 
			crectWnd.Height() );
			
	MoveChildren( this, intScroll, 0 );			
    CenterWindow();
}
// Walk through the childrem and move them over
void CRexDirDialog::MoveChildren(CWnd * pwndParent, int x, int y)
{
	/*-------------------------------------------------------------------*
	*                          LOCAL VARIABLES                           *
	*-------------------------------------------------------------------*/
	CWnd * pwndChild = pwndParent->GetWindow(GW_CHILD);
	/*********************************************************************
	*                           FUNCTION BODY                            *
	*********************************************************************/
	while ( pwndChild != NULL )
	{
		CRect crectChild;
		
		pwndChild->GetWindowRect(crectChild);
		ScreenToClient(crectChild);
		crectChild.OffsetRect(x, y);
		pwndChild->MoveWindow(crectChild.left, crectChild.top,
				crectChild.Width(), crectChild.Height() );
				
		MoveChildren(pwndChild, x, y);
		pwndChild = pwndChild->GetWindow(GW_HWNDNEXT);
	}
}

void CRexDirDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// Required kludge to repaint list box to show current selection
	if ( m_bInitialUpdate )
		{
		CListBox* plistbox = ((CListBox*)GetDlgItem(IDC_LB_DIRS));
		int wSelection = plistbox->GetCurSel();
		plistbox->SetCurSel(wSelection);
		m_bInitialUpdate = FALSE;
		}
		
	
	// Do not call CFileDialog::OnPaint() for painting messages
}

///////////////////////////////////////////////////////////////////
// Returns - current path with or without backslash
CString CRexDirDialog::GetPath(BOOL bKeepSlash /*DEF*/) const
{
	CString cstring = GetPathName();
	int wSlash = (bKeepSlash!=FALSE); // Assure 1 or 0
	
	if (!cstring.IsEmpty())
		cstring = cstring.Left(cstring.ReverseFind('\\')+wSlash);
	
	return cstring;
}
