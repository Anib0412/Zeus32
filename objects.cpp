#include "stdafx.h"
#include "atlbase.h"
#include "io.h"
#include "objects.h"		
//#include "OurPGClient.h"		
//#include "icc.h"		
//#include "xharvest.h"
//#include "..\..\source\barcode\tecbarcode.h"

// be careful not to add VERSIONABLE_SCHEMA to a nested object - we can
// have only one "version" per archive.  If we need to change a nested class,
// change the version of the parent, modify the serialize method in the child 
// to accept the parent's version and process the new version in the child
// new: UNLESS WE NEST with Serialize
IMPLEMENT_SERIAL(CNode, CObject, 0)
IMPLEMENT_SERIAL(CImageAdjustment, CObject, 0)
IMPLEMENT_SERIAL(CPhoto, CObject, VERSIONABLE_SCHEMA|5)
IMPLEMENT_SERIAL(CImage, CObject, 0)
IMPLEMENT_SERIAL(CProduct, CObject, VERSIONABLE_SCHEMA|7) 
IMPLEMENT_SERIAL(CWinBmp, CObject, 0)
IMPLEMENT_SERIAL(COrder, CObject, VERSIONABLE_SCHEMA|6)
IMPLEMENT_SERIAL(CDevOption, CObject, 0)
IMPLEMENT_SERIAL(CImageCacheElement, CObject, 0)
IMPLEMENT_SERIAL(COnlineImageCacheElement, CObject, VERSIONABLE_SCHEMA|1)
IMPLEMENT_SERIAL(CShipListEntry, CObject, 0)
IMPLEMENT_SERIAL(CEmailTriggerRule, CObject, 0)
IMPLEMENT_SERIAL(CAccount, CObject, VERSIONABLE_SCHEMA|24)
IMPLEMENT_SERIAL(CAccountHarvestRule, CObject, 0)
IMPLEMENT_SERIAL(CMasterAccountHarvestRuleItem, CObject, 0)
IMPLEMENT_SERIAL(CEmbeddedICCInfo, CObject, 0)
IMPLEMENT_SERIAL(CICCCacheElement, CObject, 0)
IMPLEMENT_SERIAL(CRedEyeAdjust, CObject, 0)
IMPLEMENT_SERIAL(CPromo, CObject, 0)
IMPLEMENT_SERIAL(CCheckListItem, CObject, 0)

extern "C" L_UINT FAR EXPORT OurRead(L_INT, L_CHAR*, L_INT, L_VOID*);
extern "C" L_INT32 FAR EXPORT OurSeek(L_INT, L_INT32, L_INT, L_VOID*);

#define PADWIDTH(x)	(((x)*8 + 31)  & (~31))/8

#define USECACHE 1

CNode::CNode()
   {   
   // default
   iType = NODETYPE_UNDEFINED;
   bIsExpanded = FALSE;
   bPopulated = FALSE;
   iSubType = SUBTYPE_UNKNOWN;
   bSelected = FALSE;
   bToDelete = FALSE;
   bMakeBold = FALSE;
   pAccountOwner = NULL;
   }

CNode::~CNode()
   { 
   int i;
   CNode* pNode;

   // delete the members array
   i = 0;
   while (i < coNodes.GetSize() )
      {
	  if (pNode = (CNode*)coNodes.GetAt(i++))		 		 
		 {							  
		 if (pNode) delete pNode;			
		 }
      }     
   coNodes.RemoveAll();
   }

