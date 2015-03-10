// dlgwait.cpp : implementation file
//

#include "stdafx.h"
#include "zeus.h"
#include "dlgwait.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWait dialog


CDlgWait::CDlgWait(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWait::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWait)
	m_prompt = _T("");
	//}}AFX_DATA_INIT
}


void CDlgWait::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWait)
	DDX_Text(pDX, IDC_PROMPT, m_prompt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWait, CDialog)
	//{{AFX_MSG_MAP(CDlgWait)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDlgWait message handlers
