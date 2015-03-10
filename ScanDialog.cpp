// ScanDialog.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "zeus.h"
#include "ScanDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 
//#define PI_VALUE  3.141592654
#define PI_VALUE  3.1416

L_INT FAR EXPORT ScanJobCallBack (pBITMAPHANDLE pBitmap, L_VOID FAR * pUserData);

/////////////////////////////////////////////////////////////////////////////
// CScanDialog dialog


CScanDialog::CScanDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CScanDialog::IDD, pParent)
{
	RECT Rect;   

	// get rect    
	Rect.left = 10;
	Rect.top = 10;
	Rect.right = 10;
	Rect.bottom = 10;            

	//{{AFX_DATA_INIT(CScanDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	bFullImage = FALSE;

#ifdef _DEBUG
	memset((void*)&hTempBitmap, 0, sizeof BITMAPHANDLE);
#endif
	memset((void*)&hLeadBitmap, 0, sizeof BITMAPHANDLE);

	pRT = new CImageTrimTracker(&Rect, CRectTracker::dottedLine | CRectTracker::resizeInside);	                            	      	   

}


/*********************************************************************

  Clean-up before the dialog goes away.

*********************************************************************/
CScanDialog::~CScanDialog()
{
	delete pRT;

	if (hLeadBitmap.Flags.Allocated)
		L_FreeBitmap( &hLeadBitmap );

#ifdef _DEBUG
	if (hTempBitmap.Flags.Allocated)
		L_FreeBitmap( &hTempBitmap );
#endif

	if (hBitmapCropped.Flags.Allocated)
		L_FreeBitmap( &hBitmapCropped );


	L_TwainCloseSession( GetSafeHwnd() );
}


void CScanDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScanDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CScanDialog::OnInitDialog() 
{

	bRightToLeft = FALSE;

#ifndef _DEBUG
	// This button is for debug purposes ONLY
	((CButton*)GetDlgItem(ID_REDISPLAY_ONLY))->ShowWindow(FALSE);
#endif
	
	pFullImageWnd	= GetDlgItem(IDC_FULL_IMAGE_PANE);
	pCroppedWnd		= GetDlgItem(IDC_CROPPED_IMAGE_PANE);

	AddMedia();

	UpdateData();

	DoScan();

	return TRUE;
}

BEGIN_MESSAGE_MAP(CScanDialog, CDialog)
	//{{AFX_MSG_MAP(CScanDialog)
	ON_BN_CLICKED(ID_RESCAN, OnRescan)
	ON_BN_CLICKED(ID_PRINT, OnPrint)
	ON_BN_CLICKED(ID_HOLD, OnHold)
	ON_WM_PAINT()
	ON_BN_CLICKED(ID_REDISPLAY_ONLY, OnRedisplayOnly)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScanDialog message handlers
BOOL CScanDialog::DoScan() 
{
	CString csTitle;
	CString csFileName;	

	TWAINCALLBACK pfnScanJob;
	TWAINPROPERTIES   TwainProps;

	HWND	hWnd = GetSafeHwnd();

	L_INT nRet;

	// load wait cursor
	AfxGetApp()->DoWaitCursor(1);

	// set the app name to LEADTOOLS demo
	L_TwainGetProps(&TwainProps);

	lstrcpy(TwainProps.AppProdName, "RasterPlus");

	TwainProps.nXPos = 0;
	TwainProps.nYPos = 0;
	TwainProps.nWidth = 8.5 * 1440;	// Size in TWIPS
	TwainProps.nHeight = 11 * 1440;	// Size in TWIPS

	UINT OldErrorMode = ::SetErrorMode(SEM_NOOPENFILEERRORBOX);
	nRet = L_TwainSetProps(hWnd, &TwainProps, 0);
	::SetErrorMode(OldErrorMode);

	// problem?
	if (nRet != SUCCESS) 
	   {
	   AfxGetApp()->DoWaitCursor(-1);
	   OnCancel();
	   return FALSE;
	   }
	   

//	DisplayScannerInfo( &TwainProps );

	pfnScanJob = (TWAINCALLBACK) MakeProcInstance ((FARPROC) ScanJobCallBack, hInst);
	
	nRet = L_TwainAcquireExt (hWnd,
								&hLeadBitmap,
//								TWAIN_SHOWUI | TWAIN_SHOWSTATUS,
								TWAIN_SHOWSTATUS,
								pfnScanJob,
								NULL);

	if ( nRet == SUCCESS )
	{
#ifdef _DEBUG
		// Save a copy so it can be redisplayed if needed
		// ReDisplay is a DEBUG only function
#ifdef LEADTOOLS_13
		L_CopyBitmap( &hTempBitmap, &hLeadBitmap );
#else
		L_CopyBitmap( &hTempBitmap, &hLeadBitmap, sizeof(BITMAPHANDLE) );
#endif
#endif
		AdjustImage();

		Invalidate();
		UpdateWindow();
	}
	else
	{
		AfxMessageBox( IDS_TWAIN_ACQUIRE_ERROR, MB_OK );
		AfxGetApp()->DoWaitCursor(-1);
		OnCancel();
		return FALSE;
	}

	//FreeProcInstance ((FARPROC) pfnScanJob);
	   

	// reset cursor
	AfxGetApp()->DoWaitCursor(-1);
/*
	if (nRet != SUCCESS)
	{
//		L_FileError(hWnd, nRet, (LPSTR) "RasterPlus Scan Job", "");
		AfxMessageBox( "LeadTools Scanning Error" );
		return FALSE;
	}
*/
	return TRUE;
}

