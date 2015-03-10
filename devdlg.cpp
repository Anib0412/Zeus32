/////////////////////////////////////////////////////////////////////////////
// DEVDLG.CPP
//                                                                        
// device dialog for RasterPlus
// Copyright (C) 1996-2000 Graphx, Inc.
//
// mods:
//
// 7-11-96  Build 40 - Removed bounds checking for Advanced mem settings (we do
//					      this in Adv Dlg
// 7-30-96  Build 41 - Change resolution didn't recompute mem - fixed
// 8-08-96  Build 41 - enable/disable combo titles when combo itself is 
//                        enable/disabled
// 1-18-96  Build 47 - changed params for AlertModeChange so we can check availability
//                     of orientation and such
// 8-05-97  Build 50 - remove ReportDevChange
// 8-27-97  Build 51 - reset papers after options in case custom papers are chosen
// 8-28-97  Build 51 - reposition AlertModeChange
// 9-29-97  Build 51 - added page dims display
// 10-17-97 Build 52 - add DEVMODE_CHANGE_MEDIA_PRMPT option to AlertModeChange
// 5-29-98  Version 2.0.4 - added more error checking for missing devices
// 9-28-98  Version 2.0.5 - Changed OnOurOk PostMessage to SendMessage for VC6.0 compat
// 11-24-98 Version 2.0.5 - call SetPaper from OnSelchangeConnection() to reload papers
//                             on port change
// 12-04-98 Version 2.0.5 - added UpdateMediaList
// 2-20-00  Version 3.0.3 - added UpdateConnectList
 
#include "stdafx.h"
#include "zeus.h"
#include "devdlg.h"
#include "inidefs.h"
#include <string.h>
#include "prefsmsg.h"
#include "mainfrm.h"
#include "AdvancedDevice.h"
#include "control.hm"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT NEAR wm_DevModeChange = RegisterWindowMessage( ID_DEVMODE_CHANGE );

// the control help-map
static DWORD dwHelpMap[] = 
   {
   IDC_DEVICE_PRMPT, HIDC_DEVICE_DEVICE,
   IDC_DEVICE, HIDC_DEVICE_DEVICE,
   IDC_CONECTION_PRMPT, HIDC_DEVICE_CONNECTION,
   IDC_CONNECTION, HIDC_DEVICE_CONNECTION,
   IDC_MEDIA_SOURCE_PRMPT, HIDC_DEVICE_MEDIA_SOURCE,
   IDC_MEDIA_SOURCE, HIDC_DEVICE_MEDIA_SOURCE,
   IDC_MEDIA_SIZE_PRMPT, HIDC_DEVICE_MEDIA_SIZE,
   IDC_MEDIA_SIZE, HIDC_DEVICE_MEDIA_SIZE,
   IDC_RESOLUTION_PRMPT, HIDC_DEVICE_RESOLUTION,
   IDC_RESOLUTION, HIDC_DEVICE_RESOLUTION,
   IDC_ORIENTATION_PRMPT, HIDC_DEVICE_ORIENTATION_LIST,
   IDC_ORIENTATION_LIST, HIDC_DEVICE_ORIENTATION_LIST,
   IDC_COPIES_PRMPT, HIDC_DEVICE_COPIES,
   IDC_OPTIONS, HIDC_DEVICE_OPTIONS,
   IDC_ADVANCED, HIDC_DEVICE_ADVANCED,
   IDC_RESET, HIDC_DEVICE_RESET,
   IDOUROK, HIDC_OUROK,
   IDOURCANCEL, HIDC_OURCANCEL,
   ID_CLOSE, HIDC_CLOSE,
   0,0
   };

/////////////////////////////////////////////////////////////////////////////
// CDialogDevSetup dialog

CDialogDevSetup::CDialogDevSetup() : CPropertyPage(CDialogDevSetup::IDD)
{
	//{{AFX_DATA_INIT(CDialogDevSetup)	
	m_device = -1;
	m_mediasize = -1;
	m_mediasource = -1;
	m_copies = 0;
	m_resolution = -1;
	m_connection = -1;
	m_orientation = -1;
	m_pagedims = _T("");
	//}}AFX_DATA_INIT
	// make the default device	
    
    // set this flag so we don't get stray UpdateData calls
    IsDlgInit = FALSE;	
	DelayDeviceFlag = FALSE;	
}

CDialogDevSetup::~CDialogDevSetup()
   {                     
   }

