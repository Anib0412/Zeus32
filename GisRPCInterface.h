/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Wed Sep 21 13:48:02 2005
 */
/* Compiler settings for D:\Source\Zeus32\GisRPCInterface.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __GisRPCInterface_h__
#define __GisRPCInterface_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_GisRPCInterface_0000 */
/* [local] */ 

typedef unsigned int UINT;

typedef int BOOL;

typedef unsigned char BYTE;

typedef unsigned short WORD;

typedef unsigned long DWORD;

typedef struct  _GISRPC_RECT
    {
    int left;
    int top;
    int right;
    int bottom;
    }	GISRPC_RECT;



extern RPC_IF_HANDLE __MIDL_itf_GisRPCInterface_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_GisRPCInterface_0000_v0_0_s_ifspec;

#ifndef __GISRPCInterface_INTERFACE_DEFINED__
#define __GISRPCInterface_INTERFACE_DEFINED__

/* interface GISRPCInterface */
/* [implicit_handle][unique][version][uuid] */ 

typedef /* [context_handle] */ void __RPC_FAR *PCONTEXT_DOC_HANDLE;

typedef /* [context_handle] */ void __RPC_FAR *PCONTEXT_JOB_HANDLE;

typedef /* [context_handle] */ void __RPC_FAR *PCONTEXT_QUEUE_HANDLE;

typedef /* [context_handle] */ void __RPC_FAR *PCONTEXT_PHOTO_HANDLE;

typedef 
enum tagGISRPC_DEVTYPE
    {	GISRPC_devtype_photoprinter	= 0,
	GISRPC_devtype_inkjet	= GISRPC_devtype_photoprinter + 1,
	GISRPC_devtype_dyesub	= GISRPC_devtype_inkjet + 1,
	GISRPC_devtype_filmrecorder	= GISRPC_devtype_dyesub + 1,
	GISRPC_devtype_unknown	= GISRPC_devtype_filmrecorder + 1
    }	GISRPC_DEVTYPE;

typedef 
enum tagGISRPC_IMAGEFILTERTYPE
    {	GISRPC_imagefilter_none	= 0,
	GISRPC_imagefilter_sharpen	= GISRPC_imagefilter_none + 1,
	GISRPC_imagefilter_contrast	= GISRPC_imagefilter_sharpen + 1,
	GISRPC_imagefilter_intensity	= GISRPC_imagefilter_contrast + 1,
	GISRPC_imagefilter_saturation	= GISRPC_imagefilter_intensity + 1,
	GISRPC_imagefilter_gamma	= GISRPC_imagefilter_saturation + 1
    }	GISRPC_IMAGEFILTERTYPE;

typedef 
enum tagGISRPC_SIZING
    {	GISRPC_fit	= 0,
	GISRPC_crop	= GISRPC_fit + 1,
	GISRPC_montage_fit	= GISRPC_crop + 1,
	GISRPC_montage_crop	= GISRPC_montage_fit + 1
    }	GISRPC_SIZING;

typedef 
enum tagGISRPC_SENDFILETYPE
    {	GISRPC_filetype_leftindex_logo	= 0,
	GISRPC_filetype_rightindex_logo	= GISRPC_filetype_leftindex_logo + 1,
	GISRPC_filetype_other	= GISRPC_filetype_rightindex_logo + 1
    }	GISRPC_SENDFILETYPE;

