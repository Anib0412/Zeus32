/////////////////////////////////////////////////////////////////////////////
// NewPreview.cpp
//                                                                        
// preview classes for RasterPlus
// Copyright (C) 1999 Graphx, Inc.
//
// 12-06-99 Version 3.0.2 - make preview location sticky
// 09-28-01 Version 5.0.0 - Added High_Defintion Preview support
// 10-01-01 Version 5.0.0 - Fix Preview All not displaying current preview image while processing
// 10-24-01 Version 5.0.0 - Calculate zoom to fit value for HD Preview
// 04-15-03 Version 5.1.2 - Modified to support Lead Tools 13
//
#include "stdafx.h"
#include "zeus.h"
#include "NewPreview.h"
#include "RipStruct.h"
#include "profile.h"
#include "inidefs.h"
#include "rip.h"

#include "..\CmnLibs\Current LeadTools\include\l_bitmap.h"
#include "..\CmnLibs\Current LeadTools\include\l_error.h"
#include "ltwnd.h"

const UINT NEAR wm_PrintJobID = RegisterWindowMessage( RIP_PRINTJOBID );

#define STATUSHEIGHT    45


#define TRY_LEADTOOLS_13

// preview architecture
//
// CPreviewManager holds an array of CPreview objects
// Each CPreview holds an array of CPreviewPage objects
// Each CPreviewPage contains displayable image data
//

const UINT NEAR wm_PrevCtrl = RegisterWindowMessage( "RP_Preview_Ctrl_Msg" );
const UINT NEAR wm_PausePreview = RegisterWindowMessage( "RP_PausePreview" );
const UINT NEAR wm_SetPreviewJob = RegisterWindowMessage( "RP_SetPreviewJob" );

#define PREVCTRL_PREV	  0
#define PREVCTRL_NEXT	  1
#define PREVCTRL_BEGIN    2
#define PREVCTRL_END	  3
#define PREVCTRL_PRINT    4
#define PREVCTRL_ZOOMIN   5
#define PREVCTRL_ZOOMOUT  6

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
   
/////////////////////////////////////////////////////////////////////////////
// CPreviewManager

CPreviewManager::CPreviewManager()
   {
   }

CPreviewManager::~CPreviewManager()
   {
   CloseAll();
   }

int CPreviewManager::NumActive()
   {
   // how many preview arrays do we have?
   return coPreviewWinArray.GetSize();
   }

void CPreviewManager::CloseAll()
   {
   int i = 0;
   CPreview* pPrev;

   // destroy the page array
   while (i < coPreviewWinArray.GetSize())
      {
	  pPrev = (CPreview*)coPreviewWinArray.GetAt(i++);
	  // destroy the windows if they exist
	  if (pPrev->WindowsExist)
		 {						
		 pPrev->cwPreviewPane.DestroyWindow();
		 pPrev->DestroyWindow();
		 //pPrev->m_bNotCreated = TRUE;
		 //pPrev->ToolTipCtrl.DestroyWindow();
		 }
	  // and delete the object
      delete pPrev;
      }     
   coPreviewWinArray.RemoveAll();
   }

int CPreviewManager::PrevSetInfo(WPARAM wParam, LPARAM lParam)
   {		   
   // if the height and width differ from our current height and
   // width OR if the multiple windows is set, create a new Preview
   // otherwise, feed the image to the closest matching preview
   // window
   int i;   
   int iRet = SYN_ERRMSG_NOPREVIEW;
   L_CHAR szFileName[255]="";
   CPreview* pPrev;
   CPreviewPage* pPreviewPage;
   BOOL bOpenAnotherOne = TRUE;
   CProfile Profile;   
   PREVIEWINFO PreviewInfo;

   // get preview info
   PreviewInfo = *((PPREVIEWINFO)lParam);

   // try to delete the job if it already exists
   DeletePreviewJob(PreviewInfo.iJobID, PreviewInfo.uiRipSessionID);

   // do we want to force the use of individual windows
   // for each view?
   BOOL bUseMultipleWindows = Profile.GetInt(IDS_PREVIEW, IDS_ALWAYS_OPEN, FALSE);
   if (!bUseMultipleWindows)
	  {
	  // otherwise, lets search our existing Preview window
	  // array for a size match
      for (i = 0; i < coPreviewWinArray.GetSize(); i++)
		 {
		 // get the new preview
         pPrev = (CPreview*)coPreviewWinArray.GetAt(i);

		 // a match?
		 if ((pPrev->iCurrentHeight == PreviewInfo.Height) &&
			 (pPrev->iCurrentWidth == PreviewInfo.Width))
			{
			// don't need to open another preview
			bOpenAnotherOne = FALSE;
			// let's just use this one
			iCurrentPreview = i;
			break;
			}
		 }
	  }

	// we need to open another preview
	if (bOpenAnotherOne)
	{	  
		// create a preview
		if ((pPrev = new CPreview) != NULL)
		{
			// add the preview to our array
			coPreviewWinArray.Add(pPrev);
			// send the info to Preview
			iRet = pPrev->PrevSetInfo(wParam, lParam);

			// create the windows if necessary
			if (!pPrev->WindowsExist)
			{
				if ( PreviewInfo.iPreviewType == RIPSTYLE_PREVIEW )
					CreateWindows(pPrev);
				else
				{
					pPreviewPage = (CPreviewPage*)(pPrev->coPreviewPageArray[pPrev->iCurrentPane]);
					CreateHDWindows(pPrev, pPreviewPage);
					pPreviewPage->PreviewInfo.iZoomValue = iZoomForFit;
					pPreviewPage->PreviewInfo.hWndParent = pPrev->cwPreviewPaneHD.m_hWnd;
				}
			}
			else
			{
				if ( PreviewInfo.iPreviewType == RIPSTYLE_HD_PREVIEW )
				{
					pPreviewPage = (CPreviewPage*)(pPrev->coPreviewPageArray[pPrev->iCurrentPane]);
					pPreviewPage->PreviewInfo.hWndParent = pPrev->cwPreviewPaneHD.m_hWnd;
					pPreviewPage->PreviewInfo.iZoomValue = iZoomForFit;
					DisplayHDPreview(pPrev->cwPreviewPaneHD.m_hWnd, pPreviewPage);
				}
			}

			// set title
			pPrev->SetWindowText(PreviewInfo.Title);
			// bring to front
			pPrev->SetWindowPos(&pPrev->wndTop, 
			0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);

			if ( PreviewInfo.iPreviewType == RIPSTYLE_PREVIEW )
			{
				pPrev->cwPreviewPane.Invalidate(TRUE);

				// reset the ruler
				pPrev->SetcRectRuler(&PreviewInfo);
				pPrev->SetRulerRanges();
			}

			// set count
			pPrev->pcdStatus->SetCount(pPrev->iCurrentPane + 1, 
						pPrev->coPreviewPageArray.GetSize(), PreviewInfo.iPreviewType);		 
		}
	}
	else
	{
		// get the current preview
		pPrev = (CPreview*)coPreviewWinArray.GetAt(iCurrentPreview);

		// send the info to Preview
		iRet = pPrev->PrevSetInfo(wParam, lParam);

		// create the windows if necessary
		if (!pPrev->WindowsExist)
		{
			if ( PreviewInfo.iPreviewType == RIPSTYLE_PREVIEW )
				CreateWindows(pPrev);
			else
			{
				pPreviewPage = (CPreviewPage*)(pPrev->coPreviewPageArray[pPrev->iCurrentPane]);
				CreateHDWindows(pPrev, pPreviewPage);
				pPreviewPage->PreviewInfo.iZoomValue = iZoomForFit;
				pPreviewPage->PreviewInfo.hWndParent = pPrev->cwPreviewPaneHD.m_hWnd;
			}
		}
		else
		{
			if ( PreviewInfo.iPreviewType == RIPSTYLE_HD_PREVIEW )
			{
				pPreviewPage = (CPreviewPage*)(pPrev->coPreviewPageArray[pPrev->iCurrentPane]);
				pPreviewPage->PreviewInfo.hWndParent = pPrev->cwPreviewPaneHD.m_hWnd;
				pPreviewPage->PreviewInfo.iZoomValue = iZoomForFit;
				DisplayHDPreview(pPrev->cwPreviewPaneHD.m_hWnd, pPreviewPage);
			}
		}

		// set title
		pPrev->SetWindowText(PreviewInfo.Title);
		// bring to front
		pPrev->SetWindowPos(&pPrev->wndTop, 
								0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);

		// force a repaint
		if ( PreviewInfo.iPreviewType == RIPSTYLE_PREVIEW )
		    pPrev->cwPreviewPane.Invalidate(TRUE);

		// set count
		pPrev->pcdStatus->SetCount(pPrev->iCurrentPane + 1, 
				pPrev->coPreviewPageArray.GetSize(), PreviewInfo.iPreviewType);	  
	}		

   // should we pause?
   int PauseFlag = Profile.GetInt(IDS_PREVIEW, IDS_PREVIEW_PAUSE, IDDM_PREVIEW_PAUSE);	   
   if (PauseFlag)
      {
	  ::SendMessage(PreviewInfo.hWndParent, wm_PausePreview, 0, 0);
	  }
   return iRet;
   }

