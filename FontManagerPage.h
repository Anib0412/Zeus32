/////////////////////////////////////////////////////////////////////////////
// FontManagerPage.h
//                                                                        
// font manager for RasterPlus
// Copyright (C) 1999 Graphx, Inc.
//

#if !defined(AFX_FONTMANAGERPAGE_H__CF1A3BA3_9868_11D3_9232_00C04FA3598D__INCLUDED_)
#define AFX_FONTMANAGERPAGE_H__CF1A3BA3_9868_11D3_9232_00C04FA3598D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FontManagerPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFontManagerPage dialog

#include "CResourceList.h"
#include "resource.h"

class CFontManagerPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFontManagerPage)

// Construction
public:
	CFontManagerPage();
	~CFontManagerPage();

// Dialog Data
	//{{AFX_DATA(CFontManagerPage)
	enum { IDD = IDD_FONT_MANAGER_PAGE };
	CString	m_FontDir;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFontManagerPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CResourceList FontList, AddFontList;
	BOOL bIsInit;
	
	BOOL InitAddFontListBox(BOOL bUseDefaultDir=TRUE);
	void UpdateAddButton();

	// Generated message map functions
	//{{AFX_MSG(CFontManagerPage)
	afx_msg void OnRemoveFont();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeInstalledFonts();
	afx_msg void OnBrowse();
	afx_msg void OnAddFont();
	afx_msg void OnDblclkNewFonts();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point); 
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);		
	afx_msg void OnSelchangeNewFonts();
	afx_msg void OnChangeFontdir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTMANAGERPAGE_H__CF1A3BA3_9868_11D3_9232_00C04FA3598D__INCLUDED_)
