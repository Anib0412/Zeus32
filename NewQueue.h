#if !defined(AFX_NEWQUEUE_H__6E9BD48A_E7E0_11D2_8E09_00C04F7975DC__INCLUDED_)
#define AFX_NEWQUEUE_H__6E9BD48A_E7E0_11D2_8E09_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewQueue.h : header file
//

#include "DeviceList.h"

/////////////////////////////////////////////////////////////////////////////
// CNewQueue dialog

class CNewQueue : public CDialog
{
// Construction
public:
	CNewQueue(CWnd* pParent = NULL);   // standard constructor

   CString csDevice;
   int iDevIdx;
   CString csPort;
   BOOL bOnBottom;
   BOOL bMakeCopyIfExist;
   int GetDefault();   
   void PositionWindow();   

   CString csRPDebugStr;
   char cDebugStr[255];

// Dialog Data
	//{{AFX_DATA(CNewQueue)
	enum { IDD = IDD_NEWQUEUE };
	CSpinButtonCtrl	m_hostid_spin;
	int		m_connection;	
	CString	m_name;	
	int		m_device;
	int		m_hostid_edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewQueue)	
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   CDeviceList cdl;
   void MakeDevName();
   int GetInstalledDevices();
   int GetNamedQueues();
   CStringArray csa;
   BOOL IsMatch(CString& cs);
   void LoadConnections();
   int DevIDtoListID(int DevID);   

	// Generated message map functions
	//{{AFX_MSG(CNewQueue)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeDevice();
	afx_msg void OnDestroy();
	afx_msg void OnFinddevice();
	afx_msg void OnDblclkDevice();	
	afx_msg void OnSelchangeConnection();	
	afx_msg void OnScsiExplorer();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWQUEUE_H__6E9BD48A_E7E0_11D2_8E09_00C04F7975DC__INCLUDED_)
