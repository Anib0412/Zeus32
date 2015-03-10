// CPrintIndexDlg.cpp : implementation file
//
// mods:
//
// 10-10-01 Version 5.0.0 - Initial
// 10-17-01 Version 5.0.0 - Add support for print index options - Image name display and max images per page
// 10-19-01 Version 5.0.0 - Improve resolution
// 10-23-01 Version 5.0.0 - Set resolution to device default
// 12-28-01 Version 5.0.1 - Add Index by Size Option
//                          Add Crop/Fit Transform selection to Index by Size
// 02-20-02 Version 5.0.4 - Wrong field used for description Change ShortName to Description
// 02-21-02 Version 5.0.4 - Modified to set Description based on incomming CQueueFile Description
// 02-28-02 Version 5.0.4 - Fix crash on disk full for Index prints
// 10-21-01 Version 5.1.0 - Support selection of Media Size, copies, output queue
// 04-15-03 Version 5.1.2 - Modified to support Lead Tools 13
// 05-23-03 Version 5.1.2 - Fix IndexPrint layout problem with Fit all images on a page
// 09-12-03 Version 5.1.7 - Index Pictures not aligning properly when Orientation is Portrait
// 10-16-03 Version 5.2.2 - A single picture indexed to some media sizes caused a crash - Fixed
// 03-31-04 Version 5.2.4 - Index Print to landscape orientation incorrect - Fixed
// 12-20-05 Version 5.3.0 - Index print with display names not using layout efficiently - Fixed
//

#include "stdafx.h"
#include "zeus.h"

#include "PrintIndexDlg.h"

#include"inidefs.h"

const UINT NEAR wm_PrevSetInfo = RegisterWindowMessage( PREV_SETINFO );
const UINT NEAR wm_RipSendMsg = RegisterWindowMessage( RIP_SENDMESSAGE );
const UINT NEAR wm_RipComplete = RegisterWindowMessage( RIP_COMPLETE );

#define INDEX_PRINT_FONTSIZE 8
#define INDEX_PRINT_WHITESPACE 0.15f

#define INDEX_PS_BUF 32000

#define SWAPINTS(a,b) {int iTempSwap;iTempSwap=a;a=b;b=iTempSwap;}

#define QUEUETRANSFORM pDoc->QueueTransform

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintIndexDlg dialog


CPrintIndexDlg::CPrintIndexDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintIndexDlg::IDD, pParent)
{
	int i;

	bLandscape   = FALSE;
	bRipComplete = TRUE;

	iImagesAcross = 0;
	iImagesDown   = 0;

	iMediaWidth  = 0;
	iMediaHeight = 0;

	iPrintIndexWidth1000  = 0;
	iPrintIndexHeight1000 = 0;

	iXoffInchesPica = 0;
	iYoffInchesPica = 0;

	CQueueFile* pQFile;

	//{{AFX_DATA_INIT(CPrintIndexDlg)
	m_Index_Copies = 0;
	m_Media_Size = -1;
	m_Select_Pane = -1;
	//}}AFX_DATA_INIT

	pCQueueFile = NULL;

	for ( i = 0; i < cQueueFileArray.GetSize(); i++ )
	{
		pQFile = (CQueueFile*)cQueueFileArray.GetAt( i );
		delete pQFile;
	}
	cQueueFileArray.RemoveAll();

	bStartBuildIndex = FALSE;

	return;
}

CPrintIndexDlg::~CPrintIndexDlg()
{
	CQueueFile* pQFile;

	int i;

	if ( pCQueueFile )
	{
		delete pCQueueFile;
		pCQueueFile = NULL;
	}

	for ( i = 0; i < cQueueFileArray.GetSize(); i++ )
	{
		pQFile = (CQueueFile*)cQueueFileArray.GetAt( i );
		delete pQFile;
	}
	cQueueFileArray.RemoveAll();

	// delete the rip
	if (cr.IsInit)
		cr.Close();
}

void CPrintIndexDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintIndexDlg)
	DDX_Control(pDX, IDC_INDEX_COPIES_SPIN, m_IndexCopiesSpin);
	DDX_Text(pDX, IDC_INDEX_COPIES, m_Index_Copies);
	DDV_MinMaxInt(pDX, m_Index_Copies, 0, 99);
	DDX_CBIndex(pDX, IDC_MEDIA_SIZE, m_Media_Size);
	DDX_CBIndex(pDX, IDC_SELECT_PANE, m_Select_Pane);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintIndexDlg, CDialog)
	//{{AFX_MSG_MAP(CPrintIndexDlg)
	ON_REGISTERED_MESSAGE(wm_PrevSetInfo, OnPrevSetInfo)			
	ON_REGISTERED_MESSAGE(wm_RipSendMsg, OnRipMessage)
	ON_REGISTERED_MESSAGE(wm_RipComplete, OnRipComplete)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_GO, OnGo)
	ON_NOTIFY(UDN_DELTAPOS, IDC_INDEX_COPIES_SPIN, OnDeltaposIndexCopiesSpin)
	ON_CBN_SELCHANGE(IDC_MEDIA_SIZE, OnSelchangeMediaSize)
	ON_CBN_SELCHANGE(IDC_SELECT_PANE, OnSelchangeSelectPane)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintIndexDlg message handlers

void CPrintIndexDlg::OnCancel() 
{
	CQueueFile* pQFile;
	int i;

	// TODO: Add extra cleanup here
	if ( iTimer )
		KillTimer( iTimer );

	for ( i = 0; i < cQueueFileArray.GetSize(); i++ )
	{
		pQFile = (CQueueFile*)cQueueFileArray.GetAt( i );
		delete pQFile;
	}
	cQueueFileArray.RemoveAll();
	
	CDialog::OnCancel();
}

void CPrintIndexDlg::OnOK() 
{
	CQueueFile* pQFile;
	CString csEntry;

	int i;

	// TODO: Add extra validation here
	if ( iTimer )
		KillTimer( iTimer );
			
	csEntry.LoadString(IDS_PI_MEDIA_SIZE);
	Profile.WriteInt(pDefaultDevice->Name, csEntry, m_Media_Size);

	for ( i = 0; i < cQueueFileArray.GetSize(); i++ )
	{
		pQFile = (CQueueFile*)cQueueFileArray.GetAt( i );
		delete pQFile;
	}
	cQueueFileArray.RemoveAll();
	
	CDialog::OnOK();
}


