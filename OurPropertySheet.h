/////////////////////////////////////////////////////////////////////////////
// OurPropertySheet.h
//                                                                        
// property sheet derivation for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __OURPROPERTYSHEET_H__
#define __OURPROPERTYSHEET_H__

/////////////////////////////////////////////////////////////////////////////
// COurPropertySheet

class COurPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(COurPropertySheet)

// Construction
public:
	COurPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, 
	   UINT iSelectPage = -1);
	COurPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, 
	   UINT iSelectPage = -1);

// Attributes
public:
    int LastPage;

// Operations
public:
   
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COurPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COurPropertySheet();

protected:
    CString csTabSection, csTabEntry;
 	int ReadLastTab(UINT nIDCaption);
    int ReadLastTab(LPCTSTR pszCaption);
	void WriteLastTab();

	// Generated message map functions
protected:
	//{{AFX_MSG(COurPropertySheet)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif