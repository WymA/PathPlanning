
#ifndef __SNGA_CLASS_H_
#define __SNGA_CLASS_H_

#include "Global.h"
//#include "..\common\objective.h"



class CAEAInd{

public:
	vector<int> x_var;
	vector<double> y_obj;

	bool is_pass ;
	int n_blocked ;

	double sectorial_angle;
	int sectorialindex;               //点所在扇形区域的index

	void   RandomInit();
	void   ObjEvaluate( const int& index );

	CAEAInd();
	~CAEAInd();

	void obj2angle_index(/*vector <double> & pseudonadirpoint,  */vector <double> & observerpoint, int sectornum);
	//void  angle2coord(int sectornum, double anglesingle);
	//void  yobj2angle(vector <double> & observerpoint);
	//bool angle_in_sector(vector <double> & observerpoint, int sectornum, double anglesingle ,int sectorcoord);


	/** Compares two points in multiple objective space
	*
	*	Returns _Dominated if this is dominated
	*	Returns _Dominating if this is dominating
	*	It may also return _Nondominated or _Equal */
	
	TCompare Compare(CAEAInd& ind2);

	//bool   operator<(const CAEAInd& ind2);
	//bool   operator==(const CAEAInd& ind2);
	//void   operator=(const CAEAInd& ind2);

};

#endif