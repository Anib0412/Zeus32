/////////////////////////////////////////////////////////////////////////////
// AntiAlias.cpp
//                                                                        
// anti-aliasing for Rip Process for RasterPlus95 project
// Copyright (C) 1996 Graphx, Inc.
//

#include "stdafx.h"
#include "AntiAlias.h"


CAntiAlias::CAntiAlias()
   {   
   Triangle_Coeffs = NULL;
   }

void CAntiAlias::SetOverSample(int os)
   {
   OverSample = os;
   }

CAntiAlias::~CAntiAlias()
   {
   if (Triangle_Coeffs) delete Triangle_Coeffs;
   }

//
// box filtering is cheapest, easiest and tackiest.
//
// Note: InputWidth and OutputWidth are actual byte widths
//
void CAntiAlias::boxfilterraster8(BYTE* pInputBits, BYTE* pOutputBits, 
   int InputWidth, int InputHeight)
   {
   int		g;
   int		y, x, yy, xx;
   BYTE		*sp, *dp, *srow, *drow, *srow1, *srow2;   
   int		div;
   int      OutputWidth;
   int      OutputHeight;
   int      delta;    
   
   srow = pInputBits;
   drow = pOutputBits;   
   div = OverSample * OverSample;
   OutputWidth = InputWidth / OverSample;
   OutputHeight = InputHeight / OverSample;
   delta = InputWidth * OverSample;

   if (OverSample == 2) 
      {
	  for (y = 0; y < OutputHeight; y++) 
  	     {
		 x = 0;
		 dp = drow;
		 srow1 = srow;
		 while (x < OutputWidth) 
		    {
		    srow2 = srow1 + InputWidth;
		    g = ((int) (srow1[0] + srow1[1] + srow2[0] + srow2[1])) >> 2;
		    *dp++ = g;
		    srow1 += 2;
		    x++;
		    }
		 drow += OutputWidth;
		 srow += delta;
	     }
      }
   else 
      {
	  for (y = 0; y < OutputHeight; y++) 
  	     {
		 x = 0;
		 dp = drow;
		 srow1 = srow;
		 while (x < OutputWidth) 
		    {
		    g = 0;
		    srow2 = srow1;
		    for (yy = 0; yy < OverSample; yy++, srow2 += InputWidth) 
			   {
			   sp = srow2;
			   for (xx = 0; xx < OverSample; xx++)
			      g += *sp++;
		       }
		    *dp++ = g / div;
		    srow1 += OverSample;
		    x++;
		    }
		 drow += OutputWidth;
		 srow += delta;
	     }
      }
   }

void CAntiAlias::boxfilterraster32(BYTE* pInputBits, BYTE* pOutputBits, 
  int InputWidth, int InputHeight)
   {
   int		r, g, b, a;
   int		y, x, yy, xx;
   BYTE		*sp, *dp, *srow, *drow, *srow1, *srow2;   
   int		div;
   int      OutputWidth;
   int      OutputHeight;
   int      delta;    
   
   srow = pInputBits;
   drow = pOutputBits;   
   div = OverSample * OverSample;
   OutputWidth = InputWidth / OverSample;
   OutputHeight = InputHeight / OverSample;
   delta = InputWidth * OverSample;
  
   if (OverSample == 2) 
	   {
	   for (y = 0; y < OutputHeight; y++) 
	      {
	      x = 0;
	      dp = drow;
	      srow1 = srow;
	      while (x < OutputWidth / 4) 
		     {
		     srow2 = srow1 + InputWidth;
		     r = ((int) (srow1[0] + srow1[4] + srow2[0] + srow2[4])) >> 2;
		     g = ((int) (srow1[1] + srow1[5] + srow2[1] + srow2[5])) >> 2;
		     b = ((int) (srow1[2] + srow1[6] + srow2[2] + srow2[6])) >> 2;
		     a = ((int) (srow1[3] + srow1[7] + srow2[3] + srow2[7])) >> 2;
		     dp[0] = r;
		     dp[1] = g;
		     dp[2] = b;
		     dp[3] = a;
			 dp += 4;
		     srow1 += 8;
		     x++;
	         }
	      drow += OutputWidth;
	      srow += delta;
	      }
       }
    else 
	   {
	   for (y = 0; y < OutputHeight; y++) 
	      {
	      x = 0;
	      dp = drow;
	      srow1 = srow;
	      while (x < OutputWidth / 4) 
		     {
		     r = g = b = a = 0;
		     srow2 = srow1;
		     for (yy = 0; yy < OverSample; yy++, srow2 += InputWidth) 
			    {
		        sp = srow2;
		        for (xx = 0; xx < OverSample; xx++, sp += 4) 
				   {
			       r += sp[0];
			       g += sp[1];
			       b += sp[2];
			       a += sp[3];
		           }
		        }
		     dp[0] = r / div;
		     dp[1] = g / div;
		     dp[2] = b / div;
		     dp[3] = a / div;		
		     dp += 4;
		     srow1 += 4 * OverSample;
		     x++;
	         }
	      drow += OutputWidth;
	      srow += delta;
	      } 
       }
	}

