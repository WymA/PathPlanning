#ifndef _RECOMBINATION_H_
#define _RECOMBINATION_H_


//#include "individual.h"

/* Routine for real polynomial mutation of an T */

template <class T>
int is_series( T &ind, const int& idx ) ;

template <class T>
void realmutation(T &ind, double rate)
{

	double rnd, delta1, delta2, mut_pow, deltaq;
	double y, yl, yu, val, xy;
	double eta_m = etam;

	int    id_rnd = int( rnd_uni( &rnd_uni_init) * nvar );

	for ( int j = 0 ; j<nvar ; j++ ){

		if ( rnd_uni( &rnd_uni_init ) <= rate ) {

			y  = ind.x_var[j];
			yl = lowBound[j];
			yu = uppBound[j];
			delta1 = (y-yl)/(yu-yl);
			delta2 = (yu-y)/(yu-yl);
			rnd = rnd_uni(&rnd_uni_init);
			mut_pow = 1.0/(eta_m+1.0);

			if ( rnd <= 0.5 ){

				xy = 1.0 - delta1 ;
				val = 2.0*rnd + (1.0 - 2.0*rnd) * (pow(xy, (eta_m + 1.0))) ;
				deltaq =  pow(val, mut_pow) - 1.0 ;
			}else{

				xy = 1.0-delta2;
				val = 2.0*(1.0 - rnd) + 2.0*(rnd - 0.5)*(pow(xy, (eta_m + 1.0)));
				deltaq = 1.0 - (pow(val, mut_pow));
			}

			if ( 2 != is_series( ind, j ) ) {

				int new_value = y + deltaq*(yu - yl) ;
				if (y < yl)
					//y = yl;
					y = yl + ( (yl-y)/(yu-yl) - floor( (yl-y)/(yu-yl) )  ) * (yu-yl) ;
				if (y > yu)
					//y = yu;
					y = yu - ( (y-yu)/(yu-yl) - floor( (y-yu)/(yu-yl) )  ) * (yu-yl) ;

				//变异出的不可以是障碍物，如果是障碍物则不变异
				int x = new_value%chart_width;
				int y = new_value/chart_width;

				if(chart[y][x] == 0)
					ind.x_var[j] = new_value ;
			}
	

		}
	}
	return;
}

///////////////////////////////////////////////////////////////////////////////
//##Added on 2013/9/9///////////////////////////////////////////////////
template <class T>
void gen_mutation( T& ind )
{
	double p = rand()%1000/1000.0;
	if( p < mutation_rate && ind.x_var.size() > 2){

		//随机产生变异的位置，不能为头和尾
		int mut_point;
		mut_point = 1 + rand()%(ind.x_var.size() - 2);
		int bSeries = is_series(ind, mut_point);
		//若该点与前后两点连续则不变异
		if(bSeries != 2){

			//随机变异出一位数，可以取道起点和终点，这里可能出错
			//变异出的不可以是障碍物，如果是障碍物则不变异
			int new_value = rand()%(out_num+1);
			int x = new_value%chart_width;
			int y = new_value/chart_width;

			if(chart[y][x] == 0)
				ind.x_var[mut_point] = new_value;
		}
	}
}


template <class T>
void gen_crossover( T& parent1, T& parent2, T& child1, T& child2 ) 
{
	double p = rand()%1000/1000.0;
	int Parent1Length = parent1.x_var.size(); //第一个以及第二个父个体长度
	int Parent2Length = parent2.x_var.size(); //

	child1.x_var.clear();
	child2.x_var.clear();

	if ( p < cross_rate ) {

		//随机产生第一个以及第二个父个体需要交叉的一位,不能是第一位（起点）     
		int Parent1CrossPoint, Parent2CrossPoint;

		//这里要注意避免起点 
		Parent1CrossPoint = 1+rand()%(Parent1Length-1); 
		Parent2CrossPoint = 1+rand()%(Parent2Length-1); 

		for (int j = 0; j < Parent1CrossPoint; ++j)
			child1.x_var.push_back( parent1.x_var[j] );
		for (int j = Parent2CrossPoint; j < Parent2Length; ++j)
			child1.x_var.push_back( parent2.x_var[j] );

		for (int j = 0; j < Parent2CrossPoint; ++j)
			child2.x_var.push_back( parent2.x_var[j] );
		for (int j = Parent1CrossPoint; j < Parent1Length; ++j)
			child2.x_var.push_back( parent1.x_var[j] );
	
	}else{

		for (int j = 0; j < Parent1Length; ++j)
			child1.x_var.push_back( parent1.x_var[j] );
		for (int j = 0; j < Parent2Length; ++j)
			child2.x_var.push_back( parent2.x_var[j] );
	}
}

