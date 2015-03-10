/////////////////////////////////////////////////////////////////////////////
// ArchiveTree.CPP
//                                                                        
// archive queue display/op class for RasterPlus95
// Copyright (C) 1999 Graphx, Inc.
//

#include "stdafx.h"
#include "zeus.h"
#include "ArchiveTree.h"
#include "zeusdoc.h"
#include "inidefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT NEAR wm_AddToPane = RegisterWindowMessage( RIP_ADDTOPANE );

#define QUEUEDEVICE    PDOC->QueueDevice
#define QUEUETRANSFORM PDOC->QueueTransform
#define QUEUECOLOR     PDOC->QueueColor

/////////////////////////////////////////////////////////////////////////////
// CArchiveTree

IMPLEMENT_DYNCREATE(CArchiveTree, CSpecialTree)

CArchiveTree::CArchiveTree()
   {   
   csTreeTitle = "Archive";
   iViewStyle = QUEUESTYLE_ARCHIVE;

   HeaderOffset = 18;
   m_cyHeader = SECLISTCTRL_HDRHEIGHT_CLISTCTRL + HeaderOffset;
   }

CArchiveTree::~CArchiveTree()
   {
   }

BEGIN_MESSAGE_MAP(CArchiveTree, CSpecialTree)
	//{{AFX_MSG_MAP(CArchiveTree)
	ON_WM_CREATE()
	ON_WM_DROPFILES()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_COMMAND(ID_OUR_FILE_PRINT, OnOurFilePrint)
	ON_COMMAND(ID_OUR_FILE_PRINT_SEL, OnOurFilePrintSel)
	ON_UPDATE_COMMAND_UI(ID_OUR_FILE_PRINT, OnUpdateOurFilePrint)
	ON_UPDATE_COMMAND_UI(ID_OUR_FILE_PRINT_SEL, OnUpdateOurFilePrintSel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArchiveTree drawing

void CArchiveTree::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CArchiveTree diagnostics

#ifdef _DEBUG
void CArchiveTree::AssertValid() const
{
	CSpecialTree::AssertValid();
}

void CArchiveTree::Dump(CDumpContext& dc) const
{
	CSpecialTree::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CArchiveTree message handlers

int CArchiveTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSpecialTree::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CArchiveTree::OnInitialUpdate() 
   {
   int i;
   CQueueFile* pCQueueFile;

   // set the header titles
   SetHeader();

   // insert pointers
   for (i = 0; i < PDOC->CQ_Archive.GetSize(); i++)
      {      
	  pCQueueFile = (CQueueFile*)(PDOC->CQ_Archive[i]);
	  InsertString(i, pCQueueFile);
      }         

   // PDOC->ArchiveTreeHeight is < 0 if uninitialized
   // if iTotalHeight is 0, don't do anything
   if (PDOC->ArchiveTreeHeight < 0)
	  {
	  int iTotalHeight = GetAvailHeight();
	  if (iTotalHeight > 0)
		 {
	     PDOC->ArchiveTreeHeight = iTotalHeight / 3;
//	     ((CSplitterWnd*)GetParent())->SetRowInfo(iViewStyle, PDOC->ArchiveTreeHeight, 0);
		 }
	  }
   else
	  {
//	  ((CSplitterWnd*)GetParent())->SetRowInfo(iViewStyle, PDOC->ArchiveTreeHeight, 0);
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
   pQueueArray = &PDOC->CQ_Archive;

   // set pointer to this window
   ((CQueueChild*)GetParentFrame())->pArchiveWnd = this;
   }

void CArchiveTree::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   CSpecialTree::BeginDrag(pNMHDR, pResult);
   }

BOOL CArchiveTree::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
   {
   return CSpecialTree::OnDrop(pDataObject, dropEffect, point, iViewStyle);
   }

void CArchiveTree::OnDropFiles(HDROP hDropInfo) 
   {
   CSpecialTree::OnDropFiles(hDropInfo, iViewStyle);
   }   

void CArchiveTree::OnOurFilePrintSel() 
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
   
   // get the selected items
   for (nArrIdx = pArrSelected->GetUpperBound(); nArrIdx >= 0; --nArrIdx)
      {                               
	  // get the index
	  dwIndex = pArrSelected->GetAt(nArrIdx);
	  // get the item
	  pCQueueFile = (CQueueFile*)GetItemData(GetItemAt(dwIndex));  
	  // add to print queue
	  ADDTOPANESTRUCT AddToPaneStruct;
	  AddToPaneStruct.iViewStyle = QUEUESTYLE_PRINT;	  
      AddToPaneStruct.bDoSubPage = FALSE;
	  AddToPaneStruct.iSubPage = 0;
	  ::SendMessage(((CQueueChild*)GetParentFrame())->pPrintWnd->GetSafeHwnd(), 
		 wm_AddToPane, (WPARAM)&AddToPaneStruct, (LPARAM)pCQueueFile);   	  
	  }

   // our doc is modified   
   ((CZeusDoc*)GetDocument())->SetModifiedFlag();                          
   // if the info box is on, update it
   UpdateInfo();
   // seem to need this to force scroll bars
   ReMeasureAllItems();					
   Invalidate();   
   }

void CArchiveTree::OnOurFilePrint()
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
	  }

   // our doc is modified   
   ((CZeusDoc*)GetDocument())->SetModifiedFlag();                          
   // if the info box is on, update it
   UpdateInfo();			  
   // seem to need this to force scroll bars
   ReMeasureAllItems();					
   Invalidate();
   }

void CArchiveTree::OnUpdateOurFilePrint(CCmdUI* pCmdUI) 
   {
   pCmdUI->Enable(CmdFilePrint && GetCount() > 0);   
   }

void CArchiveTree::OnUpdateOurFilePrintSel(CCmdUI* pCmdUI) 
   {
   pCmdUI->Enable(CmdFilePrintSel && GetSelectedCount() > 0);
   }