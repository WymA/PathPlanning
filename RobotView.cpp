// RobotView.cpp : implementation of the CRobotView class
//

#include "stdafx.h"
#include "Robot.h"
#include "RobotDoc.h"
#include "RobotView.h"
#include "MainFrm.h"
#include "MOEAD.h"
#include "SetPara.h"
#include "NSGA2.h"
#include "string.h"
#include <sstream>
#include <queue>
#include "CAEA.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRobotView

IMPLEMENT_DYNCREATE(CRobotView, CView)

BEGIN_MESSAGE_MAP(CRobotView, CView)
	ON_WM_SIZE()
	ON_COMMAND(ID_BTSTART, OnBtstart)
	ON_COMMAND(ID_BTPARA, OnBtpara)
	ON_COMMAND(ID_MENUPARA, OnMenupara)
	ON_COMMAND(ID_MENUSTART, OnMenustart)
	ON_WM_LBUTTONDOWN()
	ON_WM_CANCELMODE()
	ON_WM_CREATE()
	ON_COMMAND(ID_BTPAUSE, OnBtpause)
	ON_UPDATE_COMMAND_UI(ID_NSGA2, &CRobotView::OnUpdateNsga2)
ON_UPDATE_COMMAND_UI(ID_MOEAD, &CRobotView::OnUpdateMoead)
ON_UPDATE_COMMAND_UI(ID_CAEA, &CRobotView::OnUpdateCaea)
ON_COMMAND(ID_NSGA2, &CRobotView::OnNsga2)
ON_COMMAND(ID_MOEAD, &CRobotView::OnMoead)
ON_COMMAND(ID_CAEA, &CRobotView::OnCaea)
ON_COMMAND(ID_BTSTOP, &CRobotView::OnBtstop)
ON_COMMAND(ID_OPTONLY, &CRobotView::OnOptonly)
ON_UPDATE_COMMAND_UI(ID_OPTONLY, &CRobotView::OnUpdateOptonly)
ON_UPDATE_COMMAND_UI(ID_BTSTART, &CRobotView::OnUpdateBtstart)
//ON_UPDATE_COMMAND_UI(ID_BTSTOP, &CRobotView::OnUpdateBtstop)
ON_UPDATE_COMMAND_UI(ID_BTPAUSE, &CRobotView::OnUpdateBtpause)
ON_WM_KEYDOWN()
ON_COMMAND(ID_BTSPDN, &CRobotView::OnBtspdn)
ON_COMMAND(ID_BTSPUP, &CRobotView::OnBtspup)
ON_COMMAND(ID_BTRTSP, &CRobotView::OnBtrtsp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRobotView construction/destruction

CRobotView::CRobotView()
{
	// TODO: add construction code here
	m_origX = m_origY = 20; //画图的起点
	m_bEraseBlock = m_bAddBlock = m_bSetPara = false;
	m_bNeedInit = true;
	m_CurOpID = ID_APP_ABOUT;
	m_preState = &m_bEraseBlock;
	m_pThread = NULL;
	m_test_method = kNsga2Idx ;

	//InitMethod() ;

	ff.open("test.txt", std::ios::out);
}
CRobotView::~CRobotView()
{
	//DelMethod() ;
}



BOOL CRobotView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CRobotView drawing
void CRobotView::DrawAll(CDC* pDC) 
{
	CPoint   point;
	
	CRobotDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	DrawChart(pDC,m_origX,m_origY);
	DrawBlocks(pDC,m_origX,m_origY);
	ShowPara(pDC);

	BYTE red;
	BYTE green;
	BYTE blue;
	COLORREF m_PathColor;


	if ( !optimal_only ) {
		switch ( m_test_method ){

		case kNsga2Idx:
			{
				for(int i = 0; i < pDoc->method_nsga2->popSize;i++){

					red = (20*i)%256;
					green = 0;//(50*i)%256;
					blue = (80*i)%256;
					m_PathColor = RGB(red,green,blue);
					DrawPath(pDC, &(pDoc->method_nsga2->parent_pop.ind[i].x_var), m_PathColor);
				}
			}

			break ;
		case kMoeadIdx:
			{
				for(int i=0;i < pDoc->method_moead->population_size;i++){

					red = (20*i)%256;
					green = 0;//(50*i)%256;
					blue = (80*i)%256;
					m_PathColor = RGB(red,green,blue);
					DrawPath(pDC, &( pDoc->method_moead->population[i].indiv.x_var ), m_PathColor);
				}	
			}
			break ;
		case kCaeaIdx:
			{

				for(int i=0;i< pDoc->method_caea->population_size;i++){
					red = (20*i)%256;
					green = 0;//(50*i)%256;
					blue = (80*i)%256;
					m_PathColor = RGB(red,green,blue);
					DrawPath(pDC, &( pDoc->method_caea->sector_population[i].x_var ), m_PathColor);
				}
			
			}
			break ;
		}	
	}

	DrawPath(pDC, &(path_security), kColorSecurityRed );//NULL  method_nsga2->curBestGenome->genVec
	DrawPath(pDC, &(path_smoothness), kColorSmoothnessBlue );
	DrawPath(pDC, &(path_length), kColorLengthGreen );

}



void CRobotView::OnDraw(CDC* pDC)
{
	DrawAll(pDC) ;
}

/////////////////////////////////////////////////////////////////////////////
// CRobotView diagnostics

#ifdef _DEBUG
void CRobotView::AssertValid() const
{
	CView::AssertValid();
}

void CRobotView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRobotDoc* CRobotView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRobotDoc)));
	return (CRobotDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRobotView message handlers
void CRobotView::DrawChart(CDC* pDC,int originX,int originY)
{
	//CRobotDoc* pDoc = GetDocument();
	int chart_width = cur_parameter.width;
	int chart_height = cur_parameter.height;
	gridWidth = min(m_cxWnd/chart_width,(int)(m_cyWnd*7/8/chart_height));
	
	CPen m_ChartPen1(PS_SOLID,1,RGB(100,100,100));//设置划线的颜色
	CPen* m_OldPen = pDC->SelectObject (&m_ChartPen1);

	//先画横线
	for (int i = 0; i <= chart_height; i++){

		int x0 = originX;
		int x1 = x0 + gridWidth*chart_width ;
		int y = originY + gridWidth*i;
		pDC->MoveTo(x0,y);
		pDC->LineTo(x1,y);	
	}

	DrawPareto(pDC, originX, originY ) ;
	//////////////////////////////////////////////////	
	
	COLORREF oldColor = pDC->SetTextColor(RGB(100,100,100));//设置文本字体的颜色
	//画竖线
	CPen m_ChartPen(PS_SOLID,1,RGB(100,100,100));//设置划线的颜色
	m_OldPen = pDC->SelectObject (&m_ChartPen);
	for (int i = 0; i <= chart_width; i++){

		int y0 = originY;
		int y1 = y0 + gridWidth*chart_height;
		int x = originX + gridWidth*i;
		pDC->MoveTo(x,y0);
		pDC->LineTo(x,y1);
	}

	//显示起点和终点
	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = gridWidth/2;
	strcpy(lf.lfFaceName, "Arial");
	VERIFY(font.CreateFontIndirect(&lf));
	CFont* oldFont = pDC->SelectObject(&font);
	
	int startX = originX+(gridWidth/12);
	int startY = originY+gridWidth/3;
	pDC->TextOut(startX,startY,"起始");
	int endX = originX+(chart_width-1)*gridWidth+gridWidth/12;
	int endY = originY+(chart_height-1)*gridWidth+gridWidth/3;
	pDC->TextOut(endX,endY,"目标");

	pDC->SelectObject(m_OldPen);
	pDC->SelectObject(oldFont);
}

void CRobotView::DrawPareto(CDC* pDC,int originX,int originY) 
{

	COLORREF oldColor = pDC->SetTextColor(RGB(0,0,255));
	pDC->TextOut(chart_width*gridWidth+kChartLength-5, originY+chart_height*gridWidth, "O");
	pDC->TextOut(chart_width*gridWidth+kChartLength+15, originY+chart_height*gridWidth, "Utopian point ");
	pDC->MoveTo(chart_width*gridWidth+kChartLength,originY);//画y轴
	pDC->LineTo(chart_width*gridWidth+kChartLength,originY+chart_height*gridWidth);
	pDC->MoveTo(chart_width*gridWidth+kChartLength,originY+15);
	pDC->LineTo(chart_width*gridWidth+245,originY+25);
	pDC->MoveTo(chart_width*gridWidth+kChartLength,originY+15);
	pDC->LineTo(chart_width*gridWidth+255,originY+25);
	pDC->MoveTo(chart_width*gridWidth+kChartLength,originY+chart_height*gridWidth);//画X轴
	pDC->LineTo(chart_width*gridWidth+kChartLength+chart_height*gridWidth,originY+chart_height*gridWidth);
	pDC->MoveTo(chart_width*gridWidth+kChartLength+chart_height*gridWidth,originY+chart_height*gridWidth);
	pDC->LineTo(chart_width*gridWidth+kChartLength+chart_height*gridWidth-10,originY+chart_height*gridWidth+5);
	pDC->MoveTo(chart_width*gridWidth+kChartLength+chart_height*gridWidth,originY+chart_height*gridWidth);pDC->LineTo(chart_width*gridWidth+kChartLength+chart_height*gridWidth-10,originY+chart_height*gridWidth-5);



	if ( cur_parameter.length ){

		pDC->TextOut(
			chart_width*gridWidth+kChartLength,
			originY,
			"长度适应度");

		if ( cur_parameter.smooth  ){
			
			pDC->TextOut(
				chart_width*gridWidth+155+chart_height*gridWidth,
				originY+chart_height*gridWidth+10,
				"平滑性适应度");
		}else{

			pDC->TextOut(
				chart_width*gridWidth+155+chart_height*gridWidth,
				originY+chart_height*gridWidth+10,
				"安全性适应度");
		}
	}else{

		pDC->TextOut(
			chart_width*gridWidth+kChartLength,
			originY,
			"平滑性适应度"
			);
		pDC->TextOut(
			chart_width*gridWidth+155+chart_height*gridWidth,
			originY+chart_height*gridWidth+10,
			"安全性适应度");

	}

	//if ( run_state != kStart ) return ;


	//for ( vector<vector<double>>::iterator idx = dominated_fitness.begin() ; 
	//		idx != dominated_fitness.end() ;
	//		idx++) 
	//{
	//	if ((*idx)[obj1] >= 1 ||(*idx)[obj1] <= 0 ||
	//		(*idx)[obj2] >= 1 ||(*idx)[obj2] <= 0 )
	//			continue ;

	//	pDC->TextOut(
	//		chart_width*gridWidth+kChartLength + (*idx)[obj1]*kChartLength,
	//		originY+chart_height*gridWidth - (*idx)[obj2]*kChartLength, kParetoDominated ) ;//画帕累托图
	//}




	for ( vector<vector<double>>::iterator idx = dominating_fitness.begin() ; 
			idx != dominating_fitness.end() ;
			idx++) {

		if ( (*idx)[obj1] > 1 || (*idx)[obj1] < 0 ||
			(*idx)[obj2] > 1 ||(*idx)[obj2] < 0 )
			continue ;

		DrawPoint( pDC, 
			chart_width*gridWidth+ kChartLength + 
			(*idx)[obj2]*kChartLength,
			originY+chart_height*gridWidth - 
			(*idx)[obj1]*kChartLength) ;
	}
}

void CRobotView::DrawPoint( CDC* pDC,int originX,int originY )
{
	pDC->SetPixel( originX, originY, RGB(0, 0, 0)) ;
	pDC->SetPixel( originX+1, originY-1, RGB(0, 0,0)) ;
	pDC->SetPixel( originX-1 , originY-1, RGB(0, 0, 0)) ;
	pDC->SetPixel( originX -1, originY+1, RGB(0, 0, 0)) ;
	pDC->SetPixel( originX +1, originY +1, RGB(0, 0, 0)) ;
	pDC->SetPixel( originX, originY+1, RGB(0, 0, 0)) ;
	pDC->SetPixel( originX+1, originY, RGB(0, 0, 0)) ;
	pDC->SetPixel( originX, originY-1, RGB(0, 0, 0)) ;
	pDC->SetPixel( originX-1, originY, RGB(0, 0, 0)) ;
	pDC->SetPixel( originX+2, originY-2, RGB(0, 0,0)) ;
	pDC->SetPixel( originX-2 , originY-2, RGB(0, 0, 0)) ;
	pDC->SetPixel( originX-2, originY+2, RGB(0, 0, 0)) ;
	pDC->SetPixel( originX+2, originY+2, RGB(0, 0, 0)) ;
}

void CRobotView::OnSize(UINT nType, int cx, int cy) //调整图格大小以适合不同窗口大小
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	m_cxWnd = cx;
	m_cyWnd = cy;
}

