/////////////////////////////////////////////////////////////////////////////
// JobProps.cpp
//                                                                        
// Job Properties manager for RasterPlus
// Copyright (C) 1999-2000 Graphx, Inc.
//
// mods:
//
// 11-30-99 Version 3.0.2 - alias level not getting sent when alias style is changed which 
//                          could cause crash - fixed
// 12-02-99 Version 3.0.2 - added media description to dialog
// 12-09-99 Version 3.0.2 - can now edit package from General tab
// 1-04-00  Version 3.0.3 - Buffer to disk settings not correct
// 1-23-00  Version 3.0.3 - Add support for custom pages
// 2-16-00  Version 3.0.3 - weirdly dated files caused CFileStatus to ASSERT - 
//                          changed to FindFirstFile
// 10-04-00 Version 4.0.1 - job props custom page mods were changed queue props custom page
//                          - fixed
// 10-21-00 Version 4.0.1 - remove optimize orient selection
// 10-10-01 Version 5.0.0 - if no resolutions are available, disable res box
// 05-29-02 Version 5.0.6 - Modify PackagePlus call to pass two parameters /d{device} /p{packagename}
// 01-16-03 Version 5.1.1 - Fix inconsistent sticky Queue Properties - Values being reset incorrectly
// 05-08-03 Version 5.1.2 - Fix inconsistent sticky Queue Properties when cancelling between Queue and Job Properties
//

#include "stdafx.h"
#include "resource.h"
#include "JobProps.h"
#include "visualxf.h"
#include "Prefsmsg.h"
#include "inidefs.h"
#include "winspool.h"
#include "CustomPage.h"
#include "control.hm"
#include "profile.h"
#include "objects.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGeneralJobProps, CPropertyPage)
IMPLEMENT_DYNCREATE(CDeviceJobProps, CPropertyPage)
IMPLEMENT_DYNCREATE(CColorJobProps, CPropertyPage)

const UINT NEAR wm_DevModeChange = RegisterWindowMessage( ID_DEVMODE_CHANGE );
const UINT NEAR wm_DevModeChangeDefault = RegisterWindowMessage( ID_DEVMODE_CHANGE_DEFAULT );
const UINT NEAR wm_DevInfoChange = RegisterWindowMessage( ID_DEVINFO_CHANGE );

// the control help-map
static DWORD dwHelpMap[] = 
   {
   IDC_DEVICE_BOX, HIDC_DEVICE_BOX,
   IDC_PAGEDIMS, HIDC_PAGEDIMS,
   IDC_CONNECTION_PRMPT, HIDC_DEVICE_CONNECTION,
   IDC_CONNECTION, HIDC_DEVICE_CONNECTION,   
   IDC_ORIENTATION_PRMPT, HIDC_DEVICE_ORIENTATION_LIST,
   IDC_ORIENTATION, HIDC_DEVICE_ORIENTATION_LIST,   
   IDC_ORIENTATION_OPTIMIZE, HIDC_ORIENTATION_OPTIMIZE,
   IDC_MEDIA_SIZE_PRMPT, HIDC_DEVICE_MEDIA_SIZE,
   IDC_MEDIA_SIZE, HIDC_DEVICE_MEDIA_SIZE,
   IDC_RESOLUTION_PRMPT, HIDC_DEVICE_RESOLUTION,
   IDC_RESOLUTION, HIDC_DEVICE_RESOLUTION,
   IDC_COPIES_PRMPT, HIDC_DEVICE_COPIES,   
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
   IDOUROK, HIDC_OUROK,
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
   IDC_BUFFERTODISK, HIDC_ADVANCED_BUFFERTODISK,
   IDC_COMPRESS, HIDC_ADVANCED_COMPRESS,
   IDC_COMPRESS_BITMAP, HIDC_ADVANCED_COMPRESS_BITMAP,
   IDC_SCALE_BITMAP, HIDC_ADVANCED_SCALE_BITMAP,      
   IDC_AATYPE, HIDC_ADVANCED_AATYPE,
   IDC_AATYPE_PRMPT, HIDC_ADVANCED_AATYPE,
   IDC_AAOVERSAMPLE, HIDC_ADVANCED_AAOVERSAMPLE,
   IDC_AAOVERSAMPLE_PRMPT, HIDC_ADVANCED_AAOVERSAMPLE,         
   IDC_PRANGE_EDIT, HIDC_PRANGE_EDIT,
   IDC_PRANGE, HIDC_PRANGE,
   IDC_BITMAPSAMPLING, HIDC_BITMAPSAMPLING,
   IDC_BITMAPSAMPLING_PRMPT, HIDC_BITMAPSAMPLING,
   IDOURCANCEL, HIDC_OURCANCEL,   
   ID_CLOSE, HIDC_CLOSE,
   0,0
   };

/////////////////////////////////////////////////////////////////////////////
// CGeneralJobProps property page

CGeneralJobProps::CGeneralJobProps() : CPropertyPage(CGeneralJobProps::IDD)
{
	//{{AFX_DATA_INIT(CGeneralJobProps)
	m_filesize = _T("");
	m_copies = 0;
	m_filetype = _T("");
	m_boundingbox = _T("");
	m_aaoversample = -1;
	m_buffertodisk = FALSE;
	m_compress = FALSE;
	m_compressbitmap = FALSE;
	m_scalebitmap = FALSE;
	m_filedate = _T("");
	m_filetime = _T("");
	m_description = _T("");
	m_filename = _T("");
	m_filepages = _T("");
	m_resolution = -1;
	m_mediasize = -1;
	m_connection = -1;
	m_pagedims = _T("");
	m_orientation = -1;
	m_pagedesc = _T("");
	m_aatype = FALSE;
	m_prange_edit = _T("");
	m_prange = FALSE;
	m_bitmapsampling = -1;
	//}}AFX_DATA_INIT
    pTargetCQueue = NULL;    
	pCQueueFile = NULL;	
	pReportWnd = NULL;	
	IsInit = FALSE;
	DoingDefaultProps = FALSE;	
	DeviceIsSet = FALSE;
	pTargetDevice = NULL;
	bIsUsingDefTarg = FALSE;
	bInUpdateItProc = FALSE;
	m_bNotCreated = TRUE;
}

CGeneralJobProps::~CGeneralJobProps()
{
}

void CGeneralJobProps::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralJobProps)
	DDX_Text(pDX, IDC_FILESIZE, m_filesize);
	DDX_Text(pDX, IDC_COPIES_EDIT, m_copies);
	DDX_Text(pDX, IDC_FILETYPE, m_filetype);
	DDX_Text(pDX, IDC_BOUNDINGBOX, m_boundingbox);
	DDX_CBIndex(pDX, IDC_AAOVERSAMPLE, m_aaoversample);
	DDX_Check(pDX, IDC_BUFFERTODISK, m_buffertodisk);
	DDX_Check(pDX, IDC_COMPRESS, m_compress);
	DDX_Check(pDX, IDC_COMPRESS_BITMAP, m_compressbitmap);
	DDX_Check(pDX, IDC_SCALE_BITMAP, m_scalebitmap);
	DDX_Text(pDX, IDC_FILEDATE, m_filedate);
	DDX_Text(pDX, IDC_FILETIME, m_filetime);
	DDX_Text(pDX, IDC_DESCRIPTION, m_description);
	DDX_Text(pDX, IDC_FILENAME, m_filename);
	DDX_Text(pDX, IDC_PAGES2, m_filepages);
	DDX_CBIndex(pDX, IDC_RESOLUTION, m_resolution);
	DDX_CBIndex(pDX, IDC_MEDIA_SIZE, m_mediasize);		
	DDX_CBIndex(pDX, IDC_CONNECTION, m_connection);
	DDX_Text(pDX, IDC_PAGEDIMS, m_pagedims);
	DDX_CBIndex(pDX, IDC_ORIENTATION, m_orientation);
	DDX_Text(pDX, IDC_PAGEDESC, m_pagedesc);
	DDX_Check(pDX, IDC_AATYPE, m_aatype);
	DDX_Text(pDX, IDC_PRANGE_EDIT, m_prange_edit);
	DDX_Check(pDX, IDC_PRANGE, m_prange);
	DDX_CBIndex(pDX, IDC_BITMAPSAMPLING, m_bitmapsampling);
	//}}AFX_DATA_MAP
}														

