
#include "stdafx.h"
#include "Common.h"
//#include "cec09.h"
#include "SNGAInd.h"

extern int divide;
extern Fitness fit[1000] ;

CSNGAInd::CSNGAInd()
{
	///////////////////////////////////////////////
	//##Modified on 2013/9/10///////////////
	//for( int i = 0 ; i < nvar ; i++ )
	//	x_var.push_back(0.0);
	///////////////////////////////////////////////
	for( int j = 0 ; j < nobj; j++)
		y_obj.push_back(0.0 ) ;

	//for(int j=0; j<nobj-1; j++)
	sectorialangle = 0;//.push_back(0.0);
}

CSNGAInd::~CSNGAInd()
{
	x_var.clear();
	y_obj.clear();

	//sectorialcoord.clear();
	//sectorialangle.clear();
}

void CSNGAInd::rnd_init()             //variable initival 
{
	////////////////////////////////////////////////////
	//##Modified on 2013/9/16 ///////////////////
	x_var.clear();
	int j = rand() % (chart_height+chart_width) ;

	if(j < 3){ //只有起点和终点

		x_var.push_back(0);
		x_var.push_back(out_num);

	}else{

		x_var.push_back(0);//加入起点
		//加入起点和终点之间的值
		for( int k = 1; k < j - 1 ; ++k ){

			//这里产生的初始化种群中也可能有重复点
			//所以在Init之后要调用GenDelSame
			int n;									// x, y;
			n = rand()%out_num;
			x_var.push_back(n);
		}
		x_var.push_back(out_num);//加入终点
	}

	///////////////////////////////////////////////
	//##Modified on 2013/9/10///////////////
	//for(int n=0;n<nvar;n++){

	//	x_var[n] = lowBound[n] + rnd_uni(&rnd_uni_init)*(uppBound[n] - lowBound[n]);                 //initival the variable
	//}

	//obj_eval();
}

void CSNGAInd::obj_eval( const int& index )                                              //initival objevtive function value
{
	int n = this->x_var.size() ;

	double A = evaluate_length(n) ;
	double B = evaluate_smoothness(n) ;
	double C = evaluate_security(n) ;

	int D = 0;
	int curX, curY, nextX, nextY ;
	double kX, kY, nextk ;
	int x1, x2, y1, y2 ;
	int count = 0 ;

	for ( int i = 0 ; i < n - 1 ; i++ ) {

		x1 = x_var[i] % chart_width ;
		y1 = x_var[i] / chart_width ;
		x2 = x_var[i+1] % chart_width ;
		y2 = x_var[i+1] / chart_width ;

		curX = x1 ;
		curY = y1 ;

		do {

			if(x2==x1){//#纵向运动

				if( y2 > y1 ) { 
					
					nextY = curY + 1 ;
					nextX = curX ;
				} else { 
					
					nextY = curY - 1 ;
					nextX = curX ;
				}
			}else if( y2 == y1 ) {//#横向运动

				if( x2 > x1 ) {
					
					nextX = curX + 1 ;
					nextY = curY ;
				} else {
					
					nextX = curX - 1 ;
					nextY = curY ;
				}
			}else{ //#斜向运动

				if( x2 > x1) nextX = curX+1;
				else nextX = curX-1;

				if( y2>y1) nextY = curY+1;
				else nextY = curY-1;

				if( x2< x1 ) nextX++;
				if( y2 < y1 ) nextY++;

				kX = (nextX-x1-0.5)/(x2-x1);
				kY = (nextY-y1-0.5)/(y2-y1);
				nextk = min(kX,kY);

				nextX = floor(x1+0.5+nextk*(x2-x1));
				nextY = floor(y1+0.5+nextk*(y2-y1));

				if(x2<x1&&kX<=kY) nextX--;
				if(y2<y1&&kY<=kX) nextY--;
			}
			if( nextY >= 0 && //#Added on 2013/9/11
				chart[nextY][nextX] == 1)//如果通过了障碍物
				D++;
			curX = nextX; curY = nextY;
			count++;

			if(count>1000){

				nextX=x2;
				nextY=y2;
			}
		}while(( !( nextX == x2 && nextY == y2 )));
		count=0;
	}

	double len = sqrt((double)( (chart_width-1) * (chart_width-1)*2) );
	fit[index].A = len/A;

	if(B == 0)
		fit[index].B = 1;
	else
		fit[index].B = 1/B;

	if(C == 0)
		fit[index].C = 0;
	else
		fit[index].C = 1/C;

	y_obj[2] = A + 100000*D;					//长度
	y_obj[1] = B + 100000*D;		    		//平滑度
	y_obj[0] = -C + 100000*D;					//安全性

	return;
}


