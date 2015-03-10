// ColorTransform.cpp: implementation of the CColorTransform class.
//
// Copyright (C) 1996-2000 Graphx, Inc.
//
// mods:
//
// 3-03-00  Version 3.0.3  Pass along the return code from Monaco InitEngine()
//
// 3-23-00  Version 3.0.3  Store and retrieve Rendering Intents field ( InitStruct )
//                         Add Rendering Intents fields to Initialization, Copy, and
//                         Compare functions
//

#include "stdafx.h"
#include "zeus.h"
#include "profile.h"
#include "ColorTransform.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CColorTransform, CObject)
//IMPLEMENT_SERIAL(CColorTransform, CObject, VERSIONABLE_SCHEMA|1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColorTransform::CColorTransform()
{
	LoadDefaults();
}

CColorTransform::~CColorTransform()
{
}	 

void CColorTransform::Serialize(CArchive& ar, WORD verCheck)
{     
   CString TempName;

   WORD i;      

   BYTE* TempPtr;
   CPINITSTRUCT* pCPInit;

   // note: we don't archive the hDriver or the procs, these must be 
   // instantiated after we have gotten the object off the disk
   if (ar.IsStoring())
   {
	  //CopyInitStruct(&CPInitStruct,MonacoCMM.InStructOne);
      TempPtr = (BYTE*)&CPInitStruct;
      pCPInit = (CPINITSTRUCT*)&CPInitStruct;

	  if ( pCPInit->wInitStructVersion == (float)INIT_STRUCT_VERSION )
	  {
		  ar << pCPInit->wInitStructVersion;

		  ar << pCPInit->bColorTransform;
		  ar << pCPInit->bColorAdjustment;

		  ar << pCPInit->MaxBlack;
		  ar << pCPInit->MaxInk;
		  ar << pCPInit->GCR_UCR_Value;
		  ar << pCPInit->Brightness;
		  ar << pCPInit->Contrast;

		  for ( i = 0; i < 4; i++ )
			  ar << pCPInit->ColorLevels[i];

		  ar << pCPInit->bIgnoreAdjustments;
		  ar << pCPInit->bLockAdjustments;
		  ar << pCPInit->bLockTransformGraph;

		  // Pointer to data (size of array is (m_nLines * m_XMax)
		  // Values are Y values
	  	  for(i = 0; i < DEFAULT_LUT_SIZE * 4; i++)
			ar << pCPInit->ColorAdjTable[i];

  		  // Pointer to data (size of array is (m_nLines * m_XMax)
		  // Values are Y values
	  	  for(i = 0; i < DEFAULT_LUT_SIZE * 4; i++)
			ar << pCPInit->ColorTrnsfrmTable[i];

		  for ( i = 0; i < 7; i++ )
			  ar << pCPInit->gamma[i];

		  ar << pCPInit->CMM;
		  ar << pCPInit->DLG;

		  ar << pCPInit->TransType;
		  ar << pCPInit->Printer_CSpace;
		  ar << pCPInit->Input_CSpace;
		  ar << pCPInit->GlobalPrinterID;

		  ar << pCPInit->Preview;
		  ar << pCPInit->Prepress;
		  ar << pCPInit->WhiteIsWhite;
		  ar << pCPInit->BlackIsBlack;
		  ar << pCPInit->PreserveLuminosity;

		  ar << pCPInit->LineArt;
		  ar << pCPInit->GCR_UCR_Flag;

  		  TempName = pCPInit->RGB_In_Name;
		  ar << TempName;

  		  TempName = pCPInit->CMYK_In_Name;
		  ar << TempName;

  		  TempName = pCPInit->Monitor_Name;
		  ar << TempName;

  		  TempName = pCPInit->Press_Name;
		  ar << TempName;

  		  TempName = pCPInit->Printer_Name;
		  ar << TempName;

  		  TempName = pCPInit->Printer_Profile_Name;
		  ar << TempName;

		  ar << pCPInit->Image_Rendering_Intent;
		  ar << pCPInit->Text_Rendering_Intent;
	  }
	  else
	  {
		  for (i = 0; i < sizeof(CPINITSTRUCT); i++)
			 ar << *TempPtr++;  
	  }
   }
   else
   {
      pCPInit = (CPINITSTRUCT*)&CPInitStruct;

	  // Get the first "float" bytes to check for version
	  ar >> pCPInit->wInitStructVersion;

	  if ( pCPInit->wInitStructVersion == (float)INIT_STRUCT_VERSION )
	  {
		  ar >> pCPInit->bColorTransform;
		  ar >> pCPInit->bColorAdjustment;

		  ar >> pCPInit->MaxBlack;
		  ar >> pCPInit->MaxInk;
		  ar >> pCPInit->GCR_UCR_Value;
		  ar >> pCPInit->Brightness;
		  ar >> pCPInit->Contrast;

		  for ( i = 0; i < 4; i++ )
			  ar >> pCPInit->ColorLevels[i];

		  ar >> pCPInit->bIgnoreAdjustments;
		  ar >> pCPInit->bLockAdjustments;
		  ar >> pCPInit->bLockTransformGraph;

		  // Pointer to data (size of array is (m_nLines * m_XMax)
		  // Values are Y values
	  	  for(i = 0; i < DEFAULT_LUT_SIZE * 4; i++)
		  {
			ar >> pCPInit->ColorAdjTable[i];
		  }

  		  // Pointer to data (size of array is (m_nLines * m_XMax)
		  // Values are Y values
	  	  for(i = 0; i < DEFAULT_LUT_SIZE * 4; i++)
		  {
			ar >> pCPInit->ColorTrnsfrmTable[i];
			if ( pCPInit->ColorTrnsfrmTable[i] > 255 )
				pCPInit->ColorTrnsfrmTable[i] = pCPInit->ColorTrnsfrmTable[i] - 256;
		  }

		  for ( i = 0; i < 7; i++ )
			  ar >> pCPInit->gamma[i];

		  ar >> pCPInit->CMM;
		  ar >> pCPInit->DLG;

		  ar >> pCPInit->TransType;
		  ar >> pCPInit->Printer_CSpace;
		  ar >> pCPInit->Input_CSpace;
		  ar >> pCPInit->GlobalPrinterID;

		  ar >> pCPInit->Preview;
		  ar >> pCPInit->Prepress;
		  ar >> pCPInit->WhiteIsWhite;
		  ar >> pCPInit->BlackIsBlack;
		  ar >> pCPInit->PreserveLuminosity;

		  ar >> pCPInit->LineArt;
		  ar >> pCPInit->GCR_UCR_Flag;

		  ar >> TempName;
		  strcpy ( pCPInit->RGB_In_Name, TempName );

		  ar >> TempName;
		  strcpy ( pCPInit->CMYK_In_Name, TempName );

		  ar >> TempName;
		  strcpy ( pCPInit->Monitor_Name, TempName );

		  ar >> TempName;
		  strcpy ( pCPInit->Press_Name, TempName );

		  ar >> TempName;
		  strcpy ( pCPInit->Printer_Name, TempName );

		  ar >> TempName;
		  strcpy ( pCPInit->Printer_Profile_Name, TempName );

  		  ar >> pCPInit->Image_Rendering_Intent;
  		  ar >> pCPInit->Text_Rendering_Intent;
	  }

	  else		// Read a the Previous version of the CPINITSTRUCT
	  {			// and convert to the current version
		  if ( pCPInit->wInitStructVersion == (float)INIT_STRUCT_VERSION_OLD )
		  {
  			  // Change to the Version Number to the new structure
			  pCPInit->wInitStructVersion = (float)INIT_STRUCT_VERSION;

			  pCPInit->bColorTransform		= FALSE;
			  pCPInit->bColorAdjustment		= FALSE;

			  ar >> pCPInit->MaxBlack;
			  ar >> pCPInit->MaxInk;
			  ar >> pCPInit->GCR_UCR_Value;
			  ar >> pCPInit->Brightness;
			  ar >> pCPInit->Contrast;

			  for ( i = 0; i < 4; i++ )
				  ar >> pCPInit->ColorLevels[i];

			  ar >> pCPInit->bIgnoreAdjustments;
			  ar >> pCPInit->bLockAdjustments;
			  ar >> pCPInit->bLockTransformGraph;

			  // Pointer to data (size of array is (m_nLines * m_XMax)
			  // Values are Y values
			  for(i = 0; i < DEFAULT_LUT_SIZE * 4; i++)
			  {
				ar >> pCPInit->ColorAdjTable[i];
				if ( pCPInit->ColorAdjTable[i] > 255 )
					pCPInit->ColorAdjTable[i] = pCPInit->ColorAdjTable[i] - 256;
			  }

  			  // Pointer to data (size of array is (m_nLines * m_XMax)
			  // Values are Y values
			  for(i = 0; i < DEFAULT_LUT_SIZE * 4; i++)
			  {
				ar >> pCPInit->ColorTrnsfrmTable[i];
				if ( pCPInit->ColorTrnsfrmTable[i] > 255 )
					pCPInit->ColorTrnsfrmTable[i] = pCPInit->ColorTrnsfrmTable[i] - 256;
			  }

			  for ( i = 0; i < 7; i++ )
				  ar >> pCPInit->gamma[i];

			  ar >> pCPInit->CMM;
			  ar >> pCPInit->DLG;

			  ar >> pCPInit->TransType;
			  ar >> pCPInit->Printer_CSpace;
			  ar >> pCPInit->Input_CSpace;
			  ar >> pCPInit->GlobalPrinterID;

			  ar >> pCPInit->Preview;
			  ar >> pCPInit->Prepress;
			  ar >> pCPInit->WhiteIsWhite;
			  ar >> pCPInit->BlackIsBlack;
			  ar >> pCPInit->PreserveLuminosity;

			  ar >> pCPInit->LineArt;
			  ar >> pCPInit->GCR_UCR_Flag;

			  ar >> TempName;
			  strcpy ( pCPInit->RGB_In_Name, TempName );

			  ar >> TempName;
			  strcpy ( pCPInit->CMYK_In_Name, TempName );

			  ar >> TempName;
			  strcpy ( pCPInit->Monitor_Name, TempName );

			  ar >> TempName;
			  strcpy ( pCPInit->Press_Name, TempName );

			  ar >> TempName;
			  strcpy ( pCPInit->Printer_Name, TempName );

			  ar >> TempName;
			  strcpy ( pCPInit->Printer_Profile_Name, TempName );
		  }
		  else
		  {
			  TempPtr = (BYTE*)&Prev_CPInitStruct;

			  // Copy the first 'float' bytes to the Old Structure and move the
			  // DESTINATION pointer 'float' bytes before starting to retrieve
			  // the additional information
			  memcpy(TempPtr, pCPInit, sizeof(float) );
			  for ( i = 0; i < sizeof(float); i++ )
				*TempPtr++;

			  for (i = sizeof(float); i < sizeof(PREV_CPINITSTRUCT); i++)
				 ar >> *TempPtr++;
			
	/*************   Convert to the new version of the CPINITSTRUCT   *************/
			  // Construct an updated CPInitStruct from the retrieved
			  // information and initatilizing the NEW fields appropriately
			  //
	/*************   Convert to the new version of the CPINITSTRUCT   *************/

			  // Add the Version Number to the new structure
			  pCPInit->wInitStructVersion = (float)INIT_STRUCT_VERSION;

			  pCPInit->MaxBlack		 = Prev_CPInitStruct.MaxBlack;
			  pCPInit->MaxInk		 = Prev_CPInitStruct.MaxInk;
			  pCPInit->GCR_UCR_Value = Prev_CPInitStruct.GCR_UCR_Value;
			  pCPInit->Brightness	 = Prev_CPInitStruct.Brightness;
			  pCPInit->Contrast		 = Prev_CPInitStruct.Contrast;

			  for ( i = 0; i < 4; i++ )
				  pCPInit->ColorLevels[i] = Prev_CPInitStruct.ColorLevels[i];

			  pCPInit->bColorTransform		= FALSE;
			  pCPInit->bColorAdjustment		= FALSE;
			  pCPInit->bIgnoreAdjustments	= FALSE;
			  pCPInit->bLockAdjustments		= TRUE;
			  pCPInit->bLockTransformGraph	= FALSE;

	  		  for(i = 0; i < DEFAULT_LUT_SIZE * 4; i++)
			  {
				pCPInit->ColorAdjTable[i] = Prev_CPInitStruct.GraphLines[i];
		
				// Initialize the New Color Transform Table
				pCPInit->ColorTrnsfrmTable[i] = (WORD)(i % DEFAULT_LUT_MAX);
			  }

			  for ( i = 0; i < 7; i++ )
				  pCPInit->gamma[i] = Prev_CPInitStruct.gamma[i];

			  pCPInit->CMM = Prev_CPInitStruct.CMM;
			  pCPInit->DLG = Prev_CPInitStruct.DLG;

			  pCPInit->TransType	   = Prev_CPInitStruct.TransType;
			  pCPInit->Printer_CSpace  = Prev_CPInitStruct.Printer_CSpace;
			  pCPInit->Input_CSpace    = Prev_CPInitStruct.Input_CSpace;
			  pCPInit->GlobalPrinterID = Prev_CPInitStruct.GlobalPrinterID;

#ifdef USE_EMBEDDED_PROFILE
			  pCPInit->UseEmbedded		  = Prev_CPInitStruct.UseEmbedded;
#endif
			  pCPInit->Preview			  = Prev_CPInitStruct.Preview;
			  pCPInit->Prepress			  = Prev_CPInitStruct.Prepress;
			  pCPInit->WhiteIsWhite		  = Prev_CPInitStruct.WhiteIsWhite;
			  pCPInit->BlackIsBlack		  = Prev_CPInitStruct.BlackIsBlack;
			  pCPInit->PreserveLuminosity = Prev_CPInitStruct.PreserveLuminosity;

			  pCPInit->LineArt		= Prev_CPInitStruct.LineArt;
			  pCPInit->GCR_UCR_Flag	= Prev_CPInitStruct.GCR_UCR_Flag;

			  strcpy( pCPInit->RGB_In_Name,  Prev_CPInitStruct.RGB_In_Name );
			  strcpy( pCPInit->CMYK_In_Name, Prev_CPInitStruct.CMYK_In_Name );
			  strcpy( pCPInit->Monitor_Name, Prev_CPInitStruct.Monitor_Name );
			  strcpy( pCPInit->Press_Name,   Prev_CPInitStruct.Press_Name );
			  strcpy( pCPInit->Printer_Name, Prev_CPInitStruct.Printer_Name );
			  strcpy( pCPInit->Printer_Profile_Name, Prev_CPInitStruct.Printer_Profile_Name );
		  }
	  }
   } 
}


