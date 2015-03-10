#if !defined(AFX_OURSPLITTER_H__29F09D72_CA62_11D2_8DF7_00C04F7975DC__INCLUDED_)
#define AFX_OURSPLITTER_H__29F09D72_CA62_11D2_8DF7_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OurSplitter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COurSplitter window

class COurSplitter : public CSplitterWnd
{
// Construction
public:
	COurSplitter();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COurSplitter)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COurSplitter();

	// Generated message map functions
protected:
	//{{AFX_MSG(COurSplitter)
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OURSPLITTER_H__29F09D72_CA62_11D2_8DF7_00C04F7975DC__INCLUDED_)
