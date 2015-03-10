/////////////////////////////////////////////////////////////////////////////
// TRANSFRM.CPP
//                                                                        
// Transform class for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//
//
// mods:
//
// 10-15-96  Build 45 - replaced GetProfileString with CProfile class so that we can 
//                         make sure that Profile stuff is centralized

#include"stdafx.h"
#include"zeus.h"
#include"transfrm.h"
#include"profile.h"
#include<string.h>
#include"xfrmdefs.h"
#include"profile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CTransform, CObject, 0)

CTransform::CTransform()
  {
  }
  
CTransform::~CTransform()
  {
  }
  
void CTransform::Serialize(CArchive& ar, WORD verCheck )
   {    
   if (ar.IsStoring())
      {
      ar << Name << StartType << XZoom << YZoom << XOff << YOff << Rotate << Description;
      }      
   else                 
      {                                         
      ar >> Name >> StartType >> XZoom >> YZoom >> XOff >> YOff >> Rotate >> Description;
      }   
   }   

const CTransform& CTransform::operator=(const CTransform& ctSrc) // assignment	  
   {        
   if (this != &ctSrc)
      {        
      // these assignments will delete themselves
      if (ctSrc.Name.GetLength() > 0) 
		 Name = ctSrc.Name;
	  else
		 Name = "";
      if (ctSrc.StartType.GetLength() > 0)
		 StartType = ctSrc.StartType;
	  else
		 StartType = "";
      if (ctSrc.XZoom.GetLength() > 0)
		 XZoom = ctSrc.XZoom;
	  else
		 XZoom = "";
	  if (ctSrc.YZoom.GetLength() > 0)
		 YZoom = ctSrc.YZoom;
	  else
		 YZoom = "";
	  if (ctSrc.XOff.GetLength() > 0)
         XOff = ctSrc.XOff;
	  else
		 XOff = "";
	  if (ctSrc.YOff.GetLength() > 0)
         YOff = ctSrc.YOff;
	  else
		 YOff = "";
	  if (ctSrc.Rotate.GetLength() > 0)
         Rotate = ctSrc.Rotate;
	  else
		 Rotate = "";
	  if (ctSrc.Description.GetLength() > 0)
         Description = ctSrc.Description;
	  else
		 Description = "";
      }
   return *this;
   }   
      
BOOL CTransform::operator==(const CTransform& dm)
   {        
   return IsSame(dm);
   }

BOOL CTransform::operator!=(const CTransform& dm)
   {        
   return (!IsSame(dm));
   }
   
BOOL CTransform::IsSame(const CTransform& dm)
   {        
   if ((Name == dm.Name) &&      
       (StartType == dm.StartType) &&  
       ((atof(XZoom) - atof(dm.XZoom)) == 0.0) && 
	   ((atof(YZoom) - atof(dm.YZoom)) == 0.0) && 
	   ((atof(XOff) - atof(dm.XOff)) == 0.0) && 
	   ((atof(YOff) - atof(dm.YOff)) == 0.0) && 
	   ((atof(Rotate) - atof(dm.Rotate)) == 0.0) &&        
       (Description == dm.Description))
      return TRUE;
   else
      return FALSE;    
   }   

void CTransform::WriteToIni()
   {                        
   CString csSection;
   CString StrVals;
   CProfile Profile;
   
   // load the section
   csSection.LoadString(IDS_CUSTOMXFORMS);
   
   // write the descriptor string
   wsprintf(StrVals.GetBuffer(_MAX_PATH), "%s,%s,%s,%s,%s,%s,%s",
      (const char*)StartType, (const char*)XZoom, (const char*)YZoom, 
      (const char*)XOff, (const char*)YOff, (const char*)Rotate, (const char*)Description);
   StrVals.ReleaseBuffer(_MAX_PATH);         
   
   // write the entry
   Profile.WriteStr(csSection, Name, StrVals);
   }
   
