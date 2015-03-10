/////////////////////////////////////////////////////////////////////////////
// TransformList.cpp
//                                                                        
// get all transforms for RasterPlus95 project
// Copyright (C) 1996-97 Graphx, Inc.
//
// mods:
//
// 10-15-96  Build 45 - changed registry stuff for NT
// 4-29-97   Build 49 - LoadAll was restricted to MAXTANSFORMS which caused > MAXTRANSFORMS
//                      to crash on SaveAs in CXform - the MAXTRANSFORMS limit has been 
//                      removed

#include "stdafx.h" 
#include "zeus.h"
#include "transformlist.h"
#include "profile.h"
#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTransformList::CTransformList()
   {
   }

CTransformList::~CTransformList()
   {
   Clear();
   }

void CTransformList::Clear()
   {
   int i;  
   CTransform* ct;    

   for (i = 0; i < GetSize(); i++)
      {
      // delete it from the array		 
      ct = (CTransform*)GetAt(i);	  
	  delete ct;
      }  
   // remove the pointers
   RemoveAll();      
   }

int CTransformList::LoadAll(BOOL AddAll)
   {
   int NumLoaded = 0;

   // loade default
   if (AddAll)
      LoadFromRegistry(IDS_DEFAULTXFORMS, NumLoaded);

   // load custom
   LoadFromRegistry(IDS_CUSTOMXFORMS, NumLoaded);

   // add custom    
   if (AddAll)
      {	  
      CTransform* pTransform = new CTransform;       
      // default vals
	  pTransform->LoadCustom();
      InsertAt(NumLoaded++, pTransform);
	  }

   return NumLoaded;
   }

void CTransformList::LoadFromRegistry(int TransformType, int& idx)
   {
   CString csSection, csBaseKey, csKey;
   HKEY hKey;
   DWORD dwIndex = 0;
   char pTransformName[_MAX_PATH];
   DWORD dwSize;      
   LONG lRet;
   CTransform* pTransform;      

   // get transform key
   csSection.LoadString(TransformType);
   csBaseKey.LoadString(IDS_RP95_REGISTRY_BASE);
   csKey = csBaseKey;
   csKey += "\\";
   csKey += csSection;

   // open the key
//   if (RegOpenKeyEx(HKEY_CURRENT_USER, (const char*)csKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
   if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const char*)csKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
      {  	  
	  do
	     {
		 dwSize = _MAX_PATH;
	     lRet = RegEnumValue(hKey, dwIndex++, pTransformName, &dwSize, 
		    NULL, NULL, NULL, NULL);
		 // good one!
		 if (lRet == ERROR_SUCCESS)
		    {
			// make a new transform
            pTransform = new CTransform;       
            // load it
            if (pTransform->ReadFromIni(TransformType, pTransformName))
               {
		       // insert it into the list
	           InsertAt(idx++, pTransform);
	           }                  
			}
		 //} while ((lRet == ERROR_SUCCESS) && (dwIndex < MAXTRANSFORMS));
		 } while (lRet == ERROR_SUCCESS);
	  // close the key
	  RegCloseKey(hKey);
	  }   
   }