// PrintIndexPage.cpp : implementation file
//
// Copyright (C) 2001 Graphx, Inc.
//
// mods:
//
// 10-17-01 Version 5.0.0 - Change default for Images Per Page to 36
// 12-28-01 Version 5.0.1 - Add Index by Size Option
//                          Add Crop/Fit Transform selection to Index by Size
// 10-21-01 Version 5.1.0 - Add additional fields, whitespace, copies, pane selection
//

#include "stdafx.h"
#include "zeus.h"
#include "PrintIndexPage.h"
#include "Inidefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INDEX_PRINT_WHITESPACE .15f

/////////////////////////////////////////////////////////////////////////////
// CPrintIndexPage dialog

IMPLEMENT_DYNCREATE(CPrintIndexPage, CPropertyPage)

CPrintIndexPage::CPrintIndexPage()  : CPropertyPage(CPrintIndexPage::IDD)
{
	//{{AFX_DATA_INIT(CPrintIndexPage)
	m_DisplayImageName = TRUE;
	m_MaxImagesPerPage = 0;
	m_Image_Height = _T("");
	m_Image_Width = _T("");
	m_SinglePage = 1;
	m_ImageCount = 0;
	m_ImageSize = 0;
	m_ImageTransform = 1;
	m_ImageSize_Inch = -1;
	m_WhiteSpace = 0;
	m_HorizontalWhiteSpace = _T("");
	m_VerticalWhiteSpace = _T("");
	m_WhiteSpace_Inch = -1;
	m_Select_Pane = -1;
	m_Index_Copies = 1;
	//}}AFX_DATA_INIT
}

CPrintIndexPage::~CPrintIndexPage()
{
}


void CPrintIndexPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintIndexPage)
	DDX_Control(pDX, IDC_INDEX_COPIES_SPIN, m_IndexCopiesSpin);
	DDX_Control(pDX, IDC_IMAGE_WIDTH_SPIN, m_Image_WidthSpin);
	DDX_Control(pDX, IDC_IMAGE_HEIGHT_SPIN, m_Image_HeightSpin);
	DDX_Control(pDX, IDC_MAXIMAGES_SPIN, m_MaxImagesSpin);
	DDX_Check(pDX, IDC_DISPLAY_IMAGENAME, m_DisplayImageName);
	DDX_Text(pDX, IDC_MAXIMAGES_EDIT, m_MaxImagesPerPage);
	DDV_MinMaxInt(pDX, m_MaxImagesPerPage, IDDM_PI_IMAGES_PP_MIN, IDDM_PI_IMAGES_PP_MAX);
	DDX_Text(pDX, IDC_IMAGE_HEIGHT_EDIT, m_Image_Height);
	DDV_MaxChars(pDX, m_Image_Height, 7);
	DDX_Text(pDX, IDC_IMAGE_WIDTH_EDIT, m_Image_Width);
	DDV_MaxChars(pDX, m_Image_Width, 7);
	DDX_Radio(pDX, IDC_SINGLEPAGE, m_SinglePage);
	DDX_Radio(pDX, IDC_IMAGECOUNT, m_ImageCount);
	DDX_Radio(pDX, IDC_IMAGESIZE, m_ImageSize);
