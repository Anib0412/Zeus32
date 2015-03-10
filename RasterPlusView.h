#if !defined(AFX_RASTERPLUSVIEW_H__22AFFB7E_C1C4_11D2_8DF5_00C04F7975DC__INCLUDED_)
#define AFX_RASTERPLUSVIEW_H__22AFFB7E_C1C4_11D2_8DF5_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RasterPlusView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRasterPlusView view

class CRasterPlusView : public CView
{
protected:
	CRasterPlusView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRasterPlusView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRasterPlusView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CRasterPlusView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CRasterPlusView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RASTERPLUSVIEW_H__22AFFB7E_C1C4_11D2_8DF5_00C04F7975DC__INCLUDED_)
