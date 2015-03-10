// ListeningSocket.cpp : implementation file
//

#include "stdafx.h"
#include "zeus.h"
#include "ListeningSocket.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListeningSocket

CListeningSocket::CListeningSocket()
   {
   }

CListeningSocket::~CListeningSocket()
   {
   }

CListeningSocket::CListeningSocket(CPGPRCServer* pParent)
   {
   m_pParent = pParent;
   }


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CListeningSocket, CSocket)
	//{{AFX_MSG_MAP(CListeningSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CListeningSocket member functions

void CListeningSocket::OnAccept(int nErrorCode) 
   {
   CSocket::OnAccept(nErrorCode);
   m_pParent->ProcessPendingAccept();
   }

void CListeningSocket::OnClose(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CSocket::OnClose(nErrorCode);
}
