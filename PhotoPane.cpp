// PhotogizePhotoView.cpp : implementation of the CPhotoPane class
//

#include "stdafx.h"
#include "photopane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// the thumnail thread
DWORD _cdecl ThumbThreadProc(LPVOID pParam); 

#define CLIPARRAY      ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->ClipArray
const UINT NEAR wm_PrintThreadMessage = RegisterWindowMessage("PrintThreadMessage");  
const UINT NEAR wm_KioskDlgMessage = RegisterWindowMessage("KioskDlgMessage");  

#define NUMCOLS   3
static int bSortOrder[NUMCOLS];

/////////////////////////////////////////////////////////////////////////////
// CPhotoPane

IMPLEMENT_DYNCREATE(CPhotoPane, CListView)

BEGIN_MESSAGE_MAP(CPhotoPane, CListView)
	//{{AFX_MSG_MAP(CPhotoPane)	
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_WM_DROPFILES()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhotoPane construction/destruction

/*   
int CALLBACK SortPhotos(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
   {
   int nRetVal;

   CPhoto* pData1 = (CPhoto*)lParam1;
   CPhoto* pData2 = (CPhoto*)lParam2;
  
   switch(lParamSort)
	  {
	  case 0:
		   nRetVal = strcmp(pData1->csDisplayName, pData2->csDisplayName);
		   break;		 
	  case 1:
		   nRetVal = strcmp(pData1->csProductName, pData2->csProductName);
		   break;		 
	  case 2:
		   if (pData1->iCopies < pData2->iCopies)
			  nRetVal = -1;
		   else if (pData1->iCopies > pData2->iCopies)
			  nRetVal = 1;
		   else 
			  nRetVal = 0;
		   break;		 
	  default:
		 break;
	  }   

   if (bSortOrder[lParamSort] == 1)
      return nRetVal;
   else
	  return (-nRetVal);
   }
   
*/
CPhotoPane::CPhotoPane()
   {
//   bCancelThumbRender = FALSE;   
//   pRenderNode = NULL;      
//   bStopRender = FALSE;
//   hInRenderLoop = CreateMutex(NULL, FALSE, NULL);   

   // register our clipboard format
   CString cs;
//   cs.LoadString(IDS_PHOTOGIZE_CBOARD_FMT);   
//   uiOurListClipFormat = ::RegisterClipboardFormat((const char*)cs);   
   }

CPhotoPane::~CPhotoPane()
   {
/*
   if (hInRenderLoop) 
	  {
	  ReleaseMutex(hInRenderLoop);
      CloseHandle(hInRenderLoop);
	  }
*/
   }


BOOL CPhotoPane::PreCreateWindow(CREATESTRUCT& cs)
   {
   cs.style |= LVS_SHOWSELALWAYS;

   // get existing style
   int dwStyle = AfxGetApp()->GetProfileInt("LookAndFeel", "PhotoView", LVS_REPORT);
   cs.style |= dwStyle;

   return CListView::PreCreateWindow(cs);
   }

/////////////////////////////////////////////////////////////////////////////
// CPhotoPane drawing

void CPhotoPane::OnDraw(CDC* pDC)
{
}

void CPhotoPane::OnInitialUpdate()
   {
/*
   // register this as a drop target
   m_OleDropTarget.Register(this);

   // we can do drag and drop
   DragAcceptFiles(TRUE);
*/
   CListView::OnInitialUpdate();
   }

/////////////////////////////////////////////////////////////////////////////
// CPhotoPane diagnostics

#ifdef _DEBUG
void CPhotoPane::AssertValid() const
{
	CListView::AssertValid();
}

void CPhotoPane::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

