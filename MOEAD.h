#pragma once

#include "RobotView.h"
#include "Global.h"
#include "Common.h"
#include "MOEADInd.h"


class TMOEAD    
{
public:
	
	vector<TSOP>  population;  // current population      	   
	CIndividual *reference_point;    // reference point
	int  neighborhood_size ;                 // neighborhood size
	int  population_size ;                  // population   size	
	//vector<CIndividual>  ps;
	vector<double> hvl ;
	int cur_generation ;
	int sd ;	// sd: the integer number of generating weight vectors
	bool m_is_run ;

public :
	TMOEAD( );
	virtual ~TMOEAD();

	////////////////////////////////////////////////////
	//#Added on 2013/9/11 ///////////////////////
	void init_parameter() ;
	//#Added on 2013/9/14 /////////////////////////

	void gen_pop_del_same() ;
	void gen_delete_same( vector<int>& x_path ) ;
	////////////////////////////////////////////////////
	void init_uniformweight();    // initialize the weights for subproblems
	void init_neighbourhood();          // calculate the neighbourhood of each subproblem
	void init_population();             // initialize the population

	void update_reference(CIndividual &ind);           // update the approximation of ideal point
	void update_problem(CIndividual &child, int id);   // compare and update the neighboring solutions
	void evolution( );                                  // mating restriction, recombination, mutation, update
	void execute(CRobotView* m_myView) ;
		//int sd, /*int nc,*/ int mg, int irun, 
		//vector<double>& hvl, 
		//double& totaltime, 
		//double &unevolvetime,
		// );//vector<double>  execute(int sd, int nc, int mg, int rn);          // execute MOEAD
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
	double scalarizing(vector <double> &y_obj, vector <double> &namda, CIndividual* nbi_node) ;
	double compute_hypervolume(vector <TSOP>&  mypop, int mypopsize, int mynobj) ;

//##Modified on 2013/9/9///////////////////////////////////////////////////
	void get_best_obj( int i_obj, vector<int> &path ) ;

/////////////////////////////////////////////////////////////////////////////////////
////##Modified on 2013/9/8////////////////////////////////////////////////////


};