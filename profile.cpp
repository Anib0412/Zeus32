/////////////////////////////////////////////////////////////////////////////
// PROFILE.CPP
//                                                                        
// INI/Registry module for RasterPlus95
// Copyright (C) 1996-99 Graphx, Inc.
//
// mods:
//
// 10-08-96 Build 44 - allow base key other than HKEY_CURRENT_USER
// 7-08-99  Build 3.0.0 - unify some methods
// 08-20-01 Version 5.0.0 - Add functions to remove registry values and subkeys
//

#include "stdafx.h"
#include "profile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CProfile::CProfile()
   {                
   iLastSection = 0;
   csLastINI = _T("");
//   hkBaseKey = HKEY_CURRENT_USER;
   hkBaseKey = HKEY_LOCAL_MACHINE;
   }   
                                         
CProfile::~CProfile()
   {                
   }   
                                         
BOOL CProfile::WriteInt(UINT iSection, UINT iEntry, int val)
   {   
   // get the section and the entry strings
   if (iSection != iLastSection) csSection.LoadString(iSection);
   csEntry.LoadString(iEntry);             
   
   //return AfxGetApp()->WriteProfileInt(csSection, csEntry, val);
   return WriteProfileInt(csSection, csEntry, val);
   }
   
BOOL CProfile::WriteStr(UINT iSection, UINT iEntry, LPCSTR cs)
   {   
   // get the section and the entry strings
   if (iSection != iLastSection) csSection.LoadString(iSection);
   csEntry.LoadString(iEntry);             
   
   //return AfxGetApp()->WriteProfileString(csSection, csEntry, (const char*)cs);
   return WriteProfileString(csSection, csEntry, (const char*)cs);
   }
   
UINT CProfile::GetInt(UINT iSection, UINT iEntry, int iDefVal)
   {   
   // get the section and the entry strings
   if (iSection != iLastSection) csSection.LoadString(iSection);
   csEntry.LoadString(iEntry);             

   //return AfxGetApp()->GetProfileInt(csSection, csEntry, iDefVal);
   return GetProfileInt(csSection, csEntry, iDefVal);
   }
                                                                
CString CProfile::GetStr(UINT iSection, UINT iEntry, LPCSTR csDefVal)
   {   
   // get the section and the entry strings
   if (iSection != iLastSection) csSection.LoadString(iSection);
   csEntry.LoadString(iEntry);
   
   //return AfxGetApp()->GetProfileString(csSection, csEntry, csDefVal);
   return GetProfileString(csSection, csEntry, csDefVal);
   }
                                                                
//                                                                
// CString profile stuff
//

BOOL CProfile::WriteInt(LPCSTR csSec, LPCSTR csEnt, int val)   
   {   
   //return AfxGetApp()->WriteProfileInt(csSec, csEnt, val);
   return WriteProfileInt(csSec, csEnt, val);
   }

BOOL CProfile::WriteStr(LPCSTR csSec, LPCSTR csEnt, LPCSTR cs)
   {   
   return WriteProfileString(csSec, csEnt, cs);
   }

UINT CProfile::GetInt(LPCSTR csSec, LPCSTR csEnt, int iDefVal)
   {   
   //return AfxGetApp()->GetProfileInt(csSec, csEnt, iDefVal);
   return GetProfileInt(csSec, csEnt, iDefVal);
   }

CString CProfile::GetStr(LPCSTR csSec, LPCSTR csEnt, LPCSTR csDefVal)
   {
   //return AfxGetApp()->GetProfileString(csSec, csEnt, csDefVal);
   return GetProfileString(csSec, csEnt, csDefVal);
   }
   
//
// half and half
//
  
BOOL CProfile::WriteInt(UINT iSection, LPCSTR csEnt, int val)
   {   
   // get the section and the entry strings
   if (iSection != iLastSection) csSection.LoadString(iSection);   
   
   //return AfxGetApp()->WriteProfileInt(csSection, csEnt, val);
   return WriteProfileInt(csSection, csEnt, val);
   }
   
BOOL CProfile::WriteStr(UINT iSection, LPCSTR csEnt, LPCSTR cs)
   {   
   // get the section and the entry strings
   if (iSection != iLastSection) csSection.LoadString(iSection);   
   
   //return AfxGetApp()->WriteProfileString(csSection, csEnt, (const char*)cs);
   return WriteProfileString(csSection, csEnt, (const char*)cs);
   }   

