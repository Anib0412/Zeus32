#if !defined(AFX_LOGWND_H__3C61ECBC_07DD_11D3_8E39_00C04F7975DC__INCLUDED_)
#define AFX_LOGWND_H__3C61ECBC_07DD_11D3_8E39_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogWnd.h : header file
//

#include "webbrowser.h"
#include "Log.h"

/////////////////////////////////////////////////////////////////////////////
// CLogWnd dialog

class CLogWnd : public CWnd
{
// Construction
public:
	CLogWnd();   // standard constructor

	CWebBrowser m_Log;
	void Navigate(LPCSTR url);	
	void Refresh();
	void Print();
	CRect* pcrLogWinRect;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogWnd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLogWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	
	afx_msg void OnDestroy();
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGWND_H__3C61ECBC_07DD_11D3_8E39_00C04F7975DC__INCLUDED_)
