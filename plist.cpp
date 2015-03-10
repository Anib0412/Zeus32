/////////////////////////////////////////////////////////////////////////////
// PLIST.CPP
//                                                                        
// Pseudo list handling (actually CListView) for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#include "stdafx.h"
#include "zeus.h"
#include "plist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPseudoListBox::CPseudoListBox(CListView* pv)   
   {
   HICON hIcon;
   pView = pv;   
   ci.Create(16, 16, TRUE, 8, 0);

   hIcon = AfxGetApp()->LoadIcon(IDI_STATUS_HOLD); 
   ci.Add(hIcon);
   hIcon = AfxGetApp()->LoadIcon(IDI_STATUS_WAIT1); 
   ci.Add(hIcon);
   hIcon = AfxGetApp()->LoadIcon(IDI_STATUS_OK); 
   ci.Add(hIcon);
   hIcon = AfxGetApp()->LoadIcon(IDI_STATUS_NOTOK); 
   ci.Add(hIcon);
   hIcon = AfxGetApp()->LoadIcon(IDI_STATUS_OK_DELETED); 
   ci.Add(hIcon);
   hIcon = AfxGetApp()->LoadIcon(IDI_STATUS_PROPS_DEVICE); 
   ci.Add(hIcon);					 
   hIcon = AfxGetApp()->LoadIcon(IDI_STATUS_PROPS_TRANSFORM); 
   ci.Add(hIcon);					 
   hIcon = AfxGetApp()->LoadIcon(IDI_STATUS_PROPS_COLOR); 
   ci.Add(hIcon);					 
   									 
   ci.SetBkColor(GetSysColor(COLOR_WINDOW));

   // get the list control
   CListCtrl& ctlList = pView->GetListCtrl();

   // set our image list
   ctlList.SetImageList (&ci, LVSIL_SMALL);      
   }

CPseudoListBox::~CPseudoListBox()
   {
   ci.DeleteImageList();
   }

void CPseudoListBox::InsertString(int i, LPCSTR pCQueueFile)
   {
   LV_ITEM lvI;   
    
   CListCtrl& ctlList = pView->GetListCtrl();

   // create LV_ITEM
   lvI.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE | LVIF_IMAGE;
   lvI.state = 0;      
   lvI.stateMask = 0;  
   lvI.iItem = i;
   lvI.iImage = 0;
   lvI.iSubItem = 0;
   lvI.pszText = LPSTR_TEXTCALLBACK; 
   lvI.cchTextMax = _MAX_PATH;      
   lvI.lParam = (LPARAM)pCQueueFile;
   // insert it
   ctlList.InsertItem(&lvI);
   // insert subitems
   ctlList.SetItemText(i, 1, LPSTR_TEXTCALLBACK); // description
   ctlList.SetItemText(i, 2, LPSTR_TEXTCALLBACK); // Prefs
   }
	 
void CPseudoListBox::SetSel(int i, BOOL bVal)
   {
   CListCtrl& ctlList = pView->GetListCtrl();

   ctlList.SetItemState(i, bVal ? 0x0f : 0, LVIS_SELECTED | LVIS_FOCUSED);
   }

int CPseudoListBox::GetCount()
   {
   CListCtrl& ctlList = pView->GetListCtrl();

   return ctlList.GetItemCount();
   }

int CPseudoListBox::GetCaretIndex()
   {
   int cdebug;
   
   CListCtrl& ctlList = pView->GetListCtrl();

   cdebug = ctlList.GetNextItem(-1, LVNI_FOCUSED);

   if (cdebug < 0) cdebug = 0;
   return cdebug;
   }

int CPseudoListBox::GetSelCount()
   {
   CListCtrl& ctlList = pView->GetListCtrl();

   return ctlList.GetSelectedCount();
   }

int CPseudoListBox::GetText(int i, LPSTR pCQueueFile)
   {
   DWORD dwVal;

   CListCtrl& ctlList = pView->GetListCtrl();
   
   if (dwVal = ctlList.GetItemData(i))
      {
	  *((DWORD*)pCQueueFile) = dwVal;
      return LB_OKAY;
	  }
   else
      return LB_ERR;
   }

int CPseudoListBox::GetSelItems(int cnt, LPINT LocalSelBuf)
   {
   int i;
   int Selected = 0;

   CListCtrl& ctlList = pView->GetListCtrl();

   for (i = 0; i < ctlList.GetItemCount(); i++)
      {
	  if(ctlList.GetItemState(i, LVIS_SELECTED))
	     {
	     *LocalSelBuf++ = i;		 
		 Selected++;
		 }
	  }
   return Selected;
   }

int CPseudoListBox::DeleteString(int idx)
   {
   CListCtrl& ctlList = pView->GetListCtrl();

   ctlList.DeleteItem(idx);

   return ctlList.GetItemCount();
   }

