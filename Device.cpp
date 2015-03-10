/////////////////////////////////////////////////////////////////////////////
// DEVICE.CPP
//                                                                        
// device class methods and "C" wrapper procs for RasterPlus
// Copyright (C) 1996-2000 Graphx, Inc.
//
// Note: we convert c++ class calls to std c calls so that we 
// can transport across a static MFC dll layer
//
// mods:
//
// 10-15-96 Build 45 - added special NT memory handling
// 1-6-98   Version 2.0.1 - auto memory check is now the same under NT as 95
//                          because we fixed the nt mem problem
// 1-22-98  Version 2.0.1 - added NT-specific DLL specs
// 5-29-98  Version 2.0.4 - added more error checking for missing devices
// 6-12-98  Version 2.0.4 - pDevInfo->MemUsed calculation for AUTO wrong! - fixed
// 6-26-98  Version 2.0.4 - leave more memory to system in Automatic mode
// 9-10-98  Version 2.0.5 - added new device rip loading code (disabled for now)
// 10-20-98 Version 2.0.5 - Added FindConnection support
// 12-04-98 Version 2.0.5 - added UpdateMediaList
// 12-02-99 Version 3.0.2 - added GetCurrentMediaDescription
// 2-20-00  Version 3.0.3 - added UpdateConnectList
// 5-05-00  Version 3.0.3 - NULL pDevInfo after freeing
// 5-12-00  Version 3.0.3 - added UpdateResList
// 7-12-00  Version 4.0.0 - Fixed friendly name not being reset on startup
// 8-04-00  Version 4.0.0 - put a NormalizeRect in GetCurrentMediaRect
// 10-12-00 Version 4.0.1 - Implement late-loading device strategy for faster queue loading
// 12-07-00 Version 4.0.2 - make sure procs are loaded before calling them
// 11-30-01 Version 5.0.1 - Remove/Disable Dongle Code
// 01-30-02 Version 5.0.2 - Fix Intermittent Multi Queue DLS and Gang Print problem
// 06-28-02 Version 5.0.6 - Change Memory Allocation to not adjust Custom Size based on Number of queues
// 10-24-02 Version 5.1.0 - Remove limitation on maximum memory - Move limitation to individual drivers
// 04-02-03 Version 5.1.1 - Fix Queue Information changing between queues after accessing Adv Dev in Job Properties
//

#include"stdafx.h"
#include "zeus.h"
#include"resource.h"       
#include"device.h"
#include"devface.h"
#include"profile.h"
#include"DeviceList.h"
#include"sysmem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// if we are late loading the device, load it before we do anything else
#define VERIFYDEVLOAD() {if (bLoadLate) {LoadDevice(DevIdx);}}

CDevice::CDevice()
  {
  hInst = NULL;
  IsInit = FALSE;
  pDevInfo = NULL;
  pTempDevInfo = NULL;
  iDevInfoSize = 0;
  dwDevHandle = 0;
  DevIdx = DEVICE_IDX_NONE;
  bLoadLate = FALSE;
  PropBoxCreated = FALSE;
  // null these out for later check
   lpfn_NeedDongle = NULL;
   lpfn_FindConnection = NULL;
   lpfn_GetSetExtra = NULL;
   lpfn_AllocDevice = NULL;
   lpfn_FreeDevice = NULL;
   lpfn_GetDevInfo = NULL;
   lpfn_SetDevInfo = NULL;
   lpfn_GetMedia = NULL;
   lpfn_GetMediaBin = NULL;
   lpfn_OpenDevice = NULL;
   lpfn_OpenDeviceFromRip = NULL;
   lpfn_CloseDevice = NULL;
   lpfn_Abort = NULL;
   lpfn_OutputBytes = NULL;
   lpfn_GetDevError = NULL;
   lpfn_FillRipStruct = NULL;
   lpfn_DoDialog = NULL;
   lpfn_GetRes = NULL;
   lpfn_GetConnect = NULL;
   lpfn_ParseServerTokens = NULL;
   lpfn_MakeSPDFile = NULL;
   lpfn_CreatePSInit = NULL;
   lpfn_AlertModeChange = NULL;          
   lpfn_UpdateMediaList = NULL;          
   lpfn_UpdateResList = NULL;          
   lpfn_UpdateConnectList = NULL;          
   lpfn_DoPropBox = NULL;
   lpfn_SetDeviceWnd = NULL;
   lpfn_UpdateDevInfo = NULL;
   lpfn_UpdateDlgFromQueue = NULL;
#ifndef RP_6
   lpfn_LicenseManager = NULL;
#endif
#ifdef _PHZ_PRINT_SERVER_TEMP
   lpfn_PrintServerSetup = NULL;
#endif
#ifdef RP_6
 //  lpfn_InstallWizard = NULL;
#endif
  }

CDevice::~CDevice()
  {
  // just in case
  FreeDevice();
  }

BOOL CDevice::LoadDefaultDevice()
   {      
   CDeviceList cdl;

   // get all the devices
   cdl.LoadAll();

   // get default device index
   return LoadDevice(cdl.GetDefault());
   }

