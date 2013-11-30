#pragma once

#include "RobotView.h"
#include "Global.h"
#include "Common.h"
#include "MOEADInd.h"


class MOEAD    
{
public:
	
	vector<TSOP> population;  // current population      	   
	CIndividual *reference_point;    // reference point
	int  neighborhood_size ;                 // neighborhood size
	int  population_size ;                  // population   size	
	//vector<CIndividual>  ps;
	vector<double> hvl ;
	int cur_generation ;
	int sd ;	// sd: the integer number of generating weight vectors

	vector<vector<double>> parato_front ;
public :
	MOEAD( );
	virtual ~MOEAD();

	////////////////////////////////////////////////////
	//#Added on 2013/9/11 ///////////////////////
	void InitParameter() ;
	//#2013/11/4////////////////////////////////////
	void Population2Front( ) ;

	//#Added on 2013/9/14 /////////////////////////

	void GenPopDelSame() ;
	void GenDeleteSame( vector<int>& x_path ) ;
	////////////////////////////////////////////////////
	void InitUniformWeight();    // initialize the weights for subproblems
	void InitNeighborhood();          // calculate the neighbourhood of each subproblem
	void InitPopulation();             // initialize the population

	void UpdateReference(CIndividual &ind);           // update the approximation of ideal point
	void UpdateProblem(CIndividual &child, int id);   // compare and update the neighboring solutions
	void Evolution( );                                  // mating restriction, recombination, mutation, update
	void Execute(CRobotView* m_myView) ;
	
	void operator=(const MOEAD &emo);

/////////////////////////////////////////////////////////////////////////////////////
//#Modified on 2013/9/5////////////////////////////////////////////////////////
	//double calc_distance();
	//void save_pos(char saveFilename[]);

/////////////////////////////////////////////////////////////////////////////////////
///##Modified on 2013/9/4//////////////////////////////////////////////////////////
	double InnerProduct(vector <double>&vec1, vector <double>&vec2) ;
	double NormVector(vector <double> &x) ;
	double Scalarizing(vector <double> &y_obj, vector <double> &lambda, CIndividual* nbi_node) ;
	double compute_hypervolume(vector <TSOP>&  mypop, int mypopsize, int mynobj) ;

//##Modified on 2013/9/9///////////////////////////////////////////////////
	void GetBestObj( int i_obj, vector<int> &path ) ;

/////////////////////////////////////////////////////////////////////////////////////
////##Modified on 2013/9/23////////////////////////////////////////////////////
	int GenInsertInd( CIndividual& ind, const int& idx ) ;
	void GenInsert() ;

};