#ifndef __SERVDEFS_H__
#define __SERVDEFS_H__

#pragma pack(1)
typedef struct tagSERVERXFER
   {
   char FileName[_MAX_PATH];
   char PrinterName[_MAX_PATH];
   char PortName[_MAX_PATH];
   } SERVERXFER, * PSERVERXFER;

typedef struct tagSERVERXFER2
   {
   char FileName[_MAX_PATH];
   char PrinterName[_MAX_PATH];
   char PortName[_MAX_PATH];
   BOOL IsComplete;
   char MutexName[_MAX_PATH];
   } SERVERXFER2, * PSERVERXFER2;

typedef struct tagSERVERXFER3
   {
   char FileName[_MAX_PATH];
   char PrinterName[_MAX_PATH];
   char PortName[_MAX_PATH];
   BOOL IsComplete;
   char MutexName[_MAX_PATH];
   int iUnused;
   } SERVERXFER3, * PSERVERXFER3;

typedef struct tagSERVERXFER4
   {
   char FileName[_MAX_PATH];
   char PrinterName[_MAX_PATH];
   char PortName[_MAX_PATH];
   BOOL IsComplete;
   char MutexName[_MAX_PATH];
   int iUnused;
   // stuff from JOB_INFO_2
   DWORD  JobId;    
   char   pDriverName[_MAX_PATH];
   char   pMachineName[_MAX_PATH];
   char   pUserName[_MAX_PATH];
   char   pDocument[_MAX_PATH];
   char   pDatatype[_MAX_PATH]; 
   char   pStatus[_MAX_PATH]; 
   DWORD  Status; 
   DWORD  Priority; 
   DWORD  Position; 
   DWORD  TotalPages; 
   DWORD  PagesPrinted; 
   SYSTEMTIME Submitted;
   } SERVERXFER4, * PSERVERXFER4;
#pragma pack()

#define SERVERSECTION        TEXT("Server")
#define SERVERENTRY          TEXT("App")
#define SERVERFLUSHSIZE      TEXT("FlushSize")
#define SERVERDIRENTRY       TEXT("AppDir")
#define SERVERIPCDATAFILE    TEXT("IPCDataFile")
#define SERVERSPOOLFOLDER    TEXT("SpoolFolder")
#define SERVERTESTMODE		 TEXT("TestMode")

#define SERVERDEFAULT        TEXT("c:\\rp95\\rp95.exe")
#define SERVERDIRDEFAULT     TEXT("c:\\rp95\\")
#define SERVERCLASS          TEXT("RasterPlus95Class")

#define SERVERCLASSPHZ30     TEXT("PrintServerPlusClass")
#define SERVERCLASS30        TEXT("RasterPlusClass")

#define SERVERMUTEX			 TEXT("9C7DA091-7628-4b10-B6D7-EFD47E30EAF2")
#define SERVERPIPE			 TEXT("RP_PRINT_PIPE")

#define SERVERINIFILE        TEXT("RP95.INI")
#define SERVERSTARTPARAM     TEXT(" /m")
#define SERVERIPCDEFAULT     TEXT("c:\\rp95\\IPCDataFile")
#define SERVERFILEPATH       TEXT("FilePath")
#define SERVERPATHDEFAULT    TEXT("c:\\rp95\\server\\")
#define ADDSERVERFILEMSG     TEXT("RasterPlus30_AddJobToQueue")
#define SERVERFILEEOFMSG     TEXT("RasterPlus30_ServerFileEOF")
#define RP95_SERVER_KEY      TEXT("Software\\Graphx\\RasterPlus95\\Server")
#define RP30_SERVER_KEY      TEXT("Software\\Graphx\\RasterPlus\\Server")
//#define PHZPRT_SERVER_KEY    TEXT("Software\\Graphx\\RasterPlus\\Server")
#define PHZPRT_SERVER_KEY    TEXT("Software\\Graphx\\PhotogizePrintServer\\Server")
#define RP95_COMPLETE_MUTEX  TEXT("RP95_CompleteFileMutex_")
#define RP30_COMPLETE_MUTEX  TEXT("RP30_CompleteFileMutex_")

#define RP95_NEW_MUTEX_FLAG  0x20

#endif       

   
   
   
   