void CPrintIndexDlg::AddImage(CQueueFile* pCQueueFile)
{
	char cmpstr[] = " - ";
	char* pFound = NULL;
	int iLoc;
	int iTotalPages = 0;
	CQueueFile* pQFile;

	// Add this file to the list
	pQFile = new CQueueFile;
	pQFile->Init(pCQueueFile->LongName, iTotalPages, 0);

	pFound = strstr(pCQueueFile->Description, cmpstr);

	if ( pFound != NULL )
	{
		iLoc = (int)(pFound - pCQueueFile->Description + 1);
		pQFile->Description = pCQueueFile->Description.Right(pCQueueFile->Description.GetLength() - ( iLoc + 2 ));
	}
	else
	{
		pQFile->Description = pCQueueFile->Description;
	}

	cQueueFileArray.Add(pQFile);

	return;
}

BOOL CPrintIndexDlg::CalcIndexFileInfo()
{
	CQueueFile* pQFile;

	float fCanvasArea;
	float fIndexPrintArea;
	float fImageArea;

	int i;
	int iPrintsToLayout;
	int iImageSizeHeight;
	int iImageSizeWidth;

	int iTotalPages = 0;

	csaFilename.RemoveAll();

	// List of Index files is complete
	//
	iPrintsToIndex = 0;
	for ( i = 0; i < cQueueFileArray.GetSize(); i++ )
	{
		pQFile = (CQueueFile*)cQueueFileArray.GetAt(i);

		// This is necesary to get the page count for some multipage files
		// eg. Temp files from an app (PowerPoint)
		//
		if ( pQFile->PageCount == 0 ) 
			pQFile->Init( pQFile->LongName, iTotalPages, 0);

		iPrintsToIndex += pQFile->PageCount;
	}

	if ( iPrintsToIndex == 0 )
		return FALSE;

	m_UseSinglePage		= Profile.GetInt(IDS_GENERALSECTION, IDS_PI_SINGLEPAGE, FALSE);

	m_DisplayImageName	= Profile.GetInt(IDS_GENERALSECTION, IDS_PI_IMAGENAME, TRUE);

	m_UseImageCount		= Profile.GetInt(IDS_GENERALSECTION, IDS_PI_IMAGECOUNT, FALSE);
	m_MaxImagesPerPage	= Profile.GetInt(IDS_GENERALSECTION, IDS_PI_IMAGEPERPAGE, IDDM_PI_IMAGES_PP_DEF);

	m_UseImageSize			= Profile.GetInt(IDS_GENERALSECTION, IDS_PI_IMAGESIZE, FALSE);
	m_ImageSize_Transform	= Profile.GetInt(IDS_GENERALSECTION, IDS_PI_IMAGETRANSFORM, TRUE);
	m_ImageSize_Height		= Profile.GetStr(IDS_GENERALSECTION, IDS_PI_IMAGEHEIGHT, "3.0");
	m_ImageSize_Width		= Profile.GetStr(IDS_GENERALSECTION, IDS_PI_IMAGEWIDTH,"2.0");

	m_WhiteSpace			= Profile.GetInt(IDS_GENERALSECTION, IDS_PI_WHITE_SPACE, FALSE);
	m_HorizontalWhiteSpace	= Profile.GetStr(IDS_GENERALSECTION, IDS_PI_HORIZONTAL_WHITE, "0.15");
	m_VerticalWhiteSpace	= Profile.GetStr(IDS_GENERALSECTION, IDS_PI_VERTICAL_WHITE, "0.15");

	// get the actual frame buffer size
	pDefaultDevice->FillRipStruct(&RipStruct);

	// get the media, all photos in this method should 
	// use the same media
	pDefaultDevice->GetMedia(m_Media_Size, &Media);	  	     

	// compute the media size
	if (pDefaultDevice->pDevInfo->Orientation == DEV_ORIENT_PORTRAIT)
	{
		iMediaWidth	 = (int)(float)abs(Media.Rect.right - Media.Rect.left);
		iMediaHeight = (int)(float)abs(Media.Rect.top - Media.Rect.bottom);

		iMediaWidthPica  = (int)((float)iMediaWidth / 100.F * 72. + .5F);
		iMediaHeightPica = (int)((float)iMediaHeight / 100.F * 72. + .5F);

		m_Orientation = DEV_ORIENT_PORTRAIT;
	}
	else
	{
		iMediaHeight = (int)(float)abs(Media.Rect.right - Media.Rect.left);
		iMediaWidth  = (int)(float)abs(Media.Rect.top - Media.Rect.bottom);

		iMediaHeightPica = (int)((float)iMediaHeight / 100.F * 72. + .5F);
		iMediaWidthPica  = (int)((float)iMediaWidth / 100.F * 72. + .5F);

		m_Orientation = DEV_ORIENT_LANDSCAPE;
	}

	if ( m_WhiteSpace == TRUE )
	{
		fHorizontalWhiteSpace	= ((atof(m_HorizontalWhiteSpace) * 100) + .5f);
		fVerticalWhiteSpace		= ((atof(m_VerticalWhiteSpace) * 100) + .5f);

		fHorizontalWhiteSpace_Pica	= ((atof(m_HorizontalWhiteSpace) * 72.0F) + .5F);
		fVerticalWhiteSpace_Pica	= ((atof(m_VerticalWhiteSpace) * 72.0F) + .5F);
	}
	else
	{
		// Use the default
		fHorizontalWhiteSpace	= ((INDEX_PRINT_WHITESPACE * 100) + .5f);
		fVerticalWhiteSpace		= ((INDEX_PRINT_WHITESPACE * 100) + .5f);

		fHorizontalWhiteSpace_Pica	= ((INDEX_PRINT_WHITESPACE * 72.0F) + .5F);
		fVerticalWhiteSpace_Pica	= ((INDEX_PRINT_WHITESPACE * 72.0F) + .5F);
	}

	// Force iPrintsToLayout to be EVEN to provide a proper layout
	//
	if ( m_UseSinglePage )
	{
		iPrintsToLayout = iPrintsToIndex;
		iImagesMaxPageImages  = iPrintsToIndex;
	}
	else
	{
		if ( m_UseImageCount )
		{
			iPrintsToLayout = m_MaxImagesPerPage;
			iImagesMaxPageImages  = m_MaxImagesPerPage;
		}
		else
		{
			// Must be ImageSize so Calculatte the ImagesPerpage
			fCanvasArea = (float)(iMediaHeight * iMediaWidth);
			if ( ((float)iMediaWidth / (float)iMediaHeight ) > 1.0f )
			{
				iImageSizeHeight = (int)((atof(m_ImageSize_Width) * 100) + .5f);
				iImageSizeWidth = (int)((atof(m_ImageSize_Height) * 100) + .5f);
			}
			else
			{
				iImageSizeHeight = (int)((atof(m_ImageSize_Height) * 100) + .5f);
				iImageSizeWidth = (int)((atof(m_ImageSize_Width) * 100) + .5f);
			}

			iImageSizeHeight = (int)(((float)iImageSizeHeight + (fHorizontalWhiteSpace * 2.0f)) + 0.5f);
			if ( m_DisplayImageName )
//				iImageSizeHeight += ((INDEX_PRINT_FONTSIZE + 2) * 2);
				iImageSizeHeight += (INDEX_PRINT_FONTSIZE);

			iImageSizeWidth = (int)(((float)iImageSizeWidth + (fVerticalWhiteSpace * 2.0f)) + 0.5f);

			// Provide Space for the FileName
			fImageArea = iImageSizeHeight * iImageSizeWidth;
			iImagesMaxPageImages = iPrintsToLayout = (int)((fCanvasArea / fImageArea) - .5f);
		}
	}

	// For calculation purposes it works better as an even number
	//
	if ( (iPrintsToLayout % 2) != 0 )
		iPrintsToLayout++;

	fCanvasArea = (float)(iMediaHeight * iMediaWidth);
//	fIndexPrintArea = (float)((((float)iMediaHeight - fVerticalWhiteSpace ) * ((float)iMediaWidth - fHorizontalWhiteSpace)) / (float)iPrintsToLayout);
	fIndexPrintArea = (float)(fCanvasArea / (float)iPrintsToLayout);

	CalcImageSizeAndLocation( fCanvasArea, fIndexPrintArea );

	SetDescriptionFont();

	iImageToProcess = 0;
	iImagesOnPage = 0;

	return TRUE;
}

