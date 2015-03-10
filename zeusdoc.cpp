/////////////////////////////////////////////////////////////////////////////
// ZEUSDOC.CPP
//                                                                        
// document class for RasterPlus
// Copyright (C) 1996-2002 Graphx, Inc.
//
// mods:
//
// 7-11-96  Build 40 - don't allow close of queue if printing
// 7-18-96  Build 40 - changed doc version because of changes in various devmodes
//					      note that we should probably add version control to devmodes
//						  themselves so we don't have to do this everytime
// 11-05-96 Build 45 - added code in CloseDocument to delete TEMP files if the Queue
//				          was never saved.
// 11-04-99 Version 3.0.2 - Ctrl s was saving file and adding an extra .que to the name - fixed
// 12-02-99 Version 3.0.2 - don't delete server files if other instances exist
// 03-21-00 Version 3.0.3 - delete a file if it gives us a bad open so we don't crash next time
// 10-06-00 Version 4.0.1 - empty the print queue after reading the queue file
// 10-10-01 Version 5.0.0 - implement an open queue counter
// 02-12-02 Version 5.0.4 - delete forms on printer delete

#include "stdafx.h"
#include "zeus.h"
#include "zeusdoc.h"
#include "profile.h"
#include "inidefs.h"
#include "winspool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZeusDoc

IMPLEMENT_DYNCREATE(CZeusDoc, CDocument)

BEGIN_MESSAGE_MAP(CZeusDoc, CDocument)
	//{{AFX_MSG_MAP(CZeusDoc)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)	
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZeusDoc construction/destruction

CZeusDoc::CZeusDoc()
   {
   int i;  
   CProfile Profile;   
   CString csTempPath;

   // not in rip
   InRip = FALSE;
   // reset times
   QueueStartTime = 0;
   QueueElapsedTime = 0;   
   WasOnDisk = FALSE;

   // increment the global queue counter
   ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->QueueCount.IncrQueueCount();

   HoldTreeHeight = -1;
   PrintTreeHeight = -1;
   ArchiveTreeHeight = -1;

   // get log window defaults
   int screenWidth = GetSystemMetrics(SM_CXSCREEN);
   int screenHeight = GetSystemMetrics(SM_CYSCREEN);

   // get working area on the desktop
   RECT rc;
   SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);   

   crLogWinRect.top = rc.bottom - screenHeight * 1 / 4;
   crLogWinRect.bottom = rc.bottom;
   crLogWinRect.left = rc.left;
   crLogWinRect.right = rc.right - screenWidth * 1 / 4;
   LogWnd.pcrLogWinRect = &crLogWinRect;

   // get the initial file dir 
   InitialFileDir = Profile.GetStr(IDS_QUEUESECTION, IDS_INITIALDIR, NULL);
   // get the initial filter index	  
   FilterIdx = Profile.GetInt(IDS_QUEUESECTION, IDS_QUEUE_FILTERIDX,  IDDM_QUEUE_FILTERIDX);

   // default column widths   
   for(i = 0; i < NUMQUEUEPANES; i++)
	  {
      iQueueColumnWidth[i][QUEUECOL_DESCRIPTION] = 200;
      iQueueColumnWidth[i][QUEUECOL_FILENAME] = 75;
      iQueueColumnWidth[i][QUEUECOL_OWNER] = 50;
      iQueueColumnWidth[i][QUEUECOL_SIZE] = 50;
      iQueueColumnWidth[i][QUEUECOL_PROPS] = 50;
      iQueueColumnWidth[i][QUEUECOL_SUBMITTED] = 125;
      iQueueColumnWidth[i][QUEUECOL_STATUS] = 175;
	  }   
   bPathSet = FALSE;
   // show these by default
   bPSoutState = TRUE;
   bQStatusState = TRUE;   
   bDeleteOnClose = FALSE;
   }

