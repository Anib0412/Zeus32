/////////////////////////////////////////////////////////////////////////////
// FILEINFO.CPP
//                                                                        
// local file info for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#include "stdafx.h"
#include "zeus.h"   
#include "fileinfo.h"  
#include "prefsmsg.h"
#include "control.hm"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// the control help-map
static DWORD dwHelpMapNotes[] = 
   {
   IDC_NOTES, HIDC_NOTES_NOTES,
   IDC_NOTES_PROMPT, HIDC_NOTES_NOTES,
   IDC_CLOSE, HIDC_CLOSE,   
   0,0
   };

static DWORD dwHelpMapGeneral[] = 
   {
   IDC_FILENAME, HIDC_GENERAL_FILENAME,
   IDC_FILENAME_PRMPT, HIDC_GENERAL_FILENAME,
   IDC_FILESIZE, HIDC_GENERAL_FILESIZE,
   IDC_FILESIZE_PRMPT, HIDC_GENERAL_FILESIZE,
   IDC_FILEDATE, HIDC_GENERAL_FILEDATE,
   IDC_FILEDATE_PRMPT, HIDC_GENERAL_FILEDATE,
   IDC_FILETIME, HIDC_GENERAL_FILETIME,
   IDC_FILETIME_PRMPT, HIDC_GENERAL_FILETIME,
   IDC_DESCRIPTION, HIDC_GENERAL_DESCRIPTION,
   IDC_DESCRIPTION_PRMPT, HIDC_GENERAL_DESCRIPTION,
   IDC_FILETYPE, HIDC_GENERAL_FILETYPE,
   IDC_FILETYPE_PRMPT, HIDC_GENERAL_FILETYPE,
   IDC_PAGES, HIDC_GENERAL_PAGES,
   IDC_PAGES_PRMPT, HIDC_GENERAL_PAGES,
   IDC_BOUNDINGBOX, HIDC_GENERAL_BOUNDINGBOX,
   IDC_BOUNDINGBOX_PRMPT, HIDC_GENERAL_BOUNDINGBOX,
   IDC_STATUS, HIDC_GENERAL_STATUS,
   IDC_STATUS_PRMPT, HIDC_GENERAL_STATUS,
   IDC_PREVIEWSTATUS, HIDC_GENERAL_PREVIEWSTATUS,
   IDC_PREVIEWSTATUS_PRMPT, HIDC_GENERAL_PREVIEWSTATUS,
   IDC_CLOSE, HIDC_CLOSE,   
   0,0
   };

const UINT NEAR wm_DevModeChange = RegisterWindowMessage( ID_DEVMODE_CHANGE );

/////////////////////////////////////////////////////////////////////////////
// CFileInfo dialog

