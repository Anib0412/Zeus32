/////////////////////////////////////////////////////////////////////////////
// RULER.H
//                                                                        
// ruler class for visual transform dialog for RasterPlus95
// Copyright (C) 1997 Graphx, Inc.
//

#ifndef __RULER_H__
#define __RULER_H__

#define RULER_STYLE_NONE     0x00 
#define RULER_STYLE_TOP      0x01
#define RULER_STYLE_LEFT     0x02
#define RULER_STYLE_CORNER   0x04

//#define RULER_HEIGHT 36
//#define MINOR_TICK_WIDTH RULER_HEIGHT * 5 / 8
//#define MAJOR_TICK_WIDTH RULER_HEIGHT / 2

//#define RULER_HEIGHT     48
#define RULER_HEIGHT     36
#define MINOR_TICK_WIDTH RULER_HEIGHT * 7 / 8
#define MAJOR_TICK_WIDTH RULER_HEIGHT * 6 / 8

/////////////////////////////////////////////////////////////////////////////
// CRuler window

class CRuler : public CWnd
{
// Construction
public:
	CRuler();
	void SetStyle(int st);
	void SetRange(int lo, int hi, int td, int ut);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRuler)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRuler();

	// Generated message map functions
protected:
   int style;
   CFont* pDlgFont;
   CPen penWindowText;
   void DrawTicksTop(CPaintDC& dc);
   void DrawTicksLeft(CPaintDC& dc);
   int LoWater, HiWater, TickDiv, UpTick;

	//{{AFX_MSG(CRuler)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
