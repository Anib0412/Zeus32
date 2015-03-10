/////////////////////////////////////////////////////////////////////////////
// DeviceList.cpp
//                                                                        
// get all loaded devices for RasterPlus
// Copyright (C) 1996-2000 Graphx, Inc.
//
// mods:
//
// 10-06-96  Build 45 - added Device NT stuff
// 4-29-97   Build 49 - new code to allow for multiple installs
// 4-29-97   Build 49 - only load devices once per instance
// 8-29-98   Version 2.0.5 - increased MAXDEVICES to 200
// 10-19-00  Version 4.0.1 - Only allow DLSKodak for NT Systems
// 10-30-00  Version 4.0.2 - add support for inkjetplus and for retargeting devices
// 07-11-02  Version 5.0.6 - Modify Cloaked Printer processing to reload printer on selection...this
//                           will cause deleted printers to disappear from the list

#include "stdafx.h" 
#include "zeus.h"
#include "DeviceList.h"
#include "profile.h"
#include "inidefs.h"
#include "winspool.h"
#include "..\device\globaldevid.h"
#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDeviceEntry::CDeviceEntry()
   {
   }

CDeviceEntry::~CDeviceEntry()
   {
   }

CDeviceList::CDeviceList()
   {
   OSVERSIONINFO os;
   os.dwOSVersionInfoSize = sizeof OSVERSIONINFO;
   GetVersionEx(&os);
   if ( os.dwPlatformId == VER_PLATFORM_WIN32_NT )
	   bIsNT = TRUE;
   else
	   bIsNT = FALSE;

   DevicesLoaded = FALSE;

   // load this string
   csInkJetPlus.LoadString(IDS_INKJETPLUS);
   }

CDeviceList::~CDeviceList()
   {
   int i;

   for (i = 0; i < GetSize(); i++)
      {
      // delete it from the array		 
      delete GetAt(i);
      }  
   // remove the pointers
   RemoveAll();      

   // delete the standard printers if they exist
   DestroyPrinterList();
   }

int CDeviceList::LoadAll(BOOL bPackagePlus)
   {
   int i;
   int iLastUsedID = 0;
   CProfile Profile;
   CString cs, csEntry;
   int idx, idxreg;
   char TempBuf[_MAX_PATH];
   CDeviceEntry* cde;
   CString csPort, csnFormat, csPortDefault;
   int MaxDevices;

   // already loaded, just return count
   if (DevicesLoaded) return DevCount;

   // add in the inkjet devices
   AddInkjetDevices(bPackagePlus);

   // get the maximum number of devices to check for
   MaxDevices = Profile.GetInt(IDS_DEVICES, IDS_MAXDEVICES, IDDM_MAXDEVICES); 

   // get all devices
   idxreg = 0;
   idx = 0;
   csnFormat.LoadString(IDS_DEVICE_NFORMAT);   
   do 
      {	  
	  csEntry.Format(csnFormat, idxreg++);
      cs = Profile.GetStr(IDS_DEVICES, csEntry); 
	  if (cs.GetLength() > 0)
	     {
		 iLastUsedID = idxreg;

		 // copy to temp buffer
		 strncpy(TempBuf, (const char*)cs, _MAX_PATH);
		 // make a new device entry
	     cde = new CDeviceEntry;
		 // store the data
		 cde->Name = strtok(TempBuf, _T(","));
		 cde->Idx = atoi(strtok(NULL, _T(",")));
//#if 0
	     // don't add the raw inkjet plus driver
	     if ((cde->Name.CompareNoCase(csInkJetPlus) == 0) && cde->Idx == IDD_DEVICE_INKJETPLUS)
			{
			delete cde;
			continue;
			}
//#endif	
		 // check for NT compatibility
//		 if (IS_OS_NT)
		 if (bIsNT)
		    {
		    if (atoi(strtok(NULL, _T(","))))
			   InsertAt(idx++, cde);
		    else
			   {
			   delete cde;
			   continue;			   
			   }
		    }
		 else
			{
			 int iTemp; 
			 iTemp = atoi(strtok(NULL, _T(",")));

			 if ( iTemp != 2 )	// Indicates Only include device if this is an NT System
			   InsertAt(idx++, cde);
		     else
			   {
			   delete cde;
			   continue;			   
			   }
			}
		 }	     
	  //} while (cs.GetLength() > 0);
	    } while(idxreg < MaxDevices);

	// get ports
	csPort.LoadString(IDS_CONNECTION);
	csPortDefault.LoadString(IDS_CONNECTION_DEFAULT);
	for (i = 0; i < idx; i++)
	   {	   	   
	   cde = (CDeviceEntry*)GetAt(i);	
	   // get the port
	   cde->Port = Profile.GetStr(cde->Name, csPort, (const char*)csPortDefault); 
       }	
	// set flag
	DevicesLoaded = TRUE;
	DevCount = idx;
	// return count
    return idx;
	}

