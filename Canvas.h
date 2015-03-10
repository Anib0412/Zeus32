/////////////////////////////////////////////////////////////////////////////
// CANVAS.H
//                                                                        
// visual transform preview window for RasterPlus95
// Copyright (C) 1996-97 Graphx, Inc.
//

#ifndef __CANVAS_H__
#define __CANVAS_H__

#include "disp.h"
#include "roundit.h"
#include "frect.h"

#define EXCLUDE_XZOOM     0
#define EXCLUDE_YZOOM     1
#define EXCLUDE_ANYZOOM   2
#define EXCLUDE_XOFF      3
#define EXCLUDE_YOFF      4
#define EXCLUDE_ROTATION  5
#define EXCLUDE_START	  6
#define EXCLUDE_WIDTH     7
#define EXCLUDE_HEIGHT    8
#define EXCLUDE_NONE      99

#define ROTATION_0        0
#define ROTATION_90       1
#define ROTATION_180      2
#define ROTATION_270      3
#define NUMROTATIONIDX    4   

class NewRectTracker : public CRectTracker
{
// Construction
public:
	NewRectTracker();
	NewRectTracker(LPCRECT, UINT);
	~NewRectTracker();                  
	void OnChangedRect(const CRect&);	
    
    void UpdateEditVars(int exclude = EXCLUDE_NONE);     

	POINT ptRotateCorrect;
	POINT OriginalCorrect;
	CFrect fTrackRect;

	float PicaWidth;
	float PicaHeight;
        
    // local float copies of rect for accuracy    
	float m_width;
	float m_height;
	float m_xoff;
	float m_xzoom;
	float m_yoff;
	float m_yzoom;
	float m_rotation;		
	float m_xOriginalZoom;	
	float m_OriginalWidth;
	float m_OriginalHeight;
	float m_yOriginalZoom;	
	CRect BoundingRect;      
	CRect m_rectOriginal;	
	CWnd* pWnd;

    RECT ClientRect;  	        		
	BOOL* m_pSnap;                      	
	BOOL* m_pRetain_aspect;			
public:
    virtual void AdjustRect(int, LPRECT);
};

/////////////////////////////////////////////////////////////////////////////
// CanvasWnd window

class CanvasWnd : public CWnd
{
// Construction
public:
	CanvasWnd();    
	NewRectTracker* pRT;    		
	BOOL m_snap;	    
	BOOL m_hatch;
	BOOL m_retain_aspect;
	BOOL m_show_preview;
	BOOL m_must_repaint;	

// Attributes
public:

// Operations
public:   
   void UpdateRectFromEdit(int idx, float fval = 0.0F);
   CDisp cd;   
   float PicaWidth;
   float PicaHeight;
   BOOL Exists;
   CRect cMediaRect;
      
// Implementation
public:
	virtual ~CanvasWnd();
	void SetSizingRect(CRect*, CRect*, CRect*, POINT*);
	int OnPrevSetInfo(WPARAM, LPARAM);

protected:		
	CRect DisplayRect;
	CFrect fDisplayRect;	
	BOOL DoPaint;         
	void SetLastRepaintRect();	
	CRect MaxRect;
	RoundIt rnd;
	RECT ZoomRect;	

	float LastXzoom;
    float LastYzoom;
    float LastLeft;
    float LastRight;
    float LastTop;
    float LastBottom;   

protected:
	// Generated message map functions
	//{{AFX_MSG(CanvasWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CanvasParentWnd window

class CanvasParentWnd : public CWnd
{
// Construction
public:
	CanvasParentWnd();

// Attributes
public:
   BOOL Exists;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CanvasParentWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CanvasParentWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CanvasParentWnd)
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
