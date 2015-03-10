/////////////////////////////////////////////////////////////////////////////
// NTSAMPLE.C
//                                                                        
// Jaws DLL interface routines for RasterPlus95
// Copyright (C) 1996-97 Graphx, Inc.
//
// mods:
//
// 7-18-96  Build 40 - added FILE_FLAG_SEQUENTIAL_SCAN to create file for optimum 
//			   performance
// 7-18-96  Build 40 - made disk buffers dynamic and larger
// 7-31-96  Build 41 - set In_fp_eof before fp_eof call and check it before doing 
//             an fp_interrupt to keep Jaws from getting clobbered
// 8-27-96  Build 41 - cleaned up a memory leak with UnCompBuf
// 2-3-97   Build 48 - remove fp_eof() from SendData to make all jobs encapsulated
// 5-26-97  Build 50 - added color correction calls
// 10-28-97 Build 53 - added partial server file reading
// 11-07-98 Version 2.0.5 - completed loadfromrip dev tuning
// 1-25-99  Version 2.0.6 - large files wrapped when displaying percent - fixed
// 2-01-99  Version 2.0.6 - Added support for .PDF files (will enable in 3.0)
// 3-08-99   Version 3.0.0 - integrated into main app

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <windowsx.h>
#include <dos.h>
#include "resource.h"
#include "catdef.h"  
#include "header.dat"       
#include "IPCRipData.h" // for PSMSG_PSERR et. al.
#include "colorc.h"
#include "rip.h"
#include "ftypes.h"

static void		reverse_bits (unsigned char *, int);
static void		writetiffhdr (HANDLE, int, int, int, int, int);

HANDLE			hlib;

int 		(WINAPIV *fp_startup) ();
int			(WINAPIV *fp_finish) ();
int			(WINAPIV *fp_callbacks) ();
int			(WINAPIV *fp_colorcorrect_callbacks) ();
int			(WINAPIV *fp_input) ();
int			(WINAPIV *fp_eof) ();
int			(WINAPIV *fp_interrupt) ();

static int		rasterstride;

static void do_jaws_pause ();

static FARPROC SetFilePercent;
static FARPROC SetFileTotal;
static FARPROC DoLine;
static FARPROC DoHeader;
static DWORD dwHandle;
static FARPROC RenderTick;
static FARPROC DoPSMessage;
static int RenderTickCounter;

// decompression routines
static void InitDecompCache();
static short DecompPutCache(char c);
static void GetDecompCache();
static int FillDecompCacheBuf(int size, char *buf);
static short DecompGet();
static HANDLE fdGlobal;
static int CATHeaderNeeded;
static int NumUncompressed;
static int CancelFlag;
static BOOL In_fp_eof;
static int Cancelled;
static BOOL GotCTRLD;
static BOOL ShowPageDone;
static BYTE* UnCompBuf;
static int RipReturn;

#define RENDERTICKDELAY   2000

void WINAPIV ps_output (char *bp, int length)
   {
   if (length == 1 && *bp == 0x04) GotCTRLD = TRUE;
   DoPSMessage(dwHandle, bp, length, PSMSG_PSOUT);
   }

void WINAPIV ps_stderr (char *bp, int length)
   {
   DoPSMessage(dwHandle, bp, length, PSMSG_PSERR);
   }

void WINAPIV ps_codetick (void)
   {
   //do_jaws_pause();
   }

void WINAPIV ps_rendertick (void)
   {
   // we're awake, we're awake!!!
   // (every 2 seconds)
   if ((GetTickCount() - RenderTickCounter) > RENDERTICKDELAY)
      {
	  RenderTick(dwHandle, TRUE);
	  RenderTickCounter = GetTickCount();
	  }

   if (CancelFlag && !Cancelled)
      {	  
	  if (!In_fp_eof) fp_interrupt(0);
	  Cancelled = TRUE;	
	  }   
   }				

void WINAPIV ps_rasterheader (int	w,
			     int	h,
			     int	planes,
			     int	ind)
   {
   // the width of the raster is equal to w * 32 bits if
   // we are doing 24 bit color, or w * planes for others
   rasterstride = w * ((planes == 24) ? 32 : planes);
   rasterstride = (rasterstride + 31) &~ 31;
   rasterstride /= 8;    

   if (CancelFlag && !Cancelled)
      {	  
	  if (!In_fp_eof) fp_interrupt(0);
	  Cancelled = TRUE;	
	  }   
  else
     RenderTick(dwHandle, TRUE);

   RenderTickCounter = 0;

   ShowPageDone = TRUE;

   DoHeader(dwHandle, w, h, planes, ind);   
   }

