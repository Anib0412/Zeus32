// PrintPaneState.cpp: implementation of the CPrintPaneState class.
//
//////////////////////////////////////////////////////////////////////
//
// mods:
//
// 09-07-00   Version 4.0.0 - Added files and functions to support MultiImage COS file processing
// 02-14-02   Version 5.0.4 - Added functions to support alphmeric order numbers
// 04-15-02   Version 5.0.5 - Added two new values Surface and BorderWidth to support MiniLab Parameters
// 11-07-02   Version 5.1.0 - Fix memory leak
//


#include "stdafx.h"
#include "zeus.h"
#include "PrintPaneState.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CPrintPaneState::CPrintPaneState()
   {
   csName = "";
   pData = NULL;
   hFileMap = NULL;  
   bIsInit = FALSE;
   }

CPrintPaneState::~CPrintPaneState()
   {
   UnSetData();
   }

void CPrintPaneState::SetName(LPCSTR csn)
   {   
   if (csName.Compare(csn) == 0) return;   
   UnSetData();
   csName = csn;   
   bIsInit = TRUE;
   SetData();
   }

void CPrintPaneState::UnSetData()
   {
   if (csName.GetLength() > 0)
	  {
	  ::UnmapViewOfFile((LPVOID)pData);
	  CloseHandle(hFileMap);
	  }
   }

