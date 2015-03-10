#ifndef __OBJECTS_H__
#define __OBJECTS_H__
/*
#include "xercesc\sax\HandlerBase.hpp"
#include "xercesc\util\platformutils.hpp"
#include "xercesc\parsers\SAXParser.hpp"
#include "xercesc\util\XMLException.hpp"
#include "xercesc\framework\MemBufInputSource.hpp"	
#include "..\PhotogizeProxyClient\PhotogizeProxyClientClass.h"
#include "TransformCMM.h"
*/


#include "..\CmnLibs\Current LeadTools\include\l_bitmap.h"
#include "..\CmnLibs\Current LeadTools\include\l_error.h"

//#include "OrderDetails.h"

// very handy
#ifndef VERIFYPATH
#define VERIFYPATH(a) if (a.GetLength() > 0) { if (a[a.GetLength() - 1] != '\\' &&       \
a[a.GetLength() - 1] != ':') a += "\\"; }
#endif

#ifndef RPDEBUGSTRMFC
// send a debug string to our debug window
#define RPDEBUGSTRMFC(a)                                  \
   {													  \
   HWND hWnd;											  \
   COPYDATASTRUCT cpds;						              \
   cpds.dwData = 0;										  \
   cpds.lpData = (PVOID)(char*)(const char*)a;	 		  \
   cpds.cbData = strlen(a) + 1;							  \
   if (hWnd = ::FindWindow("RPDebug_Class", NULL))		  \
   ::SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cpds);	  \
   }
#define RPDEBUGSTR(a)                                     \
   {													  \
   HWND hWnd;											  \
   COPYDATASTRUCT cpds;						              \
   cpds.dwData = 0;										  \
   cpds.lpData = (PVOID)a;								  \
   cpds.cbData = strlen(a) + 1;							  \
   if (hWnd = FindWindow("RPDebug_Class", NULL))		  \
      SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cpds);	  \
   }
#endif


#define NODETYPE_UNDEFINED          0x00
#define NODETYPE_CATEGORY           0x01
#define NODETYPE_LOCALDRIVE         0x02
#define NODETYPE_NETWORKDRIVE		0x04
#define NODETYPE_FOLDER             0x08
#define NODETYPE_ALBUM              0x10
#define NODETYPE_USER               0x20 
#define NODETYPE_PHOTO              0x40
#define NODETYPE_ORDER				0x80
#define NODETYPE_PRODUCT            0x100 
#define NODETYPE_IMAGE              0x200 
#define NODETYPE_ACCOUNT			0x400 

#define IMAGESIZING_FIT         0
#define IMAGESIZING_CROP        1
#define IMAGESIZING_CUSTOM      2

#define IMAGESTYLE_UNDEFINED  -1
#define IMAGESTYLE_ORIGINAL   0
#define IMAGESTYLE_SPAWN      1 

#define SUBTYPE_UNKNOWN          0x00
#define SUBTYPE_ORDER            0x01
#define SUBTYPE_PRODUCT          0x02
#define SUBTYPE_PHOTOS           0x03

// for accounts
#define SUBTYPE_MASTERACCT       0
#define SUBTYPE_SLAVEACCT        1

// for products
#define SUBTYPE_PRODUCT_RPFULFILL		 0
#define SUBTYPE_PRODUCT_NOFULFILL		 1
#define SUBTYPE_PRODUCT_WHOLESALE		 2

#define PRODUCT_TYPE_UNDEFINED      -1
#define PRODUCT_TYPE_PRINTSERVER    0
#define PRODUCT_TYPE_NOFULFILL      1
#define PRODUCT_TYPE_WHOLESALE      2

#define ORDER_PRINTRULE_ALLSUBMITTED                 0
#define ORDER_PRINTRULE_ALLSUBMITTED_VALIDEMAIL      1
#define ORDER_PRINTRULE_SUBMITTED_ONLY_THESE         2
#define ORDER_PRINTRULE_ALLSUBMITTED_EXCEPT_THESE    3

#define ONLINEIMAGECACHECULLTIME           30 // days

#define EMAILTRIGGER_HARVESTED              0	  
#define EMAILTRIGGER_SHIP_PRINTEDOK         1
#define EMAILTRIGGER_PICKUP_PRINTEDOK       2
#define EMAILTRIGGER_ERRORPRINTING          3
#define EMAILTRIGGER_SHIP_ORDER_COMPLETE    4
#define EMAILTRIGGER_PICKUP_ORDER_COMPLETE  5
#define EMAILTRIGGER_READYFORPICKUP			6
#define EMAILTRIGGER_SHIPPED				7
#define EMAILTRIGGER_NUMTRIGGERS            10

#define ORDERSTATUS_LOCALCREATE      0
#define ORDERSTATUS_SUBMITTED        1
#define ORDERSTATUS_PRINTED_OK		 2
#define ORDERSTATUS_PRINTED_ERROR    3
#define ORDERSTATUS_PRINTED_COMPLETE 4
#define ORDERSTATUS_READYFORPICKUP   5
#define ORDERSTATUS_HARVESTED		 6
#define ORDERSTATUS_SHIPPED			 7

