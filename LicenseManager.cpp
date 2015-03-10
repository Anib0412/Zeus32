// LicenseManager.cpp : implementation file
//

#include "stdafx.h"
#include "zeus.h"
#include "LicenseManager.h"
#include "..\pcgi\pcgi.h"
#include "webservicesasp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLicenseManager dialog


CLicenseManager::CLicenseManager(CWnd* pParent /*=NULL*/)
	: CDialog(CLicenseManager::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLicenseManager)
	m_daysleft = _T("");
	m_email = _T("");
	m_mid = _T("");
	m_name = _T("");
	m_serialnumber = _T("");
	m_sitecode = _T("");
	m_status = _T("");
	m_company = _T("");
//	m_workforcestatus = _T("");
//	m_cdproducerstatus = _T("");
	m_nointernet = FALSE;
	m_activationcode = _T("");
//	m_specialtystatus = _T("");
	//}}AFX_DATA_INIT
}


void CLicenseManager::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLicenseManager)
	DDX_Text(pDX, IDC_DAYSLEFT, m_daysleft);
	DDX_Text(pDX, IDC_EMAIL, m_email);
	DDX_Text(pDX, IDC_MID, m_mid);
	DDX_Text(pDX, IDC_NAME, m_name);
	DDX_Text(pDX, IDC_SERIALNUMBER, m_serialnumber);
	DDX_Text(pDX, IDC_SITECODE, m_sitecode);
	DDX_Text(pDX, IDC_STATUS, m_status);
	DDX_Text(pDX, IDC_COMPANY, m_company);
//	DDX_Text(pDX, IDC_WORKFORCESTATUS, m_workforcestatus);
//	DDX_Text(pDX, IDC_CDPRODUCERSTATUS, m_cdproducerstatus);
	DDX_Check(pDX, IDC_NOINTERNET, m_nointernet);
	DDX_Text(pDX, IDC_ACTIVATIONCODE, m_activationcode);
//	DDX_Text(pDX, IDC_SPECIALTYSTATUS, m_specialtystatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLicenseManager, CDialog)
	//{{AFX_MSG_MAP(CLicenseManager)
	ON_BN_CLICKED(IDC_ACTIVATE, OnActivate)
	ON_EN_CHANGE(IDC_SERIALNUMBER, OnChangeSerialnumber)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_EN_CHANGE(IDC_EMAIL, OnChangeEmail)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_NOINTERNET, OnNointernet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLicenseManager message handlers

void CLicenseManager::GetStatus(BOOL& bAppActive, BOOL& bWFActive) 
{
	PCG_INTERFACE  PCGInterface;
	int	 InterfaceStatus;

	bAppActive = FALSE;
	bWFActive = FALSE;

	GetDlgItem(IDC_REMOVE)->EnableWindow(FALSE);   

	// structure size must be set before calling interface functions
	PCGInterface.PCGI_Size = sizeof (PCG_INTERFACE);

	// get interface data
	InterfaceStatus = GetInterfaceData(&PCGInterface);

	// check return value
	if (InterfaceStatus != PCGI_STATUS_OK)
	{
		AfxMessageBox("License Manager error!", MB_OK);
		return;
	}	

	m_sitecode.Format("%08X", PCGInterface.PCGI_SiteCode);

	m_mid.Format("%04X-%04X-%04X-%04X", PCGInterface.PCGI_MachineID[0], 
	PCGInterface.PCGI_MachineID[1], PCGInterface.PCGI_MachineID[2], PCGInterface.PCGI_MachineID[3]);

	if (!PCGInterface.PCGI_ApplicationStatus)
	{
		m_daysleft = "default";

		m_status = "Application is not activated!";
		if (PCGInterface.PCGI_DemoDaysLeft!=0xFFFFFFFF)
		{
			m_daysleft.Format("%u", PCGInterface.PCGI_DemoDaysLeft);
		}
	}
	else
	{
		bAppActive = TRUE;
		m_status = "Application is activated";	  
		m_daysleft.Empty();
		GetDlgItem(IDC_REMOVE)->EnableWindow(TRUE);   
	}	  

	UpdateData(FALSE);
}

