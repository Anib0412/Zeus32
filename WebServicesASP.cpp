#include "stdafx.h"
#include <afxinet.h>
#include "WebServicesASP.h"
#include "WebFormData.h"
#include "..\Expat-1.95.8\Source\lib\expat.h"

CWebServicesASP::CWebServicesASP()
   {
   }

CWebServicesASP::CWebServicesASP(LPCSTR lpcstrServer, LPCSTR lpcstrPath)
   {
   csServer = lpcstrServer;
   csPath = lpcstrPath;
   }

CWebServicesASP::~CWebServicesASP()
   {
   ResetFormArray();
   }

void CWebServicesASP::Reset()
   {
   ResetFormArray();
   csaResultInfo.RemoveAll();
   }

void CWebServicesASP::ResetFormArray()
   {
   int i = 0;
   CFormData* pData;

   while (i < coFormData.GetSize())
      {
	  if (pData = (CFormData*)coFormData.GetAt(i++))
		 delete pData;
      }     
   coFormData.RemoveAll();   
   }

BOOL CWebServicesASP::AddFormItem(LPCSTR lpcstrKey, LPCSTR lpcstrValue)
   {
   BOOL bRet = FALSE;
   CFormData* pData;

   if (pData = new CFormData)
	  {
	  pData->csKey = lpcstrKey;
	  pData->csValue = lpcstrValue;
	  coFormData.Add((CObject*)pData);
	  bRet = TRUE;
	  }
   return bRet;
   }

BOOL CWebServicesASP::AddFormItem(LPCSTR lpcstrKey, int iValue)
   {
   BOOL bRet = FALSE;
   CFormData* pData;

   if (pData = new CFormData)
	  {
	  pData->csKey = lpcstrKey;
	  pData->csValue.Format("%d", iValue);
	  coFormData.Add((CObject*)pData);
	  bRet = TRUE;
	  }
   return bRet;
   }

BOOL CWebServicesASP::AddFormItems(CObArray& coFormDataArray)
   {
   BOOL bRet = FALSE;
   CFormData* pData;
   CFormData *pDataIn;

   if (coFormDataArray.GetSize() < 1) return FALSE;

   for (int i = 0; i < coFormDataArray.GetSize(); i++)
	  {
	  if (pDataIn = (CFormData*)coFormDataArray.GetAt(i))
		 {
		 if (pData = new CFormData)
			{
			pData->csKey = pDataIn->csKey;
			pData->csValue = pDataIn->csValue;
			coFormData.Add((CObject*)pData);
			bRet = TRUE;
			}
		 }	
	  }
   return bRet;
   }

int CWebServicesASP::GetSingleResponse(CString& csResp)
   {
   int iRet;
   CString csTemp;

   // defaults
   iRet = WEBSERVICESASP_RESP_ERR;
   csResp = "Unknown error";
   
   if (csaResultInfo.GetSize() > 0)
	  {
	  csTemp = csaResultInfo.GetAt(0);
	  csTemp.TrimLeft();
	  csTemp.TrimRight();
	  if (csTemp.CompareNoCase("ok") == 0)
		 {
		 iRet = WEBSERVICESASP_RESP_OK;
		 if (csaResultInfo.GetSize() > 1)			
			{
			csResp = csaResultInfo.GetAt(1);		 
			csResp.TrimLeft();
			csResp.TrimRight();
			}
		 else	
			csResp.Empty();
		 }
	  else if (csTemp.CompareNoCase("error") == 0)
		 {
		 // get the error
		 if (csaResultInfo.GetSize() > 1)			
			{
			csResp = csaResultInfo.GetAt(1);		 
			csResp.TrimLeft();
			csResp.TrimRight();
			}
		 }
	  else if (csTemp.CompareNoCase("activationcode") == 0)
		 {
		 iRet = WEBSERVICESASP_RESP_OK;
		 // get the error
		 if (csaResultInfo.GetSize() > 1)			
			{
			csResp = csaResultInfo.GetAt(1);		 
			csResp.TrimLeft();
			csResp.TrimRight();
			}
		 }
	  else if (csTemp.CompareNoCase("statuscheck") == 0)
		 {
		 iRet = WEBSERVICESASP_RESP_OK;
		 // get the error
		 if (csaResultInfo.GetSize() > 1)			
			{
			csResp = csaResultInfo.GetAt(1);		 
			csResp.TrimLeft();
			csResp.TrimRight();
			}
		 }
	  }
   return iRet;
   }

