// JobPropsSheet.h : header file
//
// CJobPropsSheet is a modeless property sheet that is 
// created once and not destroyed until the application
// closes.  It is initialized and controlled from
// CPropertyFrame.
 
#ifndef __JobPropsSheet_H__
#define __JobPropsSheet_H__

#include "JobProps.h"
#include "visualxf.h"
#include "..\icc\ColorManage\AdjustDialog.h"
#include "..\icc\ColorManage\CMMDialog.h"
#include "QueueMemory.h"

/////////////////////////////////////////////////////////////////////////////
// CJobPropsSheet

class CJobPropsSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CJobPropsSheet)

// Construction
public:
	CJobPropsSheet(BOOL dp = FALSE, CWnd* pWndParent = NULL, CString csTitle = "");

// Attributes
public:
	CGeneralJobProps m_general;
	CDeviceJobProps m_device;	
	CJobSizeProps m_size;
	CCMMDialog m_cmmdialog;
	CAdjustDialog m_coloradjustdialog;
	BOOL DoingDefaultProps;
	CQueueMemory m_memory;
	CString csDefaultTitle;
	CDevice TargetDevice;
	CWnd* pReportWnd;
	BOOL *pbDeleted;
	BOOL bDeleted;

// Operations
	void SetOurActivePage();
	void StoreOurActivePage();
	void FrameShow(BOOL bShow);

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJobPropsSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CJobPropsSheet();
	 virtual void PostNcDestroy();

// Generated message map functions
protected:
	//{{AFX_MSG(CJobPropsSheet)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __JobPropsSheet_H__
