// CFrect stuff

#include <afxext.h>
#include"frect.h"
#include"roundit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif							   

CFrect::CFrect()
   {
   left = right = bottom = top = 0.0F;
   }   
   
CFrect::~CFrect()   
   {
   }
   
const CFrect& CFrect::operator=(const CFrect& cfSrc)
   {
   if (this != &cfSrc)
      {           
      left = cfSrc.left;
      top = cfSrc.top;
      right = cfSrc.right;
      bottom = cfSrc.bottom;      
      } 
   return *this;            
   }

const CFrect& CFrect::operator=(const CRect& crSrc)
   {                 
   // equate the floating rect to float conversions 
   // from a standard CRect
   left = (float)crSrc.left;
   top = (float)crSrc.top;
   right = (float)crSrc.right;
   bottom = (float)crSrc.bottom;      
   return *this;
   }
   
float CFrect::Width()
   {
   return (right - left);
   }   
   
float CFrect::Height()
   {
   return (bottom - top);
   }                   

void CFrect::ToCRect(CRect& cr)
   {
   RoundIt rnd;
   
   cr.left = rnd.Convert(left);
   cr.top = rnd.Convert(top);
   cr.right = rnd.Convert(right);
   cr.bottom = rnd.Convert(bottom);
   }