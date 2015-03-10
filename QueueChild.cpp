/////////////////////////////////////////////////////////////////////////////
// QueueChild.cpp
//                                                                        
// SECWorksheet derivative for RasterPlus
// Copyright (C) 1996-2000 Graphx, Inc.
//
// mods:
//
// 7-05-96  Build 51 - check for last prompt and don't re-display needlessly
// 10-27-97 Build 53 - added byte total reading style for interpret on the fly stuff
// 11-23-97 Version 2.0 - bad string load in interpret on the fly code - fixed
// 08-03-00 Version 4.0 - added tray icon
// 01-16-01 Version 4.0.2 - Fix protection error - No Status Prompt associated with STATUS_READING_TOTAL
// 03-07-02 Version 5.0.4 - Remove Sysmenu for queues for Polaroid Japan - Didn't want the 'Close Window Button' 
// 10-10-02 Version 5.0.7 - Remove Sysmenu for queues - Don't want the 'Close Window Button' 
//

#include "stdafx.h"
#include "QueueChild.h"
#include "ripmsg.h"
#include "server.h"
#include "profile.h"

#ifdef LAB_STYLE
#include "device.h"
#include "queuepane.h"
#include "propertypane.h"
#include "photopane.h"
#endif

#include"holdtree.h"
#include"printtree.h"
#include"archivetree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT NEAR wm_ResizeRPWindowMsg = RegisterWindowMessage( "ResizeRPWindow" );

/////////////////////////////////////////////////////////////////////////////
// CQueueChild

IMPLEMENT_DYNCREATE(CQueueChild, SECWorksheet)

CQueueChild::CQueueChild()
   {
   SetInitialSize = FALSE;   
   RipplePercent = 0;
   QueueStatus = IDS_QUEUESTATUS_IDLE;
   // load the prompts
   csStatusPrompt[STATUS_READING].LoadString(IDS_STATUS_READING_FILE);
   csStatusPrompt[STATUS_READING_TOTAL].LoadString(IDS_STATUS_READING_FILE);
   csStatusPrompt[STATUS_SCANLINE].LoadString(IDS_STATUS_SCANLINE);   
   csStatusPrompt[STATUS_RENDERING].LoadString(IDS_STATUS_RENDERING);
   csStatusPrompt[STATUS_PRINTING_RED].LoadString(IDS_STATUS_PRINTING_RED);
   csStatusPrompt[STATUS_PRINTING_GREEN].LoadString(IDS_STATUS_PRINTING_GREEN);
   csStatusPrompt[STATUS_PRINTING_BLUE].LoadString(IDS_STATUS_PRINTING_BLUE);
   csStatusPrompt[STATUS_PRINTING_CYAN].LoadString(IDS_STATUS_PRINTING_CYAN);
   csStatusPrompt[STATUS_PRINTING_YELLOW].LoadString(IDS_STATUS_PRINTING_YELLOW);
   csStatusPrompt[STATUS_PRINTING_MAGENTA].LoadString(IDS_STATUS_PRINTING_MAGENTA);
   csStatusPrompt[STATUS_PRINTING_BLACK].LoadString(IDS_STATUS_PRINTING_BLACK);
   csStatusPrompt[STATUS_PRINTING_WHITE].LoadString(IDS_STATUS_PRINTING_WHITE);
   csStatusPrompt[STATUS_PRINTING_IMAGE].LoadString(IDS_STATUS_PRINTING_IMAGE);   
   csStatusPrompt[STATUS_PRINTING_IMAGE].LoadString(IDS_STATUS_PRINTING_IMAGE);   

   Last_flag = -1;   

   // default
   //StatusTextID	= IDC_PRINT_STATUS_TEXT;
   //StatusProgressID = IDC_PRINT_STATUS_PROGRESS;
   //StatusPromptID = IDC_PRINT_STATUS_PROMPT;
   }

void CQueueChild::StartTrayControl()
   {
   m_TrayStatus.Show(TRUE);
   m_TrayStatus.Play(IDI_PRINTTRAY1, 4);
   }

