/////////////////////////////////////////////////////////////////////////////
// PREVIEW.H
//                                                                        
// preview class (derived from MDIChild) for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __PREVIEW_H__
#define __PREVIEW_H__

#include "inidefs.h"  
#include "prev.h"  
#include "disp.h"
#include "profile.h"

class CPreviewMDI : public SECMDIChildWnd
{
   DECLARE_DYNCREATE(CPreviewMDI)
public:
   CPreviewMDI();			
// Attributes
public:                    
   PREVIEWINFO PreviewInfo;   
   BOOL DoPaint;      
   void PrevSetBuffer();
   CString Title;
   CString PreviewClassName;   
   CDisp cd;                
   BOOL *pExistsFlag;
   BOOL ExistsFlagDummy;
   
// Operations
public:
   virtual BOOL PreCreateWindow(CREATESTRUCT&);	
// Implementation
protected:
	virtual ~CPreviewMDI();
	BOOL m_iconic;	
	CBrush cb;

	// Generated message map functions
	//{{AFX_MSG(CPreviewMDI)
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CPreviewMDIArray : public CObject
{
public:         
   CPreviewMDIArray();
   ~CPreviewMDIArray();
   
   void CloseAll();
   int NumActive();
   int PrevSetInfo(WPARAM, LPARAM);
   void PrevDestroyed(WPARAM, LPARAM);
   void ShowPreview();   
   CString PreviewClassName;
   
   HINSTANCE hInstDLL;   
   CPreviewMDI* pCpMDI[IDDM_MAX_PREVNUM];
   BOOL PreviewExists[IDDM_MAX_PREVNUM];
   int PreviewIdx;  
   int MaxOpenPreviews;
protected:
   CProfile Profile;
   BOOL PauseFlag;
};

/////////////////////////////////////////////////////////////////////////////
#endif