#define XCONNECT_UNITS_MINUTES       0
#define XCONNECT_UNITS_HOURS         1
#define XCONNECT_DEFAULT_INT         4

#define ORDERTYPE_WEBSERVICES        0
#define ORDERTYPE_LOCALCREATE		 1
#define ORDERTYPE_XCONNECT			 2
#define ORDERTYPE_DLS				 3

#define HARVEST_RULE_NONE          0 
#define HARVEST_RULE_SHIPVIA       1
#define HARVEST_RULE_PICKUPLOC     2
#define HARVEST_RULE_PRODUCT       3 

#define CDOPTION_NONE			   0x00000000
#define CDOPTION_HTMLINDEX         0x00000001
#define CDOPTION_AUTOSTART         0x00000002

#define BACKPRINT_NONE             0x00000000
#define BACKPRINT_CLRADJ		   0x00000001
#define BACKPRINT_DATE		       0x00000002
#define BACKPRINT_EMAIL  		   0x00000004
#define BACKPRINT_FNAME 		   0x00000008
#define BACKPRINT_ORDERID 		   0x00000010
#define BACKPRINT_CUSTOM           0x00000020

#define DEFAULTALBUMLOGO        "PhotogizeAlbum.gif"
#define DEFAULTINVOICELOGO      "PhotogizeAlbum.gif"
#define DEFAULTLEFTINDEXLOGO    "PhotogizeLeftIndex.jpg"
#define DEFAULTRIGHTINDEXLOGO   "PhotogizeRightIndex.jpg"
#define DEFAULTWEBLEFTLOGO      "kids ad.gif"
#define DEFAULTWEBRIGHTLOGO     "twins ad.gif"

#define USE_HARVESTRULES_NONE    0
#define USE_HARVESTRULES_LOCAL   1
#define USE_HARVESTRULES_MASTER  2

// forward references
class CAccount;
class CImageCache;
class CProduct;
class CICCCache;
class CICCCacheElement;

#ifndef PROMOTYPE_DISCOUNT_PERCENT
#define PROMOTYPE_DISCOUNT_PERCENT		  0x00000001
#define PROMOTYPE_DISCOUNT_ABSOLUTE		  0x00000002
#define PROMOTYPE_DISCOUNT_2NDFREE		  0x00000004
#define PROMOTYPE_APPLYTO_ALL			  0x00000010
#define PROMOTYPE_APPLYTO_PRODUCT		  0x00000020
#endif

class CNode : public CObject
   {
DECLARE_SERIAL(CNode)
public:
   CNode();
   ~CNode();
   int iType;
   int iSubType;
   BOOL bSelected; // we use this temporarily, so we don't need to serialize or copy it
   BOOL bToDelete; // we use this temporarily, so we don't need to serialize or copy it
   CString csUser;
   CString csDisplayName;
   CString csName;
   CString csNotes; 
   CObArray coNodes;   
   BOOL bIsExpanded;
   BOOL bPopulated;
   BOOL bMakeBold;
   CAccount* pAccountOwner; // save the pointer to the account node
   virtual void Serialize(CArchive& ar);
   const CNode& operator=(const CNode& cqfSrc);  // assignment
   int GetIndex(CNode* pParentNode);
   };

class CPromo : public CObject
   {
DECLARE_SERIAL(CPromo)
public:
   CPromo();
   ~CPromo();   
   CString csName;
   CString csDescription;
   CString csCode;   
   int iType;
   CString csApplyTo;
   int iAmount;
   int iMinOrder;
   CTime ctStart;
   CTime ctEnd;   

   BOOL operator==(const CPromo& iprm);              // compare      
   BOOL operator!=(const CPromo& iprm);              // compare         
   BOOL IsSame(const CPromo& iadj);
   const CPromo& operator=(const CPromo& cpSrc);  // assignment
   virtual void Serialize(CArchive& ar);
   };

class CRedEyeAdjust : public CObject
   {
DECLARE_SERIAL(CRedEyeAdjust)
public:
   CRedEyeAdjust();
   ~CRedEyeAdjust();   
   
   COLORREF rcNewColor;   
   CRect rcSetRegion;
   int uThreshold;	
   int nLightness;	
   int iOriginalWidth;
   int iOriginalHeight;

   BOOL operator==(const CRedEyeAdjust& iadj);              // compare      
   BOOL operator!=(const CRedEyeAdjust& iadj);              // compare         
   BOOL IsSame(const CRedEyeAdjust& iadj);
   const CRedEyeAdjust& operator=(const CRedEyeAdjust& cpSrc);  // assignment
   virtual void Serialize(CArchive& ar);
   };

