
#include "stdafx.h"

#include "Global.h"
#include "Recomb.h"
#include "Common.h"

#include "CAEAInd.h"
#include "CAEA.h"



CAEA::CAEA()
{
	
	cur_gen = 0 ;
	evaluations = 0 ;

}

CAEA::~CAEA()
{
	sector_population.clear();
}


void CAEA::InitParameter() 
{
	//init_chart() ;

	population_size = cur_parameter.pSize ;
	total_gen = cur_parameter.total_gen ;
	cross_rate = cur_parameter.pop_cross_rate ;
	mutation_rate = cur_parameter.pop_mutation_rate ;

	parato_front = vector<vector<double>>(population_size, vector<double>(kObjNum, 0.0)) ;

}



bool CAEA::update_extreme_point(CAEAInd& ind)        //uptate the extreme point , that's f up/down尖 
{
	bool is_anchor_updated = false;
	bool is_true_nadair_updated = false;

	for( int j=0;j<kObjNum ;j++){

		if ( ind.y_obj[j] < anchor_point[j][j] ||
			 ( ind.y_obj[j] == anchor_point[j][j] && 
			 ind.y_obj[(j+1)%2] < anchor_point[j][(j+1)%2] ))
		{

			is_anchor_updated = true;
			anchor_point[j] = ind.y_obj;
		}
	}

	for( int j = 0 ; j < kObjNum ; j++ ){

		if( ind.y_obj[j] >true_nadir_point[j] ){

			is_true_nadair_updated = true;
			true_nadir_point[j] = ind.y_obj[j];
		}
	}

	if ( is_anchor_updated ){

		for( int j = 0 ; j < kObjNum ; j++ ){

			observer_point[j] = anchor_point[j][j];
			//PseudoNadirPoint[j] = anchor_point[(j+1)%2][j];
		}
	}

	if( is_anchor_updated || is_true_nadair_updated){

		for( int j = 0 ; j < kObjNum ; j++)	{

			//if(PseudoNadirPoint[j]-observer_point[j]>0)
			//	reference_point[j] = ( true_nadir_point[j] + 1e3 * (true_nadir_point[j] - observer_point[j]) ) / (PseudoNadirPoint[j]-observer_point[j]);
			//else
			reference_point[j] = ( true_nadir_point[j] + 1e3 * (true_nadir_point[j] - observer_point[j]) );
		}
	}

	return is_anchor_updated;
}

