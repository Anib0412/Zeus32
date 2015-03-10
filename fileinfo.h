/////////////////////////////////////////////////////////////////////////////
// FILEINFO.H
//                                                                        
// local file info for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

/////////////////////////////////////////////////////////////////////////////
// CFileInfo dialog  

#ifndef __FILEINFO_H__
#define __FILEINFO_H__

#include "OurPropertySheet.h"
#include "devdlg.h"
#include "transdlg.h"
#include "cqueue.h"

/////////////////////////////////////////////////////////////////////////////
// CPrefDlgGeneral dialog

class CPrefDlgGeneral : public CPropertyPage
{
// Construction
public:
	CPrefDlgGeneral();
	CString	m_shortfilename;

// Dialog Data
	//{{AFX_DATA(CPrefDlgGeneral)
	enum { IDD = IDD_DIALOG_PREFS_GENERAL };
	CString	m_filedate;
	CString	m_filename;
	CString	m_filesize;
	CString	m_filetime;
	CString	m_description;
	CString	m_boundingbox;		
	CString	m_queuename;
	CString	m_preview_status;
	CString	m_status;
	CString	m_filetype;
	CString	m_pages;
	//}}AFX_DATA
public:
   CQueueFile* pCQueueFile;
   CWnd* pWnd;               // pointer to view class
   
// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CPrefDlgGeneral)		
	afx_msg void OnClose();	
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPrefDlgNotes dialog

class CPrefDlgNotes : public CPropertyPage
{
// Construction
public:
	CPrefDlgNotes();
	CWnd* pWnd;		
    void EnableNotes();
	void DisableNotes();
	int ReportChange;

// Dialog Data
	//{{AFX_DATA(CPrefDlgNotes)
	enum { IDD = IDD_DIALOG_PREFS_NOTES };
	CString	m_notes;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	
	// Generated message map functions
	//{{AFX_MSG(CPrefDlgNotes)		
	afx_msg void OnClose();	
	afx_msg void OnChangeNotes();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CFileInfo dialog

class CFileInfo : public COurPropertySheet
{             
// Construction
public:
	CFileInfo(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = -1);
	CFileInfo(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = -1);
	CPrefDlgGeneral m_dialog_general;	
	CPrefDlgNotes m_dialog_notes;	
	CDialogDevSetup m_dialog_dev_setup;	 
	CDialogTransSetup m_dialog_trans_setup;

public:    
    void MoveDialog();
    HICON hIcon;          
	virtual BOOL OnInitDialog();                        

protected:
    void InitVars();

// Implementation
protected:	
	// Generated message map functions
	//{{AFX_MSG(CFileInfo)	
	afx_msg void OnClose();			
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	
};
#endif                       
