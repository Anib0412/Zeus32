////////////////////////////////////////////////////////////////////////////
// CustomFileDialog.cpp
//                                                                        
// Custom File Open dialog for RasterPlus
// Copyright (C) 1999-2000 Graphx, Inc.
//
// mods:
//
// 12-14-99 Version 3.0.2 - moved rip cancel to specialtree
// 2-16-00  Version 3.0.3 - weirdly dated files caused CFileStatus to ASSERT - 
//                          changed to FindFirstFile
// 2-29-00  Version 3.0.3 - if user disabled extensions preview stopped working - fixed

#include "stdafx.h"
#include "zeus.h"
#include "CustomFileDialog.h"
#include <dlgs.h> // for (MULTI)FILEOPENORD
#include "prev.h"
#include "profile.h"
#include "inidefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LPOFNHOOKPROC g_lpfnOldHook;

const UINT NEAR wm_PrevSetInfo = RegisterWindowMessage( PREV_SETINFO );
const UINT NEAR wm_RipSendMsg = RegisterWindowMessage( RIP_SENDMESSAGE );
const UINT NEAR wm_RipComplete = RegisterWindowMessage( RIP_COMPLETE );

/////////////////////////////////////////////////////////////////////////////
// CCustomFileDialog

IMPLEMENT_DYNAMIC(CCustomFileDialog, CFileDialog)

BEGIN_MESSAGE_MAP(CCustomFileDialog, CFileDialog)
	//{{AFX_MSG_MAP(CCustomFileDialog)	
	ON_WM_HELPINFO()
	ON_WM_CONTEXTMENU()	
	ON_BN_CLICKED(IDC_ADDFOLDER, OnAddfolder)
	ON_REGISTERED_MESSAGE(wm_PrevSetInfo, OnPrevSetInfo)			
	ON_REGISTERED_MESSAGE(wm_RipSendMsg, OnRipMessage)
	ON_REGISTERED_MESSAGE(wm_RipComplete, OnRipComplete)		
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SHOWPREVIEW, OnShowpreview)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

CCustomFileDialog::CCustomFileDialog( BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, 
									 DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
   {
   //{{AFX_DATA_INIT(CCustomFileDialog)
   m_show_preview = FALSE;
   //}}AFX_DATA_INIT	
   
   // this version ONLY supports OFN_EXPLORER dialogs
   ASSERT(dwFlags & OFN_EXPLORER);

   // Most of the "customization" of CCustomFileDialog is set by the dwFlags
   // passed in to the constructor. Attempts to enable these features after
   // constructing the CCustomFileDialog, such as accessing the m_ofn structure
   // directly, may cause CCustomFileDialog to not work correctly

   m_szBigBuffer[0]='\0';
   m_ofn.lpstrFile = m_szBigBuffer;

   if (dwFlags & OFN_ALLOWMULTISELECT)
	  {
	  m_bMulti = TRUE;
	  // MFC only provides a 260 character buffer for lpstrFile
	  // This is not sufficient when you may be expecting a large number of files.
	  m_ofn.nMaxFile  = sizeof(m_szBigBuffer);
	  if (lpszFileName != NULL)
	     lstrcpyn(m_szBigBuffer, lpszFileName, sizeof(m_szBigBuffer));
	  }
   else
      m_ofn.nMaxFile = _MAX_PATH;

   if (dwFlags & OFN_EXPLORER)
	  {			
	  }
   else
      if (m_ofn.Flags & OFN_EXPLORER)
		 {
		 // MFC added it, but we don't want it
		 m_ofn.Flags &= ~(OFN_EXPLORER | OFN_SHOWHELP);
		 }	

   if (dwFlags & OFN_ENABLETEMPLATE)
	  {
	  m_bTemplate = TRUE;
	  // FILEOPENORD & MULTIFILEOPENORD can be found from the Infoviewer
	  // at Samples -> MFC Samples -> General MFC Samples -> CLIPART -> COMMDLG.RC
	  // These are the customized versions
	  
	  // pjt - note that we are sending a NULL for the 3.x template as
	  // we will always be using the explorer (ie: 4) version
	  // we do an assert above to make sure
	  SetTemplate(NULL, IDD_CUSTOM_FILE_DIALOG);      
	  }
   m_pStatic = NULL;
   csLastFile = "";
   csNewFile = "";
   }

void CCustomFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	
	// We can't do DDX/DDV if we're not using our own custom template!
	if (m_bTemplate)
	{
	//{{AFX_DATA_MAP(CCustomFileDialog)
	DDX_Check(pDX, IDC_SHOWPREVIEW, m_show_preview);
	//}}AFX_DATA_MAP
	}
}

