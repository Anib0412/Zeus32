/////////////////////////////////////////////////////////////////////////////
// GENERAL.H
//                                                                        
// general config class for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __GENERAL_H__
#define __GENERAL_H__

/////////////////////////////////////////////////////////////////////////////
// CGeneralPrefs dialog

class CGeneralPrefs : public CPropertyPage
{
// Construction
public:
	CGeneralPrefs();

// Dialog Data
	//{{AFX_DATA(CGeneralPrefs)
	enum { IDD = IDD_DIALOG_GPREFS_GENERAL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeneralPrefs)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGeneralPrefs)
	virtual BOOL OnInitDialog();	
	virtual void OnOurCancel();	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
