#include "stdafx.h"
#include "MOEADInd.h"
#include "Chart.h"

extern Fitness fit[1000] ;

CIndividual::CIndividual()
{
	x_var = vector<int>(nvar, 0);
	y_obj = vector<double>(nobj, 0);
	rank = 0;
}

CIndividual::~CIndividual()
{
	x_var.clear();
	y_obj.clear();
}

void CIndividual::rnd_init( )
{
	for( int n = 0 ; n < nvar ; n++ )
		x_var[n] = lowBound[n] +
		rnd_uni(&rnd_uni_init)*(uppBound[n] - lowBound[n]);   

	//obj_eval(n) ;
}

void CIndividual::obj_eval( const int& index )
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

		x1 = x_var[i] % chartWidth ;
		y1 = x_var[i] / chartWidth ;
		x2 = x_var[i+1] % chartWidth ;
		y2 = x_var[i+1] / chartWidth ;

		curX = x1 ;
		curY = y1 ;

		do {

			if(x2==x1){

				if( y2 > y1 ) { 
					
					nextY = curY + 1 ;
					nextX = curX ;
				} else { 
					
					nextY = curY - 1 ;
					nextX = curX ;
				}
			}else if( y2 == y1 ) {

				if( x2 > x1 ) {
					
					nextX = curX + 1 ;
					nextY = curY ;
				} else {
					
					nextX = curX - 1 ;
					nextY = curY ;
				}
			}else{

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
			if(chart[nextY][nextX] == 1)//如果通过了障碍物
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

	double len = sqrt((double)( (chartWidth-1) * (chartWidth-1)*2) );
	fit[n].A = len/A;

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

double CIndividual::evaluate_length(const int& n ) {

	double A = 0.0 ;

	//#对长度的评价
	for ( int j = 1; j < n; j++ ){

		//前一个点和当前点在栅格图上的坐标
		int x1,x2,y1,y2;
		x1 = x_var[j-1] % chartWidth ;
		y1 = x_var[j-1] / chartWidth ;
		x2 = x_var[j] % chartWidth ;
		y2 = x_var[j] / chartWidth ;
		A += sqrt( (double)(x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );//两点之间的距离
	}

	return A ;
}

double CIndividual::evaluate_smoothness(const int& n ) {

	double B = 0.0 ;

	//#对个体光滑度的评价 
	for (int j = 1; j < n-1; j++){

		//前一个点和当前点在栅格图上的坐标
		double angle, len1,len2;
		int x1, x2, y1, y2, x3, y3, 
			vectorx1, vectory1, vectorx2, vectory2 ;

		x1 = x_var[j-1] % chartWidth;
		y1 = x_var[j-1] / chartWidth;
		x2 = x_var[j] % chartWidth;
		y2 = x_var[j] / chartWidth;
		x3 = x_var[j+1] % chartWidth;
		y3 = x_var[j+1] / chartWidth;

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

double CIndividual::evaluate_security(  const int& n  ){

	double C = 0.0 ;

	//#对个体安全性的评价
	for(int j = 0; j < n-1; ++j){

		//当前点在栅格图上的坐标
		int x1, y1, x2, y2, m = 0;
		double len = 0.0;
		double L = 0.0;
		x1 = x_var[j]%chartWidth;
		y1 = x_var[j]/chartWidth;
		vector<int> vecChart;

		for(int i = 0; i < chartWidth; ++i)
			for(int k = 0; k < chartWidth; ++k)
				if(chart[i][k] == 1)
					vecChart.push_back(i*chartWidth + k);

		for(int i = 0; i < vecChart.size(); ++i){

			x2 = vecChart[i]%chartWidth;
			y2 = vecChart[i]/chartWidth;
			len = sqrt((double)((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)));
			if( check( this, ((y2-1)*chartWidth+x2)) && check( this,(y2*chartWidth+x2-1)) )
				m += 1;
			else if(check( this, ((y2+1)*chartWidth+x2)) && check( this, (y2*chartWidth+x2-1) ))
				m += 1;
			else if(check( this, ((y2+1)*chartWidth+x2)) && check( this, (y2*chartWidth+x2+1) ))
				m += 1;
			else if(check( this, ((y2-1)*chartWidth+x2)) && check( this, (y2*chartWidth+x2+1) ))
				m += 1;
			L += len;
			L = L - m * sqrt((double)(chartWidth*chartWidth));//进行惩罚
			if(L < 0)
				L = -L;

		}
		C = L / vecChart.size();
	}

	return C ;
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

void CIndividual::show_objective()
{
	for(int n=0; n<nobj; n++)
		printf("%f ",y_obj[n]);
	printf("\n");
}

void CIndividual::show_variable()
{
	for(int n=0; n<nvar; n++)
		printf("%f ",x_var[n]);
	printf("\n");
}

void CIndividual::operator=(const CIndividual &ind2)
{
	x_var = ind2.x_var;
	y_obj = ind2.y_obj;
	rank  = ind2.rank;
}

bool CIndividual::operator<(const CIndividual &ind2)
{
	bool dominated = true;
	for(int n=0; n<nobj; n++)
	{
		if(ind2.y_obj[n]<y_obj[n]) return false;
	}
	if(ind2.y_obj==y_obj) return false;
	return dominated;
}


bool CIndividual::operator<<(const CIndividual &ind2)
{
	bool dominated = true;
	for(int n=0; n<nobj; n++)
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