int CDeviceList::LookupName(CString& csDevName)
   {
   int ret;
   int i;
   CDeviceEntry* cde;

   // make sure we're loaded
   LoadAll();

   ret = -1;

   // look up device name
   for (i = 0; i < GetSize(); i++)
      {
	  cde = (CDeviceEntry*)GetAt(i);
	  if (csDevName == cde->Name)
	     {
		 ret = cde->Idx;
		 break;
		 }
	  }
   // return index
   return ret;
   }

int CDeviceList::GetDefault()
   {
   CProfile Profile;

   return Profile.GetInt(IDS_DEVICES, IDS_DEVICE_DEFAULT);
   }

void CDeviceList::SetDefault(int DevIdx)
   {
   CProfile Profile;

   // set this device as the default device
   Profile.WriteInt(IDS_DEVICES, IDS_DEVICE_DEFAULT, DevIdx);	
   }

void CDeviceList::GetDevName(int DevIdx, CString& csDevName)
   {
   int i;
   CDeviceEntry* cde;

   LoadAll();

   for (i = 0; i < GetSize(); i++)
      {
	  cde = (CDeviceEntry*)GetAt(i);
	  if (cde->Idx == DevIdx)
	     {
		 csDevName = cde->Name;
		 break;
		 }
	  }
   }

void CDeviceList::GetDevDLL(int DevIdx, CString& csDevDLL)
   {   
   int i;
   CProfile Profile;
   CString csEntry;
   CDeviceEntry* cde;

   LoadAll();

   for (i = 0; i < GetSize(); i++)
      {
	  cde = (CDeviceEntry*)GetAt(i);

	  if (cde->Idx == DevIdx)
	     {
		 // load dll ini entry name
#ifdef _DEBUG		 
		 csDevDLL = Profile.GetStr("Debug", "Root", "f:\\source\\");	  
//		 if (!IS_OS_NT)
		 if (!bIsNT)
            csEntry.LoadString(IDS_DEV_DLLNAME_DEBUG);
		 else
			csEntry.LoadString(IDS_DEV_DLLNAME_DEBUG_NT);
         // look up device name in our INI and get
         // the dll name   
         csDevDLL += Profile.GetStr(cde->Name, csEntry);
#else
//		 if (!IS_OS_NT)
		 if (!bIsNT)
            csEntry.LoadString(IDS_DEV_DLLNAME);
		 else
			csEntry.LoadString(IDS_DEV_DLLNAME_NT);
         // look up device name in our INI and get
         // the dll name   
         csDevDLL = Profile.GetStr(cde->Name, csEntry);
#endif
		 break;
		 }
	  }
}

void CDeviceList::GetDevSPD(int DevIdx, CString& csSPD)
   {   
   int i;
   CProfile Profile;
   CString csEntry;
   CDeviceEntry* cde;

   LoadAll();

   for (i = 0; i < GetSize(); i++)
      {
	  cde = (CDeviceEntry*)GetAt(i);
	  if (cde->Idx == DevIdx)
	     {
		 // load dll ini entry name
         csEntry.LoadString(IDS_SPD_DLLNAME);     
         // look up device name in our INI and get    
         // the spd name   
         csSPD = Profile.GetStr(cde->Name, csEntry);
		 break;
		 }
	  }
   }

void CDeviceList::AddDevice(LPCSTR csDisplayName, int idx, int DevIdx)
   {   
   CString cs;
   CString csEntry;
   CString csDevStr;
   CProfile Profile;

   // make Devicen=entry
   cs.LoadString(IDS_DEVICE_NFORMAT);
   wsprintf(csEntry.GetBuffer(_MAX_PATH), (const char*)cs, idx);      
   csEntry.ReleaseBuffer();	  
   // make csDevStr
   cs.LoadString(IDS_DEVICE_DEVENTRY);
   wsprintf(csDevStr.GetBuffer(_MAX_PATH), (const char*)cs, csDisplayName, DevIdx);      
   csDevStr.ReleaseBuffer();	  
   // write it
   Profile.WriteStr(IDS_DEVICES, csEntry, csDevStr); 
   }

