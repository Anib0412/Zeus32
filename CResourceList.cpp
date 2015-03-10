/////////////////////////////////////////////////////////////////////////////
// CResourceList.CPP
//                                                                        
// The device module for CResourceList
// Copyright (C) 1999 Graphx, Inc.
//

#include "CResourceList.h"

CResourceList::CResourceList(){

}

CResourceList::~CResourceList(){
	int i;
	//We are initing the list so remove anything that might already exist
	for(i=0; i<GetSize(); i++){
		delete GetAt(i);
	}
	
	RemoveAll();
}

/*
*	InitFontResourceList() will indentify all installed fonts found listed 
*	in the _EDIR_ file of the RasterPlus\jrip\font folder.
*
*	Return Value:	TRUE if able to complete list FALSE otherwise.
*/
BOOL CResourceList::InitFontResourceList(){
	CString csFontNameList, csFontFileList, IsDefaultFontList, csIsDefaultFont;
	CResource* pResEntry;
	int i;

	//We are initing the list so remove anything that might already exist
	for(i=0; i<GetSize(); i++){
		delete GetAt(i);
	}
	//Now Destroy the list
	RemoveAll();
	//Get the ';' delimited list of font names and there complimentary filename/location
	if(GetAvailFonts(csFontNameList, csFontFileList, IsDefaultFontList)){
		do{
			//Parse through lists and add each resource to the array of font resource
			pResEntry = new CResource;
			//Grab the resource name
			pResEntry->ResourceName = csFontNameList.SpanExcluding(";");
			//Now remove it from the list
			csFontNameList.Delete(0, pResEntry->ResourceName.GetLength()+1);
			//Grab the resource decription location
			pResEntry->ResourceFile = csFontFileList.SpanExcluding(";");
			//Now remove it from the list
			csFontFileList.Delete(0, pResEntry->ResourceFile.GetLength()+1);
			//Grab the string which identifies whether or not this is a default font or not
			csIsDefaultFont = IsDefaultFontList.SpanExcluding(";");
			IsDefaultFontList.Delete(0, csIsDefaultFont.GetLength()+1);
			//Now decide whether it's a default font or not
			if(csIsDefaultFont == "Default Font")
				pResEntry->IsDefaultFont = TRUE;
			else
				pResEntry->IsDefaultFont = FALSE;
			//Now add the object to the array
			Add((CObject*)pResEntry);
		}while(csFontNameList.GetLength() != 0);
		//No go through the list and replace all of the font description names which require substitutions.
		HandleFontSubstitutions();
	}
	else
		return FALSE;

	return TRUE;
}

/*
*	InitFontListUsingPFB() open all pfb files in the folder specified
*	by the pfbdir parameter and add each font specified in them to the
*	CFontResource array. This method is for identifying and listing all
*	acceptable font resources available for the user to add to their list
*	of default font resources.
*
*	Return Value:	TRUE if able to complete list FALSE otherwise.
*/
BOOL CResourceList::InitFontListUsingPFB(CString pfbdir){
	HANDLE hFile;
	WIN32_FIND_DATA FileInfo;
	CString csFileName, csPFBName, csFontName;	
	CResource* pResEntry;
	BOOL rval = FALSE; //Let's assume the worst at first
	int i;

	//We are initing the list so remove anything that might already exist
	for(i=0; i<GetSize(); i++){
		delete GetAt(i);
	}
	//Now remove them from the list
	RemoveAll();
	//Prepare the search string
	if(pfbdir.Right(1) != "\\")
		pfbdir += "\\";

	csFileName = pfbdir + "*.pfb";
	
	//Open each pfb file and extract the font name
	memset(&FileInfo, 0, sizeof WIN32_FIND_DATA);
	if((hFile = FindFirstFile(csFileName, &FileInfo)) != INVALID_HANDLE_VALUE){
		do{
			//Get the name of the font using the font handler
			csPFBName = FileInfo.cFileName;
			csFileName = pfbdir + csPFBName;
			csFontName = GetFontNameFromPFB(csFileName);
			//Make sure the font doesn't already exist.
			if(IsFontListed(csFontName) == FONT_NOT_FOUND){
				//Add resource to array
				if(csFontName.GetLength() > 0){
					//We're adding a resource so let's return with a TRUE
					if(!rval)
						rval = TRUE;

					pResEntry = new CResource;
					pResEntry->ResourceName = csFontName;
					pResEntry->ResourceFile = csFileName;
					pResEntry->IsDefaultFont = FALSE;
					Add((CObject*)pResEntry);
				}
			}
			//Keep going until we've checked all of the pfb files in the folder.
			memset(&FileInfo, 0, sizeof WIN32_FIND_DATA);
		}while(FindNextFile(hFile, &FileInfo));
	}

	return rval;
}

