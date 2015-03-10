/////////////////////////////////////////////////////////////////////////////
// NEWFORMV.CPP
//                                                                        
// main queue view for RasterPlus95
// Copyright (C) 1996-98 Graphx, Inc.
//
// mods:
//
// 7-05-96  Build 40 - The initial display of a newly opened file would not show
//					      the status icons properly until the files were moved - fixed
// 7-06-96  Build 40 - Changed PhotoShop default extension from .pcd to .psd
// 7-23-96  Build 40 - Renumber list after WinSplit inserts files
// 11-05-96 Build 45 - delete server files when the files get deleted
// 11-05-96 Build 45 - added DeleteAllTempFiles proc
// 11-25-96 Build 46 - changed RemoveFile to DeleteFile
// 1-23-97  Build 48 - Fixed CTRL+P so it does our print not regular print
// 5-02-97  Build 49 - Fixed WinSplit filename creation (strtok was being used in a called 
//                        proc in the middle of our strtok) 
// 5-26-97  Build 50 - if a drive was not ready, a possible NULL filename could be passed to 
//                     strlwr causing a crash - we now check for this
// 6-16-97  Build 50 - enable column sorting
// 7-22-97  Build 50 - FIX column sorting
// 8-04-97  Build 50 - update target queuefile on dev change for custom transform
// 8-07-97  Build 50 - use alternate device loading strategy for PM 6.5
// 9-07-97  Build 51 - check for list count on double click to prevent crashing
// 10-01-97 Build 51 - store dongle key code in app
// 10-08-97 Build 51 - reset an inrip status to hold when pasting
// 10-27-97 Build 53 - added byte total reading style for print on the fly stuff
// 10-28-97 Build 53 - only check for dongle if no other rip is running
//                     disable screen saver while in rip
// 11-09-97 Build 53 - changed way we look for active rips
// 12-12-97 Version 2.0.1 - simplify and speed up dongle code
// 12-19-97 Version 2.0.1 - turn off server flag if file is copied to clipboard
//                             to prevent deletion 
// 12-19-97 Version 2.0.1 - put in test for non-ssl dongle
// 1-06-98  Version 2.0.1 - only delete POLLED active server files or queue server files
// 1-06-98  Version 2.0.1 - update files with 0 size in queue from server
// 1-08-98  Version 2.0.1 - final fix for multiple dongles
// 3-19-98  Version 2.0.3 - FilePrint now goes into Queue as unselected
// 5-29-98  Version 2.0.4 - don't change status icon on preview
// 6-15-98  Version 2.0.4 - add RIPMSG_STATUS_OUT and RIPMSG_THROUGHPUT messages
// 6-24-98  Version 2.0.4 - always add server files at end of queue
// 6-25-98  Version 2.0.4 - don't update file size after delete
// 9-10-98  Version 2.0.5 - added new device rip loading code 
// 11-12-98 Version 2.0.5 - inrip check not working properly - fixed
// 11-12-98 Version 2.0.5 - delete during pause made go button go away - fixed
// 11-30-98 Version 2.0.5 - resizing columns didn't update list properly - fixed
// 11-16-01 Version 5.0.1 - Remove Dongle Code
//

#include "stdafx.h"
#include "zeus.h"
#include "newformv.h"
#include "zeusdoc.h"
#include "qprefs.h"
#include "ripstruct.h"
#include <string.h>
#include <stdlib.h>
#include "inidefs.h" 
#include "rip.h"
#include "xfrmdefs.h"
#include "device.h"
#include "devdlg.h"
#include "transdlg.h"
#include "prefsmsg.h"
#include "ripmsg.h"
#include "bheader.h"
#include "splitdlg.h"
#include "profile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT NEAR wm_RipSendMsg = RegisterWindowMessage( RIP_SENDMESSAGE );
const UINT NEAR wm_RipSendPSMessage = RegisterWindowMessage(RIP_SENDPSMESSAGE);
const UINT NEAR wm_RipComplete = RegisterWindowMessage( RIP_COMPLETE );
const UINT NEAR wm_DevModeChange = RegisterWindowMessage( ID_DEVMODE_CHANGE );

#define CLIPARRAY ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->ClipArray
#define INRIPARRAY ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->InRipArray

// our global devices here are stored in the document
#define QUEUEDEVICE    ((CZeusDoc*)GetDocument())->QueueDevice
#define QUEUETRANSFORM ((CZeusDoc*)GetDocument())->QueueTransform
#define QUEUESERVER    ((CZeusDoc*)GetDocument())->Server

// timer stuff
#define RP95_PRINT_TIMER_ID          199
#define RP95_SERVER_TIMER_ID         198
#define RP95_PRINT_TIMER_TIMEOUT     1000   // 1 sec
#define RP95_SERVER_TIMER_TIMEOUT    5000   // 5 sec

#define PSOUT_EDIT_WINDOW  (pCQueueChild->GetPSOutEdit())
#define PSOUT_HAVE_SELTEXT (pCQueueChild->DoesPSHaveSelText())
#define PSOUT_EMPTY        (pCQueueChild->IsPSOutEmpty())

/////////////////////////////////////////////////////////////////////////////
// CNewFormView

IMPLEMENT_DYNCREATE(CNewFormView, CListView)

// Note: we pass a screen-specific dialog box ID to the default CListCtrl 
// constructor so that our bitmap buttons look pretty
CNewFormView::CNewFormView()	           
   {        
   CProfile Profile;
   CString cs;
   
   m_iColumns = 0;
   m_iColWidth = NULL;

   // get the initial file dir 
   InitialFileDir = Profile.GetStr(IDS_QUEUESECTION, IDS_INITIALDIR, NULL);
   // get the initial filter index	  
   FilterIdx = Profile.GetInt(IDS_QUEUESECTION, IDS_QUEUE_FILTERIDX,  IDDM_QUEUE_FILTERIDX);
   	  
   // get pointer to fileinfo holder in CMainForm so we only do
   // single instance of fileinfo
   pCfDlg = ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pCfDlg;
   // get pointer to instance counter
   pInstanceCntr = 
      &(((CMainFrame*)(AfxGetApp()->m_pMainWnd))->CfDlgInstanceCntr);      
   // create the info dialog     
   CreateFileInfoDlg();      

   // no active file
   pActiveCQueueFile = NULL;
   // file state
   if (cr.RipLoaded)   	  
      InternalState = STATE_IDLE_NO_FILES;     	  
   else	  
      InternalState = STATE_IDLE_RIP_NOTAVAIL; 

   // set in OnInitialUpdate
   pRipElement = NULL;

   cs.LoadString(IDS_RP95_CBOARD_FMT);
   uiOurListClipFormat = ::RegisterClipboardFormat((const char*)cs);

   pLB = NULL;

   ServerPolling = FALSE;

   QueueFileWasConstructed = FALSE;

   for (int i = 0; i < NUMQUEUECOLUMNS; i++)
      QueueSortOrderUp[i] = FALSE;
   }

CNewFormView::~CNewFormView()
   {                                    
   CProfile Profile;

   //destroy the info dialog
   DestroyFileInfoDlg();
   
   // write the initial file dir
   Profile.WriteStr(IDS_QUEUESECTION, IDS_INITIALDIR, InitialFileDir);
   Profile.WriteInt(IDS_QUEUESECTION, IDS_QUEUE_FILTERIDX, FilterIdx);

   delete pLB;

   if (pRipElement) INRIPARRAY.DeleteElement(pRipElement);

   if(m_iColWidth) free(m_iColWidth);
   }

void CNewFormView::DoDataExchange(CDataExchange* pDX)
   {
   CListView::DoDataExchange(pDX);   
   //{{AFX_DATA_MAP(CNewFormView)
   //}}AFX_DATA_MAP
   }        

void CNewFormView::OnInitialUpdate()
   {               
   int i, QueueSize;
   CQueueFile* pCQueueFile;   
   CString cs;      

   pCQueueChild = (CQueueChild*)GetParentFrame();

   pLB = new CPseudoListBox(this);

   DragAcceptFiles(TRUE);

   CListView::OnInitialUpdate();                           
   
   // get count of list   
   QueueSize = PDOC->CQ.GetSize();        

   // create new rip element
   pRipElement = new CInRipElement;

   if (pRipElement) 
	  {
      // add pointer to document
      pRipElement->pDocPtr = (void*)PDOC;
      // add pointer to view
      pRipElement->pViewPtr = (void*)this;
      // set InRip flag
      pRipElement->InRip = FALSE; 
      // add to array
      INRIPARRAY.Add(pRipElement);
	  }

   // get the list control
   CListCtrl& ctlList = GetListCtrl();
   
   // Get the current window style. 
   DWORD dwStyle = GetWindowLong(ctlList.m_hWnd, GWL_STYLE); 
 
   // set a report style list
   ::SetWindowLong(ctlList.m_hWnd, GWL_STYLE, 
      (dwStyle & ~LVS_TYPEMASK) | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_OWNERDRAWFIXED); 

   // set the column titles   
   CRect cr;
   GetClientRect(&cr);    
   cs.LoadString(IDS_FILENAME_COLTITLE);
   AddColumn((const char*)cs, 0, 100, 0, LVCF_FMT | LVCF_TEXT | LVCF_WIDTH, LVCFMT_LEFT);
   cs.LoadString(IDS_DESCRIPTION_COLTITLE);
   AddColumn((const char*)cs, 1, 240, 0, LVCF_FMT | LVCF_TEXT | LVCF_WIDTH, LVCFMT_LEFT);
   cs.LoadString(IDS_LOCALPROPS_COLTITLE);
   AddColumn((const char*)cs, 2, cr.Width() - 340, 0, LVCF_FMT | LVCF_TEXT | LVCF_WIDTH, LVCFMT_LEFT);
   
   // insert pointers
   for (i = 0; i < QueueSize; i++)
      {      
      pCQueueFile = (CQueueFile*)(PDOC->CQ[i]);
      pLB->InsertString(i, (LPCSTR)pCQueueFile);
	  RedrawItem(i);
      }                 
   
   // if there are entries, select the first one
   if (QueueSize > 0)
      {
      pLB->SetSel(0, TRUE);
      // ...and set initial state of affairs      
      if (InternalState == STATE_IDLE_NO_FILES) SetState(STATE_IDLE_SOME_FILES);   
      }
   else
      {
	  SetState(STATE_IDLE_NO_FILES);   
      }      
   
#if 0
   // append current device to title
   cs.Format("%s (%s)", (const char*)((CZeusDoc*)GetDocument())->csBaseTitle,
      (const char*)QUEUEDEVICE.Name);
   ((CZeusDoc*)GetDocument())->SetTitle(cs);
#endif   
   ((CZeusDoc*)GetDocument())->SetTitle(QUEUEDEVICE.Name);

   // set queue status
   pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_IDLE);		 
   
   VERIFY(m_DropTarget.Register(this));      
   }                           
   
void CNewFormView::OnUpdate (CView* pSender, LPARAM lHint, CObject* pHint)
   {      
   // set server status
   pCQueueChild->SetServerFlag(QUEUESERVER.m_status);

   // reset title
   CString cs;
   
#if 0
   // now append current device to title
   cs.Format("%s (%s)", (const char*)((CZeusDoc*)GetDocument())->csBaseTitle,
      (const char*)QUEUEDEVICE.Name);
   ((CZeusDoc*)GetDocument())->SetTitle(cs);      
#endif
   ((CZeusDoc*)GetDocument())->SetTitle(QUEUEDEVICE.Name);
   
   UpdateData( FALSE );   // Transfers data from view class to screen
   Invalidate();        // Causes repainting of view   
   }

void CNewFormView::OnCommit()
   {   
   UpdateData(); //Transfers data from screen to view data members
   // our doc is modified
   PDOC->SetModifiedFlag();
   PDOC->UpdateAllViews( NULL );
   }   

BEGIN_MESSAGE_MAP(CNewFormView, CListView)
	//{{AFX_MSG_MAP(CNewFormView)		
	ON_COMMAND(ID_EDIT_ADDFILES, OnAddfiles)
	ON_COMMAND(ID_EDIT_RESETSTATUS, OnResetStatus)
	ON_COMMAND(ID_PREFERENCES_LOCAL, ShowInfo)	
	ON_COMMAND(ID_EDIT_SELECTALL, OnSelectall)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_REGISTERED_MESSAGE(wm_RipComplete, OnRipComplete)
	ON_REGISTERED_MESSAGE(wm_RipSendMsg, OnRipMessage)	
	ON_REGISTERED_MESSAGE(wm_RipSendPSMessage, OnRipPSMessage)				
	ON_COMMAND(ID_OUR_FILE_PRINT_SEL, OnPrintSel)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW_SEL, OnPreviewSel)
	ON_COMMAND(ID_PROCESS_SKIPTONEXTFILE, OnContinue)			
	ON_COMMAND(ID_PROCESS_RESUMEIMAGING, OnPause)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_CANCELIMAGING, OnUpdateProcessCancelimaging)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDFILES, OnUpdateEditAddfiles)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_SKIPTONEXTFILE, OnUpdateProcessSkiptonextfile)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_PAUSEIMAGING, OnUpdateProcessPauseimaging)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_RESUMEIMAGING, OnUpdateProcessResumeimaging)
	ON_UPDATE_COMMAND_UI(ID_OUR_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_OUR_FILE_PRINT_INDEX, OnUpdateFilePrintIndex)
	ON_UPDATE_COMMAND_UI(ID_OUR_FILE_PRINT_SEL, OnUpdateFilePrintSel)
	ON_UPDATE_COMMAND_UI(ID_OUR_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW_SEL, OnUpdateFilePrintPreviewSel)
	ON_UPDATE_COMMAND_UI(ID_OUR_FILE_PRINT_PREVIEW_HD, OnUpdateFilePrintHDPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW_SEL_HD, OnUpdateFilePrintHDPreviewSel)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RESETSTATUS, OnUpdateResetStatus)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECTALL, OnUpdateEditSelectall)
	ON_UPDATE_COMMAND_UI(ID_PREFERENCES_LOCAL, OnUpdatePreferencesLocal)	
	ON_COMMAND(ID_EDIT_ADDBATCHHEADER, OnEditInsertBatchHeader)
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_WINSPLITFILE, OnEditWinsplitfile)
	ON_UPDATE_COMMAND_UI(ID_EDIT_WINSPLITFILE, OnUpdateEditWinsplitfile)	
	ON_COMMAND(ID_PREFERENCES_QUEUE, OnPreferencesQueue)
	ON_WM_DROPFILES()
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_TOOLS_REPORT_PRINT, OnToolsReportPrint)
	ON_COMMAND(ID_TOOLS_REPORT_PRINT_PREVIEW, OnToolsReportPrintPreview)
	ON_COMMAND(ID_EDIT_COPYATTS, OnEditCopyatts)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYATTS, OnUpdateEditCopyatts)
	ON_COMMAND(ID_EDIT_PASTEATTS, OnEditPasteatts)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTEATTS, OnUpdateEditPasteatts)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_PREFERENCES_QUEUE, OnUpdatePreferencesQueue)	
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)	
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)	
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)	
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)	
	ON_REGISTERED_MESSAGE(wm_DevModeChange, OnChangeDevMode)	
	ON_COMMAND(ID_PROCESS_CANCELIMAGING, OnCancel)
	ON_COMMAND(ID_PROCESS_PAUSEIMAGING, OnPause)
	ON_WM_RBUTTONDOWN()
	ON_WM_MOVE()
	ON_WM_SIZE()	
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)		
	ON_COMMAND(ID_OUR_FILE_PRINT, OnOurFilePrint)
	ON_COMMAND(ID_OUR_FILE_PRINT_INDEX, OnOurFilePrintIndex)
	ON_COMMAND(ID_OUR_FILE_PRINT_PREVIEW, OnOurFilePrintPreview)			
	ON_COMMAND(ID_OUR_FILE_PRINT_PREVIEW_HD, OnOurFilePrintHDPreview)			
	ON_COMMAND(ID_HELP, CListView::OnHelpFinder)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewFormView message handlers
                             
#define FBUFSIZ 32000                                

