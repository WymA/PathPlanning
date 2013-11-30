///////////////////////////////////////////////////////////////////////
//
// NSGA2.h: interface for the NSGA2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYGA_H__664085B3_C966_4971_AECE_7E6A0F800536__INCLUDED_)
#define AFX_MYGA_H__664085B3_C966_4971_AECE_7E6A0F800536__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "global.h"

//#include "RobotView.h"
class CRobotView;

//#include "MOEAD.h"
#include "Global.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "Chart.h"
using std::vector;

typedef vector<int> intVec;

//����ṹ
class Individual
{
public :
	int rank;
	vector<double> y_obj;
	double crowd_dist;
	vector<int> x_var;

	Individual();
	TCompare Compare(Individual& ind2);
};

//��Ⱥ�ṹ
typedef struct
{
	Individual* ind;
}population;



//static struct  Fitness fit[1000];
//static struct Fitness F;

typedef struct lists
{
	int index;
	struct lists *parent;
	struct lists *child;
}list;


class NSGA2  
{
public:
	NSGA2();
	virtual ~NSGA2();

	void Init(Para &para);						  //��ʼ������
	int Execute(CRobotView* m_myView); //#GA entrance

	//fstream fo ;

private:
	inline void setPopSize(int popSize){this->popSize = popSize;}//������Ⱥ��С
	inline void setGenNum(int GenNum){this->GenNum = GenNum;}//���ý�������
	inline void setPCross(double Pc){this->Pc = Pc;}//���ý������
	inline void setPMutation(double Pm){this->Pm = Pm;}//���ñ������
	void InitPop(population *pop);//��ʼ����Ⱥ
	void InitInd(Individual &ind);//��ʼ������
	void GenSelection(population *old_pop, population *new_pop);//ѡ������
	Individual* Tournament(Individual *ind1, Individual *ind2);//������
	void GenCross(Individual *parent1, Individual *parent2, Individual *child1, Individual *child2);//��������
	void GenMutation(population *pop);//��������
	void GenMutationInd(Individual &ind);
	int GenIsSeries(Individual &ind, const int idx);
	void GenInsert(population *pop);//��������
	int GenInsertInd(Individual &ind, int idx);
	void GenPopDelSame(population *pop);//ɾ������
	void GenDelSame(vector<int> &x_var);
	void Evaluate(population *pop);//����Ⱥ��������
	void EvaluateInd(Individual &ind, int n);//�Ը����������
	void GetPopBestObj(int iObj, vector<int> &path);
	//bool CheckSecurity(Individual &ind, int i);
	void assign_rank_and_crowding_distance(population *new_pop, int popSize);
	void assign_crowding_distance_list(population *pop, list *list, int front_size);
	void assign_crowding_distance_indices(population *pop, int c1, int c2);
	void assign_crowding_distance(population *pop, int *dist, int **obj_array, int front_size);
	void fill_nondominated_sort(population *mixed_pop, population *new_pop, int popSize);
	int check_dominance(Individual *a, Individual *b);
	void crowding_fill(population *mixed_pop, population *new_pop, int count, int front_size, list *elite, int popSize);
	void allocate_memory_pop(population *pop, int size);
	void deallocate_memory_pop(population *pop, int size);
	void quicksort_front_obj(population *pop, int objcount, int obj_array[], int obj_array_size);
	void q_sort_front_obj(population *pop, int objcount, int obj_array[], int left, int right);
	void quicksort_dist(population *pop, int *dist, int front_size);
	void q_sort_dist(population *pop, int *dist, int left, int right);
	void merge(population *pop1, population *pop2, population *pop3, int popSize);
	void insert(list *node, int x);
	list* del(list *node);
	void copy_ind(Individual *ind1, Individual *ind2);
	void randomize();
	void warmup_random(double seed);
	void advance_random();
	double randomperc();
	int rnd(int low, int high);
	double rndreal(double low, double high);
	void report_pop(population *pop, FILE *fpt, int popSize);
	//double SearchRowNearest(const int x0, const int y0, const int curLin, const double preMin, int& minX);//������ǰ����̾���

	////////////////////////////////////////////////////////////////////
	//##2013/11/4 ///////////////////////////////////////////////////
	void Population2Front( ) ;

	////////////////////////////////////////////////////////////
public:
	int GenNum;//��������
	int curGenNum;//��ǰ��������
	double Pc;//�������
	double Pm;//�������
	int popSize ;//��Ⱥ����
	population parent_pop;//��Ⱥ
	//vector<vector<double>> parato_front ;

private:
	double oldrand[55];
	int jrand;
};

#endif // !defined(AFX_MYGA_H__664085B3_C966_4971_AECE_7E6A0F800536__INCLUDED_)