void CQueueChild::StopTrayControl()
   {
   m_TrayStatus.Stop();
   m_TrayStatus.Show(FALSE);
   }

void CQueueChild::SetTrayTitle(LPCSTR csTitle)
   {
   m_TrayStatus.csTrayTitle = csTitle;
   m_TrayStatus.csTrayTitle += ": ";
   }

CQueueChild::~CQueueChild()
   {
   }			 

BEGIN_MESSAGE_MAP(CQueueChild, SECWorksheet)	
	ON_WM_CREATE()
	ON_COMMAND_EX(CG_ID_VIEW_QUEUESTATUS, OnBarCheck)
	ON_UPDATE_COMMAND_UI(CG_ID_VIEW_QUEUESTATUS, OnUpdateControlBarMenu)
	ON_COMMAND_EX(CG_ID_VIEW_PSOUT, OnBarCheck)
	ON_UPDATE_COMMAND_UI(CG_ID_VIEW_PSOUT, OnUpdateControlBarMenu)
	ON_REGISTERED_MESSAGE(wm_ResizeRPWindowMsg, OnOurResize)	
	//{{AFX_MSG_MAP(CQueueChild)
	ON_WM_SIZE()
	ON_WM_CREATE()			
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQueueChild message handlers

int CQueueChild::OnCreate(LPCREATESTRUCT lpCreateStruct) 
   {

	if (SECWorksheet::OnCreate(lpCreateStruct) == -1)
    	return -1;

   // Initialize dialog bar m_wndQueueStatus
   if (!m_wndQueueStatusTop.Create(this, CG_IDD_RIPSTATUS,
      CBRS_TOP | WS_VISIBLE, CBRS_EX_STDCONTEXTMENU | CBRS_EX_STRETCH_ON_SIZE,
	  CG_ID_VIEW_QUEUESTATUS))
	  {
//	  TRACE0("Failed to create dialog bar m_wndQueueStatusTop\n");
	  return -1;		// fail to create
	  }

   m_wndQueueStatusTop.IsCreated = TRUE;
   m_wndQueueStatusTop.pcwndParent = this;

   m_wndQueueStatusTop.EnableDocking(CBRS_ALIGN_TOP);
   EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_wndQueueStatusTop);

   // Initialize dialog bar m_wndPSOUT
   if (!m_wndPSOUT.Create(this, CG_IDD_PSOUT,
		      CBRS_BOTTOM | WS_VISIBLE, CBRS_EX_STDCONTEXTMENU | CBRS_EX_STRETCH_ON_SIZE,
			      CG_ID_VIEW_PSOUT))
	  {
//	  TRACE0("Failed to create dialog bar m_wndPSOUT\n");
	  return -1;		// fail to create
	  }	

   m_wndPSOUT.IsCreated = TRUE;
   m_wndPSOUT.pcwndParent = this;

   m_wndPSOUT.EnableDocking(CBRS_ALIGN_BOTTOM);
   EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_wndPSOUT);		

   ShowWindow(SW_SHOWMAXIMIZED); 

   // create the tray icon
   m_TrayStatus.Create(this);
   int nFrameDelay = 30;	// animation speed
   CString csDummy = _T("RasterPlus");

   // add the icons
   m_TrayStatus.AddState(IDI_PRINTTRAY1, IDI_PRINTTRAY1, "", nFrameDelay);
   m_TrayStatus.AddState(IDI_PRINTTRAY2, IDI_PRINTTRAY2, "", nFrameDelay);
   m_TrayStatus.AddState(IDI_PRINTTRAY3, IDI_PRINTTRAY3, "", nFrameDelay);
   m_TrayStatus.AddState(IDI_PRINTTRAY4, IDI_PRINTTRAY4, "", nFrameDelay);   
   // don't show it
   m_TrayStatus.Show(FALSE);

   return 0;
   }