void CDialogDevSetup::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDevSetup)	
	DDX_CBIndex(pDX, IDC_DEVICE, m_device);
	DDX_CBIndex(pDX, IDC_MEDIA_SIZE, m_mediasize);
	DDX_CBIndex(pDX, IDC_MEDIA_SOURCE, m_mediasource);
	DDX_Text(pDX, IDC_COPIES, m_copies);	
	DDX_CBIndex(pDX, IDC_RESOLUTION, m_resolution);
	DDX_CBIndex(pDX, IDC_CONNECTION, m_connection);
	DDX_CBIndex(pDX, IDC_ORIENTATION_LIST, m_orientation);
	DDX_Text(pDX, IDC_PAGEDIMS, m_pagedims);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDialogDevSetup, CDialog)
	//{{AFX_MSG_MAP(CDialogDevSetup)
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_BN_CLICKED(IDOUROK, OnOurOK)
	ON_BN_CLICKED(IDOURCANCEL, OnOurCancel)
	ON_CBN_SELCHANGE(IDC_DEVICE, OnSelchangeDevice)
	ON_CBN_SELCHANGE(IDC_MEDIA_SIZE, OnSelchangeMediaSize)	
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_MEDIA_SOURCE, OnSelchangeMediaSource)
	ON_BN_CLICKED(ID_CLOSE, OnClose)
	ON_EN_CHANGE(IDC_COPIES, OnChangeCopies)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_CBN_SELCHANGE(IDC_CONNECTION, OnSelchangeConnection)
	ON_CBN_SELCHANGE(IDC_RESOLUTION, OnSelchangeResolution)
	ON_BN_CLICKED(IDC_ADVANCED, OnAdvanced)
	ON_CBN_SELCHANGE(IDC_ORIENTATION_LIST, OnSelchangeOrientationList)
	ON_WM_CONTEXTMENU()	
	ON_WM_HELPINFO()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDevSetup message handlers

void CDialogDevSetup::OnOptions()
   {      
   CComboBox* pCB;    
   CDevice* pCD;   
   
   // get all the data
   if (IsDlgInit) UpdateData(TRUE);

   // get the device combo box
   pCB = (CComboBox*)GetDlgItem(IDC_DEVICE);

   if (m_device < 0) return;
	
   // get the device
   pCD = (CDevice*)pCB->GetItemDataPtr(m_device);   

   // write the dialog data to the currently selected device
   UpdateDevice();

   // write the current device info
   //pCD->SetDevInfo(FALSE);
   
   // get devinfo to recompute mem stuff
   //pCD->GetDevInfo();
   
   // and apply it
   ApplyToDevice();

   // call the dialog (pass an HWND for good modal operation)
   pCD->DoDialog(pWnd->GetSafeHwnd());

   // get the current device info
   pCD->GetDevInfo();

   // reset the papers as they may have changed
   SetPaper();
   UpdateData(FALSE);
               
   // write the current settings to the CurrentDevice and
   // broadcast the change
   ApplyToDevice();	   
   }

BOOL CDialogDevSetup::OnInitDialog()
   {
   CComboBox* pCB;
   CString cs;
   CDevice* pCD;
   int idx;   
   CDeviceEntry* cde;   

#ifdef USEKANJI	   
   // temp hacks!
   CString csTemp;   
   ((CComboBox*)GetDlgItem(IDC_ORIENTATION_LIST))->ResetContent();
   csTemp.LoadString(IDS_CBOX_DEVSETUP_PORTRAIT);   
   ((CComboBox*)GetDlgItem(IDC_ORIENTATION_LIST))->AddString(csTemp);
   csTemp.LoadString(IDS_CBOX_DEVSETUP_LANDSCAPE);   
   ((CComboBox*)GetDlgItem(IDC_ORIENTATION_LIST))->AddString(csTemp);
#endif   
   
   // OK to update now
   IsDlgInit = TRUE;

   // get the combo box
   pCB = (CComboBox*)GetDlgItem(IDC_DEVICE);   

   // get all devices   
   cdl.LoadAll();

   // insert into list
   for (idx = 0; idx < cdl.GetSize(); idx++)
      {
	  // get entry
	  cde = (CDeviceEntry*)cdl.GetAt(idx);
	  // insert in list
	  //pCB->InsertString(-1, cde->Name);       
	  pCB->AddString(cde->Name);       
	  // create the device and put it in
	  pCD = new CDevice;	
	  // put in idx	 
	  pCD->DevIdx = cde->Idx;
      // reset the pointer
      //pCB->SetItemDataPtr(pCB->GetCount() - 1, (void*)pCD);
	  pCB->SetItemDataPtr(pCB->FindString(-1, cde->Name), (void*)pCD);	  
	  }
   
    // get default by mapping default DevIdx to ListID
    m_device = DevIDtoListID(cdl.GetDefault());

	// get it
	pCD = (CDevice*)pCB->GetItemDataPtr(m_device);      

	// init it	
    if (pCD->LoadDevice(ListIDtoDevID(m_device)))
	   {
	   // copy the currently selected device to the CurrentDevice
       CurrentDevice = *((CDevice*)pCB->GetItemDataPtr(m_device));   
	   }
	else
	   m_device = -1;

    // write dev data to dialog
    DevToForm();    

	// center the dialog
    CenterWindow();
	
	// update the data in the dialog
	if (IsDlgInit) UpdateData(FALSE);

	// set range of spin control
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_CONTROL))->SetRange(IDDM_MIN_COPIES, IDDM_MAX_COPIES);
   
    // base init
    CPropertyPage::OnInitDialog();

	// show buttons depending on state
   if (ShowOKFlag)
      {
      GetDlgItem(IDOUROK)->ShowWindow(SW_SHOW);
      GetDlgItem(IDOURCANCEL)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_CLOSE)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_RESET)->ShowWindow(SW_HIDE);
	  // if TRUE, must be queue prefs, so allow
	  // changing of device 
	  GetDlgItem(IDC_DEVICE)->EnableWindow(TRUE);	  
	  GetDlgItem(IDC_DEVICE_PRMPT)->EnableWindow(TRUE);	  
      }
   else
      {
      GetDlgItem(IDOUROK)->ShowWindow(SW_HIDE);    
	  GetDlgItem(IDOURCANCEL)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_CLOSE)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_RESET)->ShowWindow(SW_SHOW);
  	  // if FALSE, must be local prefs, so don't allow
	  // changing of device 
	  GetDlgItem(IDC_DEVICE)->EnableWindow(FALSE);	  
	  GetDlgItem(IDC_DEVICE_PRMPT)->EnableWindow(FALSE);	  
      }		    

   // if we have delayed setting the device, do it now
   if (DelayDeviceFlag)
      {
	  SetDevice(DelayDevice);
	  DelayDeviceFlag = FALSE;
	  }	  
   return TRUE;  // return TRUE  unless you set the focus to a control
   }

