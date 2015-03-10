#if !defined(AFX_PrintIndexPage_H__06B73D70_4D42_4399_A393_058AE17BF344__INCLUDED_)
#define AFX_PrintIndexPage_H__06B73D70_4D42_4399_A393_058AE17BF344__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintIndexPage.h : header file
//

#include "profile.h"

#define M_INCH_USEINCHES   0
#define M_INCH_USEMM       1

/////////////////////////////////////////////////////////////////////////////
// CPrintIndexPage dialog

class CPrintIndexPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPrintIndexPage)

// Construction
public:
	CPrintIndexPage();   // standard constructor
	~CPrintIndexPage();	 // Standard Destructor

// Dialog Data
	//{{AFX_DATA(CPrintIndexPage)
	enum { IDD = IDD_DIALOG_GPREFS_PRINTINDEX };
	CSpinButtonCtrl	m_IndexCopiesSpin;
	CSpinButtonCtrl	m_Image_WidthSpin;
	CSpinButtonCtrl	m_Image_HeightSpin;
	CSpinButtonCtrl	m_MaxImagesSpin;
	BOOL	m_DisplayImageName;
	int		m_MaxImagesPerPage;
	CString	m_Image_Height;
	CString	m_Image_Width;
	int		m_SinglePage;
	int		m_ImageCount;
	int		m_ImageSize;
	int		m_ImageTransform;
	int		m_ImageSize_Inch;
	int		m_WhiteSpace;
	CString	m_HorizontalWhiteSpace;
	CString	m_VerticalWhiteSpace;
	int		m_WhiteSpace_Inch;
	int		m_Select_Pane;
	int		m_Index_Copies;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintIndexPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	// Data
	CProfile Profile;

	// Functions
	void SetFields();



// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrintIndexPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnOurOK();
	afx_msg void OnOurcancel();
	afx_msg void OnSinglepage();
	afx_msg void OnDisplayImagename();
	afx_msg void OnChangeImageHeightEdit();
	afx_msg void OnChangeImageWidthEdit();
	afx_msg void OnImagecount();
	afx_msg void OnImagesize();
	afx_msg void OnCrop();
	afx_msg void OnFit();
	afx_msg void OnImagesizeCrop();
	afx_msg void OnImagesizeFit();
	afx_msg void OnDeltaposImageWidthSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposImageHeightSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeImagesizeUnits();
	afx_msg void OnWhiteSpace();
	afx_msg void OnDeltaposHorizontalWhiteSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposVerticalWhiteSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeWhiteSpaceUnits();
	afx_msg void OnChangeHorizontalWhiteEdit();
	afx_msg void OnChangeVerticalWhiteEdit();
	afx_msg void OnChangeIndexCopies();
	afx_msg void OnDeltaposIndexCopiesSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeSelectPane();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PrintIndexPage_H__06B73D70_4D42_4399_A393_058AE17BF344__INCLUDED_)
