/////////////////////////////////////////////////////////////////////////////
// HoldTree.CPP
//                                                                        
// hold queue display/op class for RasterPlus
// Copyright (C) 1999-2001 Graphx, Inc.
//
// mods:
//
//  1-13-00 Version 3.0.3 - printsel would add the files into the print queue in 
//                          reverse order - fixed
// 09-28-01 Version 5.0.0 - Added High_Defintion Preview support
// 10-13-01 Version 5.0.0 - Added Index printing capabilities
// 10-17-01 Version 5.0.0 - Modified to support Index Print Options
// 10-23-01 Version 5.0.0 - Fix problem cancelling PrintIndex
// 11-16-01 Version 5.0.0a - put in QUEUEDEVICE.LicenseManager() in OnInitialUpdate to 
//					         fix single processor bug
// 12-28-01 Version 5.0.1 - Fix bug in Index Print ALL - scratch folder not set
// 10-21-01 Version 5.1.0 - Add support for Index Printing to Hold Queue, Copies, and MediaSize
// 04-01-04 Version 5.2.4 - Index Print changing default queue property for media size - Fixed
//

#include "stdafx.h"
#include "zeus.h"
#include "HoldTree.h"
#include "zeusdoc.h"
#include "inidefs.h"
#include "ripstruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RP95_PRINT_TIMER_ID          199
#define RP95_PRINT_TIMER_TIMEOUT     1000   // 1 sec
#define RP95_SERVER_TIMER_TIMEOUT    5000   // 5 sec

const UINT NEAR wm_AddToPane = RegisterWindowMessage( RIP_ADDTOPANE );
const UINT NEAR wm_PausePreview = RegisterWindowMessage( "RP_PausePreview" );
const UINT NEAR wm_SetPreviewJob = RegisterWindowMessage( "RP_SetPreviewJob" );
const UINT NEAR wm_PrintJobID = RegisterWindowMessage( RIP_PRINTJOBID );

#define QUEUEDEVICE    PDOC->QueueDevice
#define QUEUETRANSFORM PDOC->QueueTransform
#define QUEUECOLOR     PDOC->QueueColor
/////////////////////////////////////////////////////////////////////////////
// CHoldTree

IMPLEMENT_DYNCREATE(CHoldTree, CSpecialTree)

CHoldTree::CHoldTree()
   {   
   csTreeTitle = "Hold";
   iViewStyle = QUEUESTYLE_HOLD;
   
   HeaderOffset = 18;
   m_cyHeader = SECLISTCTRL_HDRHEIGHT_CLISTCTRL + HeaderOffset;

   pPrintIndexDlg = NULL;
   }

CHoldTree::~CHoldTree()
   {
   }

BEGIN_MESSAGE_MAP(CHoldTree, CSpecialTree)
	//{{AFX_MSG_MAP(CHoldTree)
	ON_WM_CREATE()	
	ON_WM_DROPFILES()	
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)		
	ON_COMMAND(ID_FILE_PRINT_PREVIEW_SEL, OnFilePrintPreviewSel)	
	ON_COMMAND(ID_OUR_FILE_PRINT_PREVIEW, OnOurFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW_SEL_HD, OnFilePrintHDPreviewSel)	
	ON_COMMAND(ID_OUR_FILE_PRINT_PREVIEW_HD, OnOurFilePrintHDPreview)
	ON_COMMAND(ID_PROCESS_CANCELIMAGING, OnProcessCancelimaging)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_CANCELIMAGING, OnUpdateProcessCancelimaging)
	ON_COMMAND(ID_PROCESS_PAUSEIMAGING, OnProcessPauseimaging)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_PAUSEIMAGING, OnUpdateProcessPauseimaging)
	ON_COMMAND(ID_PROCESS_RESUMEIMAGING, OnProcessResumeimaging)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_RESUMEIMAGING, OnUpdateProcessResumeimaging)
	ON_COMMAND(ID_PROCESS_SKIPTONEXTFILE, OnProcessSkiptonextfile)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_SKIPTONEXTFILE, OnUpdateProcessSkiptonextfile)
	ON_COMMAND(ID_OUR_FILE_PRINT, OnOurFilePrint)
	ON_UPDATE_COMMAND_UI(ID_OUR_FILE_PRINT, OnUpdateOurFilePrint)
	ON_COMMAND(ID_OUR_FILE_PRINT_INDEX, OnOurFilePrintIndex)
	ON_UPDATE_COMMAND_UI(ID_OUR_FILE_PRINT_INDEX, OnUpdateOurFilePrint)
	ON_COMMAND(ID_OUR_FILE_PRINT_SEL_INDEX, OnOurFilePrintSelIndex)
	ON_UPDATE_COMMAND_UI(ID_OUR_FILE_PRINT_SEL_INDEX, OnUpdateOurFilePrint)
	ON_COMMAND(ID_OUR_FILE_PRINT_SEL, OnOurFilePrintSel)
	ON_UPDATE_COMMAND_UI(ID_OUR_FILE_PRINT_SEL, OnUpdateOurFilePrintSel)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW_SEL, OnUpdateFilePrintPreviewSel)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW_SEL_HD, OnUpdateFilePrintHDPreviewSel)
	ON_UPDATE_COMMAND_UI(ID_OUR_FILE_PRINT_PREVIEW, OnUpdateOurFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_OUR_FILE_PRINT_PREVIEW_HD, OnUpdateOurFilePrintHDPreview)
	ON_REGISTERED_MESSAGE(wm_PausePreview, OnPausePreview)	
	ON_REGISTERED_MESSAGE(wm_SetPreviewJob, OnSetPreviewJob)
	ON_REGISTERED_MESSAGE(wm_PrintJobID, OnPrintJobID)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHoldTree drawing

