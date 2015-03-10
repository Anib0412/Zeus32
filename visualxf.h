/////////////////////////////////////////////////////////////////////////////
// visualxf.h
//                                                                        
// visual transform dialog for RasterPlus95
// Copyright (C) 1996-99 Graphx, Inc.
//

#ifndef __JOBSIZEPROPS_H__
#define __JOBSIZEPROPS_H__

#include "canvas.h"
#include "ruler.h"
#include "transfrm.h"
#include "cqueue.h"
#include "crip.h"
#include "ripstruct.h"
#include "transformlist.h"
#include "ColorTransform.h"

#define XZOOM_IDX     0
#define YZOOM_IDX     1
#define XOFF_IDX      2
#define YOFF_IDX      3
#define ROTATION_IDX  4
#define MAX_EDIT_IDX  5

#define XFORM_UNITS_PICA    0
#define XFORM_UNITS_INCH    1
#define XFORM_UNITS_MM      2

/////////////////////////////////////////////////////////////////////////////
// CJobSizeProps dialog				    

class CJobSizeProps : public CPropertyPage
{
DECLARE_DYNCREATE(CJobSizeProps) 
// Construction
public:
	CJobSizeProps();
	CTransform& GetDefTransform();
	~CJobSizeProps(); 
	CanvasWnd cw;
	CRuler TopRuler;
	CRuler LeftRuler;
	CRuler CornerRuler;
	CQueueFile* pTargetCQueue;
	CDevice* pTargetDevice;	
	CColorTransform* pLocalColor;
	CRip cr;                                    		
	CTransform OriginalTransform;
	float fDevInchWidth;
	BOOL m_should_repaint;
	CString PicaNumToDisplay(float fNum);
	float fPrevFactor[3];
	void UpdateIt(BOOL MultipleSelection = FALSE);
	BOOL DialogInit;
	void LoadStandardXforms();	
	CString csCustom;
	CTransformList ctl;
	void EnableCustomEditing(BOOL bFlag);
	CWnd* pReportWnd;
	void ReportChange(int ItemChanged);
	BOOL CustomEdit;
	BOOL DoingDefaultProps;
	void SetTransform(CTransform* pTransform);
	void SetPreview();
	void FrameShow(BOOL bShow);
	BOOL	m_preview;
	BOOL bVarsSet;
	void GetVars();
	void CleanUp();
	void ResetPreviewWnd();
	BOOL bLastMultipleSelection;
	CString csScratchPath;

    CDevice* pLastDefaultDevice;
    CTransform* pLastDefaultTransform;
    CColorTransform* pLastLocalColor;
    CQueueFile* pLastTargetQueue;
	BOOL bForceShow;
	CRect cMediaRect;

	BOOL m_xzoom_is_indeterminate;
	BOOL m_yzoom_is_indeterminate;
	BOOL m_xoff_is_indeterminate;
	BOOL m_yoff_is_indeterminate;
	BOOL m_rotation_is_indeterminate;

	int m_LastRotation;
	
protected:
    void InsertNewVals();
    int TimerID;
    int xzoom_is_invalid, yzoom_is_invalid;
	int xoff_is_invalid, yoff_is_invalid;
	int height_is_invalid, width_is_invalid;
    int LastTickCount;
	void MoveDialog();    
    CString DisplayTextToPica(CString& csIn);
	CString PicaTextToDisplay(CString& csIn);
	float DisplayTextToPicaNum(CString& csIn);
	BOOL VerifyCanvas();
	DWORD dwCanvasStyle;
	DWORD dwCanvasParentStyle;
	CanvasParentWnd cwCanvasParent;
	CRect cRectRuler;
	void SetRulerRanges();	
	RIPSTRUCT RipStruct;	
	CTransform DefaultTransform;	
	void SetButtons();

public:
// Dialog Data
	//{{AFX_DATA(CJobSizeProps)
	enum { IDD = IDD_JOBSIZEPROPERTIES };
	CProgressCtrl	m_rip_status_progress;
	BOOL	m_snap;
	CString	m_xoff;
	CString	m_xzoom;
	CString	m_yoff;
	CString	m_yzoom;
	CString	m_targetdevice;
	CString	m_targetfile;
	CString	m_start_type;
	CString	m_description;
	BOOL	m_retain_aspect;		
	CString	m_rip_status_prmpt;
	int		m_rotation;
	int		m_units;	
	int		m_name;
	int		m_preview_size;
	CString	m_width;
	CString	m_height;
	//}}AFX_DATA

// Implementation
protected:    
    CRect CanRect;	
	CRect OldCanRect;
	CString csTemp;
	int DegreeToMap(CString& csDegree);
	void MapToDegree(CString& csRet, int map);
	float MapToFloat(int map);
	void ValidateAll();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CJobSizeProps)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CJobSizeProps)
	virtual BOOL OnInitDialog();	
	virtual void OnOK();
	afx_msg void OnSnap();
	afx_msg void OnChangeXoffEdit();
	afx_msg void OnChangeXzoomEdit();
	afx_msg void OnChangeYzoomEdit();
	afx_msg void OnRepaint();
	afx_msg LRESULT OnPrevSetInfo(WPARAM, LPARAM);
	afx_msg LRESULT OnRipMessage(WPARAM, LPARAM);
	afx_msg LRESULT OnRipComplete(WPARAM, LPARAM);
	afx_msg void OnStop();
	afx_msg void OnRetainAspect();		
	afx_msg void OnChangeYoffEdit();
	afx_msg void OnSelchangeStartType();
	afx_msg void OnDeltaposXoffSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposXzoomSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposYoffSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposYzoomSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLoad();
	afx_msg void OnSaveas();
	afx_msg void OnReset();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);	
	afx_msg void OnSelchangeRotation();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCancel();	
	afx_msg void OnSelchangeUnits();
	afx_msg void OnSelchangeTransform();
	afx_msg void OnPreview();
	afx_msg void OnSelchangePreviewSize();
	afx_msg void OnDelete();
	afx_msg void OnDestroy();
	afx_msg void OnChangeHeightEdit();
	afx_msg void OnChangeWidthEdit();
	afx_msg void OnDeltaposWidthSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposHeightSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