//	DDX_Radio(pDX, IDC_IMAGESIZE_CROP, m_ImageTransform);
	DDX_CBIndex(pDX, IDC_IMAGESIZE_UNITS, m_ImageSize_Inch);
	DDX_Check(pDX, IDC_WHITE_SPACE, m_WhiteSpace);
	DDX_Text(pDX, IDC_HORIZONTAL_WHITE_EDIT, m_HorizontalWhiteSpace);
	DDV_MaxChars(pDX, m_HorizontalWhiteSpace, 7);
	DDX_Text(pDX, IDC_VERTICAL_WHITE_EDIT, m_VerticalWhiteSpace);
	DDV_MaxChars(pDX, m_VerticalWhiteSpace, 7);
	DDX_CBIndex(pDX, IDC_WHITE_SPACE_UNITS, m_WhiteSpace_Inch);
	DDX_CBIndex(pDX, IDC_SELECT_PANE, m_Select_Pane);
	DDX_Text(pDX, IDC_INDEX_COPIES, m_Index_Copies);
	DDV_MinMaxInt(pDX, m_Index_Copies, 1, 99);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintIndexPage, CDialog)
	//{{AFX_MSG_MAP(CPrintIndexPage)
	ON_BN_CLICKED(IDC_SINGLEPAGE, OnSinglepage)
	ON_BN_CLICKED(IDC_DISPLAY_IMAGENAME, OnDisplayImagename)
	ON_EN_CHANGE(IDC_IMAGE_HEIGHT_EDIT, OnChangeImageHeightEdit)
	ON_EN_CHANGE(IDC_IMAGE_WIDTH_EDIT, OnChangeImageWidthEdit)
	ON_BN_CLICKED(IDC_IMAGECOUNT, OnImagecount)
	ON_BN_CLICKED(IDC_IMAGESIZE, OnImagesize)
	ON_BN_CLICKED(IDC_IMAGESIZE_CROP, OnImagesizeCrop)
	ON_BN_CLICKED(IDC_IMAGESIZE_FIT, OnImagesizeFit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_IMAGE_WIDTH_SPIN, OnDeltaposImageWidthSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_IMAGE_HEIGHT_SPIN, OnDeltaposImageHeightSpin)
	ON_CBN_SELCHANGE(IDC_IMAGESIZE_UNITS, OnSelchangeImagesizeUnits)
	ON_BN_CLICKED(IDC_WHITE_SPACE, OnWhiteSpace)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HORIZONTAL_WHITE_SPIN, OnDeltaposHorizontalWhiteSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_VERTICAL_WHITE_SPIN, OnDeltaposVerticalWhiteSpin)
	ON_CBN_SELCHANGE(IDC_WHITE_SPACE_UNITS, OnSelchangeWhiteSpaceUnits)
	ON_EN_CHANGE(IDC_HORIZONTAL_WHITE_EDIT, OnChangeHorizontalWhiteEdit)
	ON_EN_CHANGE(IDC_VERTICAL_WHITE_EDIT, OnChangeVerticalWhiteEdit)
	ON_EN_CHANGE(IDC_INDEX_COPIES, OnChangeIndexCopies)
	ON_NOTIFY(UDN_DELTAPOS, IDC_INDEX_COPIES_SPIN, OnDeltaposIndexCopiesSpin)
	ON_CBN_SELCHANGE(IDC_SELECT_PANE, OnSelchangeSelectPane)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintIndexPage message handlers

