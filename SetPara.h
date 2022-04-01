#if !defined(AFX_SETPARA_H__A0A52B95_8BBA_4B3E_8354_815A78BD5BDE__INCLUDED_)
#define AFX_SETPARA_H__A0A52B95_8BBA_4B3E_8354_815A78BD5BDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetPara.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SetPara dialog

class SetPara : public CDialog
{
// Construction
public:
	SetPara(CWnd* pParent = NULL);   // standard constructor
	int m_Height;
	int m_Width;
	double m_Cross;
	double m_Mutation;
	int m_PopSize;
	int m_GenNum;
	int m_Length;
	int m_Smooth;
	int m_Safe;

// Dialog Data
	//{{AFX_DATA(SetPara)
	enum { IDD = IDD_DIALOGPARA };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SetPara)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SetPara)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChangeEditcross();
	afx_msg void OnChangeEditgennum();
	afx_msg void OnChangeEditheight();
	afx_msg void OnChangeEditmutation();
	afx_msg void OnChangeEditpop();
	afx_msg void OnChangeEditwidth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnStnClickedStaticwidth();
	afx_msg void OnStnClickedStaticmutation();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETPARA_H__A0A52B95_8BBA_4B3E_8354_815A78BD5BDE__INCLUDED_)
