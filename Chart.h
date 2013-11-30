#pragma once

extern int *chart;					//表示栅格图
extern int chart_width;				//表示图中栅格宽度
extern int chart_height;				//表示图中栅格高度
extern int out_num;					//标示路径出口序号

void set_block(int num); 				  //设置障碍物
void clear_block(int h,int w);				  //清除障碍物
bool is_block(int h,int w);					  //当前点是否为障碍物
bool is_block( int number ) ;
void get_chart_size(int &h,int &w) ;
void adjust_chart(int h,int w);			  //当参数改变时，需要调整栅格图大小
void init_chart() ;
void delete_chart() ;
void DefaultInitChart() ;