void CDialogDevSetup::OnOurOK()
   {
   // tell parent that we've selected OK
   // it will handle saving data    
   GetParent()->SendMessage(PSM_PRESSBUTTON, PSBTN_OK);
   ((CPropertySheet*)GetParent())->EndDialog(IDOK);
   }

void CDialogDevSetup::OnOurCancel()
   {
   // send cancel to our parent dialog
   GetParent()->SendMessage(PSM_PRESSBUTTON, PSBTN_CANCEL);
   ((CPropertySheet*)GetParent())->EndDialog(IDCANCEL);
   }

void CDialogDevSetup::OnSelchangeDevice()
   {           
   CDevice* pCD;
                            
   // get the device combo box
   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_DEVICE);   

   // get the currently selected device index
   m_device = pCB->GetCurSel();   

   // get the device it points to
   pCD = (CDevice*)pCB->GetItemDataPtr(m_device);            

   // init the device id that the list points to
   if (!pCD->LoadDevice(ListIDtoDevID(m_device)))
	  m_device = -1;

   // write dev data to dialog
   DevToForm();

   // write the current settings to the CurrentDevice and
   // broadcast the change
   ApplyToDevice();
   }

void CDialogDevSetup::OnSelchangeMediaSource()
   {
   // write the current settings to the CurrentDevice and
   // broadcast the change
   ApplyToDevice();	      
   }

void CDialogDevSetup::OnSelchangeMediaSize()
   {             
   CComboBox* pCB;
   
   // get the device combo box
   pCB = (CComboBox*)GetDlgItem(IDC_DEVICE);
	
   // write the current settings to the CurrentDevice and
   // broadcast the change
   ApplyToDevice();	         

   // warn the user about the change
   CDevice* pCD = (CDevice*)pCB->GetItemDataPtr(m_device);   
   pCD->AlertModeChange(DEVMODE_CHANGE_MEDIA_PRMPT);

   DevToForm();    
   
   // write the current settings to the CurrentDevice and
   // broadcast the change
   ApplyToDevice();	      
   }

int CDialogDevSetup::SetPaper()
   {               
   int i;
   CComboBox* pCBDevList;
   CComboBox* pCBMediaList;    
   CDevice* pCD;   
   MEDIA Media;   

   // get the device boxes
   pCBDevList = (CComboBox*)GetDlgItem(IDC_DEVICE);
   pCBMediaList = (CComboBox*)GetDlgItem(IDC_MEDIA_SIZE);
	
   // get the device
   pCD = (CDevice*)pCBDevList->GetItemDataPtr(m_device);

   // reset the media list
   pCBMediaList->ResetContent();      

   // check for valid device info
   if (!pCD->pDevInfo) return 0;   

   // insert the papers into the list
   pCD->UpdateMediaList();	  
   for (i = 0; i < pCD->pDevInfo->TotalMedia; i++)
      {	  	  	  
	  pCD->GetMedia(i, &Media);	  
	  pCBMediaList->InsertString(-1, Media.Name);
	  }
   
   // get the media
   m_mediasize = pCD->pDevInfo->CurrentMediaIdx;

   return pCD->pDevInfo->TotalMedia;   
   }
	  