class CImageAdjustment : public CObject
   {
DECLARE_SERIAL(CImageAdjustment)
public:
   CImageAdjustment();
   ~CImageAdjustment();      
   BOOL IsAdjusted(BOOL bIgnoreRotate=FALSE);
   void Revert();
   
   int iLightDark;
   int iContrast;
   int iSharpness;
   int iSaturation;
   int iGamma;
   int iRedAdjust;
   int iGreenAdjust;
   int iBlueAdjust;      
   BOOL bDoGrayScale;
   BOOL bUseAutoLevels;
   BOOL bDoSepiaTone;
   int iRotate;   
   // red eye stuff
   BOOL bInRedEyeMode;
   CObArray coRedEyeArray;

   BOOL operator==(const CImageAdjustment& iadj);              // compare      
   BOOL operator!=(const CImageAdjustment& iadj);              // compare         
   BOOL IsSame(const CImageAdjustment& iadj);
   const CImageAdjustment& operator=(const CImageAdjustment& cpSrc);  // assignment
   void Serialize(CArchive& ar, int nVersion=0);
protected:   
   };

class CEmbeddedICCInfo : public CObject
   {
DECLARE_SERIAL(CEmbeddedICCInfo)
public:
   CEmbeddedICCInfo();
   ~CEmbeddedICCInfo();
   void Reset();
   BOOL GetICCInfo(LPCSTR lpcstrIn);   
   BOOL ExtractICCtoFile(LPCSTR lpcstrIn, LPCSTR lpcstrOut);
   BOOL bScanned;
   BOOL bExists;
   CString csName;
   unsigned int fpos; //position in file
   unsigned int size;    /* Profile size in bytes */	
   long deviceClass;	    /* Type of profile */
   long colorSpace;	    /* Clr space of data */
   long pcs;			    /* PCS: XYZ or Lab */
   int renderingIntent;  /* Rendering intent */
   int manufacturer;	/* Dev manufacturer */
   int model;			/* Dev model */
   unsigned long attributes_l;
   unsigned long attributes_h;
   unsigned int            flags;			/* Various bits */
   long creator;		/* Profile creator */					  	  
   long cmmId;			/* CMM for profile */
   int majv, minv, bfv;/* Format version - major, minor, bug fix */
   unsigned int      year;
   unsigned int      month;
   unsigned int      day;
   unsigned int      hours;
   unsigned int      minutes;
   unsigned int      seconds;
   long platform;		/* Primary Platform */
   double illuminant_X;		/* Profile illuminant */
   double illuminant_Y;		/* Profile illuminant */
   double illuminant_Z;		/* Profile illuminant */      
   virtual void Serialize(CArchive& ar);
   const CEmbeddedICCInfo& operator=(const CEmbeddedICCInfo& cqfSrc);  // assignment
   BOOL operator==(const CEmbeddedICCInfo& iadj);              // compare      
   BOOL operator!=(const CEmbeddedICCInfo& iadj);              // compare         
   BOOL IsSame(const CEmbeddedICCInfo& iadj);
   };										

class CImage : public CNode
   {
DECLARE_SERIAL(CImage)
public:
   CImage();
   ~CImage();         
   CString csFileName;   
   int iWidth;
   int iHeight;
   int iWidthSizeTo;    // we need to retain the original "size-to" width
   int iHeightSizeTo;   // and height for cache hashing
   int iStyle;
   int iSizing;   
   BOOL* pbCancelLoad;
   CTime ctFileTime;
   long FileSize;
   CString csFileSize;
   BITMAPHANDLE Bitmap;   
   BITMAPHANDLE BitmapCorrected;      
   BITMAPHANDLE BitmapGray;
   int iBitmapGrayRotation;
//   BITMAPHANDLE* GetCorrectedBitmapHandle(CImageAdjustment& cImageAdjustPhoto, 
//	  CTransformCMM* pPassedTransformCMM);
   BITMAPHANDLE* GetGrayScaleBitmapHandle(int iRotate=0);
   void CorrectBitmap(CImageAdjustment& cImageAdjustPhoto);   
   CImageAdjustment cImageAdjustActive;   
   BOOL LoadImage();   
   void MakeCoolFileSizeStr();
   const CImage& operator=(const CImage& cqfSrc);  // assignment
   virtual void Serialize(CArchive& ar);   
//   CTransformCMM* pTransformCMM;   
   };

class CProduct;