BOOL CColorTransform::operator!=(const CColorTransform& cc)
   {
   // is same thing?
   return (!IsSame(cc));
   }

BOOL CColorTransform::operator==(const CColorTransform& cc)
   {
   // is same thing?
   return (IsSame(cc));
   }

BOOL CColorTransform::IsSame(const CColorTransform& cc)
   {                                                           
   // make sure that the cpinitstruct are the same
//   if (memcmp(&CPInitStruct, &cc.CPInitStruct, sizeof(CPINITSTRUCT)) != 0) return FALSE;
   if ( !CompareInitStruct( &CPInitStruct, &cc.CPInitStruct ) )
	   return FALSE;

   // well, I guess they are the same 
   return TRUE;
   }
BOOL CColorTransform::IsSame(const CPINITSTRUCT& cc)
   {                                                           
   // make sure that the cpinitstruct are the same
//   if (memcmp(&CPInitStruct, &cc, sizeof(CPINITSTRUCT)) != 0) return FALSE;
   if ( !CompareInitStruct( &CPInitStruct, &cc ) )
	   return FALSE;
   
   // well, I guess they are the same 
   return TRUE;
   }

const CColorTransform& CColorTransform::operator=(const CColorTransform& ccSrc) // assignment	  
   {        
   if (this != &ccSrc)
      {        
   	  // copy over the cpinistruct
	  // Since there are Pointers within this Structure memcpy cannot be used
//	  memcpy(&CPInitStruct, &ccSrc.CPInitStruct, sizeof(CPINITSTRUCT));	
	   CopyInitStruct(&CPInitStruct, &ccSrc.CPInitStruct);
	  }        
   return *this;      
   }              

