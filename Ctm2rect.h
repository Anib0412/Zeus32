/////////////////////////////////////////////////////////////////////////////
// CTM2RECT.H
//                                                                        
// convert CTM to a Rect for sizing for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __CTM2RECT_H__
#define __CTM2RECT_H__

#include "stdafx.h"
#include "ripstruct.h"

#define PIVALUE  3.141592654

// struct for sizing vals
typedef struct tagSIZING_VALS
   {
   float xzoom,
         yzoom,
         xoff,
         yoff,
         rotation;
   } SIZING_VALS;
   
typedef SIZING_VALS * PSIZING_VALS;     

#ifdef USE_INFINITE_BOUNDARY
typedef struct tagFPOINT
   {
   float x, y;
   } FPOINT;
   
typedef FPOINT * PFPOINT
         
typedef struct tagSIZED_POLY
   {
   FPOINT UpperLeft,
          LowerLeft,
          UpperRight,
          LowerRight;
   }SIZED_POLY;             
#endif

class CTM2Rect
   { 
public:            
   CTM2Rect();
   ~CTM2Rect();
   void ApplySizingValsToCRect(CRect& SizedRect, PSIZING_VALS, const CRect&, 
	  POINT& ptRotateCorrect, float& PicaWidth, float& PicaHeight);
   void GetMatrixProduct(CTM_ARRAY *, CTM_ARRAY *, CTM_ARRAY *);
   };   
#endif
