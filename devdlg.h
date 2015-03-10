/////////////////////////////////////////////////////////////////////////////
// DevDlg.h
//                                                                        
// device dialog for RasterPlus95
// Copyright (C) 1996-97 Graphx, Inc.
//

#ifndef __DEVDLG_H__
#define __DEVDLG_H__

#include "device.h"
#include "DeviceList.h"

/////////////////////////////////////////////////////////////////////////////
// devdlg2 dialog

class CDialogDevSetup : public CPropertyPage
{
// Construction
public:
	CDialogDevSetup();
	~CDialogDevSetup();   			
	int SetBin();
	int SetRes();
	int SetConnect();
	int SetPaper();     	     		
	void ShowOK(BOOL);	
	CWnd* pWnd;					
	void ApplyToDevice();
	void SetMultiSelFields(CObArray& cod);
	void DelayedSetDevice(CDevice& cd);
	void SetDevice(CDevice& cd);
	CDevice GetCurrentDevice();	
	int ReportChange;

// Dialog Data
	//{{AFX_DATA(CDialogDevSetup)
	enum { IDD = IDD_DEVICE };
	int		m_device;
	int		m_mediasize;
	int		m_mediasource;
	int		m_copies;
	int		m_resolution;
	int		m_connection;
	int		m_orientation;
	CString	m_pagedims;
	//}}AFX_DATA

protected:
    void UpdateDevice();
	int DevIDtoListID(int DevID);
	int ListIDtoDevID(int ListID);
	void DevToForm();	
	CDevice CurrentDevice;
	BOOL IsDlgInit;
	CDeviceList cdl;
	void OnOK();
	BOOL ShowOKFlag;
	CDevice DelayDevice;
    BOOL DelayDeviceFlag;   		   
	CString csTemp;   

// Implementation		   	

	//{{AFX_VIRTUAL(CDialogDevSetup)
    public:
   	virtual BOOL OnInitDialog();	
	virtual void OnOurOK();
	virtual void OnOurCancel();	
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    //}}AFX_VIRTUAL

protected:	
	// Generated message map functions
	//{{AFX_MSG(CDialogDevSetup)
	afx_msg void OnOptions();
	afx_msg void OnSelchangeDevice();
	afx_msg void OnSelchangeMediaSize();	
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeMediaSource();
    afx_msg void OnClose();
	afx_msg void OnChangeCopies();
	afx_msg void OnReset();
	afx_msg void OnSelchangeConnection();
	afx_msg void OnSelchangeResolution();
	afx_msg void OnAdvanced();
	afx_msg void OnSelchangeOrientationList();		
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point); 
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);		
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif

