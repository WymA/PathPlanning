// MyGA.cpp: implementation of the MyGA class.
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

Fitness fit[1000];
Para m_CurPara;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MyGA::MyGA(Para &param)
{
	m_bAlreadyStarted = false;
	m_bAlreadyRun = false;
	curGenNum = 0;
	//初始化参数
	Init(param);
	nobj = 3;
	//初始化int **chart
	chart = new int* [chartHeight];

	for ( int i = 0; i < chartHeight ; ++i )
		chart[i] = new int[chartWidth];

	for ( int i = 0; i < chartHeight ; ++i )
		for ( int j = 0; j < chartWidth; ++j )
			chart[i][j] = 0; //初始时没有障碍物
}

MyGA::~MyGA()
{
	if (chart){

		for (int i = 0; i < chartHeight; ++i)
			delete [] chart[i];
		delete [] chart;
	}
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


void MyGA::Init(Para &para)
{
	//修改种群个体数
	setPopSize(para.pSize); 
	//修改交叉概率
	setPCross(para.propC);
	//修改变异概率
	setPMutation(para.propM);
	//修改进化代数
	setGenNum(para.T);

	//chartWidth和chartHeight的具体值根据栅格的大小做改变
	chartWidth = para.width;
	chartHeight = para.height;
	outNum = chartHeight*chartWidth - 1;

}

//生成初始化种群
void MyGA::InitPop(population *pop)
{
	for( int i = 0; i < popSize ; ++i){

		InitInd(pop->ind[i]);
		GenDelSame(pop->ind[i].xPath);//删除个体中相同的部分
	}
}

void MyGA::InitInd(individual &ind)
{

	ind.xPath.clear();
	int j = rand() % (chartHeight+chartWidth) ;

	if(j < 3){//只有起点和终点

		ind.xPath.push_back(0);
		ind.xPath.push_back(outNum);

	}else{

		ind.xPath.push_back(0);//加入起点
		//加入起点和终点之间的值
		for(int k = 1; k < j-1; ++k){

			//这里产生的初始化种群中也可能有重复点
			//所以在Init之后要调用GenDelSame
			int n; // x, y;
			n = rand()%outNum;
			ind.xPath.push_back(n);
		}
		ind.xPath.push_back(outNum);//加入终点
	}
}

//删除算子
void MyGA::GenPopDelSame(population *pop)
{
	for(int i = 0; i < popSize; ++i)
		GenDelSame(pop->ind[i].xPath);
}
//删除染色体中相同基因之间的部分
void MyGA::GenDelSame(vector<int>& xPath)
{
	int startPoint;//intVec::iterator startIter;//搜索开始的位置
	int findPoint;//intVec::iterator findIter;//找到重复点的位置
	bool findFlag;
	int k;
	for( int j = 0 ; j < ( xPath.size() - 1 ) ; ++j )
	{
		int curVal = xPath[j];
		startPoint = j+1;
		findFlag = false;
		for(k = xPath.size()-1; k > j; --k) {

			if(xPath[k] == curVal){

				findPoint = k;
				findFlag = true;
				break;
			}
		}
		if(findFlag){

			for(int i = k+1; i < xPath.size(); ++i)
				xPath[j+i-k] = xPath[i];

			for(int i = 0; i < k-j; ++i)
				xPath.pop_back();
		}
	}
}

void MyGA::GetPopBestObj(int iObj, vector<int> &path)
{
	//int startPoint;
	//int i ; //, j;
	double curBestiObjVal;
	int curBestiObjIdx;

	for( int i = 0; i < popSize; ++i ){

		if ( parent_pop.ind[i].rank == 1 ){

			curBestiObjVal = parent_pop.ind[ i ].obj[ iObj ] ;
			curBestiObjIdx = i ;
			break;
		}
	}

	for( int i = curBestiObjIdx + 1; i < popSize; ++i) {

		if ( parent_pop.ind[i].rank == 1 && parent_pop.ind[i].obj[ iObj ] < curBestiObjVal ) {

			curBestiObjVal = parent_pop.ind[i].obj[iObj] ;
			curBestiObjIdx = i ;
		}
	}

	path = parent_pop.ind[curBestiObjIdx].xPath;

}

//选择算子
void MyGA::GenSelection(population *old_pop, population *new_pop)
{
	int *a1, *a2;
	int temp, rand;
	individual *parent1, *parent2;
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
individual* MyGA::Tournament(individual *ind1, individual  *ind2)
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
void MyGA::randomize()
{
	for(int j1 = 0; j1 <= 54; ++j1)
		this->oldrand[j1] = 0.0;

	this->jrand = 0;
	warmup_random((unsigned)time(NULL));
	return;
}
/* Get randomize off and running */
void MyGA::warmup_random(double seed)
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
void MyGA::advance_random ()
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
double MyGA::randomperc( )
{
	++jrand;
	if( jrand >= 55 )	{

		jrand = 1;
		advance_random();
	}
	return ( (double)oldrand[jrand] );
}

/* Fetch a single random integer between low and high including the bounds */
int MyGA::rnd ( int low, int high )
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
double MyGA::rndreal ( double low, double high )
{
	return (low + (high-low)*randomperc());
}

//交叉基因
void MyGA::GenCross( individual *parent1, individual *parent2, individual *child1, individual *child2)
{
	double p = rand()%1000/1000.0;
	int Parent1Length = parent1->xPath.size(); //第一个以及第二个父个体长度
	int Parent2Length = parent2->xPath.size(); //

	child1->xPath.clear();
	child2->xPath.clear();

	if ( p < Pc ) {

		//随机产生第一个以及第二个父个体需要交叉的一位,不能是第一位（起点）     
		int Parent1CrossPoint, Parent2CrossPoint;

		//这里要注意避免起点 
		Parent1CrossPoint = 1+rand()%(Parent1Length-1); 
		Parent2CrossPoint = 1+rand()%(Parent2Length-1); 

		for (int j = 0; j < Parent1CrossPoint; ++j)
			child1->xPath.push_back( parent1->xPath[j] );
		for (int j = Parent2CrossPoint; j < Parent2Length; ++j)
			child1->xPath.push_back( parent2->xPath[j] );

		for (int j = 0; j < Parent2CrossPoint; ++j)
			child2->xPath.push_back( parent2->xPath[j] );
		for (int j = Parent1CrossPoint; j < Parent1Length; ++j)
			child2->xPath.push_back( parent1->xPath[j] );
	
	}else{

		for (int j = 0; j < Parent1Length; ++j)
			child1->xPath.push_back( parent1->xPath[j] );
		for (int j = 0; j < Parent2Length; ++j)
			child2->xPath.push_back( parent2->xPath[j] );
	}
}

//变异算子
void MyGA::GenMutation(population *pop)
{
	for(int i = 0; i < popSize; ++i)
		GenMutationInd(pop->ind[i]);
}

void MyGA::GenMutationInd(individual &ind)
{
	double p = rand()%1000/1000.0;
	if( p < Pm && ind.xPath.size() > 2){

		//随机产生变异的位置，不能为头和尾
		int mutPoint;
		mutPoint = 1 + rand()%(ind.xPath.size() - 2);
		int bSeries = GenIsSeries(ind, mutPoint);
		//若该点与前后两点连续则不变异
		if(bSeries != 2){

			//随机变异出一位数，可以取道起点和终点，这里可能出错
			//变异出的不可以是障碍物，如果是障碍物则不变异
			int newVal = rand()%(outNum+1);
			int x = newVal%chartWidth;
			int y = newVal/chartWidth;

			if(chart[y][x] == 0)
				ind.xPath[mutPoint] = newVal;
		}
	}
}

//0-不连续，1-与前一个连续，2-与前后都连续
int MyGA::GenIsSeries(individual &ind, const int idx)
{
	int pre, cur, next;
	pre = ind.xPath[idx-1];
	cur = ind.xPath[idx];

	if(idx+1 < ind.xPath.size())
		next = ind.xPath[idx+1];
	else
		next = ind.xPath[idx];

	int x0, y0, x1, y1, x2, y2;
	x0 = pre%chartWidth;
	y0 = pre/chartWidth;
	x1 = cur%chartWidth;
	y1 = cur/chartWidth;
	x2 = next%chartWidth;
	y2 = next/chartWidth;

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
void MyGA::GenInsert(population *pop)
{
	for(int i = 0; i < popSize; ++i){

		int len = pop->ind[i].xPath.size();
		int insertResult;
		for(int j = 1; j < len; ){

			insertResult = GenInsertInd(pop->ind[i], j);

			if(insertResult == 0)
				j += 1;
			else
				j += 2;
			len = pop->ind[i].xPath.size();

		}
	}
}


int MyGA::GenInsertInd(individual &ind, int idx)
{
	//如果和前一位连续则不插，插入中值法计算出的点
	int bSeries = GenIsSeries(ind, idx);
	if( bSeries == 0 )	{

		int pre = ind.xPath[idx-1];
		int cur = ind.xPath[idx];
		int x0, y0, x1, y1, x2, y2;
		int x3= 0, y3 = 0;//真正要插入的序号
		x1 = pre%chartWidth;
		y1 = pre/chartWidth;
		x2 = cur%chartWidth;
		y2 = cur/chartWidth;
		x0 = (x1 + x2)/2;

		if(abs(y2 - y1) == 1)
			y0 = (y1 + y2 + 1) / 2;
		else
			y0 = (y1 + y2) / 2;

		//非障碍物点，可以插入
		if(chart[y0][x0] == 0){

			x3 = x0;
			y3 = y0;
		}else{//障碍物点，找到离该点最近的非障碍物点插入

			int ySpan = max(y0, (chartHeight - 1 - y0));
			double preMinDist, curMinDist;
			//将最短距离设置为一个大的初值
			preMinDist = curMinDist = MAX_DIST;
			//搜索当前行
			curMinDist = SearchLineNearest(x0, y0, y0, preMinDist, x3);
			//在当前行中找到点

			if(curMinDist < MAX_DIST){

				y3 = y0;
			} else {

				//分别搜索上面的行和下面的行
				for(int j = 0; j <= ySpan; ++j)
				{
					if((y0 - j) >= 0)
					{
						curMinDist = SearchLineNearest(x0, y0, y0 - j, preMinDist, x3);
						if(curMinDist < preMinDist)
						{
							y3 = y0;
							preMinDist = curMinDist;
						}
					}
					if((y0 + j) < chartHeight)
					{
						curMinDist = SearchLineNearest(x0, y0, y0 + j, preMinDist, x3);
						if(curMinDist < preMinDist)
						{
							y3 = y0;
							preMinDist = curMinDist;
						}
					}

					if(curMinDist < MAX_DIST)
						break;

				}
			}
		}

		int ins = x3 + chartWidth*y3;
		//有效的插入点

		if(ins != 0 && ins != ind.xPath[idx]){

			intVec::iterator iter = ind.xPath.begin() + idx;
			ind.xPath.insert(iter, ins);
			return 1;
		}
	}

	return 0;
}

//搜索当前行最短距离
//x0,y0：要替换点的 x,y坐标
//curLin：当前扫描行的行值
//curMin：未扫描该行前最短距离
//minX：该行最短距离点的x坐标
double MyGA::SearchLineNearest( const int x0, const int y0, const int curLin, const double preMin, int& minX )
{
	int xSpan = max( x0, (chartWidth-1-x0) );
	double tempDist, curMin;//#tempDist stands for temporary distance value?
	curMin = preMin;

	for( int k = 0; k <= xSpan; ++k ){

		if( ((x0-k) >= 0) && (chart[curLin][x0 - k] == 0) ){

			tempDist = sqrt((double)(k*k + (curLin-y0)*(curLin-y0)));
			if(curMin > tempDist){

				minX = x0 - k;
				curMin = tempDist;
			}
		}
		if( ((x0+k) < chartWidth) && (chart[curLin][x0+k] == 0) ){

			tempDist = sqrt((double)(k*k + (curLin-y0)*(curLin-y0)));
			if(curMin > tempDist)	{

				minX = x0 + k;
				curMin = tempDist;
			}
		}

		//已找到该行上的最近距离点
		if(curMin < preMin)
			return curMin;

	}

	//没找到
	return preMin;
}

//对种群进行评估
void MyGA::Evaluate(population *pop)
{
	for(int i = 0; i < popSize; ++i)
		EvaluateInd(pop->ind[i], i);
}

//对个体进行评估
void MyGA::EvaluateInd(individual &ind, int index )
{
	double A = 0.0, B = 0.0, C = 0.0;
	int n = ind.xPath.size() ;

	//#对长度的评价
	for ( int j = 1; j < n; j++ ){

		//前一个点和当前点在栅格图上的坐标
		int x1,x2,y1,y2;
		x1 = ind.xPath[j-1] % chartWidth ;
		y1 = ind.xPath[j-1] / chartWidth ;
		x2 = ind.xPath[j] % chartWidth ;
		y2 = ind.xPath[j] / chartWidth ;
		A += sqrt( (double)(x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );//两点之间的距离
	}

	//#对个体光滑度的评价 
	for (int j = 1; j < n-1; j++){

		//前一个点和当前点在栅格图上的坐标
		double angle, len1,len2;
		int x1, x2, y1, y2, x3, y3, 
			vectorx1, vectory1, vectorx2, vectory2 ;

		x1 = ind.xPath[j-1] % chartWidth;
		y1 = ind.xPath[j-1] / chartWidth;
		x2 = ind.xPath[j] % chartWidth;
		y2 = ind.xPath[j] / chartWidth;
		x3 = ind.xPath[j+1] % chartWidth;
		y3 = ind.xPath[j+1] / chartWidth;

		vectorx1 = x2 - x1 ;
		vectory1 = y2 - y1 ;
		vectorx2 = x3 - x2 ;
		vectory2 = y3 - y2 ;

		len1 = sqrt((double)(vectorx1*vectorx1+vectory1*vectory1));
		len2 = sqrt((double)(vectorx2*vectorx2+vectory2*vectory2));

		angle = (vectorx1*vectorx2+vectory1*vectory2)/(len1*len2);//计算向量的夹角
		angle = acos(angle);
		B += angle;
	}

	//#对个体安全性的评价
	for(int j = 0; j < n-1; ++j){

		//当前点在栅格图上的坐标
		int x1, y1, x2, y2, m = 0;
		double len = 0.0;
		double L = 0.0;
		x1 = ind.xPath[j]%chartWidth;
		y1 = ind.xPath[j]/chartWidth;
		vector<int> vecChart;

		for(int i = 0; i < chartWidth; ++i)
			for(int k = 0; k < chartWidth; ++k)
				if(chart[i][k] == 1)
					vecChart.push_back(i*chartWidth + k);

		for(int i = 0; i < vecChart.size(); ++i){

			x2 = vecChart[i]%chartWidth;
			y2 = vecChart[i]/chartWidth;
			len = sqrt((double)((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)));
			if(check(ind,((y2-1)*chartWidth+x2)) && check(ind,(y2*chartWidth+x2-1)))
				m += 1;
			else if(check(ind,((y2+1)*chartWidth+x2)) && check(ind,(y2*chartWidth+x2-1)))
				m += 1;
			else if(check(ind,((y2+1)*chartWidth+x2)) && check(ind,(y2*chartWidth+x2+1)))
				m += 1;
			else if(check(ind,((y2-1)*chartWidth+x2)) && check(ind,(y2*chartWidth+x2+1)))
				m += 1;
			L += len;
			L = L - m * sqrt((double)(chartWidth*chartWidth));//进行惩罚
			if(L < 0)
				L = -L;

		}
		C = L / vecChart.size();
	}

	int D = 0;
	int curX, curY, nextX, nextY ;
	double kX, kY, nextk ;
	int x1, x2, y1, y2 ;
	int count = 0 ;

	for ( int i = 0 ; i < n - 1 ; i++ ){

		x1 = ind.xPath[i] % chartWidth;
		y1 = ind.xPath[i] / chartWidth;
		x2 = ind.xPath[i+1] % chartWidth;
		y2 = ind.xPath[i+1] / chartWidth;

		curX = x1 ;
		curY = y1 ;

		do{

			if(x2==x1){

				if(y2>y1) {nextY = curY+1; nextX = curX;}
				else {nextY = curY-1; nextX = curX; }
			}else if(y2==y1){

				if(x2>x1) {nextX = curX+1; nextY = curY;}
				else {nextX = curX-1; nextY = curY; }
			}else{

				if(x2>x1) nextX = curX+1;
				else nextX = curX-1;

				if(y2>y1) nextY = curY+1;
				else nextY = curY-1;

				if(x2<x1) nextX++;
				if(y2<y1) nextY++;

				kX = (nextX-x1-0.5)/(x2-x1);
				kY = (nextY-y1-0.5)/(y2-y1);
				nextk = min(kX,kY);

				nextX = floor(x1+0.5+nextk*(x2-x1));
				nextY = floor(y1+0.5+nextk*(y2-y1));

				if(x2<x1&&kX<=kY) nextX--;
				if(y2<y1&&kY<=kX) nextY--;
			}
			if(chart[nextY][nextX] == 1)//如果通过了障碍物
				D++;
			curX = nextX; curY = nextY;
			count++;

			if(count>1000){

				nextX=x2;
				nextY=y2;
			}
		}while(( !( nextX == x2 && nextY == y2 )));
		count=0;
	}

	double len = sqrt((double)( (chartWidth-1) * (chartWidth-1)*2) );
	fit[index].A = len/A;

	if(B == 0)
		fit[index].B = 1;
	else
		fit[index].B = 1/B;

	if(C == 0)
		fit[index].C = 0;
	else
		fit[index].C = 1/C;

	ind.obj[2] = A + 100000*D;//长度
	ind.obj[1] = B + 100000*D;//平滑度
	ind.obj[0] = -C + 100000*D;//安全性

	return;
}

/////////////////////////////////////////////////////////////////////
////#Function to check the ind wether is in the ind or not/////
/////////////////////////////////////////////////////////////////////
bool MyGA::check(individual &ind, int i)
{
	for(int k = 0; k < ind.xPath.size(); ++k){

		if( ind.xPath[k] == i )
			return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//// Function to assign rank and crowding distance to a population of size pop_size//
//////////////////////////////////////////////////////////////////////////////////////////////////
void MyGA::assign_rank_and_crowding_distance(population *new_pop, int popSize)
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
void MyGA::assign_crowding_distance_list(population *pop, list *lst, int front_size)
{
	int **obj_array;
	int *dist;
	int i,j;
	list *temp;
	temp = lst;
	obj_array = new int*[nobj];
	dist = new int[front_size];
	for(i = 0; i < nobj; ++i)
		obj_array[i] = new int[front_size];
	for(j = 0; j < front_size; ++j)
	{
		dist[j] = temp->index;
		temp = temp->child;
	}
	assign_crowding_distance(pop, dist, obj_array, front_size);
	delete dist;
	for(i = 0; i < nobj; ++i)
		delete obj_array[i];
	delete obj_array;
}

/* Routine to compute crowding distance based on objective function values when the population in in the form of an array */
void MyGA::assign_crowding_distance_indices(population *pop, int c1, int c2)
{
	int **obj_array;
	int *dist;
	int i, j;
	int front_size;
	front_size = c2 - c1 +1;
	obj_array = new int*[nobj];
	dist = new int[front_size];

	for(i = 0; i < nobj; ++i)
		obj_array[i] = new int[front_size];

	for(j = 0; j < front_size; ++j)
		dist[j] = c1++;

	assign_crowding_distance(pop, dist, obj_array, front_size);
	delete dist;

	for(i = 0; i < nobj; ++i)
		delete obj_array[i];

	delete obj_array;
}
/* Routine to compute crowding distances */
void MyGA::assign_crowding_distance(population *pop, int *dist, int **obj_array, int front_size)
{
	int i, j;

	for(i = 0; i < nobj; ++i)
	{
		for(j = 0; j < front_size; ++j)
			obj_array[i][j] = dist[j];
		quicksort_front_obj(pop, i, obj_array[i], front_size);
	}

	for(j = 0; j < front_size; ++j)
		pop->ind[dist[j]].crowd_dist = 0.0;

	for(i = 0; i < nobj; ++i)
		pop->ind[obj_array[i][0]].crowd_dist = INF;

	for(i = 0; i < nobj; ++i)
	{
		for(j = 1; j < front_size-1; ++j)
		{
			if(pop->ind[obj_array[i][j]].crowd_dist != INF)
			{
				if(pop->ind[obj_array[i][front_size-1]].obj[i] == pop->ind[obj_array[i][0]].obj[i])
					pop->ind[obj_array[i][j]].crowd_dist += 0.0;
				else
					pop->ind[obj_array[i][j]].crowd_dist += (pop->ind[obj_array[i][j+1]].obj[i] - pop->ind[obj_array[i][j-1]].obj[i])/(pop->ind[obj_array[i][front_size-1]].obj[i] - pop->ind[obj_array[i][0]].obj[i]);
			}
		}
	}

	for(j = 0; j < front_size; ++j)
	{
		if(pop->ind[dist[j]].crowd_dist != INF)
			pop->ind[dist[j]].crowd_dist = (pop->ind[dist[j]].crowd_dist)/nobj;
	}

}

int MyGA::check_dominance(individual *a, individual *b)
{
	int i, flag1, flag2;
	flag1 = 0;
	flag2 = 0;

	for(i = 0; i < nobj; ++i){

		if(a->obj[i] < b->obj[i] ){

			flag1 = 1;
		}else{

			if( a->obj[i] > b->obj[i] )
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

void MyGA::fill_nondominated_sort(population *mixed_pop, population *new_pop, int popSize)
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

void MyGA::crowding_fill(population *mixed_pop, population *new_pop, int count, int front_size, list *elite, int popSize)
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
void MyGA::insert(list *node, int x)
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
list* MyGA::del(list *node)
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
void MyGA::merge(population *pop1, population *pop2, population *pop3, int popSize)
{
	int i, k;

	for(i = 0; i < popSize; ++i)
		copy_ind(&(pop1->ind[i]), &(pop3->ind[i]));

	for(i = 0, k = popSize; i < popSize; ++i, ++k)
		copy_ind(&(pop2->ind[i]), &(pop3->ind[k]));

	return;
}


void MyGA::copy_ind(individual *ind1, individual *ind2)
{
	int i;
	ind2->rank = ind1->rank;
	ind2->crowd_dist = ind1->crowd_dist;
	ind2->xPath.clear();

	for(i = 0; i < ind1->xPath.size(); ++i)
		ind2->xPath.push_back(ind1->xPath[i]);

	for(i = 0; i < nobj; ++i)
		ind2->obj[i] = ind1->obj[i];
	return;
}

void MyGA::quicksort_front_obj(population *pop, int objcount, int obj_array[], int obj_array_size)
{
	q_sort_front_obj(pop, objcount, obj_array, 0, obj_array_size-1);
}

void MyGA::q_sort_front_obj(population *pop, int objcount, int obj_array[], int left, int right)
{
	int index, temp, i, j ;
	double pivot;

	if( left < right ){

		index = rnd( left, right ) ;
		temp = obj_array[right] ;
		obj_array[right] = obj_array[index];
		obj_array[index] = temp;
		pivot = pop->ind[obj_array[right]].obj[objcount];
		i = left - 1;
		for(j = left; j <right; ++j){

			if(pop->ind[obj_array[j]].obj[objcount] <= pivot){

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

void MyGA::quicksort_dist(population *pop, int *dist, int front_size)
{
	q_sort_dist(pop, dist, 0, front_size-1);
}

void MyGA::q_sort_dist(population *pop, int *dist, int left, int right)
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

void MyGA::allocate_memory_pop( population *pop, int size )
{
	//int i;
	pop->ind = new individual[size];
	for( int i = 0; i < size; ++i )
		pop->ind[i].obj = new double[nobj];
	return ;
}

void MyGA::deallocate_memory_pop(population *pop, int size)
{
	//int i;
	for( int i = 0; i < size; ++i)
		delete[] (pop->ind[i].obj);//出错误
	delete[] (pop->ind);
	return;
}

void MyGA::report_pop(population *pop, FILE *fpt, int popSize)
{
	//int i, j;
	for ( int i=0; i<popSize; i++ ){

		for ( int j=0; j<nobj; j++ ){

			fprintf(fpt,"%e\t",pop->ind[i].obj[j]);
		}
		fprintf(fpt,"%d\t",pop->ind[i].rank);
		fprintf(fpt,"%e\n",pop->ind[i].crowd_dist);
	}
	return;
}


int MyGA::gamain(CRobotView* m_myView)
{
	//////////////////////////////////////////////////////////
	//#Initialize sth.////////////////////////////////////////
	//////////////////////////////////////////////////////////
	this->curGenNum = 0;

	FILE *fpt1;
	FILE *fpt2;
	FILE *fpt3;
	FILE *fpt4;

	population child_pop;
	population mixed_pop;

	fpt1 = fopen("initial_pop.out", "w");
	fpt2 = fopen("final_pop.out", "w");
	fpt3 = fopen("all_pop.out", "w");
	fpt4 = fopen("params.out","w");

	//////////////////////////////////////////////////////////
	//#If population size is too small OR is not times of 4
	//#Then exit
	this->popSize = m_CurPara.pSize;
	if( this->popSize < 4 || ( this->popSize%4 ) != 0 ){

		exit(1);
	}
	this->nobj = 3;

	////////////////////////////////////////////////////////////////
	//#Do sth. about file output////////////////////////////////

	fprintf(fpt4, "\n Population size = %d", popSize);
	fprintf(fpt4, "\n Number of generations = %d", GenNum);
	fprintf(fpt4, "\n Number of objective functions = %d", nobj);
	fprintf(fpt4, "\n Probability of crossover of real variable = %e", Pc);
	fprintf(fpt4, "\n Probability of mutation of real variable = %e", Pm);
	fprintf(fpt1, "# of objectives = %d\n", nobj);
	fprintf(fpt2, "# of objectives = %d\n", nobj);
	fprintf(fpt3, "# of objectives = %d\n", nobj);
	////////////////////////////////////////////////////////////////

	//#Allocate memory to population with population size 
	allocate_memory_pop(&parent_pop, popSize);
	allocate_memory_pop(&child_pop, popSize);
	allocate_memory_pop(&mixed_pop, 2*popSize);
	randomize();

	//#Initialize the parameter
	this->Init(m_CurPara);

	InitPop(&parent_pop);
	Evaluate(&parent_pop); 

	//#assign the parent population
	assign_rank_and_crowding_distance(&parent_pop, popSize);

	//#find the best Obj path based on 3 objs respectivefully
	GetPopBestObj(0, path1);
	GetPopBestObj(1, path2);
	GetPopBestObj(2, path3);
	m_bAlreadyStarted = true;
	m_bAlreadyRun = true;

	m_myView->Invalidate();
	m_myView->UpdateWindow(); 


	curGenNum = 1; //#current generation number = 1
	report_pop(&parent_pop, fpt1, popSize);
	fprintf(fpt3, "# gen = 1\n");
	report_pop(&parent_pop, fpt3, popSize);
	fflush(stdout);
	fflush(fpt1);
	fflush(fpt2);
	fflush(fpt3);
	fflush(fpt4);
	////////////////////////////////////////////////////////////////////////////
	//#1st generation done///////////////////////////////////////////////

	int dstTime, srcTime;

	for( curGenNum = 2; curGenNum <= GenNum; ++curGenNum ){	
		//#Begin with curGenNum=2; A big loop

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

		fprintf(fpt3, "# gen = %d\n", curGenNum);
		report_pop(&parent_pop, fpt3, popSize);
		fflush(fpt3);

		GetPopBestObj(0, path1);
		GetPopBestObj(1, path2);
		GetPopBestObj(2, path3);

		//====================
		m_myView->Invalidate();
		m_myView->UpdateWindow();

		dstTime = srcTime = GetTickCount();
		do{
			dstTime = GetTickCount();
		}while((dstTime-srcTime) <= 100);
	}
	--curGenNum ;

	report_pop(&parent_pop, fpt2, popSize);

	GetPopBestObj(0, path1);
	GetPopBestObj(1, path2);
	GetPopBestObj(2, path3);

	//======================
	m_myView->Invalidate();
	m_myView->UpdateWindow();

	fflush(stdout);
	fflush(fpt1);
	fflush(fpt2);
	fflush(fpt3);
	fflush(fpt4);
	fclose(fpt1);
	fclose(fpt2);
	fclose(fpt3);
	fclose(fpt4);

	deallocate_memory_pop(&child_pop, popSize);
	deallocate_memory_pop(&mixed_pop, 2*popSize);

	return 0;
}
//
//
////////////////////////////////////////////////
////#moead intialise function/////////////
////////////////////////////////////////////////
//void MyGA::InitMOEAD()
//{
//	MOEAD = new TMOEAD() ;
//
//}
//
////////////////////////////////////////////////
////#moead entrance function/////////////
////////////////////////////////////////////////
//void MyGA::MOEADMain() 
//{
//	this->popSize = m_CurPara.pSize ;
//	
//	//#tmp//////////////////////////////
//	int neighborSize = 20 ;
//	int run = 1 ;
//	vector<double> hvl;          //define two vectors to record the hyper-volume and IGD 
//	vector<double> igd;
//	double unevolvetime = 0;					//#Unevolved time?
//	double totaltime = 0;
//	////////////////////////////////////
//
//	MOEAD->execute( this->popSize,
//		neighborSize,
//		this->GenNum,
//		run,
//		hvl,
//		igd,
//		unevolvetime,
//		totaltime
//		) ;
//
//
//}