/////////////////////////////////////////////////////////////////////////////
// ShellAssociate.h
//                                                                        
// shell associate for instant printing for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//
// mods:
//

#ifndef __SHELLASSOCIATE_H__
#define __SHELLASSOCIATE_H__

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CShellAssociate : public CObject
   {
public:
   CShellAssociate();
   ~CShellAssociate();
   void Init();
protected:
   void AddExtensions();   
   CStringArray csa;
   };
#endif