void CColorTransform::LoadDefaults()
{
	int i;

	CString csDefaultICC;

	// just to be sure
	memset(&CPInitStruct, 0, sizeof(CPINITSTRUCT));

	// Default to Current CPINITSTRUCT version
	CPInitStruct.wInitStructVersion = (float)INIT_STRUCT_VERSION;

	csDefaultICC.Empty();

	strncpy(CPInitStruct.Printer_Name, csDefaultICC.GetBuffer(MAX_PATH), MAX_PATH);
	strncpy(CPInitStruct.RGB_In_Name, csDefaultICC.GetBuffer(MAX_PATH), MAX_PATH);
	strncpy(CPInitStruct.CMYK_In_Name, csDefaultICC.GetBuffer(MAX_PATH), MAX_PATH);
	strncpy(CPInitStruct.Press_Name, csDefaultICC.GetBuffer(MAX_PATH), MAX_PATH);
	strncpy(CPInitStruct.Printer_Profile_Name, csDefaultICC.GetBuffer(MAX_PATH), MAX_PATH);
	strncpy(CPInitStruct.Monitor_Name, csDefaultICC.GetBuffer(MAX_PATH), MAX_PATH);

	CPInitStruct.Image_Rendering_Intent	= ICC_PERCEPTUAL;
	CPInitStruct.Text_Rendering_Intent	= ICC_RELATIVE_COLORIMETRIC;

	CPInitStruct.MaxBlack		= DEFAULT_MAX_BLACK;
	CPInitStruct.MaxInk			= DEFAULT_MAX_INK;
	CPInitStruct.GCR_UCR_Value	= DEFAULT_GCR_UCR;

	for(i = 0; i < 4; i++)
	{
		CPInitStruct.ColorLevels[i] = DEFAULT_COLOR_LEVEL;
	}

	for (i = 0; i < DEFAULT_LUT_SIZE * 4; i++ )
	{
		CPInitStruct.ColorAdjTable[i]		= (WORD)(i % (DEFAULT_LUT_MAX + 1));
		CPInitStruct.ColorTrnsfrmTable[i]	= (WORD)(i % (DEFAULT_LUT_MAX + 1));
	}

	for (i = 0; i < 7; i++)
	{
		CPInitStruct.gamma[i] = (float)DEFAULT_GAMMA; //? 
	}

	CPInitStruct.Brightness			= DEFAULT_BRIGHTNESS;
	CPInitStruct.Contrast			= DEFAULT_CONTRAST;
#ifdef MONACO_CNMM_DLL_H
	CPInitStruct.CMM				= MONACO_CMM;
#else
	CPInitStruct.CMM				= GRAPHX_CMM;
#endif
	CPInitStruct.DLG				= CMM_DLG;
	CPInitStruct.TransType			= ICC_UNKNOWN;			
	CPInitStruct.Input_CSpace		= RGB_CSPACE;
	CPInitStruct.Printer_CSpace		= CMYK_CSPACE;
#ifdef USE_EMBEDDED_PROFILE
	CPInitStruct.UseEmbedded		= FALSE;
#endif
	CPInitStruct.Preview			= FALSE;
	CPInitStruct.Prepress			= FALSE;
	CPInitStruct.WhiteIsWhite		= FALSE;
	CPInitStruct.BlackIsBlack		= FALSE;
	CPInitStruct.PreserveLuminosity	= TRUE;
	CPInitStruct.LineArt			= FALSE;
	CPInitStruct.GCR_UCR_Flag		= FALSE;
	CPInitStruct.bIgnoreAdjustments	= FALSE;
	CPInitStruct.bLockAdjustments	= TRUE;

	// added by pjt
	//CopyInitStruct(MonacoCMM.InStructOne,&CPInitStruct);	  
   }