typedef 
enum tagGISRPC_BARCODETYPE
    {	GISRPC_eBC_None	= 0,
	GISRPC_eBC_Code11	= GISRPC_eBC_None + 1,
	GISRPC_eBC_2OF5	= GISRPC_eBC_Code11 + 1,
	GISRPC_eBC_2OF5IL	= GISRPC_eBC_2OF5 + 1,
	GISRPC_eBC_2OF5IATA	= GISRPC_eBC_2OF5IL + 1,
	GISRPC_eBC_2OF5M	= GISRPC_eBC_2OF5IATA + 1,
	GISRPC_eBC_2OF5DL	= GISRPC_eBC_2OF5M + 1,
	GISRPC_eBC_2OF5IND	= GISRPC_eBC_2OF5DL + 1,
	GISRPC_eBC_3OF9	= GISRPC_eBC_2OF5IND + 1,
	GISRPC_eBC_3OF9A	= GISRPC_eBC_3OF9 + 1,
	GISRPC_eBC_EAN8	= GISRPC_eBC_3OF9A + 1,
	GISRPC_eBC_EAN8P2	= GISRPC_eBC_EAN8 + 1,
	GISRPC_eBC_EAN8P5	= GISRPC_eBC_EAN8P2 + 1,
	GISRPC_eBC_EAN13	= GISRPC_eBC_EAN8P5 + 1,
	GISRPC_eBC_EAN13P2	= GISRPC_eBC_EAN13 + 1,
	GISRPC_eBC_EAN13P5	= GISRPC_eBC_EAN13P2 + 1,
	GISRPC_eBC_EAN128	= GISRPC_eBC_EAN13P5 + 1,
	GISRPC_eBC_UPC12	= GISRPC_eBC_EAN128 + 1,
	GISRPC_eBC_CodaBar2	= GISRPC_eBC_UPC12 + 1,
	GISRPC_eBC_CodaBar18	= GISRPC_eBC_CodaBar2 + 1,
	GISRPC_eBC_Code128	= GISRPC_eBC_CodaBar18 + 1,
	GISRPC_eBC_DPLeit	= GISRPC_eBC_Code128 + 1,
	GISRPC_eBC_DPIdent	= GISRPC_eBC_DPLeit + 1,
	GISRPC_eBC_Code16K	= GISRPC_eBC_DPIdent + 1,
	GISRPC_eBC_49	= GISRPC_eBC_Code16K + 1,
	GISRPC_eBC_9OF3	= GISRPC_eBC_49 + 1,
	GISRPC_eBC_UPC25	= GISRPC_eBC_9OF3 + 1,
	GISRPC_eBC_UPCD1	= GISRPC_eBC_UPC25 + 1,
	GISRPC_eBC_UPCD2	= GISRPC_eBC_UPCD1 + 1,
	GISRPC_eBC_UPCD3	= GISRPC_eBC_UPCD2 + 1,
	GISRPC_eBC_UPCD4	= GISRPC_eBC_UPCD3 + 1,
	GISRPC_eBC_UPCD5	= GISRPC_eBC_UPCD4 + 1,
	GISRPC_eBC_UPCSCC	= GISRPC_eBC_UPCD5 + 1,
	GISRPC_eBC_UCC128	= GISRPC_eBC_UPCSCC + 1,
	GISRPC_eBC_UPCA	= GISRPC_eBC_UCC128 + 1,
	GISRPC_eBC_UPCAP2	= GISRPC_eBC_UPCA + 1,
	GISRPC_eBC_UPCAP5	= GISRPC_eBC_UPCAP2 + 1,
	GISRPC_eBC_UPCE	= GISRPC_eBC_UPCAP5 + 1,
	GISRPC_eBC_UPCEP2	= GISRPC_eBC_UPCE + 1,
	GISRPC_eBC_UPCEP5	= GISRPC_eBC_UPCEP2 + 1,
	GISRPC_eBC_PostNet5	= GISRPC_eBC_UPCEP5 + 1,
	GISRPC_eBC_PostNet6	= GISRPC_eBC_PostNet5 + 1,
	GISRPC_eBC_PostNet8	= GISRPC_eBC_PostNet6 + 1,
	GISRPC_eBC_PostNet10	= GISRPC_eBC_PostNet8 + 1,
	GISRPC_eBC_PostNet11	= GISRPC_eBC_PostNet10 + 1,
	GISRPC_eBC_PostNet12	= GISRPC_eBC_PostNet11 + 1,
	GISRPC_eBC_Plessey	= GISRPC_eBC_PostNet12 + 1,
	GISRPC_eBC_MSI	= GISRPC_eBC_Plessey + 1,
	GISRPC_eBC_POSTCODE	= GISRPC_eBC_MSI + 1,
	GISRPC_eBC_FIM	= GISRPC_eBC_POSTCODE + 1,
	GISRPC_eBC_LOGMARS	= GISRPC_eBC_FIM + 1,
	GISRPC_eBC_DUN	= GISRPC_eBC_LOGMARS + 1,
	GISRPC_eBC_PHILIPS	= GISRPC_eBC_DUN + 1,
	GISRPC_eBC_DOTCODEA	= GISRPC_eBC_PHILIPS + 1,
	GISRPC_eBC_GP	= GISRPC_eBC_DOTCODEA + 1,
	GISRPC_eBC_PDF417	= GISRPC_eBC_GP + 1,
	GISRPC_eBC_PDF417Trunc	= GISRPC_eBC_PDF417 + 1,
	GISRPC_eBC_MAXICODE	= GISRPC_eBC_PDF417Trunc + 1,
	GISRPC_eBC_QRCode	= GISRPC_eBC_MAXICODE + 1,
	GISRPC_eBC_Code128A	= GISRPC_eBC_QRCode + 1,
	GISRPC_eBC_Code128B	= GISRPC_eBC_Code128A + 1,
	GISRPC_eBC_Code128C	= GISRPC_eBC_Code128B + 1,
	GISRPC_eBC_9OF3A	= GISRPC_eBC_Code128C + 1
    }	GISRPC_BARCODETYPE;

