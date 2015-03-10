/////////////////////////////////////////////////////////////////////////////
// SCSIExplorer.cpp
//                                                                        
// SCSI Explorer module for RasterPlus95
// Copyright (C) 1997-98 Graphx, Inc.
//
// mods:
//
// 3-12-98  Version 2.0.2 - (for J version) moved string to rc file
// 6-09-98  Version 2.0.4 - change Host ID to Host Adapter ID to match 
//                          Adaptec SCSI Explorer
// 9-08-98  Version 2.0.5 - expand InfoBuffer to account for ASPI bug?
// 11-02-98 Version 2.0.5 - now report vendor and device
// 11-06-98 Version 2.0.5 - 8 char length mfgs would be cut off - fixed
//

#include "stdafx.h"
#include "zeus.h"
#include "SCSIExplorer.h"
#include "..\CmnSrc\scsi\scsi.h"
#include "..\ASPI Drivers\include\srbcmn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSCSIExplorer dialog


CSCSIExplorer::CSCSIExplorer(CWnd* pParent /*=NULL*/)
	: CDialog(CSCSIExplorer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSCSIExplorer)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSCSIExplorer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSCSIExplorer)	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSCSIExplorer, CDialog)
	//{{AFX_MSG_MAP(CSCSIExplorer)
	ON_BN_CLICKED(IDC_RESCAN, OnRescan)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCSIExplorer message handlers

BOOL CSCSIExplorer::OnInitDialog() 
   {
   CDialog::OnInitDialog();

   SetDevices();

   return TRUE;  	             
   }

void CSCSIExplorer::SetDevices()
   {
   int i, j;   
   CSCSI csi;
   int NumAdapters;
   BYTE InfoBuffer[64];
   BYTE MfgBuffer[32];
   BYTE DevBuffer[32];
   CString cs;		   
   BOOL HostDrawn;
   CString csHostID;

   CString csDebug;

   // load wait cursor
   AfxGetApp()->DoWaitCursor(1);

   csHostID.LoadString(IDS_HOST_ADAPTER_ID);

   if (csi.Init())   
	  {
	  NumAdapters = max(min(csi.GetNumAdapters(), 7), 0);

	  // for all adapters
	  for (i = 0; i < NumAdapters; i++)
		 {
		 // set the host id
		 csi.SetHostID(i);
		 HostDrawn = FALSE;
		 // for all devices
		 for (j = 0; j < 8; j++)
			{
			// set the dev id
			csi.SetDevID(j);			
			// zero out the buffer
			memset(InfoBuffer, 0, 32);
			// get the name
			if (csi.Inquiry(InfoBuffer, 32) == SS_COMP)
			   {			   
			   if (!HostDrawn)
				  {				  
				  cs.Format((const char*)csHostID, i);
				  ((CListBox*)GetDlgItem(IDC_SCSI_LIST))->AddString(cs);
				  HostDrawn = TRUE;
				  }
			   // copy manufacturer and device info into 
			   // the appropriate buffers
               memcpy(MfgBuffer, InfoBuffer + 8, 8);
			   MfgBuffer[8] = 0;
               memcpy(DevBuffer, InfoBuffer + 16, 16);
			   DevBuffer[16] = 0;

   			   cs.Format("    Dev ID #%d:     %s\t%s", j, MfgBuffer, DevBuffer);
			   ((CListBox*)GetDlgItem(IDC_SCSI_LIST))->AddString(cs);
			   }
			else
			{
   csDebug.Format("Adapter = %d, DeviceId =  %d Device Found = %s\n", i, j, "False" );
			}
			}
		 }	  
	  }
   else
	  {
			switch (csi.byAspiStatus )
			{
			case SS_INVALID_SRB:            // Invalid parameter set in SRB
			    cs = "Invalid SCSI SRB";
				break;

			case SS_BUFFER_ALIGN:           // Buffer not aligned (replaces OLD_MANAGER in Win32)
			    cs = "Buffer Not Aligned";
				break;

			case SS_ILLEGAL_MODE:           // Unsupported Windows mode
			    cs = "Unsupported Windows Mode";
				break;

			case SS_NO_ASPI:                // No ASPI managers resident
			    cs = "ASPI Manger not resident";
				break;

			case SS_FAILED_INIT:            // ASPI for windows failed init
			    cs = "ASPI Init Failed";
				break;

			case SS_ASPI_IS_BUSY:           // No resources available to execute cmd
			    cs = "ASPI is Busy Retry";
				break;

			case SS_BUFFER_TOO_BIG:         // Buffer size toobig to handle!
			    cs = "Buffer size is too big";
				break;

			case SS_MISMATCHED_COMPONENTS:  // The DLLs/EXEs of ASPI don't version check
			    cs = "ASPI Component mismatch";
				break;

			case SS_NO_ADAPTERS:            // No host adapters to manage
			    cs = "No SCSI Adapters Present";
				break;

			case SS_INSUFFICIENT_RESOURCES:	// Couldn't allocate resources needed to init
			    cs = "Insufficient ASPI Resouces";
				break;

			case SS_ASPI_IS_SHUTDOWN:		// Call came to ASPI after PROCESS_DETACH
			    cs = "ASPI is not active";
				break;

			case SS_BAD_INSTALL:			// The DLL or other components are installed wrong
			    cs = "Incorrect installation";
				break;

			default:
			    cs.LoadString(IDS_NOLOAD_ASPI);
				break;
			}
/*
	   if (csi.byAspiStatus == 0xE8)
	   {
		   cs = "No SCSI Adapters Present";
	   }
	   else
	   {
		   cs.LoadString(IDS_NOLOAD_ASPI);
	   }
*/

	  ((CListBox*)GetDlgItem(IDC_SCSI_LIST))->AddString(cs);	  
	  }
 
   // reset cursor      
   AfxGetApp()->DoWaitCursor(-1);
   }

void CSCSIExplorer::OnRescan() 
   {
   // reset the list
   ((CListBox*)GetDlgItem(IDC_SCSI_LIST))->ResetContent();
   // reset the device list
   SetDevices();
   }