void CNewFormView::OnAddfiles()
{
   char* p;
   char* path;
   int idx;
   BOOL OneFile;
   CString cs;      
   CString csTitle;
   CString csFileDir;
   CSrting csFileName;
   CSrting csFileExt;
   CQueueFile* pCQueueFile;         
   char* szFilter;   
   
   szFilter = new char[4096];
   
   DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | 
      OFN_ALLOWMULTISELECT | OFN_NOCHANGEDIR;   

#ifdef RP30
   lstrcpy (szFilter, 
       "PostScript (*.ps; *.prn; *.cps; *.eps)|*.ps; *.prn; *.cps; *.eps|"       
	   "JPEG (*.jpg; *.jff, *.jtf)|*.jpg;*.jff;*.jtf|"	   
       "Targa (*.tga; *.rnd)|*.tga;*.rnd|TIFF (*.tif)|*.tif|CALS (*.cal)|*.cal|"       
	   "Adobe PDF (*.pdf)|*.pdf|"
       "Photo CD (*.pcd)|*.pcd|WordPerfect (*.wpg)|*.wpg|SUN Raster (*.ras)|*.ras|"       
	   "Windows & OS/2 (*.bmp)|*.bmp|"
       "PCX (*.pcx)|*.pcx|GIF (*.gif)|*.gif|"       
	   "PhotoShop 3.0 (*.psd)|*.psd|"
	   "Portable Network Graphics (*.png)|*.png|"
	   "Kodak FlashPix (*.fpx)|*.fpx|"	   
       "All Files (*.*)|*.*||");                  
#else
   lstrcpy (szFilter, 
       "PostScript (*.ps; *.prn; *.cps)|*.ps; *.prn; *.cps|"
       "Encapsulated PostScript (*.eps) |*.eps|"
       "Targa (*.tga; *.rnd)|*.tga;*.rnd|TIFF (*.tif)|*.tif|CALS (*.cal)|*.cal|"
       "MacPaint (*.mac)|*.mac|GEM Image (*.img)|*.img|Microsoft Paint (*.msp)|*.msp|"
       "Photo CD (*.pcd)|*.pcd|WordPerfect (*.wpg)|*.wpg|SUN Raster (*.ras)|*.ras|"
       "Macintosh PICTURE (*.pct)|*.pct|Windows & OS/2 (*.bmp)|*.bmp|"	   
       "PCX (*.pcx)|*.pcx|GIF (*.gif)|*.gif|"
       "JPEG (*.jpg; *.jff, *.jtf)|*.jpg;*.jff;*.jtf|"	   
	   "PhotoShop (*.psd)|*.psd|"
	   "Portable Network Graphics (*.png)|*.png|"
	   "Kodak FlashPix (*.fpx)|*.fpx|"	   
       "All Files (*.*)|*.*||");                  
#endif   
   // allocate buffer for filenames 
   char *FileBuf = new char[FBUFSIZ];    
   
   // construct common dialog
   CFileDialog cfd(TRUE, NULL, NULL, dwFlags, szFilter, this);     	
   
   // get instance handle
   cfd.m_ofn.hInstance = AfxGetInstanceHandle();                
   
   // load the initial filter index
   cfd.m_ofn.nFilterIndex = FilterIdx;
   
   // open file title
   csTitle.LoadString(IDS_ADDFILES_DLGPRMPT);
   cfd.m_ofn.lpstrTitle = csTitle;
   						 
   // point to FileBuf for names
   cfd.m_ofn.lpstrFile = FileBuf;
   cfd.m_ofn.lpstrFile[0] = 0; cfd.m_ofn.lpstrFile[1] = 0;
   
   // size of buffer
   cfd.m_ofn.nMaxFile = FBUFSIZ;        
   
   // set initial dir
   csFileDir = InitialFileDir;
   
   // set initial dir
   cfd.m_ofn.lpstrInitialDir = csFileDir;
   
   // do dialog
   if (cfd.DoModal() == IDOK)
      {  
      // load wait cursor
      AfxGetApp()->DoWaitCursor(1);

	  csFileName = cfd.GetFileExt();
	  csFileExt = cfd.GetFileExt();

	  // get row to insert at   	  
	  idx = (pLB->GetCount() == 0) ? 0 : (pLB->GetCaretIndex() + 1);

	  // if FileTitle is a NULL string, we have a multi 
	  // selection beginning at nFileOffset
      if (cfd.m_ofn.lpstrFileTitle[0] == '\0')
	     {
		 OneFile = FALSE;
		 path = cfd.m_ofn.lpstrFile;
		 // need to ignore NULL cfd.m_ofn.lpstrFile (caused by "c:\asd asd \")
		 // also ignore cfd.m_ofn.nFileOffset == 65535 randomness which appears to
		 // happen when a cd-rom is not ready
		 if ((strlen(cfd.m_ofn.lpstrFile) <= 0) || (cfd.m_ofn.nFileOffset == 65535))
		    p = NULL;	 
		 else
		    // first file is at offset
		    p = cfd.m_ofn.lpstrFile + cfd.m_ofn.nFileOffset;
		 }
	  else
	     {
		 OneFile = TRUE;		 
		 path = cfd.m_ofn.lpstrFile;
		 p = NULL;
		 }
      do
         {                
         // make string point to path
         cs = strlwr(path);                            		 
         // if more than one file, append filename
         if (!OneFile) 
            {       
			if (!p) break;			
            // append backslash if necessary
			if (cs.GetLength())
			   {			   
               if ((cs[cs.GetLength() - 1] != ':') && (cs[cs.GetLength() - 1] != '\\')) 
                  cs += "\\";
               cs += strlwr(p);
			   }
            }                  
         // try to insert it into the list		 
         if (pCQueueFile = new CQueueFile)
		    {
			if (pCQueueFile->Init(cs))
		       {
			   // mark as NOT selected
               //pCQueueFile->Selected = FALSE;  
               // not touched     
			   pCQueueFile->WasTouched = FALSE;
               // add index of new QueueFile to ListBox
               pLB->InsertString(idx, (LPCSTR)pCQueueFile);              			
               // store it
               PDOC->CQ.InsertAt(idx, pCQueueFile);	    
			   }
		    else
			   // error
		       delete pCQueueFile;
			}  
		 if (OneFile) break;
		 // get to next file... a double NULL is the end
		 p += (strlen(p) + 1);
		 if (p[0] == '\0') break;
         } while(1);
      // if nothing is selected, select the first item
      if ((pLB->GetSelCount() == 0)  && (pLB->GetCount() > 0))
         {
         pLB->SetSel(0);                     
         pLB->GetText(0, (LPSTR)&pCQueueFile);
         // get pointer to first item and set selection ON
         //pCQueueFile->Selected = TRUE;
         }         
      // reset state if we are currently idle
      if (InternalState == STATE_IDLE_NO_FILES) SetState(STATE_IDLE_SOME_FILES);               
      // our doc is modified
      PDOC->SetModifiedFlag();                          
      // if the info box is on, update it
	  UpdateInfo();		
	  
	  // reset initial file dir
	  CString FullPathString; 
	  CString EndPathString; 
	  _splitpath(OneFile ? path : (const char *)cs, FullPathString.GetBuffer(_MAX_DRIVE),
	     EndPathString.GetBuffer(_MAX_PATH), NULL, NULL);	  
      FullPathString.ReleaseBuffer();	  
	  EndPathString.ReleaseBuffer();	        	     
	  InitialFileDir = FullPathString + EndPathString;
	  // nuke trailing slash
	  if (InitialFileDir.GetLength() > _MAX_DRIVE)
	     InitialFileDir = 
	        InitialFileDir.Left(InitialFileDir.GetLength() - 1);	     
	  // reset cursor      
	  AfxGetApp()->DoWaitCursor(-1);
	  
 	  // save the initial filter index
      FilterIdx = cfd.m_ofn.nFilterIndex;   
	  }
	delete FileBuf;            
	delete szFilter;

   	// renumber the list
	RenumberList();

    // restart server?
    RestartServer();
    }

void CNewFormView::AddServerFile(CQueueFile* pCQueueFile)
   {   
   int idx;

   // get row to insert at  
   //idx = (pLB->GetCount() == 0) ? 0 : (pLB->GetCaretIndex() + 1);
   // add at end
   idx = pLB->GetCount();

   // mark as un-selected
   pCQueueFile->Selected = FALSE;       
   // not touched     
   pCQueueFile->WasTouched = FALSE;               
   // add index of new QueueFile to ListBox
   pLB->InsertString(idx, (LPCSTR)pCQueueFile);              
   // store it
   PDOC->CQ.InsertAt(idx, pCQueueFile);	    
   //pLB->SetSel(idx);
   // reset state if we are currently idle
   if (InternalState == STATE_IDLE_NO_FILES) SetState(STATE_IDLE_SOME_FILES);
   // our doc is modified
   PDOC->SetModifiedFlag();                          
   // if the info box is on, update it
   UpdateInfo();			  
   // renumber the list
   RenumberList();  
   // restart server
   RestartServer();
   }   
     
void CNewFormView::OnDeletefiles()
   {  
   int i;        
   CQueueFile* pCQueueFile;           
   int *LocalSelBuf;               
   int NumSelected;

   // if no files, return
   if (pLB->GetCount() <= 0) return;
   
   // alloc buffer to hold selections
   if (pLB->GetCount() > 0)
      LocalSelBuf = new int[pLB->GetCount()];    
   else
      LocalSelBuf = NULL;
   
   // get the items selected
   // can be no more than count items
   if ((NumSelected = pLB->GetSelItems(pLB->GetCount(), LocalSelBuf)) > 0)	
     {  
     // delete these items
     for (i = NumSelected - 1; i >= 0; i--)
        {                               
        // load the text string as a pointer into a 
        // pCQueueFile object pointer
        pLB->GetText(LocalSelBuf[i], (LPSTR)&pCQueueFile);
		// don't delete files that are imaging!
	    //if (pCQueueFile->StatusFlag == FILE_STATUS_INRIP) continue;      
		if ((pCQueueFile->StatusFlag & FILE_STATUS_INRIP) == FILE_STATUS_INRIP)
		   continue;      		
        // delete the file if it's a temporary file!
		if (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_SERVERFILE)
		   ::DeleteFile(pCQueueFile->LongName);	  
		// delete the CQueue object		
        delete pCQueueFile;
        // delete the string            
        pLB->DeleteString(LocalSelBuf[i]);
		// delete from the array
        PDOC->CQ.RemoveAt(LocalSelBuf[i]);	    
        }             
     // reselect the last item             
	 if ((pCQueueFile->StatusFlag & FILE_STATUS_INRIP) != FILE_STATUS_INRIP)
		{
        if ((LocalSelBuf[0] - 1) >= 0)
           pLB->SetSel(LocalSelBuf[0] - 1, TRUE);
		}
     }
  // delete the local buffer
  if (LocalSelBuf) delete LocalSelBuf;
  // if nothing is selected, and we have items, select the first item
  if ((pCQueueFile->StatusFlag & FILE_STATUS_INRIP) != FILE_STATUS_INRIP)
	 {
     if ((pLB->GetSelCount() == 0) && (pLB->GetCount() > 0))
        pLB->SetSel(0);                              
	 }     
  // reset state if we are currently idle
  if ((InternalState != STATE_PRINTING) && (InternalState != STATE_PAUSED))
     {
     if (pLB->GetCount() > 0)	    
		SetState(STATE_IDLE_SOME_FILES);		
     else                               	    
        SetState(STATE_IDLE_NO_FILES);   		
     }     
  // our doc is modified
  PDOC->SetModifiedFlag();                          
       
  // if the info box is on, update it
  UpdateInfo();   

  // renumber the list
  RenumberList();
  }              

void CNewFormView::OnOk()
{                  
    // get the description
    UpdateData(TRUE);    
       
	PostMessage(WM_COMMAND, ID_FILE_CLOSE);	
}

void CNewFormView::CancelIt()
   {   
   // call protected version
   OnCancel();
   }   

void CNewFormView::OnCancel()
   {   
   // set CancelFlag
   CancelFlag = TRUE;
   
   if (cr.IsInit) 
      {
	  cr.CancelProcess(TRUE);
	  // if we're in server, stop it
	  if (QUEUESERVER.m_status == STATUS_SERVER_ACTIVE)
	     QUEUESERVER.m_status = STATUS_SERVER_ENABLED;	  
 	  // reset server status
      pCQueueChild->SetServerFlag(QUEUESERVER.m_status);
	  }
   }   

void CNewFormView::ShowInfo()   
   {                                 
   // if it's not visible and we have files
   if ((!pCfDlg->IsWindowVisible()) && (pLB->GetSelCount() > 0))
      {
      // move the dialog
   	  pCfDlg->MoveDialog();
   	  // show it
   	  pCfDlg->ShowWindow(SW_SHOWNA);   	     	     	  
   	  }         
   // update the info      
   UpdateInfo();   
   }   
                             
void CNewFormView::UpdateInfo()
{               
    CQueueFile* pCQueueFile;    
    CString cs;
	int NumSelected;
	int i;

    NumSelected = pLB->GetSelCount();
    
	pCQueueChild->SetSelected(NumSelected, pLB->GetCount());
    
    // if we have no items, hide it
    if (NumSelected < 1) 
       {                       
       pCfDlg->ShowWindow(SW_HIDE);
       return;
       }        
    else
       {
       // else, if window is visible, make it topmost
       if (pCfDlg->IsWindowVisible())
          {                  
          pCfDlg->BringWindowToTop();
          }
       }    
    
    // load the text string as a pointer into a 
    // pCQueueFile object pointer
	if (NumSelected == 1)
	   {
       pLB->GetText(pLB->GetCaretIndex(), (LPSTR)&pCQueueFile);  
        
       // load queue pointer    
       pCfDlg->m_dialog_general.pCQueueFile = pCQueueFile; 
	   }
    
    // load CWnd                     
    pCfDlg->m_dialog_general.pWnd = this;
    pCfDlg->m_dialog_dev_setup.pWnd = this;
    pCfDlg->m_dialog_trans_setup.pWnd = this;
    pCfDlg->m_dialog_notes.pWnd = this;
    
    // get queue title	   
    pCfDlg->m_dialog_general.m_queuename = PDOC->GetTitle();
    
    // if a multiple selection, don't show filenames, etc...
    if (NumSelected > 1)
       {                    
       cs.LoadString(IDS_MULTIPLE_SELECTION);
       pCfDlg->m_dialog_general.m_filename = cs;       
       cs.LoadString(IDS_MULTIPLE_SELECTION_NAME);
       pCfDlg->m_dialog_general.m_shortfilename = cs;
       cs.LoadString(IDS_NOT_AVAIL);            
       pCfDlg->m_dialog_general.m_filetype =
       pCfDlg->m_dialog_general.m_filedate = 
       pCfDlg->m_dialog_general.m_filetime =
       pCfDlg->m_dialog_general.m_filesize =
       pCfDlg->m_dialog_general.m_description = 
       pCfDlg->m_dialog_general.m_preview_status = 
       pCfDlg->m_dialog_general.m_status = 
	   pCfDlg->m_dialog_general.m_pages = 
       pCfDlg->m_dialog_general.m_boundingbox = cs;
       // blank notes
       pCfDlg->m_dialog_notes.m_notes = _T("");
	   // we have new data!		
	   pCfDlg->m_dialog_notes.UpdateData(FALSE);	   
	   pCfDlg->m_dialog_notes.DisableNotes();	         
       // we have new data!	
	   pCfDlg->m_dialog_general.UpdateData(FALSE);	   
       }
    else
       {   
       pCfDlg->m_dialog_general.m_filename = pCQueueFile->LongName;
       pCQueueFile->GetFileTypeName(pCfDlg->m_dialog_general.m_filetype);
       pCfDlg->m_dialog_general.m_shortfilename = pCQueueFile->ShortName;
	   pCQueueFile->GetDateString(pCfDlg->m_dialog_general.m_filedate);
	   pCQueueFile->GetTimeString(pCfDlg->m_dialog_general.m_filetime);
	   // if the size is zero, AND it is not deleted update it
	   if ((pCQueueFile->iFileSize == 0) && 
		   ((pCQueueFile->StatusFlag & FILE_STATUS_CHECK) != FILE_STATUS_OK_DELETED))
		  {
		  CFileStatus cfStatus;
		  CFile::GetStatus(pCQueueFile->LongName, cfStatus);
		  pCQueueFile->RealFileSize = pCQueueFile->iFileSize = cfStatus.m_size;
		  }
	   pCQueueFile->GetSizeString(pCfDlg->m_dialog_general.m_filesize);
	   sprintf(pCfDlg->m_dialog_general.m_pages.GetBuffer(128), "%d", pCQueueFile->PageCount);
	   pCfDlg->m_dialog_general.m_pages.ReleaseBuffer();
	   pCfDlg->m_dialog_general.m_description = pCQueueFile->Description;
	   pCfDlg->m_dialog_general.m_boundingbox = pCQueueFile->BoundingBoxStr;
	   pCfDlg->m_dialog_general.m_preview_status = pCQueueFile->PreviewStatus;
	   pCfDlg->m_dialog_general.m_status = pCQueueFile->Status;
	   // we have new data!	
	   pCfDlg->m_dialog_general.UpdateData(FALSE);
	   pCfDlg->m_dialog_notes.m_notes = pCQueueFile->Notes;	   
	   // we have new data!		
	   pCfDlg->m_dialog_notes.UpdateData(FALSE);	   
	   pCfDlg->m_dialog_notes.EnableNotes();
	   }	   	   
	
	// if multiple files selected, set indeterminate fields             
	if (NumSelected > 1)                  
	   {	   
	   // gather all the devices into a CObArray and pass to
	   // local props
	   CObArray cod;
	   int* LocalSelBuf = new int[NumSelected];
	   pLB->GetSelItems(NumSelected, LocalSelBuf);
       for (i = 0; i < NumSelected; i++)
          {
          // get the queue item
          pLB->GetText(LocalSelBuf[i], (LPSTR)&pCQueueFile);
		  // add the device to the array
		  if ((pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS) && pCQueueFile->pCD)
		     cod.Add(pCQueueFile->pCD);
		  else
		     cod.Add(&QUEUEDEVICE);
          }                       	  	     
	   // set local prop fields
	   pCfDlg->m_dialog_dev_setup.SetMultiSelFields(cod);
	   delete LocalSelBuf;
	   cod.RemoveAll();
	   }
	else if ((pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS) && pCQueueFile->pCD)
	   {	   
	   pCfDlg->m_dialog_dev_setup.SetDevice(*(pCQueueFile->pCD));
	   // set target CQueue
	   pCfDlg->m_dialog_trans_setup.pTargetCQueue = pCQueueFile;	   
	   pCfDlg->m_dialog_trans_setup.pTargetDevice = pCQueueFile->pCD;
	   }	   
	else
	   {
	   pCfDlg->m_dialog_dev_setup.SetDevice(QUEUEDEVICE);	
	   // set target CQueue
	   pCfDlg->m_dialog_trans_setup.pTargetCQueue = pCQueueFile;    
	   // set Target devmode (will be used because the pDC is NULL)
	   pCfDlg->m_dialog_trans_setup.pTargetDevice = &QUEUEDEVICE;
	   }
	
	// if multiple files selected, set indeterminate fields
	if (NumSelected > 1)                  
	   {
	   // gather all the transforms into a CObArray and pass to
	   // local props
	   CObArray cod;
	   int* LocalSelBuf = new int[NumSelected];
	   pLB->GetSelItems(NumSelected, LocalSelBuf);
       for (i = 0; i < NumSelected; i++)
          {
          // get the queue item
          pLB->GetText(LocalSelBuf[i], (LPSTR)&pCQueueFile);
		  // add the device to the array
		  if ((pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS) && pCQueueFile->pTransform)
		     cod.Add(pCQueueFile->pTransform);
		  else
		     cod.Add(&QUEUETRANSFORM);
          }                       	  	     
	   // set local prop fields
	   pCfDlg->m_dialog_trans_setup.SetMultiSelFields(cod);
	   delete LocalSelBuf;
	   cod.RemoveAll();	   
	   }	
	else if ((pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS) && pCQueueFile->pTransform)
	   pCfDlg->m_dialog_trans_setup.SetDefaultTransform(*(pCQueueFile->pTransform));
	else
	   pCfDlg->m_dialog_trans_setup.SetDefaultTransform(QUEUETRANSFORM);	
}