void COurTrayIcon::SetAllTips(const CString& sTip)
   {
   csTrayMsg = csTrayTitle;
   csTrayMsg += sTip;	  

    POSITION    pos = m_mapStateData.GetStartPosition();
    while( pos )
    {
        DWORD       nState;
        STATEDATA*  psd;

        m_mapStateData.GetNextAssoc(pos, (void*&)nState, (void*&)psd);

        psd->sTip = csTrayMsg;        
    }
   }

void CQueueChild::SetStatus1(int StatusTextID, LPCSTR p) 
   {   
   m_wndQueueStatusTop.SetDlgItemText(StatusTextID, p);   
   m_TrayStatus.SetAllTips(p);
   }	

void CQueueChild::SetStatus2(int StatusPromptID, LPCSTR p) 
   {   
   m_wndQueueStatusTop.SetDlgItemText(StatusPromptID, p);   
   }	

void CQueueChild::SetServerFlag(int flag)
   {   									  
   CString cs;

   switch(flag)
      {
      case STATUS_SERVER_DISABLED:
           cs.LoadString(IDS_SERVER_DISABLED);
		   break;
      case STATUS_SERVER_ENABLED:
           cs.LoadString(IDS_SERVER_ENABLED);
		   break;
      case STATUS_SERVER_ACTIVE:
           cs.LoadString(IDS_SERVER_ACTIVE);
		   break;   
	  }
   //m_wndQueueStatusBottom.SetDlgItemText(IDC_SERVER, cs);
   }	

void CQueueChild::SetProgress(int StatusProgressID, int Percent) 
   {   
   ((CProgressCtrl*)m_wndQueueStatusTop.GetDlgItem(StatusProgressID))->SetPos(Percent);
   //m_wndQueueStatusTop.SetDlgItemInt(IDC_STATUS_PERCENT, Percent);   
   }	

void CQueueChild::RippleProgress(int StatusProgressID)
   {
   ((CProgressCtrl*)m_wndQueueStatusTop.GetDlgItem(StatusProgressID))->SetPos(RipplePercent);
   RipplePercent += 30;
   RipplePercent %= 100;
   //m_wndQueueStatusTop.SetDlgItemText(IDC_STATUS_PERCENT, "");   
   }

void CQueueChild::SetTimeString(CString& cs, int iType, BOOL bClear)
   {   
   if (iType == QUEUESTATUS_DOTIME_HOLD)
	  {
	  if (bClear)
	     m_wndQueueStatusTop.SetDlgItemText(IDC_ELAPSED_TIME_HOLD, "");   
	  else
		 m_wndQueueStatusTop.SetDlgItemText(IDC_ELAPSED_TIME_HOLD, cs);   
	  }
   else
	  {
	  if (bClear)
	     m_wndQueueStatusTop.SetDlgItemText(IDC_ELAPSED_TIME_PRINT, "");   
	  else
		 m_wndQueueStatusTop.SetDlgItemText(IDC_ELAPSED_TIME_PRINT, cs);   
	  }
   }	

void CQueueChild::SetSelected(int NumSelected, int Total)
   {   
   csFmt.Format((const char*)"%d of %d", NumSelected, Total);
   //m_wndQueueStatusBottom.SetDlgItemText(IDC_SELECTED, csFmt);
   }	

CEdit* CQueueChild::GetPSOutEdit()
   {
   return (CEdit*)(m_wndPSOUT.GetDlgItem(IDC_PSOUT));
   }   

BOOL CQueueChild::DoesPSHaveSelText()
   {
   CEdit* ce = (CEdit*)(m_wndPSOUT.GetDlgItem(IDC_PSOUT));
   int nStartChar, nEndChar;

   ce->GetSel(nStartChar, nEndChar);

   if (nStartChar != nEndChar != 0)
      return TRUE;
   else
      return FALSE;
   }

BOOL CQueueChild::IsPSOutEmpty()
   {
   CEdit* ce = (CEdit*)(m_wndPSOUT.GetDlgItem(IDC_PSOUT));

   if (ce->GetLineCount() == 1 && ce->LineLength(0) <= 0)
      return TRUE;
   else
      return FALSE;
   }

