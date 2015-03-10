/////////////////////////////////////////////////////////////////////////////
// SPLITDLG.CPP
//                                                                        
// dialog for file splitting for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//
// mods:
//
// 7-2-96  Build 40 - EnableWindow on the non-existent HELP button caused a GPF -
//				      fixed 

#include "stdafx.h"
#include "zeus.h"
#include "splitdlg.h"
#include "winsplit.h"
#include "profile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SplitDlg dialog


SplitDlg::SplitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SplitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SplitDlg)
	m_deletefile = FALSE;
	m_removefile = FALSE;
	m_insertfile = FALSE;
	m_status = "";
	//}}AFX_DATA_INIT
}

void SplitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SplitDlg)
	DDX_Check(pDX, IDC_DELETEFILE, m_deletefile);
	DDX_Check(pDX, IDC_REMOVEFILE, m_removefile);
	DDX_Check(pDX, IDC_INSERT, m_insertfile);
	DDX_Text(pDX, IDC_STATUS, m_status);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SplitDlg, CDialog)
	//{{AFX_MSG_MAP(SplitDlg)
	ON_BN_CLICKED(IDC_DELETEFILE, OnDeletefile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SplitDlg message handlers

void SplitDlg::OnOK()
   {
   CString csSection, csEntry;   
   int ret;   
   CWinSplit* pCw;           
   CString SplitFile;
   CProfile Profile;
   
   // make sure we have current data   
   UpdateData(TRUE);
   
   m_status = "Splitting ";
   m_status += InputFileName;
   UpdateData(FALSE);

   // create a winsplit object
   if (pCw = new CWinSplit)
      {                     
      AfxGetApp()->DoWaitCursor(1);      
      GetDlgItem(IDOK)->EnableWindow(FALSE);      

	  SplitFileArray.RemoveAll();
        
      // start the process
      TotalFrames = ret = pCw->SplitDiscrete(InputFileName);
      while(ret)
         {                
         // split each file
         ret = pCw->SplitDiscrete(&SplitFile);
         if (ret == -1)
            {
            AfxMessageBox(IDS_ERRORSPLITTINGFILE, MB_OK);
            break;
            }
         else
            {                      
            m_status = SplitFile;
            m_status = " Created";            
            UpdateData(FALSE);         
			// add the file to the array
            SplitFileArray.Add(SplitFile);			
            }               
         }
      delete pCw;
      AfxGetApp()->DoWaitCursor(-1);      
      GetDlgItem(IDOK)->EnableWindow(TRUE);
      }   	  
   
   // write the current profile
   Profile.WriteInt(IDS_WINSPLIT_SECTION, IDS_WINSPLIT_DELETEFILE, m_deletefile); 
   Profile.WriteInt(IDS_WINSPLIT_SECTION, IDS_WINSPLIT_REMOVEFILE, m_removefile); 
   Profile.WriteInt(IDS_WINSPLIT_SECTION, IDS_WINSPLIT_INSERTFILE, m_insertfile); 
      
   CDialog::OnOK();
   }

BOOL SplitDlg::OnInitDialog()
   {                                  
   CString csSection, csEntry;
   CString Title;
   CProfile Profile;
   
   // default
   TotalFrames = 0;
   
   // set title
   Title.LoadString(IDS_WINSPLIT_SECTION);
   Title += ": ";
   Title += ShortFileName;
   SetWindowText(Title);
   
   CDialog::OnInitDialog();   
   
   // get the current profile
   m_deletefile = Profile.GetInt(IDS_WINSPLIT_SECTION, IDS_WINSPLIT_DELETEFILE);
   m_removefile = Profile.GetInt(IDS_WINSPLIT_SECTION, IDS_WINSPLIT_REMOVEFILE); 
   m_insertfile = Profile.GetInt(IDS_WINSPLIT_SECTION, IDS_WINSPLIT_INSERTFILE);    
   
   UpdateData(FALSE);
	
   CenterWindow();	
   return TRUE;  
   }

void SplitDlg::OnDeletefile()
   {
   // if we are selecting delete, then we must necessarily 
   // remove it from queue
   UpdateData(TRUE);   
   if (m_deletefile) m_removefile = TRUE;
   UpdateData(FALSE);
   }
