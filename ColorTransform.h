// ColorTransform.h: interface for the CColorTransform class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORTRANSFORM_H__FA22373F_0932_11D3_8E3A_00C04F7975DC__INCLUDED_)
#define AFX_COLORTRANSFORM_H__FA22373F_0932_11D3_8E3A_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #define MONACO_CNMM_DLL_H 1

#ifdef DEFAULT_GAMMA
#undef DEFAULT_GAMMA
#endif
#include "..\icc\InitStruct.h"
#ifdef MONACO_CNMM_DLL_H
#include "..\icc\Monaco\MonacoAPI.h"
//#include "..\icc\MonacoMPL\MonacoMPL.h"
#else
#include "..\icc\Windows\WindowsCMM.h"
#endif
#include "..\icc\ICCProfile.h"
#include "..\icc\ColorProfile.h"

class CColorTransform : public CObject
{
DECLARE_DYNAMIC(CColorTransform)
public:
	CColorTransform();
	virtual ~CColorTransform();

	CPINITSTRUCT CPInitStruct;

	PREV_CPINITSTRUCT Prev_CPInitStruct;

   virtual void Serialize(CArchive& ar, WORD verCheck = 0);	
   const CColorTransform& operator=(const CColorTransform& ccmSrc); // assignment
   BOOL operator==(const CColorTransform& cc);              // compare      
   BOOL operator!=(const CColorTransform& cc);              // compare      
   CColorTransform(const CColorTransform &ccSrc);                   // Copy constructor
   BOOL IsSame(const CColorTransform& cc);                  // used fo != and ==
	BOOL IsSame(const CPINITSTRUCT& cc);
	int InitEngine(CPINITSTRUCT* new_init);
	int InitEngine();
	void DeInit();
	void LoadDefaultProfiles();
	
	BOOL CompareInitStruct(CPINITSTRUCT* Dest_init, const CPINITSTRUCT* Source_init);
	BOOL CopyInitStruct(CPINITSTRUCT* Dest_init, const CPINITSTRUCT* Source_init);
	
	void LoadDefaults();
	void SetOutputType( int iOutputType );

	BOOL CopyCMMDlg(CPINITSTRUCT* Dest_init, CPINITSTRUCT* Source_init);
	BOOL CopyClrAdjDlg(CPINITSTRUCT* Dest_init, CPINITSTRUCT* Source_init);

#ifdef MONACO_CNMM_DLL_H
	CMonacoAPI MonacoCMM;
//	CMonacoMPL MonacoCMM;
#else
	CWindowsCMM WindowsCMM;
#endif

	// Generated message map functions
protected:
	

};

#endif // !defined(AFX_COLORTRANSFORM_H__FA22373F_0932_11D3_8E3A_00C04F7975DC__INCLUDED_)
