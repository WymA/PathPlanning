#pragma once

extern int **chart;					//表示栅格图
extern int chart_width;				//表示图中栅格宽度
extern int chart_height;				//表示图中栅格高度
extern int out_num;					//标示路径出口序号

void SetBlock(int h,int w);					  //设置障碍物
void ClearBlock(int h,int w);				  //清除障碍物
bool IsBlock(int h,int w);					  //当前点是否为障碍物
void GetChartSize(int &h,int &w) ;
void AdjustChart(int h,int w);			  //当参数改变时，需要调整栅格图大小
void InitChart() ;
void DeleteChart() ;
void DefaultInitChart() ;