CZeusDoc::~CZeusDoc()
   {
   CProfile Profile;
   CString cs;
   int i;
   CStringArray csaFormNames;		 

   // decrement the global queue counter
   ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->QueueCount.DecrQueueCount();

   // write default dirs
   Profile.WriteStr(IDS_QUEUESECTION, IDS_INITIALDIR, InitialFileDir);
   Profile.WriteInt(IDS_QUEUESECTION, IDS_QUEUE_FILTERIDX, FilterIdx);

   // if the delete on close flag is set, do it
   if (bDeleteOnClose)
	  {	  
	  // delete the queue file
      ::DeleteFile(csDelFile);

      PRINTER_DEFAULTS pd;
      pd.pDatatype = NULL;
      pd.pDevMode = NULL;
      pd.DesiredAccess = PRINTER_ALL_ACCESS;
	  HANDLE hPrinter;

	  if (IS_OS_NT_FAMILY)
		 {
		 // create a string array of form names
		 if (QueueDevice.GetDevInfo())
			{
			MEDIA Media;
			if (QueueDevice.pDevInfo)
			   {
			   // update the media list
			   QueueDevice.UpdateMediaList();	  
			   for (i = 0; i < QueueDevice.pDevInfo->TotalMedia; i++)
				  {	  	  	  
				  // get the media
				  QueueDevice.GetMedia(i, &Media);	  
				  // add to the list
				  csaFormNames.Add(Media.Name);
				  }
			   }	
			}   
		 }	  

	  // delete the application printer
      if (::OpenPrinter((char*)(const char*)csDelDriver, &hPrinter, &pd))
	     {
		 if (IS_OS_NT_FAMILY)
			{
			// delete all existing forms
			for (i = 0; i < csaFormNames.GetSize(); i++)
			   {	  	  	  
			   cs = csaFormNames.GetAt(i);
			   DeleteForm(hPrinter, (char*)(const char*)cs);
			   }
			}

	     if (hPrinter) DeletePrinter(hPrinter);
		 if (hPrinter) ::ClosePrinter(hPrinter);
		 // delete the printer driver         
		 // only delete if no one else is using the driver
		 if (!((CZeusApp*)AfxGetApp())->IsPrinterDriverStillUsed(QueueDevice.Name))
			{
            DeletePrinterDriver(NULL, NULL, (char*)(const char*)QueueDevice.Name);   		 
			}

		 // delete ppd files and pcf files
         ((CZeusApp*)AfxGetApp())->DeleteAllPPDPCFFiles(QueueDevice.Name, csDelDriver, QueueDevice.DevIdx);

		 }									
	  // save our state
	  ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->SaveState();
	  // need to update the menu
	  ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->AddDevNamesToMenu();
	  }

	((CZeusApp*)AfxGetApp())->DeleteMutex();
   }

BOOL CZeusDoc::OnNewDocument()
   {       
   if (!CDocument::OnNewDocument())
      return FALSE;

   // force this flag so we save something
   SetModifiedFlag();
   
   // get the default device
   QueueDevice.LoadDefaultDevice();
    // get the default transform
   QueueTransform.LoadDefaultTransform();    
    // get the default transform
   QueueColor.LoadDefaults();    

   csBaseTitle = GetTitle();
//   TRACE("DOC Base Title = %s\n", (const char*)csBaseTitle);

   return TRUE;
   }

BOOL CZeusDoc::OurSaveDocument()
   {
   // this is a way to hook the protected OnSaveDocument call
   return OnSaveDocument(GetTitle());   
   }

BOOL CZeusDoc::OnSaveDocument(LPCTSTR lpszPathName)
   {   
   BOOL ret;
   CString csTempDoc;
#ifndef RP_6
   POSITION pos;
   CView* pView;
   int i;

   // get the view
   if (pos = GetFirstViewPosition())
      {
      if (pView = GetNextView(pos))
         {
#ifndef RP_6
         // get spliiter pos
		 int MinRowHeight;		 
         ((CSplitterWnd*)(pView->GetParent()))->GetRowInfo(0, HoldTreeHeight, MinRowHeight);
		 ((CSplitterWnd*)(pView->GetParent()))->GetRowInfo(1, PrintTreeHeight, MinRowHeight);
		 ((CSplitterWnd*)(pView->GetParent()))->GetRowInfo(2, ArchiveTreeHeight, MinRowHeight);

         // save the column widths		 
	     for(i = 0; i < NUMQUEUECOLUMNS; i++)
			{
			iQueueColumnWidth[QUEUESTYLE_HOLD][i] = 
			   ((CSpecialTree*)(((CSplitterWnd*)(pView->GetParent()))->GetPane(QUEUESTYLE_HOLD, 0)))->GetTreeCtrl().GetColumnWidth(i);			   
			iQueueColumnWidth[QUEUESTYLE_PRINT][i] = 
			   ((CSpecialTree*)(((CSplitterWnd*)(pView->GetParent()))->GetPane(QUEUESTYLE_PRINT, 0)))->GetTreeCtrl().GetColumnWidth(i);			   
			iQueueColumnWidth[QUEUESTYLE_ARCHIVE][i] = 
			   ((CSpecialTree*)(((CSplitterWnd*)(pView->GetParent()))->GetPane(QUEUESTYLE_ARCHIVE, 0)))->GetTreeCtrl().GetColumnWidth(i);			   
			}
#endif
         }
      }
#endif
   if (strstr(lpszPathName, "\\"))
	  ret = CDocument::OnSaveDocument(lpszPathName);   
   else
	  {
      CString csQueuePath;
      CString csNewDocName;
      CString csTitle;

      // get app path
      csQueuePath = ((CZeusApp*)AfxGetApp())->GetQueuePath();
      // create queue folder if it doesn't exist
      ::CreateDirectory(csQueuePath, NULL);
      // create new doc name
      csNewDocName = csQueuePath;
      csTitle = lpszPathName;
      // get rid of illegal chars
      ((CZeusApp*)AfxGetApp())->NukeBadFnameChars(csTitle);
      // make sure it's real
	  if (csTitle.GetLength() > 0)
		 {
		 csNewDocName += csTitle;
         SetPathName(csNewDocName, FALSE);  	  
		 csTempDoc = csNewDocName.Right(4);
		 if (csTempDoc != ".que")
            csNewDocName += ".que";				  
	     ret = CDocument::OnSaveDocument(csNewDocName);   
		 }
	  }  

   WasOnDisk = TRUE;   

   return ret;
   }

