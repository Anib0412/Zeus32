/////////////////////////////////////////////////////////////////////////////
// NewQueue.cpp
//                                                                        
// make a new queue for RasterPlus
// Copyright (C) 1999-2000 Graphx, Inc.
//
// mods:
//
// 11-12-99 Version 3.0.2 - don't make a copy of the queue when doing a "silent" install
// 11-30-99 Version 3.0.2 - added SCSI Explorer to New Queue dialog
// 08-09-00 Version 4.0.4 - disable connection combo if there are none
// 01-18-01 Version 4.0.2 - Change default to use first entry in ListBox if the default DevID
//                          is not present in the list - Allows InkJetPlus to be installed alone.

#include "stdafx.h"
#include "zeus.h"
#include "NewQueue.h"
#include "winspool.h"
#include "device.h"
#include "profile.h"
#include "mainfrm.h"
#include "zeusdoc.h"
#include "SCSIExplorer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewQueue dialog


CNewQueue::CNewQueue(CWnd* pParent /*=NULL*/)
	: CDialog(CNewQueue::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewQueue)
	m_connection = -1;	
	m_name = _T("");	
	m_device = -1;
	//m_hostid_edit = 0;
	//}}AFX_DATA_INIT
	bOnBottom = FALSE;
	bMakeCopyIfExist = TRUE;
}


void CNewQueue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewQueue)
	//DDX_Control(pDX, IDC_HOSTID_SPIN, m_hostid_spin);
	DDX_CBIndex(pDX, IDC_CONNECTION, m_connection);		
	DDX_Text(pDX, IDC_NAME, m_name);	
	DDX_LBIndex(pDX, IDC_DEVICE, m_device);
	//DDX_Text(pDX, IDC_HOSTID_EDIT, m_hostid_edit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewQueue, CDialog)
	//{{AFX_MSG_MAP(CNewQueue)
	ON_LBN_SELCHANGE(IDC_DEVICE, OnSelchangeDevice)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_FINDDEVICE, OnFinddevice)
	ON_LBN_DBLCLK(IDC_DEVICE, OnDblclkDevice)
	ON_CBN_SELCHANGE(IDC_CONNECTION, OnSelchangeConnection)	
	ON_BN_CLICKED(IDC_SCSI_EXPLORER, OnScsiExplorer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewQueue message handlers

BOOL CNewQueue::OnInitDialog() 
   {
   CListBox* pCL;   
   int idx;   
   CDeviceEntry* cde;   

   CDialog::OnInitDialog();
   
   // get the combo box
   pCL = (CListBox*)GetDlgItem(IDC_DEVICE);

   // get all devices   
   cdl.LoadAll();
   cdl.SortByName();

   pCL->ResetContent();   

   // insert into list
   for (idx = 0; idx < cdl.GetSize(); idx++)
      {
	  // get entry
	  cde = (CDeviceEntry*)cdl.GetAt(idx);	  
	  pCL->AddString(cde->Name);       	  
	  pCL->SetItemDataPtr(idx, (void*)cde);
	  }	   		 

   // get existing devices
   //GetInstalledDevices();
   GetNamedQueues();

   // select the default   
   m_device = DevIDtoListID(cdl.GetDefault());
   if ( m_device  == -1 )
   {
	   (CButton*)GetDlgItem(IDOK)->EnableWindow(FALSE);
	   (CButton*)GetDlgItem(IDC_FINDDEVICE)->EnableWindow(FALSE);
	   return FALSE;
   }
   
   // init data
   UpdateData(FALSE);   
   
   MakeDevName();   

   LoadConnections();

   //if (bOnBottom) PositionWindow();
   if (bOnBottom) 
      SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

   //GetDlgItem(IDC_HOSTID_PRMPT)->EnableWindow(FALSE);
   //GetDlgItem(IDC_HOSTID_SPIN)->EnableWindow(FALSE);
   //GetDlgItem(IDC_HOSTID_EDIT)->EnableWindow(FALSE);
   //m_hostid_spin.SetRange(0, 7);   

   return TRUE;  
   }

void CNewQueue::OnSelchangeDevice() 
   {
   UpdateData(TRUE);

   MakeDevName();

   LoadConnections();
   }						  

void CNewQueue::MakeDevName() 
   {
   CDeviceEntry* cde; 
   CString m_newname;
   int iCanFindDevice;
   int idx = 2;
   CProfile Profile;
   CString csSection;
   
   CListBox* pCL = (CListBox*)GetDlgItem(IDC_DEVICE);

   // get the current device entry
   cde = (CDeviceEntry*)pCL->GetItemDataPtr(m_device);

   // get the name
   m_name = cde->Name;

   // can we find this device?
   csSection.LoadString(IDS_CANFINDDEVICE);
   iCanFindDevice = Profile.GetInt(m_name, csSection);
   GetDlgItem(IDC_FINDDEVICE)->EnableWindow(iCanFindDevice);

   // if a match with an existing driver,
   // keep trying until we get a unique name
   if (IsMatch(m_name) && bMakeCopyIfExist)
	  {								  
	  do
		 {
		 m_newname.Format("%s - Copy %d", m_name, idx++);		     
		 } while (IsMatch(m_newname));
	  m_name = m_newname;
	  }

   UpdateData(FALSE);
   }

int CNewQueue::GetInstalledDevices() 
   {
   LPPRINTER_INFO_2 pi;      
   DWORD dwSizeNeeded, dwNumPrinters;
   DWORD i;
   int cnt;
   CString cs, csDevName, csTemp;
   HANDLE hPrinter;      
   
   // get space needed for printers
   ::EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 2, NULL, 
      0, &dwSizeNeeded, &dwNumPrinters);

   // allocate space for all printers
   // simple error checking per Win NT sample
   if (!(pi = (LPPRINTER_INFO_2)new char[dwSizeNeeded]))
      return 0;

   // get all printers
   if (!::EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 2, (LPBYTE)pi, 
      dwSizeNeeded, &dwSizeNeeded, &dwNumPrinters))
      {
      delete pi;
      return 0;
      }	  
   
   // destroy the current list
   csa.RemoveAll();

   // get all printers
   for (i = 0, cnt = 0; i < dwNumPrinters; i++)
      {	
      // get printer  
	  if (::OpenPrinter((pi + i)->pPrinterName, &hPrinter, NULL))
	     {
		 csa.Add((pi + i)->pPrinterName);
		 ::ClosePrinter(hPrinter);
		 }
	  }

   delete pi;

   // delete the printer buffer   
   return dwNumPrinters;
   }

