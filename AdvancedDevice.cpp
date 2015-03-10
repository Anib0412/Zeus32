/////////////////////////////////////////////////////////////////////////////
// AdvancedDevice.cpp
//                                                                        
// advanced device settings for RasterPlus95
// Copyright (C) 1996-98 Graphx, Inc.
//
// mods:
//
// 7-11-96  Build 40 - Minimum mem allowed in Custom was always 4M this should 
//					      have been a min of 4M and the max image mem needed
// 7-31-96  Build 41 - call SetBufferToDisk() to update buffer stuff in 
//                        OnDeltaposMemUsedSpin
// 10-15-96 Build 45 - added special NT memory handling
// 1-6-98   Version 2.0.1 - auto memory check is now the same under NT as 95
//                          because we fixed the nt mem problem
// 9-11-98  Version 2.0.5 - new option: always buffer to disk (for novajet)
// 10-24-02 Version 5.1.0 - Remove limitation on maximum memory - Move limitation to individual drivers
//

#include "stdafx.h"
#include "zeus.h"
#include "AdvancedDevice.h"
#include "SysMem.h"
#include "control.hm"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define AATYPE_NONE			0

// the control help-map
static DWORD dwHelpMap[] = 
   {
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
   IDC_ADVANCED_RESOURCES, HIDC_ADVANCED_ADVANCED_RESOURCES,
   IDC_DEFAULT, HIDC_ADVANCED_DEFAULT,
   IDOK, HIDC_OUROK,
   IDCANCEL, HIDC_OURCANCEL,   
   0,0
   };

/////////////////////////////////////////////////////////////////////////////
// CAdvancedDevice dialog


CAdvancedDevice::CAdvancedDevice(CWnd* pParent /*=NULL*/)
	: CDialog(CAdvancedDevice::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdvancedDevice)	
	m_buffertodisk = FALSE;
	m_compress = FALSE;
	m_virtual_memory = _T("");
	m_memalloc_style = -1;
	m_memused_edit = _T("");
	m_aatype = -1;
	m_aaoversample = -1;
	m_imagememory = _T("");
	m_avail_physical = _T("");
	m_compress_bitmap = FALSE;
	m_scale_bitmap = FALSE;
	//}}AFX_DATA_INIT
}


void CAdvancedDevice::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvancedDevice)	
	DDX_Check(pDX, IDC_BUFFERTODISK, m_buffertodisk);
	DDX_Check(pDX, IDC_COMPRESS, m_compress);
	DDX_Text(pDX, IDC_VIRTUALMEMORY, m_virtual_memory);
	DDX_CBIndex(pDX, IDC_MEM_ALLOC, m_memalloc_style);
	DDX_Text(pDX, IDC_MEM_USED_EDIT, m_memused_edit);
	DDX_CBIndex(pDX, IDC_AATYPE, m_aatype);
	DDX_CBIndex(pDX, IDC_AAOVERSAMPLE, m_aaoversample);
	DDX_Text(pDX, IDC_IMAGEMEMORY, m_imagememory);
	DDX_Text(pDX, IDC_AVAIL_PHYSICAL, m_avail_physical);
	DDX_Check(pDX, IDC_COMPRESS_BITMAP, m_compress_bitmap);
	DDX_Check(pDX, IDC_SCALE_BITMAP, m_scale_bitmap);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvancedDevice, CDialog)
	//{{AFX_MSG_MAP(CAdvancedDevice)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MEM_USED_SPIN, OnDeltaposMemUsedSpin)
	ON_CBN_SELCHANGE(IDC_MEM_ALLOC, OnSelchangeMemAlloc)
	ON_CBN_SELCHANGE(IDC_AATYPE, OnSelchangeAatype)
	ON_EN_CHANGE(IDC_MEM_USED_EDIT, OnChangeMemUsedEdit)
	ON_WM_CONTEXTMENU()	
	ON_WM_HELPINFO()	
	ON_CBN_SELCHANGE(IDC_AAOVERSAMPLE, OnSelchangeAaoversample)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvancedDevice message handlers