CFileInfo::CFileInfo(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:COurPropertySheet(nIDCaption, pParentWnd, iSelectPage)
   {
   InitVars();	
   }

CFileInfo::CFileInfo(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:COurPropertySheet(pszCaption, pParentWnd, iSelectPage)
   {
   InitVars();			
   }

void CFileInfo::OnClose()
{     
   // hide the window
   ShowWindow(SW_HIDE);            
}                   

void CFileInfo::InitVars()
   {   
   AddPage(&m_dialog_general);
   AddPage(&m_dialog_dev_setup);
   AddPage(&m_dialog_trans_setup);
   AddPage(&m_dialog_notes);
	
   // load an icon for minimizing
   hIcon = AfxGetApp()->LoadIcon(IDI_ICON_FILEINFO);   
   }

BOOL CFileInfo::OnInitDialog() 
   {   
   COurPropertySheet::OnInitDialog();  

   // make all pages active so as to force OnInits to 
   // be called 
   SetActivePage(&m_dialog_dev_setup);   
   SetActivePage(&m_dialog_trans_setup);
   SetActivePage(&m_dialog_notes);
   SetActivePage(&m_dialog_general);   

   MoveDialog();
   return TRUE;  	             
   }

void CFileInfo::MoveDialog()
{
   CPoint   Point;
   CRect    DialogRect;
   CRect    ParentRect;
   CRect    StatusRect;
   int      nWidth;
   int      nHeight;
   CWnd     *DesktopWindow = NULL;
   CWnd     *MainWindow;
   CWnd     *StatusWindow;   

   // Get the size of the dialog box.
   GetWindowRect(DialogRect);
   // actually, must use the parent because this is
   // a property page   
   //GetParent()->GetWindowRect(DialogRect);   

   // Get the main window.
   MainWindow = AfxGetApp()->m_pMainWnd;
   
   // Determine if the main window exists. This can be useful when
   // the application creates the dialog box before it creates the
   // main window. If it does exist, retrieve its size to center
   // the dialog box with respect to the main window.
   if (MainWindow != NULL)
      MainWindow->GetClientRect(ParentRect);
   // If the main window does not exist, orient with respect to
   // the desktop window.
   else
   {
      DesktopWindow = GetDesktopWindow();
      DesktopWindow->GetWindowRect(ParentRect);
   }
      
   // Calculate the height and width for MoveWindow().
   nWidth = DialogRect.Width();
   nHeight = DialogRect.Height();   

   // Find the lower right point
   Point.x = ParentRect.Width();
   Point.y = ParentRect.Height();
   
   // Calculate the new X, Y starting point.
   Point.x -= nWidth;
   Point.y -= nHeight;
   
   // subtract height of status bar if it exists
   StatusWindow = AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);   
   if (StatusWindow != NULL)
      {
      StatusWindow->GetClientRect(StatusRect);         
      Point.y -= StatusRect.Height();            
      }   

   // Move the window.
   MoveWindow(Point.x, Point.y, nWidth, nHeight, FALSE);
   }
            
BEGIN_MESSAGE_MAP(CFileInfo, CPropertySheet)
	//{{AFX_MSG_MAP(CFileInfo)	
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CFileInfo::OnPaint()
{	
	if (IsIconic()) 
	   {
	   CPaintDC dc(this); // device context for painting	
	   dc.DrawIcon( 0, 0, hIcon );	                        
	   }
	else                 
	   CPropertySheet::OnPaint();	   
}

HCURSOR CFileInfo::OnQueryDragIcon()
{
   return hIcon;
}   

/////////////////////////////////////////////////////////////////////////////
// CPrefDlgGeneral dialog

CPrefDlgGeneral::CPrefDlgGeneral() : CPropertyPage(CPrefDlgGeneral::IDD)
{
	//{{AFX_DATA_INIT(CPrefDlgGeneral)
	m_filedate = _T("");
	m_filename = _T("");
	m_filesize = _T("");
	m_filetime = _T("");
	m_description = _T("");
	m_boundingbox = _T("");
	m_queuename = _T("");
	m_preview_status = _T("");
	m_status = _T("");
	m_filetype = _T("");
	m_pages = _T("");
	//}}AFX_DATA_INIT
	m_shortfilename = _T("");
}

void CPrefDlgGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrefDlgGeneral)
	DDX_Text(pDX, IDC_FILEDATE, m_filedate);
	DDX_Text(pDX, IDC_FILENAME, m_filename);	
	DDX_Text(pDX, IDC_FILESIZE, m_filesize);
	DDX_Text(pDX, IDC_FILETIME, m_filetime);
	DDX_Text(pDX, IDC_DESCRIPTION, m_description);
	DDX_Text(pDX, IDC_BOUNDINGBOX, m_boundingbox);			
	DDX_Text(pDX, IDC_PREVIEWSTATUS, m_preview_status);
	DDX_Text(pDX, IDC_STATUS, m_status);	
	DDX_Text(pDX, IDC_FILETYPE, m_filetype);
	DDX_Text(pDX, IDC_PAGES, m_pages);
	//}}AFX_DATA_MAP
	
	// set the title
    CString cs, csFormat;   	    
	csFormat.LoadString(IDS_PROPERTIES_FORMAT);
	cs.Format((const char*)csFormat, (const char*)m_shortfilename,
	   (const char*)m_queuename);

   	GetParent()->SetWindowText(cs);      	  		
}                                 

BEGIN_MESSAGE_MAP(CPrefDlgGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CPrefDlgGeneral)	
	ON_BN_CLICKED(IDC_CLOSE, OnClose)				
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefDlgGeneral message handlers

