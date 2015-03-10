/////////////////////////////////////////////////////////////////////////////
// cabinstall.cpp
//                                                                        
// cabinet dialog handler for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//
// mods:
//
// 10-15-96 Build 45 - check for validity of installlocs array (not used in NT)

#include "stdafx.h"
#include "zeus.h"
#include "cabinstall.h"
#include "rexdirdi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCabInstall dialog


CCabInstall::CCabInstall(CWnd* pParent /*=NULL*/)
	: CDialog(CCabInstall::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCabInstall)
	m_prompt1 = _T("");
	m_prompt2 = _T("");
	m_path = _T("");
	//}}AFX_DATA_INIT
}


void CCabInstall::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCabInstall)
	DDX_Text(pDX, IDC_PROMPT1, m_prompt1);
	DDX_Text(pDX, IDC_PROMPT2, m_prompt2);
	DDX_CBString(pDX, IDC_PATH, m_path);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCabInstall, CDialog)
	//{{AFX_MSG_MAP(CCabInstall)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCabInstall message handlers

void CCabInstall::OnBrowse() 
   {
   CString cs;

   cs.LoadString(IDS_SELECT_INSTALL_DIR);

   CRexDirDialog dlg((const char*)cs);      
   dlg.m_ofn.lpstrInitialDir = m_path;   
   dlg.m_ofn.Flags &= ~OFN_EXPLORER;   
   if (dlg.DoModal() == IDOK)            
      {
      m_path = dlg.GetPath(TRUE);
	  // make sure there's a trailing backslash
	  if (m_path[m_path.GetLength() - 1] != '\\' &&	      
		  m_path[m_path.GetLength() - 1] != ':')
		 m_path += "\\";
      UpdateData(FALSE);
      }
   }

BOOL CCabInstall::OnInitDialog() 
   {
   CDialog::OnInitDialog();
	
   int i;
   CString cs;
   CComboBox* cb = (CComboBox*)GetDlgItem(IDC_PATH);

   if (pcsaInstallLocs)
      {
	  for (i = 0; i < pcsaInstallLocs->GetSize(); i++)
         {
         cs = pcsaInstallLocs->GetAt(i);
         cb->InsertString(-1, cs);
         }
	  }
   // select the first entry
   cb->SetCurSel(0);   
   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
   }

void CCabInstall::OnOK() 
   {
   // check one more time
   UpdateData(TRUE);
   if (m_path[m_path.GetLength() - 1] != '\\' &&	      
 	   m_path[m_path.GetLength() - 1] != ':')
	   m_path += "\\";      	
   UpdateData(FALSE);
   CDialog::OnOK();
   }
