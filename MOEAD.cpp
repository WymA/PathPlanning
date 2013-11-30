#include "stdafx.h"
#include "MOEAD.h"
//#include "ScalarFunc.h"
#include "Chart.h"
#include "Recomb.h"


MOEAD::MOEAD( )
{
	reference_point = NULL ;
	cur_generation = 0 ;
}

MOEAD::~MOEAD()
{
	idealpoint.clear();//delete [] idealpoint;  
	population.clear();
	if ( reference_point ) 
		delete [] reference_point;
}


void MOEAD::InitPopulation()
{
	for(int i=0; i<population_size; i++){

		population[i].indiv.RandomInit();
		population[i].indiv.ObjEvaluate(i);

		UpdateReference(population[i].indiv);
	}
}


// initialize a set of evenly-distributed weight vectors
void MOEAD::InitUniformWeight()
{   
	reference_point = new CIndividual[kObjNum];    
	// initialize ideal point	
	for( int n = 0 ; n < kObjNum ; n++) {

		idealpoint.push_back( 1.0e+30 );//idealpoint[n] = 1.0e+30;  
		reference_point[n].RandomInit( );
		reference_point[n].ObjEvaluate(kNotIndex);
	}

	for( int i = 0 ; i < population_size ; i++ ){

		TSOP sop;		   
		sop.array.push_back(i);
		sop.array.push_back(sd - i );

		for( int k = 0 ; k < sop.array.size() ; k++ )
			sop.lambda.push_back( 1.0*sop.array[k] / sd );

		population.push_back(sop); 	
	}
	population_size = population.size();

}

// initialize the neighborhood of subproblems based on the distances of weight vectors
void MOEAD::InitNeighborhood()
{
	double *x   = new double[population_size];
	int    *idx = new int[population_size];
	for( int i = 0 ; i < population_size ; i++ ){	

		for( int j = 0 ; j < population_size ; j++ )	{

			x[j]    = DistVector(population[i].lambda,population[j].lambda);
			idx[j]  = j;			
		}

		minfastsort( x, idx, population_size, neighborhood_size );  

		for( int k = 0 ; k < neighborhood_size ; k++ )   
			population[i].table.push_back( idx[k] );

	}
	delete [] x;
	delete [] idx;
}

// update the best solutions of neighboring subproblems
void MOEAD::UpdateProblem(CIndividual &indiv, int id)
{
	for( int i = 0 ; i < neighborhood_size ; i++ ){

		int k  = population[id].table[i];
		double f1, f2 ;

		f1 = Scalarizing(population[k].indiv.y_obj, population[k].lambda, reference_point) ;
		f2 = Scalarizing(indiv.y_obj, population[k].lambda, reference_point) ;
		
		if( f2 < f1 ) population[k].indiv = indiv ;
	}
}


// update the reference point
void MOEAD::UpdateReference(CIndividual &ind)
{
	for( int n = 0; n < kObjNum ; n++ ) {

		if(ind.y_obj[n] < idealpoint[n]){

			idealpoint[n]  = ind.y_obj[n];
			reference_point[n]  = ind;
		}		
	}
}

