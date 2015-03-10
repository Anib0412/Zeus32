/////////////////////////////////////////////////////////////////////////////
// DEVICE.H
//                                                                        
// device class methods and "C" wrapper procs for RasterPlus
// Copyright (C) 1996-2000 Graphx, Inc.
//
// Note: we convert c++ class calls to std c calls so that we 
// can transport across a static MFC dll layer
//
// Mods:
// 9-10-98  Version 2.0.5 - added new device rip loading code (disabled for now)
// 12-04-98 Version 2.0.5 - added UpdateMediaList
// 2-20-00  Version 3.0.3 - added UpdateConnectList
// 5-12-00  Version 3.0.3 - added UpdateResList
// 10-11-00 Version 5.0.0 - added queue count checking

#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "..\device\RP_VERSION.H"
#include "devface.h"
#include "ripstruct.h"
#include "ipcripdata.h"
#include "QueueCount.h"
#include "TimeCheck.h"

class CDevice : public CObject
   {
public:
   CDevice();   
   ~CDevice();   
   
   BOOL LoadDefaultDevice();
   BOOL LoadDevice(int idx, BOOL bHaveName = FALSE);
   BOOL LoadDevice(int idx, char* dllname, char* devname);
   void SetDevInfo(BOOL bRegister);
   void SetDevInfo(PDEVINFO pd, BOOL bCommit);
   int GetDevInfo();
   int GetDevInfoSize();
   void GetCurrentMedia(MEDIA& Media);
   void GetCurrentMediaRect(CRect& cr);
   void GetCurrentMediaName(CString& csName);
   int GetCurrentMediaType();
   void GetCurrentPPDMediaName(CString& csName);
   void GetCurrentMediaDescription(CString& csName);
   BOOL GetMedia(int idx, PMEDIA pMedia);
   BOOL GetMediaBin(int idx, char *Name);
   BOOL GetRes(int idx, char *Name);   
   BOOL GetConnect(int idx, char *Name, int* pConnectType = NULL);
   void ParseServerTokens(CString& csToken, CString& csValue);
   BOOL MakeSPDFile(CString& csSPDName, CString& csShortName);
   BOOL OpenDevice(PRIPSTRUCT pRipStruct, PJOBINFO pJobInfo, int* pDevAbortFlag);
   BOOL OpenDeviceFromRip(PRIPSTRUCT pRipStruct, PJOBINFO pJobInfo, PIPCDEVINFO pip);
   void CloseDevice();
   int OutputBytes(BYTE *buf, int len, int y);   
   void Abort();   
   BOOL FillRipStruct(PRIPSTRUCT pRipStruct);
   BOOL GetDevError(char *pError);
   int DoDialog(HWND);
   int CreatePSInit(BYTE* buf);   
   BOOL AlertModeChange(int);
   void SetMem();
   BOOL NeedDongle();
   BOOL FindConnection(char* pConnectName, int *pHostAdapterID);
   BOOL GetSetExtra(int cmd, BYTE* pIn, BYTE* pOut, int iLenIn, int iLenOut);
   int UpdateMediaList();
   int UpdateResList();
   int UpdateConnectList();
   BOOL DoPropBox(int cmd, CWnd* pCw);
   void SetDeviceWnd(HWND pwnd);
   void UpdateDevInfo(PDEVINFO* pinfo, int cnt, int item);
   void UpdateDlgFromQueue(PDEVINFO* pinfo, int cnt);
   BOOL IsDlgShowing();   
   BOOL SaveDevInfo();
   void RestoreDevInfo();
   void LicenseManager();
#ifdef _PHZ_PRINT_SERVER_TEMP
   void PrintServerSetup();
#endif
#ifdef RP_6
//   void InstallWizard();
#endif
   CTimeCheck TimeCheck;

   virtual void Serialize(CArchive& ar, WORD verCheck = 0);
   const CDevice& operator=(const CDevice& cdmSrc); // assignment
   BOOL operator==(const CDevice& dm);              // compare      
   BOOL operator!=(const CDevice& dm);              // compare      
   CDevice(const CDevice &cdSrc);                   // Copy constructor
   BOOL IsSame(const CDevice& cd);                  // used fo != and ==

public:
   PDEVINFO pDevInfo;
   PDEVINFO pTempDevInfo;
   BOOL IsInit;   
   WORD DevIdx;
   CString Name;   
   CString csFriendlyName;   
   HINSTANCE hInst; 
   DWORD dwDevHandle;   
   CString DLLName;  
   BOOL bLoadLate;
   BOOL PropBoxCreated;
   CQueueCount QueueCount;


   CString csRPDebugStr;
   char cDebugStr[255];


protected:   
   WORD iDevInfoSize;

   void FreeDevice();

   // the c entry procs for the device dll
   DWORD (WINAPIV *lpfn_AllocDevice)(const char* DevName, int DevIdx);      
   void (WINAPIV *lpfn_FreeDevice)(DWORD dwDevHandle);   
   BOOL (WINAPIV *lpfn_GetDevInfo)(DWORD dwDevHandle, PDEVINFO pd);
   void (WINAPIV *lpfn_SetDevInfo)(DWORD dwDevHandle, PDEVINFO pd, BOOL bRegister);   
   BOOL (WINAPIV *lpfn_GetMedia)(DWORD dwDevHandle, int idx, PMEDIA pMedia);
   BOOL (WINAPIV *lpfn_GetMediaBin)(DWORD dwDevHandle, int idx, char* Name);
   BOOL (WINAPIV *lpfn_GetRes)(DWORD dwDevHandle, int idx, char* Name);
   BOOL (WINAPIV *lpfn_GetConnect)(DWORD dwDevHandle, int idx, char* Name, int* pConnectType);
   void (WINAPIV *lpfn_ParseServerTokens)(DWORD dwDevHandle, const char* csToken, const char* csValue);
   BOOL (WINAPIV *lpfn_MakeSPDFile)(DWORD dwDevHandle, const char* csSPDName, const char* csShortName);
   BOOL (WINAPIV *lpfn_OpenDevice)(DWORD dwDevHandle, PRIPSTRUCT pRipStruct, PJOBINFO pJobInfo, int* pDevAbortFlag);
   BOOL (WINAPIV *lpfn_OpenDeviceFromRip)(DWORD dwDevHandle, PRIPSTRUCT pRipStruct, PJOBINFO pJobInfo, PIPCDEVINFO pip);   
   void (WINAPIV *lpfn_CloseDevice)(DWORD dwDevHandle);
   int (WINAPIV *lpfn_OutputBytes)(DWORD dwDevHandle, BYTE *buf, int len, int y);   
   void (WINAPIV *lpfn_Abort)(DWORD dwDevHandle);
   BOOL (WINAPIV *lpfn_GetDevError)(DWORD dwDevHandle, char *pError);
   BOOL (WINAPIV *lpfn_FillRipStruct)(DWORD dwDevHandle, PRIPSTRUCT pRipStruct);
   int (WINAPIV *lpfn_DoDialog)(DWORD dwDevHandle, HWND hWnd);
   int (WINAPIV *lpfn_CreatePSInit)(DWORD, BYTE* buf);   
   BOOL (WINAPIV *lpfn_AlertModeChange)(DWORD, int, PDEVINFO);   
   BOOL (WINAPIV *lpfn_NeedDongle)(DWORD);   
   BOOL (WINAPIV *lpfn_FindConnection)(DWORD, char*, int*);      
   int (WINAPIV *lpfn_UpdateMediaList)(DWORD);         
   int (WINAPIV *lpfn_UpdateResList)(DWORD);         
   int (WINAPIV *lpfn_UpdateConnectList)(DWORD);         
   BOOL (WINAPIV *lpfn_DoPropBox)(DWORD, int, CWnd*);
   void (WINAPIV *lpfn_SetDeviceWnd)(DWORD dwDevHandle, HWND pwnd);         
   void (WINAPIV *lpfn_UpdateDevInfo)(DWORD dwDevHandle, PDEVINFO* pinfo, int cnt, int item);         
   void (WINAPIV *lpfn_UpdateDlgFromQueue)(DWORD dwDevHandle, PDEVINFO* pinfo, int cnt);
   BOOL (WINAPIV *lpfn_IsDlgShowing)(DWORD dwDevHandle);
   BOOL (WINAPIV *lpfn_GetSetExtra)(DWORD, int cmd, BYTE* pIn, BYTE* pOut, 
	  int iLenIn, int iLenOut);      
#ifndef RP_6
   void (WINAPIV *lpfn_LicenseManager)(DWORD); 
#endif
#ifdef _PHZ_PRINT_SERVER_TEMP
   void (WINAPIV *lpfn_PrintServerSetup)(DWORD);   
#endif

#ifdef RP_6
   void (WINAPIV *lpfn_InstallWizard)(DWORD);   
#endif
   };

typedef struct tagDevChange
   {
   CDevice* pCD;
   CString csText;
   int ItemChanged;   
   int arg1;   
   } DEVCHANGE, * PDEVCHANGE;

#define GLOBAL_PREFS_HAS_CHANGED "GlobalPrefsHasChanged"   
#define PREVIEW_DEVICE_IDX    9999
#define DEVICE_IDX_NONE       8888

#endif
						  