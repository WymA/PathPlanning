#include "stdafx.h"
#include "MOEAD.h"
//#include "ScalarFunc.h"
#include "Chart.h"
#include "Recomb.h"


TMOEAD::TMOEAD( int n_objective )
{
	nobj = n_objective ;
	m_is_run = false ;
	//idealpoint = new double[nobj];
	reference_point = new CIndividual[nobj];    
	// initialize ideal point	
	for( int n = 0 ; n < nobj ; n++) {

		idealpoint.push_back( 1.0e+30 );//idealpoint[n] = 1.0e+30;  
		reference_point[n].rnd_init( );
		reference_point[n].obj_eval( n );
	}
}

TMOEAD::~TMOEAD()
{
	idealpoint.clear();//delete [] idealpoint;  
	population.clear();
	delete [] reference_point;
}


void TMOEAD::init_population()
{
	for(int i=0; i<population_size; i++){

		population[i].indiv.rnd_init();
		population[i].indiv.obj_eval(i);

		update_reference(population[i].indiv);
	}
}


// initialize a set of evenly-distributed weight vectors
void TMOEAD::init_uniformweight()
{   
	for( int i = 0 ; i <= sd ; i++ ){

		if( nobj == 2 ){

			TSOP sop;		   
			sop.array.push_back(i);
			sop.array.push_back( sd - i ) ;

			for( int j = 0 ; j < sop.array.size() ; j++ )
				sop.namda.push_back( 1.0*sop.array[j] / sd ) ;

			population.push_back(sop); 

		} else {

			for( int j = 0 ; j <= sd ; j++ ){

				if( i + j <= sd ){

					TSOP sop;		   
					sop.array.push_back(i);
					sop.array.push_back(j);
					sop.array.push_back(sd - i - j);

					for( int k = 0 ; k < sop.array.size() ; k++ )
						sop.namda.push_back( 1.0*sop.array[k] / sd );

					population.push_back(sop); 
				}

			}
		}
	}
	population_size = population.size();

}