BOOL CAdvancedDevice::OnInitDialog() 
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

   m_avail_physical = sm.csFreePhysicalMemory;
   m_virtual_memory = sm.csFreeVirtualMemory;   
   m_maxmem = sm.FreeVirtualMemory;     
   AvailPhysicalMemory = sm.FreePhysicalMemory;
   InstalledMemory = sm.InstalledMemory;

   OriginalMem = m_imagememory_num;
   OriginalAliasStyle = m_aatype;
   OriginalAliasLevel = m_aaoversample + 2;

   // convert and put memory settings in dialog
   SetMem();

   // don't allow device scaling if aa is checked
   GetDlgItem(IDC_SCALE_BITMAP)->EnableWindow(m_aatype == AATYPE_NONE && m_canscale_bitmap);   	  

   // compression?
   GetDlgItem(IDC_COMPRESS)->EnableWindow(m_cancompress);   
   GetDlgItem(IDC_COMPRESS_BITMAP)->EnableWindow(m_cancompress);

   // anti-aliasing
   if (m_canaalias)
      {
	  GetDlgItem(IDC_AATYPE)->EnableWindow(TRUE);   
      // no anti-alias style if aa is off
      GetDlgItem(IDC_AAOVERSAMPLE)->EnableWindow(!(m_aatype == AATYPE_NONE));
	  GetDlgItem(IDC_AAOVERSAMPLE_PRMPT)->EnableWindow(!(m_aatype == AATYPE_NONE));	  
	  }
   else
      {
      GetDlgItem(IDC_AATYPE)->EnableWindow(FALSE);   
      GetDlgItem(IDC_AAOVERSAMPLE)->EnableWindow(FALSE);   
	  GetDlgItem(IDC_AAOVERSAMPLE_PRMPT)->EnableWindow(FALSE);
	  }   
   
   CDialog::OnInitDialog();
	
   return TRUE; 
   }

void CAdvancedDevice::OnDefault() 
   {
   // defaults
   if (AfxMessageBox(IDS_RESET_TO_DEFAULTS, MB_YESNO) == IDYES)
      {	  
      m_buffertodisk = m_def_buffertodisk;
      m_compress = m_def_compress;
	  m_compress_bitmap = m_def_compress_bitmap;
	  m_scale_bitmap = m_def_scale_bitmap;
      m_memalloc_style = m_def_memalloc_style;	  
      m_aatype = m_def_aatype;
      m_aaoversample = m_def_aaoversample;
	  SetMem();
      // anti-aliasing
      if (m_canaalias)
         {
	     GetDlgItem(IDC_AATYPE)->EnableWindow(TRUE);   
         // no anti-alias style if aa is off
         GetDlgItem(IDC_AAOVERSAMPLE)->EnableWindow(!(m_aatype == AATYPE_NONE));
		 GetDlgItem(IDC_AAOVERSAMPLE_PRMPT)->EnableWindow(!(m_aatype == AATYPE_NONE));
	     }
      else
	     {
         GetDlgItem(IDC_AATYPE)->EnableWindow(FALSE);   
         GetDlgItem(IDC_AAOVERSAMPLE)->EnableWindow(FALSE);   
		 GetDlgItem(IDC_AAOVERSAMPLE_PRMPT)->EnableWindow(FALSE);   
	     }   
      // don't allow device scaling if aa is checked
	  GetDlgItem(IDC_SCALE_BITMAP)->EnableWindow(m_aatype == AATYPE_NONE && m_canscale_bitmap);   
	  }	
   }

void CAdvancedDevice::OnDeltaposMemUsedSpin(NMHDR* pNMHDR, LRESULT* pResult) 
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
   m_memused = min(m_memused, m_imagememory_num);

   nm.ConvertToComma(m_memused, m_memused_edit);

   // put back in
   UpdateData(FALSE);    

   // enable/disable buffer to disk window
   SetBufferToDisk();   
   
   *pResult = 0;
   }

void CAdvancedDevice::OnSelchangeMemAlloc() 
   {
   // get data
   UpdateData(TRUE);

   SetMem();

   // enable/disable buffer to disk window
   SetBufferToDisk();
   }

void CAdvancedDevice::SetMem()
   {
   // set memory based on installed mem
   switch(m_memalloc_style)
	  {
	  case RIPMEM_ALLOC_WORKSTATION:
		   m_memused = (int)((float)InstalledMemory * .25F + .5F);
		   break;
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
	  case RIPMEM_ALLOC_CUSTOM:
		   // already set
	       break;
	  }			  
   
   // convert image memory
   nm.ConvertToComma(m_imagememory_num, m_imagememory);

   // check bounds   
   m_memused = max(min(m_memused, m_maxmem), DEVICE_MEMUSED_MIN);      
//   m_memused = min(DEVICE_MEMUSED_MAX, m_memused);
   // no more than what device needs
   m_memused = min(m_memused, m_imagememory_num);
   // convert to a string
   nm.ConvertToComma(m_memused, m_memused_edit);

   // put it in
   UpdateData(FALSE);
   // enable/disable custom fields
   GetDlgItem(IDC_MEM_USED_SPIN)->EnableWindow(m_memalloc_style == RIPMEM_ALLOC_CUSTOM);
   GetDlgItem(IDC_MEM_USED_EDIT)->EnableWindow(m_memalloc_style == RIPMEM_ALLOC_CUSTOM);
      
   // enable/disable buffer to disk window
   SetBufferToDisk();

//   TRACE("CAdvancedDevice::SetMem, m_imagememory_num = %d\n", m_imagememory_num);
//   TRACE("CAdvancedDevice::SetMem, m_memused = %d\n", m_memused);
   }

