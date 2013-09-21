#ifndef __COMMON_H_
#define __COMMON_H_


#include "global.h"
//#include <vector>
#include "sngaind.h"

void minfastsort(vector<double>& x, vector<int>& idx, int n, int m );
void minfastsort(double* x, int* idx, int n, int m);
double dist_vector(vector<double>& vec1, vector <double> &vec2);
double observepoint_dist_vector(vector <double> &vec1, vector <double> &vec2);

//double nofn_optimumpoint_dist_vector(vector <double> &vec1, vector<CSNGAInd> &vec2, int n_fn);

//bool   dominate(vector<double> &u, vector<double> &v, double epsilon);

//double fitnessfunction(vector <double> &y_obj, vector <double> &namda);

//void load_data(char filename[1024], vector< vector<double> > &data, int dim);

template <class T>
void loadpfront(char *filename, vector<T> &ps)        //load the data from filename to ps
{
	std::fstream fin;
	int line=0;
	char str[100]=" ";
	fin.open(filename,std::ios::in);
	if(fin.is_open())
	{
		const char* pch2;                                                        //point to the const object
		char  a[20],b[20],c[20],d[20];
		std::string str;
		while(!fin.eof())
		{
			T  data;
			std::getline(fin,str,'\n');                          //read the data from fin to str
			pch2 = str.c_str();                                  //change the security_fitness++ string type to the security_fitness char string 
			//sscanf(pch2,"%s %s %s %s",a,b,c,d);		
			sscanf(pch2,"%s %s %s %s",a,b,c,d);    //store the data read from fin to a and b respectively
			data.y_obj[0] = atof(a);                            //take the string into double
			data.y_obj[1] = atof(b);
			if(N_OBJ==3)                                                //if the objective function number is 3
			{
				data.y_obj[2] = atof(c);
			}
			if(N_OBJ==4) 
			{
				data.y_obj[3] = atof(d);
			}
			line++;
			ps.push_back(data);                   //add the data to the vector tail
		}
	} //end if
	else
		std::cout<<"failed to open "<<filename<<endl;
	cout<<"here"<<endl;
	fin.close();	    
}



#endif