int CColorTransform::InitEngine(CPINITSTRUCT* new_init)
{
	CopyInitStruct(&CPInitStruct,new_init);
	return InitEngine();

}
int CColorTransform::InitEngine()
{
#ifdef MONACO_CNMM_DLL_H
    CopyInitStruct(MonacoCMM.InStructOne, &CPInitStruct);
	return ( MonacoCMM.InitEngine(MonacoCMM.InStructOne) );
#else
    CopyInitStruct(WindowsCMM.InStructOne, &CPInitStruct);
	return ( WindowsCMM.InitEngine(WindowsCMM.InStructOne) );
#endif
}

void CColorTransform::DeInit()
{
#ifdef MONACO_CNMM_DLL_H
	MonacoCMM.DeInit();	
#else
	WindowsCMM.DeInit();	
#endif
}

BOOL CColorTransform::CompareInitStruct(CPINITSTRUCT* Dest_init, const CPINITSTRUCT* Source_init)
{
	if ( Dest_init->bLockTransformGraph	!= Source_init->bLockTransformGraph )
		return FALSE;

	if ( Dest_init->bColorTransform	!= Source_init->bColorTransform )
		return FALSE;

	if ( Dest_init->bLockAdjustments != Source_init->bLockAdjustments )
		return FALSE;

	if ( Dest_init->bIgnoreAdjustments	!= Source_init->bIgnoreAdjustments )
		return FALSE;

	if ( Dest_init->bColorAdjustment != Source_init->bColorAdjustment )
		return FALSE;

	if ( Dest_init->MaxBlack != Source_init->MaxBlack )
		return FALSE;

	if ( Dest_init->MaxInk != Source_init->MaxInk )
		return FALSE;

	if ( Dest_init->GCR_UCR_Value != Source_init->GCR_UCR_Value )
		return FALSE;

	if ( Dest_init->Brightness != Source_init->Brightness )
		return FALSE;

	if ( Dest_init->Contrast != Source_init->Contrast )
		return FALSE;
 
	if ( Dest_init->TransType != Source_init->TransType )
		return FALSE;

	if ( Dest_init->Printer_CSpace != Source_init->Printer_CSpace )
		return FALSE;

	if ( Dest_init->GlobalPrinterID != Source_init->GlobalPrinterID )
		return FALSE;

	int i = 0;
	BOOL bTablesEqual = TRUE;

	while ( (i < DEFAULT_LUT_SIZE * 4 ) && bTablesEqual )
	{
		// Tables from Color Adjustment Line graph
		if ( Dest_init->ColorAdjTable[i] != Source_init->ColorAdjTable[i] )
			bTablesEqual = FALSE;

		// Tables from Color Transform Line graph
		if ( Dest_init->ColorTrnsfrmTable[i] != Source_init->ColorTrnsfrmTable[i] )
			bTablesEqual = FALSE;

		i++;
	}

	if ( !bTablesEqual )
		return FALSE;

	while ( (i < 7 ) && bTablesEqual )
	{
		if ( Dest_init->gamma[i] != Source_init->gamma[i] )
			bTablesEqual = FALSE;
		i++;
	}

	if ( !bTablesEqual )
		return FALSE;

	while ( (i < 4 ) && bTablesEqual )
	for(i = 0; i < 4; i++)
	{
		if ( Dest_init->ColorLevels[i] != Source_init->ColorLevels[i] )
			bTablesEqual = FALSE;
		i++;
	}

	if ( !bTablesEqual )
		return FALSE;

#ifdef USE_EMBEDDED_PROFILE
	if ( Dest_init->UseEmbedded != Source_init->UseEmbedded )
		return FALSE;
#endif

	if ( Dest_init->Preview != Source_init->Preview )
		return FALSE;

	if ( Dest_init->Prepress != Source_init->Prepress ) 
		return FALSE;

	if ( Dest_init->WhiteIsWhite != Source_init->WhiteIsWhite )
		return FALSE;

	if ( Dest_init->BlackIsBlack != Source_init->BlackIsBlack ) 
		return FALSE;

	if ( Dest_init->GCR_UCR_Flag != Source_init->GCR_UCR_Flag )
		return FALSE;

	if ( Dest_init->TransType != Source_init->TransType )
		return FALSE;

	if ( strncmp(Dest_init->RGB_In_Name, Source_init->RGB_In_Name, MAX_PATH) != 0 )
		return FALSE;

	if ( strncmp(Dest_init->CMYK_In_Name, Source_init->CMYK_In_Name, MAX_PATH) != 0 )
		return FALSE;

	if ( strncmp(Dest_init->Monitor_Name, Source_init->Monitor_Name, MAX_PATH) != 0 )
		return FALSE;

	if ( strncmp(Dest_init->Press_Name, Source_init->Press_Name, MAX_PATH) != 0 )
		return FALSE;

	if ( strncmp(Dest_init->Printer_Name, Source_init->Printer_Name, MAX_PATH) != 0 )
		return FALSE;

	if ( strncmp(Dest_init->Printer_Profile_Name, Source_init->Printer_Profile_Name, MAX_PATH) != 0 )
		return FALSE;

	if ( Dest_init->Image_Rendering_Intent != Source_init->Image_Rendering_Intent )
		return FALSE;

	if ( Dest_init->Text_Rendering_Intent != Source_init->Text_Rendering_Intent )
		return FALSE;

	return TRUE;
}




