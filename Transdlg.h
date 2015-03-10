/////////////////////////////////////////////////////////////////////////////
// TRANSDLG.H
//                                                                        
// transform dialog for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __TRANSDLG_H__
#define __TRANSDLG_H__
#include "device.h"
#include "transfrm.h"
#include "cqueue.h"
#include "transformlist.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogTransSetup dialog

class CDialogTransSetup : public CPropertyPage
{
// Construction
public:
	CDialogTransSetup();
	~CDialogTransSetup();	
	CTransform DefaultTransform;	
	void ShowOK(BOOL flag);			
	CWnd* pWnd;		
    void SetDefaultTransform(CTransform&);
	void DelayedSetDefaultTransform(CTransform&);
	void SetMultiSelFields(CObArray& cod);
    CTransform& GetDefaultTransform();    
    void ApplyToTransform();
    CQueueFile* pTargetCQueue;    
    CDevice* pTargetDevice;
	void OnOK();
	BOOL ShowOKFlag;
	int ReportChange;

protected:    
	CTransformList ctl;
    CTransform DelayTransform;
    BOOL DelayTransformFlag;
	CString csCustom;
    
// Dialog Data
	//{{AFX_DATA(CDialogTransSetup)
	enum { IDD = IDD_TRANSFORM };
	CString	m_description;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support    
	// Generated message map functions
	//{{AFX_MSG(CDialogTransSetup)
	virtual BOOL OnInitDialog(); 
    virtual void OnOurCancel();
	virtual void OnOurOK();	
	afx_msg void OnSelchangeTransform();
	afx_msg void OnEdit();		
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnReset();	
	afx_msg void OnChangeDescription();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
