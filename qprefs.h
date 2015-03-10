/////////////////////////////////////////////////////////////////////////////
// QPREFS.H
//                                                                        
// Queue properties dialogs for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __QPREFS_H__
#define __QPREFS_H__

#include "OurPropertySheet.h"
#include "devdlg.h"
#include "transdlg.h"
#include "ServerCfg.h"
#include "ftypes.h"

/////////////////////////////////////////////////////////////////////////////
// CGPrefDlgRip dialog

class CGPrefDlgRip : public CPropertyPage
{
// Construction
public:
	CGPrefDlgRip();
	
// Dialog Data
	//{{AFX_DATA(CGPrefDlgRip)
	enum { IDD = IDD_DIALOG_GPREFS_RIP };
	CString	m_atmpath;
	CString	m_ttpath;	
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CGPrefDlgRip)
	virtual BOOL OnInitDialog();	
	virtual void OnOK();
	virtual void OnOurOK();
	afx_msg void OnOurCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CQueuePrefs dialog

class CQueuePrefs : public COurPropertySheet
{
// Construction
public:			   	
	CQueuePrefs(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = -1);
	CQueuePrefs(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = -1);
	~CQueuePrefs();
	
    CDialogDevSetup m_dialog_dev_setup;                        
	CDialogTransSetup m_dialog_trans_setup;
	CGPrefDlgRip m_dialog_rip_setup;
	CServerCfg m_dialog_server_setup;	

// Dialog Data
	//{{AFX_DATA(CQueuePrefs)	
	//}}AFX_DATA

// Implementation
protected:
    void InitVars();

	// Generated message map functions
	//{{AFX_MSG(CQueuePrefs)	
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};                       

#endif


