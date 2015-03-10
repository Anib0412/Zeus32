#if !defined(AFX_LISTENINGSOCKET_H__E85E235B_3D7A_11D4_A75F_00C04F7975DC__INCLUDED_)
#define AFX_LISTENINGSOCKET_H__E85E235B_3D7A_11D4_A75F_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListeningSocket.h : header file
//

class CPGPRCServer;

/////////////////////////////////////////////////////////////////////////////
// CListeningSocket command target

class CListeningSocket : public CSocket
{
// Attributes
public:

// Operations
public:
	CListeningSocket();
	virtual ~CListeningSocket();

// Construction
public:
	CListeningSocket(CPGPRCServer* pParent);

// Attributes
public:
	CPGPRCServer* m_pParent;


// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListeningSocket)
	public:
	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CListeningSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTENINGSOCKET_H__E85E235B_3D7A_11D4_A75F_00C04F7975DC__INCLUDED_)