BOOL CLicenseManager::OnInitDialog() 
   {      
   CDialog::OnInitDialog();
   BOOL bAppActive;
   BOOL bWFActive;

   // get pz defaults
   m_name			= Profile.GetStr("User Information", "User Name", "");	
   m_company		= Profile.GetStr("User Information", "User Organization", "");	
   m_email			= Profile.GetStr("User Information", "Email", "");	
   m_serialnumber	= Profile.GetStr("User Information", "User SerialNumber", "");	

   // get from rp if necessary
//   if (m_name.IsEmpty() || m_serialnumber.IsEmpty())
//	  CPhotogizeUtils::GetRPRegInfo(m_name, m_company, m_serialnumber);

   GetStatus(bAppActive, bWFActive);	

   UpdateData(FALSE);

   if (m_name.IsEmpty() || m_email.IsEmpty() || m_serialnumber.IsEmpty())
	  {
	  GetDlgItem(IDC_ACTIVATE)->EnableWindow(FALSE);   
	  }
   else if (!bAppActive || !bWFActive)
	  {
	  GetDlgItem(IDC_ACTIVATE)->EnableWindow(TRUE);   
	  }

   GetDlgItem(IDC_ACTIVATIONCODE)->EnableWindow(FALSE);   
   GetDlgItem(IDC_ACTIVATIONCODE_PRMPT)->EnableWindow(FALSE);   

/*
   //create the control   
   ToolTipCtrl.Create(this, TTS_ALWAYSTIP | TTS_BALLOON);	     
   // enable tool tips
   EnableToolTips();

   char* pTip1 = 
	  "<b>RasterPlus will operate for 14 days<br>"
	  "before it locks and becomes unusable.<b>  <br>"
	  "To unlock Lab, you must acquire a valid <br>"
	  "Serial Number from the Graphx sales department.<br>"
	  "Once you have a serial number, enter it in the<br>"
	  "specified field and click Activate.";
   ToolTipCtrl.AddTool(GetDlgItem(IDC_STATUS), pTip1);
   ToolTipCtrl.AddTool(GetDlgItem(IDC_STATUS_PRMPT), pTip1);

   char* pTip2 = 
	  "<b>You can use WorkForce to do 1000 transformations<br>"
	  "before it locks and becomes unusable.<b>  <br>"
	  "To unlock WorkForce, contact the Graphx<br>"
	  "sales department at 781.932.0430";
   ToolTipCtrl.AddTool(GetDlgItem(IDC_WORKFORCESTATUS), pTip2);
   ToolTipCtrl.AddTool(GetDlgItem(IDC_WORKFORCESTATUS_PRMPT), pTip2);

   char* pTip3 = "A number used to identify this specific PC and operating system";
   ToolTipCtrl.AddTool(GetDlgItem(IDC_MID), pTip3);
   ToolTipCtrl.AddTool(GetDlgItem(IDC_MID_PRMPT), pTip3);
   ToolTipCtrl.AddTool(GetDlgItem(IDC_SITECODE), pTip3);
   ToolTipCtrl.AddTool(GetDlgItem(IDC_SITECODE_PRMPT), pTip3);

   char* pTip4 = "The number of days left before Lab locks and is unusable";
   ToolTipCtrl.AddTool(GetDlgItem(IDC_DAYSLEFT), pTip4);
   ToolTipCtrl.AddTool(GetDlgItem(IDC_DAYSLEFT_PRMPT), pTip4);

   char* pTip5 = 
	  "<b>Specialty Product support must be enabled before you can create and publish<br>"
	  "Specialty Products to PhotoCentral.<b>  <br>"
	  "To add Specialty Product support, contact the Graphx<br>"
	  "sales department at 781.932.0430";
   ToolTipCtrl.AddTool(GetDlgItem(IDC_SPECIALTYSTATUS), pTip5);
   ToolTipCtrl.AddTool(GetDlgItem(IDC_SPECIALTYSTATUS_PRMPT), pTip5);

   ToolTipCtrl.AddTool(GetDlgItem(IDC_NOINTERNET), 
	  "If you don't have an Internet connection, <br>"
	  "check this box and manually enter the <br>"
	  "activation code you receive from Graphx.");

   ToolTipCtrl.AddTool(GetDlgItem(IDC_REMOVE), 
	  "You can de-activate Lab on this PC by <br>"
	  "clicking Remove.  You will be given a <br>"
	  "Remove Code when the de-activation is <br>"
	  "complete.  Be sure to record this code <br>"
	  "before clicking OK.");   

   ToolTipCtrl.AddTool(GetDlgItem(IDC_ACTIVATE), "Click to Activate!");
*/
   return TRUE;
   }

