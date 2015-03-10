/////////////////////////////////////////////////////////////////////////////
// ArchiveTree.h
//                                                                        
// archive queue display/op class for RasterPlus95
// Copyright (C) 1999 Graphx, Inc.
//

#if !defined(AFX_ARCHIVETREE_H__22AFFB82_C1C4_11D2_8DF5_00C04F7975DC__INCLUDED_)
#define AFX_ARCHIVETREE_H__22AFFB82_C1C4_11D2_8DF5_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ArchiveTree.h : header file
//

#include "SpecialTree.h"
/////////////////////////////////////////////////////////////////////////////
// CArchiveTree view

class CArchiveTree : public CSpecialTree
{
protected:
	CArchiveTree();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CArchiveTree)	  

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArchiveTree)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CArchiveTree();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CArchiveTree)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropFiles(HDROP hDropInfo);	
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);		
	afx_msg void OnUpdateOurFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOurFilePrintSel(CCmdUI* pCmdUI);
	afx_msg void OnOurFilePrint();
	afx_msg void OnOurFilePrintSel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARCHIVETREE_H__71166AA9_B277_11D2_8DD6_00C04F7975DC__INCLUDED_)
