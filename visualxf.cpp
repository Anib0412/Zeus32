/////////////////////////////////////////////////////////////////////////////
// visualxf.cpp
//                                                                        
// visual transform dialog for RasterPlus
// Copyright (C) 1996-2000 Graphx, Inc.
//
// mods:
//
// 6-28-96  Build 40 - must update DefaultTransform before doing SaveAs
// 7-12-96  Build 40 - problem loading transforms; nums got nuked - fixed
// 7-12-96  Build 40 - added timer for updating zooms cleanly
// 6-09-97  Build 50 - added stuff for non-square resolutions
// 9-09-97  Build 51 - fixed CanRect sizing bug...
//             here's the supidity:
//	           CanRect.top += (CanRect.Height() - CanRect.Width()) / 2;
//	           CanRect.bottom -= (CanRect.Height() - CanRect.Width()) / 2; // whoops!
//
//             changed to:
//             CRect cTempRect = CanRect;
//	           CanRect.top += (cTempRect.Height() - cTempRect.Width()) / 2;
//	           CanRect.bottom -= (cTempRect.Height() - cTempRect.Width()) / 2;
// 9-14-97  Build 51 - changed SCALEFACTOR back to 1 as it now seems unecessary in light
//                        of the fixed computation stuff    
// 9-26-97  Build 51 - separated preview window
//                     fixed OnReset         
// 3-3-98   Version 2.0.2 - changed all floats from 2 to 3 decimal places
// 4-06-98  Version 2.0.3 - redo rulers to reduce number "crunching"
// 6-19-98  Version 2.0.4 - more ruler rework
// 4-27-99  Version 3.0.0 - change default units to inches
// 10-12-99 Version 3.0.1 - fix for custom queue props sizing crash
// 5-04-00  Version 3.0.3 - ruler dims are reversed - fixed
// 7-10-00  Version 4.0 - previews could get too big on some devices - fixed
// 7-13-00  Version 4.0 - fixed serious memory leaks with Last... vars.  Would suck up
//                        memory after a while
// 7-24-00  Version 4.0.0 - now zoom from the center  
// 8-04-00  Version 4.0.0 - now specify height and width of image
// 8-04-00  Version 4.0.0 - fixed lots of squirrely bugs
// 9-09-02  Version 5.0.7 - Fix indeterminant state processing
//

#include "stdafx.h"
#include "zeus.h"
#include "visualxf.h"
#include "canvas.h"
#include "inidefs.h"
#include "ctm2rect.h"  
#include "prev.h"
#include "ripmsg.h"
#include "profile.h"
#include "loadtransform.h"
#include "roundit.h"
#include "xformsave.h"
#include "control.hm"
#include "prefsmsg.h"

#define XFORM_UPDATEDELAY   500

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT NEAR wm_PrevSetInfo = RegisterWindowMessage( PREV_SETINFO );
const UINT NEAR wm_RipSendMsg = RegisterWindowMessage( RIP_SENDMESSAGE );
const UINT NEAR wm_RipComplete = RegisterWindowMessage( RIP_COMPLETE );
const UINT NEAR wm_DevModeChange = RegisterWindowMessage( ID_DEVMODE_CHANGE );

// the control help-map
static DWORD dwHelpMap[] = 
   {
   IDC_XFORM_BOX, HIDC_XFORM_BOX,
   IDC_TRANSFORM, HIDC_XFORM_BOX,
   IDC_PREVIEW_SIZE, HIDC_PREVIEW_SIZE,
   IDC_PREVIEW_SIZE_PRMPT, HIDC_PREVIEW_SIZE,
   IDC_XOFF_EDIT, HIDC_VISUALXF_XOFF,
   IDC_XOFF_SPIN, HIDC_VISUALXF_XOFF,
   IDC_XOFF_PROMPT, HIDC_VISUALXF_XOFF,
   IDC_YOFF_EDIT, HIDC_VISUALXF_YOFF,
   IDC_YOFF_SPIN, HIDC_VISUALXF_YOFF,
   IDC_YOFF_PROMPT, HIDC_VISUALXF_YOFF,
   IDC_XZOOM_EDIT, HIDC_VISUALXF_XZOOM,
   IDC_XZOOM_SPIN, HIDC_VISUALXF_XZOOM,
   IDC_XZOOM_PROMPT, HIDC_VISUALXF_XZOOM,
   IDC_YZOOM_EDIT, HIDC_VISUALXF_YZOOM,
   IDC_YZOOM_SPIN, HIDC_VISUALXF_YZOOM,
   IDC_YZOOM_PROMPT, HIDC_VISUALXF_YZOOM,
   IDC_HEIGHT_PROMPT, HIDC_VISUALXF_HEIGHT,
   IDC_HEIGHT_EDIT, HIDC_VISUALXF_HEIGHT,
   IDC_HEIGHT_SPIN, HIDC_VISUALXF_HEIGHT,
   IDC_WIDTH_PROMPT, HIDC_VISUALXF_WIDTH,
   IDC_WIDTH_EDIT, HIDC_VISUALXF_WIDTH,
   IDC_WIDTH_SPIN, HIDC_VISUALXF_WIDTH,
   IDC_ROTATION, HIDC_VISUALXF_ROTATION,   
   IDC_ROTATION_PROMPT, HIDC_VISUALXF_ROTATION,
   ID_REPAINT, HIDC_VISUALXF_REPAINT,
   IDC_LOAD, HIDC_VISUALXF_LOAD,
   IDC_SAVEAS, HIDC_VISUALXF_SAVEAS,
   IDC_RESET, HIDC_VISUALXF_RESET,
   IDC_SNAP, HIDC_VISUALXF_SNAP,
   IDC_RETAIN_ASPECT, HIDC_VISUALXF_RETAIN_ASPECT,      
   IDC_TARGETFILE, HIDC_VISUALXF_TARGETFILE,
   IDC_TARGETFILE_PROMPT, HIDC_VISUALXF_TARGETFILE,
   IDC_TARGETDEVICE, HIDC_VISUALXF_TARGETDEVICE,
   IDC_TARGETDEVICE_PROMPT, HIDC_VISUALXF_TARGETDEVICE,
   IDC_RIP_STATUS_PRMPT, HIDC_VISUALXF_RIP_STATUS,
   IDC_RIP_STATUS_PROGRESS, HIDC_VISUALXF_RIP_STATUS,
   IDC_DESCRIPTION, HIDC_VISUALXF_DESCRIPTION,
   IDC_START_TYPE, HIDC_VISUALXF_START_TYPE,
   IDC_START_TYPE_PROMPT, HIDC_VISUALXF_START_TYPE,
   IDC_UNITS, HIDC_VISUALXF_UNITS,
   IDC_CANVAS, HIDC_VISUALXF_CANVAS,
   IDOK, HIDC_OUROK,
   IDCANCEL, HIDC_OURCANCEL,   
   0,0
   }; 

IMPLEMENT_DYNCREATE(CJobSizeProps, CPropertyPage)
   
/////////////////////////////////////////////////////////////////////////////
// CJobSizeProps dialog

CJobSizeProps::CJobSizeProps() : CPropertyPage(CJobSizeProps::IDD)	
   {
   DialogInit = FALSE;
   CustomEdit = FALSE;
   m_xzoom_is_indeterminate = FALSE;
   m_yzoom_is_indeterminate = FALSE;
   m_xoff_is_indeterminate = FALSE;
   m_yoff_is_indeterminate = FALSE;
   m_rotation_is_indeterminate = FALSE;
   DoingDefaultProps = FALSE;
   pTargetCQueue = NULL;
   pTargetDevice = NULL;
   bVarsSet = FALSE;
   pReportWnd = NULL;
   bLastMultipleSelection = FALSE;   
   bForceShow = FALSE;
   TimerID = 0;
   OldCanRect.top = OldCanRect.bottom = 0;
   OldCanRect.left = OldCanRect.right = 0;

   // load the preview factors
   fPrevFactor[0] = .25F;
   fPrevFactor[1] = .5F;
   fPrevFactor[2] = .75F;

   pLastDefaultDevice = NULL;
   pLastDefaultTransform = NULL;
   pLastLocalColor = NULL;
   pLastTargetQueue = NULL;

	//{{AFX_DATA_INIT(CJobSizeProps)
	m_snap = FALSE;
	m_xoff = _T("");
	m_xzoom = _T("");
	m_yoff = _T("");
	m_yzoom = _T("");
	m_targetdevice = _T("");
	m_targetfile = _T("");
	m_start_type = _T("");
	m_description = _T("");
	m_retain_aspect = FALSE;		
	m_rip_status_prmpt = _T("");
	m_rotation = -1;
	m_units = -1;	
	m_name = -1;
	m_preview_size = -1;
	m_width = _T("");
	m_height = _T("");
	//}}AFX_DATA_INIT
}                                                    

