
#include "stdafx.h"

#include "Global.h"
#include "Recomb.h"
#include "Common.h"

#include "SNGAInd.h"
#include "SNGA.h"



CSNGA::CSNGA(int pop_size){
	popsize = pop_size;
}

CSNGA::~CSNGA(){

}




bool CSNGA::update_extreme_point(CSNGAInd& ind)        //uptate the extreme point , that's f up/down尖 
{
	bool bAnchorUpdated = false;
	bool bTrueNadirUpdated = false;

	int j;

	for( j=0;j<nobj ;j++)
	{
		if(ind.y_obj[j] < AnchorPoint[j][j] || ( ind.y_obj[j] == AnchorPoint[j][j] && ind.y_obj[(j+1)%2] < AnchorPoint[j][(j+1)%2] ))
		{
			bAnchorUpdated = true;
			AnchorPoint[j] = ind.y_obj;
		}
	}

	for( j=0;j<nobj ;j++)
	{
		if( ind.y_obj[j] >TrueNadirPoint[j] )
		{
			bTrueNadirUpdated = true;
			TrueNadirPoint[j] = ind.y_obj[j];
		}
	}

	if(bAnchorUpdated)
	{
		for( j=0;j<nobj ;j++)
		{
			ObserverPoint[j] = AnchorPoint[j][j];
			//PseudoNadirPoint[j] = AnchorPoint[(j+1)%2][j];
		}
	}

	if(bAnchorUpdated || bTrueNadirUpdated)
	{
		for( j=0;j<nobj ;j++)
		{
			//if(PseudoNadirPoint[j]-ObserverPoint[j]>0)
			//	ReferencePoint[j] = ( TrueNadirPoint[j] + 1e3 * (TrueNadirPoint[j] - ObserverPoint[j]) ) / (PseudoNadirPoint[j]-ObserverPoint[j]);
			//else
			ReferencePoint[j] = ( TrueNadirPoint[j] + 1e3 * (TrueNadirPoint[j] - ObserverPoint[j]) );
		}
	}

	return bAnchorUpdated;
}