BOOL CPrintIndexPage::OnInitDialog() 
{
	float fval;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_SinglePage = Profile.GetInt(IDS_GENERALSECTION, IDS_PI_SINGLEPAGE, TRUE);

	m_DisplayImageName = Profile.GetInt(IDS_GENERALSECTION, IDS_PI_IMAGENAME, TRUE);
	
	m_MaxImagesPerPage = Profile.GetInt(IDS_GENERALSECTION, IDS_PI_IMAGEPERPAGE, IDDM_PI_IMAGES_PP_DEF);
	m_ImageCount = Profile.GetInt(IDS_GENERALSECTION, IDS_PI_IMAGECOUNT, FALSE);

	m_ImageSize = Profile.GetInt(IDS_GENERALSECTION, IDS_PI_IMAGESIZE, FALSE);
	m_ImageTransform = Profile.GetInt(IDS_GENERALSECTION, IDS_PI_IMAGETRANSFORM, TRUE);
	m_ImageSize_Inch = Profile.GetInt(IDS_GENERALSECTION, IDS_PI_IMAGEUNITS, M_INCH_USEINCHES );
	m_Image_Height = Profile.GetStr(IDS_GENERALSECTION, IDS_PI_IMAGEHEIGHT, "3.0");
	m_Image_Width = Profile.GetStr(IDS_GENERALSECTION, IDS_PI_IMAGEWIDTH,"2.0");

	// Store in Inches so convert to MM if appropriate
	if (m_ImageSize_Inch == M_INCH_USEMM)
	{
		fval = (float)atof(m_Image_Width);
		fval *= 25.4F;
		// put back in
		m_Image_Width.Format("%.3f", fval);   

		fval = (float)atof(m_Image_Height);
		fval *= 25.4F;
		// put back in
		m_Image_Height.Format("%.3f", fval);   
	}	

	m_WhiteSpace = Profile.GetInt(IDS_GENERALSECTION, IDS_PI_WHITE_SPACE, FALSE);
	m_WhiteSpace_Inch = Profile.GetInt(IDS_GENERALSECTION, IDS_PI_WHITESPACEUNITS, M_INCH_USEINCHES );
	m_HorizontalWhiteSpace = Profile.GetStr(IDS_GENERALSECTION, IDS_PI_HORIZONTAL_WHITE, "0.15");
	m_VerticalWhiteSpace = Profile.GetStr(IDS_GENERALSECTION, IDS_PI_VERTICAL_WHITE, "0.15");

	if (m_WhiteSpace_Inch == M_INCH_USEMM)
	{
		fval = (float)atof(m_HorizontalWhiteSpace);
		fval *= 25.4F;
		// put back in
		m_HorizontalWhiteSpace.Format("%.3f", fval);   

		fval = (float)atof(m_VerticalWhiteSpace);
		fval *= 25.4F;
		// put back in
		m_VerticalWhiteSpace.Format("%.3f", fval);   
	}
	else
	{
		fval = (float)atof(m_HorizontalWhiteSpace);
		// put back in
		m_HorizontalWhiteSpace.Format("%.2f", fval);   

		fval = (float)atof(m_VerticalWhiteSpace);
		// put back in
		m_VerticalWhiteSpace.Format("%.2f", fval);
	}

	m_Index_Copies	= Profile.GetInt(IDS_GENERALSECTION, IDS_PI_COPIES, 1 );
	m_Select_Pane	= Profile.GetInt(IDS_GENERALSECTION, IDS_PI_PANE, 1 );

	UpdateData(FALSE);

	m_MaxImagesSpin.SetRange(IDDM_PI_IMAGES_PP_MIN, IDDM_PI_IMAGES_PP_MAX);
	m_IndexCopiesSpin.SetRange(0, 99);

	SetFields();

	if ( m_ImageTransform == 1 )
	{
		((CButton*)GetDlgItem(IDC_IMAGESIZE_CROP))->SetCheck( TRUE );
		((CButton*)GetDlgItem(IDC_IMAGESIZE_FIT))->SetCheck( FALSE );
	}
	else
	{
		((CButton*)GetDlgItem(IDC_IMAGESIZE_CROP))->SetCheck( FALSE );
		((CButton*)GetDlgItem(IDC_IMAGESIZE_FIT))->SetCheck( TRUE );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrintIndexPage::OnSinglepage() 
{
	// TODO: Add your control notification handler code here
	m_SinglePage	= TRUE;
	m_ImageCount	= FALSE;
	m_ImageSize		= FALSE;

	SetFields();
}

void CPrintIndexPage::OnDisplayImagename() 
{
	// TODO: Add your control notification handler code here
	
}

void CPrintIndexPage::OnOurOK() 
{   
	GetParent()->SendMessage(PSM_PRESSBUTTON, PSBTN_OK);
	((CPropertySheet*)GetParent())->EndDialog(IDOK);   
}

void CPrintIndexPage::OnOurcancel() 
{   
	GetParent()->SendMessage(PSM_PRESSBUTTON, PSBTN_CANCEL);
	((CPropertySheet*)GetParent())->EndDialog(IDCANCEL);
}

void CPrintIndexPage::OnOK() 
{
	float fval;

	// make sure that the data is updated
	UpdateData(FALSE);

	// write dialog vars	
	Profile.WriteInt(IDS_GENERALSECTION, IDS_PI_SINGLEPAGE, ((CButton*)GetDlgItem(IDC_SINGLEPAGE))->GetCheck());
	
	Profile.WriteInt(IDS_GENERALSECTION, IDS_PI_IMAGENAME, m_DisplayImageName);

	Profile.WriteInt(IDS_GENERALSECTION, IDS_PI_IMAGECOUNT, ((CButton*)GetDlgItem(IDC_IMAGECOUNT))->GetCheck()); 
	Profile.WriteInt(IDS_GENERALSECTION, IDS_PI_IMAGEPERPAGE, m_MaxImagesPerPage);

	Profile.WriteInt(IDS_GENERALSECTION, IDS_PI_IMAGESIZE, ((CButton*)GetDlgItem(IDC_IMAGESIZE))->GetCheck());
	Profile.WriteInt(IDS_GENERALSECTION, IDS_PI_IMAGETRANSFORM, ((CButton*)GetDlgItem(IDC_IMAGESIZE_CROP))->GetCheck());
	Profile.WriteInt(IDS_GENERALSECTION, IDS_PI_IMAGEUNITS, m_ImageSize_Inch );

	// Always store in Inches
	if (m_ImageSize_Inch == M_INCH_USEMM)
	{
		fval = (float)atof(m_Image_Width);
		fval /= 25.4F;
		// put back in
		m_Image_Width.Format("%.3f", fval );

		fval = (float)atof(m_Image_Height);
		fval /= 25.4F;
		// put back in
		m_Image_Height.Format("%.3f", fval);   
	}	

	Profile.WriteStr(IDS_GENERALSECTION, IDS_PI_IMAGEHEIGHT, m_Image_Height);
	Profile.WriteStr(IDS_GENERALSECTION, IDS_PI_IMAGEWIDTH, m_Image_Width);

	Profile.WriteInt(IDS_GENERALSECTION, IDS_PI_WHITE_SPACE, ((CButton*)GetDlgItem(IDC_WHITE_SPACE))->GetCheck());
	Profile.WriteInt(IDS_GENERALSECTION, IDS_PI_WHITESPACEUNITS, m_WhiteSpace_Inch );

	// Always store in Inches
	if (m_WhiteSpace_Inch == M_INCH_USEMM)
	{
		fval = (float)atof(m_HorizontalWhiteSpace);
		fval /= 25.4F;
		// put back in
		m_HorizontalWhiteSpace.Format("%.3f", fval );

		fval = (float)atof(m_VerticalWhiteSpace);
		fval /= 25.4F;
		// put back in
		m_VerticalWhiteSpace.Format("%.3f", fval);   
	}	
	else
	{
		fval = (float)atof(m_HorizontalWhiteSpace);
		// put back in
		m_HorizontalWhiteSpace.Format("%.2f", fval);   

		fval = (float)atof(m_VerticalWhiteSpace);
		// put back in
		m_VerticalWhiteSpace.Format("%.2f", fval);
	}

	Profile.WriteStr(IDS_GENERALSECTION, IDS_PI_HORIZONTAL_WHITE, m_HorizontalWhiteSpace);
	Profile.WriteStr(IDS_GENERALSECTION, IDS_PI_VERTICAL_WHITE, m_VerticalWhiteSpace);

	Profile.WriteInt(IDS_GENERALSECTION, IDS_PI_COPIES, m_Index_Copies );
	Profile.WriteInt(IDS_GENERALSECTION, IDS_PI_PANE, m_Select_Pane );

	Profile.WriteInt(IDS_GENERALSECTION, IDS_PI_IMAGETRANSFORM, m_ImageTransform );

	CPropertyPage::OnOK();
}


void CPrintIndexPage::OnChangeImageHeightEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CPrintIndexPage::OnChangeImageWidthEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CPrintIndexPage::OnImagecount() 
{
	// TODO: Add your control notification handler code here
	m_ImageCount	= TRUE;
	m_SinglePage	= FALSE;
	m_ImageSize		= FALSE;

	SetFields();
	
}

void CPrintIndexPage::OnImagesize() 
{
	// TODO: Add your control notification handler code here
	m_ImageSize		= TRUE;
	m_SinglePage	= FALSE;
	m_ImageCount	= FALSE;

	SetFields();
	
}

void CPrintIndexPage::OnImagesizeCrop() 
{
	// TODO: Add your control notification handler code here
	m_ImageTransform = 1;

	((CButton*)GetDlgItem(IDC_IMAGESIZE_CROP))->SetCheck( TRUE );
	((CButton*)GetDlgItem(IDC_IMAGESIZE_FIT))->SetCheck( FALSE );
}

void CPrintIndexPage::OnImagesizeFit() 
{
	// TODO: Add your control notification handler code here
	m_ImageTransform = 0;

	((CButton*)GetDlgItem(IDC_IMAGESIZE_CROP))->SetCheck( FALSE );
	((CButton*)GetDlgItem(IDC_IMAGESIZE_FIT))->SetCheck( TRUE );
}

void CPrintIndexPage::SetFields() 
{
	GetDlgItem(IDC_IMAGESIZE_CROP)->EnableWindow( TRUE );
	GetDlgItem(IDC_IMAGESIZE_FIT)->EnableWindow( TRUE );

	if ( m_SinglePage == TRUE )
	{
		GetDlgItem(IDC_MAXIMAGES_EDIT)->EnableWindow( FALSE );
		GetDlgItem(IDC_MAXIMAGES_SPIN)->EnableWindow( FALSE );

		GetDlgItem(IDC_IMAGESIZE_WIDTH)->EnableWindow( FALSE );
		GetDlgItem(IDC_IMAGE_WIDTH_EDIT)->EnableWindow( FALSE );
		GetDlgItem(IDC_IMAGE_WIDTH_SPIN)->EnableWindow( FALSE );
		GetDlgItem(IDC_IMAGESIZE_HEIGHT)->EnableWindow( FALSE );
		GetDlgItem(IDC_IMAGESIZE_UNITS)->EnableWindow( FALSE );
		GetDlgItem(IDC_IMAGE_HEIGHT_EDIT)->EnableWindow( FALSE );
		GetDlgItem(IDC_IMAGE_HEIGHT_SPIN)->EnableWindow( FALSE );
	}
	else
	{
		if ( m_ImageCount == TRUE )
		{
			GetDlgItem(IDC_MAXIMAGES_EDIT)->EnableWindow( TRUE );
			GetDlgItem(IDC_MAXIMAGES_SPIN)->EnableWindow( TRUE );

			GetDlgItem(IDC_IMAGESIZE_WIDTH)->EnableWindow( FALSE );
			GetDlgItem(IDC_IMAGE_WIDTH_EDIT)->EnableWindow( FALSE );
			GetDlgItem(IDC_IMAGE_WIDTH_SPIN)->EnableWindow( FALSE );
			GetDlgItem(IDC_IMAGESIZE_HEIGHT)->EnableWindow( FALSE );
			GetDlgItem(IDC_IMAGESIZE_UNITS)->EnableWindow( FALSE );
			GetDlgItem(IDC_IMAGE_HEIGHT_EDIT)->EnableWindow( FALSE );
			GetDlgItem(IDC_IMAGE_HEIGHT_SPIN)->EnableWindow( FALSE );
		}
		else		// Must be Image Size
		{
			GetDlgItem(IDC_MAXIMAGES_EDIT)->EnableWindow( FALSE );
			GetDlgItem(IDC_MAXIMAGES_SPIN)->EnableWindow( FALSE );

			GetDlgItem(IDC_IMAGESIZE_WIDTH)->EnableWindow( TRUE );
			GetDlgItem(IDC_IMAGE_WIDTH_EDIT)->EnableWindow( TRUE );
			GetDlgItem(IDC_IMAGE_WIDTH_SPIN)->EnableWindow( TRUE );
			GetDlgItem(IDC_IMAGESIZE_HEIGHT)->EnableWindow( TRUE );
			GetDlgItem(IDC_IMAGESIZE_UNITS)->EnableWindow( TRUE );
			GetDlgItem(IDC_IMAGE_HEIGHT_EDIT)->EnableWindow( TRUE );
			GetDlgItem(IDC_IMAGE_HEIGHT_SPIN)->EnableWindow( TRUE );
		}
	}

	if ( m_WhiteSpace == TRUE )
	{
		GetDlgItem(IDC_WHITE_SPACE_UNITS)->EnableWindow( TRUE );
		GetDlgItem(IDC_HORIZONTAL_WHITE_EDIT)->EnableWindow( TRUE );
		GetDlgItem(IDC_HORIZONTAL_WHITE_SPIN)->EnableWindow( TRUE );
		GetDlgItem(IDC_VERTICAL_WHITE_EDIT)->EnableWindow( TRUE );
		GetDlgItem(IDC_VERTICAL_WHITE_SPIN)->EnableWindow( TRUE );
	}
	else
	{
		GetDlgItem(IDC_WHITE_SPACE_UNITS)->EnableWindow( FALSE );
		GetDlgItem(IDC_HORIZONTAL_WHITE_EDIT)->EnableWindow( FALSE );
		GetDlgItem(IDC_HORIZONTAL_WHITE_SPIN)->EnableWindow( FALSE );
		GetDlgItem(IDC_VERTICAL_WHITE_EDIT)->EnableWindow( FALSE );
		GetDlgItem(IDC_VERTICAL_WHITE_SPIN)->EnableWindow( FALSE );
	}

	((CButton*)GetDlgItem(IDC_IMAGECOUNT))->SetCheck( m_ImageCount );
	((CButton*)GetDlgItem(IDC_IMAGESIZE))->SetCheck( m_ImageSize );
	((CButton*)GetDlgItem(IDC_SINGLEPAGE))->SetCheck( m_SinglePage );
//	((CButton*)GetDlgItem(IDC_IMAGESIZE_CROP))->SetCheck( m_ImageTransform );
//	((CButton*)GetDlgItem(IDC_IMAGESIZE_FIT))->SetCheck( !m_ImageTransform );		
	((CButton*)GetDlgItem(IDC_WHITE_SPACE))->SetCheck( m_WhiteSpace );	
	
	UpdateData();
}

void CPrintIndexPage::OnDeltaposImageWidthSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	float fval = (float)atof(m_Image_Width);
	// increment/decrement
	fval += ((float)(-pNMUpDown->iDelta)) * .1F;
	// put back in
	m_Image_Width.Format("%.3f", fval);      

	// apply this      
	UpdateData(FALSE);  

	*pResult = 0;
}

void CPrintIndexPage::OnDeltaposImageHeightSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	float fval = (float)atof(m_Image_Height);
	// increment/decrement
	fval += ((float)(-pNMUpDown->iDelta)) * .1F;
	// put back in
	m_Image_Height.Format("%.3f", fval);      

	// apply this      
	UpdateData(FALSE);    

	*pResult = 0;
}