BOOL CPrintIndexDlg::CreateIndexFile()
{
	BOOL bRet;

	// make a temp file
	GetTempFileName(csScratchPath, "RPIdx", 0, pTempFileName);

	if (!cf.Open(pTempFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{
		// Error Can't Open Temp File
		return FALSE;
	}

	// Write the beginning of the PS file
	bRet = WriteIndexPSHeader(cf, iMediaWidthPica, iMediaHeightPica, TRUE);

	if ( !bRet ) 
	{
		cf.Close();
		return bRet;
	}

		// Create the Index Print with the files from the Queue
	// by using blt to put all of the Images onto the canvas
	//
	iIndexRow = 1;
	iIndexCol = 1;

	iDestWidthPica  = (int)((float)iPrintIndexWidth1000 * 72. / 1000.F + .5F);
	iDestHeightPica = (int)((float)iPrintIndexHeight1000 * 72. / 1000.F + .5F);

	return TRUE;
}

BOOL CPrintIndexDlg::EndIndexFile()
{
	// write the end of the ps file
	WriteIndexPSTrailer(cf);

	// close the file
	cf.Close();

	csaFilename.Add(pTempFileName);

	return TRUE;
}

int  CPrintIndexDlg::GetFileCount()
{
	return csaFilename.GetSize();
}

CString CPrintIndexDlg::GetIndexFileName( int iIndex )
{
	CString csFileName;

	csFileName = csaFilename.GetAt(iIndex);

	return csFileName;
}

BOOL CPrintIndexDlg::WriteIndexPSHeader(CStdioFile& cf, int iMediaWidthPica, 
										  int iMediaHeightPica, BOOL bDoEPSFStuff)
{
	// do the starting PS stuff
	fprintf(cf.m_pStream, "%%!PS-Adobe-2.0 EPSF-2.0\r\n");
	fprintf(cf.m_pStream, "%%%%Title: RasterPlus Index\r\n");
	fprintf(cf.m_pStream, "%%%%Creator: RO\r\n");
	fprintf(cf.m_pStream, "%%%%BoundingBox: 0 0 %d %d\r\n", iMediaWidthPica, iMediaHeightPica);
	fprintf(cf.m_pStream, "%%%%EndComments\r\n");
	fprintf(cf.m_pStream, "%%%%BeginSetup\r\n");
	fprintf(cf.m_pStream, "%%%%EndSetup\r\n");
	fprintf(cf.m_pStream, "/BD {bind def} bind def\r\n");
	fprintf(cf.m_pStream, "/LD {load def} BD\r\n");
	fprintf(cf.m_pStream, "/GS /gsave LD\r\n");
	fprintf(cf.m_pStream, "/GR /grestore LD\r\n");
	fprintf(cf.m_pStream, "/N /newpath LD\r\n");
	fprintf(cf.m_pStream, "/M /moveto LD\r\n");
	fprintf(cf.m_pStream, "/L /lineto LD\r\n");   
	fprintf(cf.m_pStream, "/S /show LD\r\n");
	fprintf(cf.m_pStream, "/SL /setlinewidth LD\r\n");
	fprintf(cf.m_pStream, "/FF {gsave eofill grestore} def\r\n");
	fprintf(cf.m_pStream, "/EO /eoclip LD\r\n");
	fprintf(cf.m_pStream, "/SF {gsave stroke grestore} def\r\n");
	fprintf(cf.m_pStream, "/Courier findfont [%d 0 0 %d 0 0] makefont setfont\r\n",
												INDEX_PRINT_FONTSIZE, INDEX_PRINT_FONTSIZE);
	fprintf(cf.m_pStream, "/cstring\r\n");
	fprintf(cf.m_pStream, "   { dup stringwidth pop 2 div currentpoint\r\n");
	fprintf(cf.m_pStream, "     3 -2 roll exch sub exch moveto } def\r\n");
	fprintf(cf.m_pStream, "/rstring\r\n");
	fprintf(cf.m_pStream, "   { dup stringwidth pop currentpoint\r\n");
	fprintf(cf.m_pStream, "     3 -2 roll exch sub exch moveto } def\r\n");
	fprintf(cf.m_pStream, "\r\n");

	if (bDoEPSFStuff)
	{
		fprintf(cf.m_pStream, "/BeginEPSF {\r\n");
		fprintf(cf.m_pStream, "/b4_Inc_state save def\r\n");
		fprintf(cf.m_pStream, "/dict_count countdictstack def\r\n");
		fprintf(cf.m_pStream, "/op_count count 1 sub def\r\n");
		fprintf(cf.m_pStream, "userdict begin\r\n");
		fprintf(cf.m_pStream, "/showpage {} def\r\n");
		fprintf(cf.m_pStream, "0 setgray 0 setlinecap\r\n");
		fprintf(cf.m_pStream, "1 setlinewidth 0 setlinejoin\r\n");
		fprintf(cf.m_pStream, "10 setmiterlimit [] 0 setdash newpath\r\n");
		fprintf(cf.m_pStream, "/languagelevel where\r\n");
		fprintf(cf.m_pStream, "{pop languagelevel\r\n");
		fprintf(cf.m_pStream, "1 ne\r\n");
		fprintf(cf.m_pStream, "  {false setstrokeadjust false setoverprint\r\n");
		fprintf(cf.m_pStream, "  } if\r\n");
		fprintf(cf.m_pStream, "} if\r\n");
		fprintf(cf.m_pStream, "} bind def\r\n");
		fprintf(cf.m_pStream, "\r\n");
		fprintf(cf.m_pStream, "/EndEPSF {\r\n");
		fprintf(cf.m_pStream, "count op_count sub {pop} repeat\r\n");
		fprintf(cf.m_pStream, "countdictstack dict_count sub {end} repeat\r\n");
		fprintf(cf.m_pStream, "b4_Inc_state restore\r\n");	  
		fprintf(cf.m_pStream, "} bind def\r\n");
	}

	fprintf(cf.m_pStream, "\r\n");   
	fprintf(cf.m_pStream, "GS N\r\n");
	fprintf(cf.m_pStream, "0 0 M\r\n");
	fprintf(cf.m_pStream, "%d 0 L\r\n", iMediaWidthPica);
	fprintf(cf.m_pStream, "%d %d L\r\n", iMediaWidthPica, iMediaHeightPica);
	fprintf(cf.m_pStream, "0 %d L\r\n", iMediaHeightPica);
	fprintf(cf.m_pStream, ".999 .999 .999 setrgbcolor\r\n");

	fprintf(cf.m_pStream, "FF GR\r\n");

	// for everything else
	fprintf(cf.m_pStream, ".0 .0 .0 setrgbcolor\r\n");   

	return TRUE;
}


BOOL CPrintIndexDlg::WritePSIndexBitmap(CStdioFile& cf, int iDestWidthPica, int iDestHeightPica, 
																int iDestXOffPica, int iDestYOffPica, 
																					BITMAPHANDLE PrintBitmap) 
{
	BOOL  bRet = TRUE;

	BYTE* pData; 
	
	int i;

	// write the bitmap to the eps file
	fprintf(cf.m_pStream, "GS /picstr %d string def\r\n", PrintBitmap.Width * 3);

	fprintf(cf.m_pStream, "%d %d translate %d %d scale\r\n", iDestXOffPica,
								iDestYOffPica, iDestWidthPica, iDestHeightPica);

	fprintf(cf.m_pStream, "%d %d 8 [%d 0 0 -%d 0 %d]\r\n", PrintBitmap.Width,
								PrintBitmap.Height, PrintBitmap.Width, PrintBitmap.Height, PrintBitmap.Height);

	fprintf(cf.m_pStream, "{currentfile picstr readstring pop} false 3 colorimage\r\n");

	// lock the tile bitmap
	L_AccessBitmap(&PrintBitmap);

	// write the data
	if (pData = new BYTE[PrintBitmap.Width * 3])
	{
		for (i = 0; i < PrintBitmap.Height; i++)
		{	
			L_GetBitmapRow(&PrintBitmap, pData, i, PrintBitmap.Width * 3);

			try 
			{
				cf.Write(pData, PrintBitmap.Width * 3);
			}
			catch(CFileException* e)
			{
				e->Delete();

				bAbort = TRUE;
			}
		}

		try
		{
			if ( !bAbort )
				cf.Write("\r\nGR\r\n", 6);
		}
		catch(CFileException* e)
		{
			e->Delete();

			bAbort = TRUE;
		}

		delete pData;
	}

	// release the bitmap
	L_ReleaseBitmap(&PrintBitmap);

	return TRUE;
}

BOOL CPrintIndexDlg::WritePSIndexPS(CStdioFile& cf, int iMediaWidthPica, int iMediaHeightPica,
									int iDestWidthPica, int iDestHeightPica, 
											int iDestXOffPica, int iDestYOffPica, char* pFileName)
{
	BOOL bRet = TRUE;

	BYTE* pBuf;

	CFile cfIn;

	float fxScale;
	float fyScale;

	int iNumRead;
	
	// make a buf
	if (!(pBuf = new BYTE [INDEX_PS_BUF]))
	{
		return FALSE;
	}

	Sleep(100000);

	// put the job separator file into the bigger file
	try
	{
		if (cfIn.Open(pFileName, CFile::modeRead | CFile::typeBinary))
		{	  
			// write the start
			fprintf(cf.m_pStream, "\r\nBeginEPSF\r\n");         

			// if this is a portrait frame, rotate the image
			if ( bLandscape )
			{
				fxScale = (float)iDestWidthPica / iMediaHeightPica;
				fyScale = (float)iDestHeightPica / iMediaWidthPica;
			}
			else
			{		    
//				fprintf(cf.m_pStream, "%d 0 translate\r\n", iDestWidthPica);        			
//				fprintf(cf.m_pStream, "90 rotate\r\n");     
				fxScale = (float)iDestHeightPica / iMediaWidthPica;
				fyScale = (float)iDestWidthPica / iMediaHeightPica;
				SWAPINTS(iDestXOffPica, iDestYOffPica);

//				iDestYOffPica = -iDestYOffPica;
			}

			// and the scaline
			fprintf(cf.m_pStream, "%d %d translate %f %f scale\r\n", 	  
									iDestXOffPica, iDestYOffPica, fxScale, fyScale);

			// put in the file
			do
			{
				// read the file
				iNumRead = cfIn.Read(pBuf, INDEX_PS_BUF); 
				// and write it
				if (iNumRead > 0)
					cf.Write(pBuf, iNumRead);
			} while (iNumRead > 0); 	  

			// write the end
			fprintf(cf.m_pStream, "\r\nEndEPSF\r\n");

			cfIn.Close();
		}	  
		else
		{
			bRet = FALSE;
		}
	}

	catch(CFileException* e)	  
	{
		e->Delete();
		bRet = FALSE;
	}										 	  

	// delete the buffer
	delete pBuf;

	return bRet;
}

BOOL CPrintIndexDlg::WriteIndexPSTrailer(CStdioFile& cf)
{
	// write the end
	fprintf(cf.m_pStream, "gsave showpage grestore\r\n");   

	return TRUE;
}

BOOL CPrintIndexDlg::ResizeIndexPrintBitmap(pBITMAPHANDLE pPrintBitmap, int iWidthInches1000, int iHeightInches1000)
{
	BOOL bLandscapeDevice;
	BOOL bLandscapeBitmap;
	BOOL bRet = TRUE;

	float fDevAspect;
	float fBitmapAspect;
	
	int iNewWidth, iNewHeight;

	// get aspect ratios
	fDevAspect = (float)iHeightInches1000 / (float)iWidthInches1000;
	fBitmapAspect = (float)pPrintBitmap->Height / (float)pPrintBitmap->Width;

	// landscape device?
	if (fDevAspect < 1.0)
		bLandscapeDevice = TRUE;
	else
		bLandscapeDevice = FALSE;

	// landscape bitmap?
	if (fBitmapAspect < 1.0)
		bLandscapeBitmap = TRUE;
	else
		bLandscapeBitmap = FALSE;

	// rotate it if necessary
	if (bLandscapeBitmap != bLandscapeDevice)
	{
		if (L_RotateBitmap(pPrintBitmap, 9000, ROTATE_RESIZE,  RGB(255, 255, 255)) != SUCCESS)
			return FALSE;

		// recompute
		fBitmapAspect = (float)pPrintBitmap->Height / (float)pPrintBitmap->Width;
	}

	// now compute the trim
	if (fDevAspect < fBitmapAspect)
	{
		iNewHeight = (int)((float)pPrintBitmap->Width * fDevAspect + .5F);
		if (L_TrimBitmap(pPrintBitmap, 0, (pPrintBitmap->Height - iNewHeight) / 2,
												pPrintBitmap->Width, iNewHeight) != SUCCESS)
			return FALSE;															   
	}
	else if (fDevAspect > fBitmapAspect)
	{
		iNewWidth = (int)((float)pPrintBitmap->Height / fDevAspect + .5F);
		if (L_TrimBitmap(pPrintBitmap, (pPrintBitmap->Width - iNewWidth) / 2, 0,
												iNewWidth, pPrintBitmap->Height) != SUCCESS)
			return FALSE;															   
	}								 

	return bRet;
}


void CPrintIndexDlg::CalcImageSizeAndLocation(float fCanvasArea, float fIndexPrintArea )
{
	fAspectRatio	= 0;
	fWidth			= 0;
	fHeight			= 0;

	iPrintIndexWidth	= 0;
	iPrintIndexHeight	= 0;

	iTotalImageHeight	= 0;
	iTotalImageWidth	= 0;

//	fAspectRatio = (float)iMediaWidth / (float)iMediaHeight;
	fAspectRatio = (float)iMediaHeight / (float)iMediaWidth;

	if (fAspectRatio > 1.0)
	{
		bLandscape = TRUE;
	}
	else
	{
		bLandscape = FALSE;
	}

	if ( !m_UseImageSize )
	{
		// Want to round Down
		if ( !bLandscape )
		{
			CalcLandscapeSize( fCanvasArea, fIndexPrintArea );
		}
		else
		{
			CalcPortraitSize( fCanvasArea, fIndexPrintArea );
		}

		iPrintIndexHeight1000 = iPrintIndexHeight * 10;
		iPrintIndexWidth1000  = iPrintIndexWidth * 10;
	}
	else
	{
		fHeight = atof(m_ImageSize_Height);
		fWidth	= atof(m_ImageSize_Width);

		if ( !bLandscape )
		{
			iPrintIndexHeight = (int)((fHeight * 100) + (fVerticalWhiteSpace * 2.0f) + .5f);
			iPrintIndexWidth = (int)((fWidth * 100) + (fHorizontalWhiteSpace * 2.0f) + .5f);

			// Calculate the Number of images at full size then
			iImagesAcross = iMediaWidth / iPrintIndexWidth;
			if ( iImagesAcross < 1 )
				iImagesAcross = 1;
			iImagesDown	  = iMediaHeight / iPrintIndexHeight;
			if ( iImagesDown < 1 )
				iImagesDown = 1;

			// Recalculate Max Images Incase it was not even
			//
			iImagesMaxPageImages = iImagesAcross * iImagesDown;
		}
		else
		{
			iPrintIndexHeight = (int)((fHeight * 100) + (fHorizontalWhiteSpace * 2.0f) + .5f);
			iPrintIndexWidth = (int)((fWidth * 100) + (fVerticalWhiteSpace * 2.0f) + .5f);

			// Calculate the Number of images at full size then
			iImagesAcross = iMediaWidth / iPrintIndexWidth;
			if ( iImagesAcross < 1 )
				iImagesAcross = 1;
			iImagesDown	  = iMediaHeight / iPrintIndexHeight;
			if ( iImagesDown < 1 )
				iImagesDown = 1;
/*
			// Calculate the Number of images at full size then
			iImagesAcross = iMediaHeight / iPrintIndexHeight;
			if ( iImagesAcross < 1 )
				iImagesAcross = 1;
			iImagesDown	  = iMediaWidth / iPrintIndexWidth;
			if ( iImagesDown < 1 )
				iImagesDown = 1;
*/
			// Recalculate Max Images Incase it was not even
			//
			iImagesMaxPageImages = iImagesDown * iImagesAcross;
		}

		iPrintIndexHeight1000 = (int)((fHeight * 100) * 10);
		iPrintIndexWidth1000  = (int)((fWidth * 100) * 10);
	}

#if 0
	if ( m_WhiteSpace == TRUE )
	{
		if ( bLandscape == FALSE )
		{
			iYoffInchesPica = fHorizontalWhiteSpace_Pica + 0.5;
			iXoffInchesPica = fVerticalWhiteSpace_Pica + 0.5;
		}
		else
		{
			iXoffInchesPica = fHorizontalWhiteSpace_Pica + 0.5;
			iYoffInchesPica = fVerticalWhiteSpace_Pica + 0.5;
		}
	}
	else
	{
		if ( bLandscape == FALSE )
		{
			iYoffInchesPica = (iMediaWidth - (iPrintIndexWidth * iImagesAcross)) / (iImagesAcross + 1);
			iYoffInchesPica = iYoffInchesPica * 10;
			iYoffInchesPica = (int)(((float)iYoffInchesPica * 72) / 1000.F + .5F);

			iXoffInchesPica = (iMediaHeight - (iPrintIndexHeight * iImagesDown)) / (iImagesDown + 1);
			iXoffInchesPica = iXoffInchesPica * 10;
			iXoffInchesPica = (int)(((float)iXoffInchesPica * 72) / 1000.F + .5F);
		}
		else
		{
			iXoffInchesPica = (iMediaWidth - (iPrintIndexWidth * iImagesAcross)) / (iImagesAcross + 1);
			iXoffInchesPica = iXoffInchesPica * 10;
			iXoffInchesPica = (int)(((float)iXoffInchesPica * 72) / 1000.F + .5F);

			iYoffInchesPica = (iMediaHeight - (iPrintIndexHeight * iImagesDown)) / (iImagesDown + 1);
			iYoffInchesPica = iYoffInchesPica * 10;
			iYoffInchesPica = (int)(((float)iYoffInchesPica * 72) / 1000.F + .5F);
		}
	}
#else
	if ( bLandscape == FALSE )
	{
		iYoffInchesPica = fHorizontalWhiteSpace_Pica + 0.5;
		iXoffInchesPica = fVerticalWhiteSpace_Pica + 0.5;
	}
	else
	{
		iXoffInchesPica = fHorizontalWhiteSpace_Pica + 0.5;
		iYoffInchesPica = fVerticalWhiteSpace_Pica + 0.5;
	}
#endif
}


void CPrintIndexDlg::SetDescriptionFont()
{

	return;
}

void CPrintIndexDlg::AddImageDescription( CStdioFile& cf, CString csDescription,
															int iXOffset, int iYOffset )
{
	fprintf(cf.m_pStream, "%d %d M\r\n", iXOffset, iYOffset);
	fprintf(cf.m_pStream, "(%s) cstring\r\n", CStringToPSString(csDescription));   
	fprintf(cf.m_pStream, "(%s) S\r\n", CStringToPSString(csDescription));   
}

CString CPrintIndexDlg::CStringToPSString(LPCSTR lpStrIn)
{
	int i;
	CString cs = "";
	char c;   

	for (i = 0; i < strlen(lpStrIn); i++)
	{
		c = lpStrIn[i];

		switch(c)
		{
		case '\\':
			cs += "\\\\";			  
			break;
		case '(':
			cs += "\\(";
			break;
		case ')':
			cs += "\\)";
			break;
		default:
			cs += c;
			break;
		}
	}
	return cs;
}

void CPrintIndexDlg::OnRepaint(CString csFileName)
{
	float fDevInchWidth;

	int iTotalPages = 0;
	int ret;

	if (pCQueueFile)
	{
		delete pCQueueFile;
		pCQueueFile = NULL;
	}

	// delete the rip
	if (cr.IsInit && bRipComplete )
	{
		return;
	}

	// make a new pseudo queue file
	if ((pCQueueFile = new CQueueFile) == NULL)
		return;

	pCQueueFile->Init(csFileName, iTotalPages, 0);

	if ( m_ImageSize_Transform )
	{
		pPrintIndexTransform->LoadTransform("SmartCrop");
	}
	else
	{
		pPrintIndexTransform->LoadTransform("SmartFit");
	}

	// open rip object
	cr.Open();

	// init rip   
	ret = cr.Init(pDefaultDevice, pPrintIndexTransform,  pCC,
							pCQueueFile, RIPSTYLE_PREVIEW, this, this, TRUE);

	// override our preview params and start rip   
	if (ret == SYN_ERRMSG_ALLOK)                          
	{                          
       ((CProgressCtrl*)GetDlgItem(IDC_PREVIEW_PROGRESS))->SetPos(0);		         	   

		// get some numbers
		RIPSTRUCT RipStruct;
		pDefaultDevice->FillRipStruct(&RipStruct);

		// special mods
		RipStruct.PixWidth = (int)((fRipWidth * RipStruct.WPixelsPerInch) + 0.5F);
		RipStruct.PixHeight = (int)((fRipHeight * RipStruct.HPixelsPerInch) + 0.5F);

		fDevInchWidth = (float)RipStruct.PixWidth / (float)RipStruct.WPixelsPerInch;	  

		// send new image size but scale up for accuracy
		cr.RipParamsOverride(RipStruct.PixWidth, RipStruct.PixHeight,
								(float)RipStruct.PixWidth / fDevInchWidth,
									(float)RipStruct.PixWidth / fDevInchWidth,
										this->m_hWnd, pPrintIndexTransform, TRUE);

		// start RIP  	  
		ret = cr.StartRip();   
	}                   
	else
	{
		cr.Close();
	}

	return;
}

void CPrintIndexDlg::SetPrintHWnd( HWND hWnd )
{
	hWnd_PrintPane = hWnd;
	return;
}

void CPrintIndexDlg::SetPrintIndexInfo(CWnd* pPrWnd, CDevice* pDevice, CTransform* pTransform, CColorTransform* pColorTransform)
{
	pDefaultDevice = pDevice;
	pPrintIndexTransform = pTransform;
	pCC = pColorTransform;

	pWnd = pPrWnd;

	return;
}

LRESULT CPrintIndexDlg::OnPrevSetInfo(WPARAM wParam, LPARAM lParam)
{
	FILEINFO FileInfo;

	PPREVIEWINFO pPreviewInfo;
	LOADFILEOPTION ExtFileOption;  /* Structure used for page numbers */

	int iTextXOffPica = 0;
	int iTextYOffPica = 0;
	int iDestXOffPica = 0;
	int iDestYOffPica = 0;

	//This is where I need to add the next image

	// get the pointer  
	pPreviewInfo = (PPREVIEWINFO)lParam;
    
	iDestXOffPica = (iXoffInchesPica * iIndexCol) + (iDestWidthPica * (iIndexCol - 1 ));
	iDestYOffPica = iMediaHeightPica - ((iYoffInchesPica * iIndexRow) + (iDestHeightPica * iIndexRow));

	if ( m_DisplayImageName )
	{
		iTextXOffPica = iDestXOffPica + (iDestWidthPica / 2);
		iTextYOffPica = iDestYOffPica - (iYoffInchesPica / 2);
	}

	L_GetDefaultLoadFileOption( &ExtFileOption, sizeof LOADFILEOPTION);

	ExtFileOption.XResolution = pPreviewInfo->RipHPixelsPerInch;
	ExtFileOption.YResolution = pPreviewInfo->RipWPixelsPerInch;
 
	memset(&FileInfo, 0, sizeof(FILEINFO));
    FileInfo.uStructSize = sizeof(FILEINFO);
	L_FileInfoMemory((unsigned char*)(pPreviewInfo->ImageBuffer), &FileInfo, pPreviewInfo->ImageSize, sizeof(FILEINFO), 0, &ExtFileOption);

	L_LoadBitmapMemory((unsigned char*)(pPreviewInfo->ImageBuffer), &PrintBitmap, sizeof(BITMAPHANDLE), 24, ORDER_RGB,
														pPreviewInfo->ImageSize, &ExtFileOption, &FileInfo);

	WritePSIndexBitmap(cf, iDestWidthPica, iDestHeightPica,
											iDestXOffPica, iDestYOffPica, PrintBitmap);

	if ( m_DisplayImageName )
	{
		if ( iPageNumber < 0 )
			AddImageDescription( cf, csDescription, iTextXOffPica, iTextYOffPica );
		else
		{
			CString csFileDescription;
			csFileDescription.Format("%s(Page %d)", csDescription, iPageNumber);
			AddImageDescription( cf, csFileDescription, iTextXOffPica, iTextYOffPica );
			iPageNumber++;
		}
	}

	// free the photo bitmap
	if (FileInfo.Format != FILE_EPS)
			L_FreeBitmap(&PrintBitmap);

	if ( iIndexCol < iImagesAcross )
		iIndexCol++;
	else
	{
		iIndexCol = 1;
		iIndexRow++;
	}

	return 0L;
}
   
LRESULT CPrintIndexDlg::OnRipMessage(WPARAM WParam, LPARAM LParam)
{
   PRIPMSGSTRUCT pMsgStruct;   

   // cast the LParam to a ripmsg pointer
   pMsgStruct = (PRIPMSGSTRUCT)LParam;
   
   // do message stuff based on msg type
   switch(pMsgStruct->MsgType)
      {
	  case RIPMSG_SCANLINE:   
		   // show reading status
		   ((CProgressCtrl*)GetDlgItem(IDC_PREVIEW_PROGRESS))->SetPos(pMsgStruct->LineOn * 100 / pMsgStruct->LineTotal);		      
           break;
      case RIPMSG_READING:        
		   // show reading status
		   ((CProgressCtrl*)GetDlgItem(IDC_PREVIEW_PROGRESS))->SetPos(pMsgStruct->PercentComplete);		      
           break;
      }         

   return 0L;   
}     
   
LRESULT CPrintIndexDlg::OnRipComplete(WPARAM wParam, LPARAM lParam)
{
	bRipComplete = TRUE;
	return 0L;
}

void CPrintIndexDlg::OnTimer(UINT nIDEvent) 
{
	CQueueFile* pQFile;

	// TODO: Add your message handler code here and/or call default
	if ( !bAbort )
	{
		if ( bRipComplete == TRUE )
		{
			if ( iImagesOnPage >= iImagesMaxPageImages )	// The Page is full
			{
				EndIndexFile();
				iImagesOnPage = 0;
				// There are still more to process so Create a new index file
				if ( iImageToProcess < cQueueFileArray.GetSize() )
					CreateIndexFile();
			}

			if ( iImageToProcess >= cQueueFileArray.GetSize() )
			{
				if ( iImagesOnPage > 0 )
					EndIndexFile();
				OnOK();
			}
			else
			{
				bRipComplete = FALSE;

				pQFile = (CQueueFile*)cQueueFileArray.GetAt( iImageToProcess );

				((CEdit*)GetDlgItem(IDC_INDEX_FILENAME))->SetWindowText(pQFile->ShortName);

				fRipHeight = (float)iPrintIndexHeight1000 / 1000.0F;
				fRipWidth = (float)iPrintIndexWidth1000 / 1000.0F;

				if ( pQFile->PageCount > 1 )
					iPageNumber = 1;
				else
					iPageNumber = -1;

				csDescription = pQFile->Description;

				OnRepaint( pQFile->LongName );

				iImageToProcess++;
				iImagesOnPage++;
			}
		}
	}
	else
	{
		OnCancel();
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CPrintIndexDlg::OnInitDialog() 
{
	int i = 0;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if ( pCQueueFile )
	{
		delete pCQueueFile;
		pCQueueFile = NULL;
	}

	// delete the rip
	if (cr.IsInit)
		cr.Close();

	pCBMediaSize = (CComboBox*)GetDlgItem(IDC_MEDIA_SIZE);
	pCBMediaSize->ResetContent();   					 

	iTotalMedia = pDefaultDevice->UpdateMediaList();

	// insert the sources into the list
	for (i = 0; i < iTotalMedia; i++)
	{	  
		pDefaultDevice->GetMedia(i, &Media);	  	     
		pCBMediaSize->InsertString(-1, Media.Name);
	}

	m_Index_Copies	= Profile.GetInt(IDS_GENERALSECTION, IDS_PI_COPIES, 1 );
	m_Select_Pane	= Profile.GetInt(IDS_GENERALSECTION, IDS_PI_PANE, 1 );
	m_Media_Size	= Profile.GetInt(pDefaultDevice->Name, IDS_PI_MEDIA_SIZE, 0);   

	GetDlgItem(IDC_CREATE_INDEX_TEXT)->ShowWindow(FALSE);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrintIndexDlg::OnGo() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_GO)->EnableWindow( FALSE );
	GetDlgItem(IDC_CREATE_INDEX_TEXT)->ShowWindow(TRUE);

	CalcIndexFileInfo();

	CreateIndexFile();

	SetFocus();

	bAbort = FALSE;
	
	iTimer = SetTimer(232, 100, NULL );

	return;
}

void CPrintIndexDlg::OnDeltaposIndexCopiesSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	int iVal = m_Index_Copies;
	// increment/decrement
	iVal += (pNMUpDown->iDelta);

	if ( iVal < 1 )
		iVal = 1;

	if ( iVal > 99 )
		iVal = 99;

	// put back in
	m_Index_Copies = iVal;

	// apply this      
	UpdateData(FALSE);

	*pResult = 0;
}

void CPrintIndexDlg::OnSelchangeMediaSize() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
}

void CPrintIndexDlg::OnSelchangeSelectPane() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
}

