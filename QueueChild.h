/////////////////////////////////////////////////////////////////////////////
// QueueChild.h
//                                                                        
// SECWorksheet derivative for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __QUEUECHILD_H__
#define __QUEUECHILD_H__

/////////////////////////////////////////////////////////////////////////////
// COurDialogBar window

#include "OurSplitter.h"
#include "resource.h"

class COurDialogBar : public SECDialogBar
{
// Construction
public:
	COurDialogBar();

// Attributes
public:   
   BOOL IsCreated;
   CWnd* pcwndParent;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COurDialogBar)   
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COurDialogBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(COurDialogBar)
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define QUEUESTATUS_DOTIME_HOLD   0
#define QUEUESTATUS_DOTIME_PRINT  1

/////////////////////////////////////////////////////////////////////////////

class COurTrayIcon : public SECTrayIcon
   {
public:
   COurTrayIcon(){}
   ~COurTrayIcon(){}
   void SetAllTips(const CString& sTip);
   CString csTrayTitle;   
protected:
   CString csTrayMsg;
   };

class CQueueChild : public SECWorksheet
{
	DECLARE_DYNCREATE(CQueueChild)
protected:
	CQueueChild();           // protected constructor used by dynamic creation
	int Last_flag;	

// Attributes
public:				 
   CWnd* pHoldWnd;
   CWnd* pPrintWnd;
   CWnd* pArchiveWnd;
   void StartTrayControl();
   void StopTrayControl();
   void SetTrayTitle(LPCSTR csTitle);   

protected:
   void ActivateFrame(int nCmdShow);   
	COurTrayIcon m_TrayStatus;		   

// Operations
public:					   
   COurSplitter m_splitter;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQueueChild)
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);	   
	//}}AFX_VIRTUAL

public:
   void SetStatus1(int StatusTextID, LPCSTR p);
   void SetStatus2(int StatusPromptID, LPCSTR p);      
   void SetProgress(int StatusProgressID, int Percent); 
   void SetTimeString(CString& cs, int iType, BOOL bClear = FALSE);
   void PSOut(BYTE* str, int len);
   void SetSelected(int NumSelected, int Total);
   void ClearStatus(int StatusTextID, int StatusPromptID, int StatusProgressID);
   void SetProcessStatus(int StatusPromptID, int StatusProgressID, int flag, int Data1 = 0, int Data2 = 0, int Data3 = 0, int Data4 = 0);
   void SetProcessStatus(int StatusPromptID, int StatusProgressID, char* pText);
   UINT SetQueueStatus(UINT uiStatus);
   void RippleProgress(int StatusProgressID);   
   void SetServerFlag(int flag);
   CEdit* GetPSOutEdit();
   BOOL DoesPSHaveSelText();
   BOOL IsPSOutEmpty();
   BOOL bPSoutState;
   BOOL bQStatusState;
   
protected:
   CString csFmt;
   int RipplePercent;
   BOOL SetInitialSize;
   CString csStatusPrompt[14];
   UINT QueueStatus;
   COurDialogBar m_wndQueueStatusTop;
   COurDialogBar m_wndQueueStatusBottom;
   COurDialogBar m_wndPSOUT;

// Implementation
protected:
	virtual ~CQueueChild();

	// Generated message map functions
	//{{AFX_MSG(CQueueChild)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	
	afx_msg LRESULT OnOurResize(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define STATUS_READING           0
#define STATUS_SCANLINE          1
#define STATUS_RENDERING         2
#define STATUS_PRINTING			 3
#define STATUS_PRINTING_RED      4
#define STATUS_PRINTING_GREEN    5
#define STATUS_PRINTING_BLUE     6
#define STATUS_PRINTING_CYAN     7
#define STATUS_PRINTING_YELLOW   8
#define STATUS_PRINTING_MAGENTA  9
#define STATUS_PRINTING_BLACK    10
#define STATUS_PRINTING_WHITE    11
#define STATUS_PRINTING_IMAGE    12
#define STATUS_READING_TOTAL     13
#define STATUS_PRINTING_TP       14

/////////////////////////////////////////////////////////////////////////////
#endif