void CPrintIndexPage::OnSelchangeImagesizeUnits() 
{
	// TODO: Add your control notification handler code here
   float fval;
   BOOL bOldVal = m_ImageSize_Inch;
   UpdateData(TRUE);
			  
   if (bOldVal != M_INCH_USEINCHES && m_ImageSize_Inch == M_INCH_USEINCHES)
	  {
      fval = (float)atof(m_Image_Width);
	  fval /= 25.4F;
      // put back in
	  m_Image_Width.Format("%.3f", fval );

      fval = (float)atof(m_Image_Height);
	  fval /= 25.4F;
      // put back in
      m_Image_Height.Format("%.3f", fval);   

	  m_ImageSize_Inch = M_INCH_USEINCHES;
	  
	  UpdateData(FALSE);
	  }
   else if (bOldVal != M_INCH_USEMM && m_ImageSize_Inch == M_INCH_USEMM)
	  {
      fval = (float)atof(m_Image_Width);
	  fval *= 25.4F;
      // put back in
      m_Image_Width.Format("%.3f", fval);   

      fval = (float)atof(m_Image_Height);
	  fval *= 25.4F;
      // put back in
      m_Image_Height.Format("%.3f", fval);   

	  m_ImageSize_Inch = M_INCH_USEMM;

	  UpdateData(FALSE);
	  }	
}