typedef 
enum tagGISRPC_JOBSTATUS
    {	GISRPC_jobstatus_ok	= 0,
	GISRPC_jobstatus_error	= GISRPC_jobstatus_ok + 1,
	GISRPC_jobstatus_undefined	= GISRPC_jobstatus_error + 1
    }	GISRPC_JOBSTATUS;

typedef 
enum tagGISRPC_PHOTOSTATUS
    {	GISRPC_photo_idle	= 0,
	GISRPC_photo_printing	= GISRPC_photo_idle + 1,
	GISRPC_photo_printed_ok	= GISRPC_photo_printing + 1,
	GISRPC_photo_error_printing	= GISRPC_photo_printed_ok + 1,
	GISRPC_photo_notfound	= GISRPC_photo_error_printing + 1
    }	GISRPC_PHOTOSTATUS;

typedef 
enum tagGISRPC_QUALITY
    {	GISRPC_quality_dont_care	= 0,
	GISRPC_quality_low	= GISRPC_quality_dont_care + 1,
	GISRPC_quality_medium	= GISRPC_quality_low + 1,
	GISRPC_quality_high	= GISRPC_quality_medium + 1
    }	GISRPC_QUALITY;

typedef 
enum tagGISRPC_PRIORITY
    {	GISRPC_priority_none_specified	= 0,
	GISRPC_priority_low	= GISRPC_priority_none_specified + 1,
	GISRPC_priority_medium	= GISRPC_priority_low + 1,
	GISRPC_priority_high	= GISRPC_priority_medium + 1
    }	GISRPC_PRIORITY;

typedef struct  tagGISRPC_DEVSTRING
    {
    unsigned char lpsGisRpcString[ 256 ];
    }	GISRPC_DEVSTRING;

typedef struct tagGISRPC_DEVSTRING __RPC_FAR *PGISRPC_DEVSTRING;

typedef struct  tagGISRPC_DEVINFO
    {
    int __RPC_FAR *hDevice;
    unsigned char lpsLongDevName[ 256 ];
    unsigned char lpsShortDevName[ 256 ];
    unsigned char lpsSvrLocation[ 256 ];
    unsigned char lpsDevLocation[ 256 ];
    BOOL bIsAvailable;
    int iNumBackPrintLines;
    BOOL bCanDoCustomCanvas;
    int iNumCanvases;
    int iCurrentCanvas;
    GISRPC_DEVTYPE iDevType;
    BOOL bRollMedia;
    int iMaxWidthRollInches1000;
    int iMaxLengthRollInches1000;
    int iPrintsPerHour;
    int iNumConfigOptions;
    }	GISRPC_DEVINFO;