void CNewFormView::OnSelectall()
   {   
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW)
      PSOUT_EDIT_WINDOW->SetSel(0, -1);
   else
      {
	  // if no files, return
      if (pLB->GetCount() <= 0) return;   
    
      // select all items
      pLB->SelItemRange(TRUE, 0, pLB->GetCount() - 1);                            
    
      // update the data
      UpdateInfo();		
	  }
   }   
                            
void CNewFormView::OnEditCopy()
   {   
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW)
      PSOUT_EDIT_WINDOW->Copy();
   else
      {
	  if (pLB->GetSelCount() > 0)
         CopyToClipboard(OURCLIP_FILE);
	  }
   }

void CNewFormView::CopyToClipboard(int fmt)
   {        
   int i;      
   CQueueFile* pCQueueFile;   
   CQueueFile* pNewCQueueFile;         
   int *LocalSelBuf;         
   CString csSection, csEntry;
   int NumSelected;

   // alloc buffer to hold selections
   // can be no more than count items
   if (pLB->GetCount() > 0)
      LocalSelBuf = new int[pLB->GetCount()];       
   else
      LocalSelBuf = NULL;
   
   // items selected?
   if ((NumSelected = pLB->GetSelItems(pLB->GetCount(), LocalSelBuf)) > 0)	
      {                         
	  // delete the current array if any
	  CLIPARRAY.Destroy();
	  // set the new type
	  CLIPARRAY.SetType(fmt);
	  // store the items
      for (i = NumSelected - 1; i >= 0; i--)
         {
         // get the queue item
         pLB->GetText(LocalSelBuf[i], (LPSTR)&pCQueueFile);
	     // unset serverfile flag so it can't be deleted		 
		 if (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_SERVERFILE)
		    pCQueueFile->ExtraInfo -= CQUEUE_EXTRA_SERVERFILE;		

         // make a copy
         pNewCQueueFile = new CQueueFile;
         *pNewCQueueFile = *pCQueueFile; 
		 // insert into the clip array
	     CLIPARRAY.InsertAt(NumSelected - i - 1, pNewCQueueFile);
         }                       	  
	  }  
   // delete the local buffer	   
   if (LocalSelBuf) delete LocalSelBuf;
   }  

int CNewFormView::SendFileToRIP(CQueueFile* pCQueueFile)
   {         
   UINT ret;               
   CString cs, csSection, csEntry;
   CDevice* pLocalDevice;
   CTransform* pLocalTransform;

   // set proper devmode      
   if ((pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS) && pCQueueFile->pCD)
      pLocalDevice = pCQueueFile->pCD;
   else
      pLocalDevice = &QUEUEDEVICE;
   
   // set proper transform
   if ((pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS) && pCQueueFile->pTransform)
      pLocalTransform = pCQueueFile->pTransform;
   else
      pLocalTransform = &QUEUETRANSFORM;

   // check file type
   if (pCQueueFile->FileType == FTYPE_UNKNOWN)
      return SYN_ERRMSG_BADFORMAT;

   if ( pCQueueFile->FileType == FTYPE_UNSUPPORTED)
      return SYN_ERRMSG_UNSUPPORTEDFORMAT;

   // in the rip
   ((CZeusDoc*)GetDocument())->InRip = TRUE;
   // set element pointer
   if (pRipElement) pRipElement->InRip = TRUE;
  
   // set screen saver
   SetScreenSaver(FALSE);

   // stop server polling    
   StopServerPolling();

   // open the rip object
   cr.Open();

   // init RIP   
   ret = cr.Init(pLocalDevice, pLocalTransform, pCQueueFile, 
      OutputType, this, (CMainFrame*)(AfxGetApp()->m_pMainWnd));	  
      
   // start RIP  
   if (ret == SYN_ERRMSG_ALLOK) ret = cr.StartRip();   
   
   // tell everyone we're processing
   if (ret == SYN_ERRMSG_ALLOK)
      {            
      if (OutputType == RIPSTYLE_PREVIEW || OutputType == RIPSTYLE_HD_PREVIEW)      
         {
         pCQueueFile->PreviewStatus.LoadString(IDS_SYN_PSSTAT_PROCESSING);         
         pCfDlg->m_dialog_general.m_preview_status = pCQueueFile->PreviewStatus;
         }
      else
         {                                                                         
         pCQueueFile->Status.LoadString(IDS_SYN_PSSTAT_PROCESSING);               
         pCfDlg->m_dialog_general.m_status = pCQueueFile->Status;
         }
      }
   
   // update data in prefs dlg   
   pCfDlg->m_dialog_general.UpdateData(FALSE);	

   return ret;   
   }  
   
LRESULT CNewFormView::OnRipMessage(WPARAM WParam, LPARAM LParam)
   {                       
   PRIPMSGSTRUCT pMsgStruct;      
   
   // cast the LParam to a ripmsg pointer
   pMsgStruct = (PRIPMSGSTRUCT)LParam;

   // do message stuff based on msg type
   switch(pMsgStruct->MsgType)													 
      {
      case RIPMSG_READING:           
		   pCQueueChild->SetProcessStatus(STATUS_READING, pMsgStruct->PercentComplete);
           break;
      case RIPMSG_READING_TOTAL:           
		   pCQueueChild->SetProcessStatus(STATUS_READING_TOTAL, pMsgStruct->TotalComplete);
           break;
      case RIPMSG_SCANLINE:   
           pCQueueChild->SetProcessStatus(STATUS_SCANLINE, pMsgStruct->LineOn, 
		      pMsgStruct->LineTotal);           
           break;
      case RIPMSG_PRINTING:
		   pCQueueChild->SetProcessStatus(STATUS_PRINTING, pMsgStruct->LineOn, 
		      pMsgStruct->LineTotal, pMsgStruct->ColorOn);
           break;
      case RIPMSG_INFO:
		   pCQueueChild->SetStatus1(pMsgStruct->Info);
           break;
      case RIPMSG_RENDERTITLE:
           pCQueueChild->SetProcessStatus(STATUS_RENDERING);
		   break;
      case RIPMSG_INFO2:
           // do the prompt		   
		   pCQueueChild->SetProcessStatus(pMsgStruct->Info);
           break;
      case RIPMSG_PRINTING_TP:
		   pCQueueChild->SetProcessStatus(STATUS_PRINTING_TP, pMsgStruct->LineOn, 
		      pMsgStruct->LineTotal, pMsgStruct->ColorOn, pMsgStruct->TotalComplete);
           break;
      }
   // tell the caller if we want to pause
   return (LRESULT)PauseFlag;
   }      

LRESULT CNewFormView::OnRipPSMessage(WPARAM WParam, LPARAM LParam)
   {                       
   pCQueueChild->PSOut((BYTE*)LParam, (int)WParam);
   return 0L;
   }

void CNewFormView::OnEditDelete()
   {
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW)
      {
	  PSOUT_EDIT_WINDOW->SetReadOnly(FALSE);
	  PSOUT_EDIT_WINDOW->Clear();	  
	  PSOUT_EDIT_WINDOW->SetReadOnly(TRUE);
	  }
   else
      OnDeletefiles();	
   }

void CNewFormView::OnOurFilePrintPreview()
   {                          
   // if no files in queue, return
   if (pLB->GetCount() < 1) return;   

   // none were touched!
   SetNoneTouched();        
   
   // set output type
   OutputType = RIPSTYLE_PREVIEW;

   // rip ALL
   QueueType = QUEUETYPE_ALL;   
   
   Print();
   }

void CNewFormView::OnOurFilePrintPreview()
{                          
	// if no files in queue, return
	if (pLB->GetCount() < 1)
		return;   

	// none were touched!
	SetNoneTouched();        

	// set output type
	OutputType = RIPSTYLE_HD_PREVIEW;

	// rip ALL
	QueueType = QUEUETYPE_ALL;   

	Print();
}

void CNewFormView::OnOurFilePrint()
   {     
   // if no files in queue, return
   if (pLB->GetCount() < 1) return;   

   // none were touched!
   SetNoneTouched();         
   
   // set output type
   OutputType = RIPSTYLE_OUTPUT;

   // rip ALL
   QueueType = QUEUETYPE_ALL;

   Print();
   }   

void CNewFormView::OnOurFilePrintIndex()
{     
   // if no files in queue, return
   if (pLB->GetCount() < 1)
	   return;

   // none were touched!
   SetNoneTouched();         
   
   // set output type
   OutputType = RIPSTYLE_OUTPUT;

   // rip ALL
   QueueType = QUEUETYPE_ALL;

   Print();
}   

void CNewFormView::OnPreviewSel()
   {                  
   // if no files selected, return
   if (pLB->GetSelCount() < 1) return;   

   // none were touched!
   SetNoneTouched();   
   
   // set output type
   OutputType = RIPSTYLE_PREVIEW;

   // rip selected
   QueueType = QUEUETYPE_SELECTED;
   
   PrintSel();   
   }          

void CNewFormView::OnPrintSel()
   {               
   // if no files selected, return
   if (pLB->GetSelCount() < 1) return;   

   // none were touched!
   SetNoneTouched();
   
   // set output type
   OutputType = RIPSTYLE_OUTPUT;

   // rip selected
   QueueType = QUEUETYPE_SELECTED;

   PrintSel();   	
   }
         
void CNewFormView::PrintSel()
   { 
   // set the CancelFlag
   CancelFlag = FALSE;
   
   // set the PauseFlag
   PauseFlag = FALSE;   
   
   // set the Rip complete flag;
   RipComplete = TRUE;
   
   // get the queue start time
   ((CZeusDoc*)GetDocument())->QueueStartTime = CTime::GetCurrentTime();

   // start timer   
   if (!SetTimer(RP95_PRINT_TIMER_ID, RP95_PRINT_TIMER_TIMEOUT, NULL))
      AfxMessageBox(IDS_SYS_TIMER_ERROR, MB_OK | MB_ICONEXCLAMATION);      
   else
      {
	  LastTimeCheck = GetTickCount();
      }                    
   }

void CNewFormView::Print()
   {
   // set the CancelFlag
   CancelFlag = FALSE;
   
   // set the PauseFlag
   PauseFlag = FALSE;
   
   // set the Rip complete flag;
   RipComplete = TRUE;   
   
   // get the queue start time
   ((CZeusDoc*)GetDocument())->QueueStartTime = CTime::GetCurrentTime();   

   // our doc is modified
   ((CZeusDoc*)GetDocument())->SetModifiedFlag();                                   

   // start the timer   
   if (!SetTimer(RP95_PRINT_TIMER_ID, RP95_PRINT_TIMER_TIMEOUT, NULL))
      AfxMessageBox(IDS_SYS_TIMER_ERROR, MB_OK | MB_ICONEXCLAMATION);           
   else
      {
	  LastTimeCheck = GetTickCount();
	  }
   }             

void CNewFormView::OnContinue()
   {
   if (cr.IsInit) cr.CancelProcess(FALSE);
   }

void CNewFormView::OnDeselectall()
   {
   // if no files, return
   if (pLB->GetCount() <= 0) return;   
   
   // de-select all items
   pLB->SelItemRange(FALSE, 0, pLB->GetCount() - 1);                        
    
   // update the data
   UpdateInfo();		
   }   
                            
void CNewFormView::CreateFileInfoDlg()
   {                     
   // if the instance counter is zero, init the class 
   // and create the window                      
   if (*pInstanceCntr == 0)
      {
      if (pCfDlg = new CFileInfo(IDS_FILE_PROPERTIES, AfxGetApp()->m_pMainWnd))
         {                                            
         // show close!         
         pCfDlg->m_dialog_dev_setup.ShowOK(FALSE);            
         pCfDlg->m_dialog_trans_setup.ShowOK(FALSE);
         // report when device has changed         
         pCfDlg->m_dialog_dev_setup.ReportChange = PREFS_CHANGE_REPORT_YES;
         pCfDlg->m_dialog_trans_setup.ReportChange = PREFS_CHANGE_REPORT_YES;
		 pCfDlg->m_dialog_notes.ReportChange = PREFS_CHANGE_REPORT_YES;
         // create it
         //pCfDlg->Create(IDD_DIALOG_PREFS_MAIN, AfxGetApp()->m_pMainWnd);
		 pCfDlg->Create(AfxGetApp()->m_pMainWnd, WS_CHILD | WS_CAPTION);
         // store pointer in main frame
         ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pCfDlg = pCfDlg;         
         }
      }  
   (*pInstanceCntr)++;   
   }      
         
void CNewFormView::DestroyFileInfoDlg()
   {                      
   (*pInstanceCntr)--;
   // if the instance counter is zero, destroy the window and 
   // delete the class    
   if (*pInstanceCntr == 0)
      {
      pCfDlg->DestroyWindow();
      delete pCfDlg;
      }
   }   

void CNewFormView::OnPause()
   {   
   // toggle setting
   if (PauseFlag)
      {     
	  // set flag
      PauseFlag = FALSE;         
      // set buttons
      SetState(STATE_PRINTING);            
      // call the CRip Pause method which will 
      // suspend the rip thread
      cr.ResumeProcess();	  
	  // reset status
	  pCQueueChild->SetQueueStatus(LastQueueStatus);	  
      }   
   else   
      {     
	  // set flag
      PauseFlag = TRUE;         
      // set buttons
      SetState(STATE_PAUSED);            
      // call the CRip Pause method which will 
      // resume the rip thread
      cr.PauseProcess();
	  // set status
	  LastQueueStatus = pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_PAUSED);	  
	  }
   }       
   
