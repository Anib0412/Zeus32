/////////////////////////////////////////////////////////////////////////////
// PREVIEW.CPP
//                                                                        
// preview class (derived from MDIChild) for RasterPlus95
// Copyright (C) 1996-97 Graphx, Inc.
//

#include "stdafx.h"
#include "zeus.h"
#include "preview.h"
#include "mainfrm.h"
#include "ImagePause.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreviewMDI

IMPLEMENT_DYNCREATE(CPreviewMDI, SECMDIChildWnd)

CPreviewMDI::CPreviewMDI()
   {  
   CString csSection, csEntry;
   CProfile Profile;
   
   // defaults   
   pExistsFlag = &ExistsFlagDummy;      
   // get initally iconic flag    
   m_iconic = Profile.GetInt(IDS_PREVIEW, IDS_PREVIEW_ICONIC, IDDM_PREVIEW_ICONIC);
   // create a brush to draw the background of the icon	  
   // make the same as the MDI workspace
   cb.CreateSolidBrush(GetSysColor(COLOR_APPWORKSPACE));
   }

CPreviewMDI::~CPreviewMDI()
   {                                                
   // set flag in parent class array
   *pExistsFlag = FALSE;
   // delete the brush
   cb.DeleteObject();           
   }																							 

BOOL CPreviewMDI::PreCreateWindow(CREATESTRUCT& cs)
   {
   cs.style &= ~WS_THICKFRAME;
   cs.style &= ~WS_MAXIMIZEBOX;
   cs.style &= ~WS_VISIBLE;
   // if initially iconic, set style
   if(m_iconic) cs.style |= WS_MINIMIZE;
   
   cs.cx = PreviewInfo.Width;
   cs.cy = PreviewInfo.Height;

   if (SECMDIChildWnd::PreCreateWindow(cs))
      {                               
      // set the new preview class (for new icon)
      cs.lpszClass = PreviewClassName;
      return TRUE;
      }    
   else 
      return NULL;
   }

BEGIN_MESSAGE_MAP(CPreviewMDI, SECMDIChildWnd)
	//{{AFX_MSG_MAP(CPreviewMDI)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPreviewMDI message handlers

void CPreviewMDI::PrevSetBuffer()
   {                                                                   
   // create a new display image
   if (cd.CreateImage(&PreviewInfo))	  	  
      DoPaint = TRUE;                     	  
  }     
  
CPreviewMDIArray::CPreviewMDIArray()
   {                   
   int i;
   CString cs, csEntry, csSection;   
   
   // set initial preview index
   PreviewIdx = 0;
   
   // get max open previews 
   MaxOpenPreviews = Profile.GetInt(IDS_PREVIEW, IDS_PREVIEW_NUM, IDDM_PREVIEW_NUM);

   // get PauseFlag
   PauseFlag = Profile.GetInt(IDS_PREVIEW, IDS_PREVIEW_PAUSE, IDDM_PREVIEW_PAUSE);	   
   
   // register our preview class	  
   PreviewClassName = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, 
      AfxGetApp()->LoadStandardCursor(IDC_ARROW), 
      (HBRUSH)(COLOR_WINDOW + 1),
      AfxGetApp()->LoadIcon(IDI_ICON_PREVIEW));	           
      //0);
      
   // we have no previews
   for (i = 0; i < IDDM_MAX_PREVNUM; i++)
      PreviewExists[i] = FALSE;
   }

CPreviewMDIArray::~CPreviewMDIArray()   
   {              
   }     

#define NUDGEFACTOR 10