void CJobSizeProps::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJobSizeProps)
	DDX_Control(pDX, IDC_RIP_STATUS_PROGRESS, m_rip_status_progress);
	DDX_Check(pDX, IDC_SNAP, m_snap);
	DDX_Text(pDX, IDC_XOFF_EDIT, m_xoff);
	DDX_Text(pDX, IDC_XZOOM_EDIT, m_xzoom);
	DDX_Text(pDX, IDC_YOFF_EDIT, m_yoff);
	DDX_Text(pDX, IDC_YZOOM_EDIT, m_yzoom);
	DDX_Text(pDX, IDC_TARGETDEVICE, m_targetdevice);
	DDX_Text(pDX, IDC_TARGETFILE, m_targetfile);
	DDX_CBString(pDX, IDC_START_TYPE, m_start_type);
	DDX_Text(pDX, IDC_DESCRIPTION, m_description);
	DDX_Check(pDX, IDC_RETAIN_ASPECT, m_retain_aspect);
	DDX_Text(pDX, IDC_RIP_STATUS_PRMPT, m_rip_status_prmpt);
	DDX_CBIndex(pDX, IDC_ROTATION, m_rotation);
	DDX_CBIndex(pDX, IDC_UNITS, m_units);	
	DDX_CBIndex(pDX, IDC_TRANSFORM, m_name);
	DDX_CBIndex(pDX, IDC_PREVIEW_SIZE, m_preview_size);
	DDX_Text(pDX, IDC_WIDTH_EDIT, m_width);
	DDX_Text(pDX, IDC_HEIGHT_EDIT, m_height);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CJobSizeProps, CPropertyPage)
	//{{AFX_MSG_MAP(CJobSizeProps)	
	ON_BN_CLICKED(IDC_SNAP, OnSnap)
	ON_EN_CHANGE(IDC_XOFF_EDIT, OnChangeXoffEdit)
	ON_EN_CHANGE(IDC_XZOOM_EDIT, OnChangeXzoomEdit)
	ON_EN_CHANGE(IDC_YZOOM_EDIT, OnChangeYzoomEdit)
	ON_BN_CLICKED(ID_REPAINT, OnRepaint)
	ON_REGISTERED_MESSAGE(wm_PrevSetInfo, OnPrevSetInfo)			
	ON_REGISTERED_MESSAGE(wm_RipSendMsg, OnRipMessage)
	ON_REGISTERED_MESSAGE(wm_RipComplete, OnRipComplete)	
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_RETAIN_ASPECT, OnRetainAspect)
	ON_EN_CHANGE(IDC_YOFF_EDIT, OnChangeYoffEdit)
	ON_CBN_SELCHANGE(IDC_START_TYPE, OnSelchangeStartType)
	ON_NOTIFY(UDN_DELTAPOS, IDC_XOFF_SPIN, OnDeltaposXoffSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_XZOOM_SPIN, OnDeltaposXzoomSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_YOFF_SPIN, OnDeltaposYoffSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_YZOOM_SPIN, OnDeltaposYzoomSpin)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_WM_CONTEXTMENU()	
	ON_WM_HELPINFO()	
	ON_CBN_SELCHANGE(IDC_ROTATION, OnSelchangeRotation)
	ON_WM_TIMER()	
	ON_CBN_SELCHANGE(IDC_UNITS, OnSelchangeUnits)
	ON_CBN_SELCHANGE(IDC_TRANSFORM, OnSelchangeTransform)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_CBN_SELCHANGE(IDC_PREVIEW_SIZE, OnSelchangePreviewSize)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_HEIGHT_EDIT, OnChangeHeightEdit)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_WIDTH_SPIN, OnDeltaposWidthSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HEIGHT_SPIN, OnDeltaposHeightSpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJobSizeProps message handlers

BOOL CJobSizeProps::OnInitDialog()
   {	
   CPropertyPage::OnInitDialog();		

   xzoom_is_invalid = FALSE;
   yzoom_is_invalid = FALSE;
   xoff_is_invalid = FALSE;
   yoff_is_invalid = FALSE;
   width_is_invalid = FALSE;
   height_is_invalid = FALSE;

   // parent pointer
   cw.pRT->pWnd = this;

   // get all our standard vars
   GetVars();

   // create canvas
   cwCanvasParent.Exists = FALSE;
   VerifyCanvas(); 

   // enable repaint if m_preview is set and we have a Target file
   GetDlgItem(ID_REPAINT)->EnableWindow(m_preview && pTargetCQueue);

   // otherwise, hide the window
   if (!m_preview && cw.GetSafeHwnd()) cw.ShowWindow(SW_HIDE);

   // defaults
   GetDlgItem(IDC_STOP)->ShowWindow(SW_HIDE);   
   GetDlgItem(ID_REPAINT)->ShowWindow(SW_SHOW);    

   // nuke the prompts
   csTemp.LoadString(IDS_QUEUESTATUS_IDLE);
   SetDlgItemText(IDC_RIP_STATUS_PRMPT, csTemp);
   m_rip_status_progress.SetPos(0);		        
   
   // don't need to recommend repainting, yet...
   m_should_repaint = FALSE;

   // load the standard set of transforms
   LoadStandardXforms();
   
   // don't allow preview if doing default props
   if (DoingDefaultProps) 
	  m_preview = FALSE;
   else
	  m_preview = TRUE;

   UpdateIt(bLastMultipleSelection);   

   DialogInit = TRUE;

   return TRUE;  // return TRUE  unless you set the focus to a control
   }

CJobSizeProps::~CJobSizeProps()
   {                     
   cwCanvasParent.DestroyWindow();         

   if (pLastDefaultDevice) delete pLastDefaultDevice;
   if (pLastDefaultTransform) delete pLastDefaultTransform;
   if (pLastLocalColor) delete pLastLocalColor;
   if (pLastTargetQueue) delete pLastTargetQueue; 
   }

void CJobSizeProps::OnOK()
   {
   CleanUp();
   CPropertyPage::OnOK();
   }

void CJobSizeProps::OnSnap()
   {
   CProfile Profile;

   // get data from dialog
   UpdateData(TRUE);		
   // and give it to our canvas 
   cw.m_snap = m_snap;	
   Profile.WriteInt(IDS_TRANSFORMCONFIG, IDS_SNAP, m_snap);
   }

CTransform& CJobSizeProps::GetDefTransform()
   {           
   // return the default transform
   return DefaultTransform;
   }

//#define SCALEFACTOR   2
//#define FSCALEFACTOR   2.F
#define SCALEFACTOR   1
#define FSCALEFACTOR   1.F

void CJobSizeProps::OnRepaint()
   {
   int ret;
   CRect crect;       

   ASSERT(m_preview);

   VerifyCanvas();

   // make sure numbers are updated before repainting
   ValidateAll();							

   cw.m_must_repaint = FALSE;
   m_should_repaint = FALSE;
   
   // get our preview size
   cw.GetClientRect(&crect);

   // if the scratch path is set, load it into crip
   if (csScratchPath.GetLength() > 0)
	  cr.csScratchPath = csScratchPath;

   // open rip object
   cr.Open();

   // get data from dialog
   UpdateData(TRUE);          

   // make sure this is current    
   if (atof((const char*)m_xzoom) >=.1)
      DefaultTransform.XZoom = m_xzoom;
   if (atof((const char*)m_yzoom)>=.1)
      DefaultTransform.YZoom = m_yzoom;

   DefaultTransform.XOff = DisplayTextToPica(m_xoff);
   DefaultTransform.YOff = DisplayTextToPica(m_yoff);

   MapToDegree(DefaultTransform.Rotate, m_rotation);	

   // init rip   
   ret = cr.Init(pTargetDevice, &DefaultTransform, pLocalColor,
      pTargetCQueue, RIPSTYLE_PREVIEW, this, this, TRUE);

   // store these for later check
   if (pLastDefaultDevice) delete pLastDefaultDevice;
   if (pLastDefaultTransform) delete pLastDefaultTransform;
   if (pLastLocalColor) delete pLastLocalColor;
   if (pLastTargetQueue) delete pLastTargetQueue; 

   pLastDefaultDevice = new CDevice;
   pLastDefaultTransform = new CTransform;
   pLastLocalColor = new CColorTransform;
   pLastTargetQueue = new CQueueFile;

   *(pLastDefaultDevice) = *pTargetDevice;
   *(pLastDefaultTransform) = DefaultTransform;
   *(pLastLocalColor) = *pLocalColor;
   *(pLastTargetQueue) = *pTargetCQueue;
   
   // override our preview params and start rip   
   if (ret == SYN_ERRMSG_ALLOK)                          
      {                               
	  // nuke the prompts
	  csTemp.LoadString(IDS_QUEUESTATUS_IDLE);
      SetDlgItemText(IDC_RIP_STATUS_PRMPT, csTemp);
      m_rip_status_progress.SetPos(0);		      
   
      // gray the buttons                               
      GetDlgItem(ID_REPAINT)->ShowWindow(SW_HIDE);      
      GetDlgItem(IDC_STOP)->ShowWindow(SW_SHOW);         
      
      //GetDlgItem(IDOK)->EnableWindow(FALSE);   
      //GetDlgItem(IDCANCEL)->EnableWindow(FALSE);        
      
	  GetDlgItem(IDC_LOAD)->EnableWindow(FALSE);   
	  GetDlgItem(IDC_SAVEAS)->EnableWindow(FALSE);   
      GetDlgItem(IDC_RESET)->EnableWindow(FALSE);        
            
      GetDlgItem(IDC_RETAIN_ASPECT)->EnableWindow(FALSE);   
      GetDlgItem(IDC_SNAP)->EnableWindow(FALSE);  
	  //GetDlgItem(IDC_PREVIEW)->EnableWindow(FALSE); 
   
      GetDlgItem(IDC_WIDTH_PROMPT)->EnableWindow(FALSE);   
      GetDlgItem(IDC_WIDTH_EDIT)->EnableWindow(FALSE);   
      GetDlgItem(IDC_WIDTH_SPIN)->EnableWindow(FALSE);   
      GetDlgItem(IDC_HEIGHT_PROMPT)->EnableWindow(FALSE);   
      GetDlgItem(IDC_HEIGHT_EDIT)->EnableWindow(FALSE);   
      GetDlgItem(IDC_HEIGHT_SPIN)->EnableWindow(FALSE);   
      
      GetDlgItem(IDC_XZOOM_PROMPT)->EnableWindow(FALSE);   
      GetDlgItem(IDC_XZOOM_EDIT)->EnableWindow(FALSE);   
      GetDlgItem(IDC_XZOOM_SPIN)->EnableWindow(FALSE);   
      
      GetDlgItem(IDC_YZOOM_PROMPT)->EnableWindow(FALSE);   
      GetDlgItem(IDC_YZOOM_EDIT)->EnableWindow(FALSE);   
      GetDlgItem(IDC_YZOOM_SPIN)->EnableWindow(FALSE);   
      
      GetDlgItem(IDC_XOFF_PROMPT)->EnableWindow(FALSE);   
      GetDlgItem(IDC_XOFF_EDIT)->EnableWindow(FALSE);   
      GetDlgItem(IDC_XOFF_SPIN)->EnableWindow(FALSE);   
      
      GetDlgItem(IDC_YOFF_PROMPT)->EnableWindow(FALSE);   
      GetDlgItem(IDC_YOFF_EDIT)->EnableWindow(FALSE);   
      GetDlgItem(IDC_YOFF_SPIN)->EnableWindow(FALSE);   
      GetDlgItem(IDC_START_TYPE)->EnableWindow(FALSE);

	  GetDlgItem(IDC_ROTATION_PROMPT)->EnableWindow(FALSE);
      GetDlgItem(IDC_ROTATION)->EnableWindow(FALSE);      
	  GetDlgItem(IDC_UNITS)->EnableWindow(FALSE);	  	  
      
 	  TRACE("OnRepaint: DefaultTransform.XZoom = %s, DefaultTransform.XOff = %s\n", 
	     DefaultTransform.XZoom, DefaultTransform.XOff);
	  TRACE("OnRepaint: crect.Width() = %d, crect.Height() = %d\n", 
	     crect.Width(), crect.Height());
     
	  // send new image size but scale up for accuracy
	  int iWidth = crect.Width();	  
 	  cr.RipParamsOverride(crect.Width() * SCALEFACTOR, 
		 crect.Height() * SCALEFACTOR,
	     (float)iWidth * FSCALEFACTOR / fDevInchWidth,
	     (float)iWidth * FSCALEFACTOR / fDevInchWidth,
	     m_hWnd, &DefaultTransform);

      // start RIP  
      ret = cr.StartRip();   
      
      TRACE("VISUALXF: Rip return = %d\n", ret);
      }                   
   else
      cr.Close();
   }