void CNewFormView::SetState(int State)
   {                  
   InternalState = State;

   // special if no rip is loaded
   if (!cr.RipLoaded) State = STATE_IDLE_RIP_NOTAVAIL;
      
   switch(State)
      {
	  case STATE_IDLE_RIP_NOTAVAIL:
      case STATE_IDLE_NO_FILES:                      
           CmdCancelActive = FALSE;
           CmdAddFiles = TRUE;
		   CmdQueuePreferences = TRUE;
           CmdSkip = FALSE;
           CmdPause = FALSE;
           CmdResume = FALSE;
           CmdFilePrint = FALSE;
           CmdFilePrintSel = FALSE;
           CmdFilePrintPreview = FALSE;
           CmdFilePrintPreviewSel = FALSE;
           CmdFilePrintHDPreview = FALSE;
           CmdFilePrintHDPreviewSel = FALSE;
           CmdSelectAll = FALSE;
           CmdDelete = FALSE;
           break;
      case STATE_IDLE_SOME_FILES:         
           CmdCancelActive = FALSE;
           CmdAddFiles = TRUE;           
		   CmdQueuePreferences = TRUE;           
           CmdSkip = FALSE;
           CmdPause = FALSE;
           CmdResume = FALSE;
           CmdFilePrint = TRUE;
           CmdFilePrintSel = TRUE;
           CmdFilePrintPreview = TRUE;
           CmdFilePrintPreviewSel = TRUE;
           CmdFilePrintHDPreview = TRUE;
           CmdFilePrintHDPreviewSel = TRUE;
           CmdSelectAll = TRUE;
           CmdDelete = TRUE;
           break;
      case STATE_PRINTING:      
           CmdCancelActive = TRUE;
           CmdAddFiles = FALSE;
		   CmdQueuePreferences = FALSE;
           CmdSkip = TRUE;
           CmdPause = TRUE;
           CmdResume = FALSE;
           CmdFilePrint = FALSE;
           CmdFilePrintSel = FALSE;
           CmdFilePrintPreview = FALSE;
           CmdFilePrintPreviewSel = FALSE;
           CmdFilePrintHDPreview = FALSE;
           CmdFilePrintHDPreviewSel = FALSE;
           CmdSelectAll = FALSE;
           CmdDelete = FALSE;
           break;
      case STATE_PAUSED:
           CmdCancelActive = FALSE;
           CmdAddFiles = TRUE;
		   CmdQueuePreferences = TRUE;
           CmdSkip = FALSE;
           CmdPause = FALSE;
           CmdResume = TRUE;
           CmdFilePrint = FALSE;
           CmdFilePrintSel = FALSE;
           CmdFilePrintPreview = FALSE;
           CmdFilePrintPreviewSel = FALSE;
           CmdFilePrintHDPreview = FALSE;
           CmdFilePrintHDPreviewSel = FALSE;
           CmdSelectAll = TRUE;
           CmdDelete = TRUE;
		   break;
      }     
   }   

void CNewFormView::OnUpdateProcessCancelimaging(CCmdUI* pCmdUI)
   {					   
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      pCmdUI->Enable(FALSE);
   else
      pCmdUI->Enable(CmdCancelActive);	
   }

void CNewFormView::OnUpdateEditAddfiles(CCmdUI* pCmdUI)
   {
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      pCmdUI->Enable(FALSE);
   else
      pCmdUI->Enable(CmdAddFiles);	
   }

void CNewFormView::OnUpdateProcessSkiptonextfile(CCmdUI* pCmdUI)
   {
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      pCmdUI->Enable(FALSE);
   else
      pCmdUI->Enable(CmdSkip);
   }

void CNewFormView::OnUpdateProcessPauseimaging(CCmdUI* pCmdUI)
   {
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      pCmdUI->Enable(FALSE);
   else
      pCmdUI->Enable(CmdPause);
   }

void CNewFormView::OnUpdateProcessResumeimaging(CCmdUI* pCmdUI)
   {
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      pCmdUI->Enable(FALSE);
   else
      pCmdUI->Enable(CmdResume);
   }

void CNewFormView::OnUpdateFilePrint(CCmdUI* pCmdUI)
   {  
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      pCmdUI->Enable(FALSE);
   else       
      pCmdUI->Enable(CmdFilePrint && pLB->GetCount() > 0);		
   }

void CNewFormView::OnUpdateFilePrintSel(CCmdUI* pCmdUI)
   {  
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      pCmdUI->Enable(FALSE);
   else    
      pCmdUI->Enable(CmdFilePrintSel && pLB->GetSelCount() > 0);		
   }

void CNewFormView::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
   {
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      pCmdUI->Enable(FALSE);
   else
      pCmdUI->Enable(CmdFilePrintPreview && pLB->GetCount() > 0);		
   }

void CNewFormView::OnUpdateFilePrintPreviewSel(CCmdUI* pCmdUI)
   {  
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      pCmdUI->Enable(FALSE);
   else
      pCmdUI->Enable(CmdFilePrintPreviewSel && pLB->GetSelCount() > 0);			
   }

void CNewFormView::OnUpdateFilePrintHDPreview(CCmdUI* pCmdUI)
   {
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      pCmdUI->Enable(FALSE);
   else
      pCmdUI->Enable(CmdFilePrintHDPreview && pLB->GetCount() > 0);		
   }

void CNewFormView::OnUpdateFilePrintHDPreviewSel(CCmdUI* pCmdUI)
   {  
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      pCmdUI->Enable(FALSE);
   else
      pCmdUI->Enable(CmdFilePrintHDPreviewSel && pLB->GetSelCount() > 0);			
   }

void CNewFormView::OnEditPaste()
   {	
    int i, idx;            
    CQueueFile* pCQueueFile;  
	
	if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) return; 
    
	// get row to insert at   	  
	idx = (pLB->GetCount() == 0) ? 0 : (pLB->GetCaretIndex() + 1);
	
	// available?
	if (CLIPARRAY.IsTypeAvail(OURCLIP_FILE))
	   {
	   // get all elements and insert them
	   for (i = 0; i < CLIPARRAY.GetSize(); i++)
	      {	                  
	      // create a new queue file
	      pCQueueFile = new CQueueFile;
	      // copy the data
	      *pCQueueFile = *((CQueueFile*)(CLIPARRAY.GetAt(i)));
		  // don't paste with inrip status - make it a hold
	      //if (pCQueueFile->StatusFlag == FILE_STATUS_INRIP)
		  if ((pCQueueFile->StatusFlag & FILE_STATUS_INRIP) == FILE_STATUS_INRIP) 		   		  
	         {
	         pCQueueFile->Status.LoadString(IDS_NOT_AVAIL);
             pCQueueFile->PreviewStatus.LoadString(IDS_NOT_AVAIL);
             //pCQueueFile->StatusFlag = FILE_STATUS_HOLD;   
			 pCQueueFile->StatusFlag |= FILE_STATUS_HOLD;   
		     }
	      // add index of new QueueFile to ListBox
	      pLB->InsertString(idx, (LPCSTR)pCQueueFile);	      
          // store it
          PDOC->CQ.InsertAt(idx, pCQueueFile);	    
	      }	   
       // reset state if we are currently idle
       if (InternalState == STATE_IDLE_NO_FILES) SetState(STATE_IDLE_SOME_FILES);   
       // our doc is modified
       PDOC->SetModifiedFlag();                          
       // if the info box is on, update it
	   UpdateInfo();			  	      
	   }	
   // renumber the list
   RenumberList();  

   // restart server?
   RestartServer();
   }

void CNewFormView::OnEditCut()
   {   
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW)
      {	  
	  PSOUT_EDIT_WINDOW->SetReadOnly(FALSE);
	  PSOUT_EDIT_WINDOW->Cut();	  
	  PSOUT_EDIT_WINDOW->SetReadOnly(TRUE);
	  }
   else
      {
	  // first copy the files
      OnEditCopy();          
      // now delete the files
      OnDeletefiles();	
	  }
   }

void CNewFormView::OnUpdateEditCut(CCmdUI* pCmdUI)
   {   	
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW)
      pCmdUI->Enable(PSOUT_HAVE_SELTEXT);
   else
      pCmdUI->Enable(pLB->GetSelCount() > 0 ? TRUE : FALSE);		
   }

void CNewFormView::OnUpdateEditCopy(CCmdUI* pCmdUI)
   {   	
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW)
      pCmdUI->Enable(PSOUT_HAVE_SELTEXT);
   else
      pCmdUI->Enable(pLB->GetSelCount() > 0 ? TRUE : FALSE);	
   }

void CNewFormView::OnUpdateEditDelete(CCmdUI* pCmdUI)
   {  
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW)
      pCmdUI->Enable(PSOUT_HAVE_SELTEXT);
   else 	
      pCmdUI->Enable(pLB->GetSelCount() > 0 ? TRUE : FALSE);			
   }

void CNewFormView::OnUpdateEditSelectall(CCmdUI* pCmdUI)
   {
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW)
      pCmdUI->Enable(!PSOUT_EMPTY);
   else
      pCmdUI->Enable(pLB->GetCount() > 0 ? TRUE : FALSE);		
   }

void CNewFormView::OnUpdatePreferencesLocal(CCmdUI* pCmdUI)
   {     
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      pCmdUI->Enable(FALSE);
   else
      pCmdUI->Enable(pLB->GetSelCount() > 0 ? TRUE : FALSE);		
   }

LRESULT CNewFormView::OnChangeDevMode(WPARAM WParam, LPARAM LParam)
   {                       
   CQueueFile* pCQueueFile;            
   int *LocalSelBuf;
   int i;
   CDevice* pPassedDevice;
   CTransform* pPassedTransform;
   CString* pPassedNotes;
   int NumSelected;

   if (!(WParam & PREFS_CHANGE_REPORT_YES))
	  {
	  // set the new target device devmode
	  if (WParam & PREFS_CHANGE_DEVICE)  	  
		 {
		 if (LParam != NULL)		 
	        pPrefsDlg->m_dialog_trans_setup.pTargetDevice = (CDevice*)LParam;
		 }
	  return 0L;
	  }	
   else
	  {
	  if (WParam & PREFS_CHANGE_DEVICE)  	  
		 {
		 if (LParam != NULL)		 
	        pCfDlg->m_dialog_trans_setup.pTargetDevice = (CDevice*)LParam;
		 }
	  }
   
   // retrieve data
   UpdateData(TRUE);          
    
   // make sure we have something
   if (pLB->GetCount() > 0)
      LocalSelBuf = new int[pLB->GetCount()];    
   else
      return 0L;   
   
   if (WParam & PREFS_CHANGE_TRANSFORM)
      {                        
      // a zero here means global
	  if ((pPassedTransform = (CTransform*)LParam) == NULL)       
         pPassedTransform = &QUEUETRANSFORM;	  
      // get the number of items selected
      NumSelected = pLB->GetSelItems(pLB->GetCount(), LocalSelBuf);        
      // get the selected items
      for (i = 0; i < NumSelected; i++)
         {                               
         // load the text string as a pointer into a 
         // pCQueueFile object pointer
         pLB->GetText(LocalSelBuf[i], (LPSTR)&pCQueueFile);        
         // do we have a transform now?
         if (pCQueueFile->pTransform)
            {                
            // if it's the same, then do nothing
            if (*(pCQueueFile->pTransform) == *(pPassedTransform))
               continue;                                     
            else
               {                                                        
               // delete the existing transform
               delete pCQueueFile->pTransform;
               // if its the same as global reset the prefs
               if (*(pPassedTransform) == QUEUETRANSFORM)
                  {                                              
                  pCQueueFile->pTransform = NULL; 
                  if (pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS)
                     pCQueueFile->Prefs -= USE_LOCAL_TRN_PREFS;
                  }
               else   
                  {                                      
                  // make new transform
                  pCQueueFile->pTransform = new CTransform;
                  // store transform
                  *(pCQueueFile->pTransform) = *(pPassedTransform);
                  // set status based on transform
                  pCQueueFile->Prefs |= USE_LOCAL_TRN_PREFS;
                  }
			   PDOC->SetModifiedFlag();
               }
            }   
         else
            {            
            // no transform, so let's compare it to global
            // if the same, do nothing
            if (*(pPassedTransform) == QUEUETRANSFORM)
               {               
               if (pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS)
                  pCQueueFile->Prefs -= USE_LOCAL_TRN_PREFS;            
               continue;                  
               }
            else   
               {               
               // make new transform
               pCQueueFile->pTransform = new CTransform;
               // store transform
               *(pCQueueFile->pTransform) = *(pPassedTransform);
               // change status 
               pCQueueFile->Prefs |= USE_LOCAL_TRN_PREFS;        
               }
            }                           
         // redraw it   
         RedrawItem(LocalSelBuf[i]);
		 PDOC->SetModifiedFlag();
         }     
      // a zero here means global
      if (LParam == 0) UpdateInfo();
      }
   else if (WParam & PREFS_CHANGE_DEVICE)
      {            
      // a zero here means global
	  if ((pPassedDevice = (CDevice*)LParam) == NULL)
         pPassedDevice = &QUEUEDEVICE;
      // get the number of items selected
      NumSelected = pLB->GetSelItems(pLB->GetCount(), LocalSelBuf);        
      // get the selected items
      for (i = 0; i < NumSelected; i++)
         {                               
         // load the text string as a pointer into a 
         // pCQueueFile object pointer
         pLB->GetText(LocalSelBuf[i], (LPSTR)&pCQueueFile);        
         // do we have a devmode now?
         if (pCQueueFile->pCD)
            {                
            // if it's the same, then do nothing
            if (*(pCQueueFile->pCD) == *(pPassedDevice))
               continue;                                     
            else
               {                                                        
               // if its the same as global reset the prefs
               if (*(pPassedDevice) == QUEUEDEVICE)                  
                  {                                              
				  // delete the existing devmode
                  delete pCQueueFile->pCD;               
                  pCQueueFile->pCD = NULL; 
                  if (pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS)
                     pCQueueFile->Prefs -= USE_LOCAL_DEV_PREFS;
                  }
               else   
                  {                                      
                  // overwrite devmode
                  *(pCQueueFile->pCD) = *(pPassedDevice);
                  // set status based on devmode
                  pCQueueFile->Prefs |= USE_LOCAL_DEV_PREFS;
                  }
			   PDOC->SetModifiedFlag();
               }
            }   
         else
            {            
            // no devmode, so let's compare it to global
            // if the same, do nothing
            if (*(pPassedDevice) == QUEUEDEVICE)
               {               
               if (pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS)
                  pCQueueFile->Prefs -= USE_LOCAL_DEV_PREFS;            
               continue;                  
               }
            else   
               {               
               // make new devmode
               pCQueueFile->pCD = new CDevice;
               // store devmode
               *(pCQueueFile->pCD) = *(pPassedDevice);
               // change status 
               pCQueueFile->Prefs |= USE_LOCAL_DEV_PREFS;        
               }
			PDOC->SetModifiedFlag();
            }                           
         // redraw it   
         RedrawItem(LocalSelBuf[i]);
         }     
	  
      // a zero here means global
      if (LParam == 0) UpdateInfo();			  
      }
   else if (WParam & PREFS_CHANGE_NOTES)
      {                        
      // check pointer validity
      if ((pPassedNotes = (CString*)LParam) == NULL) 
         return 0L;      
      NumSelected = pLB->GetSelItems(pLB->GetCount(), LocalSelBuf);
      // should only get here if one item is selected
      ASSERT(NumSelected == 1);
      // load the text string as a pointer into a 
      // pCQueueFile object pointer
      pLB->GetText(LocalSelBuf[0], (LPSTR)&pCQueueFile);        
      // set notes
      pCQueueFile->Notes = *pPassedNotes;

	  PDOC->SetModifiedFlag();
      }
   // delete the local buffer
   if (LocalSelBuf) delete LocalSelBuf;
  
   return 0L;
   }
         
CQueueFile* CNewFormView::GetBestQueueFileForTransform()
   {
   CQueueFile* pCQueueFile;   

   pCQueueFile = NULL;
   
   //get selection count
   if (pLB->GetSelCount() > 0)
      {
      // create a new selection buffer   
      int *SelectionBuffer = new int[pLB->GetCount()];
      
      // get the items selected
      pLB->GetSelItems(pLB->GetCount(), SelectionBuffer);
      
      // get the first selection
      pLB->GetText(SelectionBuffer[0], (LPSTR)&pCQueueFile);
      
      // delete the selection buffer
      delete SelectionBuffer;
      }
   else if (pLB->GetCount() > 0)
      pLB->GetText(pLB->GetCaretIndex(), (LPSTR)&pCQueueFile);
   else
      pCQueueFile = NULL;
      
   return pCQueueFile;
   }                                                  
   
