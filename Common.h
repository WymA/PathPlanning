#ifndef __COMMON_H_
#define __COMMON_H_

#include "global.h"
#include "CAEAind.h"

//using namespace std ;

void minfastsort(vector<double>& x, vector<int>& idx, int n, int m );
void minfastsort(double* x, int* idx, int n, int m);
double DistVector(vector<double>& vec1, vector <double> &vec2);
double observepoint_dist_vector(vector <double> &vec1, vector <double> &vec2);

//double nofn_optimumpoint_dist_vector(vector <double> &vec1, vector<CAEAInd> &vec2, int n_fn);

//bool   dominate(vector<double> &u, vector<double> &v, double epsilon);

//double fitnessfunction(vector <double> &y_obj, vector <double> &lambda);

//void load_data(char filename[1024], vector< vector<double> > &data, int dim);

//void LoadParameter(char *filename, Para& para )    
//{
//	fstream fin;
//	char str[100]=" ";
//	fin.open( filename, std::ios::in );
//	if(fin.is_open()){
//
//		//const char* tmp;                                                        //point to the const object
//		std::string str;
//
//		std::getline(fin,str,'\n');                          //read the data from fin to str
//		//tmp = ;                                  //change the security_fitness++ string type to the security_fitness char string 
//		para.height = atoi( str.c_str() ) ;
//
//	} //end if
//
//	fin.close();	    
//}



#endif