/*
CPhotogizeLabDoc* CPhotoPane::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPhotogizeLabDoc)));
	return (CPhotogizeLabDoc*)m_pDocument;
}
*/
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPhotoPane message handlers
/*
int CPhotoPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
   {											 
   lpCreateStruct->style |= LVS_AUTOARRANGE;       

   if (CListView::OnCreate(lpCreateStruct) == -1)
	  return -1;
   
//   GetDocument()->m_pPhotoView = this;	

   CListCtrl& theCtrl = GetListCtrl();
   
   HIMAGELIST hImageList;   

//   ::L_UnlockSupport(L_SUPPORT_GIFLZW, "JnjmyyTss");
//   ::L_UnlockSupport(L_SUPPORT_TIFLZW, "JGgUy78FRRx");

   // get thumbnail size
//   iThumbnailSize = AfxGetApp()->GetProfileInt("Display", "ThumbSize", THUMBNAILSIZE);

   // Create the image list with iThumbnailSize*iThumbnailSize icons and 24 bpp color depth
   // a maximum of MAXIMAGES images
//   hImageList=ImageList_Create(SMALLICONSIZE, SMALLICONSIZE, 
//	  ILC_COLOR24, 0, MAXIMAGES);   
   m_imageListSmall.Attach(hImageList);

//   hImageList=ImageList_Create(iThumbnailSize, iThumbnailSize, 
//	  ILC_COLOR24, 0, MAXIMAGES);   
   m_imageListLarge.Attach(hImageList);

   // Use the image list in the list view
   theCtrl.SetImageList(&m_imageListLarge, LVSIL_NORMAL);
   theCtrl.SetImageList(&m_imageListSmall, LVSIL_SMALL);

   // set the maximum size of the columns   
   int iMaxSize = 640;

   // get column widths
   // note: use the old ColWidth? designations as we have removed a column
   // but don't want to redo the profile settings
   int colwidth;
   colwidth = AfxGetApp()->GetProfileInt("PhotoView", "ColWidth0", 200);
   colwidth = min(max(colwidth, 10), iMaxSize);
   theCtrl.InsertColumn(0, "Name", LVCFMT_LEFT, colwidth);

   colwidth = AfxGetApp()->GetProfileInt("PhotoView", "ColWidth2", 200);
   colwidth = min(max(colwidth, 10), iMaxSize);
   theCtrl.InsertColumn(1, "Product", LVCFMT_LEFT, colwidth);

   colwidth = AfxGetApp()->GetProfileInt("PhotoView", "ColWidth3", 50);
   colwidth = min(max(colwidth, 10), iMaxSize);
   theCtrl.InsertColumn(2, "Copies", LVCFMT_LEFT, colwidth);

   theCtrl.SetExtendedStyle (theCtrl.GetExtendedStyle() 
	  | LVS_EX_FULLROWSELECT);
   theCtrl.SetIconSpacing(CSize(iThumbnailSize + 10, iThumbnailSize + 20));	

   for(int i = 0; i < NUMCOLS; i++)
	  bSortOrder[i] = 1;	   
   
   return 0;
   }

void CPhotoPane::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   CPhoto* pPhoto;   

   LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

   // get Photo
   pPhoto = (CPhoto*)pDispInfo->item.lParam;

   if (!pPhoto)
      {
	  *pResult = 0;
	  return;
	  }		  

	if (pDispInfo->item.mask & LVIF_TEXT)
	   {
	   switch(pDispInfo->item.iSubItem)
		  {
		  case 0:
			   strcpy(pDispInfo->item.pszText, (const char*)pPhoto->csDisplayName);			   
			   break;
		  case 1:			   
			   strcpy(pDispInfo->item.pszText, (const char*)pPhoto->csProductName);			   
			   break;
		  case 2:			   
			   itoa(pPhoto->iCopies, pDispInfo->item.pszText, 10);			   
			   break;
		  }	   
	   }
    if (pDispInfo->item.mask & LVIF_IMAGE)
	   {
	   pDispInfo->item.iImage = pPhoto->iImageIndex;
	   }  												   
	*pResult = 0;
   }				

void CPhotoPane::OnTreeChanged(CNode* pNode)
   {
   int i;   
   COrder* pOrder;

   // if a new node is selected, and we are rendering, stop it
   if (pRenderNode != pNode || pNode == NULL)
	  {
	  CancelRendering();	  
	  }		
	
   TRACE("OnTreeChanged\r\n");

   if (bStopRender) return;

   // if no node, clear the list, otherwise, start the thumbnail thread
   if (!pNode)					
	  {
	  CListCtrl& theCtrl = GetListCtrl();

	  theCtrl.DeleteAllItems();
	  // remove all image list items
	  for (i = m_imageListLarge.GetImageCount() - 1; i >= 0; i--)      
		 m_imageListLarge.Remove(i);   
	  for (i = m_imageListSmall.GetImageCount() - 1; i >= 0; i--)      
		 m_imageListSmall.Remove(i);   
	  pRenderNode = NULL;
	  }				
   else										    
	  {			 	  
	  // set the pointers
	  pRenderNode = pNode;
	  
	  bCancelThumbRender = FALSE;
	  
	  // load the thumbnail data
	  LoadThumbs();

	  // reset the sort order
	  if (pOrder = (COrder*)pRenderNode)
		 {
		 if (pOrder->iLastSortColumn != -1 && 
			 pOrder->bLastSortOrder != 0)
			{			
			if (pOrder->iLastSortColumn >= 0 && pOrder->iLastSortColumn < NUMCOLS)
			   {
			   CListCtrl& theCtrl = GetListCtrl();
			   bSortOrder[pOrder->iLastSortColumn] = pOrder->bLastSortOrder;
			   theCtrl.SortItems(SortPhotos, pOrder->iLastSortColumn);	   
			   bSortOrder[pOrder->iLastSortColumn] = -bSortOrder[pOrder->iLastSortColumn];
			   }
			}
		 }

	  // spin off the rendering thread
	  pThumbThread = AfxBeginThread((AFX_THREADPROC)ThumbThreadProc, 
		 (LPVOID)this, THREAD_PRIORITY_NORMAL);   
	  }
   }

void CPhotoPane::LoadThumbs()
   {
   int i, j;
   LVITEM lvis;   
   CPhoto* pPhoto;   
   HBITMAP hBitmap;
   HBITMAP hBitmapSmall = NULL;   
   HBITMAP hBitmapLarge = NULL;   
   HIMAGELIST hImageListLarge;
   HIMAGELIST hImageListSmall;   

   CListCtrl& theCtrl = GetListCtrl();

   // clear the list   
   if (bCancelThumbRender) 
	  return;
   else
	  theCtrl.DeleteAllItems();   

   // remove all image list items
   for (i = m_imageListLarge.GetImageCount() - 1; i >= 0; i--)      
	  m_imageListLarge.Remove(i);   
   for (i = m_imageListSmall.GetImageCount() - 1; i >= 0; i--)      
	  m_imageListSmall.Remove(i);   

   // if the node is null, just return
   if (!pRenderNode) return;
   
   // check all the children and see if they are photos
   hBitmapLarge = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_XIMG)); 
   if (!hBitmapLarge) return;

   hBitmapSmall = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_XIMG_SMALL)); 
   if (!hBitmapSmall) 
	  {
	  DeleteObject(hBitmapLarge);
	  return;
	  }		 

   hImageListLarge = (HIMAGELIST)m_imageListLarge;
   if (!hImageListLarge) 
	  {
	  DeleteObject(hBitmapLarge);
	  DeleteObject(hBitmapSmall);
	  return;
	  }

   hImageListSmall = (HIMAGELIST)m_imageListSmall;
   if (!hImageListSmall) 
	  {
	  DeleteObject(hBitmapLarge);
	  DeleteObject(hBitmapSmall);
	  return;
	  }

   // resize the large bitmap to the current thumbsize
   BITMAPHANDLE Bitmap;
   HDC hDC;
   if (::L_InitBitmap(&Bitmap, 0, 0, 24) == SUCCESS)
	  {
	  if (hDC = ::CreateDC("DISPLAY", NULL, NULL, NULL))
		 {
		 if (::L_ConvertFromDDB(hDC, &Bitmap, hBitmapLarge, NULL) == SUCCESS)
			{
			if (::L_SizeBitmap(&Bitmap, iThumbnailSize, iThumbnailSize, SIZE_BICUBIC) == SUCCESS)
			   {
			   DeleteObject(hBitmapLarge);
			   hBitmapLarge = (HBITMAP)::L_ConvertToDDB(hDC, &Bitmap);			  
			   ::L_FreeBitmap(&Bitmap);
			   }
			}
		 ::DeleteDC(hDC);
		 }   
	  }

   SetRedraw(FALSE);

   for (i = 0, j = 0; i < pRenderNode->coNodes.GetSize(); i++)
	  {
	  pPhoto = (CPhoto*)pRenderNode->coNodes.GetAt(i);

	  if (!pPhoto) continue;
	  if (pPhoto->iType != NODETYPE_PHOTO) continue;
	  
	  // set the index
	  pPhoto->iImageIndex = j;

	  if (bCancelThumbRender) break;

	  // if we don't already have the bmp, add the unknown image
	  if (!pPhoto->HasWinBmp(SMALLICONSIZE))
	     ImageList_Add(hImageListSmall, hBitmapSmall, (HBITMAP)NULL);
	  else
		 {
		 hBitmap = pPhoto->GetWinBmp(SMALLICONSIZE);
		 ImageList_Add(hImageListSmall, hBitmap, (HBITMAP)NULL);
		 }

	  if (!pPhoto->HasWinBmp(iThumbnailSize))
		 ImageList_Add(hImageListLarge, hBitmapLarge, (HBITMAP)NULL);		 	  	  
	  else
		 {
		 hBitmap = pPhoto->GetWinBmp(iThumbnailSize);
		 ImageList_Add(hImageListLarge, hBitmap, (HBITMAP)NULL);
		 }
		 
	  ZeroMemory(&lvis, sizeof LVITEM);	
	  lvis.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT;	  	  
	  lvis.iItem = j++;
	  lvis.iSubItem = 0;
	  lvis.state = 0;
	  lvis.stateMask = 0;
	  lvis.pszText = LPSTR_TEXTCALLBACK;
	  lvis.iImage = I_IMAGECALLBACK;	  
	  lvis.lParam = (LPARAM)pPhoto; 	  	  
	  if (!bCancelThumbRender) theCtrl.InsertItem(&lvis); 		  	  
	  }

   SetRedraw(TRUE);

   // if none selected, select the first item   
   if (!bCancelThumbRender) 
	  {
	  if (GetNumSelected() == 0)
		 {
		 UINT nState(0), nStateMask(LVIS_SELECTED);
		 nState = nStateMask;
		 theCtrl.SetItemState(0, nState, nStateMask);
		 }
	  theCtrl.RedrawItems(0, j - 1);	  
	  }   

   // delete the unknown bitmap
   DeleteObject(hBitmapSmall);
   DeleteObject(hBitmapLarge);
   }

DWORD _cdecl ThumbThreadProc(LPVOID pParam)
   {
   // cast the pointer to the view class
   CPhotoPane* cPhotoView = (CPhotoPane*)pParam;

   // check validity
   if (cPhotoView == NULL) return FALSE;

   // signal that we are in the render loop
   WaitForSingleObject(cPhotoView->hInRenderLoop, INFINITE);   

   // render the real thumbs
   cPhotoView->RenderThumbs();

   // we are out of the render loop
   ReleaseMutex(cPhotoView->hInRenderLoop);

   return TRUE;
   }

void CPhotoPane::RenderThumbs()
   {
   int i, j;
   CPhoto* pPhoto;
   HBITMAP hBitmap;
   HIMAGELIST hImageList;

   if (!pRenderNode) return;

   //TRACE("RenderThumbs\r\n");

   for (i = 0, j = 0; i < pRenderNode->coNodes.GetSize(); i++)
	  {
	  pPhoto = (CPhoto*)pRenderNode->coNodes.GetAt(i);	  
	  if (!pPhoto) continue;
	  if (pPhoto->iType != NODETYPE_PHOTO) continue;	  
	  pPhoto->bCancelLoad = FALSE;

	  if (bCancelThumbRender) break;

	  if ((GetStyle() & LVS_REPORT) == LVS_REPORT)
		 {	
		 // make or get the small thumbnail
		 hBitmap = pPhoto->GetWinBmp(SMALLICONSIZE);

		 // replace the holder bitmap with the real one
		 hImageList = (HIMAGELIST)m_imageListSmall;
		 if (hBitmap && hImageList) 
			ImageList_Replace(hImageList, 
			pPhoto->iImageIndex, hBitmap, (HBITMAP)NULL);	
		 }
	  else
		 {
		 // make or get the thumbnail
		 hBitmap = pPhoto->GetWinBmp(iThumbnailSize);
	  
		 // replace the holder bitmap with the real one
		 hImageList = (HIMAGELIST)m_imageListLarge;
		 if (hBitmap && hImageList) 
			ImageList_Replace(hImageList, 
			pPhoto->iImageIndex, hBitmap, (HBITMAP)NULL);		
		 }

	  // redraw the bitmap	  
	  if (bCancelThumbRender) 
		 break;
	  else
		 {	
		 CListCtrl& theCtrl = GetListCtrl();
		 // Do a PostMessage here instead of a RedrawItems (SendMessage)
		 // to avoid deadlock
		 // GetListCtrl().RedrawItems(pPhoto->iImageIndex, pPhoto->iImageIndex);	  
         ::PostMessage(theCtrl.m_hWnd, LVM_REDRAWITEMS, 
			pPhoto->iImageIndex, pPhoto->iImageIndex);		 
		 }
	  }   
   }

void CPhotoPane::CancelRendering()
   {							  
   int i;
   CPhoto* pPhoto;   

   //TRACE("CancelRendering\r\n");
   
   bCancelThumbRender = TRUE;
   bStopRender = TRUE;		 

   // set the cancel flag in the image
   if (pRenderNode)
	  {
	  for (i = 0; i < pRenderNode->coNodes.GetSize(); i++)
		 {
		 pPhoto = (CPhoto*)pRenderNode->coNodes.GetAt(i);	  
		 if (!pPhoto) continue;
		 if (pPhoto->iType != NODETYPE_PHOTO) continue;	  		 
		 pPhoto->bCancelLoad = TRUE;
		 }
	  }
   bStopRender = FALSE;

   // wait for mutex to signal that we are out of
   // the render loop
   WaitForSingleObject(hInRenderLoop, INFINITE);   
   ReleaseMutex(hInRenderLoop);
   }

void CPhotoPane::OnDestroy() 
   {   
   CancelRendering();

   // save column widths   
   if ((GetStyle() & LVS_REPORT) == LVS_REPORT)
	  {
	  CListCtrl& theCtrl = GetListCtrl();

      AfxGetApp()->WriteProfileInt("PhotoView", "ColWidth0", theCtrl.GetColumnWidth(0));         
      AfxGetApp()->WriteProfileInt("PhotoView", "ColWidth2", theCtrl.GetColumnWidth(1));   
      AfxGetApp()->WriteProfileInt("PhotoView", "ColWidth3", theCtrl.GetColumnWidth(2));   
	  }

   // destroy the print dlg if it exists
   if (!PrintDlg.bIsInit)
      PrintDlg.DestroyWindow();

   CListView::OnDestroy();
   }

int CPhotoPane::GetFirstSelectedItem()
   {
   CListCtrl& theCtrl = GetListCtrl();

   POSITION pos = theCtrl.GetFirstSelectedItemPosition();

   if (pos == NULL)
	  return -1;
   else
	  return theCtrl.GetNextSelectedItem(pos);
   }

#define DRAGSCROLLDELAY 1000
*/
DROPEFFECT CPhotoPane::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
   {   
   return OnDragOver(pDataObject, dwKeyState, point);
   }

void CPhotoPane::OnDragLeave() 
   {
   CListView::OnDragLeave();
   }