BOOL CProfile::WriteStr(LPCSTR csSec, UINT iEntry, LPCSTR cs)
   {   
   csEntry.LoadString(iEntry);   
      
   return WriteProfileString(csSec, csEntry, (const char*)cs);
   }   
                                                                
UINT CProfile::GetInt(UINT iSection, LPCSTR csEnt, int iDefVal)
   {   
   // get the section and the entry strings
   if (iSection != iLastSection) csSection.LoadString(iSection);   
   
   //return AfxGetApp()->GetProfileInt(csSection, csEnt, iDefVal);
   return GetProfileInt(csSection, csEnt, iDefVal);
   }
                                                                
UINT CProfile::GetInt(LPCSTR csSec, UINT iEntry, int iDefVal)
   {   
   csEntry.LoadString(iEntry);      
   
   return GetProfileInt(csSec, csEntry, iDefVal);
   }
                                                                
CString CProfile::GetStr(UINT iSection, LPCSTR csEnt, LPCSTR csDefVal)
   {  
   // get the section and the entry strings
   if (iSection != iLastSection) csSection.LoadString(iSection);   
   
   //return AfxGetApp()->GetProfileString(csSection, csEnt, csDefVal);
   return GetProfileString(csSection, csEnt, csDefVal);
   }

CString CProfile::GetStr(LPCSTR csSec, UINT iEntry, LPCSTR csDefVal)
   {  
   csEntry.LoadString(iEntry);      
   
   return GetProfileString(csSec, csEntry, csDefVal);
   }

// we put this in so we can use profile procs to store stuff in the registry
// in MACHINE or USER

void CProfile::SetBaseKey(HKEY hkey)
   {
   hkBaseKey = hkey;
   }

// INI strings are not localized
static const TCHAR szSoftware[] = _T("Software");
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#ifndef _MAC
// returns key for HKEY_LOCAL_MACHINE\"Software"\RegistryKey\ProfileName
// creating it if it doesn't exist
// responsibility of the caller to call RegCloseKey() on the returned HKEY
HKEY CProfile::GetAppRegistryKey()
{
	ASSERT(AfxGetApp()->m_pszRegistryKey != NULL);
	ASSERT(AfxGetApp()->m_pszProfileName != NULL);

	CString szRegKey = AfxGetApp()->m_pszRegistryKey;
	CString szProfName = AfxGetApp()->m_pszProfileName;

	HKEY hAppKey = NULL;
	HKEY hSoftKey = NULL;
	HKEY hCompanyKey = NULL;
	//if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSoftware, 0, KEY_WRITE|KEY_READ,
	if (RegOpenKeyEx(hkBaseKey, szSoftware, 0, KEY_WRITE|KEY_READ,
		&hSoftKey) == ERROR_SUCCESS)
	{
		DWORD dw;
		if (RegCreateKeyEx(hSoftKey, AfxGetApp()->m_pszRegistryKey, 0, REG_NONE,
			REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
			&hCompanyKey, &dw) == ERROR_SUCCESS)
		{
			RegCreateKeyEx(hCompanyKey, AfxGetApp()->m_pszProfileName, 0, REG_NONE,
				REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
				&hAppKey, &dw);
		}
	}
	if (hSoftKey != NULL)
		RegCloseKey(hSoftKey);
	if (hCompanyKey != NULL)
		RegCloseKey(hCompanyKey);

	return hAppKey;
}

// returns key for:
//      HKEY_LOCAL_MACHINE\"Software"\RegistryKey\AppName\lpszSection
// creating it if it doesn't exist.
// responsibility of the caller to call RegCloseKey() on the returned HKEY
HKEY CProfile::GetSectionKey(LPCTSTR lpszSection)
{
	ASSERT(lpszSection != NULL);

	HKEY hSectionKey = NULL;
	HKEY hAppKey = GetAppRegistryKey();
	if (hAppKey == NULL)
		return NULL;

	DWORD dw;
	RegCreateKeyEx(hAppKey, lpszSection, 0, REG_NONE,
		REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
		&hSectionKey, &dw);
	RegCloseKey(hAppKey);
	return hSectionKey;
}
#endif //!_MAC

