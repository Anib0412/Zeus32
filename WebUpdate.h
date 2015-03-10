#if !defined(AFX_WEBUPDATE_H__2053CFBF_3DD9_11D3_8E6F_00C04F7975DC__INCLUDED_)
#define AFX_WEBUPDATE_H__2053CFBF_3DD9_11D3_8E6F_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WebUpdate.h : header file
//

#include "afxinet.h"

/////////////////////////////////////////////////////////////////////////////
// CWebUpdate dialog

class CWebUpdate : public CDialog
{
// Construction
public:
	CWebUpdate(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWebUpdate)
	enum { IDD = IDD_WEBUPDATE };
	CString	m_status;
	CString	m_current_version;
	CString	m_avail_version;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebUpdate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWebUpdate)
	afx_msg void OnCheckupdate();
	afx_msg void OnGetupdate();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEBUPDATE_H__2053CFBF_3DD9_11D3_8E6F_00C04F7975DC__INCLUDED_)