void CPreviewManager::CreateWindows(CPreview* pPrev)
   {
   DWORD dwCanvasStyle;
   DWORD dwCanvasParentStyle;   
   CRect ParentRect;
   CProfile Profile;
   int m_use_rulers;

   // set window styles
   dwCanvasStyle = WS_CHILD | WS_VISIBLE;      	  
   dwCanvasParentStyle = WS_POPUPWINDOW | WS_VISIBLE | WS_MINIMIZEBOX | WS_CAPTION;

   m_use_rulers = Profile.GetInt(IDS_PREVIEW, IDS_USERULERS, IDDM_PREVIEW_USERULERS);   

   // calculate the dimensions of the parent window and display pane
   // factoring in the status bar
	CRect PaneRect(0, 0, pPrev->iCurrentWidth, 
	   pPrev->iCurrentHeight);		
	::AdjustWindowRect(&PaneRect, dwCanvasStyle, FALSE);	

	if (m_use_rulers)
	   {
	   ParentRect.bottom = PaneRect.bottom + STATUSHEIGHT + RULER_HEIGHT;
	   ParentRect.right = PaneRect.right + RULER_HEIGHT;
	   }
	else
	   {
	   ParentRect.bottom = PaneRect.bottom + STATUSHEIGHT;
	   ParentRect.right = PaneRect.right;	   
	   }
   ParentRect.top = PaneRect.top;
   ParentRect.left = PaneRect.left;
   ::AdjustWindowRect(&ParentRect, dwCanvasParentStyle, FALSE);		

   // get the last position of the window
   int iLastPosX, iLastPosY;   
   iLastPosY = Profile.GetInt(IDS_PREVIEW, IDS_LASTWINDOWPOSY, 10);
   iLastPosX = Profile.GetInt(IDS_PREVIEW, IDS_LASTWINDOWPOSX, 10);

    // create the preview parent
   pPrev->CreateEx(0,
				  AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
						::LoadCursor(NULL, IDC_ARROW),       	   
							(HBRUSH)::GetStockObject(WHITE_BRUSH), 
								AfxGetApp()->LoadIcon(IDI_ICON_PREVIEW)),
				  "Preview",
				  dwCanvasParentStyle,
				  iLastPosX,
				  iLastPosY,
				  ParentRect.Width(),      
				  ParentRect.Height(),
				  AfxGetMainWnd()->GetSafeHwnd(),
				  NULL);        

   if (m_use_rulers)
	  {
	  // create the rulers
	  pPrev->CornerRuler.SetStyle(RULER_STYLE_CORNER);
      pPrev->CornerRuler.CreateEx(WS_EX_CLIENTEDGE,
	      AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,       
	      ::LoadCursor(NULL, IDC_ARROW),       	   
	      (HBRUSH)::GetStockObject(GRAY_BRUSH), NULL),
	      NULL, WS_CHILD | WS_VISIBLE,
		  0, 0, 
		  RULER_HEIGHT, RULER_HEIGHT, 
		  pPrev->m_hWnd, NULL);        
	  // set ruler ranges based on units
	  //SetcRectRuler(&PreviewInfo);
	  //SetRulerRanges();
	  // create the rulers
	  pPrev->TopRuler.SetStyle(RULER_STYLE_TOP);	  
      pPrev->TopRuler.CreateEx(WS_EX_CLIENTEDGE,
	      AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,       
	      ::LoadCursor(NULL, IDC_ARROW),       	   
	      (HBRUSH)::GetStockObject(GRAY_BRUSH), NULL),
	      NULL, WS_CHILD | WS_VISIBLE,
		  RULER_HEIGHT, 0, 
		  PaneRect.Width(), RULER_HEIGHT, 
		  pPrev->m_hWnd, NULL);  
      pPrev->LeftRuler.SetStyle(RULER_STYLE_LEFT);	  
      pPrev->LeftRuler.CreateEx(WS_EX_CLIENTEDGE,
	      AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,       
	      ::LoadCursor(NULL, IDC_ARROW),       	   
	      (HBRUSH)::GetStockObject(GRAY_BRUSH), NULL),
	      NULL, WS_CHILD | WS_VISIBLE,
		  0, RULER_HEIGHT, 
		  RULER_HEIGHT, PaneRect.Height(),
		  pPrev->m_hWnd, NULL);        
   // create the display pane
   pPrev->cwPreviewPane.CreateEx(0,
      AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,       
	  ::LoadCursor(NULL, IDC_ARROW),       	   
	  (HBRUSH)::GetStockObject(WHITE_BRUSH), NULL),
	  "Preview",
      dwCanvasStyle, RULER_HEIGHT, RULER_HEIGHT, PaneRect.Width(), 
	  PaneRect.Height(), pPrev->m_hWnd, NULL);        	  	    
	  }
   else
	  {
      // create the display pane
      pPrev->cwPreviewPane.CreateEx(0,
         AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,       
	     ::LoadCursor(NULL, IDC_ARROW),       	   
	     (HBRUSH)::GetStockObject(WHITE_BRUSH), NULL),
	     "Preview",
      dwCanvasStyle, 0, 0, PaneRect.Width(), 
	  PaneRect.Height(), pPrev->m_hWnd, NULL);        	  	    
	  }

   pPrev->WindowsExist = TRUE;
   }


