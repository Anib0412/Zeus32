/////////////////////////////////////////////////////////////////////////////
// QueueMemory.cpp
//                                                                        
// Resource manager for RasterPlus
// Copyright (C) 1999-2000 Graphx, Inc.
//
// mods:
//
// 11-30-99 Version 3.0.2 - spin button on job entries was broken - fixed
// 05-09-00 Version 3.0.3 - Added button to Reset Hot Folder
// 10-24-02 Version 5.1.0 - Remove limitation on maximum memory - Move limitation to individual drivers
//

#include "stdafx.h"
#include "zeus.h"
#include "QueueMemory.h"
#include "rexdirdi.h"
#include "control.hm"
#include "SysMem.h"
#include "Prefsmsg.h"
#include "control.hm"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MINDISKFILES   0
#define MAXDISKFILES   10000

// the control help-map
static DWORD dwHelpMap[] = 
   {
   IDC_SPOOL_EDIT, HIDC_SPOOL,
   IDC_SPOOL_BROWSE, HIDC_SPOOL,
   IDC_SPOOL_EDIT_BOX, HIDC_SPOOL,
   IDC_SCRATCH_EDIT, HIDC_SCRATCH,
   IDC_SCRATCH_BROWSE, HIDC_SCRATCH,
   IDC_SCRATCH_EDIT_BOX, HIDC_SCRATCH,
   
   IDC_ARCHIVE_BOX, HIDC_ARCHIVE_BOX,
   IDC_LISTJOBS_PRMPT, HIDC_LISTJOBS, 
   IDC_LISTJOBS_COMBO, HIDC_LISTJOBS, 
   IDC_LISTJOBS_EDIT, HIDC_LISTJOBS, 
   IDC_LISTJOBS_SPIN, HIDC_LISTJOBS,
   
   IDC_DISKFILES_PRMPT, HIDC_DISKFILES, 
   IDC_DISKFILES_COMBO, HIDC_DISKFILES, 
   IDC_HOTFOLDER_PRMPT, HIDC_HOTFOLDER, 
   IDC_HOTFOLDER_COMBO, HIDC_HOTFOLDER, 

   IDC_MEMORY_BOX, HIDC_MEMORY_BOX,
   IDC_HOTFOLDER_BOX, HIDC_HOTFOLDER_BOX,

   IDC_IMAGEONLY_NEW, HIDC_SERVER_IMAGEONLY_NEW,      
   IDC_HOTFOLDER, HIDC_HOTFOLDER_ENABLE,
   IDC_HOTFOLDERDIR_EDIT, HIDC_HOTFOLDERDIR,
   IDC_HOTFOLDERDIR_BROWSE, HIDC_HOTFOLDERDIR,
   IDC_RESET_HOT_FOLDER, HIDC_RESETHOTFOLDER,
   IDC_HOTFOLDER_INPANE, HIDC_HOTFOLDER_INPANE,
   IDC_HOTFOLDER_INPANE_PRMPT, HIDC_HOTFOLDER_INPANE,

   IDC_IMAGEMEMORY, HIDC_ADVANCED_IMAGEMEMORY,
   IDC_IMAGEMEMORY_PRMPT, HIDC_ADVANCED_IMAGEMEMORY,
   IDC_AVAIL_PHYSICAL, HIDC_ADVANCED_PHYSICAL,
   IDC_AVAIL_PHYSICAL_PRMPT, HIDC_ADVANCED_PHYSICAL,
   IDC_VIRTUALMEMORY, HIDC_ADVANCED_VIRTUALMEMORY,
   IDC_VIRTUALMEMORY_PRMPT, HIDC_ADVANCED_VIRTUALMEMORY,
   IDC_MEM_ALLOC, HIDC_ADVANCED_MEM_ALLOC,
   IDC_MEM_ALLOC_PRMPT, HIDC_ADVANCED_MEM_ALLOC,
   IDC_MEM_USED_EDIT, HIDC_ADVANCED_MEM_USED,
   IDC_MEM_USED_SPIN, HIDC_ADVANCED_MEM_USED,
   IDC_MEM_USED_PRMPT, HIDC_ADVANCED_MEM_USED,
   IDC_DEFAULT, HIDC_ADVANCED_DEFAULT,
   IDOK, HIDC_OUROK,
   IDCANCEL, HIDC_OURCANCEL,   
   0,0
   };

