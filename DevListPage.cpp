/////////////////////////////////////////////////////////////////////////////
// DevListPage.cpp
//                                                                        
// manage devices for RasterPlus95
// Copyright (C) 1996-98 Graphx, Inc.
//
// mods:
//
// 7-22-96  Build 40 - changed SetPort to SetOurPort for VC 4.2
// 10-15-96 Build 45 - added NT stuff
// 10-24-96 Build 45 - NeedToReboot added for NT
// 12-05-96 Build 46 - support copying NT system file as well as expanding
// 12-10-96 Build 46 - OnConnect was failing under NT because of security bug - fixed
// 12-20-96 Build 47 - PhotoShop was getting port name for installed device from  
//                        Software\Microsoft\Windows NT\CurrentVersion\Devices
//					      in Registry - we add that to the NT install mix
// 12-26-96 Build 47 - Above stuff removed (doesn't stick after reboot)
// 08-08-97 Build 50 - copy PPD file to PPD folder and ppd4 pm6.5 folder
// 10-06-97 Build 51 - can now rename installed devices
// 11-18-97 Version 2.0 - added CrossPlatform INF support
// 12-18-97 Version 2.0.1 - force spooler OFF on device install
// 1-20-98  Version 2.0.1 - pm65 stuff got clobbered by CrossPlatform stuff
// 2-06-98  Version 2.0.2 - fixed some mnemonics on the devlistpage
// 3-04-98  Version 2.0.2 - was putting in .PPD instead of .SPD in data file
//                          (probably introduced in 2.0.1)
// 3-06-98  Version 2.0.2 - don't disable spooling for NT server device install because
//                             of 95->NT Server printing problems
// 3-17-98  Version 2.0.3 - leave spooler on because of Kanji problems
// 3-24-98  Version 2.0.3 - extract install stuff to CInstallPrinter
// 4-30-98  Version 2.0.3 - wrong help for ID_CLOSE
// 9-28-98  Version 2.0.5 - Changed OnOurOk PostMessage to SendMessage for VC6.0 compat
// 4-05-99  Version 2.0.6 - fixed memory leak in GetInstalledDevices() 

#include "stdafx.h"
#include "zeus.h"
#include "DevListPage.h"
#include "winspool.h"
#include "device.h"
#include "profile.h"
#include <stdlib.h>
#include <string.h>
#include "dlgwait.h"
#include "devconnect.h"
#include "control.hm"
#include "PrinterNameChange.h"

// the control help-map
static DWORD dwHelpMap[] = 
   {
   IDC_DELETE, HIDC_DEVLIST_DELETE,
   IDC_INSTALL, HIDC_DEVLIST_INSTALL,
   IDC_DEVLIST_AVAILABLE, HIDC_DEVLIST_DEVLIST_AVAILABLE,
   IDC_DEVLIST_INSTALLED, HIDC_DEVLIST_DEVLIST_INSTALLED,
   IDC_CONNECT2, HIDC_DEVLIST_CONNECT,
   IDC_PROPERTIES, HIDC_DEVLIST_PROPERTIES,
   IDC_CLONE, HIDC_DEVLIST_CLONE,
   IDC_CONTROL_PANEL, HIDC_DEVLIST_CONTROL_PANEL,
   IDC_RENAME, HIDC_DEVLIST_RENAME,
   //IDOUROK, HIDC_OUROK,
   IDOUROK, HIDC_CLOSE,
   IDOURCANCEL, HIDC_OURCANCEL,
   ID_CLOSE, HIDC_CLOSE,   
   0,0
   };	 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAXSIZE_DEPFILE_BUF 32000      

/////////////////////////////////////////////////////////////////////////////
// CDevListPage property page

IMPLEMENT_DYNCREATE(CDevListPage, CPropertyPage)

CDevListPage::CDevListPage() : CPropertyPage(CDevListPage::IDD)
{
	//{{AFX_DATA_INIT(CDevListPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDevListPage::~CDevListPage()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDevListPage message handlers

void CDevListPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDevListPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDevListPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDevListPage)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_INSTALL, OnInstall)
	ON_LBN_SETFOCUS(IDC_DEVLIST_AVAILABLE, OnSetfocusDevlistAvailable)
	ON_LBN_SETFOCUS(IDC_DEVLIST_INSTALLED, OnSetfocusDevlistInstalled)
	ON_LBN_SELCHANGE(IDC_DEVLIST_INSTALLED, OnSelchangeDevlistInstalled)
	ON_LBN_SELCHANGE(IDC_DEVLIST_AVAILABLE, OnSelchangeDevlistAvailable)
	ON_LBN_DBLCLK(IDC_DEVLIST_AVAILABLE, OnDblclkDevlistAvailable)
	ON_BN_CLICKED(IDC_PROPERTIES, OnProperties)	
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CONTROL_PANEL, OnControlPanel)
	ON_BN_CLICKED(IDOUROK, OnOurok)
	ON_WM_CONTEXTMENU()	
	ON_WM_HELPINFO()	
	ON_BN_CLICKED(IDC_CONNECT2, OnConnect)
	ON_BN_CLICKED(IDC_RENAME, OnRename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDevList message handlers

BOOL CDevListPage::OnInitDialog() 
   {   
   CPropertyPage::OnInitDialog();

   NeedToReboot = FALSE;

   InitDevList();   
   GetInstalledDevices();

   GetDlgItem(IDC_INSTALL)->ShowWindow(SW_SHOW);	
   GetDlgItem(IDC_DELETE)->ShowWindow(SW_HIDE);   
   GetDlgItem(IDC_PROPERTIES)->EnableWindow(FALSE);   
   GetDlgItem(IDC_RENAME)->EnableWindow(FALSE);   
   GetDlgItem(IDC_INSTALL)->EnableWindow(FALSE);   
   GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);      

   OnSetfocusDevlistAvailable();

   return TRUE;
   }

void CDevListPage::InitDevList()
   {
   CListBox* pCL;   
   int idx;   
   CDeviceEntry* cde;
   CString csDevName;
   CString csFormat;
   int Selection = -1;
   int DefaultSelection;

   // get the combo box
   pCL = (CListBox*)GetDlgItem(IDC_DEVLIST_AVAILABLE);

   // get all devices   
   cdl.LoadAll();

   pCL->ResetContent();

   // insert into list
   csFormat.LoadString(IDS_DEVICE_DEVONPORT);   
   DefaultSelection = cdl.GetDefault();
   for (idx = 0; idx < cdl.GetSize(); idx++)
      {
	  // get entry
	  cde = (CDeviceEntry*)cdl.GetAt(idx);	  
	  csDevName.Format(csFormat, (const char*)cde->Name, (const char*)cde->Port);
	  // get default device
	  if (DefaultSelection == cde->Idx) Selection = idx;
	  //pCL->InsertString(-1, csDevName);       	  
	  //pCL->SetItemDataPtr(idx, (void*)cde);
	  pCL->AddString(csDevName);       
	  pCL->SetItemDataPtr(pCL->FindString(-1, csDevName), (void*)cde);
	  }	   
	// select the default device
	if (Selection != -1) pCL->SetCurSel(Selection);
	}	 

void CDevListPage::OnDelete() 
   {      
   int idx;
   CListBox* cls;
   CString csDevName;
   HANDLE hPrinter;
   CString csTemp1, csTemp2;
   CDeviceEntry* cde;   
      
   // get current selection
   cls = (CListBox*)GetDlgItem(IDC_DEVLIST_INSTALLED);
   idx = cls->GetCurSel();
   if (idx == LB_ERR) return;

   // get device entry
   cde = (CDeviceEntry*)(cls->GetItemDataPtr(idx));

   // get the csDevName
   csDevName = cde->Name;

   // prompt before delete
   if (csDevName.GetLength())
      {
      csTemp1.LoadString(IDS_DEVICE_DELETEOK);
      wsprintf(csTemp2.GetBuffer(_MAX_PATH), (const char*)csTemp1, csDevName);
	  csTemp2.ReleaseBuffer();
      if (AfxMessageBox(csTemp2, MB_YESNO) == IDNO) return;
	  }

   // wait
   AfxGetApp()->DoWaitCursor(1);

   PRINTER_DEFAULTS pd;
   pd.pDatatype = NULL;
   pd.pDevMode = NULL;
   pd.DesiredAccess = PRINTER_ALL_ACCESS;

   if (csDevName.GetLength())
      {
	  // delete the printer
      if (::OpenPrinter((char*)(const char*)csDevName, &hPrinter, &pd))
	     {
	     if (hPrinter) DeletePrinter(hPrinter);

		 if (hPrinter) ::ClosePrinter(hPrinter);

		 // delete the printer driver         
		 // doesn't seem to work in NT but i think that's ok
         DeletePrinterDriver(NULL, NULL, (char*)(const char*)csDevName);   
		 
	     // reset the list
	     GetInstalledDevices();
	     // select the previous driver
	     if ((idx - 1) > 0)
	        cls->SetCurSel(idx - 1);
	     else
	        cls->SetCurSel(0);		 
		 }
	  }
   // if no items left, disable these buttons
   if(cls->GetCount() <= 0)
      {	  	  
	  GetDlgItem(IDC_DELETE)->ShowWindow(SW_HIDE);	  
      GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);	  
	  GetDlgItem(IDC_CONNECT2)->EnableWindow(FALSE);
      GetDlgItem(IDC_PROPERTIES)->EnableWindow(FALSE);
	  GetDlgItem(IDC_RENAME)->EnableWindow(FALSE);   
	  }
   // no wait
   AfxGetApp()->DoWaitCursor(-1);
   }