void CPreviewManager::CreateHDWindows(CPreview* pPrev, CPreviewPage* pPreviewPage )
{
	DWORD dwCanvasStyle;
	DWORD dwCanvasParentStyle;   
	CRect ParentRect;
	CProfile Profile;

	int PreviewHeight, PreviewWidth;

	double devAspectRatio;

	// For High Defintion Preview
	//
#ifdef TRY_LEADTOOLS_13
//	L_UseBitmapClass();
#endif

 	// set window styles
	dwCanvasParentStyle = WS_POPUPWINDOW | WS_VISIBLE | WS_MINIMIZEBOX | WS_CAPTION;
//	dwCanvasStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | L_BS_THUMBTRACK;
	dwCanvasStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | SB_THUMBTRACK;
	devAspectRatio = (double)pPreviewPage->PreviewInfo.RipPixHeight /
								(double)pPreviewPage->PreviewInfo.RipPixWidth *
										pPreviewPage->PreviewInfo.RipWPixelsPerInch /
											pPreviewPage->PreviewInfo.RipHPixelsPerInch;  

	if (devAspectRatio < 1.0)
	{
//		PreviewWidth = (int)((float)GetSystemMetrics(SM_CXSCREEN) / 2.F + .5F);
		PreviewWidth = (int)((float)GetSystemMetrics(SM_CXSCREEN) * 2.F / 3.F + .5F);
		// set ripstruct pix width
		PreviewHeight = (int)((float)PreviewWidth * ((float)pPreviewPage->PreviewInfo.RipPixHeight / pPreviewPage->PreviewInfo.RipHPixelsPerInch) /  
							((float)pPreviewPage->PreviewInfo.RipPixWidth / pPreviewPage->PreviewInfo.RipWPixelsPerInch) + .5F);
	}
	else
	{
//		PreviewHeight = (int)((float)GetSystemMetrics(SM_CYSCREEN) / 2.F + .5F);
		PreviewHeight = (int)((float)GetSystemMetrics(SM_CXSCREEN) * 2.F / 3.F + .5F);
		// set ripstruct pix width
		PreviewWidth = (int)((float)PreviewHeight * ((float)pPreviewPage->PreviewInfo.RipPixWidth / pPreviewPage->PreviewInfo.RipWPixelsPerInch) /  
								((float)pPreviewPage->PreviewInfo.RipPixHeight / pPreviewPage->PreviewInfo.RipHPixelsPerInch) + .5F);
	}

	// calculate the dimensions of the parent window and display pane
	// factoring in the status bar
	CRect PaneRect(0, 0, PreviewWidth, PreviewHeight);		
	::AdjustWindowRect(&PaneRect, dwCanvasStyle, FALSE);	

	ParentRect.bottom = PaneRect.bottom + STATUSHEIGHT;
	ParentRect.right = PaneRect.right;	   

	ParentRect.top = PaneRect.top;
	ParentRect.left = PaneRect.left;
	::AdjustWindowRect(&ParentRect, dwCanvasParentStyle, FALSE);		

	// get the last position of the window
	int iLastPosX, iLastPosY;   
	iLastPosY = Profile.GetInt(IDS_PREVIEW, IDS_LASTWINDOWPOSY, 10);
	iLastPosX = Profile.GetInt(IDS_PREVIEW, IDS_LASTWINDOWPOSX, 10);

	pPrev->CreateEx(0,
				  AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, ::LoadCursor(NULL, IDC_ARROW),       	   
										(HBRUSH)::GetStockObject(WHITE_BRUSH), AfxGetApp()->LoadIcon(IDI_ICON_PREVIEW)),
				  "High Definition Preview",
				  dwCanvasParentStyle,
				  iLastPosX,
				  iLastPosY,
				  ParentRect.Width(),      
				  ParentRect.Height(),
				  AfxGetMainWnd()->GetSafeHwnd(),
				  NULL);        

   pPrev->cwPreviewPaneHD.CreateEx(0,
				  L_BITMAPCLASS,
				  "High Definition Preview",
			      dwCanvasStyle,
				  0,
				  0,
				  PaneRect.Width(), 
				  PaneRect.Height(),
				  pPrev->m_hWnd,
				  NULL);
   
	// Calc Fit to Window Factor