LRESULT CJobSizeProps::OnPrevSetInfo(WPARAM wParam, LPARAM lParam)
   {
   int ret;
   
   if (VerifyCanvas())
      ret = cw.OnPrevSetInfo(wParam, lParam);   
   
   // get the focus back
   //SetFocus();
   
   return ret;
   }
   
LRESULT CJobSizeProps::OnRipMessage(WPARAM WParam, LPARAM LParam)
   {
   PRIPMSGSTRUCT pMsgStruct;   
   char LocalMsgBuffer[RIPMSG_MAX_MSG_LEN];   
   CString cs;
   
   // cast the LParam to a ripmsg pointer
   pMsgStruct = (PRIPMSGSTRUCT)LParam;
   
   // do message stuff based on msg type
   switch(pMsgStruct->MsgType)
      {
	  case RIPMSG_SCANLINE:   
           // invalidate all of status
		   cs.LoadString(IDS_BUILDING_SCANLINE);
           wsprintf(LocalMsgBuffer, (const char*)cs,
              pMsgStruct->LineOn, pMsgStruct->LineTotal);  
           // do the prompt		   
		   SetDlgItemText(IDC_RIP_STATUS_PRMPT, LocalMsgBuffer);
		   m_rip_status_progress.SetPos(pMsgStruct->LineOn * 100 
		      / pMsgStruct->LineTotal);
           break;      
      case RIPMSG_READING:           
           // invalidate all of status
		   cs.LoadString(IDS_READING_FILE_COMPLETE);		 
           wsprintf(LocalMsgBuffer, (const char*)cs,
              pMsgStruct->PercentComplete);
           // do the prompt
		   SetDlgItemText(IDC_RIP_STATUS_PRMPT, LocalMsgBuffer);
		   m_rip_status_progress.SetPos(pMsgStruct->PercentComplete);		      
           break;
      }         
   return 0L;   
   }     
   
LRESULT CJobSizeProps::OnRipComplete(WPARAM wParam, LPARAM lParam)
   {
   // nuke the prompts
   csTemp.LoadString(IDS_QUEUESTATUS_IDLE);
   SetDlgItemText(IDC_RIP_STATUS_PRMPT, csTemp);
   m_rip_status_progress.SetPos(0);		      
           
   // un-gray the buttons                      
   GetDlgItem(ID_REPAINT)->ShowWindow(SW_SHOW);      
   GetDlgItem(IDC_STOP)->ShowWindow(SW_HIDE);   
   
   //GetDlgItem(IDOK)->EnableWindow(TRUE);   
   //GetDlgItem(IDCANCEL)->EnableWindow(TRUE);     
   GetDlgItem(IDC_LOAD)->EnableWindow(TRUE);   
   GetDlgItem(IDC_SAVEAS)->EnableWindow(TRUE);   
   GetDlgItem(IDC_RESET)->EnableWindow(TRUE);        
      
   GetDlgItem(IDC_RETAIN_ASPECT)->EnableWindow(TRUE);
   GetDlgItem(IDC_SNAP)->EnableWindow(TRUE);
   //GetDlgItem(IDC_PREVIEW)->EnableWindow(TRUE); 
 
   GetDlgItem(IDC_WIDTH_PROMPT)->EnableWindow(TRUE);   
   GetDlgItem(IDC_WIDTH_EDIT)->EnableWindow(TRUE);   
   GetDlgItem(IDC_WIDTH_SPIN)->EnableWindow(TRUE);   
   GetDlgItem(IDC_HEIGHT_PROMPT)->EnableWindow(TRUE);   
   GetDlgItem(IDC_HEIGHT_EDIT)->EnableWindow(TRUE);   
   GetDlgItem(IDC_HEIGHT_SPIN)->EnableWindow(TRUE);   
   
   GetDlgItem(IDC_XZOOM_PROMPT)->EnableWindow(TRUE);
   GetDlgItem(IDC_XZOOM_EDIT)->EnableWindow(TRUE);
   GetDlgItem(IDC_XZOOM_SPIN)->EnableWindow(TRUE);
      
   GetDlgItem(IDC_YZOOM_PROMPT)->EnableWindow(TRUE);
   GetDlgItem(IDC_YZOOM_EDIT)->EnableWindow(TRUE);
   GetDlgItem(IDC_YZOOM_SPIN)->EnableWindow(TRUE);
      
   GetDlgItem(IDC_XOFF_PROMPT)->EnableWindow(TRUE);
   GetDlgItem(IDC_XOFF_EDIT)->EnableWindow(TRUE);
   GetDlgItem(IDC_XOFF_SPIN)->EnableWindow(TRUE);
      
   GetDlgItem(IDC_YOFF_PROMPT)->EnableWindow(TRUE);
   GetDlgItem(IDC_YOFF_EDIT)->EnableWindow(TRUE);
   GetDlgItem(IDC_YOFF_SPIN)->EnableWindow(TRUE);
   GetDlgItem(IDC_START_TYPE)->EnableWindow(TRUE);   

   GetDlgItem(IDC_ROTATION_PROMPT)->EnableWindow(TRUE);
   GetDlgItem(IDC_ROTATION)->EnableWindow(TRUE);
   GetDlgItem(IDC_UNITS)->EnableWindow(TRUE);
   
   // delete the rip
   if (cr.IsInit) cr.Close();
      
   return 0L;
   }

void CJobSizeProps::OnStop()
   {
   if (cr.IsInit) cr.CancelProcess(TRUE);
   }

void CJobSizeProps::OnRetainAspect()
   {
   CProfile Profile;

   // get data from dialog
   UpdateData(TRUE);		
   // and give it to our canvas 
   cw.m_retain_aspect = m_retain_aspect;

   Profile.WriteInt(IDS_TRANSFORMCONFIG, IDS_RETAIN_ASPECT, m_retain_aspect);      
   }  

void CJobSizeProps::OnSelchangeStartType() 
   {
   // get data from dialog
   UpdateData(TRUE);          	
   // get start type
   DefaultTransform.StartType = m_start_type;    
   if (VerifyCanvas())
      cw.UpdateRectFromEdit(TRANVAR_START);

   ReportChange(XFORMCHANGE_STARTTYPE);
   }

void CJobSizeProps::OnLoad() 
   {
   CLoadTransform dlg(this);

   if (dlg.DoModal() == IDOK)
      {	  
      // reset the sizing
      m_xoff = PicaTextToDisplay(dlg.pSelectedTransform->XOff);
      m_xzoom = dlg.pSelectedTransform->XZoom;
      m_yoff = PicaTextToDisplay(dlg.pSelectedTransform->YOff);
      m_yzoom = dlg.pSelectedTransform->YZoom;	  
      m_rotation = DegreeToMap(dlg.pSelectedTransform->Rotate);		  
	  // set start type
	  m_start_type = dlg.pSelectedTransform->StartType;
	  // set description
	  m_description = dlg.pSelectedTransform->Description;
	  // put them in
      InsertNewVals();
      UpdateData(FALSE);    
	  ReportChange(XFORMCHANGE_ALL);
	  }	
   }

void CJobSizeProps::OnSaveas() 
   {
   int idx;

   CXformSave cx(this);

   if (cx.DoModal() == IDOK)
      {
	  DefaultTransform.Name = cx.m_newxform;
	  CString csTemp;
      csTemp.LoadString(IDS_VISUALXF_PSOUTTITLE);
      csTemp += ": ";
      csTemp += DefaultTransform.Name;
      SetWindowText(csTemp);	  
	  if (cw.GetSafeHwnd()) cw.SetWindowText(csTemp);

      // get data from dialog
      UpdateData(TRUE);          

      // make sure this is current    
	  if (atof((const char*)m_xzoom) >=.1)
         DefaultTransform.XZoom = m_xzoom;
	  if (atof((const char*)m_yzoom)>=.1)
         DefaultTransform.YZoom = m_yzoom;

      DefaultTransform.XOff = DisplayTextToPica(m_xoff);
      DefaultTransform.YOff = DisplayTextToPica(m_yoff);

      MapToDegree(DefaultTransform.Rotate, m_rotation);	      
 
	  DefaultTransform.WriteToIni();

	  // reload them
	  LoadStandardXforms();

	  // select it
	  CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_TRANSFORM);	   
	  if ((idx = pCB->FindString(-1, DefaultTransform.Name)) != CB_ERR)
	     pCB->SetCurSel(idx);
	  ReportChange(XFORMCHANGE_ALL);ReportChange(XFORMCHANGE_ALL);
	  }
   }   

void CJobSizeProps::OnReset() 
   {
   // re-init the sizing vals                        
   m_xzoom = OriginalTransform.XZoom;
   m_yzoom = OriginalTransform.YZoom;

   m_xoff = PicaTextToDisplay(OriginalTransform.XOff);
   m_yoff = PicaTextToDisplay(OriginalTransform.YOff);

   m_LastRotation = m_rotation = DegreeToMap(OriginalTransform.Rotate);	      
   m_start_type = OriginalTransform.StartType;   

   // this proc does the updating
   InsertNewVals();
   UpdateData(FALSE);    

   ReportChange(XFORMCHANGE_ALL);
   }

void CJobSizeProps::OnContextMenu(CWnd* pWnd, CPoint point) 
   {   
   // call context help
   ::WinHelp(pWnd->m_hWnd, AfxGetApp()->m_pszHelpFilePath, 
      HELP_CONTEXTMENU,  (DWORD)(LPVOID)dwHelpMap);
   }  

BOOL CJobSizeProps::OnHelpInfo(HELPINFO* pHelpInfo) 
   {
   if (pHelpInfo->iContextType == HELPINFO_WINDOW)   // must be for a control
      {
      ::WinHelp ((HWND)pHelpInfo->hItemHandle,
         AfxGetApp()->m_pszHelpFilePath, HELP_WM_HELP, 
		 (DWORD)(LPVOID)dwHelpMap);
	  return TRUE;
	  }      
   else
   	  return CPropertyPage::OnHelpInfo(pHelpInfo);
   }