UINT CProfile::GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	int nDefault)
{
	CString csTemp1;
	CString csTemp2;

	ASSERT(lpszSection != NULL);
	ASSERT(lpszEntry != NULL);

	csTemp1 = AfxGetApp()->m_pszProfileName;
	csTemp2 = AfxGetApp()->m_pszRegistryKey;

#ifndef _MAC
	if (AfxGetApp()->m_pszRegistryKey != NULL) // use registry
	{
		HKEY hSecKey = GetSectionKey(lpszSection);
		if (hSecKey == NULL)
			return nDefault;
		DWORD dwValue;
		DWORD dwType;
		DWORD dwCount = sizeof(DWORD);
		LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
			(LPBYTE)&dwValue, &dwCount);
		RegCloseKey(hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_DWORD);
			ASSERT(dwCount == sizeof(dwValue));
			return (UINT)dwValue;
		}
		return nDefault;
	}
	else
#endif
	{
		ASSERT(AfxGetApp()->m_pszProfileName != NULL);
		return ::GetPrivateProfileInt(lpszSection, lpszEntry, nDefault,
			AfxGetApp()->m_pszProfileName);
	}
}

CString CProfile::GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	LPCTSTR lpszDefault)
{
	ASSERT(lpszSection != NULL);
	ASSERT(lpszEntry != NULL);
#ifndef _MAC
	if (AfxGetApp()->m_pszRegistryKey != NULL)
	{
		HKEY hSecKey = GetSectionKey(lpszSection);
		if (hSecKey == NULL)
			return lpszDefault;
		CString strValue;
		DWORD dwType, dwCount;
		LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
			NULL, &dwCount);
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_SZ);
			lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
				(LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), &dwCount);
			strValue.ReleaseBuffer();
		}
		RegCloseKey(hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_SZ);
			return strValue;
		}
		return lpszDefault;
	}
	else
#endif
	{
		ASSERT(AfxGetApp()->m_pszProfileName != NULL);

		if (lpszDefault == NULL)
			lpszDefault = &afxChNil;    // don't pass in NULL
		TCHAR szT[4096];
		DWORD dw = ::GetPrivateProfileString(lpszSection, lpszEntry,
			lpszDefault, szT, _countof(szT), AfxGetApp()->m_pszProfileName);
		ASSERT(dw < 4095);
		return szT;
	}
}

BOOL CProfile::GetProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	BYTE** ppData, UINT* pBytes)
{
	ASSERT(lpszSection != NULL);
	ASSERT(lpszEntry != NULL);
	ASSERT(ppData != NULL);
	ASSERT(pBytes != NULL);
	*ppData = NULL;
	*pBytes = 0;
#ifndef _MAC
	if (AfxGetApp()->m_pszRegistryKey != NULL)
	{
		LPBYTE lpByte = NULL;
		HKEY hSecKey = GetSectionKey(lpszSection);
		if (hSecKey == NULL)
			return FALSE;

		DWORD dwType, dwCount;
		LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
			NULL, &dwCount);
		*pBytes = dwCount;
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_BINARY);
			*ppData = new BYTE[*pBytes];
			lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
				*ppData, &dwCount);
		}
		RegCloseKey(hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_BINARY);
			return TRUE;
		}
		else
		{
			delete [] *ppData;
			*ppData = NULL;
		}
		return FALSE;
	}
	else
#endif
	{
		ASSERT(AfxGetApp()->m_pszProfileName != NULL);

		CString str = GetProfileString(lpszSection, lpszEntry, NULL);
		if (str.IsEmpty())
			return FALSE;
		ASSERT(str.GetLength()%2 == 0);
		int nLen = str.GetLength();
		*pBytes = nLen/2;
		*ppData = new BYTE[*pBytes];
		for (int i=0;i<nLen;i+=2)
		{
			(*ppData)[i/2] = (BYTE)
				(((str[i+1] - 'A') << 4) + (str[i] - 'A'));
		}
		return TRUE;
	}
}

#ifdef AFX_CORE3_SEG
#pragma code_seg(AFX_CORE3_SEG)
#endif

BOOL CProfile::WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	int nValue)
{
	ASSERT(lpszSection != NULL);
	ASSERT(lpszEntry != NULL);
#ifndef _MAC
	if (AfxGetApp()->m_pszRegistryKey != NULL)
	{
		HKEY hSecKey = GetSectionKey(lpszSection);
		if (hSecKey == NULL)
			return FALSE;
		LONG lResult = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_DWORD,
			(LPBYTE)&nValue, sizeof(nValue));
		RegCloseKey(hSecKey);
		return lResult == ERROR_SUCCESS;
	}
	else