BEGIN_MESSAGE_MAP(CGeneralJobProps, CPropertyPage)
	//{{AFX_MSG_MAP(CGeneralJobProps)		
	ON_BN_CLICKED(IDC_BUFFERTODISK, OnBuffertodisk)
	ON_BN_CLICKED(IDC_COMPRESS, OnCompress)
	ON_BN_CLICKED(IDC_COMPRESS_BITMAP, OnCompressBitmap)
	ON_BN_CLICKED(IDC_SCALE_BITMAP, OnScaleBitmap)
	ON_CBN_SELCHANGE(IDC_RESOLUTION, OnSelchangeResolution)
	ON_CBN_SELCHANGE(IDC_MEDIA_SIZE, OnSelchangeMediaSize)
	ON_EN_CHANGE(IDC_COPIES_EDIT, OnChangeCopiesEdit)
	ON_CBN_SELCHANGE(IDC_CONNECTION, OnSelchangeConnection)
	ON_WM_SHOWWINDOW()
	ON_WM_CONTEXTMENU()	
	ON_WM_HELPINFO()	
	ON_CBN_SELCHANGE(IDC_ORIENTATION, OnSelchangeOrientation)
	ON_EN_UPDATE(IDC_DESCRIPTION, OnUpdateDescription)
	ON_CBN_SELCHANGE(IDC_AAOVERSAMPLE, OnSelchangeAaoversample)
	ON_BN_CLICKED(IDC_EDIT_MEDIA, OnEditMedia)
	ON_WM_COPYDATA()
	ON_BN_CLICKED(IDC_AATYPE, OnAatype)
	ON_BN_CLICKED(IDC_PRANGE, OnPrange)
	ON_EN_UPDATE(IDC_PRANGE_EDIT, OnUpdatePrangeEdit)
	ON_CBN_SELCHANGE(IDC_BITMAPSAMPLING, OnSelchangeBitmapsampling)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDeviceJobProps property page

CDeviceJobProps::CDeviceJobProps() : CPropertyPage(CDeviceJobProps::IDD)
   {
	//{{AFX_DATA_INIT(CDeviceJobProps)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	DoingDefaultProps = FALSE;	
   DeviceIsSet = FALSE;
   pReportWnd = NULL;	
   pTargetDevice = NULL;
   IsInit = FALSE;
   }

CDeviceJobProps::~CDeviceJobProps()
{
}

void CDeviceJobProps::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeviceJobProps)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeviceJobProps, CPropertyPage)
	//{{AFX_MSG_MAP(CDeviceJobProps)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorJobProps property page

CColorJobProps::CColorJobProps() : CPropertyPage(CColorJobProps::IDD)
{
	//{{AFX_DATA_INIT(CColorJobProps)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

// defaults
pTargetDevice = NULL;
}

CColorJobProps::~CColorJobProps()
{
}

void CColorJobProps::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorJobProps)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorJobProps, CPropertyPage)
	//{{AFX_MSG_MAP(CColorJobProps)
	ON_WM_DESTROY()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CColorJobProps::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();	
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CColorJobProps::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

BOOL CColorJobProps::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnSetActive();
}

BOOL CColorJobProps::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnKillActive();
}

void CColorJobProps::UpdateIt(BOOL MultipleSelection)
   {
   }

BOOL CDeviceJobProps::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	if(pTargetDevice)
		IsInit = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeviceJobProps::OnDestroy() 
   {
   // have we created our device property box yet?
#if 0
	if(pTargetDevice){
		if (pTargetDevice->PropBoxCreated) 
		  {
		  pTargetDevice->DoPropBox(DEVPROPBOX_DESTROY, this);
		  pTargetDevice->PropBoxCreated = FALSE;
		  }
	}
#endif

   CPropertyPage::OnDestroy();	
   }

BOOL CDeviceJobProps::OnKillActive() 
   {
   // if we have a place to send back messages, tell the device
   if(pTargetDevice){
		if (pReportWnd)
		  pTargetDevice->SetDeviceWnd(pReportWnd->GetSafeHwnd());

	   //Have we created our device property box yet?
	   if (pTargetDevice->PropBoxCreated){
		  pTargetDevice->DoPropBox(DEVPROPBOX_DESTROY, this);
		  pTargetDevice->PropBoxCreated = FALSE;
	   }
   }

   return CPropertyPage::OnKillActive();
   }
	

BOOL CDeviceJobProps::OnSetActive() 
   {
	BOOL bSaved = FALSE;

   //TRACE("CDeviceJobProps::OnSetActive()\n");
   // if we have a place to send back messages, tell the device
   if(pTargetDevice){
		if (pReportWnd)
		  pTargetDevice->SetDeviceWnd(pReportWnd->GetSafeHwnd());	   

	   // have we created our device property box yet?
	   if (!pTargetDevice->PropBoxCreated)
		  {
		  // save the devmode as DEVPROPBOX_CREATE reset everything
		   if ( DoingDefaultProps )
			bSaved = pTargetDevice->SaveDevInfo();

		  if (pTargetDevice->DoPropBox(DEVPROPBOX_CREATE, this))
			 pTargetDevice->PropBoxCreated = TRUE;

		  if (bSaved)
			  pTargetDevice->RestoreDevInfo();
		  }
   }

   return CPropertyPage::OnSetActive();
   }

void CDeviceJobProps::UpdateIt(BOOL MultipleSelection){	
}

void CDeviceJobProps::ResetWindow(CDevice* pOldDevice, CDevice* pNewDevice, HWND hWnd )
   {
	BOOL bSaved = FALSE;

   if (pOldDevice && pNewDevice)
	  {
	  if (*pOldDevice != *pNewDevice)
		 {
	     //Destroy the existing window.
	     if(pOldDevice)
			{
		    if(pOldDevice->PropBoxCreated)
			   {
			   pOldDevice->DoPropBox(DEVPROPBOX_DESTROY, this);
			   pOldDevice->PropBoxCreated = FALSE;
			   }
			}
		 // Reassign the new device
		 pTargetDevice = pNewDevice;
		 }
	  }

    if ( hWnd == NULL )
		return;

	//Create the new prop box
	if(pTargetDevice){
		if(pReportWnd)
		  pTargetDevice->SetDeviceWnd(pReportWnd->GetSafeHwnd());	   

	   // have we created our device property box yet?
	   if(!pTargetDevice->PropBoxCreated)
		  {
		  // save the devmode as DEVPROPBOX_CREATE reset everything
		   if ( DoingDefaultProps )
				bSaved = pTargetDevice->SaveDevInfo();

		  if (pTargetDevice->DoPropBox(DEVPROPBOX_CREATE, this))
			 pTargetDevice->PropBoxCreated = TRUE;			 

		  // Restore the Standard DevInfo Before it gets destroyed
		  if (bSaved)
			  pTargetDevice->RestoreDevInfo();
		  }
	   else
		  pTargetDevice->DoPropBox(DEVPROPBOX_SETACTIVE, this);
	}
}

