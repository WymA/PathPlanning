// RobotView.cpp : implementation of the CRobotView class
//

#include "stdafx.h"
#include "Robot.h"

#include "RobotDoc.h"
#include "RobotView.h"
#include "MainFrm.h"
#include "MOEAD.h"
#include "SetPara.h"
#include "MyGA.h"
#include "string.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CRobotView

IMPLEMENT_DYNCREATE(CRobotView, CView)

BEGIN_MESSAGE_MAP(CRobotView, CView)
	//{{AFX_MSG_MAP(CRobotView)
	ON_WM_SIZE()
	ON_COMMAND(ID_BTSTART, OnBtstart)
	ON_COMMAND(ID_BTERASE, OnBterase)
	ON_COMMAND(ID_BTADD, OnBtadd)
	ON_COMMAND(ID_BTPARA, OnBtpara)
	ON_COMMAND(ID_MENUADD, OnMenuadd)
	ON_COMMAND(ID_MENUERASE, OnMenuerase)
	ON_COMMAND(ID_MENUPARA, OnMenupara)
	ON_COMMAND(ID_MENUSTART, OnMenustart)
	ON_WM_LBUTTONDOWN()
	ON_WM_CANCELMODE()
	ON_WM_CREATE()
	ON_COMMAND(ID_BTPAUSE, OnBtpause)
	//}}AFX_MSG_MAP
//	ON_COMMAND(ID_NSGA2, &CRobotView::OnNsga2)
//	ON_COMMAND(ID_MOEAD, &CRobotView::OnMoead)
//	ON_COMMAND(ID_CAEA, &CRobotView::OnCaea)
//	ON_UPDATE_COMMAND_UI(ID_NSGA2, &CRobotView::OnUpdateNsga2)
ON_UPDATE_COMMAND_UI(ID_NSGA2, &CRobotView::OnUpdateNsga2)
ON_UPDATE_COMMAND_UI(ID_MOEAD, &CRobotView::OnUpdateMoead)
ON_UPDATE_COMMAND_UI(ID_CAEA, &CRobotView::OnUpdateCaea)
ON_COMMAND(ID_NSGA2, &CRobotView::OnNsga2)
ON_COMMAND(ID_MOEAD, &CRobotView::OnMoead)
ON_COMMAND(ID_CAEA, &CRobotView::OnCaea)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRobotView construction/destruction
extern struct Fitness fit[1000];
CRobotView::CRobotView()
{
	// TODO: add construction code here
	m_origX = m_origY = 20; //画图的起点
	m_bEraseBlock = m_bAddBlock = m_bSetPara = false;
	m_bNeedInit = true;
	m_CurOpID = ID_APP_ABOUT;
	m_preState = &m_bEraseBlock;
	m_pThread = NULL;
	m_test_method = NSGA2 ;
}
CRobotView::~CRobotView()
{
}

BOOL CRobotView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CRobotView drawing