//	iZoomForFit = (int)((((float)PreviewHeight / (float)pPreviewPage->PreviewInfo.RipPixHeight) + 0.5F) * 100);
	if ( pPreviewPage->PreviewInfo.Height < pPreviewPage->PreviewInfo.Width )
	{
	    iZoomForFit = (int)((((float)PreviewHeight / (float)pPreviewPage->PreviewInfo.RipPixHeight) + 0.005F) * 100);

		if ( pPreviewPage->PreviewInfo.RipPixHeight == pPreviewPage->PreviewInfo.Height )
		{
			pPreviewPage->PreviewInfo.iInitialZoomValue = 1; 
		}
		else
		{
			pPreviewPage->PreviewInfo.iInitialZoomValue = (int)((float)pPreviewPage->PreviewInfo.RipPixHeight / ((float)PreviewHeight) + 0.5F);
		}
	}
	else
	{
	    iZoomForFit = (int)((((float)PreviewWidth / (float)pPreviewPage->PreviewInfo.RipPixWidth) + 0.005F) * 100);

		if ( pPreviewPage->PreviewInfo.RipPixWidth == pPreviewPage->PreviewInfo.Width )
		{
			pPreviewPage->PreviewInfo.iInitialZoomValue = 1; 
		}
		else
		{
			pPreviewPage->PreviewInfo.iInitialZoomValue = (int)((float)pPreviewPage->PreviewInfo.RipPixWidth / ((float)PreviewWidth) + 0.5F);
		}
	}

	pPreviewPage->PreviewInfo.iZoomValue = iZoomForFit;

	if ( DisplayHDPreview( pPrev->cwPreviewPaneHD.m_hWnd, pPreviewPage ))
	{
		::ShowWindow(pPrev->cwPreviewPaneHD.m_hWnd, TRUE);

		pPrev->cwPreviewPaneHD.Invalidate();
		pPrev->WindowsExist = TRUE;
	}
}

void CPreviewManager::SetPreviewActiveJob(int iJobID)
   {
   int i, j;   
   CPreview* pPrev;
   CPreviewPage* pPreviewPage;
   BOOL bFoundIt = FALSE;

   // loop through the coPreviewWinArray to find a matching page
   for (i = 0; i < coPreviewWinArray.GetSize(); i++)
      {
	  // get the preview
	  pPrev = (CPreview*)coPreviewWinArray.GetAt(i);

	  // make sure it exists
	  if (!pPrev->WindowsExist) continue;

	  // loop through the pages in the array
	  for (j = 0; j < pPrev->coPreviewPageArray.GetSize(); j++)
		 {
		 pPreviewPage = (CPreviewPage*)pPrev->coPreviewPageArray.GetAt(j);
		 // found it!
		 if (pPreviewPage->PreviewInfo.iJobID == iJobID)
			{
			// set current pane	      
			pPrev->iCurrentPane = j;

			if ( pPreviewPage->PreviewInfo.iPreviewType == RIPSTYLE_PREVIEW )
			{
				// point to the proper display page
				pPrev->cwPreviewPane.pDisplay = &pPreviewPage->cd;   
				// set title
				pPrev->SetWindowText(pPreviewPage->csTitle);
				// force a repaint
				pPrev->cwPreviewPane.Invalidate(TRUE);
			}
			else
			{
				// set title
				pPrev->SetWindowText(pPreviewPage->csTitle);
				DisplayHDPreview(pPrev->cwPreviewPaneHD.m_hWnd, pPreviewPage);
			}
			// set count
			pPrev->pcdStatus->SetCount(pPrev->iCurrentPane + 1, pPrev->coPreviewPageArray.GetSize(), 
																	pPreviewPage->PreviewInfo.iPreviewType);			
			// bring to top
			pPrev->BringWindowToTop();
            // reset the ruler
            pPrev->SetcRectRuler(&pPreviewPage->PreviewInfo);
            pPrev->SetRulerRanges();
			bFoundIt = TRUE;
			break;
			}
		 }				  
	  if (bFoundIt) break;
      }     	 
   }

void CPreviewManager::DeletePreviewJob(int iJobID, UINT uiRipSessionID, 
									   BOOL bIgnore_uiRipSessionID)
   {
   int i, j;   
   CPreview* pPrev;
   CPreviewPage* pPreviewPage;
   BOOL bBreakOut = FALSE;

   // loop through the coPreviewWinArray to find a matching page
   for (i = 0; i < coPreviewWinArray.GetSize(); i++)
      {
	  // get the preview
	  pPrev = (CPreview*)coPreviewWinArray.GetAt(i);

	  // loop through the pages in the array
	  for (j = 0; j < pPrev->coPreviewPageArray.GetSize(); j++)
		 {
		 pPreviewPage = (CPreviewPage*)pPrev->coPreviewPageArray.GetAt(j);
		 // if the iJobID are the same but the uiRipSessionID is different,
		 // delete the preview page
		 if (pPreviewPage->PreviewInfo.iJobID == iJobID && 
			 (pPreviewPage->PreviewInfo.uiRipSessionID != uiRipSessionID ||
			  bIgnore_uiRipSessionID))
			{			
			// get rid of it
			pPrev->coPreviewPageArray.RemoveAt(j);
			delete pPreviewPage;			
			bBreakOut = TRUE;
			break;
			}
		 }	 
	  // if we deleted the last pane, destroy the coPreviewPageArray
	  if (pPrev->coPreviewPageArray.GetSize() == 0 && bBreakOut)
		 {
		 // destroy the windows if they exist
		 if (pPrev->WindowsExist)
			{						
			pPrev->cwPreviewPane.DestroyWindow();
			pPrev->DestroyWindow();
			}
		 // remove from the coPreviewWinArray
		 coPreviewWinArray.RemoveAt(i);
		 // and delete the object
		 delete pPrev;
		 }
	  if (bBreakOut) break;
      }     
   }

