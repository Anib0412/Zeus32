#if !defined(AFX_STDAFX_H__097182A7_A716_11D2_8DD2_00C04F7975DC__INCLUDED_)
#define AFX_STDAFX_H__097182A7_A716_11D2_8DD2_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxsock.h>		// MFC socket extensions

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

/*
#include <gdiplus.h>				// Base include
using namespace Gdiplus;			// The "umbella"
#pragma comment(lib, "gdiplus.lib")	// The GDI+ binary
*/

// Stingray Objective Toolkit
#ifndef RIPPROC_BUILD
#include "secall.h"
#endif
#endif // !defined(AFX_STDAFX_H__097182A7_A716_11D2_8DD2_00C04F7975DC__INCLUDED_)