void CAEA::InitPopulation()
{
	vector <bool>  sectorpopFlag;

	vector <CAEAInd>  initial_indi_pop;
	CAEAInd rest_ind;
	CAEAInd ind;

	for(int n=0;n<population_size;n++) {//统计极值点or观测点  初始个体数目设置//初始化种群所有个体，每个个体是ind，种群是initial_indi_pop
	
		sectorpopFlag.push_back(false);
		ind.RandomInit();   //inivitalize the variable and objective function (PS&PF)
		ind.ObjEvaluate(n) ;
		initial_indi_pop.push_back(ind);//insert_sectorialgrid(ind, n);

	}

	//initial_observation_and_reference_point(initial_indi_pop[0]);

	for( int j=0;j<kObjNum ;j++){
	
		anchor_point.push_back( initial_indi_pop[0].y_obj );//observer_point.push_back( initial_indi_pop[0].y_obj[j] );
		true_nadir_point.push_back( initial_indi_pop[0].y_obj[j] );//reference_point.push_back( initial_indi_pop[0].y_obj[j] );
	}
	for( int j=0;j<kObjNum ;j++){
	
		observer_point.push_back( anchor_point[j][j] );
		//PseudoNadirPoint.push_back( anchor_point[(j+1)%2][j] );
	}
	for(int j=0;j<kObjNum ;j++){
	
		reference_point.push_back( true_nadir_point[j] + 1e3 * (true_nadir_point[j] - observer_point[j]) ) ;
	}

	for(int n=1;n<population_size;n++){//统计极值点or观测点  初始个体数目设置//uptate the extreme point , that's f up/down尖//update 锚点、最小点、观察点、参考点
	
		update_extreme_point(initial_indi_pop[n]);
	}

	for(int n=0;n<population_size;n++) { //统计极值点or观测点  初始个体数目设置
	
		sector_population.push_back(initial_indi_pop[0]);
	}

	vector <CAEAInd>  initial_rest_indi_pop;
	for(int n=0;n< population_size;n++) { //计算角度 第一轮按角度填充个体 并收集剩余个体
	
		//initial_indi_pop[n].yobj2angle(observer_point);
		initial_indi_pop[n].obj2angle_index( observer_point, population_size);

		if (sectorpopFlag[initial_indi_pop[n].sectorialindex] == false) {//if (sectorialgrid[initial_indi_pop[n].sectorialindex] == -1)
		
			sector_population[initial_indi_pop[n].sectorialindex] = initial_indi_pop[n];
			sectorpopFlag[initial_indi_pop[n].sectorialindex] = true;

		}else{

			bool bReplaced; 
			rest_ind = sector_population[initial_indi_pop[n].sectorialindex];//.indiv;
			bReplaced = pareto_hypervolume_compare_sectorialgrid(initial_indi_pop[n]);

			if(bReplaced){

				initial_rest_indi_pop.push_back( rest_ind );
			}else{

				initial_rest_indi_pop.push_back( initial_indi_pop[n] );
			}
		}
	}

	initial_indi_pop.clear();

	for( int n = 0 ; n < population_size ; n++ ) { //填充剩余的个体
	
		if ( sectorpopFlag[ n ] == false ){

			double min_angle_difference, angle_difference ;
			int min_angle_difference_ind ;
			int rest_size = initial_rest_indi_pop.size();

			min_angle_difference = abs( initial_rest_indi_pop[0].sectorial_angle - (double)n / (double)(population_size-1) );
			//min_angle_difference = abs( initial_rest_indi_pop[0].sectorialindex - n );
			min_angle_difference_ind = 0 ;

			for(int j = 1; j<rest_size; j++ )	{

				angle_difference = abs( initial_rest_indi_pop[j].sectorial_angle - (double)n / (double)(population_size-1) );
				//angle_difference = abs( initial_rest_indi_pop[j].sectorialindex - n );
				if(angle_difference < min_angle_difference)	{

					min_angle_difference = angle_difference;
					min_angle_difference_ind = j;
				}
			}
			sector_population[n] = initial_rest_indi_pop[min_angle_difference_ind];
			sectorpopFlag[n] = true;
			rest_ind = initial_rest_indi_pop[min_angle_difference_ind];
			initial_rest_indi_pop[min_angle_difference_ind] = initial_rest_indi_pop[ rest_size-1 ];
			initial_rest_indi_pop[ rest_size-1 ] = rest_ind;
			initial_rest_indi_pop.pop_back();

		}
	}

	//double min_angle_difference, angle_difference ;
	//int min_angle_difference_ind ;
	//int min_angle_insert_point;

	//for(int rest_size = initial_rest_indi_pop.size();rest_size>0;rest_size--)
	//{
	//	min_angle_difference = 2.0;
	//	for(int n=0;n< population_size;n++)//填充剩余的个体
	//	{
	//		if (sectorpopFlag[ n ] == false)
	//		{
	//			//min_angle_difference = abs( initial_rest_indi_pop[0].sectorial_angle - (double)n / (double)(population_size-1) );
	//			//min_angle_difference_ind = 0 ;
	//			for(int j = 0; j<rest_size; j++ )
	//			{
	//				angle_difference = abs( initial_rest_indi_pop[j].sectorial_angle - (double)n / (double)(population_size-1) );
	//				if(angle_difference < min_angle_difference)
	//				{
	//					min_angle_difference = angle_difference;
	//					min_angle_difference_ind = j;
	//					min_angle_insert_point = n;
	//				}
	//			}
	//		}
	//	}

	//	sector_population[min_angle_insert_point] = initial_rest_indi_pop[min_angle_difference_ind];
	//	sectorpopFlag[min_angle_insert_point] = true;
	//	initial_rest_indi_pop[min_angle_difference_ind] = initial_rest_indi_pop[ rest_size-1 ];
	//}

	initial_rest_indi_pop.clear();
	sectorpopFlag.clear();
}