void CScanDialog::CreateTempFileName()
{
	CString csScanPath;

	csScanPath = ((CZeusApp*)AfxGetApp())->GetAppPath();

	csScanPath += "Scan\\";
	::CreateDirectory( csScanPath, NULL );

	GetTempFileName( csScanPath, "Scn", 0, fileName );

	return;
}


L_INT FAR EXPORT ScanJobCallBack(pBITMAPHANDLE pBitmap, L_VOID FAR * pUserData)
{
	return(SUCCESS);
}


void CScanDialog::OnRescan() 
{
//	TWAINPROPERTIES   TwainProps;
//	HWND	hWnd = GetSafeHwnd();

	// TODO: Add your control notification handler code here
	if (hLeadBitmap.Flags.Allocated)
		L_FreeBitmap( &hLeadBitmap );

#ifdef _DEBUG
	if (hTempBitmap.Flags.Allocated)
		L_FreeBitmap( &hTempBitmap );
#endif

/*
	L_TwainGetProps(&TwainProps);
	lstrcpy(TwainProps.AppProdName, "RasterPlus");

	TwainProps.nXPos = -1;
	TwainProps.nYPos = -1;
	TwainProps.nWidth = -1;
	TwainProps.nHeight = -1;

	L_TwainSetProps(hWnd, &TwainProps, 0);
*/
	DoScan();
	
}

void CScanDialog::OnPrint() 
{
	CreateTempFileName();

	// TODO: Add your control notification handler code here
	L_SaveBitmap( fileName, &hBitmapCropped, FILE_TIFLZW, 24, 0, NULL );

	iQueue = QUEUESTYLE_PRINT;

	iMediaIdx = ((CComboBox*)GetDlgItem(IDC_OUTPUT_SIZE))->GetCurSel();

	if (hLeadBitmap.Flags.Allocated)
		L_FreeBitmap( &hLeadBitmap );

	CDialog::OnOK();
}

void CScanDialog::OnHold() 
{
	char *pDest;

	CString csFileName;

	CreateTempFileName();

	csFileName = fileName;

	pDest = strrchr( fileName, '.' );
	strcpy( pDest, ".TIF" );

	// TODO: Add your control notification handler code here
	L_SaveBitmap( fileName, &hBitmapCropped, FILE_TIFLZW, 24, 0, NULL );

	iQueue = QUEUESTYLE_HOLD;

	iMediaIdx = ((CComboBox*)GetDlgItem(IDC_OUTPUT_SIZE))->GetCurSel();

	if (hLeadBitmap.Flags.Allocated)
		L_FreeBitmap( &hLeadBitmap );

	DeleteFile( csFileName );

	CDialog::OnOK();
}

void CScanDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	if (hLeadBitmap.Flags.Allocated)
		L_FreeBitmap( &hLeadBitmap );

	CDialog::OnCancel();
}

void CScanDialog::DisplayScannedImage() 
{
	HPALETTE hPalette = NULL; /* Temporary copy of the current system palette */

	CPaintDC dc( pFullImageWnd ); // device context for painting
	
	// TODO: Add your message handler code here
	pFullImageWnd->GetWindowRect(&rWndSize);
	pFullImageWnd->GetClientRect(&rClientSize);

	rLeadDest = rClientSize;

	CalcAspectRatio( &rLeadDest );

	/* Set the source rectangle to use the whole bitmap */
	SetRect(&rLeadSource, 0, 0, hLeadBitmap.Width, hLeadBitmap.Height);

	/* Get the handle to the device context */
	if (hLeadBitmap.Flags.Allocated) /* Do we have an image? */
	{
		if (hpalPaint) /* If we have a paint palette, select it */
		{
			hPalette = SelectPalette (dc, hpalPaint, FALSE);
			/* Uncomment this if you do not process WM_QUERYNEWPALETTE */
			/* RealizePalette (hdc); */
		}
		/* Paint the image */
		L_PaintDC (dc,
					&hLeadBitmap,
					&rLeadSource,	/* Source rectangle */
					NULL,			/* Default source clip area */
					&rLeadDest,		/* Destination rectangle */
					NULL,			/* Dest clip set by WM_PAINT */
					SRCCOPY);		/* Normal Paint */

		pRT->Draw(&dc);
	}
}

void CScanDialog::DisplayCroppedImage() 
{
	HPALETTE hPalette = NULL; /* Temporary copy of the current system palette */
	
	CPaintDC dc(pCroppedWnd); // device context for painting
	
	// TODO: Add your message handler code here
	pCroppedWnd->GetWindowRect(&rWndSize);
	pCroppedWnd->GetClientRect(&rClientSize);

	/* Make the destination rectangle for painting the same as the client area */
	rLeadDest = rClientSize;

	CalcAspectRatio( &rLeadDest );
	
	/* Set the source rectangle to use the whole bitmap */
	SetRect(&rLeadSource, 0, 0, hLeadBitmap.Width, hLeadBitmap.Height);

	/* Get the handle to the device context */
	if (hBitmapCropped.Flags.Allocated) /* Do we have an image? */
	{
		if (hpalPaint) /* If we have a paint palette, select it */
		{
			hPalette = SelectPalette (dc, hpalPaint, FALSE);
			/* Uncomment this if you do not process WM_QUERYNEWPALETTE */
			/* RealizePalette (hdc); */
		}
		/* Paint the image */
		L_PaintDC (dc,
					&hBitmapCropped,
					&rLeadSource,	/* Source rectangle */
					NULL,			/* Default source clip area */
					&rLeadDest,		/* Destination rectangle */
					NULL,			/* Dest clip set by WM_PAINT */
					SRCCOPY);		/* Normal Paint */
	}
}


