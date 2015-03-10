/////////////////////////////////////////////////////////////////////////////
// ShellAssociate.cpp
//                                                                        
// shell associate for instant printing for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//
// mods:
//

#include "stdafx.h"
#include "zeus.h"
#include "profile.h"
#include "ShellAssociate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CShellAssociate::CShellAssociate()
   {
   }

CShellAssociate::~CShellAssociate()
   {
   }

void CShellAssociate::AddExtensions()
   {
   csa.Add(".tif");
   csa.Add(".tga");
   csa.Add(".bmp");
   }

void CShellAssociate::Init()
   {
   int i;
   CProfile Profile;
   HKEY hKey, hKey2;
   CString csBuffer;   
   DWORD dwSize, dwType;

   // see if we're inited
   if (Profile.GetInt(IDS_GENERALSECTION, IDS_ASSOCIATE_DONE, 0)) return;

   // nope!, add extensions
   AddExtensions();

   // do this for all the extensions
   for (i = 0; i < csa.GetSize(); i++)
	  {	  	  
	  if (RegOpenKeyEx(HKEY_CLASSES_ROOT, (const char*)csa.GetAt(i), 0, 
	     KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
		 {
		 //it's there!
		 dwSize = 256;			 
		 dwType = REG_SZ;
	     if (RegQueryValueEx(hKey, "", NULL, &dwType, 
			(LPBYTE)csBuffer.GetBuffer(dwSize), &dwSize) == ERROR_SUCCESS)
			{
			// no entry, so we add ours
			if (csBuffer.GetLength() == 0)
			   {
			   }
			else
			   {
			   // add a print setting DDE to the current setting
		   	   if (RegOpenKeyEx(HKEY_CLASSES_ROOT, (const char*)csBuffer, 0, 
	              KEY_ALL_ACCESS, &hKey2) == ERROR_SUCCESS)
				  {
				  }
			   }
			}
		 csBuffer.ReleaseBuffer();		 
		 RegCloseKey(hKey);
		 }
	  }
   }