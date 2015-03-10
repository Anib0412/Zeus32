/////////////////////////////////////////////////////////////////////////////
// ROUNDIT.H
//                                                                        
// int rounding routines for RasterPlus95 project
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __ROUNDIT_H__
#define __ROUNDIT_H__

class RoundIt
   {
public:
   int MulDiv(int a, int b, int c);
   int Div(int a, int b);
   int Convert(float fres);
   int Convert(double dres);
   };
#endif