BOOL CZeusDoc::OnOpenDocument(const char* pszPathName)
   {     
   BOOL ret;                                 
   
   // call parent class
   ret = CDocument::OnOpenDocument(pszPathName);

   // get doc name
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   _splitpath(pszPathName, NULL, NULL, fname, ext);
   
   csBaseTitle = fname;
   csBaseTitle += ext;
//   TRACE("DOC Base Title = %s\n", (const char*)csBaseTitle);

   WasOnDisk = TRUE;   

   // but check our flag
   if (ret)       
      return CheckDOCValid;      
   else                    
	  {
	  // delete the file - we know it's bad
	  ::DeleteFile(pszPathName);
      return ret;         
	  }
   }
   
void CZeusDoc::DeleteContents()
   {   
   int i = 0;
   CQueueFile* pCQueueFile;

   // delete the cobarray
   while (i < CQ.GetSize() )      
	  {
	  // if this flag is set, we need to delete spool files
	  // and possibly hot folder files
	  if (bDeleteOnClose)
		 {
   	     pCQueueFile = (CQueueFile*)CQ.GetAt(i);
		 if (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_SERVERFILE)
 		    ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->DeleteLastServerFile(pCQueueFile);
		 // if we are only keeping hot folder files around as
		 // long as the Job is around, nuke it now
		 else if (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_POLLEDFILE)
			{
		    if (crrrs.iHotFolderResourceSelector == HOTFOLDER_KEEP_MAX)
		       ::DeleteFile(pCQueueFile->LongName);	  
			}		 
		 }
      delete CQ.GetAt( i++ );
      }        
   CQ.RemoveAll();

   i = 0;
   // delete the cobarray
   while (i < CQ_Hold.GetSize() )
      {
	  if (bDeleteOnClose)
		 {
   	     pCQueueFile = (CQueueFile*)CQ_Hold.GetAt(i);
 		 if (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_SERVERFILE)
		    ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->DeleteLastServerFile(pCQueueFile);
		 else if (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_POLLEDFILE)
			{
		    if (crrrs.iHotFolderResourceSelector == HOTFOLDER_KEEP_MAX)
		       ::DeleteFile(pCQueueFile->LongName);	  
			}		 
		 }
      delete CQ_Hold.GetAt( i++ );
      }     
   CQ_Hold.RemoveAll();

   i = 0;
   // delete the cobarray
   while (i < CQ_Archive.GetSize() )
      {
	  if (bDeleteOnClose)
		 {
   	     pCQueueFile = (CQueueFile*)CQ_Archive.GetAt(i);
 		 if (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_SERVERFILE)
		    ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->DeleteLastServerFile(pCQueueFile);
		 else if (pCQueueFile->ExtraInfo & CQUEUE_EXTRA_POLLEDFILE)
			{
		    if (crrrs.iHotFolderResourceSelector == HOTFOLDER_KEEP_MAX)
		       ::DeleteFile(pCQueueFile->LongName);	  
			}		 
		 }
      delete CQ_Archive.GetAt( i++ );
      }     
   CQ_Archive.RemoveAll();
   }  