void CDevListPage::OnInstall() 
   {
   CDeviceEntry* cde;
   CListBox* cls;
   int idx;
   CString csDevName, csTemp, csPrompt, csErr;

   // get current selection
   cls = (CListBox*)GetDlgItem(IDC_DEVLIST_AVAILABLE);
   idx = cls->GetCurSel();
   if (idx == LB_ERR) return;

   // get device entry
   cde = (CDeviceEntry*)(cls->GetItemDataPtr(idx));         
   csTemp.LoadString(IDS_DEVICE_DEVONPORT);
   csDevName.Format(csTemp, (const char*)cde->Name, (const char*)cde->Port);		             

   cls = (CListBox*)GetDlgItem(IDC_DEVLIST_INSTALLED);
   // already installed?
   if (cls->FindStringExact(-1, csDevName) != LB_ERR)	
      return;
   
   // create a wait dlg
   CDlgWait dlg;
   CString csWait;
   csWait.LoadString(IDS_DEVICE_INSTALLING);
   dlg.m_prompt.Format(csWait, (const char*)cde->Name, (const char*)cde->Port);    
   dlg.Create(IDD_MODELESS_WAIT, this);   
    
   if (!cii.InstallPrinter(cde->Name, cde->Name, cde->Idx, cde->Port, 
	  csErr, NeedToReboot, FALSE))
      {
	  csTemp.LoadString(IDS_ERROR_INSTALL_PRINTER);
	  csPrompt.Format("%s\n%s", csTemp, csErr);
      AfxMessageBox(csPrompt);
	  }
   else
      {
	  // get the devices again
      GetInstalledDevices();

	  // select the last device
      cls = (CListBox*)GetDlgItem(IDC_DEVLIST_INSTALLED);
      idx = cls->GetCount() - 1;
      if (idx != LB_ERR) 
		 cls->SetCurSel(idx);

	  // update the install key
	  OnSelchangeDevlistAvailable();
	  }
   // destroy the wait dlg
   dlg.DestroyWindow();
   } 

void CDevListPage::OnSetfocusDevlistAvailable() 
   {
   // de-select other list
   ((CListBox*)GetDlgItem(IDC_DEVLIST_INSTALLED))->SetCurSel(-1);
   // set buttons
   GetDlgItem(IDC_DELETE)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_INSTALL)->ShowWindow(SW_SHOW);
   GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);      
   GetDlgItem(IDC_CONNECT2)->EnableWindow(FALSE);
   GetDlgItem(IDC_PROPERTIES)->EnableWindow(FALSE);
   GetDlgItem(IDC_RENAME)->EnableWindow(FALSE);   
   GetDlgItem(IDC_INSTALL)->EnableWindow(!IsInstalled());      
   }