BOOL CDevice::LoadDevice(int idx, char* devname, char* dllname)
   {
   // set names
   Name = devname;
   csFriendlyName = devname;
   DLLName = dllname;

   // now load it
   return LoadDevice(idx, TRUE);
   }							

BOOL CDevice::LoadDevice(int idx, BOOL bHaveName)
   {
   BOOL ret;
   CProfile Profile;
   CString cs, csEntry;       
   
   // default
   ret = FALSE;   

   // if it's already initialized, (and it's the same device)
   // just reload the pDevInfo
   if (IsInit && idx == DevIdx && !bLoadLate) 
      {	  
   	  GetDevInfo();

	  return IsInit;	  
	  }

   // set the new index
   DevIdx = idx;

   if (!bHaveName)
	  {
      // special for preview
      if (DevIdx == PREVIEW_DEVICE_IDX)
		 {
	     Name.LoadString(IDS_PREVIEWDEV_NAME);
#ifdef _DEBUG	  	  
   	     DLLName = Profile.GetStr("Debug", "Root", "e:\\source\\");	  
	     if (!IS_OS_NT_FAMILY)
  	        DLLName += Profile.GetStr(IDS_PREVIEW, IDS_PREVIEWDLL_DEBUG);
	     else
		    DLLName += Profile.GetStr(IDS_PREVIEW, IDS_PREVIEWDLL_DEBUG_NT);
#else
	     DLLName = Profile.GetStr(IDS_PREVIEW, IDS_PREVIEWDLL);
#endif
		 }      
      else
		 {
	     CDeviceList cdl;
	     // get all devices
	     cdl.LoadAll();

		 // get device name from index
	     cdl.GetDevName(idx, Name);

	     // get device dll from index
	     cdl.GetDevDLL(idx, DLLName);
		 }
	  }

   // no stupid error boxes
   UINT OldErrorMode = ::SetErrorMode(SEM_NOOPENFILEERRORBOX);

   // default
   IsInit = FALSE;

   // load the library   
   if (hInst = ::LoadLibrary(DLLName))   
      {	  
	  // get all the procs
      lpfn_AllocDevice = 
        (DWORD(WINAPIV *)(const char*, int))::GetProcAddress(hInst, "AllocDevice");         
      lpfn_FreeDevice = 
        (void(WINAPIV *)(DWORD))::GetProcAddress(hInst, "FreeDevice");
	  lpfn_GetDevInfo = 
	     (int(WINAPIV *)(DWORD, PDEVINFO))::GetProcAddress(hInst, "GetDevInfo");
	  lpfn_SetDevInfo = 
	     (void(WINAPIV *)(DWORD, PDEVINFO, BOOL))::GetProcAddress(hInst, "SetDevInfo");
	  lpfn_GetMedia = 
         (BOOL (WINAPIV *)(DWORD, int, PMEDIA))::GetProcAddress(hInst, "GetMedia");
      lpfn_GetMediaBin = 
         (BOOL (WINAPIV *)(DWORD, int, char*))::GetProcAddress(hInst, "GetMediaBin");
      lpfn_GetRes = 
         (BOOL (WINAPIV *)(DWORD, int, char*))::GetProcAddress(hInst, "GetRes");
      lpfn_GetConnect = 
         (BOOL (WINAPIV *)(DWORD, int, char*, int*))::GetProcAddress(hInst, "GetConnect");
	  lpfn_OpenDevice = 
         (BOOL (WINAPIV *)(DWORD, PRIPSTRUCT, PJOBINFO, int*))::GetProcAddress(hInst, "OpenDevice");
	  lpfn_OpenDeviceFromRip = 
		 (BOOL (WINAPIV *)(DWORD, PRIPSTRUCT, PJOBINFO, PIPCDEVINFO))::GetProcAddress(hInst, "OpenDeviceFromRip");
	  lpfn_CloseDevice = 
          (void (WINAPIV *)(DWORD))::GetProcAddress(hInst, "CloseDevice");   
	  lpfn_OutputBytes = 
          (int (WINAPIV *)(DWORD, BYTE*, int, int))::GetProcAddress(hInst, "OutputBytes");   
	  lpfn_Abort =
          (void (WINAPIV *)(DWORD))::GetProcAddress(hInst, "Abort");   
      lpfn_GetDevError =
          (BOOL (WINAPIV *)(DWORD, char *))::GetProcAddress(hInst, "GetDevError");   
	  lpfn_DoDialog = 
          (int (WINAPIV *)(DWORD, HWND))::GetProcAddress(hInst, "DoDialog");   
      lpfn_FillRipStruct = 
	      (BOOL (WINAPIV *)(DWORD, PRIPSTRUCT))::GetProcAddress(hInst, "FillRipStruct");   
      lpfn_ParseServerTokens = 
	      (void (WINAPIV *)(DWORD, const char*, const char*))::GetProcAddress(hInst, "ParseServerTokens");   
   	  lpfn_MakeSPDFile = 
	      (BOOL (WINAPIV *)(DWORD, const char*, const char*))::GetProcAddress(hInst, "MakeSPDFile");   
	  lpfn_CreatePSInit = 
	      (int (WINAPIV *)(DWORD, BYTE*))::GetProcAddress(hInst, "CreatePSInit");   
	  lpfn_AlertModeChange = 
	      (BOOL (WINAPIV *)(DWORD, int, PDEVINFO))::GetProcAddress(hInst, "AlertModeChange");   
	  // these procs may not be in all devices!
	  lpfn_NeedDongle = 
	      (BOOL (WINAPIV *)(DWORD))::GetProcAddress(hInst, "NeedDongle");   
	  lpfn_FindConnection = 
		 (BOOL (WINAPIV *)(DWORD, char*, int*))::GetProcAddress(hInst, "FindConnection");   		 
	  lpfn_GetSetExtra = 
		 (BOOL (WINAPIV *)(DWORD, int, BYTE*, BYTE*, int, int))::GetProcAddress(hInst, "GetSetExtra");   		 
	  lpfn_UpdateMediaList = 
		 (int (WINAPIV *)(DWORD))::GetProcAddress(hInst, "UpdateMediaList");   		 
	  lpfn_UpdateResList = 
		 (int (WINAPIV *)(DWORD))::GetProcAddress(hInst, "UpdateResList");   		 
	  lpfn_UpdateConnectList = 
		 (int (WINAPIV *)(DWORD))::GetProcAddress(hInst, "UpdateConnectList");   		 
	  lpfn_DoPropBox = 
		 (BOOL (WINAPIV *)(DWORD, int, CWnd*))::GetProcAddress(hInst, "DoPropBox");
	  lpfn_SetDeviceWnd = 
		 (void (WINAPIV *)(DWORD, HWND))::GetProcAddress(hInst, "SetDeviceWnd");
	  lpfn_UpdateDevInfo = 
		 (void (WINAPIV *)(DWORD, PDEVINFO*, int, int))::GetProcAddress(hInst, "UpdateDevInfo");
	  lpfn_UpdateDlgFromQueue = 
		 (void (WINAPIV *)(DWORD, PDEVINFO*, int))::GetProcAddress(hInst, "UpdateDlgFromQueue");
#ifndef RP_6
	  lpfn_LicenseManager = 
	     (void(WINAPIV *)(DWORD))::GetProcAddress(hInst, "LicenseManager");
#endif
#ifdef _PHZ_PRINT_SERVER_TEMP
	  lpfn_PrintServerSetup =
  	     (void(WINAPIV *)(DWORD))::GetProcAddress(hInst, "PrintServerSetup");
#endif
#ifdef RP_6
//	  lpfn_InstallWizard = 
//	     (void(WINAPIV *)(DWORD))::GetProcAddress(hInst, "InstallWizard");
#endif

      // check procs
      if (lpfn_AllocDevice && lpfn_FreeDevice && lpfn_GetDevInfo && 
          lpfn_SetDevInfo && lpfn_GetMedia && lpfn_GetMediaBin && lpfn_OpenDevice && 
		  lpfn_OpenDeviceFromRip &&
		  lpfn_CloseDevice && lpfn_Abort && lpfn_OutputBytes &&
		  lpfn_GetDevError && lpfn_FillRipStruct && lpfn_DoDialog &&
		  lpfn_GetRes && lpfn_GetConnect && lpfn_ParseServerTokens && lpfn_MakeSPDFile &&
#ifdef _PHZ_PRINT_SERVER_TEMP
		  lpfn_CreatePSInit && lpfn_AlertModeChange && lpfn_LicenseManager && lpfn_PrintServerSetup )
#endif
#ifdef RP_6
//		  lpfn_CreatePSInit && lpfn_AlertModeChange && lpfn_LicenseManager && lpfn_InstallWizard )
		  lpfn_CreatePSInit && lpfn_AlertModeChange )
#endif
#ifndef _PHZ_PRINT_SERVER_TEMP
#ifndef RP_6
		  lpfn_CreatePSInit && lpfn_AlertModeChange && lpfn_LicenseManager )