double MOEAD::compute_hypervolume(vector <TSOP>&  mypop, int mypopsize, int mynobj)
{
	//vector <vector<double>> paretofront;

	//vector<double> onepoint;
	//onepoint.resize(2);
	//for( int i=0;i<mypopsize;i++){

	//	bool Dominated = false;
	//	for( int j = 0 ; j < mypopsize ; j++ ) {

	//		if( j != i && ParetoCompare( mypop[i].indiv.y_obj, mypop[j].indiv.y_obj) == _Pareto_Dominated ){

	//			Dominated = true;
	//			break;
	//		}
	//	}
	//	if( !Dominated &&
	//		mypop[i].indiv.y_obj[0] <= xHypervolumeRefPoint_TopRight &&
	//		mypop[i].indiv.y_obj[1] <= yHypervolumeRefPoint_TopRight
	//		){

	//		onepoint[0] = mypop[i].indiv.y_obj[obj1];
	//		onepoint[1] = mypop[i].indiv.y_obj[obj2];
	//		paretofront.push_back(onepoint);
	//	}
	//}

	//QuickSortIncreasing(paretofront, paretofront.size());
	//double front2Dhv = hv2(paretofront);

	//front2Dhv = front2Dhv / ((xHypervolumeRefPoint_TopRight - xHypervolumeRefPoint_BottomLeft)*(yHypervolumeRefPoint_TopRight - yHypervolumeRefPoint_BottomLeft));

	//return front2Dhv ;
	return 0.0 ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MOEAD::Population2Front()
{
	vector<int> nDominated;
	for(int n=0;n<population.size();n++)
		nDominated.push_back(0);

	for(int k=0; k<population.size(); k++){
		for(int j=k+1; j<population.size(); j++){

			//TCompare tresult = ParetoCompare(population[k].indiv.y_obj, population[j].indiv.y_obj);
			TCompare tresult = ParetoCompare( total_fitness[k], total_fitness[j]);

			if(tresult == _Pareto_Dominated)
				nDominated[k]++;
			else if(tresult == _Pareto_Dominating)
				nDominated[j]++;
		}
	}
		
	dominating_fitness.clear() ;
	dominated_fitness.clear() ;

	for(int n=0;n<nDominated.size();n++){

		if(nDominated[n] == 0)
			dominating_fitness.push_back( total_fitness[n] ) ;
		else
			dominated_fitness.push_back( total_fitness[n] ) ;	
	}

	nDominated.clear();
}

//void save_population(vector <CIndividual>  &mypopulation, char saveFilename[1024])
//{
//	std::fstream fout;
//	fout.open(saveFilename,std::ios::out);
//	for(int n=0; n<mypopulation.size(); n++){
//
//		for(int k=0;k<kObjNum;k++){
//
//			fout<<mypopulation[n].y_obj[k]<<"  ";
//		}
//		fout<<"\n";
//	}
//	fout.close();
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// recombination, mutation, update in MOEA/D
void MOEAD::Evolution()
{
	CIndividual child, child2;
	GenInsert() ;
	for( int n = 0 ; n < population.size() ; n++){

		int   s  = population[n].table.size();

		int   r1 = rand()%s ;
		int   r2 = rand()%s ;

		int   p1 = population[n].table[r1];
		int   p2 = population[n].table[r2];

		//realbinarycrossover(population[p1].indiv,population[p2].indiv,child, child2);
		//diff_evo_xoverB( population[i].indiv, population[p1].indiv, population[p2].indiv, child, 0.5, 1.0);
		//real_sbx_xoverA( population[p1].indiv, population[p2].indiv, child, child2 );
		
		
		//realmutation( child, 1.0/nvar );

		GenCrossover( population[p1].indiv, population[p2].indiv, child, child2 ) ;
		GenMutation( child ) ;
		GenDeleteSame( population[n].indiv.x_var ) ;
		GenDeleteSame( child.x_var ) ;

		child.ObjEvaluate(n );
		UpdateReference(child);
		UpdateProblem(child, n);
	}
}

//
//void MOEAD::save_front(char saveFilename[1024])
//{
//	std::fstream fout;
//	fout.open( saveFilename,std::ios::out );
//	for(int n=0; n<population.size(); n++){
//
//		for(int k=0;k<kObjNum;k++)
//			fout<<population[n].indiv.y_obj[k]<<"  ";
//		fout<<"\n";
//	}
//	fout.close();
//}
//
//void MOEAD::save_pos(char saveFilename[1024])
//{
//	std::fstream fout;
//	fout.open(saveFilename,std::ios::out);
//	for(int n=0; n<population.size(); n++){
//
//		for( int k = 0 ; k < nvar ; k++ )
//			fout<<population[n].indiv.x_var[k]<<"  ";
//
//		fout<<"\n";
//	}
//	fout.close();
//}


void MOEAD::operator=(const MOEAD &emo)
{
	population_size        = emo.population_size;
	population  = emo.population;
	reference_point  = emo.reference_point;
	neighborhood_size       = emo.neighborhood_size;
} 


double MOEAD::NormVector(vector <double> &x)
{
	double sum = 0;
	for(int i=0;i<x.size();i++)
		sum = sum + x[i]*x[i];
	return sqrt(sum);
}

double MOEAD::InnerProduct(vector <double>&vec1, vector <double>&vec2)
{
	double sum = 0;
	for(int i=0; i<vec1.size(); i++)
		sum+= vec1[i]*vec2[i];
	return sum;
}

// Scalarizing functions for decomposition methods
double MOEAD::Scalarizing(vector <double> &y_obj, vector <double> &lambda, CIndividual* nbi_node)
{
	double max_fun = -1.0e+30 ;

	double diff = fabs( y_obj[obj1] - idealpoint[obj1] );
	double feval;
	if( lambda[0] == 0 ) 
		feval = 0.00001*diff;
	else
		feval = diff*lambda[0];
	if( feval > max_fun ) 
		max_fun = feval;

	diff = fabs( y_obj[obj2] - idealpoint[obj2] );
	if( lambda[1] == 0 ) 
		feval = 0.00001*diff;
	else
		feval = diff*lambda[1];
	if( feval > max_fun ) 
		max_fun = feval;


	return max_fun ;
}


////////////////////////////////////////////////////////////////////////////
void MOEAD::GetBestObj( int i_obj, vector<int> &path ) 
{
	double cur_best_obj_value = population[0].indiv.y_obj[ i_obj ] ;
	int cur_best_obj_index = 0 ;

	for( int i = 1 ; i < population_size; ++i ) {

		if ( population[i].indiv.y_obj[ i_obj ] < cur_best_obj_value ) {

			cur_best_obj_value = population[i].indiv.y_obj[ i_obj ] ;
			cur_best_obj_index = i ;
		}
	}

	path = population[cur_best_obj_index].indiv.x_var ;

}



void MOEAD::InitParameter() 
{
	//init_chart() ;

	population_size = cur_parameter.pSize ;
	total_gen = cur_parameter.total_gen ;
	cross_rate = cur_parameter.pop_cross_rate ;
	mutation_rate = cur_parameter.pop_mutation_rate ;
	neighborhood_size = kNeighborhoodSize ;
	sd = population_size - 1 ;

}

void MOEAD::GenPopDelSame() 
{
	for( int i = 0; i < population_size ; i++ ){

		GenDeleteSame( population[i].indiv.x_var ) ;
	}
}

void MOEAD::GenDeleteSame( vector<int>& x_path )
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

int MOEAD::GenInsertInd( CIndividual& ind, const int& idx )
{
	//如果和前一位连续则不插，插入中值法计算出的点
	int bSeries = is_series(ind, idx);
	if( bSeries == 0 )	{

		int pre = ind.x_var[idx-1];
		int cur = ind.x_var[idx];
		int x0, y0, x1, y1, x2, y2;
		int x3= 0, y3 = 0;//真正要插入的序号
		x1 = pre%chart_width;
		y1 = pre/chart_width;
		x2 = cur%chart_width;
		y2 = cur/chart_width;
		x0 = (x1 + x2)/2;

		if(abs(y2 - y1) == 1)
			y0 = (y1 + y2 + 1) / 2;
		else
			y0 = (y1 + y2) / 2;

		//非障碍物点，可以插入
		if(chart[y0*chart_width+x0] == 0){

			x3 = x0;
			y3 = y0;
		}else{//障碍物点，找到离该点最近的非障碍物点插入

			int ySpan = max(y0, (chart_height - 1 - y0));
			double preMinDist, curMinDist;
			//将最短距离设置为一个大的初值
			preMinDist = curMinDist = kMaxDist;
			//搜索当前行
			curMinDist = SearchRowNearest(x0, y0, y0, preMinDist, x3);
			//在当前行中找到点

			if(curMinDist < kMaxDist){

				y3 = y0;
			} else {

				//分别搜索上面的行和下面的行
				for(int j = 0; j <= ySpan; ++j)
				{
					if((y0 - j) >= 0)
					{
						curMinDist = SearchRowNearest(x0, y0, y0 - j, preMinDist, x3);
						if(curMinDist < preMinDist)
						{
							y3 = y0;
							preMinDist = curMinDist;
						}
					}
					if((y0 + j) < chart_height)
					{
						curMinDist = SearchRowNearest(x0, y0, y0 + j, preMinDist, x3);
						if(curMinDist < preMinDist)
						{
							y3 = y0;
							preMinDist = curMinDist;
						}
					}

					if(curMinDist < kMaxDist)
						break;

				}
			}
		}

		int ins = x3 + chart_width*y3;
		//有效的插入点

		if( ins != 0 && ins != ind.x_var[idx]){

			intVec::iterator iter = ind.x_var.begin() + idx;
			ind.x_var.insert(iter, ins);
			return 1;
		}
	}

	return 0;
}

void MOEAD::GenInsert() 
{
	for(int i = 0; i < population_size ; ++i ){

		int len = population[i].indiv.x_var.size();
		int insertResult ;
		for( int j = 1; j < len; ){

			insertResult = GenInsertInd( population[i].indiv, j ) ;

			if(insertResult == 0)
				j += 1;
			else
				j += 2;

			len = population[i].indiv.x_var.size();

		}
	}
}

////////////////////////////////////////////////////////////////////////////

void  MOEAD::Execute( CRobotView* m_myView )
{
	InitParameter() ;
	InitUniformWeight();
	InitNeighborhood();
	InitPopulation();	


	int dst_time, src_time;
	//flush_fitness() ;

	this->cur_generation = 1;

	//double hypervolume = compute_hypervolume(population, population_size, kObjNum);////calc_distance();
	//hvl.push_back(0);
	//hvl.push_back(hypervolume);

	Population2Front() ;

	GetBestObj( kLength, path_length ) ;
	GetBestObj( kSmoothness, path_smoothness ) ;
	GetBestObj( kSecurity, path_security ) ;

	GetBestFit( kLength ) ;
	GetBestFit( kSmoothness ) ;
	GetBestFit( kSecurity ) ;
				
	m_myView->Invalidate();
	m_myView->UpdateWindow(); 

	for( this->cur_generation=2; run_state != kStop && cur_generation < total_gen ; cur_generation++ ) {

		Evolution();

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
		}while((dst_time-src_time) <= run_speed);

	}
	m_myView->Invalidate();
	m_myView->UpdateWindow(); 
}
