/////////////////////////////////////////////////////////////////////////////
// TransformMgrPage.h
//                                                                        
// manage transforms for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __TRANSFORMMGRPAGE_H__
#define __TRANSFORMMGRPAGE_H__

#include "transformlist.h"
#include "transfrm.h"	  

/////////////////////////////////////////////////////////////////////////////
// CTransformMgrPage dialog

class CTransformMgrPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTransformMgrPage)

// Construction
public:
	CTransformMgrPage();
	~CTransformMgrPage();

// Dialog Data
	//{{AFX_DATA(CTransformMgrPage)
	enum { IDD = IDD_DIALOG_GPREFS_TRANSFORM };
	CString	m_xform;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTransformMgrPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   void ResetList();
   CTransformList ctl;

	// Generated message map functions
	//{{AFX_MSG(CTransformMgrPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelete();
	afx_msg void OnOurok();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point); 
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