void CDeviceList::RemoveDevice(int DevIdx)
   {
   int i, cnt, OldSize, RemoveThis;
   CDeviceEntry* cde;
   CString cs;
   CString csEntry;
   CString csDevStr;
   CProfile Profile;

   LoadAll();

   OldSize = GetSize();

   // re-write all devices except the 
   for (i = 0, cnt = 0; i < GetSize(); i++)
      {
	  cde = (CDeviceEntry*)GetAt(i);
	  if (cde->Idx != DevIdx)
	     {
		 // make Devicen= entyr										  
         cs.LoadString(IDS_DEVICE_NFORMAT);
         wsprintf(csEntry.GetBuffer(_MAX_PATH), (const char*)cs, cnt++);   
         csEntry.ReleaseBuffer();	  
         // make csDevStr
         cs.LoadString(IDS_DEVICE_DEVENTRY);
         wsprintf(csDevStr.GetBuffer(_MAX_PATH), (const char*)cs, cde->Name, cde->Idx);   
         csDevStr.ReleaseBuffer();	  
         // write it
         Profile.WriteStr(IDS_DEVICES, csEntry, csDevStr); 
		 }
	  else
	     RemoveThis = i;
	  }

   // delete the entry
   cde = (CDeviceEntry*)GetAt(RemoveThis);
   delete cde;
   // remove the list pointer
   RemoveAt(RemoveThis);   

   // delete the last key   
   cs.LoadString(IDS_DEVICE_NFORMAT);
   wsprintf(csEntry.GetBuffer(_MAX_PATH), (const char*)cs, OldSize - 1);
   csEntry.ReleaseBuffer();	              
   csDevStr = _T("");
   Profile.WriteStr(IDS_DEVICES, csEntry, csDevStr); 
   }

void CDeviceList::InvalidateDevCount()
   {
   // force a reload of devices on next LoadAll
   DevicesLoaded = FALSE;
   DevCount = 0;
   }

void CDeviceList::SortByName()
   {   
   BOOL bRet;
   LoadAll();

   do
	  {
	  bRet = DoSort();
	  } while (bRet);
   }

BOOL CDeviceList::DoSort()
   {   
   int i;
   CDeviceEntry* cde1;
   CDeviceEntry* cde2;   

   BOOL bContinue = FALSE;

   for (i = 0; i < GetSize() - 1; i++)
      {
	  cde1 = (CDeviceEntry*)GetAt(i);
	  cde2 = (CDeviceEntry*)GetAt(i + 1);
	  if (cde2->Name < cde1->Name)
	     {
		 SetAt(i, (CObject*)cde2);
		 SetAt(i + 1, (CObject*)cde1);
		 bContinue = TRUE;
		 }
	  }
   return bContinue;
   }

