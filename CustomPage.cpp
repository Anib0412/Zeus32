// CustomPage.cpp : implementation file
//

#include "stdafx.h"
#include "zeus.h"
#include "CustomPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define M_INCH_USEINCHES   0
#define M_INCH_USEMM       1

/////////////////////////////////////////////////////////////////////////////
// CCustomPage dialog


CCustomPage::CCustomPage(CWnd* pParent /*=NULL*/)
	: CDialog(CCustomPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCustomPage)
	m_height = _T("");
	m_width = _T("");
	m_inch = -1;
	m_xoffset = _T("");
	m_yoffset = _T("");
	//}}AFX_DATA_INIT
}


void CCustomPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomPage)
	DDX_Text(pDX, IDC_HEIGHT_EDIT, m_height);
	DDX_Text(pDX, IDC_WIDTH_EDIT, m_width);
	DDX_CBIndex(pDX, IDC_UNITS, m_inch);
	DDX_Text(pDX, IDC_XOFFSET_EDIT, m_xoffset);
	DDX_Text(pDX, IDC_YOFFSET_EDIT, m_yoffset);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustomPage, CDialog)
	//{{AFX_MSG_MAP(CCustomPage)
	ON_NOTIFY(UDN_DELTAPOS, IDC_WIDTH_SPIN, OnDeltaposWidthSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HEIGHT_SPIN, OnDeltaposHeightSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_XOFFSET_SPIN, OnDeltaposXoffsetSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_YOFFSET_SPIN, OnDeltaposYoffsetSpin)
	ON_CBN_SELCHANGE(IDC_UNITS, OnSelchangeUnits)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomPage message handlers

BOOL CCustomPage::OnInitDialog() 
{
   CDialog::OnInitDialog();										   

   m_margins = TRUE;

   // do we support offsets?
   GetDlgItem(IDC_XOFFSET_PRMPT)->EnableWindow(m_bDoOffsets);   
   GetDlgItem(IDC_XOFFSET_SPIN)->EnableWindow(m_bDoOffsets);   
   GetDlgItem(IDC_XOFFSET_EDIT)->EnableWindow(m_bDoOffsets);   
   GetDlgItem(IDC_YOFFSET_PRMPT)->EnableWindow(m_bDoOffsets);   
   GetDlgItem(IDC_YOFFSET_SPIN)->EnableWindow(m_bDoOffsets);   
   GetDlgItem(IDC_YOFFSET_EDIT)->EnableWindow(m_bDoOffsets);   
   
   return TRUE;  
}

void CCustomPage::OnDeltaposWidthSpin(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

   float fval = (float)atof(m_width);
   // increment/decrement
   fval += ((float)(-pNMUpDown->iDelta)) * .1F;
   // put back in
   m_width.Format("%.3f", fval);      

   // apply this      
   UpdateData(FALSE);    
   
   *pResult = 0;
   }

void CCustomPage::OnDeltaposHeightSpin(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

   float fval = (float)atof(m_height);
   // increment/decrement
   fval += ((float)(-pNMUpDown->iDelta)) * .1F;
   // put back in
   m_height.Format("%.3f", fval);      

   // apply this      
   UpdateData(FALSE);    
   
   *pResult = 0;
   }

void CCustomPage::OnDeltaposXoffsetSpin(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

   float fval = (float)atof(m_xoffset);
   // increment/decrement
   fval += ((float)(-pNMUpDown->iDelta)) * .1F;

   // bounds check (this is Encad specific, we need to change this)
   if (m_inch == M_INCH_USEMM)
	  {
      if (fval < m_minxoffsetmm) fval = m_minxoffsetmm;
	  }
   else
	  {
	  if (fval < m_minxoffset) fval = m_minxoffset;
	  }

   // put back in
   m_xoffset.Format("%.3f", fval);      

   // apply this   
   UpdateData(FALSE);    
   
   *pResult = 0;
   }

