/////////////////////////////////////////////////////////////////////////////
// PLIST.H
//                                                                        
// Pseudo list handling (actually CListView) for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __PLIST_H__
#define __PLIST_H__

#include <afxcview.h>

class CPseudoListBox : public CObject
   {
public:
   CPseudoListBox(CListView* pv);   
   ~CPseudoListBox();
   void InsertString(int i, LPCSTR pCQueueFile);
   void SetSel(int i, BOOL bVal = TRUE);
   int GetCount();
   int GetCaretIndex();
   int GetSelCount();
   int GetText(int i, LPSTR pCQueueFile);
   int GetSelItems(int cnt, LPINT LocalSelBuf);
   int DeleteString(int idx);
   int SetTopIndex(int idx);
   int SelItemRange(BOOL bSelect, int nFirstItem, int nLastItem);
   BOOL IsSelected(int i);
   BOOL CalcStringEllipsis(HDC hdc, LPTSTR lpszString, int cchMax, UINT uColWidth);
   void DrawItemColumn(HDC hdc, LPTSTR lpsz, LPRECT prcClip);	    
protected:
   CListView* pView;
   CImageList ci;   
   }; 
#endif