// RobotDoc.cpp : implementation of the CRobotDoc class
//

#include "stdafx.h"
#include "Robot.h"
#include "MOEAD.h"
#include "CAEA.h"
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
	if ( 0 != LoadParameter(cur_parameter) ){

		DefaultInitParameter() ;
	}

	//init_globe() ;
	init_chart() ; 
	InitMethod() ;
	srand((unsigned)time(NULL)) ;

	fo.open("test_myga_evl.txt", std::ios::out);

}

CRobotDoc::~CRobotDoc()
{
	SaveParameter(cur_parameter) ;

	delete_chart() ;
	DelMethod() ;
	//delete_globe() ;
	fo.close() ;
	
}

void CRobotDoc::DelMethod() 
{
	total_fitness.clear() ;
	dominated_fitness.clear() ;
	dominating_fitness.clear() ;

	delete method_nsga2 ;
	delete method_moead ;
	delete method_caea ;
}


void CRobotDoc::InitMethod() 
{
	total_fitness = vector<vector<double>>(cur_parameter.pSize , vector<double>( kObjNum, 0.0 ) );
	dominated_fitness = vector<vector<double>>(0, vector<double>( kObjNum, 0.0 ) );
	dominating_fitness = vector<vector<double>>(0, vector<double>( kObjNum, 0.0 ) );

	method_nsga2 = new NSGA2() ;
	method_moead = new MOEAD() ;
	method_caea = new CAEA() ;

	SelectObj() ;

	length_pun = (chart_width-1)/2 ;
}


BOOL CRobotDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

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