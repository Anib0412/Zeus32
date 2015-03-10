// devconnect.cpp : implementation file
//

#include "stdafx.h"
#include "zeus.h"
#include "devconnect.h"
#include "winspool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDevConnect dialog


CDevConnect::CDevConnect(CWnd* pParent /*=NULL*/)
	: CDialog(CDevConnect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDevConnect)
	m_csPort = _T("");
	//}}AFX_DATA_INIT

	m_csPort = "";
}


void CDevConnect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDevConnect)
	DDX_LBString(pDX, IDC_CONNECTLIST, m_csPort);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDevConnect, CDialog)
	//{{AFX_MSG_MAP(CDevConnect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDevConnect message handlers

BOOL CDevConnect::OnInitDialog() 
   {
   CDialog::OnInitDialog();	
   
   DWORD dwSizeNeeded, dwNumPorts;
   LPBYTE pBuf;   
   PORT_INFO_1* p1;
   DWORD i;
   char* pPortName;
   int idx;

   CListBox* cl	= (CListBox*)GetDlgItem(IDC_CONNECTLIST);
   
   // get size needed
   ::EnumPorts(NULL, 1, NULL, 0, &dwSizeNeeded, &dwNumPorts);
   pBuf = new BYTE [dwSizeNeeded];
   ::EnumPorts(NULL, 1, pBuf, dwSizeNeeded, &dwSizeNeeded, &dwNumPorts);

   // insert ports
   p1 = (PORT_INFO_1*)pBuf;
   for (i = 0; i < dwNumPorts; i++)
      {
	  pPortName = (p1 + i)->pName;
	  cl->InsertString(-1, pPortName);
	  }   
   delete pBuf;

   // set current or first if no current
   if ((idx = cl->FindStringExact(-1, m_csPort)) != LB_ERR)
      cl->SetCurSel(idx);
   else
      cl->SetCurSel(0);
   
   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
   }

void CDevConnect::OnOK() 
   {
   CListBox* cl	= (CListBox*)GetDlgItem(IDC_CONNECTLIST);

   // get current selection
   cl->GetText(cl->GetCurSel(), m_csPort);
	
   CDialog::OnOK();
   }
