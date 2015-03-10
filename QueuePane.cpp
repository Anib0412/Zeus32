// QueuePane.cpp : implementation file
//

#include "stdafx.h"
#include "zeus.h"
#include "QueuePane.h"
#include "objects.h"
#include "ZeusDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IMAGECALLBACK_CLOSEDFOLDER				   0
#define IMAGECALLBACK_OPENFOLDER				   1
#define IMAGECALLBACK_CLOSEDFOLDER_DEF			   2
#define IMAGECALLBACK_OPENFOLDER_DEF			   3
#define IMAGECALLBACK_FLOPPY					   4
#define IMAGECALLBACK_HARDDISK					   5
#define IMAGECALLBACK_NETWORK					   6
#define IMAGECALLBACK_NETWORK_DISCONNECTED		   7
#define IMAGECALLBACK_CDROM						   8 
#define IMAGECALLBACK_ORDER						   9
#define IMAGECALLBACK_ORDER_CD					   10
#define IMAGECALLBACK_PRODUCT					   11
#define IMAGECALLBACK_PRODUCT_DISCONNECTED		   12
#define IMAGECALLBACK_PRODUCT_WHOLESALE			   13
#define IMAGECALLBACK_ACCOUNTSLAVE_OPEN            14
#define IMAGECALLBACK_ACCOUNTSLAVE_CLOSED		   15
#define IMAGECALLBACK_ACCOUNTSLAVE_OPEN_DEF        16
#define IMAGECALLBACK_ACCOUNTSLAVE_CLOSED_DEF	   17

#define DRIVE_FLOPPY                         99 


/////////////////////////////////////////////////////////////////////////////
// CQueuePane

#ifdef LAB_STYLE
IMPLEMENT_DYNCREATE(CQueuePane, SECTreeView)
BEGIN_MESSAGE_MAP(CQueuePane, SECTreeView)
#else
IMPLEMENT_DYNCREATE(CQueuePane, CSpecialTree)
BEGIN_MESSAGE_MAP(CQueuePane, CSpecialTree)
#endif
	//{{AFX_MSG_MAP(CQueuePane)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_NOTIFY_REFLECT(TVN_GETDISPINFO, OnGetdispinfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CQueuePane::CQueuePane()
{
	bDataInit = FALSE;
}

CQueuePane::~CQueuePane()
{
}

void CQueuePane::AddNewNode( CDocument* pDoc )
{
	CAccount* pPrinterQueue;
	CNode* pParentProduct;
	HTREEITEM hItemOrderRoot;
	HTREEITEM hItemOrderArchiveRoot;
	HTREEITEM hItemProductRoot;

	CNode* pParentOrder;
	CNode* pParentOrderArchive;	   

	// Add Print Queues to the Tree
	pPrinterQueue = new CAccount;
	pPrinterQueue->iSubType = SUBTYPE_MASTERACCT;

	pPrinterQueue->csDisplayName = ((CZeusDoc*)pDoc)->csTitle;
	pPrinterQueue->csName = ((CZeusDoc*)pDoc)->csTitle;

	hItemPrtQueueRoot = InsertOurItem(pPrinterQueue, TVI_ROOT);

	//	coAccount.Add(pPrinterQueue);
	coNodes.Add(pPrinterQueue);

	if (pParentOrder = new CNode)
	{
#ifdef LAB_STYLE
		pParentOrder->csDisplayName = "Print";
#else
		pParentOrder->csDisplayName = "Hold";
#endif
		pParentOrder->csName.Empty();
		pParentOrder->iType = NODETYPE_CATEGORY;
		pParentOrder->iSubType = SUBTYPE_ORDER;
		hItemOrderRoot = InsertOurItem(pParentOrder, hItemPrtQueueRoot);
		pParentOrder->pAccountOwner = pPrinterQueue;
		//pPrinterQueue->coNodes.Add((CObject*)pParentOrder);
		pPrinterQueue->pParentOrder = pParentOrder;
		pPrinterQueue->hItemOrderRoot = hItemOrderRoot;
		// we store the expanded state in the account
		pParentOrder->bIsExpanded = pPrinterQueue->bIsExpanded_Orders;
	}

	if (pParentOrderArchive = new CNode)
	{
#ifdef LAB_STYLE
		pParentOrderArchive->csDisplayName = "Archive";
#else
		pParentOrderArchive->csDisplayName = "Print";
#endif
		pParentOrderArchive->csName.Empty();
		pParentOrderArchive->iType = NODETYPE_CATEGORY;
		pParentOrderArchive->iSubType = SUBTYPE_ORDER;
		hItemOrderArchiveRoot = InsertOurItem(pParentOrderArchive, hItemPrtQueueRoot);
		pParentOrderArchive->pAccountOwner = pPrinterQueue;
		//pPrinterQueue->coNodes.Add((CObject*)pParentOrderArchive);
		pPrinterQueue->pParentOrderArchive = pParentOrderArchive;
		pPrinterQueue->hItemOrderArchiveRoot = hItemOrderArchiveRoot;
		// we store the expanded state in the account
		pParentOrderArchive->bIsExpanded = pPrinterQueue->bIsExpanded_ArchivedOrders;
	}

	if (pParentProduct = new CNode)
	{
#ifdef LAB_STYLE
		pParentProduct->csDisplayName = "Media";
#else
		pParentProduct->csDisplayName = "Archive";
#endif
		pParentProduct->csName.Empty();
		pParentProduct->iType = NODETYPE_CATEGORY;
		pParentProduct->iSubType = SUBTYPE_PRODUCT;
		hItemProductRoot = InsertOurItem(pParentProduct, hItemPrtQueueRoot);
		pParentProduct->pAccountOwner = pPrinterQueue;
		//pPrinterQueue->coNodes.Add((CObject*)pParentProduct);
		pPrinterQueue->pParentProduct = pParentProduct;
		pPrinterQueue->hItemProductRoot = hItemProductRoot;
		// we store the expanded state in the account
		pParentProduct->bIsExpanded = pPrinterQueue->bIsExpanded_Products;
	}

	// expand if necessary
	if (pPrinterQueue->bIsExpanded)
		GetTreeCtrl().Expand(hItemPrtQueueRoot, TVE_EXPAND);

	return;
}

