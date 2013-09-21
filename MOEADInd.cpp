#include "stdafx.h"
#include "MOEADInd.h"
#include "Chart.h"



CIndividual::CIndividual()
{
	//x_var = vector<int>(nvar, 0);
	y_obj = vector<double>(N_OBJ, 0);
	//rank = 1;
}

CIndividual::~CIndividual()
{
	x_var.clear();
	y_obj.clear();
}

void CIndividual::rnd_init( )
{
///////////////////////////////////////////////////////////////////
//##Modified on 2013/9/10 ///////////////////////////////////
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

	//for( int n = 0 ; n < nvar ; n++ )
	//	x_var[n] = lowBound[n] +
	//	rnd_uni(&rnd_uni_init)*(uppBound[n] - lowBound[n]);   

	//obj_eval(n) ;
//////////////////////////////////////////////////////////////////////
}

void CIndividual::obj_eval( const int& index )
{
	int n = this->x_var.size() ;

	double length_fitness = evaluate_length(n) ;
	double smoothness_fitness = evaluate_smoothness(n) ;
	double security_fitness = evaluate_security(n) ;

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
	fit.length_fitness = len/length_fitness;

	if(smoothness_fitness == 0)
		fit.smoothness_fitness = 1;
	else
		fit.smoothness_fitness = 1/smoothness_fitness;

	if(security_fitness == 0)
		fit.security_fitness = 0;
	else
		fit.security_fitness = 1/security_fitness;

	y_obj[2] = length_fitness + EVAL_WEIGHT*D;					//长度
	y_obj[1] = smoothness_fitness + EVAL_WEIGHT*D;		    		//平滑度
	y_obj[0] = -security_fitness + EVAL_WEIGHT*D;					//安全性

	return;
}

double CIndividual::evaluate_length(const int& n ) {

	double length_fitness = 0.0 ;

	//#对长度的评价
	for ( int j = 1; j < n; j++ ){

		//前一个点和当前点在栅格图上的坐标
		int x1,x2,y1,y2;
		x1 = x_var[j-1] % chart_width ;
		y1 = x_var[j-1] / chart_width ;
		x2 = x_var[j] % chart_width ;
		y2 = x_var[j] / chart_width ;
		length_fitness += sqrt( (double)(x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );//两点之间的距离
	}

	return length_fitness ;
}

double CIndividual::evaluate_smoothness(const int& n ) {

	double smoothness_fitness = 0.0 ;

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
		smoothness_fitness += angle;
	}

	return smoothness_fitness ;
}

double CIndividual::evaluate_security(  const int& n  ){

	double security_fitness = 0.0 ;

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
		security_fitness = L / vecChart.size();
	}

	return security_fitness ;
}

bool CIndividual::check( CIndividual const* ind, const int& i )
{
	for(int k = 0; k < ind->x_var.size() ; ++k )
	{
		if( ind->x_var[k] == i)
			return true;
	}
	return false;
}
//
//void CIndividual::show_objective()
//{
//	for(int n=0; n<N_OBJ; n++)
//		printf("%f ",y_obj[n]);
//	printf("\n");
//}

//void CIndividual::show_variable()
//{
//	for(int n=0; n<nvar; n++)
//		printf("%f ",x_var[n]);
//	printf("\n");
//}

void CIndividual::operator=(const CIndividual &ind2)
{
	x_var = ind2.x_var;
	y_obj = ind2.y_obj;
	//rank  = ind2.rank;
}

bool CIndividual::operator<(const CIndividual &ind2)
{
	bool dominated = true;
	for(int n=0; n<N_OBJ; n++)
	{
		if(ind2.y_obj[n]<y_obj[n]) return false;
	}
	if(ind2.y_obj==y_obj) return false;
	return dominated;
}


bool CIndividual::operator<<(const CIndividual &ind2)
{
	bool dominated = true;
	for(int n=0; n<N_OBJ; n++)
	{
		if(ind2.y_obj[n]<y_obj[n]  - 0.0001) return false;
	}
	if(ind2.y_obj==y_obj) return false;
	return dominated;
}

bool CIndividual::operator==(const CIndividual &ind2)
{
	if(ind2.y_obj==y_obj) return true;
	else return false;
}

TCompare CIndividual::Compare(CIndividual& ind2) {
	bool bBetter = false;
	bool bWorse = false;

	int i = 0;
	do { 
		if(y_obj[i] < ind2.y_obj[i])
			bBetter = true;
		if(ind2.y_obj[i] < y_obj[i])
			bWorse = true;
		i++;
	}while (!(bWorse && bBetter) && (i < N_OBJ));
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


// defining subproblem 


TSOP::TSOP()
{
}

TSOP::~TSOP()
{
}


void TSOP::operator=(const TSOP&sub2)
{
	indiv  = sub2.indiv;
	table  = sub2.table;
	namda  = sub2.namda;
	array  = sub2.array;
}



//#endif