class CPhoto : public CNode
   {
DECLARE_SERIAL(CPhoto)
public:
   CPhoto();
   ~CPhoto();         
   CString csOwner;   
   BOOL bCancelLoad;
   BOOL bIsLocal;   
   int iImageIndex;
   int iCopies;
   BOOL bOverrideSizing;
   int iSizing;   
   BOOL bDeleteAfterPrinting;
   BOOL bIsPromoPhoto;
   CString csProductName;   
   CImageAdjustment cImageAdjust;   
   CImageAdjustment cImageAdjustUndo;   
   CImageAdjustment cImageAdjustRedo;   
   BOOL bCanUndoImageAdjust;
   BOOL bCanRedoImageAdjust;
   void UndoImageAdjust();
   void RedoImageAdjust();
   CRect crCrop;   
   CObArray coImages;         
   CObArray coWinBmp;  // an array of square images converted to Windows HBITMAP format
   CImage* GetOriginalImage();
   void CreateNewTempFile();
   void DeleteDiskFile(BOOL bAlwaysDelete=FALSE);   
   CImage* CreateImage(int iWidth, int iHeight, int& iTop, int& iLeft, int iSizing);
   void CreateFitDims(int iWidth, int iHeight, int iFitWidth, int iFitHeight, 
	  int& iNewWidth, int& iNewHeight, int& iTop, int& iLeft, 
	  int iSizing);
   HBITMAP GetWinBmp(int iSize);
   BOOL HasWinBmp(int iSize);
   CImageCache* pCache;  // pointer to our image cache
   void AddToCache(CImage* pImage, int iTop, int iLeft);
   CImage* GetOptimumPreview(int iSize);
   CImage* GetOriginalPreview();
   CImage* GetSampledImage(int iSize);
   BOOL CopyNamedFile(CString& csDstFolder, BOOL bRename=TRUE);
   int GetSizingToUse(CProduct* pProduct);
   const CPhoto& operator=(const CPhoto& cqfSrc);  // assignment
   virtual void Serialize(CArchive& ar);           
   void ClearMemoryCache();
   void ClearDiskCache(CString& csFileNameCompare, CTime& ctFileTimeCompare, 
      long FileSizeCompare);
   void SetAdjustmentsString(CString& m_adjustments);
   int GetNumAdjustments();
   CICCCacheElement* CheckCache(CEmbeddedICCInfo* pEmbeddedICCInfo);
   BOOL GetEmbeddedICCProfile(CString& csProfileFile);
   CEmbeddedICCInfo EmbeddedICCInfo;
   CICCCache* pICCCache;  
   BOOL GetOriginalImageFileName(CString& csFileName);
//   BOOL ApplyImageAdjustments(CProduct* pProduct, CString& csFileNameIn, 
//      CString& csFileNameOut, CTransformCMM* pTransformCMM, BOOL bIgnoreRotate=TRUE, 
//	  BOOL bApplyCropAndFit=FALSE);
   void SeedCropBox(CProduct* pProduct);
   void FitInMedia(int& iPicWidth, int& iPicHeight, CRect& crFitRect, CProduct* pProduct);
   void GetMediaRectForFit(int& iPicWidth, int& iPicHeight, CProduct* pProduct, CRect& crMediaRect);
   BOOL CreateFitBitmap(BITMAPHANDLE* pBitmapSrc, CRect& crFitRect);

protected:
   CImage* CheckCache(int iWidth, int iHeight, int iSizing, int& iTop, int& iLeft);
   void DeleteCacheFiles();
   void DrawThumbCropRect(BITMAPHANDLE* pBitmap);
   };

class CWinBmp : public CObject
   {
DECLARE_SERIAL(CWinBmp)
public:
   CWinBmp();
   ~CWinBmp();
   HBITMAP hBitmap;
   int iSize;
   CImageAdjustment cImageAdjustActive;
   };

class CDevOption : public CObject
   {
DECLARE_SERIAL(CDevOption)
public:
   CDevOption();
   ~CDevOption();
   
   CString csName;
   int iSetting;
   CStringArray csaSettings;   
   virtual void Serialize(CArchive& ar);
   const CDevOption& operator=(const CDevOption& cqfSrc);  // assignment
   };	  

class CProduct : public CNode
   {
DECLARE_SERIAL(CProduct)
public:
   CProduct();
   ~CProduct();      
   void DeleteDevOptionArray();   
   CString csDescription;  // description
   int iProductType;       // type of product
   int iUnitPricePennies;  // the unit price in pennies
   BOOL bIsLocal;
   BYTE IPAddress_a;      // IP address for Photogize.ProxyServer
   BYTE IPAddress_b;      // IP address for Photogize.ProxyServer
   BYTE IPAddress_c;      // IP address for Photogize.ProxyServer
   BYTE IPAddress_d;      // IP address for Photogize.ProxyServer
   CString csDevName;      // device name   
   CString csCanvasName;   // canvas name   
   int iDevice;	           // index of device
   int iMedia;	           // index of media on which to print
   int iSizing;                 // sizing
   int iBackPrint1;              // backprinting
   int iBackPrint2;              // backprinting
   CString csBackPrint1;
   CString csBackPrint2;
   CString csDevProfile;
   BOOL bApplyICCToPrint;
   BOOL bCustomPageSize;        // use custom page size
   int iMediaWidthInches1000;   // width of the imageable area in inches x 1000
   int iMediaHeightInches1000;  // height of the imageable area in inches x 1000   
   int iPrintWidthInches1000;   // width of the custom defined page size
   int iPrintHeightInches1000;  // height of the custom defined page size
   int iSpacingInches1000;      // spacing between custom pages    
   CObArray cobOptions;         // device options
   BOOL bPublish;                   // publish the product to WebServices?
   BOOL bUnused;                 // broadcast the product to others?
   short iBroadcastUnitPricePennies;  // the unit price in pennies charged to others
   short iWeightOunces100;            // the weight per product in hundreths of an ounce
   BOOL bInMaster;                    // temp - don't serialize or compare
   int iSortOrder;					  // temp - don't serialize or compare
   int iNumPriceBreaks;
   BOOL bUsePriceBreaks;   
   CString csWholesaler;
   int m_stdsize;
   int m_finish;
   BOOL m_usestdsize;
//   int iFrom[MAXPRODUCTPRICEBREAKS];
//   int iTo[MAXPRODUCTPRICEBREAKS];
//   int iPricePennies[MAXPRODUCTPRICEBREAKS];
   virtual void Serialize(CArchive& ar);
   const CProduct& operator=(const CProduct& cqfSrc);  // assignment
   void CreateProductPricingString(CString& csPricing);
   };