bool CAEA::pareto_hypervolume_compare_sectorialgrid(CAEAInd& ind)
{
	bool bReplaced = false;

	double contributing1, contributing2;

	//int ind_index = ind.sectorialindex;
	//TCompare iResult1 = ind.Compare( sector_population[ind.sectorialindex] );//.indiv
	//if( sector_population[ind.sectorialindex].angle_in_sector(observer_point, population_size, anglesingle, ind.sectorialindex) )
	if( sector_population[ind.sectorialindex].sectorialindex == ind.sectorialindex ){

		//if(iResult1 == _Pareto_Dominating)
		//{
		//	sector_population[ind.sectorialindex] = ind;//replace   //.indiv
		//	bReplaced = true;
		//}
		//else if(iResult1 == _Pareto_Nondominated)
		//{
		contributing1 = get_fastigiate_hypervolume(ind, ind.sectorialindex, reference_point);
		contributing2 = get_fastigiate_hypervolume(sector_population[ind.sectorialindex], ind.sectorialindex, reference_point);

		if( contributing1 < contributing2 ){

			sector_population[ind.sectorialindex] = ind;//replace
			bReplaced = true;
		}
	
	}else{

		sector_population[ind.sectorialindex] = ind;//replace
		bReplaced = true;
	}

	return bReplaced;
}

double CAEA::get_fastigiate_hypervolume(CAEAInd&  ind, int ind_index, vector <double> &reference_point)
{
	double fastigiate_volume;

	double normalizedf[kObjNum];
	for( int j = 0 ; j < kObjNum ; j++ ){

		normalizedf[j] = ind.y_obj[j] - observer_point[j] ;

	}

	if(ind_index==0){

			fastigiate_volume = 
				0.5*(ind_index+0.5)/(population_size-ind_index-1.5)
				*normalizedf[obj1]*normalizedf[obj1] 
				+ (reference_point[obj1]-normalizedf[obj1])*normalizedf[obj2];

	}else if(ind_index==population_size-1){

		fastigiate_volume = 
			0.5*(population_size-ind_index-0.5)/(ind_index-0.5)
			*normalizedf[obj2]*normalizedf[obj2] 
			+ (reference_point[obj2]-normalizedf[obj2])*normalizedf[obj1];
	}else{

		fastigiate_volume = 
			0.5*(population_size-ind_index-0.5)/(ind_index-0.5)
			*normalizedf[obj2]*normalizedf[obj2] 
			+ 0.5*(ind_index+0.5)/(population_size-ind_index-1.5)
			*normalizedf[obj1]*normalizedf[obj1] - normalizedf[obj2]*normalizedf[obj1];
	}



	return fastigiate_volume;
}

//double CAEA::get_fastigiate_hypervolume(CAEAInd&  ind, int ind_index, vector <double> &reference_point)
//{
//	double fastigiate_volume;
//
//	double Volume1, Volume2, height1, height2, BottomArea1, BottomArea2;
//
//	double f1sharp, f2sharp;
//
//	double directionf1, directionf2;
//
//	height1 = ind.y_obj[1] - observer_point[1];
//	if(ind_index == population_size-1 )
//	{
//		f1sharp = reference_point[0];
//	}else{
//		directionf1 = (ind_index + 0.5) / (double) (population_size-1) ;
//		f1sharp = directionf1 / (1 - directionf1) * (ind.y_obj[1] - observer_point[1]) +  observer_point[0];
//	}
//	BottomArea1 = f1sharp - ind.y_obj[0];
//	Volume1 = 0.5 * BottomArea1 * height1;
//
//	height2 = ind.y_obj[0] - observer_point[0];
//	if(ind_index == 0 )
//	{
//		f2sharp = reference_point[1];
//	}else{
//		directionf2 = (ind_index - 0.5) / (double) (population_size-1) ;
//		f2sharp =  (1 - directionf2) / directionf2 * (ind.y_obj[0] - observer_point[0]) +  observer_point[1];
//	}
//	BottomArea2 = f2sharp - ind.y_obj[1];
//	Volume2 = 0.5 * BottomArea2 * height2;
//	
//	fastigiate_volume = Volume1 + Volume2;
//
//	return fastigiate_volume;
//}

