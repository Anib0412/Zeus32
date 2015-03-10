/////////////////////////////////////////////////////////////////////////////
// RPTreeBase.h
//                                                                        
// lower-level base tree display/op class for RasterPlus
// Copyright (C) 1999 Graphx, Inc.
//

#if !defined(AFX_RPTREEBASE_H__CE06E314_4516_11D3_8E83_00C04F7975DC__INCLUDED_)
#define AFX_RPTREEBASE_H__CE06E314_4516_11D3_8E83_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RPTreeBase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRPTreeBase window

class CRPTreeBase : public SECTreeView
{
// Construction
public:
	CRPTreeBase();
	DECLARE_DYNCREATE(CRPTreeBase)
    
	void RecalcLayout();      

// Attributes
public:
   CString csTreeTitle; 
   int DrawHeaderPushCol;
   int iViewStyle;

protected:
   int HeaderOffset;  

// Operations
public:

protected:
   virtual int HitTest( CPoint pt, UINT* pFlags, int& nHitCol ) const;
   void DrawHeader(LvPaintContext* pPC);
   virtual void DoPaint(CDC* pDC);
   virtual void OnLButtonDown(UINT nFlags, CPoint point);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRPTreeBase)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRPTreeBase();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRPTreeBase)
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RPTREEBASE_H__CE06E314_4516_11D3_8E83_00C04F7975DC__INCLUDED_)
