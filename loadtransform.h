/////////////////////////////////////////////////////////////////////////////
// LoadTransform.h
//                                                                        
// Load a saved Transform into Custom Transform editor
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __LOADTRANSFORM_H__
#define __LOADTRANSFORM_H__

#include "transformlist.h"
#include "transfrm.h"

/////////////////////////////////////////////////////////////////////////////
// CLoadTransform dialog

class CLoadTransform : public CDialog
{
// Construction
public:
	CLoadTransform(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLoadTransform)
	enum { IDD = IDD_LOADTRANSFORM };
	CString	m_trans;
	//}}AFX_DATA

	CTransform* pSelectedTransform;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoadTransform)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    CTransformList ctl;	

	// Generated message map functions
	//{{AFX_MSG(CLoadTransform)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTlist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
