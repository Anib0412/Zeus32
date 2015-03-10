/////////////////////////////////////////////////////////////////////////////
// GlobalProperties.h
//                                                                        
// Global program properties for RasterPlus
// Copyright (C) 1996-99 Graphx, Inc.
//

#ifndef __GLOBALPROPERTIES_H__
#define __GLOBALPROPERTIES_H__

#include "OurPropertySheet.h"
#include "GeneralPrefsPage.h"
#include "FontManagerPage.h"
#include "PrintIndexPage.h"

/////////////////////////////////////////////////////////////////////////////
// CGlobalProperties

class CGlobalProperties : public CPropertySheet
{
	DECLARE_DYNAMIC(CGlobalProperties)

// Construction
public:
	//CGlobalProperties(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = -1);
	//CGlobalProperties(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = -1);
   CGlobalProperties(CWnd* pWndParent = NULL, CString csTitle = "");

   CGeneralPrefsPage m_GeneralPrefsPage;

   CFontManagerPage m_FontManagerPage;
   CPrintIndexPage	m_PrintIndexPage;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGlobalProperties)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGlobalProperties();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGlobalProperties)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
