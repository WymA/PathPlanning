#include "stdafx.h"
#include "Chart.h"

int **chart;					//表示栅格图
int chart_width;				//表示图中栅格宽度
int chart_height;				//表示图中栅格高度
int out_num;					//标示路径出口序号


void GetChartSize(int &h,int &w)
{
	h = chart_height;
	w = chart_width;
}


//调整栅格图大小
void AdjustChart(int h,int w)
{
	int** tmpChart = new int* [chart_height];
	for (int i = 0; i < chart_height; i++)
		tmpChart[i] = new int[chart_width];

	for (int i = 0; i < chart_height; i++)
		for (int j = 0; j < chart_width; j++)
			tmpChart[i][j] = chart[i][j];

	if (chart)
	{
		for (int i = 0; i < chart_height; i++)
			delete [] chart[i];

		delete [] chart;
	}

	chart = new int* [h];
	for (int i = 0; i < h; i++)
		chart[i] = new int[w];

	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			chart[i][j] = 0;

	int tmpH = min(h,chart_height);
	int tmpW = min(w,chart_width);
	for (int i = 0; i < tmpH; i++)
		for (int j = 0; j < tmpW; j++)
			chart[i][j] = tmpChart[i][j];

	for (int i = 0; i < chart_height; i++)
		delete [] tmpChart[i];

	delete [] tmpChart;	

	chart_height = h;
	chart_width = w;
}

//设置障碍物
void SetBlock(int h,int w)
{
	//起点和终点不可以设为障碍物  
	if (( w + h*chart_width ) == 0 || ( w + h*chart_width ) == out_num )
		return;
	if ( h >= 0 && h < chart_height && w >= 0 && w < chart_width )
		chart[h][w] = 1; //有障碍物的方块标识为1
}

//清除障碍物
void ClearBlock(int h,int w)
{
	if ( h >= 0 && 
		h < chart_height && 
		w >= 0 && 
		w < chart_width
		)
		chart[h][w] = 0; //无障碍物的方块标识为0
}

bool IsBlock(int h,int w)
{
	if ( h >= 0 && 
		h < chart_height && 
		w >= 0 && 
		w < chart_width && 
		chart[h][w] == 1 
		)
		return true;
	else
		return false;
}

void InitChart() 
{
	chart_height = cur_parameter.height ;
	chart_width = cur_parameter.width ;
	out_num = chart_height*chart_width - 1;

	chart = new int* [chart_height];

	for (int i = 0; i < chart_height; ++i)
		chart[i] = new int[chart_width];
	for (int i = 0; i < chart_height; ++i)
		for (int j = 0; j < chart_width; ++j)
			chart[i][j] = 0; //初始时没有障碍物
	
}

void DeleteChart()
{
	if (chart){

		for (int i = 0; i < chart_height; ++i)
			delete [] chart[i];
		delete [] chart;
	}
}

void DefaultInitChart() 
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
	SetBlock(6,3);
	SetBlock(6,4);
	SetBlock(6,5);
	SetBlock(6,6); 
	SetBlock(6,7); 
	SetBlock(7,2); 
	SetBlock(7,3); 
 	SetBlock(9,6); 
	SetBlock(9,7);
}