DROPEFFECT CPhotoPane::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
   {
   DROPEFFECT de=DROPEFFECT_NONE;
/*
   UINT iHitFlags;   
   int idx;

   // highlight this window   
   SetFocus();         

   CListCtrl& theCtrl = GetListCtrl();

   idx = theCtrl.HitTest(point, &iHitFlags);

   // ok to proceed?
   if (idx != -1)
	  {		 
	  if( dwKeyState & MK_CONTROL )
		 de=DROPEFFECT_COPY;      
	  else
		 de=DROPEFFECT_MOVE;
	  }
*/
   return de;
   }

BOOL CPhotoPane::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
   {
/*
   HGLOBAL  hGlobal;
   LPCSTR   pData;

   // Get text data from COleDataObject   
   hGlobal=pDataObject->GetGlobalData(uiOurListClipFormat);   

   // if hGlobal is NULL, return FALSE and let our
   // non-OLE code do the work   
   if (!hGlobal) return FALSE;

   // Get pointer to data
   pData=(LPCSTR)GlobalLock(hGlobal);       

   ASSERT(pData!=NULL); 

   InsertDroppedPhotos(point);	

   // Unlock memory
   GlobalUnlock(hGlobal);   
*/
   return TRUE;
   }
/*
void CPhotoPane::InsertDroppedPhotos(CPoint& point)
   {
   int i, idx;
   CPhoto* pPhoto;   
   UINT iHitFlags;

   CListCtrl& theCtrl = GetListCtrl();
   
   idx = theCtrl.HitTest(point, &iHitFlags);

   if (idx == -1) return;

   // available?
   if (CLIPARRAY.IsTypeAvail(OURCLIP_PHOTO))
      {
	  // set drop zone
	  CLIPARRAY.SetDropNode(pRenderNode);

      // get all elements and insert them
      for (i = 0; i < CLIPARRAY.GetSize(); i++)
         {	
		 if (pPhoto = new CPhoto)
			{			
			// copy the data
			*pPhoto = *((CPhoto*)(CLIPARRAY.GetAt(i)));			
			InstantiateImageFile(pPhoto);
			pRenderNode->coNodes.InsertAt(PhotoIdxToNodeIdx(idx++), (CObject*)pPhoto);			
			}
		 }      	  
	  OnTreeChanged(pRenderNode);
	  }   
   }   

void CPhotoPane::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   // Send WM_CONTEXTMENU to self
   SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());
   // Mark message as handled and suppress default handling
   *pResult = 1;
   }

void CPhotoPane::OnContextMenu(CWnd* pWnd, CPoint point) 
   {      
   CMenu *submenu;

   CListCtrl& theCtrl = GetListCtrl();
   
   // if Shift-F10
   if (point.x == -1 && point.y == -1)
	  point = (CPoint) GetMessagePos();
   
   ScreenToClient(&point);
   
   // the font popup is stored in a resource
   menu.LoadMenu(IDR_CONTEXTMENUS);
   menu.LoadToolbar(IDR_TOOLBAR_MAIN_SMALL);
   ClientToScreen(&point);

   // get the user menu
   submenu = menu.GetSubMenu(1);
   
   // post the menu                                                        
   submenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,                  
	  point.x,                                        
	  point.y,                                        
	  AfxGetApp()->m_pMainWnd, NULL); 	
   }

void CPhotoPane::OnPhotosmenuPrint() 
   {
   PhotosmenuPrint();
   }

void CPhotoPane::PhotosmenuPrint(BOOL bPrintSingleCopy) 
   {   
   CString csTemp;
   CPhoto* pPhoto;   
   LVITEM lvis;      
   COrder* pOrder;   
   COrder* pParentOrder;   
   CPhotogizeUtils utils;   
   CAccount* pAccount;

   // alloc a new order
   if (!(pOrder = new COrder)) return;

   CListCtrl& theCtrl = GetListCtrl();

   // default: assign a unique name
   utils.GetUniqueName(csTemp);
   pOrder->csName = csTemp;
   pOrder->csDisplayName = csTemp;

   // try to use the data from the parent order
   if (pRenderNode)
	  {
	  pParentOrder = (COrder*)pRenderNode;
	  if (pParentOrder->iType == NODETYPE_ORDER)
		 {
	     *pOrder = *pParentOrder;					  
		 pOrder->OrderDetails.ParseDetails();
		 }
	  }						  		  

   pOrder->bPrintIndex = FALSE;

   POSITION pos = theCtrl.GetFirstSelectedItemPosition();
   if (pos != NULL)
	  {
	  while (pos)
		 {
		 if ((lvis.iItem = theCtrl.GetNextSelectedItem(pos)) != -1)
			{
			lvis.mask = LVIF_PARAM;
			theCtrl.GetItem(&lvis); 
			// copy the photo so that it doesn't get clobbered
			// when we delete the order
			if (pPhoto = new CPhoto)
			   {			   
			   *pPhoto = *((CPhoto*)lvis.lParam);
			   // print just a single copy?
			   if (bPrintSingleCopy) pPhoto->iCopies = 1;
			   // any will do
			   pAccount = pPhoto->pAccountOwner;
			   pOrder->coNodes.Add(pPhoto);
			   }
			}
		 }
	  }

   if (!pAccount) 
	  {
	  delete pOrder;
	  return;
	  }

   if (pOrder->coNodes.GetSize() > 0)
	  {	  	
	  pOrder->pAccountOwner = pAccount;

      PrintPhotosThreading(pOrder);
	  }
   }

void CPhotoPane::OnPhotosmenuDelete() 
   {
   if (AfxMessageBox(IDS_CONFIRMDELETE, MB_YESNO | MB_SETFOREGROUND) != IDYES)
	  return;
   else
	  DeleteSelected();
   }

void CPhotoPane::DeleteSelected()
   {
   int i;
   LVITEM lvis; 
   CPhoto* pPhoto;
   int iFirstSel = -1;

   CListCtrl& theCtrl = GetListCtrl();

   POSITION pos = theCtrl.GetFirstSelectedItemPosition();
   if (pos == NULL)
      return;
   else
	  {
	  SetRedraw(FALSE);
	  // mark all as undeleted
	  for (i = 0; i < pRenderNode->coNodes.GetSize(); i++)
		 {
		 if (pPhoto = (CPhoto*)pRenderNode->coNodes.GetAt(i))
			{
			pPhoto->bToDelete = FALSE;
			}
		 }
	  // mark selected to delete
	  while (pos)
		 {
		 if ((lvis.iItem = theCtrl.GetNextSelectedItem(pos)) != -1)
			{
			if (iFirstSel == -1) iFirstSel = lvis.iItem;
			lvis.mask = LVIF_PARAM | LVIF_STATE;
			theCtrl.GetItem(&lvis); 	
			pPhoto = (CPhoto*)lvis.lParam;
			if (pPhoto) 
			   {
			   pPhoto->bToDelete = TRUE;
			   }
			}	
		 }
	  // now delete from the parent node	  
	  for (i = pRenderNode->coNodes.GetSize() - 1; i >= 0; i--)
		 {
		 if (pPhoto = (CPhoto*)pRenderNode->coNodes.GetAt(i))
			{
			if (pPhoto->bToDelete)
			   {
			   // delete the disk file
			   pPhoto->DeleteDiskFile(TRUE);
			   }
			}
		 }	 
	  // if this is an order, delete the photos explicitly,
	  // otherwise the refresh engine will do the trick
	  if (pRenderNode->iType == NODETYPE_ORDER)
	     DeleteTaggedItems();
	  SetRedraw(TRUE);
	  }   
   if (iFirstSel != -1)	  
	  theCtrl.RedrawItems(iFirstSel, theCtrl.GetItemCount() - 1); 
   
   // select none
   CMainFrame* pFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
   pFrm->SwitchMiddleView(SHOWVIEW_PREVIEWBAR);
   GetDocument()->m_pPreviewBar->UpdateInfo(NULL, 0);
   }

int CPhotoPane::GetNumSelected()
   {
   int iNumSelected = 0;

   CListCtrl& theCtrl = GetListCtrl();

   POSITION pos = theCtrl.GetFirstSelectedItemPosition();
   if (pos == NULL)
      return 0;
   else
	  {
	  while (pos)
		 {
		 if (theCtrl.GetNextSelectedItem(pos) != -1)
			iNumSelected++;
		 }
	  }
   return iNumSelected;
   }

void CPhotoPane::OnEditSelectall() 
   {
   int i;
   int iSelected;

   if (!pRenderNode) return;

   UINT nState(0), nStateMask(LVIS_SELECTED);

   CListCtrl& theCtrl = GetListCtrl();

   for (i = 0; i < theCtrl.GetItemCount(); i++)
      theCtrl.SetItemState(i, nStateMask, nStateMask);
   theCtrl.RedrawItems(0, theCtrl.GetItemCount() - 1);   

   // force preview bar display   
   CMainFrame* pFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
   pFrm->SwitchMiddleView(SHOWVIEW_PREVIEWBAR);   

   // mark selected photos
   iSelected = MarkSelected();

   // set photo pointer
   GetDocument()->m_pPreviewBar->UpdateInfo(pRenderNode, iSelected);
   }

void CPhotoPane::OnUpdateEditSelectall(CCmdUI* pCmdUI) 
   {
   CListCtrl& theCtrl = GetListCtrl();

   if (theCtrl.GetItemCount() > 0)
	  pCmdUI->Enable(TRUE);
   else
	  pCmdUI->Enable(FALSE);
   }

int CPhotoPane::MarkSelected()
   {
   LVITEM lvis;   
   CPhoto* pPhoto;   
   int iSelected = 0;   
   CListCtrl& theCtrl = GetListCtrl();

   // loop through the list and set the bSelected flag
   ZeroMemory(&lvis, sizeof LVITEM);
   for (lvis.iItem = 0; lvis.iItem < theCtrl.GetItemCount(); lvis.iItem++)	  
	  {
	  lvis.mask = LVIF_PARAM | LVIF_STATE;
	  theCtrl.GetItem(&lvis); 	
   	  if (pPhoto = (CPhoto*)lvis.lParam)
		 {
		 if (theCtrl.GetItemState(lvis.iItem, LVIS_SELECTED))
			{
			iSelected++;
			pPhoto->bSelected = TRUE;
			}
		 else
			pPhoto->bSelected = FALSE;
		 }
	  }
   return iSelected;
   }

int CPhotoPane::MarkSelectedAsDelete()
   {
   LVITEM lvis;   
   CPhoto* pPhoto;   
   int iSelected = 0;   
   CListCtrl& theCtrl = GetListCtrl();

   // loop through the list and set the bSelected flag
   ZeroMemory(&lvis, sizeof LVITEM);
   for (lvis.iItem = 0; lvis.iItem < theCtrl.GetItemCount(); lvis.iItem++)	  
	  {
	  lvis.mask = LVIF_PARAM | LVIF_STATE;
	  theCtrl.GetItem(&lvis); 	
   	  if (pPhoto = (CPhoto*)lvis.lParam)
		 {
		 if (theCtrl.GetItemState(lvis.iItem, LVIS_SELECTED))
			{
			iSelected++;
			pPhoto->bSelected = TRUE;
			pPhoto->bToDelete = TRUE;
			}
		 else
			{
			pPhoto->bSelected = FALSE;
			pPhoto->bToDelete = FALSE;
			}
		 }
	  }
   return iSelected;
   }

void CPhotoPane::DeleteTaggedItems()
   {
   int i;
   LVITEM lvis; 
   CPhoto* pPhoto;

   CListCtrl& theCtrl = GetListCtrl();

   // delete from the list   
   for (lvis.iItem = theCtrl.GetItemCount() - 1; lvis.iItem >= 0; lvis.iItem--)
	  {
	  lvis.mask = LVIF_PARAM | LVIF_STATE;
	  theCtrl.GetItem(&lvis); 	
	  pPhoto = (CPhoto*)lvis.lParam;
	  if (pPhoto) 
		 {
		 if (pPhoto->bToDelete)
			{
			theCtrl.DeleteItem(lvis.iItem);
			}
		 }
	  }

   // now delete from the parent node but don't delete disk file	  
   for (i = pRenderNode->coNodes.GetSize() - 1; i>=0; i--)
	  {
	  if (pPhoto = (CPhoto*)pRenderNode->coNodes.GetAt(i))
		 {
		 if (pPhoto->bToDelete)
			{			
			pRenderNode->coNodes.RemoveAt(i);
			delete pPhoto;	
			}
		 }
	  }   
   }

void CPhotoPane::OnPhotosmenuSelectall() 
   {
   int i;

   if (!pRenderNode) return;

   UINT nState(0), nStateMask(LVIS_SELECTED);

   CListCtrl& theCtrl = GetListCtrl();

   for (i = 0; i < theCtrl.GetItemCount(); i++)
      theCtrl.SetItemState(i, nStateMask, nStateMask);
   theCtrl.RedrawItems(0, theCtrl.GetItemCount() - 1);   

   // force preview bar display
   int iSelected;
   CMainFrame* pFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
   pFrm->SwitchMiddleView(SHOWVIEW_PREVIEWBAR);   

   // mark selected photos
   iSelected = MarkSelected();

   // set photo pointer
   GetDocument()->m_pPreviewBar->UpdateInfo(pRenderNode, iSelected);
   }

void CPhotoPane::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   int numChar;
   
   COleDataSource OleDataSource; 

   CopyToClipboard(OURCLIP_PHOTO);
   CString csHolder;
   csHolder.LoadString(IDS_OLE_CBOARDHOLDER);

   // Get the text length
   numChar = csHolder.GetLength();
    
   // Create global memory for sharing dragdrop text
   HGLOBAL hgData=GlobalAlloc(GPTR,numChar+1);   
   ASSERT(hgData!=NULL);
    
   LPSTR   lpData=(LPSTR)GlobalLock(hgData);
   ASSERT(lpData!=NULL);
                                                 
   strcpy(lpData, (const char*)csHolder);   
   
   // Cache the data, and initiate DragDrop   
   OleDataSource.CacheGlobalData(uiOurListClipFormat, hgData);              

   // mark selected files to delete
   MarkSelectedAsDelete();
   
   DROPEFFECT dropEffect =
      OleDataSource.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE,NULL); 
   
   // delete selected files
   if ((dropEffect&DROPEFFECT_MOVE)==DROPEFFECT_MOVE)        
	  {
  	  // if we are dropping within nodes, delete
	  if (CLIPARRAY.GetDropNode() == pRenderNode)
	     DeleteTaggedItems();
	  OnTreeChanged(pRenderNode);
	  }
   else if((dropEffect&DROPEFFECT_MOVE)==DROPEFFECT_COPY)        
	  {      
	  OnTreeChanged(pRenderNode);
	  }

	*pResult = 0;
}

void CPhotoPane::CopyToClipboard(int fmt)
   {        
   int j = 0;
   CPhoto* pPhoto;   
   CPhoto* pNewPhoto;               
   LVITEM lvis;      
   CListCtrl& theCtrl = GetListCtrl();

   POSITION pos = theCtrl.GetFirstSelectedItemPosition();
   if (pos == NULL)
      return;
   else
	  {
      // delete the current array if any
      CLIPARRAY.Destroy();
	  // set the new type
	  CLIPARRAY.SetType(fmt);
	  // mark selected to delete
	  while (pos)
		 {
		 if ((lvis.iItem = theCtrl.GetNextSelectedItem(pos)) != -1)
			{	
			lvis.mask = LVIF_PARAM | LVIF_STATE;
			theCtrl.GetItem(&lvis); 	
			pPhoto = (CPhoto*)lvis.lParam;
			if (pPhoto) 
			   {			   
			   // make a copy
			   if (pNewPhoto = new CPhoto)
				  {				  
				  *pNewPhoto = *pPhoto; 
				  // insert into the clip array
				  CLIPARRAY.InsertAt(j++, pNewPhoto);
				  }
			   }
			}	
		 }
	  }
   }

void CPhotoPane::OnOurFilePrint() 
   {
   OnPhotosmenuPrint();
   }

void CPhotoPane::OnUpdateOurFilePrint(CCmdUI* pCmdUI) 
   {   
   pCmdUI->Enable((GetNumSelected() > 0) && (!PrintDlg.bInPrint));   
   }

void CPhotoPane::OnSize(UINT nType, int cx, int cy) 
   {
   CListView::OnSize(nType, cx, cy);	

   GetListCtrl().RedrawItems(0, GetListCtrl().GetItemCount() - 1);   
   }

void CPhotoPane::OnOrdermenu2Makecd() 
   {
   }

int CPhotoPane::PhotoIdxToNodeIdx(int iPhoto)
   {
   int iCnt; // count of the photos
   int iRet = 0;
   int i;
   CNode* pNode;
   
   for (i = 0, iCnt = 0; i < pRenderNode->coNodes.GetSize(); i++)
	  {
	  pNode = (CNode*)pRenderNode->coNodes.GetAt(i);

	  // skip bad stuff
	  if (!pNode) continue;
	  if (pNode->iType != NODETYPE_PHOTO) continue;

	  if (iPhoto == iCnt)
		 {
		 iRet = i;
		 break;
		 }
	  iCnt++;
	  }
   return iRet;
   }

void CPhotoPane::OnUpdateEditCopy(CCmdUI* pCmdUI) 
   {
   pCmdUI->Enable(GetNumSelected() > 0);
   }

void CPhotoPane::OnEditCopy() 
   {
   int numChar;
   COleDataSource OleDataSource; 
   
   CopyToClipboard(OURCLIP_PHOTO);
   CString csHolder;
   csHolder.LoadString(IDS_OLE_CBOARDHOLDER);

   // Get the text length
   numChar = csHolder.GetLength();
    
   // Create global memory for sharing dragdrop text
   HGLOBAL hgData=GlobalAlloc(GPTR,numChar+1);   
   ASSERT(hgData!=NULL);
    
   LPSTR   lpData=(LPSTR)GlobalLock(hgData);
   ASSERT(lpData!=NULL);
                                                 
   strcpy(lpData, (const char*)csHolder);   
   
   // Cache the data
   OleDataSource.CacheGlobalData(uiOurListClipFormat, hgData);
   }

void CPhotoPane::OnUpdateEditCut(CCmdUI* pCmdUI) 
   {
   pCmdUI->Enable(GetNumSelected() > 0);	
   }

void CPhotoPane::OnEditCut() 
   {
   int numChar;
   COleDataSource OleDataSource; 
   
   CopyToClipboard(OURCLIP_PHOTO);
   CString csHolder;
   csHolder.LoadString(IDS_OLE_CBOARDHOLDER);

   // Get the text length
   numChar = csHolder.GetLength();
    
   // Create global memory for sharing dragdrop text
   HGLOBAL hgData=GlobalAlloc(GPTR,numChar+1);   
   ASSERT(hgData!=NULL);
    
   LPSTR   lpData=(LPSTR)GlobalLock(hgData);
   ASSERT(lpData!=NULL);
                                                 
   strcpy(lpData, (const char*)csHolder);   
   
   // Cache the data
   OleDataSource.CacheGlobalData(uiOurListClipFormat, hgData);              

   // mark selected files to delete
   MarkSelectedAsDelete();

   // delete items
   DeleteTaggedItems();

   OnTreeChanged(pRenderNode);
   }

void CPhotoPane::OnUpdateEditPaste(CCmdUI* pCmdUI) 
   {
   if (CLIPARRAY.IsTypeAvail(OURCLIP_PHOTO))
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
   }

void CPhotoPane::OnEditPaste() 
   {
   int i, idx;
   CPhoto* pPhoto;   
   CListCtrl& theCtrl = GetListCtrl();

   // default is at the end
   idx = theCtrl.GetItemCount();

   // get the first selected item
   POSITION pos = theCtrl.GetFirstSelectedItemPosition();   

   if (pos != NULL)	  
	  idx = theCtrl.GetNextSelectedItem(pos);	     
   
   // available?
   if (CLIPARRAY.IsTypeAvail(OURCLIP_PHOTO))
      {
	  // set drop zone
	  CLIPARRAY.SetDropNode(pRenderNode);

      // get all elements and insert them
      for (i = 0; i < CLIPARRAY.GetSize(); i++)
         {	
		 if (pPhoto = new CPhoto)
			{			
			// copy the data
			*pPhoto = *((CPhoto*)(CLIPARRAY.GetAt(i)));		
			InstantiateImageFile(pPhoto);
			pRenderNode->coNodes.InsertAt(PhotoIdxToNodeIdx(idx++), (CObject*)pPhoto);			
			}
		 }      	  
	  }
   //theCtrl.RedrawItems(0, theCtrl.GetItemCount() - 1);
   OnTreeChanged(pRenderNode);
   }

void CPhotoPane::OnUpdateEditDelete(CCmdUI* pCmdUI) 
   {
   pCmdUI->Enable(GetNumSelected() > 0);
   }

void CPhotoPane::OnEditDelete() 
   {
   DeleteSelected();
   }

void CPhotoPane::OnUpdatePhotosmenuPrint(CCmdUI* pCmdUI) 
   {
   pCmdUI->Enable((GetNumSelected() > 0) && (!PrintDlg.bInPrint));   
   }

void CPhotoPane::InstantiateImageFile(CPhoto* pPhoto)
   {
   AfxGetApp()->DoWaitCursor(1);

   // if pasting to a folder or drive, create a name based on the 
   // the display name and make sure that we don't overwrite
   if (pRenderNode->iType == NODETYPE_FOLDER ||
	   pRenderNode->iType == NODETYPE_NETWORKDRIVE ||
	   pRenderNode->iType == NODETYPE_LOCALDRIVE)
	  {
	  pPhoto->CopyNamedFile(pRenderNode->csName);
	  }
   else
	  {
	  // if pasting an order file, create a new temp file
	  pPhoto->CreateNewTempFile();	  
	  }
   AfxGetApp()->DoWaitCursor(-1);
   }

void CPhotoPane::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
   int iSelected;

   if (GetFocus() != this) return;

   if (!pRenderNode) return;

   // force preview bar display
   CMainFrame* pFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
   pFrm->SwitchMiddleView(SHOWVIEW_PREVIEWBAR);   

   // mark selected photos
   iSelected = MarkSelected();   

   // set photo pointer
   GetDocument()->m_pPreviewBar->UpdateInfo(pRenderNode, iSelected);
   //TRACE("UpdateInfo\r\n");


   *pResult = 0;
}

void CPhotoPane::OnOrdermenu2Addphotos() 
   {
   CJobView* pView = (CJobView*)GetDocument()->m_pJobView;

   pView->AddPhotos();	
   }

void CPhotoPane::OnUpdateOrdermenu2Addphotos(CCmdUI* pCmdUI) 
   {
   if (!pRenderNode) 
	  {
	  pCmdUI->Enable(FALSE);
	  return;
	  }

   if (pRenderNode->iType == NODETYPE_ORDER)
	  pCmdUI->Enable(TRUE);
   else
	  pCmdUI->Enable(FALSE);
   }

void CPhotoPane::PrintPhotosThreading(COrder* pOrder)
   {
   CMainFrame* pFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
   // if pOrder is not NULL, print ONLY this order
   if (pOrder)
	  {
	  PrintInfo.OrderNodes.RemoveAll();
	  PrintInfo.OrderNodes.Add((CObject*)pOrder);
	  }	       
   PrintInfo.csOrder.Empty();
   PrintInfo.csPhoto.Empty();
   PrintInfo.iThisPrintCount = 0;
   PrintInfo.iTotalPrintCount = 0;
   PrintInfo.iIndex = 0;
   PrintInfo.iMsgType = PRINTSTRINGS_NONE;
   PrintInfo.bIndexOnly = FALSE;
   PrintInfo.bPrintRet = FALSE;
   PrintInfo.bPromoPrint = FALSE;
   PrintInfo.pMainWnd = this;   

   PrintDlg.pPrintInfo = &PrintInfo;  

   if (!PrintDlg.bIsInit)   
      PrintDlg.Create(IDD_PRINT, this);
   else
	  {
	  if (!PrintDlg.bInPrint) PrintDlg.StartPrint();
	  }
   }

LRESULT CPhotoPane::OnPrintThreadMessage(WPARAM wParam, LPARAM lParam)
   {
   int i;
   COrder* pOrder;
   CAccount* pAccount;      
   CPhoto* pPhoto;

   switch(PrintInfo.iMsgType)
	  {
	  case PRINTSTRINGS_PRINTCOMPLETE:		   
		   // sanity check
		   if (!(pOrder = PrintInfo.pOrderPrint)) break;
		   if (!(pAccount = PrintInfo.pOrderPrint->pAccountOwner)) break;

			// delete the photos in the order
			for (i = PrintInfo.pOrderPrint->coNodes.GetSize() - 1; i >=0; i--)
			   {
			   if (pPhoto = (CPhoto*)PrintInfo.pOrderPrint->coNodes.GetAt(i))					
				  delete pPhoto;			
			   }
			PrintInfo.pOrderPrint->coNodes.RemoveAll();	
			delete PrintInfo.pOrderPrint;				
			break;
	  }						 
   return TRUE;
   }			  

void CPhotoPane::OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
{
CListCtrl& theCtrl = GetListCtrl();

NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

    *pResult = 0;

    // If this is the beginning of the control's paint cycle, request
    // notifications for each item.

    if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
        {
        *pResult = CDRF_NOTIFYITEMDRAW;
        }
    else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
        {
        // This is the pre-paint stage for an item.  We need to make another
        // request to be notified during the post-paint stage.

        *pResult = CDRF_NOTIFYPOSTPAINT;
        }
    else if ( CDDS_ITEMPOSTPAINT == pLVCD->nmcd.dwDrawStage )
        {
        // If this item is selected, re-draw the icon in its normal
        // color (not blended with the highlight color).

        LVITEM rItem;
        int    nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );

        // Get the image index and state of this item.  Note that we need to
        // check the selected state manually.  The docs _say_ that the
        // item's state is in pLVCD->nmcd.uItemState, but during my testing
        // it was always equal to 0x0201, which doesn't make sense, since
        // the max CDIS_* constant in commctrl.h is 0x0100.

        ZeroMemory ( &rItem, sizeof(LVITEM) );
        rItem.mask  = LVIF_IMAGE | LVIF_STATE;
        rItem.iItem = nItem;
        rItem.stateMask = LVIS_SELECTED;
        theCtrl.GetItem ( &rItem );

        // If this item is selected, redraw the icon with its normal colors.
        if ( rItem.state & LVIS_SELECTED )
            {
            CDC*  pDC = CDC::FromHandle ( pLVCD->nmcd.hdc );
            CRect rcIcon;

            // Get the rect that holds the item's icon.
            theCtrl.GetItemRect ( nItem, &rcIcon, LVIR_ICON );			
			
			if ((GetStyle() & LVS_REPORT) == LVS_REPORT)
			   {	
			   rcIcon.left += ((rcIcon.Width() - SMALLICONSIZE) / 2);
			   rcIcon.top += ((rcIcon.Height() - SMALLICONSIZE) / 2);
			   m_imageListSmall.Draw ( pDC, rItem.iImage, rcIcon.TopLeft(),
                             ILD_TRANSPARENT );
			   }
			else
			   {
			   rcIcon.left += (((rcIcon.Width() - iThumbnailSize) / 2) + 3);			   
			   rcIcon.top += ((rcIcon.Height() - iThumbnailSize) / 2);			   
			   m_imageListLarge.Draw ( pDC, rItem.iImage, rcIcon.TopLeft(),
                             ILD_TRANSPARENT );
			   }

            *pResult = CDRF_SKIPDEFAULT;
            }
        }
}

void CPhotoPane::OnEditDarken() 
   {   
   ApplyCorrection(IMAGEADJ_LIGHTDARK, -10);   
   }

void CPhotoPane::OnUpdateEditDarken(CCmdUI* pCmdUI) 
   {
   OnUpdateMaster(pCmdUI);
   }

void CPhotoPane::OnEditLighten() 
   {
   ApplyCorrection(IMAGEADJ_LIGHTDARK, 10);
   }

void CPhotoPane::OnUpdateEditLighten(CCmdUI* pCmdUI) 
   {
   OnUpdateMaster(pCmdUI);
   }

void CPhotoPane::OnEditMorecyan() 
   {
   ApplyCorrection(IMAGEADJ_CYAN, 10);
   }

void CPhotoPane::OnUpdateEditMorecyan(CCmdUI* pCmdUI) 
   {
   OnUpdateMaster(pCmdUI);
   }

void CPhotoPane::OnEditLesscyan() 
   {
   ApplyCorrection(IMAGEADJ_CYAN, -10);
   }

void CPhotoPane::OnUpdateEditLesscyan(CCmdUI* pCmdUI) 
   {
   OnUpdateMaster(pCmdUI);
   }

void CPhotoPane::OnEditRevert() 
   {
   ApplyCorrection(IMAGEADJ_REVERT);
   }

void CPhotoPane::OnUpdateEditRevert(CCmdUI* pCmdUI) 
   {
   OnUpdateMaster(pCmdUI);
   }

void CPhotoPane::OnEditMoremagenta() 
   {
   ApplyCorrection(IMAGEADJ_MAGENTA, 10);
   }

void CPhotoPane::OnUpdateEditMoremagenta(CCmdUI* pCmdUI) 
   {
   OnUpdateMaster(pCmdUI);
   }

void CPhotoPane::OnEditLessmagenta() 
   {
   ApplyCorrection(IMAGEADJ_MAGENTA, -10);
   }

void CPhotoPane::OnUpdateEditLessmagenta(CCmdUI* pCmdUI) 
   {
   OnUpdateMaster(pCmdUI);
   }

void CPhotoPane::OnEditMoreyellow() 
   {
   ApplyCorrection(IMAGEADJ_YELLOW, 10);
   }

void CPhotoPane::OnUpdateEditMoreyellow(CCmdUI* pCmdUI) 
   {
   OnUpdateMaster(pCmdUI);
   }

void CPhotoPane::OnEditLessyellow() 
   {
   ApplyCorrection(IMAGEADJ_YELLOW, -10);
   }

void CPhotoPane::OnUpdateEditLessyellow(CCmdUI* pCmdUI) 
   {
   OnUpdateMaster(pCmdUI);
   }

void CPhotoPane::OnEditAutolevels() 
   {   
   if (GetCheckState(CHECKADJUSTMENT_AUTOLEVEL) == 1)
      ApplyCorrection(IMAGEADJ_AUTOLEVELS, 0);
   else
	  ApplyCorrection(IMAGEADJ_AUTOLEVELS, 1);
   }

void CPhotoPane::OnUpdateEditAutolevels(CCmdUI* pCmdUI) 
   {
   OnUpdateMaster(pCmdUI);
   pCmdUI->SetCheck(GetCheckState(CHECKADJUSTMENT_AUTOLEVEL));
   }

void CPhotoPane::OnEditGrayscale() 
   {
   if (GetCheckState(CHECKADJUSTMENT_GRAYSCALE) == 1)
      ApplyCorrection(IMAGEADJ_GRAYSCALE, 0);
   else
	  ApplyCorrection(IMAGEADJ_GRAYSCALE, 1);
   }

void CPhotoPane::OnUpdateEditGrayscale(CCmdUI* pCmdUI) 
   {
   OnUpdateMaster(pCmdUI);
   pCmdUI->SetCheck(GetCheckState(CHECKADJUSTMENT_GRAYSCALE));
   }

void CPhotoPane::OnEditRedeye() 
   {
   if (GetCheckState(CHECKADJUSTMENT_REDEYE) == 1)
      ApplyCorrection(IMAGEADJ_REDEYE, 0);
   else
	  ApplyCorrection(IMAGEADJ_REDEYE, 1);
   }

void CPhotoPane::OnUpdateEditRedeye(CCmdUI* pCmdUI) 
   {
   CListCtrl& theCtrl = GetListCtrl();

   if (theCtrl.GetSelectedCount() == 1)
	  pCmdUI->Enable(TRUE);
   else
	  pCmdUI->Enable(FALSE);

   pCmdUI->SetCheck(GetCheckState(CHECKADJUSTMENT_REDEYE));
   }

void CPhotoPane::OnUpdateMaster(CCmdUI* pCmdUI) 
   {
   CListCtrl& theCtrl = GetListCtrl();

   if (theCtrl.GetSelectedCount() > 0)
	  pCmdUI->Enable(TRUE);
   else
	  pCmdUI->Enable(FALSE);
   }

void CPhotoPane::ApplyCorrection(int iType, int iAmt)
   {
   LVITEM lvis;   
   CPhoto* pPhoto;        
   HIMAGELIST hImageList;
   HBITMAP hBitmap;

   CListCtrl& theCtrl = GetListCtrl();

   // loop through the list and update the main image and the thumbnail
   ZeroMemory(&lvis, sizeof LVITEM);
   for (lvis.iItem = 0; lvis.iItem < theCtrl.GetItemCount(); lvis.iItem++)	  
	  {
	  lvis.mask = LVIF_PARAM | LVIF_STATE;
	  theCtrl.GetItem(&lvis); 	
   	  if (pPhoto = (CPhoto*)lvis.lParam)
		 {
		 if (theCtrl.GetItemState(lvis.iItem, LVIS_SELECTED))
			{
			GetDocument()->m_pPreviewBar->AdjustImage(pPhoto, iType, iAmt);			

			if ((GetStyle() & LVS_REPORT) == LVS_REPORT)
			   {	
			   // make or get the small thumbnail
			   hBitmap = pPhoto->GetWinBmp(SMALLICONSIZE);

			   // replace the holder bitmap with the real one
			   hImageList = (HIMAGELIST)m_imageListSmall;
			   if (hBitmap && hImageList) 
				  ImageList_Replace(hImageList, 
				  pPhoto->iImageIndex, hBitmap, (HBITMAP)NULL);	
			   }
			else
			   {
			   // make or get the thumbnail
			   hBitmap = pPhoto->GetWinBmp(iThumbnailSize);
			
			   // replace the holder bitmap with the real one
			   hImageList = (HIMAGELIST)m_imageListLarge;
			   if (hBitmap && hImageList) 
				  ImageList_Replace(hImageList, 
				  pPhoto->iImageIndex, hBitmap, (HBITMAP)NULL);		
			   }

			theCtrl.RedrawItems(lvis.iItem, lvis.iItem);	  
			}
		 }
	  }
   }

#define CHECKADJUSTMENT_GRAYSCALE 0
#define CHECKADJUSTMENT_AUTOLEVEL 1
#define CHECKADJUSTMENT_SEPIATONE 2

int CPhotoPane::GetCheckState(int iAdjustmentToCheck)
   {
   LVITEM lvis;   
   CPhoto* pPhoto;        
   CListCtrl& theCtrl = GetListCtrl();
   int iCheckState = 99;
   int iAdjustment;

   // loop through the list and set the bSelected flag
   ZeroMemory(&lvis, sizeof LVITEM);
   for (lvis.iItem = 0; lvis.iItem < theCtrl.GetItemCount(); lvis.iItem++)	  
	  {
	  lvis.mask = LVIF_PARAM | LVIF_STATE;
	  theCtrl.GetItem(&lvis); 	
   	  if (pPhoto = (CPhoto*)lvis.lParam)
		 {
		 if (theCtrl.GetItemState(lvis.iItem, LVIS_SELECTED))
			{
			switch(iAdjustmentToCheck)
			   {
			   case CHECKADJUSTMENT_GRAYSCALE:
				    iAdjustment = pPhoto->cImageAdjust.bDoGrayScale;
					break;
			   case CHECKADJUSTMENT_AUTOLEVEL:
				    iAdjustment = pPhoto->cImageAdjust.bUseAutoLevels;
					break;
			   case CHECKADJUSTMENT_SEPIATONE:
				    iAdjustment = pPhoto->cImageAdjust.bDoSepiaTone;
					break;
			   case CHECKADJUSTMENT_REDEYE:
				    iAdjustment = pPhoto->cImageAdjust.bInRedEyeMode;
					break;
			   default:
				    iAdjustment = 0;
					break;
			   }
			if (iAdjustment) 
			   {
			   // if state flag is 0 and this is not 0,
			   // we have an indeterminate state			   
			   if (iCheckState == 0)
				  {
				  iCheckState = 2;
				  break;
				  }
			   else
				  iCheckState = 1;
			   }
			else
			   {
			   // if state flag is 1 and this is not 1,
			   // we have an indeterminate state			   
			   if (iCheckState == 1)
				  {
				  iCheckState = 2;
				  break;
				  }
			   else
				  iCheckState = 0;
			   }
			}		 			 
		 }
	  }

   if (iCheckState == 99) iCheckState = 0;
   return iCheckState; 
   }


void CPhotoPane::OnEditRotateCw() 
   {
   ApplyCorrection(IMAGEADJ_ROTATE, 90);
   }

void CPhotoPane::OnUpdateEditRotateCw(CCmdUI* pCmdUI) 
   {
   OnUpdateMaster(pCmdUI);
   }

void CPhotoPane::OnEditRotateCcw() 
   {
   ApplyCorrection(IMAGEADJ_ROTATE, -90);
   }

void CPhotoPane::OnUpdateEditRotateCcw(CCmdUI* pCmdUI) 
   {
   OnUpdateMaster(pCmdUI);
   }

void CPhotoPane::OnEditUndo() 
   {
   ApplyCorrection(IMAGEADJ_UNDO);
   }

void CPhotoPane::OnUpdateEditUndo(CCmdUI* pCmdUI) 
   {
   LVITEM lvis;   
   CPhoto* pPhoto;        
   BOOL bCanUndo = FALSE;
   CListCtrl& theCtrl = GetListCtrl();

   // loop through the list and set the bSelected flag
   ZeroMemory(&lvis, sizeof LVITEM);
   for (lvis.iItem = 0; lvis.iItem < theCtrl.GetItemCount(); lvis.iItem++)	  
	  {
	  lvis.mask = LVIF_PARAM | LVIF_STATE;
	  theCtrl.GetItem(&lvis); 	
   	  if (pPhoto = (CPhoto*)lvis.lParam)
		 {
		 if (theCtrl.GetItemState(lvis.iItem, LVIS_SELECTED))
			{
			if (!pPhoto->bCanUndoImageAdjust)
			   {
			   bCanUndo = FALSE;
			   break;
			   }
			else
			   bCanUndo = TRUE;
			}
		 }
	  }

   if (bCanUndo)
	  pCmdUI->Enable(TRUE);
   else
	  pCmdUI->Enable(FALSE);
   }

void CPhotoPane::OnEditRedo() 
   {
   ApplyCorrection(IMAGEADJ_REDO);
   }

void CPhotoPane::OnUpdateEditRedo(CCmdUI* pCmdUI) 
   {
   LVITEM lvis;   
   CPhoto* pPhoto;        
   BOOL bCanRedo = FALSE;
   CListCtrl& theCtrl = GetListCtrl();

   // loop through the list and set the bSelected flag
   ZeroMemory(&lvis, sizeof LVITEM);
   for (lvis.iItem = 0; lvis.iItem < theCtrl.GetItemCount(); lvis.iItem++)	  
	  {
	  lvis.mask = LVIF_PARAM | LVIF_STATE;
	  theCtrl.GetItem(&lvis); 	
   	  if (pPhoto = (CPhoto*)lvis.lParam)
		 {
		 if (theCtrl.GetItemState(lvis.iItem, LVIS_SELECTED))
			{
			if (!pPhoto->bCanRedoImageAdjust)
			   {
			   bCanRedo = FALSE;
			   break;
			   }
			else
			   bCanRedo = TRUE;
			}
		 }
	  }

   if (bCanRedo)
	  pCmdUI->Enable(TRUE);
   else
	  pCmdUI->Enable(FALSE);
   }

void CPhotoPane::ResetThumbsBitmapList()
   {
   HIMAGELIST hImageList; 
   CListCtrl& theCtrl = GetListCtrl();
   
   ::DeleteObject(m_imageListLarge.Detach());

   // get thumbnail size
   iThumbnailSize = AfxGetApp()->GetProfileInt("Display", "ThumbSize", THUMBNAILSIZE);

   // Create the image list with iThumbnailSize*iThumbnailSize icons and 24 bpp color depth
   // a maximum of MAXIMAGES images
   hImageList=ImageList_Create(iThumbnailSize, iThumbnailSize, 
	  ILC_COLOR24, 0, MAXIMAGES);   
   m_imageListLarge.Attach(hImageList);

   // Use the image list in the list view
   theCtrl.SetImageList(&m_imageListLarge, LVSIL_NORMAL);   

   theCtrl.SetExtendedStyle (theCtrl.GetExtendedStyle() 
	  | LVS_EX_FULLROWSELECT);
   theCtrl.SetIconSpacing(CSize(iThumbnailSize + 10, iThumbnailSize + 20));
   }

void CPhotoPane::OnEditMorecontrast() 
   {
   ApplyCorrection(IMAGEADJ_CONTRAST, 10);
   }

void CPhotoPane::OnUpdateEditMorecontrast(CCmdUI* pCmdUI) 
   {
   OnUpdateMaster(pCmdUI);
   }

void CPhotoPane::OnEditLesscontrast() 
   {
   ApplyCorrection(IMAGEADJ_CONTRAST, -10);
   }

void CPhotoPane::OnUpdateEditLesscontrast(CCmdUI* pCmdUI) 
   {
   OnUpdateMaster(pCmdUI);
   }

void CPhotoPane::OnEditMoresharp() 
   {
   ApplyCorrection(IMAGEADJ_SHARPEN, 10);
   }

void CPhotoPane::OnUpdateEditMoresharp(CCmdUI* pCmdUI) 
   {
   OnUpdateMaster(pCmdUI);
   }

void CPhotoPane::OnEditLessSharp() 
   {
   ApplyCorrection(IMAGEADJ_SHARPEN, -10);
   }

void CPhotoPane::OnUpdateEditLessSharp(CCmdUI* pCmdUI) 
   {
   OnUpdateMaster(pCmdUI);
   }

void CPhotoPane::OnViewCropview() 
   {   
   if (!pRenderNode) return;

   CMainFrame* pFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
   pFrm->SwitchMiddleView(SHOWVIEW_PREVIEWBAR);   

   GetDocument()->m_pPreviewBar->ViewCropview();
   }

void CPhotoPane::OnUpdateViewCropview(CCmdUI* pCmdUI) 
   {
   if (!pRenderNode) return;

   CMainFrame* pFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
   pFrm->SwitchMiddleView(SHOWVIEW_PREVIEWBAR);   
   
   GetDocument()->m_pPreviewBar->UpdateViewCropview(pCmdUI);
   }

void CPhotoPane::OnViewPhotoview() 
   {
   if (!pRenderNode) return;

   CMainFrame* pFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
   pFrm->SwitchMiddleView(SHOWVIEW_PREVIEWBAR);   

   GetDocument()->m_pPreviewBar->ViewPhotoview();
   }

void CPhotoPane::OnUpdateViewPhotoview(CCmdUI* pCmdUI) 
   {
   if (!pRenderNode) return;

   CMainFrame* pFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
   pFrm->SwitchMiddleView(SHOWVIEW_PREVIEWBAR); 
   
   pCmdUI->SetCheck(!GetDocument()->m_pPreviewBar->bCropView);
   }

// we need public methods for these
void CPhotoPane::UpdateEditRedo(CCmdUI* pCmdUI) 
   {
   OnUpdateEditRedo(pCmdUI);
   }

void CPhotoPane::UpdateEditUndo(CCmdUI* pCmdUI) 
   {
   OnUpdateEditUndo(pCmdUI);
   }

void CPhotoPane::OnDropFiles(HDROP hDropInfo) 
   {
   int i, NumDropped;
   CString csFile;
   CStringArray csaPhotos;

   // get the number of files dropped
   NumDropped = DragQueryFile(hDropInfo, 0xffffffff, NULL, 0);

   if (NumDropped < 1) return;

   // insert the dropped files
   for (i = 0; i < NumDropped; i++)
      {
	  // get file
	  DragQueryFile(hDropInfo, i, csFile.GetBuffer(_MAX_PATH), _MAX_PATH);
	  csFile.ReleaseBuffer();
	  csaPhotos.Add(csFile);
	  }
   CJobView* pView = (CJobView*)GetDocument()->m_pJobView;
   pView->AddThesePhotos(csaPhotos);
   }

void CPhotoPane::ResetAdjustmentDelta()
   {
   GetDocument()->m_pPreviewBar->ResetAdjustmentDelta();
   }

void CPhotoPane::OnEditPhotoshop() 
   {
   EditWithPhotoshop();
   }

void CPhotoPane::UpdateEditPhotoshop(CCmdUI* pCmdUI) 
   {
   OnUpdateEditPhotoshop(pCmdUI);
   }

void CPhotoPane::OnUpdateEditPhotoshop(CCmdUI* pCmdUI) 
   {
   CListCtrl& theCtrl = GetListCtrl();
   CLSID clsid;
   BOOL bPhotoshopFound = FALSE;

   USES_CONVERSION;

   if (CLSIDFromProgID(T2COLE("Photoshop.Application"), &clsid) == S_OK)
	  bPhotoshopFound = TRUE;

   if (theCtrl.GetSelectedCount() == 1 && bPhotoshopFound)
	  pCmdUI->Enable(TRUE);
   else
	  pCmdUI->Enable(FALSE);
   }

void CPhotoPane::EditWithPhotoshop()
   {
   LVITEM lvis;   
   CPhoto* pPhoto;        
   CPhoto* pPhotoSel = NULL;           
   CListCtrl& theCtrl = GetListCtrl();
   IPhotoshopApplication app;
   IAutoPSDoc doc;
   CDlgWait dlg;
   CString csImageEdit;
   CPhoto* pNewPhoto;
   CImage* pImage;
   CImage* pOriginalImage;
   FILEINFO LeadFileInfo;
   HWND hwndPhotoshop = NULL;
   HWND hwndMDI = NULL;
   HWND hwndDoc = NULL;
   char strWinTitle[_MAX_PATH];
   BOOL bDocFound = FALSE;
   char strDocTitle[_MAX_PATH];
   char ext[_MAX_EXT];

   // create the edit folder
   // than that the folder already exists
   CPhotogizeUtils::GetAppPath(csImageEdit);
   csImageEdit += "ImageEdit\\";
   if ((::CreateDirectory(csImageEdit, NULL) == 0) && 
	  ::GetLastError() != ERROR_ALREADY_EXISTS)
	  {
	  AfxMessageBox(IDS_CANTCREATEIMAGEEDITDIR);
	  return;
	  }

   // loop through the list and get the selected photo
   ZeroMemory(&lvis, sizeof LVITEM);
   for (lvis.iItem = 0; lvis.iItem < theCtrl.GetItemCount(); lvis.iItem++)	  
	  {
	  lvis.mask = LVIF_PARAM | LVIF_STATE;
	  theCtrl.GetItem(&lvis); 	
   	  if (pPhoto = (CPhoto*)lvis.lParam)
		 {
		 if (theCtrl.GetItemState(lvis.iItem, LVIS_SELECTED))
			{
			pPhotoSel = pPhoto;
			break;
			}
		 }
	  }

   if (pPhotoSel)
	  {
	  // this will create a temp file with the proper extension
	  if (pNewPhoto = new CPhoto)
		 {
		 // copy the old one
		 *pNewPhoto = *pPhotoSel;
		 
		 // and instantiate the new file
		 if (pNewPhoto->CopyNamedFile(csImageEdit))
			{
			if (pImage = pNewPhoto->GetOriginalImage())
			   {  
			   dlg.m_prompt.Format("Editing %s in Photoshop\nYou must close Photoshop or the document to continue", 
				  pNewPhoto->csDisplayName);
			   dlg.Create(IDD_MODELESS_WAIT, this);   

			   if (app.CreateDispatch("Photoshop.Application"))
				  {
				  app.SetVisible(TRUE);
				  doc = app.Open(pImage->csFileName);   
				  // get the root name of the template file
				  _splitpath(pImage->csFileName, NULL, NULL, strDocTitle, ext);
				  strcat(strDocTitle, ext);
				  do
					 {
					 bDocFound = FALSE;
					 Sleep(100);
					 // is photoshop open?
					 if (hwndPhotoshop = ::FindWindow((LPTSTR)"Photoshop", (LPTSTR)NULL))
						{
						// find the client window
						if (hwndMDI = ::FindWindowEx(hwndPhotoshop, NULL, "MDIClient", (LPTSTR)NULL))
						   {
						   hwndDoc = NULL;
						   do
							  {							 
							  // cycle through the docs
							  if (hwndDoc = ::FindWindowEx(hwndMDI, hwndDoc, "PSDocC", (LPTSTR)NULL))
								 {
								 // is our doc still open?
								 ::GetWindowText(hwndDoc, strWinTitle, _MAX_PATH);
								 if (strstr(strWinTitle, strDocTitle) != NULL)
									bDocFound = TRUE;					 
								 }
							  } while (hwndDoc);
						   } 
						}
					 } while (bDocFound);
				  dlg.DestroyWindow();
				  }
			   else
				  {
				  dlg.DestroyWindow();
				  AfxMessageBox(IDS_PSHOPNOTFOUND, MB_OK | MB_SETFOREGROUND);
				  }
			   }
			}
		 if (pOriginalImage	= pPhotoSel->GetOriginalImage())
			{
			if (AfxMessageBox(IDS_PHOTOSHOPCHANGES, MB_YESNO) == IDYES)
			   {
		       // copy it back
		       CPhotogizeUtils::CopyFile(pImage->csFileName, pOriginalImage->csFileName, FALSE);

			   // reset the image dims
			   if (::L_FileInfo((char*)(const char*)pOriginalImage->csFileName, 
				  &LeadFileInfo, FILEINFO_TOTALPAGES, NULL) > 0)            
				  {
				  pOriginalImage->iWidth = LeadFileInfo.Width;
				  pOriginalImage->iHeight = LeadFileInfo.Height;
				  }			 

			   // clear the memory cache
			   pPhotoSel->ClearMemoryCache();
			   // and the disk cache
			   pPhotoSel->ClearDiskCache(pOriginalImage->csFileName, 
				  pOriginalImage->ctFileTime, pOriginalImage->FileSize);

			   // redraw the thumb
			   theCtrl.RedrawItems(lvis.iItem, lvis.iItem);	  
			   }
			// delete the temp file
			::DeleteFile(pImage->csFileName);
			}
		 delete pNewPhoto;
		 }
	  }
   }

void CPhotoPane::KeyDown(UINT nChar)
   {
   int oldItem, newItem;
   POSITION pos;  
   int iSelected;
   CListCtrl& theCtrl = GetListCtrl();

   //TRACE("Key Down = %d\n", nChar);   

   switch(nChar)
	  {
	  case VK_HOME:
		   // get the first selected item
		   if (pos = theCtrl.GetFirstSelectedItemPosition())
			  {
			  oldItem = theCtrl.GetNextSelectedItem(pos);
			  newItem = 0;
			  theCtrl.SetItemState(oldItem, 0, TVIS_SELECTED);
			  theCtrl.SetItemState(newItem, TVIS_SELECTED, TVIS_SELECTED);
			  
			  // mark selected photos
			  iSelected = MarkSelected();   
			  
			  // set photo pointer
			  GetDocument()->m_pPreviewBar->UpdateInfo(pRenderNode, iSelected);
			  }
		   break;
 	  case VK_END:	
		   // get the first selected item
		   if (pos = theCtrl.GetFirstSelectedItemPosition())
			  {
			  oldItem = theCtrl.GetNextSelectedItem(pos);
			  newItem = theCtrl.GetItemCount() - 1;
			  theCtrl.SetItemState(oldItem, 0, TVIS_SELECTED);
			  theCtrl.SetItemState(newItem, TVIS_SELECTED, TVIS_SELECTED);
			  
			  // mark selected photos
			  iSelected = MarkSelected();   
			  
			  // set photo pointer
			  GetDocument()->m_pPreviewBar->UpdateInfo(pRenderNode, iSelected);
			  }
		   break;
	  case VK_PRIOR:
	  case VK_LEFT:
	  case VK_UP:
		   // get the first selected item
		   if (pos = theCtrl.GetFirstSelectedItemPosition())
			  {
			  oldItem = theCtrl.GetNextSelectedItem(pos);
			  if (oldItem > 0)
				 newItem = oldItem - 1;
			  else
				 newItem = theCtrl.GetItemCount() - 1;
			  theCtrl.SetItemState(oldItem, 0, TVIS_SELECTED);
			  theCtrl.SetItemState(newItem, TVIS_SELECTED, TVIS_SELECTED);
			  
			  // mark selected photos
			  iSelected = MarkSelected();   
			  
			  // set photo pointer
			  GetDocument()->m_pPreviewBar->UpdateInfo(pRenderNode, iSelected);
			  }
		   break;
	  case VK_RIGHT:
	  case VK_NEXT:
	  case VK_DOWN:           
		   // get the first selected item
		   if (pos = theCtrl.GetFirstSelectedItemPosition())
			  {
			  oldItem = theCtrl.GetNextSelectedItem(pos);
			  if (oldItem < (theCtrl.GetItemCount() - 1))
				 newItem = oldItem + 1;
			  else
				 newItem = 0;				 				  
			  theCtrl.SetItemState(oldItem, 0, TVIS_SELECTED);
			  theCtrl.SetItemState(newItem, TVIS_SELECTED, TVIS_SELECTED);
			  
			  // mark selected photos
			  iSelected = MarkSelected();   
			  
			  // set photo pointer
			  GetDocument()->m_pPreviewBar->UpdateInfo(pRenderNode, iSelected);
			  }
		   break;
	  default:
		 if (pos = theCtrl.GetFirstSelectedItemPosition())			
			newItem = theCtrl.GetNextSelectedItem(pos);
		   break;
	  }

   ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_wndMiniPhotoDetails.m_order = pRenderNode->csDisplayName;
   ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_wndMiniPhotoDetails.m_status.Format("%d of %d", newItem + 1, theCtrl.GetItemCount());
   ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_wndMiniPhotoDetails.UpdateData(FALSE);
   }

void CPhotoPane::OnEditZoomin() 
   {
   GetDocument()->m_pPreviewBar->OurZoomin();
   }

void CPhotoPane::OnEditZoomout() 
   {
   GetDocument()->m_pPreviewBar->OurZoomout();
   }

void CPhotoPane::OnUpdateEditZoomin(CCmdUI* pCmdUI) 
   {
   CListCtrl& theCtrl = GetListCtrl();

   if (theCtrl.GetSelectedCount() == 1)
	  pCmdUI->Enable(TRUE);
   else
	  pCmdUI->Enable(FALSE);
   }

void CPhotoPane::OnUpdateEditZoomout(CCmdUI* pCmdUI) 
   {
   CListCtrl& theCtrl = GetListCtrl();

   if (theCtrl.GetSelectedCount() == 1)
	  pCmdUI->Enable(TRUE);
   else
	  pCmdUI->Enable(FALSE);
   }

void CPhotoPane::DoContextMenu()
   {
   SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());
   }

void CPhotoPane::OnPhotosmenuPrintOne() 
   {
   PhotosmenuPrint(TRUE);
   }

void CPhotoPane::OnUpdatePhotosmenuPrintOne(CCmdUI* pCmdUI) 
   {      
   pCmdUI->Enable((GetNumSelected() > 0) && (!PrintDlg.bInPrint));
   }

void CPhotoPane::RedrawItem(int idx)
   {
   if (!pRenderNode) return;

   CListCtrl& theCtrl = GetListCtrl();

   theCtrl.RedrawItems(idx, idx);
   }

void CPhotoPane::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
   {   
   COrder* pOrder;

   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

   CListCtrl& theCtrl = GetListCtrl();

   theCtrl.SortItems(SortPhotos, pNMListView->iSubItem);

   // save the sort order
   if (pOrder = (COrder*)pRenderNode)
	  {
	  pOrder->iLastSortColumn = pNMListView->iSubItem;
	  pOrder->bLastSortOrder = bSortOrder[pNMListView->iSubItem];	  
	  }
   
   // flip the sort order
   bSortOrder[pNMListView->iSubItem] = -bSortOrder[pNMListView->iSubItem];
   
   *pResult = 0;
   }

LRESULT CPhotoPane::OnKioskDlgMessage(WPARAM wParam, LPARAM lParam)
   {
   KIOSKDLGCOMM* pKioskDlgComm;   	 

   if (!(pKioskDlgComm = (KIOSKDLGCOMM*)wParam))
	  return FALSE;
   
   switch(pKioskDlgComm->iMsgType)
	  {
	  case KIOSKDLGCOMMMSG_PRINTPHOTO:
		   PrintKioskPhoto(pKioskDlgComm->pOrder, pKioskDlgComm->pPhoto);
		   break;		    
	  }						 
   return TRUE;
   }

void CPhotoPane::PrintKioskPhoto(COrder* pParentOrder, CPhoto* pPhotoPassed) 
   {   
   CString csTemp;
   CPhoto* pPhoto;   
   COrder* pOrder;   
   CPhotogizeUtils utils;   
   CAccount* pAccount;

   // alloc a new order
   if (!(pOrder = new COrder)) return;

   // default: assign a unique name
   utils.GetUniqueName(csTemp);
   pOrder->csName = csTemp;
   pOrder->csDisplayName = csTemp;

   // try to use the data from the parent order
   if (pParentOrder)
	  {	  
	  if (pParentOrder->iType == NODETYPE_ORDER)
		 {
	     *pOrder = *pParentOrder;					  
		 pOrder->OrderDetails.ParseDetails();
		 }
	  }						  		  

   pOrder->bPrintIndex = FALSE;

   // copy the photo so that it doesn't get clobbered
   // when we delete the order
   if (pPhoto = new CPhoto)
	  {			   
	  *pPhoto = *pPhotoPassed;
	  // print just a single copy
	  pPhoto->iCopies = 1;
	  // any will do
	  pAccount = pPhoto->pAccountOwner;
	  pOrder->coNodes.Add(pPhoto);
	  }		 			

   if (!pAccount) 
	  {
	  delete pOrder;
	  return;
	  }

   if (pOrder->coNodes.GetSize() > 0)
	  {	  	
	  pOrder->pAccountOwner = pAccount;

      PrintPhotosThreading(pOrder);
	  }
   }
*/