void CLicenseManager::OnChangeSerialnumber() 
   {
   UpdateData(TRUE);

   if (m_name.IsEmpty() || m_email.IsEmpty() || m_serialnumber.IsEmpty())
	  {
	  GetDlgItem(IDC_ACTIVATE)->EnableWindow(FALSE);   
	  }
   else
	  {
	  GetDlgItem(IDC_ACTIVATE)->EnableWindow(TRUE);   
	  }
   }

void CLicenseManager::OnChangeName() 
   {
   UpdateData(TRUE);

   if (m_name.IsEmpty())
	  {
	  GetDlgItem(IDC_ACTIVATE)->EnableWindow(FALSE);   
	  }
   else
	  {
	  GetDlgItem(IDC_ACTIVATE)->EnableWindow(TRUE);   
	  }
   }

void CLicenseManager::OnChangeEmail() 
   {
   UpdateData(TRUE);

   if (m_email.IsEmpty())
	  {
	  GetDlgItem(IDC_ACTIVATE)->EnableWindow(FALSE);   
	  }
   else
	  {
	  GetDlgItem(IDC_ACTIVATE)->EnableWindow(TRUE);   
	  }
   }

void CLicenseManager::OnActivate() 
   {
   BOOL bRet;
   CString csActivationCode;
   PCG_INTERFACE  PCGInterface;
   int	 InterfaceStatus;   
   BOOL bSuccess = FALSE;
   CString csErr;
   CString csPrompt;
   int iRet;
   BOOL bAppActive;
   BOOL bWFActive;

   UpdateData(TRUE);

   // write the input strings
   Profile.WriteStr("User Information", "User Name", m_name);	
   Profile.WriteStr("User Information", "User Organization", m_company);	
   Profile.WriteStr("User Information", "Email", m_email);	
   Profile.WriteStr("User Information", "User SerialNumber", m_serialnumber);	
/*
   AfxGetApp()->WriteProfileString("User Information", "User Name", m_name);	
   AfxGetApp()->WriteProfileString("User Information", "User Organization", m_company);	
   AfxGetApp()->WriteProfileString("User Information", "Email", m_email);	
   AfxGetApp()->WriteProfileString("User Information", "User SerialNumber", m_serialnumber);	   
*/

   if (m_nointernet)
	  {
	  bRet = TRUE;
	  csActivationCode = m_activationcode;
	  }
   else
	  {
	  bRet = GetActivationCode(csActivationCode, csErr);
	  }

   if (bRet)
	  {
	  // structure size must be set before calling interface functions
	  PCGInterface.PCGI_Size = sizeof (PCG_INTERFACE);
	  // get interface data

	  InterfaceStatus = GetInterfaceData(&PCGInterface);
	  // check return value
//	  if (InterfaceStatus == PCGI_STATUS_OK)
	  if (InterfaceStatus == PCGI_STATUS_OK || InterfaceStatus == PCGI_ERROR_ALREADY_UNLOCKED )
		 {
		 iRet = UnlockApplication((char*)(const char*)csActivationCode);

		 if (iRet == PCGI_STATUS_OK || iRet == PCGI_SUCCESS_APPLICATION_UNLOCKED)
			{
			AfxMessageBox("RasterPlus is activated!");
			bSuccess = TRUE; 	
			GetStatus(bAppActive, bWFActive);	
			if (!bAppActive || !bWFActive)
			   GetDlgItem(IDC_ACTIVATE)->EnableWindow(TRUE);
			else
			   GetDlgItem(IDC_ACTIVATE)->EnableWindow(FALSE);
			m_daysleft.Empty();
			UpdateData(FALSE);
			//PostMessage(WM_CLOSE);
			}
		 else
			{
			csPrompt.Format("Activation Error %d", iRet);
			AfxMessageBox(csPrompt);
			}
		 }
	  }
   if (!bSuccess)
	  {
	  if (csErr.IsEmpty())
	     AfxMessageBox("Error Activating RasterPlus!", MB_OK);
	  else
		 {
		 csPrompt.Format("Error Activating RasterPlus (%s)", csErr);
		 AfxMessageBox(csPrompt, MB_OK);
		 }
	  }
   }							

