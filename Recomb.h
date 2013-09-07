#ifndef _RECOMBINATION_H_
#define _RECOMBINATION_H_

#ifndef __GLOBAL_H_
#include "Global.h"
#endif
//#include "individual.h"

/* Routine for real polynomial mutation of an T */

template <class T>
void realmutation(T &ind, double rate)
{

	double rnd, delta1, delta2, mut_pow, deltaq;
	double y, yl, yu, val, xy;
	double eta_m = etam;

	int    id_rnd = int( rnd_uni( &rnd_uni_init) * nvar );

	for (int j=0; j<nvar; j++){

		if ( rnd_uni( &rnd_uni_init ) <= rate ) {

			y  = ind.x_var[j];
			yl = lowBound[j];
			yu = uppBound[j];
			delta1 = (y-yl)/(yu-yl);
			delta2 = (yu-y)/(yu-yl);
			rnd = rnd_uni(&rnd_uni_init);
			mut_pow = 1.0/(eta_m+1.0);

			if (rnd <= 0.5){

				xy = 1.0-delta1;
				val = 2.0*rnd+(1.0-2.0*rnd)*(pow(xy,(eta_m+1.0)));
				deltaq =  pow(val,mut_pow) - 1.0;
			}else{

				xy = 1.0-delta2;
				val = 2.0*(1.0-rnd)+2.0*(rnd-0.5)*(pow(xy,(eta_m+1.0)));
				deltaq = 1.0 - (pow(val,mut_pow));
			}

			y = y + deltaq*(yu-yl);
			if (y<yl)
				//y = yl;
				y = yl + ( (yl-y)/(yu-yl) - floor( (yl-y)/(yu-yl) )  ) * (yu-yl);
			if (y>yu)
				//y = yu;
				y = yu - ( (y-yu)/(yu-yl) - floor( (y-yu)/(yu-yl) )  ) * (yu-yl);
			ind.x_var[j] = y;
		}
	}
	return;
}

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

					if (parent1.x_var[i] < parent2.x_var[i]){

						y1 = parent1.x_var[i];
						y2 = parent2.x_var[i];
					}else{

						y1 = parent2.x_var[i];
						y2 = parent1.x_var[i];
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


template <class T>
void diff_evo_xoverB(T &ind0, T &ind1, T &ind2, T &child, double rate, double CR)
{
	int idx_rnd = int(rnd_uni(&rnd_uni_init)*nvar);

	for(int n=0;n<nvar;n++)
	{
		/*Selected Two Parents*/

		// strategy one 
		// child.x_var[n] = ind0.x_var[n] + rate*(ind2.x_var[n] - ind1.x_var[n]);

		///*
		// strategy two
		double rnd1 = rnd_uni(&rnd_uni_init);

		if(rnd1<CR||n==idx_rnd)
			child.x_var[n] = ind0.x_var[n] + rate*(ind2.x_var[n] - ind1.x_var[n]);
		else
			child.x_var[n] = ind0.x_var[n];
		//*/


		// handle the boundary voilation
		if(child.x_var[n]<lowBound[n]){
			//double rnd = rnd_uni(&rnd_uni_init);
			//child.x_var[n] = lowBound[n] + rnd*(ind0.x_var[n] - lowBound[n]);
			//child.x_var[n] = 2 * lowBound[n] - child.x_var[n];
			child.x_var[n] = lowBound[n] + ( (lowBound[n]-child.x_var[n])/(uppBound[n]-lowBound[n]) - floor( (lowBound[n]-child.x_var[n])/(uppBound[n]-lowBound[n]) )  ) * (uppBound[n]-lowBound[n]);
		}
		if(child.x_var[n]>uppBound[n]){ 
			//double rnd = rnd_uni(&rnd_uni_init);
			//child.x_var[n] = uppBound[n] - rnd*(uppBound[n] - ind0.x_var[n]);
			//child.x_var[n] = 2 * uppBound[n] - child.x_var[n];
			child.x_var[n] = uppBound[n] - ( (child.x_var[n]-uppBound[n])/(uppBound[n]-lowBound[n]) - floor( (child.x_var[n]-uppBound[n])/(uppBound[n]-lowBound[n]) )  ) * (uppBound[n]-lowBound[n]);
		}

		//if(child.x_var[n]<lowBound) child.x_var[n] = lowBound;
		//if(child.x_var[n]>uppBound) child.x_var[n] = uppBound;
	}
}


/* Routine for real variable SBX crossover */
//template <class T>
//void realbinarycrossover (T &parent1, T &parent2, T& child1, T& child2)
//{
//	double rand;
//	double y1, y2, yl, yu;
//	double c1, c2;
//	double alpha, beta, betaq;
//	double eta_c = etax;
//	if (rnd_uni(&rnd_uni_init) <= 1.0) 
//	{
//		for (int i=0; i<nvar; i++)
//		{
//			if (rnd_uni(&rnd_uni_init)<=0.5 )
//			{
//				if (fabs(parent1.x_var[i]-parent2.x_var[i]) > EPS)
//				{
//					if (parent1.x_var[i] < parent2.x_var[i])
//					{
//						y1 = parent1.x_var[i];
//						y2 = parent2.x_var[i];
//					}
//					else
//					{
//						y1 = parent2.x_var[i];
//						y2 = parent1.x_var[i];
//					}
//					yl = lowBound[i];
//					yu = uppBound[i];
//					rand = rnd_uni(&rnd_uni_init);
//					beta = 1.0 + (2.0*(y1-yl)/(y2-y1));
//					alpha = 2.0 - pow(beta,-(eta_c+1.0));
//					if (rand <= (1.0/alpha))
//					{
//						betaq = pow ((rand*alpha),(1.0/(eta_c+1.0)));
//					}
//					else
//					{
//						betaq = pow ((1.0/(2.0 - rand*alpha)),(1.0/(eta_c+1.0)));
//					}
//					c1 = 0.5*((y1+y2)-betaq*(y2-y1));
//					beta = 1.0 + (2.0*(yu-y2)/(y2-y1));
//					alpha = 2.0 - pow(beta,-(eta_c+1.0));
//					if (rand <= (1.0/alpha))
//					{
//						betaq = pow ((rand*alpha),(1.0/(eta_c+1.0)));
//					}
//					else
//					{
//						betaq = pow ((1.0/(2.0 - rand*alpha)),(1.0/(eta_c+1.0)));
//					}
//					c2 = 0.5*((y1+y2)+betaq*(y2-y1));
//					//if (c1<yl)
//					//	c1=yl;
//					//if (c2<yl)
//					//	c2=yl;
//					//if (c1>yu)
//					//	c1=yu;
//					//if (c2>yu)
//					//	c2=yu;
//					if (c1<yl)
//						c1 = yl + ( (yl-c1)/(yu-yl) - floor( (yl-c1)/(yu-yl) )  ) * (yu-yl);
//					if (c2<yl)
//						c2 = yl + ( (yl-c2)/(yu-yl) - floor( (yl-c2)/(yu-yl) )  ) * (yu-yl);
//					if (c1>yu)
//						c1 = yu - ( (c1-yu)/(yu-yl) - floor( (c1-yu)/(yu-yl) )  ) * (yu-yl);
//					if (c2>yu)
//						c2 = yu - ( (c2-yu)/(yu-yl) - floor( (c2-yu)/(yu-yl) )  ) * (yu-yl);
//					if (rnd_uni(&rnd_uni_init)<=0.5)
//					{
//						child1.x_var[i] = c2;
//						child2.x_var[i] = c1;
//					}
//					else
//					{
//						child1.x_var[i] = c1;
//						child2.x_var[i] = c2;
//					}
//				}
//				else
//				{
//					child1.x_var[i] = parent1.x_var[i];
//					child2.x_var[i] = parent2.x_var[i];
//				}
//			}
//			else
//			{
//				child1.x_var[i] = parent1.x_var[i];
//				child2.x_var[i] = parent2.x_var[i];
//			}
//		}
//	}
//	else
//	{
//		for (int i=0; i<nvar; i++)
//		{
//			child1.x_var[i] = parent1.x_var[i];
//			child2.x_var[i] = parent2.x_var[i];
//		}
//	}
//	return;
//}



#endif