BOOL CColorTransform::CopyInitStruct(CPINITSTRUCT* Dest_init, const CPINITSTRUCT* Source_init)
{
	Dest_init->wInitStructVersion = Source_init->wInitStructVersion;

	Dest_init->bLockTransformGraph	= Source_init->bLockTransformGraph;
	Dest_init->bColorTransform		= Source_init->bColorTransform;
	Dest_init->bLockAdjustments		= Source_init->bLockAdjustments;
	Dest_init->bIgnoreAdjustments	= Source_init->bIgnoreAdjustments;
	Dest_init->bColorAdjustment		= Source_init->bColorAdjustment;

	Dest_init->MaxBlack			= Source_init->MaxBlack;		// maximum black level, 0-100
	Dest_init->MaxInk			= Source_init->MaxInk;			// maximum ink level, 0-400
	Dest_init->GCR_UCR_Value	= Source_init->GCR_UCR_Value;	// 0-100
	Dest_init->Brightness		= Source_init->Brightness;
	Dest_init->Contrast			= Source_init->Contrast;

	Dest_init->CMM = Source_init->CMM;	// colormanagement using
	Dest_init->DLG = Source_init->DLG;	// dialog to pop up

	Dest_init->TransType		= Source_init->TransType;		// Transform Type to use
	Dest_init->Printer_CSpace	= Source_init->Printer_CSpace;	// Transform Type to use
	Dest_init->Input_CSpace		= Source_init->Input_CSpace;	// Transform Type to use
	Dest_init->GlobalPrinterID	= Source_init->GlobalPrinterID;	// Transform Type to use

	int i = 0;

	for (i = 0; i < DEFAULT_LUT_SIZE * 4; i++)
	{
		// Tables from Color Adjustment Line graph
		Dest_init->ColorAdjTable[i]		= Source_init->ColorAdjTable[i];

		// Tables from Color Transform Line graph
		Dest_init->ColorTrnsfrmTable[i]	= Source_init->ColorTrnsfrmTable[i];
	}

	for(i = 0; i < 7; i++)
	{
		Dest_init->gamma[i] = Source_init->gamma[i];
	}

	for(i = 0; i < 4; i++)
	{
		Dest_init->ColorLevels[i] = Source_init->ColorLevels[i];
	}

#ifdef USE_EMBEDDED_PROFILE
	Dest_init->UseEmbedded			= Source_init->UseEmbedded;
#endif
	Dest_init->Preview				= Source_init->Preview;
	Dest_init->Prepress				= Source_init->Prepress;
	Dest_init->WhiteIsWhite			= Source_init->WhiteIsWhite;
	Dest_init->BlackIsBlack			= Source_init->BlackIsBlack;
	Dest_init->PreserveLuminosity	= Source_init->PreserveLuminosity;

	Dest_init->GCR_UCR_Flag	= Source_init->GCR_UCR_Flag;
	Dest_init->LineArt		= Source_init->LineArt;
	Dest_init->TransType	= Source_init->TransType;

	strncpy(Dest_init->RGB_In_Name, Source_init->RGB_In_Name, MAX_PATH);
	strncpy(Dest_init->CMYK_In_Name, Source_init->CMYK_In_Name, MAX_PATH);
	strncpy(Dest_init->Monitor_Name, Source_init->Monitor_Name, MAX_PATH);
	strncpy(Dest_init->Press_Name, Source_init->Press_Name, MAX_PATH);
	strncpy(Dest_init->Printer_Name, Source_init->Printer_Name, MAX_PATH);
	strncpy(Dest_init->Printer_Profile_Name, Source_init->Printer_Profile_Name, MAX_PATH);

	Dest_init->Image_Rendering_Intent = Source_init->Image_Rendering_Intent;
	Dest_init->Text_Rendering_Intent  = Source_init->Text_Rendering_Intent;

	return TRUE;
}
BOOL CColorTransform::CopyCMMDlg(CPINITSTRUCT* Dest_init, CPINITSTRUCT* Source_init)
{
	int i = 0;

	Dest_init->wInitStructVersion = Source_init->wInitStructVersion;

	Dest_init->MaxBlack			= Source_init->MaxBlack;		// maximum black level, 0-100
	Dest_init->MaxInk			= Source_init->MaxInk;			// maximum ink level, 0-400
	Dest_init->GCR_UCR_Value	= Source_init->GCR_UCR_Value;	// 0-100

	// Tables from Color Transform Line graph

	for (i = 0; i < DEFAULT_LUT_SIZE * 4; i++)
	{
		Dest_init->ColorAdjTable[i] = Source_init->ColorAdjTable[i];
		Dest_init->ColorTrnsfrmTable[i] = Source_init->ColorTrnsfrmTable[i];
	}

	for(i = 0; i < 7; i++)
	{
		Dest_init->gamma[i] = Source_init->gamma[i];
	}

#ifdef USE_EMBEDDED_PROFILE
	Dest_init->UseEmbedded	= Source_init->UseEmbedded;
#endif
	Dest_init->Preview		= Source_init->Preview;
	Dest_init->Prepress		= Source_init->Prepress;
	Dest_init->GCR_UCR_Flag	= Source_init->GCR_UCR_Flag;
	Dest_init->TransType	= Source_init->TransType;

	Dest_init->bLockTransformGraph	= Source_init->bLockTransformGraph;
	Dest_init->bColorTransform		= Source_init->bColorTransform;

	strncpy(Dest_init->RGB_In_Name,  Source_init->RGB_In_Name,  MAX_PATH);
	strncpy(Dest_init->CMYK_In_Name, Source_init->CMYK_In_Name, MAX_PATH);
	strncpy(Dest_init->Monitor_Name, Source_init->Monitor_Name, MAX_PATH);
	strncpy(Dest_init->Press_Name,   Source_init->Press_Name,   MAX_PATH);
	strncpy(Dest_init->Printer_Name, Source_init->Printer_Name, MAX_PATH);
	strncpy(Dest_init->Printer_Profile_Name, Source_init->Printer_Profile_Name, MAX_PATH);

	Dest_init->Image_Rendering_Intent = Source_init->Image_Rendering_Intent;
	Dest_init->Text_Rendering_Intent  = Source_init->Text_Rendering_Intent;

	return TRUE;
}

