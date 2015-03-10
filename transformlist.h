/////////////////////////////////////////////////////////////////////////////
// TransformList.h
//                                                                        
// get all transforms for RasterPlus95 project
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __TRANSFORMLIST_H__
#define __TRANSFORMLIST_H__

#include <afxext.h>
#include"transfrm.h"

class CTransformList : public CObArray
   {
public:
   CTransformList();
   ~CTransformList();   
   void Clear();

   int LoadAll(BOOL AddCustom = TRUE);   
protected:   
   void LoadFromRegistry(int TransformType, int& idx);
   };

#endif