void CGeneralJobProps::UpdateIt(BOOL MultipleSelection, BOOL IsIniting)
   {   
   CComboBox* pCB;
   CComboBox* pCBR;
   CComboBox* pCBC;
   int i;
   MEDIA Media;   
   CString csRes;
   CString csConnect;   
   CString csThisConnect;
   
   //if (!DeviceIsSet) return;
   if (!pTargetDevice) return;

   bInUpdateItProc = TRUE;

   if (!MultipleSelection)
	  {   
      m_scalebitmap = (BOOL)pTargetDevice->pDevInfo->ScaleImage;
      m_canscale_bitmap = (BOOL)pTargetDevice->pDevInfo->CanScaleImage;
      m_cancompress = (BOOL)pTargetDevice->pDevInfo->CanDoCompression;
      m_canaalias = (BOOL)pTargetDevice->pDevInfo->CanDoAntiAliasing;
      m_aatype = (BOOL)(pTargetDevice->pDevInfo->AliasStyle == ANTIALIAS_STYLE_BOX);
	  m_bitmapsampling = pTargetDevice->pDevInfo->BitmapSample;
      m_aaoversample = max((int)pTargetDevice->pDevInfo->AliasLevel - 2, 0);
      m_buffertodisk = (BOOL)pTargetDevice->pDevInfo->BufferToDisk;
      m_compress = (BOOL)pTargetDevice->pDevInfo->UseCompression;
      m_compressbitmap = (BOOL)pTargetDevice->pDevInfo->UseCompressionBmp;   
      m_copies = pTargetDevice->pDevInfo->Copies;
	  m_orientation = pTargetDevice->pDevInfo->Orientation;
	  //m_orientation_optimize = pTargetDevice->pDevInfo->OptimizeOrientation;
      m_resolution = pTargetDevice->pDevInfo->CurrentResIdx;
      m_mediasize = pTargetDevice->pDevInfo->CurrentMediaIdx;
	  csThisConnect = pTargetDevice->pDevInfo->ConnectStr;
      // set all the dialog values      
	  if (pTargetCQueue)
		 {
		 m_filename = pTargetCQueue->LongName;
         m_filepages.Format("%d", pTargetCQueue->PageCount);		  
         pTargetCQueue->GetFileTypeName(m_filetype);
         pTargetCQueue->GetSizeString(m_filesize);
         pTargetCQueue->GetDateString(m_filedate);
         pTargetCQueue->GetTimeString(m_filetime);
         m_description = pTargetCQueue->Description;
         m_boundingbox = pTargetCQueue->BoundingBoxStr;		 
         m_prange_edit = pTargetCQueue->m_prange_edit;		 
		 m_prange = pTargetCQueue->m_prange;		 
   		 }
	  else
		 {
		 m_filename = "";
         m_filepages = "";
         m_filetype = "";
         m_filesize = "";
         m_filedate = "";
         m_filetime = "";
         m_description = "";
         m_boundingbox = "";
         m_prange_edit = "";
		 m_prange = FALSE;   
		 }					
	  }

   // set the state of all buttons and windows
   SetButtonState(IsIniting);
   //if (!IsWindow(m_hWnd)) return;
   //if (!IsWindowVisible()) return;

   if (pTargetCQueue)
	  {
      // if the size is zero, AND it is not deleted update it
      if ((pTargetCQueue->iFileSize == 0) && 
         ((pTargetCQueue->StatusFlag & FILE_STATUS_CHECK) != FILE_STATUS_OK_DELETED))
		 {
		 HANDLE hFile;
		 WIN32_FIND_DATA FileInfo;		 
		 if ((hFile = FindFirstFile(pTargetCQueue->LongName, 
			&FileInfo)) != INVALID_HANDLE_VALUE)
			{      			
			// close the handle
			FindClose(hFile);   
			}				 	
	     pTargetCQueue->RealFileSize = pTargetCQueue->iFileSize = FileInfo.nFileSizeLow;
		 }
	  }

   // reset the media source list
   pCB = (CComboBox*)GetDlgItem(IDC_MEDIA_SIZE);
   pCB->ResetContent();   					 
   pCBR = (CComboBox*)GetDlgItem(IDC_RESOLUTION);
   pCBR->ResetContent();   					    
   pCBC = (CComboBox*)GetDlgItem(IDC_CONNECTION);
   pCBC->ResetContent();   					    

   // check for valid device info
   if (pTargetDevice->pDevInfo)
	  {
	  pTargetDevice->pDevInfo->TotalMedia = pTargetDevice->UpdateMediaList();
	  // insert the sources into the list
      for (i = 0; i < pTargetDevice->pDevInfo->TotalMedia; i++)
		 {	  
	     pTargetDevice->GetMedia(i, &Media);	  	     
	     pCB->InsertString(-1, Media.Name);
		 }
#if 0
	  if (pTargetDevice->pDevInfo->bExtra & DEVICE_EXTRA_CANDOPACKAGES)
		 {
		 if (bPackagePlusInstalled)
		    pCB->InsertString(-1, "<< New Package >>");
		 }
#endif
      // set current
	  pCB->SetCurSel(min(m_mediasize, pTargetDevice->pDevInfo->TotalMedia - 1));	        
	  
      // insert the resolutions into the list
	  pTargetDevice->pDevInfo->TotalRes = pTargetDevice->UpdateResList();

	  if (pTargetDevice->pDevInfo->TotalRes > 0)
		 {
		 GetDlgItem(IDC_RESOLUTION_PRMPT)->EnableWindow(TRUE);	  
		 GetDlgItem(IDC_RESOLUTION)->EnableWindow(TRUE);	  
		 for (i = 0; i < pTargetDevice->pDevInfo->TotalRes; i++)
			{	  
			pTargetDevice->GetRes(i, csRes.GetBuffer(MAX_RESNAME));	  
			csRes.ReleaseBuffer();
			pCBR->InsertString(-1, csRes);
			}
		 // set current
		 pCBR->SetCurSel(min(m_resolution, pTargetDevice->pDevInfo->TotalRes - 1));
		 }
	  else
		 {
		 GetDlgItem(IDC_RESOLUTION_PRMPT)->EnableWindow(FALSE);	  
		 GetDlgItem(IDC_RESOLUTION)->EnableWindow(FALSE);	  
		 }
	  }

   // insert the connections into the list
   pTargetDevice->pDevInfo->TotalConnect = pTargetDevice->UpdateConnectList();
   if (pTargetDevice->pDevInfo->TotalConnect > 0)
	  {
	  GetDlgItem(IDC_CONNECTION)->EnableWindow(TRUE);	  
	  GetDlgItem(IDC_CONNECTION_PRMPT)->EnableWindow(TRUE);	  
	  for (i = 0; i < pTargetDevice->pDevInfo->TotalConnect; i++)
		 {	  
		 pTargetDevice->GetConnect(i, csConnect.GetBuffer(MAX_CONNECTNAME));	  
		 csConnect.ReleaseBuffer();
		 pCBC->InsertString(-1, csConnect);
		 }
	  // set current connection
	  m_connection = pCBC->FindStringExact(-1, csThisConnect);
	  }
   else
	  {
	  GetDlgItem(IDC_CONNECTION)->EnableWindow(FALSE);	  
	  GetDlgItem(IDC_CONNECTION_PRMPT)->EnableWindow(FALSE);	  
	  }
   
   // update the data in the dialog
   UpdateData(FALSE);

   // update the media dims
   UpdateMediaDims();

   // enable edit button based on editability!
   GetDlgItem(IDC_EDIT_MEDIA)->EnableWindow(EnableMediaEdit());	  

   GetDlgItem(IDC_PRANGE_EDIT)->EnableWindow(m_prange);		

   // special case for copies to set an indeterminate state
   // in the case of multiple selections
   if (m_copies == -1) SetDlgItemText(IDC_COPIES_EDIT, " ");	  

   // bring up possible alert   
   pTargetDevice->AlertModeChange(DEVMODE_CHANGE_MEDIA);
	
   bInUpdateItProc = FALSE;
   }

BOOL CGeneralJobProps::OnInitDialog() 
   {
   CPropertyPage::OnInitDialog();		

   // is packageplus installed
   bPackagePlusInstalled = IsPackagePlusInstalled();

   //create the control
   ToolTipCtrl.Create(this, TTS_ALWAYSTIP);	  
   // enable tool tips
   EnableToolTips();	
   
   if (m_bNotCreated)
	  {
      secbEditMedia.AttachButton(IDC_EDIT_MEDIA, SECBitmapButton::Al_Center, IDB_EDITPAGE, this);
      m_bNotCreated = FALSE;
      ToolTipCtrl.AddTool(&secbEditMedia, IDS_EDIT_MEDIA);   
	  }

   IsInit = TRUE;

   UpdateIt(FALSE, TRUE);
      
   if (pTargetDevice) UpdateMediaDims();   

   // enable edit button based on editability!
   GetDlgItem(IDC_EDIT_MEDIA)->EnableWindow(EnableMediaEdit());	  

   return TRUE;
   }