/////////////////////////////////////////////////////////
// returns the hypervolume of ps[0 ..] in 2D ///////
// assumes that ps is sorted improving/////////////
/////////////////////////////////////////////////////////


double CAEA::compute_hypervolume(vector <CAEAInd>&  mysectorpop, int mypopsize, int mynobj)
{
	vector <vector<double>> paretofront;

	vector<double> onepoint;
	onepoint.resize(2);      //set the size of vector is 2 at least  
	for(int i=0;i<mypopsize;i++)
	{
		bool Dominated = false;
		for(int j=0;j<mypopsize;j++)
		{
			if(j!=i && ParetoCompare(mysectorpop[i].y_obj, mysectorpop[j].y_obj) == _Pareto_Dominated)
			{
				Dominated = true;
				break;
			}
		}
		if(!Dominated && mysectorpop[i].y_obj[0]<=xHypervolumeRefPoint_TopRight && mysectorpop[i].y_obj[1]<=yHypervolumeRefPoint_TopRight)
		{
			onepoint[0] = mysectorpop[i].y_obj[0];
			onepoint[1] = mysectorpop[i].y_obj[1];
			paretofront.push_back(onepoint);
		}
	}

	QuickSortIncreasing(paretofront, paretofront.size());
	double front2Dhv = hv2(paretofront);

	front2Dhv = front2Dhv / ((xHypervolumeRefPoint_TopRight - xHypervolumeRefPoint_BottomLeft)*(yHypervolumeRefPoint_TopRight - yHypervolumeRefPoint_BottomLeft));

	//if(front2Dhv>1)
	//	front2Dhv = front2Dhv;

	return front2Dhv;
}

double  CAEA::tour_selection_hv_difference(int p, vector<CAEAInd>& mypopulation)
{
	//int p1 = int(rnd_uni(&rnd_uni_init)*population_size);
	//int p2 = int(rnd_uni(&rnd_uni_init)*population_size);
	int num = 0;
	double hv1 = get_fastigiate_hypervolume(mypopulation[p], mypopulation[p].sectorialindex, reference_point);
	double hv3,hv4,hv_difference=0;
	if(p-1>=0){

		hv3 = get_fastigiate_hypervolume(mypopulation[p-1], mypopulation[p-1].sectorialindex, reference_point);
		hv_difference += hv3 - hv1;
		num++;
	}
	if(p+1<=population_size-1){

		hv4 = get_fastigiate_hypervolume(mypopulation[p+1], mypopulation[p+1].sectorialindex, reference_point);
		hv_difference += hv4 - hv1;
		num++;
	}
	hv_difference = hv_difference / num;

	return hv_difference;
}

int  CAEA::tour_selection_hv2(vector<CAEAInd>& mypopulation)
{
	int p1 = int(rand()%population_size) ;
	int p2 = int(rand()%population_size) ;

	double hv1 = tour_selection_hv_difference( p1, mypopulation );
	double hv3 = tour_selection_hv_difference( p2, mypopulation );

	if( hv1 >= hv3 )
		return p1;
	else
		return p2;
}

void save_pop_front(vector <vector<double>>  &pop_front, char saveFilename[1024]);

//void CAEA::uniform_selection(CAEAInd*& ind_selected)
//{
//	int pop_index;
//
//	pop_index = int(rnd_uni(&rnd_uni_init)*( population_size ));
//
//	ind_selected = &(sector_population[pop_index]);//.indiv
//}