void CNode::Serialize(CArchive& ar)
   {           
   try
	  {
	  if (ar.IsStoring())
		 {
		 ar << iType << iSubType << csUser << csDisplayName << csName << csNotes << 
			bIsExpanded << bPopulated << bMakeBold;
		 }
	  else
		 {
		 ar >> iType >> iSubType >> csUser >> csDisplayName >> csName >> csNotes >> 
			bIsExpanded >> bPopulated >> bMakeBold;
		 }
	  coNodes.Serialize(ar);
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   }

const CNode& CNode::operator=(const CNode& cpSrc) 
   {
   // assignment
   if (this != &cpSrc)
      {   
	  // copy the props
	  iType = cpSrc.iType;	  
	  iSubType = cpSrc.iSubType;
	  csUser = cpSrc.csUser;
	  csDisplayName = cpSrc.csDisplayName;	  
	  csName = cpSrc.csName;	  
	  csNotes = cpSrc.csNotes;
	  bIsExpanded = cpSrc.bIsExpanded;
	  bPopulated = cpSrc.bPopulated;
	  bMakeBold = cpSrc.bMakeBold;
	  pAccountOwner = cpSrc.pAccountOwner;
	  // note that we don't copy the nodes here,
	  // we let the derived classes do that
	  }
   return *this;   
   }			   

int CNode::GetIndex(CNode* pParentNode)
   {
   int i;
   int idx = -1;
   CNode* pTestNode;

   if (!pParentNode) return idx;

   for (i = 0; i < pParentNode->coNodes.GetSize(); i++)
	  {
	  if (pTestNode = (CNode*)pParentNode->coNodes.GetAt(i))
		 {
		 if (pTestNode == this)
			{
			idx = i;
			break;
			}
		 }
	  }
   return idx;
   }

CPhoto::CPhoto()
   {         
   iImageIndex = 0;
   iCopies = 1;
   bCancelLoad = FALSE;
   iType = NODETYPE_PHOTO;   
   bOverrideSizing = FALSE;
   iSizing = IMAGESIZING_CROP;
   bIsLocal = TRUE;
   pCache = NULL;
   pICCCache = NULL;
   bDeleteAfterPrinting = FALSE;
   bIsPromoPhoto = FALSE;
   bCanUndoImageAdjust = FALSE;
   bCanRedoImageAdjust = FALSE;
   // default owner is the name of this computer
   DWORD dwSize = _MAX_PATH;
   GetComputerName(csOwner.GetBuffer(_MAX_PATH), &dwSize);
   csOwner.ReleaseBuffer();
   crCrop.SetRectEmpty();      
   EmbeddedICCInfo.Reset();
   }

CPhoto::~CPhoto()
   {
   int i = 0;
   CImage* pImage;
   CWinBmp* pWinBmp;
 
   // remove the images
   i = 0;			  
   while (i < coImages.GetSize() )
	  {
	  pImage = (CImage*)(coImages.GetAt(i++));
	  if (pImage) delete pImage;
	  }
   coImages.RemoveAll();

   // remove the bmps
   i = 0;			  
   while (i < coWinBmp.GetSize() )
	  {
	  pWinBmp = (CWinBmp*)(coWinBmp.GetAt(i++));
	  if (pWinBmp) delete pWinBmp;
	  }
   coWinBmp.RemoveAll();
   }

void CPhoto::Serialize(CArchive& ar)
   {     
   try
	  {
	  if (ar.IsStoring())
		 {
		 ar << csOwner << bCancelLoad << iImageIndex << iCopies << 
			csProductName << bOverrideSizing << iSizing << bIsLocal;
		 cImageAdjust.Serialize(ar);
		 ar << crCrop;
		 EmbeddedICCInfo.Serialize(ar);
		 }
	  else
		 {
		 int nVersion = ar.GetObjectSchema();

		 ar >> csOwner >> bCancelLoad >> iImageIndex >> iCopies >> 
			csProductName >> bOverrideSizing >> iSizing >> bIsLocal;

		 if (nVersion >= 1) 		 
			cImageAdjust.Serialize(ar, nVersion);
		 if (nVersion >= 3) 		 
			{
			ar >> crCrop;		 
			}
		 if (nVersion >= 4) 		 
			{
			EmbeddedICCInfo.Serialize(ar);
			}
		 }
   
	  // serialize the image array, note that the image data
	  // does not get serialized, it will need to be reloaded
	  coImages.Serialize(ar);

	  // call the base class   
	  CNode::Serialize(ar);
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   }

const CPhoto& CPhoto::operator=(const CPhoto& cpSrc) 
   {
   int i;
   CImage* pSrcImage;
   CImage* pDstImage;

   // assignment	  
   if (this != &cpSrc)
      {   
	  // copy the props
	  csOwner = cpSrc.csOwner;	  
	  bCancelLoad = cpSrc.bCancelLoad;	  	  
	  iImageIndex = cpSrc.iImageIndex;
	  bOverrideSizing = cpSrc.bOverrideSizing;
	  pCache = cpSrc.pCache;
	  pICCCache = cpSrc.pICCCache;
	  bIsLocal = cpSrc.bIsLocal;
	  iSizing = cpSrc.iSizing;
	  iCopies = cpSrc.iCopies;
	  csProductName = cpSrc.csProductName;
	  cImageAdjust = cpSrc.cImageAdjust;
	  crCrop = cpSrc.crCrop;	  
	  // copy the images
	  for (i = 0; i < cpSrc.coImages.GetSize(); i++)
		 {
		 if (pSrcImage = (CImage*)cpSrc.coImages.GetAt(i))
			{
			if (pDstImage = new CImage)
			   {
			   *pDstImage = *pSrcImage;
			   }
			coImages.Add((CObject*)pDstImage);
			}
		 }		 	  
	  // call the base class
	  CNode::operator =(cpSrc);
	  }
   return *this;   
   }

int CPhoto::GetSizingToUse(CProduct* pProduct)
   {
   if (!pProduct) return iSizing;	  
   
   if (bOverrideSizing) 
	  return iSizing;
   else
      return pProduct->iSizing;
   }
   
BOOL CPhoto::CopyNamedFile(CString& csDstFolder, BOOL bRename)
   {
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   CFileStatus cfs;   
   CString csBaseName;
   int i = 1;
   CString csDstFileBaseName;   
   CString csSrcFileName;   
   CString csDstFileName;      
   CImage* pImage;   

   if (!(pImage = GetOriginalImage()))
	  return FALSE;
   
   VERIFYPATH(csDstFolder);
   csDstFileName = csDstFolder;
   csDstFileName += csDisplayName;	  
   csSrcFileName = pImage->csFileName;

   // get the base filename
   _splitpath(csDstFileName, drive, dir, fname, ext);
   csBaseName = fname;   

   do
	  {
	  // split it
	  csDstFileName = drive;
	  csDstFileName += dir;
	  csDstFileName += csBaseName;
	  csDstFileName += ext;
	  csDstFileBaseName = csBaseName;
	  csDstFileBaseName += ext;
	  // create a "copy" name
	  csBaseName.Format("%s (Copy %d)", fname, i++);	  	  
	  // do this until we can't find it
	  } while (CFile::GetStatus(csDstFileName, cfs));
   if (CPhotogizeUtils::CopyFile(csSrcFileName, csDstFileName, FALSE))
	  {
	  pImage->csFileName = csDstFileName;
	  if (bRename) csDisplayName = csDstFileBaseName;
	  return TRUE;
	  }		
   else
	  return FALSE;
   }

void CPhoto::CreateNewTempFile()
   {
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   CString csOriginalFolder;
   CString csNewFileName;
   CImage* pImage;

   // get the original image
   if (!(pImage = GetOriginalImage()))
	  return;				
   
   // split the original filename
   _splitpath(pImage->csFileName, drive, dir, fname, ext);

   // get the original folder
   csOriginalFolder = drive;
   csOriginalFolder += dir;
   VERIFYPATH(csOriginalFolder);

   // make a new temp filename   
   GetTempFileName(csOriginalFolder, "lab", 0, 
	  csNewFileName.GetBuffer(_MAX_PATH));
   csNewFileName.ReleaseBuffer();

   // copy it
   if (CPhotogizeUtils::CopyFile(pImage->csFileName, csNewFileName, FALSE))
	  {
	  pImage->csFileName = csNewFileName;
	  }
   }

CImage* CPhoto::GetOriginalImage()
   {
   int i;
   CImage* pImageRet = NULL;
   CImage* pImage;
 
   for (i = 0; i < coImages.GetSize(); i++)
	  {
	  pImage = (CImage*)(coImages.GetAt(i));
	  if (pImage)
		 {
		 if (pImage->iStyle == IMAGESTYLE_ORIGINAL)
			{
			pImageRet = pImage;
			break;
			}
		 }
	  }
   return pImageRet;
   }		 

BOOL CPhoto::HasWinBmp(int iSize)
   {
   int i;
   CWinBmp* pWinBmp;

   // does it already exist? and are the adjustments equal?
   if (!cImageAdjust.IsAdjusted())
	  {
	  for(i = 0; i < coWinBmp.GetSize(); i++)
		 {
		 if (pWinBmp = (CWinBmp*)coWinBmp.GetAt(i))
			{
			if (pWinBmp->iSize == iSize)
			   {
			   if (cImageAdjust == pWinBmp->cImageAdjustActive)
				  return TRUE;
			   }
			}
		 }
	  }
   return FALSE;
   }

HBITMAP CPhoto::GetWinBmp(int iSize)
   {
//   CImage* pImage;
//   int iTop;
//   int iLeft;   
//   int iTemp;
//   BITMAPHANDLE TmpBitmap;   
//   HDC hDC;
//   int i;
//   CWinBmp* pWinBmp;
   HBITMAP hBitmap = NULL;
//   BITMAPHANDLE* pBitmap;
/*   
   // does it already exist? and are the adjustments equal?
   for(i = coWinBmp.GetSize() - 1; i >= 0; i--)
	  {
	  if (pWinBmp = (CWinBmp*)coWinBmp.GetAt(i))
		 {	
		 if (pWinBmp->iSize == iSize)					 
			{
			if (cImageAdjust == pWinBmp->cImageAdjustActive)			
			   return pWinBmp->hBitmap;						
			else
			   {
			   // we have the right size but the wrong corrections
			   // so let's delete this image and recreate
			   //coWinBmp.RemoveAt(i);
			   //delete pWinBmp;		
			   //break;
			   }
			}
		 }
	  }	  

   // create a new image
   if (pImage = CreateImage(iSize, iSize, iTop, iLeft, IMAGESIZING_FIT))
	  {
	  // swap left and top if rotated
	  if (cImageAdjust.iRotate == 90 || cImageAdjust.iRotate == 270)		 
		 {
		 iTemp = iTop;
		 iTop = iLeft;
		 iLeft = iTemp;
		 }

	  // correct it if necessary
      pBitmap = pImage->GetCorrectedBitmapHandle(cImageAdjust, NULL);	  

      // create a display DC
	  hDC = ::CreateDC("DISPLAY", NULL, NULL, NULL);

	  // now create a new bitmap
	  L_INT lRet;
      if ((lRet = ::L_CreateBitmap(&TmpBitmap, TYPE_CONV, iSize, iSize, 24, 
		 ORDER_BGR, NULL, TOP_LEFT, NULL, 0)) == SUCCESS)
		 {
		 // fill it with white
		 if (::L_FillBitmap(&TmpBitmap, RGB(255, 255, 255)) == SUCCESS)		 
			{
			// and copy the old one into the new one
			if ((lRet = ::L_CombineBitmap(&TmpBitmap, iLeft, iTop, pBitmap->Width, 
			   pBitmap->Height, pBitmap, 0, 0, L_SRC_COPY)) == SUCCESS)
			   {
			   // and convert to a DDB
			   hBitmap = (HBITMAP)::L_ConvertToDDB(hDC, &TmpBitmap);			  

			   if (hBitmap)
				  {
				  if (pWinBmp = new CWinBmp)
					 {
					 pWinBmp->hBitmap = hBitmap;
					 pWinBmp->iSize = iSize;
					 pWinBmp->cImageAdjustActive = cImageAdjust;
					 // add it to our array
					 coWinBmp.Add((CObject*)pWinBmp);
					 }	
				  else
					 {
					 ::DeleteObject(hBitmap);			   
					 hBitmap = NULL;
					 }	 
				  }
			   else
				  {
				  ::DeleteObject(hBitmap);			   
				  hBitmap = NULL;
				  }
			   }
			}
		 // release the temp bitmap
		 ::L_FreeBitmap(&TmpBitmap);
		 }									   
	  ::DeleteDC(hDC);
	  }
   // return a pointer to the HBITMAP
*/
   return hBitmap;
   }

void CPhoto::ClearDiskCache(CString& csFileNameCompare, CTime& ctFileTimeCompare, 
   long FileSizeCompare)
   {
   int i;
   CImageCacheElement* pElement = NULL;      

   if (!pCache) return;

#ifndef USECACHE
   return NULL;
#endif

   // Initialize the critical section.
   // should put these in try/finally code, but the code
   // has objects that must unwind, so we will risk an exception
   // in getting a critical section for code simplicity
   // we might want to recode this at a later date
   EnterCriticalSection(&pCache->CriticalSection); 	  

   try 
	  {
	  // go through the cache and delete all files based on this
	  // image
	  for (i = pCache->coElements.GetSize() - 1; i >= 0; i--)
		 {		 
		 if (pElement = (CImageCacheElement*)pCache->coElements.GetAt(i))
			{
			// a match?, then delete it
			if (pElement->Original_csFileName == csFileNameCompare &&
			    pElement->Original_ctFileTime == ctFileTimeCompare &&
			    pElement->Original_FileSize == FileSizeCompare)
			   {			   
			   ::DeleteFile(pElement->csFileName);
			   delete pElement;
			   pCache->coElements.RemoveAt(i);
			   continue;			
			   }
			}		
		 }			 
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   
   // Release ownership of the critical section.
   LeaveCriticalSection(&pCache->CriticalSection);      

   return;
   }

void CPhoto::ClearMemoryCache()
   {
   int i;   
   CImage* pImage;  
   CWinBmp* pWinBmp;

   try
	  {
	  // clear all the attached images in memory
	  for (i = coImages.GetSize() - 1; i >= 0; i--)
		 {
		 if (pImage = (CImage*)coImages.GetAt(i))
	  		{
			if (pImage->iStyle == IMAGESTYLE_SPAWN)			
			   {
			   delete pImage;
			   coImages.RemoveAt(i);
			   }
			else
			   {
			   // if original image, dump the in-memory bitmap
			   if (pImage->Bitmap.Flags.Allocated) 
				  ::L_FreeBitmap(&pImage->Bitmap);
			   if (pImage->BitmapCorrected.Flags.Allocated) 
				  ::L_FreeBitmap(&pImage->BitmapCorrected);   
			   if (pImage->BitmapGray.Flags.Allocated) 
				  ::L_FreeBitmap(&pImage->BitmapGray);
			   }
			}
		 }
	  // clear the win bmps
	  for (i = coWinBmp.GetSize() - 1; i >= 0; i--)
		 {
		 if (pWinBmp = (CWinBmp*)coWinBmp.GetAt(i))
			{
			delete pWinBmp;
			coWinBmp.RemoveAt(i);
			}
		 }
	  }
   catch(...)
	  {
	  }
   }
CImage* CPhoto::CreateImage(int iWidth, int iHeight, int& iTop, int& iLeft, int iSizing)
   {
   int i;
   CImage* pImageRet = NULL;
   CImage* pImage;
   CImage* pOriginalImage;
   int iNewWidth, iNewHeight;   

   // could take some time
   AfxGetApp()->DoWaitCursor(1);
 
   // first see if it exists in the image collection
   for (i = coImages.GetSize() - 1; i >= 0; i--)
	  {
	  pImage = (CImage*)(coImages.GetAt(i));
	  if (pImage)
		 {
		 if (pImage->iStyle == IMAGESTYLE_SPAWN && 
			pImage->iWidthSizeTo == iWidth && 
			pImage->iHeightSizeTo == iHeight &&
			pImage->iSizing == iSizing)			
			{
			// if it's allocated, use it, otherwise, toss it
			if (pImage->Bitmap.Flags.Allocated)
			   {
			   pImageRet = pImage;
			   // recalc iTop and iLeft for new winbmp generation
			   CreateFitDims(pImage->Bitmap.Width, pImage->Bitmap.Height, 
				  iWidth, iHeight, iNewWidth, iNewHeight, iTop, iLeft, 
				  iSizing);								 
			   }
			else
			   {
			   delete pImage;
			   coImages.RemoveAt(i);
			   }
			break;
			}
		 }
	  }
   // not there, so check the disk cache
   if (!pImageRet)
	  {
	  pImageRet = CheckCache(iWidth, iHeight, iSizing, iTop, iLeft);
	  }
   // if not, create it
   if (!pImageRet)
	  {
	  // get original image
	  pOriginalImage = GetOriginalImage();
	  if (pOriginalImage)
		 {
		 if (pImage = new CImage)
			{
			// copy it
			*pImage = *pOriginalImage;
			// this is a spawned image
			pImage->iStyle = IMAGESTYLE_SPAWN;
			pImage->iSizing = iSizing;
			pImage->pbCancelLoad = &bCancelLoad;			
			pImage->iWidthSizeTo = iWidth;
			pImage->iHeightSizeTo = iHeight;

			// load new image
			if (pImage->LoadImage())
			   {			   
			   // compute the new sizing
			   CreateFitDims(pImage->Bitmap.Width, pImage->Bitmap.Height, 
				  iWidth, iHeight, iNewWidth, iNewHeight, iTop, iLeft, 
				  iSizing);								 
			   // everything works in 24 bit, so promote it if need be
			   if (pImage->Bitmap.BitsPerPixel != 24)
				  ::L_ColorResBitmap(&pImage->Bitmap, &pImage->Bitmap, sizeof(BITMAPHANDLE), 
										24, CRF_BYTEORDERBGR, NULL, NULL, 0, NULL, NULL);
			   // and resize it
			   ::L_SizeBitmap(&pImage->Bitmap, iNewWidth, iNewHeight, SIZE_BICUBIC);
			   pImage->iWidth = iNewWidth;
			   pImage->iHeight = iNewHeight;
			   
			   // add it to the image list
			   coImages.Add((CObject*)pImage);
			   pImageRet = pImage;
			   // add to cache
			   AddToCache(pImage, iTop, iLeft);
			   }
			else
			   {
			   delete pImage;
			   pImageRet = NULL;
			   }
			}	   				 
		 }
	  }
   AfxGetApp()->DoWaitCursor(-1);

   return pImageRet;
   }

void CPhoto::DeleteDiskFile(BOOL bAlwaysDelete)
   {
   CImage* pImage = GetOriginalImage();

   if (pImage)
	  {
	  if (!bIsLocal || bAlwaysDelete) ::DeleteFile(pImage->csFileName);			 
	  // and delete the cache files if they exist
	  DeleteCacheFiles();
	  }
   }

void CPhoto::CreateFitDims(int iWidth, int iHeight, int iFitWidth, int iFitHeight, 
						   int& iNewWidth, int& iNewHeight, int& iTop, int& iLeft, 
						   int iSizing)
   {
   float fRectAspect;
   float fPhotoAspect;

   // create a "fit" paint rect
   fRectAspect = (float)iFitHeight / (float)iFitWidth;
   fPhotoAspect = (float)iHeight / (float)iWidth;

   if (iSizing == IMAGESIZING_FIT)
	  {
	  if (fPhotoAspect < fRectAspect)					 
		 {
		 iNewWidth = iFitWidth;
		 iNewHeight = (int)((float)(iFitWidth) * fPhotoAspect + .5F);
		 }
	  else
		 {
		 iNewWidth = (int)((float)iFitHeight / fPhotoAspect + .5F);
		 iNewHeight = iFitHeight;					
		 }							
	  iLeft = (iFitWidth - iNewWidth) / 2;
 	  iTop = (iFitHeight - iNewHeight) / 2;
	  }
   else
	  {					 
	  if (fPhotoAspect > fRectAspect)
		 {
		 iNewWidth = iFitWidth;
		 iNewHeight = (int)((float)(iFitWidth) * fPhotoAspect + .5F);
		 }
	  else
		 {
		 iNewWidth = (int)((float)iFitHeight / fPhotoAspect + .5F);
		 iNewHeight = iFitHeight;					
		 }
	  iLeft = 0;
	  iTop = 0;
	  }
   } 

BOOL CPhoto::GetOriginalImageFileName(CString& csFileName)
   {
   BOOL bRet = FALSE;
   CImage* pImage;

   // get the original image
   if (pImage = GetOriginalImage())
	  {
	  csFileName = pImage->csFileName;
	  bRet = TRUE;
	  }
   return bRet;
   }
/*
BOOL CPhoto::ApplyImageAdjustments(CProduct* pProduct, CString& csFileNameIn, 
   CString& csFileNameOut, CTransformCMM* pTransformCMM, BOOL bIgnoreRotate,
   BOOL bApplyCropAndFit)
   {
   CPhotogizeUtils utils;
   BITMAPHANDLE Bitmap;
   BOOL bRet = FALSE;
   CString csTempPath;      
   int iSizing;   
   CRect crFitRect;

   try
	  {	  
	  // load the bitmap
	  if (::L_LoadBitmap((char*)(const char*)csFileNameIn, &Bitmap, 0, ORDER_BGR, 
		 NULL, NULL) == SUCCESS)
		 {
		 // color correct it (don't rotate it here)
		 utils.CorrectBitmap(&Bitmap, cImageAdjust, TRUE);		 
		 
		 // if pTransformCMM, we must apply a transform
		 if (pTransformCMM)
			{			
			CPhotogizeUtils::ICCColorAdjust(pTransformCMM, &Bitmap);			   
			}														   			

		 // get the sizing
		 iSizing = GetSizingToUse(pProduct);

		 // always apply custom sizing if it is set
		 if (iSizing == IMAGESIZING_CUSTOM)
			::L_TrimBitmap(&Bitmap, crCrop.left, crCrop.top, 
			crCrop.Width(), crCrop.Height());
		 // apply fit if the flag is set
		 else if (iSizing == IMAGESIZING_CROP && bApplyCropAndFit)
			::L_TrimBitmap(&Bitmap, crCrop.left, crCrop.top, 
			crCrop.Width(), crCrop.Height());
		 else if (iSizing == IMAGESIZING_FIT && bApplyCropAndFit)
			{
			// load fit rect with size of loaded bitmap			
			FitInMedia(Bitmap.Width, Bitmap.Height, crFitRect, pProduct);			
			CreateFitBitmap(&Bitmap, crFitRect);
			}

		 // rotate it if need be
		 if (!bIgnoreRotate)
			{
			if (cImageAdjust.iRotate != 0)
			   ::L_RotateBitmap(&Bitmap, cImageAdjust.iRotate * 100, ROTATE_RESIZE, 
			   RGB(255,255,255));
			}

		 // and save it
		 if (::L_SaveBitmap((char*)(const char*)csFileNameOut, &Bitmap, 
			FILE_JFIF, 24, 2, NULL) == SUCCESS)
			bRet = TRUE;
		 ::L_FreeBitmap(&Bitmap);
		 }
	  }
   catch(...)
	  {
	  }
   return bRet;
   }
*/
CWinBmp::CWinBmp()
   {
   hBitmap = NULL;
   iSize = 0;   
   }

CWinBmp::~CWinBmp()
   {
   // free the windows bitmap if it has been allocated   
   if (hBitmap) ::DeleteObject(hBitmap);
   }									

const CImage& CImage::operator=(const CImage& cpSrc) 
   {
   // assignment	  
   if (this != &cpSrc)
      {   
	  // copy the props
	  csFileName = cpSrc.csFileName;	  
	  iWidth = cpSrc.iWidth;	  
	  iHeight = cpSrc.iHeight;
	  iWidthSizeTo = cpSrc.iWidthSizeTo;
	  iHeightSizeTo = cpSrc.iHeightSizeTo;
	  iStyle = cpSrc.iStyle;
	  iSizing = cpSrc.iSizing;	  
      ctFileTime = cpSrc.ctFileTime;
	  FileSize = cpSrc.FileSize;
	  MakeCoolFileSizeStr();
	  // call the base class
	  CNode::operator =(cpSrc);
	  }
   return *this;   
   }			   

CImage::CImage()
   {   
   iWidth = 0;
   iHeight = 0;
   iWidthSizeTo = 0;
   iHeightSizeTo = 0;
   iStyle = IMAGESTYLE_UNDEFINED;
   iSizing = IMAGESIZING_CROP;
   iType = NODETYPE_IMAGE;
   ctFileTime = 0;
   FileSize = 0;      
   ZeroMemory(&Bitmap, sizeof BITMAPHANDLE);
   ZeroMemory(&BitmapCorrected, sizeof BITMAPHANDLE);   
   ZeroMemory(&BitmapGray, sizeof BITMAPHANDLE);
   iBitmapGrayRotation = 0;
//   pTransformCMM = NULL;   
   }

CImage::~CImage()
   {
   // free bitmaps if they are loaded
   if (Bitmap.Flags.Allocated) ::L_FreeBitmap(&Bitmap);
   if (BitmapCorrected.Flags.Allocated) ::L_FreeBitmap(&BitmapCorrected);   
   if (BitmapGray.Flags.Allocated) ::L_FreeBitmap(&BitmapGray);
   }

void CImage::Serialize(CArchive& ar)
   {           
   try
	  {
	  if (ar.IsStoring())
		 {
		 ar << csFileName << iWidth << iHeight << iWidthSizeTo << iHeightSizeTo <<
			iStyle << iSizing << ctFileTime << csFileSize << FileSize;		 
		 }
	  else
		 {
		 int nVersion = ar.GetObjectSchema();

		 ar >> csFileName >> iWidth >> iHeight >> iWidthSizeTo >> iHeightSizeTo >>
			iStyle >> iSizing >> ctFileTime >> csFileSize >> FileSize;
		 }
	  // call the base class
	  CNode::Serialize(ar);
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   }

BOOL CImage::LoadImage()
   {
   BOOL bRet = FALSE;

   // already loaded?
   if (Bitmap.Flags.Allocated) return TRUE;   

   // this doesn't seem to work
   //::L_RedirectIO(NULL, OurRead, NULL, OurSeek, NULL, pbCancelLoad);      

   try
	  {
	  if (::L_LoadBitmap((char*)(const char*)csFileName,
							&Bitmap, sizeof(BITMAPHANDLE), 0, ORDER_BGR, NULL, NULL) == SUCCESS) bRet = TRUE;
	  }
   catch(...)
	  {
	  }

   //::L_RedirectIO (NULL, NULL, NULL, NULL, NULL, NULL);

   return bRet;
   }								  

void CImage::MakeCoolFileSizeStr()
   {
   if (FileSize > (1024 * 1024))
	  csFileSize.Format("%dM", FileSize / (1024 * 1024));
   else if (FileSize > (1024))
	  csFileSize.Format("%dK", FileSize / 1024);
   else 
	  csFileSize.Format("%d", FileSize);
   }

CDevOption::CDevOption()
   {
   iSetting = 0;
   }

CDevOption::~CDevOption()
   {
   }

void CDevOption::Serialize(CArchive& ar)
   {
   try
	  {
	  if (ar.IsStoring())
		 {
		 ar << csName << iSetting;
		 }
	  else
		 {
		 ar >> csName >> iSetting;
		 }
	  csaSettings.Serialize(ar);
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   }

const CDevOption& CDevOption::operator=(const CDevOption& cpSrc) 
   {
   int i;
   CString csSrc;
   CString csDst;

   // assignment	  
   if (this != &cpSrc)
      {   
	  // copy the props
	  csName = cpSrc.csName;	  
	  iSetting = cpSrc.iSetting;

	  // first clear the settings array
	  csaSettings.RemoveAll();

	  for (i = 0; i < cpSrc.csaSettings.GetSize(); i++)
		 {
		 if (csSrc = cpSrc.csaSettings.GetAt(i))
			{
			csDst = csSrc;
			csaSettings.Add(csDst);
			}
		 }		 	  

	  }
   return *this;    
   }

CProduct::CProduct()
   {
   iProductType = PRODUCT_TYPE_UNDEFINED;
   iType = NODETYPE_PRODUCT;
   iSubType = SUBTYPE_PRODUCT_RPFULFILL;
   iUnitPricePennies = 0;
   iSizing = IMAGESIZING_CROP;
   bCustomPageSize = FALSE;
   bPublish = TRUE;
   bUnused = FALSE;
   iBroadcastUnitPricePennies = 0;
   iWeightOunces100 = 0;
   IPAddress_a = 0;
   IPAddress_b = 0;
   IPAddress_c = 0;
   IPAddress_d = 0;
   iNumPriceBreaks = 1;
   bUsePriceBreaks = FALSE;
   bApplyICCToPrint = FALSE;
/*
   for (int i = 0; i < MAXPRODUCTPRICEBREAKS; i++)
	  {
      iFrom[i] = 1;
	  iTo[i] = 10;
	  iPricePennies[i] = 100;
	  }
*/
   iDevice = 0;
   iMedia = 0;
   iSizing = IMAGESIZING_CROP;

   bCustomPageSize = FALSE;
   iMediaWidthInches1000 = 0;
   iMediaHeightInches1000 = 0;
   iPrintWidthInches1000 = 0;
   iPrintHeightInches1000 = 0;
   iSpacingInches1000 = 0;

   bPublish = TRUE;
   bUnused = FALSE;
   iBroadcastUnitPricePennies = 0;   
   bInMaster = FALSE;

   bIsLocal = TRUE;

   iBackPrint1 = BACKPRINT_NONE;
   iBackPrint2 = BACKPRINT_NONE;

   m_stdsize = 0;
   m_finish = 0;
   m_usestdsize = TRUE;
   }

CProduct::~CProduct()
   {
   DeleteDevOptionArray();
   }

void CProduct::DeleteDevOptionArray()
   {
   int i = 0;
   CDevOption* pCdo;   

   while (i < cobOptions.GetSize())
      {
	  // get file
      pCdo = (CDevOption*)cobOptions.GetAt(i++);
	  // remove the string array
	  pCdo->csaSettings.RemoveAll();
	  delete pCdo;	  
	  }     
   cobOptions.RemoveAll();
   }

extern "C" L_UINT FAR EXPORT OurRead(L_INT FD, L_CHAR* pBuf, L_INT nCount, 
										  L_VOID* pUserData)
   {
   BOOL* pbCancelLoad = (BOOL*)pUserData;
   if (*pbCancelLoad)
	  {
//	  TRACE("load cancelled, feeding HFILE_ERROR to OurRead\r\n");
	  return HFILE_ERROR;
	  }
   else
	  return (_lread(FD, pBuf, nCount));
   }

extern "C" L_INT32 FAR EXPORT OurSeek(L_INT FD, L_INT32 lnPos, L_INT nCount, 
										  L_VOID* pUserData)
   {
   BOOL* pbCancelLoad = (BOOL*)pUserData;
   if (*pbCancelLoad)
	  {
//	  TRACE("load cancelled, feeding -1 to OurSeek\r\n");
	  return -1;
	  }
   else
	  return (_lseek(FD, nCount, lnPos));	  
   }

void CProduct::Serialize(CArchive& ar)
   {
//   int i;

   try
	  {
	  if (ar.IsStoring())
		 {
		 ar << csDescription << iProductType << iUnitPricePennies <<
			IPAddress_a << IPAddress_b <<IPAddress_c << IPAddress_d <<
			csDevName << csCanvasName << iMedia << 
			iSizing << iDevice << bCustomPageSize <<
			iMediaWidthInches1000 << iMediaHeightInches1000 << iPrintWidthInches1000 <<
			iPrintHeightInches1000 << iSpacingInches1000 <<
			bPublish << bUnused << iBroadcastUnitPricePennies << bIsLocal << iWeightOunces100;
		 
		 ar << bUsePriceBreaks;
		 ar << iNumPriceBreaks;
/*
		 for (i = 0; i < MAXPRODUCTPRICEBREAKS; i++)
			{
			ar << iFrom[i];
			ar << iTo[i];
			ar << iPricePennies[i];
			}
*/
		 ar << iBackPrint1;
		 ar << iBackPrint2;
		 ar << csDevProfile;
		 ar << bApplyICCToPrint;
		 ar << csWholesaler;
         ar << m_stdsize << m_finish << m_usestdsize;
		 ar << csBackPrint1 << csBackPrint2;
		 }
	  else
		 {
		 int nVersion = ar.GetObjectSchema();

		 // common
		 ar >> csDescription >> iProductType >> iUnitPricePennies >>
			IPAddress_a >> IPAddress_b >> IPAddress_c >> IPAddress_d >>
			csDevName >> csCanvasName >> iMedia >>
			iSizing >> iDevice >> bCustomPageSize >>
			iMediaWidthInches1000 >> iMediaHeightInches1000 >> iPrintWidthInches1000 >>
			iPrintHeightInches1000 >> iSpacingInches1000 >>
			bPublish >> bUnused >> iBroadcastUnitPricePennies >> bIsLocal >> iWeightOunces100;

		 // new stuff in RLC13
		 if (nVersion >= 1)
			{					
			ar >> bUsePriceBreaks;
			ar >> iNumPriceBreaks;
/*
			for (i = 0; i < MAXPRODUCTPRICEBREAKS; i++)
			   {
			   ar >> iFrom[i];
			   ar >> iTo[i];
			   ar >> iPricePennies[i];
			   }
*/
			}
		 // BUG ALERT! - we double stored the nodes here in versions prior to 2
		 // so we need to load here again
		 if (nVersion < 2)
			coNodes.Serialize(ar);
		 if (nVersion >= 3)
			{
			ar >> iBackPrint1;
			ar >> iBackPrint2;
			}
		 if (nVersion >= 4)
			{
			ar >> csDevProfile;
			ar >> bApplyICCToPrint;
			}
		 if (nVersion >= 5)
			{
			ar << csWholesaler;
			}
		 if (nVersion >= 6)
			{
			ar >> m_stdsize >> m_finish >> m_usestdsize;
			}	 		 
		 if (nVersion >= 7)
			{
			ar >> csBackPrint1 >> csBackPrint2;
			}	 		 	

		 // non-rp products have an id of -1
 	     if (iSubType != SUBTYPE_PRODUCT_RPFULFILL)
			iDevice = -1;
		 }
	  //coNodes.Serialize(ar);
	  cobOptions.Serialize(ar);
	  // call the base class   
	  CNode::Serialize(ar);
	  }	  
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   }

const CProduct& CProduct::operator=(const CProduct& cpSrc) 
   {
   int i;
   CDevOption* pSrcOption;
   CDevOption* pDstOption;

   // assignment	  
   if (this != &cpSrc)
      {   
	  // copy the props
	  csDescription = cpSrc.csDescription;	  
	  iProductType = cpSrc.iProductType;	  
	  iUnitPricePennies = cpSrc.iUnitPricePennies;
	  IPAddress_a = cpSrc.IPAddress_a;
	  IPAddress_b = cpSrc.IPAddress_b;
	  IPAddress_c = cpSrc.IPAddress_c;
	  IPAddress_d = cpSrc.IPAddress_d;
	  csDevName = cpSrc.csDevName;
	  csCanvasName = cpSrc.csCanvasName;
	  iMedia = cpSrc.iMedia;	  
	  iDevice = cpSrc.iDevice;	  
      iSizing = cpSrc.iSizing;
	  bCustomPageSize = cpSrc.bCustomPageSize;
	  iMediaWidthInches1000 = cpSrc.iMediaWidthInches1000;
	  iMediaHeightInches1000 = cpSrc.iMediaHeightInches1000;
	  iPrintWidthInches1000 = cpSrc.iPrintWidthInches1000;
	  iPrintHeightInches1000 = cpSrc.iPrintHeightInches1000;
	  iSpacingInches1000 = cpSrc.iSpacingInches1000;
      bPublish = cpSrc.bPublish;
	  bUnused = cpSrc.bUnused;
	  iBroadcastUnitPricePennies = cpSrc.iBroadcastUnitPricePennies;
	  iWeightOunces100 = cpSrc.iWeightOunces100;
	  bIsLocal = cpSrc.bIsLocal;
      iNumPriceBreaks = cpSrc.iNumPriceBreaks;
	  bUsePriceBreaks = cpSrc.bUsePriceBreaks;
/*
	  for (i = 0; i < MAXPRODUCTPRICEBREAKS; i++)
		 {
		 iFrom[i] = cpSrc.iFrom[i];
		 iTo[i] = cpSrc.iTo[i];
		 iPricePennies[i] = cpSrc.iPricePennies[i];
		 }						
*/
      iBackPrint1 = cpSrc.iBackPrint1;
	  iBackPrint2 = cpSrc.iBackPrint2;
      csBackPrint1 = cpSrc.csBackPrint1;
	  csBackPrint2 = cpSrc.csBackPrint2;
	  csDevProfile = cpSrc.csDevProfile;
	  bApplyICCToPrint = cpSrc.bApplyICCToPrint;
	  csWholesaler = cpSrc.csWholesaler;
	  m_stdsize = cpSrc.m_stdsize;
	  m_finish = cpSrc.m_finish;
	  m_usestdsize = cpSrc.m_usestdsize;

	  // first reset the option array
	  DeleteDevOptionArray();

	  for (i = 0; i < cpSrc.cobOptions.GetSize(); i++)
		 {
		 if (pSrcOption = (CDevOption*)cpSrc.cobOptions.GetAt(i))
			{
			if (pDstOption = new CDevOption)
			   {
			   *pDstOption = *pSrcOption;
			   }
			cobOptions.Add((CObject*)pDstOption);
			}
		 }		  
	  // call the base class
	  CNode::operator =(cpSrc);
	  }
   return *this;    
   }

void CProduct::CreateProductPricingString(CString& csPricing)
   {
//   int i;
   CString csTemp;

   csPricing.Empty();
/*
   // special case for no price breaks
   if (iNumPriceBreaks < 1)
	  csPricing = "0";				  
   else
	  {
	  for (i = 0; i < iNumPriceBreaks; i++)
		 {
		 csTemp.Format("%d %d %d ", iFrom[i], iTo[i], iPricePennies[i]);
		 csPricing += csTemp;
		 }
	  }
*/
   }

#define SMALLPREVIEWSIZE   320
#define MEDPREVIEWSIZE     640
#define LARGEPREVIEWSIZE   1024		  
#define XLARGEPREVIEWSIZE  1500

CImage* CPhoto::GetOptimumPreview(int iSize)
   {
   int iTop, iLeft;
   CImage* pImage;
   int i;

   // first, see if we have an image which is at least as big
   // as iSize
   for (i = 0; i < coImages.GetSize(); i++)
	  {
	  pImage = (CImage*)(coImages.GetAt(i));
	  if (pImage)
		 {
		 if ((pImage->iStyle == IMAGESTYLE_SPAWN) && 
			 (pImage->Bitmap.Height >= iSize || 
			  pImage->Bitmap.Width >= iSize))
			return pImage;
		 }
	  }	
   if (iSize <= SMALLPREVIEWSIZE)
	  pImage = CreateImage(SMALLPREVIEWSIZE,
	     SMALLPREVIEWSIZE, iTop, iLeft, IMAGESIZING_FIT);
   else if (iSize <= MEDPREVIEWSIZE)
	  pImage = CreateImage(MEDPREVIEWSIZE,
	     MEDPREVIEWSIZE, iTop, iLeft, IMAGESIZING_FIT);
   else if (iSize <= LARGEPREVIEWSIZE)
	  pImage = CreateImage(LARGEPREVIEWSIZE,
	     LARGEPREVIEWSIZE, iTop, iLeft, IMAGESIZING_FIT);
   else 
	  pImage = CreateImage(XLARGEPREVIEWSIZE,
	     XLARGEPREVIEWSIZE, iTop, iLeft, IMAGESIZING_FIT);

   return pImage;
   }

CImage* CPhoto::GetOriginalPreview()
   {
   int iTop, iLeft;
   CImage* pImage;
   int i;   

   // first, see if we have an image which is at least as big
   // as iSize
   for (i = 0; i < coImages.GetSize(); i++)
	  {
	  pImage = (CImage*)(coImages.GetAt(i));
	  if (pImage)
		 {
		 if (pImage->iStyle == IMAGESTYLE_SPAWN && 
			 pImage->Bitmap.Height == GetOriginalImage()->iHeight &&
			 pImage->Bitmap.Width == GetOriginalImage()->iWidth)
			return pImage;
		 }
	  }	
   pImage = CreateImage(GetOriginalImage()->iWidth, GetOriginalImage()->iHeight,
	     iTop, iLeft, IMAGESIZING_FIT);

   return pImage;
   }

CImage* CPhoto::GetSampledImage(int iSize)
   {
   int iTop, iLeft;
   CImage* pImage;
   int i;

   // first, see if we have an image which is the correct size
   for (i = 0; i < coImages.GetSize(); i++)
	  {
	  pImage = (CImage*)(coImages.GetAt(i));
	  if (pImage)
		 {
		 if ((pImage->iStyle == IMAGESTYLE_SPAWN) && 
			 (max(pImage->Bitmap.Height, pImage->Bitmap.Width) == iSize))
			return pImage;
		 }
	  }	
   pImage = CreateImage(iSize, iSize, iTop, iLeft, IMAGESIZING_FIT);

   return pImage;
   }

COrder::COrder()
   {
   bPrintIndex = TRUE;
   iType = NODETYPE_ORDER;
   iStatus = ORDERSTATUS_LOCALCREATE;
   iNumJobs = 0;
   csOnlineAlbum.Empty();
   CTime t = CTime::GetCurrentTime();
   csSubmittedTime = t.Format("%m/%d/%y");    
   csTime = t.Format( "%m/%d/%y" );
   ctTime = 0;
//   OrderDetails.csDateTime = t.Format("%Y-%m-%dT%H:%M:%S");
   iOrderType = ORDERTYPE_LOCALCREATE;      
   bTempMark = FALSE;
   bInserted = FALSE;   
   iLastSortColumn = -1;
   bLastSortOrder = 0;   
   }

COrder::~COrder()
   {
   ResetProductCountArray();

   // delete the bar code file if it exists
   if (csBarCodeFileName.GetLength() > 0)  
	  ::DeleteFile(csBarCodeFileName);
   }

void COrder::Serialize(CArchive& ar)
   {
   CString csEncoded;
   CPhotogizeUtils utils;   
//   int month, day, year;
//   CTime* pt;
//   const char* p;
/*
   try
	  {
	  if (ar.IsStoring())
		 {
		 // encryt the order details for security in Rev 4 of the object
		 utils.Encrypt(OrderDetails.csDetails, csEncoded);	  
		 ar << csEncoded << csStatus << iStatus << csTime << bPrintIndex << 
			csProductName << iNumJobs << csOnlineAlbum << csSubmittedTime << 
			iTotalPhotoSize << iOrderType << iLastSortColumn << bLastSortOrder << ctTime;
		 }
	  else
		 {
		 int nVersion = ar.GetObjectSchema();

		 // the order details are encrypted in v4, get 'em later
		 if (nVersion >= 4)
			{
		    ar >> csEncoded >> csStatus >> iStatus >> csTime >> bPrintIndex >> 
			   csProductName >> iNumJobs;
			utils.Decrypt(csEncoded, OrderDetails.csDetails);
			}
		 else
			{
			ar >> OrderDetails.csDetails >> csStatus >> iStatus >> csTime >> bPrintIndex >> 
			   csProductName >> iNumJobs;
			}	  

		 if (nVersion >= 1)
			{
			ar >> csOnlineAlbum;
			ar >> csSubmittedTime;		 
			}
		 // default to webservices order if we don't know any better
		 iOrderType = ORDERTYPE_WEBSERVICES;
		 if (nVersion >= 2)
			ar >> iTotalPhotoSize;
		 if (nVersion >= 3)
			ar >> iOrderType;
		 // recreate order details
		 //ParseOrderDetails();

		 if (nVersion >= 5)
			{
			ar >> iLastSortColumn;
			ar >> bLastSortOrder;
			}
		 if (nVersion >= 6)
			{
			ar >> ctTime;
			}
		 // if we don't have the time yet
		 if (ctTime == 0)
			{
			if (csTime.GetLength() == 8)
			   {
			   p = csTime;
			   month = atoi(p);
			   day = atoi(p+3);
			   year = atoi(p+6);
			   if (year > 38)
				  year += 1900;
			   else
				  year += 2000;

			   if (day >= 1 && day <= 31 && month >= 1 && month <= 12 && year >= 1970 && year <= 2038)
				  {
				  if (pt = new CTime(year, month, day, 0, 0, 0))
					 {
					 ctTime = *pt;
					 delete pt;
					 }
				  }
			   else 
				  ctTime = CTime::GetCurrentTime();
			   }
			}			
		 
		 // BUG ALERT! - we double stored the nodes here in versions prior to 3
		 // so we need to load here again
		 if (nVersion < 3)
			coNodes.Serialize(ar);
		 }   
	  // call the base class   
	  CNode::Serialize(ar);
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }		
*/
   }

const COrder& COrder::operator=(const COrder& cpSrc) 
   {
   // assignment	  
   if (this != &cpSrc)
      {   
	  // copy the props
//	  OrderDetails.csDetails = cpSrc.OrderDetails.csDetails;
//	  OrderDetails.bIsParsed = FALSE;
	  // create the order details
	  //ParseOrderDetails();
	  csStatus = cpSrc.csStatus;	  
	  iStatus = cpSrc.iStatus;
	  iNumJobs = cpSrc.iNumJobs;
	  csTime = cpSrc.csTime;
	  ctTime = cpSrc.ctTime;
	  bPrintIndex = cpSrc.bPrintIndex;
	  csProductName = cpSrc.csProductName;	  
	  iTotalPhotoSize = cpSrc.iTotalPhotoSize;
	  iOrderType = cpSrc.iOrderType;
	  csOnlineAlbum = cpSrc.csOnlineAlbum;
	  csSubmittedTime = cpSrc.csSubmittedTime;
      iLastSortColumn = cpSrc.iLastSortColumn;
      bLastSortOrder = cpSrc.bLastSortOrder;	  

	  // call the base class
	  CNode::operator =(cpSrc);
	  }
   return *this;   
   }

void COrder::SortOrderByProduct()
   {
   BOOL bRet;

   do
	  {
	  bRet = DoSort();
	  } while (bRet);
   }

BOOL COrder::DoSort()
   {   
   int i;
   CPhoto* pPhoto1;
   CPhoto* pPhoto2;   

   BOOL bContinue = FALSE;

   for (i = 0; i < coNodes.GetSize() - 1; i++)
      {
	  pPhoto1 = (CPhoto*)coNodes.GetAt(i);
	  pPhoto2 = (CPhoto*)coNodes.GetAt(i + 1);
	  if (pPhoto2->csProductName < pPhoto1->csProductName)
	     {
		 coNodes.SetAt(i, (CObject*)pPhoto2);
		 coNodes.SetAt(i + 1, (CObject*)pPhoto1);
		 bContinue = TRUE;
		 }
	  }
   return bContinue;
   }

int COrder::GetTotalCopies()
   {   
   int i;
   CPhoto* pPhoto;   
   int iTotalCopies = 0;

   for (i = 0; i < coNodes.GetSize(); i++)
      {
	  pPhoto = (CPhoto*)coNodes.GetAt(i);
	  iTotalCopies += pPhoto->iCopies;
	  }
   return iTotalCopies;
   }

int COrder::GetNumDevicesInOrder(CNode* pParentProduct)
   {
   int i;
   int iNumDevices = 0;
   CPhoto* pPhoto;
   CProduct* pProduct;
   int iLastDevice = -1;

   // get a total for each product
   for (i = 0; i < coNodes.GetSize(); i++)
      {
	  // get the photo
	  pPhoto = (CPhoto*)coNodes.GetAt(i);
	  // get the product
	  if (pProduct = GetProduct(pPhoto, pParentProduct))
		 {
		 // if this is a new device, incr. the counter
		 // and set it as the last device
		 if (pProduct->iDevice != iLastDevice)
			{
			iNumDevices++;
			iLastDevice = pProduct->iDevice;
			}
		 }
	  }
   return iNumDevices;
   }

void COrder::GetProductCounts(CNode* pParentProduct)
   {
   int i;
   int j;
   CProduct* pProduct;
   CPhoto* pPhoto;
   CProductCount* pProductCount;
   BOOL bMatch;
   CProductCount* pTestProductCount;

   // reset the product count array
   ResetProductCountArray();

   // get a total for each product
   for (i = 0; i < coNodes.GetSize(); i++)
      {
	  // get the photo
	  pPhoto = (CPhoto*)coNodes.GetAt(i);

	  // don't count promo photos
	  if (pPhoto->bIsPromoPhoto) continue;

	  // get the product
	  if (pProduct = GetProduct(pPhoto, pParentProduct))	  
		 {
		 for(bMatch = FALSE, j = 0; j < coProductArray.GetSize(); j++)
			{
			if (pTestProductCount = (CProductCount*)coProductArray.GetAt(j))
			   {
			   if (pTestProductCount->pProduct == pProduct)
				  {
				  bMatch = TRUE;
				  break;
				  }
			   }
			}
		 // if no match, add it
		 if (!bMatch)
			{
			if (pProductCount = new CProductCount)
			   {
			   // load the product pointer and the copy count
			   pProductCount->pProduct = pProduct;			   
			   pProductCount->iCopies = pPhoto->iCopies;
			   pProductCount->csName = pProduct->csName;
			   pProductCount->csDescription = pProduct->csDescription;
			   coProductArray.Add((CObject*)pProductCount);
			   }
			}
		 else
			{
			pTestProductCount->iCopies += pPhoto->iCopies;
			}	   									 
		 }
	  }   
   }

void COrder::ResetProductCountArray()
   {
   int i;
   CProductCount* pProductCount;

   for (i = coProductArray.GetSize() - 1; i >= 0; i--)
	  {		 
	  if (pProductCount = (CProductCount*)coProductArray.GetAt(i))
		 {
		 delete pProductCount;			
		 }
      }     
   coProductArray.RemoveAll();
   }

int COrder::GetTotalONLINE(CNode* pParentProduct, int* piSubTotal, int* piShipCost, int* piTotalTax, int* piPromo)
   {
   int iTotal = 0;
/*
   if (piSubTotal)
	  *piSubTotal = OrderDetails.iSubTotal;
   if (piShipCost)
	  *piShipCost = OrderDetails.iShipping;
   if (piTotalTax)
	  *piTotalTax = OrderDetails.iTaxPennies;   
   if (piPromo)
	  *piPromo = OrderDetails.iPromoDiscount;
   
   // get the total - don't allow negatives
   iTotal = max(OrderDetails.iSubTotal + OrderDetails.iShipping + OrderDetails.iTaxPennies - 
	  OrderDetails.iPromoDiscount, 0);

   GenerateProductCountArrayONLINE(pParentProduct);

   return iTotal;
*/
   return 0;
   }

int COrder::GetTotal(CNode* pParentProduct, int* piSubTotal, int* piShipCost, int* piTotalTax, int* piPromo)
   {
//   int i, j, k;
//   int iSubTotal = 0;
//   int iShipCost = 0;      
//   float fTax;
   int iTotal = 0;
//   CPhotogizeUtils utils;   
//   CShipListEntry* pShipListEntry;
//   CProductCount* pProductCount;   
//   int iTotalWeightOz;
//   int iTaxAmount;   
/*
   // if we have a set total, and are looking for nothing else, just return it
   if ((OrderDetails.iTotalPrice > 0) && !piSubTotal && !piShipCost && !piTotalTax)
	  {
	  // need to do this to force a display of the product description
	  GetProductCounts(pParentProduct);
	  return OrderDetails.iTotalPrice;
	  }

   // if we have order items, use the ONLINE style of order computation
   if (OrderDetails.coOrderItemArray.GetSize() > 0)
	  return GetTotalONLINE(pParentProduct, piSubTotal, piShipCost, piTotalTax);

   // generate the product count array
   GenerateProductCountArray(pParentProduct);

   // now total up the amounts
   for (i = 0; i < coProductArray.GetSize(); i++)
	  {
	  if (pProductCount = (CProductCount*)coProductArray.GetAt(i))
		 {
		 iSubTotal += pProductCount->iSubTotalPennies;
		 }
	  }
   
   if (piSubTotal)
	  *piSubTotal = iSubTotal;   

   // get tax
   if (pAccountOwner->m_addtax)
	  fTax = pAccountOwner->m_taxpercent;
   else
	  fTax = 0.0;

   // add tax
   iTaxAmount = (int)((float)iSubTotal * fTax / 100.F + .5F);
   iTotal = iSubTotal + iTaxAmount;		   

   // now total up the weight
   for (i = 0, j = 0; i < coProductArray.GetSize(); i++)
	  {
	  if (pProductCount = (CProductCount*)coProductArray.GetAt(i))
		 {
		 j += pProductCount->iWeightOz100;
		 }
	  }

   iTotalWeightOz = (int)((float)j / 100.f + .5F);

   for (i = 0; i < pAccountOwner->coShipList.GetSize(); i++)
	  {
	  if (pShipListEntry = (CShipListEntry*)pAccountOwner->coShipList.GetAt(i))
		 {
		 // found it
		 if (pShipListEntry->csName == OrderDetails.ShipMethod)
			{
			// start with the base cost
			iShipCost = pShipListEntry->iBaseHandlingPricePennies;

			// look through the productcount array and get the ship cost 
			// for each product group
			for (j = 0; j < coProductArray.GetSize(); j++)
			   {
			   if (pProductCount = (CProductCount*)coProductArray.GetAt(j))
				  {				  
				  // now loop through the ship list to get the ship breaks
				  for(k = 0; k < pShipListEntry->iNumBreaks; k++)
					 {
					 // at the end, we just compare the "froms"
					 if (k == (pShipListEntry->iNumBreaks - 1))
						{
						if (iTotalWeightOz >= pShipListEntry->iQtyFrom[k])
						   {
						   iShipCost += pShipListEntry->iPricePennies[k];
						   break;
						   }
						}
					 else
						{
						if (iTotalWeightOz >= pShipListEntry->iQtyFrom[k] &&
						    iTotalWeightOz <= pShipListEntry->iQtyTo[k])
						   {
						   iShipCost += pShipListEntry->iPricePennies[k];
						   break;
						   }
						}
					 }
				  }
			   }
			}
		 }
	  }

   iTotal += iShipCost;

   if (piShipCost)
	  *piShipCost = iShipCost;
   
   if (piTotalTax)
	  *piTotalTax = iTaxAmount;

   // special override
   if (OrderDetails.iTotalPrice > 0)
	  iTotal = OrderDetails.iTotalPrice;
*/
   return iTotal;
   }

void COrder::DeletePhotos()
   {
   int i;
   CPhoto* pPhoto;

   // delete the photos in the order
   for (i = coNodes.GetSize() - 1; i >= 0; i--)
	  {
	  if (pPhoto = (CPhoto*)coNodes.GetAt(i))				  
		 {
		 // this will delete the cache files and
		 // delete the disk file if it is not local
		 pPhoto->DeleteDiskFile();
		 delete pPhoto;
		 }			  
	  }
   coNodes.RemoveAll();
   }
/*
void COrder::UpdateStatus(CPhotogizeProxyClient* ppgClient)
   {
   COurPGClient pgClient;
   CString csStatusStr;   
   BOOL bMustClose = FALSE;

   if (!pAccountOwner) return;

   if (!pAccountOwner->m_usewebservices) return;

   AfxGetApp()->DoWaitCursor(1);

   // if no client, open one
   if (!ppgClient)
	  {
	  bMustClose = TRUE;	  
	  if (pgClient.OpenOurSession(pAccountOwner, pAccountOwner->m_user, TRUE) != pgpc_returnok) 
		 {
		 AfxGetApp()->DoWaitCursor(-1);
		 return;
		 }
	  else
		 {
		 ppgClient = &pgClient;
		 }	  
	  }   
   // map the istatus to a string
   GetStatus(csStatusStr);

   // and set it   
   ppgClient->SetOrderStatus(pAccountOwner->m_user, csName, csStatusStr, pAccountOwner->csStoreID);

   if (bMustClose)
      ppgClient->CloseSession();

   AfxGetApp()->DoWaitCursor(-1);
   }
*/																	   
void CShipListEntry::Serialize(CArchive& ar)
   {     
   int i;

   try
	  {
	  if (ar.IsStoring())
		 {
		 ar << csName << iNumBreaks << iBaseHandlingPricePennies;
		 for (i = 0; i < iNumBreaks; i++)
			{
			ar << iQtyFrom[i];
			ar << iQtyTo[i];
			ar << iPricePennies[i];
			}
		 }
	  else
		 {
		 ar >> csName >> iNumBreaks >> iBaseHandlingPricePennies;
		 for (i = 0; i < iNumBreaks; i++)
			{
			ar >> iQtyFrom[i];
			ar >> iQtyTo[i];
			ar >> iPricePennies[i];
			}
		 }   
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   }

void COrder::GenerateProductCountArray(CNode* pParentProduct)
   {   
   int i;
//   int j;
   CProduct* pProduct;   
   int iPricePennies;   
   int iTotalCopies = 0;   
   CProductCount* pProductCount;

   // first, get the product counts
   GetProductCounts(pParentProduct);

   // now total up the amounts
   for (i = 0; i < coProductArray.GetSize(); i++)
	  {
	  if (pProductCount = (CProductCount*)coProductArray.GetAt(i))
		 {
		 // set the product and total copies
		 pProduct = pProductCount->pProduct;
		 iTotalCopies = pProductCount->iCopies;

		 // default
		 iPricePennies = pProduct->iUnitPricePennies;
/*
		 // if we use price breaks, get the price based on the copies
		 if (pProduct->bUsePriceBreaks)
			{
			for (j = 0; j < pProduct->iNumPriceBreaks; j++)
			   {
			   // at the end, just check the "from"
			   if (j == (pProduct->iNumPriceBreaks - 1))
				  {
				  if (iTotalCopies >= pProduct->iFrom[j])
					 {
					 iPricePennies = pProduct->iPricePennies[j];
					 break;
					 }
				  }
			   else
				  {
				  if (iTotalCopies >= pProduct->iFrom[j] && iTotalCopies <= pProduct->iTo[j])
					 {
					 iPricePennies = pProduct->iPricePennies[j];
					 break;
					 }
				  }
			   }
			}
*/
		 // store this
		 pProductCount->iUnitPricePennies = iPricePennies;
		 pProductCount->iSubTotalPennies = iPricePennies * iTotalCopies;
		 pProductCount->iWeightOz100 = 
			pProductCount->pProduct->iWeightOunces100 * iTotalCopies;
		 }
	  }   
   }

CProduct* COrder::GetProduct(CPhoto* pPhoto, CNode* pParentProduct)
   {
   int i;
   CProduct* pProduct;
   CProduct* pProductRet;

   if (pParentProduct->coNodes.GetSize() < 1) return NULL;

   // in case of no match, we will return the first product
   // note that we have checked the array for a minimum of
   // one entry before we ever get here
   pProductRet = (CProduct*)pParentProduct->coNodes.GetAt(0);

   // loop through all product and look for a match
   for (i = 0; i < pParentProduct->coNodes.GetSize(); i++)
	  {
	  if (pProduct = (CProduct*)pParentProduct->coNodes.GetAt(i))
		 {
		 if (pProduct->csName == pPhoto->csProductName)
			{
			pProductRet = pProduct;
			break;
			}
		 }
	  }
   return pProductRet;
   }

CProduct* COrder::GetProduct(int idx, CNode* pParentProduct)
   {
   int i;
   CProduct* pProduct;
   CProduct* pProductRet;

   if (pParentProduct->coNodes.GetSize() < 1) return NULL;

   // in case of no match, we will return the first product
   // note that we have checked the array for a minimum of
   // one entry before we ever get here
   pProductRet = (CProduct*)pParentProduct->coNodes.GetAt(0);

   // loop through all product and look for a match
   for (i = 0; i < pParentProduct->coNodes.GetSize(); i++)
	  {
	  if (pProduct = (CProduct*)pParentProduct->coNodes.GetAt(i))
		 {
		 if (i == idx)
			{
			pProductRet = pProduct;
			break;
			}
		 }
	  }
   return pProductRet;
   }

CProduct* COrder::GetProduct(LPCSTR lpcstrName, CNode* pParentProduct)
   {
   int i;
   CProduct* pProduct;
   CProduct* pProductRet;

   if (pParentProduct->coNodes.GetSize() < 1) return NULL;

   // in case of no match, we will return the first product
   // note that we have checked the array for a minimum of
   // one entry before we ever get here
   pProductRet = (CProduct*)pParentProduct->coNodes.GetAt(0);

   // loop through all product and look for a match
   for (i = 0; i < pParentProduct->coNodes.GetSize(); i++)
	  {
	  if (pProduct = (CProduct*)pParentProduct->coNodes.GetAt(i))
		 {
		 if (pProduct->csName == lpcstrName)
			{
			pProductRet = pProduct;
			break;
			}
		 }
	  }
   return pProductRet;
   }

CProduct* COrder::GetProduct(CNode* pParentProduct)
   {
   int i;
   CProduct* pProduct;
   CProduct* pProductRet;

   if (pParentProduct->coNodes.GetSize() < 1) return NULL;

   // in case of no match, we will return the first product
   // note that we have checked the array for a minimum of
   // one entry before we ever get here
   pProductRet = (CProduct*)pParentProduct->coNodes.GetAt(0);

   // loop through all product and look for a match
   for (i = 0; i < pParentProduct->coNodes.GetSize(); i++)
	  {
	  if (pProduct = (CProduct*)pParentProduct->coNodes.GetAt(i))
		 {
		 if (pProduct->csName == csProductName)
			{
			pProductRet = pProduct;
			break;
			}
		 }
	  }
   return pProductRet;
   }

void COrder::GetStatus(CString& csStatusStr)
   {
   switch(iStatus)
	  {
	  case ORDERSTATUS_LOCALCREATE:
		   csStatusStr = "Created";
		   break;
	  case ORDERSTATUS_SUBMITTED:
		   csStatusStr = "Submitted";
		   break;
	  case ORDERSTATUS_PRINTED_OK:
		   csStatusStr = "Printed OK";
		   break;
	  case ORDERSTATUS_PRINTED_ERROR:
		   csStatusStr = "Error Printing";
		   break;
	  case ORDERSTATUS_PRINTED_COMPLETE:
		 csStatusStr = "Complete";
		   break;
      case ORDERSTATUS_HARVESTED:
		   csStatusStr = "Harvested";
		   break;
      case ORDERSTATUS_READYFORPICKUP:
		   csStatusStr = "Ready for Pickup";
		   break;
      case ORDERSTATUS_SHIPPED:
		   csStatusStr = "Shipped";
		   break;
	  default:
		   csStatusStr = "Submitted";
		   break;
	  }
   }

void COrder::SetStatusFromStr(CString& csStatusStr)
   {
   if (csStatusStr == "Created")
	  iStatus = ORDERSTATUS_LOCALCREATE;
   else if (csStatusStr == "Submitted")
	  iStatus = ORDERSTATUS_SUBMITTED;
   else if (csStatusStr == "Printed OK")
	  iStatus = ORDERSTATUS_PRINTED_OK;
   else if (csStatusStr == "Error Printing")
	  iStatus = ORDERSTATUS_PRINTED_ERROR;
   else if (csStatusStr == "Complete")
	  iStatus = ORDERSTATUS_PRINTED_COMPLETE;
   else if (csStatusStr == "Harvested")
	  iStatus = ORDERSTATUS_HARVESTED;
   else if (csStatusStr == "Ready for Pickup")
	  iStatus = ORDERSTATUS_READYFORPICKUP;
   else if (csStatusStr == "Shipped")
	  iStatus = ORDERSTATUS_SHIPPED;
   else
	  iStatus = ORDERSTATUS_SUBMITTED;
   }

CImageCache::CImageCache()
   {
   iPercent = 10; // arbitrary
   }

void CImageCache::Init(LPCSTR csCacheFileName, LPCSTR csCacheFolder)
   {
   CFile cf;   
   int i;
   CImageCacheElement* pElement;
   CFileStatus cfs;

   // save this
   csFolder = csCacheFolder;
   // make sure its ok
   VERIFYPATH(csFolder);

   // and save this
   csFileName = csCacheFolder;
   csFileName += csCacheFileName;

   // open the cache and read the contents
   if (cf.Open(csFileName, CFile::modeRead))
	  {
	  try
		 {
		 CArchive ar(&cf, CArchive::load);
         coElements.Serialize(ar);

		 // check for existence of cache files, if they don't exist
		 // delete the element
		 for (i = coElements.GetSize() - 1; i >= 0; i--)
			{
			if (pElement = (CImageCacheElement*)coElements.GetAt(i))
			   {
			   if (!(CFile::GetStatus(pElement->csFileName, cfs)))
				  {
				  delete pElement;
				  coElements.RemoveAt(i);
				  }
			   }
			}
		 }
      catch(CException* theException)
		 {      
		 // oops
		 theException->Delete();
		 }
	  cf.Close();
	  }			 
   }

void COnlineImageCache::Init(LPCSTR csCacheFileName, LPCSTR csCacheFolder)
   {
   CFile cf;   
   int i;
   COnlineImageCacheElement* pElement;
   CFileStatus cfs;

   // save this
   csFolder = csCacheFolder;
   // make sure its ok
   VERIFYPATH(csFolder);

   // and save this
   csFileName = csCacheFolder;
   csFileName += csCacheFileName;

   // open the cache and read the contents
   if (cf.Open(csFileName, CFile::modeRead))
	  {
	  try
		 {
		 CArchive ar(&cf, CArchive::load);
         coElements.Serialize(ar);

		 // check for existence of cache files, if they don't exist
		 // delete the element
		 for (i = coElements.GetSize() - 1; i >= 0; i--)
			{
			if (pElement = (COnlineImageCacheElement*)coElements.GetAt(i))
			   {
			   if (!(CFile::GetStatus(pElement->csFileName, cfs)))
				  {				  
				  delete pElement;
				  coElements.RemoveAt(i);
				  }
			   }
			}
		 }
      catch(CException* theException)
		 {      
		 // oops
		 theException->Delete();
		 }
	  cf.Close();
	  }			 
   }

CImageCache::~CImageCache()
   {
   // write the cache out
   Write();

   // and free up the memory
   DeleteElements();
   }

COnlineImageCache::~COnlineImageCache()
   {
   // write the cache out
   Write();

   // and free up the memory
   DeleteElements();
   }

CImageCacheBase::CImageCacheBase()
   {
   // Initialize the critical section.
   InitializeCriticalSection(&CriticalSection); 
   }
   
CImageCacheBase::~CImageCacheBase()
   {
   // Initialize the critical section.
   DeleteCriticalSection(&CriticalSection); 
   }	    

void CImageCacheBase::Write()
   {
   CFile cf;   
   
   if (csFileName.GetLength() < 1) return;

   // open the cache and read the contents
   if (cf.Open(csFileName, CFile::modeCreate | CFile::modeWrite))
	  {
	  try
		 {
		 CArchive ar(&cf, CArchive::store);
         coElements.Serialize(ar);
		 }
      catch(CException* theException)
		 {      
		 // oops
		 theException->Delete();
		 }
	  cf.Close();
	  }			 
   }

void CImageCache::DeleteElements(BOOL bDeleteDiskFile)
   {
   int i;
   CImageCacheElement* pElement;

   try
	  {
	  for (i = coElements.GetSize() - 1; i >= 0; i--)
		 {		 
		 if (pElement = (CImageCacheElement*)coElements.GetAt(i))
			{
			// delete the disk file?
			if (bDeleteDiskFile) ::DeleteFile(pElement->csFileName);		 
			delete pElement;			
			}
		 }
	  coElements.RemoveAll();
	  }
   catch(...)
	  {
	  }
   }

void COnlineImageCache::DeleteElements(BOOL bDeleteDiskFile)
   {
   int i;
   COnlineImageCacheElement* pElement;

   try
	  {
	  for (i = coElements.GetSize() - 1; i >= 0; i--)
		 {		 
		 if (pElement = (COnlineImageCacheElement*)coElements.GetAt(i))
			{
			// delete the disk file?
			if (bDeleteDiskFile) ::DeleteFile(pElement->csFileName);		 
			delete pElement;			
			}
		 }
	  coElements.RemoveAll();
	  }
   catch(...)
	  {
	  }
   }

void CImageCache::Clear()
   {
   // delete all the elements in the secondary cache
   // and delete the files
   DeleteElements(TRUE);
   // delete the cache file
   ::DeleteFile(csFileName);
   }

void COnlineImageCache::Clear()
   {
   // delete all the elements in the secondary cache
   // and delete the files
   DeleteElements(TRUE);
   // delete the cache file
   ::DeleteFile(csFileName);
   }

void CImageCache::Clear(CNode* pNode)
   {
   int i;
   int j;
   int k;
   CNode* pParentNode;
   CPhoto* pPhoto;
   CImage* pImage;

   // clear all the images from the primary cache
   for (i = pNode->coNodes.GetSize() - 1; i >=0; i--)
	  {
	  if (pParentNode = (CNode*)pNode->coNodes.GetAt(i))
		 {
		 for (j = pParentNode->coNodes.GetSize() - 1; j >=0; j--)
			{
			if (pPhoto = (CPhoto*)pParentNode->coNodes.GetAt(j))
			   {
			   if (pPhoto->iType == NODETYPE_PHOTO)
				  {
				  for (k = pPhoto->coImages.GetSize() - 1; k >=0; k--)
					 {
					 if (pImage = (CImage*)pPhoto->coImages.GetAt(k))
						{
						if (pImage->iStyle == IMAGESTYLE_SPAWN)
						   {
						   delete pImage;
						   pPhoto->coImages.RemoveAt(k);
						   }
						}
					 }
				  }
			   }
			}
		 }
	  }
   }

void CImageCacheBase::SetCachePercent(int iP)
   {
   iPercent = iP;
   }

BOOL CImageCacheBase::CanWeAddElement(UINT uiSize)
   {
   // get free disk space    
   float fFreeDiskSpace;    
   float fUsableDiskSpace;    

   // get free disk space   
   fFreeDiskSpace = CPhotogizeUtils::GetFreeDiskSpace(csFolder);

   // get usable disk space
   fUsableDiskSpace = fFreeDiskSpace * (float)iPercent / 100.;

   // is the size of what we want to write bigger than
   // the usable disk space?
   if ((float)uiSize > fUsableDiskSpace)
	  return FALSE;
   else
	  return TRUE;

   return TRUE;
   }

CImageCacheElement::CImageCacheElement()
   {
   }

CImageCacheElement::~CImageCacheElement()
   {
   }

void CImageCacheElement::Serialize(CArchive& ar)
   {           
   try
	  {
	  if (ar.IsStoring())
		 {
		 ar << ctCreated << ctLastAccess << csFileName << iTop << iLeft <<
			Original_csFileName << Original_ctFileTime << Original_FileSize;
		 }
	  else
		 {
		 ar >> ctCreated >> ctLastAccess >> csFileName >> iTop >> iLeft >>
			Original_csFileName >> Original_ctFileTime >> Original_FileSize;
		 }   
	  ci.Serialize(ar);
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }	 
   }

COnlineImageCache::COnlineImageCache()
   {
   }

COnlineImageCacheElement::COnlineImageCacheElement()
   {
   }

COnlineImageCacheElement::~COnlineImageCacheElement()
   {
   }

void COnlineImageCacheElement::Serialize(CArchive& ar)
   {     
   try
	  {
	  if (ar.IsStoring())
		 {
		 ar << csFileName << csUser << csAlbum << csPhoto << iSize << iDateTimeSecs << ctInstantiated;
		 }
	  else
		 {
		 int nVersion = ar.GetObjectSchema();

		 ar >> csFileName >> csUser >> csAlbum >> csPhoto >> iSize >> iDateTimeSecs;
		 if (nVersion == 0)		 
			ctInstantiated = CTime::GetCurrentTime();
		 else if (nVersion == 1)
			ar >> ctInstantiated;		  
		 }      
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   }

BOOL COnlineImageCache::CheckCache(CString& csUser, 
   CString& csAlbum, CString& csPhoto, int iSize, int iDateTimeSecs, 
   CString& csFileName)
   {
   int i;
   COnlineImageCacheElement* pElement = NULL;
   BOOL bRet = FALSE;
   CFileStatus cfs;	

   // Request ownership of the critical section.
   __try 
	  {
      EnterCriticalSection(&CriticalSection); 	  

	  for (i = coElements.GetSize() - 1; i >= 0; i--)
		 {		 
		 if (pElement = (COnlineImageCacheElement*)coElements.GetAt(i))
			{
			if (pElement)
			   {
			   if (pElement->csUser == csUser &&
				   pElement->csAlbum == csAlbum &&
			   	   pElement->csPhoto == csPhoto &&
			   	   pElement->iSize == iSize &&
			   	   pElement->iDateTimeSecs == iDateTimeSecs)
				  {
				  // see if it exists, if so, get it
				  // otherwise, delete this cache element
				  if (!(CFile::GetStatus(pElement->csFileName, cfs)))
					 {					 
					 delete pElement;
					 coElements.RemoveAt(i);
					 }
				  else
					 {					 
					 csFileName = pElement->csFileName;
				     bRet = TRUE;
					 }
				  break;
				  }
			   }
			}
		 }     
	  }
   __finally 
	  {
	  // Release ownership of the critical section.
	  LeaveCriticalSection(&CriticalSection);
	  }

   return bRet;
   }

void COnlineImageCache::Cull(int iDays)
   {
   int i;
   COnlineImageCacheElement* pElement;
   BOOL bRet = FALSE;   
   CTimeSpan ctsElapsedTime;
   char fname[_MAX_FNAME];
   CFileStatus cfs;

   try
	  {
	  // look for any aged element and delete it
	  for (i = coElements.GetSize() - 1; i >= 0; i--)
		 {		 
		 if (pElement = (COnlineImageCacheElement*)coElements.GetAt(i))
			{
			if (pElement)
			   {
			   ctsElapsedTime = CTime::GetCurrentTime() - pElement->ctInstantiated;
			   if (ctsElapsedTime.GetDays() > iDays)
				  {
				  // We need to do a special check here.  Prior to 1.0.4, the online
				  // cache used the actual downloaded filename, this will cause image
				  // files to get deleted when the cache expires.  We only want
				  // an image file to get deleted when the order is deleted.  The cache
				  // image file should be a separate file that we can delete.  The name
				  // for the image cache file prior to 1.0.4 started with "lab", in 1.0.4
				  // and beyond, the name starts with "loc".  If we see a "a" in the second 
				  // char position, it's an older cache file - delete it if the file no 
				  // longer exists
				  // get drive from path
				  fname[1] = 0;
				  _splitpath(pElement->csFileName, NULL, NULL, fname, NULL);
				  if (fname[1] == 'a')
					 {
					 if (!(CFile::GetStatus(pElement->csFileName, cfs)))
						{
						DeleteFile(pElement->csFileName);					 
						delete pElement;
						coElements.RemoveAt(i);
						}
					 else
						continue;
					 }	 
				  else
					 {
					 DeleteFile(pElement->csFileName);				  
					 delete pElement;
					 coElements.RemoveAt(i);
					 }
				  }
			   }
			}
		 }        
	  }
   catch(...)
	  {
	  }
   }

void COnlineImageCache::AddElement(CString& csUser, CString& csAlbum, CString& csPhoto, 
   int iSize, int iDateTimeSecs, CString& csFileName)
   {   
   //char pTempPath[_MAX_PATH];
   char pTempFileName[_MAX_PATH];
   COnlineImageCacheElement* pElement = NULL;
   BOOL bOK = FALSE;
   
   if (!(pElement = new COnlineImageCacheElement)) return;

   // Initialize the critical section.
   // should put these in try/finally code, but the code
   // has objects that must unwind, so we will risk an exception
   // in getting a critical section for code simplicity
   // we might want to recode this at a later date
   EnterCriticalSection(&CriticalSection); 	  

   // Request ownership of the critical section.
   try
	  {
	  // make a temp file name	  	  
	  GetTempFileName(csFolder, "loc", 0, pTempFileName);		 
	  // copy the file
	  if (CPhotogizeUtils::CopyFile(csFileName, pTempFileName, FALSE))
		 {
		 pElement->csUser = csUser;
		 pElement->csAlbum = csAlbum;
		 pElement->csPhoto = csPhoto;
		 pElement->iSize = iSize;
		 pElement->iDateTimeSecs = iDateTimeSecs;
		 pElement->csFileName = pTempFileName;
		 // time of instantiation
		 pElement->ctInstantiated = CTime::GetCurrentTime();
		 coElements.Add((CObject*)pElement);
		 bOK = TRUE;
		 }			        
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   
   // Release ownership of the critical section.
   LeaveCriticalSection(&CriticalSection);      

   try
	  {
	  if (!bOK) delete pElement;
	  }
   catch(...)
	  {
	  }
   }   

CImage* CPhoto::CheckCache(int iWidth, int iHeight, int iSizing, int& iTop, int& iLeft)
   {
   int i;
   CImageCacheElement* pElement;
   CImage* pImageRet = NULL;
   CFileStatus cfs;

   if (!pCache) return FALSE;

#ifndef USECACHE
   return NULL;
#endif

   // Initialize the critical section.
   // should put these in try/finally code, but the code
   // has objects that must unwind, so we will risk an exception
   // in getting a critical section for code simplicity
   // we might want to recode this at a later date
   EnterCriticalSection(&pCache->CriticalSection); 	  

   try 
	  {
	  // look through the cache for a matching CImage
	  for (i = pCache->coElements.GetSize() - 1; i >= 0; i--)
		 {		 
		 if (pElement = (CImageCacheElement*)pCache->coElements.GetAt(i))
			{
			// if the file is missing, remove it
			if (!(CFile::GetStatus(pElement->csFileName, cfs)))
			   {				  
			   delete pElement;
			   pCache->coElements.RemoveAt(i);
			   continue;
			   }
			if (!GetOriginalImage()) continue;
			if (pElement->ci.iWidthSizeTo == iWidth &&
			    pElement->ci.iHeightSizeTo == iHeight &&
				pElement->ci.iSizing == iSizing &&
				pElement->Original_csFileName == GetOriginalImage()->csFileName &&
			    pElement->Original_ctFileTime == GetOriginalImage()->ctFileTime &&
			    pElement->Original_FileSize == GetOriginalImage()->FileSize)
			   {
			   // see if the original file has changed
			   if (CFile::GetStatus(pElement->Original_csFileName, cfs))
				  {	
				  if (cfs.m_ctime != pElement->Original_ctFileTime)
					 {
					 delete pElement;
					 pCache->coElements.RemoveAt(i);
					 continue;
					 }
				  }
			   // a match, so add it to the image list
			   if (pImageRet = new CImage)
				  {		
				  *pImageRet = pElement->ci;
				  iTop = pElement->iTop;
				  iLeft = pElement->iLeft;
				  // mark access time
				  pElement->ctLastAccess = ::GetCurrentTime();
				  // and load the bitmap
				  pImageRet->csFileName = pElement->csFileName;
				  //if (::L_LoadBitmap((char*)(const char*)pElement->csFileName,
				  //&pImageRet->Bitmap, 0, ORDER_BGR, NULL, NULL) == SUCCESS)
				  if (pImageRet->LoadImage())
					 coImages.Add((CObject*)pImageRet);
				  else
					 {
					 delete pImageRet;
					 pImageRet = NULL;
					 }
				  }
			   break;
			   }
			}		
		 }			 
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   
   // Release ownership of the critical section.
   LeaveCriticalSection(&pCache->CriticalSection);      

   return pImageRet;
   }

void CPhoto::AddToCache(CImage* pImage, int iTop, int iLeft)
   {
   CImageCacheElement* pElement = NULL;   

   if (!pCache) return;

#ifndef USECACHE
   return;
#endif

   // Initialize the critical section.
   // should put these in try/finally code, but the code
   // has objects that must unwind, so we will risk an exception
   // in getting a critical section for code simplicity
   // we might want to recode this at a later date   
   EnterCriticalSection(&pCache->CriticalSection); 	  

   if (pCache->CanWeAddElement(pImage->FileSize))
	  {
	  if (pElement = new CImageCacheElement)
		 {			
		 // add the image
		 pElement->ci = *pImage;
		 pElement->ctCreated = ::GetCurrentTime();
		 pElement->Original_csFileName = GetOriginalImage()->csFileName;
		 pElement->Original_ctFileTime = GetOriginalImage()->ctFileTime;
		 pElement->Original_FileSize = GetOriginalImage()->FileSize;
		 pElement->iTop = iTop;
		 pElement->iLeft = iLeft;

		 pCache->coElements.Add((CObject*)pElement);

		 // create temp filename
		 GetTempFileName(pCache->csFolder, "lab", 0, 
			pElement->csFileName.GetBuffer(_MAX_PATH));
			pElement->csFileName.ReleaseBuffer();

   		 // and save the bitmap
   		 ::L_SaveBitmap((char*)(const char*)pElement->csFileName, &pImage->Bitmap, 
			FILE_JFIF, 24, 2, NULL);
		 }	 
	  }		

   // Release ownership of the critical section.
   LeaveCriticalSection(&pCache->CriticalSection);
   }

CPhotogizeUtils::CPhotogizeUtils()
   {
   // load encrypt/decrypt password
   csPassword = ".1x~g9m153i0.;/fd];aw]213;ep,mn1045gdvbh[oJasd_";      
   }

CPhotogizeUtils::~CPhotogizeUtils()
   {   
   }

void CPhotogizeUtils::CopyStringArrays(CStringArray& csaIn, CStringArray& csaOut)
   {
   int i;
   CString csTemp;

   // reset 
   csaOut.RemoveAll();
   for (i = 0; i < csaIn.GetSize(); i++)
	  {
	  csTemp = csaIn.GetAt(i);
	  if (csTemp.GetLength() > 0)
		 csaOut.Add(csTemp);
	  }
   }
					
float CPhotogizeUtils::GetFreeDiskSpace(LPCSTR lpcstrPath)
   {
   float fRet = 0.0;
   char drive[_MAX_DRIVE];
   CString csTest;
   DWORD SectorsPerCluster;
   DWORD BytesPerSector;
   DWORD NumberOfFreeClusters;
   DWORD TotalNumberOfClusters;

   // get drive from path
   _splitpath(lpcstrPath, drive, NULL, NULL, NULL);

   // add a backslash
   csTest = drive;
   csTest += "\\";

   // get free disk space
   if (GetDiskFreeSpace(csTest, &SectorsPerCluster, &BytesPerSector,
	  &NumberOfFreeClusters, &TotalNumberOfClusters))
	  {
	  fRet = (float)NumberOfFreeClusters * (float)SectorsPerCluster * (float)BytesPerSector;
	  }
   return fRet;
   }

void CPhotogizeUtils::GetUniqueName(CString& csUnique)
   {
   GUID guid;   
   
   // create the guid
   CoCreateGuid(&guid);

   // instantiate the BSTR
   CComBSTR b(39, L""); 

   // get the string
   StringFromGUID2(guid, b.m_str, 39);

   // convert
   csUnique = b;

   // strip stuff
   csUnique.Replace("-", "");   
   csUnique.Replace("{", "");   
   csUnique.Replace("}", "");
   }

BOOL CPhotogizeUtils::IsValidEmail(LPCSTR lpcstrEmailStr)
   {
   CString cs;

   // the smallest vaid email would be a@b.c
   if (strlen(lpcstrEmailStr) < 5) return FALSE;

   // let's use a CString for work
   cs = lpcstrEmailStr;

   if (cs.Find('.') <= 2) return FALSE;

   if (cs.Find('@') < 1) return FALSE;

   return TRUE;
   }

BOOL CPhotogizeUtils::IsValidEmail(CStringArray& csaEmail)
   {
   int i;
   CString cs;
   BOOL bRet = TRUE;

   if (csaEmail.GetSize() < 1)
	  return FALSE;   

   for (i = 0; i < csaEmail.GetSize(); i++)
	  {
	  cs = csaEmail.GetAt(i);
	  if (!(bRet = IsValidEmail(cs)))
		 break;
	  }

   return bRet;
   }

BOOL CPhotogizeUtils::IsInArray(LPCSTR lpcstrTest, CStringArray& csa)
   {
   int i;
   CString csTest;
   CString csInput;
   BOOL bRet = FALSE;

   // sanity checks
   if (csa.GetSize() < 1) return FALSE;
   if (strlen(lpcstrTest) < 1) return FALSE;

   // put here to do some work
   csInput = lpcstrTest;

   // nuke white space and crap
   csInput.TrimRight();
   csInput.TrimLeft();

   // loop through the array
   for (i = 0; i < csa.GetSize(); i++)
	  {
	  // get a member
	  if (csTest = csa.GetAt(i))
		 {
		 // nuke white space and crap
		 csTest.TrimRight();
		 csTest.TrimLeft();
		 // woo hoo
		 if (csTest.CompareNoCase(csInput) == 0)
			{
			bRet = TRUE;
			break;
			}
		 }
	  }
   return bRet;
   }

void CPhotogizeUtils::AppendXMLKeyStringToken(LPCSTR lpcstrKey, LPCSTR lpcstrToken,
   CString& csPost)
   {
   CString csTemp;

   csTemp.Format("<%s>%s</%s>\r\n", lpcstrKey, ReplacePreDefinedChar(lpcstrToken), lpcstrKey);
   csPost += csTemp;
   }

void CPhotogizeUtils::AppendXMLKeyIntToken(LPCSTR lpcstrKey, int iToken,
   CString& csPost)
   {
   CString csTemp;

   csTemp.Format("<%s>%d</%s>\r\n", lpcstrKey, iToken, lpcstrKey);
   csPost += csTemp;
   }

/*---------------------------------------------------------------------------
 * PreDefinedCharMap
 * XML predefined char map: such as map "<" to "&lt;"
 *
 */
void CPhotogizeUtils::PreDefinedCharMap(char inChar, CString &mappedCharStr)
{
  switch (inChar) {
    case ('>'):
      mappedCharStr = "&gt;";
      break;
    case ('<'):
      mappedCharStr = "&lt;";
      break;
    case ('&'):
      mappedCharStr = "&amp;";
      break;
    case ('\''):
      mappedCharStr = "&apos;";
      break;
    case ('"'):
      mappedCharStr = "&quot;";
      break;
    default:
      mappedCharStr = inChar;
      break;
  }

}	

/*---------------------------------------------------------------------------
 * HavePreDefinedChar
 * check string to see if it contains any XML predefined char
 *
 */
BOOL CPhotogizeUtils::HavePreDefinedChar(LPCSTR lpcstrStr)
{
  if(strlen(lpcstrStr) < 1){
    return false;
  }

  int len = strlen(lpcstrStr);
  for (int i = 0; i < len; ++i) {
    if (IsPreDefinedChar(lpcstrStr[i])) {
      return true;
    }
  }
  return false;
}	

/*---------------------------------------------------------------------------
 * IsPreDefinedChar
 * check the input char to see if it is an XML predefined char
 *
 */
BOOL CPhotogizeUtils::IsPreDefinedChar(char inChar)
{
  switch (inChar) {
    case ('>'):
    case ('<'):
    case ('&'):
    case ('\''):
    case ('"'):
      return true;
    default:
      return false;
  }
}	

/*---------------------------------------------------------------------------
 * ReplacePreDefinedChar
 * Replace XML predefined chars
 *
 */
void CPhotogizeUtils::ReplacePreDefinedChar(LPCSTR lpcstrStr, CString &outStr)
{
  // Just return the original string if it was empty
  // or it did not contain any bad characters.
  if ((strlen(lpcstrStr) < 1) || !HavePreDefinedChar(lpcstrStr)){
    outStr = lpcstrStr;
    return;
  }

  outStr.Empty();

  int     len = strlen(lpcstrStr);
  TCHAR   cChar;
  CString  mappedStr;

  for (int i = 0; i < len; ++i) {
    cChar = lpcstrStr[i];
    if (IsPreDefinedChar(cChar)) { // replace the predefined char
      PreDefinedCharMap(cChar, mappedStr);
      outStr += mappedStr;
    }
    else {
      outStr += cChar;
    }
  }
}

CString CPhotogizeUtils::ReplacePreDefinedChar(LPCSTR lpcstrStr)
   {
   CString csRet;
   
   ReplacePreDefinedChar(lpcstrStr, csRet);

   return csRet;
   }

void CPhotogizeUtils::MakeTempFileName(CString& csTempFileName, LPCSTR lpcstrStarter)
   {
   CString csTempPath;

   GetTempPath(_MAX_PATH, csTempPath.GetBuffer(_MAX_PATH));
   csTempPath.ReleaseBuffer();   
   GetTempFileName(csTempPath, lpcstrStarter, 0, csTempFileName.GetBuffer(_MAX_PATH));
   csTempFileName.ReleaseBuffer();
   }

void CPhoto::DeleteCacheFiles()
   {
   int i;
   CImageCacheElement* pElement;   
   CFileStatus cfs;

   if (!pCache) return;

#ifndef USECACHE
   return NULL;
#endif

   try
	  {
	  // look through the cache for a matching CImage
	  for (i = pCache->coElements.GetSize() - 1; i >= 0; i--)
		 {		 
		 if (pElement = (CImageCacheElement*)pCache->coElements.GetAt(i))
			{
			// a match!
			if (pElement->Original_csFileName == GetOriginalImage()->csFileName &&
			    pElement->Original_ctFileTime == GetOriginalImage()->ctFileTime &&
			    pElement->Original_FileSize == GetOriginalImage()->FileSize)
			   {	   
			   // if the file exists, delete it
			   if (CFile::GetStatus(pElement->csFileName, cfs))
				  {
				  ::DeleteFile(pElement->csFileName);
				  }
			   // and remove the cache element			   
			   delete pElement;
			   pCache->coElements.RemoveAt(i);		
			   }
			}
		 }
	  }
   catch(...)
	  {
	  }
   }

static char cHex[] = { '0', '1', '2', '3', '4', 
  '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

void CPhotogizeUtils::Encrypt(LPCSTR lpcstrIn, CString& csOut)
   {      
   //Crypto(lpcstrIn, csOut, TRUE);
   int i;
   BYTE* pBuf;
   
   csOut.Empty();
   if (strlen(lpcstrIn) < 1) return;

   // encode it to a binary buffer
   if (!(pBuf = new BYTE[strlen(lpcstrIn) + 1])) return;
   m_bb.Reset();
   m_bb.SetPassword( csPassword );
   m_bb.SetMode(SECCryptoBlackBox::OFB );   
   m_bb.SetDir(SECCryptoBlackBox::ENC);
   strcpy((char*)pBuf, lpcstrIn);
   m_bb.TransformBuf((char*)pBuf, strlen(lpcstrIn));  

   // now do a bin2ascii
   char *p = csOut.GetBuffer(strlen(lpcstrIn) * 2);	
   BYTE* pIn = pBuf;
   memset(p, 0, strlen(lpcstrIn) * 2);	
   for (i = 0; i < strlen(lpcstrIn) * 2; i += 2)
	  {
	  *(p + i)= cHex[(*pIn & 240) >> 4];
	  *(p + i + 1) = cHex[*pIn & 15];	   	   
	  pIn++;
	  }		
   csOut.ReleaseBuffer();

   delete pBuf;
   }

void CPhotogizeUtils::Decrypt(LPCSTR lpcstrIn, CString& csOut)
   {         
   BYTE* pBuf;
   int i, j;   

   csOut.Empty();
   if (strlen(lpcstrIn) < 1) return;
   
   if (!(pBuf = new BYTE[strlen(lpcstrIn) + 1])) return;    
   ZeroMemory(pBuf, strlen(lpcstrIn) + 1);		
   const char* q = lpcstrIn;
	
	// ascii 2 bin
	for (i = 0, j = 0; i < strlen(lpcstrIn); i += 2, j++)
	   {
	   *(pBuf + j) = (HexLookup(*q) << 4) + HexLookup(*(q + 1));
	   q += 2;
	   }	   

   m_bb.Reset();
   m_bb.SetPassword( csPassword );
   m_bb.SetMode(SECCryptoBlackBox::OFB );   
   m_bb.SetDir(SECCryptoBlackBox::DEC);   
   m_bb.TransformBuf((char*)pBuf, strlen(lpcstrIn) / 2);

   csOut = pBuf;

   delete pBuf;
   }

int CPhotogizeUtils::HexLookup(char c)
   {
   int i;
   int iRet = 0;
   
   for (i = 0; i < 16; i++)
	  {
	  if (c == cHex[i])
		 {
		 iRet = i;
		 break;
		 }
	  }
   return iRet;
   }

void CPhotogizeUtils::Crypto(LPCSTR lpcstrIn, CString& csOut, BOOL bEncrypt)
   {         
   BYTE* pBuf;
   
   csOut.Empty();	
   if (strlen(lpcstrIn) < 1) return;

   pBuf = new BYTE[strlen(lpcstrIn) + 1];
   m_bb.Reset();
   m_bb.SetPassword( csPassword );
   m_bb.SetMode(SECCryptoBlackBox::OFB );   
   m_bb.SetDir( bEncrypt ? (SECCryptoBlackBox::ENC) : (SECCryptoBlackBox::DEC));
   strcpy((char*)pBuf, lpcstrIn);
   m_bb.TransformBuf((char*)pBuf, strlen((const char*)pBuf));

   csOut = pBuf;

   delete pBuf;
   }

void CPhotogizeUtils::GetDollarString(CString& cs, int iPricePennies, BOOL bDoNeg)
   {		   
   if (bDoNeg)
      cs.Format("(%.2f)", (float)iPricePennies / 100);
   else
	  cs.Format("%.2f", (float)iPricePennies / 100);
   }

void CPhotogizeUtils::GetPricePennies(CString& cs, int& iPricePennies)
   {		   
   float f = atof(cs);
   f *= 100;
   //iPricePennies = (int)f;
   iPricePennies = (int)(f + .5F);   
   }

BOOL CPhotogizeUtils::FolderSelect(HWND hOwner, int iPrompt, CString& csFolder)
   {
   CString csTemp;

   csTemp.LoadString(iPrompt);
   
   return FolderSelect(hOwner, csTemp, csFolder);
   }

const char* pInitFolder;

INT CALLBACK BrowseCallbackProc(HWND hwnd, 
                                UINT uMsg,
                                LPARAM lp, 
                                LPARAM pData) 
   {
   switch(uMsg) 
	  {
	  case BFFM_INITIALIZED: 												
		   SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)pInitFolder);
		   break;
	  }
   return 0;
   }


// This is the recommended way to select a directory
// in Win95 and NT4.    
BOOL CPhotogizeUtils::FolderSelect(HWND hOwner, LPCSTR lpcstrPrompt, CString& csFolder)
   {
   BOOL bRet;
   BROWSEINFO bi;

   memset((LPVOID)&bi, 0, sizeof(bi));
   TCHAR szDisplayName[_MAX_PATH];	  
   szDisplayName[0] = '\0';
   bi.hwndOwner = hOwner;
   bi.pidlRoot = NULL;
   bi.pszDisplayName = szDisplayName;
   bi.lpszTitle = lpcstrPrompt;
   bi.ulFlags = BIF_RETURNONLYFSDIRS;
   bi.lpfn = BrowseCallbackProc;

   pInitFolder = csFolder;

   
   LPITEMIDLIST pIIL = ::SHBrowseForFolder(&bi);
   TCHAR szInitialDir[_MAX_PATH];
   strcpy(szInitialDir, csFolder);
   
   bRet = ::SHGetPathFromIDList(pIIL, (char*)&szInitialDir);
   
   if (bRet)
	  {
	  if (szInitialDir != _T(""))
		 {	 
		 csFolder = szInitialDir;		 
		 }
	  else
		 bRet = FALSE;		 
	  LPMALLOC pMalloc;
	  HRESULT hr = SHGetMalloc(&pMalloc);
	  pMalloc->Free(pIIL);
	  pMalloc->Release();
	  }
   return bRet;
   }   

int CPhotogizeUtils::TimeStringToTimeInt(CString& csTime)
   {
   CString csYear;
   CString csMonth;
   CString csDay;
   CString csHour;
   CString csMin;
   CString csSec;  
   CTime t;
   
   // must be 19 chars in length, if it isn't return
   // current time
   if (csTime.GetLength() != 19)
	  {
	  t = CTime::GetCurrentTime();
	  return t.GetTime();
	  }

   // 2001-03-15T13:45:10 for 1:45 PM on March 15, 2001
   csYear = csTime.Mid(0, 4);
   csMonth = csTime.Mid(5, 2);
   csDay = csTime.Mid(8, 2);
   csHour = csTime.Mid(11, 2);
   csMin = csTime.Mid(14, 2);
   csSec = csTime.Mid(17, 2);   

   // construct a time object
   CTime ct(atoi(csYear), atoi(csMonth), atoi(csDay), 
	  atoi(csHour), atoi(csMin), atoi(csSec));

   return ct.GetTime();
   }		

CShipListEntry::CShipListEntry()
   {
   csName.Empty();
   iBaseHandlingPricePennies = 0;
   iNumBreaks = 1;
   for (int i = 0; i < MAXSHIPPRICEMATRIX; i++)
	  {
	  iQtyFrom[i] = 0;
	  iQtyTo[i] = 16;
	  iPricePennies[i] = 0;
	  }
   }

CShipListEntry::~CShipListEntry()
   {
   }

BOOL CShipListEntry::operator==(const CShipListEntry& sle)
   {
   // is same thing?
   return (IsSame(sle));
   }
					  
BOOL CShipListEntry::operator!=(const CShipListEntry& sle)
   {
   // is same thing?
   return (!IsSame(sle));
   }

BOOL CShipListEntry::IsSame(const CShipListEntry& sle)
   {               
   // are they the same?
   if (csName == sle.csName &&
	  iNumBreaks == sle.iNumBreaks &&
	  iBaseHandlingPricePennies == sle.iBaseHandlingPricePennies &&
	  (memcmp(iQtyFrom, sle.iQtyFrom, sizeof(int) * iNumBreaks) == 0) &&
	  (memcmp(iQtyTo, sle.iQtyTo, sizeof(int) * iNumBreaks) == 0) &&
	  (memcmp(iPricePennies, sle.iPricePennies, sizeof(int) * iNumBreaks) == 0))
	  return TRUE;
   else
	  return FALSE;
   }

const CShipListEntry& CShipListEntry::operator=(const CShipListEntry& sleSrc) 
   {
   int i;

   // assignment
   if (this != &sleSrc)
      {   
	  // copy the props
	  csName = sleSrc.csName;
	  iNumBreaks = sleSrc.iNumBreaks;
	  iBaseHandlingPricePennies = sleSrc.iBaseHandlingPricePennies;
	  for (i = 0; i < MAXSHIPPRICEMATRIX; i++)
		 {
		 iQtyFrom[i] = sleSrc.iQtyFrom[i];
		 iQtyTo[i] = sleSrc.iQtyTo[i];
		 iPricePennies[i] = sleSrc.iPricePennies[i];
		 }
	  }
   return *this;   
   }			   
					  
void CPhotogizeUtils::WriteShipList(CObArray& coShipList)
   {   
   CString csFileName;
   CFile cf;   
   CString csAppPath;
   
   CPhotogizeUtils::GetAppPath(csAppPath);      

   // make filename   
   csFileName.Format("%sphotogize_shiplist.dat", csAppPath);    
   
   // open the cache and read the contents
   if (cf.Open(csFileName, CFile::modeCreate | CFile::modeWrite))
	  {
	  try
		 {
		 CArchive ar(&cf, CArchive::store);
         coShipList.Serialize(ar);
		 }
      catch(CException* theException)
		 {      
		 // oops
		 theException->Delete();
		 }
	  cf.Close();
	  }			    
   }

void CPhotogizeUtils::GetLegacyShipList(CObArray& coShipList)
   {
   CString csFileName;
   CFile cf;   
   CString csAppPath;
   
   CPhotogizeUtils::GetAppPath(csAppPath);      

   ClearShipList(coShipList);

   // make filename   
   csFileName.Format("%sphotogize_shiplist.dat", csAppPath);    

   if (cf.Open(csFileName, CFile::modeRead))
	  {
	  try
		 {
		 CArchive ar(&cf, CArchive::load);
         coShipList.Serialize(ar);
		 }						   
      catch(CException* theException)
		 {      
		 // oops
		 theException->Delete();
		 }
	  cf.Close();
	  }			 
   }

void CPhotogizeUtils::ClearShipList(CObArray& coShipList)
   {
   int i;
   CShipListEntry* pShipListEntry;

   // remove the existing ship list entries
   i = 0;
   while (i < coShipList.GetSize() )
      {
	  if (pShipListEntry = (CShipListEntry*)coShipList.GetAt(i++))
		 {							  
		 if (pShipListEntry) delete pShipListEntry;			
		 }
      }     
   coShipList.RemoveAll();
   }

CProductCount::CProductCount()
   {
   pProduct = NULL;
   iCopies = 0;
   iUnitPricePennies = 0;
   iSubTotalPennies = 0;		
   iWeightOz100 = 0;
   }

CProductCount::~CProductCount()
   {
   }

void COrder::GenerateProductCountArrayONLINE(CNode* pParentProduct)
   {   
//   int i;   
//   int j;
//   CProduct* pProduct;   
//   COrderItem* pOrderItem;   
//   CProductCount* pProductCount;
//   CProductCount* pProductCountMatch;
   BOOL bMatch = FALSE;

   ResetProductCountArray();

   if (pParentProduct->coNodes.GetSize() < 1) return;
/*
   for (i = 0; i < OrderDetails.coOrderItemArray.GetSize(); i++)
	  {
	  if (pOrderItem = (COrderItem*)OrderDetails.coOrderItemArray.GetAt(i))
		 {
		 // if the product doesn't exist, use the first avail product
		 if (!(pProduct = GetProduct(pOrderItem->csName, pParentProduct)))
			pProduct = (CProduct*)pParentProduct->coNodes.GetAt(0);		 

		 // do we have an existing product?
		 for (j = 0, bMatch=FALSE; j < coProductArray.GetSize(); j++)
			{
			if (pProductCountMatch = (CProductCount*)coProductArray.GetAt(j))
			   {
			   if (pProductCountMatch->csName == pOrderItem->csName)
				  {
				  bMatch = TRUE;
				  break;
				  }
			   }
			}
		 // if a match, just increment, otherwise create a new product count
		 if (bMatch)
			{
			pProductCountMatch->iCopies += pOrderItem->iQty;
			}
		 else
			{		 
			if (pProductCount = new CProductCount)
			   {
			   pProductCount->csName = pOrderItem->csName;			   
			   pProductCount->csDescription = pOrderItem->csDescription;			
			   pProductCount->iCopies = pOrderItem->iQty;

			   if (pOrderItem->bUseDiscountedPrice)
				  {
				  pProductCount->iUnitPricePennies = pOrderItem->iUnitDiscount;
			      pProductCount->iSubTotalPennies = pOrderItem->iUnitDiscount * pOrderItem->iQty;			
				  }
			   else
				  {
				  pProductCount->iUnitPricePennies = pOrderItem->iUnitPrice;
			      pProductCount->iSubTotalPennies = pOrderItem->iUnitPrice * pOrderItem->iQty;			
				  }
			   if (pProduct)
				  {
				  pProductCount->pProduct = pProduct;
				  pProductCount->iWeightOz100 = pProduct->iWeightOunces100;
				  }
			   coProductArray.Add((CObject*)pProductCount);
			   }
			}
		 }
	  }
*/
   }

CEmailTriggerRule::CEmailTriggerRule()
   {
   iRule = EMAILTRIGGER_HARVESTED;
   bEnable = FALSE;
   bConfirm = FALSE;
   }

CEmailTriggerRule::~CEmailTriggerRule()
   {
   }

void CEmailTriggerRule::Serialize(CArchive& ar)
   {           
   try
	  {
	  if (ar.IsStoring())
		 {
		 ar << bEnable << iRule << csSendTo << csFrom << csCc << csSubject << csMsg << bConfirm;
		 }
	  else
		 {
		 ar >> bEnable >> iRule >> csSendTo >> csFrom >> csCc >> csSubject >> csMsg >> bConfirm;
		 }
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   }

const CEmailTriggerRule& CEmailTriggerRule::operator=(const CEmailTriggerRule& cpSrc) 
   {
   if (this != &cpSrc)
      {   
	  // copy the props
	  bEnable = cpSrc.bEnable;
	  iRule = cpSrc.iRule;
	  csSendTo = cpSrc.csSendTo;
	  csFrom = cpSrc.csFrom;
	  csCc = cpSrc.csCc;
	  csSubject = cpSrc.csSubject;
	  csMsg = cpSrc.csMsg;
	  bConfirm = cpSrc.bConfirm;
	  }
   return *this;   
   }

void CPhotogizeUtils::WriteEmailTriggers(CEmailTriggerRule EmailTriggerList[])
   {   
   CString csFileName;
   CFile cf;   
   CString csAppPath;
   int i;
   
   CPhotogizeUtils::GetAppPath(csAppPath);      

   // make filename   
   csFileName.Format("%sphotogize_emailtriggerlist.dat", csAppPath);    
   
   // open the cache and read the contents
   if (cf.Open(csFileName, CFile::modeCreate | CFile::modeWrite))
	  {
	  try
		 {
		 CArchive ar(&cf, CArchive::store);
		 for (i = 0; i < EMAILTRIGGER_NUMTRIGGERS; i++)
            EmailTriggerList[i].Serialize(ar);
		 }
      catch(CException* theException)
		 {      
		 // oops
		 theException->Delete();
		 }
	  cf.Close();
	  }			    
   }

void CPhotogizeUtils::GetEmailTriggers(CEmailTriggerRule EmailTriggerList[])
   {
   CString csFileName;
   CFile cf;   
   CString csAppPath;
   int i;
   
   CPhotogizeUtils::GetAppPath(csAppPath);      

   // make filename   
   csFileName.Format("%sphotogize_emailtriggerlist.dat", csAppPath);    

   if (cf.Open(csFileName, CFile::modeRead))
	  {
	  try
		 {
		 CArchive ar(&cf, CArchive::load);
		 for (i = 0; i < EMAILTRIGGER_NUMTRIGGERS; i++)
            EmailTriggerList[i].Serialize(ar);
		 }						   
      catch(CException* theException)
		 {      
		 // oops
		 theException->Delete();
		 }
	  cf.Close();
	  }			 
   }

void CPhotogizeUtils::RecursiveDelete(LPCSTR lpcstrPath)
   {
   CFileFind ff;   

   // sanity
   if (strlen(lpcstrPath) < 2) return;
   
   CString path = lpcstrPath;
   
   if (path.Right(1) != "\\") path += "\\";
   path += "*.*";
   BOOL res = ff.FindFile(path);
   
   while(res)
	  {
	  res = ff.FindNextFile();
	  if (ff.IsDots())
		 continue;
	  if (ff.IsDirectory())
		 {
		 path = ff.GetFilePath();
		 RecursiveDelete(path);
		 RemoveDirectory(path);
		 }
	  else
		 {
		 // make sure that we don't have a read only file
		 ::SetFileAttributes(ff.GetFilePath(), FILE_ATTRIBUTE_ARCHIVE);
		 // if the file is read-only, reset the atts and try to delete again
		 ::DeleteFile(ff.GetFilePath());
		 }
	  }
   }

CAccount::CAccount()
   {
   iType = NODETYPE_ACCOUNT;   
   iSubType = SUBTYPE_MASTERACCT;
   hItemOrderRoot = NULL;
   hItemOrderArchiveRoot = NULL;
   hItemProductRoot = NULL;
   pParentOrder = NULL;
   pParentOrderArchive = NULL;
   pParentProduct = NULL;   
   bIsDefault = FALSE;

   iLastHarvestTime = 0;
   iLastXHarvestTime = 0;
   iLastPrintTime = 0;   

   m_use_visa = 1;
   m_use_mc = 1;  
   m_use_amex = 1;
   m_use_dclub = 1;
   m_use_disco = 1;
   m_ccrule = 0;   
   m_securitycode = FALSE;

   m_addtax = FALSE;		 
   m_taxpercent = 0.0F;
   m_addtaxship = FALSE;	 

   m_autoharvest = FALSE;
   m_autoprint = FALSE;
   m_moveorders = FALSE;
   m_orderdataonly = FALSE;
   m_cconinvoice = TRUE;
   m_completeonarchive = FALSE;
   m_timepoll = 10;
   m_timeprint = 10;
   m_timepollunits = 1;
   m_autoprintinvoice = FALSE;
   m_timeprintunits = 1;
//   m_makechanges = PROXY_CHANGES_IMMEDIATELY;  
   QueuedChanges = FALSE;
   iNextUpdate = 0;

//   m_port = PROXY_DEFAULT_PORT;
//   m_sslport = PROXY_DEFAULTSSL_PORT;
//   m_folder = PROXY_DEFAULT_FOLDER;		 
//   m_host = PROXY_DEFAULT_HOST;
//   m_group = PROXY_DEFAULT_GROUP;
   m_proxy_use = FALSE;
   m_proxy_address = "";
   m_proxy_port = 80;   
   m_proxy_name = "";	
//   m_url = PROXY_DEFAULT_URL;

   m_usewebservices = TRUE;

   m_printrule = 0;
   m_restrictcopies = FALSE;
   m_restrictcopies_num = 99;
   m_restrictvalue = FALSE;		 
   CURRENCY cur;
   ZeroMemory(&cur, sizeof CURRENCY);
   cur.Lo = 9900 * 100;
   m_restrictvalue_num = cur;

   // get logo data
   CString csDefLogoFile;
   CPhotogizeUtils::GetAppPath(csDefLogoFile);
   csDefLogoFile += DEFAULTALBUMLOGO;
   m_album_logo = csDefLogoFile;
   m_album_logo_link = "";

   CPhotogizeUtils::GetAppPath(csDefLogoFile);
   csDefLogoFile += DEFAULTWEBLEFTLOGO;
   m_weblogoleft = csDefLogoFile;
   m_weblogoleft_link = "";

   CPhotogizeUtils::GetAppPath(csDefLogoFile);
   csDefLogoFile += DEFAULTWEBRIGHTLOGO;
   m_weblogoright = csDefLogoFile;
   m_weblogoright_link = "";

   CPhotogizeUtils::GetAppPath(csDefLogoFile);
   csDefLogoFile += DEFAULTLEFTINDEXLOGO;
   m_leftindex_logo = csDefLogoFile;

   CPhotogizeUtils::GetAppPath(csDefLogoFile);
   csDefLogoFile += DEFAULTRIGHTINDEXLOGO;
   m_rightindex_logo = csDefLogoFile;      

   CPhotogizeUtils::GetAppPath(csDefLogoFile);
   csDefLogoFile += DEFAULTALBUMLOGO;
   m_invoice_logo = csDefLogoFile;   
   
   CPhotogizeUtils::GetAppPath(m_outputfolder);
   m_outputfolder += "PrintJobs\\";

   bIsExpanded_Orders = FALSE;
   bIsExpanded_ArchivedOrders = FALSE;
   bIsExpanded_Products = FALSE;

   bEnableXConnect = FALSE;
   csXConnectPath.Empty();   
   bDelXConnectPhotos = FALSE;
   csChannel.Empty();
   XConnect_Interval = XCONNECT_DEFAULT_INT;
   XConnect_Units = XCONNECT_UNITS_MINUTES;

   CPhotogizeUtils::GetAppPath(m_outputfolder);
   m_archivefolder += "OrderArchive\\";
   m_orderarchivehousekeeping_days = 6;
   m_orderarchivehousekeeping_method = 0;
   m_orderarchivehousekeeping = FALSE;

   InvoiceBitmapRenderTime = 0;   
   InvoiceBitmapRenderSize = 0;
   InvoiceBitmapRenderHeight = 0;
   InvoiceBitmapRenderWidth = 0;

   m_useharvestrules = USE_HARVESTRULES_NONE;

   bCDoffer = FALSE;   
   CDPrice = COleCurrency(0, 0);
   iCDWeightOunces100 = 0;
   csCDDescription = "Put all these photos on a CD";
   CDOptions = CDOPTION_HTMLINDEX | CDOPTION_AUTOSTART;
   CD_m_copyfolder = FALSE;
   CD_m_originals = TRUE;   
   CD_m_samples = FALSE;
   CD_m_sampledims = 0;   
   CD_m_index = FALSE;
   CD_m_autostart = FALSE;
   m_lockacctsettings = TRUE;
   CD_m_prejudge = FALSE;

   CTime ct(2002, 12, 12, 0, 0, 0);
   m_change_time = ct;   
   m_user = csName = csDisplayName = "Untitled";
   
   XConnect_Message = "Your photos have been placed in an online album.";
   XConnect_Subject = "Your photos";

   m_promo_textsize = 1;
   m_promo_textcolor = 0;
   m_promo_msg  = "[PROMODESC]\r\n\r\nSimply enter [PROMOCODE] in the Coupon Code field the next time you place an online order.";
   m_promo_left = 3.0;
   m_promo_right = 5.5;
   m_promo_top = 2.5;
   m_promo_bottom = 3.5;
   m_promo_typeface_str = "Arial";   

   m_promo_justifytxt = 0;
   m_promo_product = 0;
   m_promo_addtoorder = 0;   

   m_printindexbydefault = TRUE;

   ctUnused = 0;
   uiLastAcctModTime = 0;
   uiLastAcctModTimeProps = 0;

   pParentOrder = NULL;
   pParentOrderArchive = NULL;
   pParentProduct = NULL;
   }

CAccount::~CAccount()
   {
   CPromo* pPromo;
   int i;
   CPhotogizeUtils utils;
   CMasterAccountHarvestRuleItem* pMasterAccountHarvestRuleItem;
   
   // clear the ship list
   utils.ClearShipList(coShipList);
   // clear the harvest rule list
   utils.ClearHarvestRuleList(coHarvestRules);

   // must delete these if they exist
   if (pParentOrder) delete pParentOrder;
   if (pParentOrderArchive) delete pParentOrderArchive;
   if (pParentProduct) delete pParentProduct;

   // first delete the existing promo array
   for (i = coPromotions.GetSize() - 1; i >= 0; i--)
	  {
	  if (pPromo = (CPromo*)coPromotions.GetAt(i))
		 {
		 delete pPromo;
		 }
	  }
   coPromotions.RemoveAll();   

   for (i = coMasterAccountHarvestRuleSet.GetSize() - 1; i >= 0; i--)
	  {
	  if (pMasterAccountHarvestRuleItem = (CMasterAccountHarvestRuleItem*)coMasterAccountHarvestRuleSet.GetAt(i))
		 {
		 delete pMasterAccountHarvestRuleItem;
		 }
	  }      
   coMasterAccountHarvestRuleSet.RemoveAll();
   }

void CAccount::Serialize(CArchive& ar)
   {           
   CString csEncoded;	  
   CPhotogizeUtils utils;
   int i;

   try
	  {
	  if (ar.IsStoring())
		 {
		 ar << m_user
			<< m_url << m_folder << m_group << m_host << m_proxy_use << m_proxy_address << m_proxy_port
			<< m_proxy_name << m_port << m_sslport << m_change_time << m_makechanges << QueuedChanges
			<< iNextUpdate;

		 utils.Encrypt(m_password, csEncoded);	  
		 ar << csEncoded;      
		 utils.Encrypt(m_proxy_password, csEncoded);	  
		 ar << csEncoded;      

		 ar << m_autoharvest << m_autoprint << m_moveorders << m_timepoll << m_timeprint
			<< m_timepollunits << m_timeprintunits << m_printrule << m_restrictcopies
			<< m_restrictcopies_num << m_restrictvalue << m_restrictvalue_num 
			<< m_indexproduct_name << m_usewebservices << m_autoprintinvoice;			

		 ar << m_album_logo << m_album_logo_link << m_weblogoleft << m_weblogoleft_link
			<< m_weblogoright << m_weblogoright_link << m_leftindex_logo << m_rightindex_logo;		  

		 csaInclude.Serialize(ar);
		 csaExclude.Serialize(ar);

		 csaPickupLocations.Serialize(ar);
		 csaTaxStates.Serialize(ar);
		 coShipList.Serialize(ar);
      
		 ar << m_addtax << m_taxpercent << m_addtaxship;

		 ar << m_use_visa << m_use_mc << m_use_amex << m_use_dclub << m_use_disco << m_ccrule;

		 for (i = 0; i < EMAILTRIGGER_NUMTRIGGERS; i++)
			EmailTriggerList[i].Serialize(ar);

		 ar << bIsExpanded_Orders << bIsExpanded_ArchivedOrders << bIsExpanded_Products;
		 ar << bEnableXConnect << csXConnectPath << XConnect_Interval << XConnect_Units;
		 ar << bIsDefault;

		 ar << m_useharvestrules;						  
		 coHarvestRules.Serialize(ar);	 	  
		 ar << bCDoffer << CDPrice << iCDWeightOunces100 << csCDDescription << CDOptions;
		 ar << CD_m_copyfolder << CD_m_copyfoldername << CD_m_copyfolderdest << CD_m_originals;	  
		 ar << m_unused;
		 ar << CD_m_samples << CD_m_sampledims << CD_m_samplefolder 
			<< CD_m_index << CD_m_autostart;
		 ar << XConnect_Message << XConnect_Subject;
		 ar << csChannel;
		 ar << m_orderdataonly << m_cconinvoice << m_invoice_logo;
		 ar << m_lockacctsettings;

		 coPromotions.Serialize(ar);

		 ar << m_securitycode;

         ar << m_profile_biz_name << m_profile_tech_name << m_profile_biz_email <<
			m_profile_tech_email << m_profile_biz_phone << m_profile_tech_phone <<
			m_profile_company << m_profile_address << m_profile_comments <<
			m_profile_fax << m_profile_website;

         ar << m_promo_textsize << m_promo_textcolor << m_promo_msg <<
			m_promo_left << m_promo_right << m_promo_top <<
			m_promo_bottom << m_promo_typeface_str << m_promo_template <<
			m_promo_product << m_promo_addtoorder << m_promo_justifytxt <<
			m_promo_promocode << m_printindexbydefault << ctUnused << 
			uiLastAcctModTime;
		 ar << m_completeonarchive;
		 ar << uiLastAcctModTimeProps;
         ar << m_profile_lab_phone << m_profile_lab_email << m_profile_lab_name; 
		 ar << CD_m_prejudge; 
		 ar << bDelXConnectPhotos;
		 ar << m_outputfolder;
		 ar << csStoreID;
		 coMasterAccountHarvestRuleSet.Serialize(ar);
		 ar << m_archivefolder << m_orderarchivehousekeeping_days << m_orderarchivehousekeeping_method 
			<< m_orderarchivehousekeeping;
		 ar <<InvoiceBitmapRenderTime << InvoiceBitmapRenderName << InvoiceBitmapRenderSize 
			<< InvoiceBitmapRenderHeight << InvoiceBitmapRenderWidth;
		 }
	  else
		 {
		 int nVersion = ar.GetObjectSchema();

		 ar >> m_user
			>> m_url >> m_folder >> m_group >> m_host >> m_proxy_use >> m_proxy_address >> m_proxy_port
			>> m_proxy_name >> m_port >> m_sslport >> m_change_time >> m_makechanges >> QueuedChanges
			>> iNextUpdate;
		 
		 ar >> csEncoded;
		 utils.Decrypt(csEncoded, m_password);	  
		 ar >> csEncoded;
		 utils.Decrypt(csEncoded, m_proxy_password);	  

		 ar >> m_autoharvest >> m_autoprint >> m_moveorders >> m_timepoll >> m_timeprint
			>> m_timepollunits >> m_timeprintunits >> m_printrule >> m_restrictcopies
			>> m_restrictcopies_num >> m_restrictvalue >> m_restrictvalue_num
			>> m_indexproduct_name >> m_usewebservices >> m_autoprintinvoice;

		 ar >> m_album_logo >> m_album_logo_link >> m_weblogoleft >> m_weblogoleft_link
			>> m_weblogoright >> m_weblogoright_link >> m_leftindex_logo >> m_rightindex_logo;

		 csaInclude.Serialize(ar);
		 csaExclude.Serialize(ar);

		 csaPickupLocations.Serialize(ar);
		 csaTaxStates.Serialize(ar);
		 coShipList.Serialize(ar);
      
		 ar >> m_addtax >> m_taxpercent >> m_addtaxship;

		 ar >> m_use_visa >> m_use_mc >> m_use_amex >> m_use_dclub >> m_use_disco >> m_ccrule;

		 for (i = 0; i < EMAILTRIGGER_NUMTRIGGERS; i++)
			EmailTriggerList[i].Serialize(ar);

		 ar >> bIsExpanded_Orders >> bIsExpanded_ArchivedOrders >> bIsExpanded_Products;

  		 if (nVersion >= 1)
			{					
			ar >> bEnableXConnect >> csXConnectPath >> XConnect_Interval >> XConnect_Units;
			}
  		 if (nVersion >= 2)
			{					
			ar >> bIsDefault;
			}
  		 if (nVersion >= 3)
			{					
			ar >> m_useharvestrules;
			coHarvestRules.Serialize(ar);
			}
  		 if (nVersion >= 4)
			{					
			ar >> bCDoffer >> CDPrice >> iCDWeightOunces100 >> csCDDescription >> CDOptions;
			ar >> CD_m_copyfolder >> CD_m_copyfoldername >> CD_m_copyfolderdest;
			ar >> CD_m_originals;
			ar >> m_unused; 
			}	  
		 if (nVersion >= 5)
			{
			ar >> CD_m_samples >> CD_m_sampledims >> CD_m_samplefolder 
			   >> CD_m_index >> CD_m_autostart;
			}															
		 if (nVersion >= 6)
			{
			ar >> XConnect_Message >> XConnect_Subject;
			}																	 
		 if (nVersion >= 7)			
			ar >> csChannel;																	 
		 if (nVersion >= 8)			
			{
			ar >> m_orderdataonly >> m_cconinvoice;
			}
		 if (nVersion >= 9)			
			ar >> m_invoice_logo;
		 if (nVersion >= 10)			
			ar >>m_lockacctsettings;
		 if (nVersion >= 11)			
			{
			coPromotions.Serialize(ar);
			ar >> m_securitycode;
			
			ar >> m_profile_biz_name >> m_profile_tech_name >> m_profile_biz_email >>
			   m_profile_tech_email >> m_profile_biz_phone >> m_profile_tech_phone >>
			   m_profile_company >> m_profile_address >> m_profile_comments >>
			   m_profile_fax >> m_profile_website;
			}	
		 if (nVersion >= 12)
			{
			ar >> m_promo_textsize >> m_promo_textcolor >> m_promo_msg >>
			m_promo_left >> m_promo_right >> m_promo_top >>
			m_promo_bottom >> m_promo_typeface_str >> m_promo_template >>
			m_promo_product >> m_promo_addtoorder >> m_promo_justifytxt >>
			m_promo_promocode;
			}
		 if (nVersion >= 13)
			{
		    ar >> m_printindexbydefault;
			}
		 if (nVersion >= 14)
			{
		    ar >> ctUnused;
			}		 
		 if (nVersion >= 15)
			{
		    ar >> uiLastAcctModTime;
			}		 	 
		 else
			{			
			// we removed seconds from time poll units - adjust here for versions < 15
			if (m_timepollunits == 1) 
			   m_timepollunits = 0;
			else if (m_timepollunits == 2) 
			   m_timepollunits = 1;
			else
			   m_timepollunits = 0;
			}  
		 // these versions used only minutes and hours for xconnect, we're now adding back in seconds
		 if (nVersion > 15 && nVersion < 18)
			{
			if (XConnect_Units == 0) 
			   XConnect_Units = 1;
			else if (XConnect_Units == 1) 
			   XConnect_Units = 2;
			else
			   XConnect_Units = 0;
			}
		 if (nVersion >= 16)
			ar >> m_completeonarchive;
		 if (nVersion >= 17)
			ar >> uiLastAcctModTimeProps;
		 if (nVersion >= 18)
			{
			ar >> m_profile_lab_phone >> m_profile_lab_email >> m_profile_lab_name;
			}
		 if (nVersion >= 19)		
			ar >> CD_m_prejudge; 			
		 if (nVersion >= 20)			
			ar >> bDelXConnectPhotos; 			
		 if (nVersion >= 21)			
			ar >> m_outputfolder; 					 
		 if (nVersion >= 22)			
			ar >> csStoreID;
		 if (nVersion >= 23)			
			coMasterAccountHarvestRuleSet.Serialize(ar);
		 if (nVersion >= 24)
			{
			ar >> m_archivefolder >> m_orderarchivehousekeeping_days >> m_orderarchivehousekeeping_method 
			   >> m_orderarchivehousekeeping;
			ar >> InvoiceBitmapRenderTime >> InvoiceBitmapRenderName >> InvoiceBitmapRenderSize
			   >> InvoiceBitmapRenderHeight >> InvoiceBitmapRenderWidth;
			}

		 // force to zero on load just to be sure
		 uiLastAcctModTime = 0;
		 uiLastAcctModTimeProps = 0;
		 
		 // self reference
		 pAccountOwner = this;
		 }
	  // call the base class   
	  CNode::Serialize(ar);	

	  // make sure that the output folder gets created	     
	  CString csNewFolder;
	  try
		 {	     
	     csNewFolder = m_outputfolder;
	     VERIFYPATH(csNewFolder);
	     // create the output folder if needed   
	     ::CreateDirectory(csNewFolder, NULL);
		 }
	  catch(...)
		 {
		 }
      }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   }

const CAccount& CAccount::operator=(const CAccount& cpSrc) 
   {
   // when we copy an account, we copy everything EXCEPT: Orders
   // node pointers, and list handles.  Products ARE copies but
   // the caller is responsible for inserting the newly instantiated
   // products pointer into the list
   // assignment
   if (this != &cpSrc)
      {   
	  // copy the props	  
	  csDisplayName = cpSrc.csDisplayName;
	  csName = cpSrc.csName;
	  bIsDefault = cpSrc.bIsDefault;
	  
	  m_user = cpSrc.m_user;
	  m_url = cpSrc.m_url;
	  m_folder = cpSrc.m_folder;
	  m_group = cpSrc.m_group;
	  m_host = cpSrc.m_host;
	  m_password = cpSrc.m_password;
	  m_proxy_use = cpSrc.m_proxy_use;
	  m_proxy_address = cpSrc.m_proxy_address;
	  m_proxy_port = cpSrc.m_proxy_port;
	  m_proxy_name = cpSrc.m_proxy_name;
	  m_proxy_password = cpSrc.m_proxy_password;
	  m_port = cpSrc.m_port;
	  m_sslport = cpSrc.m_sslport;	
	  iNextUpdate = cpSrc.iNextUpdate;
	  m_makechanges = cpSrc.m_makechanges;
	  QueuedChanges = cpSrc.QueuedChanges;
	  m_change_time = cpSrc.m_change_time;

	  m_usewebservices = cpSrc.m_usewebservices;

      uiLastAcctModTime = cpSrc.uiLastAcctModTime;
	  uiLastAcctModTimeProps = cpSrc.uiLastAcctModTimeProps;

	  // copy order settings
	  CopyOrderSettings(cpSrc);

	  // copy logos
	  CopyLogos(cpSrc);

	  // copy biz rules
	  CopyBizRules(cpSrc);

	  // copy email triggers
	  CopyEmailTriggers(cpSrc);

	  // copy profile
	  CopyProfile(cpSrc);

	  // copy CD settings
	  CopyCDSettings(cpSrc);	  

	  // copy promotions
	  CopyPromotions(cpSrc);

	  // copy promotio text settings
	  CopyPromoText(cpSrc);

	  m_lockacctsettings = cpSrc.m_lockacctsettings;


	  // copy products only if there is an existing parentproduct
	  if (pParentProduct) CopyProducts(cpSrc);	  

      bIsExpanded_Orders = cpSrc.bIsExpanded_Orders;
      bIsExpanded_ArchivedOrders = cpSrc.bIsExpanded_ArchivedOrders;
      bIsExpanded_Products = cpSrc.bIsExpanded_Products;
	  
	  bEnableXConnect = cpSrc.bEnableXConnect;
      csXConnectPath = cpSrc.csXConnectPath;
	  bDelXConnectPhotos = cpSrc.bDelXConnectPhotos;
	  m_outputfolder = cpSrc.m_outputfolder;
      XConnect_Interval = cpSrc.XConnect_Interval;
      XConnect_Units = cpSrc.XConnect_Units;	  

	  csChannel = cpSrc.csChannel;

	  csStoreID = cpSrc.csStoreID;

	  // copy the harvest rules
	  CopyHarvestRules(cpSrc);
	  CopyMasterHarvestRules(cpSrc);

	  //
	  // don't copy orders
	  //

	  // call the base class
	  CNode::operator =(cpSrc);
	  }
   return *this;   
   }

void CAccount::CopyProducts(const CAccount& AccountSrc)
   {
   int i;
   CProduct* pProduct;
   CProduct* pProductNew;

   // remove the existing products
   i = 0;
   while (i < pParentProduct->coNodes.GetSize())
	  {
	  if (pProduct = (CProduct*)pParentProduct->coNodes.GetAt(i++))		 		 
		 {							  
		 if (pProduct) delete pProduct;			
		 }
	  }     
   pParentProduct->coNodes.RemoveAll();	  

   // copy the src products
   for (i = 0; i < AccountSrc.pParentProduct->coNodes.GetSize(); i++)
	  {
	  if (pProduct = (CProduct*)AccountSrc.pParentProduct->coNodes.GetAt(i))
		 {
		 if (pProductNew = new CProduct)
			{
			*pProductNew = *pProduct;
			pProductNew->pAccountOwner = this;
			pParentProduct->coNodes.Add((CObject*)pProductNew);
			}
		 }
	  }
   }

void CAccount::CopyPromotions(const CAccount& AccountSrc)
   {
   int i;
   CPromo* pPromo;
   CPromo* pPromoNew;

   // remove the existing promos
   for (i = coPromotions.GetSize() - 1; i >= 0; i--)
	  {
	  if (pPromo = (CPromo*)coPromotions.GetAt(i))
		 {
		 delete pPromo;
		 }
	  }
   coPromotions.RemoveAll();	  

   // copy the src promos
   for (i = 0; i < AccountSrc.coPromotions.GetSize(); i++)
	  {
	  if (pPromo = (CPromo*)AccountSrc.coPromotions.GetAt(i))
		 {
		 if (pPromoNew = new CPromo)
			{
			*pPromoNew = *pPromo;			
			coPromotions.Add((CObject*)pPromoNew);
			}
		 }
	  }
   }

void CAccount::CopyBizRules(const CAccount& AccountSrc)
   {
   int i;
   CPhotogizeUtils utils;
   CShipListEntry* pShipListEntry;
   CShipListEntry* pNewShipListEntry;

   csaPickupLocations.RemoveAll();
   for (i = 0; i < AccountSrc.csaPickupLocations.GetSize(); i++)
	  csaPickupLocations.Add(AccountSrc.csaPickupLocations.GetAt(i));

   csaTaxStates.RemoveAll();
   for (i = 0; i < AccountSrc.csaTaxStates.GetSize(); i++)
	  csaTaxStates.Add(AccountSrc.csaTaxStates.GetAt(i));

   utils.ClearShipList(coShipList);
   for (i = 0; i < AccountSrc.coShipList.GetSize(); i++)
	  {
	  if (pShipListEntry = (CShipListEntry*)AccountSrc.coShipList.GetAt(i))
		 {
		 if (pNewShipListEntry = new CShipListEntry)
			{
			*pNewShipListEntry = *pShipListEntry;
			coShipList.Add((CObject*)pNewShipListEntry);
			}
		 }
	  }
   
   m_addtax = AccountSrc.m_addtax;
   m_taxpercent = AccountSrc.m_taxpercent;
   m_addtaxship = AccountSrc.m_addtaxship;

   m_use_visa = AccountSrc.m_use_visa;
   m_use_mc = AccountSrc.m_use_mc;
   m_use_amex = AccountSrc.m_use_amex;
   m_use_dclub = AccountSrc.m_use_dclub;
   m_use_disco = AccountSrc.m_use_disco;
   m_ccrule = AccountSrc.m_ccrule;
   m_securitycode = AccountSrc.m_securitycode;   
   }

void CAccount::CopyProfile(const CAccount& AccountSrc)
   {
   m_profile_biz_name = AccountSrc.m_profile_biz_name;
   m_profile_tech_name = AccountSrc.m_profile_tech_name;
   m_profile_biz_email  = AccountSrc.m_profile_biz_email;
   m_profile_tech_email = AccountSrc.m_profile_tech_email;
   m_profile_biz_phone = AccountSrc.m_profile_biz_phone;
   m_profile_tech_phone = AccountSrc.m_profile_tech_phone;
   m_profile_company = AccountSrc.m_profile_company;
   m_profile_address = AccountSrc.m_profile_address;
   m_profile_comments = AccountSrc.m_profile_comments;
   m_profile_fax = AccountSrc.m_profile_fax;
   m_profile_website = AccountSrc.m_profile_website;
   m_profile_lab_phone = AccountSrc.m_profile_lab_phone;
   m_profile_lab_email = AccountSrc.m_profile_lab_email;
   m_profile_lab_name = AccountSrc.m_profile_lab_name;
   }

void CAccount::CopyPromoText(const CAccount& AccountSrc)
   {
   m_promo_textsize = AccountSrc.m_promo_textsize;
   m_promo_textcolor = AccountSrc.m_promo_textcolor;
   m_promo_msg = AccountSrc.m_promo_msg;
   m_promo_left = AccountSrc.m_promo_left;
   m_promo_right = AccountSrc.m_promo_right;
   m_promo_top = AccountSrc.m_promo_top;
   m_promo_bottom = AccountSrc.m_promo_bottom;
   m_promo_typeface_str = AccountSrc.m_promo_typeface_str;
   m_promo_template = AccountSrc.m_promo_template;
   m_promo_product = AccountSrc.m_promo_product;
   m_promo_addtoorder = AccountSrc.m_promo_addtoorder;
   m_promo_justifytxt = AccountSrc.m_promo_justifytxt;
   m_promo_promocode = AccountSrc.m_promo_promocode;   
   }

void CAccount::CopyHarvestRules(const CAccount& AccountSrc)
   {
   int i;
   CPhotogizeUtils utils;     
   CAccountHarvestRule* pAccountHarvestRule;
   CAccountHarvestRule* pAccountHarvestRuleDst;

   m_useharvestrules = AccountSrc.m_useharvestrules;
   
   // clear the harvest rule list
   utils.ClearHarvestRuleList(coHarvestRules);

   for (i = 0; i < AccountSrc.coHarvestRules.GetSize(); i++)
	  {
	  if (pAccountHarvestRule = (CAccountHarvestRule*)AccountSrc.coHarvestRules.GetAt(i))
		 {
		 if (pAccountHarvestRuleDst = new CAccountHarvestRule)
			{
			*pAccountHarvestRuleDst = *pAccountHarvestRule;
			coHarvestRules.Add((CObject*)pAccountHarvestRuleDst);
			}
		 }
	  }
   }

void CAccount::CopyMasterHarvestRules(const CAccount& AccountSrc)
   {
   int i;				   
   CMasterAccountHarvestRuleItem* pMasterAccountHarvestRuleItemDst;
   CMasterAccountHarvestRuleItem* pMasterAccountHarvestRuleItemSrc;

   // clear the current master harvest rules
   for (i = coMasterAccountHarvestRuleSet.GetSize() - 1; i >= 0; i--)
	  {
	  if (pMasterAccountHarvestRuleItemDst = (CMasterAccountHarvestRuleItem*)coMasterAccountHarvestRuleSet.GetAt(i))
		 {
		 delete pMasterAccountHarvestRuleItemDst;
		 }
	  }      
   coMasterAccountHarvestRuleSet.RemoveAll();	

   for (i = 0; i < AccountSrc.coHarvestRules.GetSize(); i++)
	  {
	  if (pMasterAccountHarvestRuleItemSrc = (CMasterAccountHarvestRuleItem*)AccountSrc.coMasterAccountHarvestRuleSet.GetAt(i))
		 {
		 if (pMasterAccountHarvestRuleItemDst = new CMasterAccountHarvestRuleItem)
			{
			*pMasterAccountHarvestRuleItemDst = *pMasterAccountHarvestRuleItemSrc;
			coMasterAccountHarvestRuleSet.Add((CObject*)pMasterAccountHarvestRuleItemDst);
			}
		 }
	  }
   }

void CAccount::CopyEmailTriggers(const CAccount& AccountSrc)
   {
   int i;
   
   for (i = 0; i < EMAILTRIGGER_NUMTRIGGERS; i++)		 
	  EmailTriggerList[i] = AccountSrc.EmailTriggerList[i];
   }

void CAccount::CopyLogos(const CAccount& AccountSrc)
   {
   m_invoice_logo = AccountSrc.m_invoice_logo;
   m_album_logo = AccountSrc.m_album_logo;
   m_album_logo_link = AccountSrc.m_album_logo_link;
   m_weblogoleft = AccountSrc.m_weblogoleft;
   m_weblogoleft_link = AccountSrc.m_weblogoleft_link;
   m_weblogoright = AccountSrc.m_weblogoright;
   m_weblogoright_link = AccountSrc.m_weblogoright_link;
   m_leftindex_logo = AccountSrc.m_leftindex_logo;
   m_rightindex_logo = AccountSrc.m_rightindex_logo;	  	
   }

void CAccount::CopyOrderSettings(const CAccount& AccountSrc)
   {
   int i;

   m_autoharvest = AccountSrc.m_autoharvest;
   m_autoprint = AccountSrc.m_autoprint;
   m_moveorders = AccountSrc.m_moveorders;
   m_orderdataonly = AccountSrc.m_orderdataonly;
   m_cconinvoice = AccountSrc.m_cconinvoice;
   m_completeonarchive = AccountSrc.m_completeonarchive;
   m_timepoll = AccountSrc.m_timepoll;
   m_timeprint = AccountSrc.m_timeprint;
   m_timepollunits = AccountSrc.m_timepollunits;
   m_autoprintinvoice = AccountSrc.m_autoprintinvoice;
   m_timeprintunits = AccountSrc.m_timeprintunits;
   m_printrule = AccountSrc.m_printrule;
   m_restrictcopies = AccountSrc.m_restrictcopies;
   m_restrictcopies_num = AccountSrc.m_restrictcopies_num;
   m_restrictvalue = AccountSrc.m_restrictvalue;
   m_restrictvalue_num = AccountSrc.m_restrictvalue_num;
   m_indexproduct_name = AccountSrc.m_indexproduct_name;	  
   m_printindexbydefault = AccountSrc.m_printindexbydefault;   
   
   csaInclude.RemoveAll();
   for (i = 0; i < AccountSrc.csaInclude.GetSize(); i++)
	  csaInclude.Add(AccountSrc.csaInclude.GetAt(i));

   csaExclude.RemoveAll();
   for (i = 0; i < AccountSrc.csaExclude.GetSize(); i++)
	  csaExclude.Add(AccountSrc.csaExclude.GetAt(i));
			
   m_archivefolder = AccountSrc.m_archivefolder;
   m_orderarchivehousekeeping_days = AccountSrc.m_orderarchivehousekeeping_days;
   m_orderarchivehousekeeping_method = AccountSrc.m_orderarchivehousekeeping_method;
   m_orderarchivehousekeeping = AccountSrc.m_orderarchivehousekeeping;

   InvoiceBitmapRenderTime = AccountSrc.InvoiceBitmapRenderTime;
   InvoiceBitmapRenderSize = AccountSrc.InvoiceBitmapRenderSize;
   InvoiceBitmapRenderHeight = AccountSrc.InvoiceBitmapRenderHeight;
   InvoiceBitmapRenderWidth = AccountSrc.InvoiceBitmapRenderWidth;
   }

void CAccount::CopyCDSettings(const CAccount& AccountSrc)
   {
   bCDoffer = AccountSrc.bCDoffer;
   CDPrice = AccountSrc.CDPrice;
   iCDWeightOunces100 = AccountSrc.iCDWeightOunces100;
   csCDDescription = AccountSrc.csCDDescription;
   CDOptions = AccountSrc.CDOptions;
   CD_m_copyfolder = AccountSrc.CD_m_copyfolder;
   CD_m_copyfoldername = AccountSrc.CD_m_copyfoldername;
   CD_m_copyfolderdest = AccountSrc.CD_m_copyfolderdest;
   CD_m_originals = AccountSrc.CD_m_originals;
   CD_m_samples = AccountSrc.CD_m_samples;
   CD_m_sampledims = AccountSrc.CD_m_sampledims;
   CD_m_samplefolder = AccountSrc.CD_m_samplefolder;
   CD_m_index = AccountSrc.CD_m_index;
   CD_m_autostart = AccountSrc.CD_m_autostart;   
   CD_m_prejudge = AccountSrc.CD_m_prejudge;
   }			

void CAccount::SetNextUpdate()
   {
   int SecsAfterMidnite_Now;
   int SecsAfterMidnite_Set;   

   // get the current time
   CTime ct = CTime::GetCurrentTime();

   // get the seconds after midnight right now
   SecsAfterMidnite_Now = ct.GetHour() * 60 * 60 + ct.GetMinute() * 60 + ct.GetSecond();

   // get the seconds after midnight for the update time
   SecsAfterMidnite_Set = m_change_time.GetHour() * 60 * 60 + 
						  m_change_time.GetMinute() * 60 + 
						  m_change_time.GetSecond();
	  
   // if the update time is behind us, the next update is tomorrow, otherwise it's today			   
   if (SecsAfterMidnite_Set < SecsAfterMidnite_Now)		 
	  iNextUpdate = 24 * 60 * 60 - (SecsAfterMidnite_Now - SecsAfterMidnite_Set) + GetTickCount() / 1000;
   else
	  iNextUpdate = SecsAfterMidnite_Set - SecsAfterMidnite_Now + GetTickCount() / 1000;
   }

void CAccount::GetAccountTypeStr(CString& csAccountType)
   {
   if (iSubType == SUBTYPE_SLAVEACCT)
      csAccountType = "Servant";
   else
	  csAccountType = "Master";
   }

void CPhotogizeUtils::MakeUploadString(CString& m_url, CString& m_user, CString& csUpload)
   {
/*
   URL_COMPONENTS UrlComponents;

   if (InternetCrackUrl(m_url, m_url.GetLength(), ICU_DECODE, &UrlComponents))
	  {	  
	  }
   csUpload.Format("http://%s/uploadpicture.asp?cl=%s", m_url, m_user);
*/
   }

void CPhotogizeUtils::DoHousekeeping(COnlineImageCache& coic, CImageCache& cic, 
   CObArray& coAccounts, CString& csFolder, LPCSTR lpcstrTemplate)
   {
   CFileFind finder;   
   CString csWildcard;
   BOOL bWorking;
   CString csFullFileName;
   BOOL bFound = FALSE;
   CTime ctLastWrite;
   CTime ctCurrent;

   // get current time
   ctCurrent = CTime::GetCurrentTime();

//   ::L_UnlockSupport(L_SUPPORT_GIFLZW, "JnjmyyTss");
//   ::L_UnlockSupport(L_SUPPORT_TIFLZW, "JGgUy78FRRx");
   
   csWildcard = csFolder;
   csWildcard += lpcstrTemplate;

   bWorking = finder.FindFile(csWildcard);

   while (bWorking)
	  {
	  // get the file
	  bWorking = finder.FindNextFile();

      if (finder.IsDots())
         continue;
	  else if (finder.IsDirectory())
		 {			  
		 continue;
		 }
	  else if (finder.MatchesMask(FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_COMPRESSED | FILE_ATTRIBUTE_READONLY))
		 {		 
		 // a file match
		 csFullFileName = finder.GetFilePath();
		 // an image file?
		 if (IsSupportedPhoto(csFullFileName))
			{
			// if it isn't in any of our caches, AND the file is older than 30 days,
			// nuke it - we use 30 days for safety (in case someone nukes their acct
			// by accident, but then retrieves it within a day or so
			if (!IsInImageCache(csFullFileName, cic) && !IsInOnlineCache(csFullFileName, coic)
			   && !IsInAccounts(csFullFileName, coAccounts))
			   {	  
			   finder.GetLastWriteTime(ctLastWrite);
			   CTimeSpan cts = ctCurrent - ctLastWrite;

			   if (cts.GetDays() > 30)
				  ::DeleteFile(csFullFileName);
			   }
			}			
		 }
	  }
   finder.Close();
   }

BOOL CPhotogizeUtils::IsInOnlineCache(CString& csFileName, COnlineImageCache& coic)
   {
   int i;
   COnlineImageCacheElement* pOnlineImageCacheElement;
   BOOL bRet = FALSE;

   for (i = 0; i < coic.coElements.GetSize(); i++)
	  {
	  if (pOnlineImageCacheElement = (COnlineImageCacheElement*)coic.coElements.GetAt(i))
		 {
		 if (csFileName.CompareNoCase(pOnlineImageCacheElement->csFileName) == 0)
			{
			bRet = TRUE;
			break;
			}
		 }
	  }
   return bRet;
   }

BOOL CPhotogizeUtils::IsInImageCache(CString& csFileName, CImageCache& cic)
   {
   int i;
   CImageCacheElement* pImageCacheElement;
   BOOL bRet = FALSE;

   for (i = 0; i < cic.coElements.GetSize(); i++)
	  {
	  if (pImageCacheElement = (CImageCacheElement*)cic.coElements.GetAt(i))
		 {
		 if (csFileName.CompareNoCase(pImageCacheElement->csFileName) == 0)
			{
			bRet = TRUE;
			break;
			}
		 }
	  }
   return bRet;
   }

BOOL CPhotogizeUtils::IsInAccounts(CString& csFileName, CObArray& coAccounts)
   {
   int i, j, k;
   CAccount* pAccount;
   COrder* pOrder;
   CPhoto* pPhoto;
   CImage* pImage;
   BOOL bRet = FALSE;

   for (i = 0; i < coAccounts.GetSize(); i++)
	  {
	  if (pAccount = (CAccount*)coAccounts.GetAt(i))
		 {
		 for (j = 0; j < pAccount->pParentOrder->coNodes.GetSize(); j++)
			{
			if (pOrder = (COrder*)pAccount->pParentOrder->coNodes.GetAt(j))
			   {
			   for (k = 0; k < pOrder->coNodes.GetSize(); k++)
				  {
				  if (pPhoto = (CPhoto*)pOrder->coNodes.GetAt(k))
					 {
					 if (pImage = pPhoto->GetOriginalImage())
						{
						if (csFileName.CompareNoCase(pImage->csFileName) == 0)
						   {
						   bRet = TRUE;
						   break;
						   }
						}					 
					 }				  
				  if (bRet) break;
				  }	  
			   }
			if (bRet) break;
			}
		 if (!bRet)
			{
			for (j = 0; j < pAccount->pParentOrderArchive->coNodes.GetSize(); j++)
			   {
			   if (pOrder = (COrder*)pAccount->pParentOrderArchive->coNodes.GetAt(j))
				  {
				  for (k = 0; k < pOrder->coNodes.GetSize(); k++)
					 {
					 if (pPhoto = (CPhoto*)pOrder->coNodes.GetAt(k))
						{
						if (pImage = pPhoto->GetOriginalImage())
						   {
						   if (csFileName.CompareNoCase(pImage->csFileName) == 0)
							  {
							  bRet = TRUE;
							  break;
							  }
						   }					 
						}				  
					 if (bRet) break;
					 }	  
				  }
			   if (bRet) break;
			   }
			}
		 }
	  if (bRet) break;
	  }
   return bRet;
   }

BOOL CPhotogizeUtils::IsSupportedPhoto(CString& csFileName)
   {			
   BOOL bRet = FALSE;
   FILEINFO FileInfo;

   // use lead tools
   memset(&FileInfo, 0, sizeof(FILEINFO));
   FileInfo.uStructSize = sizeof(FILEINFO);
   if (::L_FileInfo((char*)(const char*)csFileName, &FileInfo, sizeof(FILEINFO), FILEINFO_TOTALPAGES, NULL) > 0)            
	  {
	  switch(FileInfo.Format)
		 {
         case FILE_PCX:     // ZSoft PCX
         case FILE_GIF:     // CompuServe GIF
         case FILE_TGA:     // Targa                   
         case FILE_BMP:     // Windows BMP             
         case FILE_JFIF:    // Jpeg File Interchange Format               
         case FILE_LEAD1JFIF:               
         case FILE_LEAD2JFIF:
         case FILE_JTIF:    // Jpeg Tag Image File Format
		 case FILE_LEAD2JTIF:
		 case FILE_LEAD1JTIF:		               
         case FILE_TIFLZW:  // Tagged Image File Formats
         case FILE_TIF_CMYK:
         case FILE_TIFLZW_CMYK:
         case FILE_TIF_PACKBITS:
         case FILE_TIF_PACKBITS_CMYK:
		 case FILE_TIF:     
		 case FILE_TIF_YCC: 
		 case FILE_TIFLZW_YCC:
		 case FILE_TIF_PACKBITS_YCC:
		 case FILE_PNG:
		 case FILE_FPX:
		 case FILE_FPX_SINGLE_COLOR:
		 case FILE_FPX_JPEG:
		 case FILE_FPX_JPEG_QFACTOR:
         case FILE_EXIF:
		 case FILE_EXIF_YCC:
		 case FILE_EXIF_JPEG:
         case FILE_EXIF_JPEG_411:
         case FILE_PCD:     // Photo CD
			  bRet = TRUE;	
		 }
	  }
   return bRet;
   }

CAccountHarvestRule::CAccountHarvestRule()
   {
   iType = HARVEST_RULE_NONE;
   bEnable = FALSE;
   }

CAccountHarvestRule::~CAccountHarvestRule()
   {
   }

void CAccountHarvestRule::Serialize(CArchive& ar)
   {           
   try
	  {
	  if (ar.IsStoring())
		 {
		 ar << iType << csName << bEnable;
		 }
	  else
		 {
		 ar >> iType >> csName >> bEnable;
		 }
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   }

BOOL CAccountHarvestRule::operator==(const CAccountHarvestRule& ahr)
   {
   // is same thing?
   return (IsSame(ahr));
   }
					  
BOOL CAccountHarvestRule::operator!=(const CAccountHarvestRule& ahr)
   {
   // is same thing?
   return (!IsSame(ahr));
   }

BOOL CAccountHarvestRule::IsSame(const CAccountHarvestRule& ahr)
   {               
   // are they the same?
   if (iType == ahr.iType && csName == ahr.csName && bEnable == ahr.bEnable)
	   return TRUE;
   else
	  return FALSE;
   }

const CAccountHarvestRule& CAccountHarvestRule::operator=(const CAccountHarvestRule& cpSrc) 
   {
   // assignment
   if (this != &cpSrc)
      {   
	  // copy the props
	  iType = cpSrc.iType;	  
	  csName = cpSrc.csName;
	  bEnable = cpSrc.bEnable;
	  }
   return *this;   
   }			   

CMasterAccountHarvestRuleItem::CMasterAccountHarvestRuleItem()
   {
   }

CMasterAccountHarvestRuleItem::~CMasterAccountHarvestRuleItem()
   {
   int i;
   CCheckListItem* pCheckListItem;

   i = 0;
   while (i < coShip.GetSize() )
      {
	  if (pCheckListItem = (CCheckListItem*)coShip.GetAt(i++))
		 {							  
		 delete pCheckListItem;			
		 }
      }     
   coShip.RemoveAll();
   i = 0;
   while (i < coPickup.GetSize() )
      {
	  if (pCheckListItem = (CCheckListItem*)coPickup.GetAt(i++))
		 {							  
		 delete pCheckListItem;			
		 }
      }     
   coPickup.RemoveAll();
   i = 0;
   while (i < coProducts.GetSize() )
      {
	  if (pCheckListItem = (CCheckListItem*)coProducts.GetAt(i++))
		 {							  
		 delete pCheckListItem;			
		 }
      }     
   coProducts.RemoveAll();
   }

void CMasterAccountHarvestRuleItem::Serialize(CArchive& ar)
   {           
   try
	  {
	  if (ar.IsStoring())
		 {
		 ar << csStoreID;
		 }
	  else
		 {
		 ar >> csStoreID;
		 }
      coShip.Serialize(ar);
      coPickup.Serialize(ar);
      coProducts.Serialize(ar);
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   }

BOOL CMasterAccountHarvestRuleItem::operator==(const CMasterAccountHarvestRuleItem& ahr)
   {
   // is same thing?
   return (IsSame(ahr));
   }
					  
BOOL CMasterAccountHarvestRuleItem::operator!=(const CMasterAccountHarvestRuleItem& ahr)
   {
   // is same thing?
   return (!IsSame(ahr));
   }

BOOL CMasterAccountHarvestRuleItem::IsSame(const CMasterAccountHarvestRuleItem& ahr)
   {               
   int i;   
   CCheckListItem* pCheckListItemSrc;
   CCheckListItem* pCheckListItemDst;

   // are they the same?
   if (csStoreID != ahr.csStoreID) return FALSE;
   if (coShip.GetSize() != ahr.coShip.GetSize()) return FALSE;
   if (coPickup.GetSize() != ahr.coPickup.GetSize()) return FALSE;
   if (coProducts.GetSize() != ahr.coProducts.GetSize()) return FALSE;

   for (i = 0; i < coShip.GetSize(); i++)
	  {
	  if (pCheckListItemDst = (CCheckListItem*)coShip.GetAt(i))
		 {
		 if (pCheckListItemSrc = (CCheckListItem*)ahr.coShip.GetAt(i))
			{
			if (pCheckListItemSrc->csStr != pCheckListItemDst->csStr || 
			   pCheckListItemSrc->iCheck != pCheckListItemDst->iCheck)
			   {
			   return FALSE;
			   }
			}
		 }
	  }
   for (i = 0; i < coPickup.GetSize(); i++)
	  {
	  if (pCheckListItemDst = (CCheckListItem*)coPickup.GetAt(i))
		 {
		 if (pCheckListItemSrc = (CCheckListItem*)ahr.coPickup.GetAt(i))
			{
			if (pCheckListItemSrc->csStr != pCheckListItemDst->csStr || 
			   pCheckListItemSrc->iCheck != pCheckListItemDst->iCheck)
			   {
			   return FALSE;
			   }
			}
		 }
	  }
   for (i = 0; i < coProducts.GetSize(); i++)
	  {
	  if (pCheckListItemDst = (CCheckListItem*)coProducts.GetAt(i))
		 {
		 if (pCheckListItemSrc = (CCheckListItem*)ahr.coProducts.GetAt(i))
			{
			if (pCheckListItemSrc->csStr != pCheckListItemDst->csStr || 
			   pCheckListItemSrc->iCheck != pCheckListItemDst->iCheck)
			   {
			   return FALSE;
			   }
			}
		 }
	  }
   return TRUE;
   }

const CMasterAccountHarvestRuleItem& CMasterAccountHarvestRuleItem::operator=(const CMasterAccountHarvestRuleItem& cpSrc) 
   {
   int i;
   CCheckListItem* pCheckListItemDst;
   CCheckListItem* pCheckListItemSrc;

   // assignment
   if (this != &cpSrc)
      {   
	  // copy the props
	  csStoreID = cpSrc.csStoreID;	  

	  // remove the existing entries
	  i = 0;
	  while (i < coShip.GetSize() )
		 {
		 if (pCheckListItemDst = (CCheckListItem*)coShip.GetAt(i++))
			{							  
			delete pCheckListItemDst;			
			}
		 }     
	  coShip.RemoveAll();
	  i = 0;
	  while (i < coPickup.GetSize() )
		 {
		 if (pCheckListItemDst = (CCheckListItem*)coPickup.GetAt(i++))
			{							  
			delete pCheckListItemDst;			
			}
		 }     
	  coPickup.RemoveAll();
	  i = 0;
	  while (i < coProducts.GetSize() )
		 {
		 if (pCheckListItemDst = (CCheckListItem*)coProducts.GetAt(i++))
			{							  
			delete pCheckListItemDst;			
			}
		 }     
	  coProducts.RemoveAll();
   
	  // and copy over new
	  for (i = 0; i < cpSrc.coShip.GetSize(); i++)
		 {
		 if (pCheckListItemSrc = (CCheckListItem*)cpSrc.coShip.GetAt(i))
			{
			if (pCheckListItemDst = new CCheckListItem)
			   {
			   *pCheckListItemDst = *pCheckListItemSrc;
			   coShip.Add((CObject*)pCheckListItemDst);
			   }
			}
		 }
	  for (i = 0; i < cpSrc.coPickup.GetSize(); i++)
		 {
		 if (pCheckListItemSrc = (CCheckListItem*)cpSrc.coPickup.GetAt(i))
			{
			if (pCheckListItemDst = new CCheckListItem)
			   {
			   *pCheckListItemDst = *pCheckListItemSrc;
			   coPickup.Add((CObject*)pCheckListItemDst);
			   }
			}
		 }
	  for (i = 0; i < cpSrc.coProducts.GetSize(); i++)
		 {
		 if (pCheckListItemSrc = (CCheckListItem*)cpSrc.coProducts.GetAt(i))
			{
			if (pCheckListItemDst = new CCheckListItem)
			   {
			   *pCheckListItemDst = *pCheckListItemSrc;
			   coProducts.Add((CObject*)pCheckListItemDst);
			   }
			}
		 }	  
	  }
   return *this;   
   }

void CPhotogizeUtils::ClearHarvestRuleList(CObArray& coHarvestRules)
   {
   int i;
   CAccountHarvestRule* pAccountHarvestRule;

   // remove the existing ship list entries
   i = 0;
   while (i < coHarvestRules.GetSize() )
      {
	  if (pAccountHarvestRule = (CAccountHarvestRule*)coHarvestRules.GetAt(i++))
		 {							  
		 delete pAccountHarvestRule;			
		 }
      }     
   coHarvestRules.RemoveAll();
   }

void CPhotogizeUtils::ClearHarvestRuleList(CObArray* pcoHarvestRules)
   {
   int i;
   CAccountHarvestRule* pAccountHarvestRule;

   // remove the existing ship list entries
   i = 0;
   while (i < pcoHarvestRules->GetSize() )
      {
	  if (pAccountHarvestRule = (CAccountHarvestRule*)pcoHarvestRules->GetAt(i++))
		 {							  
		 if (pAccountHarvestRule) delete pAccountHarvestRule;			
		 }
      }     
   pcoHarvestRules->RemoveAll();
   }

void CPhotogizeUtils::CopyHarvestRules(const CObArray& coSrc, CObArray& coDst)
   {
   int i;
   CAccountHarvestRule* pAccountHarvestRule;
   CAccountHarvestRule* pAccountHarvestRuleDst;   

   ClearHarvestRuleList(coDst);

   for (i = 0; i < coSrc.GetSize(); i++)
	  {
	  if (pAccountHarvestRule = (CAccountHarvestRule*)coSrc.GetAt(i))
		 {
		 if (pAccountHarvestRuleDst = new CAccountHarvestRule)
			{
			*pAccountHarvestRuleDst = *pAccountHarvestRule;
			coDst.Add((CObject*)pAccountHarvestRuleDst);
			}
		 }
	  }
   }

BOOL CAccount::HarvestThisOrder(COrder* pOrder)
   {
   BOOL bRet = FALSE;
//   int i, j;
//   CAccountHarvestRule* pAccountHarvestRule;
//   CMasterAccountHarvestRuleItem* pMasterAccountHarvestRuleItem;
//   CCheckListItem* pCheckListItem;
/*
   // rules check
   if (m_useharvestrules == USE_HARVESTRULES_LOCAL)
	  {
	  // check this
	  for (i = 0; i < coHarvestRules.GetSize(); i++)
		 {
		 if (pAccountHarvestRule = (CAccountHarvestRule*)coHarvestRules.GetAt(i))
			{
			// not enabled!
			if (!pAccountHarvestRule->bEnable) continue;
			switch(pAccountHarvestRule->iType)
			   {
			   case HARVEST_RULE_SHIPVIA:
				    if (pOrder->OrderDetails.ShipMethod == pAccountHarvestRule->csName)					
			   		bRet = TRUE;
			   	 break;
			   case HARVEST_RULE_PICKUPLOC:
				    if (pOrder->OrderDetails.PickupMethod == pAccountHarvestRule->csName)					
			   		bRet = TRUE;
			   	 break;
			   }		 
			}
		 if (bRet) break;
		 }
	  }
   else if (m_useharvestrules == USE_HARVESTRULES_MASTER)
	  {
	  for (i = 0; i < coMasterAccountHarvestRuleSet.GetSize(); i++)
		 {
		 if (pMasterAccountHarvestRuleItem = (CMasterAccountHarvestRuleItem*)coMasterAccountHarvestRuleSet.GetAt(i))
			{		 
			// first, check the store id for a match
			if (csStoreID == pMasterAccountHarvestRuleItem->csStoreID)
			   {
			   for (j = 0; j < pMasterAccountHarvestRuleItem->coShip.GetSize(); j++)
				  {
				  if (pCheckListItem = (CCheckListItem*)pMasterAccountHarvestRuleItem->coShip.GetAt(j))
					 {
					 if (!pCheckListItem->iCheck) continue;
				     if (pOrder->OrderDetails.ShipMethod == pCheckListItem->csStr)
						{
						bRet = TRUE;
						break;
						}
					 }				  
				  }
			   for (j = 0; j < pMasterAccountHarvestRuleItem->coPickup.GetSize(); j++)
				  {
				  if (pCheckListItem = (CCheckListItem*)pMasterAccountHarvestRuleItem->coPickup.GetAt(j))
					 {
					 if (!pCheckListItem->iCheck) continue;
				     if (pOrder->OrderDetails.PickupMethod == pCheckListItem->csStr)
						{
						bRet = TRUE;
						break;
						}
					 }				  
				  }
			   }			
			}
		 if (bRet) break;
		 }
	  }
   else
	  return TRUE;
*/
   return bRet;
   }

BOOL CAccount::HarvestThisJob(CString& csProductCompare, CString& csCDProductName)
   {
   BOOL bRet = FALSE;
   int i, j;
   CAccountHarvestRule* pAccountHarvestRule;   
   CMasterAccountHarvestRuleItem* pMasterAccountHarvestRuleItem;
   CCheckListItem* pCheckListItem;

   // always harvest a CD product photo
   if (csProductCompare == csCDProductName) return TRUE;

   // there are no rules, so return TRUE
   if (m_useharvestrules == USE_HARVESTRULES_LOCAL)
	  {
	  // check this
	  for (i = 0; i < coHarvestRules.GetSize(); i++)
		 {
		 if (pAccountHarvestRule = (CAccountHarvestRule*)coHarvestRules.GetAt(i))
			{
			// not enabled!
			if (!pAccountHarvestRule->bEnable) continue;
			switch(pAccountHarvestRule->iType)
			   {
			   case HARVEST_RULE_PRODUCT:
				    if (csProductCompare == pAccountHarvestRule->csName)					
			   		bRet = TRUE;
			   	 break;
			   }		 
			}
		 if (bRet) break;
		 }
	  }
   else if (m_useharvestrules == USE_HARVESTRULES_MASTER)
	  {
	  for (i = 0; i < coMasterAccountHarvestRuleSet.GetSize(); i++)
		 {
		 if (pMasterAccountHarvestRuleItem = (CMasterAccountHarvestRuleItem*)coMasterAccountHarvestRuleSet.GetAt(i))
			{		 
			// first, check the store id for a match
			if (csStoreID == pMasterAccountHarvestRuleItem->csStoreID)
			   {
			   for (j = 0; j < pMasterAccountHarvestRuleItem->coProducts.GetSize(); j++)
				  {
				  if (pCheckListItem = (CCheckListItem*)pMasterAccountHarvestRuleItem->coProducts.GetAt(j))
					 {
					 if (!pCheckListItem->iCheck) continue;
				     if (csProductCompare == pCheckListItem->csStr)
						{
						bRet = TRUE;
						break;
						}
					 }				  
				  }
			   }
			}
		 }
	  }
   else
	  bRet = TRUE;

   return bRet;
   }

CImageAdjustment::CImageAdjustment()
   {   
   Revert();   
   }

void CImageAdjustment::Revert()
   {
   int i;
   CRedEyeAdjust* pRedEyeAdjust;

   iLightDark = 0;
   iContrast = 0;
   iSharpness = 0;
   iSaturation = 0;
   iGamma = 0;
   iRedAdjust = 0;
   iGreenAdjust = 0;
   iBlueAdjust = 0;   
   bDoGrayScale = FALSE;
   bUseAutoLevels = FALSE;
   bDoSepiaTone = FALSE;
   iRotate = 0;   
   bInRedEyeMode = FALSE;
   // delete the members array
   i = 0;
   while (i < coRedEyeArray.GetSize() )
      {
	  if (pRedEyeAdjust = (CRedEyeAdjust*)coRedEyeArray.GetAt(i++))
		 {							  
		 if (pRedEyeAdjust) delete pRedEyeAdjust;
		 }
      }     
   coRedEyeArray.RemoveAll();
   }

CImageAdjustment::~CImageAdjustment()
   {
   int i;
   CRedEyeAdjust* pRedEyeAdjust;

   // delete the members array
   i = 0;
   while (i < coRedEyeArray.GetSize() )
      {
	  if (pRedEyeAdjust = (CRedEyeAdjust*)coRedEyeArray.GetAt(i++))
		 {							  
		 if (pRedEyeAdjust) delete pRedEyeAdjust;
		 }
      }     
   coRedEyeArray.RemoveAll();
   }

void CImageAdjustment::Serialize(CArchive& ar, int nVersion)
   {           
   try
	  {
	  if (ar.IsStoring())
		 {
		 ar << iLightDark << iContrast << iSharpness << iSaturation << iGamma << 
			iRedAdjust << iGreenAdjust << iBlueAdjust << bDoGrayScale << 
			bUseAutoLevels << bDoSepiaTone << iRotate;
		 ar << bInRedEyeMode;
		 coRedEyeArray.Serialize(ar);
		 }
	  else
		 {
		 ar >> iLightDark >> iContrast >> iSharpness >> iSaturation >> iGamma >> 
			iRedAdjust >> iGreenAdjust >> iBlueAdjust;
   	
		 // note that this is actually the nVersion of the parent CPhoto
		 if (nVersion >= 2)
			{
			ar >> bDoGrayScale >> bUseAutoLevels >> bDoSepiaTone >> iRotate;
			}
		 if (nVersion >= 5)
			{
			ar >> bInRedEyeMode;
			coRedEyeArray.Serialize(ar);
			}
		 }
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   }

const CImageAdjustment& CImageAdjustment::operator=(const CImageAdjustment& cpSrc) 
   {
   CRedEyeAdjust* pRedEyeAdjustSrc;
   CRedEyeAdjust* pRedEyeAdjustDst;

   // assignment
   if (this != &cpSrc)
      {   
	  // reset everything
	  Revert();
	  // copy the props
	  iLightDark = cpSrc.iLightDark;	  
      iContrast = cpSrc.iContrast;
      iSharpness = cpSrc.iSharpness;
      iSaturation = cpSrc.iSaturation;
      iGamma = cpSrc.iGamma;
      iRedAdjust = cpSrc.iRedAdjust;
      iGreenAdjust = cpSrc.iGreenAdjust;
      iBlueAdjust = cpSrc.iBlueAdjust;	  
	  bDoGrayScale = cpSrc.bDoGrayScale;
	  bUseAutoLevels = cpSrc.bUseAutoLevels;
	  bDoSepiaTone = cpSrc.bDoSepiaTone;	  
	  iRotate = cpSrc.iRotate;      
	  bInRedEyeMode = cpSrc.bInRedEyeMode;
	  for (int i = 0; i < cpSrc.coRedEyeArray.GetSize(); i++)
		 {
		 if (pRedEyeAdjustSrc = (CRedEyeAdjust*)cpSrc.coRedEyeArray.GetAt(i))
			{
			if (pRedEyeAdjustDst = new CRedEyeAdjust)
			   {
			   *pRedEyeAdjustDst = *pRedEyeAdjustSrc;
			   coRedEyeArray.Add((CObject*)pRedEyeAdjustDst);
			   }
			}
		 }
	  }
   return *this;   
   }			   

BOOL CImageAdjustment::operator==(const CImageAdjustment& iadj)
   {
   // is same thing?
   return (IsSame(iadj));
   }
					  
BOOL CImageAdjustment::operator!=(const CImageAdjustment& iadj)
   {
   // is same thing?
   return (!IsSame(iadj));
   }

BOOL CImageAdjustment::IsSame(const CImageAdjustment& iadj)
   {         
   int i;
   CRedEyeAdjust* pRedEyeAdjustSrc;
   CRedEyeAdjust* pRedEyeAdjustDst;
   BOOL bRet = FALSE;

   // are they the same?
   if (iLightDark == iadj.iLightDark &&
      iContrast == iadj.iContrast &&
      iSharpness == iadj.iSharpness &&
      iSaturation == iadj.iSaturation &&
      iGamma == iadj.iGamma &&
      iRedAdjust == iadj.iRedAdjust &&
      iGreenAdjust == iadj.iGreenAdjust &&
      iBlueAdjust == iadj.iBlueAdjust &&  	  
	  bDoGrayScale == iadj.bDoGrayScale &&
	  bUseAutoLevels == iadj.bUseAutoLevels &&
	  bDoSepiaTone == iadj.bDoSepiaTone &&
	  iRotate == iadj.iRotate &&
	  bInRedEyeMode == iadj.bInRedEyeMode)	  
	  {
	  if (coRedEyeArray.GetSize() == iadj.coRedEyeArray.GetSize())
		 {
		 bRet = TRUE;
		 for(i = 0; i < coRedEyeArray.GetSize(); i++)
			{
			if (pRedEyeAdjustSrc = (CRedEyeAdjust*)coRedEyeArray.GetAt(i))
			   {
			   if (pRedEyeAdjustDst = (CRedEyeAdjust*)iadj.coRedEyeArray.GetAt(i))
				  {
				  if (*pRedEyeAdjustSrc != *pRedEyeAdjustDst)
					 {
					 bRet = FALSE;
					 break;
					 }
				  }
			   }
			}
		 }
	  }

   return bRet;
   }

BOOL CImageAdjustment::IsAdjusted(BOOL bIgnoreRotate)
   {   
   if (iLightDark == 0 &&
	  iContrast == 0 &&
	  iSharpness == 0 &&
	  iSaturation == 0 &&
	  iGamma == 0 &&
	  iRedAdjust == 0 &&
	  iGreenAdjust == 0 &&
	  iBlueAdjust == 0 && 	  
      bDoGrayScale == FALSE &&
	  bUseAutoLevels == FALSE &&
	  bDoSepiaTone == FALSE &&
	  ((iRotate == 0) || bIgnoreRotate) &&
	  coRedEyeArray.GetSize() == 0)
	  return FALSE;
   else
	  return TRUE;
   }
/*
BITMAPHANDLE* CImage::GetCorrectedBitmapHandle(CImageAdjustment& cImageAdjustPhoto, 
   CTransformCMM* pPassedTransformCMM)
   {   
   // if no adjustment, just return the standard bitmap
   if (!cImageAdjustPhoto.IsAdjusted() && pPassedTransformCMM == NULL)	  
	  return &Bitmap;
   else
	  {
	  // could take some time
	  AfxGetApp()->DoWaitCursor(1);

	  // do we already have a corrected bitmap?
	  if (BitmapCorrected.Flags.Allocated)
		 {
		 // if the same adjustment, just return the corrected bitmap
		 // otherwise, free the bitmap and reconstruct
		 if (cImageAdjustActive != cImageAdjustPhoto || 
			pPassedTransformCMM != pTransformCMM)
			{		
			// free the destination
			::L_FreeBitmap(&BitmapCorrected);
			// copy the destination
			::L_CopyBitmap(&BitmapCorrected, &Bitmap);

			// correct it
			CorrectBitmap(cImageAdjustPhoto);

			// if pPassedTransformCMM, we must apply a transform
			if (pPassedTransformCMM)
			   {
			   pTransformCMM = pPassedTransformCMM;
			   CPhotogizeUtils::ICCColorAdjust(pTransformCMM, &BitmapCorrected);			   
			   }														   			
			}		 
		 }	
	  else
		 {		 
		 // copy the destination
		 ::L_CopyBitmap(&BitmapCorrected, &Bitmap);		 
		 // correct it
		 CorrectBitmap(cImageAdjustPhoto);
		 // if iNewICCInstance > 0, we must apply a transform
		 if (pPassedTransformCMM)
			{
			pTransformCMM = pPassedTransformCMM;
			CPhotogizeUtils::ICCColorAdjust(pTransformCMM, &BitmapCorrected);			   
			}
		 }
      AfxGetApp()->DoWaitCursor(-1);
	  return &BitmapCorrected;
	  }
   }
*/
BITMAPHANDLE* CImage::GetGrayScaleBitmapHandle(int iRotate)
   {
   // do we already have a grayscale bitmap?
   // or is the rotation incorrect
   if (!BitmapGray.Flags.Allocated || iRotate != iBitmapGrayRotation)
	  {		 
	  if (BitmapGray.Flags.Allocated) ::L_FreeBitmap(&BitmapGray);
	  // copy the destination
	  ::L_CopyBitmap(&BitmapGray, &Bitmap, sizeof(BITMAPHANDLE));
	  ::L_GrayScaleBitmap(&BitmapGray, 8);
	  ::L_ChangeBitmapIntensity(&BitmapGray, 200);	  
	  if (iRotate != 0)
		 ::L_RotateBitmap(&BitmapGray, iRotate * 100, ROTATE_RESIZE, 
		 RGB(255,255,255));
	  iBitmapGrayRotation = iRotate;
	  }
   return &BitmapGray;
   }

void CImage::CorrectBitmap(CImageAdjustment& cImageAdjustPhoto)
   {
   CPhotogizeUtils utils;

   // store this as the active adjustment
   cImageAdjustActive = cImageAdjustPhoto;
   
   // call the helper
   utils.CorrectBitmap(&BitmapCorrected, cImageAdjustActive);
   }

BOOL CPhotogizeUtils::GetOurFileStatus(CString& csFileName, CFileStatus& cfs)
   {
   BOOL bRet = FALSE;
   BOOL bOK;

   // we do this here so we can trap exceptions which can happen
   // if the date or time is bad (e.g. BOTTLES.TIF)

   HANDLE hFile;
   WIN32_FIND_DATA FileInfo;
   if ((hFile = FindFirstFile(csFileName, &FileInfo)) != INVALID_HANDLE_VALUE)
	  {      
	  SYSTEMTIME SystemTime;
	  
	  // convert
	  bOK = FileTimeToSystemTime(&FileInfo.ftLastWriteTime, &SystemTime);

	  // bad time!
	  if (!bOK || SystemTime.wYear < 1970)
		 cfs.m_ctime = CTime::GetCurrentTime();
	  else
		 cfs.m_ctime = FileInfo.ftLastWriteTime;

	  // file size (assuming size less than MAXDWORD)
	  cfs.m_size = FileInfo.nFileSizeLow;		 

      // close the handle
      FindClose(hFile);   

	  bRet = TRUE;
	  }	  
   return bRet;
   }

void CPhotogizeUtils::CorrectBitmap(BITMAPHANDLE* pBitmapCorrected, 
   CImageAdjustment& cImageAdjust, BOOL bIgnoreRotate)   
   {
   int i;
   L_INT *pLookupTable = NULL;  /* Array to hold lookup table*/
   L_INT nLutLen = 256;//Get the real LUT length
   pLookupTable = (L_INT *) malloc(nLutLen * sizeof(L_INT));

   if (cImageAdjust.iLightDark != 0)
	  ::L_ChangeBitmapIntensity(pBitmapCorrected, cImageAdjust.iLightDark);	  
   if (cImageAdjust.iContrast != 0)
	  ::L_ChangeBitmapContrast(pBitmapCorrected, cImageAdjust.iContrast);
   if (cImageAdjust.iSharpness != 0)
	  ::L_SharpenBitmap(pBitmapCorrected, cImageAdjust.iSharpness);
   if (cImageAdjust.bDoGrayScale)
	  {
	  ::L_GrayScaleBitmap(pBitmapCorrected, 8);
	  // and convert back to 24bpp
	  ::L_ColorResBitmap(pBitmapCorrected, pBitmapCorrected, sizeof(BITMAPHANDLE), 
							24, CRF_BYTEORDERBGR, NULL, NULL, 0, NULL, NULL);
	  }
   if (cImageAdjust.bUseAutoLevels)
	  ::L_StretchBitmapIntensity(pBitmapCorrected);
   if (cImageAdjust.iRedAdjust != 0)
	  {
	  for (i = 0; i < 256; i++)		 
		  pLookupTable[i] = max(min(i + cImageAdjust.iRedAdjust * 255 / 1000, 255), 0);

	  ::L_RemapBitmapIntensity(pBitmapCorrected, pLookupTable, 256, CHANNEL_RED);
	  }

   memset(pLookupTable, 0, nLutLen * sizeof(L_INT));

   if (cImageAdjust.iGreenAdjust != 0)
	  {
	  for (i = 0; i < 256; i++)
	  {
		 pLookupTable[i] = max(min(i + cImageAdjust.iGreenAdjust * 255 / 1000, 255), 0);
	  }

	  ::L_RemapBitmapIntensity(pBitmapCorrected, pLookupTable, 256, CHANNEL_RED);
	  }

   memset(pLookupTable, 0, nLutLen * sizeof(L_INT));

   if (cImageAdjust.iBlueAdjust != 0)
	  {
	  for (i = 0; i < 256; i++)		 
	  {
		 pLookupTable[i] = max(min(i + cImageAdjust.iBlueAdjust * 255 / 1000, 255), 0);
	  }

	  ::L_RemapBitmapIntensity(pBitmapCorrected, pLookupTable, 256, CHANNEL_RED);
	  }
   // do red eye
   if (cImageAdjust.coRedEyeArray.GetSize() > 0)
	  DoRedEyeAdjustment(pBitmapCorrected, cImageAdjust);
   if (cImageAdjust.iRotate != 0 && !bIgnoreRotate)
	  ::L_RotateBitmap(pBitmapCorrected, cImageAdjust.iRotate * 100, ROTATE_RESIZE, 
	  RGB(255,255,255));
   }

void CPhoto::DrawThumbCropRect(BITMAPHANDLE* pBitmap)
   {   
#if 0
   int iTargetSizing;
   CRect crSrc;
   CRect crDst;

   // get the target sizing
   iTargetSizing = GetSizingToUse(pProduct);

   // get the original image so that our math is more accurate
   pOriginalImage = GetOriginalImage();

   // our destination rect is the thumbnail
   crDst.SetRectEmpty();   
   if (cImageAdjust.iRotate == 0 || 
	   cImageAdjust.iRotate == 180 || 
	   cImageAdjust.iRotate == -180)
	  {
      crDst.right = pBitmap->Width;
	  crDst.bottom = pBitmap->Height;
	  }
   else
	  {
      crDst.right = pBitmap->Height;
	  crDst.bottom = pBitmap->Width;
	  }

   switch(iTargetSizing)
	  {
	  }
#endif
   }

BOOL CPhotogizeUtils::CopyFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName,
								  BOOL bFailIfExists, BOOL bShowPrompt)
   {
   BOOL bRet;
   BOOL bBreakOut = FALSE;
   CString csPrompt;

   // if bFailIfExists is not set, do a cancel/retry loop
   do
	  {
	  bRet = ::CopyFile(lpExistingFileName, lpNewFileName, bFailIfExists);
	  if (!bRet && !bFailIfExists && bShowPrompt)
		 {		 
		 csPrompt.Format("Error Copying File %s to %s, err: %x", lpExistingFileName, lpNewFileName, ::GetLastError()); 
		 if (AfxMessageBox(csPrompt, MB_RETRYCANCEL | MB_SETFOREGROUND) == IDCANCEL)
			bBreakOut = TRUE;
		 }
	  else
		 bBreakOut = TRUE;
	  } while (!bBreakOut);	  

   return bRet;
   }

void CPhotogizeUtils::LaunchURLInNewWindow(HWND hWndParent, LPCSTR lpcstrWebUpload)
   {
   char szBuffer[_MAX_PATH];
   CString csFileName;
   CFile cf;   
   CString csAppPath;
   CString csTemp;

   csTemp = "<HTML>test</HTML>";
   
   CPhotogizeUtils::GetAppPath(csAppPath);      

   // make filename   
   csFileName.Format("%s~test.htm", csAppPath);    
   
   // open the cache and read the contents
   if (cf.Open(csFileName, CFile::modeCreate | CFile::modeWrite))
	  {
	  try
		 {
		 cf.Write(csTemp, csTemp.GetLength());         
		 }
      catch(CException* theException)
		 {      
		 // oops
		 theException->Delete();
		 }
	  cf.Close();
	  }			    

   // open in a new window by doing FindExecutable and using csWebUpload on the
   // command line			  
   if (::FindExecutable(csFileName, NULL, szBuffer) > (HINSTANCE)32)
      ::ShellExecute(hWndParent, NULL, szBuffer, lpcstrWebUpload, "", SW_SHOW);	   
   else 
	  ::ShellExecute(hWndParent, NULL, lpcstrWebUpload, "", "", SW_SHOW); // last resort	   
   }

void CPhoto::SetAdjustmentsString(CString& m_adjustments)
   {   
   CString csTemp;
   BOOL bUseLongStr;

   m_adjustments.Empty();

   if (cImageAdjust.IsAdjusted())
	  {
	  // arbitrary sizing
	  if (GetNumAdjustments() > 2)
		 bUseLongStr = FALSE;
	  else
		 bUseLongStr = TRUE;

	  if (cImageAdjust.iRedAdjust != 0)
		 {
		 csTemp.Format("%s%s%d ", bUseLongStr ? "cyan" : "cy", cImageAdjust.iRedAdjust > 0 ? "-" : "+", 
			abs(cImageAdjust.iRedAdjust / 10));
		 m_adjustments += csTemp;
		 }
	  if (cImageAdjust.iGreenAdjust != 0)
		 {
		 csTemp.Format("%s%s%d ", bUseLongStr ? "magenta" : "ma", cImageAdjust.iGreenAdjust > 0 ? "-" : "+", 
			abs(cImageAdjust.iGreenAdjust / 10));
		 m_adjustments += csTemp;
		 }
	  if (cImageAdjust.iBlueAdjust != 0)
		 {
		 csTemp.Format("%s%s%d ", bUseLongStr ? "yellow" : "ye", cImageAdjust.iBlueAdjust > 0 ? "-" : "+", 
			abs(cImageAdjust.iBlueAdjust / 10));
		 m_adjustments += csTemp;
		 }
	  if (cImageAdjust.iContrast != 0)
		 {
		 csTemp.Format("%s%s%d ", bUseLongStr ? "contrast" : "co", cImageAdjust.iContrast > 0 ? "+" : "-", 
			abs(cImageAdjust.iContrast / 10));
		 m_adjustments += csTemp;
		 }
	  if (cImageAdjust.iLightDark != 0)
		 {
		 csTemp.Format("%s%s%d ", bUseLongStr ? "light" : "ld", cImageAdjust.iLightDark > 0 ? "+" : "-", 
			abs(cImageAdjust.iLightDark / 10));
		 m_adjustments += csTemp;
		 }
	  if (cImageAdjust.iSharpness != 0)
		 {
		 csTemp.Format("%s%s%d ", bUseLongStr ? "sharp" : "sh", cImageAdjust.iSharpness > 0 ? "+" : "-", 
			abs(cImageAdjust.iSharpness / 10));
		 m_adjustments += csTemp;
		 }
	  if (cImageAdjust.bDoGrayScale)
		 {
		 if (bUseLongStr)
			m_adjustments += "gray ";
		 else
			m_adjustments += "gr ";
		 }
	  if (cImageAdjust.bUseAutoLevels)
		 {
		 if (bUseLongStr)
			m_adjustments += "magiclevel ";
		 else
			m_adjustments += "ml ";
		 }
	  }
   }

int CPhoto::GetNumAdjustments()
   {  
   int iNumAdjustments = 0;

   if (cImageAdjust.iRedAdjust != 0) iNumAdjustments++;
   if (cImageAdjust.iGreenAdjust != 0) iNumAdjustments++;
   if (cImageAdjust.iBlueAdjust != 0) iNumAdjustments++;
   if (cImageAdjust.iContrast != 0) iNumAdjustments++;
   if (cImageAdjust.iLightDark != 0) iNumAdjustments++;	  
   if (cImageAdjust.iSharpness != 0) iNumAdjustments++;
   if (cImageAdjust.bDoGrayScale) iNumAdjustments++;
   if (cImageAdjust.bUseAutoLevels) iNumAdjustments++;

   return iNumAdjustments;
   }

void CPhotogizeUtils::GetAppPath(CString& csAppPath)
   {
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char szAppPath[_MAX_PATH];
   
   GetModuleFileName(NULL, szAppPath, _MAX_PATH); 

   _splitpath(szAppPath, drive, dir, NULL, NULL);

   csAppPath = drive;
   csAppPath += dir;

   VERIFYPATH(csAppPath);
   }

CEmbeddedICCInfo::CEmbeddedICCInfo()
   {	  
   Reset();
   }

CEmbeddedICCInfo::~CEmbeddedICCInfo()
   {	  
   }

void CEmbeddedICCInfo::Serialize(CArchive& ar)
   {           
   try
	  {
	  if (ar.IsStoring())
		 {
		 ar << bScanned << bExists << csName << fpos << size << deviceClass << colorSpace << pcs << renderingIntent << 
	 	    manufacturer << model << attributes_l << attributes_h << flags << creator << 
		    cmmId << majv << minv << bfv << year << month << day << hours <<
			minutes << seconds << platform << illuminant_X << illuminant_Y << illuminant_Z;
		 }
	  else
		 {
		 ar >> bScanned >> bExists >> csName >> fpos >> size >> deviceClass >> colorSpace >> pcs >> renderingIntent >> 
	 	    manufacturer >> model >> attributes_l >> attributes_h >> flags >> creator >> 
		    cmmId >> majv >> minv >> bfv >> year >> month >> day >> hours >>
			minutes >> seconds >> platform >> illuminant_X >> illuminant_Y >> illuminant_Z;
		 }
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   }

BOOL CEmbeddedICCInfo::operator==(const CEmbeddedICCInfo& ceicc)
   {
   // is same thing?
   return (IsSame(ceicc));
   }
					  
BOOL CEmbeddedICCInfo::operator!=(const CEmbeddedICCInfo& ceicc)
   {
   // is same thing?
   return (!IsSame(ceicc));
   }

BOOL CEmbeddedICCInfo::IsSame(const CEmbeddedICCInfo& ceicc)
   {               
   if (bScanned == ceicc.bScanned &&
	   bExists == ceicc.bExists &&
	   csName == ceicc.csName &&
	   fpos == ceicc.fpos &&
	   size == ceicc.size &&
	   deviceClass == ceicc.deviceClass &&
	   colorSpace == ceicc.colorSpace &&
	   pcs == ceicc.pcs &&
	   renderingIntent == ceicc.renderingIntent &&
	   manufacturer == ceicc.manufacturer &&
	   model == ceicc.model &&
	   attributes_l == ceicc.attributes_l &&
	   attributes_h == ceicc.attributes_h &&
	   flags == ceicc.flags &&
	   creator == ceicc.creator &&
	   cmmId == ceicc.cmmId &&
	   majv == ceicc.majv &&
	   minv == ceicc.minv &&
	   bfv == ceicc.bfv &&
       year == ceicc.year &&
	   month == ceicc.month &&
	   day == ceicc.day &&
	   hours == ceicc.hours &&
	   minutes == ceicc.minutes &&
	   seconds == ceicc.seconds &&	   
	   platform == ceicc.platform &&
	   illuminant_X == ceicc.illuminant_X &&
	   illuminant_Y == ceicc.illuminant_Y &&
	   illuminant_Z == ceicc.illuminant_Z)
	  return TRUE;
   else
	  return FALSE;
   }

const CEmbeddedICCInfo& CEmbeddedICCInfo::operator=(const CEmbeddedICCInfo& cqfSrc) 
   {
   // assignment
   if (this != &cqfSrc)
      {   
	  bScanned = cqfSrc.bScanned;
	  bExists = cqfSrc.bExists;
	  csName = cqfSrc.csName;
	  fpos = cqfSrc.fpos;
	  size = cqfSrc.size;
	  deviceClass = cqfSrc.deviceClass;
	  colorSpace = cqfSrc.colorSpace;
	  pcs = cqfSrc.pcs;
	  renderingIntent = cqfSrc.renderingIntent;
	  manufacturer = cqfSrc.manufacturer;
	  model = cqfSrc.model;
	  attributes_l = cqfSrc.attributes_l;
	  attributes_h = cqfSrc.attributes_h;
	  flags = cqfSrc.flags;
	  creator = cqfSrc.creator;
	  cmmId = cqfSrc.cmmId;
	  majv = cqfSrc.majv;
	  minv = cqfSrc.minv;
	  bfv = cqfSrc.bfv;
      year = cqfSrc.year;
      month = cqfSrc.month;
	  day = cqfSrc.day;
	  hours = cqfSrc.hours;
	  minutes = cqfSrc.minutes;
	  seconds = cqfSrc.seconds;
	  platform = cqfSrc.platform;
	  illuminant_X = cqfSrc.illuminant_X;
	  illuminant_Y = cqfSrc.illuminant_Y;
	  illuminant_Z = cqfSrc.illuminant_Z;
	  }
   return *this;   
   }

void CEmbeddedICCInfo::Reset()
   {
   bScanned = FALSE;
   bExists = FALSE;
   csName.Empty();
   fpos = 0;
   size = 0;	
   deviceClass = 0;
   colorSpace = 0;
   pcs = 0;
   renderingIntent = 0;
   manufacturer = 0;
   model = 0;
   attributes_l = 0;
   attributes_h = 0;
   flags = 0;
   creator = 0;
   cmmId = 0;
   majv = 0;
   minv = 0;
   bfv = 0;
   year = 0;
   month = 0;
   day = 0;
   hours = 0;
   minutes = 0;
   seconds = 0;
   platform = 0;
   illuminant_X = 0.0;
   illuminant_Y = 0.0;
   illuminant_Z = 0.0;
   }

BOOL CEmbeddedICCInfo::GetICCInfo(LPCSTR lpcstrIn)
   {
   BOOL bRet = FALSE;   
/*
   int search = 0;   
   int offset = 0;		/* Offset to read profile from */
/*
   int found = 0;
   icmFile *fp;
   icc *icco;
   int rv = 0;

   // we've taken a look!
   bScanned = TRUE;
   
   // Open up the file for reading
   if ((fp = new_icmFileStd_name((char*)lpcstrIn,"r")) == NULL)
	  return FALSE;
   
   if ((icco = new_icc()) == NULL)
	  {
	  fp->del(fp);
	  return FALSE;
	  }
   
   // Dumb search for magic number	
   int fc = 0;
   char c;
   
   if (fp->seek(fp, offset) != 0)
	  {
	  fp->del(fp);
	  return FALSE;
	  }
   
   while(found == 0) 
	  {
	  if (fp->read(fp, &c, 1, 1) != 1) 
		 {
		 break;
		 }
	  offset++;	  
	  switch (fc) 
		 {
		 case 0:
			if (c == 'a')
			   fc++;
			else
			   fc = 0;
			break;
		 case 1:
			if (c == 'c')
			   fc++;
			else
			   fc = 0;
			break;
		 case 2:
			if (c == 's')
			   fc++;
			else
			   fc = 0;
			break;
		 case 3:
			if (c == 'p') {
			   found = 1;
			   offset -= 40;
			   }
			else
			   fc = 0;
			break;
		 }
	  }   
   if (found == 1)
	  {	 
	  if ((rv = icco->read(icco,fp,offset)) == 0)
		 {	
		 // get description
		 icTagSignature sig = (icTagSignature)str2tag("desc");
		 // Try to locate that particular tag
		 if ((rv = icco->find_tag(icco,sig)) == 0) 
			{					
			icmBase *ob;
			if ((ob = icco->read_tag(icco, sig)) != NULL) 
			   {
			   icmTextDescription *p = (icmTextDescription *)ob;
			   csName = p->desc;
			   }
			}

		 // now load the rest of the struct
		 fpos = offset;
		 size = icco->header->size;
		 deviceClass = (long)icco->header->deviceClass;
		 colorSpace = (long)icco->header->colorSpace;
		 pcs = (long)icco->header->pcs;
		 renderingIntent = (int)icco->header->renderingIntent;
		 manufacturer = (int)icco->header->manufacturer;
		 model = (int)icco->header->model;
		 attributes_l = (unsigned long)icco->header->attributes.l;
		 attributes_h = (unsigned long)icco->header->attributes.h;
		 flags = (unsigned int)icco->header->flags;
		 creator = (long)icco->header->creator;
		 cmmId = (long)icco->header->cmmId;
		 majv = (int)icco->header->majv;
		 minv = (int)icco->header->minv;
		 bfv = (int)icco->header->bfv;
         year = icco->header->date.year;
		 month = icco->header->date.month;
		 day = icco->header->date.day;
		 hours = icco->header->date.hours;
		 minutes = icco->header->date.minutes;
		 seconds = icco->header->date.seconds;
		 platform = (long)icco->header->platform;
		 illuminant_X = icco->header->illuminant.X;
		 illuminant_Y = icco->header->illuminant.Y;
		 illuminant_Z = icco->header->illuminant.Z;
		 bExists = TRUE;
		 }
	  }
   icco->del(icco);   
   fp->del(fp);
*/	
   return bRet;
   }

CICCCacheElement::CICCCacheElement()
   {
   }

CICCCacheElement::~CICCCacheElement()
   {
   }

CICCCache::CICCCache()
   {
   iPercent = 2; // arbitrary
   }

CICCCache::~CICCCache()
   {
   // write the cache out
   Write();

   // and free up the memory
   DeleteElements();
   }
      
void CICCCache::Init(LPCSTR csCacheFileName, LPCSTR csCacheFolder)
   {
   CFile cf;   
   int i;
   CICCCacheElement* pElement;
   CFileStatus cfs;

   // save this
   csFolder = csCacheFolder;
   // make sure its ok
   VERIFYPATH(csFolder);

   // and save this
   csFileName = csCacheFolder;
   csFileName += csCacheFileName;

   // open the cache and read the contents
   if (cf.Open(csFileName, CFile::modeRead))
	  {
	  try
		 {
		 CArchive ar(&cf, CArchive::load);
         coElements.Serialize(ar);

		 // check for existence of cache files, if they don't exist
		 // delete the element
		 for (i = coElements.GetSize() - 1; i >= 0; i--)
			{
			if (pElement = (CICCCacheElement*)coElements.GetAt(i))
			   {
			   if (!(CFile::GetStatus(pElement->csFileName, cfs)))
				  {				  
				  delete pElement;
				  coElements.RemoveAt(i);
				  }
			   }
			}
		 }
      catch(CException* theException)
		 {      
		 // oops
		 theException->Delete();
		 }
	  cf.Close();
	  }			 
   }

void CICCCache::Clear()
   {
   // delete all the elements in the secondary cache
   // and delete the files
   DeleteElements(TRUE);
   // delete the cache file
   ::DeleteFile(csFileName);
   }

void CICCCache::DeleteElements(BOOL bDeleteDiskFile)
   {
   int i;
   CICCCacheElement* pElement;

   try
	  {
	  for (i = coElements.GetSize() - 1; i >= 0; i--)
		 {		 
		 if (pElement = (CICCCacheElement*)coElements.GetAt(i))
			{
			// delete the disk file?
			if (bDeleteDiskFile) ::DeleteFile(pElement->csFileName);		 
			delete pElement;			
			}
		 }     
	  coElements.RemoveAll();
	  }
   catch(...)
	  {
	  }
   }

BOOL CPhotogizeUtils::GetDefaultDisplayProfile(CString& csFileName)
   {
   HDC hDC;
   char pTempBuf[_MAX_PATH];
   BOOL bRet = FALSE;
   DWORD dwLen = _MAX_PATH;

   csFileName.Empty();

   if (hDC = ::CreateDC("DISPLAY", NULL, NULL, NULL))
	  {
	  if (bRet = GetICMProfile(hDC, &dwLen, pTempBuf))
		 {
		 csFileName = pTempBuf;
		 bRet = TRUE;
		 }
	  ::DeleteDC(hDC);
	  }				 
   return bRet;
   }

CICCCacheElement* CPhoto::CheckCache(CEmbeddedICCInfo* pEmbeddedICCInfo)
   {
   int i;
   CICCCacheElement* pElement = NULL;
   CICCCacheElement* pICCRet = NULL;
   CFileStatus cfs;

   if (!pICCCache) return NULL;

#ifndef USECACHE
   return NULL;
#endif

   // Initialize the critical section.
   // should put these in try/finally code, but the code
   // has objects that must unwind, so we will risk an exception
   // in getting a critical section for code simplicity
   // we might want to recode this at a later date
   EnterCriticalSection(&pICCCache->CriticalSection); 	  

   try 
	  {
	  // look through the cache for a matching EmbeddedICCInfo
	  for (i = pICCCache->coElements.GetSize() - 1; i >= 0; i--)
		 {		 
		 if (pElement = (CICCCacheElement*)pICCCache->coElements.GetAt(i))
			{
			// if the file is missing, remove it
			if (!(CFile::GetStatus(pElement->csFileName, cfs)))
			   {				  
			   delete pElement;
			   pICCCache->coElements.RemoveAt(i);
			   continue;
			   }
			if (pElement->EmbeddedICCInfo == *pEmbeddedICCInfo)
			   {
			   // a match
			   pICCRet = pElement; 			   
			   break;
			   }
			}		
		 }			 
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   
   // Release ownership of the critical section.
   LeaveCriticalSection(&pICCCache->CriticalSection);      

   return pICCRet;
   }

BOOL CPhoto::GetEmbeddedICCProfile(CString& csProfileFile)
   {
   BOOL bRet = FALSE;   
   CICCCacheElement* pICCCacheElement;
   CString csPhotoFile;

   if (!pICCCache) return FALSE;

   if (!GetOriginalImageFileName(csPhotoFile)) return FALSE;

   // try to load the icc info if we haven't yet
   if (!EmbeddedICCInfo.bScanned)
	  {	  
	  EmbeddedICCInfo.GetICCInfo(csPhotoFile);
	  }

   // just return FALSE if we have no embedded profile
   if (!EmbeddedICCInfo.bExists)
	  return FALSE;

   // we've got embedded info, see if the file is in the cache
   if (pICCCacheElement = CheckCache(&EmbeddedICCInfo))
	  {
	  csProfileFile = pICCCacheElement->csFileName;
	  bRet = TRUE;
	  }
   else
	  {
	  if (pICCCacheElement = new CICCCacheElement)
		 {
		 // create temp filename
		 GetTempFileName(pICCCache->csFolder, "icc", 0, 
			pICCCacheElement->csFileName.GetBuffer(_MAX_PATH));
		 pICCCacheElement->csFileName.ReleaseBuffer();
		 // need to extract the file
		 if (EmbeddedICCInfo.ExtractICCtoFile(csPhotoFile, pICCCacheElement->csFileName))
			{
			csProfileFile = pICCCacheElement->csFileName;
			pICCCacheElement->EmbeddedICCInfo = EmbeddedICCInfo;			
			pICCCache->coElements.Add((CObject*)pICCCacheElement);
			bRet = TRUE;
			}
		 }		 
	  }
   return bRet;
   }

BOOL CEmbeddedICCInfo::ExtractICCtoFile(LPCSTR lpcstrIn, LPCSTR lpcstrOut)
   {
   CFile cfIn;
   CFile cfOut;
   BYTE* buf;
   BOOL bRet = FALSE;         

   if (buf = new BYTE[size])
	  {
	  if (cfIn.Open(lpcstrIn, CFile::modeRead))
		 {
		 if (cfOut.Open(lpcstrOut, CFile::modeCreate | CFile::modeWrite))
			{
			if (cfIn.Seek(fpos, CFile::begin) == fpos)
			   {
			   if (cfIn.Read(buf, size) == size)
				  {
				  cfOut.Write(buf, size);
				  bRet = TRUE;
				  }
			   }
			cfOut.Close();
			}
		 cfIn.Close();
		 }
	  delete buf;
	  }
   return bRet;
   }

CRedEyeAdjust::CRedEyeAdjust()
   {
   rcNewColor = RGB(0, 0, 0);   
   rcSetRegion.SetRectEmpty();
   uThreshold = 142;	
   nLightness = 100;
   iOriginalWidth = 100;
   iOriginalHeight = 100;
   }

CRedEyeAdjust::~CRedEyeAdjust()
   {
   }

BOOL CRedEyeAdjust::operator==(const CRedEyeAdjust& iadj)
   {
   // is same thing?
   return (IsSame(iadj));
   }
					  
BOOL CRedEyeAdjust::operator!=(const CRedEyeAdjust& iadj)
   {
   // is same thing?
   return (!IsSame(iadj));
   }

BOOL CRedEyeAdjust::IsSame(const CRedEyeAdjust& iadj)
   {               
   // are they the same?
   if (rcNewColor == iadj.rcNewColor &&
	  rcSetRegion == iadj.rcSetRegion &&
	  uThreshold == iadj.uThreshold &&
	  nLightness == iadj.nLightness &&
      iOriginalWidth == iadj.iOriginalWidth &&
	  iOriginalHeight == iadj.iOriginalHeight)
	  return TRUE;
   else
	  return FALSE;
   }

void CRedEyeAdjust::Serialize(CArchive& ar)
   {           
   try
	  {
	  if (ar.IsStoring())
		 {
		 ar << rcNewColor << rcSetRegion << uThreshold << nLightness <<
			iOriginalWidth << iOriginalHeight;
		 }
	  else
		 {
		 ar >> rcNewColor >> rcSetRegion >> uThreshold >> nLightness >>
			iOriginalWidth >> iOriginalHeight;
		 }	  
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   }

const CRedEyeAdjust& CRedEyeAdjust::operator=(const CRedEyeAdjust& cpSrc) 
   {
   // assignment	  
   if (this != &cpSrc)
      {   
      rcNewColor = cpSrc.rcNewColor;	  
      rcSetRegion = cpSrc.rcSetRegion;
	  uThreshold = cpSrc.uThreshold;
	  nLightness = cpSrc.nLightness;
	  iOriginalWidth = cpSrc.iOriginalWidth;
	  iOriginalHeight = cpSrc.iOriginalHeight;
	  }
   return *this;   
   }			   

void CPhotogizeUtils::DoRedEyeAdjustment(BITMAPHANDLE* pBitmapCorrected, 
   CImageAdjustment& cImageAdjust)
   {
   CRedEyeAdjust* pRedEyeAdjust;
   int i;
   CRect crScaledRect;
   
   for(i = 0; i < cImageAdjust.coRedEyeArray.GetSize(); i++)
	  {
	  if (pRedEyeAdjust = (CRedEyeAdjust*)cImageAdjust.coRedEyeArray.GetAt(i))
		 {
		 // scale from the original
		 crScaledRect.left = MulDiv(pRedEyeAdjust->rcSetRegion.left, 
			max(pBitmapCorrected->Width, pBitmapCorrected->Height),
			max(pRedEyeAdjust->iOriginalWidth, pRedEyeAdjust->iOriginalHeight));
		 crScaledRect.right = MulDiv(pRedEyeAdjust->rcSetRegion.right, 
			max(pBitmapCorrected->Width, pBitmapCorrected->Height),
			max(pRedEyeAdjust->iOriginalWidth, pRedEyeAdjust->iOriginalHeight));
		 crScaledRect.top = MulDiv(pRedEyeAdjust->rcSetRegion.top, 
			max(pBitmapCorrected->Width, pBitmapCorrected->Height),
			max(pRedEyeAdjust->iOriginalWidth, pRedEyeAdjust->iOriginalHeight));
		 crScaledRect.bottom = MulDiv(pRedEyeAdjust->rcSetRegion.bottom, 
			max(pBitmapCorrected->Width, pBitmapCorrected->Height),
			max(pRedEyeAdjust->iOriginalWidth, pRedEyeAdjust->iOriginalHeight));

		 // set the degion
		 ::L_SetBitmapRgnEllipse(pBitmapCorrected, NULL, &crScaledRect, L_RGN_SET);
		 // do the red eye
		 ::L_RemoveRedeyeBitmap(pBitmapCorrected, pRedEyeAdjust->rcNewColor, 
			pRedEyeAdjust->uThreshold, pRedEyeAdjust->nLightness);      

		 // DEBUG!!!
		 // ::L_ChangeBitmapIntensity(pBitmapCorrected, +1000);  

		 ::L_FreeBitmapRgn(pBitmapCorrected);
		 }
	  }
   }

int CPhotogizeUtils::GetInstalledMemory()
   {
   MEMORYSTATUS mmi;   
   
   GlobalMemoryStatus(&mmi);
   
   return mmi.dwTotalPhys;
   }

int CPhotogizeUtils::GetFreePhysicalMemory()
   {
   MEMORYSTATUS mmi;   
   
   GlobalMemoryStatus(&mmi);
   
   return mmi.dwAvailPhys;
   }

int CPhotogizeUtils::GetFreeVirtualMemory()
   {
   MEMORYSTATUS mmi;   

   GlobalMemoryStatus(&mmi);
   
   return mmi.dwAvailPageFile;
   }

int CPhotogizeUtils::GetPercentMemoryLoad()
   {
   MEMORYSTATUS mmi;   
   
   GlobalMemoryStatus(&mmi);
   
   return mmi.dwMemoryLoad;
   }

CPromo::CPromo()
   {
   iType = (PROMOTYPE_DISCOUNT_PERCENT | PROMOTYPE_APPLYTO_ALL);
   iAmount = 0;   
   iMinOrder = 0;
   CTime ctCurrent = CTime::GetCurrentTime();
   
   // default start time is now, end time is a week from now
   CTime ct(ctCurrent.GetYear(), ctCurrent.GetMonth(), ctCurrent.GetDay(), 0, 0, 0);

   ctStart = ct;   
   CTimeSpan ctWeek(7, 0, 0, 0);
   ctEnd = ctStart + ctWeek;      
   }

CPromo::~CPromo()
   {
   }

const CPromo& CPromo::operator=(const CPromo& cpSrc) 
   {  
   // assignment
   if (this != &cpSrc)
      {   	  
	  csName = cpSrc.csName;
	  csCode = cpSrc.csCode;
	  iType = cpSrc.iType;
	  csApplyTo = cpSrc.csApplyTo;
	  csDescription = cpSrc.csDescription;
	  iMinOrder = cpSrc.iMinOrder;
	  iAmount = cpSrc.iAmount;
	  ctStart = cpSrc.ctStart;
	  ctEnd = cpSrc.ctEnd;
	  }
   return *this;   
   }			   

BOOL CPromo::operator==(const CPromo& iprm)
   {
   // is same thing?
   return (IsSame(iprm));
   }
					  
BOOL CPromo::operator!=(const CPromo& iprm)
   {
   // is same thing?
   return (!IsSame(iprm));
   }

BOOL CPromo::IsSame(const CPromo& iprm)
   {         
   // are they the same?
   if (csName == iprm.csName && csCode == iprm.csCode && iType == iprm.iType && 
	  iAmount == iprm.iAmount && csApplyTo == iprm.csApplyTo && 
	  csDescription == iprm.csDescription && 
	  iMinOrder == iprm.iMinOrder &&
	  ctStart == iprm.ctStart &&
	  ctEnd == iprm.ctEnd)
	  return TRUE;	  
   else
	  return FALSE;
   }

void CPromo::Serialize(CArchive& ar)
   {           
   try
	  {
	  if (ar.IsStoring())
		 {
		 ar << csName << csCode << iType << iAmount << 
			csApplyTo << iMinOrder << csDescription <<
			ctStart << ctEnd;
		 }
	  else
		 {
		 ar >> csName >> csCode >> iType >> iAmount >> 
			csApplyTo >> iMinOrder >> csDescription >>
			ctStart >> ctEnd;
		 }	  
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   }

BOOL CAccount::IsProfileFilledOut()
   {
   if (m_profile_company.IsEmpty() ||
       m_profile_biz_name.IsEmpty() || 
	   m_profile_tech_name.IsEmpty() ||
	   m_profile_lab_name.IsEmpty() ||
	   m_profile_biz_email.IsEmpty() ||
	   m_profile_tech_email.IsEmpty() ||
	   m_profile_lab_email.IsEmpty())	  
	   return FALSE;
   else
	  return TRUE;
   }

void CPhotogizeUtils::CopyOrderArchiveToTemp(CString& csArchiveFile)
   {
   CString csTempFileName;
   CString csTempPath;

   GetAppPath(csTempPath);
	
   csTempPath += "OrderArchiveBackup\\";
	
   if ((::CreateDirectory(csTempPath, NULL) == 0) && 
	  ::GetLastError() != ERROR_ALREADY_EXISTS)		 		 
	  return;

   GetTempFileName(csTempPath, "arc", 0, csTempFileName.GetBuffer(_MAX_PATH));
   csTempFileName.ReleaseBuffer();

   CopyFile(csArchiveFile, csTempFileName, FALSE);
   }

BOOL CPhotogizeUtils::CheckArchivesForDupeOrders(CString& csOrderID)
   {
   CFileFind ff;   
   CString csTempFileName;
   CString csTempPath;
   CFileStatus cfs;	  
   CTimeSpan cts;
   CObArray coNodesTemp;
   BOOL bRet = FALSE;
   int i;
   CNode* pNode;
   CString csCompare;
   CString csCompareTo;

   GetAppPath(csTempPath);
	
   csTempPath += "OrderArchiveBackup\\*.*";	

   BOOL res = ff.FindFile(csTempPath);

   try
	  {   
	  while(res)
		 {
		 res = ff.FindNextFile();
		 if (ff.IsDots())
			continue;
		 if (ff.IsDirectory())
			continue;
		 else
			{
			// get date and time, if it is older than 30 days, delete it
			if (CFile::GetStatus(ff.GetFilePath(), cfs))
			   {
			   // get time span
			   cts = CTime::GetCurrentTime() - cfs.m_ctime;

			   if (cts.GetDays() > 30)
				  ::DeleteFile(ff.GetFilePath());
			   else
				  {
				  // load the file into memory
				  CFile myFile(ff.GetFilePath(), CFile::modeRead);
				  CArchive ar(&myFile, CArchive::load);   
				  // Serialize the nodes from the archive.to standard orders
				  coNodesTemp.Serialize(ar);    	  
				  ar.Close();
				  myFile.Close();				  
				  // look for the order
				  i = 0;
				  while (i < coNodesTemp.GetSize() )
					 {
					 if (pNode = (CNode*)coNodesTemp.GetAt(i++))		 		 
						{					
						if (pNode)
						   {
						   // make sure it's an order
						   if (pNode->iType == NODETYPE_ORDER)
							  {							   						   
							  // copy the name
							  csCompare = pNode->csName;
							  csCompareTo = csOrderID;

							  // get rid of "partial"
							  csCompare.Replace("(partial)", "");

							  // get rid of whitespace
							  csCompare.TrimLeft();
							  csCompare.TrimRight();
							  csCompareTo.TrimLeft();
							  csCompareTo.TrimRight();

							  if (csCompareTo == csCompare)
								 {
								 bRet = TRUE;
								 break;
								 }
							  }	 						   
						   }
						}
					 }     		
				  // and finally, delete the array
				  DeleteOrderArray(coNodesTemp);
				  }
			   }
			}
		 }
	  }	
   catch(...)
	  {			
	  }		
   return bRet;
   }

void CPhotogizeUtils::DeleteOrderArray(CObArray& coNodesTemp)
   {
   int i;
   CNode* pNode;

   // delete the members array
   i = 0;
   while (i < coNodesTemp.GetSize() )
      {
	  if (pNode = (CNode*)coNodesTemp.GetAt(i++))		 		 
		 {					
		 try
			{
		    if (pNode) delete pNode;			
			}
		 catch(...)
			{			
			}
		 }
      }     
   coNodesTemp.RemoveAll();
   }
/*
void CPhotogizeUtils::ICCColorAdjust(CTransformCMM* pTransformCMM, 
   BITMAPHANDLE* pBitmap)
   {
   BYTE* pBuf;   
   int row;

   ::L_AccessBitmap(pBitmap);
	  
   if (pBuf = new BYTE[pBitmap->BytesPerLine])
	  {
	  for (row = 0; row < pBitmap->Height; row++)
		 {
		 if (::L_GetBitmapRow(pBitmap, pBuf, row, pBitmap->BytesPerLine))
			{
			pTransformCMM->PrejudgeTranslate(pBuf, pBuf, pBitmap->Width);
			::L_PutBitmapRow(pBitmap, pBuf, row, pBitmap->BytesPerLine);
			}
		 }
	  delete pBuf;
	  }
   ::L_ReleaseBitmap(pBitmap);	      
   }
*/
void CPhotogizeUtils::ForceJPGExtension(CString& csOriginalFileName)
   {
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   CString csNewFileName;

   // get the original name
   _splitpath(csOriginalFileName, drive, dir, fname, ext);
   csNewFileName = drive;
   csNewFileName += dir;
   csNewFileName += fname;
   // add a jpg
   csNewFileName += ".jpg";

   // rename it
   try
	  {
	  CFile::Rename(csOriginalFileName, csNewFileName);
	  }
   catch(...)
	  {
	  }
   }

BOOL CAccount::PublishXConnectData()
   {
   BOOL bRet = FALSE;
/*
   XHarvest xharvest;
   CString csFileName;
   XProduct* pxProduct;
   CProduct* pProduct;
   int i;
   CObArray coProducts;
   CObArray coXShipList;
   CStringArray csaXPickupLocations;
   CShipListEntry* pShipListEntry;
   XShipListEntry* pxShipListEntry;
   CObArray coXPromotions;
   CString csTemp;
   XPromo* pxPromo;
   CPromo* pPromo;

   csFileName = csXConnectPath;
   VERIFYPATH(csFileName);
   csFileName += "server.xml";   

   // add the products
   for (i = 0; i < pParentProduct->coNodes.GetSize(); i++)
	  {
	  if (pProduct = (CProduct*)pParentProduct->coNodes.GetAt(i))
		 {
		 if (pxProduct = new XProduct)
			{
			pxProduct->csDevice = pProduct->csDevName;
			pxProduct->csDescription = pProduct->csDescription;
			pxProduct->csName = pProduct->csName;
			
			if (pProduct->bCustomPageSize)
			   {
			   pxProduct->iPrintWidthInches1000 = pProduct->iPrintWidthInches1000;
			   pxProduct->iPrintHeightInches1000 = pProduct->iPrintHeightInches1000;
			   }
			else
			   {
			   pxProduct->iPrintWidthInches1000 = pProduct->iMediaWidthInches1000;
			   pxProduct->iPrintHeightInches1000 = pProduct->iMediaHeightInches1000;
			   }
			pxProduct->iUnitPricePennies = pProduct->iUnitPricePennies;
			pxProduct->iWeightOunces100 = pProduct->iWeightOunces100;
			pxProduct->iNumPriceBreaks = pProduct->iNumPriceBreaks;
			pxProduct->bUsePriceBreaks = pProduct->bUsePriceBreaks;
			memcpy(pxProduct->iFrom, pProduct->iFrom, MAXPRODUCTPRICEBREAKS *
			   sizeof(int));
			memcpy(pxProduct->iTo, pProduct->iTo, MAXPRODUCTPRICEBREAKS *
			   sizeof(int));
			memcpy(pxProduct->iPricePennies, pProduct->iPricePennies, 
			   MAXPRODUCTPRICEBREAKS * sizeof(int));

			coProducts.Add((CObject*)pxProduct);
			}	 
		 }
	  }
   // add the ship list
   for (i = 0; i < coShipList.GetSize(); i++)
	  {
	  if (pShipListEntry = (CShipListEntry*)coShipList.GetAt(i))
		 {
		 if (pxShipListEntry = new XShipListEntry)
			{
			pxShipListEntry->csName = pShipListEntry->csName;
			pxShipListEntry->iNumBreaks = pShipListEntry->iNumBreaks;
			pxShipListEntry->iBaseHandlingPricePennies = 
			   pShipListEntry->iBaseHandlingPricePennies;
			memcpy(pxShipListEntry->iQtyFrom, pShipListEntry->iQtyFrom, 
			   MAXSHIPPRICEMATRIX * sizeof(int));
			memcpy(pxShipListEntry->iQtyTo, pShipListEntry->iQtyTo, 
			   MAXSHIPPRICEMATRIX * sizeof(int));
			memcpy(pxShipListEntry->iPricePennies, pShipListEntry->iPricePennies, 
			   MAXSHIPPRICEMATRIX * sizeof(int));
			coXShipList.Add((CObject*)pxShipListEntry);
			}
		 }
	  }

   // add the pickuplocations
   for (i = 0; i < csaPickupLocations.GetSize(); i++)
	  {
	  csTemp = csaPickupLocations.GetAt(i);
	  csaXPickupLocations.Add(csTemp);
	  }

   // add the promotions
   for (i = 0; i < coPromotions.GetSize(); i++)
	  {
	  if (pPromo = (CPromo*)coPromotions.GetAt(i))
		 {
		 if (pxPromo = new XPromo)
			{
			pxPromo->csApplyTo = pPromo->csApplyTo;
			pxPromo->csCode = pPromo->csCode;
			pxPromo->csDescription = pPromo->csDescription;
			pxPromo->csName = pPromo->csName;
			pxPromo->ctEnd = pPromo->ctEnd;
			pxPromo->ctStart = pPromo->ctStart;
			pxPromo->iAmount = pPromo->iAmount;
			pxPromo->iMinOrder = pPromo->iMinOrder;
			pxPromo->iType = pPromo->iType;
			coXPromotions.Add((CObject*)pxPromo);
			}
		 }
	  }

   // publish the data
   int iTax, iTaxShip;
   if (m_addtax)
      iTax = (int)(m_taxpercent * 1000.F);
   else
	  iTax = 0;
   if (m_addtaxship)
      iTaxShip = (int)(m_taxpercent * 1000.F);
   else
	  iTaxShip = 0;

   int iCCSupported = CREDITCARDTYPE_NONE;
   if (m_use_visa) iCCSupported += CREDITCARDTYPE_VISA;
   if (m_use_mc) iCCSupported += CREDITCARDTYPE_MC;
   if (m_use_amex) iCCSupported += CREDITCARDTYPE_AMEX;
   if (m_use_dclub) iCCSupported += CREDITCARDTYPE_DCLUB;   
   if (m_use_disco) iCCSupported += CREDITCARDTYPE_DISCO;   
   // add the rule
   switch(m_ccrule)
	  {
	  case 0:
		   iCCSupported += CREDITCARDRULE_ALWAYSREQUIRE;
		   break;
	  case 1:
		   iCCSupported += CREDITCARDRULE_FORSHIPMENTONLY;
		   break;
	  case 2:
		   iCCSupported += CREDITCARDRULE_NEVERREQUIRE;
		   break;
	  }

   if (m_securitycode)
	  iCCSupported += CREDITCARDRULE_USESECCODE;

   bRet = xharvest.PublishServerData(csFileName, coProducts,
	  csaXPickupLocations, coXShipList, coXPromotions, 
	  iTax, iTaxShip, csaTaxStates, iCCSupported);

   // remove the products
   for (i = coProducts.GetSize() - 1; i >=0; i--)
	  {
	  if (pxProduct = (XProduct*)coProducts.GetAt(i))
		 {
		 delete pxProduct;
		 }
	  }
   coProducts.RemoveAll();

   // remove the ship list
   for (i = coXShipList.GetSize() - 1; i >=0; i--)
	  {
	  if (pxShipListEntry = (XShipListEntry*)coXShipList.GetAt(i))
		 {
		 delete pxShipListEntry;
		 }
	  }
   coXShipList.RemoveAll();

   // remove the promo list
   for (i = coXPromotions.GetSize() - 1; i >=0; i--)
	  {
	  if (pxPromo = (XPromo*)coXPromotions.GetAt(i))
		 {
		 delete pxPromo;
		 }
	  }
   coXPromotions.RemoveAll();
*/
   return bRet;
   }

void CPhoto::SeedCropBox(CProduct* pProduct)
   {
   float fPicAspect, fDstAspect;
   float fWidth;
   float fHeight;
   CImage* pImageOriginal;   

   if (!pProduct) return;

   // get the original image
   if (!(pImageOriginal = GetOriginalImage()))
	  return;

   // get aspect ratio of source
   if (cImageAdjust.iRotate == 0 || cImageAdjust.iRotate == 180 || 
	  cImageAdjust.iRotate == -180)
      fPicAspect = (float)pImageOriginal->iHeight / (float)pImageOriginal->iWidth;
   else
	  fPicAspect = (float)pImageOriginal->iWidth / (float)pImageOriginal->iHeight;

  // find the media dims
   if (pProduct->bCustomPageSize)
	  {
	  fWidth = (float)pProduct->iPrintWidthInches1000;
	  fHeight = (float)pProduct->iPrintHeightInches1000;	  
	  }
   else 
	  {
	  fWidth = (float)pProduct->iMediaWidthInches1000;
	  fHeight = (float)pProduct->iMediaHeightInches1000;	  
	  }

   // initial aspect ratio
   fDstAspect = fHeight / fWidth;

   // flip it to match
   if ((fPicAspect < 1.0 && fDstAspect > 1.0) || (fPicAspect > 1.0 && fDstAspect < 1.0))
	  fDstAspect = fWidth / fHeight;   

   // initial value of crop box
   crCrop.SetRectEmpty();   

   if (cImageAdjust.iRotate == 0 || cImageAdjust.iRotate == 180 || 
	  cImageAdjust.iRotate == -180)
	  {
	  if (fDstAspect > fPicAspect)
		 {
		 fWidth = (float)pImageOriginal->iWidth * fPicAspect / fDstAspect;
		 crCrop.left = (int)(((float)pImageOriginal->iWidth - fWidth) / 2. + .5);
		 crCrop.right = crCrop.left + (int)(fWidth + .5);	  
		 crCrop.top = 0;
		 crCrop.bottom = pImageOriginal->iHeight;
		 }	  
	  else
		 {
		 fHeight = (float)pImageOriginal->iHeight * fDstAspect / fPicAspect;
		 crCrop.top = (int)(((float)pImageOriginal->iHeight - fHeight) / 2. + .5);
		 crCrop.bottom = crCrop.top + (int)(fHeight + .5);
		 crCrop.left = 0;
		 crCrop.right = pImageOriginal->iWidth;
		 }
	  }
   else
	  {
	  if (fDstAspect > fPicAspect)
		 {
		 fWidth = (float)pImageOriginal->iHeight * fPicAspect / fDstAspect;
		 crCrop.top = (int)(((float)pImageOriginal->iHeight - fWidth) / 2. + .5);
		 crCrop.bottom = crCrop.top + (int)(fWidth + .5);	  
		 crCrop.left = 0;
		 crCrop.right = pImageOriginal->iWidth;
		 }	  
	  else
		 {
		 fHeight = (float)pImageOriginal->iWidth * fDstAspect / fPicAspect;
		 crCrop.left = (int)(((float)pImageOriginal->iWidth - fHeight) / 2. + .5);
		 crCrop.right = crCrop.left + (int)(fHeight + .5);
		 crCrop.top = 0;
		 crCrop.bottom = pImageOriginal->iHeight;
		 }
	  }	  
   }

void CPhoto::GetMediaRectForFit(int& iPicWidth, int& iPicHeight, CProduct* pProduct, CRect& crMediaRect)
   {   
   float fWidth;
   float fHeight;      
   float fPicAspect;
   float fMediaAspect;
   float fPicWidth;
   float fPicHeight;
   
   // find the media dims
   if (pProduct->bCustomPageSize)
	  {
	  fWidth = (float)pProduct->iPrintWidthInches1000;
	  fHeight = (float)pProduct->iPrintHeightInches1000;	  
	  }
   else 
	  {
	  fWidth = (float)pProduct->iMediaWidthInches1000;
	  fHeight = (float)pProduct->iMediaHeightInches1000;	  
	  }

   fPicWidth = (float)iPicWidth;
   fPicHeight = (float)iPicHeight;

   // get landscape aspect ratios
   fPicAspect = min(fPicHeight, fPicWidth) / max(fPicHeight, fPicWidth);
   fMediaAspect = min(fHeight, fWidth) / max(fHeight, fWidth);

   // 0 based rect
   crMediaRect.left = 0;
   crMediaRect.top = 0;

   if (fPicAspect > fMediaAspect)
	  {
	  crMediaRect.bottom = iPicHeight;
	  crMediaRect.right = (int)((float)iPicHeight / fMediaAspect + .5F);
	  }
   else
	  {
	  crMediaRect.right = iPicWidth;
	  crMediaRect.bottom = (int)((float)iPicWidth * fMediaAspect + .5F);
	  }
   }

void CPhoto::FitInMedia(int& iPicWidth, int& iPicHeight, CRect& crDstRect, CProduct* pProduct)
   {      
   float fSrcAspect;
   float fDstAspect;   
   CRect crDrawingSpace;
   float fWidth;
   float fHeight;
   CRect crMediaRect;

   // get the media rect
   GetMediaRectForFit(iPicWidth, iPicHeight, pProduct, crMediaRect);

   // get sizing
   // the destination is the media rect
   crDstRect = crMediaRect;
   // get aspect ratio of source
   fSrcAspect = (float)iPicHeight / (float)iPicWidth;
   // get aspect ratio of target
   fDstAspect = (float)crDstRect.Height() / (float)crDstRect.Width();
   
   fWidth = (float)crDstRect.Width();
   fHeight = (float)crDstRect.Height();
   
   // modify the destination with white space on the top and bottom
   if (fSrcAspect < fDstAspect)
	  {
	  crDstRect.top += (int)((fHeight - fSrcAspect * fWidth) / 2. + .5);
	  crDstRect.bottom -= (int)((fHeight - fSrcAspect * fWidth) / 2. + .5);		 
	  }
   else
	  {		 
	  crDstRect.left += (int)((fWidth - fHeight / fSrcAspect) / 2. + .5);
	  crDstRect.right -= (int)((fWidth - fHeight / fSrcAspect) / 2. + .5);		 
	  }	 
   }

BOOL CPhoto::CreateFitBitmap(BITMAPHANDLE* pBitmapSrc, CRect& crFitRect)
   {
   BITMAPHANDLE TmpBitmap;
   BOOL bSuccess = FALSE;

   if (::L_CreateBitmap(&TmpBitmap, sizeof(BITMAPHANDLE), TYPE_CONV, crFitRect.Width() + crFitRect.left * 2, 
							crFitRect.Height() + crFitRect.top * 2, 24, 
								ORDER_BGR, NULL, TOP_LEFT, NULL, 0) == SUCCESS)
	  {
	  // fill it with white
	  if (::L_FillBitmap(&TmpBitmap, RGB(255, 255, 255)) == SUCCESS)		 
		 {
		 // and copy the old one into the new one
		 if (::L_CombineBitmap(&TmpBitmap, crFitRect.left, crFitRect.top, pBitmapSrc->Width, 
			pBitmapSrc->Height, pBitmapSrc, 0, 0, L_SRC_COPY) == SUCCESS)
			{
			// free the original bitmap
			::L_FreeBitmap(pBitmapSrc);
			// and copy over the original
			memcpy(pBitmapSrc, &TmpBitmap, sizeof BITMAPHANDLE);
			bSuccess = TRUE;
			}
		 }	 
	  // release the temp bitmap if in error
	  if (!bSuccess) ::L_FreeBitmap(&TmpBitmap);
	  }									   
   return bSuccess;
   }

const CCheckListItem& CCheckListItem::operator=(const CCheckListItem& cpSrc) 
   {
   // assignment
   if (this != &cpSrc)
      {   
      csStr = cpSrc.csStr;
	  iCheck = cpSrc.iCheck;
	  }
   return *this;   
   }			   

void CCheckListItem::Serialize(CArchive& ar)
   {           
   try
	  {
	  if (ar.IsStoring())
		 {
		 ar << csStr << iCheck;
		 }
	  else
		 {
		 ar >> csStr >> iCheck;
		 }	  
	  }
   catch(CException* theException)
	  {      
	  // oops
	  theException->Delete();
	  }
   }

BOOL COrder::MakeBarCode()
   {
/*
   ERRCODE eCode = S_OK;
   t_BarCode* m_pBarCode;
   CString csTempPath;
   
   // do the license
   BCLicenseMe ("Graphx, Woburn", (e_licKind)3,
	  1, "D1F438BA", (e_licProduct)5);		
   // Allocate and initialize bar code structure
   if (BCAlloc (&m_pBarCode) == S_OK)
	  {
	  // set the text height
      LOGFONT* plfFont;
	  plfFont = BCGetLogFont (m_pBarCode);
	  plfFont->lfHeight = -50;
	  BCSetLogFont (m_pBarCode, plfFont);	

	  // set the bar code text
	  eCode = (eCode == S_OK) ? BCSetText(m_pBarCode, csDisplayName, 
		 csDisplayName.GetLength()) : eCode;
	  // set the type to code 39 ascii	  
	  eCode = (eCode == S_OK) ? BCSetBCType(m_pBarCode, eBC_3OF9) : eCode;
	  // check digit method
	  eCode = (eCode == S_OK) ? BCSetCDMethod(m_pBarCode, eCDStandard) : eCode;
	  eCode = (eCode == S_OK) ? BCSetPrintText(m_pBarCode, FALSE, 
		 FALSE) : eCode;
      eCode = (eCode == S_OK) ? BCSetRotation(m_pBarCode, deg0) : eCode;

	  // check it
	  eCode = (eCode == S_OK) ? BCCheck	(m_pBarCode) : eCode;

	  // check the cd type
	  eCode = (eCode == S_OK) ? BCCalcCD	(m_pBarCode) : eCode;
	  
	  // create it
	  eCode = (eCode == S_OK) ? BCCreate	(m_pBarCode) : eCode;
	  // save it
      CPhotogizeUtils::MakeTempFileName(csBarCodeFileName, "bar");
      eCode = (eCode == S_OK) ? BCSaveImage(m_pBarCode, csBarCodeFileName, 
		 eIMBmp, 1200, 300, 300, 300) : eCode;

	  BCFree (m_pBarCode);
	  }
   // error check
   if (eCode != S_OK)	  
	  return FALSE;	  
   else
	  {
	  return TRUE;
	  }
*/
	  return FALSE;	  
   }

BOOL COrder::GetBarCodeFileName(CString& csFileName)
   {
   BOOL bRet;   
   BOOL m_oninvoice;   

   // do we want to put it on the invoice?
   if (!(m_oninvoice = AfxGetApp()->GetProfileInt("Barcode", "Invoice", FALSE)))
	  return FALSE;   

   if (csBarCodeFileName.GetLength() > 0)
	  {
	  csFileName = csBarCodeFileName;
	  return TRUE;
	  }

   if (bRet = MakeBarCode())
	  csFileName = csBarCodeFileName;
   else
	  csBarCodeFileName.Empty();

   return bRet;
   }

BOOL COrder::ResetStatusFromServer(CString& csNewStatus, CString& csNewDetails)
   {
   BOOL bRet = FALSE;
   CString csCurrentStatus;

   // get existing status
   GetStatus(csCurrentStatus);

   if (csCurrentStatus != csNewStatus)
	  {
	  SetStatusFromStr(csNewStatus);
	  bRet = TRUE;
	  }
/*
   if (OrderDetails.csDetails != csNewDetails)
	  {
	  OrderDetails.csDetails = "<ORDERDETAILS>";		 
	  OrderDetails.csDetails += csNewDetails;		 
	  OrderDetails.csDetails += "</ORDERDETAILS>";		 		 
	  bRet = TRUE;
	  }

   if (bRet) OrderDetails.ParseDetails();
*/
   return bRet;
   }
