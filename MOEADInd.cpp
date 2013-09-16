#include "stdafx.h"
#include "MOEADInd.h"
#include "Chart.h"

extern Fitness fit[1000] ;

CIndividual::CIndividual()
{
	//x_var = vector<int>(nvar, 0);
	y_obj = vector<double>(nobj, 0);
	rank = 1;
}

CIndividual::~CIndividual()
{
	x_var.clear();
	y_obj.clear();
}

void CIndividual::rnd_init( )
{
///////////////////////////////////////////////////////////////////
//##Modified on 2013/9/10 ///////////////////////////////////
	x_var.clear();
	int j = rand() % (chart_height+chart_width) ;

	if(j < 3){ //ֻ�������յ�

		x_var.push_back(0);
		x_var.push_back(out_num);

	}else{

		x_var.push_back(0);//�������
		//���������յ�֮���ֵ
		for( int k = 1; k < j - 1 ; ++k ){

			//��������ĳ�ʼ����Ⱥ��Ҳ�������ظ���
			//������Init֮��Ҫ����GenDelSame
			int n;									// x, y;
			n = rand()%out_num;
			x_var.push_back(n);
		}
		x_var.push_back(out_num);//�����յ�
	}

	//for( int n = 0 ; n < nvar ; n++ )
	//	x_var[n] = lowBound[n] +
	//	rnd_uni(&rnd_uni_init)*(uppBound[n] - lowBound[n]);   

	//obj_eval(n) ;
//////////////////////////////////////////////////////////////////////
}

