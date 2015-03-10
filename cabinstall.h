/////////////////////////////////////////////////////////////////////////////
// cabinstall.h
//                                                                        
// cabinet dialog handler for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

/////////////////////////////////////////////////////////////////////////////
// CCabInstall dialog

#ifndef __CABINSTALL_H__
#define __CABINSTALL_H__

class CCabInstall : public CDialog
{
// Construction
public:
	CCabInstall(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCabInstall)
	enum { IDD = IDD_CABINSTALL };
	CString	m_prompt1;
	CString	m_prompt2;
	CString	m_path;
	//}}AFX_DATA	

    CStringArray* pcsaInstallLocs;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCabInstall)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCabInstall)
	afx_msg void OnBrowse();	
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