BOOL CCustomFileDialog::OnFileNameOK()
   {
   // CFileDialog's m_ofn.lpstrFile will contain last set of selections!
   return FALSE;
   }

BOOL CCustomFileDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
	{
		// Will only get WM_HELP from Open File Dialog for our controls
		AfxGetApp()->WinHelp(pHelpInfo->iCtrlId+0x50000,HELP_CONTEXTPOPUP);
	}

	return TRUE; 
}

void CCustomFileDialog::OnContextMenu(CWnd* pWnd, CPoint point) 
{

	const DWORD helpIDs[] = 
	{
		0,0 
	};

	::WinHelp(pWnd->m_hWnd, AfxGetApp()->m_pszHelpFilePath,
		HELP_CONTEXTMENU, (DWORD)(LPVOID)helpIDs);
}

void CCustomFileDialog::OnHelp()
{
	// TODO: How do I bring up the main topic as a contextpopup?
	AfxGetApp()->WinHelp(1, HELP_CONTEXTPOPUP);
}

BOOL CCustomFileDialog::OnInitDialog() 
   {
   CProfile Profile;

   CFileDialog::OnInitDialog();	

   GetDlgItem(IDC_ADDFOLDER)->EnableWindow(FALSE);   

   DoPaint = FALSE;
   pCQueueFile = NULL;

   RECT Rect;					   
   GetDlgItem(IDC_PREVIEW_PANE)->GetWindowRect(&Rect);

    GetParent()->ScreenToClient(&Rect);

    m_pStatic = new CStatic();
    m_pStatic->Create(NULL,SS_BLACKFRAME|WS_VISIBLE|WS_EX_NOPARENTNOTIFY,
		      Rect,this,10101); 

	// get preview state
	m_show_preview = Profile.GetInt(IDS_PREVIEW, 
	   IDS_FILEOPENPREVIEW, IDDM_FILEOPENPREVIEW);   

	UpdateData(FALSE);
	
	return TRUE;  
	}

// In MFC 4.1, the GetNextPathName function incorrectly checked
// the version of Windows in order to determine which delimiter 
// separated filenames in the m_ofn.lpstrFile structure. Actually,
// it is whether the CFileDialog is old-style or Explorer that
// determines the delimiter.
// MFC 4.2 fixed this problem, and the code there is similar to
// this:
CString CCustomFileDialog::FixedGetNextPathName(POSITION& pos) const
{
#ifndef _MAC
	BOOL bExplorer = m_ofn.Flags & OFN_EXPLORER;
	TCHAR chDelimiter;
	if (bExplorer)
		chDelimiter = '\0';
	else
		chDelimiter = ' ';

	LPTSTR lpsz = (LPTSTR)pos;
	if (lpsz == m_ofn.lpstrFile) // first time
	{
		if ((m_ofn.Flags & OFN_ALLOWMULTISELECT) == 0)
		{
			pos = NULL;
			return m_ofn.lpstrFile;
		}

		// find char pos after first Delimiter
		while(*lpsz != chDelimiter && *lpsz != '\0')
			lpsz = _tcsinc(lpsz);
		lpsz = _tcsinc(lpsz);

		// if single selection then return only selection
		if ((lpsz - m_ofn.lpstrFile) > m_ofn.nFileOffset)
		{
			pos = NULL;
			return m_ofn.lpstrFile;
		}
	}

	CString strPath = m_ofn.lpstrFile;
	if (!bExplorer)
	{
		LPTSTR lpszPath = m_ofn.lpstrFile;
		while(*lpszPath != chDelimiter)
			lpszPath = _tcsinc(lpszPath);
		strPath = strPath.Left(lpszPath - m_ofn.lpstrFile);
	}

	LPTSTR lpszFileName = lpsz;
	CString strFileName = lpsz;

	// find char pos at next Delimiter
	while(*lpsz != chDelimiter && *lpsz != '\0')
		lpsz = _tcsinc(lpsz);

	if (!bExplorer && *lpsz == '\0')
		pos = NULL;
	else
	{
		if (!bExplorer)
			strFileName = strFileName.Left(lpsz - lpszFileName);

		lpsz = _tcsinc(lpsz);
		if (*lpsz == '\0') // if double terminated then done
			pos = NULL;
		else
			pos = (POSITION)lpsz;
	}

	// only add '\\' if it is needed
	if (!strPath.IsEmpty())
	{
		// check for last back-slash or forward slash (handles DBCS)
		LPCTSTR lpsz = _tcsrchr(strPath, '\\');
		if (lpsz == NULL)
			lpsz = _tcsrchr(strPath, '/');
		// if it is also the last character, then we don't need an extra
		if (lpsz != NULL && 
			(lpsz - (LPCTSTR)strPath) == strPath.GetLength()-1)
		{
			ASSERT(*lpsz == '\\' || *lpsz == '/');
			return strPath + strFileName;
		}
	}
	return strPath + '\\' + strFileName;
#else
	pos = NULL;
	return m_ofn.lpstrFile;
#endif
}

