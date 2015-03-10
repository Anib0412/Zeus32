// devconnect.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDevConnect dialog

#ifndef __DEVCONNECT_H__
#define __DEVCONNECT_H__

class CDevConnect : public CDialog
{
// Construction
public:
	CDevConnect(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDevConnect)
	enum { IDD = IDD_DEVCONNECT };
	CString	m_csPort;
	//}}AFX_DATA
		
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDevConnect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDevConnect)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