void CPrintIndexDlg::CalcPortraitSize( float fCanvasArea, float fIndexPrintArea, float fExcess )
{	
	float fWhiteSpaceAdj = 0;
	float fPTExcess = 0;

	iPrintIndexHeight = (int)(sqrt(fIndexPrintArea * fAspectRatio ) - 0.5);
	if ( iImagesMaxPageImages > 1 )
	{
		if ( iPrintIndexHeight * 2 > iMediaHeight )
			iPrintIndexHeight = (int)((float)iMediaHeight / 2) - (((float)iMediaHeight / 2) * .10F);
	}

	iPrintIndexWidth = (int)((float)iPrintIndexHeight / fAspectRatio);

/*
	// Adjust area to leave space for the file name
	//
	if ( m_DisplayImageName )
	{
		iPrintIndexHeight -= ((INDEX_PRINT_FONTSIZE + 2) * 2);
		iPrintIndexWidth  = (int)((float)iPrintIndexHeight / fAspectRatio);
	}
*/

	// Calculate the Number of images at full size then
	iImagesDown = iMediaHeight / iPrintIndexHeight;
	if ( iImagesDown < 1 )
		iImagesDown = 1;
	iImagesAcross = iImagesMaxPageImages / iImagesDown;

	if ( (iImagesMaxPageImages % iImagesDown) > 0 )
		iImagesAcross++;

	if ( iImagesMaxPageImages > 1 )
	{
		iTotalImageWidth = iPrintIndexWidth * iImagesAcross;

		fPTExcess = (float)iTotalImageWidth / (float)iMediaWidth;
		if ( (float)iTotalImageWidth > ((float)iMediaWidth * 1.15f ) && fPTExcess > fExcess )
		{
			CalcLandscapeSize( fCanvasArea, fIndexPrintArea, fPTExcess );
		}
		else
		{
			if ( iTotalImageWidth > iMediaWidth )
			{
				iPrintIndexWidth -= (((iTotalImageWidth - iMediaWidth) + (fHorizontalWhiteSpace / 2)) / iImagesAcross );
				iPrintIndexHeight  = (int)((float)iPrintIndexWidth * fAspectRatio);
			}

			// Adjust area to leave space for the file name
			//
			if ( m_DisplayImageName )
			{
				iPrintIndexHeight -= (INDEX_PRINT_FONTSIZE);

				fWhiteSpaceAdj = iPrintIndexWidth;
				iPrintIndexWidth = (int)((float)iPrintIndexHeight / fAspectRatio);
				fWhiteSpaceAdj -= iPrintIndexWidth;
			}

			iPrintIndexHeight = (int)((float)iPrintIndexHeight - fVerticalWhiteSpace - 0.5F);

			iPrintIndexWidth = (int)((float)iPrintIndexWidth + fWhiteSpaceAdj - fHorizontalWhiteSpace - 0.5F);
		}
	}
	else
	{
		// Adjust area to leave space for the file name
		//
		if ( m_DisplayImageName )
		{
			iPrintIndexHeight -= (INDEX_PRINT_FONTSIZE);

			fWhiteSpaceAdj = iPrintIndexWidth;
			iPrintIndexWidth = (int)((float)iPrintIndexHeight / fAspectRatio);
			fWhiteSpaceAdj -= iPrintIndexWidth;
			
			iPrintIndexWidth  = (int)((float)iPrintIndexHeight / fAspectRatio);
		}

		iPrintIndexHeight = (int)((float)iPrintIndexHeight - fVerticalWhiteSpace - 0.5F);

		iPrintIndexWidth = (int)((float)iPrintIndexWidth + fWhiteSpaceAdj - fHorizontalWhiteSpace - 0.5F);
	}

	return;
}

