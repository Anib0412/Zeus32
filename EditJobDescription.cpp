// EditJobDescription.cpp : implementation file
//

#include "stdafx.h"
#include "zeus.h"
#include "EditJobDescription.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditJobDescription dialog


CEditJobDescription::CEditJobDescription(CWnd* pParent /*=NULL*/)
	: CDialog(CEditJobDescription::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditJobDescription)
	m_description = _T("");
	//}}AFX_DATA_INIT
}


void CEditJobDescription::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditJobDescription)
	DDX_Text(pDX, IDC_DESCRIPTION, m_description);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditJobDescription, CDialog)
	//{{AFX_MSG_MAP(CEditJobDescription)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditJobDescription message handlers