int CJobSizeProps::DegreeToMap(CString& csDegree)
   {
   int ret;
   int degree;

   degree = atoi((const char*)csDegree);

   switch(degree)
      {
	  case 0:
	       ret = 0;
		   break;
	  case 90:
	       ret = 1;
		   break;
	  case 180:
	       ret = 2;
		   break;
	  case 270:
	       ret = 3;
		   break;
	  default:
	       ret = 0;
		   break;
	  }
   return ret;
   }

void CJobSizeProps::MapToDegree(CString& csRet, int map)
   {
   int ret;   

   switch(map)
      {
	  case 0:
	       ret = 0;
		   break;
	  case 1:
	       ret = 90;
		   break;
	  case 2:
	       ret = 180;
		   break;
	  case 3:
	       ret = 270;
		   break;
	  default:
	       ret = 0;
		   break;
	  }
   csRet.Format("%d", ret);
   }

float CJobSizeProps::MapToFloat(int map)
   {
   CString cs;
   
   MapToDegree(cs, map);
   return (float)atof((const char*)cs);
   }

void CJobSizeProps::OnSelchangeRotation() 
   {   
   m_LastRotation = m_rotation;
   m_should_repaint = TRUE;
   UpdateData(TRUE);
   InsertNewVals();

   ReportChange(XFORMCHANGE_ROTATE);
   }

void CJobSizeProps::InsertNewVals()
   {
   CTM2Rect ct2r;
   SIZING_VALS sv;
   CRect NewBoundingRect;   
   CRect BoundingRect;   
   CRect ZoomRect;
   POINT ptRotateCorrect;
   float ftemp;
   CString csTemp;

   // note special bounding box for 90, 270 rotation
   if (m_rotation == ROTATION_0 || m_rotation == ROTATION_180)
      {
	  ZoomRect.right = CanRect.Width();
	  ZoomRect.bottom = CanRect.Height();
	  }
   else
      {
	  ZoomRect.bottom = CanRect.Width();
	  ZoomRect.right = CanRect.Height();
	  }	   
	    
   ZoomRect.left = 0;   
   ZoomRect.top = 0;
   
   // make the BoundingRect zero based
   BoundingRect.right = CanRect.Width();
   BoundingRect.bottom = CanRect.Height();
   BoundingRect.left = 0;   
   BoundingRect.top = 0;
   
   sv.xzoom = (float)atof(m_xzoom);
   sv.yzoom = (float)atof(m_yzoom);
   //sv.xoff = (float)atof(m_xoff);
   //sv.yoff = (float)atof(m_yoff);
   sv.xoff = DisplayTextToPicaNum(m_xoff);
   sv.yoff = DisplayTextToPicaNum(m_yoff);
   sv.rotation = MapToFloat(m_rotation);
   //ct2r.ApplySizingValsToCRect(NewBoundingRect, &sv, BoundingRect, ptRotateCorrect, 
	 // cw.PicaWidth, cw.PicaHeight);	  

   // load float values of sizing vals
   // into tracker rect
   cw.pRT->m_xzoom = (float)atof(m_xzoom);            
   cw.pRT->m_yzoom = (float)atof(m_yzoom);               
   //cw.pRT->m_xoff = (float)atof(m_xoff);            
   //cw.pRT->m_yoff = (float)atof(m_yoff);               
   cw.pRT->m_xoff = DisplayTextToPicaNum(m_xoff);            
   cw.pRT->m_yoff = DisplayTextToPicaNum(m_yoff);        

   cw.pRT->BoundingRect = CanRect;

   // get the width and height in pica
   float fwidth, fheight;
   if (m_LastRotation == ROTATION_0 || m_LastRotation == ROTATION_180)
	  {
      fwidth = cw.pRT->m_xzoom * (float)cMediaRect.Width() / 100. * 72.F;
      fheight = cw.pRT->m_yzoom * (float)cMediaRect.Height() / 100. * 72.F;	  
	  }
   else
	  {
      fwidth = cw.pRT->m_xzoom * (float)cMediaRect.Height() / 100. * 72.F;
      fheight = cw.pRT->m_yzoom * (float)cMediaRect.Width() / 100. * 72.F;	  
	  }

   if (m_LastRotation == ROTATION_0 || m_LastRotation == ROTATION_180)
	  {
	  if (m_rotation == ROTATION_0 || m_rotation == ROTATION_180)
		 {
		 m_width = PicaNumToDisplay(fwidth);
		 m_height = PicaNumToDisplay(fheight);   
		 // set the internal vals
		 cw.pRT->m_width = fwidth;
		 cw.pRT->m_height = fheight;      
		 }
	  else
		 {
		 m_height = PicaNumToDisplay(fwidth);
		 m_width = PicaNumToDisplay(fheight);   
		 // set the internal vals
		 cw.pRT->m_height = fwidth;
		 cw.pRT->m_width = fheight;      
		 // swap zooms
		 ftemp = cw.pRT->m_xzoom;
		 cw.pRT->m_xzoom = cw.pRT->m_yzoom;
		 cw.pRT->m_yzoom = ftemp;
		 ftemp = sv.xzoom;
		 sv.xzoom = sv.yzoom;
		 sv.yzoom = ftemp;
		 csTemp = m_xzoom;
		 m_xzoom = m_yzoom;
		 m_yzoom = csTemp;
		 }
	  }
   else
	  {
	  if (m_rotation == ROTATION_90 || m_rotation == ROTATION_270)
		 {
		 m_width = PicaNumToDisplay(fwidth);
		 m_height = PicaNumToDisplay(fheight);   
		 // set the internal vals
		 cw.pRT->m_width = fwidth;
		 cw.pRT->m_height = fheight;      
		 }
	  else
		 {
		 m_height = PicaNumToDisplay(fwidth);
		 m_width = PicaNumToDisplay(fheight);   
		 // set the internal vals
		 cw.pRT->m_height = fwidth;
		 cw.pRT->m_width = fheight;      
		 // swap zooms
		 ftemp = cw.pRT->m_xzoom;
		 cw.pRT->m_xzoom = cw.pRT->m_yzoom;
		 cw.pRT->m_yzoom = ftemp;
		 ftemp = sv.xzoom;
		 sv.xzoom = sv.yzoom;
		 sv.yzoom = ftemp;
		 csTemp = m_xzoom;
		 m_xzoom = m_yzoom;
		 m_yzoom = csTemp;
		 }
	  }	  

   ct2r.ApplySizingValsToCRect(NewBoundingRect, &sv, BoundingRect, ptRotateCorrect,    
	  cw.PicaWidth, cw.PicaHeight);	  

   UpdateData(FALSE);

   cw.SetSizingRect(&NewBoundingRect, &BoundingRect, &ZoomRect, &ptRotateCorrect);    
   if (VerifyCanvas())
      cw.UpdateRectFromEdit(TRANVAR_ROTATION);

   ReportChange(XFORMCHANGE_ALL);
   }

void CJobSizeProps::OnCancel() 
   {
   if (TimerID) KillTimer(TimerID);
   TimerID = 0;
	
   CPropertyPage::OnCancel();
   }

void CJobSizeProps::MoveDialog()
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
   Point.x = ParentRect.Width();
   Point.y = ParentRect.Height();
   
   // Calculate the new X, Y starting point.
   Point.x -= nWidth;
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

void CJobSizeProps::OnTimer(UINT nIDEvent) 
   {	
   CPropertyPage::OnTimer(nIDEvent);   

   // wait for a second to have elapsed
   // before we do our updating to prevent 
   // squirrely display artifacts
   if ((::GetTickCount() -  LastTickCount) < XFORM_UPDATEDELAY)
      return;

   // update all numbers
   ValidateAll();
   }

void CJobSizeProps::OnChangeXzoomEdit()
   {  
   // on edit change, the zoom becomes invalid
   xzoom_is_invalid = TRUE;       
   LastTickCount = ::GetTickCount();
   TRACE("OnChangeXZoomEdit\n");
   }

void CJobSizeProps::OnChangeYzoomEdit()
   {                
   // on edit change, the zoom becomes invalid
   yzoom_is_invalid = TRUE;
   LastTickCount = ::GetTickCount();
   TRACE("OnChangeYZoomEdit\n");
   }

void CJobSizeProps::OnDeltaposXzoomSpin(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   NM_UPDOWN* pStuff = (NM_UPDOWN*)pNMHDR;   

   // get current val
   float fval = (float)atof(m_xzoom);
   // increment/decrement
   fval += ((float)(-pStuff->iDelta)) * .1F;
   // don't allow zero
   if (fval == 0.0F) fval = .1F;
   // put back in
   sprintf(m_xzoom.GetBuffer(64), "%.3f", fval);   
   m_xzoom.ReleaseBuffer();   
   UpdateData(FALSE); 
   xzoom_is_invalid = TRUE;    
   m_xzoom_is_indeterminate = FALSE;
   LastTickCount = ::GetTickCount();

   *pResult = 0;
   }

void CJobSizeProps::OnDeltaposYzoomSpin(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   NM_UPDOWN* pStuff = (NM_UPDOWN*)pNMHDR;   

   // get current val
   float fval = (float)atof(m_yzoom);
   // increment/decrement
   fval += ((float)(-pStuff->iDelta)) * .1F;
   // don't allow zero
   if (fval == 0.0F) fval = .1F;
   // put back in
   sprintf(m_yzoom.GetBuffer(64), "%.3f", fval);   
   m_yzoom.ReleaseBuffer();   
   UpdateData(FALSE);
   yzoom_is_invalid = TRUE;
   m_yzoom_is_indeterminate = FALSE;
   LastTickCount = ::GetTickCount();
   *pResult = 0;
   }

void CJobSizeProps::OnChangeXoffEdit()
   {
   xoff_is_invalid = TRUE;    
   LastTickCount = ::GetTickCount();
   }

void CJobSizeProps::OnChangeYoffEdit()
   {
   yoff_is_invalid = TRUE;    
   LastTickCount = ::GetTickCount();
   }

void CJobSizeProps::OnDeltaposXoffSpin(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   NM_UPDOWN* pStuff = (NM_UPDOWN*)pNMHDR;   

   // get current val
   float fval = (float)atof(m_xoff);   
   // increment/decrement
   fval += ((float)(-pStuff->iDelta)) * 1.F;
   // put back in
   sprintf(m_xoff.GetBuffer(64), "%.3f", fval);
   m_xoff.ReleaseBuffer();
   UpdateData(FALSE);
   xoff_is_invalid = TRUE;
   LastTickCount = ::GetTickCount();
   *pResult = 0;
   }