void CSNGA::init_population()
{
	vector <bool>  sectorpopFlag;

	vector <CSNGAInd>  initial_indi_pop;
	CSNGAInd rest_ind;
	CSNGAInd ind;

	for(int n=0;n<popsize;n++)//统计极值点or观测点  初始个体数目设置//初始化种群所有个体，每个个体是ind，种群是initial_indi_pop
	{
		sectorpopFlag.push_back(false);
		ind.rnd_init();   //inivitalize the variable and objective function (PS&PF)
		//nfes++;
		initial_indi_pop.push_back(ind);//insert_sectorialgrid(ind, n);
	}

	//initial_observation_and_reference_point(initial_indi_pop[0]);

	for(int j=0;j<nobj ;j++)
	{
		AnchorPoint.push_back( initial_indi_pop[0].y_obj );//ObserverPoint.push_back( initial_indi_pop[0].y_obj[j] );
		TrueNadirPoint.push_back( initial_indi_pop[0].y_obj[j] );//ReferencePoint.push_back( initial_indi_pop[0].y_obj[j] );
	}
	for(int j=0;j<nobj ;j++)
	{
		ObserverPoint.push_back( AnchorPoint[j][j] );
		//PseudoNadirPoint.push_back( AnchorPoint[(j+1)%2][j] );
	}
	for(int j=0;j<nobj ;j++)
	{
		ReferencePoint.push_back( TrueNadirPoint[j] + 1e3 * (TrueNadirPoint[j] - ObserverPoint[j]) ) ;
	}

	for(int n=1;n<popsize;n++)//统计极值点or观测点  初始个体数目设置//uptate the extreme point , that's f up/down尖//update 锚点、最小点、观察点、参考点
	{
		update_extreme_point(initial_indi_pop[n]);
	}

	for(int n=0;n<popsize;n++)//统计极值点or观测点  初始个体数目设置
	{
		sectorpop.push_back(initial_indi_pop[0]);
	}

	vector <CSNGAInd>  initial_rest_indi_pop;
	for(int n=0;n< popsize;n++)//计算角度 第一轮按角度填充个体 并收集剩余个体
	{
		//initial_indi_pop[n].yobj2angle(ObserverPoint);
		initial_indi_pop[n].obj2angle_index( /*PseudoNadirPoint, */ObserverPoint, popsize);

		if (sectorpopFlag[initial_indi_pop[n].sectorialindex] == false)//if (sectorialgrid[initial_indi_pop[n].sectorialindex] == -1)
		{
			sectorpop[initial_indi_pop[n].sectorialindex] = initial_indi_pop[n];
			sectorpopFlag[initial_indi_pop[n].sectorialindex] = true;
		}else{

			bool bReplaced; 
			rest_ind = sectorpop[initial_indi_pop[n].sectorialindex];//.indiv;
			bReplaced = Pareto_HyperVolume_compare_sectorialgrid(initial_indi_pop[n]);

			if(bReplaced){

				initial_rest_indi_pop.push_back( rest_ind );
			}else{

				initial_rest_indi_pop.push_back( initial_indi_pop[n] );
			}
		}
	}

	initial_indi_pop.clear();

	for( int n = 0 ; n < popsize ; n++ )//填充剩余的个体
	{
		if ( sectorpopFlag[ n ] == false )
		{
			double min_angle_difference, angle_difference ;
			int min_angle_difference_ind ;
			int rest_size = initial_rest_indi_pop.size();

			min_angle_difference = abs( initial_rest_indi_pop[0].sectorialangle - (double)n / (double)(popsize-1) );
			//min_angle_difference = abs( initial_rest_indi_pop[0].sectorialindex - n );
			min_angle_difference_ind = 0 ;
			for(int j = 1; j<rest_size; j++ )
			{
				angle_difference = abs( initial_rest_indi_pop[j].sectorialangle - (double)n / (double)(popsize-1) );
				//angle_difference = abs( initial_rest_indi_pop[j].sectorialindex - n );
				if(angle_difference < min_angle_difference)
				{
					min_angle_difference = angle_difference;
					min_angle_difference_ind = j;
				}
			}
			sectorpop[n] = initial_rest_indi_pop[min_angle_difference_ind];
			sectorpopFlag[n] = true;
			rest_ind = initial_rest_indi_pop[min_angle_difference_ind];
			initial_rest_indi_pop[min_angle_difference_ind] = initial_rest_indi_pop[ rest_size-1 ];
			initial_rest_indi_pop[ rest_size-1 ] = rest_ind;
			initial_rest_indi_pop.pop_back();
		}
	}

	//double min_angle_difference, angle_difference ;
	//int min_angle_difference_ind ;
	//int min_angle_insert_point;

	//for(int rest_size = initial_rest_indi_pop.size();rest_size>0;rest_size--)
	//{
	//	min_angle_difference = 2.0;
	//	for(int n=0;n< popsize;n++)//填充剩余的个体
	//	{
	//		if (sectorpopFlag[ n ] == false)
	//		{
	//			//min_angle_difference = abs( initial_rest_indi_pop[0].sectorialangle - (double)n / (double)(popsize-1) );
	//			//min_angle_difference_ind = 0 ;
	//			for(int j = 0; j<rest_size; j++ )
	//			{
	//				angle_difference = abs( initial_rest_indi_pop[j].sectorialangle - (double)n / (double)(popsize-1) );
	//				if(angle_difference < min_angle_difference)
	//				{
	//					min_angle_difference = angle_difference;
	//					min_angle_difference_ind = j;
	//					min_angle_insert_point = n;
	//				}
	//			}
	//		}
	//	}

	//	sectorpop[min_angle_insert_point] = initial_rest_indi_pop[min_angle_difference_ind];
	//	sectorpopFlag[min_angle_insert_point] = true;
	//	initial_rest_indi_pop[min_angle_difference_ind] = initial_rest_indi_pop[ rest_size-1 ];
	//}

	initial_rest_indi_pop.clear();
	sectorpopFlag.clear();
}

