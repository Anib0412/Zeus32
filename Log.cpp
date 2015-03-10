/////////////////////////////////////////////////////////////////////////////
// log.cpp
//                                                                        
// log class for RasterPlus
// Copyright (C) 1999-2000 Graphx, Inc.
//
// mods:
//
// 10-18-99 Version 3.0.1 - support for multiple page times in log
//  1-04-00 Version 3.0.3 - "Submitted" spelled incorrectly (duh!)
//  7-10-00 Version 4.0   - log files are now unique for each queue, not device
// 10-08-04 Version 5.2.7 - Log files entries lost after closing RasterPlus - Fixed

#include "stdafx.h"
#include "zeus.h"
#include "zeusdoc.h"
#include "Log.h"
#include "Profile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CLogEntry, CObject, 0)
IMPLEMENT_SERIAL(CLog, CObject, 0)

// need to dynamically set this
#define MAXLOGENTRIES 2000

WORD Log_VerCheck;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogEntry::CLogEntry()
   {
   // default
   iType = LOGENTRY_TYPE_JOB;
   }

CLogEntry::~CLogEntry()
   {
   }

void CLogEntry::Serialize(CArchive& ar, WORD verCheck )
{           
	if (ar.IsStoring())
	{
		ar << iType << csText << csName << csType << csDescription << csScaling << 
		csDevice << csOwner << csSubmitted << csElapsedTime <<
		csResolution << csMedia << csCopies << csPages << csResult << iStatusFlag;
	}      
	else                 
	{
		switch ( verCheck )
		{
		case ZEUSDOC_VERSION_A:
		case ZEUSDOC_VERSION_9:
		case ZEUSDOC_VERSION_8:
		case ZEUSDOC_VERSION_7:
		case ZEUSDOC_VERSION_6:
			ar >> iType >> csText >> csName >> csType >> csDescription >> csScaling >> 
			csDevice >> csOwner >> csSubmitted >> csElapsedTime >>
			csResolution >> csMedia >> csCopies >> csPages >> csResult >> iStatusFlag;
			break;
		case ZEUSDOC_NO_VERSION:
		default:
			ar >> iType >> csText >> csName >> csType >> csDescription >> csScaling >> 
			csDevice >> csOwner >> csSubmitted >> csElapsedTime >>
			csResolution >> csMedia >> csCopies >> csPages >> csResult >> iStatusFlag;
			break;
		}		
	}
} 

void CLogEntry::Serialize(CArchive& ar )
{           
	if (ar.IsStoring())
	{
		ar << iType << csText << csName << csType << csDescription << csScaling << 
		csDevice << csOwner << csSubmitted << csElapsedTime <<
		csResolution << csMedia << csCopies << csPages << csResult << iStatusFlag;
	}      
	else                 
	{
		if ( Log_VerCheck != ZEUSDOC_VERSION_9 )
		{
			ar >> iType >> csText >> csName >> csType >> csDescription >> csScaling >> 
			csDevice >> csOwner >> csSubmitted >> csElapsedTime >>
			csResolution >> csMedia >> csCopies >> csPages >> csResult >> iStatusFlag;
		}
	}
} 

CLog::CLog()
   {
   CProfile Profile;
   
   // get max log entries
   iMaxLogEntries = Profile.GetInt("Log", "MaxEntries", MAXLOGENTRIES);
   }

CLog::~CLog()
   {
   // delete all entries
   DeleteAll();

   CProfile Profile;
   
   // set max log entries
   Profile.WriteInt("Log", "MaxEntries", iMaxLogEntries);
   }

