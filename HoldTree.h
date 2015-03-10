/////////////////////////////////////////////////////////////////////////////
// HoldTree.h
//                                                                        
// hold queue display/op class for RasterPlus95
// Copyright (C) 1999 Graphx, Inc.
//

#if !defined(AFX_HOLDTREE_H__22AFFB80_C1C4_11D2_8DF5_00C04F7975DC__INCLUDED_)
#define AFX_HOLDTREE_H__22AFFB80_C1C4_11D2_8DF5_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HoldTree.h : header file
//

#include "SpecialTree.h"
// #include "IndexPrint.h"
#include "PrintIndexDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CHoldTree view

class CHoldTree : public CSpecialTree
{
protected:
	CHoldTree();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CHoldTree)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHoldTree)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CHoldTree();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

//	CIndexPrint indexPrint;
	CPrintIndexDlg* pPrintIndexDlg;

	// Generated message map functions
protected:
	//{{AFX_MSG(CHoldTree)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	
	afx_msg void OnDropFiles(HDROP hDropInfo);	
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);		
	afx_msg void OnFilePrintPreviewSel();	
	afx_msg void OnOurFilePrintPreview();
	afx_msg void OnFilePrintHDPreviewSel();	
	afx_msg void OnOurFilePrintHDPreview();
	afx_msg void OnProcessCancelimaging();
	afx_msg void OnUpdateProcessCancelimaging(CCmdUI* pCmdUI);
	afx_msg void OnProcessPauseimaging();
	afx_msg void OnUpdateProcessPauseimaging(CCmdUI* pCmdUI);
	afx_msg void OnProcessResumeimaging();
	afx_msg void OnUpdateProcessResumeimaging(CCmdUI* pCmdUI);
	afx_msg void OnProcessSkiptonextfile();
	afx_msg void OnUpdateProcessSkiptonextfile(CCmdUI* pCmdUI);
	afx_msg void OnOurFilePrint();
	afx_msg void OnOurFilePrintIndex();
	afx_msg void OnOurFilePrintSelIndex();
	afx_msg void OnUpdateOurFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnOurFilePrintSel();
	afx_msg void OnUpdateOurFilePrintSel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintPreviewSel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOurFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintHDPreviewSel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOurFilePrintHDPreview(CCmdUI* pCmdUI);
	afx_msg LRESULT OnPausePreview(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetPreviewJob(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPrintJobID(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOLDTREE_H__71166AAA_B277_11D2_8DD6_00C04F7975DC__INCLUDED_)
