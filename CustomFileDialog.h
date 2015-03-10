#if !defined(AFX_CUSTOMFILEDIALOG_H__FA223744_0932_11D3_8E3A_00C04F7975DC__INCLUDED_)
#define AFX_CUSTOMFILEDIALOG_H__FA223744_0932_11D3_8E3A_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomFileDialog.h : header file
//

#include "crip.h"
#include "ripstruct.h"
#include "disp.h"
#include "cqueue.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomFileDialog dialog

class CCustomFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CCustomFileDialog)
public:
    CCustomFileDialog( BOOL bOpenFileDialog, 
			LPCTSTR lpszDefExt = NULL, 
			LPCTSTR lpszFileName = NULL, 
			DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
			LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL );

// Attributes
public:
	//CStringList m_listDisplayNames;    // list of actual items selected in listview
	TCHAR m_szBigBuffer[10000];
	CRip cr;
	void OnRepaint(CString& csFileName);
	CDevice* pDefaultDevice;
	CTransform* pDefaultTransform;
	CColorTransform* pCC;
	float fDevInchWidth;
	CDisp cd;
	BOOL DoPaint;
	CQueueFile* pCQueueFile;
	void PaintIt();
	CStatic * m_pStatic;  
	CString csLastFile;
	CString csNewFile;

// Dialog Data
	//{{AFX_DATA(CCustomFileDialog)
	enum { IDD = IDD_CUSTOM_FILE_DIALOG };
	CListBox	m_wndSelectionsList;
	BOOL	m_bExplorer;
	BOOL	m_bMulti;
	BOOL	m_bTemplate;
	BOOL	m_show_preview;
	//}}AFX_DATA

// Operations
public:
	CString FixedGetNextPathName(POSITION& pos) const;

	virtual int DoModal();
	static BOOL CALLBACK FileDlgHookProc(HWND,UINT,WPARAM,LPARAM);
	static CCustomFileDialog * StaticGetPointer(HWND);
    void OnFileNameChange();
    void OnFolderChange();
	void OnNewFile(LPTSTR lpszFileName);
	TCHAR m_szCurrentFile[MAX_PATH+1]; //For paints

#ifdef _UNICODE
    void SetCurrentFile(const unsigned short* lpszNewFile)
#else
    void SetCurrentFile(LPCTSTR lpszNewFile)
#endif
		{_tcscpy(m_szCurrentFile,lpszNewFile);};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomFileDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL OnFileNameOK();

// Implementation
protected:	
	//{{AFX_MSG(CCustomFileDialog)	
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	virtual BOOL OnInitDialog();	
	afx_msg void OnAddfolder();
	afx_msg LRESULT OnPrevSetInfo(WPARAM, LPARAM);
	afx_msg LRESULT OnRipMessage(WPARAM, LPARAM);
	afx_msg LRESULT OnRipComplete(WPARAM, LPARAM);	
	afx_msg void OnDestroy();
	afx_msg void OnShowpreview();
	//}}AFX_MSG
	afx_msg void OnHelp();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMFILEDIALOG_H__FA223744_0932_11D3_8E3A_00C04F7975DC__INCLUDED_)