void CGeneralJobProps::SetButtonState(BOOL IsIniting /*= FALSE*/) 
   {
   BOOL setting = FALSE;
   if (!IsWindow(m_hWnd)) return;
   //if (!IsWindowVisible()) return;

   // turn off file specific stuff for default props
   EnableFileProps(!DoingDefaultProps);
   GetDlgItem(IDC_PRANGE)->EnableWindow(!DoingDefaultProps);      

   // if indeterminate, make this a different kind of check
   ((CButton*)GetDlgItem(IDC_BUFFERTODISK))->SetButtonStyle((m_buffertodisk == 2) ? 
      BS_AUTO3STATE : BS_AUTOCHECKBOX);
   
   //Make sure that we really need to have the buffer to disk selection enabled.
   if (pTargetDevice->pDevInfo->EnableBufferToDisk == DEV_BUFFERTODISK_DISABLE)
	  setting = FALSE;
   else if (pTargetDevice->pDevInfo->EnableBufferToDisk == DEV_BUFFERTODISK_ALWAYSALLOW)
	  setting = TRUE;
   else									
	  {
	  if (pTargetDevice->pDevInfo->MemUsed >= (pTargetDevice->pDevInfo->BandSize / 1024 * pTargetDevice->pDevInfo->BandWidth))
	     setting = FALSE;
	  else
		 setting = TRUE;
	  }	 
   ((CButton*)GetDlgItem(IDC_BUFFERTODISK))->EnableWindow(setting);		

   ((CButton*)GetDlgItem(IDC_COMPRESS))->SetButtonStyle((m_compress == 2) ? 
      BS_AUTO3STATE : BS_AUTOCHECKBOX);
   ((CButton*)GetDlgItem(IDC_COMPRESS_BITMAP))->SetButtonStyle((m_compressbitmap == 2) ? 
      BS_AUTO3STATE : BS_AUTOCHECKBOX);
   ((CButton*)GetDlgItem(IDC_SCALE_BITMAP))->SetButtonStyle((m_scalebitmap == 2) ? 
      BS_AUTO3STATE : BS_AUTOCHECKBOX);

   if (!pTargetDevice->pDevInfo)
      {	        	  
	  GetDlgItem(IDC_MEDIA_SIZE)->EnableWindow(FALSE);	  
	  GetDlgItem(IDC_MEDIA_SIZE_PRMPT)->EnableWindow(FALSE);	  
	  //GetDlgItem(IDC_RESOLUTION)->EnableWindow(FALSE);
	  //GetDlgItem(IDC_RESOLUTION_PRMPT)->EnableWindow(FALSE);
      GetDlgItem(IDC_ORIENTATION)->EnableWindow(FALSE);      
	  GetDlgItem(IDC_ORIENTATION_PRMPT)->EnableWindow(FALSE);      
	  GetDlgItem(IDC_ORIENTATION_OPTIMIZE)->EnableWindow(FALSE);      
	  //GetDlgItem(IDC_PRANGE)->EnableWindow(FALSE);      
	  //GetDlgItem(IDC_PRANGE_EDIT)->EnableWindow(FALSE);      
	  }   
   else
      {      	              
	  GetDlgItem(IDC_MEDIA_SIZE)->EnableWindow(TRUE);
	  GetDlgItem(IDC_MEDIA_SIZE_PRMPT)->EnableWindow(TRUE);
	  //GetDlgItem(IDC_RESOLUTION)->EnableWindow(TRUE);
	  //GetDlgItem(IDC_RESOLUTION_PRMPT)->EnableWindow(TRUE);	  
      GetDlgItem(IDC_ORIENTATION)->EnableWindow(pTargetDevice->pDevInfo->CanDoOrientation);      
	  GetDlgItem(IDC_ORIENTATION_PRMPT)->EnableWindow(pTargetDevice->pDevInfo->CanDoOrientation);      
   	  // should we enable this?
 	  //GetDlgItem(IDC_ORIENTATION_OPTIMIZE)->EnableWindow(pTargetDevice->pDevInfo->CanOptimizeOrientation);
	  //GetDlgItem(IDC_PRANGE)->EnableWindow(TRUE);      
	  //GetDlgItem(IDC_PRANGE_EDIT)->EnableWindow(m_prange);      
	  }
   // make sure the right options are available
   GetDlgItem(IDC_SCALE_BITMAP)->EnableWindow(!m_aatype && m_canscale_bitmap);
   GetDlgItem(IDC_COMPRESS)->EnableWindow(m_cancompress);   
   GetDlgItem(IDC_COMPRESS_BITMAP)->EnableWindow(m_cancompress);

   // anti-aliasing
   if (m_canaalias)
      {
	  GetDlgItem(IDC_AATYPE)->EnableWindow(TRUE);   
      // no anti-alias style if aa is off
      GetDlgItem(IDC_AAOVERSAMPLE)->EnableWindow(m_aatype);
	  GetDlgItem(IDC_AAOVERSAMPLE_PRMPT)->EnableWindow(m_aatype);
	  }
   else
      {
      GetDlgItem(IDC_AATYPE)->EnableWindow(FALSE);   
      GetDlgItem(IDC_AAOVERSAMPLE)->EnableWindow(FALSE);   
	  GetDlgItem(IDC_AAOVERSAMPLE_PRMPT)->EnableWindow(FALSE);
	  }   

   ((CSpinButtonCtrl*)GetDlgItem(IDC_COPIES_SPIN))->SetRange(IDDM_MIN_COPIES, IDDM_MAX_COPIES);   
	//Now update the other tabs in the dialog
   if (pTargetDevice)
   {
	   DEVCHANGE DevChange;

		//Load structure
		DevChange.ItemChanged = 0;
		DevChange.pCD = pTargetDevice;

		UpdateMediaDims();
		if(pReportWnd && !IsIniting)
			pReportWnd->SendMessage(wm_DevModeChangeDefault, PREFS_CHANGE_DEVICE, (LPARAM)&DevChange);
   }
}

void CGeneralJobProps::OnBuffertodisk() 
   {
   UpdateData(TRUE);
   //Do we need to create a new object?
   if(bIsUsingDefTarg){
      // We're using the default QUEUEDEVICE.
      // We will now need to create a new CDEVICE object so that 
	  // we don't overwrite the defaults.
	  // init with the current target device
	  InternalDevice = *pTargetDevice;
	  // and point to it
	  pTargetDevice = &InternalDevice;
      //Now make the switch
      bIsUsingDefTarg = FALSE;
   }
   // remove indeterminancy
   ((CButton*)GetDlgItem(IDC_BUFFERTODISK))->SetButtonStyle(BS_AUTOCHECKBOX);
   pTargetDevice->pDevInfo->BufferToDisk = m_buffertodisk;
   // make sure the queue knows about this
   ReportChange(DEVCHANGE_BUFFERTODISK);
   }

void CGeneralJobProps::OnCompress() 
   {
   UpdateData(TRUE);
   //Do we need to create a new object?
   if(bIsUsingDefTarg){
      // We're using the default QUEUEDEVICE.
      // We will now need to create a new CDEVICE object so that 
	  // we don't overwrite the defaults.
	  // init with the current target device
	  InternalDevice = *pTargetDevice;
	  // and point to it
	  pTargetDevice = &InternalDevice;
      //Now make the switch
      bIsUsingDefTarg = FALSE;
   }
   // remove indeterminancy
   ((CButton*)GetDlgItem(IDC_COMPRESS))->SetButtonStyle(BS_AUTOCHECKBOX);
   pTargetDevice->pDevInfo->UseCompression = m_compress;
   // make sure the queue knows about this
   ReportChange(DEVCHANGE_USECOMPRESSION);
   }

void CGeneralJobProps::OnCompressBitmap() 
   {
   UpdateData(TRUE);
   //Do we need to create a new object?
   if(bIsUsingDefTarg){
      // We're using the default QUEUEDEVICE.
      // We will now need to create a new CDEVICE object so that 
	  // we don't overwrite the defaults.
	  // init with the current target device
	  InternalDevice = *pTargetDevice;
	  // and point to it
	  pTargetDevice = &InternalDevice;
      //Now make the switch
      bIsUsingDefTarg = FALSE;
   }
   // remove indeterminancy
   ((CButton*)GetDlgItem(IDC_COMPRESS_BITMAP))->SetButtonStyle(BS_AUTOCHECKBOX);
   pTargetDevice->pDevInfo->UseCompressionBmp = m_compressbitmap;   
   // make sure the queue knows about this
   ReportChange(DEVCHANGE_USECOMPRESSIONBMP);
   }

void CGeneralJobProps::OnScaleBitmap() 
   {
   UpdateData(TRUE);
   //Do we need to create a new object?
   if(bIsUsingDefTarg){
      // We're using the default QUEUEDEVICE.
      // We will now need to create a new CDEVICE object so that 
	  // we don't overwrite the defaults.
	  // init with the current target device
	  InternalDevice = *pTargetDevice;
	  // and point to it
	  pTargetDevice = &InternalDevice;
      //Now make the switch
      bIsUsingDefTarg = FALSE;
   }
   // remove indeterminancy
   ((CButton*)GetDlgItem(IDC_SCALE_BITMAP))->SetButtonStyle(BS_AUTOCHECKBOX);
   pTargetDevice->pDevInfo->ScaleImage = m_scalebitmap;
   // make sure the queue knows about this
   ReportChange(DEVCHANGE_SCALEIMAGE);
   }

void CGeneralJobProps::OnSelchangeResolution() 
   {
   UpdateData(TRUE);
   //Do we need to create a new object?
   if(bIsUsingDefTarg){
      // We're using the default QUEUEDEVICE.
      // We will now need to create a new CDEVICE object so that 
	  // we don't overwrite the defaults.
	  // init with the current target device
	  InternalDevice = *pTargetDevice;
	  // and point to it
	  pTargetDevice = &InternalDevice;
      //Now make the switch
      bIsUsingDefTarg = FALSE;
   }
   pTargetDevice->pDevInfo->CurrentResIdx = m_resolution;
   // make sure the queue knows about this
   ReportChange(DEVCHANGE_RESIDX);
   }

