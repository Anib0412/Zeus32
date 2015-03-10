/////////////////////////////////////////////////////////////////////////////
// XFormSave.h
//                                                                        
// Save a Custom Transform
// Copyright (C) 1996 Graphx, Inc.
//

/////////////////////////////////////////////////////////////////////////////
// CXformSave dialog

#ifndef __XFORMSAVE_H__
#define __XFORMSAVE_H__

#include "transformlist.h"
#include "transfrm.h"

class CXformSave : public CDialog
{
// Construction
public:
	CXformSave(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CXformSave)
	enum { IDD = IDD_SAVE_XFORM };
	CString	m_newxform;
	CString	m_xformsel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXformSave)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTransformList ctl;	

	// Generated message map functions
	//{{AFX_MSG(CXformSave)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeXformList();
	virtual void OnOK();
	afx_msg void OnChangeNewXform();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