int CDialogDevSetup::SetBin()
   {               
   int i;
   CComboBox* pCBDevList;
   CComboBox* pCBSourceList;    
   CDevice* pCD;
   CString csBin;   

   // get the device boxes
   pCBDevList = (CComboBox*)GetDlgItem(IDC_DEVICE);
   pCBSourceList = (CComboBox*)GetDlgItem(IDC_MEDIA_SOURCE);
	
   // get the device
   pCD = (CDevice*)pCBDevList->GetItemDataPtr(m_device);
	
   // reset the media source list
   pCBSourceList->ResetContent();   

   // check for valid device info
   if (!pCD->pDevInfo) return 0;   

   // insert the sources into the list
   for (i = 0; i < pCD->pDevInfo->TotalBin; i++)
      {	  
	  pCD->GetMediaBin(i, csBin.GetBuffer(MAX_MEDIANAME));	  
	  csBin.ReleaseBuffer();
	  pCBSourceList->InsertString(-1, csBin);
	  }

   // get the source
   m_mediasource = pCD->pDevInfo->CurrentBinIdx;
      
   return pCD->pDevInfo->TotalBin;   
   }

int CDialogDevSetup::SetRes()
   {               
   int i;
   CComboBox* pCBDevList;
   CComboBox* pCBRes;    
   CDevice* pCD;
   CString csRes;   

   // get the device boxes
   pCBDevList = (CComboBox*)GetDlgItem(IDC_DEVICE);
   pCBRes = (CComboBox*)GetDlgItem(IDC_RESOLUTION);
	
   // get the device
   pCD = (CDevice*)pCBDevList->GetItemDataPtr(m_device);
	
   // reset the resolution list
   pCBRes->ResetContent();   

   // check for valid device info
   if (!pCD->pDevInfo) return 0;   

   // insert the resoluitions into the list
   pCD->pDevInfo->TotalRes = pCD->UpdateResList();
   for (i = 0; i < pCD->pDevInfo->TotalRes; i++)
      {	  
	  pCD->GetRes(i, csRes.GetBuffer(MAX_RESNAME));	  
	  csRes.ReleaseBuffer();
	  pCBRes->InsertString(-1, csRes);
	  }

   // get the current res
   m_resolution = pCD->pDevInfo->CurrentResIdx;
      
   return pCD->pDevInfo->TotalRes;   
   }

int CDialogDevSetup::SetConnect()
   {               
   int i;
   CComboBox* pCBDevList;
   CComboBox* pCBConnect;    
   CDevice* pCD;
   CString csConnect;   

   // get the device boxes
   pCBDevList = (CComboBox*)GetDlgItem(IDC_DEVICE);
   pCBConnect = (CComboBox*)GetDlgItem(IDC_CONNECTION);
	
   // get the device
   pCD = (CDevice*)pCBDevList->GetItemDataPtr(m_device);
	
   // reset the connection list
   pCBConnect->ResetContent();   

   // check for valid device info
   if (!pCD->pDevInfo) return 0;   

   // insert the connections into the list   
   pCD->pDevInfo->TotalConnect = pCD->pDevInfo->UpdateConnectList();
   for (i = 0; i < pCD->pDevInfo->TotalConnect; i++)
      {	  
	  pCD->GetConnect(i, csConnect.GetBuffer(MAX_CONNECTNAME));	  
	  csConnect.ReleaseBuffer();
	  pCBConnect->InsertString(-1, csConnect);
	  }

   // get the current connection
   m_connection = pCD->pDevInfo->CurrentConnectIdx;

   return pCD->pDevInfo->TotalConnect;   
   }

void CDialogDevSetup::OnOK()
   {          
   // get the device combo box
   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_DEVICE);

   if (m_device < 0) return;

   // get the device
   CDevice* pCD = (CDevice*)pCB->GetItemDataPtr(m_device);

   // check for valid device info
   if (!pCD->pDevInfo) return;							   

   // write the dialog data to the currently selected device
   UpdateDevice();

   // commit the data		   
   pCD->SetDevInfo(TRUE);

   // get devinfo to recompute mem stuff
   pCD->GetDevInfo();

   // and apply it
   ApplyToDevice();

   // set this device as the default device
   cdl.SetDefault(pCD->DevIdx);	
   }								

void CDialogDevSetup::OnDestroy()
   {
   int i;   
   CDevice* pCD;
		
   CPropertyPage::OnDestroy();

   // get the device combo box
   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_DEVICE);
   
   for (i = 0; i < pCB->GetCount(); i++)
      {
      // get the device
      pCD = (CDevice*)pCB->GetItemDataPtr(i);	
      // delete the device
      if (pCD) delete pCD;
      }	
   }   
   