void CScanDialog::OnPaint() 
{
	CDialog::OnPaint();

	DisplayCroppedImage();

	DisplayScannedImage();

	CPaintDC dc(this); // keeps windows happy    

	// Do not call CDialog::OnPaint() for painting messages
}

void CScanDialog::DisplayError( CString csFunctionName, L_INT nErrorCode ) 
{
	CString csErrorMessage;
	char szMessage[50];

	csErrorMessage.Empty();

	switch ( nErrorCode )
	{
	case ERROR_IMAGE_EMPTY:
		
		csErrorMessage += "Attempt to acquire TWAIN image failed.\n\nNo image was returned";
		break;

	default:	// All other cases determine if the slope is continuing
		szMessage[0] = NULL;
		wsprintf(&szMessage[lstrlen(szMessage)], "ErrorCode = %d\n", nErrorCode );
		csErrorMessage += szMessage;
		break;
	}

#ifdef _DEBUG
	csErrorMessage += "\n( Function = ";
	csErrorMessage += csFunctionName;
	csErrorMessage += " )";
#endif

	AfxMessageBox( csErrorMessage, MB_OK );
}

void CScanDialog::DisplayError( CString csFunctionName, CString csErrorText) 
{
	CString csErrorMessage;

	csErrorMessage.Empty();
	csErrorMessage += csErrorText;

#ifdef _DEBUG
	csErrorMessage += "\n( ";
	csErrorMessage += csFunctionName;
	csErrorMessage += " )";
#endif
	
	AfxMessageBox( csErrorMessage, MB_OK );
}

void CScanDialog::CalcAspectRatio( LPRECT rClientRect )
{
	CRect rRectSize;

	rRectSize = rClientRect;

	CalcDisplay ( rRectSize.right,				/* Width allowed */
					rRectSize.bottom,			/* Height allowed */
					BITMAPWIDTH(&hLeadBitmap),	/* Width factor, for aspect ratio */
					BITMAPHEIGHT(&hLeadBitmap),	/* Height factor, for aspect ratio */
					NULL,						/* Resulting left value, not used */
					NULL,						/* Resulting top value, not used */
					&DisplayWidth,				/* Resulting width value */
					&DisplayHeight);			/* Resulting height value */
	
	/* Make the destination rectangle for painting the same as the client area */
	rClientRect->right	= min( (rRectSize.right - rRectSize.left),
									( rWndSize.right - rWndSize.left + DisplayWidth - rRectSize.right ));
	rClientRect->bottom	= min( (rRectSize.bottom - rRectSize.top ),
									( rWndSize.bottom - rWndSize.top + DisplayHeight - rRectSize.bottom ));

	return;
}

/***************************************************************************

  Convert the Trim Rect from image coordinates to window coordinates

***************************************************************************/
BOOL CScanDialog::WindowTrimRect( LPRECT rClientArea, LPRECT rSelectedRect )
{
	// Need to Calculate the proper co-ordinates for the TrackerRectangle.
	float fWidthRatio	= (float)rClientArea->right		/ (float)BITMAPWIDTH(&hLeadBitmap);
	float fHeightRatio	= (float)rClientArea->bottom	/ (float)BITMAPHEIGHT(&hLeadBitmap);

	pRT->m_rect.left	= (int)((rSelectedRect->left	* fWidthRatio)	+ 0.5);
	pRT->m_rect.right	= (int)((rSelectedRect->right	* fWidthRatio)	+ 0.5);
	pRT->m_rect.top		= (int)((rSelectedRect->top		* fHeightRatio) + 0.5);
	pRT->m_rect.bottom	= (int)((rSelectedRect->bottom	* fHeightRatio) + 0.5);
	
	return TRUE;
}

/***************************************************************************

  Convert the Tracker Rect from window coordinates to image coordinates

***************************************************************************/
BOOL CScanDialog::ImageTrimRect( LPRECT rClientArea, LPRECT rSelectedRect )
{
	// Need to Calculate the proper co-ordinates for the TrackerRectangle.
	float fWidthRatio	= (float)BITMAPWIDTH(&hLeadBitmap) / (float)rClientArea->right;
	float fHeightRatio	= (float)BITMAPHEIGHT(&hLeadBitmap) / (float)rClientArea->bottom;

	rTrimRect.left		= (int)((rSelectedRect->left	* fWidthRatio)	+ 0.5);
	rTrimRect.right		= (int)((rSelectedRect->right	* fWidthRatio)	+ 0.5);
	rTrimRect.top		= (int)((rSelectedRect->top		* fHeightRatio) + 0.5);
	rTrimRect.bottom	= (int)((rSelectedRect->bottom	* fHeightRatio) + 0.5);
	
	return TRUE;
}

/*******************************************************************************************

	Parameters: WidthAllowed Maximum width
				HeightAllowed Maximum height
				WidthFactor For preserving aspect ratio, usually bitmap width
				HeightFactor For preserving aspect ratio, usually bitmap height
				*ResultLeft Pass NULL if you do not care about centering
	
				Otherwise, this is updated with the X offset
		*ResultTop Pass NULL if you do not care about centering
		Otherwise, this is updated with the Y offset
		*ResultWidth Address of the width variable to update
		*ResultHeight Address of the height variable to update

	Notes: Use this function to fit a displayed image in a particular
			space, while preserving the aspect ratio.
/*******************************************************************************************/

