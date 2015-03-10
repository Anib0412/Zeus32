/////////////////////////////////////////////////////////////////////////////
// NFORMAT.H
//                                                                        
// number formatting routines for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __NFORMAT_H__
#define __NFORMAT_H__

#include <afxext.h>
class NumberFormat : public CObject
   {
public:
   NumberFormat();
   ~NumberFormat();
   void ConvertToComma(unsigned int val, char *buf);
   void ConvertToComma(unsigned int val, CString& cs);
   };
#endif