void CQueuePane::OnInitialUpdate()
{
	int i = 0;
//	CQueueFile* pCQueueFile;

	CZeusDoc* pDoc = PDOC;

//	((CQueueChild*)GetParentFrame())->AddQueue(PDOC);
//	((CQueueChild*)GetParentFrame())->PopulateNode( PDOC );
#ifdef LAB_STYLE
	AddNewNode( PDOC );
#endif

	// set pointer to this window
/*
	if (!bDataInit) 
	{
		 if (InitCoreData())
		 {
			// force a selection so that the middle pane gets
			// updated properly and the ui timer starts
			HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
			if (hItem)
				DoSelect(hItem);
		 }
	  }
*/

#ifdef LAB_STYLE
	SECTreeView::OnInitialUpdate();
#else
	CSpecialTree::OnInitialUpdate();
#endif
}

BOOL CQueuePane::InitCoreData()
{   
	LoadPrinters();

	// create the standard nodes
	PopulateStandardNodes();      

	return TRUE;
}

BOOL CQueuePane::LoadPrinters()
{
//	CPrinterQueue* pPrinters;

//	coPrinters.Add(pPrinters);

	return TRUE;
}

void CQueuePane::PopulateStandardNodes()
{
   CAccount* pPrinterQueue;
   CNode* pParentProduct;
   HTREEITEM hItemOrderRoot;
   HTREEITEM hItemOrderArchiveRoot;
   HTREEITEM hItemProductRoot;

//   HTREEITEM hItem;
   
   CNode* pParentOrder;
   CNode* pParentOrderArchive;	   

	// Add Print Queues to the Tree
	pPrinterQueue = new CAccount;
	pPrinterQueue->iSubType = SUBTYPE_MASTERACCT;

	pPrinterQueue->csDisplayName = "Fuji Frontier Series";
	pPrinterQueue->csName = "Fuji Frontier Series";

	//hItemPrtQueueRoot = InsertOurItem(pPrinterQueue, TVI_ROOT);

//	coAccount.Add(pPrinterQueue);
	coNodes.Add(pPrinterQueue);

		 if (pParentOrder = new CNode)
			{
#ifdef LAB_STYLE
			pParentOrder->csDisplayName = "Print";
#else
			pParentOrder->csDisplayName = "Hold";
#endif
			pParentOrder->csName.Empty();
			pParentOrder->iType = NODETYPE_CATEGORY;
			pParentOrder->iSubType = SUBTYPE_ORDER;
			hItemOrderRoot = InsertOurItem(pParentOrder, hItemPrtQueueRoot);
			pParentOrder->pAccountOwner = pPrinterQueue;
			//pPrinterQueue->coNodes.Add((CObject*)pParentOrder);
			pPrinterQueue->pParentOrder = pParentOrder;
			pPrinterQueue->hItemOrderRoot = hItemOrderRoot;
			// we store the expanded state in the account
			pParentOrder->bIsExpanded = pPrinterQueue->bIsExpanded_Orders;
			}

		 if (pParentOrderArchive = new CNode)
			{
#ifdef LAB_STYLE
			pParentOrderArchive->csDisplayName = "Archive";
#else
			pParentOrderArchive->csDisplayName = "Print";
#endif
			pParentOrderArchive->csName.Empty();
			pParentOrderArchive->iType = NODETYPE_CATEGORY;
			pParentOrderArchive->iSubType = SUBTYPE_ORDER;
			hItemOrderArchiveRoot = InsertOurItem(pParentOrderArchive, hItemPrtQueueRoot);
			pParentOrderArchive->pAccountOwner = pPrinterQueue;
			//pPrinterQueue->coNodes.Add((CObject*)pParentOrderArchive);
			pPrinterQueue->pParentOrderArchive = pParentOrderArchive;
			pPrinterQueue->hItemOrderArchiveRoot = hItemOrderArchiveRoot;
			// we store the expanded state in the account
			pParentOrderArchive->bIsExpanded = pPrinterQueue->bIsExpanded_ArchivedOrders;
			}

		 if (pParentProduct = new CNode)
			{
#ifdef LAB_STYLE
			pParentProduct->csDisplayName = "Media";
#else
			pParentProduct->csDisplayName = "Archive";
#endif
			pParentProduct->csName.Empty();
			pParentProduct->iType = NODETYPE_CATEGORY;
			pParentProduct->iSubType = SUBTYPE_PRODUCT;
			hItemProductRoot = InsertOurItem(pParentProduct, hItemPrtQueueRoot);
			pParentProduct->pAccountOwner = pPrinterQueue;
			//pPrinterQueue->coNodes.Add((CObject*)pParentProduct);
			pPrinterQueue->pParentProduct = pParentProduct;
			pPrinterQueue->hItemProductRoot = hItemProductRoot;
			// we store the expanded state in the account
			pParentProduct->bIsExpanded = pPrinterQueue->bIsExpanded_Products;
			}

		 // expand if necessary
		 if (pPrinterQueue->bIsExpanded)
			 GetTreeCtrl().Expand(hItemPrtQueueRoot, TVE_EXPAND);

	return;
}

