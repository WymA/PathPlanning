#include "stdafx.h"
#include "global.h"



//------------- Parameters in test instance ------------------

//int     /*nvar,  */kObjNum;                    //  the number of variables and objectives

double cross_rate ;
double mutation_rate ;

////////////////////////////////////////////////////////////////
//##Modified on 2013/9/11 ////////////////////////////////

vector<int> path_length ;
vector<int> path_security ;
vector<int> path_smoothness ;

////////////////////////////////////////////////////////////////////////
vector<vector<double>> total_fitness ;
vector<vector<double>> dominated_fitness ;
vector<vector<double>> dominating_fitness ;
//double  lowBound = 0,   uppBound = 1;   //  lower and upper bounds of variables
double  lowBound[kVarMaxLength], uppBound[kVarMaxLength];

//char    strTestInstance[256];     //the name of the test BOP problem 

TDecompositionType    tDecompType = _Decomposition_TCH1;

int total_gen = 0 ;
double divide = 0.8;

double length_pun  ;

Fitness fit ;
Para cur_parameter;
int run_state = kStop ;

int run_speed = 100 ;

queue<Fitness> avg ;

int obj1 = 0,
	obj2 = 1 ;

//------------- Parameters in random number ------------------
int     seed    = 177;
long    rnd_uni_init;        


//------------- Parameters in MOEA/D -------------------------

vector <double> idealpoint;
double          scale[100];  

double xHypervolumeRefPoint_TopRight = 1;
double yHypervolumeRefPoint_TopRight = 1;

double xHypervolumeRefPoint_BottomLeft =0;
double yHypervolumeRefPoint_BottomLeft =0;

fstream fo;
bool optimal_only = false ; 

///15 20
int		etax    =  20 , 	etam    = 20;   // distribution indexes of crossover and mutation
//the random generator in [0,1)         a special method                                      
double rnd_uni(long *idum)
{

	long j;
	long k;
	static long idum2=123456789;
	static long iy=0;
	static long iv[NTAB];

	if ( *idum <= 0 ){

		if (-(*idum) < 1) *idum=1;
		else *idum = -(*idum);
		idum2=(*idum);
		for (j=NTAB+7;j>=0;j--){

			k = (*idum)/IQ1 ;
			*idum = IA1*(*idum-k*IQ1)-k*IR1;
			if ( *idum < 0 ) *idum += IM1;
			if ( j < NTAB ) iv[j] = *idum;
		}
		iy=iv[0];
	}

	k=(*idum)/IQ1;
	*idum=IA1*(*idum-k*IQ1)-k*IR1;
	if (*idum < 0) *idum += IM1;

	k=idum2/IQ2 ;
	idum2=IA2*(idum2-k*IQ2)-k*IR2;

	if (idum2 < 0) idum2 += IM2;

	j = iy / NDIV ;
	iy = iv[j] - idum2 ;
	iv[j] = *idum ;

	if (iy < 1) iy += IMM1 ;

	double temp ;
	if (( temp = AM*iy ) > RNMX ) return RNMX ;
	else return temp;

}/*------End of rnd_uni()--------------------------*/


TCompare ParetoCompare( const vector<double>& y_obj1, const vector<double>& y_obj2)	//judge the pareto dominated
{  

	bool bBetter = false;
	bool bWorse = false;

	if(y_obj1[obj1] < y_obj2[obj1])
		bBetter = true;
	else if(y_obj2[obj1] < y_obj1[obj1])
		bWorse = true;

	if(y_obj1[obj2] < y_obj2[obj2])
		bBetter = true;
	else if(y_obj2[obj2] < y_obj1[obj2])
		bWorse = true;

	if (bWorse) {

		if (bBetter)
			return _Pareto_Nondominated; //#Worse and better
		else
			return _Pareto_Dominated; //#Worse and not better
	} else {

		if (bBetter) 
			return _Pareto_Dominating; //#Not worse and better
		else 
			return _Pareto_Equal; //#Not worse and not better
	}
}
TCompare ParetoCompare(const double* y_obj1, const double* y_obj2) //judge the pareto dominated
{  

	bool bBetter = false;
	bool bWorse = false;

	if(y_obj1[obj1] < y_obj2[obj1])
		bBetter = true;
	if(y_obj2[obj1] < y_obj1[obj1])
		bWorse = true;

	if(y_obj1[obj2] < y_obj2[obj2])
		bBetter = true;
	if(y_obj2[obj2] < y_obj1[obj2])
		bWorse = true;

	if (bWorse) {

		if (bBetter)
			return _Pareto_Nondominated; //#Worse and better
		else
			return _Pareto_Dominated; //#Worse and not better
	} else {

		if (bBetter)
			return _Pareto_Dominating; //#Not worse and better
		else
			return _Pareto_Equal; //#Not worse and not better
	}
}

