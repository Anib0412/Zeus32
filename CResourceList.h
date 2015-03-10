/////////////////////////////////////////////////////////////////////////////
// CResourceList.H
//                                                                        
// base class for CResourceList
// Copyright (C) 1999 Graphx, Inc.
//
// CResourceList is a linked list of Resources

#ifndef __CRESOURCELIST_H__
#define __CRESOURCELIST_H__

#include <afx.h>			//For CObject
#include <afxcoll.h>		//For CObArray
#include "profile.h"

enum { EDIR_NOT_READ = 0, FONT_FOUND, FONT_NOT_FOUND }; //Enumerated return values for IsFontListed()

class CResource : public CObject{
public:
	CResource(){};
	~CResource(){};

	CString ResourceName;	//Name of the resource
	CString ResourceFile;	//File which decribes the resource
	BOOL IsDefaultFont;		//Identifies whether the font was installed by default or added by the user.
};

class CResourceList : public CObArray{
public:
	CResourceList();
	~CResourceList();

	//Fill in list of all available resources
	CResource* GetResource(CString fontname);
	BOOL InitFontResourceList();
	BOOL InitFontListUsingPFB(CString pfbdir);
	BOOL AddNewFont(CString fontname, CString fontfile);
	BOOL AddFontToEdir(CString fontname, CString fontfile);
	BOOL AddResource(CResource* resource);
	BOOL RemoveFontResource(CString fontnameBOOL);
	int	GetResIdx(CString resname);
	int IsFontListed(CString fontname);
	CString GetATMDir();
	
protected:
	BOOL HandleFontSubstitutions();
	BOOL GetAvailFonts(CString& resname, CString& filename, CString& isdefault);
	CString GetFontLocation();
	BOOL GetResourceLocation(CString resfilename, CString& namebuf, BOOL& isdefaultfont);
	CString GetFontNameFromPFB(CString pfbfile);
	
};

#endif