void CJobSizeProps::OnDeltaposYoffSpin(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   NM_UPDOWN* pStuff = (NM_UPDOWN*)pNMHDR;   

   // get current val
   float fval = (float)atof(m_yoff);
   // increment/decrement
   fval += ((float)(-pStuff->iDelta)) * 1.F;
   // put back in
   sprintf(m_yoff.GetBuffer(64), "%.3f", fval);   
   m_yoff.ReleaseBuffer();
   UpdateData(FALSE); 
   yoff_is_invalid = TRUE;
   LastTickCount = ::GetTickCount();
   *pResult = 0;
   }

BOOL CJobSizeProps::VerifyCanvas()
   {
   CProfile Profile;

   if (!m_preview) return FALSE;
   if (!pTargetCQueue) return FALSE;
   if (!bVarsSet) return FALSE;
   // do nothing if parent is not visible
   if (!IsWindowVisible() && !bForceShow) return FALSE;

   if (!cwCanvasParent.Exists)
	  {
	  TRACE("Creating visual xform window\n");

	  CString csTemp;
      csTemp.LoadString(IDS_VISUALXF_PSOUTTITLE);
      csTemp += ": ";
      csTemp += DefaultTransform.Name;
      SetWindowText(csTemp);
	  CRect AdjustedParentRect;
	  CRect AdjustedCanvasRect;

	  // set window styles
      dwCanvasStyle = WS_CHILD | WS_VISIBLE;      
      dwCanvasParentStyle = WS_POPUP |WS_VISIBLE | WS_MINIMIZEBOX | WS_CAPTION;

      // initially set adjusted canvas to our image size
      AdjustedCanvasRect = CanRect;

	  // now adjust it based on the window style
	  ::AdjustWindowRect(&AdjustedCanvasRect, dwCanvasStyle, FALSE);	 

	  // our parent is equal to the adjusted canvas rect + the rulers
	  AdjustedParentRect.left = AdjustedCanvasRect.left;
	  AdjustedParentRect.right = AdjustedCanvasRect.right + RULER_HEIGHT;
	  AdjustedParentRect.top = AdjustedCanvasRect.top;
	  AdjustedParentRect.bottom = AdjustedCanvasRect.bottom + RULER_HEIGHT;		 

	  // now adjust this window
	  ::AdjustWindowRect(&AdjustedParentRect, dwCanvasParentStyle, FALSE);	 

      // get the last position of the window
	  int iLastPosX, iLastPosY;
	  iLastPosY = Profile.GetInt(IDS_PREVIEW, IDS_LASTWINDOWPOSY, 10);
	  iLastPosX = Profile.GetInt(IDS_PREVIEW, IDS_LASTWINDOWPOSX, 10);

	  // create the canvas parent
      cwCanvasParent.CreateEx(0,
	      AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,       
	      ::LoadCursor(NULL, IDC_ARROW),       	   
	      (HBRUSH)::GetStockObject(WHITE_BRUSH), 
		  AfxGetApp()->LoadIcon(IDI_VISUALXFORM_ICON)),
	      (const char*)csTemp,
          dwCanvasParentStyle, iLastPosX, iLastPosY, AdjustedParentRect.Width(),
		  AdjustedParentRect.Height(), AfxGetMainWnd()->GetSafeHwnd(), NULL);        
	  // create the canvas
      cw.CreateEx(0,
	      AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,       
	      ::LoadCursor(NULL, IDC_ARROW),       	   
	      (HBRUSH)::GetStockObject(WHITE_BRUSH), NULL),
	      (const char*)csTemp,
          dwCanvasStyle, RULER_HEIGHT, RULER_HEIGHT, AdjustedCanvasRect.Width(), 
	      AdjustedCanvasRect.Height(), cwCanvasParent.m_hWnd, NULL);        
	  // create the rulers
	  CornerRuler.SetStyle(RULER_STYLE_CORNER);
      CornerRuler.CreateEx(WS_EX_CLIENTEDGE,
	      AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,       
	      ::LoadCursor(NULL, IDC_ARROW),       	   
	      (HBRUSH)::GetStockObject(GRAY_BRUSH), NULL),
	      NULL, WS_CHILD | WS_VISIBLE,
		  0, 0, 
		  RULER_HEIGHT, RULER_HEIGHT, 
		  cwCanvasParent.m_hWnd, NULL);        
	  // set ruler ranges based on units
	  SetRulerRanges();
	  // create the rulers
	  TopRuler.SetStyle(RULER_STYLE_TOP);	  
      TopRuler.CreateEx(WS_EX_CLIENTEDGE,
	      AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,       
	      ::LoadCursor(NULL, IDC_ARROW),       	   
	      (HBRUSH)::GetStockObject(GRAY_BRUSH), NULL),
	      NULL, WS_CHILD | WS_VISIBLE,
		  RULER_HEIGHT, 0, 
		  AdjustedCanvasRect.Width(), RULER_HEIGHT, 
		  cwCanvasParent.m_hWnd, NULL);  
      LeftRuler.SetStyle(RULER_STYLE_LEFT);	  
      LeftRuler.CreateEx(WS_EX_CLIENTEDGE,
	      AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,       
	      ::LoadCursor(NULL, IDC_ARROW),       	   
	      (HBRUSH)::GetStockObject(GRAY_BRUSH), NULL),
	      NULL, WS_CHILD | WS_VISIBLE,
		  0, RULER_HEIGHT, 
		  RULER_HEIGHT, AdjustedCanvasRect.Height(),
		  cwCanvasParent.m_hWnd, NULL);        
	  cwCanvasParent.Exists = TRUE;
	  }
   return TRUE;
   }

void CJobSizeProps::OnSelchangeUnits() 
   {
   CProfile Profile;

   CString m_xoff_temp, m_yoff_temp;
   CString m_width_temp, m_height_temp;

   // convert stuff using old units
   m_xoff_temp = DisplayTextToPica(m_xoff);
   m_yoff_temp = DisplayTextToPica(m_yoff);
   m_width_temp = DisplayTextToPica(m_width);
   m_height_temp = DisplayTextToPica(m_height);

   // get everything
   UpdateData(TRUE);   

   // convert to proper units
   m_xoff = PicaTextToDisplay(m_xoff_temp);
   m_yoff = PicaTextToDisplay(m_yoff_temp);
   m_width = PicaTextToDisplay(m_width_temp);
   m_height = PicaTextToDisplay(m_height_temp);

   // reset the rulers
   SetRulerRanges();

   // force redraw
   TopRuler.Invalidate();
   LeftRuler.Invalidate();

   // update everything
   UpdateData(FALSE);      

   Profile.WriteInt(IDS_TRANSFORMCONFIG, IDS_UNITS, m_units);

   ReportChange(XFORMCHANGE_ALL);
   }

#define INCH_TO_MM(a) ((int)((float)a * 25.4F + .5F))

void CJobSizeProps::SetRulerRanges()
   {
   int iTest, iTick;

   // redo the rulers
   switch(m_units)
	  {
	  case XFORM_UNITS_PICA:    
		   iTest = (cRectRuler.right * 72 - cRectRuler.left * 72) / 1000;
		   iTick = max(iTest * 100, 100);		   
           TopRuler.SetRange(cRectRuler.left * 72, cRectRuler.right * 72, 			  
			  10, iTick);
           LeftRuler.SetRange(cRectRuler.top * 72, cRectRuler.bottom * 72, 			  
			  10, iTick);
		   break;
	  case XFORM_UNITS_INCH:
		   iTest = (cRectRuler.right - cRectRuler.left) / 1000;
		   iTick = max(iTest * 100, 100);		   
           TopRuler.SetRange(cRectRuler.left, cRectRuler.right, 			  
			  10, iTick);
           LeftRuler.SetRange(cRectRuler.top, cRectRuler.bottom, 			  
			  10, iTick);
		   break;
	  case XFORM_UNITS_MM:
		   iTest = (INCH_TO_MM(cRectRuler.right) - INCH_TO_MM(cRectRuler.left)) / 1000;
		   iTick = max(iTest * 100, 100);		   
           TopRuler.SetRange(INCH_TO_MM(cRectRuler.left), INCH_TO_MM(cRectRuler.right), 
			  10, iTick);
           LeftRuler.SetRange(INCH_TO_MM(cRectRuler.top), INCH_TO_MM(cRectRuler.bottom), 
			  10, iTick);
		   break;		   	

	  }
   }

CString CJobSizeProps::PicaTextToDisplay(CString& csIn)
   {
   float fval;
   CString csRet;   

   switch(m_units)
	  {
	  case XFORM_UNITS_PICA:
		   // stored units are pica - do nothing
		   csRet = csIn;
		   break;
	  case XFORM_UNITS_INCH:
		   // convert to inches
		   fval = (float)atof(csIn) / 72.F;
           sprintf(csRet.GetBuffer(64), "%.3f", fval);
		   csRet.ReleaseBuffer();
		   break;
	  case XFORM_UNITS_MM:
		   // convert to mm
		   fval = (float)atof(csIn) / 72.F * 25.4F;
           sprintf(csRet.GetBuffer(64), "%.3f", fval);
		   csRet.ReleaseBuffer();	
		   break;		   	
	  }
   return csRet;
   }

CString CJobSizeProps::DisplayTextToPica(CString& csIn)
   {
   float fval;
   CString csRet;   

   switch(m_units)
	  {
	  case XFORM_UNITS_PICA:
		   // stored units are pica - do nothing
		   csRet = csIn;
		   break;
	  case XFORM_UNITS_INCH:
		   // convert from inches
           fval = (float)atof(csIn) * 72.F;
           sprintf(csRet.GetBuffer(64), "%.3f", fval);
           csRet.ReleaseBuffer();
		   break;
	  case XFORM_UNITS_MM:
		   // convert from mm
		   fval = (float)atof(csIn) * 72.F / 25.4F;
           sprintf(csRet.GetBuffer(64), "%.3f", fval);
		   csRet.ReleaseBuffer();	
		   break;		   	
	  }
   return csRet;
   }

float CJobSizeProps::DisplayTextToPicaNum(CString& csIn)
   {
   float fRet;

   switch(m_units)
	  {
	  case XFORM_UNITS_PICA:
		   // stored units are pica
		   fRet = (float)atof(csIn);
		   break;
	  case XFORM_UNITS_INCH:
		   // convert from inches
           fRet = (float)atof(csIn) * 72.F;
		   break;
	  case XFORM_UNITS_MM:
		   // convert from mm
		   fRet = (float)atof(csIn) * 72.F / 25.4F;
		   break;		   	
	  }
   return fRet;
   }