void CAntiAlias::boxfilterraster24(BYTE* pInputBits, BYTE* pOutputBits, 
  int InputWidth, int InputHeight)
   {
   int		r, g, b;
   int		y, x, yy, xx;
   BYTE		*sp, *dp, *srow, *drow, *srow1, *srow2;   
   int		div;
   int      OutputWidth;
   int      OutputHeight;
   int      delta;    
   
   srow = pInputBits;
   drow = pOutputBits;   
   div = OverSample * OverSample;
   OutputWidth = InputWidth / OverSample;
   OutputHeight = InputHeight / OverSample;
   delta = InputWidth * OverSample;
  
   if (OverSample == 2)    
	   {
	   for (y = 0; y < OutputHeight; y++) 
	      {
	      x = 0;
	      dp = drow;
	      srow1 = srow;
	      while (x < OutputWidth / 3) 
		     {
		     srow2 = srow1 + InputWidth;
		     r = ((int) (srow1[0] + srow1[3] + srow2[0] + srow2[3])) >> 2;
		     g = ((int) (srow1[1] + srow1[4] + srow2[1] + srow2[4])) >> 2;
		     b = ((int) (srow1[2] + srow1[5] + srow2[2] + srow2[5])) >> 2;		     
		     dp[0] = r;
		     dp[1] = g;
		     dp[2] = b;		     
			 dp += 3;
		     srow1 += 6;
		     x++;
	         }
	      drow += OutputWidth;
	      srow += delta;
	      }
       }
    else 
	   {
	   for (y = 0; y < OutputHeight; y++) 
	      {
	      x = 0;
	      dp = drow;
	      srow1 = srow;
	      while (x < OutputWidth / 3) 
		     {
		     r = g = b = 0;
		     srow2 = srow1;
		     for (yy = 0; yy < OverSample; yy++, srow2 += InputWidth) 
			    {
		        sp = srow2;
		        for (xx = 0; xx < OverSample; xx++, sp += 3) 
				   {
			       r += sp[0];
			       g += sp[1];
			       b += sp[2];			       
		           }
		        }
		     dp[0] = r / div;
		     dp[1] = g / div;
		     dp[2] = b / div;		     
		     dp += 3;
		     srow1 += 3 * OverSample;
		     x++;
	         }
	      drow += OutputWidth;
	      srow += delta;
	      } 
       }
	}

void CAntiAlias::InitTriangleFilter()
    {
    int			n, j, k;
    float		*fp, x, y, dx, m;
    
	n = OverSample * OverSample;
	if (Triangle_Coeffs) delete Triangle_Coeffs;
	fp = Triangle_Coeffs = new float[n];
	dx = 1.0F / OverSample;
	y = dx;
	for (k = 0; k < OverSample; k++, y += dx) 
	   {
	   m = 0.25F * (y * y - (y - dx) * (y - dx));
	   for (j = 0, x = 0.F; j < OverSample; j++, x += dx) 
	      {
		  *fp++ = m * (dx * (x + x + dx));
 	      }  
	   }
	}

void CAntiAlias::trifilterraster(BYTE* pInputBits, BYTE* pOutputBits, 
   int InputWidth, int InputHeight)
   {
   float	g;
   int		val;
   int		y, x, yy, xx;   
   BYTE		*sp1, *sp2, *dp, *srow, *drow, *srow1;
   int		delta;   
   float	*kp;
   int		ind1, ind2;   
   int		OutputWidth;
   int		OutputHeight;

   srow = pInputBits;
   drow = pOutputBits;   
   delta = OverSample * InputWidth;
   OutputWidth = InputWidth / OverSample;
   OutputHeight = InputHeight / OverSample;

   for (y = 0; y < OutputHeight; y++) 
      {
	  x = 0;
	  dp = drow;
	  srow1 = srow;
	  while (x < OutputWidth) 
	     {
		 g = 0.F;
		 sp1 = srow1;
		 sp2 = srow1 + (2 * OverSample - 1) * InputWidth;
		 kp = Triangle_Coeffs;
		 for (yy = 0; yy < OverSample; yy++, sp1 += InputWidth, sp2 -= InputWidth) 
		    {
		    ind1 = 0;
		    ind2 = 2 * OverSample - 1;
		    for (xx = 0; xx < OverSample; xx++, ind1++, ind2--) 
			   {
			   val = sp1[ind1] + sp1[ind2] + sp2[ind1] + sp2[ind2];
			   g += val * kp[0];
			   kp++;
		       }
		    }
		 *dp++ = (BYTE)g;
		 srow1 += OverSample;
		 x++;
	     }
	     drow += OutputWidth;
	     srow += delta;
	  }
   }