void CPrintIndexPage::OnWhiteSpace() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if ( m_WhiteSpace == TRUE )
	{
		GetDlgItem(IDC_WHITE_SPACE_UNITS)->EnableWindow( TRUE );
		GetDlgItem(IDC_HORIZONTAL_WHITE_EDIT)->EnableWindow( TRUE );
		GetDlgItem(IDC_HORIZONTAL_WHITE_SPIN)->EnableWindow( TRUE );
		GetDlgItem(IDC_VERTICAL_WHITE_EDIT)->EnableWindow( TRUE );
		GetDlgItem(IDC_VERTICAL_WHITE_SPIN)->EnableWindow( TRUE );
	}
	else
	{
		GetDlgItem(IDC_WHITE_SPACE_UNITS)->EnableWindow( FALSE );
		GetDlgItem(IDC_HORIZONTAL_WHITE_EDIT)->EnableWindow( FALSE );
		GetDlgItem(IDC_HORIZONTAL_WHITE_SPIN)->EnableWindow( FALSE );
		GetDlgItem(IDC_VERTICAL_WHITE_EDIT)->EnableWindow( FALSE );
		GetDlgItem(IDC_VERTICAL_WHITE_SPIN)->EnableWindow( FALSE );
	}	
}

void CPrintIndexPage::OnDeltaposHorizontalWhiteSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	float fval = (float)atof(m_HorizontalWhiteSpace);
	// increment/decrement
	if ( m_WhiteSpace_Inch == M_INCH_USEMM )
	{
		fval += ((float)(-pNMUpDown->iDelta)) * .001F;
		m_HorizontalWhiteSpace.Format("%.3f", fval);
	}
	else
	{
		fval += ((float)(-pNMUpDown->iDelta)) * .01F;
		m_HorizontalWhiteSpace.Format("%.2f", fval);
	}
	
	// apply this      
	UpdateData(FALSE);    

	*pResult = 0;
}

