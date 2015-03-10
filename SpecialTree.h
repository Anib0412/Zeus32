/////////////////////////////////////////////////////////////////////////////
// SpecialTree.h
//                                                                        
// base tree display/op class for RasterPlus
// Copyright (C) 1999 Graphx, Inc.
//

#if !defined(AFX_SPECIALTREE_H__22AFFB7F_C1C4_11D2_8DF5_00C04F7975DC__INCLUDED_)
#define AFX_SPECIALTREE_H__22AFFB7F_C1C4_11D2_8DF5_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpecialTree.h : header file
//

#include "cqueue.h"
#include "crip.h"
#include "QueueChild.h"
#include "PropertyFrame.h"	
#include "mainfrm.h"
#include "bheader.h"
#include "RPTreeBase.h"
#include "PrintPaneState.h"

// process states
#define STATE_IDLE_NO_FILES         1
#define STATE_IDLE_SOME_FILES       2
#define STATE_PRINTING              3
#define STATE_PAUSED                4
#define STATE_IDLE_RIP_NOTAVAIL     5
#define STATE_PREVIEWING            6

// selection state
#define QUEUETYPE_SELECTED      0
#define QUEUETYPE_ALL           1

// tree type
#define NUMQUEUEPANES           3

#define QUEUESTYLE_HOLD         0
#define QUEUESTYLE_PRINT        1
#define QUEUESTYLE_ARCHIVE      2

#define NUMQUEUECOLUMNS         7

#define QUEUECOL_DESCRIPTION    0
#define QUEUECOL_FILENAME		1
#define QUEUECOL_OWNER			2
#define QUEUECOL_SIZE			3
#define QUEUECOL_PROPS			4
#define QUEUECOL_SUBMITTED		5
#define QUEUECOL_STATUS			6

/////////////////////////////////////////////////////////////////////////////
// CSpecialTree view

class CSpecialTree : public CRPTreeBase
{
protected:
	CSpecialTree();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSpecialTree)

// Attributes
public:
   void RenumberList();   
   void DeleteFiles(); 
   void OnDropFiles(HDROP hDropInfo, WORD InQueue);
   void BeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
   BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, 
	  CPoint point, WORD InQueue);   
   void InitialUpdate();
   void SetNoneTouched();
   void Print();   
   CRip cr;      
   int MapStatusImageState(CQueueFile* pCQueueFile);
   
   int StatusTextID;
   int StatusPromptID;
   int StatusProgressID;

   CQueueChild* pCQueueChild;	 
   CJobPropsSheet* pJopProps;    
   void CreateJobPropsDlg();
   void DestroyJobPropsDlg();
   int *pInstanceCntr;
   BOOL bPrintServerStarted;
   int GetAvailHeight();
   BOOL bResizeHack;
   CPrintPaneState* pPrintPaneState;   

   void StartServerPolling(BOOL FirstTime = FALSE);
   void StopServerPolling();
   void ProcessServerPolling();
   BOOL ServerPolling;
   void ResetPrompts();
   CObArray* pQueueArray;
   CPaneElement* pPaneElement;   

   BOOL RipComplete;
   void SetScreenSaver(BOOL bRestore);
   BOOL bScreenSaverState;
   void UpdateInfo();
   void RestartServer();
   void StartServer();

   void SortCol(int DrawHeaderPushCol);
   BOOL CompareAndSwap(int pos, int DrawHeaderPushCol);
   BOOL QueueSortOrderUp[NUMQUEUECOLUMNS];

   int CheckImageSize(RECT rImageRect);

   CPropertyFrame* m_pPropFrame;
   CJobPropsSheet* m_pModalPropFrame;
   void SetMultiSelFields(CDWordArray *pArrSelected);
   void UpdateDeviceDlg();
   void UpdateSpecificDeviceItem(CDevice* cdFrom, CQueueFile* pCQueueFile, 
	  DEVCHANGE* pDevChange);
   void UpdateSpecificColorItem(PCPINITSTRUCT ctFrom, PCPINITSTRUCT ctTo, int Item);
   void UpdateSpecificXFormItem(CTransform* ctFrom, CTransform* ctTo, int Item);
   void AddToQueue(CQueueFile* pCQueueFile);    
   void NotifyViewClose(); 
   CEdit* EditLabel(HTREEITEM hti, int nCol);
   BOOL bSentStartToLog;
   void WriteLog();
   void ResetProps(BOOL bDoAll = FALSE);
   int AnyJobProps();
   BOOL InsertQueueFile(CString& csFile, int& idx);

private:

   CString csRPDebugStr;
   char cDebugStr[255];

   void ReinstallAppDriver( BOOL bAutoPrompt = FALSE );