class COrder : public CNode
   {
DECLARE_SERIAL(COrder)
public:
   COrder();
   ~COrder();      
   int iOrderType;   
   int iStatus;
   int iNumJobs;
   int iTotalPhotoSize;
   CString csStatus;
   CString csTime;
   BOOL bTempMark;
   BOOL bInserted;
   BOOL bPrintIndex;
   CString csProductName;
   CString csOnlineAlbum;
   CString csSubmittedTime;
//   COrderDetails OrderDetails;
   void SortOrderByProduct();         
   BOOL ResetStatusFromServer(CString& csNewStatus, CString& csNewDetails);
   void SetStatusFromStr(CString& csStatusStr);
   int GetTotalCopies();   
   int GetTotal(CNode* pParentProduct, int* piSubTotal=NULL, int* piShipCost=NULL, int* piTotalTax=NULL, int* piPromo=NULL);
   int GetTotalONLINE(CNode* pParentProduct, int* piSubTotal=NULL, int* piShipCost=NULL, int* piTotalTax=NULL, int* piPromo=NULL);   
   void GenerateProductCountArray(CNode* pParentProduct);   
   void GenerateProductCountArrayONLINE(CNode* pParentProduct);
   CProduct* GetProduct(CPhoto* pPhoto, CNode* pParentProduct);
   CProduct* GetProduct(LPCSTR lpcstrName, CNode* pParentProduct);
   CProduct* GetProduct(CNode* pParentProduct);
   CProduct* GetProduct(int idx, CNode* pParentProduct);
   void GetStatus(CString& csStatusStr);
   virtual void Serialize(CArchive& ar);
   const COrder& operator=(const COrder& cqfSrc);  // assignment
   void GetProductCounts(CNode* pParentProduct);
   int GetNumDevicesInOrder(CNode* pParentProduct);
   void ResetProductCountArray();
   CObArray coProductArray;   
   void DeletePhotos();
   CStringArray csaEmailTo;	    // for xconnect
   CString csEmailFrom;	        // for xconnect
   int iLastSortColumn;  // to save sorts
   int bLastSortOrder;	 // to save sorts
   CTime ctTime;   
//   void UpdateStatus(CPhotogizeProxyClient* ppgClient=NULL);   
   BOOL GetBarCodeFileName(CString& csFileName);
protected:
   BOOL MakeBarCode();
   CString csBarCodeFileName; // gets dumped on exit   
   BOOL DoSort();
   };

class CImageCacheElement : public CObject
   {
DECLARE_SERIAL(CImageCacheElement)
public:
   CImageCacheElement();
   ~CImageCacheElement();
   CString csFileName;
   CTime ctCreated;
   CTime ctLastAccess;
   CImage ci;
   int iTop;
   int iLeft;
   CString Original_csFileName;
   CTime Original_ctFileTime;
   long Original_FileSize;
   virtual void Serialize(CArchive& ar);
   };

class CImageCacheBase : public CObject
   {
public:
   CImageCacheBase();
   ~CImageCacheBase();
   void Write();
   void SetCachePercent(int iP);
   BOOL CanWeAddElement(UINT uiSize);
   CObArray coElements;   
   CString csFolder;   
   CRITICAL_SECTION CriticalSection; 
protected:
   CString csFileName;   
   int iPercent;   
   };

class CImageCache : public CImageCacheBase
   {
public:   
   CImageCache();
   ~CImageCache();
   void Init(LPCSTR csCacheFileName, LPCSTR csCacheFolder);      
   void Clear();   
   void Clear(CNode* pNode);   
protected:   
   void DeleteElements(BOOL bDeleteDiskFile=FALSE);
   };

class COnlineImageCacheElement : public CObject
   {
DECLARE_SERIAL(COnlineImageCacheElement)
public:
   COnlineImageCacheElement();
   ~COnlineImageCacheElement();
   CString csFileName;
   CString csUser;
   CString csAlbum;
   CString csPhoto;
   int iSize;
   int iDateTimeSecs;   
   CTime ctInstantiated;
   virtual void Serialize(CArchive& ar);
   };

class COnlineImageCache : public CImageCacheBase
   {
public:   
   COnlineImageCache();
   ~COnlineImageCache();   
   void Init(LPCSTR csCacheFileName, LPCSTR csCacheFolder);   
   void Clear();   
   BOOL CheckCache(CString& csUser, CString& csAlbum, CString& csPhoto, 
	  int iSize, int iDateTimeSecs, CString& csFileName);
   void AddElement(CString& csUser, CString& csAlbum, CString& csPhoto, 
	  int iSize, int iDateTimeSecs, CString& csFileName);
   void Cull(int iDays);
protected:
   void DeleteElements(BOOL bDeleteDiskFile=FALSE);
   };