bool CSNGA::Pareto_HyperVolume_compare_sectorialgrid(CSNGAInd& ind)
{
	bool bReplaced = false;

	double contributing1, contributing2;

	//int ind_index = ind.sectorialindex;
	//TCompare iResult1 = ind.Compare( sectorpop[ind.sectorialindex] );//.indiv
	//if( sectorpop[ind.sectorialindex].angle_in_sector(ObserverPoint, popsize, anglesingle, ind.sectorialindex) )
	if( sectorpop[ind.sectorialindex].sectorialindex == ind.sectorialindex )
	{
		//if(iResult1 == _Pareto_Dominating)
		//{
		//	sectorpop[ind.sectorialindex] = ind;//replace   //.indiv
		//	bReplaced = true;
		//}
		//else if(iResult1 == _Pareto_Nondominated)
		//{
		contributing1 = GetFastigiateHyperVolume(ind, ind.sectorialindex, ReferencePoint);
		contributing2 = GetFastigiateHyperVolume(sectorpop[ind.sectorialindex], ind.sectorialindex, ReferencePoint);

		if(contributing1 < contributing2)
		{
			sectorpop[ind.sectorialindex] = ind;//replace
			bReplaced = true;
		}
		//}
	}else{
		//if( iResult1 != _Pareto_Dominated )
		{
			sectorpop[ind.sectorialindex] = ind;//replace
			bReplaced = true;
		}

	}

	return bReplaced;
}

double CSNGA::GetFastigiateHyperVolume(CSNGAInd&  ind, int ind_index, vector <double> &ReferencePoint)
{
	double FastigiateVolume;

	double normalizedf[2];
	for(int j=0; j<nobj; j++)
	{
		//if(PseudoNadirPoint[j]-ObserverPoint[j]>0)
		//	normalizedf[j] = (ind.y_obj[j] - ObserverPoint[j] ) / (PseudoNadirPoint[j] - ObserverPoint[j]);
		//else
		normalizedf[j] = ind.y_obj[j] - ObserverPoint[j] ;

	}
	if(ind_index==0){
		FastigiateVolume = 0.5*(ind_index+0.5)/(popsize-ind_index-1.5)*normalizedf[1]*normalizedf[1] + (ReferencePoint[1]-normalizedf[1])*normalizedf[0];
	}else if(ind_index==popsize-1){
		FastigiateVolume = 0.5*(popsize-ind_index-0.5)/(ind_index-0.5)*normalizedf[0]*normalizedf[0] + (ReferencePoint[0]-normalizedf[0])*normalizedf[1];
	}else{
		FastigiateVolume = 0.5*(popsize-ind_index-0.5)/(ind_index-0.5)*normalizedf[0]*normalizedf[0] + 0.5*(ind_index+0.5)/(popsize-ind_index-1.5)*normalizedf[1]*normalizedf[1] - normalizedf[0]*normalizedf[1];
	}

	return FastigiateVolume;
}

//double CSNGA::GetFastigiateHyperVolume(CSNGAInd&  ind, int ind_index, vector <double> &ReferencePoint)
//{
//	double FastigiateVolume;
//
//	double Volume1, Volume2, height1, height2, BottomArea1, BottomArea2;
//
//	double f1sharp, f2sharp;
//
//	double directionf1, directionf2;
//
//	height1 = ind.y_obj[1] - ObserverPoint[1];
//	if(ind_index == popsize-1 )
//	{
//		f1sharp = ReferencePoint[0];
//	}else{
//		directionf1 = (ind_index + 0.5) / (double) (popsize-1) ;
//		f1sharp = directionf1 / (1 - directionf1) * (ind.y_obj[1] - ObserverPoint[1]) +  ObserverPoint[0];
//	}
//	BottomArea1 = f1sharp - ind.y_obj[0];
//	Volume1 = 0.5 * BottomArea1 * height1;
//
//	height2 = ind.y_obj[0] - ObserverPoint[0];
//	if(ind_index == 0 )
//	{
//		f2sharp = ReferencePoint[1];
//	}else{
//		directionf2 = (ind_index - 0.5) / (double) (popsize-1) ;
//		f2sharp =  (1 - directionf2) / directionf2 * (ind.y_obj[0] - ObserverPoint[0]) +  ObserverPoint[1];
//	}
//	BottomArea2 = f2sharp - ind.y_obj[1];
//	Volume2 = 0.5 * BottomArea2 * height2;
//	
//	FastigiateVolume = Volume1 + Volume2;
//
//	return FastigiateVolume;
//}

/////////////////////////////////////////////////////////
// returns the hypervolume of ps[0 ..] in 2D ///////
// assumes that ps is sorted improving/////////////
/////////////////////////////////////////////////////////