void CDeviceList::ReTargetDeviceSettings(LPCSTR csOldDevName, LPCSTR csNewDevName, 
										 LPCSTR csRealName, BOOL bAlwaysWrite)
   {
   CProfile Profile;   
   CString cs;
   CString csPath;
   int idx;   
   CString csSPDName;   
   CString csSPDNameRoot;   
   CFileStatus cfs;
   CFile cf;
   CString csStub;

   csStub = "stub!\r\n";

   // if bAlwaysWrite is not set, check to see if it is already there
   if (!bAlwaysWrite)
	  {
	  cs = Profile.GetStr(csNewDevName, IDS_DEV_DLLNAME);
	  if (cs.GetLength() > 0) return;
	  }

   // spd
   csPath = Profile.GetStr(IDS_DEVICES, IDS_DEVICE_DEVINSTALLDIR);
   if ( cs.GetLength() > 0 )
   {
	   if (csPath[csPath.GetLength() - 1] != '\\' &&
		  csPath[csPath.GetLength() - 1] != ':')
		  csPath += "\\";		   		      
	   idx = 0;				   
	   do
		 {
		 // create a unique filename
		 csSPDNameRoot.Format("PPD%04d.SPD", idx++);
		 csSPDName = csPath;
		 csSPDName += csSPDNameRoot;
		 } while (CFile::GetStatus(csSPDName, cfs));		   
	   Profile.WriteStr(csNewDevName, IDS_SPD_DLLNAME, csSPDNameRoot);   

	   // now write a stub so that we "hold" the name
	   if (cf.Open(csSPDName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
		  {
		  cf.Write(csStub, csStub.GetLength());
		  cf.Close();
		  }
   }

   // ppd
   csPath = Profile.GetStr(IDS_DEVICES, IDS_DEVICE_DEVINSTALLDIR_NT);
   if ( cs.GetLength() > 0 )
   {
	   if (csPath[csPath.GetLength() - 1] != '\\' &&
		  csPath[csPath.GetLength() - 1] != ':')
		  csPath += "\\";		   	
	   idx = 0;				   
	   do
		 {
		 // create a unique filename
		 csSPDNameRoot.Format("PPD%04d.PPD", idx++);
		 csSPDName = csPath;
		 csSPDName += csSPDNameRoot;
		 } while (CFile::GetStatus(csSPDName, cfs));		      
	   Profile.WriteStr(csNewDevName, IDS_SPD_DLLNAME_NT, csSPDNameRoot);   

	   // now write a stub so that we "hold" the name
	   if (cf.Open(csSPDName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
		  {
		  cf.Write(csStub, csStub.GetLength());
		  cf.Close();
		  }
   }

   // rewrite the setup strings to a new device  		 
   cs = Profile.GetStr(csOldDevName, IDS_DEV_DLLNAME);
   Profile.WriteStr(csNewDevName, IDS_DEV_DLLNAME, cs);
   cs = Profile.GetStr(csOldDevName, IDS_DEV_DLLNAME_NT);
   Profile.WriteStr(csNewDevName, IDS_DEV_DLLNAME_NT, cs);   
   cs = Profile.GetStr(csOldDevName, IDS_DEV_DLLNAME_DEBUG);
   Profile.WriteStr(csNewDevName, IDS_DEV_DLLNAME_DEBUG, cs);
   cs = Profile.GetStr(csOldDevName, IDS_DEV_DLLNAME_DEBUG_NT);
   Profile.WriteStr(csNewDevName, IDS_DEV_DLLNAME_DEBUG_NT, cs);
   cs = Profile.GetStr(csOldDevName, IDS_CONNECTION);
   Profile.WriteStr(csNewDevName, IDS_CONNECTION, cs); 
   
   cs = Profile.GetStr(csOldDevName, IDS_DEVICE_DEPENDENT_FILESET_2000);
   Profile.WriteStr(csNewDevName, IDS_DEVICE_DEPENDENT_FILESET_2000, cs);

   cs = Profile.GetStr(csOldDevName, IDS_DEVICE_DEPENDENT_FILESET_NT);
   Profile.WriteStr(csNewDevName, IDS_DEVICE_DEPENDENT_FILESET_NT, cs);

   cs = Profile.GetStr(csOldDevName, IDS_DEVICE_DEPENDENT_FILESET);
   Profile.WriteStr(csNewDevName, IDS_DEVICE_DEPENDENT_FILESET, cs);

   cs = Profile.GetStr(csOldDevName, IDS_DEVICE_DEPENDENT_FILESET_AMD64);
   Profile.WriteStr(csNewDevName, IDS_DEVICE_DEPENDENT_FILESET_AMD64, cs);

   cs = Profile.GetStr(csOldDevName, IDS_DEVICE_DEPENDENT_FILESET_W7X64);
   Profile.WriteStr(csNewDevName, IDS_DEVICE_DEPENDENT_FILESET_W7X64, cs);

   cs = Profile.GetStr(csOldDevName, IDS_DEVICE_DEPENDENT_FILESET_2003);
   Profile.WriteStr(csNewDevName, IDS_DEVICE_DEPENDENT_FILESET_2003, cs);

   cs = Profile.GetStr(csOldDevName, IDS_DEVICE_PRINTERCAPS_FOLDER);
   Profile.WriteStr(csNewDevName, IDS_DEVICE_PRINTERCAPS_FOLDER, cs);   
   Profile.WriteStr(csNewDevName, IDS_DEVICE_INKJET_REALNAME, csRealName);   
   }

int CDeviceList::GetAllNonRPPrinters()
   {   
	LPPRINTER_INFO_4 pPrinterInfo4;
	LPPRINTER_INFO_1 pPrinterInfo1;   
	DWORD dwSizeNeeded, dwNumPrinters;
	DWORD i;      
	DWORD Flags;
	CStdPrinter* pPrinter;
	PRINTER_DEFAULTS pd;
	pd.pDatatype = NULL;
	pd.pDevMode = NULL;
	pd.DesiredAccess = PRINTER_ACCESS_USE;
	LPDRIVER_INFO_3 di;
	HANDLE hPrinter;
	DWORD dwRequired;   
	CString csOurMonitor;      
	CString csOurVirtualPort;
	CString cs;
	char pTempBuf[_MAX_PATH];
	char* p;						 
	CString csPrinterName;      

	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof OSVERSIONINFO;
	GetVersionEx(&os);

	csOurMonitor = "RasterPlus Language Monitor";   
	csOurVirtualPort = "Graphx Virtual Port";

	// reset the list
	DestroyPrinterList();   
   
	// get space needed for printers
	if (!(os.dwPlatformId == VER_PLATFORM_WIN32_NT))
	{
		Flags = PRINTER_ENUM_LOCAL; //local printers
		::EnumPrinters(Flags, NULL, 1, NULL, 
							0, &dwSizeNeeded, &dwNumPrinters);
		if (!(pPrinterInfo1 = (LPPRINTER_INFO_1)new char[dwSizeNeeded]))
			return 0;

		if (!::EnumPrinters(Flags, NULL, 1, (LPBYTE)pPrinterInfo1, 
									dwSizeNeeded, &dwSizeNeeded, &dwNumPrinters))
		{
			delete pPrinterInfo1;
			return 0;
		}	  
	}
	else
	{      
		Flags = PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_LOCAL; //local printers
		::EnumPrinters(Flags, NULL, 4, NULL, 
								0, &dwSizeNeeded, &dwNumPrinters);	  
		if (!(pPrinterInfo4 = (LPPRINTER_INFO_4)new char[dwSizeNeeded]))
			return 0;

		if (!::EnumPrinters(Flags, NULL, 4, 
						(LPBYTE)pPrinterInfo4, dwSizeNeeded, &dwSizeNeeded, &dwNumPrinters))
		{						   
			delete pPrinterInfo4;
			return 0;
		}	  

	}
   
   // get all printers   
   for (i = 0; i < dwNumPrinters; i++)
      {		
	  if (!(os.dwPlatformId == VER_PLATFORM_WIN32_NT))
	  {
	     csPrinterName = (pPrinterInfo1 + i)->pName;
		 csRPDebugStr.Format("pPrinterInfo1 pName = %s\n", (pPrinterInfo1 + i)->pName);
		 strcpy(cDebugStr, csRPDebugStr);
		 RPDEBUGSTRMFC(cDebugStr);
	  }
      else
	  {
	     csPrinterName = (pPrinterInfo4 + i)->pPrinterName;   
		 csRPDebugStr.Format("pPrinterInfo4 pName = %s\n", (pPrinterInfo4 + i)->pPrinterName);
		 strcpy(cDebugStr, csRPDebugStr);
		 RPDEBUGSTRMFC(cDebugStr);
	  }

      // get printer  		    
	  if (::OpenPrinter((char*)(const char*)csPrinterName, &hPrinter, &pd))  	  
	     {
		 // virtual port driver?
		 if (strstr(csPrinterName, csOurVirtualPort) != NULL)
			continue;

		 // get the driver
		 dwRequired = 0;		 
		::GetPrinterDriver(hPrinter, NULL, 3, NULL, 0, &dwRequired);
		 if (dwRequired)
		    {
		    if (di = (LPDRIVER_INFO_3)new char [dwRequired])
			   {
			   if (::GetPrinterDriver(hPrinter, NULL, 3, 		 
				  (LPBYTE)di, dwRequired, &dwRequired))
				  {			   
				  // right monitor
				  if (di->pMonitorName)
					 {				  
					 csRPDebugStr.Format("di->pMonitorName = %s\n", di->pMonitorName);
					 strcpy(cDebugStr, csRPDebugStr);
					 RPDEBUGSTRMFC(cDebugStr);
					 // check for our monitor
					 if (csOurMonitor.Compare(di->pMonitorName) == 0)
						{
						// don't list rp printers
						delete di;
						continue;
	     				}		 	     
					 }			   
				  }
			   delete di;
			   }
			}
		 ::ClosePrinter(hPrinter);
		 }	
	  else
	  {
		 csRPDebugStr.Format("OpenPrinter FAILED = %s\n", csPrinterName);
		 strcpy(cDebugStr, csRPDebugStr);
		 RPDEBUGSTRMFC(cDebugStr);
		 continue; // no printer driver would indicate that this is not an RP printer
	  }

	  if (pPrinter = new CStdPrinter)
		 {
		 // store the real name
		 pPrinter->csRealName = csPrinterName;
		 pPrinter->csDisplayName = csPrinterName;
		 // do we have a locator?
		 if (pPrinter->csDisplayName.Find("\\") != -1)
			{
			strcpy(pTempBuf, csPrinterName);
			strtok(pTempBuf, "\\");
			p = strtok(NULL, "\\");
			if (p) 
			   cs = p;
			else
			   cs = csPrinterName;
			}
		 else
			cs = csPrinterName;

		 pPrinter->csDisplayName = "RP-";
		 pPrinter->csDisplayName += cs;
		 // add it
		 coStdPrinters.Add(pPrinter);
		 }	  
	  }

//   if (!IS_OS_NT_FAMILY)
   if (!(os.dwPlatformId == VER_PLATFORM_WIN32_NT))
	  delete pPrinterInfo1;
   else
	  delete pPrinterInfo4;

   return coStdPrinters.GetSize();
   }

void CDeviceList::DestroyPrinterList()
   { 
   int i = 0;
   CStdPrinter* pPrinter;
   
   // destroy all the printers
   while (i < coStdPrinters.GetSize())
      {
	  pPrinter = (CStdPrinter*)coStdPrinters.GetAt(i++);
	  // and delete the printer
      delete pPrinter;
      }     
   coStdPrinters.RemoveAll();
   }

CStdPrinter::CStdPrinter()
   {
   }

CStdPrinter::~CStdPrinter()
   {
   }

void CDeviceList::AddInkjetDevices(BOOL bPackagePlus)
   {
   int i, j;
   CStdPrinter* pPrinter;   
   BOOL bAlreadyThere;
   CProfile Profile;
   CString cs;
   int MaxDevices;
   int iNewDevID;
   int iLastUsedID = 0;
   CStringArray csaPrinters;
   char TempBuf[_MAX_PATH];
   int idx;
   CString csEntry;   

   // is inkjetplus installed?
   cs = Profile.GetStr(csInkJetPlus, IDS_DEV_DLLNAME);
   if (cs.GetLength() < 1) return;

   // get max devices
   MaxDevices = Profile.GetInt(IDS_DEVICES, IDS_MAXDEVICES, IDDM_MAXDEVICES); 

   // get all the current device names so we can check for dups
   // and so we know where to start the next new device
   CString csnFormat;
   iNewDevID = 0;
   csnFormat.LoadString(IDS_DEVICE_NFORMAT);   
   do 
      {	  
	  csEntry.Format(csnFormat, iNewDevID++);
      cs = Profile.GetStr(IDS_DEVICES, csEntry); 
	  if (cs.GetLength() > 0)
		 {
		  if ( cs.Left(3) != "RP-" )
			iLastUsedID = iNewDevID;

		 // copy to temp buffer
		 strncpy(TempBuf, (const char*)cs, _MAX_PATH);
		 // get the name
		 cs = strtok(TempBuf, _T(","));
		 // get the id
		 idx = atoi(strtok(NULL, _T(",")));
		 if ( cs.Left(3) == "RP-" )
		 {
			CString csSection;
			csSection.LoadString(IDS_DEVICES);
			Profile.RemoveRegValue(csSection, csEntry);
			Profile.RemoveSubKey(csSection, csEntry);
		 }
		 else
			csaPrinters.Add(cs);
		 }
	  } while(iNewDevID < MaxDevices);

   // first get all non-rp printers
   GetAllNonRPPrinters();			

   // and add them
   for (i = 0; i < coStdPrinters.GetSize(); i++)
	  {
	  // get the printer
	  pPrinter = (CStdPrinter*)coStdPrinters.GetAt(i);

	  // check for matches against existing devices
	  for (bAlreadyThere = FALSE, j = 0; j < csaPrinters.GetSize(); j++)
		 {
		 cs = csaPrinters.GetAt(j);
		 if (cs.GetLength() > 0)
			{
			if (pPrinter->csDisplayName.CompareNoCase(cs) == 0)
			   {
			   bAlreadyThere = TRUE;
			   break;
			   }
			}
		 }

	  // need to add this?
	  if (!bAlreadyThere)
		 {
		  int iDevNum;
		 // re-target the device
		 if ( !bPackagePlus )
			ReTargetDeviceSettings(csInkJetPlus, pPrinter->csDisplayName, pPrinter->csRealName);
		 // and add it
	     iDevNum = max(iLastUsedID, 151);
		 AddDevice(pPrinter->csDisplayName, iDevNum, iDevNum);		 
		 iDevNum++;
		 iLastUsedID = iDevNum;
		 }		 
	  }	
   // get rid of the printer list
   DestroyPrinterList();

   // remove all the printer names
   csaPrinters.RemoveAll();
   }