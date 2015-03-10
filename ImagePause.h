/////////////////////////////////////////////////////////////////////////////
// ImagePause.h
//                                                                        
// dialog to pause imaging for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __IMAGEPAUSE_H__
#define __IMAGEPAUSE_H__

/////////////////////////////////////////////////////////////////////////////
// CImagePause dialog

class CImagePause : public CDialog
{
// Construction
public:
	CImagePause(CWnd* pParent = NULL);   // standard constructor	

// Dialog Data
	//{{AFX_DATA(CImagePause)
	enum { IDD = IDD_IMAGE_PAUSE };
	BOOL	m_stop;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImagePause)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    void MoveDialog();

	// Generated message map functions
	//{{AFX_MSG(CImagePause)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