void CDialogDevSetup::ShowOK(BOOL flag)
   {
   // show buttons depending on state
   ShowOKFlag = flag;
   }   

void CDialogDevSetup::ApplyToDevice()
   {
   CDevice* pCD;

   // write the dialog data to the currently selected device
   UpdateDevice();

   // get the device combo box
   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_DEVICE);

   if (m_device < 0) return;

   // set current device
   pCD = (CDevice*)pCB->GetItemDataPtr(m_device);

   // check for valid device info
   if (!pCD->pDevInfo) return;

   // set current device
   CurrentDevice = *pCD;									 

   // send back changes to the queue which uses it to modify the 
   // devmode of selected files
   pWnd->SendMessage(wm_DevModeChange, 
	  PREFS_CHANGE_DEVICE | ReportChange, (LONG)&CurrentDevice);	            
   }

void CDialogDevSetup::OnClose()
   {       
   // hide it
   GetParent()->ShowWindow(SW_HIDE);
   }

void CDialogDevSetup::OnChangeCopies()
   {   
   // if we've blanked out the field, don't apply
   // to the device   
   GetDlgItemText(IDC_COPIES, csTemp);
   if (csTemp == " ") return;
   // write the current settings to the CurrentDevice and
   // broadcast the change
   ApplyToDevice();	   
   }

void CDialogDevSetup::OnReset()
   {                                                              
   // inform the caller      
   pWnd->SendMessage(wm_DevModeChange, PREFS_CHANGE_DEVICE | ReportChange, 0L);               	  
   }
   
void CDialogDevSetup::DelayedSetDevice(CDevice& cd)
   {
   DelayDevice = cd;
   DelayDeviceFlag = TRUE;
   }
   
void CDialogDevSetup::SetDevice(CDevice& cd)
   {
   CComboBox* pCB;
   CDevice* pCD;

   // get the device combo box
   pCB = (CComboBox*)GetDlgItem(IDC_DEVICE);

   // set the device using the index
   m_device = DevIDtoListID(cd.DevIdx);
		   
   // get the new device
   pCD = (CDevice*)pCB->GetItemDataPtr(m_device);      

   // copy over the new device   
   if (*pCD != cd) *pCD = cd;   
   
   // if it is not loaded, this is a bad device
   if (!pCD->IsInit) m_device = -1;

   // write dev data to dialog
   DevToForm();
   }

CDevice CDialogDevSetup::GetCurrentDevice()
   {
   return CurrentDevice;
   }

void CDialogDevSetup::UpdateDevice()
   {
   CComboBox* pCB;
   CDevice* pCD;   

   // get all the data
   if (IsDlgInit) UpdateData(TRUE);

   // get the device combo box
   pCB = (CComboBox*)GetDlgItem(IDC_DEVICE);

   if (m_device < 0) return;

   // get the current device
   pCD = (CDevice*)pCB->GetItemDataPtr(m_device);   
   
   // check for valid device info
   if (!pCD->pDevInfo) return;

   // set copies
   pCD->pDevInfo->Copies = m_copies;

   // set orientation
   pCD->pDevInfo->Orientation = m_orientation;
   
   // get the media
   pCD->pDevInfo->CurrentMediaIdx = m_mediasize;

   // get the source
   pCD->pDevInfo->CurrentBinIdx = m_mediasource;

   // get the resolution
   pCD->pDevInfo->CurrentResIdx = m_resolution;

   // get the connection
   pCD->pDevInfo->CurrentConnectIdx = m_connection;

   // write the current device info
   pCD->SetDevInfo(FALSE);
   
   // get devinfo to recompute mem stuff
   pCD->GetDevInfo();
   }