/*
*	HandleFontSubstitutions() will look for fonts which need substitutions
*	and replace their bogus file decription name with the file description
*	of the substitute font.
*
*	Return Value:	TRUE if able to find a substitute resource description
*					file otherwise FALSE.
*/
BOOL CResourceList::HandleFontSubstitutions(){
	int i, j;
	CResource* pWorkFont;
	CResource* pSubFont;
	CString csSubFileName;

	//Search through font array for ResourceFile containing text "Substitute with:"
	//immediately following text will be the name of the font resource with which
	//to substitute.

	for(i=0; i<GetSize(); i++){
		pWorkFont = (CResource*)GetAt(i);
		//Does ResourceFile tell me to find a substitution?
		if(pWorkFont->ResourceFile.Find("Substitute with:") != -1){
			csSubFileName = pWorkFont->ResourceFile.Right(pWorkFont->ResourceFile.GetLength() - (pWorkFont->ResourceFile.ReverseFind(':')+1));
			//Find Resource with ResourceName mathcing the name of the substitute font
			for(j=0; j<GetSize(); j++){
				pSubFont = (CResource*)GetAt(j);
 				if(pSubFont->ResourceName == csSubFileName){
					pWorkFont->ResourceFile = pSubFont->ResourceFile;
					break;
				}
			}
		}
	}

	return FALSE;
}

/*
*	GetResource() will Locate the CResource with the associated ResourceName
*	specified in it's parameter.
*
*	Return Value:	A pointer to the matching resource otherwise NULL
*/
CResource* CResourceList::GetResource(CString fontname){
	int i;
	CResource* pCurResource;

	for(i=0; i<GetSize(); i++){
		pCurResource = (CResource*)GetAt(i);
		if(pCurResource->ResourceName == fontname)
			return pCurResource;
	}

	//Unable to find a match
	return NULL;

}

