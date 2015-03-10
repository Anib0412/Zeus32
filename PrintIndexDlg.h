#if !defined(AFX_CPrintIndexDlg_H__FF031F90_C65D_46D4_A95B_2F1CB2AEE932__INCLUDED_)
#define AFX_CPrintIndexDlg_H__FF031F90_C65D_46D4_A95B_2F1CB2AEE932__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CPrintIndexDlg.h : header file
//

#include"crip.h"
#include "prev.h"
#include "profile.h"

#include "..\CmnLibs\Current LeadTools\include\l_bitmap.h"
#include "..\CmnLibs\Current LeadTools\include\l_error.h"

/////////////////////////////////////////////////////////////////////////////
// CPrintIndexDlg dialog

class CPrintIndexDlg : public CDialog
{
// Construction
public:
	CPrintIndexDlg(CWnd* pParent = NULL);   // standard constructor

	~CPrintIndexDlg();						// standard destructor
// Dialog Data
	//{{AFX_DATA(CPrintIndexDlg)
	enum { IDD = IDD_PRINT_INDEX };
	CSpinButtonCtrl	m_IndexCopiesSpin;
	int		m_Index_Copies;
	int		m_Media_Size;
	int		m_Select_Pane;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintIndexDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
// Implementation
	void OnRepaint(CString csFileName);

	void AddImage(CQueueFile* pCQueueFile);

	BOOL CheckRipStatus();

	BOOL CalcIndexFileInfo();

	BOOL CreateIndexFile();

//	BOOL AddImageToIndexFile( CQueueFile* pQFile );

	void SetPrintHWnd( HWND );
	void SetPrintIndexInfo(CWnd* pPrWnd, CDevice* pDevice, CTransform* pTransform, CColorTransform* pColorTransform);

	CString GetIndexFileName( int iIndex );
	int GetFileCount();

	// Data
	CString csRPDebugStr;
	char cDebugStr[255];

	BOOL bRipComplete;
	CString csScratchPath;

	int m_Orientation;

private:
	// Data
	BOOL bStartBuildIndex;

	CComboBox* pCBMediaSize;
	CProfile Profile;

	int iTotalMedia;

	MEDIA Media;

	RIPSTRUCT RipStruct;

	// Functions
	BOOL ResizeIndexPrintBitmap(pBITMAPHANDLE pPhotoBitmap, int iWidthInches1000, int iHeightInches1000);

	BOOL WriteIndexPSHeader(CStdioFile& cf, int iMediaWidthPica, int iMediaHeightPica, BOOL bDoEPSFStuff = FALSE);

	BOOL WritePSIndexBitmap(CStdioFile& cf, int iDestWidthPica, int iDestHeightPica, 
									int iDestXOffPica, int iDestYOffPica, BITMAPHANDLE PrintBitmap);

	BOOL WritePSIndexPS(CStdioFile& cf, int iMediaWidthPica, int iMediaHeightPica, int iDestWidthPica, int iDestHeightPica, 
									int iDestXOffPica, int iDestYOffPica, char* pFileName);

	BOOL WriteIndexPSTrailer(CStdioFile& cf);

	CString CStringToPSString(LPCSTR lpStrIn);

	void AddImageDescription( CStdioFile& cf, CString csDescription, int iXOffset, int iYOffset );

	void CalcImageSizeAndLocation(float fCanvasArea, float fIndexPrintArea );

	void SetDescriptionFont();

	BOOL EndIndexFile();

	// Data

	// Calcualte Size and Location
	float fAspectRatio;
	float fWidth;
	float fHeight;

	int iPrintIndexWidth;
	int iPrintIndexHeight;

	int iTotalImageHeight;
	int iTotalImageWidth;

	CString csDescription;

	CStdioFile cf;

	BITMAPHANDLE PrintBitmap;

	BOOL bLandscape;
	BOOL bCFOpen;
	BOOL bAbort;

	CDevice* pDefaultDevice;
	CTransform* pPrintIndexTransform;
	CColorTransform* pCC;

	char pTempFileName[_MAX_PATH];
    
	CObArray cQueueFileArray;
	CStringArray csaFilename;

	CQueueFile* pCQueueFile;

	CRip cr;
	CWnd* pWnd;
	HWND hWnd_PrintPane;

	int iPageNumber;

	int iDestWidthPica;
	int iDestHeightPica;

	float fRipHeight;
	float fRipWidth;

	int iImagesAcross;
	int iImagesDown;

	int	iIndexRow;
	int iIndexCol;

	int iMediaWidth;
	int iMediaHeight;
	int iMediaWidthPica;
	int iMediaHeightPica;

	int iPrintIndexWidth1000;
	int iPrintIndexHeight1000;

	int iImageToProcess;
	int iImagesOnPage;
	int iImagesMaxPageImages;

	int iPrintsToIndex;

	int iXoffInchesPica;
	int iYoffInchesPica;

	int m_UseSinglePage;

	int m_DisplayImageName;
	
	int m_UseImageCount;
	int m_MaxImagesPerPage;

	int m_UseImageSize;
	CString m_ImageSize_Width;
	CString m_ImageSize_Height;
	int m_ImageSize_Transform;

	int m_WhiteSpace;
	CString	m_HorizontalWhiteSpace;
	CString m_VerticalWhiteSpace;
	float fHorizontalWhiteSpace;
	float fVerticalWhiteSpace;
	float fHorizontalWhiteSpace_Pica;
	float fVerticalWhiteSpace_Pica;

	int iTimer;

	void CalcPortraitSize( float fCanvasArea, float fIndexPrintArea, float fExcess = 0 );
	void CalcLandscapeSize( float fCanvasArea, float fIndexPrintArea, float fExcess = 0 );


protected:

	// Generated message map functions
	//{{AFX_MSG(CPrintIndexDlg)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg LRESULT OnPrevSetInfo(WPARAM, LPARAM);
	afx_msg LRESULT OnRipMessage(WPARAM, LPARAM);
	afx_msg LRESULT OnRipComplete(WPARAM, LPARAM);
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnGo();
	afx_msg void OnDeltaposIndexCopiesSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeMediaSize();
	afx_msg void OnSelchangeSelectPane();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CPrintIndexDlg_H__FF031F90_C65D_46D4_A95B_2F1CB2AEE932__INCLUDED_)