void CDialogDevSetup::DevToForm()
   {   
   CDevice* pCD;
   CComboBox* pCB;
   
   // get the device combo box
   pCB = (CComboBox*)GetDlgItem(IDC_DEVICE);

   if (m_device < 0) return;
      
   // get the current device
   pCD = (CDevice*)pCB->GetItemDataPtr(m_device);   

   // tell the device!
   // m_mediasize was updated on ApplyToDevice      
   pCD->AlertModeChange(DEVMODE_CHANGE_MEDIA);	 	
   
   // get media
   CRect cMediaRect;   
   pCD->GetCurrentMediaRect(cMediaRect);
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

   if (!pCD->pDevInfo)
      {
	  GetDlgItem(IDC_OPTIONS)->EnableWindow(FALSE);	  
	  GetDlgItem(IDC_COPIES)->EnableWindow(FALSE);
	  GetDlgItem(IDC_COPIES_PRMPT)->EnableWindow(FALSE);
      GetDlgItem(IDC_SPIN_CONTROL)->EnableWindow(FALSE);
      GetDlgItem(IDC_ORIENTATION_LIST)->EnableWindow(FALSE);      
	  GetDlgItem(IDC_ORIENTATION)->EnableWindow(FALSE);      
	  GetDlgItem(IDC_ORIENTATION_PRMPT)->EnableWindow(FALSE);      	  
	  GetDlgItem(IDC_CONNECTION)->EnableWindow(FALSE);   
	  GetDlgItem(IDC_CONECTION_PRMPT)->EnableWindow(FALSE);   
	  GetDlgItem(IDC_MEDIA_SOURCE)->EnableWindow(FALSE);   
	  GetDlgItem(IDC_MEDIA_SOURCE_PRMPT)->EnableWindow(FALSE);
	  GetDlgItem(IDC_MEDIA_SIZE)->EnableWindow(FALSE);	  
	  GetDlgItem(IDC_MEDIA_SIZE_PRMPT)->EnableWindow(FALSE);	  
	  GetDlgItem(IDC_RESOLUTION)->EnableWindow(FALSE);
	  GetDlgItem(IDC_RESOLUTION_PRMPT)->EnableWindow(FALSE);
	  }   
   else
      {
      GetDlgItem(IDC_OPTIONS)->EnableWindow(TRUE);	  	        
	  GetDlgItem(IDC_ORIENTATION_LIST)->EnableWindow(pCD->pDevInfo->CanDoOrientation);
	  GetDlgItem(IDC_ORIENTATION)->EnableWindow(pCD->pDevInfo->CanDoOrientation);
	  GetDlgItem(IDC_ORIENTATION_PRMPT)->EnableWindow(pCD->pDevInfo->CanDoOrientation);	  
	  GetDlgItem(IDC_CONNECTION)->EnableWindow(TRUE);
	  GetDlgItem(IDC_CONECTION_PRMPT)->EnableWindow(TRUE);
	  GetDlgItem(IDC_MEDIA_SOURCE)->EnableWindow(TRUE);
	  GetDlgItem(IDC_MEDIA_SOURCE_PRMPT)->EnableWindow(TRUE);
	  GetDlgItem(IDC_MEDIA_SIZE)->EnableWindow(TRUE);
	  GetDlgItem(IDC_MEDIA_SIZE_PRMPT)->EnableWindow(TRUE);
	  GetDlgItem(IDC_RESOLUTION)->EnableWindow(TRUE);
	  GetDlgItem(IDC_RESOLUTION_PRMPT)->EnableWindow(TRUE);
	  
      // set copies
      m_copies = pCD->pDevInfo->Copies;
      // set orientation                            
      m_orientation = pCD->pDevInfo->Orientation;
      // show orientation icon
      SendDlgItemMessage(IDC_ORIENTATION, STM_SETICON, 
        m_orientation == DEV_ORIENT_LANDSCAPE ?
        (WORD)(AfxGetApp()->LoadIcon(IDI_LANDSCAPE)) :
        (WORD)(AfxGetApp()->LoadIcon(IDI_PORTRAIT)), 0L);		       	           

      // enable copies
      GetDlgItem(IDC_COPIES)->EnableWindow(TRUE);
	  GetDlgItem(IDC_COPIES_PRMPT)->EnableWindow(TRUE);
      GetDlgItem(IDC_SPIN_CONTROL)->EnableWindow(TRUE);               
      }
   
   // set paper types	  
   if (!SetPaper())
      {
      ((CComboBox*)GetDlgItem(IDC_MEDIA_SIZE))->ResetContent();
      GetDlgItem(IDC_MEDIA_SIZE)->EnableWindow(FALSE);
	  GetDlgItem(IDC_MEDIA_SIZE_PRMPT)->EnableWindow(FALSE);
      }   
   // set bins
   if (!SetBin())
      {
      ((CComboBox*)GetDlgItem(IDC_MEDIA_SOURCE))->ResetContent();
      GetDlgItem(IDC_MEDIA_SOURCE)->EnableWindow(FALSE);   
	  GetDlgItem(IDC_MEDIA_SOURCE_PRMPT)->EnableWindow(FALSE);
      }
   // set res
   if (!SetRes())
      {
      ((CComboBox*)GetDlgItem(IDC_RESOLUTION))->ResetContent();
      GetDlgItem(IDC_RESOLUTION)->EnableWindow(FALSE);   
	  GetDlgItem(IDC_RESOLUTION_PRMPT)->EnableWindow(FALSE);   
      }         
   // set connect
   if (!SetConnect())
      {
      ((CComboBox*)GetDlgItem(IDC_CONNECTION))->ResetContent();
      GetDlgItem(IDC_CONNECTION)->EnableWindow(FALSE);   
	  GetDlgItem(IDC_CONECTION_PRMPT)->EnableWindow(FALSE);   
      }
         
   // update the data in the dialog
   if (IsDlgInit) UpdateData(FALSE);      
   }

