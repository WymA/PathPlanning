
#include "stdafx.h"
#include "Common.h"
//#include "cec09.h"
#include "SNGAInd.h"

extern int divide;

CSNGAInd::CSNGAInd()
{

	for(int i=0; i<nvar; i++)
		x_var.push_back(0.0);
	for(int j=0; j<nobj; j++)
		y_obj.push_back(0.0);

	//for(int j=0; j<nobj-1; j++)
	sectorialangle = 0;//.push_back(0.0);
}

CSNGAInd::~CSNGAInd()
{
	x_var.clear();
	y_obj.clear();

	//sectorialcoord.clear();
	//sectorialangle.clear();
}

void CSNGAInd::rnd_init()             //variable initival 
{
	for(int n=0;n<nvar;n++)
	{
		x_var[n] = lowBound[n] + rnd_uni(&rnd_uni_init)*(uppBound[n] - lowBound[n]);                 //initival the variable
	}

	obj_eval();
}

void CSNGAInd::obj_eval()                                              //initival objevtive function value
{
	//obj_eval_ywq(x_var, y_obj);	
	//pFunc[iCurTestInstance](&(*(x_var.begin())), &(*(y_obj.begin())), nvar);
}

void CSNGAInd::show_objective()
{

	for(int n=0;n<nobj;n++)
		std::cout<<y_obj[n]<<" ";
	//	for(int n=0;n<nobj-1;n++)
	std::cout<<sectorialindex<<" ";
	//std::cout<<rank<<" ";
	std::cout<<"\n";

}





//void CSNGAInd::yobj2angle(vector <double> & observerpoint)
//{
//
//	//double angle,distance;
//
//	double obj_relative_sum = 0.0;
//	for(int j=0; j<nobj; j++)
//	{
//		obj_relative_sum += y_obj[j] - (observerpoint[j] );
//	}
//
//	//for(int j=0; j<nobj; j++)
//	{
//
//		if(obj_relative_sum!=0)
//			sectorialangle = (y_obj[0] - (observerpoint[0] )) / obj_relative_sum ;
//		else 
//			sectorialangle = 0;
//	}
//}

//bool CSNGAInd::angle_in_sector(vector <double> & observerpoint, int sectornum, double anglesingle , int sectorcoord)
//{
//
//	bool bInSector = false;
//
//	//for(int j=0; j<nobj-1; j++)
//	{
////与修改角度计算有关的重要地方
//			if( floor( sectorialangle * (sectornum - 1) + 0.5 ) == sectorcoord )
//			bInSector = true;
//
//	}
//
//	return bInSector;
//}

void CSNGAInd::obj2angle_index(/*vector <double> & pseudonadirpoint, */vector <double> & observerpoint, int sectornum)  //观察点、扇形区域
{
	double obj_relative_sum = 0.0;
	double normalizedf[2];
	for(int j=0; j<nobj; j++)
	{
		//if(pseudonadirpoint[j] - observerpoint[j]>0)
		//	normalizedf[j] = (y_obj[j] - observerpoint[j] ) / (pseudonadirpoint[j] - observerpoint[j]);
		//else
		normalizedf[j] = y_obj[j] - observerpoint[j] ;
		obj_relative_sum += normalizedf[j];
	}

	//for(int j=0; j<nobj; j++)
	if(obj_relative_sum!=0)
		sectorialangle = normalizedf[0] / obj_relative_sum ;
	else 
		sectorialangle = 0;

	sectorialindex = floor( divide*sectorialangle * (sectornum - 1) + 0.5 );
}

TCompare CSNGAInd::Compare(CSNGAInd& ind2) {
	bool bBetter = false;
	bool bWorse = false;

	int i = 0;
	do { 
		if(y_obj[i] < ind2.y_obj[i])
			bBetter = true;
		if(ind2.y_obj[i] < y_obj[i])
			bWorse = true;
		i++;
	}while (!(bWorse && bBetter) && (i < nobj));
	if (bWorse) {
		if (bBetter)
			return _Pareto_Nondominated;
		else
			return _Pareto_Dominated;
	}
	else {
		if (bBetter)
			return _Pareto_Dominating;
		else
			return _Pareto_Equal;
	}
}

//bool CSNGAInd::operator<(const CSNGAInd& ind2)
//{
//	int flag2 = 0;
//	for(int n=0;n<nobj;n++)
//	{
//	    if(ind2.y_obj[n] < y_obj[n])
//	        return false;
//		if(ind2.y_obj[n] == y_obj[n])
//			flag2++;
//    }
//
//    if(flag2==nobj) return false;
//
//	return true;
//}
//
//bool CSNGAInd::operator==(const CSNGAInd& ind2)
//{
//	int flag = 0;
//	for(int n=0;n<nobj;n++)
//	{
//	    if(ind2.y_obj[n] !=y_obj[n])
//	        return false;
//    }
//    return true;
//}
//
//void CSNGAInd::operator=(const CSNGAInd& ind2)
//{
//	int n;
//	for(n=0;n<nobj;n++)
//	    y_obj[n] = ind2.y_obj[n];
//
//	for(n=0;n<nvar;n++)
//	    x_var[n] = ind2.x_var[n];
//    //rank  = ind2.rank;
//
//	for(n=0;n<nobj-1;n++)
//		sectorialcoord[n] = ind2.sectorialcoord[n];
//
//	for(n=0;n<nobj/*-1*/;n++)
//		sectorialangle[n] = ind2.sectorialangle[n];
//
//	sectorialindex = ind2.sectorialindex;
//
//}

