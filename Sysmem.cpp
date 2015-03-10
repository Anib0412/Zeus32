/////////////////////////////////////////////////////////////////////////////
// SYSMEM.CPP
//                                                                        
// get system memory for for RasterPlus95
// Copyright (C) 1996-98 Graphx, Inc.
//
// mods:
//
// 1-6-98  Version 2.0.1 - different memory check for Windows NT
// 7-16-98 Version 3.0.3 - make the 95/96 mem handling the same as NT


#include<afxwin.h>
#include"sysmem.h"
#include "nformat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SysMem::SysMem()
   {
   MEMORYSTATUS mmi;
   NumberFormat nm;
   OSVERSIONINFO os;

   GlobalMemoryStatus(&mmi);
   InstalledMemory = mmi.dwTotalPhys / 1024;

   os.dwOSVersionInfoSize = sizeof OSVERSIONINFO;
   GetVersionEx(&os);
#if 0
   if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
      FreePhysicalMemory = mmi.dwAvailPhys / 1024;
   else      
      FreePhysicalMemory = mmi.dwTotalPhys * (100 - mmi.dwMemoryLoad) / (100 * 1024);
#endif
   FreePhysicalMemory = mmi.dwAvailPhys / 1024;
   FreeVirtualMemory = mmi.dwAvailPageFile / 1024;
   
   nm.ConvertToComma(InstalledMemory, csInstalledMemory);
   nm.ConvertToComma(FreePhysicalMemory, csFreePhysicalMemory);
   nm.ConvertToComma(FreeVirtualMemory, csFreeVirtualMemory);   
   }

SysMem::~SysMem()
   {
   }