int CPreviewManager::DisplayHDPreview(HWND hWnd, CPreviewPage* pPreviewPage )
{
	LOADFILEOPTION LoadFileOption;
	FILEINFO FileInfo;

#ifdef LEADTOOLS_13
	LEADBITMAPLOAD LoadStruct;
#endif

	L_GetDefaultLoadFileOption(&LoadFileOption, sizeof LOADFILEOPTION);

	memset(&FileInfo, 0, sizeof(FILEINFO));
    FileInfo.uStructSize = sizeof(FILEINFO);
	L_FileInfo (pPreviewPage->PreviewInfo.LongName, &FileInfo, sizeof(FILEINFO), 0, &LoadFileOption);

	iZoom = pPreviewPage->PreviewInfo.iZoomValue * pPreviewPage->PreviewInfo.iInitialZoomValue;

	if (SendMessage (hWnd, L_BM_GETBITMAP, 0, 0) != 0)
	{
		SendMessage (hWnd, L_BM_FREE, 0, 0);
		SendMessage (hWnd, L_BM_SETZOOM, TRUE, iZoom);
	}

#ifdef LEADTOOLS_13
	LoadStruct.pOptions = NULL;
	lstrcpy(LoadStruct.Name, pPreviewPage->PreviewInfo.LongName );
	LoadStruct.StructSize = sizeof (LoadStruct);
#endif

	if ( SendMessage (hWnd, L_BM_COPYBITMAP, TRUE, (LONG)pPreviewPage->cd.GetBitmap()) != SUCCESS )
	{
		MessageBox(NULL, "Unable to display HD preview", "RasterPlus", MB_OK );
		return FALSE;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CPreviewPage

CPreviewPage::CPreviewPage()
   {   
   }

CPreviewPage::~CPreviewPage()
   {
   }

/////////////////////////////////////////////////////////////////////////////
// CPreview

CPreview::CPreview()
   {
   // we have no width or height setting
   iCurrentHeight = 0;
   iCurrentWidth = 0;
   iCurrentPane = 0;
   iZoom = 0;
   WindowsExist = FALSE;
   pcdStatus = NULL;
//   pPreviewPane = NULL;
//   pPreviewPaneHD = NULL;
   }

CPreview::~CPreview()
   {
   int i = 0;

   // destroy the page array
   while (i < coPreviewPageArray.GetSize())
      {
      delete coPreviewPageArray.GetAt(i++);
      }     
   coPreviewPageArray.RemoveAll();

   // delete the status dialog bar
   if (pcdStatus)
	  {
	  pcdStatus->DestroyWindow();
	  delete pcdStatus;
	  pcdStatus = NULL;
	  }
   }  

BEGIN_MESSAGE_MAP(CPreview, CWnd)
	//{{AFX_MSG_MAP(CPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_REGISTERED_MESSAGE(wm_PrevCtrl, OnPrevCtrl)				
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CPreview::PrevSetInfo(WPARAM wParam, LPARAM lParam)
   {
   CPreviewPage* pPreviewPage;   
   CProfile Profile;

   // create a new page
   if ((pPreviewPage = new CPreviewPage) == NULL) 
	  return SYN_ERRMSG_NOPREVIEW;

   // here is where we get the preview information
   pPreviewPage->PreviewInfo = *((PPREVIEWINFO)lParam);

   SetcRectRuler(&pPreviewPage->PreviewInfo);
   SetRulerRanges();

   // set current dims
   iCurrentHeight = pPreviewPage->PreviewInfo.Height;
   iCurrentWidth = pPreviewPage->PreviewInfo.Width;

   // set title
   pPreviewPage->csTitle = pPreviewPage->PreviewInfo.Title;

   // try to create the actual display image
   // storage is allocated here
   if (!(pPreviewPage->cd.CreateImage(&pPreviewPage->PreviewInfo)))	  	  
	  {
	  // problem creating the image
	  delete pPreviewPage;
	  return SYN_ERRMSG_NOPREVIEW;
      }

   // now add it to the array
   coPreviewPageArray.Add(pPreviewPage);

   // get auto advance flag
   int m_auto_advance = Profile.GetInt(IDS_PREVIEW, IDS_AUTOADVANCE, TRUE);

   // if flag is set OR we have no initial display, do it
   if (m_auto_advance || !cwPreviewPane.pDisplay || !cwPreviewPane.GetSafeHwnd())
	  {
	   if ( pPreviewPage->PreviewInfo.iPreviewType == RIPSTYLE_PREVIEW )
	   {
		  // set the display pointer to this one
		  cwPreviewPane.pDisplay = &pPreviewPage->cd;   

		  // reset the ruler
		  SetcRectRuler(&pPreviewPage->PreviewInfo);
		  SetRulerRanges();								  
	   }
		  
	   // set current pane (it's at the end)
	   iCurrentPane = coPreviewPageArray.GetSize() - 1;
	  }														     
   return SYN_ERRMSG_ALLOK;
   }

void CPreview::SetcRectRuler(PPREVIEWINFO pPreviewInfo)
   {
   float devAspectRatio =
      (float)pPreviewInfo->RipPixHeight / 
	  (float)pPreviewInfo->RipPixWidth *
      pPreviewInfo->RipWPixelsPerInch / 
	  pPreviewInfo->RipHPixelsPerInch;

   // get width and height based on orientation
   if (devAspectRatio < 1.0)
      {
	  cRectRuler.left = pPreviewInfo->crMediaRect.top;
	  cRectRuler.right = pPreviewInfo->crMediaRect.bottom;
	  cRectRuler.top = pPreviewInfo->crMediaRect.left;
	  cRectRuler.bottom = pPreviewInfo->crMediaRect.right;	  
	  }    	  
    else
      {	  
	  cRectRuler.left = pPreviewInfo->crMediaRect.left;
	  cRectRuler.right = pPreviewInfo->crMediaRect.right;
	  cRectRuler.top = pPreviewInfo->crMediaRect.top;
	  cRectRuler.bottom = pPreviewInfo->crMediaRect.bottom;	  
	  }      
   // normalize the coords   
   cRectRuler.NormalizeRect();
   }

/////////////////////////////////////////////////////////////////////////////
// CPreviewPane

CPreviewPane::CPreviewPane()
   {
   pDisplay = NULL;
   }

CPreviewPane::~CPreviewPane()
   {
   }

BEGIN_MESSAGE_MAP(CPreviewPane, CWnd)
	//{{AFX_MSG_MAP(CPreviewPane)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPreviewPaneHD

CPreviewPaneHD::CPreviewPaneHD()
{
}

CPreviewPaneHD::~CPreviewPaneHD()
{
}

BEGIN_MESSAGE_MAP(CPreviewPaneHD, CWnd)
	//{{AFX_MSG_MAP(CPreviewPane)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CPreview::OnCreate(LPCREATESTRUCT lpCreateStruct) 
   {
   if (CWnd::OnCreate(lpCreateStruct) == -1)
      return -1;	

   pcdStatus = new CPrevCtrl;
	
   pcdStatus->m_bNotCreated = TRUE;

   // create the dialog box at the bottom of the window
   pcdStatus->Create(this, IDD_PREV_CTRL, CBRS_BOTTOM, 4020);

   return 0;
   }

void CPreview::OnSize(UINT nType, int cx, int cy) 
   {
   CWnd::OnSize(nType, cx, cy);

   pcdStatus->SetWindowPos(NULL, 0, cy - STATUSHEIGHT, cx, 
	  STATUSHEIGHT, SWP_SHOWWINDOW | SWP_NOZORDER);
   }		  

void CPreviewPane::OnPaint() 
   {
   CPaintDC dc(this); // device context for painting

   if (pDisplay)
	  {             
      RECT Rect;
      GetClientRect(&Rect);
      pDisplay->DisplayImage(&Rect, dc);         
      }	       
   }		   

void CPreview::OnClose() 
{
	CPreviewPage* pPreviewPage;

	WindowsExist = FALSE;

	int i = 0;

	// destroy the page array
	while (i < coPreviewPageArray.GetSize())
	{
		pPreviewPage = (CPreviewPage*)coPreviewPageArray.GetAt(i++);
//		if ( pPreviewPage->PreviewInfo.iPreviewType == RIPSTYLE_HD_PREVIEW )
//			::DeleteFile(pPreviewPage->PreviewInfo.LongName);

		delete pPreviewPage;
	}     
	coPreviewPageArray.RemoveAll();

	pcdStatus->DestroyWindow();

	delete pcdStatus;

	pcdStatus = NULL;

    iZoom = 0;

	CWnd::OnClose();
}
		   
LRESULT CPreview::OnPrevCtrl(WPARAM wParam, LPARAM lParam)
{
	CPreviewPage* pPreviewPage;

	LOADFILEOPTION LoadFileOption;
	FILEINFO FileInfo;
//	LEADBITMAPLOAD LoadStruct;
	pBITMAPHANDLE pImageBitmap;

	// get last pPreviewPage
	CPreviewPage* pLastPreviewPage = (CPreviewPage*)coPreviewPageArray.GetAt(iCurrentPane);

	if ( pLastPreviewPage->PreviewInfo.iPreviewType == RIPSTYLE_PREVIEW )
	{
		switch(wParam)
		{
		case PREVCTRL_BEGIN:
			iCurrentPane = 0;
			break;
		case PREVCTRL_NEXT:
			iCurrentPane++;
			break;
		case PREVCTRL_PREV:
			iCurrentPane--;
			break;
		case PREVCTRL_END:
			iCurrentPane = coPreviewPageArray.GetSize() - 1;
			break;
		case PREVCTRL_PRINT:
			// get job id of current page
			pPreviewPage = (CPreviewPage*)coPreviewPageArray.GetAt(iCurrentPane);
			// send a message to print this page of this job
			::SendMessage(pPreviewPage->PreviewInfo.hWndParent, wm_PrintJobID, 
							(WPARAM)pPreviewPage->PreviewInfo.iJobID, 
								(LPARAM)pPreviewPage->PreviewInfo.iPageNum);   	  
			break;
		case PREVCTRL_ZOOMIN:
		case PREVCTRL_ZOOMOUT:
			break;
		}

		// check bounds
		iCurrentPane = min(max(iCurrentPane, 0), coPreviewPageArray.GetSize() - 1);

		// get the current preview page   
		pPreviewPage = (CPreviewPage*)coPreviewPageArray.GetAt(iCurrentPane);

		// set title
		SetWindowText(pPreviewPage->csTitle);

		// point to it
		cwPreviewPane.pDisplay = &pPreviewPage->cd;   

		// force a repaint
		cwPreviewPane.Invalidate(TRUE);

		// reset the ruler
		SetcRectRuler(&pPreviewPage->PreviewInfo);
		SetRulerRanges();
	}
	else
	{
		switch(wParam)
		{
		case PREVCTRL_BEGIN:
			iCurrentPane = 0;
			break;
		case PREVCTRL_NEXT:
			iCurrentPane++;
			break;
		case PREVCTRL_PREV:
			iCurrentPane--;
			break;
		case PREVCTRL_END:
			iCurrentPane = coPreviewPageArray.GetSize() - 1;
			break;
		case PREVCTRL_PRINT:
			// get job id of current page
			pPreviewPage = (CPreviewPage*)coPreviewPageArray.GetAt(iCurrentPane);
			// send a message to print this page of this job
			::SendMessage(pPreviewPage->PreviewInfo.hWndParent, wm_PrintJobID, 
							(WPARAM)pPreviewPage->PreviewInfo.iJobID, 
								(LPARAM)pPreviewPage->PreviewInfo.iPageNum);   	  
			break;
		case PREVCTRL_ZOOMIN:
			pPreviewPage = (CPreviewPage*)coPreviewPageArray.GetAt(iCurrentPane);

			if ( iZoom == 0 )
				iZoom = pPreviewPage->PreviewInfo.iZoomValue * pPreviewPage->PreviewInfo.iInitialZoomValue;

			if ( iZoom >= pPreviewPage->PreviewInfo.iZoomValue )
				iZoom += pPreviewPage->PreviewInfo.iZoomValue / 2;
			else
				iZoom += pPreviewPage->PreviewInfo.iZoomValue / 10;
			::SendMessage(pPreviewPage->PreviewInfo.hWndParent, L_BM_SETZOOM, (WPARAM)TRUE, (LPARAM)iZoom);
			break;
		case PREVCTRL_ZOOMOUT:
			pPreviewPage = (CPreviewPage*)coPreviewPageArray.GetAt(iCurrentPane);

			if ( iZoom == 0 )
				iZoom = pPreviewPage->PreviewInfo.iZoomValue * pPreviewPage->PreviewInfo.iInitialZoomValue;

			if ( iZoom <= pPreviewPage->PreviewInfo.iZoomValue )
				iZoom -= pPreviewPage->PreviewInfo.iZoomValue / 10;
			else
				iZoom -= pPreviewPage->PreviewInfo.iZoomValue / 2;
			iZoom = max(1, iZoom);
			::SendMessage(pPreviewPage->PreviewInfo.hWndParent, L_BM_SETZOOM, (WPARAM)TRUE, (LPARAM)iZoom);
			break;
		}

		// check bounds
		iCurrentPane = min(max(iCurrentPane, 0), coPreviewPageArray.GetSize() - 1);

		// get the current preview page   
		pPreviewPage = (CPreviewPage*)coPreviewPageArray.GetAt(iCurrentPane);

		// set title
		SetWindowText(pPreviewPage->csTitle);

		if ( wParam != PREVCTRL_ZOOMIN && wParam != PREVCTRL_ZOOMOUT )
		{
			L_GetDefaultLoadFileOption(&LoadFileOption, sizeof LOADFILEOPTION);

			memset(&FileInfo, 0, sizeof(FILEINFO));
			FileInfo.uStructSize = sizeof(FILEINFO);
			L_FileInfo (pPreviewPage->PreviewInfo.LongName, &FileInfo, sizeof(FILEINFO), 0, &LoadFileOption);
		
			if (::SendMessage (pPreviewPage->PreviewInfo.hWndParent, L_BM_GETBITMAP, 0, 0) != 0)
			{
				::SendMessage (pPreviewPage->PreviewInfo.hWndParent, L_BM_FREE, 0, 0);
				::SendMessage (pPreviewPage->PreviewInfo.hWndParent, L_BM_SETZOOM, TRUE, iZoom);
			}

			pImageBitmap = pPreviewPage->cd.GetBitmap();
			if ( ::SendMessage (pPreviewPage->PreviewInfo.hWndParent, L_BM_COPYBITMAP, TRUE, (LONG)pImageBitmap) != SUCCESS )
			{
				::MessageBox(NULL, "L_BM_LOAD FAILED", "NewPreview.Cpp", MB_OK );
			}
		}
	}

	// set count
	pcdStatus->SetCount(iCurrentPane + 1, coPreviewPageArray.GetSize(), pPreviewPage->PreviewInfo.iPreviewType);   

	// if the new preview page is a different job (implying a non-multi page
	// file), then tell the parent to highlight a new job
	if (pPreviewPage->PreviewInfo.iJobID != pLastPreviewPage->PreviewInfo.iJobID)
		// tell the parent that we've changed the preview
		::SendMessage(pPreviewPage->PreviewInfo.hWndParent, wm_SetPreviewJob, 
													pPreviewPage->PreviewInfo.iJobID, 0);

	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CPrevCtrl dialog


CPrevCtrl::CPrevCtrl()
   {
   m_bNotCreated = TRUE;
   }

void CPrevCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrevCtrl)		
	DDX_Control(pDX, IDC_CLOSE, m_close);
	DDX_Text(pDX, IDC_COUNT, m_count);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrevCtrl, CDialogBar)
	//{{AFX_MSG_MAP(CPrevCtrl)
	ON_BN_CLICKED(IDC_BEGIN, OnBegin)
	ON_BN_CLICKED(IDC_END, OnEnd)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_PREV, OnPrev)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_WM_GETDLGCODE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_PRINT, OnPrint)
	ON_BN_CLICKED(IDC_ZOOMIN, OnZoomIn)
	ON_BN_CLICKED(IDC_ZOOMOUT, OnZoomOut)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrevCtrl message handlers