int CNewQueue::GetNamedQueues() 
   {
   int i;
   CPaneElement* pRipElement;   
   CZeusDoc* pDoc;
   CString csTitle;
   int iNumDevices = 0;

   for (i = 0; i < ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pPaneArray.GetSize(); i++)
	  {
	  pRipElement = (CPaneElement*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pPaneArray.GetAt(i);

	  if (!pRipElement) continue;

	  // only do on archive
	  if (pRipElement->iViewStyle != QUEUESTYLE_ARCHIVE) continue;

      // get the active document
      if (pDoc = (CZeusDoc*)(pRipElement->pDocPtr))
		 {
		 // get the title
		 csTitle = pDoc->GetTitle();
		 // add to the array
		 csa.Add(csTitle);
		 iNumDevices++;
         }   
	  }
   return iNumDevices;
   }					

BOOL CNewQueue::IsMatch(CString& cs)
   {
   BOOL bRet = FALSE;

   for (int i = 0; i < csa.GetSize(); i++)
	  {
	  if (csa.GetAt(i) == cs)
		 {
		 bRet = TRUE;
		 break;
		 }
	  }
   return bRet;
   }

void CNewQueue::OnDestroy() 
   {
   CDialog::OnDestroy();
	
   // get all data before closing
   UpdateData(TRUE);

   if ( m_device == -1 )
	   return;

   // get info
   CListBox* pCLDevice = (CListBox*)GetDlgItem(IDC_DEVICE);
   CDeviceEntry* cde = (CDeviceEntry*)pCLDevice->GetItemDataPtr(m_device);   
   iDevIdx = cde->Idx;
   csDevice = cde->Name;
   csPort = cde->Port;
   }							 

void CNewQueue::LoadConnections()
   {
   CDevice* pCD;
   CDeviceEntry* cde; 
   int i;
   CString csConnect;
   int ConnectType;
   int idx;

   // get box pointers
   CListBox* pCLDevice = (CListBox*)GetDlgItem(IDC_DEVICE);
   CComboBox* pCBConnect = (CComboBox*)GetDlgItem(IDC_CONNECTION);

   // get the current device entry
   cde = (CDeviceEntry*)pCLDevice->GetItemDataPtr(m_device);

   // create the device
   pCD = new CDevice;

   // load the device   
   if (pCD->LoadDevice(cde->Idx))
	  {
	  pCD->GetDevInfo();
	  pCBConnect->ResetContent();

      // insert the connections into the list
	  pCD->pDevInfo->TotalConnect = pCD->UpdateConnectList();

	  // only do this if we have connections
	  if (pCD->pDevInfo->TotalConnect > 0)
		 {
		 GetDlgItem(IDC_CONNECTION)->EnableWindow(TRUE);
		 GetDlgItem(IDC_CONNECTION_PRMPT)->EnableWindow(TRUE);
		 for (i = 0; i < pCD->pDevInfo->TotalConnect; i++)
			{	  
			pCD->GetConnect(i, csConnect.GetBuffer(MAX_CONNECTNAME), &ConnectType);	  		 
			csConnect.ReleaseBuffer();

			pCBConnect->InsertString(-1, csConnect);
			pCBConnect->SetItemData(pCBConnect->FindStringExact(-1, csConnect), ConnectType);
			}      
		 if ((idx = pCBConnect->FindStringExact(-1, pCD->pDevInfo->ConnectStr)) != CB_ERR)
			pCBConnect->SetCurSel(idx);	  
		 }
	  else
		 {
		 GetDlgItem(IDC_CONNECTION)->EnableWindow(FALSE);
		 GetDlgItem(IDC_CONNECTION_PRMPT)->EnableWindow(FALSE);
		 }
	  //GetDlgItem(IDC_HOSTID_PRMPT)->EnableWindow(pCBConnect->GetItemData(pCBConnect->GetCurSel()) == CONNECT_TYPE_SCSI);
	  //GetDlgItem(IDC_HOSTID_SPIN)->EnableWindow(pCBConnect->GetItemData(pCBConnect->GetCurSel()) == CONNECT_TYPE_SCSI);
	  //GetDlgItem(IDC_HOSTID_EDIT)->EnableWindow(pCBConnect->GetItemData(pCBConnect->GetCurSel()) == CONNECT_TYPE_SCSI);
	  }
   // delete it
   delete pCD;
   }

int CNewQueue::GetDefault()
   {
   CProfile Profile;

   return Profile.GetInt(IDS_DEVICES, IDS_DEVICE_DEFAULT);
   }

int CNewQueue::DevIDtoListID(int DevID)
   {
   CListBox* pCL;    
   int idx;
   int iDevCount;
   int ListID;   
   CDeviceEntry* cde;

   // get the device combo box
   pCL = (CListBox*)GetDlgItem(IDC_DEVICE);   

   iDevCount = pCL->GetCount();
   if (iDevCount == 0 )
	   return -1;

   // look up list id
   // If the Default DevID is not found use the first (0)
   // entry in the List Box
   for (idx = 0, ListID = 0; idx < iDevCount; idx++)
      {	  
      // get the device
	  cde = (CDeviceEntry*)pCL->GetItemDataPtr(idx);
      
	  if (cde->Idx == DevID)
	     {
	     ListID = idx;
		 break;
		 }
	  }
   return ListID;
   }

void CNewQueue::OnFinddevice() 
   {
   CDevice* pCD;   
   CDeviceEntry* cde;
   CString csPort;
   int idx;
   CString csErr;   
   int iHostAdapterID;

   AfxGetApp()->DoWaitCursor(1);

   UpdateData(TRUE);

   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_CONNECTION);
   CListBox* pCL = (CListBox*)GetDlgItem(IDC_DEVICE);      

   cde = (CDeviceEntry*)pCL->GetItemDataPtr(m_device);	  	  
   if (pCD = new CDevice)
	  {
      if (pCD->LoadDevice(cde->Idx))
		 {
	     // find the connection
         if (pCD->FindConnection(csPort.GetBuffer(_MAX_PATH), &iHostAdapterID))
			{
			csPort.ReleaseBuffer();
		    // find it
		    if ((idx = pCB->FindStringExact(-1, csPort)) != LB_ERR)
			   {
			   // select it
			   pCB->SetCurSel(idx);
               // update port
               cde->Port = csPort;
			   //cde->iHostID = iHostAdapterID;
	           //GetDlgItem(IDC_HOSTID_PRMPT)->EnableWindow(pCB->GetItemData(idx) == CONNECT_TYPE_SCSI);
	           //GetDlgItem(IDC_HOSTID_SPIN)->EnableWindow(pCB->GetItemData(idx) == CONNECT_TYPE_SCSI);
	           //GetDlgItem(IDC_HOSTID_EDIT)->EnableWindow(pCB->GetItemData(idx) == CONNECT_TYPE_SCSI);
			   }
			}
		 else
			{			
			csPort.ReleaseBuffer();
			// can't find it
			csErr.Format(IDS_CANTFINDDEVICE, pCD->Name);
			AfxMessageBox(csErr);
			}
		 }		 	  			   
	  delete pCD;
	  }
   AfxGetApp()->DoWaitCursor(-1);
   }