void QuickSortIncreasing(vector <vector<double>> &paretofront, int nPoints)
{
	double temp;
	for(int i=0; i<nPoints; i++)
	{
		for(int j=i+1; j<nPoints; j++)
			if(paretofront[i][0]>paretofront[j][0])
			{
				temp = paretofront[i][0];
				paretofront[i][0]        = paretofront[j][0];
				paretofront[j][0]        = temp;

				temp = paretofront[i][1];
				paretofront[i][1]        = paretofront[j][1];
				paretofront[j][1]        = temp;
			}
	}
}

double hv2(vector <vector<double>> &paretofront)
{
	double volume = 0;
	//int j=0;
	//while(j<paretofront.size() && paretofront[j][1] >= yHypervolumeRefPoint_TopRight )
	//{
	//	j++;
	//}
	if( paretofront.size() > 0 ) {
		volume = ((xHypervolumeRefPoint_TopRight - paretofront[0][0]  ) * 
			( yHypervolumeRefPoint_TopRight - paretofront[0][1] )); 
	}
	for (int i = 1; i < paretofront.size(); i++) { // && paretofront[i][0]<xHypervolumeRefPoint_TopRight
		volume += ( (xHypervolumeRefPoint_TopRight - paretofront[i][0]  ) * 
			( paretofront[i - 1][1] - paretofront[i][1] ) ) ;
	}

	return volume;
}

void SelectObj() 
{	
	if ( cur_parameter.length ) {

		obj1 = kLength ;

		if ( cur_parameter.security ) {

			obj2 = kSecurity ;
		}else {

			obj2 = kSmoothness ;
		}
	}else{

		obj1 = kSmoothness ;
		obj2 = kSecurity ;
	}
}


///////////////////////////////////////////////////////////////////////////
//##Added on 2013/9/22 /////////////////////////////////////////////
//void flush_fitness() 
//{
//	for ( int i = 0 ; i < cur_parameter.pSize ; i++ ) {
//
//		fit.clear() ;
//	}
//	fit.clear() ;
//	fit.security_fitness = 1.0 ;
//
//}

void Fitness::clear()
{
	length_fitness = 0.0 ;
	security_fitness = 0.0 ;
	smoothness_fitness = 0.0 ;
}

//void update_fit()
//{
//
//	fit = fitness_point[0] ;
//
//	for ( int i = 1 ; i < cur_parameter.pSize ; i++ ) {
//
//		if ( fitness_point[i].length_fitness > fit.length_fitness ){
//
//			fit.length_fitness = fitness_point[i].length_fitness ;
//		}
//		if ( fitness_point[i].smoothness_fitness > fit.smoothness_fitness ) {
//
//			fit.smoothness_fitness = fitness_point[i].smoothness_fitness ;
//		}
//		if ( fitness_point[i].security_fitness < fit.security_fitness ) {
//
//			fit.security_fitness = fitness_point[i].security_fitness ;
//		}
//	}
//}
//##Added on 2013/9/23 //////////////////////////////////////////////