#endif
	{
		ASSERT(AfxGetApp()->m_pszProfileName != NULL);

		TCHAR szT[16];
		wsprintf(szT, _T("%d"), nValue);
		return ::WritePrivateProfileString(lpszSection, lpszEntry, szT,
			AfxGetApp()->m_pszProfileName);
	}
}

BOOL CProfile::WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,
			LPCTSTR lpszValue)
{
	ASSERT(lpszSection != NULL);
#ifndef _MAC
	if (AfxGetApp()->m_pszRegistryKey != NULL)
	{
		LONG lResult;
		if (lpszEntry == NULL) //delete whole section
		{
			HKEY hAppKey = GetAppRegistryKey();
			if (hAppKey == NULL)
				return FALSE;
			lResult = ::RegDeleteKey(hAppKey, lpszSection);
			RegCloseKey(hAppKey);
		}
		else if (lpszValue == NULL)
		{
			HKEY hSecKey = GetSectionKey(lpszSection);
			if (hSecKey == NULL)
				return FALSE;
			// necessary to cast away const below
			lResult = ::RegDeleteValue(hSecKey, (LPTSTR)lpszEntry);
			RegCloseKey(hSecKey);
		}
		else
		{
			HKEY hSecKey = GetSectionKey(lpszSection);
			if (hSecKey == NULL)
				return FALSE;
			lResult = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_SZ,
				(LPBYTE)lpszValue, (lstrlen(lpszValue)+1)*sizeof(TCHAR));
			RegCloseKey(hSecKey);
		}
		return lResult == ERROR_SUCCESS;
	}
	else
#endif
		{
		ASSERT(AfxGetApp()->m_pszProfileName != NULL);
		ASSERT(lstrlen(AfxGetApp()->m_pszProfileName) < 4095); // can't read in bigger
		return ::WritePrivateProfileString(lpszSection, lpszEntry, lpszValue,
			AfxGetApp()->m_pszProfileName);
	}
}

BOOL CProfile::WriteProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	LPBYTE pData, UINT nBytes)
{
	ASSERT(lpszSection != NULL);
#ifndef _MAC
	if (AfxGetApp()->m_pszRegistryKey != NULL)
	{
		LONG lResult;
		HKEY hSecKey = GetSectionKey(lpszSection);
		if (hSecKey == NULL)
			return FALSE;
		lResult = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_BINARY,
			pData, nBytes);
		RegCloseKey(hSecKey);
		return lResult == ERROR_SUCCESS;
	}
#endif
	// convert to string and write out
	LPTSTR lpsz = new TCHAR[nBytes*2+1];
	for (UINT i = 0; i < nBytes; i++)
	{
		lpsz[i*2] = (TCHAR)((pData[i] & 0x0F) + 'A'); //low nibble
		lpsz[i*2+1] = (TCHAR)(((pData[i] >> 4) & 0x0F) + 'A'); //high nibble
	}
	lpsz[i*2] = 0;

	ASSERT(AfxGetApp()->m_pszProfileName != NULL);

	BOOL bResult = WriteProfileString(lpszSection, lpszEntry, lpsz);
	delete[] lpsz;
	return bResult;
}


BOOL CProfile::RemoveRegValue(LPCTSTR lpszSection, LPCTSTR lpszEntry )
{
	BOOL bRetValue = FALSE;

	HKEY hSecKey = GetSectionKey(lpszSection);
	if (hSecKey == NULL)
		return FALSE;

	if ( RegDeleteValue( hSecKey, lpszEntry ) == ERROR_SUCCESS )
		bRetValue = TRUE;
	else
	{
		bRetValue = FALSE;
	}

	RegCloseKey(hSecKey);

	return bRetValue;
}

BOOL CProfile::RemoveSubKey(LPCTSTR lpszSection, LPCTSTR lpszEntry )
{
	BOOL bRetValue = FALSE;

	HKEY hSecKey = GetSectionKey(lpszSection);
	if (hSecKey == NULL)
		return FALSE;

	if ( RegDeleteKey( hSecKey, lpszEntry ) == ERROR_SUCCESS )
		bRetValue = TRUE;
	else
	{
		bRetValue = FALSE;
	}

	RegCloseKey(hSecKey);

	return bRetValue;
}