BOOL CColorTransform::CopyClrAdjDlg(CPINITSTRUCT* Dest_init, CPINITSTRUCT* Source_init)
{
	Dest_init->wInitStructVersion = Source_init->wInitStructVersion;

	Dest_init->Brightness		= Source_init->Brightness;
	Dest_init->Contrast			= Source_init->Contrast;
	Dest_init->Printer_CSpace	= Source_init->Printer_CSpace;

	// Tables from Color Adjustment Line graph
	int i = 0;
 	for (i = 0; i < DEFAULT_LUT_SIZE * 4; i++)
	{
		Dest_init->ColorAdjTable[i] = Source_init->ColorAdjTable[i];
	}

	for(i = 0; i < 4; i++)
	{
		Dest_init->ColorLevels[i] = Source_init->ColorLevels[i];
	}

	Dest_init->WhiteIsWhite = Source_init->WhiteIsWhite;
	Dest_init->BlackIsBlack = Source_init->BlackIsBlack;

	Dest_init->bLockAdjustments		= Source_init->bLockAdjustments;
	Dest_init->bIgnoreAdjustments	= Source_init->bIgnoreAdjustments;

	Dest_init->bColorAdjustment		= Source_init->bColorAdjustment;

	strncpy(Dest_init->Printer_Name, Source_init->Printer_Name, MAX_PATH);

	return TRUE;
}