void CDevListPage::OnSetfocusDevlistInstalled() 
   {
   // de-select other list
   ((CListBox*)GetDlgItem(IDC_DEVLIST_AVAILABLE))->SetCurSel(-1);
   // set buttons   
   GetDlgItem(IDC_INSTALL)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_DELETE)->ShowWindow(SW_SHOW);
   GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);        
   GetDlgItem(IDC_CONNECT2)->EnableWindow(TRUE);
   GetDlgItem(IDC_PROPERTIES)->EnableWindow(TRUE);   
   GetDlgItem(IDC_RENAME)->EnableWindow(TRUE);   
   GetDlgItem(IDC_INSTALL)->EnableWindow(FALSE);      
   }

void CDevListPage::OnSelchangeDevlistInstalled() 
   {
   }

void CDevListPage::OnSelchangeDevlistAvailable() 
   {
   GetDlgItem(IDC_INSTALL)->EnableWindow(!IsInstalled());   
   }

DWORD CDevListPage::GetInstalledDevices() 
   {
   LPPRINTER_INFO_2 pi;      
   DWORD dwSizeNeeded, dwNumPrinters;
   DWORD i;
   int cnt;
   CString cs, csDevName, csTemp;
   CListBox* pCL;      
   HANDLE hPrinter;   
   LPDRIVER_INFO_3 di;
   DWORD dwRequired;
   CString csMonitor;
   CDeviceEntry* cde;   

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
   DestroyInstalledList();
   // get list
   pCL = (CListBox*)GetDlgItem(IDC_DEVLIST_INSTALLED);      
   // monitor
   csMonitor.LoadString(IDS_DEVICE_LANG_MONITOR_DESC);
   // look for our monitor
   for (i = 0, cnt = 0; i < dwNumPrinters; i++)
      {	
      // get printer  
	  if (::OpenPrinter((pi + i)->pPrinterName, &hPrinter, NULL))
	     {
		 // get the driver
		 dwRequired = 0;
		::GetPrinterDriver(hPrinter, NULL, 3, NULL, 0, &dwRequired);
		 if (dwRequired)
		    {
		    di = (LPDRIVER_INFO_3)new char [dwRequired];
		    if (::GetPrinterDriver(hPrinter, NULL, 3, 		 
		       (LPBYTE)di, dwRequired, &dwRequired))
			   {
			   // right monitor
			   if (di->pMonitorName)
	              {
	              if (csMonitor.Compare(di->pMonitorName) == 0)
	                 {
					 // make new dev entry
					 cde = new CDeviceEntry;
					 cde->Name = (pi + i)->pPrinterName;
					 cde->Port = (pi + i)->pPortName;
					 cde->Idx = 0; // not used
					 // make device string name
					 csTemp.LoadString(IDS_DEVICE_DEVONPORT);
					 csDevName.Format(csTemp, (const char*)cde->Name, (const char*)cde->Port);
		             // insert it into list
		             //pCL->InsertString(-1, csDevName); 					 
					 //pCL->SetItemDataPtr(cnt++, (void*)cde);
					 pCL->AddString(csDevName);       
 	                 pCL->SetItemDataPtr(pCL->FindString(-1, csDevName), 
					    (void*)cde);
					 cnt++;	  
					 }		 	     
		          }
			   }
			delete di;
			}
		 ::ClosePrinter(hPrinter);
		 }
	  }
   // delete the printer buffer
   delete pi;
   return dwNumPrinters;
   }

void CDevListPage::OnDblclkDevlistAvailable() 
   {
   OnInstall();
   }

void CDevListPage::OnProperties() 
   {      
   int idx;
   CListBox* cls;   
   HANDLE hPrinter;   
   CDeviceEntry* cde;   

   // get current selection
   cls = (CListBox*)GetDlgItem(IDC_DEVLIST_INSTALLED);
   idx = cls->GetCurSel();
   if (idx == LB_ERR) return;

   // get device entry
   cde = (CDeviceEntry*)cls->GetItemDataPtr(idx);      
   
   if (cde->Name.GetLength())
      {
	  // delete the printer
      if (::OpenPrinter((char*)(const char*)cde->Name, &hPrinter, NULL))
	     {
	     if (hPrinter) 
	        {
	        PrinterProperties(GetSafeHwnd(), hPrinter);			
			}
		 if (hPrinter) ::ClosePrinter(hPrinter);
		 }
	  }  												
   }
														