void CPrintIndexPage::OnDeltaposVerticalWhiteSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	float fval = (float)atof(m_VerticalWhiteSpace);
	// increment/decrement
	if ( m_WhiteSpace_Inch == M_INCH_USEMM )
	{
		fval += ((float)(-pNMUpDown->iDelta)) * .001F;
		m_VerticalWhiteSpace.Format("%.3f", fval);
	}
	else
	{
		fval += ((float)(-pNMUpDown->iDelta)) * .01F;
		m_VerticalWhiteSpace.Format("%.2f", fval);
	}
	
	// apply this      
	UpdateData(FALSE);    

	*pResult = 0;
}

void CPrintIndexPage::OnSelchangeWhiteSpaceUnits() 
{
	// TODO: Add your control notification handler code here
	float fval;
	BOOL bOldVal = m_WhiteSpace_Inch;
	UpdateData(TRUE);

	if (bOldVal != M_INCH_USEINCHES && m_WhiteSpace_Inch == M_INCH_USEINCHES)
	{
		fval = (float)atof(m_HorizontalWhiteSpace);
		fval /= 25.4F;
		// put back in
		m_HorizontalWhiteSpace.Format("%.2f", fval );

		fval = (float)atof(m_VerticalWhiteSpace);
		fval /= 25.4F;
		// put back in
		m_VerticalWhiteSpace.Format("%.2f", fval);   

		m_WhiteSpace_Inch = M_INCH_USEINCHES;

		UpdateData(FALSE);
	}
	else if (bOldVal != M_INCH_USEMM && m_WhiteSpace_Inch == M_INCH_USEMM)
	{
		fval = (float)atof(m_HorizontalWhiteSpace);
		fval *= 25.4F;
		// put back in
		m_HorizontalWhiteSpace.Format("%.3f", fval);   

		fval = (float)atof(m_VerticalWhiteSpace);
		fval *= 25.4F;
		// put back in
		m_VerticalWhiteSpace.Format("%.3f", fval);   

		m_WhiteSpace_Inch = M_INCH_USEMM;

		UpdateData(FALSE);
	}	
}

void CPrintIndexPage::OnChangeHorizontalWhiteEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CPrintIndexPage::OnChangeVerticalWhiteEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	
}

void CPrintIndexPage::OnChangeIndexCopies() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CPrintIndexPage::OnDeltaposIndexCopiesSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	int iVal = m_Index_Copies;
	// increment/decrement
	iVal += (pNMUpDown->iDelta);

	if ( iVal < 1 )
		iVal = 1;

	if ( iVal > 99 )
		iVal = 99;

	// put back in
	m_Index_Copies = iVal;

	// apply this      
	UpdateData(FALSE);    

	*pResult = 0;
}

void CPrintIndexPage::OnSelchangeSelectPane() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}