const UINT NEAR wm_DevModeChangeDefault = RegisterWindowMessage( ID_DEVMODE_CHANGE_DEFAULT );
/////////////////////////////////////////////////////////////////////////////
// CQueueMemory property page

IMPLEMENT_DYNCREATE(CQueueMemory, CPropertyPage)

CQueueMemory::CQueueMemory() : CPropertyPage(CQueueMemory::IDD)
{
	//{{AFX_DATA_INIT(CQueueMemory)
	m_spool = _T("");
	m_virtual_memory = _T("");
	m_scratch = _T("");
	m_avail_physical = _T("");
	m_memused_edit = _T("");
	m_memalloc_style = -1;
	m_diskfiles = -1;
	m_listjobs = -1;
	m_diskfiles_max = 0;
	m_listjobs_max = 0;
	m_enable_hotfolder = FALSE;
	m_hotfolder_dir = _T("");
	m_hotfolder = -1;
	m_hotfolder_max = 0;
	m_hotfolder_inpane = -1;
	m_hotfolder_delay = 0;
	m_image_only_new = FALSE;
	m_bSelect_By_Size = FALSE;
	m_hotfolder_dpi = 300;
	m_hotfolder_variance = 0.25f;
	//}}AFX_DATA_INIT
	pTargetDevice = NULL;
}

CQueueMemory::~CQueueMemory()
{
}