void CAEA::reset_angle()
{
	int n;
	for(n=0; n<population_size; n++) {

		//sector_population[n].yobj2angle(observer_point);
		sector_population[n].obj2angle_index(/*PseudoNadirPoint, */observer_point, population_size);
	}
}

//void CAEA::save_population(vector <CAEAInd>  &mypopulation, char saveFilename[1024])
//{
//	std::fstream fout;
//	fout.open(saveFilename,std::ios::out);
//	for(int n=0; n<mypopulation.size(); n++)
//	{
//		for(int k=0;k<kObjNum;k++)
//		{
//			fout<<mypopulation[n].y_obj[k]<<"  ";
//		}
//		fout<<"\n";
//	}
//	fout.close();
//}

//void save_pop_front(vector <vector<double>>  &pop_front, char saveFilename[1024])
//{
//	std::fstream fout;
//	fout.open(saveFilename,std::ios::out);
//	for(int n=0; n<pop_front.size(); n++)
//	{
//		for(int k=0;k<kObjNum;k++)
//		{
//			fout<<pop_front[n][k]<<"  ";
//		}
//		fout<<"\n";
//	}
//	fout.close();
//}
//
//void CAEA::save_ps(char saveFilename[1024])
//{
//	std::fstream fout;
//	fout.open(saveFilename,std::ios::out);
//	for(int n=0; n<population_size; n++)
//	{
//		///////////////////////////////////////////////
//	//##Modified on 2013/9/10///////////////
//		//for(int k=0;k<nvar;k++)
//		//{
//		//	fout<<sector_population[n].x_var[k]<<"  ";
//		//}
//		//fout<<"\n";
//	}
//	fout.close();
//}

//void CAEA::Population2Front(vector <CAEAInd>  &mypopulation, vector <CAEAInd>  &parato_front)
void CAEA::Population2Front()
{
	vector<int> nDominated;
	for(int n=0;n<sector_population.size();n++)
		nDominated.push_back(0);

	for(int k=0; k<sector_population.size(); k++){

		for(int j=k+1; j<sector_population.size(); j++){

			//TCompare tresult = sector_population[k].Compare( sector_population[j] );
			
			TCompare tresult = ParetoCompare( total_fitness[k], total_fitness[j]);

			if(tresult == _Pareto_Dominated)
				nDominated[k]++;
			else if(tresult == _Pareto_Dominating)
				nDominated[j]++;
		}
	}

	dominating_fitness.clear() ;
	dominated_fitness.clear() ;

	for( int n = 0 ; n < sector_population.size() ; n++ ) {

		if( nDominated[n] == 0 )
			dominating_fitness.push_back( total_fitness[n] ) ;
		else
			dominated_fitness.push_back( total_fitness[n] ) ;
		
	}

	nDominated.clear();
}

////////////////////////////////////////////////////////////////////////////
void CAEA::GetBestObj( const int& i_obj, vector<int> &path ) 
{
	double cur_best_obj_value = sector_population[0].y_obj[ i_obj ] ;
	int cur_best_obj_index = 0 ;

	for( int i = 1 ; i < population_size; ++i ) {

		if ( sector_population[i].y_obj[ i_obj ] < cur_best_obj_value ) {

			cur_best_obj_value = sector_population[i].y_obj[ i_obj ] ;
			cur_best_obj_index = i ;
		}	
	}

	path = sector_population[cur_best_obj_index].x_var ;
}

//
//double CAEA::calc_distance()    //calculate the average distance between ps and current solutions
//{
//	double distance = 0;
//	for(int i=0; i<ps.size(); i++)
//	{
//		double min_d = 1.0e+10;
//		for(int j=0; j<population_size; j++)
//		{
//			double d = DistVector(ps[i].y_obj, sector_population[j].y_obj);
//			if(d<min_d)  min_d = d;
//		}
//		distance+= min_d;
//	}
//	distance/=ps.size();
//
//	return distance;
//}


void CAEA::evaluation() 
{
	for ( int i = 0 ; i < sector_population.size() ; i++ ) {

		sector_population[i].ObjEvaluate(i) ;
	}
	fo<<endl ;
}


