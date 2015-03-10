/////////////////////////////////////////////////////////////////////////////
// ROUNDIT.CPP
//                                                                        
// int rounding routines for RasterPlus95 project
// Copyright (C) 1996-97 Graphx, Inc.
//
// mods:
//
// 9-14-97  Build 51 - simplify code by using + .5F short-cut a la Jaws

#include "stdafx.h"
#include "roundit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int RoundIt::MulDiv(int a, int b, int c)
   {
   int res;

   // get floating result
   res = (int)((float)a * (float)b / (float)c + .5F);

   return res;
   }

int RoundIt::Div(int a, int b)
   {
   int res;

   // get floating result
   res = (int)((float)a / (float)b + .5F);

   return res;					  
   }

int RoundIt::Convert(float fres)
   {   
   int res;

   // convert to int result
   res = (int)(fres + .5F);

   return res;					  
   }

int RoundIt::Convert(double dres)
   {   
   int res;

   // convert to int result
   res = (int)(dres + .5);

   return res;					  
   }