BOOL CTransform::ReadFromIni(UINT iSection, char *cName)
   {                        
   CProfile Profile;
   CString csSection, csWorkingBuffer;
   char* p;  
   
   // store name    
   Name = cName;          
   
   // get the transform data   
   csWorkingBuffer = Profile.GetStr(iSection, Name);
   
   // check length
   if (csWorkingBuffer.GetLength() < 1) return FALSE;
   
   // get a buffer
   p = csWorkingBuffer.GetBuffer(_MAX_PATH);  
   
   // extract the transform vars
   StartType = strtok(p, ",");     
   XZoom = strtok(NULL, ",");
   YZoom = strtok(NULL, ",");
   XOff = strtok(NULL, ",");
   YOff = strtok(NULL, ",");
   Rotate = strtok(NULL, ",");
   Description = strtok(NULL, ",");           
   
   // release the buffer
   csWorkingBuffer.ReleaseBuffer(_MAX_PATH);  
   
   return TRUE;
   }
   
BOOL CTransform::ReadFromIni()
   {                        
   CString csCustom, csSection, csWorkingBuffer;
   char* p;     
   CProfile Profile;

   // get custom xform name
   csCustom.LoadString(IDS_CUSTOMXFORMNAME);

   // are we loading custom?
   if (csCustom == Name) 
	   {
	   LoadCustom();
	   return TRUE;
	   }
   
   // get the transform name section
   csSection.LoadString(IDS_CUSTOMXFORMS);	   
   
   // get the transform data
   csWorkingBuffer = Profile.GetStr(csSection, Name);
   
   // check length
   if (csWorkingBuffer.GetLength() < 1) return FALSE;
   
   // get a buffer
   p = csWorkingBuffer.GetBuffer(_MAX_PATH);  
   
   // extract the transform vars
   StartType = strtok(p, ",");     
   XZoom = strtok(NULL, ",");
   YZoom = strtok(NULL, ",");
   XOff = strtok(NULL, ",");
   YOff = strtok(NULL, ",");
   Rotate = strtok(NULL, ",");
   Description = strtok(NULL, ",");           
   
   // release the buffer
   csWorkingBuffer.ReleaseBuffer(_MAX_PATH);  
   
   return TRUE;
   }   

void CTransform::LoadTransform(LPCSTR csName)
   {         
   CString cs, csSection;
   char *p;   
   CProfile Profile;
   
   // store name
   Name = csName;
   							 
   // look in default transforms for our xform
   csSection.LoadString(IDS_DEFAULTXFORMS);   
   cs = Profile.GetStr(csSection, Name);

   // if not found, look in the custom transforms for our xform
   if (cs.GetLength() <= 0)
      {
	  csSection.LoadString(IDS_CUSTOMXFORMS);
      // now look up rest of stuff         
	  cs = Profile.GetStr(csSection, Name);
	  }
   // still not found, must be custom
   if (cs.GetLength() <= 0)
      {
	  LoadCustom();
	  }
   else
      {
	  // get a buffer
      p = cs.GetBuffer(_MAX_PATH);
   
      // extract the transform vars
      StartType = strtok(p, ",");     
      XZoom = strtok(NULL, ",");
      YZoom = strtok(NULL, ",");
      XOff = strtok(NULL, ",");
      YOff = strtok(NULL, ",");
      Rotate = strtok(NULL, ",");             
      Description = strtok(NULL, ",");             
	  }
   }

void CTransform::LoadDefaultTransform()
   {
   CProfile Profile;
   CString csName;

   csName = Profile.GetStr(IDS_RIPSECTION, IDS_TRANSFORM_DEFAULT);

   LoadTransform(csName);   
   }
   
void CTransform::LoadServerTransform()
   {
   CProfile Profile;
   CString csName;

   csName = Profile.GetStr(IDS_QUEUESECTION, IDS_DEFSERVER_XFORM);

   LoadTransform(csName);      
   }         
   
int CTransform::StartIndex()
   {
   CString csFit, csCrop;

   csFit.LoadString(IDS_SMARTFIT);
   csCrop.LoadString(IDS_SMARTCROP);
   
   if (StartType == csFit) 
      return SMARTFITTRANSFORM;
   else if (StartType == csCrop) 
      return SMARTCROPTRANSFORM;
   else
      return NOTRANSFORM;
   }      

void CTransform::LoadCustom()
   {
   StartType.LoadString(IDS_SMARTCROP);
   XZoom = "1.0";
   YZoom = "1.0";
   XOff = "0.0";
   YOff = "0.0";
   Rotate = "0.0";
   Description = "";
   Name.LoadString(IDS_CUSTOMXFORMNAME);
   }      