BOOL CLog::AddEntry(CQueueFile* pCQueueFile,
					CDevice* pLocalDevice, 
					CTransform* pLocalTransform, 
					CColorTransform* pLocalColor)
   {
   CLogEntry* pLe;
   BOOL bRet = FALSE;
   MEDIA Media;
   CTimeSpan* pCtsp;
   CString csTemp;

   // check size and delete the first one if necessary
   if (caLogArray.GetSize() >= MAXLOGENTRIES)
	  {
	  delete caLogArray.GetAt(0);
	  caLogArray.RemoveAt(0);	
	  }

   // make a new log entry
   if ((pLe = new CLogEntry) != NULL)
	  {
	  pLe->iType = LOGENTRY_TYPE_JOB;
      pLe->csName = pCQueueFile->ShortName;
      pCQueueFile->GetFileTypeName(pLe->csType);
      pLe->csDescription = pCQueueFile->Description;
      pLe->csScaling = pLocalTransform->Name;
      pLe->csDevice = pLocalDevice->Name;
	  pLe->csOwner = pCQueueFile->csOwner;	  
	  pLe->csSubmitted = pCQueueFile->csSubmitted;	  
	  // break out print times on multi-page files
	  if (pCQueueFile->cdwaPageTimes.GetSize() > 1)
		 {
		 pLe->csElapsedTime = "";
		 for (int i = 0; i < pCQueueFile->cdwaPageTimes.GetSize(); i++)
			{
			pCtsp = (CTimeSpan*)pCQueueFile->cdwaPageTimes.GetAt(i);
			pLe->csElapsedTime += pCtsp->Format("%H:%M:%S");			
			csTemp.Format(" (pg %d)<BR>", i + 1);
			pLe->csElapsedTime += csTemp;
			}		 
		 pLe->csElapsedTime += pCQueueFile->ctsElapsedTime.Format("%H:%M:%S (Total)");
		 }
	  else
		 pLe->csElapsedTime = pCQueueFile->ctsElapsedTime.Format("%H:%M:%S");
      pLocalDevice->GetRes(pLocalDevice->pDevInfo->CurrentResIdx, 
	     pLe->csResolution.GetBuffer(_MAX_PATH));
      pLe->csResolution.ReleaseBuffer();

	  // get the media
	  pLocalDevice->pDevInfo->TotalMedia = pLocalDevice->UpdateMediaList();
	  pLocalDevice->GetMedia(pLocalDevice->pDevInfo->CurrentMediaIdx, &Media);	  

      pLe->csMedia = Media.Name;
      pLe->csCopies.Format("%d", pLocalDevice->pDevInfo->Copies);
      pLe->csPages.Format("%d", pCQueueFile->PageCount);      
      pLe->csResult = pCQueueFile->Status;	  
	  pLe->iStatusFlag = pCQueueFile->StatusFlag;
      try
		 {
		 // add to the array
         caLogArray.Add((CObject*)pLe);
		 bRet = TRUE;
		 }
      catch(CException* theException)
		 {      
		 // oops
		 theException->Delete();
		 delete pLe;
         bRet = FALSE;
		 }
	  }	 
   return bRet;
   }

BOOL CLog::AddText(CString& csText)
   {									
   CLogEntry* pLe;
   BOOL bRet = FALSE;

   // check size and delete the first one if necessary
   if (caLogArray.GetSize() >= MAXLOGENTRIES)
	  {
	  delete caLogArray.GetAt(0);
	  caLogArray.RemoveAt(0);	
	  }

   // make a new log entry
   if ((pLe = new CLogEntry) != NULL)
	  {
	  pLe->iType = LOGENTRY_TYPE_TEXT;
      pLe->csText = csText;
      try
		 {
		 // add to the array
         caLogArray.Add((CObject*)pLe);
		 bRet = TRUE;
		 }
      catch(CException* theException)
		 {      
		 // oops
		 theException->Delete();
		 delete pLe;
         bRet = FALSE;
		 }
	  }	 
   return bRet;
   }

void CLog::DeleteAll()
   {
   int i = 0;
   CLogEntry* pLe;

   // remove all array elements
   while (i < caLogArray.GetSize() )
      {
	  if (pLe = (CLogEntry*)caLogArray.GetAt(i++))		 		 
		 {
		 if (IsValidEntry(pLe))
			delete pLe;         
		 }
      }     
   caLogArray.RemoveAll();
   }
		 
void CLog::Serialize(CArchive& ar, WORD verCheck )
{ 
	Log_VerCheck = verCheck;

	caLogArray.Serialize(ar);
} 

int CLog::GetNumEntries()
   { 
   return caLogArray.GetSize();
   } 

BOOL CLog::IsValidEntry(CLogEntry* pLe)
   {
   if (!pLe) return FALSE;

   if (pLe->iType == LOGENTRY_TYPE_JOB ||
	   pLe->iType == LOGENTRY_TYPE_TEXT)
	   return TRUE;
   else			   
	  return FALSE;
   }

