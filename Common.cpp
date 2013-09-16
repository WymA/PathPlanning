
#include "stdafx.h"
#include "Common.h"

void minfastsort(vector<double> &x, vector<int> &idx, int n, int m)
{
	minfastsort((double*)&(*(x.begin())), (int*)&(*(idx.begin())),  n,  m);
}

void minfastsort(double* x, int* idx, int n, int m)               //sort the vector and the index
{
	for(int i=0; i<m; i++)
	{
		for(int j=i+1; j<n; j++)
			if(x[i]>x[j])
			{
				double temp = x[i];
				x[i]        = x[j];
				x[j]        = temp;
				int id      = idx[i];
				idx[i]      = idx[j];
				idx[j]      = id;
			}
	}
}


double dist_vector(vector <double> &vec1, vector <double> &vec2)        //the distance of two vector
{
	int dim = vec1.size();
    double sum = 0;
	for(int n=0; n<dim; n++)
	    sum+=(vec1[n] - vec2[n])*(vec1[n] - vec2[n]);

	return sqrt(sum);
}

double observepoint_dist_vector(vector <double> &vec1, vector <double> &vec2)
{
	int dim = vec1.size();
	double sum = 0;
	for(int n=0; n<dim; n++)
		sum+=(vec1[n] - (vec2[n] ) )*(vec1[n] - (vec2[n] ));
	return sqrt(sum);
}

//double fn_dist_vector(vector <double> &vec1, vector <double> &vec2, int n_fn)
//{
//	//int dim = vec1.size();
//	double dist ;
//	//for(int n=0; n<dim; n++)
//	//	sum+=(vec1[n] - vec2[n])*(vec1[n] - vec2[n]);
//	dist = abs( vec1[n_fn] - vec2[n_fn] );
//	return dist;
//}

//double nofn_dist_vector(vector <double> &vec1, vector <double> &vec2, int n_fn)
//{
//	int dim = vec1.size();
//	double sum = 0;
//	for(int n=0; n<dim; n++)
//		if(n!=n_fn)
//			sum+=(vec1[n] - vec2[n])*(vec1[n] - vec2[n]);
//	return sqrt(sum);
//}

//double nofn_optimumpoint_dist_vector(vector <double> &vec1, vector<CSNGAInd> &vec2, int n_fn)
//{
//	int dim = vec1.size();
//	double sum = 0;
//	for(int n=0; n<dim; n++)
//		if(n!=n_fn)
//			sum+=(vec1[n] - vec2[n].y_obj[n])*(vec1[n] - vec2[n].y_obj[n]);
//	return sqrt(sum);
//}

//bool   dominate(vector<double> &u, vector<double> &v, double epsilon)
//{
//    int dim = u.size();
//	for(int i=0; i<dim; i++)
//	{
//	    if(u[i]<v[i]-epsilon)
//		{
//		    return false;
//		}
//	}
//	return true;
//}