void CHoldTree::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CHoldTree diagnostics

#ifdef _DEBUG
void CHoldTree::AssertValid() const
{
	CSpecialTree::AssertValid();
}

void CHoldTree::Dump(CDumpContext& dc) const
{
	CSpecialTree::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHoldTree message handlers

int CHoldTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSpecialTree::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CHoldTree::OnInitialUpdate() 
   {
   int i;
   CQueueFile* pCQueueFile;

   // set the header titles
   SetHeader();

   // insert pointers
   for (i = 0; i < PDOC->CQ_Hold.GetSize(); i++)
      {      
	  pCQueueFile = (CQueueFile*)(PDOC->CQ_Hold[i]);
	  InsertString(i, pCQueueFile);	  
      }         
   
   // get pointer to child frame
   pCQueueChild = (CQueueChild*)GetParentFrame();

   // set status ids
   StatusTextID = IDC_PREVIEW_STATUS_TEXT;
   StatusProgressID = IDC_PREVIEW_STATUS_PROGRESS;
   StatusPromptID = IDC_PREVIEW_STATUS_PROMPT;

   // PDOC->HoldTreeHeight is < 0 if uninitialized
   // if iTotalHeight is 0, don't do anything
   if (PDOC->HoldTreeHeight < 0)
	  {
	  int iTotalHeight = GetAvailHeight();
	  if (iTotalHeight > 0)
		 {
	     PDOC->HoldTreeHeight = iTotalHeight / 3;
	     ((CSplitterWnd*)GetParent())->SetRowInfo(iViewStyle, PDOC->HoldTreeHeight, 0);
		 }
	  }
   else
	  {
	  ((CSplitterWnd*)GetParent())->SetRowInfo(iViewStyle, PDOC->HoldTreeHeight, 0);	  
	  }

   ((CSplitterWnd*)GetParent())->RecalcLayout();
   PDOC->SetModifiedFlag();	  

   InitialUpdate();
   // seem to need this to force scroll bars
   ReMeasureAllItems();					
   Invalidate();

   // we can drop to this target
   m_OleDropTarget.Register( this );

   // set pointer to this array      
   pQueueArray = &PDOC->CQ_Hold;

   // set pointer to this window
   ((CQueueChild*)GetParentFrame())->pHoldWnd = this;

   // start the jaws rip
   //cr.StartJaws();

   // make sure that the QUEUEDEVICE is loaded to force
   // TimeCheck mem to get initialized
   QUEUEDEVICE.LoadDevice(QUEUEDEVICE.DevIdx);
   }

void CHoldTree::OnDropFiles(HDROP hDropInfo) 
   {
   CSpecialTree::OnDropFiles(hDropInfo, iViewStyle);
   }

void CHoldTree::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   CSpecialTree::BeginDrag(pNMHDR, pResult);
   }

BOOL CHoldTree::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
   {
   return CSpecialTree::OnDrop(pDataObject, dropEffect, point, iViewStyle);
   }

void CHoldTree::OnFilePrintPreviewSel() 
   {
   // if no files selected, return
   if (GetSelectedCount() < 1) return;   

   // none were touched!
   SetNoneTouched();   
   
   // set output type
   OutputType = RIPSTYLE_PREVIEW;

   // rip selected
   QueueType = QUEUETYPE_SELECTED;   

   // do it
   Print();
   }