void CLog::WriteLog(CString& csFileName)
   { 
   int i;
   CStdioFile cf;
   CLogEntry* pLe;
   CString csBack;      
   int iNumEntries = 0;
   CString csColor;

   // start with app dir
   csBack = ((CZeusApp*)AfxGetApp())->GetAppPath();   
   // add background
   csBack = "antique.gif";      
   for (i = caLogArray.GetSize() - 1; i >= 0; i--)
	  {
      pLe = (CLogEntry*)caLogArray.GetAt(i);
      if (pLe->iType == LOGENTRY_TYPE_JOB)		
		 {
		 // count the job entries
		 iNumEntries++;
		 }	     
	  }

   if (cf.Open(csFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
      {
	  // write HTML header	
	  fprintf(cf.m_pStream, "<HTML>\r\n");
	  fprintf(cf.m_pStream, "<HEAD>\r\n");
	  fprintf(cf.m_pStream, "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=windows-1252\">\r\n");
	  fprintf(cf.m_pStream, "<META NAME=\"Generator\" CONTENT=\"RasterPlus\">\r\n");
	  fprintf(cf.m_pStream, "<TITLE>RasterPlus Log</TITLE>\r\n");
	  fprintf(cf.m_pStream, "</HEAD>\r\n");
	  fprintf(cf.m_pStream, "<BODY LINK=\"#0000ff\" VLINK=\"#800080\">\r\n");
	  fprintf(cf.m_pStream, "<body background=\"%s\">\r\n", csBack);	  
	  if (iNumEntries == 1)
	     fprintf(cf.m_pStream, 
		 "<B><I><FONT FACE=\"Arial\" COLOR=\"#0000ff\"><P>RasterPlus Log - 1 entry (%s)</P>\r\n", csDevice);
	  else
	     fprintf(cf.m_pStream, 
		 "<B><I><FONT FACE=\"Arial\" COLOR=\"#0000ff\"><P>RasterPlus Log - %d entries (%s)</P>\r\n", iNumEntries, csDevice);
	  fprintf(cf.m_pStream, "</B></I></FONT>\r\n");
	  
	  // write the header
	  fprintf(cf.m_pStream, "<TABLE CELLSPACING=0 BORDER=1>\r\n");
      fprintf(cf.m_pStream, "<TR>\r\n");
      fprintf(cf.m_pStream, "<TH ALIGN=LEFT>Description</TH>\r\n");
      fprintf(cf.m_pStream, "<TH ALIGN=LEFT>File</TH>\r\n");
      fprintf(cf.m_pStream, "<TH ALIGN=LEFT>Print Time</TH>\r\n");
	  fprintf(cf.m_pStream, "<TH ALIGN=LEFT>Result</TH>\r\n");
	  fprintf(cf.m_pStream, "<TH ALIGN=LEFT>Media</TH>\r\n");
	  fprintf(cf.m_pStream, "<TH ALIGN=LEFT>Resolution</TH>\r\n");
      fprintf(cf.m_pStream, "<TH ALIGN=LEFT>Copies</TH>\r\n");
      fprintf(cf.m_pStream, "<TH ALIGN=LEFT>Pages</TH>\r\n");      
	  fprintf(cf.m_pStream, "<TH ALIGN=LEFT>Scaling</TH>\r\n");
      fprintf(cf.m_pStream, "<TH ALIGN=LEFT>Owner</TH>\r\n");
      fprintf(cf.m_pStream, "<TH ALIGN=LEFT>Submitted</TH>\r\n");
      fprintf(cf.m_pStream, "</TR>\r\n");

	  // write in reverse order
	  for (i = caLogArray.GetSize() - 1; i >= 0; i--)
		 {
		 pLe = (CLogEntry*)caLogArray.GetAt(i);
		 fprintf(cf.m_pStream, "<TR ALIGN=LEFT>\r\n");
		 // special case for text
		 if (pLe->iType == LOGENTRY_TYPE_TEXT)
			{
			fprintf(cf.m_pStream, "<TD ALIGN=LEFT COLSPAN=12>%s</TD>\r\n", pLe->csText);		 
		    // end row
		    fprintf(cf.m_pStream, "</TR>\r\n");
			continue;
			}
		 // red for bad, black for ok
		 if ((pLe->iStatusFlag & FILE_STATUS_CHECK) == FILE_STATUS_OK)
			csColor = "";
		 else
			csColor = "<FONT SIZE=3 COLOR=\"CC0000\">";

		 // description		 
		 fprintf(cf.m_pStream, "<TD ALIGN=LEFT>%s%s</TD>\r\n", csColor, pLe->csDescription);
		 // filename		 
		 fprintf(cf.m_pStream, "<TD ALIGN=LEFT>%s%s</TD>\r\n", csColor, pLe->csName);
		 // print time
		 fprintf(cf.m_pStream, "<TD ALIGN=LEFT>%s%s</TD>\r\n", csColor, pLe->csElapsedTime);
		 // result		 
		 fprintf(cf.m_pStream, "<TD ALIGN=LEFT>%s%s</TD>\r\n", csColor, pLe->csResult);
		 // media		 
		 fprintf(cf.m_pStream, "<TD ALIGN=LEFT>%s%s</TD>\r\n", csColor, pLe->csMedia);
		 // resolution
		 fprintf(cf.m_pStream, "<TD ALIGN=LEFT>%s%s</TD>\r\n", csColor, pLe->csResolution);
		 // copies		 
		 fprintf(cf.m_pStream, "<TD ALIGN=LEFT>%s%s</TD>\r\n", csColor, pLe->csCopies);
		 // pages		 
		 fprintf(cf.m_pStream, "<TD ALIGN=LEFT>%s%s</TD>\r\n", csColor, pLe->csPages);
		 // scaling		 
		 fprintf(cf.m_pStream, "<TD ALIGN=LEFT>%s%s</TD>\r\n", csColor, pLe->csScaling);
		 // owner		 
		 fprintf(cf.m_pStream, "<TD ALIGN=LEFT>%s%s</TD>\r\n", csColor, pLe->csOwner);
		 // submitted
		 fprintf(cf.m_pStream, "<TD ALIGN=LEFT>%s%s</TD>\r\n", csColor, pLe->csSubmitted);
		 // end row
		 fprintf(cf.m_pStream, "</TR>\r\n");
		 }
	  // end table
	  fprintf(cf.m_pStream, "</TABLE>\r\n");
      // end stuff	  
	  fprintf(cf.m_pStream, "</BODY>\r\n");
	  fprintf(cf.m_pStream, "</HTML>\r\n");
      cf.Close();     
	  }
   } 
   