void CScanDialog::CalcDisplay(L_INT WidthAllowed, L_INT HeightAllowed,
									L_INT WidthFactor, L_INT HeightFactor,
										L_INT *ResultLeft, L_INT *ResultTop,
										L_INT *ResultWidth, L_INT *ResultHeight)
{
	/* Local variables for calculating results */
	L_INT Left, Top, Width, Height;
	/* See if using the maximum width will make the image too tall */

	if(MulDiv(WidthAllowed, HeightFactor, WidthFactor) < HeightAllowed)
	{ /* Use the maximum width, and calculate the height and top values */
		Left = 0;
		Width = WidthAllowed;
		Height = MulDiv(Width, HeightFactor, WidthFactor);
		Top = (HeightAllowed - Height) / 2;
	}
	else
	{ /* Use the maximum height, and calculate the width and left values */
		Top = 0;
		Height = HeightAllowed;
		Width = MulDiv(Height, WidthFactor, HeightFactor);
		Left = (WidthAllowed - Width) / 2;
	}


	/* Update the top and left results, if the caller did not pass NULL */
	if (ResultTop != NULL)
		*ResultTop = Top;

	if (ResultLeft != NULL)
		*ResultLeft = Left;

	/* Update the width and height results */
	*ResultWidth = Width;
	*ResultHeight = Height;
	
	return;
}

#ifdef LEADTOOLS_13
void CScanDialog::DisplayScannerInfo( pTWAINPROPERTIES TwainProps )
#else		// LeadTools v15
void CScanDialog::DisplayScannerInfo( pLTWAINPROPERTIES TwainProps )
#endif
{
	L_CHAR szMessage[300];

	wsprintf (szMessage, "AppManufacturer = %s\n",
									(L_CHAR FAR *) TwainProps->AppManufacturer);
	wsprintf (&szMessage[lstrlen(szMessage)], "AppProdFamily = %s\n",
									(L_CHAR FAR *) TwainProps->AppProdFamily);
	wsprintf (&szMessage[lstrlen(szMessage)], "AppProdName = %s\n",
									(L_CHAR FAR *) TwainProps->AppProdName);
	wsprintf (&szMessage[lstrlen(szMessage)], "AppSourceName = %s\n",
									(L_CHAR FAR *) TwainProps->SourceName);
	wsprintf (&szMessage[lstrlen(szMessage)], "nMaxPages = %d\n", TwainProps->nMaxPages);
	wsprintf (&szMessage[lstrlen(szMessage)], "nXPos = %d\n", TwainProps->nXPos);
	wsprintf (&szMessage[lstrlen(szMessage)], "nYPos = %d\n", TwainProps->nYPos);
	wsprintf (&szMessage[lstrlen(szMessage)], "nWidth = %d\n", TwainProps->nWidth);
	wsprintf (&szMessage[lstrlen(szMessage)], "nHeight = %d\n", TwainProps->nHeight);
	wsprintf (&szMessage[lstrlen(szMessage)], "nBits = %d\n", TwainProps->nBits);
	wsprintf (&szMessage[lstrlen(szMessage)], "nPixelType = %d\n", TwainProps->nPixelType);
	wsprintf (&szMessage[lstrlen(szMessage)], "nRes = %d\n", TwainProps->nRes);

	AfxMessageBox( szMessage );

	return; 
}


BOOL CScanDialog::AdjustImage()
{
	CRect rClientRect;

	BOOL nRet = SUCCESS;

	nRet = DeskewImage();

	if ( GetTrimRect() )
	{
		pFullImageWnd->GetClientRect(&rClientRect);
		pFullImageWnd->GetWindowRect(&rWndSize);

		CalcAspectRatio( &rClientRect );

		WindowTrimRect(&rClientRect, &rTrimRect );
		TrimImage();
	}

	return nRet;
}

/**********************************************************

  DeskewImage()

	This function calls the Lead Tools rotation function
	to re-orient the scanned image.

**********************************************************/
BOOL CScanDialog::DeskewImage()
{
	BOOL nRet = SUCCESS;

	int	iDeskewAngle;

	if ( GetTrimRect( &hLeadBitmap ) == SUCCESS )
	{
		iDeskewAngle = FindDeskewAngle();

		nRet = L_RotateBitmap(&hLeadBitmap, iDeskewAngle, 0, crBitmapBackground );
		if ( nRet != SUCCESS )
			DisplayError( "L_RotateBitmap", nRet );
	}

	return nRet;
}

