/////////////////////////////////////////////////////////////////////////////
// NFORMAT.CPP
//                                                                        
// number formatting routines for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#include"nformat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

NumberFormat::NumberFormat()
   {
   }

NumberFormat::~NumberFormat()
   {
   }

void NumberFormat::ConvertToComma(unsigned int val, CString& cs)
   {   
   char* buf;

   // get buffer
   buf = cs.GetBuffer(_MAX_PATH);
   // cal the proc
   ConvertToComma(val, buf);
   // release the buffer
   cs.ReleaseBuffer();
   }

void NumberFormat::ConvertToComma(unsigned int val, char *buf)
   {
   unsigned long billions, millions, thousands, ones;

   billions = val / 1000000000L;
   millions = (val - billions * 1000000000L) / 1000000L;
   thousands = (val - billions * 1000000000L - millions * 1000000L) / 1000L;
   ones =
      val - billions * 1000000000L - millions * 1000000L - thousands * 1000L;

   /* value can be 0 to 4,294,967,295 */
   if (billions)
      sprintf(buf, "%ld,%03ld,%03ld,%03ld", billions, millions, thousands, ones);
   else if (millions)
      sprintf(buf, "%ld,%03ld,%03ld", millions, thousands, ones);
   else if (thousands)
      sprintf(buf, "%ld,%03ld", thousands, ones);
   else
      sprintf(buf, "%ld", ones);
   //lstrcat(buf, " bytes");
   }