void CNewFormView::EditExistingBatchHeader(CQueueFile* pCQueueFile)
   {           
   CBheader cb;                 
   
   // set existing options      
   cb.m_date = (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_DATE) ? TRUE : FALSE;
   cb.m_time = (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_TIME) ? TRUE : FALSE;
   cb.m_imagecorners = (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_CORNERS) ? TRUE : FALSE;
   cb.m_show_imagecount = (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_COUNT) ? TRUE : FALSE;
   if (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_BACKWHITE)
      cb.m_background = BATCHHEADER_BACKGROUND_WHITE;
   else if (pCQueueFile->BatchHeaderOptions & OPTIONS_BATCHHEADER_BACKBLACK)
      cb.m_background = BATCHHEADER_BACKGROUND_BLACK;
   else
      cb.m_background = BATCHHEADER_BACKGROUND_NONE;
           
   // get notes
   if(pCQueueFile->BatchHeaderNotes.GetLength()) 
      {                                            
      // if there is a tab, extract the first part                                             
      int TabPos;
      if ((TabPos = pCQueueFile->BatchHeaderNotes.Find("\t")) > 0)
         {
         cb.m_notes1 = pCQueueFile->BatchHeaderNotes.Left(TabPos);
         cb.m_notes2 = 
            pCQueueFile->BatchHeaderNotes.Right(pCQueueFile->BatchHeaderNotes.GetLength() - TabPos - 1);
         }
      else
         {   
         cb.m_notes1 = pCQueueFile->BatchHeaderNotes;
         cb.m_notes2 = _T("");
         }
      }   

   // do the dialog stuff   
   DoBatchHeaderDialog(cb, pCQueueFile->BatchHeaderCount, pCQueueFile);

   ((CZeusDoc*)GetDocument())->SetModifiedFlag();
   }   

void CNewFormView::OnEditInsertBatchHeader()
   {
   int i, idx;
   CQueueFile* pCQueueFile;
   int ImageCount;
   CBheader cb;

   // get row to insert at   	  
   idx = (pLB->GetCount() == 0) ? 0 : (pLB->GetCaretIndex());
      
   // default image count is queue size
   ImageCount = pLB->GetCount();
   // get next bheader
   for (i = idx; i < pLB->GetCount(); i++)
      {
      pLB->GetText(i, (LPSTR)&pCQueueFile);
      // the next blist is here so the count is up to this
      if (pCQueueFile->FileType == FTYPE_BLIST)
         {
         ImageCount = i;
         break;
         }
      }   
   
   // create a new queue file object
   if (!(pCQueueFile = new CQueueFile)) return;      
   if (!(pCQueueFile->InitBListFile())) return;         
   
   // if user cancels, delete, else
   // if OK, add index of new QueueFile to ListBox
   if (DoBatchHeaderDialog(cb, ImageCount, pCQueueFile) == IDOK)
      {
      pLB->InsertString(idx, (LPCSTR)pCQueueFile);     
      // store it
      PDOC->CQ.InsertAt(idx, pCQueueFile);	    
	  // not touched     
	  pCQueueFile->WasTouched = FALSE;               
      // mark as selected
      //pCQueueFile->Selected = TRUE;
      pLB->SetSel(idx);
      // if the info box is on, update it
      UpdateInfo();			  
	  // reset state if we are currently idle
      if (InternalState == STATE_IDLE_NO_FILES) SetState(STATE_IDLE_SOME_FILES);
	  // renumber the list
	  RenumberList();  
      }
   else   
      delete pCQueueFile;
   
   // restart server?
   RestartServer();
   
   ((CZeusDoc*)GetDocument())->SetModifiedFlag();
   }      
   
int CNewFormView::DoBatchHeaderDialog(CBheader& cb, int ImageCount, 
   CQueueFile* pCQueueFile)
   {                                         
   char NumStr[10];
   
   // default image count is queue size
   cb.m_edit_imagecount = itoa(ImageCount, NumStr, 10);
   
   // show the dialog
   if (cb.DoModal() == IDCANCEL) return (IDCANCEL);   
   
   // start with nothing, in case we are re-editing
   pCQueueFile->BatchHeaderOptions = 0;
   
   // get new settings
   if (cb.m_date)
      pCQueueFile->BatchHeaderOptions |= OPTIONS_BATCHHEADER_DATE;
   if (cb.m_time)
      pCQueueFile->BatchHeaderOptions |= OPTIONS_BATCHHEADER_TIME;
   if (cb.m_imagecorners)
      pCQueueFile->BatchHeaderOptions |= OPTIONS_BATCHHEADER_CORNERS;
   if (cb.m_show_imagecount)
      pCQueueFile->BatchHeaderOptions |= OPTIONS_BATCHHEADER_COUNT;
   if (cb.m_notes1.GetLength() > 0)
      pCQueueFile->BatchHeaderNotes = cb.m_notes1;    
   if (cb.m_notes2.GetLength() > 0)
      {
      pCQueueFile->BatchHeaderNotes += "\t"; // add a tab
      pCQueueFile->BatchHeaderNotes += cb.m_notes2;    
      }
   pCQueueFile->BatchHeaderCount = atoi(cb.m_edit_imagecount);
   
   // get background
   switch(cb.m_background)
      {	   
      case BATCHHEADER_BACKGROUND_WHITE:
           pCQueueFile->BatchHeaderOptions |= OPTIONS_BATCHHEADER_BACKWHITE;
           break;
      case BATCHHEADER_BACKGROUND_BLACK:
           pCQueueFile->BatchHeaderOptions |= OPTIONS_BATCHHEADER_BACKBLACK;
           break;
      case BATCHHEADER_BACKGROUND_NONE:
           pCQueueFile->BatchHeaderOptions |= OPTIONS_BATCHHEADER_BACKNONE;
           break;
      }              
   
   return(IDOK);   
   }

void CNewFormView::WriteTheCObArray()
   {
   int i;
   CQueueFile* pCQueueFile;   
   
   return;
   
   // delete the CObArray   
   PDOC->CQ.RemoveAll();      

   if (!pLB) return;
   
   // re-store all the items in the CObArray in the proper
   // order
   for (i = 0; i < pLB->GetCount(); i++)
      {
      // get the item
      pLB->GetText(i, (LPSTR)&pCQueueFile);
      // store it
      PDOC->CQ.InsertAt(i, pCQueueFile);	    
      }	
   }

void CNewFormView::DeleteAllTempFiles()
   {
   int i;
   CQueueFile* pCQueueFile;         
   
   if (!pLB) return;
   
   // re-store all the items in the CObArray in the proper
   // order
   for (i = 0; i < pLB->GetCount(); i++)
      {
      // get the item
      pLB->GetText(i, (LPSTR)&pCQueueFile);
	  if (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_SERVERFILE)
		 ::DeleteFile(pCQueueFile->LongName);	  
      }	
   }	

void CNewFormView::OnDestroy()
   {
   // stop polling
   StopServerPolling();

   // make sure the data is written	
   WriteTheCObArray();   

   CListView::OnDestroy();
   }

void CNewFormView::OnEditWinsplitfile()
   {  
   int i, Select;
   CQueueFile* pCQueueFile;      
   CQueueFile* pTargetCQueueFile;      
   SplitDlg* pSP;     
   CString cs;

   // only do this if one file is selected
   if (pLB->GetSelCount() == 1)
      {                  
      pLB->GetSelItems(1, &Select);        
      // get the queuefile
      pLB->GetText(Select, (LPSTR)&pTargetCQueueFile);
      if (pTargetCQueueFile->FileType == FTYPE_PSCRIPT)
         {       
         pSP = new SplitDlg;
         pSP->InputFileName = pTargetCQueueFile->LongName;
         pSP->ShortFileName = pTargetCQueueFile->ShortName;         
         if (pSP->DoModal() == IDOK)
            {                    
            if (pSP->m_insertfile)                           
               {
               for (i = 0; i < pSP->TotalFrames; i++)
                  {
				  cs = pSP->SplitFileArray.GetAt(i);
                  // create a new queue file object
                  pCQueueFile = new CQueueFile;
				  pCQueueFile->Init(cs);
                  // copy stuff from original                  
                  if (pTargetCQueueFile->pTransform)
                     {                       
                     pCQueueFile->pTransform = new CTransform;
                     *pCQueueFile->pTransform = *pTargetCQueueFile->pTransform;
                     }                      
                  if (pTargetCQueueFile->pCD)
                     {                       
                     pCQueueFile->pCD = new CDevice;
                     *pCQueueFile->pCD = *pTargetCQueueFile->pCD;
                     }                      
                  pCQueueFile->pCD = pTargetCQueueFile->pCD;
                  pCQueueFile->Prefs = pTargetCQueueFile->Prefs;
                  // add index of new QueueFile to ListBox
                  pLB->InsertString(Select + i + 1, (LPCSTR)pCQueueFile);     
                  // store it
                  PDOC->CQ.InsertAt(Select + i + 1, pCQueueFile);	    
                  // select it
                  //pCQueueFile->Selected = TRUE;                  
				  // not touched     
			      pCQueueFile->WasTouched = FALSE;               
                  pLB->SetSel(Select + i + 1, TRUE);                  
                  }               			   
               }
            if (pSP->TotalFrames > 0)   
               {          
               // if the remove flag is set, take it out of the list
               if (pSP->m_removefile)
                  {
                  CString DeleteFileName;                                     
                  // load the text string as a pointer into a 
                  // pCQueueFile object pointer
                  pLB->GetText(Select, (LPSTR)&pCQueueFile); 
                  // save this in case we want to delete the file                 
                  DeleteFileName = pCQueueFile->LongName;
                  // delete the CQueue object
                  delete pCQueueFile;                        
                  // delete the string            
                  pLB->DeleteString(Select);
		          // delete from the array
                  PDOC->CQ.RemoveAt(Select);	    
                  if (pSP->m_deletefile) ::DeleteFile(DeleteFileName);
                  }                              
	           // renumber the list
	           RenumberList();
               }               
            }         
         delete pSP;
         }         
      }      
   }

void CNewFormView::OnUpdateEditWinsplitfile(CCmdUI* pCmdUI)
   {   
   int Select;
   CQueueFile* pCQueueFile;

   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      {
      pCmdUI->Enable(FALSE);
	  return;
	  }
   
   // only do this if one file is selected
   if (pLB->GetSelCount() == 1)
      {                  
      pLB->GetSelItems(1, &Select);        
      // get the queuefile
      pLB->GetText(Select, (LPSTR)&pCQueueFile);
	  // is it winsplittable?      
	  if (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_WINSPLIT)
         {
         pCmdUI->Enable(TRUE);
         }      
      else
         pCmdUI->Enable(FALSE);
      }
   else
      {  
      pCmdUI->Enable(FALSE);
      }         
   }

void CNewFormView::OnPreferencesQueue() 
   {   	                              
   CString cs;
   BOOL PollingWasHalted;

   // stop polling
   if (QUEUESERVER.m_status != STATUS_SERVER_DISABLED && QUEUESERVER.m_enable_polling)
      {
	  StopServerPolling();
	  PollingWasHalted = TRUE;
	  }
   else
      PollingWasHalted = FALSE;

   // hide the file properties dialog if it
   // is visible
   pCfDlg->ShowWindow(SW_HIDE);
   
   // construct the preferences dialog
   // this will reload the device list
   pPrefsDlg = new CQueuePrefs(_T(""));
							 
   // set the device to use for the visual transform stuff
   QueueFileWasConstructed = SetTransformTarget();

   //load the window
   pPrefsDlg->m_dialog_dev_setup.pWnd = this;
   pPrefsDlg->m_dialog_trans_setup.pWnd = this;

   // set the default device in the list to our global device   
   pPrefsDlg->m_dialog_dev_setup.DelayedSetDevice(QUEUEDEVICE);   
   	
   // set default transform   
   pPrefsDlg->m_dialog_trans_setup.DelayedSetDefaultTransform(QUEUETRANSFORM);
     
   // show OK
   pPrefsDlg->m_dialog_dev_setup.ShowOK(TRUE);         
   pPrefsDlg->m_dialog_trans_setup.ShowOK(TRUE);         

   // don't report that dev has changed   
   pPrefsDlg->m_dialog_dev_setup.ReportChange = PREFS_CHANGE_REPORT_NO;
   pPrefsDlg->m_dialog_trans_setup.ReportChange = PREFS_CHANGE_REPORT_NO;   
   
   // set server 
   pPrefsDlg->m_dialog_server_setup.SetServer(QUEUESERVER);

   // set pWnd
   pPrefsDlg->m_dialog_server_setup.pWnd = this;

   // do dialog
   if (pPrefsDlg->DoModal() == IDOK)
      {
	  // get new QUEUEDEVICE	  
	  QUEUEDEVICE = pPrefsDlg->m_dialog_dev_setup.GetCurrentDevice();

	  // get new global transform	  
	  QUEUETRANSFORM = pPrefsDlg->m_dialog_trans_setup.GetDefaultTransform();

	  // get new queue server object
	  pPrefsDlg->m_dialog_server_setup.GetServer(QUEUESERVER);

	  // if server flag indicates an active server, start it
	  if (QUEUESERVER.m_status == STATUS_SERVER_ACTIVE)
	     StartServer();

      // set server status
      pCQueueChild->SetServerFlag(QUEUESERVER.m_status);

#if 0
	  // reset title by appending current device to title
      cs.Format("%s (%s)", (const char*)((CZeusDoc*)GetDocument())->csBaseTitle,
         (const char*)QUEUEDEVICE.Name);
      ((CZeusDoc*)GetDocument())->SetTitle(cs);
#endif	  
	  ((CZeusDoc*)GetDocument())->SetTitle(QUEUEDEVICE.Name);

	  ((CZeusDoc*)GetDocument())->SetModifiedFlag();
      }
   delete pPrefsDlg;	
   // delete the queue file if we constructed it here	      
   if (QueueFileWasConstructed) 
	  {
	  delete pCQueueFileTarget;
	  QueueFileWasConstructed = FALSE;
	  }

   // start polling again, we do a complete re-init if PollingWasHalted
   if (QUEUESERVER.m_status != STATUS_SERVER_DISABLED && QUEUESERVER.m_enable_polling)
      StartServerPolling(!PollingWasHalted);	      
   }

BOOL CNewFormView::SetTransformTarget()
   {   
   BOOL ret;
   
   // set default construction flag
   ret = FALSE;      

   // try to get a queue file for transform purposes
   pCQueueFileTarget = GetBestQueueFileForTransform();

   // construct a dummy queue file if one is not found
   if (!pCQueueFileTarget)
      {
  	  pCQueueFileTarget = new CQueueFile;          
	  pCQueueFileTarget->Description.LoadString(IDS_NOT_AVAIL);
	  pCQueueFileTarget->pCD = NULL;
	  ret = TRUE;  
	  }   

   // set a target devmode if one does not exist
   if (!pCQueueFileTarget->pCD)                     
	  pPrefsDlg->m_dialog_trans_setup.pTargetDevice = &QUEUEDEVICE;
   else
      pPrefsDlg->m_dialog_trans_setup.pTargetDevice = pCQueueFileTarget->pCD;

   // set the queue file in the trans dialog
   pPrefsDlg->m_dialog_trans_setup.pTargetCQueue = pCQueueFileTarget;

   return ret;   
   }

void CNewFormView::OnDropFiles(HDROP hDropInfo) 
   {
   int i, idx, NumDropped;
   CString csFile;
   CQueueFile *pCQueueFile;
   CFileStatus cfsTest;
   CFile cfTest;	

   // get the number of files dropped
   NumDropped = DragQueryFile(hDropInfo, 0xffffffff, NULL, 0);

   // get row to insert at   	  
   idx = (pLB->GetCount() == 0) ? 0 : (pLB->GetCaretIndex() + 1);      

   // insert the dropped files
   for (i = 0; i < NumDropped; i++)
      {
	  // get file
	  DragQueryFile(hDropInfo, i, csFile.GetBuffer(_MAX_PATH), _MAX_PATH);
	  csFile.ReleaseBuffer();

      // try to insert it into the list		 
      if (pCQueueFile = new CQueueFile)
         {
		 if (pCQueueFile->Init(csFile))
		    {
		    // mark as NOT selected
            //pCQueueFile->Selected = FALSE;       
			// not touched     
			pCQueueFile->WasTouched = FALSE;               
            // add index of new QueueFile to ListBox
            pLB->InsertString(idx, (LPCSTR)pCQueueFile);              
            // store it
            PDOC->CQ.InsertAt(idx, pCQueueFile);	    
		    }
		 }
      else
	     // error
	     delete pCQueueFile;			  
	  // show it
	  UpdateInfo();
	  }
   if (NumDropped > 0)
      {
 	  // reset state if we are currently idle
      if (InternalState == STATE_IDLE_NO_FILES) SetState(STATE_IDLE_SOME_FILES);                     
	  // if nothing is selected, select the first item
      if ((pLB->GetSelCount() == 0)  && (pLB->GetCount() > 0))
         {
         pLB->SetSel(0);                     
         pLB->GetText(0, (LPSTR)&pCQueueFile);
         // get pointer to first item and set selection ON
         //pCQueueFile->Selected = TRUE;
         }         
	  // renumber the list
	  RenumberList();
	  }
   CListView::OnDropFiles(hDropInfo);

   // restart server?
   RestartServer();
   }