#endif
#endif
		 {		 
		 // try to allocate the device	     
		 if (dwDevHandle = lpfn_AllocDevice((const char*)Name, DevIdx))
			IsInit = TRUE;			
		 }
	  else
	     {
	     ::FreeLibrary(hInst);
		 hInst = NULL;
	     }	  
      }
	  else
	  {
		  DWORD dwLastError = GetLastError();

		    // ::LoadLibrary Error
		  if ( FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwLastError, 0, cDebugStr,
																sizeof(cDebugStr), NULL ) == 0 )
		  {
		    csRPDebugStr.Format("LoadLibrary Failure = %s\n", cDebugStr);
		  }
		  else
		  {
		    csRPDebugStr.Format("LoadLibrary %s Failure\n", "Unknown");
		  }
	  }

   // reset stupid error boxes
   ::SetErrorMode(OldErrorMode);                           

   // default
   if ( csFriendlyName.IsEmpty() )
	   csFriendlyName = Name;

   // force the creation of the mapped mem based on
   // the devid
   if (IsInit)
	  {
	  char strDevId[32];
	  itoa(DevIdx, strDevId, 10); 
	  TimeCheck.SetCheck(strDevId);
	  }

   if (IsInit && !pDevInfo)
      {
	  // get the device info size
      iDevInfoSize = GetDevInfoSize();
      // allocate the buffer
	  if (iDevInfoSize > 0)
	     {		 
		 // get the device info
         if (pDevInfo = (PDEVINFO)(new BYTE[iDevInfoSize]))         
			{
			ZeroMemory(pDevInfo, iDevInfoSize);
            GetDevInfo();	            
			}
		 else										 
		    iDevInfoSize  = 0;		 
		 }	  
	  }   
   bLoadLate = FALSE;
   return IsInit;      
   }

