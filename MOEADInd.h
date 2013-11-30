#ifndef __INDIVIDUAL_H_
#define __INDIVIDUAL_H_

#ifndef __GLOBAL_H_
#include "Global.h"
#endif
//#include "testfunc.h"
//#include "problem.h"

/** Possible relations between points in multiple objective space */
//enum TCompare {_Pareto_Dominating, _Pareto_Dominated, _Pareto_Nondominated, _Pareto_Equal}; 

class CIndividual{

public:

	//double EvaluateLength( const int& n  ) ;
	//double EvaluateSmoothness( const int& n ) ;
	//double EvaluateSecurity(const int& n) ;

public:
	CIndividual();
	virtual ~CIndividual();

	vector <int> x_var ;
	vector <double> y_obj ;

	int n_blocked ;
	//bool is_pass ;
	//int    rank;
	//int    count;

	void   RandomInit();
	void   ObjEvaluate( const int& i );
	//void   show_objective();
	//void   show_variable();

	//bool CheckSecurity( CIndividual const* ind, const int& n ) ;

	bool   operator<(const CIndividual &ind2);
	bool   operator<<(const CIndividual &ind2);
	bool   operator==(const CIndividual &ind2);
	void   operator=(const CIndividual &ind2);

	/** Compares two points in multiple objective space
	*
	*	Returns _Dominated if this is dominated
	*	Returns _Dominating if this is dominating
	*	It may also return _Nondominated or _Equal */
	TCompare Compare(CIndividual& ind2);
};

// defining subproblem 

class TSOP 
{
public:
	TSOP();
	virtual ~TSOP();

	//void show();

	CIndividual     indiv;
	vector <double> lambda;    
	vector <int>    table;     // the vector for the indexes of neighboring subproblems
	vector <int>    array;

	void  operator=(const TSOP&sub2);
};


#endif