int CPreviewMDIArray::PrevSetInfo(WPARAM wParam, LPARAM lParam)
   {                              
   CString csEntry, csSection;
   POINT Point;
   RECT NewRect, ProgRect;                     
   CImagePause ip;
   int ret = SYN_ERRMSG_ALLOK;
        
   // if it exists, nuke it
   if (PreviewExists[PreviewIdx])
      {
      //pCpMDI[PreviewIdx]->MDIDestroy();	  
	  pCpMDI[PreviewIdx]->DestroyWindow();
	  PreviewExists[PreviewIdx] = FALSE;
	  }
   
   // get currently active MDIChild                                   
   SECMDIChildWnd* pLastActiveChild = 
      //((CMainFrame*)(AfxGetApp()->m_pMainWnd))->MDIGetActive(NULL);
	  (SECMDIChildWnd*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->MDIGetActive(NULL);
	  
	  

   // get max open previews again (it may have changed)
   MaxOpenPreviews = Profile.GetInt(IDS_PREVIEW, IDS_PREVIEW_NUM, IDDM_PREVIEW_NUM);

   // get PauseFlag
   PauseFlag = Profile.GetInt(IDS_PREVIEW, IDS_PREVIEW_PAUSE, IDDM_PREVIEW_PAUSE);	      
      
   // alloc space for the window
   // we don't delete because the default MDI
   // destructor does the deleting
   pCpMDI[PreviewIdx] = new CPreviewMDI;      

   // get PreviewInfo           
   pCpMDI[PreviewIdx]->PreviewInfo = *((PPREVIEWINFO)lParam);
   
   // set pointer to exists flag
   pCpMDI[PreviewIdx]->pExistsFlag =  &(PreviewExists[PreviewIdx]);
   
   // set the image data
   pCpMDI[PreviewIdx]->PrevSetBuffer();

   // create the window      
   pCpMDI[PreviewIdx]->PreviewClassName = PreviewClassName;
   pCpMDI[PreviewIdx]->Create(NULL, pCpMDI[PreviewIdx]->PreviewInfo.Title);   
   PreviewExists[PreviewIdx] = TRUE;
      
   // get window rect based on desired client size
   NewRect.left = 0; NewRect.top = 0; 
   NewRect.right = pCpMDI[PreviewIdx]->PreviewInfo.Width; 
   NewRect.bottom = pCpMDI[PreviewIdx]->PreviewInfo.Height;

   ::AdjustWindowRect(&NewRect,    
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 
      FALSE);

   // add in Dr. GUI notated bug
   //NewRect.bottom += GetSystemMetrics(SM_CYBORDER); 

   // get size of app window
   //pCpMDI[PreviewIdx]->GetParent()->GetClientRect(&ProgRect);       
   AfxGetMainWnd()->GetClientRect(&ProgRect);       
                                    
   // get initial pos to put at bottom right 
   Point.x = ProgRect.right - (NewRect.right - NewRect.left);
   Point.y = ProgRect.bottom - (NewRect.bottom - NewRect.top);     

   // add a nudge for subsequent windows     
   Point.x -= PreviewIdx * NUDGEFACTOR;
   Point.y -= PreviewIdx * NUDGEFACTOR;

   // make sure it's on the screen
   Point.y = max(0, Point.y);
   Point.x = max(0, Point.x);

   // set the window pos if preview is not iconic
   if (!pCpMDI[PreviewIdx]->IsIconic())
      {      
      pCpMDI[PreviewIdx]->SetWindowPos(&pCpMDI[PreviewIdx]->wndTop, Point.x, Point.y, 
         NewRect.right - NewRect.left,         
         NewRect.bottom - NewRect.top, 0);
      } 
   else
      {                   
      // set the size of the window when it is maximized
      WINDOWPLACEMENT WindowPlacement;                       
      memset(&WindowPlacement, 0, sizeof(WINDOWPLACEMENT));
      WindowPlacement.length = sizeof(WINDOWPLACEMENT);      
      WindowPlacement.showCmd = SW_SHOWMINNOACTIVE;      
      NewRect.right += Point.x;         
      NewRect.left += Point.x;         
      NewRect.bottom += Point.y;         
      NewRect.top += Point.y;                      
      WindowPlacement.rcNormalPosition = NewRect;
      pCpMDI[PreviewIdx]->SetWindowPlacement(&WindowPlacement);
      }

   pCpMDI[PreviewIdx]->ShowWindow(SW_SHOW);
      
   // increment index
   PreviewIdx++;                   
   
   // no more than MaxOpenPreviews
   PreviewIdx = (PreviewIdx % MaxOpenPreviews);
   
   // activate previously active child window
   //if (pLastActiveChild) pLastActiveChild->MDIActivate();   

   // should we pause?
   if (PauseFlag)
      {
	  if (ip.DoModal() == IDCANCEL)
         {
		 // cancel imaging
	     ret = SYN_ERRMSG_CANCELIMAGE;
	     }
	  else
	     {
		 // should we quit pausing?
		 if (ip.m_stop)		    
		    {			
		    Profile.WriteInt(IDS_PREVIEW, IDS_PREVIEW_PAUSE, PauseFlag = FALSE);		    
			}
		 }
	  }
   return ret;
   }   

void CPreviewMDIArray::CloseAll()
   {
   int i;

   for (i = 0; i < IDDM_MAX_PREVNUM; i++)
      {
	  // if it exists, nuke it
      if (PreviewExists[i])
	     {
         //pCpMDI[i]->MDIDestroy();
		 pCpMDI[i]->DestroyWindow();
		 PreviewExists[i] = FALSE;
		 }
	  }   
   }
   
int CPreviewMDIArray::NumActive()
   {
   int i;
   int cnt;

   for (i = 0, cnt = 0; i < IDDM_MAX_PREVNUM; i++)
      {
	  // if it exists, nuke it
      if (PreviewExists[i])
	     {
         cnt++;				   
		 }
	  }   
   return cnt;
   }
   
void CPreviewMDI::OnPaint()
   {
   CPaintDC dc(this); // device context for painting   

   if (DoPaint)
      {             
      RECT Rect;
      GetClientRect(&Rect);
      if (IsIconic())
         {
         CBrush* pOldBrush;         
         // draw a rectangle
         pOldBrush = dc.SelectObject(&cb);
         // blank it
         dc.FillRect(&Rect, &cb);         
         // cleanup DC
         if (pOldBrush != NULL) dc.SelectObject(pOldBrush);      
         }
      cd.DisplayImage(&Rect, dc);         
      }	       
   // Do not call SECMDIChildWnd::OnPaint() for painting messages
   }      

HCURSOR CPreviewMDI::OnQueryDragIcon()
   {
   return cd.DragIcon();
   }
