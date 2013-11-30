#include "stdafx.h"
#include "Common.h"
//#include "cec09.h"
#include "CAEAInd.h"

CAEAInd::CAEAInd()
{
	///////////////////////////////////////////////
	//##Modified on 2013/9/10///////////////
	//for( int i = 0 ; i < nvar ; i++ )
	//	x_var.push_back(0.0);
	///////////////////////////////////////////////
	for( int j = 0 ; j < kObjNum; j++)
		y_obj.push_back(0.0 ) ;
		//total_fitness.push_back(0.0) ;
	

	//for(int j=0; j<kObjNum-1; j++)
	sectorial_angle = 0;//.push_back(0.0);
	is_pass = false ;
	n_blocked = 0 ;
}

CAEAInd::~CAEAInd()
{
	x_var.clear();
	y_obj.clear();

	//sectorialcoord.clear();
	//sectorial_angle.clear();
}

void CAEAInd::RandomInit()             //variable initival 
{
	////////////////////////////////////////////////////
	//##Modified on 2013/9/16 ///////////////////
	x_var.clear();
	int j = rand() % (chart_height+chart_width);

	if(j < 3){ //只有起点和终点

		x_var.push_back(0);
		x_var.push_back(out_num);

	}else{

		x_var.push_back(0);//加入起点
		//加入起点和终点之间的值
		for( int k = 1; k < j - 1 ; ++k ){

			int n;
			n = rand()%out_num;

			x_var.push_back(n);
		}

		x_var.push_back(out_num);//加入终点
	}


}

void CAEAInd::ObjEvaluate( const int& index )                                              //initival objevtive function value
{
	n_blocked = ObjEval( this->x_var, index, this->y_obj ) ;

	if ( n_blocked == 0 ) 
		is_pass = true ;

	return;
}

//
//double CAEAInd::EvaluateLength(const int& n ) 
//{
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
//double CAEAInd::EvaluateSmoothness(const int& n ) 
//{
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
//		////////////////////////////////////////////////
//		//## Added on 2013/9/24 /////////////////
//		if ( 0 == len1*len2 ) angle = 1.0 ;
//		if ( angle >= 1.0 ) angle = 1.0 ;
//		if ( angle <= -1.0 ) angle = -1.0 ; 
//		///////////////////////////////////////////////
//		angle = acos(angle);
//		smoothness_fitness += angle;
//	}
//
//	return smoothness_fitness ;
//}
//
//double CAEAInd::EvaluateSecurity(  const int& n  )
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
//		security_fitness += L / vec_chart.size();
//	}
//
//	return security_fitness ;
//}
//
//
//bool CAEAInd::CheckSecurity( CAEAInd const* ind, const int& i )
//{
//	for(int k = 0; k < ind->x_var.size() ; ++k )
//	{
//		if( ind->x_var[k] == i)
//			return true;
//	}
//	return false;
//}
//void CAEAInd::show_objective()
//{
//
//	for(int n=0;n<kObjNum;n++)
//		std::cout<<y_obj[n]<<" ";
//	//	for(int n=0;n<kObjNum-1;n++)
//	std::cout<<sectorialindex<<" ";
//	//std::cout<<rank<<" ";
//	std::cout<<"\n";
//
//}





//void CAEAInd::yobj2angle(vector <double> & observerpoint)
//{
//
//	//double angle,distance;
//
//	double obj_relative_sum = 0.0;
//	for(int j=0; j<kObjNum; j++)
//	{
//		obj_relative_sum += y_obj[j] - (observerpoint[j] );
//	}
//
//	//for(int j=0; j<kObjNum; j++)
//	{
//
//		if(obj_relative_sum!=0)
//			sectorial_angle = (y_obj[0] - (observerpoint[0] )) / obj_relative_sum ;
//		else 
//			sectorial_angle = 0;
//	}
//}

//bool CAEAInd::angle_in_sector(vector <double> & observerpoint, int sectornum, double anglesingle , int sectorcoord)
//{
//
//	bool bInSector = false;
//
//	//for(int j=0; j<kObjNum-1; j++)
//	{
////与修改角度计算有关的重要地方
//			if( floor( sectorial_angle * (sectornum - 1) + 0.5 ) == sectorcoord )
//			bInSector = true;
//
//	}
//
//	return bInSector;
//}

void CAEAInd::obj2angle_index( vector<double>& observerpoint, int sectornum )  //观察点、扇形区域
{
	double obj_relative_sum = 0.0;
	double normalizedf[kObjNum] ;
	for(int j=0; j<kObjNum; j++){

		//if(pseudonadirpoint[j] - observerpoint[j]>0)
		//	normalizedf[j] = (y_obj[j] - observerpoint[j] ) / (pseudonadirpoint[j] - observerpoint[j]);
		//else

		normalizedf[j] = y_obj[j] - observerpoint[j] ;
		//obj_relative_sum += normalizedf[j];
	}

	obj_relative_sum = normalizedf[obj1] + normalizedf[obj2] ;

	//for(int j=0; j<kObjNum; j++)
	if(obj_relative_sum!=0)
		sectorial_angle = normalizedf[obj1] / obj_relative_sum ;
	else 
		sectorial_angle = 0;

	sectorialindex = floor( divide*sectorial_angle * (sectornum - 1) + 0.5 );
}

TCompare CAEAInd::Compare(CAEAInd& ind2) {

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