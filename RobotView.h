// RobotView.h : interface of the CRobotView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROBOTVIEW_H__E17FE45E_076B_4A19_8BBB_86632A16E4EF__INCLUDED_)
#define AFX_ROBOTVIEW_H__E17FE45E_076B_4A19_8BBB_86632A16E4EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CRobotView : public CView
{
protected: // create from serialization only
	CRobotView();
	DECLARE_DYNCREATE(CRobotView)

// Attributes
public:
	CRobotDoc* GetDocument();
	int m_cxWnd,m_cyWnd;
	int m_origX,m_origY;
	int gridWidth;
	bool m_bAddBlock;
	bool m_bEraseBlock;
	bool m_bSetPara;
	//bool m_bAlreadyStarted;
	//bool m_bAlreadyRun;
	bool m_bNeedInit;
	bool* m_preState;
	int m_CurOpID;
	CWinThread* m_pThread;

// Operations
public:
	void DrawChart(CDC* pDC,int originX,int originY);
	void DrawBlocks(CDC* pDC,int originX,int originY);
	void DrawPath(CDC* pDC,vector<int>* path, COLORREF m_PathColor);
	void ShowPara(CDC* pDC);

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

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CRobotView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBtstart();
	afx_msg void OnBterase();
	afx_msg void OnBtadd();
	afx_msg void OnBtpara();
	afx_msg void OnBtresult() ;
	afx_msg void OnMenuadd();
	afx_msg void OnMenuerase();
	afx_msg void OnMenupara();
	afx_msg void OnMenustart();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBtpause();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in RobotView.cpp
inline CRobotDoc* CRobotView::GetDocument()
   { return (CRobotDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROBOTVIEW_H__E17FE45E_076B_4A19_8BBB_86632A16E4EF__INCLUDED_)
