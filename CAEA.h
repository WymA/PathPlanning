#pragma once

//class CAEAInd;
#include "CAEAind.h"
#include "RobotView.h"

//enum TUpdated {_No_Updated, _New_Inserted, _Distance_Replaced, _Angle_Replaced, _Distance_Angle_Replaced}; 

class CAEA    
{
	/////////////////////////////////////////////////////
	//##Added on 2013/9/16
public :
	vector<double> hvl ;
	//bool m_is_run ;
	int evaluations ;
	int cur_gen ; 
	vector <vector<double>> parato_front ;

	void GetBestObj( const int& i_obj, vector<int> &path ) ;
	void InitParameter() ;
	void evaluation() ;
	void GenPopDelSame() ;
	void GenDeleteSame( vector<int>& x_path ) ;
	void DeleteNotPass() ;
	int   GenInsertInd( CAEAInd &ind, const int& idx ) ;
	void GenInsert() ;
	void GenInsertInd( CAEAInd& ind ) ;
	///////////////////////////////////////////////////////
public:
	CAEA();
	virtual ~CAEA();

	void Execute( CRobotView* m_myView );//vector<double> Execute(int run);
	void InitPopulation();
	//double calc_distance();

	//void save_population(vector <CAEAInd>  &mypopulation, char saveFilename[1024]);                              // save the pareto front into files
	//void save_ps(char savefilename[1024]);

	//vector <int>  sectorialgrid;

	vector <CAEAInd>  sector_population;
	//vector <CAEAInd>  ps;

	CAEAInd onechild;
	int population_size;
	
	vector<vector <double>> anchor_point;     //Ãªµã
	vector<double> true_nadir_point;               //true-nadir-point
	vector<double> observer_point;
	vector<double> reference_point;

	//int sectornum;
	//double anglesingle;
	//int p_nparts;
	//vector <double> UtopiaPoint;
	//vector <double> PseudoNadirPoint;	
	//double distance;
	//int    nfes;
	//void operator=(const CAEA &alg);

	bool pareto_hypervolume_compare_sectorialgrid(CAEAInd& ind);
	bool update_extreme_point(CAEAInd& ind);

	//void initial_observation_and_reference_point(CAEAInd& ind);
	//void uniform_selection(CAEAInd*& ind_selected);
	//void sbx_mutation_evolution(CAEAInd& ind_selectedparents1, CAEAInd& ind_selectedparents2);

	void reset_angle();
	void Population2Front( );//vector <CAEAInd> 
	double get_fastigiate_hypervolume(CAEAInd&  ind, int ind_index, vector <double> &reference_point);
	int  tour_selection_hv2(vector <CAEAInd>  &mypopulation);
	double  tour_selection_hv_difference(int p, vector <CAEAInd>  &mypopulation);

	///////////////////////////////////////////////////////////////////////////////////////////
	//#Modified on 2013/9/5//////////////////////////////////////////////////////////////
	double compute_hypervolume(vector <CAEAInd>&  mysectorpop, int mypopsize, int mynobj) ;

	//######## 2013/11/2 ///////////////////////////////////
	void GenInsert( CAEAInd& indiv ) ;
};