class CICCCacheElement : public CObject
   {
DECLARE_SERIAL(CICCCacheElement)
public:
   CICCCacheElement();
   ~CICCCacheElement();

   CEmbeddedICCInfo EmbeddedICCInfo;
   CString csFileName;   
   };

class CICCCache : public CImageCacheBase
   {
public:   
   CICCCache();
   ~CICCCache();
   void Init(LPCSTR csCacheFileName, LPCSTR csCacheFolder);      
   void Clear();      
protected:   
   void DeleteElements(BOOL bDeleteDiskFile=FALSE);
   };

class CEmailTriggerRule : public CObject
   {
DECLARE_SERIAL(CEmailTriggerRule)
public:
   CEmailTriggerRule();
   ~CEmailTriggerRule();
   BOOL bEnable;
   int iRule;
   CString csSendTo;   
   CString csFrom;   
   CString csCc;   
   CString csSubject;
   CString csMsg;
   BOOL bConfirm;
   virtual void Serialize(CArchive& ar);
   const CEmailTriggerRule& operator=(const CEmailTriggerRule& cpSrc);
   };

class CPhotogizeUtils : public CObject
   {
public:
   CPhotogizeUtils();
   ~CPhotogizeUtils();   
   BOOL IsValidEmail(LPCSTR lpcstrEmailStr);
   BOOL IsValidEmail(CStringArray& csaEmail);
   BOOL IsInArray(LPCSTR lpcstrTest, CStringArray& csa);
   static float GetFreeDiskSpace(LPCSTR lpcstrPath);
   void GetUniqueName(CString& csUnique);   
   void Decrypt(LPCSTR lpcstrIn, CString& csOut);
   void Encrypt(LPCSTR lpcstrIn, CString& csOut);
   void GetDollarString(CString& cs, int iPricePennies, BOOL bDoNeg=FALSE);
   void GetPricePennies(CString& cs, int& iPricePennies);
   BOOL FolderSelect(HWND hOwner, LPCSTR lpcstrPrompt, CString& csFolder);
   BOOL FolderSelect(HWND hOwner, int iPrompt, CString& csFolder);
   void GetLegacyShipList(CObArray& coShipList);
   void WriteShipList(CObArray& coShipList);
   void ClearShipList(CObArray& coShipList);   
   void AppendXMLKeyStringToken(LPCSTR lpcstrKey, LPCSTR lpcstrToken,
	  CString& csPost);
   void AppendXMLKeyIntToken(LPCSTR lpcstrKey, int iToken,
	  CString& csPost); 
   void WriteEmailTriggers(CEmailTriggerRule EmailTriggerList[]);
   void GetEmailTriggers(CEmailTriggerRule EmailTriggerList[]);   
   void RecursiveDelete(LPCSTR lpcstrPath);
   void CopyStringArrays(CStringArray& csaIn, CStringArray& csaOut);
   static void MakeTempFileName(CString& csTempFileName, LPCSTR lpcstrStarter);
   void DoHousekeeping(COnlineImageCache& coic, CImageCache& cic, 
	  CObArray& coAccounts, CString& csFolder, LPCSTR lpcstrTemplate);
   BOOL IsInOnlineCache(CString& csFileName, COnlineImageCache& coic);
   BOOL IsInImageCache(CString& csFileName, CImageCache& cic);
   BOOL IsInAccounts(CString& csFileName, CObArray& coAccounts);
   BOOL IsSupportedPhoto(CString& csFileName);
   void ClearHarvestRuleList(CObArray& coHarvestRules);
   void ClearHarvestRuleList(CObArray* pcoHarvestRules);
   void CopyHarvestRules(const CObArray& coSrc, CObArray& coDst);   
   int TimeStringToTimeInt(CString& csTime);
   BOOL GetOurFileStatus(CString& csFileName, CFileStatus& cfs);
   void CorrectBitmap(BITMAPHANDLE* pBitmapCorrected, CImageAdjustment& cImageAdjust, 
	  BOOL bIgnoreRotate=FALSE);         
   void MakeUploadString(CString& m_url, CString& m_user, CString& csUpload);
   static BOOL CopyFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName,
	  BOOL bFailIfExists, BOOL bShowPrompt=TRUE);
   static void LaunchURLInNewWindow(HWND hWndParent, LPCSTR lpcstrWebUpload);
   static void GetAppPath(CString& csAppPath);      
   static BOOL GetDefaultDisplayProfile(CString& csFileName);
   void DoRedEyeAdjustment(BITMAPHANDLE* pBitmapCorrected, 
      CImageAdjustment& cImageAdjust);
   static int GetFreePhysicalMemory();
   static int GetInstalledMemory();
   static int GetFreeVirtualMemory();
   static int GetPercentMemoryLoad();
   void CopyOrderArchiveToTemp(CString& csArchiveFile);
   BOOL CheckArchivesForDupeOrders(CString& csOrderID);
   void DeleteOrderArray(CObArray& coNodesTemp);
//   static void ICCColorAdjust(CTransformCMM* pTransformCMM, BITMAPHANDLE* pBitmap);
   static void ForceJPGExtension(CString& csOriginalFileName);