void CPrintIndexDlg::CalcLandscapeSize( float fCanvasArea, float fIndexPrintArea, float fExcess )
{
	float fWhiteSpaceAdj = 0;
	float fLSExcess;

	iPrintIndexWidth = (int)(sqrt(fIndexPrintArea / fAspectRatio ) - 0.5);
	if ( iImagesMaxPageImages > 1 )
	{
		if ( iPrintIndexWidth * 2 > iMediaWidth )
			iPrintIndexWidth = (int)((float)iMediaWidth / 2) - (((float)iMediaWidth / 2) * .10F);
	}

	iPrintIndexHeight  = (int)(fAspectRatio * (float)iPrintIndexWidth);

/*
	// Adjust area to leave space for the file name
	//
	if ( m_DisplayImageName )
	{
		iPrintIndexWidth -= ((INDEX_PRINT_FONTSIZE + 2) * 2);
		iPrintIndexHeight  = (int)(fAspectRatio * (float)iPrintIndexWidth);
	}
*/

	// Calculate the Number of images at full size then
	iImagesAcross	= iMediaWidth / iPrintIndexWidth;
	if ( iImagesAcross < 1 )
		iImagesAcross = 1;
	iImagesDown	= iImagesMaxPageImages / iImagesAcross;

	if ( (iImagesMaxPageImages % iImagesAcross) > 0 )
		iImagesDown++;

	if ( iImagesMaxPageImages > 1 )
	{
		iTotalImageHeight = iPrintIndexHeight * iImagesDown;

		fLSExcess = (float)iTotalImageHeight / (float)iMediaHeight;
		if ( (float)iTotalImageHeight > ( (float)iMediaHeight * 1.15f ) && fLSExcess > fExcess )
		{
			CalcPortraitSize( fCanvasArea, fIndexPrintArea, fLSExcess );
		}
		else
		{
			if ( iTotalImageHeight > iMediaHeight )
			{
				iPrintIndexHeight -= (((iTotalImageHeight - iMediaHeight) + (fHorizontalWhiteSpace / 2)) / iImagesDown );
				iPrintIndexWidth  = (int)((float)iPrintIndexHeight / fAspectRatio);
			}
			// Adjust area to leave space for the file name
			//
			if ( m_DisplayImageName )
			{
				iPrintIndexWidth -= (INDEX_PRINT_FONTSIZE);

				fWhiteSpaceAdj = iPrintIndexHeight;
				iPrintIndexHeight = (int)(fAspectRatio * (float)iPrintIndexWidth);
				fWhiteSpaceAdj -= iPrintIndexHeight;
			}

			// shrink the image height by WhiteSpace 
			iPrintIndexHeight = (int)((float)iPrintIndexHeight + fWhiteSpaceAdj - fHorizontalWhiteSpace - 0.5F);

			iPrintIndexWidth = (int)((float)iPrintIndexWidth - fVerticalWhiteSpace - 0.5F);
		}
	}
	else
	{
		// Adjust area to leave space for the file name
		//
		if ( m_DisplayImageName )
		{
			iPrintIndexWidth -= (INDEX_PRINT_FONTSIZE);

			fWhiteSpaceAdj = iPrintIndexHeight;
			iPrintIndexHeight = (int)(fAspectRatio * (float)iPrintIndexWidth);
			fWhiteSpaceAdj -= iPrintIndexHeight;
		}

		// shrink the image height by WhiteSpace 
		iPrintIndexHeight = (int)((float)iPrintIndexHeight + fWhiteSpaceAdj - fHorizontalWhiteSpace - 0.5F);

		iPrintIndexWidth = (int)((float)iPrintIndexWidth - fVerticalWhiteSpace - 0.5F);
	}

	return;
}