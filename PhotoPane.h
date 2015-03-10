// PhotopANE.h : interface of the CPhotoPane class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RASTERPLUSPHOTOVIEW_H__F83EC9FA_EDE3_4B60_86AE_DE609ED4B74B__INCLUDED_)
#define AFX_RASTERPLUSPHOTOVIEW_H__F83EC9FA_EDE3_4B60_86AE_DE609ED4B74B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include <afxcview.h>
#include "rpnode.h"

class CPhotoPane : public CListView
{
protected: // create from serialization only
	CPhotoPane();
	DECLARE_DYNCREATE(CPhotoPane)
/*
	CImageList m_imageListSmall;
	CImageList m_imageListLarge;	
	void LoadThumbs();
	CWinThread* pThumbThread;   
	void RenderThumbs();
	CRPNode* pRenderNode;		
	BOOL bCancelThumbRender;	
	BOOL bStopRender;		
	int MarkSelected();
	HANDLE hInRenderLoop;
	void CopyToClipboard(int fmt);
	UINT uiOurListClipFormat;
	void InsertDroppedPhotos(CPoint& point);
	COleDropTarget m_OleDropTarget;
	int MarkSelectedAsDelete();
	void DeleteTaggedItems();
	int PhotoIdxToNodeIdx(int iPhoto);
	void DeleteSelected();
//	void InstantiateImageFile(CPhoto* pPhoto);	
//	PRINTINFO PrintInfo;
//  CPrintDlg PrintDlg;	
//	void PrintPhotosThreading(COrder* pOrder=NULL);     	
	int iThumbnailSize;	
	void KeyDown(UINT nChar);	
*/
// Attributes
public:
	void OnTreeChanged(CRPNode* pNode);
	void CancelRendering();
	void ResetThumbsBitmapList();	
	void OnUpdateMaster(CCmdUI* pCmdUI);
	void ApplyCorrection(int iType, int iAmt=0);
	int GetCheckState(int iAdjustmentToCheck);
    void UpdateEditRedo(CCmdUI* pCmdUI);
	void UpdateEditUndo(CCmdUI* pCmdUI);
	void ResetAdjustmentDelta();
    void UpdateEditPhotoshop(CCmdUI* pCmdUI);
	void EditWithPhotoshop();
	void DoContextMenu();
	void PhotosmenuPrint(BOOL bPrintSingleCopy=FALSE);
	void RedrawItem(int idx);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhotoPane)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPhotoPane();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
   int GetFirstSelectedItem();
   int GetNumSelected();   
   
// Generated message map functions
protected:
	//{{AFX_MSG(CPhotoPane)    
/*
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPhotosmenuPrint();
	afx_msg void OnPhotosmenuDelete();
	afx_msg void OnEditSelectall();
	afx_msg void OnUpdateEditSelectall(CCmdUI* pCmdUI);
	afx_msg void OnPhotosmenuSelectall();	
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOurFilePrint();
	afx_msg void OnUpdateOurFilePrint(CCmdUI* pCmdUI);	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnOrdermenu2Makecd();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);	
	afx_msg void OnUpdatePhotosmenuPrint(CCmdUI* pCmdUI);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOrdermenu2Addphotos();
	afx_msg void OnUpdateOrdermenu2Addphotos(CCmdUI* pCmdUI);
	afx_msg LRESULT OnPrintThreadMessage(WPARAM, LPARAM);	
	afx_msg void OnEditDarken();
	afx_msg void OnUpdateEditDarken(CCmdUI* pCmdUI);
	afx_msg void OnEditLighten();
	afx_msg void OnUpdateEditLighten(CCmdUI* pCmdUI);
	afx_msg void OnEditMorecyan();
	afx_msg void OnUpdateEditMorecyan(CCmdUI* pCmdUI);
	afx_msg void OnEditLesscyan();
	afx_msg void OnUpdateEditLesscyan(CCmdUI* pCmdUI);
	afx_msg void OnEditRevert();
	afx_msg void OnUpdateEditRevert(CCmdUI* pCmdUI);
	afx_msg void OnEditMoremagenta();
	afx_msg void OnUpdateEditMoremagenta(CCmdUI* pCmdUI);
	afx_msg void OnEditLessmagenta();
	afx_msg void OnUpdateEditLessmagenta(CCmdUI* pCmdUI);
	afx_msg void OnEditMoreyellow();
	afx_msg void OnUpdateEditMoreyellow(CCmdUI* pCmdUI);
	afx_msg void OnEditLessyellow();
	afx_msg void OnUpdateEditLessyellow(CCmdUI* pCmdUI);
	afx_msg void OnEditAutolevels();
	afx_msg void OnUpdateEditAutolevels(CCmdUI* pCmdUI);
	afx_msg void OnEditGrayscale();
	afx_msg void OnUpdateEditGrayscale(CCmdUI* pCmdUI);
	afx_msg void OnEditRotateCw();
	afx_msg void OnUpdateEditRotateCw(CCmdUI* pCmdUI);
	afx_msg void OnEditRotateCcw();
	afx_msg void OnUpdateEditRotateCcw(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditMorecontrast();
	afx_msg void OnUpdateEditMorecontrast(CCmdUI* pCmdUI);
	afx_msg void OnEditLesscontrast();
	afx_msg void OnUpdateEditLesscontrast(CCmdUI* pCmdUI);
	afx_msg void OnEditMoresharp();
	afx_msg void OnUpdateEditMoresharp(CCmdUI* pCmdUI);
	afx_msg void OnEditLessSharp();
	afx_msg void OnUpdateEditLessSharp(CCmdUI* pCmdUI);
	afx_msg void OnViewCropview();
	afx_msg void OnUpdateViewCropview(CCmdUI* pCmdUI);
	afx_msg void OnViewPhotoview();
	afx_msg void OnUpdateViewPhotoview(CCmdUI* pCmdUI);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnEditPhotoshop();
	afx_msg void OnUpdateEditPhotoshop(CCmdUI* pCmdUI);
	afx_msg void OnEditRedeye();
	afx_msg void OnUpdateEditRedeye(CCmdUI* pCmdUI);
	afx_msg void OnEditZoomin();
	afx_msg void OnEditZoomout();
	afx_msg void OnUpdateEditZoomin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditZoomout(CCmdUI* pCmdUI);
	afx_msg void OnPhotosmenuPrintOne();
	afx_msg void OnUpdatePhotosmenuPrintOne(CCmdUI* pCmdUI);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnKioskDlgMessage(WPARAM, LPARAM);	
*/
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/*
#ifndef _DEBUG  // debug version in PhotogizePhotoView.cpp
inline CPhotogizeLabDoc* CPhotoPane::GetDocument()
   { return (CPhotogizeLabDoc*)m_pDocument; }
#endif
*/

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RASTERPLUSPHOTOVIEW_H__F83EC9FA_EDE3_4B60_86AE_DE609ED4B74B__INCLUDED_)