BOOL CLicenseManager::GetActivationCode(CString& csActivationCode, CString& csErr)
   {   
   BOOL bRet = FALSE;
   CString csResp;  
   
   UpdateData(TRUE);

   csActivationCode.Empty();	   

#ifdef _DEBUG
   m_mid = "A9CC-D8DC-6A76-9D60";
   m_sitecode = "28AF08DF";
#endif

   CWebServicesASP cweb("www.photogize.com", "/activate/pz_activate.asp");		 	  
   cweb.AddFormItem("name", m_name);
   cweb.AddFormItem("email", m_email);
   cweb.AddFormItem("company", m_company);
   cweb.AddFormItem("serialnumber", m_serialnumber);
   cweb.AddFormItem("mid", m_mid);
   cweb.AddFormItem("sitecode", m_sitecode);
   cweb.AddFormItem("version", "1");
   cweb.AddFormItem("passcode", "Ale3~fktu");
   cweb.AddFormItem("application", "RPW");
   cweb.PostFormData(TRUE);
   if (cweb.GetSingleResponse(csResp) == WEBSERVICESASP_RESP_OK)
	  {
	  csActivationCode = csResp;
	  bRet = TRUE;
	  }
   else	
	  csErr = csResp;  

   cweb.Reset();

   return bRet;
   }

void CLicenseManager::OnRemove() 
   {
   DWORD dwRemoveCode;
   PCG_INTERFACE  PCGInterface;
   int	 InterfaceStatus;   
   BOOL bSuccess = FALSE;
   CString csErr;
   CString csPrompt;
   int iRet;

   BOOL bRet = FALSE;
   CString csRemoveCode;
   CString csResp;  
   
   csRemoveCode.Empty();	   

   if (AfxMessageBox("Are you sure you want to deactivate RasterPlus?", 
	  MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION | MB_SETFOREGROUND) == IDYES)
	  {
	  // structure size must be set before calling interface functions
	  PCGInterface.PCGI_Size = sizeof (PCG_INTERFACE);
	  // get interface data

	  InterfaceStatus = GetInterfaceData(&PCGInterface);
	  // check return value
	  if (InterfaceStatus == PCGI_STATUS_OK)
		 {		 
		 iRet = RemoveLicense(&dwRemoveCode);

		 if (iRet == PCGI_STATUS_OK)
			{
			csPrompt.Format("RasterPlus is deactivated, the Remove Code is %X.  RECORD THIS CODE before clicking OK!.  RasterPlus will terminate.", dwRemoveCode);
			AfxMessageBox(csPrompt, MB_OK | MB_SETFOREGROUND);
			// close the app
			(AfxGetApp()->GetMainWnd())->DestroyWindow();
			}
		 else
			AfxMessageBox("Can't remove RasterPlus!", MB_OK | MB_SETFOREGROUND);
		 }
	  }
   }

void CLicenseManager::OnNointernet() 
   {
   UpdateData(TRUE);

   GetDlgItem(IDC_ACTIVATIONCODE)->EnableWindow(m_nointernet);   
   GetDlgItem(IDC_ACTIVATIONCODE_PRMPT)->EnableWindow(m_nointernet);   
   }

BOOL CLicenseManager::PreTranslateMessage(MSG* pMsg) 
   {
//   ToolTipCtrl.RelayEvent(pMsg);	
   return CDialog::PreTranslateMessage(pMsg);
   }

