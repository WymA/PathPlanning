#pragma once

extern int *chart;					//��ʾդ��ͼ
extern int chart_width;				//��ʾͼ��դ����
extern int chart_height;				//��ʾͼ��դ��߶�
extern int out_num;					//��ʾ·���������

void set_block(int num); 				  //�����ϰ���
void clear_block(int h,int w);				  //����ϰ���
bool is_block(int h,int w);					  //��ǰ���Ƿ�Ϊ�ϰ���
bool is_block( int number ) ;
void get_chart_size(int &h,int &w) ;
void adjust_chart(int h,int w);			  //�������ı�ʱ����Ҫ����դ��ͼ��С
void init_chart() ;
void delete_chart() ;
void DefaultInitChart() ;