double CSNGAInd::evaluate_length(const int& n ) {

	double A = 0.0 ;

	//#对长度的评价
	for ( int j = 1; j < n; j++ ){

		//前一个点和当前点在栅格图上的坐标
		int x1,x2,y1,y2;
		x1 = x_var[j-1] % chart_width ;
		y1 = x_var[j-1] / chart_width ;
		x2 = x_var[j] % chart_width ;
		y2 = x_var[j] / chart_width ;
		A += sqrt( (double)(x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );//两点之间的距离
	}

	return A ;
}

double CSNGAInd::evaluate_smoothness(const int& n ) {

	double B = 0.0 ;

	//#对个体光滑度的评价 
	for (int j = 1; j < n-1; j++){

		//前一个点和当前点在栅格图上的坐标
		double angle, len1,len2;
		int x1, x2, y1, y2, x3, y3, 
			vectorx1, vectory1, vectorx2, vectory2 ;

		x1 = x_var[j-1] % chart_width;
		y1 = x_var[j-1] / chart_width;
		x2 = x_var[j] % chart_width;
		y2 = x_var[j] / chart_width;
		x3 = x_var[j+1] % chart_width;
		y3 = x_var[j+1] / chart_width;

		vectorx1 = x2 - x1 ;
		vectory1 = y2 - y1 ;
		vectorx2 = x3 - x2 ;
		vectory2 = y3 - y2 ;

		len1 = sqrt((double)(vectorx1*vectorx1+vectory1*vectory1));
		len2 = sqrt((double)(vectorx2*vectorx2+vectory2*vectory2));

		angle = (vectorx1*vectorx2+vectory1*vectory2) / (len1*len2);//计算向量的夹角
		angle = acos(angle);
		B += angle;
	}

	return B ;
}

double CSNGAInd::evaluate_security(  const int& n  ){

	double C = 0.0 ;

	//#对个体安全性的评价
	for(int j = 0; j < n-1; ++j){

		//当前点在栅格图上的坐标
		int x1, y1, x2, y2, m = 0;
		double len = 0.0;
		double L = 0.0;
		x1 = x_var[j]%chart_width;
		y1 = x_var[j]/chart_width;
		vector<int> vecChart;

		for(int i = 0; i < chart_width; ++i)
			for(int k = 0; k < chart_width; ++k)
				if(chart[i][k] == 1)
					vecChart.push_back(i*chart_width + k);

		for(int i = 0; i < vecChart.size(); ++i){

			x2 = vecChart[i]%chart_width;
			y2 = vecChart[i]/chart_width;
			len = sqrt((double)((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)));
			if( check( this, ((y2-1)*chart_width+x2)) && check( this,(y2*chart_width+x2-1)) )
				m += 1;
			else if(check( this, ((y2+1)*chart_width+x2)) && check( this, (y2*chart_width+x2-1) ))
				m += 1;
			else if(check( this, ((y2+1)*chart_width+x2)) && check( this, (y2*chart_width+x2+1) ))
				m += 1;
			else if(check( this, ((y2-1)*chart_width+x2)) && check( this, (y2*chart_width+x2+1) ))
				m += 1;
			L += len;
			L = L - m * sqrt((double)(chart_width*chart_width));//进行惩罚
			if(L < 0)
				L = -L;

		}
		C = L / vecChart.size();
	}

	return C ;
}


bool CSNGAInd::check( CSNGAInd const* ind, const int& i )
{
	for(int k = 0; k < ind->x_var.size() ; ++k )
	{
		if( ind->x_var[k] == i)
			return true;
	}
	return false;
}
//void CSNGAInd::show_objective()
//{
//
//	for(int n=0;n<nobj;n++)
//		std::cout<<y_obj[n]<<" ";
//	//	for(int n=0;n<nobj-1;n++)
//	std::cout<<sectorialindex<<" ";
//	//std::cout<<rank<<" ";
//	std::cout<<"\n";
//
//}





//void CSNGAInd::yobj2angle(vector <double> & observerpoint)
//{
//
//	//double angle,distance;
//
//	double obj_relative_sum = 0.0;
//	for(int j=0; j<nobj; j++)
//	{
//		obj_relative_sum += y_obj[j] - (observerpoint[j] );
//	}
//
//	//for(int j=0; j<nobj; j++)
//	{
//
//		if(obj_relative_sum!=0)
//			sectorialangle = (y_obj[0] - (observerpoint[0] )) / obj_relative_sum ;
//		else 
//			sectorialangle = 0;
//	}
//}

//bool CSNGAInd::angle_in_sector(vector <double> & observerpoint, int sectornum, double anglesingle , int sectorcoord)
//{
//
//	bool bInSector = false;
//
//	//for(int j=0; j<nobj-1; j++)
//	{
////与修改角度计算有关的重要地方
//			if( floor( sectorialangle * (sectornum - 1) + 0.5 ) == sectorcoord )
//			bInSector = true;
//
//	}
//
//	return bInSector;
//}

void CSNGAInd::obj2angle_index(/*vector <double> & pseudonadirpoint, */vector <double> & observerpoint, int sectornum)  //观察点、扇形区域
{
	double obj_relative_sum = 0.0;
	double normalizedf[2];
	for(int j=0; j<nobj; j++)
	{
		//if(pseudonadirpoint[j] - observerpoint[j]>0)
		//	normalizedf[j] = (y_obj[j] - observerpoint[j] ) / (pseudonadirpoint[j] - observerpoint[j]);
		//else
		normalizedf[j] = y_obj[j] - observerpoint[j] ;
		obj_relative_sum += normalizedf[j];
	}

	//for(int j=0; j<nobj; j++)
	if(obj_relative_sum!=0)
		sectorialangle = normalizedf[0] / obj_relative_sum ;
	else 
		sectorialangle = 0;

	sectorialindex = floor( divide*sectorialangle * (sectornum - 1) + 0.5 );
}

TCompare CSNGAInd::Compare(CSNGAInd& ind2) {
	bool bBetter = false;
	bool bWorse = false;

	int i = 0;
	do { 
		if(y_obj[i] < ind2.y_obj[i])
			bBetter = true;
		if(ind2.y_obj[i] < y_obj[i])
			bWorse = true;
		i++;
	}while (!(bWorse && bBetter) && (i < nobj));
	if (bWorse) {
		if (bBetter)
			return _Pareto_Nondominated;
		else
			return _Pareto_Dominated;
	}
	else {
		if (bBetter)
			return _Pareto_Dominating;
		else
			return _Pareto_Equal;
	}
}

//bool CSNGAInd::operator<(const CSNGAInd& ind2)
//{
//	int flag2 = 0;
//	for(int n=0;n<nobj;n++)
//	{
//	    if(ind2.y_obj[n] < y_obj[n])
//	        return false;
//		if(ind2.y_obj[n] == y_obj[n])
//			flag2++;
//    }
//
//    if(flag2==nobj) return false;
//
//	return true;
//}
//
//bool CSNGAInd::operator==(const CSNGAInd& ind2)
//{
//	int flag = 0;
//	for(int n=0;n<nobj;n++)
//	{
//	    if(ind2.y_obj[n] !=y_obj[n])
//	        return false;
//    }
//    return true;
//}
//
//void CSNGAInd::operator=(const CSNGAInd& ind2)
//{
//	int n;
//	for(n=0;n<nobj;n++)
//	    y_obj[n] = ind2.y_obj[n];
//
//	for(n=0;n<nvar;n++)
//	    x_var[n] = ind2.x_var[n];
//    //rank  = ind2.rank;
//
//	for(n=0;n<nobj-1;n++)
//		sectorialcoord[n] = ind2.sectorialcoord[n];
//
//	for(n=0;n<nobj/*-1*/;n++)
//		sectorialangle[n] = ind2.sectorialangle[n];
//
//	sectorialindex = ind2.sectorialindex;
//
//}

