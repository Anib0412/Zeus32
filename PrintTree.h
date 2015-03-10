/////////////////////////////////////////////////////////////////////////////
// PrintTree.h
//                                                                        
// hold queue display/op class for RasterPlus95
// Copyright (C) 1999 Graphx, Inc.
//

#if !defined(AFX_PRINTTREE_H__22AFFB81_C1C4_11D2_8DF5_00C04F7975DC__INCLUDED_)
#define AFX_PRINTTREE_H__22AFFB81_C1C4_11D2_8DF5_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintTree.h : header file
//

#include "SpecialTree.h"

/////////////////////////////////////////////////////////////////////////////
// CPrintTree view

class CPrintTree : public CSpecialTree
{
protected:
	CPrintTree();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPrintTree)	 

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintTree)
	protected:
    virtual void OnInitialUpdate();
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPrintTree();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CPrintTree)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropFiles(HDROP hDropInfo);	
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnProcessCancelimaging();
	afx_msg void OnUpdateProcessCancelimaging(CCmdUI* pCmdUI);
	afx_msg void OnProcessPauseimaging();
	afx_msg void OnUpdateProcessPauseimaging(CCmdUI* pCmdUI);
	afx_msg void OnProcessResumeimaging();
	afx_msg void OnUpdateProcessResumeimaging(CCmdUI* pCmdUI);
	afx_msg void OnProcessSkiptonextfile();
	afx_msg void OnUpdateProcessSkiptonextfile(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTTREE_H__71166AA8_B277_11D2_8DD6_00C04F7975DC__INCLUDED_)