void CRobotView::DrawBlocks(CDC* pDC,int originX,int originY)
{
	//CRobotDoc* pDoc = GetDocument();
	int width = cur_parameter.width;
	int height = cur_parameter.height;
	int upleftX,upleftY,lowrightX,lowrightY;
	CRect blockRect;
	CBrush m_BlockBrush;
	m_BlockBrush.CreateSolidBrush(RGB(0,0,0));
	CBrush* oldBrush = pDC->SelectObject(&m_BlockBrush);

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			if ( is_block(i,j) )
			{
				upleftX = originX+j*gridWidth;
				upleftY = originY+i*gridWidth;
				lowrightX = upleftX+gridWidth+1;
				lowrightY = upleftY+gridWidth+1;
				blockRect.SetRect(upleftX,upleftY,lowrightX,lowrightY);
				pDC->Rectangle(blockRect);
			}
		}

	pDC->SelectObject(oldBrush);
}

void CRobotView::DrawPath(CDC* pDC,vector<int>* path,COLORREF m_PathColor)
{
	if (path){			//如果路径不为空则画路径
	
		//CRobotDoc* pDoc = GetDocument();
		int chartH,chartW;
		get_chart_size(chartH,chartW);
		
		CPen m_PathPen(PS_DASH,2,m_PathColor);
		CPen* m_OldPen = pDC->SelectObject(&m_PathPen);

		if (path->size()!=0 && chartH!=0 && chartW!=0)	{

			int len = path->size();
			int curY,curX;
			curY = (*path)[0]/chartW;
			curX = (*path)[0]%chartW;
			curY = m_origY + curY*gridWidth + (gridWidth+1)/2;
			curX = m_origX + curX*gridWidth + (gridWidth+1)/2;
			pDC->MoveTo(curX,curY);

			for (int i = 1; i < len; i++)	{

				curY = (*path)[i]/chartW;
				curX = (*path)[i]%chartW;
				curY = m_origY + curY*gridWidth + (gridWidth+1)/2;
				curX = m_origX + curX*gridWidth + (gridWidth+1)/2;
				pDC->LineTo(curX,curY);
				pDC->MoveTo(curX,curY);
			}
		}
		pDC->SelectObject(m_OldPen);
	}
}

