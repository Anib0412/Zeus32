// OurWorkspace.cpp: implementation of the COurWorkspace class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "zeus.h"
#include "OurWorkspace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(COurWorkspace, SECWorkspaceManagerEx)

COurWorkspace::COurWorkspace()
   {
   bMainWndMaximized = FALSE;
   }

COurWorkspace::~COurWorkspace()
   {
   }

void COurWorkspace::SaveWorkspaceCustomData (SECPTNFactory& nodeFactory,
                    SECPersistentTreeNode* pRoot, const CString& strWorkspaceName) 
   { 
   SECPersistentTreeNode*  pCustomNode;
    // The pRoot PTN object passed in is the 
    // appropriate parent for this custom data. 
    // Now create a child node to store additional
    // information... 
    pCustomNode = nodeFactory.CreateNode(_T("bMainWndMaximized"),pRoot); 

    ASSERT(pCustomNode); 

    // And add the custom data. This 
    // will automatically 
    // be saved by the recursive save 
    // operation initiated 
    // by the workspace mgr - no additional 
    // coding required. 
    pCustomNode->AddKeyValueBool(
        _T("bMainWndMaximized"),
        bMainWndMaximized); 

    // We could store more key-value pairs 
    // here if we wanted. 
    // The only restriction is 
    // that each key must have a unique name. 
    // Invoke the base... 
    SECWorkspaceManagerEx::SaveWorkspaceCustomData(
        nodeFactory,pRoot,strWorkspaceName); 
} 

void COurWorkspace::OpenWorkspaceCustomData(SECPersistentTreeNode* pRoot,
                      const CString& strWorkspaceName) 
   { 
   SECPersistentTreeNode* pCustomNode;

    // Locate the custom data PTN node 
    // stored above... 
    pCustomNode=pRoot->FindChildNode(_T("bMainWndMaximized")); 

	if (pCustomNode)
	   {
       // Now, load the custom data stored in this 
       // object (again, we could load multiple 
       // key-values here if they were stored...)     
       pCustomNode->GetKeyValueBool(
                 _T("bMainWndMaximized"),
                 bMainWndMaximized); 
	   }

    // Invoke the base... 
    SECWorkspaceManagerEx::OpenWorkspaceCustomData(
        pRoot,strWorkspaceName); 
}

void COurWorkspace::SWACP() 
   {
   SetWorkspaceActiveChildFrame(NULL);
   }