/**********************************************************

  FindDeskewAngle()

	This function determines the background color and
	then call the FindEdge() function to determine the
	deskew rotation angle.

**********************************************************/
int CScanDialog::FindDeskewAngle()
{
	BOOL nRet = SUCCESS;

	BOOL bBackGroundSet;
	BOOL bColDone;
	BOOL bRowDone;

	BITMAPHANDLE hDeskewBitmap;

	COLORREF	PixelColor = 0;
	COLORREF	crBackGroundColor;
	COLORREF	ColorRef_Black	= 0x00000000;
	COLORREF	ColorRef_White	= 0x00ffffff;

	int iDeskewAngle = 0;

	int nSampleIncr; // Sample increment for determining an 'edge' or straight line

#ifdef LEADTOOLS_13
	L_CopyBitmap( &hDeskewBitmap, &hLeadBitmap );
#else		//		LeadTools v15
	L_CopyBitmap( &hDeskewBitmap, &hLeadBitmap, sizeof(BITMAPHANDLE) );
#endif
	
	L_INT			nRow;				/* Row counter */
	L_INT			nCol;				/* Column counter */
	L_INT			nMaxColumns;		/* Number of columns to search */
	L_INT			nColVerify;			/* Column counter (Verify Background Loop)*/
	L_INT			nBitmapWidth = BITMAPWIDTH(&hDeskewBitmap);
	L_INT			nBitmapHeight = BITMAPHEIGHT(&hDeskewBitmap);

	// Black
	L_UINT uiLow = 110;
	L_UINT uiHigh = 255;
	L_IntensityDetectBitmap(&hDeskewBitmap, uiLow, uiHigh );

	GetTrimRect( &hDeskewBitmap );

	/* Process each row in the bitmap */
	L_AccessBitmap(&hDeskewBitmap);

	// Find the Background Color to use as a base
	//
	// Number of Columns to search for a background Color
	// Use 5 less since 5 is the number of columns
	// checked to Verify a consistent color
	//
	nMaxColumns = (rTrimRect.right - rTrimRect.left) - 5;

	bRowDone = FALSE;

	nRow = rTrimRect.top;
	nSampleIncr = 1;

	while ( !bRowDone )
	{
		bColDone = FALSE;
		nCol = rTrimRect.left;

		while ( !bColDone )
		{
			PixelColor	= L_GetPixelColor( &hDeskewBitmap, nRow, nCol );
			if ( PixelColor == ColorRef_Black ||
						PixelColor == ColorRef_White )
			{
				crBackGroundColor = PixelColor;

				// Check the next five Columns to be sure
				//
				bBackGroundSet = TRUE;

				for ( nColVerify = nCol; nColVerify < (nCol + 5 ); nColVerify++ )
				{
					PixelColor	= L_GetPixelColor( &hDeskewBitmap, nRow, nCol );
					if ( PixelColor != crBackGroundColor )
						bBackGroundSet = FALSE;
				}

				if ( bBackGroundSet )
				{
					// The intensity function values reverse the background color
					// Set the Scan Image background to be used in the 
					// L_RotateBitmap function
					//
					if ( crBackGroundColor == ColorRef_Black )
					{
						crBitmapBackground = ColorRef_White;
//						crBackGroundColor  = ColorRef_White;
					}
					else
					{
						crBitmapBackground = ColorRef_Black;
//						crBackGroundColor  = ColorRef_Black;
					}

					bColDone = TRUE;
					bRowDone = TRUE;
				}
			}
			nCol++;
			if ( nCol > nMaxColumns ) 
				bColDone = TRUE;
		}
		nRow++;
		if ( nRow > rTrimRect.bottom )
			bRowDone = TRUE;
	}
	
	// If we found a background color then 
	// Search for an Edge line:
	//				Row    - Top to Bottom
	//				Column - Left to Right
	// Rotate the image and then look for
	// an Edge line doing a Reverse Search
	//				Row    - Top to Bottom
	//				Column - Right to Left ( Reverse Search )
	// Keep processing in this manner until 
	// no angle is detected ( iAngle = 0 ) or
	// Ten attempts have been made
	// If an edge is still not found then
	// no Rotation will be applied 
	//
	iDeskewAngle = 0;

	if ( bBackGroundSet )
	{
		int iAdjustAngle;
		int iFindCnt = 0;
		int iAngle = -1;

		// Get the angle until it reaches zero...
		// The iDeskewAngle is the sum of the
		// intermediate(iAngle) angles found
		//
		while ( iAngle != 0 )
		{
			iFindCnt++;

			bReverseSrch = FALSE;
			GetTrimRect( &hDeskewBitmap );
			iAngle = FindEdge( hDeskewBitmap, crBackGroundColor, nSampleIncr, rTrimRect.left, rTrimRect.right );

			iAdjustAngle = iAngle;
			if ( iAngle > 4500 )
				iAdjustAngle = (9000 - iAngle);

			if ( bRightToLeft )
				iAdjustAngle = iAdjustAngle * -1;

			L_RotateBitmap(&hDeskewBitmap, iAdjustAngle, 0, crBackGroundColor );
			iDeskewAngle += iAdjustAngle;

			// Do a Reverse Search
			bReverseSrch = TRUE;
			GetTrimRect( &hDeskewBitmap );
			iAngle = FindEdge( hDeskewBitmap, crBackGroundColor, nSampleIncr, rTrimRect.right, rTrimRect.left );
			
			iAdjustAngle = iAngle;
			if ( iAngle > 4500 )
				iAdjustAngle = (9000 - iAngle);

			if ( !bRightToLeft )
				iAdjustAngle = iAdjustAngle * -1;

			L_RotateBitmap(&hDeskewBitmap, iAdjustAngle, 0, crBackGroundColor );
			iDeskewAngle += iAdjustAngle;

			// Try to calculate the rotation angle a Max of 10 times
			// After that assume it is in a loop ... get out and
			// DO NOT rotate the image
			//
			if ( iFindCnt > 10 )
				iDeskewAngle = iAngle = 0;
		}

		// Limit the maximum correction to a 90.00F
		//
		if ( abs(iDeskewAngle) > 9000 )
		{
			int iAngleSign = int(iDeskewAngle / abs(iDeskewAngle));
			while ( fabs(iDeskewAngle) > 9000 )
				iDeskewAngle = abs(iDeskewAngle) - 9000;

			iDeskewAngle = iDeskewAngle * iAngleSign;
		}
	}

	L_ReleaseBitmap(&hDeskewBitmap);

	if (hDeskewBitmap.Flags.Allocated)
		L_FreeBitmap( &hDeskewBitmap );

	// Reset the BackGround Color
	crBitmapBackground = ColorRef_Black;

	return iDeskewAngle;
}


