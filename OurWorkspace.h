// OurWorkspace.h: interface for the COurWorkspace class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OURWORKSPACE_H__2D7C8E75_CF83_11D2_8DFD_00C04F7975DC__INCLUDED_)
#define AFX_OURWORKSPACE_H__2D7C8E75_CF83_11D2_8DFD_00C04F7975DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COurWorkspace : public SECWorkspaceManagerEx  
{
DECLARE_DYNCREATE(COurWorkspace) 
public:
	COurWorkspace();
	virtual ~COurWorkspace();
    void SaveWorkspaceCustomData (SECPTNFactory& nodeFactory,
                    SECPersistentTreeNode* pRoot, const CString& strWorkspaceName); 
    void OpenWorkspaceCustomData(SECPersistentTreeNode* pRoot,
                      const CString& strWorkspaceName);
	void SWACP();
	BOOL bMainWndMaximized;    
};

#endif // !defined(AFX_OURWORKSPACE_H__2D7C8E75_CF83_11D2_8DFD_00C04F7975DC__INCLUDED_)
