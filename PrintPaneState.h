/////////////////////////////////////////////////////////////////////////////
// CPrintPaneState.h
//                                                                        
// global data holder for print data for RasterPlus
// Copyright (C) 2000 Graphx, Inc.
//

#ifndef __CPrintPaneState__H_
#define __CPrintPaneState__H_

#define MAX_GANG_FILES   80

typedef struct tagPPCOMMON
   {
   int iTotalPrintPaneEntries;   
   int iTotalGangPrintEntries;      
   POINT GangPrintDims[MAX_GANG_FILES];
   char GangPrintTempFileName[MAX_GANG_FILES][_MAX_PATH];
   int iMultiImageCopies[MAX_GANG_FILES];
   int iMultiImageMedia[MAX_GANG_FILES];
   int iFileType[MAX_GANG_FILES];
   int bFullBleed[MAX_GANG_FILES];
   char lpstrOwner[MAX_GANG_FILES][_MAX_PATH];
   int iOrderNum[MAX_GANG_FILES];	  
   char lpstrOrderNum[MAX_GANG_FILES][_MAX_PATH];
   int iSurface[MAX_GANG_FILES];	  
   int iBorderWidth[MAX_GANG_FILES];	  
   char lpstrBPLine1[MAX_GANG_FILES][_MAX_PATH];
   int iBPLine1[MAX_GANG_FILES];	  
   char lpstrBPLine2[MAX_GANG_FILES][_MAX_PATH];
   int iBPLine2[MAX_GANG_FILES];
   int iImageDataSize[MAX_GANG_FILES];
   char* pImageData[MAX_GANG_FILES];
   } PPCOMMON, *PPPCOMMON;

class CPrintPaneState : public CObject
   {
public:
   CPrintPaneState();
   ~CPrintPaneState();   
   void SetName(LPCSTR csn);
   void SetData();
   void UnSetData();
   void IncrPrintPaneEntries(int iUpTick = 1);
   void DecrPrintPaneEntries(int iDownTick = 1);   
   int GetPrintPaneEntries();   
   void SetPrintPaneEntries(int iEntries);
   BOOL bIsInit;

   // gang stuff
   BOOL AddGangPrintFile(CString& csName, int iWidth, int iHeight, int iOrderNum = 0, LPCSTR lpstrOrderNum = NULL);
   void DeleteGangPrintFile(CString& csName);
   void GetGangPrintFile(int idx, CString& csName);
   void GetGangPrintFileDims(int idx, int& iWidth, int& iHeight);
   int GetGangPrintEntries();
   int GetMultiImageCopies( int idx );
   int GetMultiImageMedia( int idx );
   void DeleteGangPrintEntry(int idx, BOOL bDeleteFile = TRUE);

   BOOL AddMultiImage(char* pImageData, int iLength);
   void GetMultiImageData(int idx, char* pImagedata);
   void UpdateMultiImage(int idx, char* pImageData, int iLength );

   BOOL AddMultiImage(int iCopies, int iMedia, LPCSTR lpstrOwner=NULL, int iOrderNum=0,
						int iFileType=0, BOOL bFullBleed = FALSE, int iSurface = 0, int iBorderWidth = 0,
							LPCSTR lpstrLine1 = NULL, BOOL bLine1 = FALSE, LPCSTR lpstrLine2 = NULL, BOOL bLine2 = FALSE);
   BOOL AddMultiImage(int iCopies, int iMedia, LPCSTR lpstrOwner=NULL, LPCSTR lpstrOrderNum=NULL,
						int iFileType=0, BOOL bFullBleed = FALSE, int iSurface = 0, int iBorderWidth = 0,
							LPCSTR lpstrLine1 = NULL, BOOL bLine1 = FALSE, LPCSTR lpstrLine2 = NULL, BOOL bLine2 = FALSE);

   void GetMultiImageOwner(int idx, CString& csOwner);
   int GetMultiImageOrderNum( int idx );
   void GetMultiImageOrderNum( int idx, CString& csOrderNum );
   int GetFileType(int idx);
   BOOL GetFullBleed(int idx);
   int GetSurface(int idx);
   int GetBorderWidth(int idx);
   void GetBackprintLine1( int idx, CString& csLine1 );
   BOOL GetLine1Status( int idx );
   void GetBackprintLine2( int idx, CString& csLine2 );
   BOOL GetLine2Status( int idx );

   BOOL IspData();

protected:
   CString csName;
   HANDLE hFileMap;
   PPPCOMMON pData;
   };
#endif
