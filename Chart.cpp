#include "stdafx.h"
#include "Chart.h"

int *chart;					//��ʾդ��ͼ
int chart_width;				//��ʾͼ��դ����
int chart_height;				//��ʾͼ��դ��߶�
int out_num;					//��ʾ·���������


void get_chart_size(int &h,int &w)
{
	h = chart_height;
	w = chart_width;
}


//����դ��ͼ��С
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

//�����ϰ���
void set_block(int num)
{
	//�����յ㲻������Ϊ�ϰ���  
	if ((num) == 0 || (num) == out_num )
		return;
	if ( num > 0 && num < out_num )
		chart[num] = 1; //���ϰ���ķ����ʶΪ1
}

//����ϰ���
void clear_block(int h,int w)
{
	if ( h >= 0 && 
		h < chart_height && 
		w >= 0 && 
		w < chart_width
		)
		chart[h*chart_width+w] = 0; //���ϰ���ķ����ʶΪ0
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