#ifdef LAB_STYLE
void CQueuePane::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	SECTreeView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
#endif

void CQueuePane::DoSelect(HTREEITEM hItem, BOOL bMultipleSelection, BOOL bAllowSwitchMiddle)
{      
//	CRPNode* pNode;
	BOOL bFoundIt = FALSE;
}

HTREEITEM CQueuePane::InsertOurItem(CNode* pNode, HTREEITEM hItem, HTREEITEM hItemAfter)
{
	HTREEITEM hItemRet;
	TV_INSERTSTRUCT tvis;

	memset(&tvis, 0, sizeof(TV_INSERTSTRUCT));	

	tvis.hParent      = hItem;
	tvis.hInsertAfter = hItemAfter;
	tvis.item.lParam =  (LPARAM)pNode;
	tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;   
	tvis.item.iSelectedImage = I_IMAGECALLBACK;
	tvis.item.iImage = I_IMAGECALLBACK;
	tvis.item.pszText = LPSTR_TEXTCALLBACK;   
	hItemRet = GetTreeCtrl().InsertItem(&tvis);

	// make sure the + signs are showing
	GetTreeCtrl().ReMeasureAllItems();	  
	GetTreeCtrl().Invalidate();

	return hItemRet;
}

/////////////////////////////////////////////////////////////////////////////
// CQueuePane drawing

