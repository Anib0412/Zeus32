#if !defined(AFX_CUSTOMPAGE_H__1E895833_D151_11D3_A6FB_00C04F7975DC__INCLUDED_)
#define AFX_CUSTOMPAGE_H__1E895833_D151_11D3_A6FB_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomPage dialog

class CCustomPage : public CDialog
{
// Construction
public:
	CCustomPage(CWnd* pParent = NULL);   // standard constructor
	
	BOOL m_bDoOffsets;
	float m_mediawidth;	
	float m_mediawidthmm;	
	float m_minxoffset;
	float m_minyoffset;
	float m_minxoffsetmm;
	float m_minyoffsetmm;
	BOOL m_margins;

// Dialog Data
	//{{AFX_DATA(CCustomPage)
	enum { IDD = IDD_CUSTOM_PAGE };
	CString	m_height;
	CString	m_width;
	int		m_inch;
	CString	m_xoffset;
	CString	m_yoffset;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCustomPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposWidthSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposHeightSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposXoffsetSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposYoffsetSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeUnits();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMPAGE_H__1E895833_D151_11D3_A6FB_00C04F7975DC__INCLUDED_)