void CAEA::GenPopDelSame() 
{
	for( int i = 0; i < population_size ; i++ ){

		GenDeleteSame( sector_population[i].x_var ) ;
	}
}

void CAEA::GenDeleteSame( vector<int>& x_path )
{
	int start_point;
	int find_point;
	bool find_flag;
	int k;
	for( int j = 0 ; j < ( x_path.size() - 1 ) ; ++j )
	{
		int cur_value = x_path[j];
		start_point = j+1;
		find_flag = false;
		for(k = x_path.size()-1; k > j; --k) {

			if(x_path[k] == cur_value){

				find_point = k;
				find_flag = true;
				break;
			}
		}
		if(find_flag){

			for( int i = k+1 ; i < x_path.size() ; i++ )
				x_path[j+i-k] = x_path[i];

			for(int i = 0; i < k-j; ++i)
				x_path.pop_back();
		}
	}
}

////////////////////////////////////////////////////////////////
void CAEA::DeleteNotPass() 
{
	for( vector<CAEAInd>::iterator index = sector_population.begin() ;
		index != sector_population.end() ; index++ ) {
	
			if ( !index->is_pass )
				index->x_var.clear() ;
	}
}

int CAEA::GenInsertInd( CAEAInd &ind, const int& idx )
{
	//如果和前一位连续则不插，插入中值法计算出的点
	int bSeries = is_series(ind, idx);
	if( bSeries == 0 )	{

		int pre = ind.x_var[idx-1];
		int cur = ind.x_var[idx];
		int x0, y0 ;
		int x_pre, y_pre ;
		int x_cur, y_cur ;
		int x_ins= 0, y_ins = 0; //真正要插入的序号
		x_pre = pre%chart_width;
		y_pre = pre/chart_width;
		x_cur = cur%chart_width;
		y_cur = cur/chart_width;
		x0 = (x_pre + x_cur)/2;

		if(abs(y_cur - y_pre) == 1)
			y0 = (y_pre + y_cur + 1) / 2;
		else
			y0 = (y_pre + y_cur) / 2;

		//非障碍物点，可以插入
		if(chart[y0*chart_width+x0] == 0){

			x_ins = x0;
			y_ins = y0;

		}else{//障碍物点，找到离该点最近的非障碍物点插入

			int ySpan = max(y0, (chart_height - 1 - y0));
			double preMinDist, curMinDist;
			//将最短距离设置为一个大的初值
			preMinDist = curMinDist = kMaxDist;
			//搜索当前行
			curMinDist = SearchRowNearest(x0, y0, y0, preMinDist, x_ins );
			//在当前行中找到点

			if(curMinDist < kMaxDist){

				y_ins = y0;
			} else {

				//分别搜索上面的行和下面的行
				for(int j = 0; j <= ySpan; ++j) {

					if((y0 - j) >= 0){

						curMinDist = SearchRowNearest(x0, y0, y0 - j, preMinDist, x_ins );
						if(curMinDist < preMinDist){

							y_ins = y0;
							preMinDist = curMinDist;
						}
					}
					if((y0 + j) < chart_height)	{

						curMinDist = SearchRowNearest(x0, y0, y0 + j, preMinDist, x_ins );
						if(curMinDist < preMinDist){

							y_ins = y0;
							preMinDist = curMinDist;
						}
					}

					if(curMinDist < kMaxDist)
						break;
				}
			}
		}

		int ins = x_ins + chart_width*y_ins;
		//有效的插入点

		if( ins != 0 && ins != ind.x_var[idx]){

			intVec::iterator iter = ind.x_var.begin() + idx;
			ind.x_var.insert(iter, ins);
			return 1;
		}
	}

	return 0;
}

void CAEA::GenInsert( ) 
{
	for(int i = 0; i < population_size ; ++i ){

		int len = sector_population[i].x_var.size();
		int insertResult ;
		for( int j = 1; j < len; ){

			insertResult = GenInsertInd( sector_population[i], j ) ;

			if( insertResult == 0 ) 
				j += 2 ; //##1
			else
				j += 3 ; //##2
			

			len = sector_population[i].x_var.size();

		}
	}
}