/*
*	AddNewFont() will check to see whether a resource already exists that already 
*	matches the one that the user is trying to add. If it does not exist then the
*	method will create a new resource and add it to the CResource array. That 
*	shouldcopy parameter will tell the method whether it wants to permanently copy
*	the resource to the font folder.
*
*	Return Value:	TRUE if the new font is succesfully added FALSE otherwise
*/
BOOL CResourceList::AddNewFont(CString fontname, CString fontfile){
	CResource*	pResource;
	CStdioFile	NewPSFile, EdirFile;
	CString		csFontFolder, csNewFileName, csCurLine, csNewPFBFile;
	int beg;
	BOOL bStillReading;
	
	//Check to see whether the font name already exists.
	if(GetResource(fontname) != NULL){
		AfxMessageBox("A font with that name already exists!", MB_OK);
		return FALSE;
	}
	//Get the font folder location
	csFontFolder  = GetFontLocation();
	//Start by opening the _EDIR_ file. If we can't open that then there's no use continuing
	if(!EdirFile.Open(csFontFolder + "\\_edir_", CFile::modeReadWrite)){
		AfxMessageBox("Unable to edit index file!", MB_OK);
		return FALSE;
	}
	//Locate the position the pfb file name begins at
	if((beg = fontfile.ReverseFind('\\')) == -1)
		return FALSE;
	//Extract the name of the PFB file to copy
	csNewPFBFile = fontfile.Mid(beg+1, fontfile.GetLength() - beg);
	//Now copy the file
	if(!::CopyFile(fontfile, (csFontFolder + "\\" + csNewPFBFile), TRUE)){
		CString csError;

		if(GetLastError() == ERROR_FILE_EXISTS)
			csError.Format("Unable to copy resource file. The file: %s already exists.", csNewPFBFile);
		else
			csError = "Unable to copy resource file";
		
		AfxMessageBox(csError, MB_OK);

		return FALSE;
	}
	//Specify the new file name
	csNewFileName = "RP" + fontname + ".ps";
	//Create a new .ps file
	if(!NewPSFile.Open(csFontFolder + "\\" + csNewFileName, CFile::modeCreate  | CFile::modeReadWrite)){
		AfxMessageBox("Unable to create font!", MB_OK);
		return FALSE;
	}
	//Write information to the .ps file.
	//The file is getting placed into the default folder so I can just use the %font% variable
	csCurLine = "%!\n%Created by RasterPlus\n(%font%" + csNewPFBFile + ") (r) file /PFBDecode filter cvx exec\n";
	NewPSFile.WriteString(csCurLine);
	//I've made it this far now let's edit the _EDIR_ file
	CString csEdirText;
	BOOL bIsPSListed = FALSE;
	BOOL bIsPFBListed = FALSE;
	BOOL bAddNewLine = FALSE;	
	do{
		//Grab next line in file
		bStillReading = EdirFile.ReadString(csCurLine);
		//Did I need to add a new line character?
		if(bAddNewLine){
			csEdirText += "\n";
			bAddNewLine = FALSE;
		}
		//Trim up the currentline
		csCurLine.TrimLeft();
		//Is this where we want to add our listing?
		if(csCurLine.GetLength() > 0){
			//Add the .ps file reference?
			if((csCurLine.GetAt(0) == '/' && csCurLine.Find(".ps)") != -1) && !bIsPSListed){
				csEdirText += "/" + fontname + "\t\t(" + csNewFileName + ")\n";
				bIsPSListed = TRUE;
			}
			else if((csCurLine.GetAt(0) == '/' && csCurLine.Find(".pfb)") != -1) && !bIsPFBListed){
				csEdirText += "/" + csNewPFBFile + "\t(" + csNewPFBFile + ")\n";
				bIsPFBListed = TRUE;
			}

			csEdirText += csCurLine + "\n";
		}
		else
			bAddNewLine = TRUE;		

	}while(bStillReading);
	//Remove everything from the file
	EdirFile.SetLength(0);
	//Move to the beginning
	EdirFile.SeekToBegin();
	//Write our text
	EdirFile.WriteString(csEdirText);
	//All done now close the file
	EdirFile.Close();
	//We've made it this far now let's add the font to the font array
	pResource = new CResource;
	pResource->ResourceFile  = csNewPFBFile;
	pResource->ResourceName	 = fontname;
	pResource->IsDefaultFont = FALSE;
	Add((CObject*)pResource);

	return TRUE;
}

