/////////////////////////////////////////////////////////////////////////////
// NEWFORMV.H
//                                                                        
// main queue view for RasterPlus95
// Copyright (C) 1996-98 Graphx, Inc.
//

#ifndef __NEWFORMV_H__
#define __NEWFORMV_H__

#include "afxole.h"
#include "mainfrm.h"
#include "fileinfo.h" 
#include "qprefs.h" 
#include "cqueue.h"
#include "bheader.h"
#include "device.h"
#include "transfrm.h"
#include "report.h"
#include "crip.h"
#include <afxcview.h>
#include "QueueChild.h"
#include "plist.h"

// 
//save this here because class wizard nukes it everytime!
//afx_msg BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
//

#define NUMQUEUECOLUMNS       3

class COleListBoxDropTarget : public COleDropTarget
{
// Construction
public:
    COleListBoxDropTarget();

// Implementation
public:
    virtual ~COleListBoxDropTarget();   
    
   //
   // These members MUST be overridden for an OLE drop target
   // See DRAG and DROP section of OLE classes reference
   //
   DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD 
                                                dwKeyState, CPoint point );
   DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD 
                                               dwKeyState, CPoint point );
   void OnDragLeave(CWnd* pWnd);               
   
   BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT 
                                          dropEffect, CPoint point );
protected:
   DWORD dwScrollStart;
};

class CNewFormView : public CListView
{                                  
	DECLARE_DYNCREATE(CNewFormView)  
protected:
	 CPseudoListBox* pLB;	 
	 int m_iColumns;
	 int *m_iColWidth;	 

public:                          
     CFileInfo* pCfDlg;          
     void DestroyFileInfoDlg();
	 void CancelIt();
     void CreateFileInfoDlg();     
	 void AddServerFile(CQueueFile* pCQueueFile);
     int *pInstanceCntr;         
	 UINT uiOurListClipFormat;     
	 CInRipElement* pRipElement;
     
     void SetState(int);
     
     CString InitialFileDir;                     
     int OutputType;      // are we doing preview or print?     
     BOOL MoveFiles;      
     void UpdateInfo();          
     int SendFileToRIP(CQueueFile*);
     void Print();          
     BOOL RipComplete;
     CRip cr;       
     BOOL CancelFlag;                 
     int PauseFlag;              	 
     int InternalState;     
     CQueueFile* GetBestQueueFileForTransform();
     int DoBatchHeaderDialog(CBheader&, int, CQueueFile*);     
     void EditExistingBatchHeader(CQueueFile*);     
     void WriteTheCObArray();
	 void DeleteAllTempFiles();

protected:     
	 int GlobalRIPState;   
	 BOOL SetTransformTarget();
	 class CQueuePrefs* pPrefsDlg;
	 BOOL QueueFileWasConstructed;      
     CQueueFile* pCQueueFileTarget;   
	 CQueueFile* pActiveCQueueFile;
	 void RenumberList();
	 int QueueType;
	 CTime RipStartTime;
     CTimeSpan FileElapsedTime;     
     CString TimeString;	 	 	 	 

	 // report stuff
	 CReport Report;
	 void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo); 
     void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
     void OnPrint(CDC* pDC, CPrintInfo* pInfo);
     BOOL OnPreparePrinting(CPrintInfo* pInfo);
     void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);    

	 void StartServerPolling(BOOL FirstTime = FALSE);
	 void StopServerPolling();
	 void ProcessServerPolling();
	 BOOL ServerPolling;	 
	 DWORD LastTimeCheck;

	 UINT LastQueueStatus;

public:
	 BOOL AddColumn(
		LPCTSTR strItem,int nItem,int nSubItem = -1,int width = -1,
		int nMask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
		int nFmt = LVCFMT_LEFT);	 

	 virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	 virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);

public:
	void  SetDragSourceRect();
    CRect GetDragSourceRect();
    COleListBoxDropTarget m_DropTarget;
	void InsertDroppedQueueFiles(CPoint& point);

private:
    COleDataSource m_COleDataSource;
     
