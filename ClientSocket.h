#if !defined(AFX_CLIENTSOCKET_H__E85E235C_3D7A_11D4_A75F_00C04F7975DC__INCLUDED_)
#define AFX_CLIENTSOCKET_H__E85E235C_3D7A_11D4_A75F_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClientSocket.h : header file
//

class CPGPRCServer;

#define PGPRC_SOCKET_DEADTIME    60 // minutes after which we will consider the socket dead

/////////////////////////////////////////////////////////////////////////////
// CClientSocket command target

class CClientSocket : public CSocket
{
// Attributes
public:

// Operations
public:
	CClientSocket(CPGPRCServer* pParent);
	virtual ~CClientSocket();
	void Init();

	CPGPRCServer* m_pParent;	
	CSocketFile* m_pFile;
	CArchive* m_pArchiveIn;
	CArchive* m_pArchiveOut;
	CTime ctCreated;
	CTime ctLastAccessed;

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CClientSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSOCKET_H__E85E235C_3D7A_11D4_A75F_00C04F7975DC__INCLUDED_)
