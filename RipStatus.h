#if !defined(AFX_RIPSTATUS_H__46DAB21F_D237_11D2_8DFE_00C04F7975DC__INCLUDED_)
#define AFX_RIPSTATUS_H__46DAB21F_D237_11D2_8DFE_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RipStatus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRipStatus dialog

class CRipStatus : public CDialog
{
// Construction
public:
	CRipStatus(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRipStatus)
	enum { IDD = CG_IDD_RIPSTATUS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRipStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRipStatus)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RIPSTATUS_H__46DAB21F_D237_11D2_8DFE_00C04F7975DC__INCLUDED_)
