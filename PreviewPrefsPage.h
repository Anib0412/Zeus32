/////////////////////////////////////////////////////////////////////////////
// PreviewPrefsPage.h
//                                                                        
// preview preferences for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef _PREVIEWPREFSPAGE_H__
#define _PREVIEWPREFSPAGE_H__

/////////////////////////////////////////////////////////////////////////////
// CPreviewPrefsPage dialog

class CPreviewPrefsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPreviewPrefsPage)

// Construction
public:
	CPreviewPrefsPage();
	~CPreviewPrefsPage();	

// Dialog Data
	//{{AFX_DATA(CPreviewPrefsPage)
	enum { IDD = IDD_DIALOG_GPREFS_PREVIEW };
	CSpinButtonCtrl	m_maxpreview_spin;
	int		m_prev_maxopen;	
	BOOL	m_compression;
	int		m_prev_size;
	BOOL	m_pause;
	BOOL	m_largetoolbar;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreviewPrefsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:                    

	// Generated message map functions
	//{{AFX_MSG(CPreviewPrefsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnOurOK();
	afx_msg void OnOurcancel();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point); 
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);		
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
