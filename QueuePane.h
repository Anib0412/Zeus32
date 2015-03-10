#if !defined(AFX_QUEUEPANE_H__0E775E6C_77BF_45D7_8BDF_35C89ACE478C__INCLUDED_)
#define AFX_QUEUEPANE_H__0E775E6C_77BF_45D7_8BDF_35C89ACE478C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QueuePane.h : header file
//

#include "SpecialTree.h"

#include "RPNode.h"
#include "objects.h"


#define NODETYPE_UNDEFINED          0x00
#define NODETYPE_CATEGORY           0x01
#define NODETYPE_LOCALDRIVE         0x02
#define NODETYPE_NETWORKDRIVE		0x04
#define NODETYPE_FOLDER             0x08
#define NODETYPE_ALBUM              0x10
#define NODETYPE_USER               0x20 
#define NODETYPE_PHOTO              0x40
#define NODETYPE_ORDER				0x80
#define NODETYPE_PRODUCT            0x100 
#define NODETYPE_IMAGE              0x200 
#define NODETYPE_ACCOUNT			0x400 

#define IMAGESIZING_FIT         0
#define IMAGESIZING_CROP        1
#define IMAGESIZING_CUSTOM      2

#define IMAGESTYLE_UNDEFINED  -1
#define IMAGESTYLE_ORIGINAL   0
#define IMAGESTYLE_SPAWN      1 

#define SUBTYPE_UNKNOWN          0x00
#define SUBTYPE_ORDER            0x01
#define SUBTYPE_PRODUCT          0x02
#define SUBTYPE_PHOTOS           0x03

/////////////////////////////////////////////////////////////////////////////
// CQueuePane view

#ifdef LAB_STYLE
class CQueuePane : public SECTreeView
#else
class CQueuePane : public CSpecialTree
#endif
{
protected:
	CQueuePane();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CQueuePane)

// Attributes
public:

//	CObArray coPrinters;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQueuePane)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate(); // called first time after construct
#ifdef LAB_STYLE
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
#endif
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQueuePane();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Data
	CObArray coNodes;	

	// Functions
	BOOL LoadPrinters();

	void PopulateStandardNodes();
	void AddNewNode( CDocument* pDoc );

protected:
	// Data
	BOOL bDataInit;

	HTREEITEM hItemPrtQueueRoot;

	// Functions
	void DoSelect(HTREEITEM hItem, BOOL bMultipleSelection=FALSE, 
										   BOOL bAllowSwitchMiddle=TRUE);
	BOOL InitCoreData();
	HTREEITEM InsertOurItem(CNode* pNode, HTREEITEM hItem, HTREEITEM hItemAfter=TVI_LAST);

	// Generated message map functions
protected:
   //{{AFX_MSG(CQueuePane)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUEUEPANE_H__0E775E6C_77BF_45D7_8BDF_35C89ACE478C__INCLUDED_)
