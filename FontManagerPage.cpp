/////////////////////////////////////////////////////////////////////////////
// FontManagerPage.cpp
//                                                                        
// font manager for RasterPlus
// Copyright (C) 1999 Graphx, Inc.
//
// mods:
//
// 11-11-99 Version 3.0.2 - font manager was not working at all! - fixed
//

#include "stdafx.h"
#include "zeus.h"
#include "profile.h"
#include "FontManagerPage.h"
#include "rexdirdi.h"
#include "control.hm"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFontManagerPage property page

// the control help-map
static DWORD dwHelpMap[] = 
   {
   IDC_REMOVE_FONT, HIDC_REMOVE_FONT,
   IDC_INSTALLED_FONTS, HIDC_INSTALLED_FONTS,
   IDC_NEW_FONTS, HIDC_NEW_FONTS,
   ID_ADD_FONT, HIDC_ADD_FONT,
   IDC_NEW_FONTS, HIDC_NEW_FONTS,
   IDC_AVAIL_BOX, HIDC_INSTALLED_FONTS,
   IDC_NEW_BOX, HIDC_NEW_FONTS,
   IDC_BROWSE, HIDC_BROWSE_FONTS,
   IDC_BROWSE_PRMPT, HIDC_BROWSE_FONTS,
   IDC_FONTDIR_BROWSE, HIDC_BROWSE_FONTS,
   0,0
   };							

IMPLEMENT_DYNCREATE(CFontManagerPage, CPropertyPage)

CFontManagerPage::CFontManagerPage() : CPropertyPage(CFontManagerPage::IDD)
{
	//{{AFX_DATA_INIT(CFontManagerPage)
	m_FontDir = _T("");
	//}}AFX_DATA_INIT
}

CFontManagerPage::~CFontManagerPage()
{
}

void CFontManagerPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFontManagerPage)
	DDX_Text(pDX, IDC_FONTDIR, m_FontDir);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFontManagerPage, CPropertyPage)
	//{{AFX_MSG_MAP(CFontManagerPage)
	ON_BN_CLICKED(IDC_REMOVE_FONT, OnRemoveFont)
	ON_LBN_SELCHANGE(IDC_INSTALLED_FONTS, OnSelchangeInstalledFonts)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(ID_ADD_FONT, OnAddFont)
	ON_LBN_DBLCLK(IDC_NEW_FONTS, OnDblclkNewFonts)
	ON_WM_CONTEXTMENU()	
	ON_WM_HELPINFO()	
	ON_LBN_SELCHANGE(IDC_NEW_FONTS, OnSelchangeNewFonts)
	ON_EN_CHANGE(IDC_FONTDIR, OnChangeFontdir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontManagerPage message handlers

void CFontManagerPage::OnRemoveFont() 
{
	CString csCurFont;

	//Get the list box
	CListBox* pLB = (CListBox*)GetDlgItem(IDC_INSTALLED_FONTS);
	//Get the current selection
	int CurSel = pLB->GetCurSel();
	//Get the name of the currently selected font
	pLB->GetText(CurSel, csCurFont);
	//Is the user trying to remove a default font?
	if(csCurFont.GetAt(0) == '*'){
			csCurFont.Delete(0);
			AfxMessageBox("Unable to remove default fonts. All Font names beginning with * are default fonts", MB_OK);
			return;
	}
	//Remove the font from the array
	if(FontList.RemoveFontResource(csCurFont)){
		pLB->DeleteString(CurSel--);
	}
	if(CurSel > 0){
		//Reset the current selection
		pLB->SetCurSel(CurSel);
		//Change the pfb location info
		pLB->GetText(CurSel, csCurFont);
		pLB->GetText(CurSel, csCurFont);
		if(csCurFont.GetAt(0) == '*')
			csCurFont.Delete(0);
		CResource* pResource = FontList.GetResource(csCurFont);
		if(pResource){
			//Do we need to disable the remove button?
			((CButton*)GetDlgItem(IDC_REMOVE_FONT))->EnableWindow(!pResource->IsDefaultFont);
		}
	}
	else{
		//Disable the reomve button
		((CButton*)GetDlgItem(IDC_REMOVE_FONT))->EnableWindow(FALSE);
	}
	//Re-init the add font list box
	InitAddFontListBox(FALSE);
}

void CFontManagerPage::OnOK() 
{
	// TODO: Add extra validation here
	
	CPropertyPage::OnOK();
}

BOOL CFontManagerPage::OnInitDialog() 
{
	int ListSize, i;
	CResource* Font;
	CString NewFontName;

	CPropertyPage::OnInitDialog();
	
	//Fill in list with all available fonts
	CListBox* FontNames		= (CListBox*)GetDlgItem(IDC_INSTALLED_FONTS);
	//Get list of available fonts
	if(FontList.InitFontResourceList()){
		ListSize = FontList.GetSize();
		//Fill in combo box with list of fonts
		for(i=0; i<ListSize; i++){
			//All font Rasterplus default installed fonts begin with an *
			Font = (CResource*)FontList.GetAt(i);
			if(Font->IsDefaultFont)
				NewFontName = '*';
			else
				NewFontName.Empty();

			NewFontName += Font->ResourceName;
			FontNames->AddString(NewFontName);
		}
		//Set the first item in the list as the selected item by default
		FontNames->SetCurSel(0);
		//Fill in list box with Location of the selected font
		Font = (CResource*)FontList.GetAt(FontNames->GetCurSel());
		if(Font){
			//Do we need to disable the remove button?
			((CButton*)GetDlgItem(IDC_REMOVE_FONT))->EnableWindow(!Font->IsDefaultFont);
		}

		bIsInit = TRUE;
		UpdateData(TRUE);
	}
	else{
		FontNames->AddString("Unable to load font list");
	}
	//Now fill in Add Font list box
	InitAddFontListBox();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CFontManagerPage::OnSetActive() 
{
	CString SelectedFont;
	// Grab the list box
	CListBox* FontNames	= (CListBox*)GetDlgItem(IDC_INSTALLED_FONTS);
	FontNames->GetText(FontNames->GetCurSel(), SelectedFont);
	if(SelectedFont.GetAt(0) == '*')
		SelectedFont.Delete(0, 1);

	CResource* Font = FontList.GetResource(SelectedFont);
	if(Font){
		//Do we need to disable the remove button?
		((CButton*)GetDlgItem(IDC_REMOVE_FONT))->EnableWindow(!Font->IsDefaultFont);
	}

	return CPropertyPage::OnSetActive();
}

void CFontManagerPage::OnSelchangeInstalledFonts() 
{
	CString SelectedFont;
	// Grab the list box
	CListBox* FontNames	= (CListBox*)GetDlgItem(IDC_INSTALLED_FONTS);
	FontNames->GetText(FontNames->GetCurSel(), SelectedFont);
	if(SelectedFont.GetAt(0) == '*')
		SelectedFont.Delete(0, 1);

	CResource* Font = FontList.GetResource(SelectedFont);
	if(Font){
		//Do we need to disable the remove button?
		((CButton*)GetDlgItem(IDC_REMOVE_FONT))->EnableWindow(!Font->IsDefaultFont);
	}
}

void CFontManagerPage::OnBrowse() 
{
	CString cs;

	cs = "Select Font Folder";
   
	CRexDirDialog dlg((const char*)cs);      
	dlg.m_ofn.lpstrInitialDir = m_FontDir;
	dlg.m_ofn.Flags &= ~OFN_EXPLORER;   
	if (dlg.DoModal() == IDOK){
		CString csNewPath = dlg.GetPath(TRUE);
		//Did we change the path?
		if(m_FontDir != csNewPath){
			//Record the new path
			m_FontDir = csNewPath;		
			//Restock the listbox with the new files
			InitAddFontListBox(FALSE);
		}
	}
}

BOOL CFontManagerPage::InitAddFontListBox(BOOL bUseDefaultDir/*=TRUE*/){
	CListBox*	pFontList;
	CResource*	pResource;
	CString		cs;
	BOOL		rval;
	int			i;
	
	pFontList = (CListBox*)GetDlgItem(IDC_NEW_FONTS);
	pFontList->ResetContent();

	if(bUseDefaultDir)
		m_FontDir = AddFontList.GetATMDir();

	rval = AddFontList.InitFontListUsingPFB(m_FontDir);

	if(rval){
		for(i=0; i<AddFontList.GetSize(); i++){
			//Grab the list box
			pFontList = (CListBox*)GetDlgItem(IDC_NEW_FONTS);
			//Grab the next resource
			pResource = (CResource*)AddFontList.GetAt(i);
			//Add the resource to the list
			pFontList->AddString(pResource->ResourceName);
		}
	}
	else{
		cs.LoadString(IDS_NO_FONTS);
		pFontList->AddString(cs);
	}

	UpdateData(FALSE);

	pFontList->SetCurSel(0);
	UpdateAddButton();

	return rval;
}

void CFontManagerPage::OnAddFont() 
{
	CString	FontName;
	int		CurSel;

	//Grab the list box
 	CListBox* pLb = ((CListBox*)GetDlgItem(IDC_NEW_FONTS));
	//Grab the selected font name
	CurSel = pLb->GetCurSel();
	if(CurSel != LB_ERR){
		pLb->GetText(CurSel, FontName);
		//Get the font resource from our array
		CResource* pResource = AddFontList.GetResource(FontName);
		if(pResource){
			AddFontList.AddFontToEdir(pResource->ResourceName, pResource->ResourceFile);
			pLb->DeleteString(CurSel--);
			if(CurSel >= 0){
				pLb->SetCurSel(CurSel);
				UpdateAddButton();
			}
			else{
				((CListBox*)GetDlgItem(IDC_NEW_FONTS))->SetCurSel(0);
				UpdateAddButton();
			}

		}
		//Now update the available font list.
		pLb = ((CListBox*)GetDlgItem(IDC_INSTALLED_FONTS));
		//Add the resource to the list of font resources
		FontList.InitFontResourceList();
		//Add the resource name to the list box.
		pLb->AddString(FontName);
	}


	UpdateData(TRUE);
}

void CFontManagerPage::OnDblclkNewFonts() 
{
	CString	FontName;
	int		CurSel;

	//Grab the list box
 	CListBox* pLb = ((CListBox*)GetDlgItem(IDC_NEW_FONTS));
	//Grab the selected font name
	CurSel = pLb->GetCurSel();
	if(CurSel != LB_ERR){
		pLb->GetText(CurSel, FontName);
		//Get the font resource from our array
		CResource* pResource = AddFontList.GetResource(FontName);
		if(pResource){
			AddFontList.AddFontToEdir(pResource->ResourceName, pResource->ResourceFile);
			pLb->DeleteString(CurSel--);
			if(CurSel >= 0)
				pLb->SetCurSel(CurSel);
		}
		//Now update the available font list.
		pLb = ((CListBox*)GetDlgItem(IDC_INSTALLED_FONTS));
		//Add the resource to the list of font resources
		FontList.InitFontResourceList();
		//Add the resource name to the list box.
		pLb->AddString(FontName);
	}


	UpdateData(TRUE);
	
}

BOOL CFontManagerPage::OnHelpInfo(HELPINFO* pHelpInfo) 
   {
   if (pHelpInfo->iContextType == HELPINFO_WINDOW)   // must be for a control
      {
      ::WinHelp ((HWND)pHelpInfo->hItemHandle,
         AfxGetApp()->m_pszHelpFilePath, HELP_WM_HELP, 
		 (DWORD)(LPVOID)dwHelpMap);
	  return TRUE;
	  }      
   else
   	  return CDialog::OnHelpInfo(pHelpInfo);
   }
 
void CFontManagerPage::OnContextMenu(CWnd* pWnd, CPoint point) 
   {   
   // call context help
   ::WinHelp(pWnd->m_hWnd, AfxGetApp()->m_pszHelpFilePath, 
      HELP_CONTEXTMENU,  (DWORD)(LPVOID)dwHelpMap);
   }

/////////////////////////////////////////////////////////////////////////////
// CFontManagerPage message handlers

void CFontManagerPage::OnSelchangeNewFonts() 
{
	UpdateAddButton();
}

void CFontManagerPage::UpdateAddButton(){
	CListBox* pFontList = (CListBox*)GetDlgItem(IDC_NEW_FONTS);
	CString csCurSel;
	CString csNoFontStr;
	
	int Idx = pFontList->GetCurSel();
	
	if(Idx == LB_ERR){
		((CButton*)GetDlgItem(ID_ADD_FONT))->EnableWindow(FALSE);
		return;
	}
	
	csNoFontStr.LoadString(IDS_NO_FONTS);


	pFontList->GetText(Idx, csCurSel);

	if(csCurSel == csNoFontStr)
		((CButton*)GetDlgItem(ID_ADD_FONT))->EnableWindow(FALSE);
	else
		((CButton*)GetDlgItem(ID_ADD_FONT))->EnableWindow(TRUE);
}

void CFontManagerPage::OnChangeFontdir() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	UpdateData(TRUE);
	InitAddFontListBox(FALSE);
	
}
