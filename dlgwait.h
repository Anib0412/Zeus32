// dlgwait.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWait dialog

#ifndef __DLGWAIT_H__
#define __DLGWAIT_H__

class CDlgWait : public CDialog
{
// Construction
public:
	CDlgWait(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWait)
	enum { IDD = IDD_MODELESS_WAIT };
	CString	m_prompt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWait)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWait)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
