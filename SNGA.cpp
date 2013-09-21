
#include "stdafx.h"

#include "Global.h"
#include "Recomb.h"
#include "Common.h"

#include "SNGAInd.h"
#include "SNGA.h"



CSNGA::CSNGA()
{
	init_parameter() ;
	m_is_run = false ;
	cur_gen = 0 ;
	evaluations = 0 ;
}

CSNGA::~CSNGA(){

}


void CSNGA::init_parameter() 
{
	//InitChart() ;

	population_size = cur_parameter.pSize ;
	total_gen = cur_parameter.T ;
	cross_rate = cur_parameter.propC ;
	mutation_rate = cur_parameter.propM ;

}



bool CSNGA::update_extreme_point(CSNGAInd& ind)        //uptate the extreme point , that's f up/down尖 
{
	bool bAnchorUpdated = false;
	bool bTrueNadirUpdated = false;

	for( int j=0;j<N_OBJ ;j++)
	{
		if ( ind.y_obj[j] < anchor_point[j][j] || ( ind.y_obj[j] == anchor_point[j][j] && ind.y_obj[(j+1)%2] < anchor_point[j][(j+1)%2] ))
		{
			bAnchorUpdated = true;
			anchor_point[j] = ind.y_obj;
		}
	}

	for( int j = 0 ; j < N_OBJ ; j++ )
	{
		if( ind.y_obj[j] >true_nadir_point[j] ){

			bTrueNadirUpdated = true;
			true_nadir_point[j] = ind.y_obj[j];
		}
	}

	if ( bAnchorUpdated )
	{
		for( int j = 0 ; j < N_OBJ ; j++ ){

			observer_point[j] = anchor_point[j][j];
			//PseudoNadirPoint[j] = anchor_point[(j+1)%2][j];
		}
	}

	if( bAnchorUpdated || bTrueNadirUpdated){

		for( int j = 0 ; j < N_OBJ ; j++)	{

			//if(PseudoNadirPoint[j]-observer_point[j]>0)
			//	reference_point[j] = ( true_nadir_point[j] + 1e3 * (true_nadir_point[j] - observer_point[j]) ) / (PseudoNadirPoint[j]-observer_point[j]);
			//else

			reference_point[j] = ( true_nadir_point[j] + 1e3 * (true_nadir_point[j] - observer_point[j]) );
		}
	}

	return bAnchorUpdated;
}

