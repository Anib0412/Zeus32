/////////////////////////////////////////////////////////////////////////////
// SpecialTree.CPP
//                                                                        
// base tree display/op class for RasterPlus
// Copyright (C) 1999-2000 Graphx, Inc.
//
// mods:
//
// 11-16-05  Version 5.2.15 - Fixed Handle not being allocated/closed properly
// 11-16-05  Version 5.2.15 - PData Count not being incremented properly


#include "stdafx.h"
#include "zeus.h"
#include "TimeCheck.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAXSECS  (4294967295 / 1000)

CTimeCheck::CTimeCheck()
   {   
   pData = NULL;
   hFileMap = NULL;        
   }

CTimeCheck::~CTimeCheck()
   {
   UnSetData();
   }

void CTimeCheck::UnSetData()
{
	if (pData)
	{
		if ( pData->dwCount == 0 )
		{
			::UnmapViewOfFile((LPVOID)pData);
		}
		else
			pData->dwCount--;
	}

	if ( hFileMap )
		CloseHandle(hFileMap);
}

BOOL CTimeCheck::SetData(CString& csName)
{   
	// Create an in-memory memory-mapped file 
	if ( hFileMap == NULL )
	{
		hFileMap = ::CreateFileMapping((HANDLE) 0xFFFFFFFF, 
		NULL, PAGE_READWRITE, 0, (DWORD)(sizeof TIMECHECK),
		(const char*)csName);
	}

	if (hFileMap != NULL) 
	{
		// Map a view of the file into the address space.
		if ( pData == NULL )
		{
			pData = (PTIMECHECK)::MapViewOfFile(hFileMap,
						FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

			if (pData)
			{
				// if it doesn't yet exist, init it
				if (GetLastError() != ERROR_ALREADY_EXISTS) 
				{			
					// init with the number of seconds since
					// system start
					pData->dwSecs = 0;
					pData->dwCount = 0;
					return TRUE;				// First time creted
				}
				else
				{
					pData->dwCount++;
					return FALSE;				// Already EXISTS continue
				}
			}
		}
	}

	return FALSE;						// Continue 
}

BOOL CTimeCheck::SetCheck(const char* csN)
   {
   CString csName;

   // create unique name
   csName.Format("RasterPlus_TimeCheck_%s", csN);

   // create the memory map
   return (SetData(csName));
   }

void CTimeCheck::GetElapsed(DWORD& dwSecs)
   {
   DWORD dwElapsed;
   
   // on error, return maximum time
   if (!pData)
	  {
	  dwSecs = MAXSECS;
	  return;
	  }

   // a wrap?
   if (::GetTickCount() / 1000 < pData->dwSecs)
	  {
	  // return the maximum number of ticks
	  dwElapsed = MAXSECS;
      // and store the new count
//      pData->dwSecs = ::GetTickCount() / 1000;
	  }
   else
	  {
	  // get elapsed time since last check
	  dwElapsed = ::GetTickCount() / 1000 - pData->dwSecs;
      // store elapsed time
//      pData->dwSecs = dwElapsed;
	  }
   }

void CTimeCheck::GetTotalElapsed(DWORD& dwSecs)
   {
   // on error, return maximum time
   if (!pData)
	  {
	  dwSecs = MAXSECS;
	  return;
	  }

   // a wrap?
   if (::GetTickCount() / 1000 < pData->dwSecs)
	  {
	  // return the maximum number of ticks
	  dwSecs = MAXSECS;
      // and store the new count
//      pData->dwSecs = ::GetTickCount() / 1000;
	  }
   else
	  {
	  // get elapsed time since last check
	  dwSecs = ::GetTickCount() / 1000 - pData->dwSecs;
	  }
   return;
   }

void CTimeCheck::GetLastSet(DWORD& dwSecs)
   {
   // on error, return maximum time
   if (!pData)
	  {
	  dwSecs = MAXSECS;
	  return;
	  }

   dwSecs = pData->dwSecs;

   return;
   }


void CTimeCheck::ResetElapsed()
   {
   // on error, return maximum time
   if (!pData)
	  {
	  return;
	  }

   pData->dwSecs = ::GetTickCount() / 1000;

   return;
   }