double CSNGA::compute_hypervolume(vector <CSNGAInd>&  mysectorpop, int mypopsize, int mynobj)
{
	vector <vector<double>> paretofront;

	vector<double> onepoint;
	onepoint.resize(2);      //set the size of vector is 2 at least  
	for(int i=0;i<mypopsize;i++)
	{
		bool Dominated = false;
		for(int j=0;j<mypopsize;j++)
		{
			if(j!=i && ParetoCompare(mysectorpop[i].y_obj, mysectorpop[j].y_obj) == _Pareto_Dominated)
			{
				Dominated = true;
				break;
			}
		}
		if(!Dominated && mysectorpop[i].y_obj[0]<=xHypervolumeRefPoint_TopRight && mysectorpop[i].y_obj[1]<=yHypervolumeRefPoint_TopRight)
		{
			onepoint[0] = mysectorpop[i].y_obj[0];
			onepoint[1] = mysectorpop[i].y_obj[1];
			paretofront.push_back(onepoint);
		}
	}

	quicksort_increasing(paretofront, paretofront.size());
	double front2Dhv = hv2(paretofront);

	front2Dhv = front2Dhv / ((xHypervolumeRefPoint_TopRight - xHypervolumeRefPoint_BottomLeft)*(yHypervolumeRefPoint_TopRight - yHypervolumeRefPoint_BottomLeft));

	//if(front2Dhv>1)
	//	front2Dhv = front2Dhv;

	return front2Dhv;
}

double  CSNGA::tour_selection_hv_difference(int p, vector <CSNGAInd>  &mypopulation)
{
	//int p1 = int(rnd_uni(&rnd_uni_init)*popsize);
	//int p2 = int(rnd_uni(&rnd_uni_init)*popsize);
	int num = 0;
	double hv1 = GetFastigiateHyperVolume(mypopulation[p], mypopulation[p].sectorialindex, ReferencePoint);
	double hv3,hv4,hv_difference=0;
	if(p-1>=0)
	{
		hv3 = GetFastigiateHyperVolume(mypopulation[p-1], mypopulation[p-1].sectorialindex, ReferencePoint);
		hv_difference += hv3 - hv1;
		num++;
	}
	if(p+1<=popsize-1)
	{
		hv4 = GetFastigiateHyperVolume(mypopulation[p+1], mypopulation[p+1].sectorialindex, ReferencePoint);
		hv_difference += hv4 - hv1;
		num++;
	}
	hv_difference = hv_difference / num;

	return hv_difference;
}

int  CSNGA::tour_selection_hv2(vector <CSNGAInd>  &mypopulation)
{
	int p1 = int(rnd_uni(&rnd_uni_init)*mypopulation.size());
	int p2 = int(rnd_uni(&rnd_uni_init)*mypopulation.size());

	double hv1 = tour_selection_hv_difference( p1, mypopulation );
	double hv3 = tour_selection_hv_difference( p2, mypopulation );
	if(hv1 >= hv3 )
		return p1;
	else

		return p2;
}

void save_pop_front(vector <vector<double>>  &pop_front, char saveFilename[1024]);

//void CSNGA::uniform_selection(CSNGAInd*& ind_selected)
//{
//	int pop_index;
//
//	pop_index = int(rnd_uni(&rnd_uni_init)*( popsize ));
//
//	ind_selected = &(sectorpop[pop_index]);//.indiv
//}


void CSNGA::reset_angle()
{
	int n;
	for(n=0; n<popsize; n++)
	{
		//sectorpop[n].yobj2angle(ObserverPoint);
		sectorpop[n].obj2angle_index(/*PseudoNadirPoint, */ObserverPoint, popsize);
	}
}

//void CSNGA::save_population(vector <CSNGAInd>  &mypopulation, char saveFilename[1024])
//{
//	std::fstream fout;
//	fout.open(saveFilename,std::ios::out);
//	for(int n=0; n<mypopulation.size(); n++)
//	{
//		for(int k=0;k<nobj;k++)
//		{
//			fout<<mypopulation[n].y_obj[k]<<"  ";
//		}
//		fout<<"\n";
//	}
//	fout.close();
//}

