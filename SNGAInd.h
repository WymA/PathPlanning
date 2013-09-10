
#ifndef __SNGA_CLASS_H_
#define __SNGA_CLASS_H_

#include "Global.h"
//#include "..\common\objective.h"



class CSNGAInd{
public:
	vector <double> x_var;
	vector <double> y_obj;

	//int    rank, count;
	//vector <int>        sectorialcoord;
	//vector <double>     
	double sectorialangle;
	int sectorialindex;               //点所在扇形区域的index

	void   rnd_init();
	void   obj_eval();
	void   show_objective();

	CSNGAInd();
	~CSNGAInd();



	//void  yobj2angle(vector <double> & observerpoint);

	void obj2angle_index(/*vector <double> & pseudonadirpoint,  */vector <double> & observerpoint, int sectornum);//void  angle2coord(int sectornum, double anglesingle);

	//bool angle_in_sector(vector <double> & observerpoint, int sectornum, double anglesingle ,int sectorcoord);


	/** Compares two points in multiple objective space
	*
	*	Returns _Dominated if this is dominated
	*	Returns _Dominating if this is dominating
	*	It may also return _Nondominated or _Equal */
	TCompare Compare(CSNGAInd& ind2);

	//bool   operator<(const CSNGAInd& ind2);
	//bool   operator==(const CSNGAInd& ind2);
	//void   operator=(const CSNGAInd& ind2);

};

#endif