int CCustomFileDialog::DoModal()
   {    
   g_lpfnOldHook = m_ofn.lpfnHook;
    
   //No need to set OFN_ENABLEHOOK, MFC sets for us..
   m_ofn.lpfnHook = (LPOFNHOOKPROC)CCustomFileDialog::FileDlgHookProc;    

   int nRetVal =  CFileDialog::DoModal();
   return nRetVal;
   }


BOOL CALLBACK CCustomFileDialog::FileDlgHookProc(HWND hDlg, UINT uMsg,
						 WPARAM wParam, LPARAM lParam)
   {
   // We have to hook into the paint, or the static
   // that we inserted won't bet painted correctly.
   CCustomFileDialog * pFileDlg;

   //If we get a paint, update our thumbnail image.
   if (uMsg == WM_PAINT)
	  {
	  pFileDlg = StaticGetPointer(hDlg);
	  pFileDlg->PaintIt();
	  }    
    
   //Call old hook
   return g_lpfnOldHook(hDlg,uMsg,wParam,lParam);
   }

CCustomFileDialog * CCustomFileDialog::StaticGetPointer(HWND hDlg)
   {
   CDialog* pDlg = (CDialog*)CWnd::FromHandlePermanent(hDlg);
   if (pDlg == NULL && (::GetWindowLong(hDlg, GWL_STYLE) & WS_CHILD))
   pDlg = (CDialog*)CWnd::FromHandlePermanent(::GetParent(hDlg));
   ASSERT(pDlg != NULL);
   ASSERT(pDlg->IsKindOf(RUNTIME_CLASS(CCustomFileDialog)));
    
   CCustomFileDialog * pFileDlg = (CCustomFileDialog *)pDlg;
   return pFileDlg;
   }

