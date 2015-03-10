/////////////////////////////////////////////////////////////////////////////
// DevListPage.h
//                                                                        
// manage devices for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __DEVLISTPAGE_H__
#define __DEVLISTPAGE_H__

#include "DeviceList.h"
#include "InstallPrinter.h"

/////////////////////////////////////////////////////////////////////////////
// CDevListPage dialog

class CDevListPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDevListPage)

// Construction
public:
	CDevListPage();
	~CDevListPage();	

// Dialog Data
	//{{AFX_DATA(CDevListPage)
	enum { IDD = IDD_DIALOG_GPREFS_DEVLIST };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDevListPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
    DWORD GetInstalledDevices();
	CDeviceList cdl;	
    void InitDevList();	
	BOOL GetDependentFiles(CString& csFileSetTitle, CObArray& cFileSetArray);		
	void SetOurPort(CDeviceEntry* cde);
	void DestroyInstalledList();
	BOOL IsInstalled();
	BOOL NeedToReboot;	
	CInstallPrinter cii;

	// Generated message map functions
	//{{AFX_MSG(CDevListPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelete();
	afx_msg void OnInstall();
	afx_msg void OnSetfocusDevlistAvailable();
	afx_msg void OnSetfocusDevlistInstalled();
	afx_msg void OnSelchangeDevlistInstalled();
	afx_msg void OnSelchangeDevlistAvailable();
	afx_msg void OnDblclkDevlistAvailable();
	afx_msg void OnProperties();	
	afx_msg void OnDestroy();
	afx_msg void OnControlPanel();
	afx_msg void OnOurok();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point); 
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);		
	afx_msg void OnConnect();
	afx_msg void OnRename();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