void CQueueMemory::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQueueMemory)
	DDX_Text(pDX, IDC_SPOOL_EDIT, m_spool);
	DDX_Text(pDX, IDC_VIRTUALMEMORY, m_virtual_memory);
	DDX_Text(pDX, IDC_SCRATCH_EDIT, m_scratch);
	DDX_Text(pDX, IDC_AVAIL_PHYSICAL, m_avail_physical);
	DDX_Text(pDX, IDC_MEM_USED_EDIT, m_memused_edit);
	DDX_CBIndex(pDX, IDC_MEM_ALLOC, m_memalloc_style);
	DDX_CBIndex(pDX, IDC_DISKFILES_COMBO, m_diskfiles);
	DDX_CBIndex(pDX, IDC_LISTJOBS_COMBO, m_listjobs);	
	DDX_Text(pDX, IDC_LISTJOBS_EDIT, m_listjobs_max);
	DDX_Check(pDX, IDC_HOTFOLDER, m_enable_hotfolder);
	DDX_Text(pDX, IDC_HOTFOLDERDIR_EDIT, m_hotfolder_dir);
	DDX_CBIndex(pDX, IDC_HOTFOLDER_COMBO, m_hotfolder);	
	DDX_CBIndex(pDX, IDC_HOTFOLDER_INPANE, m_hotfolder_inpane);
	DDX_Check(pDX, IDC_IMAGEONLY_NEW, m_image_only_new);
	DDX_Text(pDX, IDC_HOTFOLDER_DELAY, m_hotfolder_delay);
	DDV_MinMaxInt(pDX, m_hotfolder_delay, 0, 10);	
	DDX_Check(pDX, IDC_HF_SELECTBYSIZE, m_bSelect_By_Size);
	DDX_Text(pDX, IDC_HF_SELECTDPI, m_hotfolder_dpi);
	DDX_Text(pDX, IDC_HF_VARIANCE, m_hotfolder_variance);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQueueMemory, CPropertyPage)
	//{{AFX_MSG_MAP(CQueueMemory)
	ON_BN_CLICKED(IDC_SPOOL_BROWSE, OnSpoolBrowse)
	ON_CBN_SELCHANGE(IDC_MEM_ALLOC, OnSelchangeMemAlloc)
	ON_BN_CLICKED(IDC_SCRATCH_BROWSE, OnScratchBrowse)
	ON_CBN_SELCHANGE(IDC_LISTJOBS_COMBO, OnSelchangeListjobsCombo)
	ON_CBN_SELCHANGE(IDC_DISKFILES_COMBO, OnSelchangeDiskfilesCombo)
	ON_BN_CLICKED(IDC_HOTFOLDER, OnHotfolder)
	ON_BN_CLICKED(IDC_HF_SELECTBYSIZE, OnSelectBySize)
	ON_BN_CLICKED(IDC_HOTFOLDERDIR_BROWSE, OnHotfolderdirBrowse)
	ON_BN_CLICKED(IDC_RESET_HOT_FOLDER, OnResetHotFolder )
	ON_NOTIFY(UDN_DELTAPOS, IDC_MEM_USED_SPIN, OnDeltaposMemUsedSpin)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_MEM_USED_EDIT, OnChangeMemUsedEdit)
	ON_EN_CHANGE(IDC_HF_SELECTDPI, OnChangeSelectDPI)
	ON_EN_CHANGE(IDC_HF_VARIANCE, OnChangeVariance)
	ON_WM_CONTEXTMENU()	
	ON_WM_HELPINFO()	
	ON_EN_KILLFOCUS(IDC_HOTFOLDERDIR_EDIT, OnKillfocusHotfolderdirEdit)
	ON_EN_CHANGE(IDC_HOTFOLDERDIR_EDIT, OnChangeHotfolderdirEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQueueMemory message handlers

void CQueueMemory::OnSpoolBrowse() 
   {
   CString cs;
   UpdateData(TRUE);

   // let the user select the spool disk folder
   cs.LoadString(IDS_SELECT_SPOOL_DIR);
   CRexDirDialog dlg((const char*)cs, (const char*)cs);
   dlg.m_ofn.lpstrInitialDir = m_spool;   
   dlg.m_ofn.Flags &= ~OFN_EXPLORER;   
   if (dlg.DoModal() == IDOK)            
      {
      m_spool = dlg.GetPath(TRUE);
      UpdateData(FALSE);
      }   
   }

void CQueueMemory::OnSelchangeMemAlloc() 
   {
   // get data
   UpdateData(TRUE);

   SetMem();	

   // convert to a number
   CString cs;
   cs = m_memused_edit;
   cs.Replace(",", "");
   // convert to a number
   m_memused = atoi(cs);
   //Now change the devinfo structure
   if(pTargetDevice)
      pTargetDevice->pDevInfo->MemUsed = m_memused;
   }

BOOL CQueueMemory::OnInitDialog() 
   {
   SysMem sm;

#ifdef USEKANJI	       
   // temp hacks!
   CString csTemp;
   ((CComboBox*)GetDlgItem(IDC_MEM_ALLOC))->ResetContent();
   csTemp.LoadString(IDS_CBOX_ADVANCED_AUTOMATIC);
   ((CComboBox*)GetDlgItem(IDC_MEM_ALLOC))->AddString(csTemp);
   csTemp.LoadString(IDS_CBOX_ADVANCED_CUSTOM);
   ((CComboBox*)GetDlgItem(IDC_MEM_ALLOC))->AddString(csTemp);
   ((CComboBox*)GetDlgItem(IDC_AATYPE))->ResetContent();
   csTemp.LoadString(IDS_CBOX_ADVANCED_NONE);
   ((CComboBox*)GetDlgItem(IDC_AATYPE))->AddString(csTemp);
   csTemp.LoadString(IDS_CBOX_ADVANCED_BOX);
   ((CComboBox*)GetDlgItem(IDC_AATYPE))->AddString(csTemp);
   ((CComboBox*)GetDlgItem(IDC_AAOVERSAMPLE))->ResetContent();
   ((CComboBox*)GetDlgItem(IDC_AAOVERSAMPLE))->AddString("2");
   ((CComboBox*)GetDlgItem(IDC_AAOVERSAMPLE))->AddString("3");
   ((CComboBox*)GetDlgItem(IDC_AAOVERSAMPLE))->AddString("4");
   ((CComboBox*)GetDlgItem(IDC_AAOVERSAMPLE))->AddString("5");
   ((CComboBox*)GetDlgItem(IDC_AAOVERSAMPLE))->AddString("6");
#endif   

   m_bHotFolderReset = FALSE;

   m_avail_physical = sm.csFreePhysicalMemory;
   m_virtual_memory = sm.csFreeVirtualMemory;   
   m_maxmem = sm.FreeVirtualMemory;     
   AvailPhysicalMemory = sm.FreePhysicalMemory;
   InstalledMemory = sm.InstalledMemory;

   // set spin control range
   ((CSpinButtonCtrl*)GetDlgItem(IDC_LISTJOBS_SPIN))->SetRange(MINDISKFILES, MAXDISKFILES);

   ((CSpinButtonCtrl*)GetDlgItem(IDC_HOTFOLDER_DELAY_SPIN))->SetRange(0, 10);

   // convert and put memory settings in dialog
   SetMem();

   // set state of spin buttons in spool section
   SetSpoolSpinState();

   // set hot folder button state
   SetHotFolderButtonState();

   if ( m_enable_hotfolder )
   {
	   GetDlgItem(IDC_HF_SELECTDPI)->EnableWindow( m_bSelect_By_Size );
	   GetDlgItem(IDC_HF_VARIANCE)->EnableWindow( m_bSelect_By_Size );
   }

   CPropertyPage::OnInitDialog();

   return TRUE;  
   }

void CQueueMemory::OnScratchBrowse() 
   {
   CString cs;
   UpdateData(TRUE);

   // let the user select the scrath disk folder
   cs.LoadString(IDS_SELECT_SCRATCH_DIR);
   CRexDirDialog dlg((const char*)cs, (const char*)cs);
   dlg.m_ofn.lpstrInitialDir = m_scratch;   
   dlg.m_ofn.Flags &= ~OFN_EXPLORER;   
   if (dlg.DoModal() == IDOK)            
      {
      m_scratch = dlg.GetPath(TRUE);
      UpdateData(FALSE);
      }   
   }

void CQueueMemory::SetMem()
   {
   // set memory based on installed mem
   switch(m_memalloc_style)
	  {
	  case RIPMEM_ALLOC_WORKSTATION:
		   m_memused = (int)((float)InstalledMemory * .25F + .5F);
		   break;
#if 0
	  case RIPMEM_ALLOC_SERVER_1D:
		   m_memused = (int)((float)InstalledMemory * .75F + .5F);
		   break;
	  case RIPMEM_ALLOC_SERVER_2D:
		   m_memused = (int)((float)InstalledMemory * .45F + .5F);
		   break;
	  case RIPMEM_ALLOC_SERVER_3D:
		   m_memused = (int)((float)InstalledMemory * .25F + .5F);
		   break;
	  case RIPMEM_ALLOC_SERVER_4D:
		   m_memused = (int)((float)InstalledMemory * .20F + .5F);
		   break;
#endif
	  case RIPMEM_ALLOC_SERVER:
		   m_memused = (int)((float)InstalledMemory * .75F + .5F);
		   break;
	  case RIPMEM_ALLOC_CUSTOM:
		   // already set
	       break;
	  }			  

   // modify by the queue count
   if (QueueCount.GetQueueCount() > 0)
      m_memused /= QueueCount.GetQueueCount();

   // check bounds   
   m_memused = min(m_memused, m_maxmem);
//   m_memused = max(min(DEVICE_MEMUSED_MAX, m_memused), DEVICE_MEMUSED_MIN); 
   // convert to a string
   nm.ConvertToComma(m_memused, m_memused_edit);

   // put it in
   UpdateData(FALSE);
   // enable/disable custom fields
   GetDlgItem(IDC_MEM_USED_SPIN)->EnableWindow(m_memalloc_style == RIPMEM_ALLOC_CUSTOM);
   GetDlgItem(IDC_MEM_USED_EDIT)->EnableWindow(m_memalloc_style == RIPMEM_ALLOC_CUSTOM);

//   TRACE("CQueueMemory::SetMem, m_memused = %d\n", m_memused);
   }

void CQueueMemory::OnSelchangeListjobsCombo() 
   {
   UpdateData(TRUE);
   SetSpoolSpinState();	
   }

void CQueueMemory::OnSelchangeDiskfilesCombo() 
   {
   UpdateData(TRUE);
   SetSpoolSpinState();	
   }

void CQueueMemory::SetSpoolSpinState() 
   {
   // set state
   GetDlgItem(IDC_LISTJOBS_EDIT)->EnableWindow(m_listjobs != 0);
   GetDlgItem(IDC_LISTJOBS_SPIN)->EnableWindow(m_listjobs != 0);
   }

void CQueueMemory::SetHotFolderButtonState()
{
   // set state
   GetDlgItem(IDC_HOTFOLDERDIR_EDIT)->EnableWindow(m_enable_hotfolder);
   GetDlgItem(IDC_HOTFOLDER_COMBO)->EnableWindow(m_enable_hotfolder);   
   GetDlgItem(IDC_HOTFOLDER_INPANE)->EnableWindow(m_enable_hotfolder);
   GetDlgItem(IDC_IMAGEONLY_NEW)->EnableWindow(m_enable_hotfolder);
   GetDlgItem(IDC_HOTFOLDER_PRMPT)->EnableWindow(m_enable_hotfolder);
   GetDlgItem(IDC_INSERT_HOTFOLDER_PRMPT)->EnableWindow(m_enable_hotfolder);

   GetDlgItem(IDC_HOTFOLDER_DELAY)->EnableWindow(m_enable_hotfolder);
   GetDlgItem(IDC_HOTFOLDER_DELAY_SPIN)->EnableWindow(m_enable_hotfolder);

   if ( m_enable_hotfolder )
	{
		CString		strFolderName;
		CFileFind	Finder;
		DWORD		LastError;

		strFolderName = m_hotfolder_dir + "*.*";
		  //Try and find the folder
		  if(!Finder.FindFile(strFolderName)){
			  //Find out why we failed
			  LastError = GetLastError();
			  if(LastError == ERROR_FILE_NOT_FOUND || LastError == ERROR_PATH_NOT_FOUND)
				   GetDlgItem(IDC_RESET_HOT_FOLDER)->EnableWindow(FALSE);
			  else
			  	   GetDlgItem(IDC_RESET_HOT_FOLDER)->EnableWindow(TRUE);
		  }
		  else
		  {
		  	   GetDlgItem(IDC_RESET_HOT_FOLDER)->EnableWindow(TRUE);
		  }
	}
   else
	{
	   GetDlgItem(IDC_RESET_HOT_FOLDER)->EnableWindow(m_enable_hotfolder);
	}

   GetDlgItem(IDC_HF_SELECTBYSIZE)->EnableWindow( m_enable_hotfolder );
   GetDlgItem(IDC_HF_SELECTDPI)->EnableWindow( m_bSelect_By_Size );
   GetDlgItem(IDC_HF_VARIANCE)->EnableWindow( m_bSelect_By_Size );
}

void CQueueMemory::OnHotfolder() 
   {
   UpdateData(TRUE);
   SetHotFolderButtonState();	  
   }

void CQueueMemory::OnSelectBySize() 
   {
   UpdateData(TRUE);
   GetDlgItem(IDC_HF_SELECTDPI)->EnableWindow( m_bSelect_By_Size );
   GetDlgItem(IDC_HF_VARIANCE)->EnableWindow( m_bSelect_By_Size );
   }

void CQueueMemory::OnHotfolderdirBrowse() 
   {
   CString cs;
   UpdateData(TRUE);

   // let the user select the scrath disk folder
   cs.LoadString(IDS_SELECT_HOTFOLDER_DIR);
   CRexDirDialog dlg((const char*)cs, (const char*)cs);
   dlg.m_ofn.lpstrInitialDir = m_hotfolder_dir;   
   dlg.m_ofn.Flags &= ~OFN_EXPLORER;   
   if (dlg.DoModal() == IDOK)            
      {
      m_hotfolder_dir = dlg.GetPath(TRUE);
      UpdateData(FALSE);
	  // We need to do this to properly set the Reset Button
	  SetHotFolderButtonState();
      }   
   }

void CQueueMemory::OnResetHotFolder()
{
	CString csMessage;

	csMessage.Format(IDS_RESET_HOT_FOLDER, m_hotfolder_dir );
	if (AfxMessageBox(csMessage, MB_YESNO) == IDYES)
	{
		BOOL bFileFound;
		CFileFind	Finder;
		CString csSearch, csFile;

		csSearch = m_hotfolder_dir;

		// make sure the slash is there
		VERIFYPATH(csSearch);

		csSearch = csSearch + "*.*";

		bFileFound = Finder.FindFile(csSearch);
		while ( bFileFound )
		{      
			bFileFound = Finder.FindNextFile();

			if ( Finder.IsDots() )
				continue;

			 // Only Delete if NOT one of these
			 if ( !Finder.IsHidden() && !Finder.IsSystem() &&
						!Finder.IsDirectory() )
			 {
				 ::DeleteFile(Finder.GetFilePath());
			 }
		}

		// Now clear the List of checked files
		m_bHotFolderReset = TRUE;
	}
}


void CQueueMemory::OnDeltaposMemUsedSpin(NMHDR* pNMHDR, LRESULT* pResult) 
   {		
   NM_UPDOWN* pStuff = (NM_UPDOWN*)pNMHDR;   

   // get current val
   UpdateData(TRUE);
   // increment/decrement
   m_memused += (-pStuff->iDelta) * 512;

   // check bounds   
   m_memused = max(min(m_memused, m_maxmem), DEVICE_MEMUSED_MIN);      
//   m_memused = min(DEVICE_MEMUSED_MAX, m_memused);
   // no more than what device needs
   //m_memused = min(m_memused, m_imagememory_num);

   nm.ConvertToComma(m_memused, m_memused_edit);

   // put back in
   UpdateData(FALSE);    
   //Now change the devinfo structure
   if(pTargetDevice)
	   pTargetDevice->pDevInfo->MemUsed = m_memused;

   *pResult = 0;
   }


void CQueueMemory::OnDestroy() 
   {
   // make sure we have all the latest stuff
   UpdateData(TRUE);
   // convert to a number
   CString cs;
   cs = m_memused_edit;
   cs.Replace(",", "");
   // convert to a number
   m_memused = atoi(cs);
    
   CPropertyPage::OnDestroy();
   }

void CQueueMemory::OnChangeMemUsedEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.	
	UpdateData(TRUE);
    //Remove any commas from the string.
    // convert to a number
    CString cs;
    cs = m_memused_edit;
    cs.Replace(",", "");
    // convert to a number
    m_memused = atoi(cs);
	//Now change the devinfo structure
	if(pTargetDevice)
	   pTargetDevice->pDevInfo->MemUsed = m_memused;
   }


