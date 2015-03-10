/////////////////////////////////////////////////////////////////////////////
// WINSPLIT.H
//                                                                        
// file splitting stuff for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef _WINSPLIT_H_
#define _WINSPLIT_H_

#include <afxext.h>

class CWinSplit : public CObject
   {            
public: 
   CWinSplit();  
   int Split(CString&);
   int SplitDiscrete(CString&);
   int SplitDiscrete(CString*);
   ~CWinSplit();

protected:
   int NewStyle;
   int TotalFrames;     
   long *FrameBuf;
   CString InputFile;
   CString UncompressedFileName;                               
   CString FilePath;
   int HeaderNeeded;
   int MultiPage;     
   size_t GlobalOutCount; 
   short crcval;    
   FILE *GlobalInFile;
   FILE *GlobalOutFile;
   BOOL CatFileFlag;   
   int DiscreteSplitCount;   
   int FramesToDo;
#ifdef USECFILE
   BOOL Fgets(char* buf, int buflen, CStdioFile& cfile);
#endif
public:   
   short GetWinFrame(CString&, short);
   short ConstructWinFrame(CString&, CString&, short); 
   int DecompCPSFile(CString&, CString&, long);
   BOOL CopyChunk(BYTE *, unsigned long, FILE *, FILE *, unsigned long, unsigned long);
   };
#endif
   
   
