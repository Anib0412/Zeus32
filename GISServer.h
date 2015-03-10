////////////////////////////////////////////////////////////////////////////
// GISSERVER.H
//                                                                        
// Photogize.PrintServer class for RasterPlus
// Copyright (C) 2000-2001 Graphx, Inc.
//

#ifndef __GISSERVER_H__
#define __GISSERVER_H__ 

#include "..\gis\giscmds.h"
#include "..\gis\GImageClient.h"
#include "listeningsocket.h"
#include "clientsocket.h"
#include "rippane.h"

#include "..\CmnLibs\Current LeadTools\include\l_bitmap.h"
#include "..\CmnLibs\Current LeadTools\include\l_error.h"

// our internal photo structure
typedef struct tagOURPGPRCPHOTOSPECS
   {
   PGPRCPHOTOSPECS2 PhotoSpecs;
   char lpszTempDiskFile[PGPRC_MAXSTRLEN]; 
   BOOL bUseTempFile;                
   int iPage;                        
   HANDLE hCqueueFile;			     
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
   } OURPGPRCPHOTOSPECS, *POURPGPRCPHOTOSPECS;

class CSocketJob : public CObject
   {	  
DECLARE_DYNAMIC(CSocketJob)
public:
   CSocketJob();
   ~CSocketJob();
   CString csName;
   PGPRCPRINTSPECS PGPRCPrintSpecs;  // print specs
   JOBSEPARATOR JobSeparator;	 // the job separator
   BOOL bDoJobSeparator;
   CString csJobSeparatorFile;
   int iNumSeparatorPages;
   HANDLE hDevice;               // device to print to
   CPtrList Photos;              // an array of PGPRCPHOTOSPECS from client   
   int iPhotoCounter;
   int iTotalPages;
   void DeleteAllPhotos();
   };