typedef struct tagGISRPC_DEVINFO __RPC_FAR *PGISRPC_DEVINFO;

typedef struct  tagGISRPC_CANVAS
    {
    unsigned char lpsCanvasName[ 256 ];
    unsigned char lpsCanvasDesc[ 256 ];
    int iWidthInches1000;
    int iHeightInches1000;
    int iStyle;
    }	GISRPC_CANVAS;

typedef struct tagGISRPC_CANVAS __RPC_FAR *PGISRPC_CANVAS;

typedef struct  tagGISRPC_IMAGEFILTER
    {
    int iNumFiltersToUse;
    GISRPC_IMAGEFILTERTYPE pgprcImageFilterType[ 5 ];
    int iVar[ 5 ];
    }	GISRPC_IMAGEFILTER;

typedef struct tagGISRPC_IMAGEFILTER __RPC_FAR *PGISRPC_IMAGEFILTER;

typedef struct  tagGISRPC_PHOTOSPECS
    {
    unsigned int hPhoto;
    unsigned char lpszDiskFile[ 256 ];
    unsigned char lpszOutFile[ 256 ];
    GISRPC_IMAGEFILTER pgprcImageFilter;
    int iCopies;
    int iUndefined;
    unsigned char lpszProductName[ 256 ];
    BOOL bApplyFilter;
    BOOL bDeleteWhenDone;
    GISRPC_SIZING pgprcSizing;
    int iCanvasToUse;
    int iMontageWidthInches1000;
    int iMontageHeightInches1000;
    int iMontagePhotoSpacingXInches1000;
    int iMontagePhotoSpacingYInches1000;
    BOOL bSendActualData;
    int iPhotoSize;
    BYTE PhotoData[ 32000 ];
    int iPhotoBytesCopied;
    }	GISRPC_PHOTOSPECS;

typedef struct tagGISRPC_PHOTOSPECS __RPC_FAR *PGISRPC_PHOTOSPECS;

typedef struct  tagGISRPC_PHOTOSPECS2
    {
    GISRPC_PHOTOSPECS BasePhotoSpecs;
    unsigned char cBackPrint1[ 256 ];
    unsigned char cBackPrint2[ 256 ];
    unsigned char cBackPrint3[ 256 ];
    unsigned char Undefined[ 256 ];
    unsigned char Undefined2[ 256 ];
    unsigned char Undefined3[ 256 ];
    unsigned char Undefined4[ 256 ];
    int Undefined5;
    int Undefined6;
    int Undefined7;
    int Undefined8;
    }	GISRPC_PHOTOSPECS2;

typedef struct tagGISRPC_PHOTOSPECS2 __RPC_FAR *PGISRPC_PHOTOSPECS2;

typedef struct  tagGISRPC_OURPHOTOSPECS
    {
    GISRPC_PHOTOSPECS2 PhotoSpecs;
    unsigned char lpszTempDiskFile[ 256 ];
    BOOL bUseTempFile;
    int iPage;
    PCONTEXT_QUEUE_HANDLE hCqueueFile;
    int iJobID;
    int iPhotoCount;
    BOOL bPutInThumbnails;
    int FileType;
    int iXoffInches1000;
    int iYoffInches1000;
    int bIsSeparator;
    BOOL bMontageThis;
    BOOL bMontageDone;
    BOOL bAddedToQueue;
    int iReportedCopyCount;
    }	GISRPC_OURPGPRCPHOTOSPECS;

typedef struct tagGISRPC_OURPHOTOSPECS __RPC_FAR *PGISRPC_OURPGPRCPHOTOSPECS;