void CHoldTree::OnFilePrintHDPreviewSel() 
{
	// if no files selected, return
	if (GetSelectedCount() < 1)
		return;

	// none were touched!
	SetNoneTouched();   

	// set output type
	OutputType = RIPSTYLE_HD_PREVIEW;

	// rip selected
	QueueType = QUEUETYPE_SELECTED;   

	// do it
	Print();
}

void CHoldTree::OnOurFilePrintPreview() 
   {                          
   // if no files in queue, return
   if (GetSelectedCount() < 1) return;   

   // none were touched!
   SetNoneTouched();        
   
   // set output type
   OutputType = RIPSTYLE_PREVIEW;

   // rip ALL
   QueueType = QUEUETYPE_ALL;   
   
   Print();
   }

void CHoldTree::OnOurFilePrintHDPreview() 
{                          
	// if no files in queue, return
	if (GetSelectedCount() < 1)
		return;   

	// none were touched!
	SetNoneTouched();        

	// set output type
	OutputType = RIPSTYLE_HD_PREVIEW;

	// rip ALL
	QueueType = QUEUETYPE_ALL;   

	Print();
}

void CHoldTree::OnProcessCancelimaging() 
   {   
   // set CancelFlag
   CancelFlag = TRUE;
   
   if (cr.IsInit) 
      {
	  cr.CancelProcess(TRUE);
	  }
   }   

void CHoldTree::OnUpdateProcessCancelimaging(CCmdUI* pCmdUI) 
   {					   
   pCmdUI->Enable(CmdCancelActive);	
   }

void CHoldTree::OnProcessPauseimaging() 
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

void CHoldTree::OnUpdateProcessPauseimaging(CCmdUI* pCmdUI) 
   {
   pCmdUI->Enable(CmdPause);
   }

void CHoldTree::OnProcessResumeimaging() 
   {
   // set flag
   PauseFlag = FALSE;         
   // set buttons
   SetState(STATE_PREVIEWING);
   // call the CRip Pause method which will 
   // suspend the rip thread
   cr.ResumeProcess();	  
   // reset status
   pCQueueChild->SetQueueStatus(iLastQueueStatus);	  
   }

void CHoldTree::OnUpdateProcessResumeimaging(CCmdUI* pCmdUI) 
   {
   pCmdUI->Enable(CmdResume);
   }

void CHoldTree::OnProcessSkiptonextfile() 
   {
   if (cr.IsInit) cr.CancelProcess(FALSE);
   }

void CHoldTree::OnUpdateProcessSkiptonextfile(CCmdUI* pCmdUI) 
   {
   pCmdUI->Enable(CmdSkip);
   }

void CHoldTree::OnOurFilePrintSel() 
   {
   int nArrIdx, NumSelected;   
   CQueueFile* pCQueueFile;   
   CDWordArray *pArrSelected = NULL;
   DWORD dwIndex;   

   // get selection array
   pArrSelected = GetSelectionArray();

   // how many are selected?
   NumSelected = pArrSelected->GetSize();   
    
   // if we have no items, return
   if (NumSelected < 1) return;              

   // add file jobs
   for (nArrIdx = 0; nArrIdx <= pArrSelected->GetUpperBound(); nArrIdx++)
      {                               
	  // get the index
	  dwIndex = pArrSelected->GetAt(nArrIdx);
	  // get the item
	  pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(dwIndex));  
	  ADDTOPANESTRUCT AddToPaneStruct;
	  AddToPaneStruct.iViewStyle = QUEUESTYLE_PRINT;	  
      AddToPaneStruct.bDoSubPage = FALSE;
	  AddToPaneStruct.iSubPage = 0;
	  // add to print queue
	  ::SendMessage(((CQueueChild*)GetParentFrame())->pPrintWnd->GetSafeHwnd(), 
		 wm_AddToPane, (WPARAM)&AddToPaneStruct, (LPARAM)pCQueueFile);   	  
  	  }

   // delete the jobs
   for (nArrIdx = pArrSelected->GetUpperBound(); nArrIdx >= 0; --nArrIdx)
      {                               
	  // get the index
	  dwIndex = pArrSelected->GetAt(nArrIdx);
	  // get the item
	  pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(dwIndex));  
	  // delete the item from the list
      DeleteItem(GetItemAt(dwIndex));
	  // delete from the array
      pQueueArray->RemoveAt(dwIndex);
	  // delete the CQueue object
	  delete pCQueueFile;
 	  // make sure the numbering is straight
	  RenumberList();			 
	  }

   // our doc is modified   
   ((CZeusDoc*)GetDocument())->SetModifiedFlag();                          
   // if the info box is on, update it
   UpdateInfo();			  
   }