/**********************************************************

  FindEdge()

	This function is called to located an Edge for the
	purpose of calculating a deskew rotation angle.

    The direction of the column search along the row
	is determined by the relative value of the Start
	and End columns.

**********************************************************/
int CScanDialog::FindEdge( BITMAPHANDLE hDeskewBitmap, COLORREF crBackground,
											int iIncr, int iStartCol, int iEndCol )
{
	BOOL bSampleCols;
	BOOL bIsALine;
	BOOL bColDone;
	BOOL bRowDone;

	COLORREF	PixelColor = 0;
	COLORREF	crReferenceColor;

	float fAngle;
	float fRun;
	float fRise;
	float fRadians;
		
	int iAngle;
	int iFirstRow;
	int iLastRow;

	int iLinePoint;

	int iSampleRows;

	int	nRow;				/* Row counter */
	int	nCol;				/* Column counter */

	int iRefCol;
	
	// Sample Row
	//	Take three samples along a row to determine the distance(in Pixels) that the 
	//	EDGE of the scanned image is from a Vertical line
	if ( iStartCol > iEndCol )
		iRefCol = iEndCol;
	else
		iRefCol = iStartCol;

	nRow = rTrimRect.top;
	bRowDone = FALSE;

	bIsALine = FALSE;

	/* Process each row in the bitmap */
	L_AccessBitmap(&hDeskewBitmap);

	// High Level Search to find the 'First' row to check for a line
	//
	while ( !bRowDone )
	{
		bColDone	= FALSE;
		nCol		= iStartCol;			// First Column to Check

		// Search the Selected Row to find the first Column that is not equal
		// to the background Color
		//
		while ( !bColDone )
		{
			PixelColor	= L_GetPixelColor( &hDeskewBitmap, nRow, nCol );

			// Find the Location of the first column that
			// is not the background color
			//
			if ( PixelColor != crBackground )
			{
				crReferenceColor = PixelColor;

				iFirstRow = iLastRow = nRow;
				bColDone = TRUE;
				iSampleRows = 1;

				for ( int iInit = 0; iInit < LINESAMPLES; iInit++ )
				{
					iLinePoints[iInit] = 0;
				}

				// This is a check to verify that the Col Points have all
				// been filled
				//
				iLinePoints[LINESAMPLES - 1] = -1;

				// Sample the LINESAMPLES rows using the sample increment
				// to determine the LINESAMPLES Rows to be checked
				//
				iLinePoint = abs(iRefCol - nCol);
				MapLinePoints( iLinePoint, 0 );

				while ( iSampleRows < LINESAMPLES )
				{
					iLastRow += iIncr;
					nCol = iStartCol;

					bSampleCols = FALSE;
					bIsALine = FALSE;

					while ( !bSampleCols )
					{
						PixelColor	= L_GetPixelColor( &hDeskewBitmap, iLastRow, nCol );
						if ( PixelColor != crBackground )
						{
							iLinePoint = abs(iRefCol - nCol);
							if ( !MapLinePoints( iLinePoint, iSampleRows ) )
							{
								iLinePoints[LINESAMPLES - 1] = -1;
								iSampleRows = LINESAMPLES;		// Force an exit
							}

							bSampleCols = TRUE;
						}

						if ( bReverseSrch )
						{
							nCol--;
							if ( nCol < iEndCol )
							{
								iLinePoints[LINESAMPLES - 1] = -1;
								iSampleRows = LINESAMPLES;		// Force an exit
								bSampleCols = TRUE;
							}
						}
						else
						{
							nCol++;
							if ( nCol > iEndCol )
							{
								iLinePoints[LINESAMPLES - 1] = -1;
								iSampleRows = LINESAMPLES;		// Force an exit
								bSampleCols = TRUE;
							}
						}

					}		// End of bSampleCols while loop

					iSampleRows++;

				}		// End of iSampleRows < LINESAMPLES while loop

				bIsALine = FALSE;
				if ( iLinePoints[LINESAMPLES - 1] != -1 )
				{
					bIsALine = IsLine();
				}

			}		// End of PixelColor != crBackground Test

			if ( bReverseSrch )
			{
				nCol--;
				if ( bIsALine || nCol < iEndCol )
					bColDone = TRUE;
			}
			else
			{
				nCol++;
				if ( bIsALine || nCol > iEndCol )
					bColDone = TRUE;
			}
		}		// End of bColDone while loop

		nRow++;
		if ( bIsALine || nRow > rTrimRect.bottom )
			bRowDone = TRUE;
	}		// End of bRowDone while loop

	L_ReleaseBitmap(&hDeskewBitmap);

	fAngle = 0;
	if ( bIsALine )
	{
		fRun		= iLastRow - iFirstRow;
		fRise		= fabs(iLinePoints[0] - iLinePoints[LINESAMPLES - 1]);
		fRadians	= atan2(fRise, fRun);
		fAngle		= fabs(fRadians * ( 180.00F / PI_VALUE ));
	}

	// Round and Truncate since the Rotation function
	// expects tha angle in Hundreths of degrees
	// ie 90 degrees == 9000
	//
	iAngle = int((fAngle + 0.005F) * 100.00F );

	return iAngle;
}



