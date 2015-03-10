// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
#include "zeus.h"
#include "ClientSocket.h"
#include "GISServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientSocket

CClientSocket::CClientSocket(CPGPRCServer* pParent)
   {
   m_pParent = pParent;
   
   m_pFile = NULL;
   m_pArchiveIn = NULL;
   m_pArchiveOut = NULL;
   }

void CClientSocket::Init()
   {
   m_pFile = new CSocketFile(this);
   m_pArchiveIn = new CArchive(m_pFile,CArchive::load);
   m_pArchiveOut = new CArchive(m_pFile,CArchive::store);

   // set the creation time
   ctCreated = CTime::GetCurrentTime();

   struct linger linger2;
   linger2.l_onoff = 1 ;       // wait to send data
   linger2.l_linger = 20000 ;	// 20 second timeout
   SetSockOpt(SO_LINGER, &linger2, sizeof(linger2));
   }

CClientSocket::~CClientSocket()
   {
   // close the socket
   Close();

   if (m_pArchiveOut != NULL)
	  delete m_pArchiveOut;
   
   if (m_pArchiveIn != NULL)
	  delete m_pArchiveIn;
   
   if (m_pFile != NULL)
	  delete m_pFile;
   }				 

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CClientSocket, CSocket)
	//{{AFX_MSG_MAP(CClientSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CClientSocket member functions

void CClientSocket::OnReceive(int nErrorCode) 
   {
   CSocket::OnReceive(nErrorCode);

   // set the last time this was accessed
   ctLastAccessed = CTime::GetCurrentTime();

   m_pParent->ProcessPendingRead(this);	    
   }
