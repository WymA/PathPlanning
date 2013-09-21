#pragma once

//class CSNGAInd;
#include "sngaind.h"
#include "RobotView.h"

//enum TUpdated {_No_Updated, _New_Inserted, _Distance_Replaced, _Angle_Replaced, _Distance_Angle_Replaced}; 

class CSNGA    
{
	/////////////////////////////////////////////////////
	//##Added on 2013/9/16
public :
	vector<double> hvl ;
	bool m_is_run ;
	int evaluations ;
	int cur_gen ; 

	void get_best_obj( const int& i_obj, vector<int> &path ) ;
	void init_parameter() ;
	void evaluation() ;
	void gen_pop_del_same() ;
	void gen_delete_same( vector<int>& x_path ) ;
	void delete_not_pass() ;
	///////////////////////////////////////////////////////
public:
	CSNGA();
	virtual ~CSNGA();

	void execute( CRobotView* m_myView );//vector<double> execute(int run);
	void init_population();
	//double calc_distance();

	//void save_population(vector <CSNGAInd>  &mypopulation, char saveFilename[1024]);                              // save the pareto front into files
	//void save_ps(char savefilename[1024]);

	//vector <int>  sectorialgrid;

	vector <CSNGAInd>  sector_population;
	//vector <CSNGAInd>  ps;

	CSNGAInd onechild;
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
	//void operator=(const CSNGA &alg);

	bool Pareto_HyperVolume_compare_sectorialgrid(CSNGAInd& ind);
	bool update_extreme_point(CSNGAInd& ind);

	//void initial_observation_and_reference_point(CSNGAInd& ind);
	//void uniform_selection(CSNGAInd*& ind_selected);
	//void sbx_mutation_evolution(CSNGAInd& ind_selectedparents1, CSNGAInd& ind_selectedparents2);

	void reset_angle();
	void population2front(vector <CSNGAInd>  &mypopulation, vector <vector<double>> &population_front);//vector <CSNGAInd> 
	double get_fastigiate_hypervolume(CSNGAInd&  ind, int ind_index, vector <double> &reference_point);
	int  tour_selection_hv2(vector <CSNGAInd>  &mypopulation);
	double  tour_selection_hv_difference(int p, vector <CSNGAInd>  &mypopulation);

	///////////////////////////////////////////////////////////////////////////////////////////
	//#Modified on 2013/9/5//////////////////////////////////////////////////////////////
	double compute_hypervolume(vector <CSNGAInd>&  mysectorpop, int mypopsize, int mynobj) ;

};