void CCustomPage::OnDeltaposYoffsetSpin(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

   float fval = (float)atof(m_yoffset);
   // increment/decrement
   fval += ((float)(-pNMUpDown->iDelta)) * .1F;

   // bounds check (this is Encad specific, we need to change this)
   if (m_inch == M_INCH_USEMM)
	  {
      if (fval < m_minyoffsetmm) fval = m_minyoffsetmm;
	  }
   else
	  {
	  if (fval < m_minyoffset) fval = m_minyoffset;
	  }

   // put back in
   m_yoffset.Format("%.3f", fval);      

   // apply this   
   UpdateData(FALSE);    
   
   *pResult = 0;
   }

void CCustomPage::OnSelchangeUnits() 
   {
   float fval;
   BOOL bOldVal = m_inch;
   UpdateData(TRUE);
			  
   if (bOldVal != M_INCH_USEINCHES && m_inch == M_INCH_USEINCHES)
	  {
	  //UpdateData(TRUE);

      fval = (float)atof(m_width);
	  fval /= 25.4F;
      // put back in
      sprintf(m_width.GetBuffer(64), "%.3f", fval);   
      m_width.ReleaseBuffer();   

      fval = (float)atof(m_height);
	  fval /= 25.4F;
      // put back in
      sprintf(m_height.GetBuffer(64), "%.3f", fval);   
      m_height.ReleaseBuffer();   

      fval = (float)atof(m_xoffset);
	  fval /= 25.4F;
      // put back in
      sprintf(m_xoffset.GetBuffer(64), "%.3f", fval);   
      m_xoffset.ReleaseBuffer();   

      fval = (float)atof(m_yoffset);
	  fval /= 25.4F;
      // put back in
      sprintf(m_yoffset.GetBuffer(64), "%.3f", fval);   
      m_yoffset.ReleaseBuffer();   

	  m_inch = M_INCH_USEINCHES;	  
	  
	  UpdateData(FALSE);
	  }
   else if (bOldVal != M_INCH_USEMM && m_inch == M_INCH_USEMM)
	  {
	  //UpdateData(TRUE);

      fval = (float)atof(m_width);
	  fval *= 25.4F;
      // put back in
      sprintf(m_width.GetBuffer(64), "%.3f", fval);   
      m_width.ReleaseBuffer();   

      fval = (float)atof(m_height);
	  fval *= 25.4F;
      // put back in
      sprintf(m_height.GetBuffer(64), "%.3f", fval);   
      m_height.ReleaseBuffer();   

      fval = (float)atof(m_xoffset);
	  fval *= 25.4F;
      // put back in
      sprintf(m_xoffset.GetBuffer(64), "%.3f", fval);   
      m_xoffset.ReleaseBuffer();   

      fval = (float)atof(m_yoffset);
	  fval *= 25.4F;
      // put back in
      sprintf(m_yoffset.GetBuffer(64), "%.3f", fval);   
      m_yoffset.ReleaseBuffer();   

	  m_inch = M_INCH_USEMM;	  

	  UpdateData(FALSE);
	  }
   }

void CCustomPage::OnOK() 
   {
   float fThisWidth;
   int fWidthToUse;

   UpdateData(TRUE);

   // compute max print width
   if (m_inch == M_INCH_USEMM)
	  {		
	  fWidthToUse = m_mediawidthmm;
	  if (m_margins)			
		 {
		 fThisWidth = (float)atof(m_width);
		 }
	  else			
		 {
		 fThisWidth = (float)atof(m_width) + (float)atof(m_xoffset) - m_minxoffsetmm;
		 }
	  }			
   else
	  {
	  fWidthToUse = m_mediawidth;
	  if (m_margins)			
		 {
		 fThisWidth = (float)atof(m_width);
		 }
	  else
		 {
		 fThisWidth = (float)atof(m_width) + (float)atof(m_xoffset) - m_minxoffset;
		 }
	  }
  if (fThisWidth > fWidthToUse)
	 {
	 AfxMessageBox(IDS_EXCEED_WIDTH, MB_OK | MB_SETFOREGROUND);
	 return;
	 }
   CDialog::OnOK();
   }