/*
*	AddFontToEdir() will check to see whether a resource already exists that already 
*	matches the one that the user is trying to add. If it does not exist then the
*	method will add it to the list of fonts in the _EDIR_ file. The shouldcopy parameter
*	will tell the method whether it wants to permanently copy the resource to the font folder.
*
*	Return Value:	TRUE if the new font is succesfully added FALSE otherwise
*/
BOOL CResourceList::AddFontToEdir(CString fontname, CString fontfile){
	CStdioFile	NewPSFile, EdirFile;
	CString		csFontFolder, csNewFileName, csCurLine, csNewPFBFile;
	int beg;
	BOOL bStillReading;
	CFileException e;
	
	//Get the font folder location
	csFontFolder  = GetFontLocation();
	//Start by opening the _EDIR_ file. If we can't open that then there's no use continuing
	if(!EdirFile.Open(csFontFolder + "\\_edir_", CFile::modeReadWrite, &e)){
		AfxMessageBox("Unable to edit index file!", MB_OK);
		return FALSE;
	}
	//Locate the position the pfb file name begins at
	if((beg = fontfile.ReverseFind('\\')) == -1)
		return FALSE;
	//Extract the name of the PFB file to copy
	csNewPFBFile = fontfile.Mid(beg+1, fontfile.GetLength() - beg);
	//Let's edit the _EDIR_ file
	CString csEdirText;
	BOOL bIsPSListed = FALSE;
	BOOL bIsPFBListed = FALSE;
	BOOL bAddNewLine = FALSE;	
	//Specify the new file name
	csNewFileName = "RP" + fontname + ".ps";
	do{
		//Grab next line in file
		bStillReading = EdirFile.ReadString(csCurLine);
		//Did I need to add a new line character?
		if(bAddNewLine){
			csEdirText += "\n";
			bAddNewLine = FALSE;
		}
		//Trim up the currentline
		csCurLine.TrimLeft();
		//Is this where we want to add our listing?
		if(csCurLine.GetLength() > 0){
			//Add the .ps file reference?
			if((csCurLine.GetAt(0) == '/' && csCurLine.Find(".ps)") != -1) && !bIsPSListed){
				//Is this font already listed?
				if(IsFontListed(fontname) == FONT_FOUND){
					AfxMessageBox("A font with that name already exists!", MB_OK);
					return FALSE;
				}
				csEdirText += "/" + fontname + "\t\t(" + csNewFileName + ")\n";
				bIsPSListed = TRUE;
			}
			else if((csCurLine.GetAt(0) == '/' && csCurLine.Find(".pfb)") != -1) && !bIsPFBListed){
				csEdirText += "/" + csNewPFBFile + "\t(" + csNewPFBFile + ")\n";
				bIsPFBListed = TRUE;
			}

			csEdirText += csCurLine + "\n";
		}
		else
			bAddNewLine = TRUE;		

	}while(bStillReading);
	//Remove everything from the file
	EdirFile.SetLength(0);
	//Move to the beginning
	EdirFile.SeekToBegin();
	//Write our text
	EdirFile.WriteString(csEdirText);
	//All done now close the file
	EdirFile.Close();
	//Now copy the file
	if(!::CopyFile(fontfile, (csFontFolder + "\\" + csNewPFBFile), TRUE)){
		CString csError;

		if(GetLastError() == ERROR_FILE_EXISTS)
			csError.Format("Unable to copy resource file. The file: %s already exists.", csNewPFBFile);
		else
			csError = "Unable to copy resource file";
		
		AfxMessageBox(csError, MB_OK);

		return FALSE;
	}
	//Specify the new file name
	csNewFileName = "RP" + fontname + ".ps";
	//Create a new .ps file
	if(!NewPSFile.Open(csFontFolder + "\\" + csNewFileName, CFile::modeCreate  | CFile::modeReadWrite)){
		AfxMessageBox("Unable to create font!", MB_OK);
		return FALSE;
	}
	//Write information to the .ps file.
	//The file is getting placed into the default folder so I can just use the %font% variable
	csCurLine = "%!\n%Created by RasterPlus\n(%font%" + csNewPFBFile + ") (r) file /PFBDecode filter cvx exec\n";
	NewPSFile.WriteString(csCurLine);


	return TRUE;
}

