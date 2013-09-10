#ifndef __GLOBAL_H_
#define __GLOBAL_H_

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <memory.h>
#include <vector>
#include <cassert>
//#include <algorithm>

using namespace std;

//#include "random.h"
//#include "cec09.h"

#define PI  3.1415926535897932384626433832795

//#//#define POPSIZE 40
//#//#define WIDTH 20
//#//#define HEIGTH 20
//#//#define GENNUM 500
//#//#define PC 0.9
//#//#define PM 0.05
#define MAX_DIST 1000 //æ‡¿Î…œœﬁ
#define INF 1.0e14
#define E  2.71828182845905


//#define EPS 1.0e-14
//#define PI 3.14159265358979


/*------Constants for rnd_uni()--------------------------------------------*/

#define IM1 2147483563
#define IM2 2147483399
#define AM (1.0/IM1)
#define IMM1 (IM1-1)
#define IA1 40014
#define IA2 40692
#define IQ1 53668
#define IQ2 52774
#define IR1 12211
#define IR2 3791
#define NTAB 32
#define NDIV (1+IMM1/NTAB)
#define EPS 1.0e-14
#define RNMX (1.0-EPS)

double rnd_uni(long *idum);

//------------- Parameters in test instance ------------------

extern int nvar, nobj ;                    //  the number of variables and objectives
//extern double  mymoving;

///////////////////////////////////////////////////////////////////
//#Added on 2013/9/9////////////////////////////////////////
extern double cross_rate ;
extern double mutation_rate ;
///////////////////////////////////////////////////////////////////


//double  lowBound = 0,   uppBound = 1;   //  lower and upper bounds of variables
#define VAR_MAX_LENGTH 30

//void obj_eval_ywq(vector <double>& x_var, vector <double>& y_obj);

enum TDecompositionType {_Decomposition_TCH1, _Decomposition_TCH2, _Decomposition_PBI}; 

extern TDecompositionType tDecompType ;

/** Possible relations between points in multiple objective space */
enum TCompare {_Pareto_Dominating, _Pareto_Dominated, _Pareto_Nondominated, _Pareto_Equal}; 

extern double xHypervolumeRefPoint_TopRight;
extern double yHypervolumeRefPoint_TopRight ;

extern double xHypervolumeRefPoint_BottomLeft ;
extern double yHypervolumeRefPoint_BottomLeft ;

extern double  lowBound[], uppBound[];

//******** Common parameters in MOEAs **********************************************
extern int		total_gen,    //  the maximal number of generations
	total_run ;//,//1,      //  the maximal number of runs
//	m_population_size ;    //  the population size
//	nfes;             //  the number of function evluations
//**********************************************************************************

//------------- Parameters in random number ------------------
extern int     seed ;
extern long    rnd_uni_init;        

//------------- Parameters in MOEA/D -------------------------
extern vector<double> idealpoint;
extern double scale[100];  

extern int	 etax, etam  ;   // distribution indexes of crossover and mutation
//extern double  realx,  realm,  realb;    // crossover, mutation, selection probabilities

///////////////////////////////////////////////////////////////////////
//# 2013/9/4////////////////////////////////////////////////////////
TCompare ParetoCompare(vector <double> & y_obj1, vector <double>& y_obj2);
void quicksort_increasing(vector <vector<double>> &paretofront, int nPoints);
double hv2(vector <vector<double>> &paretofront);


#endif