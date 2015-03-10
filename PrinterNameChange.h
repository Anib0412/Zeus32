// PrinterNameChange.h : header file
//

#ifndef __PRINTERNAMECHANGE_H__
#define __PRINTERNAMECHANGE_H__

/////////////////////////////////////////////////////////////////////////////
// CPrinterNameChange dialog

class CPrinterNameChange : public CDialog
{
// Construction
public:
	CPrinterNameChange(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPrinterNameChange)
	enum { IDD = IDD_PRINTER_NAMECHANGE };
	CString	m_newname;
	CString	m_oldname;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrinterNameChange)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrinterNameChange)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif