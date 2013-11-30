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
#include <queue>

using namespace std;

#define PI  3.1415926535897932384626433832795
#define kMaxDist 1000 //距离上限
#define INF 1.0e14
#define E  2.71828182845905
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

const int /*nvar, */kObjNum = 3 ;                    //  the number of variables and objectives
//extern double  mymoving;
///////////////////////////////////////////////////////////////////
//#Added on 2013/9/9////////////////////////////////////////
extern double cross_rate ;
extern double mutation_rate ;
///////////////////////////////////////////////////////////////////
//#Added on 2013/9/11 /////////////////////////////////////
extern vector<int> path_length ;
extern vector<int> path_security ;
extern vector<int> path_smoothness ;
//////////////////////////////////////////////////////////////////
//# Added on 2013/9/21 ////////////////////////////////////
const int kSecurity = 0 ;
const int kSmoothness = 1 ;
const int kLength = 2 ;
const int kEvalWeight = 1000 ;//100000

extern int run_speed ;

class Fitness
{
public:

	double length_fitness;//对个体长度的评价
	double smoothness_fitness;//对个体光滑度的评价
	double security_fitness;//对个体安全性的评价

	Fitness() {

		length_fitness = 0.0 ;
		smoothness_fitness = 0.0 ;
		security_fitness = 0.0 ;
	}

	void clear() ;
};

typedef struct Parameter
{
	int pSize;
	int total_gen;
	double pop_cross_rate;
	double pop_mutation_rate;
	int width;
	int height;
	bool length;
	bool smooth;
	bool security;
	vector<int> chart ;
}Para;

extern Para cur_parameter;

const int kNeighborhoodSize = 10 ;

extern double divide ;
///////////////////////////////////////////////////////////////////
//double  lowBound = 0,   uppBound = 1;   //  lower and upper bounds of variables
const int kVarMaxLength = 30 ;

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


extern int		total_gen ;    //  the maximal number of generations

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
TCompare ParetoCompare(const vector <double> & y_obj1, const vector <double>& y_obj2);
TCompare ParetoCompare(const double* y_obj1, const double* y_obj2) ;
double SearchRowNearest( const int& x0, const int& y0, const int& curLin, const double& preMin, int& minX ) ;

void QuickSortIncreasing(vector <vector<double>> &paretofront, int nPoints);
double hv2(vector <vector<double>> &paretofront);

////////////////////////////////////////////////////////////
//# 2013/10/22 ///////////////////////////////////////////
int LoadParameter(Para& para )    ;
int SaveParameter(Para& para) ;
void DefaultInitParameter() ;

const char kFileName[] = "config.ini" ;

//# 2013/11/2 //////////////////////////////////////
extern int run_state ;

const int kStop = 0 ;
const int kStart = 1 ;
const int kPause = 2 ;

const int kChartLength = 250 ;

const int kNsga2Idx = 0 ;
const int kMoeadIdx = 1 ;
const int kCaeaIdx = 2 ;

const int kNotIndex = -1 ;

const int kTimes = 1 ;

const double kHalfSqrt2 = 0.707 ;

extern bool optimal_only; 

extern double length_pun ;
//const double kSmoothPun = 

extern queue<Fitness> avg ;

const COLORREF kColorLengthGreen = RGB( 0, 255, 0 ) ;
const COLORREF kColorSmoothnessBlue = RGB( 250, 200, 0 ) ;
const COLORREF kColorSecurityRed = RGB( 200, 0, 200 ) ;

extern Fitness fit ;
extern vector<vector<double>> total_fitness ;
extern vector<vector<double>> dominated_fitness ;
extern vector<vector<double>> dominating_fitness ;

extern int obj1, obj2 ;

void SelectObj() ;

double EvaluateLength( const vector<int>& x_var, const int& n ) ;
double EvaluateSmoothness( const vector<int>& x_var, const int& n ) ;
double EvaluateSecurity( const vector<int>& x_var, const int& n  );
int CheckSecurity( const vector<int>& x_var, const int& idx,const int& w, const int& h ) ;
int BlockNum( const vector<int>& x_var, const int& n ) ;

int ObjEval( const vector<int>x_var, const int& index, vector<double>& y_obj ) ;
void GetBestFit( const int& i_obj/*, vector<int> &path */) ;

void SpeedUp() ;
void SpeedDown() ;

extern fstream fo ;

#endif