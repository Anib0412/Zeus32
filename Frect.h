// frect.h : header file
//      

#ifndef __FRECT_H__
#define __FRECT_H__

class CFrect
   {              
public:
   CFrect();
   ~CFrect();
   
   float left, top, right, bottom;   
   
   float Width();
   float Height();
   
   void ToCRect(CRect&);
   
   const CFrect& operator=(const CFrect& cfSrc);       // assignment
   const CFrect& operator=(const CRect& crSrc); // assignment of CRect
   };   

#endif