BOOL CDevice::operator!=(const CDevice& cd)
   {
   // is same thing?
   return (!IsSame(cd));
   }

BOOL CDevice::operator==(const CDevice& cd)
   {
   // is same thing?
   return (IsSame(cd));
   }

BOOL CDevice::IsSame(const CDevice& cd)
   {                                                           
   // make sure that we have a dll
   if (!hInst || !cd.hInst) return FALSE;
   
   // make sure that both devices are initialized
   if (!IsInit || !cd.IsInit) return FALSE;

   // make sure that we have the devinfo
   if (!pDevInfo || !cd.pDevInfo) return FALSE;

   // make sure they are the same size
   if (iDevInfoSize != cd.iDevInfoSize) return FALSE;

   // make sure they are the same name
   if (Name != cd.Name) return FALSE;

   // make sure they are the same name
   if (csFriendlyName != cd.csFriendlyName) return FALSE;

   // make sure they have the same index
   if (DevIdx != cd.DevIdx) return FALSE;

   // make sure that the devinfos are the same
   if (memcmp(pDevInfo, cd.pDevInfo, iDevInfoSize) != 0) return FALSE;
   
   // well, I guess they are the same 
   return TRUE;
   }

const CDevice& CDevice::operator=(const CDevice& cdSrc) // assignment	  
   {        
   if (this != &cdSrc)
      {        
      // call FreeDevice in case the device is already loaded	  
	  FreeDevice();
	  // load the device based on the new idx
	  ASSERT(cdSrc.DevIdx != DEVICE_IDX_NONE);	  
#if 0
	  // copy over the devinfo
	  if (LoadDevice(cdSrc.DevIdx))	  
		 {
	     memcpy(pDevInfo, cdSrc.pDevInfo, iDevInfoSize);		 
		 csFriendlyName = cdSrc.csFriendlyName;
		 }
#endif
	  iDevInfoSize = cdSrc.iDevInfoSize;
	  Name = cdSrc.Name;		 
	  DevIdx = cdSrc.DevIdx;
	  csFriendlyName = cdSrc.csFriendlyName;

	  if (pDevInfo = (PDEVINFO)(new BYTE[iDevInfoSize]))
		 {
		 memcpy(pDevInfo, cdSrc.pDevInfo, iDevInfoSize);		 
		 }		 
	  // we haven't loaded the device yet!
	  bLoadLate = TRUE;
	  }				  	  
   return *this;      
   }              

void CDevice::FreeDevice()
   {   
   // free the actual device
   if (dwDevHandle) lpfn_FreeDevice(dwDevHandle);

   // delete our DevInfo   
   if (pDevInfo) delete pDevInfo;
   if (pTempDevInfo) delete pTempDevInfo;

   // free the library   
   if (hInst) ::FreeLibrary(hInst);      
   // zero everything out
   dwDevHandle = 0;
   iDevInfoSize = 0	;
   hInst = NULL;
   IsInit = FALSE;
   DevIdx = DEVICE_IDX_NONE;   
   pDevInfo = NULL;
   pTempDevInfo = NULL;
   // null these out for later check
   lpfn_NeedDongle = NULL;
   lpfn_FindConnection = NULL;
   lpfn_GetSetExtra = NULL;
   lpfn_AllocDevice = NULL;
   lpfn_FreeDevice = NULL;
   lpfn_GetDevInfo = NULL;
   lpfn_SetDevInfo = NULL;
   lpfn_GetMedia = NULL;
   lpfn_GetMediaBin = NULL;
   lpfn_OpenDevice = NULL;
   lpfn_OpenDeviceFromRip = NULL;
   lpfn_CloseDevice = NULL;
   lpfn_Abort = NULL;
   lpfn_OutputBytes = NULL;
   lpfn_GetDevError = NULL;
   lpfn_FillRipStruct = NULL;
   lpfn_DoDialog = NULL;
   lpfn_GetRes = NULL;
   lpfn_GetConnect = NULL;
   lpfn_ParseServerTokens = NULL;
   lpfn_MakeSPDFile = NULL;
   lpfn_CreatePSInit = NULL;
   lpfn_AlertModeChange = NULL;          
   lpfn_UpdateMediaList = NULL;          
   lpfn_UpdateResList = NULL;          
   lpfn_UpdateConnectList = NULL;          
   lpfn_DoPropBox = NULL;
   lpfn_SetDeviceWnd = NULL;
   lpfn_UpdateDevInfo = NULL;
   lpfn_UpdateDlgFromQueue = NULL;
#ifndef RP_6
   lpfn_LicenseManager = NULL;
#endif
#ifdef _PHZ_PRINT_SERVER_TEMP
   lpfn_PrintServerSetup = NULL;
#endif
#ifdef RP_6
//   lpfn_InstallWizard = NULL;
#endif
   }