void CQueueMemory::OnChangeSelectDPI() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.	
	UpdateData(TRUE);
}

void CQueueMemory::OnChangeVariance() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.	
	UpdateData(TRUE);
}


BOOL CQueueMemory::OnHelpInfo(HELPINFO* pHelpInfo) 
   {
   if (pHelpInfo->iContextType == HELPINFO_WINDOW)   // must be for a control
      {
      ::WinHelp ((HWND)pHelpInfo->hItemHandle,
         AfxGetApp()->m_pszHelpFilePath, HELP_WM_HELP, 
		 (DWORD)(LPVOID)dwHelpMap);
	  return TRUE;
	  }      
   else
   	  return CDialog::OnHelpInfo(pHelpInfo);
   }
 
void CQueueMemory::OnContextMenu(CWnd* pWnd, CPoint point) 
   {   
   // call context help
   ::WinHelp(pWnd->m_hWnd, AfxGetApp()->m_pszHelpFilePath, 
      HELP_CONTEXTMENU,  (DWORD)(LPVOID)dwHelpMap);
   }

void CQueueMemory::OnKillfocusHotfolderdirEdit() 
{
	// TODO: Add your control notification handler code here
	// Need to do this to Set the State of the Reset Folder Button
	UpdateData();
	SetHotFolderButtonState();
}

void CQueueMemory::OnChangeHotfolderdirEdit() 
   {
   CString strFolderName;
   CFileFind	Finder;
   DWORD		LastError;

   // get the current text
   GetDlgItemText(IDC_HOTFOLDERDIR_EDIT, strFolderName.GetBuffer(_MAX_PATH), _MAX_PATH);   
   strFolderName.ReleaseBuffer();   
   VERIFYPATH(strFolderName);
   strFolderName += "*.*";

   // Try to find the folder
   if(!Finder.FindFile(strFolderName))
	  {
	  // Find out why we failed
	  LastError = GetLastError();
	  if(LastError == ERROR_FILE_NOT_FOUND || LastError == ERROR_PATH_NOT_FOUND)
		 GetDlgItem(IDC_RESET_HOT_FOLDER)->EnableWindow(FALSE);
	  else
		 GetDlgItem(IDC_RESET_HOT_FOLDER)->EnableWindow(TRUE);
	  }
   else
	  {
	  GetDlgItem(IDC_RESET_HOT_FOLDER)->EnableWindow(TRUE);
	  }
   }
   