protected:
   CString csPassword;
   SECCryptoBlackBox m_bb;
   void Crypto(LPCSTR lpcstrIn, CString& csOut, BOOL bEncrypt);
   int HexLookup(char c);
   CString ReplacePreDefinedChar(LPCSTR lpcstrStr);
   void ReplacePreDefinedChar(LPCSTR lpcstrStr, CString &outStr);
   BOOL IsPreDefinedChar(char inChar);
   BOOL HavePreDefinedChar(LPCSTR lpcstrStr);
   void PreDefinedCharMap(char inChar, CString &mappedCharStr);
   };

#define MAXSHIPPRICEMATRIX  25

class CShipListEntry : public CObject
   {
DECLARE_SERIAL(CShipListEntry)
public:
   CShipListEntry();
   ~CShipListEntry();
   CString csName;
   int iNumBreaks;
   int iBaseHandlingPricePennies;
   int iQtyFrom[MAXSHIPPRICEMATRIX];
   int iQtyTo[MAXSHIPPRICEMATRIX];
   int iPricePennies[MAXSHIPPRICEMATRIX];
   virtual void Serialize(CArchive& ar);
   BOOL operator==(const CShipListEntry& sle);              // compare      
   BOOL operator!=(const CShipListEntry& sle);              // compare      
   const CShipListEntry& operator=(const CShipListEntry& sleSrc);  // assignment
   BOOL IsSame(const CShipListEntry& sle);
   }; 

class CProductCount : public CObject
   {
public:
   CProductCount();
   ~CProductCount();
   CProduct* pProduct;
   CString csName;
   CString csDescription;
   int iCopies;
   int iUnitPricePennies;
   int iSubTotalPennies;
   int iWeightOz100;
   };

class CAccountHarvestRule : public CObject
   {
DECLARE_SERIAL(CAccountHarvestRule)
public:
   CAccountHarvestRule();
   ~CAccountHarvestRule();
   int iType;
   CString csName;
   BOOL bEnable;
   virtual void Serialize(CArchive& ar);
   BOOL operator==(const CAccountHarvestRule& ahr);              // compare      
   BOOL operator!=(const CAccountHarvestRule& ahr);              // compare      
   const CAccountHarvestRule& operator=(const CAccountHarvestRule& ahrSrc);  // assignment
   BOOL IsSame(const CAccountHarvestRule& ahr);
   };			

class CCheckListItem : public CObject
   {
DECLARE_SERIAL(CCheckListItem)
public:
   CCheckListItem(){iCheck=0;}
   ~CCheckListItem(){}
   CString csStr;
   int iCheck;
   const CCheckListItem& operator=(const CCheckListItem& cpSrc);  // assignment
   virtual void Serialize(CArchive& ar);
   };		  

class CMasterAccountHarvestRuleItem : public CObject
   {
DECLARE_SERIAL(CMasterAccountHarvestRuleItem)
public:
   CMasterAccountHarvestRuleItem();
   ~CMasterAccountHarvestRuleItem();
   CString csStoreID;
   CObArray coShip;
   CObArray coPickup;
   CObArray coProducts;
   virtual void Serialize(CArchive& ar);
   BOOL operator==(const CMasterAccountHarvestRuleItem& ahr);              // compare      
   BOOL operator!=(const CMasterAccountHarvestRuleItem& ahr);              // compare      
   const CMasterAccountHarvestRuleItem& operator=(const CMasterAccountHarvestRuleItem& ahrSrc);  // assignment
   BOOL IsSame(const CMasterAccountHarvestRuleItem& ahr);
   };			