void CDevice::SetDevInfo(BOOL bCommit)
   {  
   VERIFYDEVLOAD();
   if (lpfn_SetDevInfo) lpfn_SetDevInfo(dwDevHandle, pDevInfo, bCommit);
   }

void CDevice::SetDevInfo(PDEVINFO pd, BOOL bCommit)
   {  
   VERIFYDEVLOAD();
   if (lpfn_SetDevInfo) lpfn_SetDevInfo(dwDevHandle, pd, bCommit);
   }

#ifndef RP_6
void CDevice::LicenseManager()
   {  
   VERIFYDEVLOAD();
   if (lpfn_LicenseManager) lpfn_LicenseManager(dwDevHandle);
   }
#endif

#ifdef _PHZ_PRINT_SERVER_TEMP
void CDevice::PrintServerSetup()
{
	VERIFYDEVLOAD();

	if (lpfn_PrintServerSetup)
		lpfn_PrintServerSetup(dwDevHandle);
}
#endif
#ifdef RP_6
/*
void CDevice::InstallWizard()
{  
	VERIFYDEVLOAD();

	if (lpfn_InstallWizard)
		lpfn_InstallWizard(dwDevHandle);
}
*/ 
#endif

int CDevice::GetDevInfo()
   {  
   int ret;   

   VERIFYDEVLOAD();

   if (lpfn_GetDevInfo) 
	  ret = lpfn_GetDevInfo(dwDevHandle, pDevInfo);
   else
	  ret = 0;

   if (!pDevInfo) return TRUE;

   // set memory
   SetMem();

   return ret;	     
   }

void CDevice::SetMem()
   {
   SysMem sm;   

   // set memory based on installed mem
   switch(pDevInfo->MemUsedIdx)
	  {
	  case RIPMEM_ALLOC_WORKSTATION:
		   pDevInfo->MemUsed = (int)((float)sm.InstalledMemory * .25F + .5F);
		   break;
#if 0
	  case RIPMEM_ALLOC_SERVER_1D:
		   pDevInfo->MemUsed = (int)((float)sm.InstalledMemory * .75F + .5F);
		   break;
	  case RIPMEM_ALLOC_SERVER_2D:
		   pDevInfo->MemUsed = (int)((float)sm.InstalledMemory * .45F + .5F);
		   break;
	  case RIPMEM_ALLOC_SERVER_3D:
		   pDevInfo->MemUsed = (int)((float)sm.InstalledMemory * .25F + .5F);
		   break;
	  case RIPMEM_ALLOC_SERVER_4D:
		   pDevInfo->MemUsed = (int)((float)sm.InstalledMemory * .20F + .5F);
		   break;
#endif
	  case RIPMEM_ALLOC_SERVER:
		   pDevInfo->MemUsed = (int)((float)sm.InstalledMemory * .75F + .5F);
		   break;
	  case RIPMEM_ALLOC_CUSTOM:
		   // already set
	       break;
	  }			  

   // modify by the queue count
   // if not custom
   if ( pDevInfo->MemUsedIdx != RIPMEM_ALLOC_CUSTOM )
   {
	   if (QueueCount.GetQueueCount() > 0)
		  pDevInfo->MemUsed /= QueueCount.GetQueueCount();
   }

   // no more than what device needs
   pDevInfo->MemUsed = min(pDevInfo->MemUsed, 
								(pDevInfo->BandSize / 1024) * pDevInfo->BandWidth);   

   // rip bounds
//   pDevInfo->MemUsed = max(min(DEVICE_MEMUSED_MAX, pDevInfo->MemUsed), DEVICE_MEMUSED_MIN);

//   TRACE("CDevice::SetMem, pDevInfo->MemUsed = %d\n", pDevInfo->MemUsed);
   }

int CDevice::GetDevInfoSize()
   {  
   VERIFYDEVLOAD();

   if (lpfn_GetDevInfo) 
	  return lpfn_GetDevInfo(dwDevHandle, NULL);
   else
	  return 0;
   }

BOOL CDevice::GetMedia(int idx, PMEDIA pMedia)
   {   
   VERIFYDEVLOAD();

   if (!lpfn_GetMedia) return FALSE;

   // defaults
   memset(pMedia->Name, 0, MAX_MEDIANAME);
   memset(pMedia->PPDName, 0, MAX_MEDIANAME);
   memset(pMedia->Description, 0, _MAX_PATH);

   pMedia->Rect.left = 0;
   pMedia->Rect.right = 0;
   pMedia->Rect.bottom = 0;
   pMedia->Rect.top = 0;
   
   return lpfn_GetMedia(dwDevHandle, idx, pMedia);
   }

int CDevice::UpdateMediaList()
   {
   VERIFYDEVLOAD();

   if (lpfn_UpdateMediaList)
      return lpfn_UpdateMediaList(dwDevHandle);
   else 
	  return 0;
   }

int CDevice::UpdateResList()
   {
   VERIFYDEVLOAD();

   if (lpfn_UpdateResList)
      return lpfn_UpdateResList(dwDevHandle);
   else 
	  return 0;
   }

