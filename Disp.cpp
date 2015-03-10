/////////////////////////////////////////////////////////////////////////////
// DISP.CPP
//                                                                        
// display class for RasterPlus95
// Copyright (C) 1996-2001 Graphx, Inc.
//
// mods:
//
// 11-25-96 Build 46      - changed RemoveFile to DeleteFile
//  9-02-97 Build 51      - integrated lead 80
//  9-26-97 Build 51      - finally remove iconic preview window code
// 09-28-01 Version 5.0.0 - Changes to support High_Defintion Preview
// 04-15-03 Version 5.1.2 - Modified to support Lead Tools 13
//
#include"stdafx.h"
#include"zeus.h"
#include"disp.h"
#include"inidefs.h"
#include"profile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif	 

// display wrappers
CDisp::CDisp()
   {       
   m_pPalette = new(CPalette);   
   pBitmap = NULL;
   bPalette = FALSE;                      
   hPaletteHandle = NULL;
   hPalette = NULL;                            
   DefIcon = AfxGetApp()->LoadIcon(IDI_ICON_PREVIEW);
   return;   
   }
              
CDisp::~CDisp()              
   {                  
   // block others from using the bitmap dll
   EnterCriticalSection(&BITMAPENGINEMUTEX);
   // destroy the current image                         
   if (pBitmap) 
      {
      ::L_FreeBitmap(pBitmap);         
//      TRACE("DISP: Freeing bitmap\n");
      delete pBitmap;
//      TRACE("DISP: Deleting bitmap\n");
      }
   if (hPaletteHandle) ::GlobalFree(hPaletteHandle);
   if (m_pPalette) delete m_pPalette; 
   if (hPalette) ::DeleteObject(hPalette);

   // free up the bitmap engine
   LeaveCriticalSection(&BITMAPENGINEMUTEX);	 
   }
   
void CDisp::DisplayImage(LPRECT pRect, CPaintDC& dc)
   {               
   // Do painting
   if (pBitmap) 
      {                  
	  // block others from using the bitmap dll
      EnterCriticalSection(&BITMAPENGINEMUTEX);
   
      // create a palette
      LoadPalette(dc);            
      
      // paint the bitmap
	  ::L_PaintDC(dc.m_ps.hdc, pBitmap, NULL, NULL, pRect, NULL, SRCCOPY);      

	  // free up the bitmap engine
      LeaveCriticalSection(&BITMAPENGINEMUTEX);
	  }   
   }      
      
void CDisp::DisplayImage(LPRECT pRect, CClientDC& dc)
   {                              
   // Do painting
   if (pBitmap) 
      {                  
	  // block others from using the bitmap dll
      EnterCriticalSection(&BITMAPENGINEMUTEX);
   
      // create a palette
      LoadPalette(dc);            
      
      // paint the bitmap
	  ::L_PaintDC(dc.m_hDC, pBitmap, NULL, NULL, pRect, NULL, SRCCOPY);      

	  // free up the bitmap engine
      LeaveCriticalSection(&BITMAPENGINEMUTEX);
	  }   
   }      
      
BOOL CDisp::CreateImage(PPREVIEWINFO pPreviewInfo)
{   
	int nRet;
	CString csSection, csEntry;
	CString csTempPath;

	//BOOL compression;
	CProfile Profile;

	LOADFILEOPTION LoadFileOption;

	// get compression flag
	//compression = Profile.GetInt(IDS_PREVIEW, IDS_PREVIEW_COMPRESSION, IDDM_PREVIEW_COMPRESSION);   

	// block others from using the bitmap dll
	EnterCriticalSection(&BITMAPENGINEMUTEX);

	// destroy the current image
	if (pBitmap)
	{
		::L_FreeBitmap(pBitmap);
		delete pBitmap;
//		TRACE("DISP: Deleting bitmap\n");
		pBitmap = NULL;
	}
//	TRACE("DISP: Freeing bitmap\n");

	L_GetDefaultLoadFileOption(&LoadFileOption, sizeof LOADFILEOPTION);

	LoadFileOption.XResolution = pPreviewInfo->RipHPixelsPerInch;
	LoadFileOption.YResolution = pPreviewInfo->RipWPixelsPerInch;

	// get info about the bitmap data
	memset(&ImageFileInfo, 0, sizeof(FILEINFO));
    ImageFileInfo.uStructSize = sizeof(FILEINFO);
	nRet = ::L_FileInfoMemory((unsigned char *)pPreviewInfo->ImageBuffer, &ImageFileInfo, 
											pPreviewInfo->ImageSize, sizeof(FILEINFO), FILEINFO_TOTALPAGES, &LoadFileOption);

	// make a new bitmap
	if (nRet > 0)
	{
		pBitmap = new BITMAPHANDLE;
		memset(pBitmap, 0, sizeof(BITMAPHANDLE));

		pBitmap->XResolution = pPreviewInfo->RipHPixelsPerInch;
		pBitmap->YResolution = pPreviewInfo->RipWPixelsPerInch;

		// init it
		::L_InitBitmap(pBitmap, sizeof(BITMAPHANDLE), ImageFileInfo.Width, ImageFileInfo.Height, ImageFileInfo.BitsPerPixel);

		ImageFileInfo.XResolution = pPreviewInfo->RipHPixelsPerInch;
		ImageFileInfo.YResolution = pPreviewInfo->RipWPixelsPerInch;
		ImageFileInfo.Format = FILE_TGA;
		ImageFileInfo.Flags = FILEINFO_INFOVALID;

		// load it
		if(::L_LoadBitmapMemory((unsigned char *)pPreviewInfo->ImageBuffer, pBitmap, sizeof(BITMAPHANDLE), 0, ORDER_BGR,
											pPreviewInfo->ImageSize, NULL, &ImageFileInfo) <= 0)
		{
			delete pBitmap; // error!
//			TRACE("DISP: Deleting bitmap on error\n");
			pBitmap = NULL;
		}
	}
	else	
		pBitmap = NULL;

	// free up the bitmap engine
	LeaveCriticalSection(&BITMAPENGINEMUTEX);

	return (pBitmap ? TRUE : FALSE);
}