void CRobotView::OnBtstart() 
{
	//TODO: Add your command handler code here
	/*((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,FALSE);
	*m_preState = false;

	m_CurOpID = ID_BTSTART;
	((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,TRUE);*/

	CRobotDoc* pDoc = GetDocument();

	if ( run_state == kStop ){//##Start a new thread

		pDoc->DelMethod() ;
		pDoc->InitMethod();

		m_pThread = AfxBeginThread(ThreadFunc,this);
	}	
	if ( run_state == kPause ) {

		m_pThread->ResumeThread() ;
		run_state = kStart ;
	}
}

void CRobotView::OnBtpause() 
{
	//TODO: Add your command handler code here
	/*((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,FALSE);
	*m_preState = false;

	m_CurOpID = ID_BTPAUSE;
	((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,TRUE);*/
	
	if(m_pThread && run_state == kStart ){

		m_pThread->SuspendThread();
	
		run_state = kPause ;

	}	
}

void CRobotView::OnBtstop()
{
	// TODO: 在此添加命令处理程序代码
	//((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,FALSE);
	//*m_preState = false;

	run_state = kStop ;
}

void CRobotView::OnBtpara() //调整参数
{
	// TODO: Add your command handler code here
	((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,FALSE);
	*m_preState = false;
	m_bSetPara = !m_bSetPara;
	m_preState = &m_bSetPara;
	m_CurOpID = ID_BTPARA;

	SetPara m_ChangePara;
	if (m_ChangePara.DoModal() == IDOK)
	{

		cur_parameter.height = m_ChangePara.m_Height;
		cur_parameter.width = m_ChangePara.m_Width;
		cur_parameter.pop_cross_rate = m_ChangePara.m_Cross;
		cur_parameter.pop_mutation_rate = m_ChangePara.m_Mutation;
		cur_parameter.pSize = m_ChangePara.m_PopSize;
		cur_parameter.total_gen = m_ChangePara.m_GenNum;
		cur_parameter.length = m_ChangePara.m_Length;
		cur_parameter.smooth = m_ChangePara.m_Smooth;
		cur_parameter.security = m_ChangePara.m_Safe;

		//init_globe() ;
		init_chart() ;
		//pDoc->InitMethod() ;

		Invalidate();
		UpdateWindow();
	}
}

void CRobotView::OnBtresult()
{
	((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,FALSE);
	*m_preState = false;
	m_CurOpID = ID_BTPARA;
	((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,TRUE);
	Invalidate();
	UpdateWindow();
}

void CRobotView::OnMenupara() 
{
	// TODO: Add your command handler code here
	OnBtpara();	
}

void CRobotView::OnMenustart() 
{
	// TODO: Add your command handler code here
	OnBtstart();
}

void CRobotView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CView::OnLButtonDown(nFlags, point);

	if ( run_state == kStart ) 
		return ;
	

	int curBlockULX = 0 ;
	int curBlockULY = 0 ;

	if ( point.x >= cur_parameter.width*gridWidth+m_origX || 
		point.y >= cur_parameter.width*gridWidth+m_origY ){

			return ;
	}else {
		curBlockULX = (point.x-m_origX)/gridWidth;
		curBlockULY = (point.y-m_origY)/gridWidth;
	}

	//CRobotDoc* pDoc = GetDocument();

	if ( is_block(curBlockULY, curBlockULX ) ){

		clear_block(curBlockULY,curBlockULX);
		Invalidate();
		UpdateWindow();
	}else{

		set_block( curBlockULY*chart_width+curBlockULX );
		Invalidate();
		UpdateWindow();
	}

}


void CRobotView::ShowPara(CDC* pDC)
{
	CRobotDoc* pDoc = GetDocument();
	int curBound = m_origX+cur_parameter.width*gridWidth+1;
	int startX = curBound+20;
	int startY = 20;
	int num_generation = 0 ;

	switch( m_test_method ) {

	case kNsga2Idx:
		{
			num_generation = pDoc->method_nsga2->curGenNum ;
		}
		break ;
	case kMoeadIdx:
		{
			num_generation = pDoc->method_moead->cur_generation ;
		}
		break ;
	case kCaeaIdx:
		{
			num_generation = pDoc->method_caea->cur_gen ;
		}
		break ;
	}

	//Fitness tFit ;
	//tFit.length_fitness = fit.length_fitness ;
	//tFit.smoothness_fitness = fit.smoothness_fitness ;
	//tFit.security_fitness = fit.security_fitness ;

	//if (avg.size() >= 10 ) 
	//	avg.pop() ;
	//avg.push( tFit ) ;

	double length_avg = fit.length_fitness ;
	double smooth_avg = fit.smoothness_fitness ;
	double security_avg = fit.security_fitness ;
	//
	//queue<Fitness> tmp ;
	//for ( int i = 0 ; i < avg.size() ; i++ ) {

	//	length_avg += avg.front().length_fitness ;
	//	smooth_avg += avg.front().smoothness_fitness ;
	//	security_avg += avg.front().security_fitness ;

	//	tmp.push(avg.front()) ;
	//	avg.pop() ;
	//}

	//for ( int i = 0 ; i < tmp.size() ; i++ ){

	//	avg.push(tmp.front()) ;
	//	tmp.pop() ;
	//}
	//
	//length_avg /= avg.size() ;
	//smooth_avg /= avg.size() ;
	//security_avg /= avg.size() ;


	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 16;
	strcpy(lf.lfFaceName, "Arial");
	VERIFY(font.CreateFontIndirect(&lf));
	CFont* oldFont = pDC->SelectObject(&font);
	COLORREF oldColor = pDC->SetTextColor(RGB(100,100,100));
	pDC->TextOut(startX,startY,"当前种群规模:");
	pDC->TextOut(startX,startY+30,"当前进化代数:");
	pDC->TextOut(startX,startY+60,"当前长度值:");
	pDC->TextOut(startX,startY+90,"当前平滑度值:");
	pDC->TextOut(startX,startY+120,"当前安全性值:");
	pDC->TextOut(startX,startY+170,"长度:");
	pDC->TextOut(startX,startY+200,"平滑度:");
	pDC->TextOut(startX,startY+230,"安全性:");
	pDC->SelectObject(oldFont);
	
	oldColor = pDC->SetTextColor(RGB(255,0,255));
	
	CString cur_generation;
	cur_generation.Format(_T("%d"),cur_parameter.pSize);
	pDC->TextOut(startX+120,startY,cur_generation);
	cur_generation.Format(_T("%d"), num_generation );
	pDC->TextOut(startX+120,startY+30,cur_generation);
	cur_generation.Format(_T("%.3f"),length_avg);
	pDC->TextOut(startX+120,startY+60,cur_generation);
	cur_generation.Format(_T("%.3f"),smooth_avg);
	pDC->TextOut(startX+120,startY+90,cur_generation);
	cur_generation.Format(_T("%.3f"),security_avg);
	pDC->TextOut(startX+120,startY+120,cur_generation);


	CPen m_ChartPen1(PS_SOLID,1,kColorLengthGreen);//设置划线的颜色
	pDC->SelectObject (&m_ChartPen1);
	pDC->MoveTo(startX+120,startY+170);
	pDC->LineTo(startX+170,startY+170);
	CPen m_ChartPen2(PS_SOLID,1,kColorSmoothnessBlue);//设置划线的颜色
	pDC->SelectObject (&m_ChartPen2);
	pDC->MoveTo(startX+120,startY+200);
	pDC->LineTo(startX+170,startY+200);
	CPen m_ChartPen3(PS_SOLID,1,kColorSecurityRed);//设置划线的颜色
	pDC->SelectObject (&m_ChartPen3);
	pDC->MoveTo(startX+120,startY+230);
	pDC->LineTo(startX+170,startY+230);

	pDC->SetTextColor(oldColor);
}



void CRobotView::OnUpdateNsga2(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	if ( m_test_method == kNsga2Idx ) {
		pCmdUI->SetCheck(true) ;
	}else{
		pCmdUI->SetCheck(false) ;
	}
}


void CRobotView::OnUpdateMoead(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	if ( m_test_method == kMoeadIdx ) {
		pCmdUI->SetCheck(true) ;
	}else{
		pCmdUI->SetCheck(false) ;
	}
}


void CRobotView::OnUpdateCaea(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	if ( m_test_method == kCaeaIdx ) {
		pCmdUI->SetCheck(true) ;
	}else{
		pCmdUI->SetCheck(false) ;
	}
}


void CRobotView::OnNsga2()
{
	// TODO: 在此添加命令处理程序代码
	if ( run_state == kStop )
		m_test_method = kNsga2Idx ;
}


void CRobotView::OnMoead()
{
	// TODO: 在此添加命令处理程序代码
	if ( run_state == kStop )
		m_test_method = kMoeadIdx ;
}


void CRobotView::OnCaea()
{
	// TODO: 在此添加命令处理程序代码
	if ( run_state == kStop )
		m_test_method = kCaeaIdx ;
}

UINT ThreadFunc(LPVOID pParam)
{
	CRobotView* m_CurView = (CRobotView*)pParam;
	CRobotDoc* pDoc = m_CurView->GetDocument();

	run_state = kStart ;

	switch ( m_CurView->m_test_method ) { 

	case kNsga2Idx :
		{
			pDoc->method_nsga2->Execute(m_CurView);
		}
		break;
	case kMoeadIdx:
		{
			pDoc->method_moead->Execute(m_CurView) ;
		}
		break ;
	case kCaeaIdx :
		{
			pDoc->method_caea->Execute(m_CurView) ;
		}
		break ;
	}

	run_state = kStop ;


	return 0;
}


void CRobotView::OnOptonly()
{
	optimal_only = !optimal_only ;
	Invalidate();
	UpdateWindow();
}


void CRobotView::OnUpdateOptonly(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	pCmdUI->SetCheck(optimal_only) ;
}


void CRobotView::OnUpdateBtstart(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(run_state==kStart) ;
}


void CRobotView::OnUpdateBtpause(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(run_state==kPause) ;
}


void CRobotView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch( nChar){

	case VK_SPACE :
		{
			if ( run_state == kStart ) OnBtpause() ;
			else OnBtstart();
		}
		break ;
	case VK_ESCAPE :
		{
			if ( run_state != kStop ) OnBtstop() ;
		}
		break ;
	case VK_SHIFT :
		{
			if ( run_state == kStart ) OnOptonly() ;
		}
		break ;
	}


	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CRobotView::OnBtspdn()
{
	// TODO: 在此添加命令处理程序代码
	SpeedDown() ;
}


void CRobotView::OnBtspup()
{
	// TODO: 在此添加命令处理程序代码
	SpeedUp() ;
}


void CRobotView::OnBtrtsp()
{
	run_speed = 100 ;
}