int CDevice::UpdateConnectList()
   {
   VERIFYDEVLOAD();

   if (lpfn_UpdateConnectList)
      return lpfn_UpdateConnectList(dwDevHandle);
   else 
	  return 0;
   }

BOOL CDevice::DoPropBox(int cmd, CWnd* pCw)
   {
   VERIFYDEVLOAD();

   if (lpfn_DoPropBox)
      return lpfn_DoPropBox(dwDevHandle, cmd, pCw);
   else
	  return FALSE;
   }

void CDevice::SetDeviceWnd(HWND pwnd)
   {
   VERIFYDEVLOAD();

   if (lpfn_SetDeviceWnd)
      lpfn_SetDeviceWnd(dwDevHandle, pwnd);
   }

void CDevice::UpdateDevInfo(PDEVINFO* pinfo, int cnt, int item)
   {
   VERIFYDEVLOAD();

   if (lpfn_UpdateDevInfo)
      lpfn_UpdateDevInfo(dwDevHandle, pinfo, cnt, item);
   }

void CDevice::UpdateDlgFromQueue(PDEVINFO* pinfo, int cnt)
   {
   VERIFYDEVLOAD();

   if (lpfn_UpdateDevInfo)
      lpfn_UpdateDlgFromQueue(dwDevHandle, pinfo, cnt);
   }
	
BOOL CDevice::GetMediaBin(int idx, char* Name)
   {
   VERIFYDEVLOAD();

   if (lpfn_GetMediaBin)
      return lpfn_GetMediaBin(dwDevHandle, idx, Name);
   else
	  return FALSE;
   }

BOOL CDevice::GetRes(int idx, char* Name)
   {
   VERIFYDEVLOAD();

   if (lpfn_GetRes)
	  return lpfn_GetRes(dwDevHandle, idx, Name);
   else
	  return FALSE;
   }

BOOL CDevice::GetConnect(int idx, char* Name, int* pConnectType)
   {
   int* pC;
   int iDummy = CONNECT_TYPE_OTHER;

   VERIFYDEVLOAD();

   if (!lpfn_GetConnect) return FALSE;

   // unused?
   if (pConnectType == NULL)
	  pC = &iDummy;
   else
	  pC = pConnectType;
   return lpfn_GetConnect(dwDevHandle, idx, Name, pC);
   }

void CDevice::ParseServerTokens(CString& csToken, CString& csValue)
   {
   VERIFYDEVLOAD();

   if (lpfn_ParseServerTokens)
      lpfn_ParseServerTokens(dwDevHandle, (const char*)csToken, (const char*)csValue);
   }

BOOL CDevice::MakeSPDFile(CString& csSPDName, CString& csShortName)
   {
   VERIFYDEVLOAD();

   if (lpfn_MakeSPDFile)
      return lpfn_MakeSPDFile(dwDevHandle, (const char*)csSPDName, (const char*)csShortName);
   else
	  return FALSE;
   }

BOOL CDevice::OpenDevice(PRIPSTRUCT pRipStruct, PJOBINFO pJobInfo, int* pDevAbortFlag)
   {
   VERIFYDEVLOAD();

   if (lpfn_OpenDevice)
	  return lpfn_OpenDevice(dwDevHandle, pRipStruct, pJobInfo, pDevAbortFlag);
   else
	  return FALSE;
   }
   
BOOL CDevice::OpenDeviceFromRip(PRIPSTRUCT pRipStruct, PJOBINFO pJobInfo, PIPCDEVINFO pip)
   {
   VERIFYDEVLOAD();

   if (lpfn_OpenDeviceFromRip)
      return lpfn_OpenDeviceFromRip(dwDevHandle, pRipStruct, pJobInfo, pip);
   else
	  return FALSE;
   }
   
void CDevice::CloseDevice()
   {
   VERIFYDEVLOAD();

   if (lpfn_CloseDevice)
      lpfn_CloseDevice(dwDevHandle);
   }

void CDevice::Abort()
   {
   VERIFYDEVLOAD();

   if (lpfn_Abort)
      lpfn_Abort(dwDevHandle);
   }

int CDevice::CreatePSInit(BYTE* buf)
   {
   VERIFYDEVLOAD();

   if (lpfn_CreatePSInit)
      return lpfn_CreatePSInit(dwDevHandle, buf);
   else
	  return 0;
   }

BOOL CDevice::AlertModeChange(int mode)
   {
   VERIFYDEVLOAD();

   if (lpfn_AlertModeChange)
      return lpfn_AlertModeChange(dwDevHandle, mode, pDevInfo);
   else
	  return FALSE;
   }

int CDevice::OutputBytes(BYTE *buf, int len, int y)
   {
   VERIFYDEVLOAD();

   if (lpfn_OutputBytes)
      return lpfn_OutputBytes(dwDevHandle, buf, len, y);
   else
	  return 0;
   }

BOOL CDevice::GetDevError(char *pError)
   {
   VERIFYDEVLOAD();

   if (lpfn_GetDevError)
      return lpfn_GetDevError(dwDevHandle, pError);
   else
	  return FALSE;
   }

int CDevice::DoDialog(HWND hWnd)
   {
   VERIFYDEVLOAD();

   if (lpfn_DoDialog)
      return lpfn_DoDialog(dwDevHandle, hWnd);
   else
	  return 0;
   }
   
