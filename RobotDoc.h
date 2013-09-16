// RobotDoc.h : interface of the CRobotDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROBOTDOC_H__30F8DB1C_8B63_414E_94A1_2C2459F6BCB7__INCLUDED_)
#define AFX_ROBOTDOC_H__30F8DB1C_8B63_414E_94A1_2C2459F6BCB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRobotDoc : public CDocument
{
protected: // create from serialization only
	CRobotDoc();
	DECLARE_DYNCREATE(CRobotDoc)

// Attributes
public:
	//MyGA *GARoad;
	MyGA *GARoad;
	//int generation;

	TMOEAD *MOEAD ;

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRobotDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRobotDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CRobotDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROBOTDOC_H__30F8DB1C_8B63_414E_94A1_2C2459F6BCB7__INCLUDED_)