void CAdvancedDevice::OnChangeMemUsedEdit() 
   {
   UpdateData(TRUE);
   
   m_memused = atoi((const char*)m_memused_edit);

   // only allow buffer to disk if there is m_memused < m_imagememory_num
   SetBufferToDisk();
   }

void CAdvancedDevice::SetBufferToDisk()
   {
   BOOL setting;

   if (m_EnableBufferToDisk == DEV_BUFFERTODISK_DISABLE)
      setting = FALSE;
   else if (m_EnableBufferToDisk == DEV_BUFFERTODISK_ALWAYSALLOW)
      setting = TRUE;
   else									
      {
	  // only allow buffer to disk if 
	  // m_memused < m_imagememory_num
      if (m_memused >= m_imagememory_num)
	     setting = FALSE;
	  else
	     setting = TRUE;
	  }
   GetDlgItem(IDC_BUFFERTODISK)->EnableWindow(setting);
   }

BOOL CAdvancedDevice::OnHelpInfo(HELPINFO* pHelpInfo) 
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
 
void CAdvancedDevice::OnContextMenu(CWnd* pWnd, CPoint point) 
   {   
   // call context help
   ::WinHelp(pWnd->m_hWnd, AfxGetApp()->m_pszHelpFilePath, 
      HELP_CONTEXTMENU,  (DWORD)(LPVOID)dwHelpMap);
   }

void CAdvancedDevice::OnSelchangeAaoversample() 
   {
   // get data
   UpdateData(TRUE);

   if (OriginalAliasStyle == AATYPE_NONE && m_aatype != AATYPE_NONE)      
	  m_imagememory_num = OriginalMem * (m_aaoversample + 2) * (m_aaoversample + 2);
   else if (OriginalAliasStyle != AATYPE_NONE && m_aatype == AATYPE_NONE)
      m_imagememory_num = OriginalMem / (OriginalAliasLevel * OriginalAliasLevel);
   else if (OriginalAliasStyle != AATYPE_NONE && m_aatype != AATYPE_NONE)
      m_imagememory_num = OriginalMem / (OriginalAliasLevel * OriginalAliasLevel) * 
	     (m_aaoversample + 2) * (m_aaoversample + 2);

   SetMem();
   }

void CAdvancedDevice::OnSelchangeAatype() 
   {
   // get data
   UpdateData(TRUE);

   // anti-aliasing
   if (m_canaalias)
      {
	  GetDlgItem(IDC_AATYPE)->EnableWindow(TRUE);   
      // no anti-alias style if aa is off
      GetDlgItem(IDC_AAOVERSAMPLE)->EnableWindow(!(m_aatype == AATYPE_NONE));
	  GetDlgItem(IDC_AAOVERSAMPLE_PRMPT)->EnableWindow(!(m_aatype == AATYPE_NONE));
	  }
   else
      {
      GetDlgItem(IDC_AATYPE)->EnableWindow(FALSE);   
      GetDlgItem(IDC_AAOVERSAMPLE)->EnableWindow(FALSE);   
	  GetDlgItem(IDC_AAOVERSAMPLE_PRMPT)->EnableWindow(FALSE);   
	  }   

   if (OriginalAliasStyle == AATYPE_NONE && m_aatype != AATYPE_NONE)      
	  m_imagememory_num = OriginalMem * (m_aaoversample + 2) * (m_aaoversample + 2);
   else if (OriginalAliasStyle == AATYPE_NONE && m_aatype == AATYPE_NONE)      
	  m_imagememory_num = OriginalMem;
   else if (OriginalAliasStyle != AATYPE_NONE && m_aatype == AATYPE_NONE)
      m_imagememory_num = OriginalMem / (OriginalAliasLevel * OriginalAliasLevel);
   else if (OriginalAliasStyle != AATYPE_NONE && m_aatype != AATYPE_NONE)
      m_imagememory_num = OriginalMem / (OriginalAliasLevel * OriginalAliasLevel) * 
	     (m_aaoversample + 2) * (m_aaoversample + 2);

   SetMem();
   }

void CAdvancedDevice::OnOK() 
   {
   UpdateData(TRUE);   
   // check bounds   
   m_memused = max(min(m_memused, m_maxmem), DEVICE_MEMUSED_MIN);      
//   m_memused = min(DEVICE_MEMUSED_MAX, m_memused);
   // no more than what device needs
   m_memused = min(m_memused, m_imagememory_num);   
   CDialog::OnOK();
   }