void CGeneralJobProps::OnSelchangeMediaSize() 
   {
   UpdateData(TRUE);

   //Do we need to create a new object?
   if(bIsUsingDefTarg){
      // We're using the default QUEUEDEVICE.
      // We will now need to create a new CDEVICE object so that 
	  // we don't overwrite the defaults.
	  // init with the current target device
	  InternalDevice = *pTargetDevice;
	  // and point to it
	  pTargetDevice = &InternalDevice;
      //Now make the switch
      bIsUsingDefTarg = FALSE;
   }
   pTargetDevice->pDevInfo->CurrentMediaIdx = m_mediasize;
   // make sure the queue knows about this
   ReportChange(DEVCHANGE_MEDIAIDX);

   // bring up possible alert
   pTargetDevice->AlertModeChange(DEVMODE_CHANGE_MEDIA_PRMPT);
   pTargetDevice->AlertModeChange(DEVMODE_CHANGE_MEDIA);	 	

   if(pTargetDevice->pDevInfo){
	   GetDlgItem(IDC_ORIENTATION)->EnableWindow(pTargetDevice->pDevInfo->CanDoOrientation);
	   GetDlgItem(IDC_ORIENTATION_PRMPT)->EnableWindow(pTargetDevice->pDevInfo->CanDoOrientation);
   }

   // this may have changed
   ReportChange(DEVCHANGE_ORIENTATION);

   // enable edit button based on editability!
   GetDlgItem(IDC_EDIT_MEDIA)->EnableWindow(EnableMediaEdit());	  

   UpdateMediaDims();
   }

void CGeneralJobProps::OnChangeCopiesEdit() 
   {
   //Do we need to create a new object?
   if(bIsUsingDefTarg){
      // We're using the default QUEUEDEVICE.
      // We will now need to create a new CDEVICE object so that 
	  // we don't overwrite the defaults.
	  // init with the current target device
	  InternalDevice = *pTargetDevice;
	  // and point to it
	  pTargetDevice = &InternalDevice;
      //Now make the switch
      bIsUsingDefTarg = FALSE;
   }			
   // make sure the dialog is inited before doing this
   // to prevent crash
   if (!IsInit) return;
   // if we've blanked out the field, don't apply
   // to the device   
   GetDlgItemText(IDC_COPIES_EDIT, csTemp);
   if (csTemp == " ") return;
   // write the current settings to the CurrentDevice and
   // broadcast the change
   // make sure the queue knows about this
   //if (DeviceIsSet) 
   if (pTargetDevice)
	  {
	  UpdateData(TRUE);
	  pTargetDevice->pDevInfo->Copies = m_copies;
      ReportChange(DEVCHANGE_COPIES);	
	  }
   }

void CGeneralJobProps::ReportChange(int ItemChanged)
   {
   DEVCHANGE DevChange;   

   // Load structure
   DevChange.ItemChanged = ItemChanged;
   DevChange.pCD = pTargetDevice;
   //Send to report to all concerned parties
   if(pReportWnd && !bInUpdateItProc)
	  {
	  //Send report to all other tabs
	  pReportWnd->SendMessage(wm_DevModeChangeDefault, 
		 PREFS_CHANGE_DEVICE, (LPARAM)&DevChange);
      //Now send report to window
      if(!DoingDefaultProps)
		 pReportWnd->SendMessage(wm_DevModeChange, PREFS_CHANGE_DEVICE, (LPARAM)&DevChange);
   }	 	
}

CDevice& CGeneralJobProps::GetDefDevice()
   {
   // store device
   return *pTargetDevice;
   }							 

void CGeneralJobProps::EnableFileProps(BOOL bEnable)
   {				   
   GetDlgItem(IDC_FILESIZE)->EnableWindow(bEnable);
   GetDlgItem(IDC_FILESIZE_PRMPT)->EnableWindow(bEnable);
   GetDlgItem(IDC_FILENAME)->EnableWindow(bEnable);
   GetDlgItem(IDC_FILENAME_PRMPT)->EnableWindow(bEnable);
   GetDlgItem(IDC_FILETYPE)->EnableWindow(bEnable);
   GetDlgItem(IDC_FILETYPE_PRMPT)->EnableWindow(bEnable);
   GetDlgItem(IDC_BOUNDINGBOX)->EnableWindow(bEnable);
   GetDlgItem(IDC_BOUNDINGBOX_PRMPT)->EnableWindow(bEnable);
   GetDlgItem(IDC_FILEDATE)->EnableWindow(bEnable);
   GetDlgItem(IDC_FILEDATE_PRMPT)->EnableWindow(bEnable);
   GetDlgItem(IDC_FILETIME)->EnableWindow(bEnable);
   GetDlgItem(IDC_FILETIME_PRMPT)->EnableWindow(bEnable);
   GetDlgItem(IDC_DESCRIPTION)->EnableWindow(bEnable);
   GetDlgItem(IDC_DESCRIPTION_PRMPT)->EnableWindow(bEnable);
   GetDlgItem(IDC_PAGES2)->EnableWindow(bEnable);
   GetDlgItem(IDC_PAGES_PRMPT)->EnableWindow(bEnable);
   }

void CGeneralJobProps::OnSelchangeConnection() 
   {
   //Do we need to create a new object?
   if(bIsUsingDefTarg){
      // We're using the default QUEUEDEVICE.
      // We will now need to create a new CDEVICE object so that 
	  // we don't overwrite the defaults.
	  // init with the current target device
	  InternalDevice = *pTargetDevice;
	  // and point to it
	  pTargetDevice = &InternalDevice;
      //Now make the switch
      bIsUsingDefTarg = FALSE;
   }
   UpdateData(TRUE);
   
   // get connection
   CComboBox* pCBC = (CComboBox*)GetDlgItem(IDC_CONNECTION);
   pCBC->GetLBText(m_connection, pTargetDevice->pDevInfo->ConnectStr);

   // make sure the queue knows about this
   ReportChange(DEVCHANGE_CONNECTIDX);
   // set the print driver
#if 0
   CString csConnect;
   pTargetDevice->GetConnect(pTargetDevice->pDevInfo->CurrentConnectIdx, 
	  csConnect.GetBuffer(MAX_CONNECTNAME));	  
   csConnect.ReleaseBuffer();
   SetOurPort(pTargetDevice->csFriendlyName, csConnect);
#endif
   SetOurPort(pTargetDevice->csFriendlyName, pTargetDevice->pDevInfo->ConnectStr);

   // reset the paper list
   ResetPaperList();
   }

void CGeneralJobProps::UpdateMediaDims()
   {   
   CRect cMediaRect;   

   // get media rect
   pTargetDevice->GetCurrentMediaRect(cMediaRect);   
#ifdef USEKANJI	   
   sprintf(m_pagedims.GetBuffer(256), "%.2f mm x %.2f mm  (%.2f in x %.2f in)", 	  
	  (float)cMediaRect.Height() * 25.4F / 100.F, (float)cMediaRect.Width() * 25.4 / 100.F,
      (float)cMediaRect.Height() / 100.F, (float)cMediaRect.Width() / 100.F);   
#else
   sprintf(m_pagedims.GetBuffer(256), "%.2f in x %.2f in  (%.2f mm x %.2f mm)", 
	  (float)cMediaRect.Height() / 100.F, (float)cMediaRect.Width() / 100.F,
	  (float)cMediaRect.Height() * 25.4F / 100.F, (float)cMediaRect.Width() * 25.4 / 100.F);   
#endif
   m_pagedims.ReleaseBuffer();   
   pTargetDevice->GetCurrentMediaDescription(m_pagedesc);
   UpdateData(FALSE);
   }

void CGeneralJobProps::OnShowWindow(BOOL bShow, UINT nStatus) 
   {
   CPropertyPage::OnShowWindow(bShow, nStatus);
   BOOL setting = FALSE;

   //Make sure that we really need to have the buffer to disk selection enabled.
   if(bShow && pTargetDevice)
	  {  
	  if (pTargetDevice->pDevInfo->EnableBufferToDisk == DEV_BUFFERTODISK_DISABLE)
         setting = FALSE;
      else if (pTargetDevice->pDevInfo->EnableBufferToDisk == DEV_BUFFERTODISK_ALWAYSALLOW)
         setting = TRUE;
      else									
		 {
	     if (pTargetDevice->pDevInfo->MemUsed >= (pTargetDevice->pDevInfo->BandSize / 1024 * pTargetDevice->pDevInfo->BandWidth))
	        setting = FALSE;
	     else
		    setting = TRUE;
		 }	 
      ((CButton*)GetDlgItem(IDC_BUFFERTODISK))->EnableWindow(setting);		
	  }
   }

