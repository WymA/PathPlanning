#pragma once

extern int **chart;					//��ʾդ��ͼ
extern int chart_width;				//��ʾͼ��դ����
extern int chart_height;				//��ʾͼ��դ��߶�
extern int out_num;					//��ʾ·���������

void SetBlock(int h,int w);					  //�����ϰ���
void ClearBlock(int h,int w);				  //����ϰ���
bool IsBlock(int h,int w);					  //��ǰ���Ƿ�Ϊ�ϰ���
void GetChartSize(int &h,int &w) ;
void AdjustChart(int h,int w);			  //�������ı�ʱ����Ҫ����դ��ͼ��С
void InitChart() ;
void DeleteChart() ;
void DefaultInitChart() ;