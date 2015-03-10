/////////////////////////////////////////////////////////////////////////////
// ImagePause.cpp
//                                                                        
// dialog to pause imaging for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#include "stdafx.h"
#include "zeus.h"
#include "ImagePause.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImagePause dialog


CImagePause::CImagePause(CWnd* pParent /*=NULL*/)
	: CDialog(CImagePause::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImagePause)
	m_stop = FALSE;
	//}}AFX_DATA_INIT
}


void CImagePause::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImagePause)
	DDX_Check(pDX, IDC_STOP, m_stop);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImagePause, CDialog)
	//{{AFX_MSG_MAP(CImagePause)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImagePause message handlers

BOOL CImagePause::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	MoveDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CImagePause::MoveDialog()
{
   CPoint   Point;
   CRect    DialogRect;
   CRect    ParentRect;
   CRect    StatusRect;
   int      nWidth;
   int      nHeight;
   CWnd     *DesktopWindow = NULL;
   CWnd     *MainWindow;
   CWnd     *StatusWindow;   

   // Get the size of the dialog box.
   GetWindowRect(DialogRect);
   // actually, must use the parent because this is
   // a property page   
   //GetParent()->GetWindowRect(DialogRect);   

   // Get the main window.
   MainWindow = AfxGetApp()->m_pMainWnd;
   
   // Determine if the main window exists. This can be useful when
   // the application creates the dialog box before it creates the
   // main window. If it does exist, retrieve its size to center
   // the dialog box with respect to the main window.
   if (MainWindow != NULL)
      MainWindow->GetClientRect(ParentRect);
   // If the main window does not exist, orient with respect to
   // the desktop window.
   else
   {
      DesktopWindow = GetDesktopWindow();
      DesktopWindow->GetWindowRect(ParentRect);
   }
      
   // Calculate the height and width for MoveWindow().
   nWidth = DialogRect.Width();
   nHeight = DialogRect.Height();   

   // Find the lower right point
   //Point.x = ParentRect.Width();
   Point.x = 0;
   Point.y = ParentRect.Height();
   
   // Calculate the new X, Y starting point.
   //Point.x -= nWidth;
   Point.y -= nHeight;
   
   // subtract height of status bar if it exists
   StatusWindow = AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);   
   if (StatusWindow != NULL)
      {
      StatusWindow->GetClientRect(StatusRect);         
      Point.y -= StatusRect.Height();            
      }   

   // Move the window.
   MoveWindow(Point.x, Point.y, nWidth, nHeight, FALSE);
   }

void CImagePause::OnOK() 
   {
   // get data
   UpdateData(TRUE);
   CDialog::OnOK();
   }