int CWebServicesASP::GetMultipleResponse(CStringArray& csaResp)
   {
   int i;
   int iRet;
   CString csTemp;
   CString csUnknown;

   csUnknown = "Unknown error";
   
   if (csaResultInfo.GetSize() > 0)
	  {
	  csTemp = csaResultInfo.GetAt(0);
	  csTemp.TrimLeft();
	  csTemp.TrimRight();
	  if (csTemp.CompareNoCase("ok") == 0)
		 {
		 iRet = WEBSERVICESASP_RESP_OK;		 
		 // get the extra stuff
		 for (i = 1; i < csaResultInfo.GetSize(); i++)
			{
			csTemp = csaResultInfo.GetAt(i);		 
			csTemp.TrimLeft();
			csTemp.TrimRight();
			csaResp.Add(csTemp);
			}
		 }
	  else if (csTemp.CompareNoCase("yes") == 0)
		 {
		 iRet = WEBSERVICESASP_RESP_YES;		 
		 // get the extra stuff
		 for (i = 1; i < csaResultInfo.GetSize(); i++)
			{
			csTemp = csaResultInfo.GetAt(i);		 
			csTemp.TrimLeft();
			csTemp.TrimRight();
			csaResp.Add(csTemp);
			}
		 }
	  else if (csTemp.CompareNoCase("no") == 0)
		 {
		 iRet = WEBSERVICESASP_RESP_NO;		 
		 }
	  else if (csTemp.CompareNoCase("error") == 0)
		 {
		 iRet = WEBSERVICESASP_RESP_ERR;
		 // get the error
		 if (csaResultInfo.GetSize() > 1)			
			{
			csTemp = csaResultInfo.GetAt(1);		 
			csTemp.TrimLeft();
			csTemp.TrimRight();
			csaResp.Add(csTemp);
			}
		 }
	  else
		 {
		 iRet = WEBSERVICESASP_RESP_ERR;
		 csaResp.Add(csUnknown);
		 }
	  }
   else
	  {
	  iRet = WEBSERVICESASP_RESP_ERR;
	  csaResp.Add(csUnknown);
	  }
   return iRet;
   }

			  
BOOL CWebServicesASP::PostFormData(BOOL bSecure, int iProcess)
   {
   int i;   
   CString strFormData;
   CString csValue;
   DWORD dwRet;
   CInternetSession session;   
   TCHAR sz[255];	
   BOOL bRet = FALSE;   
   CFormData* pData;
   CString csTemp; 
   CHttpConnection* pConnection = NULL;
   CHttpFile* pFile = NULL;
   
   // empty the strings
   csResult.Empty();   
   strFormData.Empty();

   // remove all the response strings
   csaResultInfo.RemoveAll();
   
   CString strHeaders =
      _T("Content-Type: application/x-www-form-urlencoded");
   
   // create the forms post
   for (i = 0; i < coFormData.GetSize(); i++)
	  {
	  if (pData = (CFormData*)coFormData.GetAt(i))
		 {
		 csValue = URLEncode(pData->csValue);
		 csTemp.Format("%s=%s", pData->csKey, csValue);
		 strFormData += csTemp;
		 if (i < (coFormData.GetSize() - 1))
			strFormData += "&";
		 }	  
	  }

   if (bSecure)
      pConnection = session.GetHttpConnection(csServer, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);
   else
      pConnection = session.GetHttpConnection(csServer, (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);

   if (!pConnection) return FALSE;

   if (bSecure)
      pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, csPath, NULL, 1, NULL, NULL, 
	  INTERNET_FLAG_SECURE | INTERNET_FLAG_EXISTING_CONNECT);
   else
	  pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, csPath, NULL, 1, NULL, NULL, 							  
	  INTERNET_FLAG_EXISTING_CONNECT);

   if (!pFile)
	  {
   	  pConnection->Close();
	  delete pConnection;
	  return FALSE;
	  }	

   try
	  {
	  if (pFile->SendRequest(strHeaders,
		 (LPVOID)(LPCTSTR)strFormData, strFormData.GetLength()))
		 { 
		 pFile->QueryInfoStatusCode(dwRet);
		 // read the file one buffer at a time and
		 // concatenate to the output string reference
		 //if (dwRet == HTTP_STATUS_OK) 
		 if (1)
			{
			while (pFile->ReadString(sz, 255)) 
			   {
			   csResult += sz;
			   }

			if (iProcess == RESPONSE_PROCESS_XML)
			   ParseXML((char*)(const char*)csResult, csResult.GetLength());
			else if (iProcess == RESPONSE_PROCESS_PIPE)
			   ParseReturnInfo("|", csResult);
			else
			   csaResultInfo.Add(csResult);
			
			bRet = TRUE;
			}  	  
		 }

	  if (pFile) 
		 {
		 pFile->Close();
		 delete pFile;
		 }
	  if (pConnection)
		 {
		 pConnection->Close();
		 delete pConnection;
		 }
	  }
   catch(...)
	  {
	  bRet = FALSE;
	  }
   return bRet;
   }