void CColorTransform::LoadDefaultProfiles()
   {
   CProfile Profile;
   CString cs;
   CString csFolder;
   CString csFileName;
   CString csDefault;

   cs = Profile.GetStr("TrueGraphx", "DefaultRGBInICC");
   strcpy(CPInitStruct.RGB_In_Name, cs);

   cs = Profile.GetStr("TrueGraphx", "DefaultCMYKInICC");
   strcpy(CPInitStruct.CMYK_In_Name, cs);

   cs = Profile.GetStr("TrueGraphx", "DefaultPressICC");
   strcpy(CPInitStruct.Press_Name, cs);

   cs = Profile.GetStr("TrueGraphx", "DefaultMonitorICC");
   strcpy(CPInitStruct.Monitor_Name, cs);   

   // defaults for all devices
   if (CPInitStruct.Printer_CSpace == CMYK_CSPACE)
      csDefault = Profile.GetStr("TrueGraphx", "DefaultCMYKDevICC");
   else
	  csDefault = Profile.GetStr("TrueGraphx", "DefaultRGBDevICC");   
   // now try for real one
   csFolder = Profile.GetStr(CPInitStruct.Printer_Name, 
	  IDS_PROFILEDIR);      
   csFileName = Profile.GetStr(CPInitStruct.Printer_Name, 
	  IDS_DEFAULT_PROFILE);
   if ((csFolder.GetLength() > 0) && (csFileName.GetLength() > 0))
	  {
	  cs = csFolder;
	  VERIFYPATH(cs);
	  cs += csFileName;
	  strcpy(CPInitStruct.Printer_Profile_Name, cs);   	
	  }
   else
	  strcpy(CPInitStruct.Printer_Profile_Name, csDefault);   	

   // and get trans type
   CPInitStruct.TransType = Profile.GetInt(CPInitStruct.Printer_Name, 
	  IDS_CORRECTIONTYPE, 0);
   }

void CColorTransform::SetOutputType( int iOutputType )
{
#ifdef MONACO_CNMM_DLL_H
	MonacoCMM.SetOutputType( iOutputType );
#else
	WindowsCMM.SetOutputType( iOutputType );
#endif
}