protected:
   void InsertString(int i, CQueueFile* pCQueueFile);   
   void SetHeader();
   CImageList m_ImageList;   
   COleDropTarget m_OleDropTarget;
   HTREEITEM m_hItemDragged;
   DWORD dwScrollStart;
   UINT uiOurListClipFormat;     
   void CopyToClipboard(int fmt);
   void InsertDroppedQueueFiles(CPoint& point, WORD InQueue);
   BOOL IsSelected(int idx);
   void SetState(int State);   
   void OnContinuePrinting();
   void AddFolder(CString& csFile, int& idx);    
   void RestoreOldJobIDs();
     // command states
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
          CmdFilePrintHDPreview,
          CmdFilePrintHDPreviewSel,
          CmdSelectAll,
          CmdDelete;   

   int InternalState;
   int OutputType;
   int QueueType;   
   BOOL CancelFlag;
   BOOL DoingDefaultProps;
   int PauseFlag;              	 
   DWORD LastTimeCheck;
   CTime RipStartTime;
   CTime LastPageTime;
   CTimeSpan FileElapsedTime;     
   CString TimeString;	
   CQueueFile* pActiveCQueueFile;
   PCPINITSTRUCT pCPInitStruct;
   int SendFileToRIP(CQueueFile* pCQueueFile, BOOL bDumpGangPrints=FALSE);   
   void AddToLog(CQueueFile* pCQueueFile);
   CString GetLogFileName();      
   int DoBatchHeaderDialog(CBheader&, int, CQueueFile*);     
   void EditExistingBatchHeader(CQueueFile*);     
   int iLastQueueStatus;
   char FirstDriveFromMask(ULONG unitmask);   

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecialTree)
	public:
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);	
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view			
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSpecialTree();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif	

	// Generated message map functions
protected:
	//{{AFX_MSG(CSpecialTree)
    afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditAddfiles();
	afx_msg void OnGetdispinfoTree(NMHDR* pNMHDR, LRESULT* pResult);		
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);					
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnRipComplete(WPARAM, LPARAM);	
	afx_msg LRESULT OnAddToPane(WPARAM, LPARAM);
	afx_msg LRESULT OnStartPolling(WPARAM, LPARAM);
	afx_msg LRESULT OnStopPolling(WPARAM, LPARAM);
	afx_msg LRESULT OnRipMessage(WPARAM, LPARAM);
	afx_msg LRESULT OnRipPSMessage(WPARAM, LPARAM);	
	afx_msg LRESULT OnDevModeChange(WPARAM, LPARAM);
	afx_msg LRESULT OnDevModeChangeDefault(WPARAM, LPARAM);
	afx_msg LRESULT OnDevInfoChange(WPARAM, LPARAM);	
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);		
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnEditCopyatts();
	afx_msg void OnUpdateEditCopyatts(CCmdUI* pCmdUI);
	afx_msg void OnEditPasteatts();
	afx_msg void OnUpdateEditPasteatts(CCmdUI* pCmdUI);
	afx_msg void OnEditResetstatus();
	afx_msg void OnUpdateEditResetstatus(CCmdUI* pCmdUI);
	afx_msg void OnPreferencesQueue();
	afx_msg void OnSetFocus(CWnd* pOldWnd);	
	afx_msg void OnToolsLog();
	afx_msg void OnUpdateToolsLog(CCmdUI* pCmdUI);
	afx_msg void OnToolsPrintlog();
	afx_msg void OnUpdateToolsPrintlog(CCmdUI* pCmdUI);	
	afx_msg void OnToolsResetlog();
	afx_msg void OnUpdateToolsResetlog(CCmdUI* pCmdUI);
	afx_msg void OnSelectall();
	afx_msg void OnEditCopy();	
	afx_msg void OnEditPaste();
	afx_msg void OnEditCut();
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);	
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditSelectall(CCmdUI* pCmdUI);
    afx_msg void OnEditInsertBatchHeader();
	afx_msg void OnUpdateEditAddbatchheader(CCmdUI* pCmdUI);
	afx_msg void OnToolsArchivelog();
	afx_msg void OnUpdateToolsArchivelog(CCmdUI* pCmdUI);
	afx_msg void OnEditResetprops();
	afx_msg void OnUpdateEditResetprops(CCmdUI* pCmdUI);	
	afx_msg LRESULT OnResizePane(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPreferencesLocal();	
	afx_msg void OnUpdatePreferencesLocal(CCmdUI* pCmdUI); 
    afx_msg void OnUpdatePreferencesQueue(CCmdUI* pCmdUI); 		
	afx_msg void OnFileReinstallprinter();
	afx_msg void OnFileAppdriverConfig();	
	afx_msg void OnUpdateEditJobdescription(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnToolsLicenseManager();
	afx_msg void OnUpdateToolsLicenseManager(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPECIALTREE_H__71166AA6_B277_11D2_8DD6_00C04F7975DC__INCLUDED_)
