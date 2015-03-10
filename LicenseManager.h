#if !defined(AFX_LICENSEMANAGER_H__E4A6DC43_1B05_4AC2_BA29_B7E4B9B16F4F__INCLUDED_)
#define AFX_LICENSEMANAGER_H__E4A6DC43_1B05_4AC2_BA29_B7E4B9B16F4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LicenseManager.h : header file
//

#ifndef	TTS_BALLOON
#define	TTS_BALLOON		0x40
#endif

//#include "pptooltip.h"
#include "profile.h"

/////////////////////////////////////////////////////////////////////////////
// CLicenseManager dialog

class CLicenseManager : public CDialog
{
// Construction
public:
	CLicenseManager(CWnd* pParent = NULL);   // standard constructor

    void GetStatus(BOOL& bAppActive, BOOL& bWFActive);
    void CheckStatus(int& iActState);
    void AutoRemove();

	BOOL GetActivationCode();
	BOOL GetActivationCode(CString& csActivationCode, CString& csErr);

	CString csDebug;
	CProfile Profile;

//	CPPToolTip ToolTipCtrl;	

// Dialog Data
	//{{AFX_DATA(CLicenseManager)
	enum { IDD = IDD_LICENSE_MANAGER };
	CString	m_daysleft;
	CString	m_email;
	CString	m_mid;
	CString	m_name;
	CString	m_serialnumber;
	CString	m_sitecode;
	CString	m_status;
	CString	m_company;
//	CString	m_workforcestatus;
//	CString	m_cdproducerstatus;
	BOOL	m_nointernet;
	CString	m_activationcode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLicenseManager)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLicenseManager)
	afx_msg void OnActivate();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeSerialnumber();
	afx_msg void OnChangeName();
	afx_msg void OnChangeEmail();
	afx_msg void OnRemove();
	afx_msg void OnNointernet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LICENSEMANAGER_H__E4A6DC43_1B05_4AC2_BA29_B7E4B9B16F4F__INCLUDED_)