void CGeneralJobProps::SetOurPort(LPCSTR csDev, LPCSTR csPort)
   {      
   HANDLE hPrinter;   
   PRINTER_INFO_2* pi2;
   DWORD cbNeeded;
   LPBYTE lpBuf;   

   PRINTER_DEFAULTS pd;
   pd.pDatatype = NULL;
   pd.pDevMode = NULL;
   pd.DesiredAccess = PRINTER_ALL_ACCESS;
   
   // must tell windows!!!
   if (::OpenPrinter((char*)(const char*)csDev, &hPrinter, &pd))
      {
   	  if (hPrinter) 
	     {
	  	 ::GetPrinter(hPrinter, 2, NULL, 0, &cbNeeded);
		 // make the buffer equal to the old plus the new port info
		 lpBuf = (LPBYTE)(new char[cbNeeded + strlen(csPort) + 1]);
		 if (::GetPrinter(hPrinter, 2, lpBuf, cbNeeded, &cbNeeded))
		    {
		    // copy the new port
		    strcpy((char*)(lpBuf + cbNeeded), (const char*)csPort);
		    // point to the buffer
		    pi2 = (PRINTER_INFO_2*)lpBuf;
		    // set the new port	
		    pi2->pPortName = (LPSTR)(lpBuf + cbNeeded);
		    // do it
		    ::SetPrinter(hPrinter, 2, lpBuf, 0);
		    }
		 delete lpBuf;
		 ::ClosePrinter(hPrinter);
		 }	  
      }
   }

BOOL CGeneralJobProps::OnHelpInfo(HELPINFO* pHelpInfo) 
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
 
void CGeneralJobProps::OnContextMenu(CWnd* pWnd, CPoint point) 
   {   
   // call context help
   ::WinHelp(pWnd->m_hWnd, AfxGetApp()->m_pszHelpFilePath, 
      HELP_CONTEXTMENU,  (DWORD)(LPVOID)dwHelpMap);
   }

BOOL CGeneralJobProps::OnSetActive() 
   {
   CComboBox* pCB;
   CComboBox* pCBR;
   CComboBox* pCBC;
   int i;
   MEDIA Media;   
   CString csRes;
   CString csConnect;   

   //if (!DeviceIsSet) return CPropertyPage::OnSetActive();
   if (!pTargetDevice) return CPropertyPage::OnSetActive();

   // reset the media source list
   pCB = (CComboBox*)GetDlgItem(IDC_MEDIA_SIZE);
   pCB->ResetContent();   					 
   pCBR = (CComboBox*)GetDlgItem(IDC_RESOLUTION);
   pCBR->ResetContent();   					    
   pCBC = (CComboBox*)GetDlgItem(IDC_CONNECTION);
   pCBC->ResetContent();   					    

   // check for valid device info
   if (pTargetDevice->pDevInfo)
	  {
	  pTargetDevice->pDevInfo->TotalMedia = pTargetDevice->UpdateMediaList();
	  // insert the sources into the list
      for (i = 0; i < pTargetDevice->pDevInfo->TotalMedia; i++)
		 {	  
	     pTargetDevice->GetMedia(i, &Media);	  	     
	     pCB->InsertString(-1, Media.Name);
		 }

#if 0
	  if (pTargetDevice->pDevInfo->bExtra & DEVICE_EXTRA_CANDOPACKAGES)
		 {
		 if (bPackagePlusInstalled)
		    pCB->InsertString(-1, "<< New Package >>");
		 }
#endif

      // set current
      pCB->SetCurSel(min(m_mediasize, pTargetDevice->pDevInfo->TotalMedia - 1));	  

      // insert the resoluitions into the list
	  pTargetDevice->pDevInfo->TotalRes = pTargetDevice->UpdateResList();
      for (i = 0; i < pTargetDevice->pDevInfo->TotalRes; i++)
		 {	  
	     pTargetDevice->GetRes(i, csRes.GetBuffer(MAX_RESNAME));	  
	     csRes.ReleaseBuffer();
	     pCBR->InsertString(-1, csRes);
		 }
      // set current      
	  pCBR->SetCurSel(min(m_resolution, pTargetDevice->pDevInfo->TotalRes - 1));
	  }

   // insert the connections into the list
   pTargetDevice->pDevInfo->TotalConnect = pTargetDevice->UpdateConnectList();
   for (i = 0; i < pTargetDevice->pDevInfo->TotalConnect; i++)
      {	  
	  pTargetDevice->GetConnect(i, csConnect.GetBuffer(MAX_CONNECTNAME));	  
	  csConnect.ReleaseBuffer();
	  pCBC->InsertString(-1, csConnect);
	  }
   // set current   
   pCBC->SetCurSel(min(m_connection, pTargetDevice->pDevInfo->TotalConnect - 1));
   
   // update the data in the dialog
   UpdateData(FALSE);

   // update the media dims
   UpdateMediaDims();

   return CPropertyPage::OnSetActive();
   }

void CGeneralJobProps::OnSelchangeOrientation() 
   {
   UpdateData(TRUE);
   //Do we need to create a new object?
   if(bIsUsingDefTarg){
      // We're using the default QUEUEDEVICE.
      // We will now need to create a new CDEVICE object so that 
	  // we don't overwrite the defaults.
	  // init with the current target device
	  InternalDevice = *pTargetDevice;
	  // and point to it
	  pTargetDevice = &InternalDevice;
      //Now make the switch
      bIsUsingDefTarg = FALSE;
   }
   pTargetDevice->pDevInfo->Orientation = m_orientation;
   // make sure the queue knows about this
   ReportChange(DEVCHANGE_ORIENTATION);
   }

#if 0
void CGeneralJobProps::OnOrientationOptimize() 
   {
   UpdateData(TRUE);
   //Do we need to create a new object?
   if(bIsUsingDefTarg){
      // We're using the default QUEUEDEVICE.
      // We will now need to create a new CDEVICE object so that 
	  // we don't overwrite the defaults.
	  // init with the current target device
	  InternalDevice = *pTargetDevice;
	  // and point to it
	  pTargetDevice = &InternalDevice;
      //Now make the switch
      bIsUsingDefTarg = FALSE;
   }
   pTargetDevice->pDevInfo->OptimizeOrientation = m_orientation_optimize;
   // make sure the queue knows about this
   ReportChange(DEVCHANGE_ORIENTATION_OPT);
   }
#endif

void CGeneralJobProps::OnUpdateDescription() 
   {
   UpdateData(TRUE);
   //Do we need to create a new object?
   if(bIsUsingDefTarg){
      // We're using the default QUEUEDEVICE.
      // We will now need to create a new CDEVICE object so that 
	  // we don't overwrite the defaults.
	  // init with the current target device
	  InternalDevice = *pTargetDevice;
	  // and point to it
	  pTargetDevice = &InternalDevice;
      //Now make the switch
      bIsUsingDefTarg = FALSE;
   }
   // remove indeterminancy
   //((CButton*)GetDlgItem(IDC_COMPRESS))->SetButtonStyle(BS_AUTOCHECKBOX);
   //pTargetDevice->pDevInfo->UseCompression = m_compress;
   // make sure the queue knows about this
   //ReportChange(DEVCHANGE_DESCRIPTION);

   DEVCHANGE DevChange;   

   // Load structure
   DevChange.ItemChanged = DEVCHANGE_DESCRIPTION;
   DevChange.csText = m_description;   
   //Send to report to all concerned parties
   if(pReportWnd)
	  {
      //Now send report to window
      if(!DoingDefaultProps)
		 pReportWnd->SendMessage(wm_DevModeChange, PREFS_CHANGE_DESCRIPTION, 
		 (LPARAM)&DevChange);
	  }
   }

void CGeneralJobProps::OnSelchangeAaoversample() 
   {
   // get data
   UpdateData(TRUE);
   //Do we need to create a new object?
   if(bIsUsingDefTarg){
      // We're using the default QUEUEDEVICE.
      // We will now need to create a new CDEVICE object so that 
	  // we don't overwrite the defaults.
	  // init with the current target device
	  InternalDevice = *pTargetDevice;
	  // and point to it
	  pTargetDevice = &InternalDevice;
      //Now make the switch
      bIsUsingDefTarg = FALSE;
	  }
   
   pTargetDevice->pDevInfo->AliasLevel = m_aaoversample + 2;

   // make sure the queue knows about this
   ReportChange(DEVCHANGE_ALIASLEVEL);
   }

void CGeneralJobProps::ResetPaperList() 
   {
   CComboBox* pCB;
   int i;
   MEDIA Media;   

   //if (!DeviceIsSet) return;

   if (!pTargetDevice) return;
   // reset the media source list
   pCB = (CComboBox*)GetDlgItem(IDC_MEDIA_SIZE);
   pCB->ResetContent();   					 

   // check for valid device info
   if (pTargetDevice->pDevInfo)
	  {
	  pTargetDevice->pDevInfo->TotalMedia = pTargetDevice->UpdateMediaList();
	  // insert the sources into the list
      for (i = 0; i < pTargetDevice->pDevInfo->TotalMedia; i++)
		 {	  
	     pTargetDevice->GetMedia(i, &Media);	  	     
	     pCB->InsertString(-1, Media.Name);
		 }

#if 0
	  if (pTargetDevice->pDevInfo->bExtra & DEVICE_EXTRA_CANDOPACKAGES)
		 {
		 if (bPackagePlusInstalled)
		    pCB->InsertString(-1, "<< New Package >>");
		 }
#endif

      // set current
      pCB->SetCurSel(min(m_mediasize, pTargetDevice->pDevInfo->TotalMedia - 1));	  
	  }
   }

