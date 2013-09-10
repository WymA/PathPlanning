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

//个体结构
typedef struct individual
{
	int rank;
	double *obj;
	double crowd_dist;
	vector<int> xPath;
}individual;

//种群结构
typedef struct
{
	individual* ind;
}population;

struct Fitness
{
	double A;//对个体长度的评价
	double B;//对个体光滑度的评价
	double C;//对个体安全性的评价
	//	double D;//综合评价
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

	void Init(Para &para);						  //初始化数据
	int gamain(CRobotView* m_myView); //#GA entrance

private:
	inline void setPopSize(int popSize){this->popSize = popSize;}//设置种群大小
	inline void setGenNum(int GenNum){this->GenNum = GenNum;}//设置进化代数
	inline void setPCross(double Pc){this->Pc = Pc;}//设置交叉概率
	inline void setPMutation(double Pm){this->Pm = Pm;}//设置变异概率
	void InitPop(population *pop);//初始化种群
	void InitInd(individual &ind);//初始化个体
	void GenSelection(population *old_pop, population *new_pop);//选择算子
	individual* Tournament(individual *ind1, individual *ind2);//锦标赛
	void GenCross(individual *parent1, individual *parent2, individual *child1, individual *child2);//交叉算子
	void GenMutation(population *pop);//变异算子
	void GenMutationInd(individual &ind);
	int GenIsSeries(individual &ind, const int idx);
	void GenInsert(population *pop);//插入算子
	int GenInsertInd(individual &ind, int idx);
	void GenPopDelSame(population *pop);//删除算子
	void GenDelSame(vector<int> &xPath);
	void Evaluate(population *pop);//对种群进行评估
	void EvaluateInd(individual &ind, int n);//对个体进行评估
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
	double SearchLineNearest(const int x0, const int y0, const int curLin, const double preMin, int& minX);//搜索当前行最短距离

public:
	int GenNum;//进化代数
	int curGenNum;//当前进化代数
	double Pc;//交叉概率
	double Pm;//变异概率
	int popSize ;//种群个数
	int nobj ;
	population parent_pop;//种群
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
