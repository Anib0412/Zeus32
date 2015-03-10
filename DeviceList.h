/////////////////////////////////////////////////////////////////////////////
// DeviceList.h
//                                                                        
// get all loaded devices for RasterPlus
// Copyright (C) 1996-2000 Graphx, Inc.
//

#ifndef __DEVICELIST_H__
#define __DEVICELIST_H__

class CDeviceEntry : public CObject
   {
public:
   CDeviceEntry();
   ~CDeviceEntry();

   CString Name;
   CString Port;
   int iHostID;
   int Idx;
   };

class CStdPrinter : public CObject
   {
public:
   CStdPrinter();
   ~CStdPrinter();
   CString csRealName;
   CString csDisplayName;
   }; 

class CDeviceList : public CObArray
   {
public:
   CDeviceList();
   ~CDeviceList();

   void SortByName();   
   int LoadAll(BOOL bPackagePlus = FALSE);
   int LookupName(CString& csDevName);
   int GetDefault();
   void SetDefault(int DevIdx);
   void GetDevName(int DevIdx, CString& csDevName);
   void GetDevDLL(int DevIdx, CString& csDevDLL);
   void GetDevSPD(int DevIdx, CString& csSPD);
   void InvalidateDevCount();
   void AddDevice(LPCSTR csDisplayName, int idx, int DevIdx);
   void RemoveDevice(int DevIdx);
   void ReTargetDeviceSettings(LPCSTR csOldDevName, LPCSTR csNewDevName, 
	  LPCSTR csRealName, BOOL bAlwaysWrite=FALSE);
protected:
   CString csInkJetPlus;
   BOOL DevicesLoaded;
   BOOL DoSort();
   int DevCount;
   bool bIsNT;
   CObArray coStdPrinters;
   void DestroyPrinterList();
   int GetAllNonRPPrinters();
   void AddInkjetDevices(BOOL bPackagePlus = FALSE);

   CString csRPDebugStr;
   char cDebugStr[255];
   };

#endif