typedef struct  tagGISRPC_SENDFILEINFO
    {
    GISRPC_SENDFILETYPE pgprcFileType;
    unsigned char lpszLocalFileName[ 256 ];
    unsigned char lpszFileName[ 256 ];
    int iFileSize;
    unsigned char ExtraData[ 256 ];
    int iExtraInfo;
    int iUndefined1;
    int iUndefined2;
    int iUndefined3;
    int iUndefined4;
    }	GISRPC_SENDFILEINFO;

typedef struct tagGISRPC_SENDFILEINFO __RPC_FAR *PGISRPC_SENDFILEINFO;

typedef struct  tagGISRPC_JOBSEPARATOR
    {
    int iStyle;
    unsigned int hPhoto;
    BOOL bDoThumbnails;
    BOOL bDoBarCode;
    GISRPC_BARCODETYPE pgprcBarCodeType;
    BOOL bTextUnderBarCode;
    BOOL bDoLeftLogo;
    BOOL bDoRightLogo;
    unsigned char lpszStoreID[ 256 ];
    unsigned char lpszAddress[ 6 ][ 256 ];
    unsigned char lpszOrderInfo[ 20 ][ 256 ];
    unsigned char lpszShipMethod[ 256 ];
    unsigned char lpszLeftLogoFile[ 256 ];
    unsigned char lpszRightLogoFile[ 256 ];
    unsigned char TimeStamp[ 256 ];
    int iFontSize[ 3 ];
    GISRPC_SIZING pgprcSizing;
    int iCanvasToUse;
    int iMontageWidthInches1000;
    int iMontageHeightInches1000;
    int iMontagePhotoSpacingXInches1000;
    int iMontagePhotoSpacingYInches1000;
    int iContactImage_Width;
    int iContactImage_Height;
    int iContactImage_SpacingX;
    int iContactImage_SpacingY;
    int iContactImage_NumAccross;
    int iContactImage_NumDown_Page_1;
    int iContactImage_NumDown_Page_N;
    int iSeparatorBBox_Width;
    int iSeparatorBBox_Height;
    float fSeparatorBBox_Width;
    float fSeparatorBBox_Height;
    }	GISRPC_JOBSEPARATOR;

typedef struct tagGISRPC_JOBSEPARATOR __RPC_FAR *PGISRPC_JOBSEPARATOR;

typedef struct  tagGISRPC_JOBSTATUSSTRUCT
    {
    int iTotalPhotos;
    int iTotalPages;
    int iHoldCount;
    int iPrintCount;
    int iArchiveCountGood;
    int iArchiveCountBad;
    unsigned char FirstError[ 256 ];
    GISRPC_JOBSTATUS pgprcJobStatus;
    }	GISRPC_JOBSTATUSSTRUCT;

typedef struct tagGISRPC_JOBSTATUSSTRUCT __RPC_FAR *PGISRPC_JOBSTATUSSTRUCT;

typedef struct  tagGISRPC_PHOTOSTATUSSTRUCT
    {
    unsigned int hJob;
    unsigned int hPhoto;
    int iPaneLocation;
    GISRPC_PHOTOSTATUS pgprcJobStatus;
    unsigned char StatusStr[ 256 ];
    }	GISRPC_PHOTOSTATUSSTRUCT;

typedef struct tagGISRPC_PHOTOSTATUSSTRUCT __RPC_FAR *PGISRPC_PHOTOSTATUSSTRUCT;

typedef struct  tagGISRPC_PRINTSPECS
    {
    GISRPC_QUALITY pgprcQuality;
    BOOL bPrintToHold;
    GISRPC_PRIORITY pgprcPriority;
    BOOL bPromptBeforePrint;
    int isecStartDelay;
    BOOL bPrintJobSeparatorOnly;
    unsigned char Owner[ 256 ];
    int OrderID;
    unsigned char OrderIDString[ 256 ];
    }	GISRPC_PRINTSPECS;

typedef struct tagGISRPC_PRINTSPECS __RPC_FAR *PGISRPC_PRINTSPECS;

long GISRPCGetServerVersion( 
    /* [out] */ int __RPC_FAR *iVersion);

long GISRPCGetNumDevices( 
    /* [out] */ int __RPC_FAR *iNumDevices);