/////////////////////////////////////////////////////////////////////////////
// CZeusDoc serialization
void CZeusDoc::Serialize(CArchive& ar)
   {         
   WORD wVersion;
   int i, j;

   wVersion = (WORD)ZEUSDOC_VERSION;

   if (ar.IsStoring())
	  {                           
	  // write version 
	  ar << wVersion;
	  ar << QueueStartTime;
      ar << QueueElapsedTime;          
	  ar << HoldTreeHeight;
	  ar << PrintTreeHeight;
	  ar << ArchiveTreeHeight;
	  ar << crLogWinRect;
      ar << bPSoutState;
      ar << bQStatusState;
	  crrrs.Serialize(ar, (WORD)ZEUSDOC_VERSION);
	  cLog.Serialize(ar, (WORD)ZEUSDOC_VERSION);
	  QueueDevice.Serialize(ar, (WORD)ZEUSDOC_VERSION);
	  QueueTransform.Serialize(ar, (WORD)ZEUSDOC_VERSION);
	  QueueColor.Serialize(ar, (WORD)ZEUSDOC_VERSION);		  
	  CQ.Serialize(ar);
	  CQ_Hold.Serialize(ar);
	  CQ_Archive.Serialize(ar);
	  ar << csTitle;
	  for (i = 0; i < NUMQUEUEPANES; i++)
		 {
		 for(j = 0; j < NUMQUEUECOLUMNS; j++)
			{
			ar << iQueueColumnWidth[i][j];
			}
		  }	  
	   }
   else
	  {                         
	  // check version
	  ar >> wVerCheck;
	  ((CZeusApp*)AfxGetApp())->SetFileVersion(wVerCheck);
	  switch ( wVerCheck )
	  {
	  case ZEUSDOC_VERSION_A:
	  case ZEUSDOC_VERSION_9:
	  case ZEUSDOC_VERSION_8:
	  case ZEUSDOC_VERSION_7:
	  case ZEUSDOC_VERSION_6:
	     ar >> QueueStartTime;
         ar >> QueueElapsedTime;   
	     ar >> HoldTreeHeight;
	     ar >> PrintTreeHeight;
	     ar >> ArchiveTreeHeight;
		 ar >> crLogWinRect;
         ar >> bPSoutState;
		 ar >> bQStatusState;
	     crrrs.Serialize(ar, wVerCheck);
	     cLog.Serialize(ar, wVerCheck);
         QueueDevice.Serialize(ar, wVerCheck);
	     QueueTransform.Serialize(ar, wVerCheck);	   
	     QueueColor.Serialize(ar, wVerCheck);	
	     CQ.Serialize(ar);
		 // empty the print queue after reading the queue file
		 // we don't want files in the print queue on start up!	  
		 for (i = 0; i < CQ.GetSize(); i++)
			delete CQ.GetAt(i);
		 CQ.RemoveAll();
	     CQ_Hold.Serialize(ar);
	     CQ_Archive.Serialize(ar);
		 ar >> csTitle;
	     for (i = 0; i < NUMQUEUEPANES; i++)
			{
		    for(j = 0; j < NUMQUEUECOLUMNS; j++)
			   {
			   ar >> iQueueColumnWidth[i][j];
			   }
			}		 
	     CheckDOCValid = TRUE;
		 // we need to populate the job id array with these new
		 // jobs brought in off the disk
		 PopulateJobIDArrays();
		 break;
	  // default is ALWAYS the current version
	  case ZEUSDOC_NO_VERSION:
	  default:
	     ar >> QueueStartTime;
         ar >> QueueElapsedTime;   
	     ar >> HoldTreeHeight;
	     ar >> PrintTreeHeight;
	     ar >> ArchiveTreeHeight;
		 ar >> crLogWinRect;
         ar >> bPSoutState;
		 ar >> bQStatusState;
	     crrrs.Serialize(ar, wVerCheck);
		 cLog.Serialize(ar, wVerCheck);
         QueueDevice.Serialize(ar, wVerCheck);
	     QueueTransform.Serialize(ar, wVerCheck);	   
	     QueueColor.Serialize(ar, wVerCheck);
	     CQ.Serialize(ar);
		 // empty the print queue after reading the queue file
		 // we don't want files in the print queue on start up!	  
		 for (i = 0; i < CQ.GetSize(); i++)
			delete CQ.GetAt(i);
		 CQ.RemoveAll();
	     CQ_Hold.Serialize(ar);
	     CQ_Archive.Serialize(ar);
		 ar >> csTitle;
	     for (i = 0; i < NUMQUEUEPANES; i++)
			{
		    for(j = 0; j < NUMQUEUECOLUMNS; j++)
			   {
			   ar >> iQueueColumnWidth[i][j];
			   }
			}		 
	     CheckDOCValid = TRUE;
		 // we need to populate the job id array with these new
		 // jobs brought in off the disk
		 PopulateJobIDArrays();
		 break;
	  }
	  }
   }                         

