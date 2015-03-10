#if !defined(AFX_EDITJOBDESCRIPTION_H__33294B37_563A_11D3_8E95_00C04F7975DC__INCLUDED_)
#define AFX_EDITJOBDESCRIPTION_H__33294B37_563A_11D3_8E95_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditJobDescription.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditJobDescription dialog

class CEditJobDescription : public CDialog
{
// Construction
public:
	CEditJobDescription(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditJobDescription)
	enum { IDD = IDD_JOBDESCRIPTION };
	CString	m_description;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditJobDescription)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditJobDescription)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITJOBDESCRIPTION_H__33294B37_563A_11D3_8E95_00C04F7975DC__INCLUDED_)