void CDevListPage::SetOurPort(CDeviceEntry* cde)
   {   
   CDlgWait dlg;   
   HANDLE hPrinter;   
   PRINTER_INFO_2* pi2;
   DWORD cbNeeded;
   LPBYTE lpBuf;   

   // create the wait dlg
   dlg.m_prompt.LoadString(IDS_CHANGING_CONNECTION);      
   dlg.Create(IDD_MODELESS_WAIT, this);   

   PRINTER_DEFAULTS pd;
   pd.pDatatype = NULL;
   pd.pDevMode = NULL;
   pd.DesiredAccess = PRINTER_ALL_ACCESS;
   
   // must tell windows!!!
   if (::OpenPrinter((char*)(const char*)cde->Name, &hPrinter, &pd))
      {
   	  if (hPrinter) 
	     {
	  	 ::GetPrinter(hPrinter, 2, NULL, 0, &cbNeeded);
		 // make the buffer equal to the old plus the new port info
		 lpBuf = (LPBYTE)(new char[cbNeeded + cde->Port.GetLength() + 1]);
		 if (::GetPrinter(hPrinter, 2, lpBuf, cbNeeded, &cbNeeded))
		    {
		    // copy the new port
		    strcpy((char*)(lpBuf + cbNeeded), (const char*)cde->Port);
		    // point to the buffer
		    pi2 = (PRINTER_INFO_2*)lpBuf;
		    // set the new port	
		    pi2->pPortName = (LPSTR)(lpBuf + cbNeeded);
		    // do it
		    ::SetPrinter(hPrinter, 2, lpBuf, 0);

		    // update installed devices
            GetInstalledDevices();	  
		    }
		 delete lpBuf;
		 ::ClosePrinter(hPrinter);
		 }	  
      }
   // destroy the wait dlg
   dlg.DestroyWindow();   
   }
	  
BOOL CDevListPage::IsInstalled()
   {
   CListBox* cls;   
   CDeviceEntry* cde;
   CString csDevName, csTemp;
   int idx;   

   cls = (CListBox*)GetDlgItem(IDC_DEVLIST_AVAILABLE);

   if ((idx = cls->GetCurSel()) == LB_ERR) return FALSE;

   // get current selection   
   cde = (CDeviceEntry*)cls->GetItemDataPtr(idx);

   csTemp.LoadString(IDS_DEVICE_DEVONPORT);
   csDevName.Format(csTemp, (const char*)cde->Name, (const char*)cde->Port);

   cls = (CListBox*)GetDlgItem(IDC_DEVLIST_INSTALLED);
   // already installed?
   if (cls->FindStringExact(-1, csDevName) != LB_ERR)	
      return TRUE;
   else
      return FALSE;
   }

void CDevListPage::DestroyInstalledList() 
   {
   int i;   	
   CDeviceEntry* cde;	  

   CListBox* cls = (CListBox*)GetDlgItem(IDC_DEVLIST_INSTALLED);

   for (i = 0; i < cls->GetCount(); i++)
      {
	  // delete device entry
	  cde = (CDeviceEntry*)(cls->GetItemDataPtr(i));
      if (cde) delete cde;
	  }
   // reset all
   cls->ResetContent();
   }

void CDevListPage::OnDestroy() 
   {
   DestroyInstalledList();
   CPropertyPage::OnDestroy();
   }

void CDevListPage::OnControlPanel() 
   {
   STARTUPINFO StartupInfo;
   PROCESS_INFORMATION ProcessInfo;   
   CString cs;

   // NULL the StartupInfo struct
   memset(&StartupInfo, 0, sizeof(STARTUPINFO));
   // the size
   StartupInfo.cb = sizeof(STARTUPINFO);   
   
   cs.LoadString(IDS_CONTROL_PRINTERS);

   // launch the control panel
   if (!CreateProcess(NULL, cs.GetBuffer(cs.GetLength()), NULL, NULL, FALSE, 
      NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &ProcessInfo))
  	  {
	  AfxMessageBox(IDS_NO_LOAD_CONTROL_PANEL);
	  }		
   cs.ReleaseBuffer();
   }

void CDevListPage::OnOurok() 
   {
   GetParent()->SendMessage(PSM_PRESSBUTTON, PSBTN_OK);
   ((CPropertySheet*)GetParent())->EndDialog(IDOK);   
   }

void CDevListPage::OnOK() 
   {   
   // have we made re-bootable changes?
   if (NeedToReboot) AfxMessageBox(IDS_NEEDTOREBOOT);   
   CPropertyPage::OnOK();
   }