//void save_pop_front(vector <vector<double>>  &pop_front, char saveFilename[1024])
//{
//	std::fstream fout;
//	fout.open(saveFilename,std::ios::out);
//	for(int n=0; n<pop_front.size(); n++)
//	{
//		for(int k=0;k<nobj;k++)
//		{
//			fout<<pop_front[n][k]<<"  ";
//		}
//		fout<<"\n";
//	}
//	fout.close();
//}
//
//void CSNGA::save_ps(char saveFilename[1024])
//{
//	std::fstream fout;
//	fout.open(saveFilename,std::ios::out);
//	for(int n=0; n<popsize; n++)
//	{
//		///////////////////////////////////////////////
//	//##Modified on 2013/9/10///////////////
//		//for(int k=0;k<nvar;k++)
//		//{
//		//	fout<<sectorpop[n].x_var[k]<<"  ";
//		//}
//		//fout<<"\n";
//	}
//	fout.close();
//}

//void CSNGA::population2front(vector <CSNGAInd>  &mypopulation, vector <CSNGAInd>  &population_front)
void CSNGA::population2front(vector <CSNGAInd>  &mypopulation, vector <vector<double>>  &population_front)
{
	vector<int> nDominated;
	for(int n=0;n<mypopulation.size();n++)
		nDominated.push_back(0);


	for(int k=0; k<mypopulation.size(); k++)
		for(int j=k+1; j<mypopulation.size(); j++)
		{
			TCompare tresult = mypopulation[k].Compare(mypopulation[j]);
			if(tresult == _Pareto_Dominated)
				nDominated[k]++;
			else if(tresult == _Pareto_Dominating)
				nDominated[j]++;
		}

		for(int n=0;n<mypopulation.size();n++)
			if(nDominated[n] == 0)
				population_front.push_back(mypopulation[n].y_obj);

		nDominated.clear();
}

//
//double CSNGA::calc_distance()    //calculate the average distance between ps and current solutions
//{
//	double distance = 0;
//	for(int i=0; i<ps.size(); i++)
//	{
//		double min_d = 1.0e+10;
//		for(int j=0; j<popsize; j++)
//		{
//			double d = dist_vector(ps[i].y_obj, sectorpop[j].y_obj);
//			if(d<min_d)  min_d = d;
//		}
//		distance+= min_d;
//	}
//	distance/=ps.size();
//
//	return distance;
//}