void CDisp::LoadPalette(CPaintDC& dc)
   {
   CPalette* m_pOldPalette;
   int i;                        
   static PALETTEENTRY peEntry[256];   

   // block others from using the bitmap dll
   EnterCriticalSection(&BITMAPENGINEMUTEX);
 	
   if(bPalette)
      {
      dc.SelectPalette(m_pPalette,0);
 	  dc.RealizePalette();
 	  }
   else
 	  {        
      // Create the palette
      hPalette = ::L_CreatePaintPalette(dc.m_ps.hdc, pBitmap);
 							
 	  if(hPalette) 
 	     {             
	     // Create the LogPalette
	     ::GetPaletteEntries(hPalette, 0, 256, peEntry); 
 	     hPaletteHandle = ::GlobalAlloc(GMEM_MOVEABLE,
 		     sizeof(LOGPALETTE) * 256 * sizeof(PALETTEENTRY));
 		 lpLogPalette = (LPLOGPALETTE) ::GlobalLock(hPaletteHandle);
 		 lpLogPalette->palVersion = 0x300;
 		 lpLogPalette->palNumEntries = 256;
 		 for (i = 0; i < 256; i++)
 		    {
 		    lpLogPalette->palPalEntry[i].peRed = peEntry[i].peRed;
 		 	lpLogPalette->palPalEntry[i].peGreen = peEntry[i].peGreen;
 			lpLogPalette->palPalEntry[i].peBlue = peEntry[i].peBlue;
 			lpLogPalette->palPalEntry[i].peFlags = 0;
 			}	     
 	 	  // Initilaize the CPalette object
 		  if(m_pPalette->CreatePalette(lpLogPalette))
 		     {
 		     m_pOldPalette = dc.SelectPalette(m_pPalette,0);
 			   dc.RealizePalette();
 			 bPalette = TRUE;
 			 }
 		 ::GlobalUnlock(hPaletteHandle);		
 		 }
      }     
   // free up the bitmap engine
   LeaveCriticalSection(&BITMAPENGINEMUTEX);
   }
   
void CDisp::LoadPalette(CClientDC& dc)
   {
   CPalette* m_pOldPalette;
   int i;                        
   static PALETTEENTRY peEntry[256];   

   // block others from using the bitmap dll
   EnterCriticalSection(&BITMAPENGINEMUTEX);
 	
   if(bPalette)
      {
      dc.SelectPalette(m_pPalette,0);
 	  dc.RealizePalette();
 	  }
   else
 	  {        
      // Create the palette
      hPalette = ::L_CreatePaintPalette(dc.m_hDC, pBitmap);
 							
 	  if(hPalette) 
 	     {             
	     // Create the LogPalette
	     ::GetPaletteEntries(hPalette, 0, 256, peEntry); 
 	     hPaletteHandle = ::GlobalAlloc(GMEM_MOVEABLE,
 		     sizeof(LOGPALETTE) * 256 * sizeof(PALETTEENTRY));
 		 lpLogPalette = (LPLOGPALETTE) ::GlobalLock(hPaletteHandle);
 		 lpLogPalette->palVersion = 0x300;
 		 lpLogPalette->palNumEntries = 256;
 		 for (i = 0; i < 256; i++)
 		    {
 		    lpLogPalette->palPalEntry[i].peRed = peEntry[i].peRed;
 		 	lpLogPalette->palPalEntry[i].peGreen = peEntry[i].peGreen;
 			lpLogPalette->palPalEntry[i].peBlue = peEntry[i].peBlue;
 			lpLogPalette->palPalEntry[i].peFlags = 0;
 			}	     
 	 	  // Initilaize the CPalette object
 		  if(m_pPalette->CreatePalette(lpLogPalette))
 		     {
 		     m_pOldPalette = dc.SelectPalette(m_pPalette,0);
 			   dc.RealizePalette();
 			 bPalette = TRUE;
 			 }
 		 ::GlobalUnlock(hPaletteHandle);		
 		 }
      }     
   // free up the bitmap engine
   LeaveCriticalSection(&BITMAPENGINEMUTEX);
   }
   
