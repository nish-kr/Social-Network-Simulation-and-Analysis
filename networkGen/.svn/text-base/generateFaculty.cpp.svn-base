/*
 * generateFaculty.cpp
 *
 *  Created on: Oct 11, 2013
 *      Author: cs1120239
 */

#include <random>
#include <vector>
#include <fstream>
#include <pthread.h>
#include "generateFaculty.h"
#include "Network.h"
#include "generator.h"
using namespace std;

extern ofstream logStream;
extern Network N;
extern string names[];
default_random_engine rand_en_fac;

void initRandomFac(){
	rand_en_fac.seed(N.randomSeedFaculty);
}

void* initGenerateFaculty(void* arg){
	initRandomFac();
	for(int i=0; i< N.universities.size() ; i++){
		int employeeID = 0;
		University* curUniv = N.universities[i];

		for(int j= 0 ;j < curUniv->departments.size() ; j++){
			Department* curDep = curUniv->departments[j];
			int n_fac = curDep->numOfFaculty;
			for(int k=0 ; k<n_fac ; k++){
				employeeID++;
				Faculty* curFac = new Faculty();

				curFac->university = curUniv;
				curFac->empID = employeeID;
				curFac->dept = curDep;
				curDep->faculties.push_back(curFac);					//adding the current faculty to the vector of faculties of department
				curUniv->faculties.push_back(curFac);					//adding the current faculty to the vector of faculties of University

				N.faculties.push_back(curFac);							//adding the current faculty to the vector of faculties of the network
				curFac->UID = N.faculties.size();
				curFac->isTeaching = false;
				uniform_int_distribution<int> dist(0 , curUniv->houselocalities.size()-1);
			 	int randno = dist(rand_en_fac);
			 	curUniv->houselocalities[randno]->allotHouse(curFac);	//house alloted of faculty
/**/		 	curUniv->houselocalities[randno]->residents.push_back(curFac);

			 	curFac->housel = curUniv->houselocalities[randno];		//house locality added of faculty
			 	uniform_int_distribution<int> dist_interest(1 , 100);	//make a distribution to select a number
			 															//at random from 0 to 100
			 	vector<float> curPopularity = curUniv->popularity;
			 	for(int i=0;i<curPopularity.size() ; i++){

			 		if (curPopularity[i] == 0) continue;				//if the current interest popularity has a value of
			 															//of 0 then this means that this interest will never be
			 															//alloted

			 		Interest* curInterest = N.globalInterests[i];
			 		int popNo = curPopularity[i]*100;
			 		int noReturned = dist_interest(rand_en_fac);

			 		if (noReturned <= popNo){
			 			//add the interest to this faculty
			 			curFac->interests.push_back(curInterest);		//add the interest to the faculty's list of interests
			 			curInterest->faculties.push_back(curFac);		//add the faculty to the interest's list of faculties
			 		}
			 	}

			 	//add a random name
			 	uniform_int_distribution<int> dist_name(0 , 150);		//select a random index from 0 to 150
			 															//151 names are there in the list of names
			 	int nameIndex = dist_name(rand_en_fac);
			 	curFac->name = names[nameIndex];

			 	//add to the log file
			 	logStream<<"Faculty "<<curFac->name<<" of Department "<<curDep->name
			 			<<" and of university "<<curUniv->name<<" generated .\n";
			}
		}
	}
	return NULL;
}


