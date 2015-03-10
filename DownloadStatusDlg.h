#if !defined(AFX_DOWNLOADSTATUSDLG_H__371BEA6D_35DC_41EF_84D6_342425424545__INCLUDED_)
#define AFX_DOWNLOADSTATUSDLG_H__371BEA6D_35DC_41EF_84D6_342425424545__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DownloadStatusDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDownloadStatusDlg dialog

class CDownloadStatusDlg : public CDialog
{
// Construction
public:
	CDownloadStatusDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL bCancel;

// Dialog Data
	//{{AFX_DATA(CDownloadStatusDlg)
	enum { IDD = IDD_DOWNLOADFILE };
	CProgressCtrl	m_progress;
	CString	m_status;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDownloadStatusDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDownloadStatusDlg)
	afx_msg void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOWNLOADSTATUSDLG_H__371BEA6D_35DC_41EF_84D6_342425424545__INCLUDED_)

