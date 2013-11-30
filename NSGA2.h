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

//个体结构
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

//种群结构
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

	void Init(Para &para);						  //初始化数据
	int Execute(CRobotView* m_myView); //#GA entrance

	//fstream fo ;

private:
	inline void setPopSize(int popSize){this->popSize = popSize;}//设置种群大小
	inline void setGenNum(int GenNum){this->GenNum = GenNum;}//设置进化代数
	inline void setPCross(double Pc){this->Pc = Pc;}//设置交叉概率
	inline void setPMutation(double Pm){this->Pm = Pm;}//设置变异概率
	void InitPop(population *pop);//初始化种群
	void InitInd(Individual &ind);//初始化个体
	void GenSelection(population *old_pop, population *new_pop);//选择算子
	Individual* Tournament(Individual *ind1, Individual *ind2);//锦标赛
	void GenCross(Individual *parent1, Individual *parent2, Individual *child1, Individual *child2);//交叉算子
	void GenMutation(population *pop);//变异算子
	void GenMutationInd(Individual &ind);
	int GenIsSeries(Individual &ind, const int idx);
	void GenInsert(population *pop);//插入算子
	int GenInsertInd(Individual &ind, int idx);
	void GenPopDelSame(population *pop);//删除算子
	void GenDelSame(vector<int> &x_var);
	void Evaluate(population *pop);//对种群进行评估
	void EvaluateInd(Individual &ind, int n);//对个体进行评估
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
	//double SearchRowNearest(const int x0, const int y0, const int curLin, const double preMin, int& minX);//搜索当前行最短距离

	////////////////////////////////////////////////////////////////////
	//##2013/11/4 ///////////////////////////////////////////////////
	void Population2Front( ) ;

	////////////////////////////////////////////////////////////
public:
	int GenNum;//进化代数
	int curGenNum;//当前进化代数
	double Pc;//交叉概率
	double Pm;//变异概率
	int popSize ;//种群个数
	population parent_pop;//种群
	//vector<vector<double>> parato_front ;

private:
	double oldrand[55];
	int jrand;
};

#endif // !defined(AFX_MYGA_H__664085B3_C966_4971_AECE_7E6A0F800536__INCLUDED_)
