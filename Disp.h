/////////////////////////////////////////////////////////////////////////////
// DISP.H
//                                                                        
// display class for RasterPlus95
// Copyright (C) 1996-97 Graphx, Inc.
//

#ifndef __DISP_H__
#define __DISP_H__

//#include <afxext.h>
//#include "lead61\include\l_bitmap.h"
//#include "lead61\include\l_error.h"
//#include "..\CmnLibs\lead80\include\l_bitmap.h"
//#include "..\CmnLibs\lead80\include\l_error.h"

#include "..\CmnLibs\Current LeadTools\include\l_bitmap.h"
#include "..\CmnLibs\Current LeadTools\include\l_error.h"

#include "prev.h" 

class CDisp
   {
public:
   CDisp();
   ~CDisp();   
   void DisplayImage(LPRECT pRect, CPaintDC& dc);   
   void DisplayImage(LPRECT pRect, CClientDC& dc);

   void DestroyImage();
   BOOL CreateImage(PPREVIEWINFO);
   HCURSOR DragIcon();

   pBITMAPHANDLE GetBitmap();

protected:            
   HICON DefIcon;
   FILEINFO ImageFileInfo;
   pBITMAPHANDLE pBitmap;
   HPALETTE hPalette;    
   LPLOGPALETTE lpLogPalette; 
   HANDLE hPaletteHandle;
   CPalette* m_pPalette;
   BOOL bPalette;       
   void LoadPalette(CPaintDC&);
   void LoadPalette(CClientDC& dc);
   void MatchAspectRatio(LPRECT, int, int);
   void CompressImage();
   int IconOffset;   
   };
#endif
