/////////////////////////////////////////////////////////////////////////////
// WINSPLIT.CPP
//                                                                        
// class that splits WinSplit-created files into their component pages
// for RasterPlus95
//
// Copyright (C) 1996 Graphx, Inc.
//
// mods:
//
// 11-25-96 Build 46 - changed remove to DeleteFile
//

#include"winsplit.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"catdef.h"
#include"decomp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* boolean values */
#define TRUE 1
#define FALSE 0

#define TBUFSIZ    20000
#define NUMBUFSIZ  15
#define LNUMBUFSIZ 15L

// if we define USECFILE, CFileStdio stuff is used - but we have
// to change the fseeks, et. al., and we should change the DECOMP

CWinSplit::~CWinSplit()
   {   
   delete FrameBuf;   
   }                                    
   
CWinSplit::CWinSplit()
   {
   // set these flags as default
   NewStyle = FALSE;
   MultiPage = TRUE;   
   FramesToDo = TotalFrames = 0;
   
   // get an uncompressed filename   
   //GetTempFileName(0, "tmp", 0, UncompressedFileName.GetBuffer(_MAX_PATH));
   CString csTempPath;
   GetTempPath(_MAX_PATH, csTempPath.GetBuffer(_MAX_PATH));
   csTempPath.ReleaseBuffer();
   GetTempFileName(csTempPath, "tmp", 0, 
      UncompressedFileName.GetBuffer(_MAX_PATH));   
   UncompressedFileName.ReleaseBuffer();      
   
   // alloc a new buffer to hold the header stuff
   FrameBuf = new long[1000];
   }                                    

int CWinSplit::SplitDiscrete(CString& iff)
   {                        
   InputFile = iff;
   
   DiscreteSplitCount = 0;
   // get total frames
   return (FramesToDo = TotalFrames = GetWinFrame(InputFile, 0));      
   }      
   
int CWinSplit::SplitDiscrete(CString* csSf)
   {      
   CString FrameFileName;         
   char tempbuf[_MAX_PATH];   
   
   // make a new filename
   FrameFileName = FilePath;
   sprintf(tempbuf, "FRAME%02d.PS", DiscreteSplitCount + 1);	     
   FrameFileName += tempbuf;
   // load this filename
   *csSf = FrameFileName;
   // construct it from the uncompressed file if CAT is
   // enabled, otherwise use input file
   if (ConstructWinFrame(CatFileFlag ? UncompressedFileName : InputFile, 
      FrameFileName, DiscreteSplitCount++) != 0)
      {
      // file error!                                                                 
      if (CatFileFlag) ::DeleteFile(UncompressedFileName);   
      return(-1);
      }
   FramesToDo--;
   // if a cat file, remove it         
   if (FramesToDo == 0 && CatFileFlag) ::DeleteFile(UncompressedFileName);   
   
   return FramesToDo;	  	  
   }	        

int CWinSplit::Split(CString& iff)
   {      
   short i;
   CString FrameFileName;         
   char tempbuf[_MAX_PATH];
   
   InputFile = iff;
   
   // get total frames
   if ((FramesToDo = TotalFrames = GetWinFrame(InputFile, 0)) != 0)
      {
      for (i = 0; i < FramesToDo; i++)
	     {
	     // make a new filename
	     FrameFileName = FilePath;
	     sprintf(tempbuf, "FRAME%02d.PS", i + 1);	     
	     FrameFileName += tempbuf;
	     // construct it from the uncompressed file if CAT is
	     // enabled, otherwise use input file
	     if (ConstructWinFrame(CatFileFlag ? UncompressedFileName : InputFile, 
	        FrameFileName, i) != 0)
	        {
	        // file error!                                                                 
            FramesToDo = -1;
            break;
            }	        
	     }
      }            
   // if a cat file, remove it         
   if (CatFileFlag) ::DeleteFile(UncompressedFileName);
   return FramesToDo;   
   }      

short CWinSplit::GetWinFrame(CString& fname, short NumWrote)
   {
#ifdef USECFILE
   CStdioFile pfile;
#else
   FILE* pfile;
#endif   
   char* p;
   short ret;
   short NumFrames, i;
   long lNumFrames;
   long SaveFilePos;
   char LocalLineBuf[_MAX_PATH];   
   BYTE EOFByte;
   long CPSStartPos;   
   long OffsetList;
   long EOFoffset;

#ifdef USECFILE
   /* open file */
   if (!pfile.Open((const char*)fname, CFile::modeRead | CFile::typeBinary))
      return 0;
#else
   if ((pfile = fopen(fname, "rb")) == NULL)
      {
      // error opening file
      return(0);
      } 
#endif
      
   // extract path from long file name
   p = FilePath.GetBuffer(_MAX_PATH);
   // ...using LocalLineBuf as trash
   _splitpath((const char*)fname, p, p + 2, LocalLineBuf, LocalLineBuf);
   FilePath.ReleaseBuffer();

   /* check for WinSlide tag */   
#ifndef USECFILE
   fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile);
   fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile);