void WINAPIV ps_rasterbits (int ylo, int yhi, unsigned char *bits)
   {
   if (CancelFlag)
      {
	  if (!Cancelled) 
	     {			 
		 if (!In_fp_eof) fp_interrupt(0);
		 Cancelled = TRUE;
		 }
	  return;
	  }
   DoLine(dwHandle, ylo, yhi, bits, rasterstride);   
   }

#define BUFSIZE 32767

void SetProcs(FARPROC dl, FARPROC dh)
   {
   DoLine = dl;
   DoHeader = dh;
   }

HINSTANCE StartRip(FARPROC sfp, FARPROC sft, FARPROC rt, FARPROC psm, 
   const char* DLLName, const char* RipPaths)
   {
   int err; 						  
   
   SetFilePercent = sfp;   
   SetFileTotal = sft;      
   RenderTick = rt;   
   DoPSMessage = psm;      

#ifdef _DEBUG
	//DebugBreak();
#endif
         
   hlib = LoadLibrary(DLLName);

   if (hlib)
      {
	  fp_startup = (int(WINAPIV *)())GetProcAddress (hlib, "_postscript_dll_startup");

      fp_finish = (int(WINAPIV *)())GetProcAddress (hlib, "_postscript_dll_finish");

      fp_callbacks = (int(WINAPIV *)())GetProcAddress (hlib, "_postscript_dll_callbacks");

	  fp_colorcorrect_callbacks = (int(WINAPIV *)())GetProcAddress (hlib, "_colorcorrect_dll_callbacks");

      fp_input = (int(WINAPIV *)())GetProcAddress (hlib, "_postscript_dll_input");

      fp_eof = (int(WINAPIV *)())GetProcAddress (hlib, "_postscript_dll_eof");

      fp_interrupt = (int(WINAPIV *)())GetProcAddress (hlib, "_postscript_dll_interrupt");

      err = (fp_callbacks) (ps_output,
		  ps_stderr,
		  ps_codetick,
		  ps_rendertick,
		  ps_rasterheader,
		  ps_rasterbits);

	  if (fp_colorcorrect_callbacks)
		 err = (fp_colorcorrect_callbacks) (dwHandle, colorcorrect_rgb2cmyk,
		    colorcorrect_cmyk2cmyk, colorcorrect_rgb2rgb, colorcorrect_cmyk2rgb);
    
       // start the rip 
	   //"%os%=/;%font%+d:/jaws/font;%resource%+d:/jaws/resource;%lib%+d:/jaws/lib;%ttfont%=/windows/system;%temp%=/temp");				
	   err = (fp_startup) (0, 0, RipPaths);

	   /* Run init.ps straight away without waiting for input */    
       fp_input("", 0);
	   }
	return hlib;
	}
    
void CloseJaws()
   {
   int err;

   char *cp = "true () startjob quit ";
   err = (fp_input)  (cp, strlen (cp));

   err = (fp_eof) ();

   err = (fp_finish) ();

   FreeLibrary (hlib);
   }

void CancelIt()
   {
   // set flag
   CancelFlag = TRUE;
   }

void CancelItWithReturn(int ret)
   {
   // set flag
   CancelFlag = TRUE;
   RipReturn = ret;
   }

