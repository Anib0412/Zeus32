/////////////////////////////////////////////////////////////////////////////
// TRANSFRM.H
//                                                                        
// Transform class for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#ifndef __TRANSFRM_H__
#define __TRANSFRM_H__
                           
// transform vars                           
#define TRANVAR_XZOOM     0
#define TRANVAR_YZOOM     1
#define TRANVAR_XOFF      2
#define TRANVAR_YOFF      3
#define TRANVAR_ROTATION  4
#define TRANVAR_START	  5
#define TRANVAR_WIDTH     6
#define TRANVAR_HEIGHT    7

#define IDS_DEFSERVER_XFORM_ENTRY   IDS_SMARTFIT

class CTransform : public CObject
   { 
DECLARE_SERIAL(CTransform);
public:   
   CTransform();
   ~CTransform();
public:         
   CString Name;
   CString StartType;
   CString XZoom;
   CString YZoom;  
   CString XOff;
   CString YOff;   
   CString Rotate;
   CString Description;
   void WriteToIni();
   BOOL ReadFromIni(UINT, char *);
   BOOL ReadFromIni();
   void LoadDefaultTransform();
   void LoadServerTransform();
   int StartIndex();
   virtual void Serialize(CArchive& ar, WORD verCheck = 0);	
   const CTransform& operator=(const CTransform& cdmSrc); // assignment
   BOOL operator==(const CTransform& dm); // compare      
   BOOL operator!=(const CTransform& dm); // compare         
   void LoadCustom();   
   void LoadTransform(LPCSTR csName);
protected:
   BOOL IsSame(const CTransform& dm);   
   };   
#define MAXTRANSFORMS          50       

typedef struct tagXFormChange
   {
   CTransform* pCT;
   int ItemChanged;   
   } XFORMCHANGE, * PXFORMCHANGE;


#endif
   