void CRobotView::OnDraw(CDC* pDC)
{
	CPoint   point;
	
	CRobotDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	
	DrawChart(pDC,m_origX,m_origY);
	DrawBlocks(pDC,m_origX,m_origY);
	ShowPara(pDC);

	BYTE red;
	BYTE green;
	BYTE blue;
	COLORREF m_PathColor;

	switch ( m_test_method ){

	case NSGA2:
		{

			if(pDoc->GARoad->m_bAlreadyRun){

				for(int i=0;i<pDoc->GARoad->popSize;i++){

					red = (20*i)%256;
					green = 0;//(50*i)%256;
					blue = (80*i)%256;
					m_PathColor = RGB(red,green,blue);
					DrawPath(pDC, &(pDoc->GARoad->parent_pop.ind[i].xPath), m_PathColor);
				}
			}

			if(pDoc->GARoad->m_bAlreadyStarted){

				red = 255;
				green = 0;
				blue = 0;
				m_PathColor = RGB(red,green,blue);
				DrawPath(pDC, &(path_security), m_PathColor);//NULL  pDoc->GARoad->curBestGenome->genVec

				red = 0;
				green = 0;
				blue = 255;
				m_PathColor = RGB(red,green,blue);
				DrawPath(pDC, &(path_smoothness), m_PathColor);

				red = 0;
				green = 255;
				blue = 0;
				m_PathColor = RGB(red,green,blue);
				DrawPath(pDC, &(path_length), m_PathColor);
			}
		}

		break ;
	case MOEAD:
		{
			if( pDoc->MOEAD->m_is_run ){

				for(int i=0;i<pDoc->MOEAD->population_size;i++){

					red = (20*i)%256;
					green = 0;//(50*i)%256;
					blue = (80*i)%256;
					m_PathColor = RGB(red,green,blue);
					DrawPath(pDC, &(pDoc->MOEAD->population[i].indiv.x_var ), m_PathColor);
				}

				red = 255;
				green = 0;
				blue = 0;
				m_PathColor = RGB(red,green,blue);
				DrawPath(pDC, &(path_security), m_PathColor);//NULL  pDoc->GARoad->curBestGenome->genVec

				red = 0;
				green = 0;
				blue = 255;
				m_PathColor = RGB(red,green,blue);
				DrawPath(pDC, &(path_smoothness), m_PathColor);

				red = 0;
				green = 255;
				blue = 0;
				m_PathColor = RGB(red,green,blue);
				DrawPath(pDC, &(path_length), m_PathColor);
			}
		}
		break ;
	case CAEA:
		{
		}
		break ;
	}

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
	CRobotDoc* pDoc = GetDocument();
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

	if(cur_parameter.smooth)
	{
		COLORREF oldColor = pDC->SetTextColor(RGB(0,0,255));
		pDC->MoveTo(chart_width*gridWidth+250,originY);//画y轴
		pDC->LineTo(chart_width*gridWidth+250,originY+chart_height*gridWidth);
		pDC->MoveTo(chart_width*gridWidth+250,originY+15);
		pDC->LineTo(chart_width*gridWidth+245,originY+25);
		pDC->MoveTo(chart_width*gridWidth+250,originY+15);
		pDC->LineTo(chart_width*gridWidth+255,originY+25);
		pDC->TextOut(chart_width*gridWidth+250,originY,"长度适应度（优）");
		pDC->MoveTo(chart_width*gridWidth+250,originY+chart_height*gridWidth);//画X轴
		pDC->LineTo(chart_width*gridWidth+250+chart_height*gridWidth,originY+chart_height*gridWidth);
		pDC->MoveTo(chart_width*gridWidth+250+chart_height*gridWidth,originY+chart_height*gridWidth);
		pDC->LineTo(chart_width*gridWidth+250+chart_height*gridWidth-10,originY+chart_height*gridWidth+5);
		pDC->MoveTo(chart_width*gridWidth+250+chart_height*gridWidth,originY+chart_height*gridWidth);
		pDC->LineTo(chart_width*gridWidth+250+chart_height*gridWidth-10,originY+chart_height*gridWidth-5);
		pDC->TextOut(chart_width*gridWidth+155+chart_height*gridWidth,originY+chart_height*gridWidth+10,"平滑度适应度（优）");
		for(int i=0; i<cur_parameter.pSize;i++)
			pDC->TextOut(chart_width*gridWidth+250+fit[i].B*250,originY+chart_height*gridWidth-fit[i].A*250,"*");//画帕累托图
	}
	else if(cur_parameter.security)
	{
		COLORREF oldColor = pDC->SetTextColor(RGB(0,0,255));
		pDC->MoveTo(chart_width*gridWidth+250,originY);//画y轴
		pDC->LineTo(chart_width*gridWidth+250,originY+chart_height*gridWidth);
		pDC->MoveTo(chart_width*gridWidth+250,originY+15);
		pDC->LineTo(chart_width*gridWidth+245,originY+25);
		pDC->MoveTo(chart_width*gridWidth+250,originY+15);
		pDC->LineTo(chart_width*gridWidth+255,originY+25);
		pDC->TextOut(chart_width*gridWidth+250,originY,"长度适应度（优）");
		pDC->MoveTo(chart_width*gridWidth+250,originY+chart_height*gridWidth);//画X轴
		pDC->LineTo(chart_width*gridWidth+250+chart_height*gridWidth,originY+chart_height*gridWidth);
		pDC->MoveTo(chart_width*gridWidth+250+chart_height*gridWidth,originY+chart_height*gridWidth);
		pDC->LineTo(chart_width*gridWidth+250+chart_height*gridWidth-10,originY+chart_height*gridWidth+5);
		pDC->MoveTo(chart_width*gridWidth+250+chart_height*gridWidth,originY+chart_height*gridWidth);pDC->LineTo(chart_width*gridWidth+250+chart_height*gridWidth-10,originY+chart_height*gridWidth-5);
		pDC->TextOut(chart_width*gridWidth+155+chart_height*gridWidth,originY+chart_height*gridWidth+10,"安全性适应度（优）");
		for(int i=0; i<cur_parameter.pSize;i++)
			pDC->TextOut(chart_width*gridWidth+250+(1-fit[i].C)*250,originY+chart_height*gridWidth-fit[i].A*250,"*");//画帕累托图
	}
	
	COLORREF oldColor = pDC->SetTextColor(RGB(100,100,100));//设置文本字体的颜色
	//画竖线
	CPen m_ChartPen(PS_SOLID,1,RGB(100,100,100));//设置划线的颜色
	m_OldPen = pDC->SelectObject (&m_ChartPen);
	for (int i = 0; i <= chart_width; i++)
	{
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

void CRobotView::OnSize(UINT nType, int cx, int cy) //调整图格大小以适合不同窗口大小
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	m_cxWnd = cx;
	m_cyWnd = cy;
}

void CRobotView::DrawBlocks(CDC* pDC,int originX,int originY)
{
	CRobotDoc* pDoc = GetDocument();
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
			if ( IsBlock(i,j) )
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
	if (path)//如果路径不为空则画路径
	{
		CRobotDoc* pDoc = GetDocument();
		int chartH,chartW;
		GetChartSize(chartH,chartW);
		
		CPen m_PathPen(PS_DASH,2,m_PathColor);
		CPen* m_OldPen = pDC->SelectObject(&m_PathPen);

		if (path->size()!=0 && chartH!=0 && chartW!=0)
		{
			int len = path->size();
			int curY,curX;
			curY = (*path)[0]/chartW;
			curX = (*path)[0]%chartW;
			curY = m_origY + curY*gridWidth + (gridWidth+1)/2;
			curX = m_origX + curX*gridWidth + (gridWidth+1)/2;
			pDC->MoveTo(curX,curY);

			for (int i = 1; i < len; i++)
			{
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

UINT ThreadFunc(LPVOID pParam)
{
	CRobotView* m_CurView = (CRobotView*)pParam;
	CRobotDoc* pDoc = m_CurView->GetDocument();

	switch ( m_CurView->m_test_method ) { 

	case NSGA2 :
		{
		
			MyGA* m_Robot = pDoc->GARoad;
			if (m_Robot)
			{
				MyGA* m_Robot = pDoc->GARoad;
				m_Robot->gamain(m_CurView);
				m_Robot->Init(cur_parameter);
			}

		}
		break;
	case MOEAD:
		{
			TMOEAD *moead = pDoc->MOEAD ;
			moead->execute(m_CurView) ;
		}
		break ;
	case CAEA :
		break ;
	}

	return 0;
}

void CRobotView::OnBtstart() 
{
	// TODO: Add your command handler code here
	((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,FALSE);
	*m_preState = false;

	m_CurOpID = ID_BTSTART;
	((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,TRUE);

	//if (pDoc->GARoad->m_bAlreadyStarted)
	{
	m_pThread = AfxBeginThread(ThreadFunc,this);
	}	
}

void CRobotView::OnBtpause() 
{
	// TODO: Add your command handler code here
	((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,FALSE);
	*m_preState = false;

	m_CurOpID = ID_BTPAUSE;
	((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,TRUE);
	if(m_pThread)
	{
		m_pThread->SuspendThread();

		//暂停时对当前最优的序列进行一次插值
		//使之更加连续
		switch (m_test_method){

		case NSGA2:
			{
	/*			CRobotDoc* pDoc = GetDocument();
				MyGA* m_Robot = pDoc->GARoad;
				Invalidate();
				UpdateWindow();*/
			}
			break ;
		case MOEAD:

			break ;
		case CAEA :

			break ;
		}

	}	
}

void CRobotView::OnBterase() 
{
	// TODO: Add your command handler code here
	((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,FALSE);
	*m_preState = false;
	m_bEraseBlock = !m_bEraseBlock;
	m_preState = &m_bEraseBlock;
	m_CurOpID = ID_BTERASE;
	if (m_bEraseBlock)
		((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,TRUE);
	else
		((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,FALSE);
}

void CRobotView::OnBtadd() 
{
	// TODO: Add your command handler code here
	((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,FALSE);
	*m_preState = false;
	m_bAddBlock = !m_bAddBlock;
	m_preState = &m_bAddBlock;
	m_CurOpID = ID_BTADD;
	if (m_bAddBlock)
		((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,TRUE);
	else
		((CMainFrame*)::AfxGetMainWnd())->checkButton(m_CurOpID,FALSE);	
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
		//注意参数变了之后，需要对chart的大小进行调整
		CRobotDoc* pDoc = GetDocument();
		AdjustChart(m_ChangePara.m_Height,m_ChangePara.m_Width);

		cur_parameter.height = m_ChangePara.m_Height;
		cur_parameter.width = m_ChangePara.m_Width;
		cur_parameter.propC = m_ChangePara.m_Cross;
		cur_parameter.propM = m_ChangePara.m_Mutation;
		cur_parameter.pSize = m_ChangePara.m_PopSize;
		cur_parameter.T = m_ChangePara.m_GenNum;
		cur_parameter.length = m_ChangePara.m_Length;
		cur_parameter.smooth = m_ChangePara.m_Smooth;
		cur_parameter.security = m_ChangePara.m_Safe;
		
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
void CRobotView::OnMenuadd() 
{
	// TODO: Add your command handler code here
	OnBtadd();	
}

void CRobotView::OnMenuerase() 
{
	// TODO: Add your command handler code here
	OnBterase();
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
	int curBlockULX = (point.x-m_origX)/gridWidth;
	int curBlockULY = (point.y-m_origY)/gridWidth;

	CRobotDoc* pDoc = GetDocument();

	if (m_bEraseBlock)//如果处于清除block状态
	{
		ClearBlock(curBlockULY,curBlockULX);
		Invalidate();
		UpdateWindow();
	}
	else if (m_bAddBlock)//如果处于添加block状态
	{
		SetBlock(curBlockULY,curBlockULX);
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

	case NSGA2:
		{
			num_generation = pDoc->GARoad->curGenNum ;
		}
		break ;
	case MOEAD:
		{
			num_generation = pDoc->MOEAD->cur_generation ;
		}
		break ;
	case CAEA:
		{
		}
		break ;
	}

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
	cur_generation.Format(_T("%.3f"),fit[0].A);
	pDC->TextOut(startX+120,startY+60,cur_generation);
	cur_generation.Format(_T("%.3f"),fit[0].B);
	pDC->TextOut(startX+120,startY+90,cur_generation);
	cur_generation.Format(_T("%.3f"),fit[0].C);
	pDC->TextOut(startX+120,startY+120,cur_generation);
	CPen m_ChartPen1(PS_SOLID,1,RGB(0,0,255));//设置划线的颜色
	pDC->SelectObject (&m_ChartPen1);
	pDC->MoveTo(startX+120,startY+170);
	pDC->LineTo(startX+170,startY+170);
	CPen m_ChartPen2(PS_SOLID,1,RGB(0,255,0));//设置划线的颜色
	pDC->SelectObject (&m_ChartPen2);
	pDC->MoveTo(startX+120,startY+200);
	pDC->LineTo(startX+170,startY+200);
	CPen m_ChartPen3(PS_SOLID,1,RGB(255,0,0));//设置划线的颜色
	pDC->SelectObject (&m_ChartPen3);
	pDC->MoveTo(startX+120,startY+230);
	pDC->LineTo(startX+170,startY+230);

	pDC->SetTextColor(oldColor);
}



void CRobotView::OnUpdateNsga2(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	if ( m_test_method == NSGA2 ) {
		pCmdUI->SetCheck(true) ;
	}else{
		pCmdUI->SetCheck(false) ;
	}
}


void CRobotView::OnUpdateMoead(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	if ( m_test_method == MOEAD ) {
		pCmdUI->SetCheck(true) ;
	}else{
		pCmdUI->SetCheck(false) ;
	}
}


void CRobotView::OnUpdateCaea(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	if ( m_test_method == CAEA ) {
		pCmdUI->SetCheck(true) ;
	}else{
		pCmdUI->SetCheck(false) ;
	}
}


void CRobotView::OnNsga2()
{
	// TODO: 在此添加命令处理程序代码
	m_test_method = NSGA2 ;
}


void CRobotView::OnMoead()
{
	// TODO: 在此添加命令处理程序代码
	m_test_method = MOEAD ;
}


void CRobotView::OnCaea()
{
	// TODO: 在此添加命令处理程序代码
	m_test_method = CAEA ;
}
