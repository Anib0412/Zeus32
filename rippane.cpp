////////////////////////////////////////////////////////////////////////////
// RIPPANE.CPP
//                                                                        
// rip pane defs for RasterPlus
// Copyright (C) 2000 Graphx, Inc.
//
// mods:
//

#include "stdafx.h"
#include "zeus.h"
#include "rippane.h"

IMPLEMENT_DYNAMIC(CPaneElement, CObject)

void CPaneArray::Destroy()
   {
   int i = 0;

   while (i < GetSize())
      {
      delete GetAt(i++);
      }     
   RemoveAll();
   }

void CPaneArray::DeleteElement(CPaneElement* pPassedElement)
   {
   int i = 0;
   CPaneElement* pRipElement;

   if (pPassedElement == NULL) return;

   for (i = 0; i < GetSize(); i++)
      {
	  if (pRipElement = (CPaneElement*)GetAt(i))
		 {
         if (pRipElement->pDocPtr == pPassedElement->pDocPtr)
			{
			delete pRipElement;
		    RemoveAt(i);
		    break;
			}
		 }
      }     
   }

BOOL CPaneArray::AnyRipActive()
   {
   int i;   
   BOOL bRet = FALSE;
   CPaneElement* pRipElement;

   for (i = 0; i < GetSize(); i++)
      {
	  // get the element
	  pRipElement = (CPaneElement*)GetAt(i);
	  // are we in rip?
	  if (pRipElement->InRip)
		 {
		 bRet = TRUE;
		 break;
		 }
      }        
   return bRet;
   }

