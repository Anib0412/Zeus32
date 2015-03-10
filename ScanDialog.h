#if !defined(AFX_SCANDIALOG_H__0B16EF67_9601_11D3_81A3_00C04F6B9DE0__INCLUDED_)
#define AFX_SCANDIALOG_H__0B16EF67_9601_11D3_81A3_00C04F6B9DE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScanDialog.h : header file
//
#include "device.h"

#include "..\CmnLibs\Current LeadTools\include\lttwn.h"
#include "..\CmnLibs\Current LeadTools\include\l_bitmap.h"
#include "..\CmnLibs\Current LeadTools\include\l_error.h"
#include "ltwnd.h"

#define QUEUESTYLE_HOLD         0
#define QUEUESTYLE_PRINT        1

#define LINESAMPLES				10

class CImageTrimTracker;

/////////////////////////////////////////////////////////////////////////////
// CScanDialog dialog

class CScanDialog : public CDialog
{
// Construction
public:
	CScanDialog(CWnd* pParent = NULL);	// standard constructor

	~CScanDialog();						// Destructor

	CDevice* pCDev;

// Dialog Data
	//{{AFX_DATA(CScanDialog)
	enum { IDD = IDD_SCAN_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScanDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

public:

	char	fileName[_MAX_PATH];
	int		iQueue;
	int		iMediaIdx;

protected:

	// Generated message map functions
	//{{AFX_MSG(CScanDialog)
	virtual BOOL OnInitDialog();	
	afx_msg void OnRescan();
	afx_msg void OnPrint();
	afx_msg void OnHold();
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg void OnRedisplayOnly();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	
	BITMAPHANDLE hLeadBitmap;
	BITMAPHANDLE hBitmapCropped;
	BITMAPHANDLE hTempBitmap;

	BOOL bFullImage;
	BOOL bReverseSrch;
	BOOL bRightToLeft;

	CImageTrimTracker* pRT;

	COLORREF crBitmapBackground;			// Background used when rotating the Scanned imgae
	
	CWnd* pFullImageWnd;
	CWnd* pCroppedWnd;

	int iLinePoints[LINESAMPLES];

	RECT rWndSize;		/* RECT for sizing the window. */
	RECT rClientSize;	/* RECT for the client area. */
	RECT rLeadDest;		/* RECT for the destination rectangle for painting. */
	RECT rLeadSource;	/* RECT for the source rectangle for painting. */

	RECT rTrimRect;		/* RECT to describe the area to be trimmed */

	HPALETTE hpalPaint; /* Paint palette handle. */

	L_INT DisplayTop;
	L_INT DisplayLeft;
	L_INT DisplayWidth;		/* Width of the displayed image */
	L_INT DisplayHeight;	/* Height of the displayed image */

	void	AddMedia();

	BOOL	AdjustImage();
	BOOL	DeskewImage();
	BOOL	GetTrimRect( pBITMAPHANDLE pBitmap = NULL );
	BOOL	TrimImage();

	void	CScanDialog::CalcAspectRatio( LPRECT rClientRect );

	BOOL	WindowTrimRect( LPRECT rClientRect, LPRECT selectRect  );
	BOOL	ImageTrimRect( LPRECT rClientRect, LPRECT selectRect  );

	int		FindDeskewAngle();
	int		FindEdge( BITMAPHANDLE hDeskewBitmap, COLORREF crBackground,
								int iIncr, int iStartCol, int iEndCol );

	BOOL	MapLinePoints( int iColPoint, int iSampleRow );

	BOOL	IsLine();

	void	CreateTempFileName();

	void	CalcDisplay(L_INT WidthAllowed, L_INT HeightAllowed,
									L_INT WidthFactor, L_INT HeightFactor,
										L_INT *ResultLeft, L_INT *ResultTop,
										L_INT *ResultWidth, L_INT *ResultHeight);

	void	DisplayError( CString csFunctionName, L_INT iErrorNumber );
	void	DisplayError( CString csFunctionName, CString csErrorText );

	void	DisplayScannedImage();
	void	DisplayCroppedImage();
	void	DisplayScannerInfo( pLTWAINPROPERTIES TwainProps );
	BOOL	DoScan();
};

class CImageTrimTracker : public CRectTracker
{
// Construction
public:
	CImageTrimTracker();
	CImageTrimTracker(LPCRECT lpSrcRect, UINT nStyle);
	~CImageTrimTracker();

	int	 nHitValue;
    RECT ClientRect;

private:
	void DrawTrackerRect( LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd );
	void AdjustRect( int nHandle, LPRECT lpRect );

	void ValidateRectLocation();
	void ValidateRectLocation( LPRECT lpRect );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCANDIALOG_H__0B16EF67_9601_11D3_81A3_00C04F6B9DE0__INCLUDED_)
