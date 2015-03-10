// RipStatus.cpp : implementation file
//

#include "stdafx.h"
#include "zeus.h"
#include "RipStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRipStatus dialog


CRipStatus::CRipStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CRipStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRipStatus)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRipStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRipStatus)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRipStatus, CDialog)
	//{{AFX_MSG_MAP(CRipStatus)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRipStatus message handlers