void CDialogDevSetup::OnSelchangeConnection() 
   {
   // write the current settings to the CurrentDevice and
   // broadcast the change
   ApplyToDevice();	      

   // reset the papers as they may have changed
   SetPaper();
   UpdateData(FALSE);
   }

void CDialogDevSetup::OnSelchangeResolution() 
   {
   // write the current settings to the CurrentDevice and
   // broadcast the change
   ApplyToDevice();	      
   }

int CDialogDevSetup::DevIDtoListID(int DevID)
   {
   CComboBox* pCB;    
   int idx;
   int ListID;
   CDevice* pCD;

   // get the device combo box
   pCB = (CComboBox*)GetDlgItem(IDC_DEVICE);   

   // look up list id
   for (idx = 0, ListID = -1; idx < pCB->GetCount(); idx++)
      {	  
      // get the device
      pCD = (CDevice*)pCB->GetItemDataPtr(idx);
	  if (pCD->DevIdx == DevID)
	     {
	     ListID = idx;
		 break;
		 }
	  }
   return ListID;
   }

int CDialogDevSetup::ListIDtoDevID(int ListID)
   {   
   CDevice* pCD;
   CComboBox* pCB;    

   // get the device combo box
   pCB = (CComboBox*)GetDlgItem(IDC_DEVICE);      

   // get the device
   pCD = (CDevice*)pCB->GetItemDataPtr(ListID);

   // return the dev id
   return pCD->DevIdx;
   }

void CDialogDevSetup::OnAdvanced() 
   {
   CAdvancedDevice adv;
   CDevice* pCD;
   CComboBox* pCB;   

   // get the device combo box
   pCB = (CComboBox*)GetDlgItem(IDC_DEVICE);
   
   if (m_device < 0) return;
   
   // get the current device
   pCD = (CDevice*)pCB->GetItemDataPtr(m_device);   

   // check for valid device info
   if (!pCD->pDevInfo) return;   

   // write the dialog data to the currently selected device
   UpdateDevice();

   // write the current device info
   //pCD->SetDevInfo(FALSE);

   // now get it again to update band size
   //pCD->GetDevInfo();				  

   // load vars from device into advanced dialog    
   adv.m_buffertodisk = (BOOL)pCD->pDevInfo->BufferToDisk;
   adv.m_compress = (BOOL)pCD->pDevInfo->UseCompression;
   adv.m_compress_bitmap = (BOOL)pCD->pDevInfo->UseCompressionBmp;
   adv.m_scale_bitmap = (BOOL)pCD->pDevInfo->ScaleImage;
   adv.m_canscale_bitmap = (BOOL)pCD->pDevInfo->CanScaleImage;
   adv.m_cancompress = (BOOL)pCD->pDevInfo->CanDoCompression;
   adv.m_canaalias = (BOOL)pCD->pDevInfo->CanDoAntiAliasing;
   adv.m_memalloc_style = pCD->pDevInfo->MemUsedIdx;
   adv.m_aatype = (int)pCD->pDevInfo->AliasStyle;
   adv.m_aaoversample = max((int)pCD->pDevInfo->AliasLevel - 2, 0);
   //adv.m_imagememory_num = pCD->pDevInfo->BandSize * pCD->pDevInfo->BandWidth / 1024;   
   adv.m_imagememory_num = pCD->pDevInfo->BandSize / 1024 * pCD->pDevInfo->BandWidth;
   adv.m_EnableBufferToDisk = pCD->pDevInfo->EnableBufferToDisk;
   adv.m_memused = pCD->pDevInfo->MemUsed;

   adv.m_def_buffertodisk = (BOOL)pCD->pDevInfo->Def_BufferToDisk;
   adv.m_def_compress = (BOOL)pCD->pDevInfo->Def_UseCompression;
   adv.m_def_compress_bitmap = (BOOL)pCD->pDevInfo->Def_UseCompressionBmp;
   adv.m_def_scale_bitmap = (BOOL)pCD->pDevInfo->Def_ScaleImage;
   adv.m_def_memalloc_style = pCD->pDevInfo->Def_MemUsedIdx;
   adv.m_def_aatype = pCD->pDevInfo->Def_AliasStyle;
   adv.m_def_aaoversample = max((int)pCD->pDevInfo->Def_AliasLevel - 2, 0);   
   
   if (adv.DoModal() == IDOK)
      {      
      pCD->pDevInfo->BufferToDisk = (BYTE)adv.m_buffertodisk;
      pCD->pDevInfo->UseCompression = (BYTE)adv.m_compress;
	  pCD->pDevInfo->UseCompressionBmp = (BYTE)adv.m_compress_bitmap;
      pCD->pDevInfo->MemUsedIdx = (BYTE)adv.m_memalloc_style;	  
	  pCD->pDevInfo->MemUsed = (DWORD)(adv.m_memused);
      pCD->pDevInfo->AliasStyle = (BYTE)adv.m_aatype;
      pCD->pDevInfo->AliasLevel = (BYTE)adv.m_aaoversample + 2;
	  pCD->pDevInfo->ScaleImage = (BYTE)adv.m_scale_bitmap;
	  // tell everyone
	  ApplyToDevice();
      }
   }