BOOL CGeneralJobProps::IsPackagePlusInstalled()
   {
   HKEY hKey;
	BOOL bRet = TRUE;
	CString csAppPath;
    CPhotogizeUtils utils;

   DWORD lDataSize = _MAX_PATH;   
   DWORD dwType = REG_SZ;
   char *pEntry = "App";

//   utils.GetAppPath(csAppPath);
//   csPackagePlusExe = csAppPath + "PackagePlus.Exe";

   csPackagePlusExe = "";

   if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Graphx\\PackagePlus", 0, 
	  KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
      {  	 	  
      // get value
	  if (RegQueryValueEx(hKey, pEntry, NULL, &dwType, 
		 (LPBYTE)(csPackagePlusExe.GetBuffer(lDataSize)), &lDataSize) == ERROR_SUCCESS)
		 {		 
		 bRet = TRUE;
		 }
	  csPackagePlusExe.ReleaseBuffer();	  
	  RegCloseKey(hKey);
	  }

   return bRet;
   }

BOOL CGeneralJobProps::PreTranslateMessage(MSG* pMsg) 
   {
   // route this to the tool tip engine
   if (!m_bNotCreated) 
	  ToolTipCtrl.RelayEvent(pMsg);   
	
   return CPropertyPage::PreTranslateMessage(pMsg);
   }

void CGeneralJobProps::OnEditMedia() 
   {   
   int iType = pTargetDevice->GetCurrentMediaType();

   if (pTargetDevice->pDevInfo->bExtra & DEVICE_EXTRA_CANDOPACKAGES && 
	  bPackagePlusInstalled && iType == MEDIA_TYPE_PACKAGE)
	  EditPackageMedia();
   else if (pTargetDevice->pDevInfo->bExtra & DEVICE_EXTRA_CANDOCUSTOM && 
	  iType == MEDIA_TYPE_CUSTOM)	  
	  EditCustomMedia(FALSE);	  
   else if (pTargetDevice->pDevInfo->bExtra & DEVICE_EXTRA_CANDOCUSTOMOFFSET && 
	  iType == MEDIA_TYPE_CUSTOM)	  
	  EditCustomMedia(TRUE);	  
   }

void CGeneralJobProps::EditPackageMedia() 
   {   
   CProfile Profile;

   CString csTempApp;

   MEDIA Media;

   UpdateData(TRUE);

   // add dev name to the app string
   csTempApp = csPackagePlusExe;
   csTempApp += " /d";
   csTempApp += pTargetDevice->Name;
   csTempApp += " /p";

   pTargetDevice->GetCurrentMedia(Media);
   if (Media.Name[0] == '?')
   {
	   csTempApp += Media.Name + 1;
   }
   else
   {
	   csTempApp += Media.Name;
   }
/*
   CString csCmdLine;

   csCmdLine += " /m2";
   csCmdLine += " /d";
   csCmdLine += pTargetDevice->Name;
   csCmdLine += " /p";

   pTargetDevice->GetCurrentMedia(Media);
   if (Media.Name[0] == '?')
   {
	   csCmdLine += Media.Name + 1;
   }
   else
   {
	   csCmdLine += Media.Name;
   }
   csCmdLine += " /iRasterGize";

   csTempApp = Profile.GetStr("Server", "AppDir", "" );
   csTempApp += "\\PACKAGEPLUS.EXE";

   csTempApp += csCmdLine;
*/

   // start packageplus
   STARTUPINFO StartupInfo;
   PROCESS_INFORMATION ProcessInfo;
   HWND hPackagePlusWnd = NULL;

   // NULL the StartupInfo struct
   memset(&StartupInfo, 0, sizeof(STARTUPINFO));
   // the size
   StartupInfo.cb = sizeof(STARTUPINFO);
   // show the window
   StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
   StartupInfo.wShowWindow = SW_SHOW;
   // start PackagePlus
   if (CreateProcess(NULL, (char*)(const char*)csTempApp, NULL, NULL, FALSE, 
      NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &ProcessInfo))
	  {
	  // look for PackagePlus for 20 seconds
	  WaitForInputIdle(ProcessInfo.hProcess, 20000);
	  // now look for the DIALOG that says PackagePlus
	  if (hPackagePlusWnd = ::FindWindow("#32770", "PackagePlus"))
		 {		 
		 // put on top
		 ::BringWindowToTop(hPackagePlusWnd);
		 } 
	  else
		 {
		 hPackagePlusWnd = NULL;
		 }
	  // close the handles				
	  CloseHandle(ProcessInfo.hThread);
	  CloseHandle(ProcessInfo.hProcess);	  
	  }
   }

void CGeneralJobProps::EditCustomMedia(BOOL bDoOffsets)
   {
   CCustomPage cp;
   CProfile Profile;

   //Do we need to create a new object?
   if(bIsUsingDefTarg){
      // We're using the default QUEUEDEVICE.
      // We will now need to create a new CDEVICE object so that 
	  // we don't overwrite the defaults.
	  // init with the current target device
	  InternalDevice = *pTargetDevice;
	  // and point to it
	  pTargetDevice = &InternalDevice;
      //Now make the switch
      bIsUsingDefTarg = FALSE;
   }

   
   // if landscape, flip these
   if (pTargetDevice->pDevInfo->Orientation == DEV_ORIENT_LANDSCAPE)
	  {
      cp.m_width.Format("%.3f", (float)pTargetDevice->pDevInfo->CustomPage_Height / 1000.F);  
      cp.m_height.Format("%.3f", (float)pTargetDevice->pDevInfo->CustomPage_Width / 1000.F);  
      cp.m_xoffset.Format("%.3f", (float)pTargetDevice->pDevInfo->CustomPage_YOffset / 1000.F);  
      cp.m_yoffset.Format("%.3f", (float)pTargetDevice->pDevInfo->CustomPage_XOffset / 1000.F);     	  
	  cp.m_minyoffset = (float)pTargetDevice->pDevInfo->CustomPage_Min_XOffset / 1000.F;
	  cp.m_minxoffset = (float)pTargetDevice->pDevInfo->CustomPage_Min_YOffset / 1000.F;
      cp.m_inch = Profile.GetInt(IDS_DEVICES, IDS_CUSTOMPAGE_UNITS, 0);
	  }
   else
	  {
      cp.m_width.Format("%.3f", (float)pTargetDevice->pDevInfo->CustomPage_Width / 1000.F);  
      cp.m_height.Format("%.3f", (float)pTargetDevice->pDevInfo->CustomPage_Height / 1000.F);  
      cp.m_xoffset.Format("%.3f", (float)pTargetDevice->pDevInfo->CustomPage_XOffset / 1000.F);  
      cp.m_yoffset.Format("%.3f", (float)pTargetDevice->pDevInfo->CustomPage_YOffset / 1000.F);     	  
	  cp.m_minxoffset = (float)pTargetDevice->pDevInfo->CustomPage_Min_XOffset / 1000.F;
	  cp.m_minyoffset = (float)pTargetDevice->pDevInfo->CustomPage_Min_YOffset / 1000.F;
      cp.m_inch = Profile.GetInt(IDS_DEVICES, IDS_CUSTOMPAGE_UNITS, 0);
	  }
   cp.m_bDoOffsets = bDoOffsets;
   cp.m_mediawidth = (float)pTargetDevice->pDevInfo->CustomPage_MediaWidth / 1000.F;

   // in millimeters
   cp.m_minxoffsetmm = cp.m_minxoffset * 25.4F;
   cp.m_minyoffsetmm = cp.m_minyoffset * 25.4F;
   cp.m_mediawidthmm = cp.m_mediawidth * 25.4F;

   /*
   int CustomPage_Width;       // width in inches x 1000
   int CustomPage_Height;      // height in inches x 1000
   int CustomPage_XOffset;     // xoffset in inches x 1000
   int CustomPage_YOffset;     // yoffset in inches x 1000
   int CustomPage_Min_XOffset; // min xoffset in inches x 1000
   int CustomPage_Min_YOffset; // min yoffset in inches x 1000
   int CustomPage_MediaWidth;  // media width in inches x 1000
	 */
	  
   if (cp.DoModal() == IDOK)
	  {
	  // write the units
	  Profile.WriteInt(IDS_DEVICES, IDS_CUSTOMPAGE_UNITS, cp.m_inch);

	  // get stuff
	  // note that we always need to store the page as a portrait so flip them if
	  // needed
	  if (cp.m_width > cp.m_height)
		 {
		 pTargetDevice->pDevInfo->CustomPage_Height = (int)(atof(cp.m_width) * 1000.F);
	     pTargetDevice->pDevInfo->CustomPage_Width = (int)(atof(cp.m_height) * 1000.F);
	     pTargetDevice->pDevInfo->CustomPage_YOffset = (int)(atof(cp.m_xoffset) * 1000.F);
	     pTargetDevice->pDevInfo->CustomPage_XOffset = (int)(atof(cp.m_yoffset) * 1000.F);		 
		 pTargetDevice->pDevInfo->Orientation = DEV_ORIENT_LANDSCAPE;
		 }
	  else
		 {
		 pTargetDevice->pDevInfo->CustomPage_Width = (int)(atof(cp.m_width) * 1000.F);
	     pTargetDevice->pDevInfo->CustomPage_Height = (int)(atof(cp.m_height) * 1000.F);
	     pTargetDevice->pDevInfo->CustomPage_XOffset = (int)(atof(cp.m_xoffset) * 1000.F);
	     pTargetDevice->pDevInfo->CustomPage_YOffset = (int)(atof(cp.m_yoffset) * 1000.F);		 
		 pTargetDevice->pDevInfo->Orientation = DEV_ORIENT_PORTRAIT;
		 }

	  // report the change
	  ReportChange(DEVCHANGE_CUSTOMPAGE);

      // bring up possible alert   
      pTargetDevice->AlertModeChange(DEVMODE_CHANGE_MEDIA);

      // update the orientation
	  m_orientation = pTargetDevice->pDevInfo->Orientation;
      //UpdateData(FALSE);

      // update the media dims
      UpdateMediaDims();
	  }
   }

