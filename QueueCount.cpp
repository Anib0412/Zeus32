#include "stdafx.h"
#include "zeus.h"
#include "QueueCount.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CQueueCount::CQueueCount()
   {   
   pData = NULL;
   hFileMap = NULL;     
   SetData();
   }

CQueueCount::~CQueueCount()
   {
   UnSetData();
   }

void CQueueCount::UnSetData()
   {
   if (pData)
	  {
	  ::UnmapViewOfFile((LPVOID)pData);
	  CloseHandle(hFileMap);
	  }
   }

void CQueueCount::SetData()
   {   
   // Create an in-memory memory-mapped file 
   hFileMap = ::CreateFileMapping((HANDLE) 0xFFFFFFFF, 
     NULL, PAGE_READWRITE, 0, (DWORD)(sizeof(QUEUECOUNT)),
     "RasterPlus_QueueCount");
					 
   if (hFileMap != NULL) 
      {
      // Map a view of the file into the address space.
      pData = (PQUEUECOUNT)::MapViewOfFile(hFileMap,
         FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

	  // if it doesn't yet exist, zero it out
      if (GetLastError() != ERROR_ALREADY_EXISTS) 
	     {
		 memset(pData, 0, sizeof(QUEUECOUNT));
	     pData->iQueueCount = 0;
		 }
	  }   
   }

void CQueueCount::IncrQueueCount(int iUpTick)
   {
   if (!pData) return;
   pData->iQueueCount += iUpTick;
   }

void CQueueCount::DecrQueueCount(int iDownTick)
   {
   if (!pData) return;
   pData->iQueueCount -= iDownTick;
   }

int CQueueCount::GetQueueCount()
   {
   if (!pData) return 0;   
   return pData->iQueueCount;
   }

void CQueueCount::SetQueueCount(int iCount)
   {
   if (pData) pData->iQueueCount = iCount;
   }
