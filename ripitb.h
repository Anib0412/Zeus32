/////////////////////////////////////////////////////////////////////////////
// RIPITB.H
//                                                                        
// the encapsulation class for the bitmap rip Process
// Copyright (C) 1996-98 Graphx, Inc.
//
// Mods:
// 9-10-98  Version 2.0.5 - added new device rip loading code (disabled for now)
//

#ifndef __RIPITB_H__
#define __RIPITB_H__

#include "ripthrd.h"

class CRipItB : public CRipThread
   {
DECLARE_DYNAMIC(CRipItB)
public:
   CRipItB();
   ~CRipItB();

public:   
   BOOL Init(CDevice* pCD, int ot);
   void SetRipStruct(PRIPSTRUCT pR);            
   int Process();
   BOOL SuspendProcess();
   BOOL ResumeProcess();   
   BOOL GetDevErrorFromRip(CString& csErrStr);

protected:   
   BOOL CancelFlag;      

   CString cDebugStr;
   };

// from lead tools!
#ifdef TOP_LEFT
#undef TOP_LEFT
#endif 
//#define TOP_LEFT            0 /* Top-left of image is first in memory */
#define TOP_LEFT        1  // Top-left of image is first in memory

#ifdef TOP_RIGHT
#undef TOP_RIGHT
#endif 
//#define TOP_RIGHT           1 /* Top-right of image is first in memory */
#define TOP_RIGHT       2  // Top-right of image is first in memory

#ifdef BOTTOM_LEFT
#undef BOTTOM_LEFT
#endif 
//#define BOTTOM_LEFT         2 /* Bottom-left of image is first in memory */
#define BOTTOM_LEFT     4  // Bottom-left of image is first in memory

#ifdef BOTTOM_RIGHT
#undef BOTTOM_RIGHT
#endif 
//#define BOTTOM_RIGHT        3 /* Bottom-right of image is first in memory */
#define BOTTOM_RIGHT    3  // Bottom-right of image is first in memory

#ifdef TOP_LEFT90
#undef TOP_LEFT90
#endif 
//#define TOP_LEFT90          4
#define TOP_LEFT90      6  // RIGHT_TOP is TOP_LEFT rotated clockwise by 90 degrees 

#ifdef BOTTOM_LEFT90
#undef BOTTOM_LEFT90
#endif 
//#define BOTTOM_LEFT90       5
#define BOTTOM_LEFT90   5  // LEFT_TOP is BOTTOM_LEFT rotated clockwise by 90 degrees

#ifdef BOTTOM_LEFT270
#undef BOTTOM_LEFT270
#endif 
//#define BOTTOM_LEFT270      7
#define BOTTOM_LEFT270  7  // RIGHT_BOTTOM is BOTTOM_LEFT rotated clockwise by 270 degrees

#ifdef TOP_LEFT270
#undef TOP_LEFT270
#endif 
//#define TOP_LEFT270         8
#define TOP_LEFT270     8  // LEFT_BOTTOM is TOP_LEFT rotated clockwise by 270 degrees 

/*

#define BOTTOM_LEFT180  2  // TOP_RIGHT is BOTTOM_LEFT rotated clockwise by 180 degrees
#define TOP_LEFT180     3  // BOTTOM_RIGHT is TOP_LEFT rotated clockwise by 180 degrees

#define RIGHT_TOP       6  // First row is the right side, first column is top side
#define LEFT_BOTTOM     8  // First row is the left side, first column is top side

#define LEFT_TOP        5  // First row is the left side, first column is the top side
#define RIGHT_BOTTOM    7  // First row is the right side, first column is the bottom side
*/

// #define TOP_LEFT            0 /* Top-left of image is first in memory */
// #define TOP_RIGHT           1 /* Top-right of image is first in memory */
// #define BOTTOM_LEFT         2 /* Bottom-left of image is first in memory */
// #define BOTTOM_RIGHT        3 /* Bottom-right of image is first in memory */
// #define TOP_LEFT90          4
// #define BOTTOM_LEFT90       5
// #define BOTTOM_LEFT270      7
// #define TOP_LEFT270         8

#endif