void CPrevCtrl::OnBegin() 
   {   
   GetParent()->PostMessage(wm_PrevCtrl, PREVCTRL_BEGIN);
   }

void CPrevCtrl::OnEnd() 
   {   
   GetParent()->PostMessage(wm_PrevCtrl, PREVCTRL_END);
   }

void CPrevCtrl::OnNext() 
   {   
   GetParent()->PostMessage(wm_PrevCtrl, PREVCTRL_NEXT);
   }

void CPrevCtrl::OnPrev() 
   {   
   GetParent()->PostMessage(wm_PrevCtrl, PREVCTRL_PREV);
   }

void CPrevCtrl::SetCount(int Page, int Total, int iPreviewType)
   {
   m_count.Format("%d of %d", Page, Total);
   SetDlgItemText(IDC_COUNT, m_count);

   if (m_bNotCreated)
	  {
	  secbNext.AttachButton(IDC_NEXT, SECBitmapButton::Al_Center, IDB_PREVCTRL_NEXT, this);
      secbPrev.AttachButton(IDC_PREV, SECBitmapButton::Al_Center, IDB_PREVCTRL_PREV, this);
      secbBegin.AttachButton(IDC_BEGIN, SECBitmapButton::Al_Center, IDB_PREVCTRL_BEGIN, this);
      secbEnd.AttachButton(IDC_END, SECBitmapButton::Al_Center, IDB_PREVCTRL_END, this);
	  secbClose.AttachButton(IDC_CLOSE, SECBitmapButton::Al_Center, IDB_PREVCTRL_CLOSE, this);
	  secbPrint.AttachButton(IDC_PRINT, SECBitmapButton::Al_Center, IDB_PREVCTRL_PRINT, this);
	  secbZoomIn.AttachButton(IDC_ZOOMIN, SECBitmapButton::Al_Center, IDB_PREVCTRL_ZOOMIN, this);
	  secbZoomOut.AttachButton(IDC_ZOOMOUT, SECBitmapButton::Al_Center, IDB_PREVCTRL_ZOOMOUT, this);
	  m_bNotCreated = FALSE;
	  
	  // add the tools
	  ToolTipCtrl.AddTool(&secbNext, IDS_PREVCTRL_TTIP_NEXT);
	  ToolTipCtrl.AddTool(&secbPrev, IDS_PREVCTRL_TTIP_PREV);
	  ToolTipCtrl.AddTool(&secbBegin, IDS_PREVCTRL_TTIP_BEGIN);
	  ToolTipCtrl.AddTool(&secbEnd, IDS_PREVCTRL_TTIP_END);
	  ToolTipCtrl.AddTool(&secbClose, IDS_PREVCTRL_TTIP_CLOSE);
	  ToolTipCtrl.AddTool(&secbPrint, IDS_PREVCTRL_TTIP_PRINT);
	  ToolTipCtrl.AddTool(&secbZoomIn, IDS_PREVCTRL_TTIP_ZOOMIN);
	  ToolTipCtrl.AddTool(&secbZoomOut, IDS_PREVCTRL_TTIP_ZOOMOUT);
	  }   

   // set buttons
   if (Page == 1)
	  {
	  GetDlgItem(IDC_BEGIN)->EnableWindow(FALSE);
	  GetDlgItem(IDC_PREV)->EnableWindow(FALSE);	  
	  }
   else
	  {
	  GetDlgItem(IDC_BEGIN)->EnableWindow(TRUE);
	  GetDlgItem(IDC_PREV)->EnableWindow(TRUE);	  
	  }

   if (Page == Total)
	  {
	  GetDlgItem(IDC_NEXT)->EnableWindow(FALSE);
	  GetDlgItem(IDC_END)->EnableWindow(FALSE);	  
	  }
   else
	  {
	  GetDlgItem(IDC_NEXT)->EnableWindow(TRUE);
	  GetDlgItem(IDC_END)->EnableWindow(TRUE);	  
	  }

   if ( iPreviewType == RIPSTYLE_PREVIEW )
   {
	  GetDlgItem(IDC_ZOOMIN)->ShowWindow(SW_HIDE);
	  GetDlgItem(IDC_ZOOMOUT)->ShowWindow(SW_HIDE);
   }
   //UpdateData(FALSE);
   }