void CHoldTree::OnOurFilePrint()
   {
   int nItems;
   CQueueFile* pCQueueFile;      
   int i;

   // if we have no items, return
   if ((nItems = GetCount()) < 1) return;              
   
   // get the selected items
   for (i = 0; i < nItems; i++)
      {                               
	  // get the item
	  // note: we always get the 0 based item because we are removing
	  // them from the top down
	  pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(0));  
	  // add to print queue
	  ADDTOPANESTRUCT AddToPaneStruct;
	  AddToPaneStruct.iViewStyle = QUEUESTYLE_PRINT;	  
      AddToPaneStruct.bDoSubPage = FALSE;
	  AddToPaneStruct.iSubPage = 0;
	  ::SendMessage(((CQueueChild*)GetParentFrame())->pPrintWnd->GetSafeHwnd(), 
		 wm_AddToPane, (WPARAM)&AddToPaneStruct, (LPARAM)pCQueueFile);   	  
  	  // delete the item from the list
      DeleteItem(GetItemAt(0));
	  // delete from the array
      pQueueArray->RemoveAt(0);
	  // delete the CQueue object
	  delete pCQueueFile;
 	  // make sure the numbering is straight
	  RenumberList();			 
	  }

   // our doc is modified   
   ((CZeusDoc*)GetDocument())->SetModifiedFlag();                          
   // if the info box is on, update it
   UpdateInfo();			  
   }

void CHoldTree::OnOurFilePrintSelIndex()
{
	CString csFileName;

	int iQCount = 0;
	int iTotalPages = 0;

	CDWordArray *pArrSelected = NULL;

	CQueueFile* pCQueueFile;      

	DWORD dwIndex;   

	int nArrIdx, NumSelected;
	int iFileIndex = 0;
	int iFilesCreated;

	// get selection array
	pArrSelected = GetSelectionArray();

	// how many are selected?
	NumSelected = pArrSelected->GetSize();   

	// if we have no items, return
	if (NumSelected < 1)
		return;

	if (!(pPrintIndexDlg = new CPrintIndexDlg))
	   return;

	// set scratch path
	pPrintIndexDlg->csScratchPath = PDOC->crrrs.csScratchPath;

	for (nArrIdx = 0; nArrIdx <= pArrSelected->GetUpperBound(); nArrIdx++)
	{           
		// get the index
		dwIndex = pArrSelected->GetAt(nArrIdx);
		// get the item
		pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(dwIndex));  

		pPrintIndexDlg->AddImage(pCQueueFile);
	}

	// our doc is modified   
	((CZeusDoc*)GetDocument())->SetModifiedFlag();

	// if the info box is on, update it
	UpdateInfo();			  

	pPrintIndexDlg->SetPrintHWnd( ((CQueueChild*)GetParentFrame())->pPrintWnd->GetSafeHwnd() );

	pPrintIndexDlg->SetPrintIndexInfo((CQueueChild*)GetParentFrame(), &QUEUEDEVICE, &QUEUETRANSFORM, &QUEUECOLOR );

	if ( pPrintIndexDlg->DoModal() == IDOK )
	{
		iFilesCreated = pPrintIndexDlg->GetFileCount();

		for ( iFileIndex = 0; iFileIndex < iFilesCreated; iFileIndex++ )
		{
			pCQueueFile = new CQueueFile;

			pCQueueFile->pCD = new CDevice;
			*(pCQueueFile->pCD) = QUEUEDEVICE;
			pCQueueFile->Prefs |= USE_LOCAL_DEV_PREFS;

			csFileName = pPrintIndexDlg->GetIndexFileName(iFileIndex);
			pCQueueFile->Init(csFileName, iTotalPages, 0);
			pCQueueFile->ExtraInfo = CQUEUE_EXTRA_INDEXFILE;
			pCQueueFile->pCD->pDevInfo->Copies = pPrintIndexDlg->m_Index_Copies;
			pCQueueFile->pCD->pDevInfo->CurrentMediaIdx = pPrintIndexDlg->m_Media_Size;
			pCQueueFile->pCD->pDevInfo->Orientation = pPrintIndexDlg->m_Orientation;

			if ( pPrintIndexDlg->m_Select_Pane == 0)
			{
						 // not touched     
				pCQueueFile->WasTouched = FALSE;
				// in what queue
				pCQueueFile->InQueue = 0;
				pCQueueFile->iPrintToHold = 0;
				pCQueueFile->iJobID = ((CZeusApp*)AfxGetApp())->GetJobID();

				pCQueueFile->Status.LoadString(IDS_READY);
				pCQueueFile->PreviewStatus.LoadString(IDS_READY);
				pCQueueFile->StatusFlag |= FILE_STATUS_HOLD;
				pCQueueFile->ctsElapsedTime = 0;

				iQCount = GetCount();

				// store it            
				pQueueArray->InsertAt(iQCount, pCQueueFile);	    

				InsertString(iQCount++, pCQueueFile);	  
			}
			else
			{
				ADDTOPANESTRUCT AddToPaneStruct;
				AddToPaneStruct.iViewStyle = QUEUESTYLE_PRINT;	  
				AddToPaneStruct.bDoSubPage = FALSE;
				AddToPaneStruct.iSubPage = 0;
				::SendMessage(((CQueueChild*)GetParentFrame())->pPrintWnd->GetSafeHwnd(), 
												 wm_AddToPane, (WPARAM)&AddToPaneStruct, (LPARAM)pCQueueFile);

				delete pCQueueFile;
			}
		}
	}

	if (pPrintIndexDlg->m_Select_Pane == 0)
	{
		Invalidate();       
		// if the info box is on, update it

		UpdateInfo();			  
		// renumber the list
		
		RenumberList();  
		// force a reshow
		
		ReMeasureAllItems();
		
		Invalidate();
	}

	delete pPrintIndexDlg;
	pPrintIndexDlg = NULL;
}

