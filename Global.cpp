#include "stdafx.h"
#include "global.h"



//------------- Parameters in test instance ------------------

//int     /*nvar,  */N_OBJ;                    //  the number of variables and objectives

double cross_rate ;
double mutation_rate ;

////////////////////////////////////////////////////////////////
//##Modified on 2013/9/11 ////////////////////////////////

vector<int> path_length ;
vector<int> path_security ;
vector<int> path_smoothness ;

////////////////////////////////////////////////////////////////////////

//double  lowBound = 0,   uppBound = 1;   //  lower and upper bounds of variables
double  lowBound[VAR_MAX_LENGTH], uppBound[VAR_MAX_LENGTH];

//char    strTestInstance[256];     //the name of the test BOP problem 

TDecompositionType    tDecompType = _Decomposition_TCH1;

//int iCurTestInstance;     

//******** Common parameters in MOEAs **********************************************
int total_gen = 0 ;//250,//250, //500 1500 1000,//500,//    //  the maximal number of generations
	//total_run = 2 ; //, //30,//      //  the maximal number of runs
	//population_size    = 100;//200,    //  the population size
//nfes;             //  the number of function evluations
double divide = 0.8;
//**********************************************************************************


Fitness fit ;
Para cur_parameter;

//------------- Parameters in random number ------------------
int     seed    = 177;
long    rnd_uni_init;        


//------------- Parameters in MOEA/D -------------------------

vector <double> idealpoint;
double          scale[100];  

double xHypervolumeRefPoint_TopRight = 1;
double yHypervolumeRefPoint_TopRight = 1;

double xHypervolumeRefPoint_BottomLeft =0;
double yHypervolumeRefPoint_BottomLeft =0;

///15 20
int		etax    =  20 , 	etam    = 20;   // distribution indexes of crossover and mutation

//double  realx,  realm,  realb = 0.9;    // crossover, mutation, selection probabilities

//void (*pFunc[11])(double *, double *, const unsigned int);


//the random generator in [0,1)         a special method                                      
double rnd_uni(long *idum)
{

	long j;
	long k;
	static long idum2=123456789;
	static long iy=0;
	static long iv[NTAB];
	double temp;

	if ( *idum <= 0 ){

		if (-(*idum) < 1) *idum=1;
		else *idum = -(*idum);
		idum2=(*idum);
		for (j=NTAB+7;j>=0;j--){

			k = (*idum)/IQ1 ;
			*idum = IA1*(*idum-k*IQ1)-k*IR1;
			if ( *idum < 0 ) *idum += IM1;
			if ( j < NTAB ) iv[j] = *idum;
		}
		iy=iv[0];
	}

	k=(*idum)/IQ1;
	*idum=IA1*(*idum-k*IQ1)-k*IR1;
	if (*idum < 0) *idum += IM1;

	k=idum2/IQ2 ;
	idum2=IA2*(idum2-k*IQ2)-k*IR2;

	if (idum2 < 0) idum2 += IM2;

	j = iy / NDIV ;
	iy = iv[j] - idum2 ;
	iv[j] = *idum ;

	if (iy < 1) iy += IMM1 ;

	if (( temp = AM*iy ) > RNMX ) return RNMX ;
	else return temp;

}/*------End of rnd_uni()--------------------------*/


TCompare ParetoCompare(vector <double> & y_obj1, vector <double>& y_obj2) //judge the pareto dominated
{  

	bool bBetter = false;
	bool bWorse = false;

	int i = 0;

	do { 

		//#Do the comparison
		if(y_obj1[i] < y_obj2[i])
			bBetter = true;
		if(y_obj2[i] < y_obj1[i])
			bWorse = true;
		i++;

	}while (!(bWorse && bBetter) && (i < N_OBJ));

	if (bWorse) {

		if (bBetter)
			return _Pareto_Nondominated; //#Worse and better
		else
			return _Pareto_Dominated; //#Worse and not better
	} else {

		if (bBetter)
			return _Pareto_Dominating; //#Not worse and better
		else
			return _Pareto_Equal; //#Not worse and not better
	}
}



void quicksort_increasing(vector <vector<double>> &paretofront, int nPoints)
{
	double temp;
	for(int i=0; i<nPoints; i++)
	{
		for(int j=i+1; j<nPoints; j++)
			if(paretofront[i][0]>paretofront[j][0])
			{
				temp = paretofront[i][0];
				paretofront[i][0]        = paretofront[j][0];
				paretofront[j][0]        = temp;

				temp = paretofront[i][1];
				paretofront[i][1]        = paretofront[j][1];
				paretofront[j][1]        = temp;
			}
	}
}


double hv2(vector <vector<double>> &paretofront)
{
	double volume = 0;
	//int j=0;
	//while(j<paretofront.size() && paretofront[j][1] >= yHypervolumeRefPoint_TopRight )
	//{
	//	j++;
	//}
	if( paretofront.size() > 0 ) {
		volume = ((xHypervolumeRefPoint_TopRight - paretofront[0][0]  ) * 
						( yHypervolumeRefPoint_TopRight - paretofront[0][1] )); 
	}
	for (int i = 1; i < paretofront.size(); i++) { // && paretofront[i][0]<xHypervolumeRefPoint_TopRight
		volume += ( (xHypervolumeRefPoint_TopRight - paretofront[i][0]  ) * 
							( paretofront[i - 1][1] - paretofront[i][1] ) ) ;
	}

	return volume;
}

///////////////////////////////////////////////////////////////////////////
//##Added on 2013/9/11 /////////////////////////////////////////////