void CPrevCtrl::OnClose() 
   {
   // tell the parent to close
   GetParent()->SendMessage(WM_CLOSE);	
   }

BOOL CPrevCtrl::PreTranslateMessage(MSG* pMsg) 
   {
   // route this to the tool tip engine
   if (!m_bNotCreated) ToolTipCtrl.RelayEvent(pMsg);
	
   return CDialogBar::PreTranslateMessage(pMsg);
   }

UINT CPrevCtrl::OnGetDlgCode() 
{
	return DLGC_WANTARROWS|DLGC_WANTALLKEYS;
	
	return CDialogBar::OnGetDlgCode();
}

int CPrevCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
   {
   if (CDialogBar::OnCreate(lpCreateStruct) == -1)
      return -1;
   //create the control
   ToolTipCtrl.Create(this, TTS_ALWAYSTIP);	  
   // enable tool tips
   EnableToolTips();	
   return 0;
   }

#define INCH_TO_MM(a) ((int)((float)a * 25.4F + .5F))
void CPreview::SetRulerRanges()
   {
   int iTest, iTick;
   CProfile Profile;

   int m_rulers_units = Profile.GetInt(IDS_PREVIEW, IDS_RULERSUNITS, IDDM_PREVIEW_RULERSUNITS);

   // redo the rulers
   switch(m_rulers_units)
	  {
	  case XFORM_UNITS_PICA:    
		   iTest = (cRectRuler.right * 72 - cRectRuler.left * 72) / 1000;
		   iTick = max(iTest * 100, 100);		   
           TopRuler.SetRange(cRectRuler.left * 72, cRectRuler.right * 72, 			  
			  10, iTick);
           LeftRuler.SetRange(cRectRuler.top * 72, cRectRuler.bottom * 72, 			  
			  10, iTick);
		   break;
	  case XFORM_UNITS_INCH:
		   iTest = (cRectRuler.right - cRectRuler.left) / 1000;
		   iTick = max(iTest * 100, 100);		   
           TopRuler.SetRange(cRectRuler.left, cRectRuler.right, 			  
			  10, iTick);
           LeftRuler.SetRange(cRectRuler.top, cRectRuler.bottom, 			  
			  10, iTick);
		   break;
	  case XFORM_UNITS_MM:
		   iTest = (INCH_TO_MM(cRectRuler.right) - INCH_TO_MM(cRectRuler.left)) / 1000;
		   iTick = max(iTest * 100, 100);		   
           TopRuler.SetRange(INCH_TO_MM(cRectRuler.left), INCH_TO_MM(cRectRuler.right), 
			  10, iTick);
           LeftRuler.SetRange(INCH_TO_MM(cRectRuler.top), INCH_TO_MM(cRectRuler.bottom), 
			  10, iTick);
		   break;		   	 
	  }
   if (TopRuler.GetSafeHwnd()) TopRuler.Invalidate();
   if (LeftRuler.GetSafeHwnd()) LeftRuler.Invalidate();
   }