CString CJobSizeProps::PicaNumToDisplay(float fNum)
   {
   CString csRet;   

   switch(m_units)
	  {
	  case XFORM_UNITS_PICA:
		   // stored units are pica - do nothing
           sprintf(csRet.GetBuffer(64), "%.3f", fNum);
		   csRet.ReleaseBuffer();
		   break;
	  case XFORM_UNITS_INCH:
		   // convert to inches		   
           sprintf(csRet.GetBuffer(64), "%.3f", fNum / 72.F);
		   csRet.ReleaseBuffer();
		   break;
	  case XFORM_UNITS_MM:
		   // convert to mm
		   sprintf(csRet.GetBuffer(64), "%.3f", fNum / 72.F * 25.4F);           
		   csRet.ReleaseBuffer();	
		   break;		   	
	  }
   return csRet;
   }

void CJobSizeProps::ValidateAll()
   {
   if (xzoom_is_invalid)
      {
	  m_should_repaint = TRUE;
      UpdateData(TRUE);
      // don't allow zero
      if (atof(m_xzoom) == 0.0F) 
		 {
		 if (!m_xzoom_is_indeterminate)						
		    m_xzoom = ".1";						
		 UpdateData(FALSE);
		 }   
      // special for retain aspect
      if (cw.m_retain_aspect)
         {		 
		 if (!m_xzoom_is_indeterminate && !m_yzoom_is_indeterminate)
            m_yzoom = m_xzoom;		 
	     UpdateData(FALSE);          
		 if (VerifyCanvas())
	        cw.UpdateRectFromEdit(TRANVAR_YZOOM, (float)atof(m_yzoom));	      		 
	     }	  
	  
	  if (VerifyCanvas())
         cw.UpdateRectFromEdit(TRANVAR_XZOOM, (float)atof(m_xzoom));		  

	  xzoom_is_invalid = FALSE;
	  if (!m_xzoom_is_indeterminate)
	     ReportChange(XFORMCHANGE_XZOOM);
	  if (!m_yzoom_is_indeterminate)
	     ReportChange(XFORMCHANGE_YZOOM);
	  }
   if (yzoom_is_invalid)
      {
	  m_should_repaint = TRUE;
	  UpdateData(TRUE);
      // don't allow zero
      if (atof(m_yzoom) == 0.0F) 
		 {
		 if (!m_yzoom_is_indeterminate)
		    m_yzoom = ".1";
		 UpdateData(FALSE);
		 }
      // special for retain aspect
      if (cw.m_retain_aspect)
         {
		 if (!m_xzoom_is_indeterminate && !m_yzoom_is_indeterminate)
            m_xzoom = m_yzoom;		 
	     UpdateData(FALSE);
		 if (VerifyCanvas())
	        cw.UpdateRectFromEdit(TRANVAR_XZOOM, (float)atof(m_xzoom));	      		 
	     }

	  if (VerifyCanvas())
         cw.UpdateRectFromEdit(TRANVAR_YZOOM, (float)atof(m_yzoom));		  

	  yzoom_is_invalid = FALSE;	  
	  if (!m_xzoom_is_indeterminate)
	     ReportChange(XFORMCHANGE_XZOOM);
	  if (!m_yzoom_is_indeterminate)
	     ReportChange(XFORMCHANGE_YZOOM);
	  }
   if (xoff_is_invalid)
      {
	  m_should_repaint = TRUE;
      UpdateData(TRUE);
	  if (VerifyCanvas())
	     cw.UpdateRectFromEdit(TRANVAR_XOFF, DisplayTextToPicaNum(m_xoff));	  	  
	  xoff_is_invalid = FALSE;
	  if (!m_xoff_is_indeterminate)
		ReportChange(XFORMCHANGE_XOFF);
	  }
   if (yoff_is_invalid)
      {
	  m_should_repaint = TRUE;
      UpdateData(TRUE);
	  if (VerifyCanvas())
	     cw.UpdateRectFromEdit(TRANVAR_YOFF, DisplayTextToPicaNum(m_yoff));	  	  
	  yoff_is_invalid = FALSE;
	  if (!m_yoff_is_indeterminate)
		  ReportChange(XFORMCHANGE_YOFF);
	  }   
   if (width_is_invalid)
      {
	  m_should_repaint = TRUE;
      UpdateData(TRUE);
	  if (VerifyCanvas())
	     cw.UpdateRectFromEdit(TRANVAR_WIDTH, DisplayTextToPicaNum(m_width));
	  width_is_invalid = FALSE;
	  }   
   if (height_is_invalid)
      {
	  m_should_repaint = TRUE;
      UpdateData(TRUE);
	  if (VerifyCanvas())
	     cw.UpdateRectFromEdit(TRANVAR_HEIGHT, DisplayTextToPicaNum(m_height));
	  height_is_invalid = FALSE;
	  }   
   }

void CJobSizeProps::UpdateIt(BOOL MultipleSelection)
   {
   POINT ptRotateCorrect;	
   float devAspectRatio;
   CTM2Rect ct2r;
   SIZING_VALS sv;
   CRect NewBoundingRect;   
   CRect BoundingRect;   
   CRect ZoomRect;	 												  
   CComboBox* pCB;
   int idx;

   if (!DialogInit) return;		
   if (!pTargetDevice) return;  

   // if any of these things have changed, invalidate
   if (pTargetDevice && pLastDefaultDevice && pLastDefaultTransform &&
	   pLocalColor && pLastLocalColor && pTargetCQueue && pLastTargetQueue)	  
	  {
      if (*pTargetDevice != *pLastDefaultDevice || DefaultTransform != *pLastDefaultTransform || 
	     *pLocalColor != *pLastLocalColor || *pTargetCQueue != *pLastTargetQueue)
		 {
	     cw.m_must_repaint = TRUE;
		 if (m_preview && cw.GetSafeHwnd())
		    cw.Invalidate();
		 }
	  }
   else
	  {
	  // if the last... pointers are not set, then we must invalidate the
	  // window just to show the track rect
	  if (!pLastDefaultDevice && !pLastDefaultTransform &&
	      !pLastLocalColor && !pLastTargetQueue)	  
		 {
		 cw.m_must_repaint = TRUE;
		 if (m_preview && cw.GetSafeHwnd())
			cw.Invalidate();
		 }
	  }

   // save this flag;
   bLastMultipleSelection = MultipleSelection;

   // disable preview if multi selection
   if (MultipleSelection) 
	  m_preview = FALSE;
   else
	  m_preview = TRUE;

   //ResetPreviewWnd();

   pCB = (CComboBox*)GetDlgItem(IDC_TRANSFORM);	   

   if ((idx = pCB->FindString(-1, DefaultTransform.Name)) != CB_ERR)         
      m_name = idx;

   // set buttons
   SetButtons();
      
   m_targetdevice = pTargetDevice->Name;   
   
   // set view perspective
   if (pTargetCQueue)
	  {
      // set target file
      m_targetfile = pTargetCQueue->Description;	        
	  }
   else
	  m_targetfile.LoadString(IDS_NOT_AVAIL);

   // get media rect	   
   pTargetDevice->GetCurrentMediaRect(cMediaRect);	

   // zero this out
   memset(&RipStruct, 0, sizeof RIPSTRUCT);

   // ripstruct with bounding box in case it is needed for
   // packagizing
   if(pTargetCQueue)
	  {
      RipStruct.PixWidth = abs(pTargetCQueue->BoundingBox.Width());
      RipStruct.PixHeight = abs(pTargetCQueue->BoundingBox.Height());
	  }

   // get the ripstruct so we can compute the pica width	
   pTargetDevice->FillRipStruct(&RipStruct);
   devAspectRatio =
      (float)RipStruct.PixHeight / (float)RipStruct.PixWidth *
      RipStruct.WPixelsPerInch / RipStruct.HPixelsPerInch;

   // modify the media rect based on the orientation which we derive from
   // the RipStruct
   if (devAspectRatio < 1.0)
      {
	  CRect crTemp;

	  crTemp.left = cMediaRect.top;
	  crTemp.right = cMediaRect.bottom;
	  crTemp.top = cMediaRect.left;
	  crTemp.bottom = cMediaRect.right;	  
	  cMediaRect = crTemp;
	  }

   // store the media rect in the canvas
   cw.cMediaRect = cMediaRect;

   if (!MultipleSelection)
	  {
      // set start type
      m_start_type = DefaultTransform.StartType;	
      // set description
      m_description = DefaultTransform.Description;
      // init the sizing vals          
      m_xoff = PicaTextToDisplay(DefaultTransform.XOff);
      m_yoff = PicaTextToDisplay(DefaultTransform.YOff);
      m_xzoom = DefaultTransform.XZoom;   
      m_yzoom = DefaultTransform.YZoom;
      m_LastRotation = m_rotation = DegreeToMap(DefaultTransform.Rotate);		  
	  }
   else
	  {
	  // if nothing here, blank it
	  if (m_start_type.GetLength() < 1)		 
		 ((CComboBox*)GetDlgItem(IDC_START_TYPE))->SetCurSel(-1);		 
      // init the sizing vals          
      m_xoff = PicaTextToDisplay(DefaultTransform.XOff);
      m_yoff = PicaTextToDisplay(DefaultTransform.YOff);
	  if (m_xzoom.GetLength() >= 1)		 
         m_xzoom = DefaultTransform.XZoom;   
      m_yzoom = DefaultTransform.YZoom;
      m_LastRotation = m_rotation = DegreeToMap(DefaultTransform.Rotate);		  
	  }

   CanRect.left = 0;
   CanRect.top = 0;

   // the rect ruler is equal to the media rect
   cRectRuler = cMediaRect;

   // get width and height based on orientation
   //if (pTargetDevice->pDevInfo->Orientation != DEV_ORIENT_LANDSCAPE)
   if (devAspectRatio < 1.0)
      {
	  CanRect.right = (int)((float)GetSystemMetrics(SM_CXSCREEN) * 
		 fPrevFactor[m_preview_size] + .5F);
	  CanRect.bottom = (int)((float)GetSystemMetrics(SM_CXSCREEN) * 
		 fPrevFactor[m_preview_size] * devAspectRatio + .5F);
	  }    	  
    else
      {	
	  CanRect.bottom = (int)((float)GetSystemMetrics(SM_CYSCREEN) * 
		 fPrevFactor[m_preview_size] + .5F);
	  CanRect.right = (int)((float)GetSystemMetrics(SM_CYSCREEN) * 
		 fPrevFactor[m_preview_size] / devAspectRatio + .5F);	  
	  }      

   // normalize the coords
   CanRect.NormalizeRect();   
   cRectRuler.NormalizeRect();

   cw.PicaWidth = (float)RipStruct.PixWidth * 72.F /
      (float)RipStruct.WPixelsPerInch;
   cw.PicaHeight = (float)RipStruct.PixHeight * 72.F /
      (float)RipStruct.HPixelsPerInch;

   fDevInchWidth = (float)RipStruct.PixWidth / (float)RipStruct.WPixelsPerInch;   

   ResetPreviewWnd();

   OldCanRect = CanRect;

   // set title to the transform
   CString csTemp;
   csTemp.LoadString(IDS_VISUALXF_PSOUTTITLE);
   csTemp += ": ";
   csTemp += DefaultTransform.Name;
   SetWindowText(csTemp);

   //UpdateData(FALSE);   

   // note special bounding box for 90, 270 rotation
   if (m_rotation == ROTATION_0 || m_rotation == ROTATION_180)
      {
	  ZoomRect = CanRect;
	  }
   else
      {
	  ZoomRect.left = CanRect.top;
	  ZoomRect.right = CanRect.bottom;
	  ZoomRect.top = CanRect.left;
	  ZoomRect.bottom = CanRect.right;
	  }	   
	    
   // make the BoundingRect zero based
   BoundingRect.right = CanRect.Width();
   BoundingRect.bottom = CanRect.Height();
   BoundingRect.left = 0;   
   BoundingRect.top = 0;
   
   sv.xzoom = (float)atof(m_xzoom);
   sv.yzoom = (float)atof(m_yzoom);   
   sv.xoff = DisplayTextToPicaNum(m_xoff);
   sv.yoff = DisplayTextToPicaNum(m_yoff);
   sv.rotation = MapToFloat(m_rotation);
   ct2r.ApplySizingValsToCRect(NewBoundingRect, &sv, BoundingRect, ptRotateCorrect, 
	  cw.PicaWidth, cw.PicaHeight);	  

   // load float values of sizing vals
   // into tracker rect
   cw.pRT->m_xzoom = (float)atof(m_xzoom);            
   cw.pRT->m_yzoom = (float)atof(m_yzoom);               
   cw.pRT->m_xoff = DisplayTextToPicaNum(m_xoff);            
   cw.pRT->m_yoff = DisplayTextToPicaNum(m_yoff);                  
   cw.pRT->BoundingRect = CanRect;

   // get the width and height in pica
   float fwidth, fheight;
   fwidth = cw.pRT->m_xzoom * (float)cMediaRect.Width() / 100. * 72.F;
   fheight = cw.pRT->m_yzoom * (float)cMediaRect.Height() / 100. * 72.F;	  
   // set the internal vals
   cw.pRT->m_width = fwidth;
   cw.pRT->m_height = fheight;      
   // and create the display vals
   m_width = PicaNumToDisplay(fwidth);
   m_height = PicaNumToDisplay(fheight);   
   
   UpdateData(FALSE);   

   cw.SetSizingRect(&NewBoundingRect, &BoundingRect, &ZoomRect, &ptRotateCorrect);    
   
   // show the correct buttons
   GetDlgItem(IDC_STOP)->ShowWindow(SW_HIDE);   
   GetDlgItem(ID_REPAINT)->ShowWindow(SW_SHOW);    

   // nuke the prompts
   csTemp.LoadString(IDS_QUEUESTATUS_IDLE);
   SetDlgItemText(IDC_RIP_STATUS_PRMPT, csTemp);
   m_rip_status_progress.SetPos(0);		        

#if 0   
   // problem loading rip, don't allow repaint
   if (!cr.RipLoaded)   
      {
      GetDlgItem(ID_REPAINT)->ShowWindow(SW_SHOW);      
      GetDlgItem(IDC_STOP)->ShowWindow(SW_HIDE);   
      GetDlgItem(ID_REPAINT)->EnableWindow(FALSE);   	  
	  }
#endif

   xzoom_is_invalid = FALSE;
   yzoom_is_invalid = FALSE;
   xoff_is_invalid = FALSE;
   yoff_is_invalid = FALSE;
   width_is_invalid = FALSE;
   height_is_invalid = FALSE;
   OriginalTransform = DefaultTransform;

   // don't need to recommend repainting, yet...
   m_should_repaint = FALSE;

   if (MultipleSelection)	  
	  {
	  if (m_xzoom_is_indeterminate) 		 
		 SetDlgItemText(IDC_XZOOM_EDIT, " ");
	  if (m_yzoom_is_indeterminate)		 
		 SetDlgItemText(IDC_YZOOM_EDIT, " ");
	  if (m_xoff_is_indeterminate)		 
		 SetDlgItemText(IDC_XOFF_EDIT, " ");
	  if (m_yoff_is_indeterminate)		 
		 SetDlgItemText(IDC_YOFF_EDIT, " ");
	  if (m_rotation_is_indeterminate)
		 ((CComboBox*)GetDlgItem(IDC_ROTATION))->SetCurSel(-1);
	  }			
   SetPreview();

   bVarsSet = TRUE;
   }

