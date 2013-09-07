#include "stdafx.h"
#include "MOEAD.h"
//#include "ScalarFunc.h"
#include "Chart.h"

TMOEAD::TMOEAD( int nVariable, int nObjective )
{
	nvar = nVariable ;
	nobj = nObjective ;

	//idealpoint = new double[nobj];
	indivpoint = new CIndividual[nobj];    
	// initialize ideal point	
	for( int n = 0 ; n < nobj ; n++) {

		idealpoint.push_back( 1.0e+30 );//idealpoint[n] = 1.0e+30;  
		indivpoint[n].rnd_init( );
		indivpoint[n].obj_eval( n );
	}
}

TMOEAD::~TMOEAD()
{
	idealpoint.clear();//delete [] idealpoint;    
	delete [] indivpoint;
}


void TMOEAD::init_population()
{
	for(int i=0; i<m_population_size; i++){

		population[i].indiv.rnd_init();
		population[i].indiv.obj_eval(i);

		update_reference(population[i].indiv);
	}
}


// initialize a set of evenly-distributed weight vectors
void TMOEAD::init_uniformweight(int sd)
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
	m_population_size = population.size();

}

// initialize the neighborhood of subproblems based on the distances of weight vectors
void TMOEAD::init_neighbourhood()
{
	double *x   = new double[m_population_size];
	int    *idx = new int[m_population_size];
	for( int i = 0 ; i < m_population_size ; i++ ){	

		for( int j = 0 ; j < m_population_size ; j++ )	{

			x[j]    = dist_vector(population[i].namda,population[j].namda);
			idx[j]  = j;			
		}

		minfastsort( x, idx, m_population_size, m_neighborhood_size );  

		for( int k = 0 ; k < m_neighborhood_size ; k++ )   
			population[i].table.push_back( idx[k] );

	}
	delete [] x;
	delete [] idx;
}

// update the best solutions of neighboring subproblems
void TMOEAD::update_problem(CIndividual &indiv, int id)
{
	for( int i = 0 ; i < m_neighborhood_size ; i++ ){

		int    k  = population[id].table[i];
		double f1, f2 ;
		f1 = scalar_func(population[k].indiv.y_obj, population[k].namda, indivpoint) ;
		f2 = scalar_func(indiv.y_obj, population[k].namda, indivpoint) ;
		if( f2 < f1 ) population[k].indiv = indiv ;
	}
}


// update the reference point
void TMOEAD::update_reference(CIndividual &ind)
{
	for(int n=0; n<nobj; n++) {

		if(ind.y_obj[n]<idealpoint[n]){

			idealpoint[n]  = ind.y_obj[n];
			indivpoint[n]  = ind;
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
		for(int j=0;j<mypopsize;j++){

			if( j != i && ParetoCompare(mypop[i].indiv.y_obj, mypop[j].indiv.y_obj) == _Pareto_Dominated){

				Dominated = true;
				break;
			}
		}
		if(!Dominated &&
			mypop[i].indiv.y_obj[0]<=xHypervolumeRefPoint_TopRight &&
			mypop[i].indiv.y_obj[1]<=yHypervolumeRefPoint_TopRight
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
		real_sbx_xoverA( population[p1].indiv, population[p2].indiv, child, child2 );
		realmutation( child, 1.0/nvar );
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
	m_population_size        = emo.m_population_size;
	population  = emo.population;
	indivpoint  = emo.indivpoint;
	m_neighborhood_size       = emo.m_neighborhood_size;
} 

//
//double TMOEAD::calc_distance()
//{
//	double distance = 0;
//	for(int i=0; i<ps.size(); i++){
//
//		double min_d = 1.0e+10;
//		for(int j=0; j<population.size(); j++){
//
//			double d = dist_vector(ps[i].y_obj, population[j].indiv.y_obj);
//			if(d<min_d)  min_d = d;
//		}
//		distance+= min_d;
//	}
//	distance/=ps.size();
//
//	return distance;
//}




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
double TMOEAD::scalar_func(vector <double> &y_obj, vector <double> &namda, CIndividual* nbi_node)
{

	double fvalue = 0;

	// Tchebycheff approach
	if(tDecompType == _Decomposition_TCH1)
	{
		double max_fun = -1.0e+30;
		for(int n=0; n<nobj; n++)
		{
			double diff = fabs(y_obj[n] - idealpoint[n] );
			double feval;
			if(namda[n]==0) 
				feval = 0.00001*diff;
			else
				feval = diff*namda[n];
			if(feval>max_fun) max_fun = feval;

		}

		fvalue = max_fun;
	}

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




void  TMOEAD::execute(
	int sd, int nc, int mg, int irun, 
	vector<double>& hvl, vector<double>& igd, 
	double& totaltime, double &unevolvetime
	)
{
	// sd: integer number for generating weight vectors
	// nc: size of neighborhood
	// mg: maximal number of generations 

	double totaltimebegin = clock();

	m_neighborhood_size = nc;	
	init_uniformweight(sd);
	init_neighbourhood();
	init_population();	

	//char filename[1024];
	//sprintf(filename,"POF0/%s.dat",strTestInstance);
	//loadpfront( filename, ps ) ;

	int gen = 1;

	double hypervolume = compute_hypervolume(population, m_population_size, nobj);////calc_distance();
	hvl.push_back(0);
	hvl.push_back(hypervolume);

	//double distance = calc_distance();
	//igd.push_back(0); 
	//igd.push_back(distance);
	//cout<<"gen =  "<<gen<<"  hypervolume = "<<hypervolume<<"  "<<"  IGD = "<<distance<<"  "<<endl;

	for( gen=2; gen <= mg ; gen++ ) {

		evolution();

		int dd = int( mg / 25.0 );

		// calculate igd-values
		if( gen%dd == 0 ){

			double begintime = clock();

			hypervolume = compute_hypervolume(population, m_population_size, nobj);//cout<<"gen = "<<gen<<"  hypervolume = "<<hypervolume<<"  "<<endl;
			hvl.push_back( int(1.0*gen/dd) );
			hvl.push_back(hypervolume);

			//distance = calc_distance();
			//igd.push_back( int(1.0*gen / dd ));
			//igd.push_back(distance);

			double endtime = clock();
			unevolvetime += endtime - begintime;

		}
	}

	// save the final population - X space
	//if(gen%mg==0)
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

	population.clear();

	double totaltimeend = clock();
	totaltime += totaltimeend - totaltimebegin;

	//return gd;
}