void CSNGA::execute(int run, vector<double>& hvl, vector<double>& igd, double& totaltime,  double &unevolvetime)
{
	////seed = (seed + 23)%1377;					
	////rnd_uni_init = -(long)seed;	
	//vector<double> gd;

	//char filename[1024];   
	//cout<<"fuck"<<endl;
	//sprintf(filename,"POF0/%s.dat",strTestInstance);
	//loadpfront(filename,ps);     //read the data to ps
	//nfes    = 0; 	 

	//double totaltimebegin = clock();

	// first generation
	//int cur_generation   = 1;
	int evaluations = popsize;
	init_population();                              //initival the population
	double hypervolume = compute_hypervolume(sectorpop, popsize, nobj);//cout<<"evaluations = "<<evaluations<<"  hypervolume = "<<hypervolume<<"  "<<endl;
	hvl.push_back(0); 
	hvl.push_back(hypervolume); //gd.push_back(0);  gd.push_back(distance);

	//double distance = calc_distance();  //the distance between ps and solutions
	//igd.push_back(0); 
	//igd.push_back(distance);

	//cout<<"evaluations = "<<evaluations<<"  hypervolume = "<<hypervolume<<"  "<<"  IGD = "<<distance<<"  "<<endl;

	int iLength = 0;
	//int iGen_Utility = 0;

	CSNGAInd child2;
	while( evaluations<=(total_gen*popsize) )   //total_gen =250: 250 generation
	{
		int parent_index1, parent_index2;

		if( iLength  == 0 )
			parent_index1 = 0;
		else if( iLength == 1 )
			parent_index1 = popsize - 1;
		else 
			parent_index1 = tour_selection_hv2( sectorpop );//int(rnd_uni(&rnd_uni_init)*( popsize )); ////int(rnd_uni(&rnd_uni_init)*( popsize )); //
		//ind_selected = &(sectorpop[parent_index1]);

		parent_index2 =  tour_selection_hv2( sectorpop );//int(rnd_uni(&rnd_uni_init)*( popsize ));// //int(rnd_uni(&rnd_uni_init)*( popsize )); //
		//ind_selected2 = &(sectorpop[parent_index2]);
		//int parent_index3 =  tour_selection_hv2( sectorpop );
		//sbx_mutation_evolution(*ind_selected, *ind_selected2);
		//if(rnd_uni(&rnd_uni_init)<0.9)

		/////////////////////////////////////////////////////////////////////////////////
		//##Modified on 2013/9/9///////////////////////////
		////////////////////////////////////////////////////////////
		//real_sbx_xoverA(sectorpop[parent_index1], sectorpop[parent_index2], onechild, child2);
		//realmutation(onechild, 1.0/nvar);


		/////////////////////////////////////////////////////////////////
		//diff_evo_xoverB(sectorpop[parent_index1], sectorpop[parent_index2], sectorpop[parent_index3], onechild, 0.5, 1.0);

		gen_crossover( sectorpop[parent_index1], sectorpop[parent_index2], onechild, child2 ) ;
		gen_mutation( onechild ) ;

		//##!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		onechild.obj_eval(0);

		//nfes++;
		evaluations++;

		bool updated;
		updated = update_extreme_point(onechild);//update_anglerange_maxminindi(onechild, true); //update_nondominated_obj_maxmin(onechild);//update_angle_maxmin(onechild);//
		if(updated)
		{
			reset_angle();
		}

		//onechild.yobj2angle(ObserverPoint);
		onechild.obj2angle_index( /*PseudoNadirPoint,*/ ObserverPoint, popsize);//之前之所以出现奇异点 是因为计算onechild角度是在统计极端点前 极端点改变了 而onechild的角度是老的

		//update_sectorialgrid(onechild);
		Pareto_HyperVolume_compare_sectorialgrid(onechild);

		int dd = int(total_gen*popsize/25.0);
		if(evaluations%dd==0)
		{
			double begintime = clock();

			hypervolume = compute_hypervolume(sectorpop, popsize, nobj);//cout<<"evaluations = "<<evaluations<<"  hypervolume = "<<hypervolume<<"  "<<endl;
			hvl.push_back(int(1.0*evaluations/dd));
			hvl.push_back(hypervolume);

			/*distance = calc_distance();
			igd.push_back(int(1.0*evaluations/dd)); 
			igd.push_back(distance);
*/
//			cout<<"evaluations = "<<evaluations<<"  hypervolume = "<<hypervolume<<"  "<<"  IGD = "<<distance<<"  "<<endl;

			double endtime = clock();
			unevolvetime += endtime - begintime;
		}  

		iLength++;
		if(iLength == popsize / /*8*/10 )//5 8
		{
			iLength = 0;
		}
		/*double real_hypervolume;
		if (evaluations==(total_gen*popsize))
		{
		real_hypervolume = compute_hypervolume(ps, 1000, nobj);
		cout<<"fuck"<<endl;
		cout<<strTestInstance<<':'<<real_hypervolume<<endl;
		}*/
	}

	//double totaltimeend = clock();
	//totaltime += totaltimeend - totaltimebegin;

	// save the final population - F space
	//if(evaluations%(total_gen*popsize)==0)
	//{
	//sprintf(filename,"POF/POP_SNGA_%s_R%d.dat",strTestInstance,run,total_gen);
	//save_population(sectorpop, filename);//保存原始扇形存档种群
	//sprintf(filename,"POF/PF_SNGA_%s_R%d.dat",strTestInstance,run,total_gen);
	vector <vector<double>>  sectorfront;
	population2front(sectorpop, sectorfront);
	//save_pop_front(sectorfront, filename);//保存扇形存档种群的非劣前沿
	sectorfront.clear();
//	}

	// save the final population - X space
	//if(evaluations%(total_gen*popsize)==0)
	//{
	//sprintf(filename,"POS/PS_SNGA_%s_R%d.dat",strTestInstance,run,total_gen);
	//save_ps(filename);
	//} 	

	//gd.push_back( popsize ); 

	sectorpop.clear();

	//ps.clear();
	//std::cout<<" Outcome of the "<<run<<"th run:  hypervolume= "<<hypervolume<<" nevaluations = "<<evaluations<<endl;//std::cout<<" Outcome of the "<<run<<"th run:  distance= "<<distance<<" nevaluations = "<<evaluations<<endl;
	//return gd;

}
