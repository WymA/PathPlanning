// RobotView.h : interface of the CRobotView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROBOTVIEW_H__E17FE45E_076B_4A19_8BBB_86632A16E4EF__INCLUDED_)
#define AFX_ROBOTVIEW_H__E17FE45E_076B_4A19_8BBB_86632A16E4EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "RobotDoc.h"

//////////////////////////////////////////////////////////////////
//## 2013/11/5 //////////////////////////////////////////////////
UINT ThreadFunc(LPVOID pParam) ;

/////////////////////////////////////////////////////////////////////

class CRobotView : public CView
{
protected: // create from serialization only
	CRobotView();
	DECLARE_DYNCREATE(CRobotView)

	//////////////////////////////////////////////////////////////
	//# Added on 2013/9/11 //////////////////////////////////
public :
	int m_test_method ;

// Attributes
public:
	CRobotDoc* GetDocument();
	int m_cxWnd,m_cyWnd;
	int m_origX,m_origY;
	int gridWidth;
	bool m_bAddBlock;
	bool m_bEraseBlock;
	bool m_bSetPara;
	bool m_bNeedInit;
	bool* m_preState;
	int m_CurOpID;
	CWinThread* m_pThread;
	fstream ff;

// Operations
public:
	void DrawChart(CDC* pDC,int originX,int originY);
	void DrawBlocks(CDC* pDC,int originX,int originY);
	void DrawPath(CDC* pDC,vector<int>* path, COLORREF m_PathColor);
	void ShowPara(CDC* pDC);
	void DrawPareto(CDC* pDC,int originX,int originY) ;
	void DrawAll(CDC* pDC) ;
	void DrawPoint( CDC* pDC,int originX,int originY ) ;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRobotView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRobotView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public :
	//##2013/11/3
	//NSGA2 *method_nsga2;
	//MOEAD *method_moead ;
	//CAEA *method_caea ;

// Generated message map functions
protected:
	//{{AFX_MSG(CRobotView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBtstart();
	afx_msg void OnBtpara();
	afx_msg void OnBtresult() ;
	afx_msg void OnMenupara();
	afx_msg void OnMenustart();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBtpause();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnUpdateNsga2(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMoead(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCaea(CCmdUI *pCmdUI);
	afx_msg void OnNsga2();
	afx_msg void OnMoead();
	afx_msg void OnCaea();
	afx_msg void OnBtstop();
	afx_msg void OnOptonly();
	afx_msg void OnUpdateOptonly(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBtstart(CCmdUI *pCmdUI);
//	afx_msg void OnUpdateBtstop(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBtpause(CCmdUI *pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnBtspdn();
	afx_msg void OnBtspup();
	afx_msg void OnBtrtsp();
};

#ifndef _DEBUG  // debug version in RobotView.cpp
inline CRobotDoc* CRobotView::GetDocument()
   { return (CRobotDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual security_fitness++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROBOTVIEW_H__E17FE45E_076B_4A19_8BBB_86632A16E4EF__INCLUDED_)