protected:     
     void PrintSel();     
     void OnContinuePrinting();     
	 void SetNoneTouched();
	 void CopyToClipboard(int fmt);

protected:
	 virtual void OnUpdate(CView*, LPARAM, CObject*);
	 virtual void OnInitialUpdate();	
	 virtual void OnCommit(); 	 
	 LRESULT OnChangeDevMode(WPARAM, LPARAM);	 	 
	 void RedrawItem(int);	 
	 void ResetPrompts();	 
	 void StartServer();

	 void OnOurFilePrintPreview();
	 void OnOurFilePrint();	 
	 void RestartServer();

	 void SetScreenSaver(BOOL bRestore);
	 BOOL bScreenSaverState;
	 
	 CQueueChild* pCQueueChild;

	 BOOL CompareAndSwapDescription(int pos);
	 BOOL CompareAndSwapName(int pos);
	 BOOL CompareAndSwapPrefs(int pos);
	 void SortDescription();
	 void SortName();
	 void SortPrefs();
	 BOOL QueueSortOrderUp[NUMQUEUECOLUMNS];

	 void OnTrackEnd(HWND hwndHeader, int iItem);

protected:
	CNewFormView();			// protected constructor used by dynamic creation

// Form Data
public:
	//{{AFX_DATA(CNewFormView)		
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

protected:
     BOOL CmdCancelActive,
          CmdAddFiles,
		  CmdQueuePreferences,
          CmdSkip,
          CmdPause,
          CmdResume,
          CmdFilePrint,
          CmdFilePrintSel,
          CmdFilePrintPreview,
          CmdFilePrintPreviewSel,
          CmdSelectAll,
          CmdDelete;
    
    DWORD FilterIdx;
	virtual ~CNewFormView();  	
    
// Implementation
protected:
	// Generated message map functions
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//{{AFX_MSG(CNewFormView)	
	afx_msg void OnAddfiles();
	afx_msg void OnResetStatus();
	afx_msg void OnDeletefiles();
	afx_msg void OnOk();
	afx_msg void OnCancel();
	afx_msg void ShowInfo();	
	afx_msg void OnSelectall();
	afx_msg void OnEditCopy();	
	afx_msg void OnEditDelete();
	afx_msg LRESULT OnRipComplete(WPARAM, LPARAM);	
	afx_msg LRESULT OnRipMessage(WPARAM, LPARAM);
	afx_msg LRESULT OnRipPSMessage(WPARAM, LPARAM);	
	afx_msg void OnPrintSel();
	afx_msg void OnPreviewSel();
	afx_msg void OnContinue();
	afx_msg void OnDeselectall();
	afx_msg void OnPause();		
	afx_msg void OnUpdateProcessCancelimaging(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditAddfiles(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProcessSkiptonextfile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProcessPauseimaging(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProcessResumeimaging(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintSel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintPreviewSel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintHDPreview(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintHDPreviewSel(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateResetStatus(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditSelectall(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePreferencesLocal(CCmdUI* pCmdUI);
	afx_msg void OnEditInsertBatchHeader();
	afx_msg void OnDestroy();
	afx_msg void OnEditWinsplitfile();
	afx_msg void OnUpdateEditWinsplitfile(CCmdUI* pCmdUI);
	afx_msg void OnPreferencesQueue();
	afx_msg void OnDropFiles(HDROP hDropInfo);	
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnToolsReportPrint();
	afx_msg void OnToolsReportPrintPreview();	
	afx_msg void OnEditCopyatts();
	afx_msg void OnUpdateEditCopyatts(CCmdUI* pCmdUI);
	afx_msg void OnEditPasteatts();
	afx_msg void OnUpdateEditPasteatts(CCmdUI* pCmdUI);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdatePreferencesQueue(CCmdUI* pCmdUI);	
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);		
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

// process states
#define STATE_IDLE_NO_FILES         1
#define STATE_IDLE_SOME_FILES       2
#define STATE_PRINTING              3
#define STATE_PAUSED                4
#define STATE_IDLE_RIP_NOTAVAIL     5

// queue types
#define QUEUETYPE_SELECTED      0
#define QUEUETYPE_ALL           1

#endif