class CPGPRCServer : public CObject
   {
public:
   CPGPRCServer();
   ~CPGPRCServer();

   void ProcessPendingRead(CClientSocket* pSocket);
   void ProcessPendingAccept();   
   void StartSocketPolling();
   void StopSocketPolling();   
   BOOL SocketPolling;   
   BOOL StartSocketServer();
   void CheckAwake();

protected:
   BOOL bCheckAwake;
   CListeningSocket* m_pListenSocket;
   CPtrList m_connectionList;
   void CloseSocket(CClientSocket* pSocket);   
   void CloseAllSockets();
   void DoSeparatorText(CSocketJob* pJob, CStdioFile& cf);
   PGPRC_RETURNVAL GetDevInfo(int len, int idx, CClientSocket* pSocket);
   PGPRC_RETURNVAL GetCanvasInfo(int len, HANDLE hDevice, int idx, CClientSocket* pSocket);   
   PGPRC_RETURNVAL OpenJob(int len, HANDLE hDevice, CClientSocket* pSocket);
   PGPRC_RETURNVAL AddJobSeparator(int len, HANDLE hJob, CClientSocket* pSocket);   
   PGPRC_RETURNVAL AddPhoto(int len, HANDLE hJob, CClientSocket* pSocket, int iStyle);
   PGPRC_RETURNVAL PutFile(int len, HANDLE hJob, CClientSocket* pSocket);   
   PGPRC_RETURNVAL PrintJob(int len, HANDLE hJob, CClientSocket* pSocket);
   PGPRC_RETURNVAL GetJobStatus(int len, HANDLE hJob, CClientSocket* pSocket);
   PGPRC_RETURNVAL GetPhotoStatus(int len, CClientSocket* pSocket);
   PGPRC_RETURNVAL CloseJob(HANDLE hJob, CClientSocket* pSocket);
   PGPRC_RETURNVAL ServerSysMsg(int len, int iType, CClientSocket* pSocket);
   void DumpIncomingData(int len, CClientSocket* pSocket);
   PGPRC_RETURNVAL InsertJobs(CSocketJob* pJob);
   PGPRC_RETURNVAL ApplyFilter(CSocketJob* pJob, POURPGPRCPHOTOSPECS pPhoto);
   PGPRC_RETURNVAL GetInternalPhotoStatus(CSocketJob* pJob, POURPGPRCPHOTOSPECS pPhoto, 
	  PPGPRCJOBSTATUSSTRUCT pPgprcJobStatusStruct);

   PGPRC_RETURNVAL GetDevOptionName(int len, HANDLE hDevice, int iOption, 
									   CClientSocket* pSocket);
   PGPRC_RETURNVAL GetDevOptionSettingName(int len, HANDLE hDevice, int iOption, 
	  int iSetting, CClientSocket* pSocket);
   PGPRC_RETURNVAL GetDevOption(int len, HANDLE hDevice, int iOption, 
	  CClientSocket* pSocket);
   PGPRC_RETURNVAL SetDevOption(HANDLE hDevice, int iOption, int iSetting, CClientSocket* pSocket);
   PGPRC_RETURNVAL SetBackPrintLine(int len, HANDLE hDevice, int iLine, 
	  CClientSocket* pSocket);
   BOOL CreateJobSeparatorFiles_0(CSocketJob* pJob, int iStart, int iEnd, int iPage);
   BOOL CreateJobSeparatorFiles_1(CSocketJob* pJob, int iStart, int iEnd, int iPage);
   int GetRealPhotoCount(CSocketJob* pJob);
   CPaneElement* GetRipPane(CSocketJob* pJob, int iViewStyle);
   CObArray coSocketJobArray;   
   void DestroyAllSocketJobs();						    
   CString csLastGISError;   
   CString CStringToPSString(LPCSTR lpStrIn);
   void GetTotalSeparatorPages(CSocketJob* pJob);
   BOOL DoBarCode(CSocketJob* pJob, CString& csTempBarCodeFileName);
   PGPRC_RETURNVAL CreateQueueFiles(CSocketJob* pJob);
   PGPRC_RETURNVAL MontagePhotos(CSocketJob* pJob);
   PGPRC_RETURNVAL ComputeMontagePhotosVars(CSocketJob* pJob, int& iPage, int iMontageCanvasToUse, BOOL bOverrideMisFit = FALSE);
   PGPRC_RETURNVAL CreateMontagePhoto(CSocketJob* pJob, CPtrArray& coPhotosToDo);
   PGPRC_RETURNVAL WritePSStart(CStdioFile& cf, int iMediaWidthPica, 
	  int iMediaHeightPica, BOOL bDoEPSFStuff);
   PGPRC_RETURNVAL WritePSStartN(CStdioFile& cf, int iMediaWidthPica, 
	  int iMediaHeightPica);
   PGPRC_RETURNVAL WritePSMontageBitmap(CStdioFile& cf, int iDestWidthPica, int iDestHeightPica, 
	  int iDestXOffPica, int iDestYOffPica, BITMAPHANDLE PhotoBitmap);
   PGPRC_RETURNVAL WritePSMontagePS(CStdioFile& cf, int iDestWidthPica, int iDestHeightPica, 
	  int iDestXOffPica, int iDestYOffPica, POURPGPRCPHOTOSPECS pPhoto);
   PGPRC_RETURNVAL WritePSMontageEnd(CStdioFile& cf);
   BOOL ResizeMontageBitmap(pBITMAPHANDLE pPhotoBitmap, POURPGPRCPHOTOSPECS pPhoto);   
   void ReturnDummyInfo(int len, CClientSocket* pSocket);
   void SwapCustomPageDims(CSocketJob* pJob);      
   void ResetMontageFlags(CSocketJob* pJob);
   int CheckNextMontageSet(CSocketJob* pJob, int& iMontageCanvasToUse);
   void TagPhotos(CSocketJob* pJob, CStdioFile& cf, int iStart, int iEnd, int iPage);   
   void ReportByteCount(BOOL bWrite, int iNum);
   // job separator defs
   int PGPRC_CONTACTIMAGE_WIDTH;
   int PGPRC_CONTACTIMAGE_HEIGHT;
   int PGPRC_CONTACTIMAGE_SPACINGX;
   int PGPRC_CONTACTIMAGE_SPACINGY;
   int PGPRC_CONTACTIMAGE_NUMACCROSS;
   int PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_1;
   int PGPRC_CONTACTIMAGE_NUMDOWN_PAGE_N;
   int PGPRC_SEPARATOR_BBOX_WIDTH;
   int PGPRC_SEPARATOR_BBOX_HEIGHT;
   float PGPRC_SEPARATOR_BBOX_WIDTHF;
   float PGPRC_SEPARATOR_BBOX_HEIGHTF;
   };
#endif