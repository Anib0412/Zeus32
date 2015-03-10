/////////////////////////////////////////////////////////////////////////////
// SYSMEM.H
//                                                                        
// get system memory for for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __SYSMEM_H__
#define __SYSMEM_H__

class SysMem : public CObject
   {   
public:      
   SysMem();
   ~SysMem();      
  
   int InstalledMemory;
   int FreePhysicalMemory;
   int FreeVirtualMemory;
   CString csInstalledMemory;
   CString csFreePhysicalMemory;
   CString csFreeVirtualMemory;
   };
#endif