BOOL CJobSizeProps::OnKillActive() 
   {
   // clean up, write vars, etc.
   CleanUp();   

   if (TimerID) KillTimer(TimerID);
   TimerID = 0;

   // if custom edit and preview is on, hide the window
   if (CustomEdit && m_preview && cwCanvasParent.Exists && pTargetCQueue)	  
	  cwCanvasParent.ShowWindow(SW_HIDE);	     
   return CPropertyPage::OnKillActive();
   }

BOOL CJobSizeProps::OnSetActive() 
   {	   
   LastTickCount = 0;
   if (TimerID) KillTimer(TimerID);   
   TimerID = SetTimer(0, 100, NULL);
   bForceShow = TRUE;

   // if custom edit and preview is on, show the window
   if (CustomEdit && m_preview && cwCanvasParent.Exists && pTargetCQueue)	  
	  cwCanvasParent.ShowWindow(SW_SHOWNA);

   UpdateIt(bLastMultipleSelection);

   bForceShow = FALSE;

   return CPropertyPage::OnSetActive();
   }										

void CJobSizeProps::LoadStandardXforms()
   {       
   int i, NumXforms;
   CComboBox* pCB;
   CTransform* pTransform;
   int DefTransformIdx = -1;  
   
   csCustom.LoadString(IDS_CUSTOMXFORMNAME);
    
   // get the transform combo box
   pCB = (CComboBox*)GetDlgItem(IDC_TRANSFORM);

   pCB->ResetContent();
	
   // get all transforms
   NumXforms = ctl.LoadAll(TRUE);	
   	   
   for (i = 0; i < NumXforms; i++)
       {    	   
	   // get transform
	   pTransform = (CTransform*)ctl.GetAt(i); 
	   // if this is the default, store the index
       if (DefaultTransform.Name == pTransform->Name)
          DefTransformIdx = pCB->GetCount();
       // insert name into list       
	   pCB->AddString(pTransform->Name);
       // insert pTransform into list      
 	   pCB->SetItemDataPtr(pCB->FindString(-1, pTransform->Name), 
	      (void*)pTransform);	  
       }    	    
    
    // select the default transform
    //if (DefTransformIdx != -1) pCB->SetCurSel(DefTransformIdx);	
	if ((DefTransformIdx = pCB->FindString(-1, DefaultTransform.Name)) != CB_ERR) 	   
	   m_name = DefTransformIdx;

   // set buttons
   SetButtons();

   // set description    
   m_description = DefaultTransform.Description;    
   }

void CJobSizeProps::EnableCustomEditing(BOOL bFlag)
   {
   CustomEdit = bFlag;

   // make sure the dialog has been initialized
   if (!DialogInit) return;

   GetDlgItem(IDC_WIDTH_EDIT)->EnableWindow(bFlag);
   GetDlgItem(IDC_WIDTH_SPIN)->EnableWindow(bFlag);
   GetDlgItem(IDC_WIDTH_PROMPT)->EnableWindow(bFlag);
   GetDlgItem(IDC_HEIGHT_EDIT)->EnableWindow(bFlag);
   GetDlgItem(IDC_HEIGHT_SPIN)->EnableWindow(bFlag);
   GetDlgItem(IDC_HEIGHT_PROMPT)->EnableWindow(bFlag);
   GetDlgItem(IDC_XOFF_EDIT)->EnableWindow(bFlag);
   GetDlgItem(IDC_XOFF_SPIN)->EnableWindow(bFlag);
   GetDlgItem(IDC_XOFF_PROMPT)->EnableWindow(bFlag);
   GetDlgItem(IDC_YOFF_EDIT)->EnableWindow(bFlag);
   GetDlgItem(IDC_YOFF_SPIN)->EnableWindow(bFlag);
   GetDlgItem(IDC_YOFF_PROMPT)->EnableWindow(bFlag);
   GetDlgItem(IDC_XZOOM_EDIT)->EnableWindow(bFlag);
   GetDlgItem(IDC_XZOOM_SPIN)->EnableWindow(bFlag);
   GetDlgItem(IDC_XZOOM_PROMPT)->EnableWindow(bFlag);
   GetDlgItem(IDC_YZOOM_EDIT)->EnableWindow(bFlag);
   GetDlgItem(IDC_YZOOM_SPIN)->EnableWindow(bFlag);
   GetDlgItem(IDC_YZOOM_PROMPT)->EnableWindow(bFlag);
   GetDlgItem(IDC_ROTATION)->EnableWindow(bFlag);
   GetDlgItem(IDC_ROTATION_PROMPT)->EnableWindow(bFlag);
   GetDlgItem(ID_REPAINT)->EnableWindow(bFlag && m_preview && pTargetCQueue);
   GetDlgItem(IDC_LOAD)->EnableWindow(bFlag);
   GetDlgItem(IDC_SAVEAS)->EnableWindow(bFlag);
   GetDlgItem(IDC_RESET)->EnableWindow(bFlag);
   GetDlgItem(IDC_SNAP)->EnableWindow(bFlag);
   //GetDlgItem(IDC_PREVIEW)->EnableWindow(bFlag); 
   GetDlgItem(IDC_RETAIN_ASPECT)->EnableWindow(bFlag);
   GetDlgItem(IDC_DESCRIPTION)->EnableWindow(bFlag);
   GetDlgItem(IDC_START_TYPE)->EnableWindow(bFlag);
   GetDlgItem(IDC_START_TYPE_PROMPT)->EnableWindow(bFlag);
   GetDlgItem(IDC_COORDS_PROMPT)->EnableWindow(bFlag);
   GetDlgItem(IDC_UNITS)->EnableWindow(bFlag);         
   GetDlgItem(IDC_UNITS_PROMPT)->EnableWindow(bFlag);
   GetDlgItem(IDC_PREVIEW_SIZE)->EnableWindow(bFlag);
   GetDlgItem(IDC_PREVIEW_SIZE_PROMPT)->EnableWindow(bFlag);
   }