double SearchRowNearest( const int& x0, const int& y0, const int& curLin, const double& preMin, int& minX )
{
	int xSpan = max( x0, (chart_width-1-x0) );
	double tempDist, curMin;//#tempDist stands for temporary distance value?
	curMin = preMin;

	for( int k = 0; k <= xSpan; ++k ){

		if( ((x0-k) >= 0) && (chart[curLin*chart_width+ x0-k ] == 0) ){

			tempDist = sqrt((double)(k*k + (curLin-y0)*(curLin-y0)));
			if(curMin > tempDist){

				minX = x0 - k;
				curMin = tempDist;
			}
		}
		if( ((x0+k) < chart_width) && (chart[curLin*chart_width+x0+k] == 0) ){

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

int SaveParameter(Para& para )    
{

	if (
		para.height <= 0 ||
		para.pop_cross_rate <= 0.0 || 
		para.pop_mutation_rate <= 0.0 ||
		para.pSize <= 0 ||
		para.total_gen <= 0 ||
		para.width <= 0 ) {

			return 1 ; //##Check the data
	}

	fstream fout;
	fout.open(kFileName, std::ios::out);

	fout<<para.height<<"\n";
	fout<<para.width<<"\n";
	fout<<para.pop_cross_rate<<"\n";
	fout<<para.pop_mutation_rate<<"\n";
	fout<<para.pSize<<"\n";
	fout<<para.length<<"\n";
	fout<<para.security<<"\n";
	fout<<para.smooth<<"\n";
	fout<<para.total_gen<<"\n";

	for ( int i = 0 ; i < out_num ; i++ ) {

		if ( 1 == chart[i] ) {

			fout<<i<<"\n" ;
		}
	}

	fout.close();

	return 0 ;
}

int LoadParameter(Para& para )    
{
	fstream fin;
	fin.open( kFileName, std::ios::in );
	if(fin.is_open()){

		std::string str;

		if ( !fin.eof() ) {

			std::getline(fin,str,'\n');                        
			para.height = atoi( str.c_str() ) ; //##Height
		}else{

			return 1 ;
		}

		if ( !fin.eof() ) {

			std::getline(fin,str,'\n');
			para.width = atoi( str.c_str() ) ; //##Width
		}else{

			return 1 ;
		}

		if ( !fin.eof() ) {

			std::getline(fin,str,'\n');
			para.pop_cross_rate = atof( str.c_str() ) ; //##Cross rate
		}else{

			return 1 ;
		}

		if ( !fin.eof() ) {

			std::getline(fin,str,'\n');
			para.pop_mutation_rate = atof( str.c_str() ) ; //##Mutate rate
		}else{

			return 1 ;
		}

		if ( !fin.eof() ) {

			std::getline(fin,str,'\n');
			para.pSize = atoi( str.c_str() ) ; //##Population size
		}else{

			return 1 ;
		}

		if ( !fin.eof() ) {

			std::getline(fin,str,'\n');
			para.length = atoi( str.c_str() ) ; //##Length
		}else{

			return 1 ;
		}

		if ( !fin.eof() ) {

			std::getline(fin,str,'\n');
			para.security = atoi( str.c_str() ) ; //##Security
		}else{

			return 1 ;
		}

		if ( !fin.eof() ) {

			std::getline(fin,str,'\n');
			para.smooth = atoi( str.c_str() ) ; //##Smoothness
		}else{

			return 1 ;
		}

		if ( !fin.eof() ) {

			std::getline(fin,str,'\n');
			para.total_gen = atoi( str.c_str() ) ; //Total generation
		}else{

			return 1 ;
		}

		for ( ; !fin.eof() ; ) {

			std::getline(fin,str,'\n');
			int tmp = atoi( str.c_str() )  ;

			para.chart.push_back(tmp) ;

		}

	} //end if
	else {

		fin.close();	    
		return 1 ;
	}

	fin.close();	    
	return 0 ;
}

void DefaultInitParameter() 
{
	cur_parameter.height = 20 ;
	cur_parameter.width = 20 ;
	cur_parameter.pop_cross_rate = 0.9 ;
	cur_parameter.pop_mutation_rate = 0.05 ;
	cur_parameter.pSize = 40;
	cur_parameter.total_gen = 300;
	cur_parameter.length = TRUE ;
	cur_parameter.smooth = TRUE ;
	cur_parameter.security = FALSE ;

}

double EvaluateLength( const vector<int>& x_var, const int& n ) 
{

	double length_fitness = 0.0 ;

	//#对长度的评价
	for ( int j = 1; j < n; j++ ){

		//前一个点和当前点在栅格图上的坐标
		int x1, y1 ;
		int x2, y2 ;
		x1 = x_var[j-1] % chart_width ;
		y1 = x_var[j-1] / chart_width ;
		x2 = x_var[j] % chart_width ;
		y2 = x_var[j] / chart_width ;
		length_fitness += sqrt( (double)(x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );//两点之间的距离
	}

	return length_fitness ;
}

double EvaluateSmoothness( const vector<int>& x_var, const int& n ) 
{

	double smoothness_fitness = 0.0 ;

	//#对个体光滑度的评价 
	for (int j = 1; j < n-1; j++){

		//前一个点和当前点在栅格图上的坐标
		int x_pre, x_cur, y_pre, y_cur, x_next, y_next ;
		int vectorx_pre, vectory_pre, vectorx_next, vectory_next ;

		x_pre = x_var[j-1] % chart_width;
		y_pre = x_var[j-1] / chart_width;
		x_cur = x_var[j] % chart_width;
		y_cur = x_var[j] / chart_width;
		x_next = x_var[j+1] % chart_width;
		y_next = x_var[j+1] / chart_width;

		vectorx_pre = x_cur - x_pre ;
		vectory_pre = y_cur - y_pre ;
		vectorx_next = x_next - x_cur ;
		vectory_next = y_next - y_cur ;

		double len1 = sqrt((double)( vectorx_pre*vectorx_pre + vectory_pre*vectory_pre ));
		double len2 = sqrt((double)( vectorx_next*vectorx_next + vectory_next*vectory_next ));

		double angle = (vectorx_pre*vectorx_next+vectory_pre*vectory_next) / (len1*len2);//计算向量的夹角

		////////////////////////////////////////////////
		//## Added on 2013/9/24 /////////////////
		if ( 0 == len1*len2 ) angle = 0.0 ;
		if ( angle >= 1.0 ) angle = 1.0 ;
		if ( angle <= -1.0 ) angle = -1.0 ; 
		///////////////////////////////////////////////
		angle = acos(angle);
		if (angle>PI/2) angle -= PI/2 ;
		smoothness_fitness += angle;
	}

	if (n>2) return smoothness_fitness/(n-2) ;
	else return 0;
}

double EvaluateSecurity( const vector<int>& x_var, const int& n  )
{
	double security_fitness = 0.0 ;
	vector<int> vec_chart;

	for(int i = 0; i < out_num+1 ; ++i)
		if(is_block(i))
			vec_chart.push_back(i);

	//#对个体安全性的评价
	for(int j = 0; j < n-1  ; ++j){

		//当前点在栅格图上的坐标
		int x_ind, y_ind, x_blk, y_blk, m = 0;
		double len = 0.0;
		double L = 0.0;
		x_ind = x_var[j]%chart_width;
		y_ind = x_var[j]/chart_width;

		for( int i = 0; i < vec_chart.size(); ++i ){

			x_blk = vec_chart[i]%chart_width;
			y_blk = vec_chart[i]/chart_width;
			len = sqrt((double)((x_ind-x_blk)*(x_ind-x_blk) + (y_ind-y_blk)*(y_ind-y_blk)));

			m = CheckSecurity( x_var, j,  x_blk, y_blk ) ;

			L += len;
			L = L - m * double( chart_width) ; //进行惩罚

			if(L < 0)
				L = 0;
		}

		security_fitness += L / vec_chart.size();
	}

	return security_fitness/(n-1);
}

int CheckSecurity( const vector<int>& x_var, const int& idx,const int& w, const int& h )
{
	int num = 0 ;

	if ( x_var[idx] == (h-1)*chart_width+w && 
		x_var[idx+1] == h*chart_width+w-1 ||
		x_var[idx+1] == (h-1)*chart_width+w && 
		x_var[idx] == h*chart_width+w-1){

			num++ ;
	}
	if ( x_var[idx]  == (h-1)*chart_width+w && 
		x_var[idx+1] == h*chart_width+w+1 ||
		x_var[idx]  == (h-1)*chart_width+w && 
		x_var[idx+1] == h*chart_width+w+1){

			num++ ;
	}
	if ( x_var[idx]  == (h+1)*chart_width+w && 
		x_var[idx+1] == h*chart_width+w-1 ||
		x_var[idx+1]  == (h+1)*chart_width+w && 
		x_var[idx] == h*chart_width+w-1){

			num++ ;
	}
	if ( x_var[idx] == (h+1)*chart_width+w && 
		x_var[idx+1] == h*chart_width+w+1 ||
		x_var[idx+1] == (h+1)*chart_width+w && 
		x_var[idx] == h*chart_width+w+1){

			num++ ;
	}

	return num ;

}

int BlockNum( const vector<int>& x_var, const int& n ) 
{
	int pass_fitness = 0;
	int curX, curY, nextX, nextY ;
	double kX, kY, nextk ;
	int x1, x2, y1, y2 ;
	int count = 0 ;

	for ( int i = 0 ; i < n - 1 ; i++ ) {

		x1 = x_var[i] % chart_width ;
		y1 = x_var[i] / chart_width ;
		x2 = x_var[i+1] % chart_width ;
		y2 = x_var[i+1] / chart_width ;

		curX = x1;
		curY = y1;


		if(x2==x1){//#纵向运动
			if( y2 > y1 ) { 

				for ( int n = 1 ; n < y2-y1 ; n++ )
					if( is_block( y1+n, x1 ) )
						pass_fitness++;
			} else { 

				for ( int n = 1 ; n < y1-y2 ; n++ )
					if( is_block( y1-n, x1 ) )
						pass_fitness++;
			}


		}else if( y2 == y1 ) {//#横向运动

			if( x2 > x1 ) { 

				for ( int n = 1 ; n < x2-x1 ; n++ )
					if( is_block( y1, x1+n ) )
						pass_fitness++;
			} else { 

				for ( int n = 1 ; n < x1-x2 ; n++ )
					if( is_block( x1, x1-n ) )
						pass_fitness++;
			}

		}else{ //#斜向运动

			int X = x2-x1 ;
			int Y = y2-y1 ;

			double A = double(Y)/*/X*/ ;
			double B = double(-X) ;
			double C = -A*x1 -B*y1;

			int M , N ;
			M = (abs(X)+1) ;
			N = (abs(Y)+1) ;
			//if ( abs(X) > abs(Y) ) {

			//	M = (abs(X)-1) ;
			//	N = (abs(Y)+1) ;
			//}else {
			//	if ( abs(Y) > abs(X) ) { 
			//		M = (abs(X)+1);
			//		N  = (abs(Y)-1) ;
			//	}else 
			//		if ( abs(Y) == abs(X) ) {
			//			M = (abs(X)-1);
			//			N = (abs(Y)-1) ;
			//		}
			//}

			int x0 = X > 0? x1: x2 ;
			int y0 = Y > 0? y1: y2 ;

			for ( int n = 0 ; n < N ; n++ ){
				for ( int m = 0 ; m < M ; m++ ) {
					if ( is_block(y0+n, x0+m ) ){

						double dist = abs( A*(x0+m) + B*(y0+n) + C )/ sqrt(A*A+B*B) ;
						if ( dist < kHalfSqrt2 ) 
							pass_fitness++ ;
					}
				}
			}
		}
			
		//do {

		//	if(x2==x1){//#纵向运动

		//		if( y2 > y1 ) { 
		//			
		//			nextY = curY + 1 ;
		//			nextX = curX ;
		//		} else { 
		//			
		//			nextY = curY - 1 ;
		//			nextX = curX ;
		//		}
		//	}else if( y2 == y1 ) {//#横向运动

		//		if( x2 > x1 ) {
		//			
		//			nextX = curX + 1 ;
		//			nextY = curY ;
		//		} else {
		//			
		//			nextX = curX - 1 ;
		//			nextY = curY ;
		//		}
		//	}else{ //#斜向运动

		//		if( x2 > x1) nextX = curX+1;
		//		else nextX = curX-1;

		//		if( y2>y1) nextY = curY+1;
		//		else nextY = curY-1;

		//		if( x2 < x1 ) nextX++;
		//		if( y2 < y1 ) nextY++;

		//		kX = (nextX-x1-0.5)/(x2-x1);
		//		kY = (nextY-y1-0.5)/(y2-y1);
		//		nextk = min(kX,kY);

		//		nextX = floor(x1+0.5+nextk*(x2-x1));
		//		nextY = floor(y1+0.5+nextk*(y2-y1));

		//		if(x2<x1&&kX<=kY) nextX--;
		//		if(y2<y1&&kY<=kX) nextY--;

		//	}
		//	if( is_block( nextY, nextX ) )//如果通过了障碍物
		//		pass_fitness++;
		//	curX = nextX; 
		//	curY = nextY;
		//	count++;

		//	if(count>kMaxDist){

		//		nextX=x2;
		//		nextY=y2;
		//	}
		//}while( !( nextX == x2 && nextY == y2 ));
		//count=0;
	}

	fo<<pass_fitness<<endl;
	return pass_fitness ;
}

int ObjEval( const vector<int>x_var, const int& index, vector<double>& y_obj )
{
	int n = x_var.size() ;

	double length_fitness = EvaluateLength(x_var, n) ;
	double smoothness_fitness = EvaluateSmoothness(x_var, n) ;
	double security_fitness = EvaluateSecurity(x_var, n) ;
	int pass_fitness = BlockNum(x_var, n ) ;

	if ( index != kNotIndex ) {

		double len = sqrt((double)( (chart_width-1) * (chart_width-1)*2) );

		total_fitness[index][kLength] =  1.0 - len/(length_fitness+pass_fitness*length_pun) ;

		if(smoothness_fitness <= 0 || (smoothness_fitness+pass_fitness)> PI/2)
			total_fitness[index][kSmoothness] = 1.0 ;
		else
			total_fitness[index][kSmoothness] = (smoothness_fitness+pass_fitness)/(PI/2) ;

		double max_dist = (len + (chart_width-1) *2) / 4 ;

		if( security_fitness <= 0 || security_fitness-pass_fitness > max_dist )
			total_fitness[index][kSecurity] = 1.0;
		else
			total_fitness[index][kSecurity] =  1.0 - (security_fitness-pass_fitness)/max_dist ;
	}
	y_obj[kLength] = length_fitness + kEvalWeight*pass_fitness;					//长度
	y_obj[kSmoothness] = smoothness_fitness + kEvalWeight*pass_fitness;		    		//平滑度
	y_obj[kSecurity] = chart_width - security_fitness + kEvalWeight*pass_fitness ;					//安全性

	return pass_fitness ;
}

void GetBestFit( const int& i_obj/*, vector<int> &path*/ ) 
{
	double cur_best_obj_value = total_fitness[0][i_obj] ;

	for( int i = 1 ; i < cur_parameter.pSize ; ++i ) {

		if ( total_fitness[i][i_obj] < cur_best_obj_value ) {

			cur_best_obj_value = total_fitness[i][i_obj] ;
		}	
	}

	fo<< cur_best_obj_value ;

	if ( i_obj == kLength ) {

		fit.length_fitness = cur_best_obj_value ;
	}else{

		if ( i_obj == kSmoothness ) {

			fit.smoothness_fitness = cur_best_obj_value ;
		}else {

			fit.security_fitness = cur_best_obj_value ;
			fo<<endl ;
		}
	}

}

void SpeedUp() 
{
	if ( run_speed>10 ) 
		run_speed -= 10 ;
}

void SpeedDown() 
{
	if ( run_speed<200 ) 
		run_speed += 10 ;
}