int RipPS(PJOBINFO pJobInfo, HINSTANCE hInst, DWORD dwh)
   {
    int			err;
	BYTE       *buf;
	char		bufFmt[_MAX_PATH * 2];
	char		bufOut[_MAX_PATH * 2];
    int			n;
    HANDLE		fd;
	DWORD NumRead = 0;
	DWORD dwFileSize, dummy;		
	BOOL EndOfFile;	
	HANDLE hMutex;
	DWORD dwRet;
	BOOL WaitForFileComplete;  
	char* fname;
	int StartOfValidPS;
	int RealFileSize;
	int chn;
	BOOL IsComplete;
	char* MutexName;

	fname = pJobInfo->LongName;
	StartOfValidPS = pJobInfo->StartofValidPS;
	RealFileSize = pJobInfo->RealFileSize;
	chn = pJobInfo->CATHeaderNeeded;
	IsComplete = pJobInfo->IsComplete;
	MutexName = pJobInfo->MutexName;		
	dwHandle = dwh;
	
	//float xlo, xhi, ylo, yhi;	
	RipReturn = SYN_ERRMSG_ALLOK;


#ifdef _DEBUG
	//DebugBreak();
#endif

    CancelFlag = FALSE;		
	Cancelled = FALSE;
	buf = NULL;	
	UnCompBuf = NULL;

	if (!IsComplete && MutexName)	   
	   {
	   WaitForFileComplete = TRUE;
	   hMutex = CreateMutex(NULL, FALSE, MutexName);
#if 0
	   if (!hMutex)
		  {
		  char d[180];
		  wsprintf(d, "Unable to create mutex: %s", MutexName);
		  MessageBox(NULL, d, "RIPPROC.EXE", MB_OK | MB_SETFOREGROUND);	
		  }
#endif
	   }
	else
	   {
	   WaitForFileComplete = FALSE;
	   hMutex = NULL;
	   }
	
	CATHeaderNeeded = chn;

	SetFilePercent(dwHandle, 0); 

	buf = GlobalAllocPtr(GMEM_MOVEABLE, BUFSIZE);

	// tell the psout window what we're doing
	LoadString(hInst, IDS_PROCESSING_FMT, bufFmt, _MAX_PATH * 2);
	wsprintf(bufOut, bufFmt, fname);	
	DoPSMessage(dwHandle, bufOut, strlen(bufOut), PSMSG_PSOUT);	

	fd = CreateFile (fname, GENERIC_READ,
		     FILE_SHARE_READ | FILE_SHARE_WRITE,			 
		     0,
		     OPEN_EXISTING,
		     FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
		     0);
    // get file size
	dwFileSize = GetFileSize(fd, &dummy);	

	// seek to start
	SetFilePointer(fd, StartOfValidPS, NULL, FILE_BEGIN);		

	// no show page yet
	ShowPageDone = FALSE;
	EndOfFile = FALSE;
	In_fp_eof = FALSE;

	// special for cat decompression
	if (CATHeaderNeeded)
       {
	   UnCompBuf = GlobalAllocPtr(GMEM_MOVEABLE, BUFSIZE);
	   fdGlobal = fd;
	   NumUncompressed = StartOfValidPS;
	   InitDecompCache();              	   
	   for (;;)
	      {
		  if (CancelFlag) break;
		  n = FillDecompCacheBuf(BUFSIZE, buf);	      
	      SetFilePercent(dwHandle, 
			 (int)((float)NumUncompressed * 100.F / (float)dwFileSize)); 
	      if (n <= 0) break;
	      err = (fp_input) (buf, n);	      
		  }
	   }
	else if (pJobInfo->FileType == FTYPE_PDF)
	   {		   	   	  
	   // copy filename
	   wsprintf(buf, "(%%os%%%s) PDFrun\n", fname);		   
	   // replace backslashes
	   for (n = 0; n < (int)strlen(buf); n++)
		  {
		  if (buf[n] == '\\') buf[n] = '/';
		  }	   
	   err = (fp_input) (buf, strlen(buf));		   
	   }
	else
	   {	   
	   NumRead += StartOfValidPS;
	   for (;;) 
	      {	
  		  if (CancelFlag) break;
	      ReadFile (fd, buf, BUFSIZE, &n, 0);
		  if (n <= 0) 
		     {
			 // if WaitForFileComplete is set, we should check for mutex ending
			 if (WaitForFileComplete)
				{	
	            // wait for 5 seconds
                dwRet = WaitForSingleObject(hMutex, 5000);
	            // end of file is signaled 
	            if (dwRet == WAIT_OBJECT_0) 
				   {
				   WaitForFileComplete = FALSE; // unset
				   }
	            // break on cancel
	            if (CancelFlag) break;
				// continue unless otherwise specified
				continue;
	            }  
			 else
				{
			    EndOfFile = TRUE;
			    break;
				}
			 }
	      NumRead += n;
		  if (IsComplete)	         
	         SetFilePercent(dwHandle, 
			    (int)((float)NumRead * 100.F / (float)dwFileSize)); 
		  else
			 SetFileTotal(dwHandle, NumRead);
	      // too much (might be EPS crap)
		  if (NumRead > (DWORD)RealFileSize && IsComplete) 
		     {
			 err = (fp_input) (buf, n - (NumRead - RealFileSize));
    		 EndOfFile = TRUE;			 
			 break;
			 }
	      err = (fp_input) (buf, n);
		  }
	   // no showpage but reached end-of-file, must be an EPS file
	   if (!ShowPageDone && EndOfFile && IsComplete)
	      err = (fp_input) ("showpage\n", 9);
	   }
	  if (IsComplete) 
		 {
		 if (!CancelFlag)
		    SetFilePercent(dwHandle, 100); 		 
		 }
	  else
         SetFileTotal(dwHandle, -1);

	  // set this so we don't do an fp_interrupt during an fp_eof
	  In_fp_eof = TRUE;
	  fp_eof();	  

    CloseHandle (fd);

	if (buf) GlobalFreePtr(buf);
	if (UnCompBuf) GlobalFreePtr(UnCompBuf);

	// nuke the mutex
    if (hMutex) 
	   {
       ReleaseMutex(hMutex);
       CloseHandle(hMutex);
	   }
	
    //return 0;  // SYN_ERRMSG_ALLOK

	return RipReturn;
}