BOOL CDevice::FillRipStruct(PRIPSTRUCT pRipStruct)
   {
   VERIFYDEVLOAD();

   if (lpfn_FillRipStruct)
      return lpfn_FillRipStruct(dwDevHandle, pRipStruct);
   else
	  return FALSE;
   }
   
BOOL CDevice::NeedDongle()
{
/*
   VERIFYDEVLOAD();

   if (lpfn_NeedDongle)
      return lpfn_NeedDongle(dwDevHandle);
   else
	  return TRUE;
*/
	return FALSE;
}
   
BOOL CDevice::FindConnection(char* pConnectName, int* pHostAdapterID)
   {
   VERIFYDEVLOAD();

   if (lpfn_FindConnection)
      return lpfn_FindConnection(dwDevHandle, pConnectName, pHostAdapterID);
   else
	  return FALSE;
   }

BOOL CDevice::GetSetExtra(int cmd, BYTE* pIn, BYTE* pOut, int iLenIn, int iLenOut)
   {
   VERIFYDEVLOAD();

   if (lpfn_GetSetExtra)
      return lpfn_GetSetExtra(dwDevHandle, cmd, pIn, pOut, iLenIn, iLenOut);
   else
	  return FALSE;
   }


void CDevice::Serialize(CArchive& ar, WORD verCheck )
   {     
   WORD i;               
   BYTE* TempPtr;
   CString csTempFriendlyName;




   // note: we don't archive the hDriver or the procs, these must be 
   // instantiated after we have gotten the object off the disk
   if (ar.IsStoring())
      {
	  // write stuff
      ar << iDevInfoSize << Name << csFriendlyName << DevIdx;	        
      // if iDevInfoSize is not zero in length, store it
      if (iDevInfoSize > 0)
         {
         TempPtr = (BYTE*)pDevInfo;
         for (i = 0; i < iDevInfoSize; i++)
            ar << *TempPtr++;
         }   
	  }      
    else                 
	{              
      // set as default                                           
	  IsInit = FALSE;
	  // get stuff
      ar >> iDevInfoSize >> Name >> csTempFriendlyName >> DevIdx;
#if 0



	  // load the device to get other stuff
	  LoadDevice(DevIdx);




      // copy over saved pDevInfo
      if (iDevInfoSize > 0)
         {                     
         TempPtr = (BYTE*)pDevInfo;         
         if (TempPtr)
            {
            for (i = 0; i < iDevInfoSize; i++)
               ar >> *TempPtr++;
            }
         else
            iDevInfoSize  = 0; //whoops!
         }             
#endif
      // copy over saved pDevInfo
		if (iDevInfoSize > 0)
		{           
			if (!pDevInfo)
				pDevInfo = (PDEVINFO)(new BYTE[iDevInfoSize]);
			if (pDevInfo)
			{
				ZeroMemory(pDevInfo, iDevInfoSize);
				TempPtr = (BYTE*)pDevInfo;

				if (TempPtr)
				{
					// The first two bytes are a version number
					//
					for (i = 0; i < 2; i++)
						ar >> *TempPtr++;

					if ( pDevInfo->Version == CURRENT_DEVICE_VERSION )
					{
						// We have the current version so get the rest
						// of the information
						for (i; i < iDevInfoSize; i++)
							ar >> *TempPtr++;
					}
					else
					{
						// This is a newer version so we have to make sure
						// that we get everything and that it fits.


						// N.B. Right now we are just getting the ret of the information
						for (i; i < iDevInfoSize; i++)
							ar >> *TempPtr++;

						// Set the version to the new value
						pDevInfo->Version = CURRENT_DEVICE_VERSION;
					}
				}
				else
					iDevInfoSize  = 0; //whoops!
			}
			// we haven't loaded the device yet!
			bLoadLate = TRUE;
		}             
	}      

	  // Reset csFriendlyName
	  csFriendlyName = csTempFriendlyName;
   }   