/*
*	RemoveFontResource() will locate a font resource using the font name passed
*	in as a parameter and remove from font resource array.
*
*	Return Value:	TRUE if the new font is succesfully removed FALSE otherwise
*/
BOOL CResourceList::RemoveFontResource(CString fontname){
	CResource*	pResource;
	CStdioFile	StdFile;
	CString		csFontFolder, csCurLine, csEdirText, csPSSearchStr, csPFBSearchStr;
	BOOL		bStillReading;

	//Grab the resource
	pResource = GetResource(fontname);
	//Do we need to edit the _EDIR_ file and remove files?
	//Did we find a matching resource?
	if(pResource == NULL){
		TRACE("Couldn't find the font in the font resource array");
		return FALSE;
	}	
	//Let's just make sure that it's a default font.
	if(pResource->IsDefaultFont){
		TRACE("User is trying to remove a default font and it did not get picked up in our first check.");
		return FALSE;
	}
	//Locate the location of the default font location
	csFontFolder = GetFontLocation();
	//Open the _EDIR_ file and locate the reference to the selected font.
	if(!StdFile.Open(csFontFolder + "\\_edir_", CFile::modeReadWrite)){
			AfxMessageBox("Unable to edit index file!", MB_OK);
			return FALSE;
	}
	
	BOOL bAddNewLine = FALSE;
	do{
		//Grab next line in file
		bStillReading = StdFile.ReadString(csCurLine);
		//Did I need to add a new line character?
		if(bAddNewLine){
			csEdirText += "\n";
			bAddNewLine = FALSE;
		}
		//Is this the line that we want to remove?
		if(csCurLine.GetLength() > 0){
			//Create a search string for the .ps reference
			csPSSearchStr = "/" + fontname + "\t\t(RP" + fontname + ".ps)";
			csPFBSearchStr = "/" + pResource->ResourceFile + "\t(" + pResource->ResourceFile + ")";
			if(csCurLine.Find(csPSSearchStr) == -1 && csCurLine.Find(csPFBSearchStr) == -1 ){
				csEdirText += csCurLine + "\n";
			}
		}
		else
			bAddNewLine = TRUE;

	}while(bStillReading);
	//Remove everything from the file
	StdFile.SetLength(0);
	//Move to the beginning
	StdFile.SeekToBegin();
	//Write our text
	StdFile.WriteString(csEdirText);
	//All done now close the file
	StdFile.Close();
	//Delete the .ps file
	StdFile.Remove(csFontFolder + "\\RP" + fontname + ".ps");
	//Remove the font description file.
	
	//Now delete the .pfb file
	StdFile.Remove(csFontFolder + "\\" + pResource->ResourceFile);

	RemoveAt(GetResIdx(fontname));
	delete pResource;

	return TRUE;
}

/*
*	GetResIdx() will locate a resource using the resource name passed
*	in as a parameter and pass back it's index within the array.
*
*	Return Value:	The index of the resource within the array -1 otherwise.
*/
int CResourceList::GetResIdx(CString resname){
	int i;
	
	for(i=0; i<GetSize(); i++){
		if(((CResource*)GetAt(i))->ResourceName == resname)
			return i;
	}

	return -1;
}

/*
*	GetAvailFonts() will open the _EDIR_ file in the default RasterPlus
*	font directory as specified in the registry. It will then parse through
*	all of the .ps files specied in the _EDIR_ file to find the location
*	of all of the font resources listed within. As this process continues
*	GetAvailFonts() will append the font resource name, the font resource
*	file name, and a string identifying whether or not the font resource is
*	one added by the user or a default resource installed with RasterPlus to 
*	the three respective CStrings passed in as parameters. 
*
*	Return Value:	TRUE is successfull otherwise FALSE
*/
BOOL CResourceList::GetAvailFonts(CString& resname, CString& filename, CString& isdefault){
	BOOL bStillReading, isDefaultFont;
	BOOL rval = FALSE; //Not true until we fill in a value.
	CStdioFile EdirFile;
	CString csEdirtext, csIsDefaultString, csEdirFileName;
	int beg, end;

	//Locate the _edir_ file for the font resources
	//We always know the name of the file we're looking for. Append the filename to the path string.
	csEdirFileName = GetFontLocation() + "\\_edir_";

	/*Build the list of available RasterPlus fonts*/
	//Open the _edir_ file
	if(EdirFile.Open(csEdirFileName, CFile::modeRead)){
		//Parse through the _edir_ file
		do{
			//Grab next line in file
			bStillReading = EdirFile.ReadString(csEdirtext);
			//Remove any white spaces that might be at the beginning of the string
			csEdirtext.TrimLeft();
			//Is it a valid line or just a newline char?
			if(csEdirtext.GetLength() > 0){
				//Is it a line I want to look at or just a comment? Comments begin with '%'
				if(csEdirtext.GetAt(0) == '/' && csEdirtext.Find(".ps)") != -1){
					//Always assume it's a default font until proven otherwise
					//That'll better assure that we won't try and delete a bogus font
					csIsDefaultString = "Default Font;";
					//Get the resource name.
					end = csEdirtext.FindOneOf("\t ");
					if(end != -1){
						resname += csEdirtext.Mid(1, end-1);
						resname += ';';
						//We can now safely return a TRUE status if we haven't decided so already.
						if(!rval)
							rval = TRUE;
					}
					//Get the name of the resource decription file.
					beg = csEdirtext.Find('(');
					end = csEdirtext.Find(')');
					if(beg != -1 && end != -1){
						if(GetResourceLocation(csEdirtext.Mid(beg+1, end-(beg+1)), filename, isDefaultFont))
							filename += ';';
						else
							filename += "No Resource Available;";
						//Is it a default font?
						if(!isDefaultFont)
							//Nope, it's a user added font
							csIsDefaultString = "User Added Font;";
					}
					//Add string to the isdefaultfont list which identifies whether it is or not
					isdefault += csIsDefaultString;
				}
			}
		}while(bStillReading);
	}

	return rval;
}