BOOL CDevListPage::OnHelpInfo(HELPINFO* pHelpInfo) 
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
 
void CDevListPage::OnContextMenu(CWnd* pWnd, CPoint point) 
   {   
   // call context help
   ::WinHelp(pWnd->m_hWnd, AfxGetApp()->m_pszHelpFilePath, 
      HELP_CONTEXTMENU,  (DWORD)(LPVOID)dwHelpMap);
   }

/* 

is this the share answer?

this restarts the shell - Q137572

  HWND hwndShell = FindWindow("Progman", NULL);
   PostMessage(hwndShell, WM_QUIT, 0, 0L);
   WinExec("Explorer.exe",SW_SHOW);
 
*/

void CDevListPage::OnConnect() 
   {
   CListBox* cls;
   CDevConnect dlg;
   int idx;			 
   CDeviceEntry* cde;

   // get current selection
   cls = (CListBox*)GetDlgItem(IDC_DEVLIST_INSTALLED);
   idx = cls->GetCurSel();
   if (idx == LB_ERR) return;

   // get device entry
   cde = (CDeviceEntry*)(cls->GetItemDataPtr(idx));
   dlg.m_csPort = cde->Port;

   if (dlg.DoModal()) 
      {
	  cde->Port = dlg.m_csPort;
	  SetOurPort(cde);
	  // select the device again
	  if (idx > 0)
	     cls->SetCurSel(idx);
	  else
   	     cls->SetCurSel(0);		 
	  }
   }

void CDevListPage::OnRename() 
   {
   CDlgWait dlg;   
   HANDLE hPrinter;   
   PRINTER_INFO_2* pi2;
   DWORD cbNeeded;
   LPBYTE lpBuf;   
   CPrinterNameChange cpnchg;
   CListBox* cls;
   int idx;			 
   CDeviceEntry* cde;

   // get current selection
   cls = (CListBox*)GetDlgItem(IDC_DEVLIST_INSTALLED);
   idx = cls->GetCurSel();
   if (idx == LB_ERR) return;

   // get device entry
   cde = (CDeviceEntry*)(cls->GetItemDataPtr(idx));
   
   // prompt for new name
   cpnchg.m_oldname = cde->Name;
   cpnchg.m_newname = cde->Name;
   // cancelled!
   if (cpnchg.DoModal() == IDCANCEL) return;
   // same name
   if (cpnchg.m_oldname == cpnchg.m_newname) return;

   // create the wait dlg
   dlg.m_prompt.LoadString(IDS_CHANGING_NAME);      
   dlg.Create(IDD_MODELESS_WAIT, this);   

   PRINTER_DEFAULTS pd;
   pd.pDatatype = NULL;
   pd.pDevMode = NULL;
   pd.DesiredAccess = PRINTER_ALL_ACCESS;
   
   // must tell windows!!!
   if (::OpenPrinter((char*)(const char*)cde->Name, &hPrinter, &pd))
      {
   	  if (hPrinter) 
	     {
	  	 ::GetPrinter(hPrinter, 2, NULL, 0, &cbNeeded);
		 // make the buffer equal to the old plus the new port info
		 lpBuf = (LPBYTE)(new char[cbNeeded + cpnchg.m_newname.GetLength() + 1]);
		 if (::GetPrinter(hPrinter, 2, lpBuf, cbNeeded, &cbNeeded))
		    {
		    // copy the new name
		    strcpy((char*)(lpBuf + cbNeeded), (const char*)cpnchg.m_newname);
		    // point to the buffer
		    pi2 = (PRINTER_INFO_2*)lpBuf;
		    // set the new port	
		    pi2->pPrinterName = (LPSTR)(lpBuf + cbNeeded);
		    // do it
		    ::SetPrinter(hPrinter, 2, lpBuf, 0);

		    // update installed devices
            GetInstalledDevices();	  
	        
			// select the device again
	        if (idx > 0)
	           cls->SetCurSel(idx);
	        else
   	           cls->SetCurSel(0);		 
		    }
		 delete lpBuf;
		 ::ClosePrinter(hPrinter);
		 }	  
      }
   // destroy the wait dlg
   dlg.DestroyWindow();   
   }