/**********************************************************

  MapLinePoints()

	Fill the array of Line Points that will be used
	to determine if and 'Edge' has been located.

**********************************************************/
BOOL CScanDialog::MapLinePoints( int iLinePoint, int iSampleRow)
{
	BOOL bRet = TRUE;

	iLinePoints[iSampleRow] = iLinePoint;
	switch ( iSampleRow )
	{
	case 0:		// This is the first(BASE) Row for the Line
		break;

	case 1:		// Determine direction of Slope
		if ( iLinePoints[0] < iLinePoints[1] )
			bRightToLeft = TRUE;
		else
			bRightToLeft = FALSE;
		break;

	default:	// All other cases determine if the slope is continuing
		if ( bRightToLeft )
		{
			if ( iLinePoints[ iSampleRow - 1 ] >= iLinePoints[ iSampleRow ] )
				bRet = FALSE;
		}
		else
		{
			if ( iLinePoints[ iSampleRow - 1 ] <= iLinePoints[ iSampleRow ] )
				bRet = FALSE;
		}
	}

	return bRet;
}


/**********************************************************

  IsLine()

	Determine if the LinePoints constitute a line with
	the following definition.
	No point on the line shall be more than 2 Standard
	deviations away from the line.

**********************************************************/
BOOL CScanDialog::IsLine()
{
	BOOL bIsLine		= TRUE;

	int i;

	int iStdDev = 0;
	int iDevSum = 0;
	int iSqrdSum = 0;

	int iCheckPoint;
	int iLineIncr;

	for ( i = 1; i < LINESAMPLES; i++ )
	{
		iDevSum += abs(iLinePoints[0] - iLinePoints[i]);
	}

	iSqrdSum = iDevSum^2;

	double dInterValue = iSqrdSum / (LINESAMPLES - 1);
	iStdDev = (int)(sqrt( dInterValue ) + 0.5F);

	// Looking for a straight line
	// The increse should be consistent calculate the Line Increment
	float fLinePtsDiff = fabs((float)(iLinePoints[0] - iLinePoints[LINESAMPLES - 1]));
	iLineIncr = (int)((fLinePtsDiff / LINESAMPLES ) + 0.5F );

	for ( i = 1; i < LINESAMPLES; i++ )
	{
		if ( bRightToLeft )
		{
			iCheckPoint = iLinePoints[0] + (iLineIncr * i);
			if (( iCheckPoint < iLinePoints[i] - (iStdDev * 2)) || 
					( iCheckPoint > iLinePoints[i] + (iStdDev * 2) ) )
				bIsLine = FALSE;
		}
		else
		{
			iCheckPoint = iLinePoints[0] - (iLineIncr * i);
			if (( iCheckPoint < iLinePoints[i] - (iStdDev * 2) ) || 
					( iCheckPoint > iLinePoints[i] + (iStdDev * 2) ) )
				bIsLine = FALSE;
		}
	}
	
	return bIsLine;
}



/**********************************************************

  GetTrimRect()

	Get the rectangle coordinates that will be used when
	the scanned image is CROPPED to the size of the 
	picture that is scanned.

**********************************************************/
BOOL CScanDialog::GetTrimRect( pBITMAPHANDLE pBitmap )
{
	pBITMAPHANDLE pFullBitmap;

	BOOL nRet = SUCCESS;

	L_UINT nThreshold = 180;

	if ( pBitmap )
		pFullBitmap = pBitmap;
	else
		pFullBitmap = &hLeadBitmap;

	nRet =  L_GetAutoTrimRect( pFullBitmap, nThreshold, &rTrimRect );

	if ( nRet != SUCCESS )
	{
		// Default to a full image ie No Trimming
		//
		if ( nRet != ERROR_IMAGE_EMPTY )
			DisplayError( "L_GetAutoTrimRect", nRet );

		rTrimRect.top		= 0;
		rTrimRect.bottom	= BITMAPHEIGHT(pFullBitmap);
		rTrimRect.left		= 0;
		rTrimRect.right		= BITMAPWIDTH(pFullBitmap);
	}

	return nRet;
}


/**********************************************************

  TrimImage()

	Make a copy of the CROPPED bitmap for display in 
	the Cropped Image Window in the dialog box.

**********************************************************/
BOOL CScanDialog::TrimImage()
{
	BOOL nRet = SUCCESS;

	L_INT nCol		= rTrimRect.left;
	L_INT nRow		= rTrimRect.top;
	L_UINT nWidth	= rTrimRect.right	- rTrimRect.left;
	L_UINT nHeight	= rTrimRect.bottom	- rTrimRect.top;

	if (hBitmapCropped.Flags.Allocated) /* Do we have an image? */
		L_FreeBitmap( &hBitmapCropped );

#ifdef LEADTOOLS_13
	nRet = L_CopyBitmapRect( &hBitmapCropped, &hLeadBitmap, nCol, nRow, nWidth, nHeight );
#else		// LeadTools v15
	nRet = L_CopyBitmapRect( &hBitmapCropped, &hLeadBitmap, sizeof(BITMAPHANDLE), nCol, nRow, nWidth, nHeight );
#endif

	return nRet;
}


/**********************************************************

  AddMedia()

	Fill the Media Size ComboBox with the media sizes
	available for this device.

**********************************************************/
void CScanDialog::AddMedia()
{
	MEDIA Media;   

	for (int i = 0; i < pCDev->pDevInfo->TotalMedia; i++)
	{	  
		pCDev->GetMedia(i, &Media);	  	     
		((CComboBox*)GetDlgItem(IDC_OUTPUT_SIZE))->InsertString( -1, Media.Name );
	}

	iMediaIdx = pCDev->pDevInfo->CurrentMediaIdx;

	((CComboBox*)GetDlgItem(IDC_OUTPUT_SIZE))->SetCurSel(iMediaIdx);

	return;
}


