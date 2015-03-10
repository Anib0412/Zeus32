/////////////////////////////////////////////////////////////////////////////
// AdvancedDevice.h
//                                                                        
// advanced device settings for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

/////////////////////////////////////////////////////////////////////////////
// CAdvancedDevice dialog

#ifndef __ADVANCEDDEVICE_H__
#define __ADVANCEDDEVICE_H__

#include "nformat.h"
#include "devface.h"

class CAdvancedDevice : public CDialog
{
// Construction
public:
	CAdvancedDevice(CWnd* pParent = NULL);   // standard constructor
	int m_memused;
	int m_imagememory_num;

public:    
    BOOL m_def_buffertodisk;
    BOOL m_def_compress;
	BOOL m_def_compress_bitmap;
	BOOL m_def_scale_bitmap;
	BOOL m_canscale_bitmap;
    int m_def_memalloc_style;
    int m_def_aatype;
    int m_def_aaoversample;
	BOOL m_cancompress;	
	BOOL m_canaalias;
	BOOL m_EnableBufferToDisk;

// Dialog Data
	//{{AFX_DATA(CAdvancedDevice)
	enum { IDD = IDD_ADVANCED_DEVICE };
	BOOL	m_buffertodisk;
	BOOL	m_compress;
	CString	m_virtual_memory;
	int		m_memalloc_style;
	CString	m_memused_edit;
	int		m_aatype;
	int		m_aaoversample;
	CString	m_imagememory;
	CString	m_avail_physical;
	BOOL	m_compress_bitmap;
	BOOL	m_scale_bitmap;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvancedDevice)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

    void SetMem();
	int AvailPhysicalMemory;
	int InstalledMemory;
	NumberFormat nm;
    int m_maxmem;	
	void SetBufferToDisk();
	int OriginalMem;
    int OriginalAliasStyle;
    int OriginalAliasLevel;

	// Generated message map functions
	//{{AFX_MSG(CAdvancedDevice)
	virtual BOOL OnInitDialog();
	afx_msg void OnDefault();
	afx_msg void OnDeltaposMemUsedSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeMemAlloc();
	afx_msg void OnSelchangeAatype();
	afx_msg void OnChangeMemUsedEdit();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point); 
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);	
	afx_msg void OnSelchangeAaoversample();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
