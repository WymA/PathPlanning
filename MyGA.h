///////////////////////////////////////////////////////////////////////
//
// MyGA.h: interface for the MyGA class.
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
typedef struct individual
{
	int rank;
	double *obj;
	double crowd_dist;
	vector<int> xPath;
}individual;

//��Ⱥ�ṹ
typedef struct
{
	individual* ind;
}population;

struct Fitness
{
	double A;//�Ը��峤�ȵ�����
	double B;//�Ը���⻬�ȵ�����
	double C;//�Ը��尲ȫ�Ե�����
	//	double D;//�ۺ�����
};

//static struct  Fitness fit[1000];
//static struct Fitness F;

typedef struct lists
{
	int index;
	struct lists *parent;
	struct lists *child;
}list;

typedef struct Parameter
{
	int pSize;
	int T;
	double propC;
	double propM;
	int width;
	int height;
	bool length;
	bool smooth;
	bool safe;
}Para;

class MyGA  
{
public:
	MyGA(Para &param);
	virtual ~MyGA();

	void Init(Para &para);						  //��ʼ������
	int gamain(CRobotView* m_myView); //#GA entrance

private:
	inline void setPopSize(int popSize){this->popSize = popSize;}//������Ⱥ��С
	inline void setGenNum(int GenNum){this->GenNum = GenNum;}//���ý�������
	inline void setPCross(double Pc){this->Pc = Pc;}//���ý������
	inline void setPMutation(double Pm){this->Pm = Pm;}//���ñ������
	void InitPop(population *pop);//��ʼ����Ⱥ
	void InitInd(individual &ind);//��ʼ������
	void GenSelection(population *old_pop, population *new_pop);//ѡ������
	individual* Tournament(individual *ind1, individual *ind2);//������
	void GenCross(individual *parent1, individual *parent2, individual *child1, individual *child2);//��������
	void GenMutation(population *pop);//��������
	void GenMutationInd(individual &ind);
	int GenIsSeries(individual &ind, const int idx);
	void GenInsert(population *pop);//��������
	int GenInsertInd(individual &ind, int idx);
	void GenPopDelSame(population *pop);//ɾ������
	void GenDelSame(vector<int> &xPath);
	void Evaluate(population *pop);//����Ⱥ��������
	void EvaluateInd(individual &ind, int n);//�Ը����������
	void GetPopBestObj(int iObj, vector<int> &path);
	bool check(individual &ind, int i);
	void assign_rank_and_crowding_distance(population *new_pop, int popSize);
	void assign_crowding_distance_list(population *pop, list *list, int front_size);
	void assign_crowding_distance_indices(population *pop, int c1, int c2);
	void assign_crowding_distance(population *pop, int *dist, int **obj_array, int front_size);
	void fill_nondominated_sort(population *mixed_pop, population *new_pop, int popSize);
	int check_dominance(individual *a, individual *b);
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
	void copy_ind(individual *ind1, individual *ind2);
	void randomize();
	void warmup_random(double seed);
	void advance_random();
	double randomperc();
	int rnd(int low, int high);
	double rndreal(double low, double high);
	void report_pop(population *pop, FILE *fpt, int popSize);
	double SearchLineNearest(const int x0, const int y0, const int curLin, const double preMin, int& minX);//������ǰ����̾���

public:
	int GenNum;//��������
	int curGenNum;//��ǰ��������
	double Pc;//�������
	double Pm;//�������
	int popSize ;//��Ⱥ����
	int nobj ;
	population parent_pop;//��Ⱥ
	vector<int> path1;
	vector<int> path2;
	vector<int> path3;
	bool m_bAlreadyStarted;
	bool m_bAlreadyRun;

private:
	double oldrand[55];
	int jrand;
};

#endif // !defined(AFX_MYGA_H__664085B3_C966_4971_AECE_7E6A0F800536__INCLUDED_)