void CSNGA::init_population()
{
	vector <bool>  sectorpopFlag;

	vector <CSNGAInd>  initial_indi_pop;
	CSNGAInd rest_ind;
	CSNGAInd ind;

	for(int n=0;n<population_size;n++) {//统计极值点or观测点  初始个体数目设置//初始化种群所有个体，每个个体是ind，种群是initial_indi_pop
	
		sectorpopFlag.push_back(false);
		ind.rnd_init();   //inivitalize the variable and objective function (PS&PF)
		ind.obj_eval(n) ;
		initial_indi_pop.push_back(ind);//insert_sectorialgrid(ind, n);

	}

	//initial_observation_and_reference_point(initial_indi_pop[0]);

	for( int j=0;j<N_OBJ ;j++){
	
		anchor_point.push_back( initial_indi_pop[0].y_obj );//observer_point.push_back( initial_indi_pop[0].y_obj[j] );
		true_nadir_point.push_back( initial_indi_pop[0].y_obj[j] );//reference_point.push_back( initial_indi_pop[0].y_obj[j] );
	}
	for( int j=0;j<N_OBJ ;j++){
	
		observer_point.push_back( anchor_point[j][j] );
		//PseudoNadirPoint.push_back( anchor_point[(j+1)%2][j] );
	}
	for(int j=0;j<N_OBJ ;j++){
	
		reference_point.push_back( true_nadir_point[j] + 1e3 * (true_nadir_point[j] - observer_point[j]) ) ;
	}

	for(int n=1;n<population_size;n++){//统计极值点or观测点  初始个体数目设置//uptate the extreme point , that's f up/down尖//update 锚点、最小点、观察点、参考点
	
		update_extreme_point(initial_indi_pop[n]);
	}

	for(int n=0;n<population_size;n++) { //统计极值点or观测点  初始个体数目设置
	
		sector_population.push_back(initial_indi_pop[0]);
	}

	vector <CSNGAInd>  initial_rest_indi_pop;
	for(int n=0;n< population_size;n++) { //计算角度 第一轮按角度填充个体 并收集剩余个体
	
		//initial_indi_pop[n].yobj2angle(observer_point);
		initial_indi_pop[n].obj2angle_index( /*PseudoNadirPoint, */observer_point, population_size);

		if (sectorpopFlag[initial_indi_pop[n].sectorialindex] == false) {//if (sectorialgrid[initial_indi_pop[n].sectorialindex] == -1)
		
			sector_population[initial_indi_pop[n].sectorialindex] = initial_indi_pop[n];
			sectorpopFlag[initial_indi_pop[n].sectorialindex] = true;

		}else{

			bool bReplaced; 
			rest_ind = sector_population[initial_indi_pop[n].sectorialindex];//.indiv;
			bReplaced = Pareto_HyperVolume_compare_sectorialgrid(initial_indi_pop[n]);

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

			min_angle_difference = abs( initial_rest_indi_pop[0].sectorialangle - (double)n / (double)(population_size-1) );
			//min_angle_difference = abs( initial_rest_indi_pop[0].sectorialindex - n );
			min_angle_difference_ind = 0 ;
			for(int j = 1; j<rest_size; j++ )
			{
				angle_difference = abs( initial_rest_indi_pop[j].sectorialangle - (double)n / (double)(population_size-1) );
				//angle_difference = abs( initial_rest_indi_pop[j].sectorialindex - n );
				if(angle_difference < min_angle_difference)
				{
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
	//			//min_angle_difference = abs( initial_rest_indi_pop[0].sectorialangle - (double)n / (double)(population_size-1) );
	//			//min_angle_difference_ind = 0 ;
	//			for(int j = 0; j<rest_size; j++ )
	//			{
	//				angle_difference = abs( initial_rest_indi_pop[j].sectorialangle - (double)n / (double)(population_size-1) );
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
	evaluation() ;
}

bool CSNGA::Pareto_HyperVolume_compare_sectorialgrid(CSNGAInd& ind)
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

		if( contributing1 < contributing2){

			sector_population[ind.sectorialindex] = ind;//replace
			bReplaced = true;
		}
	
	}else{

		sector_population[ind.sectorialindex] = ind;//replace
		bReplaced = true;

	}

	return bReplaced;
}

double CSNGA::get_fastigiate_hypervolume(CSNGAInd&  ind, int ind_index, vector <double> &reference_point)
{
	double fastigiate_volume;

	double normalizedf[N_OBJ];
	for( int j = 0 ; j < N_OBJ ; j++ ){

		normalizedf[j] = ind.y_obj[j] - observer_point[j] ;
	}

	if(ind_index==0){

			fastigiate_volume = 
				0.5*(ind_index+0.5)/(population_size-ind_index-1.5)
				*normalizedf[1]*normalizedf[1] 
				+ (reference_point[1]-normalizedf[1])*normalizedf[0];

	}else if(ind_index==population_size-1){

		fastigiate_volume = 
			0.5*(population_size-ind_index-0.5)/(ind_index-0.5)
			*normalizedf[0]*normalizedf[0] 
			+ (reference_point[0]-normalizedf[0])*normalizedf[1];
	}else{

		fastigiate_volume = 
			0.5*(population_size-ind_index-0.5)/(ind_index-0.5)
			*normalizedf[0]*normalizedf[0] 
			+ 0.5*(ind_index+0.5)/(population_size-ind_index-1.5)
			*normalizedf[1]*normalizedf[1] - normalizedf[0]*normalizedf[1];
	}

	return fastigiate_volume;
}

//double CSNGA::get_fastigiate_hypervolume(CSNGAInd&  ind, int ind_index, vector <double> &reference_point)
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


double CSNGA::compute_hypervolume(vector <CSNGAInd>&  mysectorpop, int mypopsize, int mynobj)
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

	quicksort_increasing(paretofront, paretofront.size());
	double front2Dhv = hv2(paretofront);

	front2Dhv = front2Dhv / ((xHypervolumeRefPoint_TopRight - xHypervolumeRefPoint_BottomLeft)*(yHypervolumeRefPoint_TopRight - yHypervolumeRefPoint_BottomLeft));

	//if(front2Dhv>1)
	//	front2Dhv = front2Dhv;

	return front2Dhv;
}

double  CSNGA::tour_selection_hv_difference(int p, vector<CSNGAInd>& mypopulation)
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

int  CSNGA::tour_selection_hv2(vector <CSNGAInd>  &mypopulation)
{
	int p1 = int(rnd_uni(&rnd_uni_init)*mypopulation.size());
	int p2 = int(rnd_uni(&rnd_uni_init)*mypopulation.size());

	double hv1 = tour_selection_hv_difference( p1, mypopulation );
	double hv3 = tour_selection_hv_difference( p2, mypopulation );

	if(hv1 >= hv3 )
		return p1;
	else
		return p2;
}

void save_pop_front(vector <vector<double>>  &pop_front, char saveFilename[1024]);

//void CSNGA::uniform_selection(CSNGAInd*& ind_selected)
//{
//	int pop_index;
//
//	pop_index = int(rnd_uni(&rnd_uni_init)*( population_size ));
//
//	ind_selected = &(sector_population[pop_index]);//.indiv
//}


void CSNGA::reset_angle()
{
	int n;
	for(n=0; n<population_size; n++) {

		//sector_population[n].yobj2angle(observer_point);
		sector_population[n].obj2angle_index(/*PseudoNadirPoint, */observer_point, population_size);
	}
}

//void CSNGA::save_population(vector <CSNGAInd>  &mypopulation, char saveFilename[1024])
//{
//	std::fstream fout;
//	fout.open(saveFilename,std::ios::out);
//	for(int n=0; n<mypopulation.size(); n++)
//	{
//		for(int k=0;k<N_OBJ;k++)
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
//		for(int k=0;k<N_OBJ;k++)
//		{
//			fout<<pop_front[n][k]<<"  ";
//		}
//		fout<<"\n";
//	}
//	fout.close();
//}
//
//void CSNGA::save_ps(char saveFilename[1024])
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

//void CSNGA::population2front(vector <CSNGAInd>  &mypopulation, vector <CSNGAInd>  &population_front)
void CSNGA::population2front(vector <CSNGAInd>  &mypopulation, vector <vector<double>>  &population_front)
{
	vector<int> nDominated;
	for(int n=0;n<mypopulation.size();n++)
		nDominated.push_back(0);


	for(int k=0; k<mypopulation.size(); k++){

		for(int j=k+1; j<mypopulation.size(); j++)
		{
			TCompare tresult = mypopulation[k].Compare(mypopulation[j]);
			if(tresult == _Pareto_Dominated)
				nDominated[k]++;
			else if(tresult == _Pareto_Dominating)
				nDominated[j]++;
		}
	}

		for(int n=0;n<mypopulation.size();n++)
			if(nDominated[n] == 0)
				population_front.push_back(mypopulation[n].y_obj);

		nDominated.clear();
}

////////////////////////////////////////////////////////////////////////////
void CSNGA::get_best_obj( const int& i_obj, vector<int> &path ) 
{
	double cur_best_obj_value = sector_population[0].y_obj[ i_obj ] ;
	int cur_best_obj_index = 0 ;

	for( int i = 1 ; i < population_size; ++i ) {

		if ( i_obj == SECURITY ) { //# Security: the greater the better

			if ( /*sector_population[i].y_obj[ i_obj ] > 0 &&*/ sector_population[i].y_obj[ i_obj ] > cur_best_obj_value ) {

				cur_best_obj_value = sector_population[i].y_obj[ i_obj ] ;
				cur_best_obj_index = i ;
				break;
			}

		}else{ //# Length and Smoothness

			if ( /*sector_population[i].y_obj[ i_obj ] < 1 &&*/ sector_population[i].y_obj[ i_obj ] < cur_best_obj_value ) {

				cur_best_obj_value = sector_population[i].y_obj[ i_obj ] ;
				cur_best_obj_index = i ;
				break;
			}
		}
	}

	path = sector_population[cur_best_obj_index].x_var ;
}

//
//double CSNGA::calc_distance()    //calculate the average distance between ps and current solutions
//{
//	double distance = 0;
//	for(int i=0; i<ps.size(); i++)
//	{
//		double min_d = 1.0e+10;
//		for(int j=0; j<population_size; j++)
//		{
//			double d = dist_vector(ps[i].y_obj, sector_population[j].y_obj);
//			if(d<min_d)  min_d = d;
//		}
//		distance+= min_d;
//	}
//	distance/=ps.size();
//
//	return distance;
//}


void CSNGA::evaluation() 
{
	for ( int i = 0 ; i < sector_population.size() ; i++ ) {

		sector_population[i].obj_eval(i) ;
	}
}


void CSNGA::gen_pop_del_same() 
{
	for( int i = 0; i < population_size ; i++ ){

		gen_delete_same( sector_population[i].x_var ) ;
	}
}

void CSNGA::gen_delete_same( vector<int>& x_path )
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
void CSNGA::delete_not_pass() 
{
	for( vector<CSNGAInd>::iterator index = sector_population.begin() ;
		index != sector_population.end() ; index++ ) {
	
			if ( !index->is_pass ){

				index->x_var.clear() ;
			}
		
	}
}
////////////////////////////////////////////////////////////////


void CSNGA::execute( CRobotView* m_myView )
{
	////seed = (seed + 23)%1377;					
	////rnd_uni_init = -(long)seed;	
	//vector<double> gd;

	//char filename[1024];   
	//cout<<"fuck"<<endl;
	//sprintf(filename,"POF0/%s.dat",strTestInstance);
	//loadpfront(filename,ps);     //read the data to ps
	//nfes    = 0; 	 

	//double totaltimebegin = clock();

	// first generation
	//int cur_generation   = 1;

	evaluations = population_size ;
	init_population() ;                              //initival the population
	double hypervolume = compute_hypervolume(sector_population, population_size, N_OBJ);//cout<<"evaluations = "<<evaluations<<"  hypervolume = "<<hypervolume<<"  "<<endl;
	hvl.push_back(0) ; 
	hvl.push_back(hypervolume) ; 

	get_best_obj( LENGTH, path_length ) ;
	get_best_obj( SMOOTHNESS, path_smoothness ) ;
	get_best_obj( SECURITY, path_security ) ;
				
	//cur_gen = 1 ;
	m_is_run = true ;

	m_myView->Invalidate();
	m_myView->UpdateWindow(); 

	//gd.push_back(0);  gd.push_back(distance);

	//double distance = calc_distance();  //the distance between ps and solutions
	//igd.push_back(0); 
	//igd.push_back(distance);

	//cout<<"evaluations = "<<evaluations<<"  hypervolume = "<<hypervolume<<"  "<<"  IGD = "<<distance<<"  "<<endl;

	int iLength = 0;
	//int iGen_Utility = 0;

	evaluation() ;

	CSNGAInd child2;
	while( evaluations <= ( total_gen*population_size ) ) {  //total_gen =250: 250 generation
	
		int parent_index1, parent_index2;

		if( iLength  == 0 )
			parent_index1 = 0;
		else if( iLength == 1 )
			parent_index1 = population_size - 1;
		else 
			parent_index1 = tour_selection_hv2( sector_population );//int(rnd_uni(&rnd_uni_init)*( population_size )); ////int(rnd_uni(&rnd_uni_init)*( population_size )); //
		//ind_selected = &(sector_population[parent_index1]);

		parent_index2 =  tour_selection_hv2( sector_population );

		//int(rnd_uni(&rnd_uni_init)*( population_size ));// //int(rnd_uni(&rnd_uni_init)*( population_size )); //
		//ind_selected2 = &(sector_population[parent_index2]);
		//int parent_index3 =  tour_selection_hv2( sector_population );
		//sbx_mutation_evolution(*ind_selected, *ind_selected2);
		//if(rnd_uni(&rnd_uni_init)<0.9)

		/////////////////////////////////////////////////////////////////////////////////
		//##Modified on 2013/9/9///////////////////////////
		////////////////////////////////////////////////////////////
		//real_sbx_xoverA(sector_population[parent_index1], sector_population[parent_index2], onechild, child2);
		//realmutation(onechild, 1.0/nvar);


		/////////////////////////////////////////////////////////////////
		//diff_evo_xoverB(sector_population[parent_index1], sector_population[parent_index2], sector_population[parent_index3], onechild, 0.5, 1.0);

		///////////////////////////////////////////////////////////////////
		//##Modified on 2013/9/17 ///////////////////////////////////


		//onechild.obj_eval(0) ;

		gen_crossover( sector_population[parent_index1], sector_population[parent_index2], onechild, child2 ) ;
		gen_mutation( onechild ) ;

		//##!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		onechild.obj_eval(0) ;
	
		//nfes++;
		evaluations++;

		bool updated = update_extreme_point(onechild);//update_anglerange_maxminindi(onechild, true); //update_nondominated_obj_maxmin(onechild);//update_angle_maxmin(onechild);//
		
		if(updated)	{

			reset_angle();
		}

		//onechild.yobj2angle(observer_point);
		onechild.obj2angle_index( observer_point, population_size);//之前之所以出现奇异点 是因为计算onechild角度是在统计极端点前 极端点改变了 而onechild的角度是老的

		//update_sectorialgrid(onechild);
		Pareto_HyperVolume_compare_sectorialgrid(onechild);


		int dd = int(total_gen*population_size/25.0);

		if( evaluations%dd == 0 ){

			hypervolume = compute_hypervolume(sector_population, population_size, N_OBJ);//cout<<"evaluations = "<<evaluations<<"  hypervolume = "<<hypervolume<<"  "<<endl;
			hvl.push_back(int(1.0*evaluations/dd));
			hvl.push_back(hypervolume);

			/*distance = calc_distance();
			igd.push_back(int(1.0*evaluations/dd)); 
			igd.push_back(distance);
*/
//			cout<<"evaluations = "<<evaluations<<"  hypervolume = "<<hypervolume<<"  "<<"  IGD = "<<distance<<"  "<<endl;

			//double endtime = clock();
			//unevolvetime += endtime - begintime;
		}  

		int tmp = cur_gen ;
		cur_gen = evaluations / population_size + 1 ;

		if ( cur_gen != tmp ) {

			//gen_pop_del_same() ;

			get_best_obj( LENGTH, path_length ) ;
			get_best_obj( SMOOTHNESS, path_smoothness ) ;
			get_best_obj( SECURITY, path_security ) ;

			m_myView->Invalidate();
			m_myView->UpdateWindow(); 

		}

		iLength++;
		if( iLength == population_size / 10/*8*/ ) { //5 8

			iLength = 0;
		}


	}

	delete_not_pass() ;
	m_myView->Invalidate();
	m_myView->UpdateWindow(); 

	///////////////////////////////////////////////
	//#Modified on 2013/9/17 ////////////////
	m_is_run = false ;

	sector_population.clear();
}