// initialize the neighborhood of subproblems based on the distances of weight vectors
void TMOEAD::init_neighbourhood()
{
	double *x   = new double[population_size];
	int    *idx = new int[population_size];
	for( int i = 0 ; i < population_size ; i++ ){	

		for( int j = 0 ; j < population_size ; j++ )	{

			x[j]    = dist_vector(population[i].namda,population[j].namda);
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
void TMOEAD::update_problem(CIndividual &indiv, int id)
{
	for( int i = 0 ; i < neighborhood_size ; i++ ){

		int k  = population[id].table[i];
		double f1, f2 ;

		f1 = scalarizing(population[k].indiv.y_obj, population[k].namda, reference_point) ;
		f2 = scalarizing(indiv.y_obj, population[k].namda, reference_point) ;
		
		if( f2 < f1 ) population[k].indiv = indiv ;
	}
}


// update the reference point
void TMOEAD::update_reference(CIndividual &ind)
{
	for( int n = 0; n < nobj ; n++ ) {

		if(ind.y_obj[n] < idealpoint[n]){

			idealpoint[n]  = ind.y_obj[n];
			reference_point[n]  = ind;
		}		
	}
}

double TMOEAD::compute_hypervolume(vector <TSOP>&  mypop, int mypopsize, int mynobj)
{
	vector <vector<double>> paretofront;

	vector<double> onepoint;
	onepoint.resize(2);
	for( int i=0;i<mypopsize;i++){

		bool Dominated = false;
		for( int j = 0 ; j < mypopsize ; j++ ) {

			if( j != i && ParetoCompare( mypop[i].indiv.y_obj, mypop[j].indiv.y_obj) == _Pareto_Dominated ){

				Dominated = true;
				break;
			}
		}
		if( !Dominated &&
			mypop[i].indiv.y_obj[0] <= xHypervolumeRefPoint_TopRight &&
			mypop[i].indiv.y_obj[1] <= yHypervolumeRefPoint_TopRight
			){

			onepoint[0] = mypop[i].indiv.y_obj[0];
			onepoint[1] = mypop[i].indiv.y_obj[1];
			paretofront.push_back(onepoint);
		}
	}

	quicksort_increasing(paretofront, paretofront.size());
	double front2Dhv = hv2(paretofront);

	front2Dhv = front2Dhv / ((xHypervolumeRefPoint_TopRight - xHypervolumeRefPoint_BottomLeft)*(yHypervolumeRefPoint_TopRight - yHypervolumeRefPoint_BottomLeft));

	return front2Dhv;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void population2front(vector <TSOP>  &mypopulation, vector <CIndividual>  &population_front)
//{
//	vector<int> nDominated;
//	for(int n=0;n<mypopulation.size();n++)
//		nDominated.push_back(0);
//
//	for(int k=0; k<mypopulation.size(); k++)
//		for(int j=k+1; j<mypopulation.size(); j++){
//
//			TCompare tresult = ParetoCompare(mypopulation[k].indiv.y_obj, mypopulation[j].indiv.y_obj);
//			
//			if(tresult == _Pareto_Dominated)
//				nDominated[k]++;
//			else if(tresult == _Pareto_Dominating)
//				nDominated[j]++;
//		}
//
//		for(int n=0;n<mypopulation.size();n++)
//			if(nDominated[n] == 0)
//				population_front.push_back(mypopulation[n].indiv);
//
//		nDominated.clear();
//}

//void save_population(vector <CIndividual>  &mypopulation, char saveFilename[1024])
//{
//	std::fstream fout;
//	fout.open(saveFilename,std::ios::out);
//	for(int n=0; n<mypopulation.size(); n++){
//
//		for(int k=0;k<nobj;k++){
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
void TMOEAD::evolution()
{
	CIndividual child, child2;
	for( int i = 0 ; i < population.size() ; i++){

		int   n  =  i; 
		int   s  = population[n].table.size();
		int   r1 = int(s*rnd_uni(&rnd_uni_init));
		int   r2 = int(s*rnd_uni(&rnd_uni_init));
		int   p1 = population[n].table[r1];
		int   p2 = population[n].table[r2];

		//realbinarycrossover(population[p1].indiv,population[p2].indiv,child, child2);
		//diff_evo_xoverB( population[i].indiv, population[p1].indiv, population[p2].indiv, child, 0.5, 1.0);
		//real_sbx_xoverA( population[p1].indiv, population[p2].indiv, child, child2 );
		
		
		//realmutation( child, 1.0/nvar );

		gen_crossover( population[p1].indiv, population[p2].indiv, child, child2 ) ;
		gen_mutation( child ) ;
		gen_pop_del_same() ;


		child.obj_eval(n );
		update_reference(child);
		update_problem(child, n);
	}
}

//
//void TMOEAD::save_front(char saveFilename[1024])
//{
//	std::fstream fout;
//	fout.open( saveFilename,std::ios::out );
//	for(int n=0; n<population.size(); n++){
//
//		for(int k=0;k<nobj;k++)
//			fout<<population[n].indiv.y_obj[k]<<"  ";
//		fout<<"\n";
//	}
//	fout.close();
//}
//
//void TMOEAD::save_pos(char saveFilename[1024])
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


void TMOEAD::operator=(const TMOEAD &emo)
{
	population_size        = emo.population_size;
	population  = emo.population;
	reference_point  = emo.reference_point;
	neighborhood_size       = emo.neighborhood_size;
} 


double TMOEAD::norm_vector(vector <double> &x)
{
	double sum = 0;
	for(int i=0;i<x.size();i++)
		sum = sum + x[i]*x[i];
	return sqrt(sum);
}

double TMOEAD::innerproduct(vector <double>&vec1, vector <double>&vec2)
{
	double sum = 0;
	for(int i=0; i<vec1.size(); i++)
		sum+= vec1[i]*vec2[i];
	return sum;
}

// scalarizing functions for decomposition methods
double TMOEAD::scalarizing(vector <double> &y_obj, vector <double> &namda, CIndividual* nbi_node)
{

	double fvalue = 0;

	/////////////////////////////////////////////////////////////////////////////
	//## Default approach //////////////////////////////////////////////////
	// Tchebycheff approach
	if( tDecompType == _Decomposition_TCH1 )
	{
		double max_fun = -1.0e+30 ;
		for(int n=0; n<nobj; n++)
		{
			double diff = fabs( y_obj[n] - idealpoint[n] );
			double feval;
			if( namda[n] == 0 ) 
				feval = 0.00001*diff;
			else
				feval = diff*namda[n];
			if( feval > max_fun ) max_fun = feval;

		}

		fvalue = max_fun;
	}
	/////////////////////////////////////////////////////////////////////////////

	// normalized Tchebycheff approach
	if(tDecompType == _Decomposition_TCH2)
	{
		vector <double> scale;
		for(int i=0; i<nobj; i++)
		{
			double min = 1.0e+30, max = -1.0e+30;
			for(int j=0; j<nobj; j++)
			{
				double tp = nbi_node[j].y_obj[i];
				if(tp>max) max = tp;
				if(tp<min) min = tp;
			}
			scale.push_back(max-min);
			if(max-min==0) return 1.0e+30;
		}

		double max_fun = -1.0e+30;
		for(int n=0; n<nobj; n++)
		{
			double diff = (y_obj[n] - idealpoint[n])/scale[n];
			double feval;
			if(namda[n]==0) 
				feval = 0.0001*diff;
			else
				feval = diff*namda[n];
			if(feval>max_fun) max_fun = feval;

		}
		fvalue = max_fun;
	}


	//* Boundary intersection approach
	if(tDecompType == _Decomposition_PBI)
	{

		// normalize the weight vector (line segment)
		double nd = norm_vector(namda);
		///for(int i=0; i<nobj; i++)
		///	namda[i] = namda[i]/nd;

		vector <double> realA(nobj);
		vector <double> realB(nobj);

		// difference beween current point and reference point
		for(int n=0; n<nobj; n++)
			realA[n] = (y_obj[n] - idealpoint[n]);

		// distance along the line segment
		double d1 = fabs(innerproduct(realA,namda)) / nd;

		// distance to the line segment
		for(int n=0; n<nobj; n++)
			realB[n] = (y_obj[n] - (idealpoint[n] + d1*namda[n]));
		double d2 = norm_vector(realB);

		fvalue = d1 + 5*d2;

	}

	return fvalue;
}


////////////////////////////////////////////////////////////////////////////
void TMOEAD::get_best_obj( int i_obj, vector<int> &path ) 
{
	double cur_best_obj_value ;
	int cur_best_obj_index = 0 ;

	for( int i = 0; i < population_size; ++i ) {

		if ( population[i].indiv.rank == 1 ) {

			cur_best_obj_value = population[i].indiv.y_obj[ i_obj ] ;
			cur_best_obj_index = i ;
			break;
		
		}
	}

	for( int i = cur_best_obj_index + 1; i < population_size; ++i) {

		if ( population[i].indiv.rank == 1 && population[i].indiv.y_obj[ i_obj ] < cur_best_obj_value ) {

			cur_best_obj_value = population[i].indiv.y_obj[i_obj] ;
			cur_best_obj_index = i ;
		}
	}

	path = population[cur_best_obj_index].indiv.x_var ;
}



void TMOEAD::init_parameter() 
{
	//InitChart() ;

	population_size = cur_parameter.pSize ;
	total_gen = cur_parameter.T ;
	cross_rate = cur_parameter.propC ;
	mutation_rate = cur_parameter.propM ;
	neighborhood_size = NEIGHBORHOOD_SIZE ;
	sd = population_size - 1 ;
	
}

void TMOEAD::gen_pop_del_same() 
{
	for( int i = 0; i < population_size ; i++ ){

		gen_delete_same( population[i].indiv.x_var ) ;
	}
}
void TMOEAD::gen_delete_same( vector<int>& x_path )
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

////////////////////////////////////////////////////////////////////////////

void  TMOEAD::execute( CRobotView* m_myView )
	//int sd, int nc, int mg, int irun, 
	//vector<double>& hvl,
	//double& totaltime, double &unevolvetime,
{
	// sd: the integer number of generating weight vectors
	// nc: the size of neighborhood
	// mg: the maximal number of generations 

	double totaltimebegin = clock();

	//neighborhood_size = nc;	
	init_parameter() ;
	init_uniformweight();
	init_neighbourhood();
	init_population();	

	//char filename[1024];
	//sprintf(filename,"POF0/%s.dat",strTestInstance);
	//loadpfront( filename, ps ) ;

	this->cur_generation = 1;

	double hypervolume = compute_hypervolume(population, population_size, nobj);////calc_distance();
	hvl.push_back(0);
	hvl.push_back(hypervolume);

	get_best_obj( 0, path_length ) ;
	get_best_obj( 1, path_smoothness ) ;
	get_best_obj( 2, path_security ) ;
				
	m_is_run = true ;

	m_myView->Invalidate();
	m_myView->UpdateWindow(); 

	//double distance = calc_distance();
	//igd.push_back(0); 
	//igd.push_back(distance);
	//cout<<"cur_generation =  "<<cur_generation<<"  hypervolume = "<<hypervolume<<"  "<<"  IGD = "<<distance<<"  "<<endl;

	for( this->cur_generation=2; cur_generation <= total_gen ; cur_generation++ ) {

		evolution();

		int dd = int( total_gen / 25.0 );

		// calculate igd-values
		if( cur_generation%dd == 0 ){

			//double begintime = clock();

			hypervolume = compute_hypervolume(population, population_size, nobj);//cout<<"cur_generation = "<<cur_generation<<"  hypervolume = "<<hypervolume<<"  "<<endl;
			hvl.push_back( int(1.0*cur_generation/dd) );
			hvl.push_back(hypervolume);


			get_best_obj( 0, path_length ) ;
			get_best_obj( 1, path_smoothness ) ;
			get_best_obj( 2, path_security ) ;


			//distance = calc_distance();
			//igd.push_back( int(1.0*cur_generation / dd ));
			//igd.push_back(distance);

			//double endtime = clock();
			//unevolvetime += endtime - begintime;

		}

		m_myView->Invalidate();
		m_myView->UpdateWindow(); 
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	// save the final population - X space
	//if(cur_generation%mg==0)
	//{
	
	//sprintf( filename,"POF/POP_MOEAD_%s_R%d.dat",strTestInstance, irun,total_gen);
	//save_front(filename);

	//sprintf(filename,"POF/PF_MOEAD_%s_R%d.dat",strTestInstance,irun);
	//vector <CIndividual>  frontpop;
	//population2front(population, frontpop);
	//save_population(frontpop, filename);//保存扇形存档种群的非劣前沿
	//frontpop.clear();

	//sprintf(filename,"POS/PS_MOEAD_%s_R%d.dat",strTestInstance,irun, total_gen);
	//save_pos(filename);

	//}

	//char savefilename[1024];
	//   sprintf(savefilename,"ParetoFront/MOEAD_%s_R%d.dat",strTestInstance,irun);
	//save_front(savefilename);

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////



	//double totaltimeend = clock();
	//totaltime += totaltimeend - totaltimebegin;

	//return gd;
}