int SendData(BYTE* buf, int len)
   {
   int err;

   err = (fp_input) (buf, len);
   //fp_eof();
   return 0;
   }

static void do_jaws_pause ()
{
    MSG			msg;

    while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
	if (msg.message == WM_QUIT)
	  break;
	TranslateMessage (&msg);
	DispatchMessage (&msg);
    }

}
#define DECOMPCACHESIZE  2048
// make a buffer with enough space to hold DECOMPCACHESIZE plus
// the extra 256 bytes that the decomp could return plus
// the max size of the header
//static BYTE DecompCache[DECOMPCACHESIZE + 256 + max(sizeof(PSHeaderDICT), sizeof(EPSHeaderDICT))];
static BYTE DecompCache[DECOMPCACHESIZE + 256 + 10000]; // guessing - must check
static short CacheCount;
static int CacheLineCount;
static BOOL DecompEOF = FALSE;
static BYTE *DecompCachePtr;

//static BYTE UnCompBuf[BUFSIZE];
static int DecompBufCountUp;
static int DecompBufCount;

static void InitDecompCache()
   {
   DecompBufCountUp = 0;
   DecompBufCount = 0;
   
   // init the lzw routines
   DecompDiscreet(LZW_INIT, DecompGet, DecompPutCache);

   // init the vars
   CacheCount = 0;
   CacheLineCount = 0;
   DecompEOF = FALSE;
   }

static int FillDecompCacheBuf(int size, char *buf)
   {
   int ret;

   ret = 0;

   do
      {
      // do we need some data
      if (CacheCount <= 0) GetDecompCache();
      // check for EOF      
      if (DecompEOF && CacheCount <= 0) break;
      do
             {
             // fill passed buffer with data from decomp cache
             *buf++ = *DecompCachePtr++;
             // decrement the passed counter and our cache counter
             size--; CacheCount--; ret++;
             // do while we still have a passed count and data in the cache
             } while (size > 0 && CacheCount > 0);
      } while (size > 0);
   return ret;
   }

static void GetDecompCache()
  {
  int count;

  // loop to get the data
  do
     {
     count = DecompDiscreet(LZW_CONTINUE, DecompGet, DecompPutCache);
     } while ((count != -1) && (CacheCount < DECOMPCACHESIZE));
  // init the up-count pointer
  DecompCachePtr = DecompCache;
  // are we at end?
  if (count == -1) DecompEOF = TRUE;
  }

static short DecompPutCache(char c)
   {
   DecompCache[CacheCount++] = (BYTE)c;
   /* do line by line until we find header spec */
   if (CacheLineCount < 8)
      {
      /* found a line feed, incr line count */
      if (c == 0x0a)
             {
             CacheLineCount++;
             if (CacheLineCount == 8)
                {
                if (CATHeaderNeeded == FULL_HEADER)
                   {
                   // copy ps header
                   memcpy(DecompCache + CacheCount, PSHeaderDICT, sizeof(PSHeaderDICT));
                   CacheCount += sizeof(PSHeaderDICT);
                   }               
                else if (CATHeaderNeeded == EPS_HEADER)
                   {
                   // copy eps header
                   memcpy(DecompCache + CacheCount, EPSHeaderDICT, sizeof(EPSHeaderDICT));
                   CacheCount += sizeof(EPSHeaderDICT);
                   }
                }   
             }
          }
   return(0);
   }

static short DecompGet()
   {
   BYTE byte;   

   if (DecompBufCount)
      {
      byte = UnCompBuf[DecompBufCountUp++];
	  NumUncompressed++;
      DecompBufCount--;
      return (short)byte;
      }
   else
      {
      ReadFile (fdGlobal, UnCompBuf, BUFSIZE, &DecompBufCount, 0);      
      DecompBufCountUp = 0;
      if (DecompBufCount)
         {
         byte = UnCompBuf[DecompBufCountUp++];
		 NumUncompressed++;
         DecompBufCount--;
	     return (short)byte;
	     }
      else
         return EOF;
      }   
   }