void CNewFormView::OnShowWindow(BOOL bShow, UINT nStatus) 
   {
   CListView::OnShowWindow(bShow, nStatus);   
   }

void CNewFormView::RenumberList()
   {
   int i;
   CQueueFile* pCQueueFile;   

   CListCtrl& ctlList = GetListCtrl();

   // renumber the list
   for (i = 0; i < pLB->GetCount(); i++)      
      {
	  pLB->GetText(i, (LPSTR)&pCQueueFile);
	  pCQueueFile->QPos = i;
	  // reset selection state
	  //ctlList.SetItemState(i, pCQueueFile->Selected ? 0x0f : 0, LVIS_SELECTED | LVIS_FOCUSED);	  
	  }
   }

void CNewFormView::SetNoneTouched()
   {
   int i;
   CQueueFile* pCQueueFile;   

   // renumber the list
   for (i = 0; i < pLB->GetCount(); i++)      
      {
	  pLB->GetText(i, (LPSTR)&pCQueueFile);
	  pCQueueFile->WasTouched = FALSE;
	  }
   }

void CNewFormView::ResetPrompts()
   {
   pCQueueChild->ClearStatus();           
   UpdateData(FALSE);
   }	
   
void CNewFormView::OnContinuePrinting()         
   {                
   CQueueFile* pCQueueFile;   
   int RipReturn;
   int i;
   
   // check to see if rip is busy
   if (!RipComplete) 
      {
	  // do timer every second
	  if ((GetTickCount() - LastTimeCheck) > 1)
	     {
		 LastTimeCheck = GetTickCount();
         // get the file elapsed time
         FileElapsedTime = CTime::GetCurrentTime() - RipStartTime;   
         // format the time string
         TimeString = FileElapsedTime.Format("%H:%M:%S / ");   
         // get the queue elapsed time
         ((CZeusDoc*)GetDocument())->QueueElapsedTime = CTime::GetCurrentTime() - 
           ((CZeusDoc*)GetDocument())->QueueStartTime;   
         // add the new time string
         TimeString += ((CZeusDoc*)GetDocument())->QueueElapsedTime.Format("%H:%M:%S");            	  
	     pCQueueChild->SetTimeString(TimeString);
		 }
	  return;
      }

   // nothing in the queue
   pActiveCQueueFile = NULL;   
   
   // if CancelFlag is not set, look for next available file in list
   if (!CancelFlag)
      {      
      for (i = 0; i < pLB->GetCount(); i++)      
         {
	     pLB->GetText(i, (LPSTR)&pCQueueFile);
		 // found one!
		 if (!pCQueueFile->WasTouched)
		    {
			// if selected or if QueueType == QUEUETYPE_ALL, make it active		    
			if (pLB->IsSelected(i) || QueueType == QUEUETYPE_ALL)
			   {
	           pActiveCQueueFile = pCQueueFile;
			   break;
			   }
			}
	     }
	  }
   
   // is an item available?
   if (pActiveCQueueFile)
      {                                        
      // make sure it's visible
      pLB->SetTopIndex(pActiveCQueueFile->QPos);

	  // we had our chance...
	  pActiveCQueueFile->WasTouched = TRUE;

 	  // check for delete status, if deleted, beep and return
	  // note that we put this after wastouched so we don't 
	  // do it again
      //if (pCQueueFile->StatusFlag == FILE_STATUS_OK_DELETED)
	  if ((pCQueueFile->StatusFlag & FILE_STATUS_CHECK) == FILE_STATUS_OK_DELETED)
	     {
		 ::MessageBeep(MB_ICONEXCLAMATION);
         return;
		 }

      // set flag  
      RipComplete = FALSE;
      
      // set status
      //pActiveCQueueFile->StatusFlag = FILE_STATUS_INRIP;      
	  pActiveCQueueFile->StatusFlag |= FILE_STATUS_INRIP;      

	  // redraw it
	  RedrawItem(pActiveCQueueFile->QPos);
      
      // close the rip object
      if (cr.IsInit) 
         {
         cr.Close();         

		 // set queue status
	     pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_IDLE);
	
		 // out of the rip
		 ((CZeusDoc*)GetDocument())->InRip = FALSE;
         // set element pointer
         if (pRipElement) pRipElement->InRip = FALSE;
		 
		 // set screen saver
		 SetScreenSaver(TRUE);

		 // start potentially polling
		 StartServerPolling();
         }

	  // set queue status
	  if ((OutputType == RIPSTYLE_OUTPUT) && 
		  (QueueType == QUEUETYPE_SELECTED))
	     pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_PRINT_SELECT);
	  else if ((OutputType == RIPSTYLE_OUTPUT) && 
		  (QueueType == QUEUETYPE_ALL))
	     pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_PRINT_ALL);
	  else if ((OutputType == RIPSTYLE_PREVIEW) && 
		  (QueueType == QUEUETYPE_SELECTED))
	     pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_PREVIEW_SELECT);
	  else if ((OutputType == RIPSTYLE_PREVIEW) && 
		  (QueueType == QUEUETYPE_ALL))
	     pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_PREVIEW_ALL);
      
      // get start time
      RipStartTime = CTime::GetCurrentTime();      
      
      // set buttons
      SetState(STATE_PRINTING);
      
      // send file to RIP
      if ((RipReturn = SendFileToRIP(pActiveCQueueFile)) != SYN_ERRMSG_ALLOK)
         {                                       
		 // make sure that we have the elapsed time
         FileElapsedTime = CTime::GetCurrentTime() - RipStartTime;   
      
         // process return         
         OnRipComplete(RipReturn, 0L);

		 // problem! Cancel the rest!!
         KillTimer(RP95_PRINT_TIMER_ID);

		 // set queue status
	     pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_IDLE);		 
		 
         // close the rip object
         if (cr.IsInit) 
            {
            cr.Close();                     

		    // set queue status
	        pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_IDLE);

			// out of the rip
		    ((CZeusDoc*)GetDocument())->InRip = FALSE;
            // set element pointer
            if (pRipElement) pRipElement->InRip = FALSE;

			// set screen saver
			SetScreenSaver(TRUE);

		    // start potentially polling
		    StartServerPolling();
            }                     
         // set buttons
         if (pLB->GetCount() > 0)
		    {
			SetState(STATE_IDLE_SOME_FILES);
			}
         else        
		    {                       
            SetState(STATE_IDLE_NO_FILES);   
			}
         }         
      // no elapsed time
      FileElapsedTime = 0;                  
      }   
   else
      {                
      KillTimer(RP95_PRINT_TIMER_ID);

	  // set queue status
	  pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_IDLE);		 

      // close the rip object
      if (cr.IsInit)
         {
         cr.Close();         

 		 // set queue status
	     pCQueueChild->SetQueueStatus(IDS_QUEUESTATUS_IDLE);

		 // out of the rip
		 ((CZeusDoc*)GetDocument())->InRip = FALSE;         
         // set element pointer
         if (pRipElement) pRipElement->InRip = FALSE;

		 // set screen saver
		 SetScreenSaver(TRUE);

		 // start potentially polling
		 StartServerPolling();
         }
      
      // nuke the prompts	  
      ResetPrompts();

	  // set buttons
      if (pLB->GetCount() > 0)
	     {
		 SetState(STATE_IDLE_SOME_FILES);
		 }
      else
	     {
	     SetState(STATE_IDLE_NO_FILES);  
		 }
      }      
   // if the info box is on, update it
   UpdateInfo();               
   }
   
LRESULT CNewFormView::OnRipComplete(WPARAM wParam, LPARAM lParam)
   {     
   // set flag  
   RipComplete = TRUE;             

   ASSERT(pActiveCQueueFile);
   
   // load return status, wParam has image result                
   if (OutputType == RIPSTYLE_PREVIEW)           
      {
      // look up return                                                         
      cr.MapErrorReturn(wParam, pActiveCQueueFile->PreviewStatus);      
      // add time
      pActiveCQueueFile->PreviewStatus += FileElapsedTime.Format(" (%H:%M:%S)");	  
      pCfDlg->m_dialog_general.m_preview_status = pActiveCQueueFile->PreviewStatus;
      }
   else
      {          
      // look up return                                                         
      cr.MapErrorReturn(wParam, pActiveCQueueFile->Status);                      
      // add time                  
      pActiveCQueueFile->Status += FileElapsedTime.Format(" (%H:%M:%S)");
      pCfDlg->m_dialog_general.m_status = pActiveCQueueFile->Status;
      }

   // we know that it is now complete
   pActiveCQueueFile->IsComplete = TRUE;
      
   if (wParam == 0) 
      {
      //pActiveCQueueFile->StatusFlag = FILE_STATUS_OK;
	  if (OutputType == RIPSTYLE_PREVIEW)           
	     pActiveCQueueFile->StatusFlag -= FILE_STATUS_INRIP;
	  else
		 pActiveCQueueFile->StatusFlag = FILE_STATUS_OK;
	  // if server is active and deletefiles flag is set and it was a polled file OR 
	  // a server file, nuke the file
	  if (QUEUESERVER.m_status == STATUS_SERVER_ACTIVE && QUEUESERVER.m_deletefiles &&
		 ((pActiveCQueueFile->ExtraInfo & CQUEUE_EXTRA_POLLEDFILE) || 
		   (pActiveCQueueFile->ExtraInfo & CQUEUE_EXTRA_SERVERFILE)))
	     {
		 ::DeleteFile(pActiveCQueueFile->LongName);	  
		 //pActiveCQueueFile->StatusFlag = FILE_STATUS_OK_DELETED;
		 pActiveCQueueFile->StatusFlag |= FILE_STATUS_OK_DELETED;
		 // add deleted prompt
		 CString cs;
		 cs.LoadString(IDS_DELETED_ADDENDUM);
		 pActiveCQueueFile->Status += cs;
         pCfDlg->m_dialog_general.m_status = pActiveCQueueFile->Status;      
		 }
	  }
   else
      {
	  //pActiveCQueueFile->StatusFlag = FILE_STATUS_NOTOK;   
	  if (OutputType == RIPSTYLE_PREVIEW)           
	     pActiveCQueueFile->StatusFlag -= FILE_STATUS_INRIP;
	  else
	     pActiveCQueueFile->StatusFlag = FILE_STATUS_NOTOK;   
	  // need to force this because if we cancel preview
	  // using the preview pause box, the flag doesn't get 
	  // set here
	  if (wParam == SYN_ERRMSG_CANCELALL) CancelFlag = TRUE;      
	  }

   // our doc is modified
   ((CZeusDoc*)GetDocument())->SetModifiedFlag();

   // redraw item   
   RedrawItem(pActiveCQueueFile->QPos);
   
   // nuke the prompts   
   ResetPrompts();

   // update data in file info box                    
   pCfDlg->m_dialog_general.UpdateData(FALSE);	   

   // output result to PS window
   if (OutputType == RIPSTYLE_PREVIEW)           
      pCQueueChild->PSOut((BYTE*)(const char*)pActiveCQueueFile->PreviewStatus, 
	     pActiveCQueueFile->PreviewStatus.GetLength());
   else
      pCQueueChild->PSOut((BYTE*)(const char*)pActiveCQueueFile->Status, 
	     pActiveCQueueFile->Status.GetLength());
   
   return 1L;
   }

void CNewFormView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
   {   	
   // CReport does this
   Report.Print(pDC, pInfo, pLB, ((CZeusDoc*)GetDocument())->QueueStartTime,
      ((CZeusDoc*)GetDocument())->QueueElapsedTime, QUEUEDEVICE);
   }          

BOOL CNewFormView::OnPreparePrinting(CPrintInfo* pInfo)
   {      
   // CDC* pDC;

   // set the number of pages in the document
   // Report.SetPages(pDC = GetDC(), pLB, pInfo, QUEUEDEVICE);
   // ReleaseDC(pDC);
   
   return DoPreparePrinting(pInfo);
   }                        

void CNewFormView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
   {
   // CReport does this
   Report.PrepareDC(pDC, pInfo);
   }
   
void CNewFormView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
   {   
   // CReport does this
   if (pInfo) Report.BeginPrinting(pDC, pInfo);
   }   
   
void CNewFormView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)   
   {               
   // CReport does this
   Report.EndPrinting(pDC, pInfo);
   }

void CNewFormView::OnToolsReportPrint() 
   {
   // tell MFC to do a print
   CListView::OnFilePrint();   
   }

void CNewFormView::OnToolsReportPrintPreview() 
   {
   // tell MFC to do a print preview
   CListView::OnFilePrintPreview();   
   }

void CNewFormView::OnEditCopyatts() 
   {   
   // only do a copy atts if one file is selected
   if (pLB->GetSelCount() == 1)      
	  CopyToClipboard(OURCLIP_ATTS);
   }

void CNewFormView::OnUpdateEditCopyatts(CCmdUI* pCmdUI) 
   {
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      pCmdUI->Enable(FALSE);
   else
      pCmdUI->Enable((pLB->GetSelCount() == 1) ? TRUE : FALSE);	
   }

void CNewFormView::OnEditPasteatts() 
   {
   int i, NumSelected;
   int *LocalSelBuf;
   CQueueFile* pCQueueFile;
   CQueueFile* pSrcCQueueFile;   
   
   pSrcCQueueFile = NULL;

   if (CLIPARRAY.IsTypeAvail(OURCLIP_ATTS))      
      {      
	  // create a new queue file
	  pSrcCQueueFile = new CQueueFile;	  
	  // only get first item
	  *pSrcCQueueFile = *((CQueueFile*)(CLIPARRAY.GetAt(0)));
	  }
   
   // nothing!
   if (!pSrcCQueueFile) return;

   // alloc buffer to hold selections
   if (pLB->GetCount() > 0)
      LocalSelBuf = new int[pLB->GetCount()];    
   else
      LocalSelBuf = NULL;
   
   // get the items selected   
   NumSelected = pLB->GetSelItems(pLB->GetCount(), LocalSelBuf);

   for (i = 0; i < NumSelected; i++)
      {
	  // get queue file
	  pLB->GetText(LocalSelBuf[i], (LPSTR)&pCQueueFile);	  	  

	  // copy device and transform stuff
	  if (pSrcCQueueFile->pCD && (pSrcCQueueFile->Prefs & USE_LOCAL_DEV_PREFS))  
	     {
		 // alloc new device
		 if (!(pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS))
		    pCQueueFile->pCD = new CDevice;
		 *(pCQueueFile->pCD) = *(pSrcCQueueFile->pCD);
		 }
	  else
	     {
		 // no device, so delete this one
		 if (pCQueueFile->pCD && (pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS))
		    delete pCQueueFile->pCD;
		 }
	  // transform
	  if (pSrcCQueueFile->pTransform && (pSrcCQueueFile->Prefs & USE_LOCAL_TRN_PREFS))
	     {
		 // alloc new transform
		 if (!(pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS))		 
		    pCQueueFile->pTransform = new CTransform;
	     *(pCQueueFile->pTransform) = *(pSrcCQueueFile->pTransform);
		 }
	  else
	     {
		 // no transform, so delete this one
		 if (pCQueueFile->pTransform && (pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS))
		    delete pCQueueFile->pTransform;
		 }

      // NOW copy atts flag
	  pCQueueFile->Prefs = pSrcCQueueFile->Prefs;
      
      // redraw it   
      RedrawItem(LocalSelBuf[i]);         
	  }
   if (LocalSelBuf) delete LocalSelBuf;

   // our doc is modified   
   ((CZeusDoc*)GetDocument())->SetModifiedFlag();                          
   // if the info box is on, update it
   UpdateInfo();			  

   if (pSrcCQueueFile) delete pSrcCQueueFile;
   }   

void CNewFormView::OnUpdateEditPasteatts(CCmdUI* pCmdUI) 
   {
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      {	  
      pCmdUI->Enable(FALSE);
	  return;
	  }
   
   if (CLIPARRAY.IsTypeAvail(OURCLIP_ATTS))
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
   }

void CNewFormView::OnUpdateEditPaste(CCmdUI* pCmdUI)
   {
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      {	  
      pCmdUI->Enable(FALSE);
	  return;
	  }

   if (CLIPARRAY.IsTypeAvail(OURCLIP_FILE))
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
   }

void CNewFormView::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   CQueueFile* pCQueueFile;         

   // get queue file
   pCQueueFile = (CQueueFile*)pDispInfo->item.lParam;

   if (!pCQueueFile)
      {
	  *pResult = 0;
	  return;
	  }	

   switch(pDispInfo->item.iSubItem)
      {
	  case 0:
	       pDispInfo->item.pszText = (LPTSTR)(const char*)pCQueueFile->ShortName;
		   break;
	  case 1:
	       pDispInfo->item.pszText = (LPTSTR)(const char*)pCQueueFile->Description;
		   break;
	  }  
   *pResult = 0;
   }

void CNewFormView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   CMenu menu;                                                             
   CMenu *submenu;      
   POINT point;
   DWORD dwPos;
 
   dwPos = GetMessagePos();
 
   point.x = LOWORD (dwPos);
   point.y = HIWORD (dwPos);
 
   // load the menu                                                        
   menu.LoadMenu(IDR_ZEUSTYPE);
                                                                             
   // get the edit popup menu                                                   
   submenu = menu.GetSubMenu(1);                                           

   // post the menu                                                        
   submenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,                  
                           point.x,                                        
                           point.y,                                        
                           AfxGetApp()->m_pMainWnd, NULL);                  
	
   *pResult = 0;
   }

void CNewFormView::OnTimer(UINT nIDEvent) 
   {
   // CListView::OnTimer nukes our timer! (bug in VC 4.0?)
   // so only go to the base class impl. if not our timer
   if (nIDEvent == RP95_PRINT_TIMER_ID)
      {
	  // call print continue proc
      OnContinuePrinting();    	  
	  }
   else if (nIDEvent == RP95_SERVER_TIMER_ID)
      {
	  // process polling timer tick
	  ProcessServerPolling();
	  }
   else	      
      CListView::OnTimer(nIDEvent);
   }

void CNewFormView::OnUpdatePreferencesQueue(CCmdUI* pCmdUI) 
   {
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      pCmdUI->Enable(FALSE);
   else
      pCmdUI->Enable(CmdQueuePreferences);	
   }

void CNewFormView::RedrawItem(int Index)
   {
   LV_ITEM lvI;   
   CQueueFile* pCQueueFile;   
   
   // get the list 
   CListCtrl& ctlList = GetListCtrl();

   // get item 
   lvI.mask = LVIF_PARAM;
   lvI.iItem = Index;
   lvI.iSubItem = 0;
   ctlList.GetItem(&lvI);

   // get the queue file
   pCQueueFile = (CQueueFile*)lvI.lParam;

   if ((pCQueueFile->StatusFlag & FILE_STATUS_INRIP) == FILE_STATUS_INRIP)
	  lvI.iImage = 1;
   else
	  {
      switch (pCQueueFile->StatusFlag & FILE_STATUS_CHECK)
		 {
	     case FILE_STATUS_HOLD:
	          lvI.iImage = 0;
		      break;
	     case FILE_STATUS_OK:
	          lvI.iImage = 2;
		      break;
	      case FILE_STATUS_NOTOK:
	          lvI.iImage = 3;
		      break;
	     case FILE_STATUS_OK_DELETED:
	          lvI.iImage = 4;
		      break;
	     default:
	          lvI.iImage = 0;
		      break;
		 }
	  }

   // reset it   
   lvI.mask = LVIF_IMAGE;
   ctlList.SetItem(&lvI);
   // update it
   ctlList.Update(Index);
   }             

void CNewFormView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   CQueueFile* pCQueueFile;   

   // if nothing in the list, just return
   // to prevent crashing!
   if (pLB->GetCount() < 1) 
	  {
	  *pResult = 0;
	  return;
	  }

   // if this is a blist, show it
   if (pLB->GetText(pLB->GetCaretIndex(), (LPSTR)&pCQueueFile) != LB_ERR)
      {
      // if this is a batch file, launch the edit dialog
      if (pCQueueFile->FileType == FTYPE_BLIST)
	     EditExistingBatchHeader(pCQueueFile);
	  else
	     ShowInfo();		     
      }                 
   else   
      // launch info window if not already there and
      // put up info   
      ShowInfo();		
	
   *pResult = 0;
   }

void CNewFormView::OnTrackEnd(HWND hwndHeader, int iItem)
{		
    HD_ITEM hdi;

    // Get the column width from the control
    hdi.mask = HDI_WIDTH;
	if((BOOL)::SendMessage(hwndHeader, HDM_GETITEM, (WPARAM)(int)(iItem), (LPARAM)(HD_ITEM FAR*)(&hdi)))
	{
        // Update the width of this column in the column width
        // array
        m_iColWidth[iItem] = hdi.cxy;
    }

    // Force the ListView control to repaint
    InvalidateRect(NULL, TRUE);	
}

BOOL CNewFormView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
   {
   NMHDR* pnmh = (LPNMHDR) lParam; 

   //TRACE("pnmh->code = %d\n", pnmh->code);   
   
   if(pnmh->code == HDN_ITEMCHANGEDA || pnmh->code == HDN_ITEMCHANGEDW)      
      {
      HD_NOTIFY *phdn = (HD_NOTIFY *)pnmh;
	  OnTrackEnd(phdn->hdr.hwndFrom, phdn->iItem);
      return TRUE;
	  }   
   else	
      return CListView::OnNotify(wParam, lParam, pResult);
   }

BOOL CNewFormView::AddColumn(LPCTSTR strItem,int nItem,int nWidth,int nSubItem,int nMask,int nFmt)
   {
   CListCtrl& ctlList = GetListCtrl();

   LV_COLUMN lvc;
   lvc.mask = nMask;
   lvc.fmt = nFmt;
   lvc.pszText = (LPTSTR) strItem;
   lvc.cx = (nWidth == -1) ? ctlList.GetStringWidth(lvc.pszText) + 15: nWidth;
   if(nMask & LVCF_SUBITEM)
      {
      if(nSubItem != -1)
	     lvc.iSubItem = nSubItem;
  	  else
	 	 lvc.iSubItem = nItem;
	  }

   m_iColumns++;
   if(m_iColumns == 1)
      m_iColWidth = (int *)malloc(sizeof(int));
   else
  	  m_iColWidth = (int *)realloc(m_iColWidth,sizeof(int)*m_iColumns);

   m_iColWidth[m_iColumns-1] = lvc.cx;

   return ctlList.InsertColumn(nItem,&lvc);
   }

void CNewFormView::DrawItem(LPDRAWITEMSTRUCT lpDrawItem)
{
    CImageList* himl;
    LV_ITEM lvi;
    int cxImage = 0, cyImage = 0;
    RECT rcClip;    
	UINT uiFlags = ILD_TRANSPARENT;	
	CListCtrl& ctlList = GetListCtrl();
	CPoint pt;

    // Get the item image to be displayed
    lvi.mask = LVIF_IMAGE | LVIF_STATE;
    lvi.iItem = lpDrawItem->itemID;
    lvi.iSubItem = 0;
    ctlList.GetItem(&lvi);

    // Check to see if this item is selected
    if (lpDrawItem->itemState & ODS_SELECTED)
    {
        // Set the text background and foreground colors
        ::SetTextColor(lpDrawItem->hDC,GetSysColor(COLOR_HIGHLIGHTTEXT));
        ::SetBkColor(lpDrawItem->hDC,GetSysColor(COLOR_HIGHLIGHT));

		// Also add the ILD_BLEND50 so the images come out selected
		uiFlags |= ILD_BLEND50;
    }
    else
    {
        // Set the text background and foreground colors to the standard window
        // colors
        ::SetTextColor(lpDrawItem->hDC,GetSysColor(COLOR_WINDOWTEXT));
        ::SetBkColor(lpDrawItem->hDC,GetSysColor(COLOR_WINDOW));
    }


    // Get the image list and draw the image
    himl = ctlList.GetImageList(LVSIL_SMALL);
    if (himl)
    {
		pt.x = lpDrawItem->rcItem.left; pt.y = lpDrawItem->rcItem.top;
		himl->Draw(CDC::FromHandle(lpDrawItem->hDC),lvi.iImage,
                   pt, uiFlags);

        // Find out how big the image we just drew was
        ImageList_GetIconSize(himl->GetSafeHandle(), &cxImage, &cyImage);
    }
    // Calculate the width of the first column after the image width.  If
    // There was no image, then cxImage will be zero.
	
	// get the pointer to the queuefile
    lvi.mask = LVIF_PARAM;
    lvi.iItem = lpDrawItem->itemID;
	lvi.iSubItem = 0;
    ctlList.GetItem(&lvi);

    // get the queue file
    CQueueFile* pCQueueFile = (CQueueFile*)lvi.lParam;
	
    // Set up the new clipping rect for the first column text and draw it
    rcClip.left = lpDrawItem->rcItem.left + cxImage;
    rcClip.right = lpDrawItem->rcItem.left + m_iColWidth[0];
    rcClip.top = lpDrawItem->rcItem.top;
    rcClip.bottom = lpDrawItem->rcItem.bottom;
	
	// short name of file
	pLB->DrawItemColumn(lpDrawItem->hDC, (LPTSTR)(const char*)pCQueueFile->ShortName,
                   &rcClip);

	// description
	rcClip.left = rcClip.right;
	rcClip.right = rcClip.left + m_iColWidth[1];
	pLB->DrawItemColumn(lpDrawItem->hDC, 
       (LPTSTR)(const char*)pCQueueFile->Description, &rcClip);

	int RightClip = rcClip.right;

	// local props		   
	pt.x = rcClip.right; pt.y = lpDrawItem->rcItem.top;
	if(pCQueueFile->Prefs & USE_LOCAL_DEV_PREFS)
	   himl->Draw(CDC::FromHandle(lpDrawItem->hDC), 5, pt, uiFlags);
	// Find out how big the image we just drew was
    ImageList_GetIconSize(himl->GetSafeHandle(), &cxImage, &cyImage);
	rcClip.right += cxImage;	
	
	pt.x = rcClip.right; pt.y = lpDrawItem->rcItem.top;
	if ((pCQueueFile->Prefs & USE_LOCAL_TRN_PREFS))
	   himl->Draw(CDC::FromHandle(lpDrawItem->hDC), 6, pt, uiFlags);
	// Find out how big the image we just drew was
    ImageList_GetIconSize(himl->GetSafeHandle(), &cxImage, &cyImage);
	rcClip.right += cxImage;
	   
	pt.x = rcClip.right; pt.y = lpDrawItem->rcItem.top;
	if (pCQueueFile->Prefs & USE_LOCAL_COL_PREFS)
	   himl->Draw(CDC::FromHandle(lpDrawItem->hDC), 7, pt, uiFlags);
	// Find out how big the image we just drew was
    ImageList_GetIconSize(himl->GetSafeHandle(), &cxImage, &cyImage);	

#if 0
    // If we changed the colors for the selected item, undo it
    if (lpDrawItem->itemState & ODS_SELECTED)
       {
       // Set the text background and foreground colors
       ::SetTextColor(lpDrawItem->hDC,GetSysColor(COLOR_WINDOWTEXT));
       ::SetBkColor(lpDrawItem->hDC,GetSysColor(COLOR_WINDOW));
	   // set selection state
	   //pCQueueFile->Selected = TRUE;  
       }
	//else
	    //pCQueueFile->Selected = FALSE;  	
#endif
    // If the item is focused, now draw a focus rect around the entire row
    if (lpDrawItem->itemState & ODS_FOCUS)
       {
       // Adjust the left edge to exclude the image
       rcClip = lpDrawItem->rcItem;
       rcClip.left += cxImage;
	   rcClip.right = RightClip;

       // Draw the focus rect
       DrawFocusRect(lpDrawItem->hDC, &rcClip);
       }  
    return;
}

#define CX_SMICON	16
#define CY_SMICON	16
void CNewFormView::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItem)
{
    TEXTMETRIC tm;
	CClientDC dc(this);
	CFont* pOldFont;
	// Set the font of the DC to the same font the control is using
	pOldFont = dc.SelectObject(GetFont());

    // Get the height of the font used by the control
    if (!dc.GetTextMetrics(&tm))
        return;

    // Add a little extra space between items
    lpMeasureItem->itemHeight = tm.tmHeight + 1;

    // Make sure there is enough room for the images which are CY_SMICON high
    if (lpMeasureItem->itemHeight < (CY_SMICON + 1))
    	lpMeasureItem->itemHeight = CY_SMICON + 1;

	dc.SelectObject(pOldFont);
}

void CNewFormView::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;   

   if ((pNMListView->iItem != -1) && (pNMListView->iSubItem == 0))
      {
	  // if a new file has been selected, update the info
	  if((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED))
	     UpdateInfo();	
	  }
   *pResult = 0;
   }

CRect CNewFormView::GetDragSourceRect()
   {   
   // return the drag/drop source rect (maintained in MainFrm class)    
   return ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_DragSourceRect;	
   }

void CNewFormView::SetDragSourceRect()
   {                                                            
   CRect ListWindowRect;
    
   GetWindowRect(&ListWindowRect);
    
   // Set drag/drop source rectangle (screen coord)
   //  (maintained in the MainFrm class)    
   ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_DragSourceRect = ListWindowRect;
   }

/////////////////////////////////////////////////////////////////////////////
// COleListBoxDropTarget - Enable OLE dragdrop for the list control

COleListBoxDropTarget::COleListBoxDropTarget() {}

COleListBoxDropTarget::~COleListBoxDropTarget() {}

#define DRAGSCROLLDELAY 1000
//
// OnDragEnter is called by OLE dll's when drag cursor enters
// a window that is REGISTERed with the OLE dll's
//
DROPEFFECT COleListBoxDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* 
           pDataObject, DWORD dwKeyState, CPoint point )
   {   
   dwScrollStart = 0;
                 
   // Check if the Control key was pressed
   if((dwKeyState&MK_CONTROL) == MK_CONTROL)
      return DROPEFFECT_COPY; // copy source text
   else
      return DROPEFFECT_MOVE; // move source text   
   }

//
// OnDragLeave is called by OLE dll's when drag cursor leaves
// a window that is REGISTERed with the OLE dll's
//
void COleListBoxDropTarget::OnDragLeave(CWnd* pWnd)
   {
   // Call base class implementation
   COleDropTarget:: OnDragLeave(pWnd);
   }

// 
// OnDragOver is called by OLE dll's when cursor is dragged over 
// a window that is REGISTERed with the OLE dll's
//
DROPEFFECT COleListBoxDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* 
           pDataObject, DWORD dwKeyState, CPoint point )
   {                            
   UINT flag;
   CListCtrl& ctlList = ((CNewFormView*)pWnd)->GetListCtrl();
   // if hovering over top index, scroll up
   if (ctlList.HitTest(point, &flag) == ctlList.GetTopIndex())
      {
	  if (dwScrollStart == 0)
	     {
	     dwScrollStart = GetTickCount();
		 }
	  else if ((GetTickCount() - dwScrollStart) > DRAGSCROLLDELAY)
	     {
		 CSize cs(0, -10);
	     ctlList.Scroll(cs);		 
		 }	  	  
	  }   
   // if hovering over bottom index, scroll down
   else if (ctlList.HitTest(point, &flag) >= 
      (ctlList.GetTopIndex() + ctlList.GetCountPerPage() - 1))
      {
	  if (dwScrollStart == 0)
	     {
	     dwScrollStart = GetTickCount();
		 }
	  else if ((GetTickCount() - dwScrollStart) > DRAGSCROLLDELAY)
	     {
		 CSize cs(0, +10);
	     ctlList.Scroll(cs);		 
		 }	  	  
	  }   
   else
      dwScrollStart = 0;

   if((dwKeyState&MK_CONTROL) == MK_CONTROL)
      return DROPEFFECT_COPY;
   else
      return DROPEFFECT_MOVE;    
   }

// 
// OnDrop is called by OLE dll's when item is dropped in a window
// that is REGISTERed with the OLE dll's
//
BOOL COleListBoxDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, 
                 DROPEFFECT dropEffect, CPoint point )
   {           
   HGLOBAL  hGlobal;
   LPCSTR   pData;
    
   // Get text data from COleDataObject   
   hGlobal=pDataObject->GetGlobalData(((CNewFormView*)pWnd)->uiOurListClipFormat);   

   // if hGlobal is NULL, return FALSE and let our
   // non-OLE code do the work   
   if (!hGlobal) return FALSE;

   // Get pointer to data
   pData=(LPCSTR)GlobalLock(hGlobal);       

   ASSERT(pData!=NULL); 

   ((CNewFormView*)pWnd)->InsertDroppedQueueFiles(point);	

   // Unlock memory
   GlobalUnlock(hGlobal);

   return TRUE;
   }  

