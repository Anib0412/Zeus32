#ifndef __WEBSERVICESASP_H__
#define __WEBSERVICESASP_H__

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


#define WEBSERVICESASP_RESP_OK              0
#define WEBSERVICESASP_RESP_NO				1
#define WEBSERVICESASP_RESP_YES             2
#define WEBSERVICESASP_RESP_ERR            -1

#define RESPONSE_PROCESS_PIPE               0
#define RESPONSE_PROCESS_XML                1
#define RESPONSE_PROCESS_NONE               3 

class CWebServicesASP : public CObject
   {
public:
   CWebServicesASP();
   CWebServicesASP(LPCSTR lpcstrServer, LPCSTR lpcstrPath);
   ~CWebServicesASP();
   CString csServer;
   CString csPath;      
   BOOL PostFormData(BOOL bSecure=FALSE, int iProcess=RESPONSE_PROCESS_PIPE); 
   CStringArray csaResultInfo;
   void ResetFormArray();
   BOOL AddFormItem(LPCSTR lpcstrKey, LPCSTR lpcstrValue);
   BOOL AddFormItem(LPCSTR lpcstrKey, int iValue);
   BOOL AddFormItems(CObArray& coFormDataArray);
   void Reset();
   int GetSingleResponse(CString& csResp);
   int GetMultipleResponse(CStringArray& csaResp);
   CString URLEncode(CString sIn);
   CString csResult;

   CString rpString;
protected:
   CObArray coFormData;   
   void ParseReturnInfo(LPCSTR csDelimChar, CString& csResponseInfo);   
   void ParseXML(char* Buff, int len);
   };
#endif