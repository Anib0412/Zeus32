/////////////////////////////////////////////////////////////////////////////
// NewPreview.h
//                                                                        
// preview classes for RasterPlus
// Copyright (C) 1999 Graphx, Inc.
//

#if !defined(AFX_NEWPREVIEW_H__C51CC771_BB12_11D2_8DEA_00C04F7975DC__INCLUDED_)
#define AFX_NEWPREVIEW_H__C51CC771_BB12_11D2_8DEA_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include"prev.h"
#include"disp.h"
#include "ruler.h"
#define XFORM_UNITS_PICA    0
#define XFORM_UNITS_INCH    1
#define XFORM_UNITS_MM      2

// NewPreview.h : header file
//

// the object holding the preview
// information
class CPreviewPage : public CObject
   {
public:
   CPreviewPage();
   ~CPreviewPage();
   CString csTitle;
   PREVIEWINFO PreviewInfo;   
   CDisp cd;
   };				  

/////////////////////////////////////////////////////////////////////////////
// CPrevCtrl dialog

class CPrevCtrl : public CDialogBar
{
// Construction
public:
	CPrevCtrl();
	void SetCount(int Page, int Total, int iPreviewType );	
	SECBitmapButton secbNext;
	SECBitmapButton secbPrev;
	SECBitmapButton secbBegin;
	SECBitmapButton secbEnd;
	SECBitmapButton secbClose;
	SECBitmapButton secbPrint;
	SECBitmapButton secbZoomIn;
	SECBitmapButton secbZoomOut;

	BOOL m_bNotCreated;

// Dialog Data
	//{{AFX_DATA(CPrevCtrl)
	enum { IDD = IDD_PREV_CTRL };
	CButton	m_close;
	CString	m_count;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrevCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

   CToolTipCtrl ToolTipCtrl;

	// Generated message map functions
	//{{AFX_MSG(CPrevCtrl)
	afx_msg void OnBegin();
	afx_msg void OnEnd();
	afx_msg void OnNext();
	afx_msg void OnPrev();
	afx_msg void OnClose();
	afx_msg UINT OnGetDlgCode();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPrint();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPreviewPane window

class CPreviewPane : public CWnd
{
// Construction
public:
	CPreviewPane();

// Attributes
public:
   CDisp* pDisplay;   

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreviewPane)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPreviewPane();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPreviewPane)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CPreviewPane window

class CPreviewPaneHD : public CWnd
{
// Construction
public:
	CPreviewPaneHD();

// Attributes
public:

	// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreviewPane)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPreviewPaneHD();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPreviewPane)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CPreview window

class CPreview : public CWnd
{
// Construction
public:
	CPreview();
	//CPrevCtrl cdStatus;
	CPrevCtrl* pcdStatus;
	int PrevSetInfo(WPARAM wParam, LPARAM lParam);
	CPreviewPane cwPreviewPane;
	CPreviewPaneHD cwPreviewPaneHD;
	BOOL WindowsExist;	

public:
    CObArray coPreviewPageArray;
    int iCurrentHeight;
    int iCurrentWidth;
	int iCurrentPane;

	int iZoom;

    CRuler TopRuler;
    CRuler LeftRuler;
    CRuler CornerRuler;
    CRect cRectRuler;
    void SetRulerRanges();
    void SetcRectRuler(PPREVIEWINFO pPreviewInfo);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreview)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPreview();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPreview)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg LRESULT OnPrevCtrl(WPARAM wParam, LPARAM lParam);	
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CPreviewManager : public CObject
   {   
public:
   CPreviewManager();
   ~CPreviewManager();

   int PrevSetInfo(WPARAM wParam, LPARAM lParam);
   void CreateWindows(CPreview* pPrev);
   void SetPreviewActiveJob(int iJobID);     
   void CloseAll();
   int NumActive();
   void DeletePreviewJob(int iJobID, UINT uiRipSessionID, BOOL bIgnore_uiRipSessionID=FALSE);

   // High Definition Functions
   void CreateHDWindows(CPreview* pPrev, CPreviewPage*);

protected:
   CObArray coPreviewWinArray;      
   int iCurrentPreview;

private:
   int	DisplayHDPreview(HWND hPrevWnd, CPreviewPage*);

   HWND hPrevWnd;

   int iZoomForFit;
   int iZoom;
   };

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWPREVIEW_H__C51CC771_BB12_11D2_8DEA_00C04F7975DC__INCLUDED_)