void CPreview::OnDestroy() 
   {
   CProfile Profile;
   RECT Rect;
   GetWindowRect(&Rect);

   // write the last position of the window
   Profile.WriteInt(IDS_PREVIEW, IDS_LASTWINDOWPOSY, Rect.top);
   Profile.WriteInt(IDS_PREVIEW, IDS_LASTWINDOWPOSX, Rect.left);

   CWnd::OnDestroy();
   }

void CPreview::OnSetFocus(CWnd* pOldWnd) 
{
//	TRACE("NewPreview, CPreview: OnSetFocus\r\n");

	// check bounds
	iCurrentPane = min(max(iCurrentPane, 0), coPreviewPageArray.GetSize() - 1);

	if ( iCurrentPane < 0 )
		return;

	// get the current preview page   
	CPreviewPage* pPreviewPage = (CPreviewPage*)coPreviewPageArray.GetAt(iCurrentPane);
	if (WindowsExist)
	{
		// set title
		SetWindowText(pPreviewPage->csTitle);

		if ( pPreviewPage->PreviewInfo.iPreviewType == RIPSTYLE_PREVIEW )
		{
			// point to it
			cwPreviewPane.pDisplay = &pPreviewPage->cd;

			// force a repaint
			cwPreviewPane.Invalidate(TRUE);

			// reset the ruler
			SetcRectRuler(&pPreviewPage->PreviewInfo);
			SetRulerRanges();
		}

		// set count
		pcdStatus->SetCount(iCurrentPane + 1, coPreviewPageArray.GetSize(), pPreviewPage->PreviewInfo.iPreviewType); 	  

		// tell the parent that we've changed the preview
		::SendMessage(pPreviewPage->PreviewInfo.hWndParent, wm_SetPreviewJob,    
													pPreviewPage->PreviewInfo.iJobID, 0);	  	  
	}

	CWnd::OnSetFocus(pOldWnd);
}

void CPreview::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
   {   
//   TRACE("OnKeyDown = %d\r\n", nChar);

   switch(nChar)
	  {
	  case VK_END:
		   OnPrevCtrl(PREVCTRL_END, 0);
		   break;
	  case VK_HOME:
		   OnPrevCtrl(PREVCTRL_BEGIN, 0);
		   break;
	  case VK_PRIOR:
	  case VK_LEFT:
	  case VK_UP:
		   OnPrevCtrl(PREVCTRL_PREV, 0);
		   break;
	  case VK_RIGHT:
	  case VK_NEXT:
	  case VK_DOWN:
		   OnPrevCtrl(PREVCTRL_NEXT, 0);
		   break;
	  }
   CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
   }

void CPrevCtrl::OnPrint() 
   {
   GetParent()->PostMessage(wm_PrevCtrl, PREVCTRL_PRINT);
   }

void CPrevCtrl::OnZoomIn() 
{
	GetParent()->PostMessage(wm_PrevCtrl, PREVCTRL_ZOOMIN);
}

void CPrevCtrl::OnZoomOut() 
{
	GetParent()->PostMessage(wm_PrevCtrl, PREVCTRL_ZOOMOUT);
}

void CPrevCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
   {
   CDialogBar::OnKeyDown(nChar, nRepCnt, nFlags);
   }
