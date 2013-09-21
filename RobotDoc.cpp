// RobotDoc.cpp : implementation of the CRobotDoc class
//

#include "stdafx.h"
#include "Robot.h"
#include "MOEAD.h"
#include "SNGA.h"
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
	cur_parameter.height = 10;
	cur_parameter.width = 10;
	cur_parameter.propC = 0.9;//0.6;
	cur_parameter.propM = 0.05;//0.01;
	cur_parameter.pSize = 40;//50;
 	cur_parameter.T = 500;//50;
	cur_parameter.length = TRUE ;
	cur_parameter.smooth = TRUE ;
	cur_parameter.security = FALSE ;
	//GARoad = new MyGA(cur_parameter);	

	InitChart() ; 
	DefaultInitChart() ;
	srand((unsigned)time(NULL)) ;

	GARoad = new MyGA(cur_parameter);
	MOEAD = new TMOEAD() ;
	CAEA = new CSNGA() ;
}

CRobotDoc::~CRobotDoc()
{
	delete GARoad ;
	delete MOEAD ;
	DeleteChart() ;
}

BOOL CRobotDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	//GARoad->releaseData();
	GARoad->Init(cur_parameter);

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