/*   
void CDevice::Serialize(CArchive& ar, WORD verCheck )
   {     
   WORD i;               
   BYTE* TempPtr;
   CString csTempFriendlyName;

   int iOldDevInfoSize = 0;
   int iNewDevInfoSize = 0;

   // note: we don't archive the hDriver or the procs, these must be 
   // instantiated after we have gotten the object off the disk
   if (ar.IsStoring())
      {
	  // write stuff
      ar << iDevInfoSize << Name << csFriendlyName << DevIdx;	        
      // if iDevInfoSize is not zero in length, store it
      if (iDevInfoSize > 0)
         {
         TempPtr = (BYTE*)pDevInfo;
         for (i = 0; i < iDevInfoSize; i++)
            ar << *TempPtr++;
         }   
	  }      
    else                 
	{              
      // set as default                                           
	  IsInit = FALSE;
	  // get stuff
      ar >> iDevInfoSize >> Name >> csTempFriendlyName >> DevIdx;

	  // Save the Dev Info Size
	  iOldDevInfoSize = iDevInfoSize;

	  // load the device to get other stuff
	  LoadDevice(DevIdx);
	  iNewDevInfoSize = GetDevInfoSize();

//	  iDevInfoSize = iOldDevInfoSize;
#if 0
      // copy over saved pDevInfo
      if (iDevInfoSize > 0)
         {                     
         TempPtr = (BYTE*)pDevInfo;         
         if (TempPtr)
            {
            for (i = 0; i < iDevInfoSize; i++)
               ar >> *TempPtr++;
            }
         else
            iDevInfoSize  = 0; //whoops!
         }             
#endif
      // copy over saved pDevInfo
		if (iDevInfoSize > 0)
		{           
			if (!pDevInfo)
				pDevInfo = (PDEVINFO)(new BYTE[iDevInfoSize]);
			if (pDevInfo)
			{
				ZeroMemory(pDevInfo, iDevInfoSize);
				TempPtr = (BYTE*)pDevInfo;

				if (TempPtr)
				{
					// The first two bytes are a version number
					//
					for (i = 0; i < 2; i++)
						ar >> *TempPtr++;

					if ( pDevInfo->Version == CURRENT_DEVICE_VERSION )
					{
						// We have the current version so get the rest
						// of the information
						for (i; i < iOldDevInfoSize; i++)
							ar >> *TempPtr++;
					}
					else
					{
						// This is a newer version so we have to make sure
						// that we get everything and that it fits.


						// N.B. Right now we are just getting the ret of the information
						for (i; i < iOldDevInfoSize; i++)
							ar >> *TempPtr++;

						// Set the version to the new value
						pDevInfo->Version = CURRENT_DEVICE_VERSION;
					}
				}
				else
					iDevInfoSize  = 0; //whoops!
			}
			// we haven't loaded the device yet!
			bLoadLate = FALSE;
		}             
	}      

	  // Reset csFriendlyName
	  csFriendlyName = csTempFriendlyName;
   }   
*/

void CDevice::GetCurrentMedia(MEDIA& Media)
   {      
   VERIFYDEVLOAD();

   if (!lpfn_GetMedia) return;

   // defaults
   memset(Media.Name, 0, MAX_MEDIANAME);
   memset(Media.PPDName, 0, MAX_MEDIANAME);
   memset(Media.Description, 0, _MAX_PATH);

   Media.Rect.left = 0;
   Media.Rect.right = 0;
   Media.Rect.bottom = 0;
   Media.Rect.top = 0;

   // get the current media
   lpfn_GetMedia(dwDevHandle, pDevInfo->CurrentMediaIdx, &Media);
   }   

void CDevice::GetCurrentMediaRect(CRect& cr)
   {  
   MEDIA Media;   

   // get the current media
   GetCurrentMedia(Media);
   // set the imaging rect
   cr = Media.Rect;	  	  
   cr.NormalizeRect();
   }
   
void CDevice::GetCurrentMediaName(CString& csName)
   {
   MEDIA Media;   

   // default
   csName = "";   

   // get the current media
   GetCurrentMedia(Media);
   // set the imaging rect
   csName = Media.Name;	  	     
   }

void CDevice::GetCurrentPPDMediaName(CString& csName)
   {
   MEDIA Media;   

   // default
   csName = "";   

   // get the current media
   GetCurrentMedia(Media);
   // set the imaging rect
   csName = Media.PPDName;	  	     
   }

void CDevice::GetCurrentMediaDescription(CString& csName)
   {
   MEDIA Media;   

   // default
   csName = "";   

   // get the current media
   GetCurrentMedia(Media);
   // set the imaging rect
   csName = Media.Description;	  	     
   }

int CDevice::GetCurrentMediaType()
   {
   int iRet = MEDIA_TYPE_NORMAL;
   MEDIA Media;   

   // get the current media
   GetCurrentMedia(Media);
   // set the imaging rect
   iRet = Media.iType;	  	     

   return iRet;
   }

CDevice::CDevice(const CDevice &cdSrc)
   { 
   // load a new device and copy over the devinfo
   if (LoadDevice(cdSrc.DevIdx))   
      memcpy(pDevInfo, cdSrc.pDevInfo, iDevInfoSize);
   }

BOOL CDevice::SaveDevInfo()
   {
   VERIFYDEVLOAD();

   // save the current device info for later restoration
   if (!pDevInfo) return FALSE;
   if (!IsInit) return FALSE;

   // allocate the buffer
   if (iDevInfoSize > 0)
	  {		 
	  if (pTempDevInfo) delete pTempDevInfo;  

	  // get the device info
	  if (pTempDevInfo = (PDEVINFO)(new BYTE[iDevInfoSize]))         
		 {
		 ZeroMemory(pTempDevInfo, iDevInfoSize);
		 lpfn_GetDevInfo(dwDevHandle, pTempDevInfo);   
		 return TRUE;
		 }
	  }  
   return FALSE;
   }

void CDevice::RestoreDevInfo()
   {
   //restore it
   // This memcpy Restore the Standard DevInfo 
   memcpy(pTempDevInfo, pDevInfo, sizeof(DEVINFO) );

   // This Memcpy restores the Device Specific Information
   // Both are needed or some General Information is corrupted
   memcpy(pDevInfo, pTempDevInfo, iDevInfoSize);

   // and tell the device
   SetDevInfo(FALSE);
   }