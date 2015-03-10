/////////////////////////////////////////////////////////////////////////////
// PROFILE.H
//                                                                        
// INI/Registry module for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __PROFILE_H__
#define __PROFILE_H__

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CProfile : public CObject
   {            
public:   
   CProfile();
   ~CProfile();

   void SetBaseKey(HKEY hkey);
   
   // std profile stuff
   BOOL WriteInt(UINT iSection, UINT iEntry, int val = 0);   
   BOOL WriteStr(UINT iSection, UINT iEntry, LPCSTR cs);   
   
   UINT GetInt(UINT iSection, UINT iEntry, int iDefVal = 0);
   CString GetStr(UINT iSection, UINT iEntry, LPCSTR csDefVal = NULL);         
   
   // CString profile stuff
   BOOL WriteInt(LPCSTR csSec, LPCSTR csEnt, int val = 0);   
   BOOL WriteStr(LPCSTR csSec, LPCSTR csEnt, LPCSTR cs);         
   
   UINT GetInt(LPCSTR csSec, LPCSTR csEnt, int iDefVal = 0);
   CString GetStr(LPCSTR csSec, LPCSTR csEnt, LPCSTR csDefVal = NULL);
   
   // half and half
   UINT GetInt(UINT iSection, LPCSTR csEnt, int iDefVal = 0);
   UINT GetInt(LPCSTR csSec, UINT iEntry, int iDefVal = 0);

   BOOL WriteInt(UINT iSection, LPCSTR csEnt, int val);
   BOOL WriteStr(UINT iSection, LPCSTR csEnt, LPCSTR cs);
   BOOL WriteStr(LPCSTR csSec, UINT iEntry, LPCSTR csDefVal);

   CString GetStr(UINT iSection, LPCSTR csEnt, LPCSTR csDefVal = NULL);
   CString GetStr(LPCSTR csSec, UINT iEntry, LPCSTR csDefVal = NULL);

   BOOL RemoveRegValue(LPCTSTR lpszSection, LPCTSTR lpszEntry );
   BOOL RemoveSubKey(LPCTSTR lpszSection, LPCTSTR lpszEntry );
   
protected:   
   HKEY GetAppRegistryKey();
   HKEY GetSectionKey(LPCTSTR lpszSection);
   UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,
      int nDefault);
   CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	  LPCTSTR lpszDefault);
   BOOL GetProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	  BYTE** ppData, UINT* pBytes);
   BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	  int nValue);
   BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	  LPCTSTR lpszValue);
   BOOL WriteProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry,
      LPBYTE pData, UINT nBytes);

   CString csSection, csEntry;
   UINT iLastSection;
   CString csLastINI;
   HKEY hkBaseKey;
   };
#endif
   