void CPrintPaneState::SetData()
   {   
   CString csMapName;

   // create map file name with base name and index
   csMapName.Format("%s_%s", "CPrintPaneState", csName); 

   // Create an in-memory memory-mapped file 
   hFileMap = ::CreateFileMapping((HANDLE) 0xFFFFFFFF, 
     NULL, PAGE_READWRITE, 0, (DWORD)(sizeof(PPCOMMON)),
     (const char*)csMapName);
					 
   if (hFileMap != NULL) 
      {
      // Map a view of the file into the address space.
      pData = (PPPCOMMON)::MapViewOfFile(hFileMap,
         FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

	  // if it doesn't yet exist, zero it out
      if (GetLastError() != ERROR_ALREADY_EXISTS) 
	     {
		 memset(pData, 0, sizeof(PPCOMMON));
	     pData->iTotalPrintPaneEntries = 0;
		 }
	  }   
   }

void CPrintPaneState::IncrPrintPaneEntries(int iUpTick)
   {
   if (!pData) return;
   pData->iTotalPrintPaneEntries += iUpTick;
   }

void CPrintPaneState::DecrPrintPaneEntries(int iDownTick)
   {
   if (!pData) return;
   pData->iTotalPrintPaneEntries -= iDownTick;
   }

int CPrintPaneState::GetPrintPaneEntries()
   {
   if (!pData) return 0;   
   return pData->iTotalPrintPaneEntries;
   }

void CPrintPaneState::SetPrintPaneEntries(int iEntries)
   {
   if (pData) pData->iTotalPrintPaneEntries = iEntries;
   }


// Additional information for Multi Image Cos File printing
//
BOOL CPrintPaneState::AddMultiImage(int iCopies, int iMedia, LPCSTR lpstrOwner, int iOrderNum, 
										int iFileType, BOOL bFullBleed, int iSurface, int iBorderWidth,
											LPCSTR lpstrLine1, BOOL bLine1, LPCSTR lpstrLine2, BOOL bLine2)
   {
   // do we have room?
   if (pData->iTotalGangPrintEntries >= MAX_GANG_FILES)
	  return FALSE;
   else
	  {
	  // add it
	  pData->iMultiImageCopies[pData->iTotalGangPrintEntries] = iCopies;
	  pData->iMultiImageMedia[pData->iTotalGangPrintEntries] = iMedia;
	  pData->iFileType[pData->iTotalGangPrintEntries] = iFileType;
	  pData->bFullBleed[pData->iTotalGangPrintEntries] = bFullBleed;
	  if (lpstrOwner)
	     strcpy(pData->lpstrOwner[pData->iTotalGangPrintEntries], lpstrOwner);
	  pData->iOrderNum[pData->iTotalGangPrintEntries] = iOrderNum;	  

	  pData->iSurface[pData->iTotalGangPrintEntries] = iSurface;
	  pData->iBorderWidth[pData->iTotalGangPrintEntries] = iBorderWidth;
	  if ( lpstrLine1 )
	     strcpy(pData->lpstrBPLine1[pData->iTotalGangPrintEntries], lpstrLine1);
	  pData->iBPLine1[pData->iTotalGangPrintEntries] = bLine1;
	  if ( lpstrLine2 )
		  strcpy(pData->lpstrBPLine2[pData->iTotalGangPrintEntries], lpstrLine2);
	  pData->iBPLine2[pData->iTotalGangPrintEntries] = bLine2;

	  return TRUE;
	  }
   }

// Additional information for Multi Image Cos File printing
//
BOOL CPrintPaneState::AddMultiImage(int iCopies, int iMedia, LPCSTR lpstrOwner, LPCSTR lpstrOrderNum, 
										int iFileType, BOOL bFullBleed, int iSurface, int iBorderWidth,
											LPCSTR lpstrLine1, BOOL bLine1, LPCSTR lpstrLine2, BOOL bLine2)
   {
   // do we have room?
   if (pData->iTotalGangPrintEntries >= MAX_GANG_FILES)
	  return FALSE;
   else
	  {
	  // add it
	  pData->iMultiImageCopies[pData->iTotalGangPrintEntries] = iCopies;
	  pData->iMultiImageMedia[pData->iTotalGangPrintEntries] = iMedia;
	  pData->iFileType[pData->iTotalGangPrintEntries] = iFileType;
	  pData->bFullBleed[pData->iTotalGangPrintEntries] = bFullBleed;
	  if (lpstrOwner)
	     strcpy(pData->lpstrOwner[pData->iTotalGangPrintEntries], lpstrOwner);
	  if (lpstrOrderNum)
	  {
	     strcpy(pData->lpstrOrderNum[pData->iTotalGangPrintEntries], lpstrOrderNum);
		 pData->iOrderNum[pData->iTotalGangPrintEntries] = atoi(lpstrOrderNum);
	  }
	  else
		 pData->iOrderNum[pData->iTotalGangPrintEntries] = 0;

	  pData->iSurface[pData->iTotalGangPrintEntries] = iSurface;
	  pData->iBorderWidth[pData->iTotalGangPrintEntries] = iBorderWidth;
	  if ( lpstrLine1 )
	     strcpy(pData->lpstrBPLine1[pData->iTotalGangPrintEntries], lpstrLine1);
	  pData->iBPLine1[pData->iTotalGangPrintEntries] = bLine1;
	  if ( lpstrLine2 )
		  strcpy(pData->lpstrBPLine2[pData->iTotalGangPrintEntries], lpstrLine2);
	  pData->iBPLine2[pData->iTotalGangPrintEntries] = bLine2;

	  return TRUE;
	  }
   }

BOOL CPrintPaneState::AddMultiImage(char* pImageData, int iLength )
{
	// do we have room?

	if (pData->iTotalGangPrintEntries >= MAX_GANG_FILES)
		return FALSE;
	else
	{
		pData->pImageData[pData->iTotalGangPrintEntries] = NULL;
		pData->iImageDataSize[pData->iTotalGangPrintEntries] = iLength;
		pData->pImageData[pData->iTotalGangPrintEntries] = new char[iLength + 1];
		if ( pData->pImageData[pData->iTotalGangPrintEntries] != NULL )
		{
			memcpy(pData->pImageData[pData->iTotalGangPrintEntries], pImageData, iLength );
			return TRUE;
		}
	}

	return FALSE;
}

void CPrintPaneState::UpdateMultiImage(int idx, char* pImageData, int iLength )
{
	memcpy(pData->pImageData[idx], pImageData, iLength );

	return;
}


int CPrintPaneState::GetFileType(int idx)
   {
   int iRet = 0;

   if (idx >= 0 && idx < pData->iTotalGangPrintEntries)
      iRet = pData->iFileType[idx];

   return iRet;
   }

BOOL CPrintPaneState::GetFullBleed(int idx)
   {
   BOOL bRet = FALSE;

   if (idx >= 0 && idx < pData->iTotalGangPrintEntries)
      bRet = pData->bFullBleed[idx];

   return bRet;
   }

int CPrintPaneState::GetSurface(int idx)
{
	int iSurface = 0;

	if (idx >= 0 && idx < pData->iTotalGangPrintEntries)
		iSurface = pData->iSurface[idx];

	return iSurface;
}

int CPrintPaneState::GetBorderWidth(int idx)
{
	int iBorderWidth = 0;

	if (idx >= 0 && idx < pData->iTotalGangPrintEntries)
		iBorderWidth = pData->iBorderWidth[idx];

	return iBorderWidth;
}

//
// gang print stuff
//

BOOL CPrintPaneState::AddGangPrintFile(CString& csName, int iWidth, int iHeight, int iOrderNumber, LPCSTR lpstrOrderNum )
   {
   // do we have room?
   if (pData->iTotalGangPrintEntries >= MAX_GANG_FILES)
	  return FALSE;
   else
	  {
	  // add it
	  strcpy(pData->GangPrintTempFileName[pData->iTotalGangPrintEntries], csName);
	  pData->GangPrintDims[pData->iTotalGangPrintEntries].x = iWidth;
	  pData->GangPrintDims[pData->iTotalGangPrintEntries].y = iHeight;
	  if (lpstrOrderNum)
	  {
	     strcpy(pData->lpstrOrderNum[pData->iTotalGangPrintEntries], lpstrOrderNum);
		 pData->iOrderNum[pData->iTotalGangPrintEntries] = atoi(lpstrOrderNum);
	  }
	  else
	  {
		pData->iOrderNum[pData->iTotalGangPrintEntries] = iOrderNumber;
	  }
	  pData->iTotalGangPrintEntries++;
	  return TRUE;
	  }
   }

void CPrintPaneState::DeleteGangPrintFile(CString& csName)
   {
   int i, j;

   if (pData->iTotalGangPrintEntries < 1) return;

   for (i = 0; i < pData->iTotalGangPrintEntries; i++)
	  {
	  // a match
	  if (csName.CompareNoCase(pData->GangPrintTempFileName[i]) == 0)
		 {
		 // delete the file in case the driver has not done so
		 ::DeleteFile(csName);
		 // copy down
		 for (j = i; j < pData->iTotalGangPrintEntries - 1; j++)
			{
		    strcpy(pData->GangPrintTempFileName[j], pData->GangPrintTempFileName[j + 1]);
	        pData->GangPrintDims[j].x = pData->GangPrintDims[j + 1].x;
	        pData->GangPrintDims[j].y = pData->GangPrintDims[j + 1].y;			
			}
		 pData->iTotalGangPrintEntries--;
		 break;
		 }
	  }
   }

void CPrintPaneState::DeleteGangPrintEntry(int idx, BOOL bDeleteFile)
{
	int i;

	if (pData->iTotalGangPrintEntries > 1)
	{
		i = idx;

		// delete the file in case the driver has not done so
		if ( bDeleteFile )
			::DeleteFile(pData->GangPrintTempFileName[i]);


		if ( i == pData->iTotalGangPrintEntries - 1 )
		{
			// Last entry just clear it
			if ( pData->pImageData[i] )
			{
				delete pData->pImageData[i];
				pData->pImageData[i] = NULL;
			}
		}
		else
		{
			// copy down
			for (i; i < pData->iTotalGangPrintEntries - 1; i++)
			{
				strcpy(pData->GangPrintTempFileName[i], pData->GangPrintTempFileName[i + 1]);
				pData->GangPrintDims[i].x = pData->GangPrintDims[i + 1].x;
				pData->GangPrintDims[i].y = pData->GangPrintDims[i + 1].y;
				
				pData->iMultiImageCopies[i] = pData->iMultiImageCopies[i + 1];
				pData->iMultiImageMedia[i] = pData->iMultiImageMedia[i + 1];

				pData->iFileType[i] = pData->iFileType[i + 1];
				pData->bFullBleed[i] = pData->bFullBleed[i + 1];
				
				if (pData->lpstrOwner[i])
					strcpy(pData->lpstrOwner[i], pData->lpstrOwner[i + 1]);
				
				pData->iOrderNum[i] = pData->iOrderNum[i + 1];
				pData->iSurface[i] = pData->iSurface[i + 1];
				pData->iBorderWidth[i] = pData->iBorderWidth[i + 1];
				
				if ( pData->lpstrBPLine1[i] )
					strcpy(pData->lpstrBPLine1[i], pData->lpstrBPLine1[i + 1]);
				pData->iBPLine1[i] = pData->iBPLine1[i + 1];
				
				if ( pData->lpstrBPLine2[i] )
					strcpy(pData->lpstrBPLine2[i], pData->lpstrBPLine2[i + 1]);
				pData->iBPLine2[i] = pData->iBPLine2[i + 1];
				
				pData->iImageDataSize[i] = pData->iImageDataSize[i+1];
				if ( pData->pImageData[i] )
				{
					delete pData->pImageData[i];
					pData->pImageData[i] = NULL;
					pData->pImageData[i] = new char[pData->iImageDataSize[i] + 1];
					memcpy(pData->pImageData[i], pData->pImageData[i+1], pData->iImageDataSize[i] );
				}
			}

			if ( i == pData->iTotalGangPrintEntries - 1 )
			{
				// Last entry just clear it
				if ( pData->pImageData[i] )
				{
					delete pData->pImageData[i];
					pData->pImageData[i] = NULL;
				}
			}
		}
		pData->iTotalGangPrintEntries--;
	}
	else if (pData->iTotalGangPrintEntries == 1)
	{
		// delete the file in case the driver has not done so
		if ( bDeleteFile )
		::DeleteFile(pData->GangPrintTempFileName[0]);

		if ( pData->pImageData[0] )
		{
			delete pData->pImageData[0];
			pData->pImageData[0] = NULL;
		}

		pData->iTotalGangPrintEntries = 0;
	}
}

void CPrintPaneState::GetGangPrintFile(int idx, CString& csName)
   {
   if (idx < pData->iTotalGangPrintEntries)
      csName = pData->GangPrintTempFileName[idx];
   }

void CPrintPaneState::GetGangPrintFileDims(int idx, int& iWidth, int& iHeight)
   {
   iWidth = iHeight = 0;

   if (idx >= 0 && idx < pData->iTotalGangPrintEntries)
	  {
	  iWidth = pData->GangPrintDims[idx].x;
	  iHeight = pData->GangPrintDims[idx].y;
	  }      
   }

int CPrintPaneState::GetGangPrintEntries()
   {
   if (!pData) return 0;   
   return pData->iTotalGangPrintEntries;
   }

int CPrintPaneState::GetMultiImageCopies( int idx )
   {
   if (!pData) return 0;   
   return pData->iMultiImageCopies[idx];
   }

int CPrintPaneState::GetMultiImageMedia( int idx )
   {
   if (!pData) return 0;   
   return pData->iMultiImageMedia[idx];
   }

void CPrintPaneState::GetMultiImageOwner(int idx, CString& csOwner)
   {
   if (pData)
      csOwner = pData->lpstrOwner[idx];
   }

void CPrintPaneState::GetMultiImageOrderNum(int idx, CString& csOrderNum)
   {
   if (pData)
      csOrderNum = pData->lpstrOrderNum[idx];
   }

int CPrintPaneState::GetMultiImageOrderNum( int idx )
   {
   if (!pData) return 0;   
   return pData->iOrderNum[idx];
   }

void CPrintPaneState::GetMultiImageData(int idx, char* pImageData)
{
   if (pData)
	memcpy(pImageData, pData->pImageData[idx], pData->iImageDataSize[idx]);
}

void CPrintPaneState::GetBackprintLine1( int idx, CString& csLine1 )
{
   if (pData)
      csLine1 = pData->lpstrBPLine1[idx];
}

BOOL CPrintPaneState::GetLine1Status( int idx )
{
	return pData->iBPLine1[idx];
}

void CPrintPaneState::GetBackprintLine2( int idx, CString& csLine2 )
{
   if (pData)
      csLine2 = pData->lpstrBPLine2[idx];
}

BOOL CPrintPaneState::GetLine2Status( int idx )
{
	return pData->iBPLine2[idx];
}

BOOL CPrintPaneState::IspData()
{
   if (pData)
	   return TRUE;
   else
	   return FALSE;
}