BOOL CGeneralJobProps::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
   {
   // here's where we get a new media name
   int len;
   char pNewMedia[1024];   
   int idx;
   CComboBox* cbm = (CComboBox*)GetDlgItem(IDC_MEDIA_SIZE);

   // get length
   len = pCopyDataStruct->cbData; 
   
   // copy it
   memcpy(pNewMedia, pCopyDataStruct->lpData, len);
   // add a terminating char in case
   pNewMedia[len] = '\0';

   idx = CB_ERR;
   // set new media
   if ((idx = cbm->FindStringExact(-1, pNewMedia)) != CB_ERR)
	  {
	  m_mediasize = idx;
	  }	
   else
	  {
      // try this   
      pNewMedia[0] = '?';
      memcpy(pNewMedia + 1, pCopyDataStruct->lpData, len);
      // add a terminating char in case
      pNewMedia[len + 1] = '\0';
      // set new media
      if ((idx = cbm->FindStringExact(-1, pNewMedia)) != CB_ERR)
		 {
	     m_mediasize = idx;
		 }	
	  }

   if (idx != CB_ERR)
	  {   
      // update the data in the dialog
      UpdateData(FALSE);

      // update the media dims
      UpdateMediaDims();

      // special case for copies to set an indeterminate state
      // in the case of multiple selections
      if (m_copies == -1) SetDlgItemText(IDC_COPIES_EDIT, " ");	  

      // bring up possible alert   
      pTargetDevice->AlertModeChange(DEVMODE_CHANGE_MEDIA);
	  }
   return CPropertyPage::OnCopyData(pWnd, pCopyDataStruct);
   }

BOOL CGeneralJobProps::EnableMediaEdit()
   {
   BOOL bEnableEdit = FALSE;

   if (pTargetDevice) 
	  {
      // get the media type
      int iType = pTargetDevice->GetCurrentMediaType();

      if ((pTargetDevice->pDevInfo->bExtra & DEVICE_EXTRA_CANDOPACKAGES) && bPackagePlusInstalled)
		 {	   	     
	     if (iType == MEDIA_TYPE_PACKAGE)
			{
		    // enable for packages
		    bEnableEdit = TRUE;
			}
		 }	 
	  else if (pTargetDevice->pDevInfo->bExtra & DEVICE_EXTRA_CANDOCUSTOM)
		 {
		 if (iType == MEDIA_TYPE_CUSTOM)
			{
	        // enable for custom pages
	        bEnableEdit = TRUE;	  
			}
		 }  	  
	  else if (pTargetDevice->pDevInfo->bExtra & DEVICE_EXTRA_CANDOCUSTOMOFFSET)
		 {
		 if (iType == MEDIA_TYPE_CUSTOM)
			{
	        // enable for custom pages
	        bEnableEdit = TRUE;	  
			}
		 }  	  
	  }  
   return bEnableEdit;
   }

void CGeneralJobProps::OnAatype() 
   {
   // get data
   UpdateData(TRUE);

   //Do we need to create a new object?
   if(bIsUsingDefTarg){
      // We're using the default QUEUEDEVICE.
      // We will now need to create a new CDEVICE object so that 
	  // we don't overwrite the defaults.
	  // init with the current target device
	  InternalDevice = *pTargetDevice;
	  // and point to it
	  pTargetDevice = &InternalDevice;
      //Now make the switch
      bIsUsingDefTarg = FALSE;
   }
   // anti-aliasing
   if (m_canaalias)
      {
	  GetDlgItem(IDC_AATYPE)->EnableWindow(TRUE);   
      // no anti-alias style if aa is off
      GetDlgItem(IDC_AAOVERSAMPLE)->EnableWindow(m_aatype);
	  GetDlgItem(IDC_AAOVERSAMPLE_PRMPT)->EnableWindow(m_aatype);
      GetDlgItem(IDC_SCALE_BITMAP)->EnableWindow(!m_aatype && m_canscale_bitmap);
	  }
   else
      {
      GetDlgItem(IDC_AATYPE)->EnableWindow(FALSE);   
      GetDlgItem(IDC_AAOVERSAMPLE)->EnableWindow(FALSE);   
	  GetDlgItem(IDC_AAOVERSAMPLE_PRMPT)->EnableWindow(FALSE);
	  }   

   pTargetDevice->pDevInfo->CanDoAntiAliasing = m_canaalias;
   pTargetDevice->pDevInfo->AliasStyle = (m_aatype ? ANTIALIAS_STYLE_BOX : ANTIALIAS_STYLE_NONE);

   //TRACE("CGeneralJobProps::OnAatype, m_aatype = %d\r\n", m_aatype);
   //TRACE("CGeneralJobProps::OnAatype, pTargetDevice->pDevInfo->AliasStyle = %d\r\n", pTargetDevice->pDevInfo->AliasStyle);

   // make sure the queue knows about this
   ReportChange(DEVCHANGE_ALIASSTYLE);

   // must do this also!!
   pTargetDevice->pDevInfo->AliasLevel = m_aaoversample + 2;

   // make sure the queue knows about this
   ReportChange(DEVCHANGE_ALIASLEVEL);
   }


void CGeneralJobProps::OnUpdatePrangeEdit() 
   {
   OnPrange();
   }

void CGeneralJobProps::OnPrange() 
   {
   UpdateData(TRUE);
   //Do we need to create a new object?
   if(bIsUsingDefTarg){
      // We're using the default QUEUEDEVICE.
      // We will now need to create a new CDEVICE object so that 
	  // we don't overwrite the defaults.
	  // init with the current target device
	  InternalDevice = *pTargetDevice;
	  // and point to it
	  pTargetDevice = &InternalDevice;
      //Now make the switch
      bIsUsingDefTarg = FALSE;
   }   

   GetDlgItem(IDC_PRANGE_EDIT)->EnableWindow(m_prange);      

   DEVCHANGE DevChange;   
   // Load structure
   DevChange.ItemChanged = DEVCHANGE_PRANGE;   
   DevChange.csText = m_prange_edit;
   DevChange.arg1 = m_prange;
   //Send to report to all concerned parties
   if(pReportWnd)
	  {
      //Now send report to window
      if(!DoingDefaultProps)
		 pReportWnd->SendMessage(wm_DevModeChange, PREFS_CHANGE_PRANGE, 
		 (LPARAM)&DevChange);
	  }
   }

void CGeneralJobProps::OnSelchangeBitmapsampling() 
   {
   UpdateData(TRUE);
   //Do we need to create a new object?
   if(bIsUsingDefTarg){
      // We're using the default QUEUEDEVICE.
      // We will now need to create a new CDEVICE object so that 
	  // we don't overwrite the defaults.
	  // init with the current target device
	  InternalDevice = *pTargetDevice;
	  // and point to it
	  pTargetDevice = &InternalDevice;
      //Now make the switch
      bIsUsingDefTarg = FALSE;
   }
   pTargetDevice->pDevInfo->BitmapSample = m_bitmapsampling;
   // make sure the queue knows about this
   ReportChange(DEVCHANGE_BITMAPSAMPLING);	
   }
