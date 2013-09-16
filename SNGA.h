

#ifndef __SNGA_FUNC_H_
#define __SNGA_FUNC_H_

//class CSNGAInd;
#include "sngaind.h"

//enum TUpdated {_No_Updated, _New_Inserted, _Distance_Replaced, _Angle_Replaced, _Distance_Angle_Replaced}; 

class CSNGA    
{
public:
	CSNGA(int pop_size);
	virtual ~CSNGA();

	void execute(int run, vector<double>& hvl, vector<double>& igd, double& totaltime,  double &unevolvetime);//vector<double> execute(int run);
	void init_population();
	//double calc_distance();

	//void save_population(vector <CSNGAInd>  &mypopulation, char saveFilename[1024]);                              // save the pareto front into files
	//void save_ps(char savefilename[1024]);

	//vector <int>  sectorialgrid;
	vector <CSNGAInd>  sectorpop;
	vector <CSNGAInd>  ps;

	CSNGAInd onechild;
	int popsize;
	
	vector<vector <double>> AnchorPoint;     //Ãªµã
	vector <double> TrueNadirPoint;               //true-nadir-point
	vector <double> ObserverPoint;
	vector <double> ReferencePoint;

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
	double GetFastigiateHyperVolume(CSNGAInd&  ind, int ind_index, vector <double> &ReferencePoint);
	int  tour_selection_hv2(vector <CSNGAInd>  &mypopulation);
	double  tour_selection_hv_difference(int p, vector <CSNGAInd>  &mypopulation);

	///////////////////////////////////////////////////////////////////////////////////////////
	//#Modified on 2013/9/5//////////////////////////////////////////////////////////////
	double compute_hypervolume(vector <CSNGAInd>&  mysectorpop, int mypopsize, int mynobj) ;

};

#endif

