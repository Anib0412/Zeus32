// JobProps.h : header file
//

#ifndef __JOBPROPS_H__
#define __JOBPROPS_H__

#include "device.h"
#include "transfrm.h"
#include "cqueue.h"

/////////////////////////////////////////////////////////////////////////////
// CGeneralJobProps dialog

class CGeneralJobProps : public CPropertyPage
{
	DECLARE_DYNCREATE(CGeneralJobProps)

// Construction
public:
	CGeneralJobProps();
	~CGeneralJobProps();
	BOOL bInUpdateItProc;

	BOOL DoingDefaultProps;
	SECBitmapButton secbEditMedia;
	CToolTipCtrl ToolTipCtrl;
	BOOL m_bNotCreated;
	BOOL bPackagePlusInstalled;

	void UpdateIt(BOOL MultipleSelection = FALSE, BOOL IsIniting = FALSE);
	BOOL m_canscale_bitmap;
	BOOL m_cancompress;
	BOOL m_canaalias;  	
		
    CQueueFile* pTargetCQueue;        
	CQueueFile* pCQueueFile;
	CString csCustom;
	CWnd* pReportWnd;
	BOOL IsInit;
	BOOL DeviceIsSet;
	BOOL bIsUsingDefTarg; //This flag will let us know whether we need to create a new device object when something is changed.
    CDevice* pTargetDevice;
	void ReportChange(int ItemChanged);	
	void EnableFileProps(BOOL bEnable);
	void UpdateMediaDims();
	CDevice InternalDevice;
	void SetOurPort(LPCSTR csDev, LPCSTR csPort);
	void SetButtonState(BOOL IsIniting = FALSE);
	CDevice& GetDefDevice();
	void ResetPaperList();
	BOOL IsPackagePlusInstalled();	
	BOOL EnableMediaEdit();
	CString csPackagePlusExe;
	void EditPackageMedia();
	void EditCustomMedia(BOOL bDoOffsets);

protected:
	CString csTemp;	 	
	
public:


// Dialog Data
	//{{AFX_DATA(CGeneralJobProps)
	enum { IDD = IDD_JOBPROPERTIES };
	CString	m_filesize;
	int		m_copies;
	CString	m_filetype;
	CString	m_boundingbox;
	int		m_aaoversample;
	BOOL	m_buffertodisk;
	BOOL	m_compress;
	BOOL	m_compressbitmap;
	BOOL	m_scalebitmap;
	CString	m_filedate;
	CString	m_filetime;
	CString	m_description;
	CString	m_filename;
	CString	m_filepages;
	int		m_resolution;
	int		m_mediasize;
	int		m_connection;
	CString	m_pagedims;
	int		m_orientation;
	CString	m_pagedesc;
	BOOL	m_aatype;
	CString	m_prange_edit;
	BOOL	m_prange;
	int		m_bitmapsampling;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGeneralJobProps)
	public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGeneralJobProps)
	virtual BOOL OnInitDialog();		
	afx_msg void OnBuffertodisk();
	afx_msg void OnCompress();
	afx_msg void OnCompressBitmap();
	afx_msg void OnScaleBitmap();
	afx_msg void OnSelchangeResolution();
	afx_msg void OnSelchangeMediaSize();
	afx_msg void OnChangeCopiesEdit();
	afx_msg void OnSelchangeConnection();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point); 
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);		
	afx_msg void OnSelchangeOrientation();
	afx_msg void OnUpdateDescription();
	afx_msg void OnSelchangeAaoversample();
	afx_msg void OnEditMedia();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnAatype();
	afx_msg void OnPrange();
	afx_msg void OnUpdatePrangeEdit();
	afx_msg void OnSelchangeBitmapsampling();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CDeviceJobProps dialog

class CDeviceJobProps : public CPropertyPage
{
	DECLARE_DYNCREATE(CDeviceJobProps)

// Construction
public:
	CDeviceJobProps();
	~CDeviceJobProps();

	CWnd* pReportWnd;
	void UpdateIt(BOOL MultipleSelection = FALSE);		
	//void ResetWindow(CDevice* pNewDevice);
	void ResetWindow(CDevice* pOldDevice, CDevice* pNewDevice, HWND hWnd);
	CDevice* pTargetDevice;	
	BOOL DoingDefaultProps;
	BOOL DeviceIsSet;
	BOOL IsInit;

// Dialog Data
	//{{AFX_DATA(CDeviceJobProps)
	enum { IDD = IDD_JOBDEVPROPERTIES };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDeviceJobProps)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDeviceJobProps)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CColorJobProps dialog

class CColorJobProps : public CPropertyPage
{
	DECLARE_DYNCREATE(CColorJobProps)

// Construction
public:
	CColorJobProps();
	~CColorJobProps();
	CWnd* pReportWnd;
	void UpdateIt(BOOL MultipleSelection = FALSE);
	CDevice* pTargetDevice;

// Dialog Data
	//{{AFX_DATA(CColorJobProps)
	enum { IDD = IDD_JOBCLRPROPERTIES };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CColorJobProps)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();	
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CColorJobProps)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif // __JOBPROPS_H__