BOOL CPrefDlgGeneral::OnInitDialog()
{
	CPropertyPage::OnInitDialog();	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPrefDlgGeneral::OnClose()
   {
   // hide it
   GetParent()->ShowWindow(SW_HIDE);
   }

void CPrefDlgGeneral::OnContextMenu(CWnd* pWnd, CPoint point) 
   {   
   // call context help
   ::WinHelp(pWnd->m_hWnd, AfxGetApp()->m_pszHelpFilePath, 
      HELP_CONTEXTMENU,  (DWORD)(LPVOID)dwHelpMapGeneral);
   }  

BOOL CPrefDlgGeneral::OnHelpInfo(HELPINFO* pHelpInfo) 
   {
   if (pHelpInfo->iContextType == HELPINFO_WINDOW)   // must be for a control
      {
      ::WinHelp ((HWND)pHelpInfo->hItemHandle,
         AfxGetApp()->m_pszHelpFilePath, HELP_WM_HELP, 
		 (DWORD)(LPVOID)dwHelpMapGeneral);
	  return TRUE;
	  }      
   else
   	  return CDialog::OnHelpInfo(pHelpInfo);
   }

/////////////////////////////////////////////////////////////////////////////
// CPrefDlgNotes message handlers

CPrefDlgNotes::CPrefDlgNotes() : CPropertyPage(CPrefDlgNotes::IDD)
{       
	//{{AFX_DATA_INIT(CPrefDlgNotes)
	m_notes = "";
	//}}AFX_DATA_INIT
	
}

void CPrefDlgNotes::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrefDlgNotes)
	DDX_Text(pDX, IDC_NOTES, m_notes);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPrefDlgNotes, CPropertyPage)
	//{{AFX_MSG_MAP(CPrefDlgNotes)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)			
	ON_EN_CHANGE(IDC_NOTES, OnChangeNotes)
	ON_WM_CONTEXTMENU()	
	ON_WM_HELPINFO()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CPrefDlgNotes::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPrefDlgNotes::OnClose()
   {
   // hide it
   GetParent()->ShowWindow(SW_HIDE);
   }

void CPrefDlgNotes::OnChangeNotes()
   {
   // get note
   UpdateData(TRUE);
   // tell the parent
   pWnd->SendMessage(wm_DevModeChange, PREFS_CHANGE_NOTES | ReportChange, (LPARAM)&m_notes);
   }

void CPrefDlgNotes::EnableNotes()
   {
   GetDlgItem(IDC_NOTES)->EnableWindow(TRUE);
   }
   
void CPrefDlgNotes::DisableNotes()
   {
   GetDlgItem(IDC_NOTES)->EnableWindow(FALSE);
   }  

void CPrefDlgNotes::OnContextMenu(CWnd* pWnd, CPoint point) 
   {   
   // call context help
   ::WinHelp(pWnd->m_hWnd, AfxGetApp()->m_pszHelpFilePath, 
      HELP_CONTEXTMENU,  (DWORD)(LPVOID)dwHelpMapNotes);
   }  

BOOL CPrefDlgNotes::OnHelpInfo(HELPINFO* pHelpInfo) 
   {
   if (pHelpInfo->iContextType == HELPINFO_WINDOW)   // must be for a control
      {
      ::WinHelp ((HWND)pHelpInfo->hItemHandle,
         AfxGetApp()->m_pszHelpFilePath, HELP_WM_HELP, 
		 (DWORD)(LPVOID)dwHelpMapNotes);
	  return TRUE;
	  }      
   else
   	  return CDialog::OnHelpInfo(pHelpInfo);
   }

/*
Hey, My Screen Is Flashing!
If you happen to use a single template for all of the pages in your property sheet, 
the user may encounter annoying screen flashes when switching between pages. Your 
application can minimize or eliminate these flashes by responding to the WM_SHOWWINDOW 
message. The following code snippet demonstrates one method of eliminating the screen flash:
  
    case WM_SHOWWINDOW:
        // Check if the window is being shown via a ShowWindow.
        if (wParam && !LOWORD(lParam))
            // It is, so post a message to myself.
            PostMessage(hDlg, WM_APP, 0, 0L);
        break;

    case WM_APP:
        // Remove the rectangle for the page from the invalid list.
        ValidateRect(hDlg, NULL);
        // Invalidate any and all controls within the page.
        InvalidateRect(GetDlgItem(hDlg, ID_CONTROL1), NULL, FALSE);
        InvalidateRect(GetDlgItem(hDlg, ID_CONTROL2), NULL, FALSE);
        .
        .
        .
        InvalidateRect(GetDlgItem(hDlg, ID_CONTROLn), NULL, FALSE);
        break;
  
An application that uses this method repaints only the controls that need repainting inside the page, instead of repainting the whole window when the WM_SHOWWINDOW message is sent. A page will also need to call InvalidateRect with the fErase parameter set to TRUE for controls that do not completely paint their client area during a WM_PAINT message (for example, for a list box that is not full).
*/