void CQueueChild::PSOut(BYTE* str, int len)
   {      
   //char* p;

   CEdit* pEdit = (CEdit*)(m_wndPSOUT.GetDlgItem(IDC_PSOUT));

   // put in correct line feeds
   if ((len == 1 && str[len - 1] == 0x0a) ||
	   (len > 1 && str[len - 1] == 0x0a && str[len - 2] != 0x0d))
	  {
	  str[len - 1] = 0x0d;
	  str[len] = 0x0a;
	  len++;
	  }

   // add a terminating char in case
   str[len] = '\0';
#if 0
   // break the string into lines
   p = strtok((char*)str, "\r\n");

   while (p)
      {
	  // add back the cr-lf
	  csFmt = p;
	  csFmt += "\r\n";
	  pEdit->ReplaceSel(csFmt);
		 
	  p = strtok(NULL, "\r\n");
	  }	  
#endif
   pEdit->ReplaceSel((const char*)str);
   }

void CQueueChild::ClearStatus(int StatusTextID, int StatusPromptID, int StatusProgressID)
   {
   SetStatus1(StatusTextID, "");
   SetStatus2(StatusPromptID, "");
   SetProgress(StatusProgressID, 0);
   }

void CQueueChild::SetProcessStatus(int StatusPromptID, int StatusProgressID, char* pText)
   {
   SetStatus2(StatusPromptID, pText);
   SetProgress(StatusProgressID, 0);
   }
      
UINT CQueueChild::SetQueueStatus(UINT uiStatus)
   {
   CString cs;
   UINT uiRet;
   
   // save last for return
   uiRet = QueueStatus;
   
   // set new
   QueueStatus = uiStatus;

   cs.LoadString (uiStatus);
   //m_wndQueueStatusBottom.SetDlgItemText(IDC_STATUS, (const char*)cs);   

   // update the general status bar
   ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_wndStatusBar.SetPaneText(1, cs, TRUE);

   return uiRet;
   }
      