void CNewQueue::OnDblclkDevice() 
   {
   CNewQueue::OnOK();
   }

void CNewQueue::OnSelchangeConnection() 
   {
   UpdateData(TRUE);

   // get port
   CComboBox* pCBConnect = (CComboBox*)GetDlgItem(IDC_CONNECTION);
   if (m_connection == CB_ERR) return;   
   pCBConnect->GetLBText(m_connection, csPort);

   //GetDlgItem(IDC_HOSTID_PRMPT)->EnableWindow(pCBConnect->GetItemData(m_connection) == CONNECT_TYPE_SCSI);
   //GetDlgItem(IDC_HOSTID_SPIN)->EnableWindow(pCBConnect->GetItemData(m_connection) == CONNECT_TYPE_SCSI);
   //GetDlgItem(IDC_HOSTID_EDIT)->EnableWindow(pCBConnect->GetItemData(m_connection) == CONNECT_TYPE_SCSI);

   // get the current device entry
   CListBox* pCL = (CListBox*)GetDlgItem(IDC_DEVICE);
   CDeviceEntry* cde = (CDeviceEntry*)pCL->GetItemDataPtr(m_device);

   // update port
   cde->Port = csPort;
   //cde->iHostID = iHostAdapterID;
   }

void CNewQueue::PositionWindow()
   {
   CPoint   Point;
   CRect    OurRect;
   CRect    ParentRect;   
   int      nWidth;
   int      nHeight;   

   // Get the size of the dialog box.
   GetWindowRect(OurRect);
   
   // get working area on the desktop
   RECT rc;
   SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
   ParentRect = rc;					 
      
   // Calculate the height and width for MoveWindow().
   nWidth = OurRect.Width();
   nHeight = OurRect.Height();   

   // Find the lower center point
   Point.x = ParentRect.Width() / 2;
   Point.y = ParentRect.Height();
   
   // Calculate the new X, Y starting point.
   Point.x -= nWidth / 2;
   //Point.y -= nHeight;
   Point.y = 0;

   // Move the window.
   MoveWindow(Point.x, Point.y, nWidth, nHeight, FALSE);
   }

void CNewQueue::OnScsiExplorer() 
   {
   CSCSIExplorer se;    
   // show the scsi explorer
   se.DoModal();   
   }

void CNewQueue::OnOK() 
   {
   UpdateData(TRUE);

   if (m_name.GetLength() > 0)
	  {
	  m_name.TrimLeft();
	  m_name.TrimRight();
	  }	
   CDialog::OnOK();
   }