class CAccount : public CNode
   {
DECLARE_SERIAL(CAccount)
public:
   CAccount();
   ~CAccount();
   void CopyProducts(const CAccount& AccountSrc);
   void CopyBizRules(const CAccount& AccountSrc);
   void CopyHarvestRules(const CAccount& AccountSrc);
   void CopyProfile(const CAccount& AccountSrc);
   void CopyEmailTriggers(const CAccount& AccountSrc);   
   void CopyLogos(const CAccount& AccountSrc);
   void CopyOrderSettings(const CAccount& AccountSrc);
   void CopyCDSettings(const CAccount& AccountSrc);
   void CopyPromotions(const CAccount& AccountSrc);
   void CopyPromoText(const CAccount& AccountSrc);
   void SetNextUpdate();
   void GetAccountTypeStr(CString& csAccountType);
   BOOL HarvestThisOrder(COrder* pOrder);
   BOOL HarvestThisJob(CString& csProductCompare, CString& csCDProductName);
   void MakeUploadString(CString& csUpload);
   BOOL IsProfileFilledOut();
   BOOL PublishXConnectData();   
   void CopyMasterHarvestRules(const CAccount& AccountSrc);

   BOOL bIsDefault;

   // account connection data
   CString	m_user;
   CString	m_url;
   CString	m_folder;
   CString	m_group;
   CString	m_host;
   CString	m_password;
   BOOL m_proxy_use;
   CString m_proxy_address;
   UINT m_proxy_port;
   CString m_proxy_name;
   CString m_proxy_password;
   UINT m_port;
   UINT m_sslport;
   CTime m_change_time;
   int iNextUpdate;
   int m_makechanges;   
   int QueuedChanges; 
   CString csChannel;
   
   BOOL m_usewebservices;

   // account order data
   int m_autoharvest;
   int m_autoprint;
   int m_moveorders;
   int m_orderdataonly;
   int m_cconinvoice;
   int m_completeonarchive;
   int m_timepoll;
   int m_timeprint;
   int m_timepollunits;
   BOOL m_autoprintinvoice;
   int m_timeprintunits;
   int m_printrule;
   int m_restrictcopies;
   int m_restrictcopies_num;
   int m_restrictvalue;	
   COleCurrency m_restrictvalue_num;
   CString m_indexproduct_name;

   // logo data
   CString m_album_logo;
   CString m_album_logo_link;
   CString m_weblogoleft;
   CString m_weblogoleft_link;
   CString m_weblogoright;
   CString m_weblogoright_link;
   CString m_leftindex_logo;
   CString m_rightindex_logo;
   CString m_invoice_logo;

   // email include/exclude lists
   CStringArray csaInclude;
   CStringArray csaExclude;

   // order stuff
   CStringArray csaPickupLocations;
   CStringArray csaTaxStates;
   CObArray coShipList;
   int m_addtax;
   float m_taxpercent;
   int m_addtaxship;

   // cc rules
   int m_use_visa;
   int m_use_mc;
   int m_use_amex;
   int m_use_dclub;
   int m_use_disco;
   int m_ccrule;
   int m_securitycode;

   // email triggers
   CEmailTriggerRule EmailTriggerList[EMAILTRIGGER_NUMTRIGGERS];
																
   // temp pointer stuff
   HTREEITEM hItemOrderRoot;
   HTREEITEM hItemOrderArchiveRoot;
   HTREEITEM hItemProductRoot;		
   CNode* pParentOrder;
   CNode* pParentOrderArchive;
   CNode* pParentProduct;

   int iLastHarvestTime;
   int iLastXHarvestTime;
   int iLastPrintTime;

   BOOL bIsExpanded_Orders;
   BOOL bIsExpanded_ArchivedOrders;
   BOOL bIsExpanded_Products;

   BOOL bEnableXConnect;
   CString csXConnectPath;
   BOOL bDelXConnectPhotos;
   int XConnect_Interval;
   int XConnect_Units;
   
   CString XConnect_Message;
   CString XConnect_Subject;

   BOOL m_useharvestrules;
   CObArray coHarvestRules;   

   BOOL bCDoffer;
   COleCurrency CDPrice;
   int iCDWeightOunces100;
   CString csCDDescription;
   int CDOptions;

   BOOL CD_m_copyfolder;
   CString CD_m_copyfoldername;
   CString CD_m_copyfolderdest;
   BOOL CD_m_originals;
   BOOL CD_m_samples;
   BOOL CD_m_sampledims;
   CString CD_m_samplefolder;
   BOOL CD_m_index;
   BOOL CD_m_autostart;
   BOOL CD_m_prejudge;
   BOOL m_unused;
   BOOL m_lockacctsettings;
   CObArray coPromotions;

   // profile stuff
   CString m_profile_biz_name;	  
   CString m_profile_tech_name;
   CString m_profile_biz_email;	  
   CString m_profile_tech_email;
   CString m_profile_biz_phone;
   CString m_profile_tech_phone;
   CString m_profile_company;
   CString m_profile_address;
   CString m_profile_comments;
   CString m_profile_fax;
   CString m_profile_website;

   CString m_profile_lab_phone;
   CString m_profile_lab_email;
   CString m_profile_lab_name;

   // promo stuff
   int m_promo_textsize;
   int m_promo_textcolor;
   CString m_promo_msg;
   float m_promo_left;
   float m_promo_right;
   float m_promo_top;
   float m_promo_bottom;
   CString m_promo_typeface_str;
   CString m_promo_template;   
   int m_promo_product;
   int m_promo_addtoorder;
   BOOL	m_promo_justifytxt;
   int m_promo_promocode;
   BOOL m_printindexbydefault;

   CString csStoreID;
   
   CString m_outputfolder;
   CObArray coMasterAccountHarvestRuleSet;

   CTime ctUnused;
   unsigned int uiLastAcctModTime;  // last webservices mod time
   unsigned int uiLastAcctModTimeProps; // last webservices mod time - for servant props checking

   // new order archive stuff in 3.0
   CString m_archivefolder;
   int m_orderarchivehousekeeping_days;
   int m_orderarchivehousekeeping_method;
   BOOL	m_orderarchivehousekeeping;   

   CTime InvoiceBitmapRenderTime;
   CString InvoiceBitmapRenderName;
   LONG InvoiceBitmapRenderSize;
   int InvoiceBitmapRenderHeight;
   int InvoiceBitmapRenderWidth;

   virtual void Serialize(CArchive& ar);
   const CAccount& operator=(const CAccount& cqfSrc);  // assignment
   };
#endif
