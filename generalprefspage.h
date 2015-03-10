/////////////////////////////////////////////////////////////////////////////
// GeneralPrefsPage.h
//                                                                        
// general preferences for RasterPlus
// Copyright (C) 1996-2000 Graphx, Inc.
//

#ifndef _GeneralPrefsPAGE_H__
#define _GeneralPrefsPAGE_H__

/////////////////////////////////////////////////////////////////////////////
// CGeneralPrefsPage dialog

class CGeneralPrefsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGeneralPrefsPage)

// Construction
public:
	CGeneralPrefsPage();
	~CGeneralPrefsPage();	

// Dialog Data
	//{{AFX_DATA(CGeneralPrefsPage)
	enum { IDD = IDD_DIALOG_GPREFS };
	CSpinButtonCtrl	m_maxpreview_spin;
	int		m_prev_maxopen;	
	int		m_prev_size;
	BOOL	m_pause;
	BOOL	m_largetoolbar;
	BOOL	m_always_open;
	BOOL	m_auto_advance;
	BOOL	m_use_rulers;
	int		m_rulers_units;
	BOOL	m_enableprintserver;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGeneralPrefsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:                    

	// Generated message map functions
	//{{AFX_MSG(CGeneralPrefsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnOurOK();
	afx_msg void OnOurcancel();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point); 
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);		
	afx_msg void OnShowrulers();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
