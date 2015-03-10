/////////////////////////////////////////////////////////////////////////////
// TimeCheck.h
//                                                                        
// global time check for RasterPlus
// Copyright (C) 2001 Graphx, Inc.
//

#ifndef __TIMECHECK_H__
#define __TIMECHECK_H__

typedef struct tagTIMECHECK
   {   
   DWORD dwSecs;
   DWORD dwCount;
   } TIMECHECK, *PTIMECHECK;


class CTimeCheck : public CObject
   {
public:
   CTimeCheck();
   ~CTimeCheck();      
   BOOL SetCheck(const char* csN);
   void GetElapsed(DWORD& dwSecs);
   void GetTotalElapsed(DWORD& dwSecs);
   void GetLastSet(DWORD& dwSecs);
   void ResetElapsed();

protected:   
   HANDLE hFileMap;
   PTIMECHECK pData;
   BOOL SetData(CString& csName);
   void UnSetData();

private:
	CString csRPDebug;
	char DebugOutStr[255];
   };
#endif
