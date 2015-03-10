/////////////////////////////////////////////////////////////////////////////
// BHEADER.H
//                                                                        
// BatchHeader class for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef _BHEADER_H_
#define _BHEADER_H_

/////////////////////////////////////////////////////////////////////////////
// bheader dialog

class CBheader : public CDialog
{
// Construction
public:
	CBheader(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBheader)
	enum { IDD = IDD_DIALOG_BHEADER };
	BOOL	m_date;
	BOOL	m_time;
	CString	m_notes1;
	CString	m_notes2;
	CString	m_edit_imagecount;
	BOOL	m_show_imagecount;
	BOOL	m_imagecorners;	
	int		m_background;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CBheader)
	virtual BOOL OnInitDialog();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);	
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define OPTIONS_BATCHHEADER_TIME       0x01
#define OPTIONS_BATCHHEADER_DATE       0x02
#define OPTIONS_BATCHHEADER_COUNT      0x04
#define OPTIONS_BATCHHEADER_CORNERS    0x08
#define OPTIONS_BATCHHEADER_BACKWHITE  0x10
#define OPTIONS_BATCHHEADER_BACKBLACK  0x20
#define OPTIONS_BATCHHEADER_BACKNONE   0x40

#define BATCHHEADER_BACKGROUND_BLACK   0
#define BATCHHEADER_BACKGROUND_WHITE   1	
#define BATCHHEADER_BACKGROUND_NONE	   2
#endif