void CQueuePane::OnDraw(CDC* pDC)
{
//	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CQueuePane diagnostics

#ifdef _DEBUG
void CQueuePane::AssertValid() const
{
#ifdef LAB_STYLE
	SECTreeView::AssertValid();
#else
	CSpecialTree::AssertValid();
#endif
}

void CQueuePane::Dump(CDumpContext& dc) const
{
#ifdef LAB_STYLE
	SECTreeView::Dump(dc);
#else
	CSpecialTree::Dump(dc);
#endif
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CQueuePane message handlers

BOOL CQueuePane::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
   cs.style |= 	  
	  (TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_EDITLABELS | 
	  TVS_SHOWSELALWAYS);    	  

   cs.dwExStyle |= (LVXS_HILIGHTSUBITEMS | TVXS_MULTISEL);
	
#ifdef LAB_STYLE
	return SECTreeView::PreCreateWindow(cs);
#else
	return CSpecialTree::PreCreateWindow(cs);
#endif
}

void CQueuePane::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CNode* pNode;
   int iImage;
   int iSelectedImage;

   // get a pointer to the item
   TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

   // get node
   pNode = (CNode*)pTVDispInfo->item.lParam;

   if (!pNode) return;

   // default
  if (pNode->bIsExpanded)     
     iSelectedImage = iImage = IMAGECALLBACK_OPENFOLDER;		      
  else     
     iSelectedImage = iImage = IMAGECALLBACK_CLOSEDFOLDER;		      

   // get the icon based on the node type
   switch(pNode->iType)
	  {
	  case NODETYPE_UNDEFINED:
		   break;
	  case NODETYPE_CATEGORY:
		   break;
	  case NODETYPE_LOCALDRIVE:
		   switch(pNode->iSubType)
			  {
			  case DRIVE_UNKNOWN:
				   break;
			  case DRIVE_NO_ROOT_DIR:
				   break;
			  case DRIVE_REMOVABLE:
				   break;
			  case DRIVE_FIXED:
				   iSelectedImage = iImage = IMAGECALLBACK_HARDDISK;
				   break;
			  case DRIVE_REMOTE:
				   iSelectedImage = iImage = IMAGECALLBACK_NETWORK; 
				   break;
			  case DRIVE_CDROM:
				   iSelectedImage = iImage = IMAGECALLBACK_CDROM;		 
				   break;
			  case DRIVE_RAMDISK:
				   break;
			  case DRIVE_FLOPPY:
				   iSelectedImage = iImage = IMAGECALLBACK_FLOPPY;		 
				   break;
			  }
		   break;
	  case NODETYPE_NETWORKDRIVE:
		   iSelectedImage = iImage = IMAGECALLBACK_NETWORK; 
		   break;
	  case NODETYPE_FOLDER:
		   if (pNode->bIsExpanded)
			  iSelectedImage = iImage = IMAGECALLBACK_OPENFOLDER;		 
		   else
			  iSelectedImage = iImage = IMAGECALLBACK_CLOSEDFOLDER;		 
		   break;
	  case NODETYPE_ACCOUNT:
		   // note that slaves can't be default accounts
		   if (pNode->bIsExpanded)
			  {
			  if (((CAccount*)pNode)->bIsDefault)
				 {
				 if (((CAccount*)pNode)->iSubType == SUBTYPE_MASTERACCT)
			        iSelectedImage = iImage = IMAGECALLBACK_OPENFOLDER_DEF;
				 else
					iSelectedImage = iImage = IMAGECALLBACK_ACCOUNTSLAVE_OPEN_DEF;
				 }
			  else 
				 {
				 if (((CAccount*)pNode)->iSubType == SUBTYPE_MASTERACCT)
					iSelectedImage = iImage = IMAGECALLBACK_OPENFOLDER;
				 else
					iSelectedImage = iImage = IMAGECALLBACK_ACCOUNTSLAVE_OPEN;
				 }
			  }
		   else
			  {
			  if (((CAccount*)pNode)->bIsDefault)
				 {
				 if (((CAccount*)pNode)->iSubType == SUBTYPE_MASTERACCT)
			        iSelectedImage = iImage = IMAGECALLBACK_CLOSEDFOLDER_DEF;
				 else
					iSelectedImage = iImage = IMAGECALLBACK_ACCOUNTSLAVE_CLOSED_DEF;
				 }
			  else 
				 {
				 if (((CAccount*)pNode)->iSubType == SUBTYPE_MASTERACCT)
				    iSelectedImage = iImage = IMAGECALLBACK_CLOSEDFOLDER;
			     else
				    iSelectedImage = iImage = IMAGECALLBACK_ACCOUNTSLAVE_CLOSED;
				 }
			  }
		   break;
	  case NODETYPE_ALBUM:
		   break;
	  case NODETYPE_USER:
		   break;
	  case NODETYPE_PHOTO:
		   break;
	  case NODETYPE_ORDER:
//		   if (((COrder*)pNode)->OrderDetails.iCDQty > 0)
//			  iSelectedImage = iImage = IMAGECALLBACK_ORDER_CD;
//		   else
//			  iSelectedImage = iImage = IMAGECALLBACK_ORDER;		 
		   break;
	  case NODETYPE_PRODUCT:
		   switch(pNode->iSubType)
			  {
			  case SUBTYPE_PRODUCT_RPFULFILL:
				   iSelectedImage = iImage = IMAGECALLBACK_PRODUCT;
				   break;
			  case SUBTYPE_PRODUCT_WHOLESALE:
				   iSelectedImage = iImage = IMAGECALLBACK_PRODUCT_WHOLESALE;
				   break;
			  case SUBTYPE_PRODUCT_NOFULFILL:
				   iSelectedImage = iImage = IMAGECALLBACK_PRODUCT_DISCONNECTED;
				   break;
			  default:
				   iSelectedImage = iImage = IMAGECALLBACK_PRODUCT;
				   break;										   
			  }
		   break;
	  }				

   if(pTVDispInfo->item.mask & TVIF_TEXT)
	  strcpy(pTVDispInfo->item.pszText, pNode->csDisplayName);
   if (pTVDispInfo->item.mask & TVIF_IMAGE)
	  pTVDispInfo->item.iImage = iImage;
   if (pTVDispInfo->item.mask & TVIF_SELECTEDIMAGE)
	  pTVDispInfo->item.iSelectedImage = iSelectedImage;
	
   *pResult = 0;
}
