#include "stdafx.h"
#include "Chart.h"

int *chart;					//表示栅格图
int chart_width;				//表示图中栅格宽度
int chart_height;				//表示图中栅格高度
int out_num;					//标示路径出口序号


void get_chart_size(int &h,int &w)
{
	h = chart_height;
	w = chart_width;
}


//调整栅格图大小
void adjust_chart(int h,int w)
{
	int* tmpChart = new int[out_num+1];
	int tmp_out_num = out_num ;

	for (int i = 0; i < out_num+1 ; i++)
		tmpChart[i] = chart[i];

	if (chart){

		delete [] chart;
	}

	cur_parameter.height = h ;
	cur_parameter.width = w ;

	init_chart() ;

	tmp_out_num = min(out_num,tmp_out_num);

	for (int i = 0; i < tmp_out_num; i++)
		chart[i] = tmpChart[i];

	delete [] tmpChart;
}

//设置障碍物
void set_block(int num)
{
	//起点和终点不可以设为障碍物  
	if ((num) == 0 || (num) == out_num )
		return;
	if ( num > 0 && num < out_num )
		chart[num] = 1; //有障碍物的方块标识为1
}

//清除障碍物
void clear_block(int h,int w)
{
	if ( h >= 0 && 
		h < chart_height && 
		w >= 0 && 
		w < chart_width
		)
		chart[h*chart_width+w] = 0; //无障碍物的方块标识为0
}

bool is_block(int h,int w)
{
	if ( h >= 0 && 
		h < chart_height && 
		w >= 0 && 
		w < chart_width && 
		chart[h*chart_width+w] == 1 
		)
		return true;
	else
		return false;
}

bool is_block( int number ) 
{
	if ( number >=0 &&
		 number < out_num+1 &&
		 chart[number] == 1 ) {

		return true ;
	}else{
		return false ;
	}
}

void init_chart() 
{
	chart_height = cur_parameter.height ;
	chart_width = cur_parameter.width ;
	out_num = chart_height*chart_width - 1;

	chart = new int[ out_num+1 ];

	for (int i = 0; i < out_num+1 ; ++i) {
		chart[i] = 0 ;
	}

	for ( vector<int>::iterator i = cur_parameter.chart.begin() ;
		i != cur_parameter.chart.end() ;
		i++ ) {

			if ( *i > 0 && *i < out_num ) {

				chart[*i] = 1 ;
			}
	}
		
	//cur_parameter.chart.clear() ;
}

void delete_chart()
{
	if (chart){

		delete[] chart;
	}
}

void DefaultInitChart() 
{
}