void CLicenseManager::CheckStatus(int& iActState)
{
	CString csResp;
	CString csStatusCode;
	CString csErr;

	PCG_INTERFACE  PCGInterface;
	int	 InterfaceStatus;   

	CString csDebug;

	iActState = 0;	// Default Activation OK

	csDebug.Format("Starting Function(%s)\n", "CLicenseManager::CheckStatus" );
	RPDEBUGSTRMFC(csDebug);

	// structure size must be set before calling interface functions
	PCGInterface.PCGI_Size = sizeof (PCG_INTERFACE);

	// get interface data
	InterfaceStatus = GetInterfaceData(&PCGInterface);

	// check return value
	if (InterfaceStatus != PCGI_STATUS_OK)
	{
		AfxMessageBox("License Manager error!", MB_OK);
		return;
	}	
   
	m_name			= Profile.GetStr("User Information", "User Name", "");	
	m_company		= Profile.GetStr("User Information", "User Organization", "");	
	m_email			= Profile.GetStr("User Information", "Email", "");	
	m_serialnumber	= Profile.GetStr("User Information", "User SerialNumber", "");	

	m_sitecode.Format("%08X", PCGInterface.PCGI_SiteCode);

	m_mid.Format("%04X-%04X-%04X-%04X", PCGInterface.PCGI_MachineID[0], 
		PCGInterface.PCGI_MachineID[1], PCGInterface.PCGI_MachineID[2], PCGInterface.PCGI_MachineID[3]);

#ifdef _DEBUG
   m_mid = "A9CC-D8DC-6A76-9D60";
   m_sitecode = "28AF08DF";
#endif
	csDebug.Format("m_mid = %s   m_sitecode = %s\n", m_mid, m_sitecode );
	RPDEBUGSTRMFC(csDebug);

	csStatusCode.Empty();

	CWebServicesASP cweb("www.photogize.com", "/activate/pz_checkstatus.asp");	
	
	cweb.AddFormItem("name", m_name);
	cweb.AddFormItem("email", m_email);
	cweb.AddFormItem("company", m_company);
	cweb.AddFormItem("serialnumber", m_serialnumber);
	cweb.AddFormItem("mid", m_mid);
	cweb.AddFormItem("sitecode", m_sitecode);
	cweb.AddFormItem("version", "1");
	cweb.AddFormItem("passcode", "Ale3~fktu");
	cweb.AddFormItem("application", "RPW");
	cweb.PostFormData(TRUE);
	int cWebRet = cweb.GetSingleResponse(csResp);
	if (cWebRet == WEBSERVICESASP_RESP_OK)
	{
		csStatusCode = csResp;
		iActState = atoi(csStatusCode);
	}
	else	
		csErr = csResp;

	csDebug.Format("csResp = %s\n", csResp );
	RPDEBUGSTRMFC(csDebug);

	csDebug.Format("Function(%s)  Complete\n", "CLicenseManager::CheckStatus" );
	RPDEBUGSTRMFC(csDebug);

	return;
}

void CLicenseManager::AutoRemove() 
{
	DWORD dwRemoveCode;
	PCG_INTERFACE  PCGInterface;
	int	 InterfaceStatus;   
	BOOL bSuccess = FALSE;
	CString csErr;
	CString csPrompt;
	CString csResp;  
	CString csSerNumber;
	int iRet;

	// structure size must be set before calling interface functions
	PCGInterface.PCGI_Size = sizeof (PCG_INTERFACE);
	// get interface data
	InterfaceStatus = GetInterfaceData(&PCGInterface);
	// check return value
	if (InterfaceStatus == PCGI_STATUS_OK)
	{		 
		iRet = RemoveLicense(&dwRemoveCode);

		if (iRet == PCGI_STATUS_OK)
		{
			csPrompt.Format("Serial Number %s is marked as Inactive.\r\nRasterPlus Has Been Deactivated.\r\nRasterPlus will terminate.\r\n\r\nPlease Contact Graphx if you have any questions.\r\nThank You.", m_serialnumber);
			AfxMessageBox(csPrompt, MB_OK | MB_SETFOREGROUND);

			csSerNumber = "DNR" + m_serialnumber;

			CWebServicesASP cweb("www.photogize.com", "/activate/pz_remove.asp");		 	  
			cweb.AddFormItem("name", m_name);
			cweb.AddFormItem("email", m_email);
			cweb.AddFormItem("company", m_company);
			cweb.AddFormItem("serialnumber", csSerNumber);
			cweb.AddFormItem("mid", m_mid);
			cweb.AddFormItem("sitecode", m_sitecode);
			cweb.AddFormItem("version", "1");
			cweb.AddFormItem("passcode", "Ale3~fktu");
			cweb.AddFormItem("application", "RPW");
			cweb.PostFormData(TRUE);
			int cWebRet = cweb.GetSingleResponse(csResp);
		}
	}
}