void CQueueChild::SetProcessStatus(int StatusPromptID, int StatusProgressID, int flag, int Data1, int Data2, int Data3, int Data4)
   {
   const char* pTemp;

   switch(flag)
      {
	  case STATUS_READING:
		   SetStatus2(StatusPromptID, csStatusPrompt[flag]);
		   // if 0, nuke the possible reading total prompt
//		   if (Data1 == 0)
//			  m_wndQueueStatusTop.SetDlgItemText(IDC_READ_TOTAL, "");
//           m_wndQueueStatusTop.SetDlgItemText(IDC_STATUS2, 
//	 	     (const char*)csStatusPrompt[flag]);
	 	   SetProgress(StatusProgressID, Data1);
		   break;	      
	  case STATUS_READING_TOTAL:
		   SetStatus2(StatusPromptID, csStatusPrompt[flag]);
           //m_wndQueueStatusTop.SetDlgItemText(IDC_STATUS2, 
	 	   //  (const char*)csStatusPrompt[flag]);
	 	   SetProgress(StatusProgressID, 0);
#if 0
		   if (Data1 == -1)
			  m_wndQueueStatusTop.SetDlgItemText(IDC_READ_TOTAL, "");
		   else
			  {
			  csFmt.Format("%d bytes read", Data1);
			  m_wndQueueStatusTop.SetDlgItemText(IDC_READ_TOTAL, csFmt);
			  }
#endif
		   break;	      
	  case STATUS_SCANLINE:
		   SetStatus2(StatusPromptID, csStatusPrompt[flag]);
//	       csFmt.Format("%s %d of %d", (const char*)csStatusPrompt[flag], Data1, Data2);
//           m_wndQueueStatusTop.SetDlgItemText(IDC_STATUS2, csFmt);
		   if ( Data2 > 0 )
				SetProgress(StatusProgressID, Data1 * 100 / Data2);
		   break;	      
	  case STATUS_PRINTING:
      case STATUS_PRINTING_TP:		   
	       switch(Data3)
		      {
			  case RIPMSG_COLOR_RED:
			       pTemp = (const char*)csStatusPrompt[STATUS_PRINTING_RED];
			 	   break;
			  case RIPMSG_COLOR_GREEN:
			       pTemp = (const char*)csStatusPrompt[STATUS_PRINTING_GREEN];
			  	   break;
			  case RIPMSG_COLOR_BLUE:
			       pTemp = (const char*)csStatusPrompt[STATUS_PRINTING_BLUE];
			  	   break;
			  case RIPMSG_COLOR_CYAN:
			       pTemp = (const char*)csStatusPrompt[STATUS_PRINTING_CYAN];
				   break;
			  case RIPMSG_COLOR_YELLOW:
			       pTemp = (const char*)csStatusPrompt[STATUS_PRINTING_YELLOW];
				   break;
			  case RIPMSG_COLOR_MAGENTA:
			       pTemp = (const char*)csStatusPrompt[STATUS_PRINTING_MAGENTA];
				   break;
			  case RIPMSG_COLOR_BLACK:
			       pTemp = (const char*)csStatusPrompt[STATUS_PRINTING_BLACK];
				   break;
			  case RIPMSG_COLOR_WHITE:
			       pTemp = (const char*)csStatusPrompt[STATUS_PRINTING_WHITE];
				   break;
			  case RIPMSG_COLOR_IMAGE:
			       pTemp = (const char*)csStatusPrompt[STATUS_PRINTING_IMAGE];
				   break;
			  }		  
		   SetStatus2(StatusPromptID, (char*)pTemp);
#if 0
           m_wndQueueStatusTop.SetDlgItemText(IDC_STATUS2, pTemp);	      
		   if (flag == STATUS_PRINTING)
		      csFmt.Format("%s %d of %d", pTemp, Data1, Data2);
		   else
			  csFmt.Format("%s %d of %d (%dKb/s)", pTemp, Data1, Data2, Data4);
		   m_wndQueueStatusTop.SetDlgItemText(IDC_STATUS2, csFmt);		   
#endif
		   if ( Data2 > 0 )
			   SetProgress(StatusProgressID, Data1 * 100 / Data2);
		   break;	      			  
	  case STATUS_RENDERING:
		   if (Last_flag != flag)
			  {			  			  
			  SetProgress(StatusProgressID, 0);
			  SetStatus2(StatusPromptID, csStatusPrompt[flag]);
//			  m_wndQueueStatusTop.SetDlgItemText(IDC_READ_TOTAL, "");
//	          m_wndQueueStatusTop.SetDlgItemText(IDC_STATUS2, 
//		         (const char*)csStatusPrompt[flag]);
			  }
		   //RippleProgress(StatusProgressID);
		   break;
	  }  
   Last_flag = flag;
   }		  

/////////////////////////////////////////////////////////////////////////////
// COurDialogBar

COurDialogBar::COurDialogBar()
   {   
   // defaults
   IsCreated = FALSE;
   pcwndParent = NULL;
   }

COurDialogBar::~COurDialogBar()
   {
   }

BEGIN_MESSAGE_MAP(COurDialogBar, SECDialogBar)
	//{{AFX_MSG_MAP(COurDialogBar)
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COurDialogBar message handlers

void CQueueChild::OnSize(UINT nType, int cx, int cy) 
   {	
#ifndef LAB_STYLE
   SECWorksheet::OnSize(nType, cx, cy);
#endif
#ifndef RP30
   // if the top status bar has been created, and we
   // haven't set our initial size, do it now so we look
   // pretty...
   if (m_wndQueueStatusTop.IsCreated && !SetInitialSize)
      {	 
	  // make the queue height == to the screen height * 3 / 4
	  int InitHeight = GetSystemMetrics(SM_CYSCREEN) * 3 / 4;
	  CRect cr;
	  m_wndQueueStatusTop.GetClientRect(&cr);
	  if (cr.Width() > 0)
	     SetWindowPos(NULL, 0, 0, cr.Width() + 2, InitHeight, 
		    SWP_NOMOVE | SWP_NOZORDER);
	  SetInitialSize = TRUE;
	  }   
#endif
   if (m_wndPSOUT.IsCreated)
      {	 
	  CRect crParent;
	  CRect cr;
	  m_wndPSOUT.GetParent()->GetClientRect(&crParent);
	  m_wndPSOUT.GetClientRect(&cr);
	  if (cr.Width() > 0)
	     m_wndPSOUT.SetWindowPos(NULL, 0, 0, crParent.Width(), cr.Height(), 
		    SWP_NOMOVE | SWP_NOZORDER);	  
	  }   
   }

void COurDialogBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
   { 
	SECDialogBar::OnWindowPosChanged(lpwndpos);

	// need to resize the toolbar when it is made visible via the menu
	if ((lpwndpos->flags & SWP_SHOWWINDOW) && pcwndParent)
	   {	   	
	   ::PostMessage(pcwndParent->GetSafeHwnd(), wm_ResizeRPWindowMsg, 0, 0);
	   }  	
   }
 
LRESULT CQueueChild::OnOurResize(WPARAM wParam, LPARAM lParam)
   {
   // resize the window
   if (m_wndPSOUT.IsCreated)
      {	 
	  CRect crParent;
	  CRect cr;
	  m_wndPSOUT.GetParent()->GetClientRect(&crParent);
	  m_wndPSOUT.GetClientRect(&cr);
	  if (cr.Width() > 0)
	     m_wndPSOUT.SetWindowPos(NULL, 0, 0, crParent.Width(), cr.Height(), 
		    SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);	  
	  }   			 
   return 0L;
   }

BOOL CQueueChild::PreCreateWindow(CREATESTRUCT& cs)
   {
   CProfile Profile;
/*
   BOOL bQueueCloseOK = Profile.GetInt(IDS_QUEUESECTION, IDS_QUEUE_CLOSE,  1);

   if ( bQueueCloseOK )
	   cs.style &= ~WS_MINIMIZEBOX;
   else
*/
	   cs.style &= ~WS_SYSMENU;

   if( !SECWorksheet::PreCreateWindow(cs) )
      return FALSE;
   return TRUE;
   }

void CQueueChild::ActivateFrame(int nCmdShow)
   {
   if (nCmdShow == -1)
      nCmdShow = SW_SHOWMAXIMIZED;

   SECWorksheet::ActivateFrame(nCmdShow);
   }

BOOL CQueueChild::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
   {

#ifdef LAB_STYLE
   if( m_splitter.CreateStatic( this, 1, 3 ) )
	  {
	  if( m_splitter.CreateView( 0, 0, RUNTIME_CLASS(CQueuePane), CSize(200,300), pContext ) )
		{
		}
	  if( m_splitter.CreateView( 0, 1, RUNTIME_CLASS(CPropertyPane), CSize(200,300), pContext ) )
		{
		}
	  if( m_splitter.CreateView( 0, 2, RUNTIME_CLASS(CPhotoPane), CSize(200,300), pContext ) )
		{
		}
	  }
#else
   if( m_splitter.CreateStatic( this, 3, 1 ) )
	  {
	  if( m_splitter.CreateView( 0, 0, RUNTIME_CLASS(CHoldTree), CSize(100,100), pContext ) )
		{
		}
	  if( m_splitter.CreateView( 1, 0, RUNTIME_CLASS(CPrintTree), CSize(100,100), pContext ) )
		{
		}
	  if( m_splitter.CreateView( 2, 0, RUNTIME_CLASS(CArchiveTree), CSize(100,100), pContext ) )
		{
		}
	  }
#endif
   // make hold queue the initial focus
   m_splitter.SetActivePane(QUEUESTYLE_HOLD, 0);	

   m_splitter.DragAcceptFiles(TRUE);

   return TRUE;
   }							

/*   
CDocument* CQueueChild::GetActiveDocument()
{

	return CFrameWnd::GetActiveDocument();
}

void CQueueChild::AddQueue( CDocument* pDoc )
{
	coPrinters.Add(pDoc);
}

void CQueueChild::PopulateNode( CDocument* pDoc )
{
	pQueuePane->AddNewNode( pDoc );
}
*/