/*
*	GetFontLocation() Will look into the registry to locate the rasterplus installed font directory.
*
*	Return Value:	A string which identifies the location of the rasterplus font directory. The default 
*	directory is the debugging directory on my machine. (Shawn)
*/
CString CResourceList::GetFontLocation(){
	CProfile Profile;

	//Get the location of the RasterPlus font resource folder from the registry
	//The default font location should point to the jrip font resources of the source folder of the RasterPlus project
	return Profile.GetStr("newripsection", "PSFontPath", "e:\\Source\\jrip\\font");
}

/*
*	GetResourceLocation() Will look into the .ps file to find the
*	location of the resource description file
*
*	Return Value:	TRUE if a location is found, FALSE otherwise.
*/
BOOL CResourceList::GetResourceLocation(CString resfilename, CString& namebuf, BOOL& isdefaultfont){
	CStdioFile ResFile;
	BOOL bStillReading;
	CString csResFileText, csFile;
	int end;

	//Always assume it's a default font until proven otherwise
	//That'll better assure that we won't try and delete a bogus font
	isdefaultfont = TRUE;
	//Get the location of the font folder
	csFile = GetFontLocation() + '\\' + resfilename;

	if(ResFile.Open(csFile, CFile::modeRead) ){
		//Parse through the .ps file
		do{
			//Grab next line in file
			bStillReading = ResFile.ReadString(csResFileText);
			//Was this a font added by the user?
			if(csResFileText.Find("%Created by RasterPlus") != -1){
				//Yep, it's a user added font
				isdefaultfont = FALSE;
				//Now let's grab the next string and continue
				bStillReading = ResFile.ReadString(csResFileText);
			}
			//Is it a valid line or just a newline char?
			//Remove any white spaces that might be at the beginning of the string
			csResFileText.TrimLeft();
			if(csResFileText.GetLength() > 0){
				//Is it a line I want to look at or just a comment? Comments begin with '%'
				if(csResFileText.GetAt(0) == '('){
					//Get the resource name.
					end = csResFileText.Find(")");
					if(end != -1){
						namebuf += csResFileText.Mid(1, end-1);
						//Remove the %font%string
						namebuf.Replace("%font%", "");
						//We can now safely return a TRUE status if we haven't decided so already.
						return TRUE;
					}
				}
				//Is it a substitute resource?
				else if(csResFileText.GetAt(0) == '/'){
					namebuf += "Substitute with:";
					namebuf += csResFileText.Right(csResFileText.GetLength() - (csResFileText.ReverseFind('/')+1));
					return TRUE;
				}
			}
		}while(bStillReading);
	}

	return FALSE;
}