void CNewFormView::InsertDroppedQueueFiles(CPoint& point)
   {
   int i, idx;            
   CQueueFile* pCQueueFile;   
   int ItemHalfHeight;
    
   // get the list control
   CListCtrl& ctlList = GetListCtrl();

   // get item height / 2
   TEXTMETRIC tm;
   CClientDC dc(this);
   CFont* pOldFont;
   // Set the font of the DC to the same font the control is using
   pOldFont = dc.SelectObject(GetFont());
   // Get the height of the font used by the control
   if (!dc.GetTextMetrics(&tm))
      return;
   ItemHalfHeight = tm.tmHeight / 2;
   dc.SelectObject(pOldFont);

   // first see if we're alone
   if (ctlList.GetItemCount() == 0)            
	  idx = 0;   	     
   else
      {
	  // nope, keep going
	  POINT ItemPos;
	  int MidPoint;
	  //default
	  idx = ctlList.GetItemCount();
      // loop through items looking for nearest member
      // we want to insert AFTER the item which is nearest
	  // get position of first item	  
      for (i = ctlList.GetTopIndex(); i < ctlList.GetItemCount(); i++)
         {
	     ctlList.GetItemPosition(i, &ItemPos);
	     // get midpoint
	     MidPoint = ItemPos.y + ItemHalfHeight;
	     if (point.y < MidPoint)
	        {
		    idx = i;			
		    break;
		    }
		 }
	  }

   // available?
   if (CLIPARRAY.IsTypeAvail(OURCLIP_FILE))
      {
      // get all elements and insert them
      for (i = 0; i < CLIPARRAY.GetSize(); i++)
         {	                  
         // create a new queue file
         pCQueueFile = new CQueueFile;
         // copy the data
         *pCQueueFile = *((CQueueFile*)(CLIPARRAY.GetAt(i)));
         // add index of new QueueFile to ListBox
         pLB->InsertString(idx, (LPCSTR)pCQueueFile);	      
         // store it
         PDOC->CQ.InsertAt(idx, pCQueueFile);	    
         }	   
      // reset state if we are currently idle
      if (InternalState == STATE_IDLE_NO_FILES) SetState(STATE_IDLE_SOME_FILES);   
      // our doc is modified
      PDOC->SetModifiedFlag();                          
      // if the info box is on, update it
	  UpdateInfo();			  	      
	  }	
   // renumber the list
   RenumberList();  

   // reset selections      
   LV_ITEM lvI;   
   for (i = 0; i < pLB->GetCount(); i++)      
      {
	  pLB->GetText(i, (LPSTR)&pCQueueFile);	  
	  // reset selection state
	  ctlList.SetItemState(i, pCQueueFile->Selected ? 0x0f : 0, LVIS_SELECTED | LVIS_FOCUSED);	  

      lvI.mask = LVIF_PARAM;
      lvI.iItem = i;
      lvI.iSubItem = 0;
      ctlList.GetItem(&lvI);

      if ((pCQueueFile->StatusFlag & FILE_STATUS_INRIP) == FILE_STATUS_INRIP)
	     lvI.iImage = 1;
      else
		 {
         switch (pCQueueFile->StatusFlag & FILE_STATUS_CHECK)
			{
	        case FILE_STATUS_HOLD:
	             lvI.iImage = 0;
		         break;
	        case FILE_STATUS_OK:
	             lvI.iImage = 2;
		         break;
	         case FILE_STATUS_NOTOK:
	             lvI.iImage = 3;
		         break;
	        case FILE_STATUS_OK_DELETED:
	             lvI.iImage = 4;
		         break;
	        default:
	             lvI.iImage = 0;
		         break;
			}
		 }
      // reset it   
      lvI.mask = LVIF_IMAGE;
      ctlList.SetItem(&lvI);   
	  }

   // restart server?
   RestartServer();
   }   

void CNewFormView::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;	    
   int i, cnt, numChar;
   CQueueFile* pCQueueFile;

//   TRACE("Begin Drag\n");

   CListCtrl& ctlList = GetListCtrl();

   if (ctlList.GetSelectedCount() <= 0) 
      {
	  *pResult = 0;
	  return;
	  }
   
   // mark for potential deletion
   for (i = 0; i < pLB->GetCount(); i++)
      {                         
      // get the queue item
      pLB->GetText(i, (LPSTR)&pCQueueFile);
	  //if (pCQueueFile->Selected)
	  if (pLB->IsSelected(i))
	     {
		 // set selected flag for when we drop
		 pCQueueFile->Selected = TRUE;
		 // don't allow drag and drop on a file that's ripping
		 //if (pCQueueFile->StatusFlag == FILE_STATUS_INRIP) return;
		 if ((pCQueueFile->StatusFlag & FILE_STATUS_INRIP) == FILE_STATUS_INRIP) return;
	     pCQueueFile->DragDelete = TRUE;
		 }
	  else
	     {
		 // set selected flag for when we drop
		 pCQueueFile->Selected = FALSE;
	     pCQueueFile->DragDelete = FALSE;
		 }
	  }  
       
   CopyToClipboard(OURCLIP_FILE);
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
   m_COleDataSource.CacheGlobalData(uiOurListClipFormat, hgData);              
    
   // Set drag/drop source rectangle (screen coord)
   SetDragSourceRect();
    
   DROPEFFECT dropEffect =
      m_COleDataSource.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE,NULL);       
   
   // ifa move, we must delete the files  
   if((dropEffect&DROPEFFECT_MOVE)==DROPEFFECT_MOVE)     
      {
	  cnt = pLB->GetCount();
	  // do it in reverse order so count stays the same
      for (i = cnt - 1; i >= 0; i--)
         {                         
         // get the queue item
         pLB->GetText(i, (LPSTR)&pCQueueFile);
	     if (pCQueueFile->DragDelete)
	        {
			// don't delete files that are imaging!
	        //if (pCQueueFile->StatusFlag == FILE_STATUS_INRIP) continue;              
			if ((pCQueueFile->StatusFlag & FILE_STATUS_INRIP) == FILE_STATUS_INRIP) continue;              
	        // delete the CQueue object
            delete pCQueueFile;                        
            // delete the string            
            pLB->DeleteString(i);
			// delete from the array
            PDOC->CQ.RemoveAt(i);	    
		    }
		 }
   	  // renumber the list
	  RenumberList();
      }
   // Clear the Data Source's cache
   m_COleDataSource.Empty();        
   *pResult = 0;
   }

void CNewFormView::RestartServer()
   {
   // only restart if an active server doc
   if (QUEUESERVER.m_status != STATUS_SERVER_ACTIVE) return;
   
   // if so, see if there are files an start rip
   if (InternalState == STATE_IDLE_SOME_FILES)
      {
	  // set output type
      OutputType = RIPSTYLE_OUTPUT;
	  
      // rip ALL
      QueueType = QUEUETYPE_ALL;
   
      // do it
      Print();				
	  }    
   }

void CNewFormView::StartServer()
   {                       
   // set server status
   pCQueueChild->SetServerFlag(QUEUESERVER.m_status);

   // set output type
   OutputType = RIPSTYLE_OUTPUT;
	  
   // rip ALL
   QueueType = QUEUETYPE_ALL;
   
   // do it
   Print();				
   }

void CNewFormView::OnResetStatus()   
   {
   int i;      
   CQueueFile* pCQueueFile;
   int *LocalSelBuf;         
   int NumSelected;

   if (pLB->GetCount() > 0)
      LocalSelBuf = new int[pLB->GetCount()];    
   else
      LocalSelBuf = NULL;
      
   // get the items selected
   // can be no more than count items
   if ((NumSelected = pLB->GetSelItems(pLB->GetCount(), LocalSelBuf)) > 0)	
     {  
     // reset these items
     for (i = NumSelected - 1; i >= 0; i--)
        {                        
        // get a pointer to the queue file
        pLB->GetText(LocalSelBuf[i], (LPSTR)&pCQueueFile);
        // load the default strings
        pCQueueFile->Status.LoadString(IDS_NOT_AVAIL);
        pCQueueFile->PreviewStatus.LoadString(IDS_NOT_AVAIL);
        //pCQueueFile->StatusFlag = FILE_STATUS_HOLD;      
		pCQueueFile->StatusFlag |= FILE_STATUS_HOLD;      
		pCQueueFile->WasTouched = FALSE;
		// redraw the item
		RedrawItem(LocalSelBuf[i]);
        }
     // update the file info box     
     UpdateInfo();     
     // our doc is modified
	 ((CZeusDoc*)GetDocument())->SetModifiedFlag();
     }
  // delete the local buffer   
  if (LocalSelBuf) delete LocalSelBuf;     
  }   

void CNewFormView::OnUpdateResetStatus(CCmdUI* pCmdUI)
   {   	
   if (GetFocus() == (CWnd*)PSOUT_EDIT_WINDOW) 
      pCmdUI->Enable(FALSE);
   else
      pCmdUI->Enable(pLB->GetSelCount() > 0 ? TRUE : FALSE);		
   }

void CNewFormView::StartServerPolling(BOOL FirstTime)
   {
   // do we have an active or enabled queue with polling enabled?
   // or is polling already enabled?
   if (QUEUESERVER.m_status == STATUS_SERVER_DISABLED || 
      !QUEUESERVER.m_enable_polling || ServerPolling)
      return;

   // start server timer   
   if (!SetTimer(RP95_SERVER_TIMER_ID, RP95_SERVER_TIMER_TIMEOUT, NULL))
      AfxMessageBox(IDS_SYS_TIMER_ERROR, MB_OK | MB_ICONEXCLAMATION);      
   else
      {
	  if (FirstTime) QUEUESERVER.InitPolling();
      ServerPolling = TRUE;
	  }
   }

void CNewFormView::StopServerPolling()
   {
   if (ServerPolling)
      {
	  ServerPolling = FALSE;
      KillTimer(RP95_SERVER_TIMER_ID);
	  }
   }

void CNewFormView::ProcessServerPolling()
   {
   CStringArray csa;
   CString csFileName;
   int idx, i, NumFound;
   CQueueFile* pCQueueFile;

   // get poll files	  
   NumFound = QUEUESERVER.GetPollFiles(csa);

   // get row to insert at   	  
   //idx = (pLB->GetCount() == 0) ? 0 : (pLB->GetCaretIndex() + 1);
   // insert at end
   idx = pLB->GetCount();

   // add to queue
   for (i = 0; i < NumFound; i++)
      {
      // try to insert it into the list		 
      if (pCQueueFile = new CQueueFile)
         {
		 csFileName = csa.GetAt(i);
		 if (pCQueueFile->Init(csFileName))
		    {
		    // mark as NOT selected
            //pCQueueFile->Selected = FALSE;       
			// not touched     
			pCQueueFile->WasTouched = FALSE;               
			// set this flag so we can intelligently erase
			pCQueueFile->ExtraInfo += CQUEUE_EXTRA_POLLEDFILE;
            // add index of new QueueFile to ListBox
            pLB->InsertString(idx, (LPCSTR)pCQueueFile);              
            // store it
            PDOC->CQ.InsertAt(idx, pCQueueFile);	    
		    }
		 }
      else
	     // error
	     delete pCQueueFile;			  
	  }
   if (NumFound > 0)
      {
      // reset state if we are currently idle
      if (InternalState == STATE_IDLE_NO_FILES) SetState(STATE_IDLE_SOME_FILES);               
      // our doc is modified
      PDOC->SetModifiedFlag();                          
      // if the info box is on, update it
	  UpdateInfo();		
	  // renumber the list
      RenumberList();  
      // restart server
      RestartServer();
	  }
   }  

void CNewFormView::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

   *pResult = 0;

   // must have document
   if (!pLB) return;

   // must have at least 2 files
   if (pLB->GetCount() < 2) return;

   // must not be ripping
   if (((CZeusDoc*)GetDocument())->InRip) return;

   AfxGetApp()->DoWaitCursor(1);

   // sort different columns
   switch(pNMListView->iSubItem)
	  {
	  case 0:
		   SortName();
		   break;
	  case 1:
		   SortDescription();
	       break;
	  case 2:
		   SortPrefs();
	       break;
	  }
   
   AfxGetApp()->DoWaitCursor(-1);
   }

void CNewFormView::SortName()
   {
   BOOL bNotDone = TRUE;
 
   while (bNotDone)
      {
      bNotDone = FALSE;
      for(int pos = 0;pos < pLB->GetCount() - 1;pos++)
         bNotDone |= CompareAndSwapName(pos);
      }
   QueueSortOrderUp[0] = !(QueueSortOrderUp[0]);
   }

void CNewFormView::SortDescription()
   {
   BOOL bNotDone = TRUE;
 
   while (bNotDone)
      {
      bNotDone = FALSE;
      for(int pos = 0;pos < pLB->GetCount() - 1;pos++)
         bNotDone |= CompareAndSwapDescription(pos);
      }
   QueueSortOrderUp[1] = !(QueueSortOrderUp[1]);
   }
 
void CNewFormView::SortPrefs()
   {
   BOOL bNotDone = TRUE;
 
   while (bNotDone)
      {
      bNotDone = FALSE;
      for(int pos = 0;pos < pLB->GetCount() - 1;pos++)
         bNotDone |= CompareAndSwapPrefs(pos);
      }
   QueueSortOrderUp[2] = !(QueueSortOrderUp[2]);
   }
 
BOOL CNewFormView::CompareAndSwapName(int pos)
   {   
   int posFirst = pos;
   int posNext = pos + 1;
   CQueueFile* pCQueueFileFirst;
   CQueueFile* pCQueueFileNext;
   int ret;

   // get the queue files
   pLB->GetText(posFirst, (LPSTR)&pCQueueFileFirst);  
   pLB->GetText(posNext, (LPSTR)&pCQueueFileNext);  

   // compare  
   ret = pCQueueFileFirst->ShortName.CompareNoCase(pCQueueFileNext->ShortName);
   if (QueueSortOrderUp[0]) ret = -ret;

   if (ret > 0)
      {
	  pLB->InsertString(posFirst, (LPCSTR)pCQueueFileNext);
      // store it
      PDOC->CQ.InsertAt(posFirst, pCQueueFileNext);	    
	  pLB->DeleteString(posNext + 1);      
	  // delete from the array
      PDOC->CQ.RemoveAt(posNext + 1);	    
      return TRUE;
      }
   return FALSE;
   }
 
BOOL CNewFormView::CompareAndSwapDescription(int pos)
   {   
   int posFirst = pos;
   int posNext = pos + 1;
   CQueueFile* pCQueueFileFirst;
   CQueueFile* pCQueueFileNext;
   int ret;

   // get the queue files
   pLB->GetText(posFirst, (LPSTR)&pCQueueFileFirst);  
   pLB->GetText(posNext, (LPSTR)&pCQueueFileNext);  

   // compare  
   ret = pCQueueFileFirst->Description.CompareNoCase(pCQueueFileNext->Description);
   if (QueueSortOrderUp[1]) ret = -ret;

   if (ret > 0)
      {
	  pLB->InsertString(posFirst, (LPCSTR)pCQueueFileNext);
      // store it
      PDOC->CQ.InsertAt(posFirst, pCQueueFileNext);	    
	  pLB->DeleteString(posNext + 1);      
	  // delete from the array
      PDOC->CQ.RemoveAt(posNext + 1);	    
      return TRUE;
      }
   return FALSE;
   }

BOOL CNewFormView::CompareAndSwapPrefs(int pos)
   {   
   int posFirst = pos;
   int posNext = pos + 1;
   CQueueFile* pCQueueFileFirst;
   CQueueFile* pCQueueFileNext;
   int ret;

   // get the queue files
   pLB->GetText(posFirst, (LPSTR)&pCQueueFileFirst);  
   pLB->GetText(posNext, (LPSTR)&pCQueueFileNext);  

   // compare  
   if (pCQueueFileFirst->Prefs > pCQueueFileNext->Prefs)
	  ret = 1;
   else if (pCQueueFileFirst->Prefs < pCQueueFileNext->Prefs)
	  ret = -1;
   else 
	  ret = 0;
   
   if (QueueSortOrderUp[2]) ret = -ret;

   if (ret > 0)
      {
	  pLB->InsertString(posFirst, (LPCSTR)pCQueueFileNext);
      // store it
      PDOC->CQ.InsertAt(posFirst, pCQueueFileNext);	    
	  pLB->DeleteString(posNext + 1);      
	  // delete from the array
      PDOC->CQ.RemoveAt(posNext + 1);	    
      return TRUE;
      }
   return FALSE;
   }

void CNewFormView::SetScreenSaver(BOOL bRestore)
   {
   if (bRestore)
	  {
      // restore screen saver 
      SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, bScreenSaverState, NULL, SPIF_SENDCHANGE);
	  }
   else
	  {
      // get screen saver state
      bScreenSaverState = 
	     SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, NULL, SPIF_SENDCHANGE);
      // turn screen saver off
      SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, SPIF_SENDCHANGE);
	  }
   }