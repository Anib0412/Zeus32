// PrinterNameChange.cpp : implementation file
//

#include "stdafx.h"
#include "zeus.h"
#include "PrinterNameChange.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrinterNameChange dialog


CPrinterNameChange::CPrinterNameChange(CWnd* pParent /*=NULL*/)
	: CDialog(CPrinterNameChange::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrinterNameChange)
	m_newname = _T("");
	m_oldname = _T("");
	//}}AFX_DATA_INIT
}


void CPrinterNameChange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrinterNameChange)
	DDX_Text(pDX, IDC_NEWNAME, m_newname);
	DDX_Text(pDX, IDC_OLDNAME, m_oldname);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrinterNameChange, CDialog)
	//{{AFX_MSG_MAP(CPrinterNameChange)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrinterNameChange message handlers
