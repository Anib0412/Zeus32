// OrderForm.h : interface of the CPropertyPane class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ORDERFORM_H__C61A0591_3040_11D2_BE74_E1F262EE6A58__INCLUDED_)
#define AFX_ORDERFORM_H__C61A0591_3040_11D2_BE74_E1F262EE6A58__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include "Mainfrm.h"
#include "rpnode.h"

class CPropertyPane : public CFormView
{
protected: // create from serialization only
	CPropertyPane();
	DECLARE_DYNCREATE(CPropertyPane)

public:
	//{{AFX_DATA(CPropertyPane)
	enum{ IDD = IDD_PRINTDEV_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:	
   void OnTreeChanged(CRPNode* pNode);
   void CancelRendering();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPane)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo*);	
	//}}AFX_VIRTUAL

// Implementation
public:
	void DoPrint();
	void DoPrintPreview();
	virtual ~CPropertyPane();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyPane)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/*
#ifndef _DEBUG  // debug version in OrderForm.cpp
inline CPhotogizeLabDoc* CPropertyPane::GetDocument()
   { return (CPhotogizeLabDoc*)m_pDocument; }
#endif
*/
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ORDERFORM_H__C61A0591_3040_11D2_BE74_E1F262EE6A58__INCLUDED_)