void CWebServicesASP::ParseReturnInfo(LPCSTR csDelimChar, CString& csResponseInfo)
   {
   CString csTemp;
   
   int iStart = 0;
   int iLength = 0;
   int iResponseLen;
   csaResultInfo.RemoveAll();
   
   iResponseLen = csResponseInfo.GetLength();
   
   for ( iLength = 0; iLength != -1; )
	  {
	  iLength = csResponseInfo.Find(csDelimChar, iStart);
	  
	  csTemp = csResponseInfo.Mid(iStart, iLength - iStart);
	  
	  if (csTemp.GetLength() > 0) csaResultInfo.Add(csTemp);
	  iStart = iLength + 1;
	  }
   }

inline BYTE toHex(const BYTE &x)
{
	return x > 9 ? x + 55: x + 48;
}

CString CWebServicesASP::URLEncode(CString sIn)
{
	CString sOut;
	
	const int nLen = sIn.GetLength() + 1;

	register LPBYTE pOutTmp = NULL;
	LPBYTE pOutBuf = NULL;
	register LPBYTE pInTmp = NULL;
	LPBYTE pInBuf =(LPBYTE)sIn.GetBuffer(nLen);
	BYTE b = 0;
	
	//alloc out buffer
	pOutBuf = (LPBYTE)sOut.GetBuffer(nLen*3 - 2);//new BYTE [nLen  * 3];

	if(pOutBuf)
	{
		pInTmp	= pInBuf;
		pOutTmp = pOutBuf;
		
		// do encoding
		while (*pInTmp)
		{
			if(isalnum(*pInTmp))
				*pOutTmp++ = *pInTmp;
			else
				if(isspace(*pInTmp))
					*pOutTmp++ = '+';
				else
				{
					*pOutTmp++ = '%';
					*pOutTmp++ = toHex(*pInTmp>>4);
					*pOutTmp++ = toHex(*pInTmp%16);
				}
			pInTmp++;
		}
		*pOutTmp = '\0';
		//sOut=pOutBuf;
		//delete [] pOutBuf;
		sOut.ReleaseBuffer();
	}
	sIn.ReleaseBuffer();
	return sOut;
}

static CString csXMLResult;

static void XMLCALL
start(void *data, const char *el, const char **attr)
{
}

static void XMLCALL
end(void *data, const char *el)
{
}

static void XMLCALL
character(void *userData,const XML_Char *s, int len)
{
char pBuf[4096];

if (len > 0)
   {
   ZeroMemory(pBuf, 4096);
   memcpy(pBuf, s, min(len, 4095));
   csXMLResult = pBuf;
   }
}	

void CWebServicesASP::ParseXML(char* Buff, int len)
   {	 
   XML_Parser p;

   if (p = XML_ParserCreate(NULL))
	  {
	  XML_SetElementHandler(p, start, end);
	  XML_SetCharacterDataHandler(p, character);
	  if (XML_Parse(p, Buff, len, 1) != XML_STATUS_ERROR) 
		 {									  
		 csaResultInfo.RemoveAll();
		 // if csXMLResult is "error", don't say "OK"
		 if (csXMLResult.CompareNoCase("error") == 0)
		    csaResultInfo.Add("error");
		 else
			csaResultInfo.Add("ok");			
		 csaResultInfo.Add(csXMLResult);
		 }
	  XML_ParserFree(p);
	  }
   }