#else
   if (!Fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile)) return 0;
   if (!Fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile)) return 0;
#endif
   if ((strstr(LocalLineBuf, "WinSlide") == NULL) &&
       (strstr(LocalLineBuf, "GraphxPS") == NULL))
      {
#ifndef USECFILE
      fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile);
#else
      if (!Fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile)) return 0;     
#endif
      if ((strstr(LocalLineBuf, "WinSlide") == NULL) &&
	  (strstr(LocalLineBuf, "GraphxPS") == NULL))
	     {
#ifndef USECFILE
	     // not a winslide ps file
	     fclose(pfile);
#else
         pfile.Close();
#endif
	     return(0);
	     }
      }  
   /* check for CPS file */
   if (strstr(LocalLineBuf, "(CAT!)") != NULL)
      {
      CatFileFlag = TRUE;			   
      /* save file pos */
      SaveFilePos = ftell(pfile);
      /* get more lines */
#ifndef USECFILE
      fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile); // title
      fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile); // bbox
      fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile); // needed res
      fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile); // supplied res
      fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile); // pages
      fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile); // procset
      fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile); // header needed
#else
      Fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile); // title
      Fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile); // bbox
      Fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile); // needed res
      Fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile); // supplied res
      Fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile); // pages
      Fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile); // procset
      Fgets(LocalLineBuf, sizeof(LocalLineBuf), pfile); // header needed
#endif
      HeaderNeeded = atoi(LocalLineBuf + 22);           // get number
      EOFByte = fgetc(pfile);                           // get EOF
      /* get pos of start of compression */
      CPSStartPos = ftell(pfile);
      /* close current file */
#ifndef USECFILE
      fclose(pfile);
#else
      pfile.Close();
#endif
      /* check eof */
      if (EOFByte != 0x1a)
	     {
	     // not a cat file
	     return(0);
	     }
      // decompress it
      if (DecompCPSFile(fname, UncompressedFileName, CPSStartPos) != 0)
	     {
	     // error decompressing
	     return(0);
	     }
      else
	     {	     
	     /* re-open */
	     if ((pfile = fopen(UncompressedFileName, "r+b")) == NULL)
	        {
	        // can't open file
	        return(0);
	        }
	     /* reset file pos */
	     fseek(pfile, SaveFilePos, SEEK_SET);
	     }
      }
   else
      CatFileFlag = FALSE;

   /* check for WinSlide multi-page file */
   fseek(pfile, -11L, SEEK_END);
   fread(LocalLineBuf, 11, 1, pfile);

   /* if last two chars are cr, lf, go back */
   if ((LocalLineBuf[9] == 0x0d) && (LocalLineBuf[10] == 0x0a))
      {
      fseek(pfile, -13L, SEEK_END);
      fread(LocalLineBuf, 11, 1, pfile);
      }

   /* if last char is a cr, EOF, go back */
   if ((LocalLineBuf[8] == 0x0d) && (LocalLineBuf[9] == 0x0a) &&
       (LocalLineBuf[10] == 0x04))
      {
      fseek(pfile, -14L, SEEK_END);
      fread(LocalLineBuf, 11, 1, pfile);
      EOFoffset = -1;
      }
   else
      {
      EOFoffset = 0;
      }

   if (memcmp(LocalLineBuf + 2, "MULTIPAGE", 9) == 0)
      {
      /* get number of frames */
      fseek(pfile, -13L, SEEK_END);
      fread(&lNumFrames, sizeof(lNumFrames), 1, pfile);
      /* seek to beginning of list */
      fseek(pfile, -4L, SEEK_CUR);
      fseek(pfile, -4L * (lNumFrames + 1), SEEK_CUR);
      /* read offsets */
      NumFrames = (short)lNumFrames;
      for (i = 0; i <= NumFrames; i++)
	     {
	     fread(&FrameBuf[i], sizeof(long), 1, pfile);
	     /* add transform stuff counter */
	     FrameBuf[i] += (long)NumWrote;
	     }
      ret = NumFrames;
      }
   else if (memcmp(LocalLineBuf, "MULTIPAGE-2", 11) == 0)
      {
      NewStyle = TRUE;
      /* get number of frames */
      fseek(pfile, -28L + EOFoffset, SEEK_END);
      fread(LocalLineBuf, NUMBUFSIZ, 1, pfile);
      lNumFrames = atol(LocalLineBuf);
      /* seek to beginning of list */
      fseek(pfile, -30L + EOFoffset, SEEK_CUR);
      fseek(pfile, -LNUMBUFSIZ * lNumFrames, SEEK_CUR);
      /* get pos */
      OffsetList = ftell(pfile) - 3L;
      /* read offsets */
      NumFrames = (short)lNumFrames;
      for (i = 0; i <= NumFrames; i++)
	     {
	     fread(LocalLineBuf, NUMBUFSIZ, 1, pfile);
	     FrameBuf[i] = atol(LocalLineBuf);
	     /* add transform stuff counter */
	     FrameBuf[i] += (long)NumWrote;
	     }
      /* if CAT!, re-write offsets, adding header offsets */
      if (CatFileFlag)
	     {
	     fseek(pfile, OffsetList, SEEK_SET);
	     for (i = 0; i <= NumFrames; i++)
	        {
	        if (HeaderNeeded == FULL_HEADER)
	           FrameBuf[i] += sizeof(PSHeaderDICT);
	        if (HeaderNeeded == EPS_HEADER)
	           FrameBuf[i] += sizeof(EPSHeaderDICT);
	        sprintf(LocalLineBuf, "%s%-10ld\r\n", "%  ", FrameBuf[i]);
	        fwrite(LocalLineBuf, strlen(LocalLineBuf), 1, pfile);
	        }
	     }
      ret = NumFrames;
      }
   else
      {
      // not a multi-page file
      MultiPage = FALSE;
      ret = 0;
      }