void CCustomFileDialog::OnFileNameChange()
   {
   TCHAR szFile[MAX_PATH];  
   CString cs;
   CString strItemText;
   BOOL bAddedExtension = FALSE;

   // special hack so that when a user clicks a folder, the 
   // edit box is updated...this is not normal behavior
   // for an open file dialog
   CWnd* pWnd = GetParent()->GetDlgItem(lst2);
   if (pWnd != NULL)
	  {		
	  CListCtrl* wndLst1 = (CListCtrl*)(pWnd->GetDlgItem(1));	   
	  if (wndLst1->GetSelectedCount() == 1)
		 {		 
		 cs = wndLst1->GetItemText(wndLst1->GetNextItem(-1,LVNI_SELECTED),0);		 
		 CommDlg_OpenSave_SetControlText(::GetParent(m_hWnd), edt1, (const char*)cs);

		 // now check to see if it is a folder
         CString strDirectory = GetFolderPath();		 
         if (strDirectory.Right(1) == _T("\\"))
            strItemText = strDirectory + cs;
		 else
		 	strItemText = strDirectory + _T("\\") + cs;
		 
		 // get the status
 	     HANDLE hFile;
		 WIN32_FIND_DATA FileInfo;		 
         if ((hFile = FindFirstFile(strItemText, &FileInfo)) != INVALID_HANDLE_VALUE)
			{      
		    // close the handle
		    FindClose(hFile);   
			}				 
		 else
			{
			// user may have extensions turned off, try adding it
		    // get the (undocumented) contents of the data buffer
		    DWORD dwExtra;		 
		    dwExtra = wndLst1->GetItemData(wndLst1->GetNextItem(-1,LVNI_SELECTED));		 
		    // find the string
		    cs = (const char*)dwExtra + 14;
			strItemText = strDirectory + _T("\\") + cs;			
			if ((hFile = FindFirstFile(strItemText, &FileInfo)) != INVALID_HANDLE_VALUE)
			   {      			
			   bAddedExtension = TRUE;
			   // close the handle
			   FindClose(hFile);   
			   }				 
			}

		 // copy into the m_ofn.lpstrFile buffer
		 memset(m_ofn.lpstrFile, 0, MAX_PATH);
		 strcpy(m_ofn.lpstrFile, strItemText);

		 // enable the button		 
		 if (FileInfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		    GetDlgItem(IDC_ADDFOLDER)->EnableWindow(TRUE);   
		 else
			GetDlgItem(IDC_ADDFOLDER)->EnableWindow(FALSE);   
		 } 
	  }
    
   if (bAddedExtension)
	  {
	  SetCurrentFile((char*)(const char*)strItemText);
	  OnNewFile((char*)(const char*)strItemText);	
	  }
   else
	  {
	  // Get the path of the selected file.
      if (CommDlg_OpenSave_GetFilePath(::GetParent(m_hWnd),
         szFile, sizeof(szFile)) <= sizeof(szFile))
		 {
	     SetCurrentFile(szFile);
	     OnNewFile(szFile);	
		 }						    
	  }
   }

void CCustomFileDialog::OnFolderChange()
   {    
   CommDlg_OpenSave_SetControlText(::GetParent(m_hWnd), edt1, _T(""));
   GetDlgItem(IDC_ADDFOLDER)->EnableWindow(FALSE);   

   SetCurrentFile(_T(""));
   OnNewFile(_T(""));	
   }

void CCustomFileDialog::OnNewFile(LPTSTR lpszFileName)
{
    ASSERT(lpszFileName != NULL);
    ASSERT_VALID(this);    

	// a new file to rip
	csNewFile = lpszFileName; 

	// if we are ripping, just return
	if (cr.IsInit) return;	   

	// store last file name
	csLastFile = lpszFileName;

	// if a real file, go for it	
	HANDLE hFile;
	WIN32_FIND_DATA FileInfo;
	BOOL bExists = FALSE;
	if ((hFile = FindFirstFile(lpszFileName, &FileInfo)) != INVALID_HANDLE_VALUE)
	   {      
	   bExists = TRUE;
	   // close the handle
	   FindClose(hFile);   
	   }				 	
	if (bExists && !cr.IsInit && m_show_preview)
	   {
	   if (FileInfo.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
	      OnRepaint(csLastFile);						
	   }
}

void CCustomFileDialog::OnAddfolder() 
   {    
   ((CDialog*)GetParent())->EndDialog(IDOK);
   }

void CCustomFileDialog::OnRepaint(CString& csFileName)
   {
   int ret;
   CRect crect;       
   CString csTemp;
   int iTotalPages = 0;

   // get our preview size
   GetDlgItem(IDC_PREVIEW_PANE)->GetClientRect(&crect);

   if (pCQueueFile) delete pCQueueFile;

   // make a new pseudo queue file
   if ((pCQueueFile = new CQueueFile) == NULL)
	  return;
   pCQueueFile->Init(csFileName, iTotalPages, 0);   

   // open rip object
   cr.Open();
   
   // init rip   
   ret = cr.Init(pDefaultDevice, pDefaultTransform,  pCC,
      pCQueueFile, RIPSTYLE_PREVIEW, this, this, TRUE);	  
   
   // override our preview params and start rip   
   if (ret == SYN_ERRMSG_ALLOK)                          
      {                               
	  // nuke the prompts
	  //csTemp.LoadString(IDS_QUEUESTATUS_IDLE);
      //SetDlgItemText(IDC_RIP_STATUS_PRMPT, csTemp);
      ((CProgressCtrl*)GetDlgItem(IDC_PREVIEW_PROGRESS))->SetPos(0);		         	   
	  
	  // make a properly sized rectangle based on driver
	  //m_pStatic->SetWindowPos(NULL, 0, 0, cr.Width() + 2, InitHeight, 
//		    SWP_NOMOVE | SWP_NOZORDER);

	  // get some numbers
	  RIPSTRUCT RipStruct;
	  pDefaultDevice->FillRipStruct(&RipStruct);

	  // special mods
	  RipStruct.PixWidth = crect.Width(); 
	  RipStruct.PixHeight = crect.Height(); 
      RipStruct.WPixelsPerInch = 24;
      RipStruct.HPixelsPerInch = 24;

	  fDevInchWidth = (float)RipStruct.PixWidth / (float)RipStruct.WPixelsPerInch;	  
      
	  // send new image size but scale up for accuracy
	  int iWidth = crect.Width();	  
 	  cr.RipParamsOverride(crect.Width(), 
		 crect.Height(), (float)iWidth / fDevInchWidth, (float)iWidth / fDevInchWidth,
	     m_hWnd, pDefaultTransform);

      // start RIP  	  
      ret = cr.StartRip();   
      }                   
   else
      cr.Close();
   }

LRESULT CCustomFileDialog::OnRipMessage(WPARAM WParam, LPARAM LParam)
   {
   PRIPMSGSTRUCT pMsgStruct;   
   //char LocalMsgBuffer[RIPMSG_MAX_MSG_LEN];   
   //CString cs;

   // cast the LParam to a ripmsg pointer
   pMsgStruct = (PRIPMSGSTRUCT)LParam;
   
   // do message stuff based on msg type
   switch(pMsgStruct->MsgType)
      {
#if 0
	  case RIPMSG_SCANLINE:   
		   // show scanline status
		   ((CProgressCtrl*)GetDlgItem(IDC_PREVIEW_PROGRESS))->SetPos(pMsgStruct->LineOn * 100 
		      / pMsgStruct->LineTotal);
           break;      
#endif
      case RIPMSG_READING:        
		   // show reading status
		   ((CProgressCtrl*)GetDlgItem(IDC_PREVIEW_PROGRESS))->SetPos(pMsgStruct->PercentComplete);		      
           break;
      }         

   return 0L;   
   }     

LRESULT CCustomFileDialog::OnRipComplete(WPARAM wParam, LPARAM lParam)
   {
   // rezero the status
   ((CProgressCtrl*)GetDlgItem(IDC_PREVIEW_PROGRESS))->SetPos(0);		                 

   // delete the rip
   if (cr.IsInit) cr.Close();   

   if (csNewFile != csLastFile)
	  {
  	  CString csTemp = csNewFile;
	  OnNewFile((char*)(const char*)csTemp);	
	  }     
   return 0L;
   }

LRESULT CCustomFileDialog::OnPrevSetInfo(WPARAM wParam, LPARAM lParam)
   {
   PPREVIEWINFO pPreviewInfo;

   // get the pointer  
   pPreviewInfo = (PPREVIEWINFO)lParam;     

   // create a new display image
   if (cd.CreateImage(pPreviewInfo))
      {      
      DoPaint = TRUE;
      }
   // force a repaint   
   InvalidateRect(NULL);

   // force a cancel after first image
   return SYN_ERRMSG_CANCELIMAGE;
   }

void CCustomFileDialog::PaintIt()
   {
   if (DoPaint)
	  {
	  CRect rectPaint;
	  CClientDC dc(m_pStatic);	  
      m_pStatic->GetClientRect(&rectPaint);    
      cd.DisplayImage(&rectPaint, dc);         
//	  TRACE("PaintIt\n");
	  }
   }

void CCustomFileDialog::OnDestroy() 
   {
   CProfile Profile;

   // save preview state
   UpdateData(TRUE);
   Profile.WriteInt(IDS_PREVIEW, 
      IDS_FILEOPENPREVIEW, m_show_preview);   
#if 0
   if (cr.IsInit) 
	  {	  
	  cr.CancelProcess(TRUE);	  
	  cr.Close();
	  }
#endif
   CFileDialog::OnDestroy();
#if 0	
   if (pCQueueFile) delete pCQueueFile;
#endif
   if (m_pStatic != NULL)
	  {
	  m_pStatic->DestroyWindow();
	  delete m_pStatic;
	  m_pStatic = NULL;
	  }    		
   }

void CCustomFileDialog::OnShowpreview() 
   {
   UpdateData(TRUE);
   
   if (m_show_preview) 	  
	  {
	  CString csTemp = csLastFile;
	  OnNewFile((char*)(const char*)csTemp);	
	  }
   }