////////////////////////////////////////////////////////////////////////////////////
///##Modified on 2013/9/9/////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
/* Routine for real variable SBX crossover */
template <class T>
void real_sbx_xoverA(T &parent1, T &parent2, T &child1, T &child2)
{
	double rand;
	double y1, y2, yl, yu;
	double c1, c2;
	double alpha, beta, betaq;
	double eta_c = etax;

	if (rnd_uni(&rnd_uni_init) <= 1.0) {

		for ( int i = 0 ; i < nvar ; i++ ){//#Start a big loop;

			if ( rnd_uni(&rnd_uni_init) <= 0.5 ){

				if ( fabs( double( parent1.x_var[i] - parent2.x_var[i] ) ) > EPS ){

					if ( parent1.x_var[i] < parent2.x_var[i] ) {

						y1 = parent1.x_var[i];
						y2 = parent2.x_var[i];
					}else{

						y2 = parent1.x_var[i];
						y1 = parent2.x_var[i];
					}

					yl = lowBound[i];
					yu = uppBound[i];
					rand = rnd_uni(&rnd_uni_init);
					beta = 1.0 + (2.0*(y1-yl)/(y2-y1));
					alpha = 2.0 - pow(beta,-(eta_c+1.0));

					if (rand <= (1.0/alpha)){

						betaq = pow ((rand*alpha),(1.0/(eta_c+1.0)));
					}else{

						betaq = pow ((1.0/(2.0 - rand*alpha)),(1.0/(eta_c+1.0)));
					}

					c1 = 0.5*((y1+y2)-betaq*(y2-y1));
					beta = 1.0 + (2.0*(yu-y2)/(y2-y1));
					alpha = 2.0 - pow(beta,-(eta_c+1.0));
					if (rand <= (1.0/alpha)){

						betaq = pow ((rand*alpha),(1.0/(eta_c+1.0)));
					}else{

						betaq = pow ((1.0/(2.0 - rand*alpha)),(1.0/(eta_c+1.0)));
					}

					c2 = 0.5*((y1+y2)+betaq*(y2-y1));
					if (c1<yl)
						//c1=yl;
						c1 = yl + ( (yl-c1)/(yu-yl) - floor( (yl-c1)/(yu-yl) )  ) * (yu-yl);
					if (c2<yl)
						//c2=yl;
						c2 = yl + ( (yl-c2)/(yu-yl) - floor( (yl-c2)/(yu-yl) )  ) * (yu-yl);
					if (c1>yu)
						//c1=yu;
						c1 = yu - ( (c1-yu)/(yu-yl) - floor( (c1-yu)/(yu-yl) )  ) * (yu-yl);
					if (c2>yu)
						//c2=yu;
						c2 = yu - ( (c2-yu)/(yu-yl) - floor( (c2-yu)/(yu-yl) )  ) * (yu-yl);

					if (rnd_uni(&rnd_uni_init)<=0.5){

						child1.x_var[i] = c2;
						child2.x_var[i] = c1;
					}else{

						child1.x_var[i] = c1;
						child2.x_var[i] = c2;
					}
				}else	{

					child1.x_var[i] = parent1.x_var[i];
					child2.x_var[i] = parent2.x_var[i];
				}
			}else{

				child1.x_var[i] = parent1.x_var[i];
				child2.x_var[i] = parent2.x_var[i];
			}
		}//# End of loop ;
	}else{

		for (int i=0; i<nvar; i++){

			child1.x_var[i] = parent1.x_var[i];
			child2.x_var[i] = parent2.x_var[i];
		}
	}

	return;
}
//
//
//template <class T>
//void diff_evo_xoverB(T &ind0, T &ind1, T &ind2, T &child, double rate, double CR)
//{
//	int idx_rnd = int(rnd_uni(&rnd_uni_init)*nvar);
//
//	for(int n=0;n<nvar;n++)
//	{
//		/*Selected Two Parents*/
//
//		// strategy one 
//		// child.x_var[n] = ind0.x_var[n] + rate*(ind2.x_var[n] - ind1.x_var[n]);
//
//		///*
//		// strategy two
//		double rnd1 = rnd_uni(&rnd_uni_init);
//
//		if(rnd1<CR||n==idx_rnd)
//			child.x_var[n] = ind0.x_var[n] + rate*(ind2.x_var[n] - ind1.x_var[n]);
//		else
//			child.x_var[n] = ind0.x_var[n];
//		//*/
//
//
//		// handle the boundary voilation
//		if(child.x_var[n]<lowBound[n]){
//			//double rnd = rnd_uni(&rnd_uni_init);
//			//child.x_var[n] = lowBound[n] + rnd*(ind0.x_var[n] - lowBound[n]);
//			//child.x_var[n] = 2 * lowBound[n] - child.x_var[n];
//			child.x_var[n] = lowBound[n] + ( (lowBound[n]-child.x_var[n])/(uppBound[n]-lowBound[n]) - floor( (lowBound[n]-child.x_var[n])/(uppBound[n]-lowBound[n]) )  ) * (uppBound[n]-lowBound[n]);
//		}
//		if(child.x_var[n]>uppBound[n]){ 
//			//double rnd = rnd_uni(&rnd_uni_init);
//			//child.x_var[n] = uppBound[n] - rnd*(uppBound[n] - ind0.x_var[n]);
//			//child.x_var[n] = 2 * uppBound[n] - child.x_var[n];
//			child.x_var[n] = uppBound[n] - ( (child.x_var[n]-uppBound[n])/(uppBound[n]-lowBound[n]) - floor( (child.x_var[n]-uppBound[n])/(uppBound[n]-lowBound[n]) )  ) * (uppBound[n]-lowBound[n]);
//		}
//
//		//if(child.x_var[n]<lowBound) child.x_var[n] = lowBound;
//		//if(child.x_var[n]>uppBound) child.x_var[n] = uppBound;
//	}
//}

//
//double TMOEAD::calc_distance()
//{
//	double distance = 0;
//	for(int i=0; i<ps.size(); i++){
//
//		double min_d = 1.0e+10;
//		for(int j=0; j<population.size(); j++){
//
//			double d = dist_vector(ps[i].y_obj, population[j].indiv.y_obj);
//			if(d<min_d)  min_d = d;
//		}
//		distance+= min_d;
//	}
//	distance/=ps.size();
//
//	return distance;
//}

template <class T>
int is_series( T &ind, const int& idx ) 
{
	int next;
	int pre = ind.x_var[idx-1];
	int cur = ind.x_var[idx];

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

#endif