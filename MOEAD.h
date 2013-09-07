#pragma once

#include "Global.h"
#include "Common.h"
#include "MOEADInd.h"
#include "Recomb.h"



class TMOEAD    
{
public:
	
	vector <TSOP>  population;  // current population      	   
	CIndividual *indivpoint;    // reference point
	int  m_neighborhood_size ;                 // neighborhood size
	int  m_population_size ;                  // population   size	
	vector <CIndividual>  ps;

	TMOEAD( int nVariable, int nObjective );
	virtual ~TMOEAD();

	void init_uniformweight(int sd);    // initialize the weights for subproblems
	void init_neighbourhood();          // calculate the neighbourhood of each subproblem
	void init_population();             // initialize the population

	void update_reference(CIndividual &ind);           // update the approximation of ideal point
	void update_problem(CIndividual &child, int id);   // compare and update the neighboring solutions
	void evolution( );                                  // mating restriction, recombination, mutation, update
	void execute(int sd, int nc, int mg, int irun, vector<double>& hvl, vector<double>& igd, double& totaltime,  double &unevolvetime );//vector<double>  execute(int sd, int nc, int mg, int rn);          // execute MOEAD
	//void save_front(char savefilename[1024]);          // save the pareto front into files

	void operator=(const TMOEAD &emo);

	/////////////////////////////////////////////////////////////////////////////////////
	//#Modified on 2013/9/5////////////////////////////////////////////////////////
	//double calc_distance();
	//void save_pos(char saveFilename[]);

	/////////////////////////////////////////////////////////////////////////////////////
	///##Modified on 2013/9/4//////////////////////////////////////////////////////////
	double innerproduct(vector <double>&vec1, vector <double>&vec2) ;
	double norm_vector(vector <double> &x) ;
	double scalar_func(vector <double> &y_obj, vector <double> &namda, CIndividual* nbi_node) ;
	double compute_hypervolume(vector <TSOP>&  mypop, int mypopsize, int mynobj) ;

};