void CHoldTree::OnOurFilePrintIndex()
{
	CString csFileName;

	int iFileIndex = 0;
	int iFilesCreated;
	int iQCount = 0;
	int iTotalPages = 0;

	int nItems;
	CQueueFile* pCQueueFile;      
	int i;

	// if we have no items, return
	if ((nItems = GetCount()) < 1)
		return;

	pPrintIndexDlg = new CPrintIndexDlg;

	// set scratch path
	pPrintIndexDlg->csScratchPath = PDOC->crrrs.csScratchPath;

	// get the selected items
	for (i = 0; i < nItems; i++)
	{                           
		pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(i));

		pPrintIndexDlg->AddImage(pCQueueFile);
	}

	// our doc is modified   
	((CZeusDoc*)GetDocument())->SetModifiedFlag();

	// if the info box is on, update it
	UpdateInfo();

	pPrintIndexDlg->SetPrintHWnd( ((CQueueChild*)GetParentFrame())->pPrintWnd->GetSafeHwnd() );

	pPrintIndexDlg->SetPrintIndexInfo((CQueueChild*)GetParentFrame(), &QUEUEDEVICE, &QUEUETRANSFORM, &QUEUECOLOR );

	if ( pPrintIndexDlg->DoModal() == IDOK )
	{
		iFilesCreated = pPrintIndexDlg->GetFileCount();

		for ( iFileIndex = 0; iFileIndex < iFilesCreated; iFileIndex++ )
		{
			pCQueueFile = new CQueueFile;

			pCQueueFile->pCD = new CDevice;
			*(pCQueueFile->pCD) = QUEUEDEVICE;
			pCQueueFile->Prefs |= USE_LOCAL_DEV_PREFS;

			csFileName = pPrintIndexDlg->GetIndexFileName(iFileIndex);
			pCQueueFile->Init(csFileName, iTotalPages, 0);
			pCQueueFile->ExtraInfo = CQUEUE_EXTRA_INDEXFILE;
			pCQueueFile->pCD->pDevInfo->Copies = pPrintIndexDlg->m_Index_Copies;
			pCQueueFile->pCD->pDevInfo->CurrentMediaIdx = pPrintIndexDlg->m_Media_Size;

			if ( pPrintIndexDlg->m_Select_Pane == 0)
			{
						 // not touched     
				pCQueueFile->WasTouched = FALSE;
				// in what queue
				pCQueueFile->InQueue = 0;
				pCQueueFile->iPrintToHold = 0;
				pCQueueFile->iJobID = ((CZeusApp*)AfxGetApp())->GetJobID();

				pCQueueFile->Status.LoadString(IDS_READY);
				pCQueueFile->PreviewStatus.LoadString(IDS_READY);
				pCQueueFile->StatusFlag |= FILE_STATUS_HOLD;
				pCQueueFile->ctsElapsedTime = 0;


				iQCount = GetCount();

				// store it            
				pQueueArray->InsertAt(iQCount, pCQueueFile);	    

				InsertString(iQCount++, pCQueueFile);	  
			}
			else
			{
				ADDTOPANESTRUCT AddToPaneStruct;
				AddToPaneStruct.iViewStyle = QUEUESTYLE_PRINT;	  
				AddToPaneStruct.bDoSubPage = FALSE;
				AddToPaneStruct.iSubPage = 0;
				::SendMessage(((CQueueChild*)GetParentFrame())->pPrintWnd->GetSafeHwnd(), 
												 wm_AddToPane, (WPARAM)&AddToPaneStruct, (LPARAM)pCQueueFile);

				delete pCQueueFile;
			}
		}
	}

	if (pPrintIndexDlg->m_Select_Pane == 0)
	{
		Invalidate();       
		// if the info box is on, update it

		UpdateInfo();			  
		// renumber the list
		
		RenumberList();  
		// force a reshow
		
		ReMeasureAllItems();
		
		Invalidate();
	}

	delete pPrintIndexDlg;
	pPrintIndexDlg = NULL;
}