void CAEA::GenInsertInd( CAEAInd& ind ) 
{
	int len = ind.x_var.size();
	int insertResult ;
	for( int idx = 1; idx < len; ){

		insertResult = GenInsertInd( ind, idx ) ;

		if( insertResult == 0 ) 
			idx += 1 ; //##1
		else
			idx += 2 ; //##2
			

		len = ind.x_var.size();

	}
}

void CAEA::GenInsert( CAEAInd& indiv ) 
{
	int len = indiv.x_var.size();
	int insertResult ;
	for( int j = 1; j < len; ){

		insertResult = GenInsertInd( indiv, j ) ;

		if( insertResult == 0 ) 
			j += 1;
		else
			j += 2;
			

		len = indiv.x_var.size();

	}
}


////////////////////////////////////////////////////////////////


void CAEA::Execute( CRobotView* m_myView )
{

	InitParameter() ;
	evaluations = population_size ;
	InitPopulation() ;                              //initival the population

	int dst_time, src_time;

	GenPopDelSame() ;
	GenInsert() ;

	Population2Front() ;

	GetBestObj( kLength, path_length ) ;
	GetBestObj( kSmoothness, path_smoothness ) ;
	GetBestObj( kSecurity, path_security ) ;

	GetBestFit( kLength ) ;
	GetBestFit( kSmoothness ) ;
	GetBestFit( kSecurity ) ;
				
	m_myView->Invalidate();
	m_myView->UpdateWindow(); 

	int iLength = 0;

	//evaluation() ;
	GenPopDelSame() ;

	CAEAInd child2;
	while( run_state != kStop ) { 
	
		int parent_index1, parent_index2;

		if( iLength  == 0 )
			parent_index1 = 0;
		else if( iLength == 1 )
			parent_index1 = population_size - 1;
		else 
			parent_index1 = tour_selection_hv2( sector_population );

		parent_index2 =  tour_selection_hv2( sector_population );
		
		
		GenCrossover( sector_population[parent_index1], sector_population[parent_index2], onechild, child2 ) ;
		GenMutation( onechild ) ;

		//##!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		onechild.ObjEvaluate(kNotIndex) ;
	
		//nfes++;
		evaluations++;

		bool updated = update_extreme_point(onechild);//update_anglerange_maxminindi(onechild, true); //update_nondominated_obj_maxmin(onechild);//update_angle_maxmin(onechild);//
		
		if(updated)	
			reset_angle();

		//onechild.yobj2angle(observer_point);
		onechild.obj2angle_index( observer_point, population_size);//之前之所以出现奇异点 是因为计算onechild角度是在统计极端点前 极端点改变了 而onechild的角度是老的

		//update_sectorialgrid(onechild);
		pareto_hypervolume_compare_sectorialgrid(onechild);

		int tmp = cur_gen ;
		cur_gen = evaluations / population_size + 1 ;

		if ( cur_gen != tmp ) {

			GenPopDelSame() ;
			GenInsert() ;
			evaluation() ;
			Population2Front() ;

			GetBestObj( kLength, path_length ) ;
			GetBestObj( kSmoothness, path_smoothness ) ;
			GetBestObj( kSecurity, path_security ) ;

			GetBestFit( kLength ) ;
			GetBestFit( kSmoothness ) ;
			GetBestFit( kSecurity ) ;

			m_myView->Invalidate();
			m_myView->UpdateWindow(); 

			dst_time = src_time = GetTickCount();
			do{
				dst_time = GetTickCount();
			}while((dst_time-src_time) <= run_speed );

		}

		iLength++;
		if( iLength == population_size / 10/*8*/ ) //5 8
			iLength = 0;

		if ( cur_gen == total_gen ){

			DeleteNotPass() ;
			m_myView->Invalidate();
			m_myView->UpdateWindow(); 
			break ;
		}

	}

}