/*
*	GetFontNameFromPFB() will open the specified .pfb file and extract the
*	font name which the .pfb file describes.
*
*	Return Value:	The font name if found otherwise the string ""
*/
CString CResourceList::GetFontNameFromPFB(CString pfbfile){
	BOOL bStillReading;
	CString FontName; //By default this string will always be "" until otherwise specified.
	CStdioFile PFBFile;
	CString csCurLine;
	int beg;

	//Open the .pfb file
	if(PFBFile.Open(pfbfile, CFile::modeRead)){
		do{
			//Parse throught the .pfb file
			bStillReading = PFBFile.ReadString(csCurLine);
			//Trim up the line
			csCurLine.TrimLeft();
			csCurLine.TrimRight();
			//Verify that it's a valid line and not just a newline.
			if(csCurLine.GetLength() > 0){
				//Fine the string FontName
				beg = csCurLine.Find("/FontName");
				if(beg != -1){
					//Remove everything before the found string
					csCurLine.Delete(0, beg);
					//Remove everything after the font name
					beg = csCurLine.Find("def");
					if(beg != -1){
						csCurLine.Delete(beg, csCurLine.GetLength() - beg);
						//Grab the font name and trim it up
						beg = csCurLine.ReverseFind('/');
						if(beg != -1){
							FontName = csCurLine.Mid(beg+1);
							FontName.TrimLeft();
							FontName.TrimRight();
							return FontName;
						}
					}
				}
			}
		}while(bStillReading);
	}	

	return FontName;
}

/*
*	GetATMDir() will open the registry and look for a listing for Adobe ATM
*	and retrieve it's pfb file directory listing.
*
*	Return Value:	A string specifying the directory in which the Adobe ATM
*	stores it's pfb files. If unable to locate the directory then the strong 
*	returned will be the default directory of the installed Rasterplus fonts.
*/
CString CResourceList::GetATMDir(){
	HKEY hKey;
	DWORD dwType = REG_SZ;
	CString csFontDir;
	LPTSTR lpSubKey = "SOFTWARE\\Adobe\\Adobe Type Manager\\setup";
	TCHAR KeyValue[_MAX_PATH];
	DWORD MaxSize = _MAX_PATH;

#ifdef UNICODE								  
   if((RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_READ, &hKey)) == ERROR_SUCCESS)
#else
   if((RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_READ, &hKey)) == ERROR_SUCCESS)
#endif
   {
	   if((RegQueryValueEx(hKey, "PFB_DIR", NULL, &dwType, (LPBYTE)KeyValue, &MaxSize)) == ERROR_SUCCESS)
		   return (CString) KeyValue;
	   
	   RegCloseKey(hKey);
   }

   return GetFontLocation();

}

/*
*	IsFontListed() will open the _EDIR_ and specify whether a font is listed.
*	
*	
*	Return Values:	EDIR_NOT_READ if unable to read the _EDIR_ file for any reason
*					FONT_FOUND if the font is listed in the _EDIR_ file
*					FONT_NOT_FOUND is fontname is not found listed in the _EDIR_ file.
*/
int CResourceList::IsFontListed(CString fontname){
	CStdioFile EdirFile;
	CString csCurLine;
	BOOL bStillReading;

	//Start by opening the _EDIR_ file. If we can't open that then there's no use continuing
	if(!EdirFile.Open(GetFontLocation() + "\\_edir_", CFile::modeRead))
		return EDIR_NOT_READ;

	do{
		//Grab next line in file
		bStillReading = EdirFile.ReadString(csCurLine);
		//Is this a line we want to check?
		if(csCurLine.GetLength() > 0){
			if(csCurLine.Find(fontname + "\t") != -1 || csCurLine.Find(fontname + " ") != -1)
				return FONT_FOUND;
		}
	}while(bStillReading);

	return FONT_NOT_FOUND;
}

BOOL CResourceList::AddResource(CResource* resource){
	int i;
	
	//Make sure that the resource doesn't already exist
	for(i=0; i<GetSize(); i++){
		if(((CResource*)GetAt(i))->ResourceName == resource->ResourceName)
			return FALSE;
	}

	Add((CObject*)resource);

	return TRUE;
}