void CHoldTree::OnUpdateFilePrintPreviewSel(CCmdUI* pCmdUI) 
   {      
   pCmdUI->Enable(CmdFilePrintPreviewSel && GetSelectedCount() > 0);   
   }

void CHoldTree::OnUpdateFilePrintHDPreviewSel(CCmdUI* pCmdUI) 
   {      
   pCmdUI->Enable(CmdFilePrintHDPreviewSel && GetSelectedCount() > 0);   
   }

void CHoldTree::OnUpdateOurFilePrintPreview(CCmdUI* pCmdUI) 
   {   
   pCmdUI->Enable(CmdFilePrintPreview && GetCount() > 0);   
   }

void CHoldTree::OnUpdateOurFilePrintHDPreview(CCmdUI* pCmdUI) 
   {   
   pCmdUI->Enable(CmdFilePrintHDPreview && GetCount() > 0);   
   }

void CHoldTree::OnUpdateOurFilePrint(CCmdUI* pCmdUI) 
   {
   pCmdUI->Enable(CmdFilePrint && GetCount() > 0);   
   }

void CHoldTree::OnUpdateOurFilePrintSel(CCmdUI* pCmdUI) 
   {
   pCmdUI->Enable(CmdFilePrintSel && GetSelectedCount() > 0);
   }

LRESULT CHoldTree::OnPausePreview(WPARAM wParam, LPARAM lParam)
   {
   OnProcessPauseimaging();

   return 0L;
   }

LRESULT CHoldTree::OnSetPreviewJob(WPARAM wParam, LPARAM lParam)
   {
   int i;
   CQueueFile* pCQueueFile;

   // don't do this if we are ripping
   if (PDOC->InRip == TRUE) return 0L;

   // select no items   
   GetListCtrl().SelectItemRange(0, GetCount() - 1, FALSE);   

   // loop through all the jobs and highlight the proper one   
   for (i = 0; i < GetCount(); i++)      
	  {					   
	  pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(i));
	  // found it!
	  if (pCQueueFile->iJobID == (int)wParam)
		 {
		 SelectItem(GetItemAt(i));
		 }
	  }   
   return 0L;
   }

LRESULT CHoldTree::OnPrintJobID(WPARAM WParam, LPARAM LParam)
   {   
   int i;
   CQueueFile* pCQueueFile;
   int iJobID;
   int iPageNum;

  // get the job id
   iJobID = (int)WParam;
   // get the page
   iPageNum = (int)LParam;
   for (i = 0; i < GetCount(); i++)
	  {		
	  // get the queue item
	  pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(i));
	  // a match?
	  if (pCQueueFile->iJobID == iJobID)
		 {
		 ADDTOPANESTRUCT AddToPaneStruct;
		 AddToPaneStruct.iViewStyle = QUEUESTYLE_PRINT;	  
		 AddToPaneStruct.bDoSubPage = TRUE;
		 AddToPaneStruct.iSubPage = iPageNum;	  
		 // send it to the print pane
	     ::SendMessage(((CQueueChild*)GetParentFrame())->pPrintWnd->GetSafeHwnd(), 
		    wm_AddToPane, (WPARAM)&AddToPaneStruct, (LPARAM)pCQueueFile);   	  
		 break;
		 }			  	 
	  }
   return 0L;   
   }