void CDialogDevSetup::OnSelchangeOrientationList() 
   {
   // get all the data
   if (IsDlgInit) UpdateData(TRUE);

   SendDlgItemMessage(IDC_ORIENTATION, STM_SETICON, 
        m_orientation == DEV_ORIENT_LANDSCAPE ?
        (WORD)(AfxGetApp()->LoadIcon(IDI_LANDSCAPE)) :
        (WORD)(AfxGetApp()->LoadIcon(IDI_PORTRAIT)), 0L);		       	           

   // write the current settings to the CurrentDevice and
   // broadcast the change
   ApplyToDevice();	   
   }

void CDialogDevSetup::SetMultiSelFields(CObArray& cod)
   {
   int i;
   CDevice* pCD;
   int MediaIdx, BinIdx, Copies, Resolution, Orientation, Connection;
   BOOL MediaIdxDiffer, BinIdxDiffer, CopiesDiffer, ResolutionsDiffer;
   BOOL OrientationsDiffer, ConnectionsDiffer;

   // get first device
   pCD = (CDevice*)cod.GetAt(0);
   // get first media selection
   MediaIdx = pCD->pDevInfo->CurrentMediaIdx;   
   // get first source selection
   BinIdx = pCD->pDevInfo->CurrentBinIdx;   
   // get first copies selection
   Copies = pCD->pDevInfo->Copies;   
   // get first res selection
   Resolution = pCD->pDevInfo->CurrentResIdx;
   // get first orientation selection
   Orientation = pCD->pDevInfo->Orientation;
   // get first connection selection
   Connection = pCD->pDevInfo->CurrentConnectIdx;
   
   // default to no difference
   CopiesDiffer = MediaIdxDiffer = BinIdxDiffer = ResolutionsDiffer = FALSE;
   ConnectionsDiffer = OrientationsDiffer = FALSE;
   
   // see if they are all the same
   for (i = 1; i < cod.GetSize(); i++)
      {      
	  // get device
	  pCD = (CDevice*)cod.GetAt(i);
	  // if they differ, set indeterminite state
	  if (pCD->pDevInfo->CurrentMediaIdx != MediaIdx)	     
		 MediaIdxDiffer = TRUE;		 		 	  	  
	  if (pCD->pDevInfo->CurrentBinIdx != BinIdx)	     
		 BinIdxDiffer = TRUE;		 		 		 	  
	  if (pCD->pDevInfo->Copies != Copies)	     
		 CopiesDiffer = TRUE;		 		 		 		 
	  if (pCD->pDevInfo->CurrentResIdx != Resolution)	     
		 ResolutionsDiffer = TRUE;		 		 		 		 
	  if (pCD->pDevInfo->Orientation != Orientation)	     
		 OrientationsDiffer = TRUE;
	  if (pCD->pDevInfo->CurrentConnectIdx != Connection)	     
		 ConnectionsDiffer = TRUE;		 
	  }

   ((CComboBox*)GetDlgItem(IDC_MEDIA_SIZE))->SetCurSel(MediaIdxDiffer ? -1 : MediaIdx);
   ((CComboBox*)GetDlgItem(IDC_MEDIA_SOURCE))->SetCurSel(BinIdxDiffer ? -1 : BinIdx);
   ((CComboBox*)GetDlgItem(IDC_MEDIA_SOURCE_PRMPT))->SetCurSel(BinIdxDiffer ? -1 : BinIdx);
   ((CComboBox*)GetDlgItem(IDC_RESOLUTION))->SetCurSel(ResolutionsDiffer ? -1 : Resolution);
   ((CComboBox*)GetDlgItem(IDC_ORIENTATION_LIST))->SetCurSel(OrientationsDiffer ? -1 : Orientation);      
   ((CComboBox*)GetDlgItem(IDC_CONNECTION))->SetCurSel(ConnectionsDiffer ? -1 : Connection);   
   if (CopiesDiffer) SetDlgItemText(IDC_COPIES, " ");

   //UpdateData(TRUE);
   return;
   }     

BOOL CDialogDevSetup::OnHelpInfo(HELPINFO* pHelpInfo) 
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
 
void CDialogDevSetup::OnContextMenu(CWnd* pWnd, CPoint point) 
   {   
   // call context help
   ::WinHelp(pWnd->m_hWnd, AfxGetApp()->m_pszHelpFilePath, 
      HELP_CONTEXTMENU,  (DWORD)(LPVOID)dwHelpMap);
   }
