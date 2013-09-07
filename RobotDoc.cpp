// RobotDoc.cpp : implementation of the CRobotDoc class
//

#include "stdafx.h"
#include "Robot.h"

#include "RobotDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRobotDoc

IMPLEMENT_DYNCREATE(CRobotDoc, CDocument)

BEGIN_MESSAGE_MAP(CRobotDoc, CDocument)
	//{{AFX_MSG_MAP(CRobotDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRobotDoc construction/destruction

CRobotDoc::CRobotDoc()
{
	// TODO: add one-time construction code here
	m_CurPara.height = 10;
	m_CurPara.width = 10;
	m_CurPara.propC = 0.9;//0.6;
	m_CurPara.propM = 0.05;//0.01;
	m_CurPara.pSize = 40;//50;
 	m_CurPara.T = 500;//50;
	m_CurPara.length = 1;
	m_CurPara.smooth = 1;
	m_CurPara.safe = 0;
	//GARoad = new MyGA(m_CurPara);	
	GARoad = new MyGA(m_CurPara);
}

CRobotDoc::~CRobotDoc()
{
	delete GARoad;
}

BOOL CRobotDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	//GARoad->releaseData();
	GARoad->Init(m_CurPara);

	SetInitialBlock();

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CRobotDoc serialization

void CRobotDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRobotDoc diagnostics

#ifdef _DEBUG
void CRobotDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRobotDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRobotDoc commands
void CRobotDoc::SetInitialBlock()
{
	SetBlock(2,2); 
	SetBlock(2,3); 
	SetBlock(2,4);
	SetBlock(2,5);
	SetBlock(2,6);

	SetBlock(4,4); 
	SetBlock(4,5); 
	SetBlock(2,7); 
	SetBlock(2,8); 
	SetBlock(6,3);//¼Ó
	SetBlock(6,4);//¼Ó
	SetBlock(6,5);//¼Ó
	SetBlock(6,6); 
	SetBlock(6,7); 
	SetBlock(7,2); 
	SetBlock(7,3); 
 	SetBlock(9,6); 
	SetBlock(9,7);
}