#ifndef USECFILE
   fclose(pfile);
#else
   pfile.Close();
#endif
   return(ret);
   }

/***************************************************************************/
/*                                CopyChunk                                */
/***************************************************************************/

BOOL CWinSplit::CopyChunk(BYTE *Buffer, unsigned long BufSiz,
		       FILE *infile, FILE *outfile,
		       unsigned long start, unsigned long end)
   {
   unsigned long len;
   size_t numread;

   /* get copy length */
   len = end - start;
   
   TRACE("CWinSplit: Copying %lx bytes from %lx to %lx\n", len, start, end);

   /* seek start */
   fseek(infile, start, SEEK_SET);

   /* copy chunk to new file */
   do
      {
      numread = fread(Buffer, 1, (size_t)(min(BufSiz, len)), infile); 
      if (numread == 0)                   
         break;         
      if (fwrite(Buffer, 1, numread, outfile) != numread)
         {
         break;
         return FALSE;
         }
      } while ((len -= (size_t)numread) > 0);
   if (len > 0) 
      return FALSE;
   else
      return TRUE;   
   }         

/***************************************************************************/
/*                             ConstructWinFrame                           */
/***************************************************************************/

short CWinSplit::ConstructWinFrame(CString& fname, CString& tfname, short FrameNum)
   {
   FILE *infile, *outfile;
   BYTE *Buffer;
   unsigned long flen;

   if ((Buffer = (BYTE *)new char[TBUFSIZ]) == NULL)
      return(-1);

   if ((infile = fopen(fname, "rb")) == NULL)
      {
      delete Buffer;
      return(-1);
      }
   if ((outfile = fopen(tfname, "wb")) == NULL)
      {
      delete Buffer;
      fclose(infile);
      return(-1);
      }

   /* write prelude */
   CopyChunk(Buffer, TBUFSIZ, infile, outfile, 0L, FrameBuf[0]);

   /* write chunk */
   CopyChunk(Buffer, TBUFSIZ, infile, outfile, FrameBuf[FrameNum],
	     FrameBuf[FrameNum + 1]);

   /* get filelength */
   fseek(infile, 0L, SEEK_END);
   flen = ftell(infile);

   /* subtract MULTIFILE junk */
   if (NewStyle)
      flen = flen - 16L - LNUMBUFSIZ * (long)(TotalFrames + 2);
   else
      flen = flen - 13L - 4L * (long)(TotalFrames + 1);

   /* write trailer */
   CopyChunk(Buffer, TBUFSIZ, infile, outfile, FrameBuf[TotalFrames], flen);

   fclose(infile);
   fclose(outfile);
   delete Buffer;
   return(0);
   }

/***************************************************************************/
/* DecompCPSFile                                                           */
/*                                                                         */
/* decompressed CPS file                                                   */
/***************************************************************************/

int CWinSplit::DecompCPSFile(CString& infname, CString& outfname, long StartPos)
   {   
   int ret;
   CDecomp* pCD;

   ret = -1;
   crcval = 0;   
   
   pCD = new CDecomp;

   if ((GlobalInFile = fopen(infname, "rb")) != NULL)
      {
      if ((GlobalOutFile = fopen(outfname, "wb")) != NULL)
	     {
	     /* seek start of compression */
	     fseek(GlobalInFile, StartPos, SEEK_SET);
	     /* decompress file */
	     if (pCD->Start(GlobalInFile, GlobalOutFile, HeaderNeeded) == 0)
	        pCD->Finish(); 
	     fclose(GlobalOutFile);
	     }
      fclose(GlobalInFile);
      }         
   delete pCD;   
   return(0);
   }

#ifdef USECFILE
BOOL CWinSplit::Fgets(char* buf, int buflen, CStdioFile& cfile)
   {
   BOOL ret;

   ret = TRUE;

   TRY
      {
      cfile.ReadString(buf, buflen);
      }
   CATCH(CException, e)
      {      
      ret = FALSE;
      }
   return ret;
   }
#endif
