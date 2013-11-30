// NSGA2.cpp: implementation of the NSGA2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Robot.h"
#include "RobotDoc.h"
#include "RobotView.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <time.h>
#include <math.h>
#include <string.h>
#include <algorithm>
using namespace std;

//#define max(a, b) ((a > b) ? a : b)
//#define min(a, b) ((a < b) ? a : b)


////////////////////////////////////////////////////////////////////
// Individual ///////////////////////////////////////////////////

Individual::Individual()
{
}

TCompare Individual::Compare(Individual& ind2)
{

	bool bBetter = false;
	bool bWorse = false;

	int i = 0;
	do { 

		if(y_obj[i] < ind2.y_obj[i])
			bBetter = true;
		if(ind2.y_obj[i] < y_obj[i])
			bWorse = true;
		i++;

	}while (!(bWorse && bBetter) && (i < kObjNum));

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






//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NSGA2::NSGA2()
{
	curGenNum = 0;
	
}

NSGA2::~NSGA2()
{
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


void NSGA2::Init(Para &para)
{
	//修改种群个体数
	setPopSize(para.pSize); 
	//修改交叉概率
	setPCross(para.pop_cross_rate);
	//修改变异概率
	setPMutation(para.pop_mutation_rate);
	//修改进化代数
	setGenNum(para.total_gen);

	//chart_width和chart_height的具体值根据栅格的大小做改变
	//chart_width = para.width;
	//chart_height = para.height;
	//out_num = chart_height*chart_width - 1;
	//parato_front = vector<vector<double>>(popSize, vector<double>(kObjNum, 0.0)) ;
}

//生成初始化种群
void NSGA2::InitPop(population *pop)
{
	for( int i = 0; i < popSize ; ++i){

		InitInd(pop->ind[i]);
		GenDelSame(pop->ind[i].x_var);//删除个体中相同的部分
	}
}

void NSGA2::InitInd(Individual &ind)
{

	ind.x_var.clear();
	int j = rand() % (chart_height+chart_width) ;

	if(j < 3){//只有起点和终点

		ind.x_var.push_back(0);
		ind.x_var.push_back(out_num);

	}else{

		ind.x_var.push_back(0);//加入起点
		//加入起点和终点之间的值
		for( int k = 1; k < j - 1 ; ++k ){

			//这里产生的初始化种群中也可能有重复点
			//所以在Init之后要调用GenDelSame
			int n;									// x, y;
			n = rand()%out_num;
			ind.x_var.push_back(n);
		}
		ind.x_var.push_back(out_num);//加入终点
	}
}

//删除算子
void NSGA2::GenPopDelSame(population *pop)
{
	for(int i = 0; i < popSize; ++i)
		GenDelSame(pop->ind[i].x_var);
}
//删除染色体中相同基因之间的部分
void NSGA2::GenDelSame(vector<int>& x_var)
{
	int startPoint;//intVec::iterator startIter;//搜索开始的位置
	int findPoint;//intVec::iterator findIter;//找到重复点的位置
	bool findFlag;
	int k;
	for( int j = 0 ; j < ( x_var.size() - 1 ) ; ++j )
	{
		int curVal = x_var[j];
		startPoint = j+1;
		findFlag = false;
		for(k = x_var.size()-1; k > j; --k) {

			if(x_var[k] == curVal){

				findPoint = k;
				findFlag = true;
				break;
			}
		}
		if(findFlag){

			for(int i = k+1; i < x_var.size(); ++i)
				x_var[j+i-k] = x_var[i];

			for(int i = 0; i < k-j; ++i)
				x_var.pop_back();
		}
	}
}

void NSGA2::GetPopBestObj(int iObj, vector<int> &path)
{
	//int startPoint;
	//int i ; //, j;
	double cur_best_obj_value;
	int cur_best_obj_index;

	for( int i = 0; i < popSize; ++i ){

		if ( parent_pop.ind[i].rank == 1 ){

			cur_best_obj_value = parent_pop.ind[i].y_obj[iObj] ;
			cur_best_obj_index = i ;
			break;
		}
	}

	for( int i = cur_best_obj_index + 1; i < popSize; ++i) {

		if ( parent_pop.ind[i].rank == 1 && parent_pop.ind[i].y_obj[iObj] < cur_best_obj_value ) {

			cur_best_obj_value = parent_pop.ind[i].y_obj[iObj] ;
			cur_best_obj_index = i ;
		}
	}

	path = parent_pop.ind[cur_best_obj_index].x_var;
}

//选择算子
void NSGA2::GenSelection(population *old_pop, population *new_pop)
{
	int *a1, *a2;
	int temp, rand;
	Individual *parent1, *parent2;
	a1 = new int[popSize];
	a2 = new int[popSize];

	for( int i = 0; i < popSize; ++i)
		a1[i] = a2[i] = i;

	for( int i = 0; i < popSize; ++i ){

		rand = rnd(i, popSize-1);
		temp = a1[rand];
		a1[rand] = a1[i];
		a1[i] = temp;

		rand = rnd(i, popSize-1);
		temp = a2[rand];
		a2[rand] = a2[i];
		a2[i] = temp;

	}

	for( int i = 0; i < popSize; i += 4){

		parent1 = Tournament(&old_pop->ind[a1[i]], &old_pop->ind[a1[i+1]] );
		parent2 = Tournament(&old_pop->ind[a1[i+2]], &old_pop->ind[a1[i+3]] );
		GenCross( parent1, parent2, &new_pop->ind[i], &new_pop->ind[i+1] );

		parent1 = Tournament(&old_pop->ind[a2[i]], &old_pop->ind[a2[i+1]] );
		parent2 = Tournament(&old_pop->ind[a2[i+2]], &old_pop->ind[a2[i+3]] );
		GenCross( parent1, parent2, &new_pop->ind[i+2], &new_pop->ind[i+3] );

	}

	free(a1);
	free(a2);

}
//锦标赛
Individual* NSGA2::Tournament(Individual *ind1, Individual  *ind2)
{
	int flag;
	flag = check_dominance(ind1, ind2);

	if(flag == 1)
		return ind1;
	if(flag == -1)
		return ind2;

	if(ind1->crowd_dist > ind2->crowd_dist)
		return ind1;
	if(ind2->crowd_dist > ind1->crowd_dist)
		return ind2;

	if(randomperc() <= 0.5)
		return ind1;
	else
		return ind2;
}

/* Get seed number for random and start i	t up */
void NSGA2::randomize()
{
	for(int j1 = 0; j1 <= 54; ++j1)
		this->oldrand[j1] = 0.0;

	this->jrand = 0;
	warmup_random((unsigned)time(NULL));
	return;
}
/* Get randomize off and running */
void NSGA2::warmup_random(double seed)
{
	int j1, ii;
	double new_random, prev_random;
	oldrand[54] = seed;
	new_random = 0.000000001;
	prev_random = seed;
	for(j1 = 1; j1 <= 54; ++j1)
	{
		ii = (21*j1)%54;
		oldrand[ii] = new_random;
		new_random = prev_random - new_random;
		if(new_random < 0.0)
			new_random += 1.0;
		prev_random = oldrand[ii];
	}
	advance_random();
	advance_random();
	advance_random();
	jrand = 0;
}
/* Create next batch of 55 random numbers */
void NSGA2::advance_random ()
{
	double new_random;
	for( int j1 = 0 ; j1 < 24 ; j1++ )	{

		new_random = oldrand[j1]-oldrand[j1+31];
		if( new_random < 0.0 )	{

			new_random = new_random+1.0;
		}
		oldrand[j1] = new_random;
	}
	for( int j1 = 24 ; j1 < 55 ; j1++ ) {

		new_random = oldrand[j1]-oldrand[j1-24];
		if( new_random < 0.0 ){

			new_random = new_random+1.0;
		}
		oldrand[j1] = new_random;
	}
}

/* Fetch a single random number between 0.0 and 1.0 */
double NSGA2::randomperc( )
{
	++jrand;
	if( jrand >= 55 )	{

		jrand = 1;
		advance_random();
	}
	return ( (double)oldrand[jrand] );
}

/* Fetch a single random integer between low and high including the bounds */
int NSGA2::rnd ( int low, int high )
{
	int res;
	if (low >= high){

		res = low;
	}else{

		res = low + ( randomperc()*( high - low + 1 ) ) ;
		if ( res > high ){

			res = high;
		}
	}
	return res;
}
/* Fetch a single random real number between low and high including the bounds */
double NSGA2::rndreal ( double low, double high )
{
	return (low + (high-low)*randomperc());
}

//交叉基因
void NSGA2::GenCross( Individual *parent1, Individual *parent2, Individual *child1, Individual *child2)
{
	double p = rand()%1000/1000.0;
	int Parent1Length = parent1->x_var.size(); //第一个以及第二个父个体长度
	int Parent2Length = parent2->x_var.size(); //

	child1->x_var.clear();
	child2->x_var.clear();

	if ( p < Pc ) {

		//随机产生第一个以及第二个父个体需要交叉的一位,不能是第一位（起点）     
		int Parent1CrossPoint, Parent2CrossPoint;

		//这里要注意避免起点 
		Parent1CrossPoint = 1+rand()%(Parent1Length-1); 
		Parent2CrossPoint = 1+rand()%(Parent2Length-1); 

		for (int j = 0; j < Parent1CrossPoint; ++j)
			child1->x_var.push_back( parent1->x_var[j] );
		for (int j = Parent2CrossPoint; j < Parent2Length; ++j)
			child1->x_var.push_back( parent2->x_var[j] );

		for (int j = 0; j < Parent2CrossPoint; ++j)
			child2->x_var.push_back( parent2->x_var[j] );
		for (int j = Parent1CrossPoint; j < Parent1Length; ++j)
			child2->x_var.push_back( parent1->x_var[j] );
	
	}else{

		for (int j = 0; j < Parent1Length; ++j)
			child1->x_var.push_back( parent1->x_var[j] );
		for (int j = 0; j < Parent2Length; ++j)
			child2->x_var.push_back( parent2->x_var[j] );
	}
}

//变异算子
void NSGA2::GenMutation(population *pop)
{
	for(int i = 0; i < popSize; ++i)
		GenMutationInd(pop->ind[i]);
}

void NSGA2::GenMutationInd(Individual &ind)
{
	double p = rand()%1000/1000.0;
	if( p < Pm && ind.x_var.size() > 2){

		//随机产生变异的位置，不能为头和尾
		int mut_point;
		mut_point = 1 + rand()%(ind.x_var.size() - 2);
		int bSeries = GenIsSeries(ind, mut_point);
		//若该点与前后两点连续则不变异
		if(bSeries != 2){

			//随机变异出一位数，可以取道起点和终点，这里可能出错
			//变异出的不可以是障碍物，如果是障碍物则不变异
			int new_val = rand()%(out_num+1);

			if(chart[new_val] == 0)
				ind.x_var[mut_point] = new_val;
		}
	}
}

//0-不连续，1-与前一个连续，2-与前后都连续
int NSGA2::GenIsSeries(Individual &ind, const int idx)
{
	int pre, cur, next;
	pre = ind.x_var[idx-1];
	cur = ind.x_var[idx];

	if(idx+1 < ind.x_var.size())
		next = ind.x_var[idx+1];
	else
		next = ind.x_var[idx];

	int x0, y0, x1, y1, x2, y2;
	x0 = pre%chart_width;
	y0 = pre/chart_width;
	x1 = cur%chart_width;
	y1 = cur/chart_width;
	x2 = next%chart_width;
	y2 = next/chart_width;

	int c2p = max(abs(x1-x0), abs(y1-y0));
	int c2n = max(abs(x2-x1), abs(y2-y1));

	if((c2n == 1) && (c2p == 1))
		return 2;
	else if(c2p == 1)
		return 1;
	else
		return 0;
}

//插入算子
void NSGA2::GenInsert(population *pop)
{
	for(int i = 0; i < popSize; ++i){

		int len = pop->ind[i].x_var.size();
		int insertResult;
		for(int j = 1; j < len; ){

			insertResult = GenInsertInd(pop->ind[i], j);

			if(insertResult == 0)
				j += 1;
			else
				j += 2;
			len = pop->ind[i].x_var.size();

		}
	}
}


int NSGA2::GenInsertInd(Individual &ind, int idx)
{
	//如果和前一位连续则不插，插入中值法计算出的点
	int bSeries = GenIsSeries(ind, idx);
	if( bSeries == 0 )	{

		int pre = ind.x_var[idx-1];
		int cur = ind.x_var[idx];
		int x0, y0, x1, y1, x2, y2;
		int x3= 0, y3 = 0;//真正要插入的序号
		x1 = pre%chart_width;
		y1 = pre/chart_width;
		x2 = cur%chart_width;
		y2 = cur/chart_width;
		x0 = (x1 + x2)/2;

		if(abs(y2 - y1) == 1)
			y0 = (y1 + y2 + 1) / 2;
		else
			y0 = (y1 + y2) / 2;

		//非障碍物点，可以插入
		if(chart[y0*chart_width + x0] == 0){

			x3 = x0;
			y3 = y0;
		}else{//障碍物点，找到离该点最近的非障碍物点插入

			int ySpan = max(y0, (chart_height - 1 - y0));
			double preMinDist, curMinDist;
			//将最短距离设置为一个大的初值
			preMinDist = curMinDist = kMaxDist;
			//搜索当前行
			curMinDist = SearchRowNearest(x0, y0, y0, preMinDist, x3);
			//在当前行中找到点

			if(curMinDist < kMaxDist){

				y3 = y0;
			} else {

				//分别搜索上面的行和下面的行
				for(int j = 0; j <= ySpan; ++j)
				{
					if((y0 - j) >= 0)
					{
						curMinDist = SearchRowNearest(x0, y0, y0 - j, preMinDist, x3);
						if(curMinDist < preMinDist)
						{
							y3 = y0;
							preMinDist = curMinDist;
						}
					}
					if((y0 + j) < chart_height)
					{
						curMinDist = SearchRowNearest(x0, y0, y0 + j, preMinDist, x3);
						if(curMinDist < preMinDist)
						{
							y3 = y0;
							preMinDist = curMinDist;
						}
					}

					if(curMinDist < kMaxDist)
						break;

				}
			}
		}

		int ins = x3 + chart_width*y3;
		//有效的插入点

		if(ins != 0 && ins != ind.x_var[idx]){

			intVec::iterator iter = ind.x_var.begin() + idx;
			ind.x_var.insert(iter, ins);
			return 1;
		}
	}

	return 0;
}

//搜索当前行最短距离
//x0,y0：要替换点的 x,y坐标
//curLin：当前扫描行的行值
//curMin：未扫描该行前最短距离
////minX：该行最短距离点的x坐标
//double NSGA2::SearchRowNearest( const int x0, const int y0, const int curLin, const double preMin, int& minX )
//{
//	int xSpan = max( x0, (chart_width-1-x0) );
//	double tempDist, curMin;//#tempDist stands for temporary distance value?
//	curMin = preMin;
//
//	for( int k = 0; k <= xSpan; ++k ){
//
//		if( ((x0-k) >= 0) && (chart[curLin][x0 - k] == 0) ){
//
//			tempDist = sqrt((double)(k*k + (curLin-y0)*(curLin-y0)));
//			if(curMin > tempDist){
//
//				minX = x0 - k;
//				curMin = tempDist;
//			}
//		}
//		if( ((x0+k) < chart_width) && (chart[curLin][x0+k] == 0) ){
//
//			tempDist = sqrt((double)(k*k + (curLin-y0)*(curLin-y0)));
//			if(curMin > tempDist)	{
//
//				minX = x0 + k;
//				curMin = tempDist;
//			}
//		}
//
//		//已找到该行上的最近距离点
//		if(curMin < preMin)
//			return curMin;
//
//	}
//
//	//没找到
//	return preMin;
//}

//对种群进行评估
void NSGA2::Evaluate(population *pop)
{
	for(int i = 0; i < popSize; ++i)
		EvaluateInd(pop->ind[i], i);
}

//对个体进行评估
void NSGA2::EvaluateInd(Individual &ind, int index )
{
	
	ObjEval( ind.x_var, index, ind.y_obj ) ;

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//// Function to assign rank and crowding distance to a population of size pop_size//
//////////////////////////////////////////////////////////////////////////////////////////////////
void NSGA2::assign_rank_and_crowding_distance(population *new_pop, int popSize)
{
	int flag,/* i, */end, front_size, rank = 1;
	list* orig, * cur, * temp1, * temp2;
	orig = new list;
	cur = new list;
	front_size = 0;
	orig->index = -1;
	orig->parent = NULL;
	orig->child = NULL;
	cur->index = -1;
	cur->parent = NULL;
	cur->child = NULL;
	temp1 = orig;

	for( int i = 0; i < popSize ; ++i ){

		insert( temp1, i ) ;
		temp1 = temp1->child;
	}
	do{

		if( orig->child->child == NULL )	{

			new_pop->ind[orig->child->index].rank = rank;
			new_pop->ind[orig->child->index].crowd_dist = INF;
			break;
		}

		temp1 = orig->child;
		insert(cur, temp1->index);
		front_size = 1;
		temp2 = cur->child;
		temp1 = del(temp1);
		temp1 = temp1->child;

		do{

			temp2 = cur->child;

			do{
				end = 0;
				flag = check_dominance(&(new_pop->ind[temp1->index]), &(new_pop->ind[temp2->index]));
				if(flag == 1)
				{
					insert(orig, temp2->index);
					temp2 = del(temp2);
					--front_size;
					temp2 = temp2->child;
				}
				if (flag == 0)
				{
					temp2 = temp2->child;
				}
				if (flag == -1)
				{
					end = 1;
				}
			}while(end != 1 && temp2 != NULL);

			if (flag == 0 || flag == 1)
			{
				insert (cur, temp1->index);
				front_size++;
				temp1 = del (temp1);
			}
			temp1 = temp1->child;

		}while(temp1 != NULL);

		temp2 = cur->child;
		do{

			new_pop->ind[temp2->index].rank = rank;
			temp2 = temp2->child;
		}while (temp2 != NULL);

		assign_crowding_distance_list (new_pop, cur->child, front_size);
		temp2 = cur->child;

		do{

			temp2 = del (temp2);
			temp2 = temp2->child;
		}while (cur->child !=NULL);

		rank+=1;
	}

	while (orig->child!=NULL);

	free (orig);
	free (cur);
}

/* Routine to compute crowding distance based on ojbective function values when the population in in the form of a list */
void NSGA2::assign_crowding_distance_list(population *pop, list *lst, int front_size)
{
	int **obj_array;
	int *dist;
	int i,j;
	list *temp;
	temp = lst;
	obj_array = new int*[kObjNum];
	dist = new int[front_size];
	for(i = 0; i < kObjNum; ++i)
		obj_array[i] = new int[front_size];
	for(j = 0; j < front_size; ++j)
	{
		dist[j] = temp->index;
		temp = temp->child;
	}
	assign_crowding_distance(pop, dist, obj_array, front_size);
	delete dist;
	for(i = 0; i < kObjNum; ++i)
		delete obj_array[i];
	delete obj_array;
}

/* Routine to compute crowding distance based on objective function values when the population in in the form of an array */
void NSGA2::assign_crowding_distance_indices(population *pop, int c1, int c2)
{
	int **obj_array;
	int *dist;
	int i, j;
	int front_size;
	front_size = c2 - c1 +1;
	obj_array = new int*[kObjNum];
	dist = new int[front_size];

	for(i = 0; i < kObjNum; ++i)
		obj_array[i] = new int[front_size];

	for(j = 0; j < front_size; ++j)
		dist[j] = c1++;

	assign_crowding_distance(pop, dist, obj_array, front_size);
	delete dist;

	for(i = 0; i < kObjNum; ++i)
		delete obj_array[i];

	delete obj_array;
}
/* Routine to compute crowding distances */
void NSGA2::assign_crowding_distance(population *pop, int *dist, int **obj_array, int front_size)
{
	int i, j;

	for(i = 0; i < kObjNum; ++i)
	{
		for(j = 0; j < front_size; ++j)
			obj_array[i][j] = dist[j];
		quicksort_front_obj(pop, i, obj_array[i], front_size);
	}

	for(j = 0; j < front_size; ++j)
		pop->ind[dist[j]].crowd_dist = 0.0;

	for(i = 0; i < kObjNum; ++i)
		pop->ind[obj_array[i][0]].crowd_dist = INF;

	for(i = 0; i < kObjNum; ++i)
	{
		for(j = 1; j < front_size-1; ++j)
		{
			if(pop->ind[obj_array[i][j]].crowd_dist != INF)
			{
				if(pop->ind[obj_array[i][front_size-1]].y_obj[i] == pop->ind[obj_array[i][0]].y_obj[i])
					pop->ind[obj_array[i][j]].crowd_dist += 0.0;
				else
					pop->ind[obj_array[i][j]].crowd_dist += (pop->ind[obj_array[i][j+1]].y_obj[i] - pop->ind[obj_array[i][j-1]].y_obj[i])/(pop->ind[obj_array[i][front_size-1]].y_obj[i] - pop->ind[obj_array[i][0]].y_obj[i]);
			}
		}
	}

	for(j = 0; j < front_size; ++j)
	{
		if(pop->ind[dist[j]].crowd_dist != INF)
			pop->ind[dist[j]].crowd_dist = (pop->ind[dist[j]].crowd_dist)/kObjNum;
	}

}

int NSGA2::check_dominance(Individual *a, Individual *b)
{
	int i, flag1, flag2;
	flag1 = 0;
	flag2 = 0;

	for(i = 0; i < kObjNum; ++i){

		if(a->y_obj[i] < b->y_obj[i] ){

			flag1 = 1;
		}else{

			if( a->y_obj[i] > b->y_obj[i] )
				flag2 = 1;
		}
	}

	if( flag1 == 1 && flag2 == 0 ){

		return 1;
	}else{

		if( flag1 == 0 && flag2 == 1 )
			return -1;
		else
			return 0;
	}
}

void NSGA2::fill_nondominated_sort(population *mixed_pop, population *new_pop, int popSize)
{
	int flag, i, j, end, front_size, archieve_size, rank = 1;
	list *pool, *elite, *temp1, *temp2;
	pool = new list;
	elite = new list;
	front_size = 0;
	archieve_size = 0;
	pool->index = -1;
	pool->parent = NULL;
	pool->child = NULL;
	elite->parent = NULL;
	elite->child = NULL;
	temp1 = pool;

	for(i = 0; i < 2*popSize; ++i){

		insert(temp1, i);
		temp1 = temp1->child;
	}
	i = 0;

	do{

		temp1 = pool->child;
		insert(elite, temp1->index);
		front_size = 1;
		temp2 = elite->child;
		temp1 = del(temp1);
		temp1 = temp1->child;

		do{

			temp2 = elite->child;
			if(temp1 == NULL)
				break;
			do{
				end = 0;
				flag = check_dominance(&(mixed_pop->ind[temp1->index]), &(mixed_pop->ind[temp2->index]));
				if( flag == 1){

					insert(pool, temp2->index);
					temp2 = del(temp2);
					--front_size;
					temp2 = temp2->child;
				}

				if(flag == 0)
					temp2 = temp2->child;
				if(flag == -1)
					end = 1;

			}while (end != 1 && temp2 != NULL) ;

			if(flag == 0 || flag == 1){

				insert(elite, temp1->index);
				++front_size;
				temp1 = del(temp1);
			}
			temp1 = temp1->child;

		}while(temp1 != NULL);

		temp2 = elite->child;
		j = i ;
		if((archieve_size+front_size) <= popSize){

			do{

				copy_ind(&mixed_pop->ind[temp2->index], &new_pop->ind[i]);
				new_pop->ind[i].rank = rank;
				++archieve_size;
				temp2 = temp2->child;
				++i;
			}while(temp2 != NULL);

			assign_crowding_distance_indices(new_pop, j, i-1);
			++rank;
		} else {

			crowding_fill(mixed_pop, new_pop, i, front_size, elite, popSize);
			archieve_size = popSize;
			for(j = i; j < popSize; ++j)
				new_pop->ind[j].rank = rank;

		}
		temp2 = elite->child;
		do{

			temp2 = del(temp2);
			temp2 = temp2->child;
		}while(elite->child != NULL);

	}while(archieve_size < popSize);

	while(pool != NULL){

		temp1 = pool;
		pool = pool->child;
		delete temp1;
	}

	while(elite != NULL){

		temp1 = elite;
		elite = elite->child;
		delete temp1;
	}
	return;
}

void NSGA2::crowding_fill(population *mixed_pop, population *new_pop, int count, int front_size, list *elite, int popSize)
{
	int *dist, i, j ;
	list *temp;
	assign_crowding_distance_list(mixed_pop, elite->child, front_size);
	dist = new int[front_size];
	temp = elite->child;

	for(j = 0; j < front_size; ++j){

		dist[j] = temp->index;
		temp = temp->child;
	}

	quicksort_dist(mixed_pop, dist, front_size);
	for(i = count, j = front_size-1; i < popSize; ++i, --j)
		copy_ind(&mixed_pop->ind[dist[j]], &new_pop->ind[i]);

	delete dist;

	return;
}

/* Insert an element X into the list at location specified by NODE */
void NSGA2::insert(list *node, int x)
{
	list *temp;
	if(node == NULL){

		exit(1);
	}

	temp = new list;
	temp->index = x;
	temp->child = node->child;
	temp->parent = node;

	if(node->child != NULL){

		node->child->parent = temp;
	}
	node->child =temp;
	return;
}

/* Delete the node NODE from the list */
list* NSGA2::del(list *node)
{
	list *temp;
	if(node == NULL)
		exit(1);

	temp = node->parent;
	temp->child = node->child;

	if(temp->child != NULL)
		temp->child->parent = temp;

	delete node;
	return temp;
}

/* Routine to merge two populations into one */
void NSGA2::merge(population *pop1, population *pop2, population *pop3, int popSize)
{
	int i, k;

	for(i = 0; i < popSize; ++i)
		copy_ind(&(pop1->ind[i]), &(pop3->ind[i]));

	for(i = 0, k = popSize; i < popSize; ++i, ++k)
		copy_ind(&(pop2->ind[i]), &(pop3->ind[k]));

	return;
}


void NSGA2::copy_ind(Individual *ind1, Individual *ind2)
{
	int i;
	ind2->rank = ind1->rank;
	ind2->crowd_dist = ind1->crowd_dist;
	ind2->x_var.clear();

	for(i = 0; i < ind1->x_var.size(); ++i)
		ind2->x_var.push_back(ind1->x_var[i]);

	for(i = 0; i < kObjNum; ++i)
		ind2->y_obj[i] = ind1->y_obj[i];
	return;
}

void NSGA2::quicksort_front_obj(population *pop, int objcount, int obj_array[], int obj_array_size)
{
	q_sort_front_obj(pop, objcount, obj_array, 0, obj_array_size-1);
}

void NSGA2::q_sort_front_obj(population *pop, int objcount, int obj_array[], int left, int right)
{
	int index, temp, i, j ;
	double pivot;

	if( left < right ){

		index = rnd( left, right ) ;
		temp = obj_array[right] ;
		obj_array[right] = obj_array[index];
		obj_array[index] = temp;
		pivot = pop->ind[obj_array[right]].y_obj[objcount];
		i = left - 1;
		for(j = left; j <right; ++j){

			if(pop->ind[obj_array[j]].y_obj[objcount] <= pivot){

				++i ;
				temp = obj_array[j] ;
				obj_array[j] = obj_array[i] ;
				obj_array[i] = temp ;
			}
		}
		index = i + 1;
		temp = obj_array[index];
		obj_array[index] = obj_array[right];
		obj_array[right] = temp;
		q_sort_front_obj(pop, objcount, obj_array, left, index-1);
		q_sort_front_obj (pop, objcount, obj_array, index+1, right);

	}
}

void NSGA2::quicksort_dist(population *pop, int *dist, int front_size)
{
	q_sort_dist(pop, dist, 0, front_size-1);
}

void NSGA2::q_sort_dist(population *pop, int *dist, int left, int right)
{
	int index, temp, i, j ;
	double pivot ;
	if ( left < right ){

		index = rnd(left, right);
		temp = dist[right];
		dist[right] = dist[index];
		dist[index] = temp;
		pivot = pop->ind[dist[right]].crowd_dist;
		i = left-1;
		for (j=left; j<right; j++)
		{
			if (pop->ind[dist[j]].crowd_dist <= pivot)
			{
				i+=1;
				temp = dist[j];
				dist[j] = dist[i];
				dist[i] = temp;
			}
		}
		index=i+1;
		temp = dist[index];
		dist[index] = dist[right];
		dist[right] = temp;
		q_sort_dist (pop, dist, left, index-1);
		q_sort_dist (pop, dist, index+1, right);

	}
}

void NSGA2::allocate_memory_pop( population *pop, int size )
{
	//int i;
	pop->ind = new Individual[size];
	for( int i = 0; i < size; ++i )
		pop->ind[i].y_obj = vector<double>(kObjNum, 0.0 );
	return ;
}

void NSGA2::deallocate_memory_pop(population *pop, int size)
{
	//int i;
	for( int i = 0; i < size; ++i)
		pop->ind[i].y_obj.clear() ;
	delete[] (pop->ind);
	return;
}

void NSGA2::report_pop(population *pop, FILE *fpt, int popSize)
{
	//int i, j;
	for ( int i=0; i<popSize; i++ ){

		for ( int j=0; j<kObjNum; j++ ){

			fprintf(fpt,"%e\t",pop->ind[i].y_obj[j]);
		}
		fprintf(fpt,"%d\t",pop->ind[i].rank);
		fprintf(fpt,"%e\n",pop->ind[i].crowd_dist);
	}
	return;
}

void NSGA2::Population2Front( )
{
	vector<int> nDominated;
	for(int n=0;n<popSize ;n++)
		nDominated.push_back(0);

	for(int k=0; k<popSize; k++){

		for(int j=k+1; j<popSize; j++){

			//TCompare tresult = ParetoCompare(parent_pop.ind[k].y_obj, parent_pop.ind[j].y_obj);
			
			TCompare tresult = ParetoCompare( total_fitness[k], total_fitness[j]);

			if( tresult == _Pareto_Dominated ){
				nDominated[k]++;
			}else 
				if(tresult == _Pareto_Dominating)
					nDominated[j]++;
		}
	}
	dominating_fitness.clear() ;
	dominated_fitness.clear() ;

	for(int n=0;n<popSize;n++){

		if(nDominated[n] == 0)
			dominating_fitness.push_back( total_fitness[n] ) ;
		else
			dominated_fitness.push_back( total_fitness[n] ) ;
	}

	nDominated.clear();
}


int NSGA2::Execute(CRobotView* m_myView)
{
	//////////////////////////////////////////////////////////
	//#Initialize sth.////////////////////////////////////////
	//////////////////////////////////////////////////////////
	this->curGenNum = 0;
	Init(cur_parameter);

	//fo.open("test_myga_evl.txt", std::ios::out);

	population child_pop;
	population mixed_pop;

	this->popSize = cur_parameter.pSize;
	if( this->popSize < 4 || ( this->popSize%4 ) != 0 ){

		exit(1);
	}

	//#Allocate memory to population with population size 
	allocate_memory_pop(&parent_pop, popSize);
	allocate_memory_pop(&child_pop, popSize);
	allocate_memory_pop(&mixed_pop, 2*popSize);
	randomize();

	//#Initialize the parameter
	this->Init(cur_parameter);

	InitPop(&parent_pop);
	Evaluate(&parent_pop); 

	//#assign the parent population
	assign_rank_and_crowding_distance(&parent_pop, popSize);

	//#find the best Obj path based on 3 objs respectivefully
	GetPopBestObj(0, path_security);//#security
	GetPopBestObj(1, path_smoothness);//#smoothness
	GetPopBestObj(2, path_length);//#length

	m_myView->Invalidate();
	m_myView->UpdateWindow(); 


	curGenNum = 1; //#current generation number = 1

	int dst_time, src_time;
	Population2Front( ) ;

	for( curGenNum = 2;  run_state != kStop && curGenNum <= GenNum; ++curGenNum ){	
		//#Begin with curGenNum=2; length_fitness big loop

		////////////////////////////////////////////////////////////////////////////
		//#Evolving//////////////////////////////////////////////////////////////
		GenSelection(&parent_pop, &child_pop); 
		GenMutation(&child_pop);
		GenInsert(&child_pop);
		GenPopDelSame(&child_pop);
		Evaluate(&child_pop);
		merge(&parent_pop, &child_pop, &mixed_pop, popSize);
		fill_nondominated_sort(&mixed_pop, &parent_pop, popSize);

		////////////////////////////////////////////////////////////////////////////
		Population2Front() ;

		GetPopBestObj(0, path_security);
		GetPopBestObj(1, path_smoothness);
		GetPopBestObj(2, path_length);
		GetBestFit( kLength ) ;
		GetBestFit( kSmoothness ) ;
		GetBestFit( kSecurity ) ;

		//====================
		m_myView->Invalidate();
		m_myView->UpdateWindow();

		dst_time = src_time = GetTickCount();
		do{
			dst_time = GetTickCount();
		}while((dst_time-src_time) <= run_speed);
	}
	--curGenNum ;

	GetPopBestObj(0, path_security);
	GetPopBestObj(1, path_smoothness);
	GetPopBestObj(2, path_length);
	GetBestFit( kLength ) ;
	GetBestFit( kSmoothness ) ;
	GetBestFit( kSecurity ) ;

	//======================
	m_myView->Invalidate();
	m_myView->UpdateWindow();

	deallocate_memory_pop(&child_pop, popSize);
	deallocate_memory_pop(&mixed_pop, 2*popSize);

	return 0;
}