HCURSOR CDisp::DragIcon()
   {
   return DefIcon;
   }

#define RECTWIDTH(lpRect) (((LPRECT) lpRect)->right - ((LPRECT) lpRect)->left)
#define RECTHEIGHT(lpRect) (((LPRECT) lpRect)->bottom - ((LPRECT) lpRect)->top)
   
void CDisp::MatchAspectRatio(LPRECT pRect, int nWidth, int nHeight)
   {
   int cxRect, cyRect, oldBottom;            
   
   oldBottom = pRect->bottom;
   
   cyRect = RECTHEIGHT (pRect);
   cxRect = MulDiv (cyRect, nWidth, nHeight);   

   if (cxRect > RECTWIDTH (pRect))
      {
      cxRect = RECTWIDTH (pRect);
      cyRect = MulDiv (cxRect, nHeight, nWidth);
      }
   pRect->bottom = pRect->top + cyRect;   
   pRect->right = pRect->left + cxRect;   
   
   // offset it so it sits at the bottom of the rect
   IconOffset = oldBottom - pRect->bottom;
   
   return;
   }

void CDisp::CompressImage()
   {    
   int ret;                            
   char szTempFileName[_MAX_PATH];
   
   // get temp filename
   //GetTempFileName(0, "img", 0, szTempFileName);
   CString csTempPath;
   GetTempPath(_MAX_PATH, csTempPath.GetBuffer(_MAX_PATH));
   csTempPath.ReleaseBuffer();
   GetTempFileName(csTempPath, "img", 0, szTempFileName);   

   // compress the image   
   ret = ::L_SaveBitmap(szTempFileName, pBitmap, FILE_LEAD, 
      ImageFileInfo.BitsPerPixel, PQ1, NULL);
   
   // free the current bitmap   
   ::L_FreeBitmap(pBitmap); 
//   TRACE("DISP: Freeing bitmap\n");
   // get info about the new bitmap
   memset(&ImageFileInfo, 0, sizeof(FILEINFO));
   ImageFileInfo.uStructSize = sizeof(FILEINFO);
   ret = ::L_FileInfo(szTempFileName, &ImageFileInfo, sizeof(FILEINFO), FILEINFO_TOTALPAGES, NULL);
   // init it 
   if (ret > 0)
       {       
       memset(pBitmap, 0, sizeof(BITMAPHANDLE));
       // init it
	   ::L_InitBitmap(pBitmap, sizeof(BITMAPHANDLE), ImageFileInfo.Width, ImageFileInfo.Height, ImageFileInfo.BitsPerPixel);

	   // read it back   
	   if(::L_LoadBitmap(szTempFileName, pBitmap, sizeof(BITMAPHANDLE), 0, ORDER_BGR, NULL, NULL) <= 0)
	      {
	      // whoops
	      delete pBitmap;
//	      TRACE("DISP: Deleting bitmap on error\n");
	      pBitmap = NULL;
	      }
	   }
	else
	   {
	   delete pBitmap;
//	   TRACE("DISP: Deleting bitmap on error\n");
	   pBitmap = NULL;
	   }   
	
	// delete the temp file   
	::DeleteFile(szTempFileName);
   }
   
extern "C" L_INT FAR EXPORT WindowsOpen(const L_CHAR FAR *pFile, L_INT nMode, L_INT nShare)
   {
   return(5);
   }

L_INT FAR EXPORT WindowsClose(L_INT FD)
   {
   return TRUE;
   }

L_INT FAR EXPORT WindowsWrite(L_INT FD, L_CHAR FAR *pBuf, L_INT nCount)
   {
   return(nCount);
   }
      
L_INT32 FAR EXPORT WindowsSeek( L_INT FD, L_INT32 lnPos, L_INT nOrigin )
   {
   switch (nOrigin)
      {
	  case 0: /* SEEK_SET */
		   break;
      case 1: /* SEEK_CUR */
           break;
      case 2: /* SEEK_END */
    	   break;
	  }
   return(0L);
   }
   
pBITMAPHANDLE CDisp::GetBitmap()
{
	return pBitmap;
}