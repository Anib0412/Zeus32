/////////////////////////////////////////////////////////////////////////////
// PrintTree.CPP
//                                                                        
// print queue display/op class for RasterPlus95
// Copyright (C) 1999 Graphx, Inc.
//

#include "stdafx.h"
#include "zeus.h"
#include "PrintTree.h"
#include "zeusdoc.h"
#include "inidefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define QUEUESERVER    PDOC->Server

/////////////////////////////////////////////////////////////////////////////
// CPrintTree

IMPLEMENT_DYNCREATE(CPrintTree, CSpecialTree)

CPrintTree::CPrintTree()
   {   
   csTreeTitle = "Print";
   iViewStyle = QUEUESTYLE_PRINT;

   HeaderOffset = 18;   
   m_cyHeader = SECLISTCTRL_HDRHEIGHT_CLISTCTRL + HeaderOffset;
   }

CPrintTree::~CPrintTree()
{
}


BEGIN_MESSAGE_MAP(CPrintTree, CSpecialTree)
	//{{AFX_MSG_MAP(CPrintTree)
	ON_WM_CREATE()	
	ON_WM_DROPFILES()	
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_COMMAND(ID_PROCESS_CANCELIMAGING, OnProcessCancelimaging)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_CANCELIMAGING, OnUpdateProcessCancelimaging)
	ON_COMMAND(ID_PROCESS_PAUSEIMAGING, OnProcessPauseimaging)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_PAUSEIMAGING, OnUpdateProcessPauseimaging)
	ON_COMMAND(ID_PROCESS_RESUMEIMAGING, OnProcessResumeimaging)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_RESUMEIMAGING, OnUpdateProcessResumeimaging)
	ON_COMMAND(ID_PROCESS_SKIPTONEXTFILE, OnProcessSkiptonextfile)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_SKIPTONEXTFILE, OnUpdateProcessSkiptonextfile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintTree drawing

void CPrintTree::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CPrintTree diagnostics

#ifdef _DEBUG
void CPrintTree::AssertValid() const
{
	CSpecialTree::AssertValid();
}

void CPrintTree::Dump(CDumpContext& dc) const
{
	CSpecialTree::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPrintTree message handlers

int CPrintTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSpecialTree::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CPrintTree::OnInitialUpdate() 
   {
   int i;
   CQueueFile* pCQueueFile;

   // set the header titles
   SetHeader();

   // insert pointers
   for (i = 0; i < PDOC->CQ.GetSize(); i++)
      {      
	  pCQueueFile = (CQueueFile*)(PDOC->CQ[i]);
	  InsertString(i, pCQueueFile);
      }                 				   

   // get pointer to child frame
   pCQueueChild = (CQueueChild*)GetParentFrame();

   // set status ids
   StatusTextID = IDC_PRINT_STATUS_TEXT;
   StatusProgressID = IDC_PRINT_STATUS_PROGRESS;
   StatusPromptID = IDC_PRINT_STATUS_PROMPT;

   // PDOC->PrintTreeHeight is < 0 if uninitialized
   // if iTotalHeight is 0, don't do anything
   if (PDOC->PrintTreeHeight < 0)
	  {
	  int iTotalHeight = GetAvailHeight();
	  if (iTotalHeight > 0)
		 {
	     PDOC->PrintTreeHeight = iTotalHeight / 3;
//	     ((CSplitterWnd*)GetParent())->SetRowInfo(iViewStyle, PDOC->PrintTreeHeight, 0);
		 }
	  }
   else
	  {
//	  ((CSplitterWnd*)GetParent())->SetRowInfo(iViewStyle, PDOC->PrintTreeHeight, 0);
	  }
 
   ((CSplitterWnd*)GetParent())->RecalcLayout();
   PDOC->SetModifiedFlag();	  

   InitialUpdate();
   // seem to need this to force scroll bars
   ReMeasureAllItems();					
   Invalidate();

   // we can drop to this target
   m_OleDropTarget.Register( this );

   //QUEUESERVER.m_status = STATUS_SERVER_ACTIVE;

   // and start the server!
   //StartServer();

   // set pointer to this array   
   pQueueArray = &PDOC->CQ;

   // set pointer to this window
   ((CQueueChild*)GetParentFrame())->pPrintWnd = this;

   // start the jaws rip
   //cr.StartJaws();
   }

void CPrintTree::OnDropFiles(HDROP hDropInfo) 
   {
   CSpecialTree::OnDropFiles(hDropInfo, iViewStyle);
   }

void CPrintTree::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   CSpecialTree::BeginDrag(pNMHDR, pResult);
   }

BOOL CPrintTree::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
   {
   return CSpecialTree::OnDrop(pDataObject, dropEffect, point, iViewStyle);
   }

void CPrintTree::OnProcessCancelimaging() 
   {   
   // set CancelFlag
   CancelFlag = TRUE;
   
   if (cr.IsInit) 
      {
	  cr.CancelProcess(TRUE);
	  }
   }   

void CPrintTree::OnUpdateProcessCancelimaging(CCmdUI* pCmdUI) 
   {					   
   pCmdUI->Enable(CmdCancelActive);	
   }

void CPrintTree::OnProcessPauseimaging() 
   {   
   // set flag
   PauseFlag = TRUE;         
   // set buttons
   SetState(STATE_PAUSED);            
   // call the CRip Pause method which will 
   // resume the rip thread
   cr.PauseProcess();
   // set status
   iLastQueueStatus = pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_PAUSED);
   }       

void CPrintTree::OnUpdateProcessPauseimaging(CCmdUI* pCmdUI) 
   {
   pCmdUI->Enable(CmdPause);
   }

void CPrintTree::OnProcessResumeimaging() 
   {
   // set flag
   PauseFlag = FALSE;         
   // set buttons
   SetState(STATE_PRINTING);            
   // call the CRip Pause method which will 
   // suspend the rip thread
   cr.ResumeProcess();	  
   // reset status
   pCQueueChild->SetQueueStatus(iLastQueueStatus);
   }

void CPrintTree::OnUpdateProcessResumeimaging(CCmdUI* pCmdUI) 
   {
   pCmdUI->Enable(CmdResume);
   }

void CPrintTree::OnProcessSkiptonextfile() 
   {
   if (cr.IsInit) cr.CancelProcess(FALSE);
   }

void CPrintTree::OnUpdateProcessSkiptonextfile(CCmdUI* pCmdUI) 
   {
   pCmdUI->Enable(CmdSkip);
   }