#include "stdafx.h"
#include "MOEADInd.h"
#include "Chart.h"



CIndividual::CIndividual()
{
	////x_var = vector<int>(nvar, 0);
	y_obj = vector<double>(kObjNum, 0.0);
	//total_fitness = vector<double>(kObjNum, 0.0);
	//is_pass = false ;
	n_blocked = 0 ;
	//rank = 1;
}

CIndividual::~CIndividual()
{
	x_var.clear();
	y_obj.clear();
}

void CIndividual::RandomInit( )
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

	//ObjEvaluate(n) ;
//////////////////////////////////////////////////////////////////////
}

void CIndividual::ObjEvaluate( const int& index )
{

	ObjEval( this->x_var, index, this->y_obj ) ;

	return;
}
//
//double CIndividual::EvaluateLength(const int& n ) {
//
//	double length_fitness = 0.0 ;
//
//	//#对长度的评价
//	for ( int j = 1; j < n; j++ ){
//
//		//前一个点和当前点在栅格图上的坐标
//		int x1,x2,y1,y2;
//		x1 = x_var[j-1] % chart_width ;
//		y1 = x_var[j-1] / chart_width ;
//		x2 = x_var[j] % chart_width ;
//		y2 = x_var[j] / chart_width ;
//		length_fitness += sqrt( (double)(x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );//两点之间的距离
//	}
//
//	return length_fitness ;
//}
//
//double CIndividual::EvaluateSmoothness(const int& n ) {
//
//	double smoothness_fitness = 0.0 ;
//
//	//#对个体光滑度的评价 
//	for (int j = 1; j < n-1; j++){
//
//		//前一个点和当前点在栅格图上的坐标
//		double angle, len1,len2;
//		int x1, x2, y1, y2, x3, y3, 
//			vectorx1, vectory1, vectorx2, vectory2 ;
//
//		x1 = x_var[j-1] % chart_width;
//		y1 = x_var[j-1] / chart_width;
//		x2 = x_var[j] % chart_width;
//		y2 = x_var[j] / chart_width;
//		x3 = x_var[j+1] % chart_width;
//		y3 = x_var[j+1] / chart_width;
//
//		vectorx1 = x2 - x1 ;
//		vectory1 = y2 - y1 ;
//		vectorx2 = x3 - x2 ;
//		vectory2 = y3 - y2 ;
//
//		len1 = sqrt((double)(vectorx1*vectorx1+vectory1*vectory1));
//		len2 = sqrt((double)(vectorx2*vectorx2+vectory2*vectory2));
//
//		angle = (vectorx1*vectorx2+vectory1*vectory2) / (len1*len2);//计算向量的夹角
//		
//		if ( 0 == len1*len2 ) angle = 1.0 ;
//		if ( angle >= 1.0 ) angle = 1.0 ;
//		if ( angle <= -1.0 ) angle = -1.0 ; 
//
//		angle = acos(angle);
//		smoothness_fitness += angle;
//	}
//
//	return smoothness_fitness ;
//}
//
//double CIndividual::EvaluateSecurity(  const int& n  )
//{
//
//	double security_fitness = 0.0 ;
//	vector<int> vec_chart;
//
//	for(int i = 0; i < out_num+1 ; ++i)
//		if(is_block(i))
//			vec_chart.push_back(i);
//
//	//#对个体安全性的评价
//	for(int j = 0; j < n-1; ++j){
//
//		//当前点在栅格图上的坐标
//		int x_ind, y_ind, x_blk, y_blk, m = 0;
//		double len = 0.0;
//		double L = 0.0;
//		x_ind = x_var[j]%chart_width;
//		y_ind = x_var[j]/chart_width;
//
//		for(int i = 0; i < vec_chart.size(); ++i){
//
//			x_blk = vec_chart[i]%chart_width;
//			y_blk = vec_chart[i]/chart_width;
//			len = sqrt((double)((x_ind-x_blk)*(x_ind-x_blk) + (y_ind-y_blk)*(y_ind-y_blk)));
//			if( CheckSecurity( this, ((y_blk-1)*chart_width+x_blk)) && CheckSecurity( this,(y_blk*chart_width+x_blk-1)) )
//				m += 1;
//			else if(CheckSecurity( this, ((y_blk+1)*chart_width+x_blk)) && CheckSecurity( this, (y_blk*chart_width+x_blk-1) ))
//				m += 1;
//			else if(CheckSecurity( this, ((y_blk+1)*chart_width+x_blk)) && CheckSecurity( this, (y_blk*chart_width+x_blk+1) ))
//				m += 1;
//			else if(CheckSecurity( this, ((y_blk-1)*chart_width+x_blk)) && CheckSecurity( this, (y_blk*chart_width+x_blk+1) ))
//				m += 1;
//			L += len;
//			L = L - m * double( chart_width) ; //进行惩罚
//			if(L < 0)
//				L = 0;
//		}
//
//		security_fitness = L / vec_chart.size();
//	}
//
//	return security_fitness ;
//}
//
//bool CIndividual::CheckSecurity( CIndividual const* ind, const int& i )
//{
//	for(int k = 0; k < ind->x_var.size() ; ++k )
//	{
//		if( ind->x_var[k] == i)
//			return true;
//	}
//	return false;
//}
//
//void CIndividual::show_objective()
//{
//	for(int n=0; n<kObjNum; n++)
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
	for(int n=0; n<kObjNum; n++)
	{
		if(ind2.y_obj[n]<y_obj[n]) return false;
	}
	if(ind2.y_obj==y_obj) return false;
	return dominated;
}


bool CIndividual::operator<<(const CIndividual &ind2)
{
	bool dominated = true;
	for(int n=0; n<kObjNum; n++)
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

TCompare CIndividual::Compare(CIndividual& ind2)
{
	bool bBetter = false;
	bool bWorse = false;

	if(y_obj[obj1] < ind2.y_obj[obj1])
		bBetter = true;
	if(ind2.y_obj[obj1] < y_obj[obj1])
		bWorse = true;
	
	if(y_obj[obj2] < ind2.y_obj[obj2])
		bBetter = true;
	if(ind2.y_obj[obj2] < y_obj[obj2])
		bWorse = true;

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
	lambda  = sub2.lambda;
	array  = sub2.array;
}



//#endif