int CPseudoListBox::SetTopIndex(int idx)
  {
  CListCtrl& ctlList = pView->GetListCtrl();

  if (ctlList.EnsureVisible(idx, FALSE))
     return LB_OKAY;
  else
     return LB_ERR;
  }

int CPseudoListBox::SelItemRange(BOOL bSelect, int nFirstItem, int nLastItem)
   {
   int i;

   CListCtrl& ctlList = pView->GetListCtrl();

   for (i = nFirstItem; i <= nLastItem; i++)
      { 
      ctlList.SetItemState(i, bSelect ? 0x0f : 0, LVIS_SELECTED | LVIS_FOCUSED);
	  }
   return LB_OKAY;
   }

BOOL CPseudoListBox::IsSelected(int i)
   {
   CListCtrl& ctlList = pView->GetListCtrl();

   return (BOOL)ctlList.GetItemState(i, LVIS_SELECTED);
   }

//
//  FUNCTION:   CalcStringEllipsis(HDC, LPTSTR, int, UINT)
//
//  PURPOSE:    Determines whether the specified string is too wide to fit in
//              an allotted space, and if not truncates the string and adds some
//              points of ellipsis to the end of the string.
//
//  PARAMETERS:
//      hdc        - Handle of the DC the string will be drawn on.
//      lpszString - Pointer to the string to verify
//      cchMax     - Maximum size of the lpszString buffer.
//      uColWidth  - Width of the space in pixels to fit the string into.
//
//  RETURN VALUE:
//      Returns TRUE if the string needed to be truncated, or FALSE if it fit
//      into uColWidth.
//

BOOL CPseudoListBox::CalcStringEllipsis(HDC hdc, LPTSTR lpszString, int cchMax, UINT uColWidth)
{
    const TCHAR szEllipsis[] = TEXT("...");
    SIZE   sizeString;
    SIZE   sizeEllipsis;
    int    cbString;
    LPTSTR lpszTemp;
    BOOL   fSuccess = FALSE;

    // Adjust the column width to take into account the edges
    uColWidth -= 4;

    __try
    {
        // Allocate a string for us to work with.  This way we can mangle the
        // string and still preserve the return value
        lpszTemp = (LPTSTR) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cchMax);
        if (!lpszTemp)
        {	            
            __leave;
        }
        lstrcpy(lpszTemp, lpszString);

        // Get the width of the string in pixels
        cbString = lstrlen(lpszTemp);
        if (!(GetTextExtentPoint32)(hdc, lpszTemp, cbString, &sizeString))
        {	            
            __leave;
        }

        // If the width of the string is greater than the column width shave
        // the string and add the ellipsis
        if ((ULONG)sizeString.cx > uColWidth)
        {
            if (!(GetTextExtentPoint32)(hdc, szEllipsis, lstrlen(szEllipsis),
                                       &sizeEllipsis))
            {	                
                __leave;
            }

            while (cbString > 0)
            {
                lpszTemp[--cbString] = 0;
                if (!(GetTextExtentPoint32)(hdc, lpszTemp, cbString, &sizeString))
                {	                    
                    __leave;
                }

                if ((ULONG)(sizeString.cx + sizeEllipsis.cx) <= uColWidth)
                {
                    // The string with the ellipsis finally fits, now make sure
                    // there is enough room in the string for the ellipsis
                    if (cchMax >= (cbString + lstrlen(szEllipsis)))
                    {
                        // Concatenate the two strings and break out of the loop
                        lstrcat(lpszTemp, szEllipsis);
                        lstrcpy(lpszString, lpszTemp);
                        fSuccess = TRUE;
                        __leave;
                    }
                }
            }
        }
        else
        {
            // No need to do anything, everything fits great.
            fSuccess = TRUE;
        }
    }
    __finally
    {
        // Free the memory
        HeapFree(GetProcessHeap(), 0, (LPVOID)lpszTemp);
        return (fSuccess);
    }
}

//
//  FUNCTION:   DrawItemColumn(HDC, LPTSTR, LPRECT)
//
//  PURPOSE:    Draws the text for one of the columns in the list view.
//
//  PARAMETERS:
//      hdc     - Handle of the DC to draw the text into.
//      lpsz    - String to draw.
//      prcClip - Rectangle to clip the string to.
//

void CPseudoListBox::DrawItemColumn(HDC hdc, LPTSTR lpsz, LPRECT prcClip)
   {
   TCHAR szString[256];

   // Check to see if the string fits in the clip rect.  If not, truncate
   // the string and add "...".
   lstrcpy(szString, lpsz);
   CalcStringEllipsis(hdc, szString, 256, prcClip->right - prcClip->left);

   // print the text
   ExtTextOut(hdc, prcClip->left + 2, prcClip->top + 1, ETO_CLIPPED | ETO_OPAQUE,   
      prcClip, szString, lstrlen(szString), NULL);
   }

