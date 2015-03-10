// rexdirdi.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRexDirDialog dialog

class CRexDirDialog : public CFileDialog
{
// Construction
public:
	CRexDirDialog(const CString& = CString(""),
	            const CString& = CString("Select Directory"),
				CWnd* pParent = NULL);	// standard constructor

	enum {  IDC_ST_DIRS = -1,
			                       
			IDC_PB_READONLY = 1038,                       
			IDC_CB_READONLY	= 1040,
			
	        IDC_ST_FILETYPE = 1089,
			IDC_ST_FILENAME = 1090,
			IDC_ST_DRIVES	= 1091,
			
			IDC_LB_FILENAME = 1120,
			IDC_LB_DIRS	= 1121,
			
			IDC_CL_FILETYPE = 1136,
			IDC_CL_DRIVES   = 1137,
			
			IDC_ED_FILENAME = 1152,
			
			
			IDD = 1536 
			};

// Dialog Data
	//{{AFX_DATA(CRexDirDialog)
			// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	public:
		BOOL m_bInitialUpdate;
		CString m_cstringTitle;
		CString m_cstringPrompt;
		
		CString GetPath(BOOL = FALSE) const;

	void SetTitle(CString csTitle);
	void SetPrompt(CString csPrompt);


// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CRexDirDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	//}}AFX_MSG
	void ResizeWindow(void);
	void MoveChildren( CWnd * pwndParent, int x, int y );
	DECLARE_MESSAGE_MAP()
};
