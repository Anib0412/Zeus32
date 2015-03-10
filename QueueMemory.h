#if !defined(AFX_QUEUEMEMORY_H__C3C0276F_1896_11D3_8E4F_00C04F7975DC__INCLUDED_)
#define AFX_QUEUEMEMORY_H__C3C0276F_1896_11D3_8E4F_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QueueMemory.h : header file
//

#include "nformat.h"
#include "device.h"
#include "QueueCount.h"

/////////////////////////////////////////////////////////////////////////////
// CQueueMemory dialog

class CQueueMemory : public CPropertyPage
{
	DECLARE_DYNCREATE(CQueueMemory)

// Construction
public:
	CQueueMemory();
	~CQueueMemory();
	void SetMem();
	int AvailPhysicalMemory;
	int InstalledMemory;
	NumberFormat nm;
    int m_maxmem;		
	int m_memused;
	int m_imagememory_num;
	BOOL m_bHotFolderReset;
	CDevice* pTargetDevice;
	void SetSpoolSpinState();
	void SetHotFolderButtonState();
	CQueueCount QueueCount;

public:        

// Dialog Data
	//{{AFX_DATA(CQueueMemory)
	enum { IDD = IDD_QUEUEMEMORY };
	CString	m_spool;
	CString	m_virtual_memory;
	CString	m_scratch;
	CString	m_avail_physical;
	CString	m_memused_edit;
	int		m_memalloc_style;
	int		m_diskfiles;
	int		m_listjobs;
	int		m_diskfiles_max;
	int		m_listjobs_max;
	BOOL	m_enable_hotfolder;
	CString	m_hotfolder_dir;
	int		m_hotfolder;
	int		m_hotfolder_max;
	int		m_hotfolder_inpane;
	BOOL	m_image_only_new;
	int		m_hotfolder_delay;
	BOOL	m_bSelect_By_Size;
	int		m_hotfolder_dpi;
	float	m_hotfolder_variance;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CQueueMemory)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CQueueMemory)
	afx_msg void OnSpoolBrowse();
	afx_msg void OnSelchangeMemAlloc();
	virtual BOOL OnInitDialog();
	afx_msg void OnScratchBrowse();
	afx_msg void OnSelchangeListjobsCombo();
	afx_msg void OnSelchangeDiskfilesCombo();
	afx_msg void OnHotfolder();
	afx_msg void OnSelectBySize();
	afx_msg void OnHotfolderdirBrowse();
	afx_msg void OnResetHotFolder();
	afx_msg void OnDeltaposMemUsedSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnChangeMemUsedEdit();
	afx_msg void OnChangeSelectDPI();
	afx_msg void OnChangeVariance();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point); 
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);		
	afx_msg void OnKillfocusHotfolderdirEdit();
	afx_msg void OnChangeHotfolderdirEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUEUEMEMORY_H__C3C0276F_1896_11D3_8E4F_00C04F7975DC__INCLUDED_)
