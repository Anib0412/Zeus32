#ifndef __RPNODE_H__
#define __RPNODE_H__


#include "stdafx.h"

#ifndef RPDEBUGSTRMFC
// send a debug string to our debug window
#define RPDEBUGSTRMFC(a)                                  \
   {													  \
   HWND hWnd;											  \
   COPYDATASTRUCT cpds;						              \
   cpds.dwData = 0;										  \
   cpds.lpData = (PVOID)(char*)(const char*)a;	 		  \
   cpds.cbData = strlen(a) + 1;							  \
   if (hWnd = ::FindWindow("RPDebug_Class", NULL))		  \
   ::SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cpds);	  \
   }
#define RPDEBUGSTR(a)                                     \
   {													  \
   HWND hWnd;											  \
   COPYDATASTRUCT cpds;						              \
   cpds.dwData = 0;										  \
   cpds.lpData = (PVOID)a;								  \
   cpds.cbData = strlen(a) + 1;							  \
   if (hWnd = FindWindow("RPDebug_Class", NULL))		  \
      SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cpds);	  \
   }
#endif

class CRPNode : public CObject
{
	DECLARE_SERIAL(CRPNode)
public:
   CRPNode();
   ~CRPNode();

   int iType;
   int iSubType;

   CString csDisplayName;
   CString csName;
private:
};


class CPrinterQueue : public CRPNode
{
DECLARE_SERIAL(CPrinterQueue)
public:
   CPrinterQueue();
   ~CPrinterQueue();
};

class CJobs : public CRPNode
{
DECLARE_SERIAL(CJobs)
public:
   CJobs();
   ~CJobs();
};

#endif		//  __RPNODE_H__