long GISRPCGetDevInfo( 
    /* [in] */ int __RPC_FAR *iIdx,
    /* [out] */ GISRPC_DEVINFO __RPC_FAR *pDevInfo,
    /* [out] */ PCONTEXT_DOC_HANDLE __RPC_FAR *phDevice);

long GISRPCGetCanvasInfo( 
    /* [in] */ PCONTEXT_DOC_HANDLE phDevice,
    /* [in] */ int __RPC_FAR *iIdx,
    /* [out] */ GISRPC_CANVAS __RPC_FAR *GISCanvas);

long GISRPCSetBackPrintLine( 
    /* [in] */ PCONTEXT_DOC_HANDLE phDevice,
    /* [in] */ int __RPC_FAR *iLine);

long GISRPCGetDevOptionName( 
    /* [in] */ PCONTEXT_DOC_HANDLE phDevice,
    /* [in] */ int __RPC_FAR *iOption,
    /* [out] */ GISRPC_DEVSTRING __RPC_FAR *pOptionName);

long GISRPCGetDevOptionSettingName( 
    /* [in] */ PCONTEXT_DOC_HANDLE phDevice,
    /* [in] */ int __RPC_FAR *iOption,
    /* [in] */ int __RPC_FAR *iSetting,
    /* [out] */ GISRPC_DEVSTRING __RPC_FAR *pOptionName);

long GISRPCGetDevOption( 
    /* [in] */ PCONTEXT_DOC_HANDLE phDevice,
    /* [in] */ int __RPC_FAR *iOption,
    /* [out] */ int __RPC_FAR *iSetting);

long GISRPCSetDevOption( 
    /* [in] */ PCONTEXT_DOC_HANDLE phDevice,
    /* [in] */ int __RPC_FAR *iOption,
    /* [in] */ int __RPC_FAR *iSetting);

long GISRPCOpenJob( 
    /* [in] */ PCONTEXT_DOC_HANDLE phDevice,
    /* [string][in] */ unsigned char __RPC_FAR *jobName,
    /* [out] */ PCONTEXT_JOB_HANDLE __RPC_FAR *phJob);

long GISRPCCloseJob( 
    /* [in] */ PCONTEXT_JOB_HANDLE hJob);

long GISRPCPrintJob( 
    /* [in] */ PCONTEXT_JOB_HANDLE hJob,
    /* [in] */ GISRPC_PRINTSPECS __RPC_FAR *pPrintSpecs);

long GISRPCAddJobSeparator( 
    /* [in] */ PCONTEXT_JOB_HANDLE hJob,
    /* [in] */ GISRPC_JOBSEPARATOR __RPC_FAR *pJobSepinfo);

long GISRPCPutFile( 
    /* [in] */ PCONTEXT_JOB_HANDLE hJob,
    /* [in] */ GISRPC_SENDFILEINFO __RPC_FAR *pSendFileInfo);

long GISRPCAddPhoto( 
    /* [in] */ PCONTEXT_JOB_HANDLE hJob,
    /* [in] */ GISRPC_PHOTOSPECS __RPC_FAR *pPhotoSpecs,
    /* [out] */ GISRPC_DEVSTRING __RPC_FAR *pOutFileName);

long GISRPCAddPhoto2( 
    /* [in] */ PCONTEXT_JOB_HANDLE hJob,
    /* [in] */ GISRPC_PHOTOSPECS2 __RPC_FAR *pPhotoSpecs);


extern handle_t GISRPCInterface_ifHandle;


extern RPC_IF_HANDLE GISRPCInterface_v1_0_c_ifspec;
extern RPC_IF_HANDLE GISRPCInterface_v1_0_s_ifspec;
#endif /* __GISRPCInterface_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

void __RPC_USER PCONTEXT_DOC_HANDLE_rundown( PCONTEXT_DOC_HANDLE );
void __RPC_USER PCONTEXT_JOB_HANDLE_rundown( PCONTEXT_JOB_HANDLE );

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