/**********************************************************

  OnRedisplayOnly()

	This function is being used for test purposes only. It
	causes the scanned image to be returned to its 
	original scanned state without rescanning the image.

**********************************************************/
void CScanDialog::OnRedisplayOnly() 
{
	// TODO: Add your control notification handler code here
#ifdef LEADTOOLS_13
	L_CopyBitmap( &hLeadBitmap, &hTempBitmap );
#else		// LeadTools v15
	L_CopyBitmap( &hLeadBitmap, &hTempBitmap, sizeof(BITMAPHANDLE) );
#endif

	AdjustImage();
	
	Invalidate();
	
	UpdateWindow();

}


void CScanDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	BOOL bTrackOnce = FALSE;
	BOOL bGotAHit = FALSE;

	CPoint newPoint;

	CRect rClientRect;

	CRect rectSave, rectTrue;  

	POINT fullImagePoint	= point;

	// TODO: Add your message handler code here and/or call default
	
	pRT->GetTrueRect(rectSave);   

	// Store Client Rect
	//
	pFullImageWnd->GetClientRect( &pRT->ClientRect );

	// Convert the MOUSE point to a screen point then a to point
	// within the Full Image window
	//
	fullImagePoint = point;
	ClientToScreen( &fullImagePoint );
	pFullImageWnd->ScreenToClient(&fullImagePoint);

	newPoint = fullImagePoint;

	// Determine if there is a 'Hit' within the tracker rectangle
	//	Saved the HitTest value to determine if the Tracker Rectangle
	//  is being moved or resized
	//
	pRT->nHitValue = pRT->HitTest( newPoint );
	if ( pRT->nHitValue >= 0 )
	{
		if ( !bGotAHit )
			bGotAHit = TRUE;

		InvalidateRect(rectSave);	  		 

		if (pRT->Track(pFullImageWnd, newPoint, FALSE, pFullImageWnd ) && !bTrackOnce )
		{
			// invalidate last position
			InvalidateRect(rectSave);

			// get new position
			pRT->GetTrueRect(&rectTrue);
	
			// invalidate new position
			InvalidateRect(rectTrue);

			pFullImageWnd->GetClientRect( &rClientRect );
			pFullImageWnd->GetWindowRect(&rWndSize);

			CalcAspectRatio( &rClientRect );

			ImageTrimRect( &rClientRect, &pRT->m_rect );
			
			if ( TrimImage() )
				pCroppedWnd->Invalidate();

			pFullImageWnd->Invalidate();

			Invalidate();
			UpdateWindow();

			bTrackOnce = TRUE;
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}



/***********************************************************

  Constructor

***********************************************************/
CImageTrimTracker::CImageTrimTracker()
{
}

/**********************************************************

  Constructor

**********************************************************/
CImageTrimTracker::CImageTrimTracker(LPCRECT lpSrcRect, UINT nStyle) 
 : CRectTracker(lpSrcRect, nStyle) 
{
}


/**********************************************************

  Standard Destructor

**********************************************************/
CImageTrimTracker::~CImageTrimTracker()
{
}


/***********************************************************

  Override these member functions to prevent the tracker
  rectangle for   going outside of the Image Display Window

***********************************************************/
void CImageTrimTracker::DrawTrackerRect( LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd )
{
	ValidateRectLocation();

	CRectTracker::DrawTrackerRect( lpRect, pWndClipTo, pDC, pWnd );
}


void CImageTrimTracker::AdjustRect( int nHandle, LPRECT lpRect )
{
	if ( this->nHitValue != CRectTracker::hitMiddle )
		ValidateRectLocation( lpRect );

	CRectTracker::AdjustRect( nHandle, lpRect );
}


/***********************************************************

  Make sure that the new Tracker Rectangle remains inside
  of the Image Display Window when the Tracker Rectangle
  is being moved.

***********************************************************/
void CImageTrimTracker::ValidateRectLocation()
{
	int nHeight = this->m_rect.bottom - this->m_rect.top;
	int nWidth	= this->m_rect.right - this->m_rect.left;

	if ( this->m_rect.top < 0 )
	{
		this->m_rect.top	= 0;
		this->m_rect.bottom = nHeight;
	}

	if ( this->m_rect.left < 0 )
	{
		this->m_rect.left	= 0;
		this->m_rect.right	= nWidth;
	}

	if ( this->m_rect.right > ClientRect.right )
	{
		this->m_rect.right	= ClientRect.right;
		this->m_rect.left	= this->m_rect.right - nWidth;
	}

	if ( this->m_rect.bottom > ClientRect.bottom )
	{
		this->m_rect.bottom = ClientRect.bottom;
		this->m_rect.top	= this->m_rect.bottom - nHeight;
	}

	return;
}


/***********************************************************

  Make sure that the new Tracker Rectangle remains inside of
  the Image Display Window when the Tracker Rectangle
  is being resized.

***********************************************************/
void CImageTrimTracker::ValidateRectLocation( LPRECT lpRect )
{
	int nHeight = lpRect->bottom - lpRect->top;
	int nWidth	= lpRect->right	 - lpRect->left;

	if ( lpRect->top < 0 )
		lpRect->top = 0;

	if ( lpRect->left < 0 )
		lpRect->left = 0;

	if ( lpRect->right > ClientRect.right )
		lpRect->right = ClientRect.right;

	if ( lpRect->bottom > ClientRect.bottom )
		lpRect->bottom = ClientRect.bottom;

	return;
}