void CJobSizeProps::OnSelchangeTransform() 
{
   // get the transform combo box
   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_TRANSFORM);
   UpdateData(TRUE);

   CTransform* pTransform = (CTransform*)pCB->GetItemDataPtr(pCB->GetCurSel());	   

   // read from ini
   pTransform->ReadFromIni();

   // set buttons
   SetButtons();

   // set description    
   m_description = pTransform->Description;       

   DefaultTransform = *pTransform;

   // put it in dialog
   UpdateData(FALSE);

   UpdateIt(bLastMultipleSelection);

   ReportChange(XFORMCHANGE_ALL);
   }								

void CJobSizeProps::ReportChange(int ItemChanged)
   {
   XFORMCHANGE XFormChange;

   // load structure
   XFormChange.ItemChanged = ItemChanged;
   XFormChange.pCT = &DefaultTransform;   
   
   DefaultTransform.StartType = m_start_type;    
   DefaultTransform.Description = m_description;
   // make sure it's valid before reporting it	  
   if (!m_xzoom_is_indeterminate)
      DefaultTransform.XZoom = m_xzoom;
   if (!m_yzoom_is_indeterminate)
      DefaultTransform.YZoom = m_yzoom;
   DefaultTransform.XOff = DisplayTextToPica(m_xoff);
   DefaultTransform.YOff = DisplayTextToPica(m_yoff);
   MapToDegree(DefaultTransform.Rotate, m_rotation);
   if (pReportWnd && !DoingDefaultProps) 
      pReportWnd->SendMessage(wm_DevModeChange, 
	     PREFS_CHANGE_TRANSFORM, (LPARAM)&XFormChange);	  
   }   

void CJobSizeProps::OnPreview() 
   {
   // get data from dialog
   UpdateData(TRUE);		

   if (CustomEdit)
	  {
	  // enable the repaint button
      GetDlgItem(ID_REPAINT)->EnableWindow(m_preview && pTargetCQueue);
	  if (!m_preview) 
		 {
		 if (cwCanvasParent.GetSafeHwnd())
		    cwCanvasParent.ShowWindow(SW_HIDE);
		 }
	  else  
		 {
		 VerifyCanvas();
		 if (cwCanvasParent.GetSafeHwnd() && IsWindowVisible())
		    cwCanvasParent.ShowWindow(SW_SHOWNA);
		 }
	  }
   else
	  {	  
	  if (cwCanvasParent.GetSafeHwnd())
		 {
	     cwCanvasParent.ShowWindow(SW_HIDE);
	     GetDlgItem(ID_REPAINT)->EnableWindow(FALSE);
		 }
	  }
   }

void CJobSizeProps::SetTransform(CTransform* pTransform)
   {
   // store device
   DefaultTransform = *pTransform;
   }							 

void CJobSizeProps::SetPreview() 
   {
   // do nothing if parent is not visible and we are not doing a "force show"
   if (!IsWindowVisible() && !bForceShow) return;   

   if (CustomEdit)
	  {
	  // enable the repaint button
      GetDlgItem(ID_REPAINT)->EnableWindow(m_preview && pTargetCQueue);
	  if (!m_preview) 
		 {
		 if (cwCanvasParent.GetSafeHwnd())
		    cwCanvasParent.ShowWindow(SW_HIDE);
		 }
	  else  
		 {
		 VerifyCanvas();
		 if (cwCanvasParent.GetSafeHwnd() && IsWindowVisible())
		    cwCanvasParent.ShowWindow(SW_SHOWNA);
		 }
	  }
   else
	  {	  
	  if (cwCanvasParent.GetSafeHwnd())
		 {
	     cwCanvasParent.ShowWindow(SW_HIDE);
	     GetDlgItem(ID_REPAINT)->EnableWindow(FALSE);
		 }
	  }
   }

void CJobSizeProps::FrameShow(BOOL bShow)
   {
   // pass it here so we can hide/unhide the
   // preview window
   if (!bShow)
	  {
	  if (cwCanvasParent.GetSafeHwnd())
		 {
		  CleanUp();
	     cwCanvasParent.ShowWindow(SW_HIDE);	     
		 }
	  }
   }

void CJobSizeProps::CleanUp()
   {
   CString cs;
   CProfile Profile;

   //Anyone who calls this should do this but we'll do it here anyway, why trust anyone when you don't have to?
   if(!cwCanvasParent.GetSafeHwnd())
	   return;

#if 0
   if (m_should_repaint)
	  {	  	  
	  if ((AfxMessageBox(IDS_ACCURATE_VIEW, MB_YESNO)) == IDYES)
		 {
		 OnRepaint();
		 return;
		 }
	  }
#endif 	
   // get data from dialog
   UpdateData(TRUE);          
	
   // get start type, description, zoom, etc.
   DefaultTransform.StartType = m_start_type;    
   DefaultTransform.Description = m_description;
   DefaultTransform.XZoom = m_xzoom;
   DefaultTransform.YZoom = m_yzoom;
   DefaultTransform.XOff = DisplayTextToPica(m_xoff);
   DefaultTransform.YOff = DisplayTextToPica(m_yoff);

   MapToDegree(DefaultTransform.Rotate, m_rotation);
   }

void CJobSizeProps::GetVars()
   {
   CProfile Profile;

   // get snap
   m_snap = Profile.GetInt(IDS_TRANSFORMCONFIG, IDS_SNAP, IDDM_XFORM_SNAP);
   // get aspect
   m_retain_aspect = Profile.GetInt(IDS_TRANSFORMCONFIG, IDS_RETAIN_ASPECT, 
      IDDM_XFORM_RETAIN_ASPECT);
   m_units = Profile.GetInt(IDS_TRANSFORMCONFIG, IDS_UNITS, XFORM_UNITS_INCH);    		
   // give it to our canvas 											
   cw.m_snap = m_snap;
   // give it to our canvas 
   cw.m_retain_aspect = m_retain_aspect;   		
   // get preview size
   m_preview_size = Profile.GetInt(IDS_TRANSFORMCONFIG, IDS_PREVIEW_SIZE, 
      IDDM_PREVIEW_SIZE);   

   UpdateData(FALSE);
   }

void CJobSizeProps::OnSelchangePreviewSize() 
   {
   CProfile Profile;

   // get everything
   UpdateData(TRUE);   

   // redo all the math and redisplay
   UpdateIt(bLastMultipleSelection);

   // write the preview size
   Profile.WriteInt(IDS_TRANSFORMCONFIG, IDS_PREVIEW_SIZE, m_preview_size);
   }

void CJobSizeProps::ResetPreviewWnd()
   {
   // reshow everything
   if (cwCanvasParent.GetSafeHwnd())
	  {
	  // destroy and recreate the window if things have changed
	  if (CanRect != OldCanRect)
		 {	  	  	  
         cwCanvasParent.DestroyWindow();         
	     cwCanvasParent.Exists = FALSE;      	  
		 }
	  VerifyCanvas();
	  }
   }

void CJobSizeProps::OnDelete() 
   {	
   CString csTemp, csPrompt, csSection;   
   CProfile Profile;
   CString csName;
   int idx;

   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_TRANSFORM);

   // get the selection
   UpdateData(TRUE);

   pCB->GetLBText(m_name, csName);

   // make the prompt
   csTemp.LoadString(IDS_DELETEXFORM);
   csPrompt.Format(csTemp, csName);

   // ask if we really want to delete it...
   if (AfxMessageBox(csPrompt, MB_YESNO) == IDYES)
      {
      // delete the entry
      csSection.LoadString(IDS_CUSTOMXFORMS);
      Profile.WriteStr(csSection, csName, NULL);	

      //reload the xforms
      ctl.Clear();
      // get all transforms
      ctl.LoadAll(TRUE);	
   
	  // reload them
	  LoadStandardXforms();
	  
	  // default to smartfit	  
	  if ((idx = pCB->FindString(-1, "SmartFit")) != CB_ERR)
	     m_name = idx;
	  UpdateData(FALSE);
	  }
   }

void CJobSizeProps::OnDestroy() 
   {
   CPropertyPage::OnDestroy();
	
   if (TimerID) KillTimer(TimerID);
   TimerID = 0;
   }

void CJobSizeProps::SetButtons()
   {
   // disable edit box if no custom      
   if (DefaultTransform.Name != csCustom)
      EnableCustomEditing(FALSE);
   else   	  
      EnableCustomEditing(TRUE);

   // we need these
   if (DefaultTransform.Name == csCustom ||
	   DefaultTransform.Name == "SmartFit" ||
	   DefaultTransform.Name == "SmartCrop")
	   GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
   else
	  GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);
   }

void CJobSizeProps::OnChangeHeightEdit() 
   {
   height_is_invalid = TRUE;    
   LastTickCount = ::GetTickCount();
   }

void CJobSizeProps::OnChangeWidthEdit() 
   {
   width_is_invalid = TRUE;    
   LastTickCount = ::GetTickCount();
   }

void CJobSizeProps::OnDeltaposWidthSpin(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   NM_UPDOWN* pStuff = (NM_UPDOWN*)pNMHDR;   

   // get current val
   float fval = (float)atof(m_width);   
   // increment/decrement
   fval += ((float)(-pStuff->iDelta)) * 1.F;
   // put back in
   sprintf(m_width.GetBuffer(64), "%.3f", fval);
   m_width.ReleaseBuffer();
   UpdateData(FALSE);
   width_is_invalid = TRUE;
   LastTickCount = ::GetTickCount();
   *pResult = 0;
   }

void CJobSizeProps::OnDeltaposHeightSpin(NMHDR* pNMHDR, LRESULT* pResult) 
   {
   NM_UPDOWN* pStuff = (NM_UPDOWN*)pNMHDR;   

   // get current val
   float fval = (float)atof(m_height);   
   // increment/decrement
   fval += ((float)(-pStuff->iDelta)) * 1.F;
   // put back in
   sprintf(m_height.GetBuffer(64), "%.3f", fval);
   m_height.ReleaseBuffer();
   UpdateData(FALSE);
   height_is_invalid = TRUE;
   LastTickCount = ::GetTickCount();
   *pResult = 0;
   }
