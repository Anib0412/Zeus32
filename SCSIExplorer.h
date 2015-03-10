// SCSIExplorer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSCSIExplorer dialog

class CSCSIExplorer : public CDialog
{
// Construction
public:
	CSCSIExplorer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSCSIExplorer)
	enum { IDD = IDD_SCSI_EXPLORER };	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCSIExplorer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   
   void SetDevices();

	// Generated message map functions
	//{{AFX_MSG(CSCSIExplorer)
	virtual BOOL OnInitDialog();
	afx_msg void OnRescan();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