WORD CZeusDoc::GetFileVersion()
{
	return wVerCheck;
}


/////////////////////////////////////////////////////////////////////////////
// CZeusDoc diagnostics

#ifdef _DEBUG
void CZeusDoc::AssertValid() const
{
	CDocument::AssertValid();	
}

void CZeusDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CZeusDoc commands

BOOL CZeusDoc::CanCloseFrame(CFrameWnd* cf)
   {   
   if (InRip)
      {	  
	  AfxMessageBox(IDS_NOCLOSEQUEUE_WHILEACTIVE);
	  return 0;
	  }

   // need to do this here as well
   CString csPrompt;
   // make the prompt
   csPrompt.Format(IDS_CHECKCLOSEQUEUE, GetTitle());
   // are you sure you want to close this queue?
   if (AfxMessageBox(csPrompt, MB_YESNO | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_DEFBUTTON2) != IDYES)
	  return 0;		
   else
	  {
	  bDeleteOnClose = TRUE;
	  csDelFile = GetPathName();
	  csDelDriver = GetTitle();
	  }

   return(CDocument::CanCloseFrame(cf));
   }	

void CZeusDoc::OnFileSave() 
   {
   CString csQueuePath;
   CString csNewDocName;
   CString csTitle;

   // get queue path
   csQueuePath = ((CZeusApp*)AfxGetApp())->GetQueuePath();
   // create queue folder if it doesn't exist
   ::CreateDirectory(csQueuePath, NULL);
   // create new doc name
   csNewDocName = csQueuePath;
   csTitle = GetTitle();
   // get rid of illegal chars
   ((CZeusApp*)AfxGetApp())->NukeBadFnameChars(csTitle);

   csNewDocName += csTitle;
   SetPathName(csNewDocName, FALSE);  

   csNewDocName += ".que";
   
   // we've set the path
   bPathSet = TRUE;

   DoSave(csNewDocName, TRUE);
   }

void CZeusDoc::OnFileSaveAs() 
   {
   CDocument::OnFileSaveAs();

   csBaseTitle = GetTitle();
//   TRACE("DOC Base Title = %s\n", (const char*)csBaseTitle);
   }

void CZeusDoc::OnCloseDocument()
   {
   if (IsModified()) OnSaveDocument(GetTitle());   

   CDocument::OnCloseDocument();
   }

BOOL CZeusDoc::SaveModified()
   {
   return TRUE;
   }

void CZeusDoc::OnFileClose() 
   {
   CString csPrompt;

   // make the prompt
   csPrompt.Format(IDS_CHECKCLOSEQUEUE, GetTitle());
   // are you sure you want to close this queue?
   if (AfxMessageBox(csPrompt, MB_YESNO | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_DEFBUTTON2) != IDYES)
	  return;

   bDeleteOnClose = TRUE;
   csDelFile = GetPathName();
   csDelDriver = GetTitle();

   CDocument::OnFileClose();
   }

void CZeusDoc::PopulateJobIDArrays()
   {   
   int i;
   CQueueFile* pCQueueFile;

   // re-create jobids
   for (i = 0; i < CQ.GetSize(); i++)
	  {
	  pCQueueFile = (CQueueFile*)CQ.GetAt(i);
	  ((CZeusApp*)AfxGetApp())->PopulateJobIDArray(pCQueueFile->iJobID);
      }              
   for (i = 0; i < CQ_Hold.GetSize(); i++)
      {
	  pCQueueFile = (CQueueFile*)CQ_Hold.GetAt(i);
	  ((CZeusApp*)AfxGetApp())->PopulateJobIDArray(pCQueueFile->iJobID);
      }           
   for (i = 0; i < CQ_Archive.GetSize(); i++)
      {
	  pCQueueFile = (CQueueFile*)CQ_Archive.GetAt(i);
	  ((CZeusApp*)AfxGetApp())->PopulateJobIDArray(pCQueueFile->iJobID);
	  }  
   }
/*
BOOL CZeusDoc::InitFields()
{
	return OnNewDocument();
}
*/