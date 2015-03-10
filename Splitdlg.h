/////////////////////////////////////////////////////////////////////////////
// SPLITDLG.H
//                                                                        
// dialog for file splitting for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef _SPLITDLG_H_
#define _SPLITDLG_H_

/////////////////////////////////////////////////////////////////////////////
// SplitDlg dialog

class SplitDlg : public CDialog
{
// Construction
public:
	SplitDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(SplitDlg)
	enum { IDD = IDD_DIALOG_WINSPLIT };
	BOOL	m_deletefile;
	BOOL	m_removefile;
	BOOL	m_insertfile;
	CString	m_status;
	//}}AFX_DATA
	
	CString InputFileName;
	CString ShortFileName;
	int TotalFrames;	
	CStringArray SplitFileArray;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(SplitDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeletefile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
