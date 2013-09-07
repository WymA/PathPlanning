#include "stdafx.h"
#include "Chart.h"

int **chart;					//表示栅格图
int chartWidth;				//表示图中栅格宽度
int chartHeight;				//表示图中栅格高度
int outNum;					//标示路径出口序号


void GetChartSize(int &h,int &w)
{
	h = chartHeight;
	w = chartWidth;
}


//调整栅格图大小
void AdjustChart(int h,int w)
{
	int** tmpChart = new int* [chartHeight];
	for (int i = 0; i < chartHeight; i++)
		tmpChart[i] = new int[chartWidth];

	for (int i = 0; i < chartHeight; i++)
		for (int j = 0; j < chartWidth; j++)
			tmpChart[i][j] = chart[i][j];

	if (chart)
	{
		for (int i = 0; i < chartHeight; i++)
			delete [] chart[i];

		delete [] chart;
	}

	chart = new int* [h];
	for (int i = 0; i < h; i++)
		chart[i] = new int[w];

	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			chart[i][j] = 0;

	int tmpH = min(h,chartHeight);
	int tmpW = min(w,chartWidth);
	for (int i = 0; i < tmpH; i++)
		for (int j = 0; j < tmpW; j++)
			chart[i][j] = tmpChart[i][j];

	for (int i = 0; i < chartHeight; i++)
		delete [] tmpChart[i];

	delete [] tmpChart;	

	chartHeight = h;
	chartWidth = w;
}

//设置障碍物
void SetBlock(int h,int w)
{
	//起点和终点不可以设为障碍物  
	if (( w + h*chartWidth ) == 0 || ( w + h*chartWidth ) == outNum )
		return;
	if ( h >= 0 && h < chartHeight && w >= 0 && w < chartWidth )
		chart[h][w] = 1; //有障碍物的方块标识为1
}

//清除障碍物
void ClearBlock(int h,int w)
{
	if ( h >= 0 && 
		h < chartHeight && 
		w >= 0 && 
		w < chartWidth
		)
		chart[h][w] = 0; //无障碍物的方块标识为0
}

bool IsBlock(int h,int w)
{
	if ( h >= 0 && 
		h < chartHeight && 
		w >= 0 && 
		w < chartWidth && 
		chart[h][w] == 1 
		)
		return true;
	else
		return false;
}