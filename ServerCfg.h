/////////////////////////////////////////////////////////////////////////////
// ServerCfg.h
//                                                                        
// Server properties dialogs for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __SERVERCFG_H__
#define __SERVERCFG_H__

#include "transfrm.h"
#include "transformlist.h"
#include "server.h"

/////////////////////////////////////////////////////////////////////////////
// CServerCfg dialog

class CServerCfg : public CPropertyPage
{
	DECLARE_DYNCREATE(CServerCfg)

// Construction
public:
	CServerCfg();
	~CServerCfg();

	CWnd* pWnd;

   void SetServer(CServer& Server);
   void GetServer(CServer& Server);

// Dialog Data
	//{{AFX_DATA(CServerCfg)
	enum { IDD = IDD_DIALOG_GPREFS_SERVER };
	CSpinButtonCtrl	m_delay_spin;
	CString	m_polldir;
	BOOL	m_enable_polling;
	BOOL	m_image_only_new;
	int		m_delay_edit;
	CString	m_transform;	
	BOOL	m_deletefiles;
	int		m_status;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CServerCfg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
   CTransformList ctl;   
   void OnOK();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CServerCfg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPollDir();
	afx_msg void OnEnablePolling();
	afx_msg void OnOurCancel();
	afx_msg void OnOurOK();
	afx_msg void OnStart();
	afx_msg void OnSelchangeStatus();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);	
	afx_msg void OnDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