void CIndividual::obj_eval( const int& index )
{
	int n = this->x_var.size() ;

	double A = evaluate_length(n) ;
	double B = evaluate_smoothness(n) ;
	double C = evaluate_security(n) ;

	int D = 0;
	int curX, curY, nextX, nextY ;
	double kX, kY, nextk ;
	int x1, x2, y1, y2 ;
	int count = 0 ;

	for ( int i = 0 ; i < n - 1 ; i++ ) {

		x1 = x_var[i] % chart_width ;
		y1 = x_var[i] / chart_width ;
		x2 = x_var[i+1] % chart_width ;
		y2 = x_var[i+1] / chart_width ;

		curX = x1 ;
		curY = y1 ;

		do {

			if(x2==x1){//#�����˶�

				if( y2 > y1 ) { 
					
					nextY = curY + 1 ;
					nextX = curX ;
				} else { 
					
					nextY = curY - 1 ;
					nextX = curX ;
				}
			}else if( y2 == y1 ) {//#�����˶�

				if( x2 > x1 ) {
					
					nextX = curX + 1 ;
					nextY = curY ;
				} else {
					
					nextX = curX - 1 ;
					nextY = curY ;
				}
			}else{ //#б���˶�

				if( x2 > x1) nextX = curX+1;
				else nextX = curX-1;

				if( y2>y1) nextY = curY+1;
				else nextY = curY-1;

				if( x2< x1 ) nextX++;
				if( y2 < y1 ) nextY++;

				kX = (nextX-x1-0.5)/(x2-x1);
				kY = (nextY-y1-0.5)/(y2-y1);
				nextk = min(kX,kY);

				nextX = floor(x1+0.5+nextk*(x2-x1));
				nextY = floor(y1+0.5+nextk*(y2-y1));

				if(x2<x1&&kX<=kY) nextX--;
				if(y2<y1&&kY<=kX) nextY--;
			}
			if( nextY >= 0 && //#Added on 2013/9/11
				chart[nextY][nextX] == 1)//���ͨ�����ϰ���
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

	double len = sqrt((double)( (chart_width-1) * (chart_width-1)*2) );
	fit[index].A = len/A;

	if(B == 0)
		fit[index].B = 1;
	else
		fit[index].B = 1/B;

	if(C == 0)
		fit[index].C = 0;
	else
		fit[index].C = 1/C;

	y_obj[2] = A + 100000*D;					//����
	y_obj[1] = B + 100000*D;		    		//ƽ����
	y_obj[0] = -C + 100000*D;					//��ȫ��

	return;
}

double CIndividual::evaluate_length(const int& n ) {

	double A = 0.0 ;

	//#�Գ��ȵ�����
	for ( int j = 1; j < n; j++ ){

		//ǰһ����͵�ǰ����դ��ͼ�ϵ�����
		int x1,x2,y1,y2;
		x1 = x_var[j-1] % chart_width ;
		y1 = x_var[j-1] / chart_width ;
		x2 = x_var[j] % chart_width ;
		y2 = x_var[j] / chart_width ;
		A += sqrt( (double)(x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );//����֮��ľ���
	}

	return A ;
}

double CIndividual::evaluate_smoothness(const int& n ) {

	double B = 0.0 ;

	//#�Ը���⻬�ȵ����� 
	for (int j = 1; j < n-1; j++){

		//ǰһ����͵�ǰ����դ��ͼ�ϵ�����
		double angle, len1,len2;
		int x1, x2, y1, y2, x3, y3, 
			vectorx1, vectory1, vectorx2, vectory2 ;

		x1 = x_var[j-1] % chart_width;
		y1 = x_var[j-1] / chart_width;
		x2 = x_var[j] % chart_width;
		y2 = x_var[j] / chart_width;
		x3 = x_var[j+1] % chart_width;
		y3 = x_var[j+1] / chart_width;

		vectorx1 = x2 - x1 ;
		vectory1 = y2 - y1 ;
		vectorx2 = x3 - x2 ;
		vectory2 = y3 - y2 ;

		len1 = sqrt((double)(vectorx1*vectorx1+vectory1*vectory1));
		len2 = sqrt((double)(vectorx2*vectorx2+vectory2*vectory2));

		angle = (vectorx1*vectorx2+vectory1*vectory2) / (len1*len2);//���������ļн�
		angle = acos(angle);
		B += angle;
	}

	return B ;
}

double CIndividual::evaluate_security(  const int& n  ){

	double C = 0.0 ;

	//#�Ը��尲ȫ�Ե�����
	for(int j = 0; j < n-1; ++j){

		//��ǰ����դ��ͼ�ϵ�����
		int x1, y1, x2, y2, m = 0;
		double len = 0.0;
		double L = 0.0;
		x1 = x_var[j]%chart_width;
		y1 = x_var[j]/chart_width;
		vector<int> vecChart;

		for(int i = 0; i < chart_width; ++i)
			for(int k = 0; k < chart_width; ++k)
				if(chart[i][k] == 1)
					vecChart.push_back(i*chart_width + k);

		for(int i = 0; i < vecChart.size(); ++i){

			x2 = vecChart[i]%chart_width;
			y2 = vecChart[i]/chart_width;
			len = sqrt((double)((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)));
			if( check( this, ((y2-1)*chart_width+x2)) && check( this,(y2*chart_width+x2-1)) )
				m += 1;
			else if(check( this, ((y2+1)*chart_width+x2)) && check( this, (y2*chart_width+x2-1) ))
				m += 1;
			else if(check( this, ((y2+1)*chart_width+x2)) && check( this, (y2*chart_width+x2+1) ))
				m += 1;
			else if(check( this, ((y2-1)*chart_width+x2)) && check( this, (y2*chart_width+x2+1) ))
				m += 1;
			L += len;
			L = L - m * sqrt((double)(chart_width*chart_width));//���гͷ�
			if(L < 0)
				L = -L;

		}
		C = L / vecChart.size();
	}

	return C ;
}

bool CIndividual::check( CIndividual const* ind, const int& i )
{
	for(int k = 0; k < ind->x_var.size() ; ++k )
	{
		if( ind->x_var[k] == i)
			return true;
	}
	return false;
}
//
//void CIndividual::show_objective()
//{
//	for(int n=0; n<nobj; n++)
//		printf("%f ",y_obj[n]);
//	printf("\n");
//}

//void CIndividual::show_variable()
//{
//	for(int n=0; n<nvar; n++)
//		printf("%f ",x_var[n]);
//	printf("\n");
//}

void CIndividual::operator=(const CIndividual &ind2)
{
	x_var = ind2.x_var;
	y_obj = ind2.y_obj;
	rank  = ind2.rank;
}

bool CIndividual::operator<(const CIndividual &ind2)
{
	bool dominated = true;
	for(int n=0; n<nobj; n++)
	{
		if(ind2.y_obj[n]<y_obj[n]) return false;
	}
	if(ind2.y_obj==y_obj) return false;
	return dominated;
}


bool CIndividual::operator<<(const CIndividual &ind2)
{
	bool dominated = true;
	for(int n=0; n<nobj; n++)
	{
		if(ind2.y_obj[n]<y_obj[n]  - 0.0001) return false;
	}
	if(ind2.y_obj==y_obj) return false;
	return dominated;
}

bool CIndividual::operator==(const CIndividual &ind2)
{
	if(ind2.y_obj==y_obj) return true;
	else return false;
}

TCompare CIndividual::Compare(CIndividual& ind2) {
	bool bBetter = false;
	bool bWorse = false;

	int i = 0;
	do { 
		if(y_obj[i] < ind2.y_obj[i])
			bBetter = true;
		if(ind2.y_obj[i] < y_obj[i])
			bWorse = true;
		i++;
	}while (!(bWorse && bBetter) && (i < nobj));
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


// defining subproblem 


TSOP::TSOP()
{
}

TSOP::~TSOP()
{
}


void TSOP::operator=(const TSOP&sub2)
{
	indiv  = sub2.indiv;
	table  = sub2.table;
	namda  = sub2.namda;
	array  = sub2.array;
}



//#endif

