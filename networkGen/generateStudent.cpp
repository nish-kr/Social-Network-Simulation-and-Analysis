/*
 * generateStudent.cpp
 *
 *  Created on: Oct 11, 2013
 *      Author: cs1120234
 */
#include <random>
#include <vector>
#include <fstream>
#include <pthread.h>
#include <errno.h>
#include <math.h>
#include "generateStudent.h"
#include "Network.h"
#include "generator.h"
using namespace std;

extern long mtime;
extern long timelimit;
extern pthread_cond_t cond[4];
extern pthread_mutex_t Nlock;
extern pthread_mutex_t cond_mutex[4];
extern pthread_mutex_t logLock;
extern pthread_mutex_t wLock;
extern pthread_mutex_t tLock;
extern bool isWaiting[4];
extern bool isWorking[4];
extern bool isMainWaiting[4];
extern pthread_cond_t condBack[4];

extern ofstream logStream;
extern Network N;
extern string names[];

default_random_engine rand_en_stud;
static long nextTimeToRun = 0;

void initRandomStud(){
	rand_en_stud.seed(N.randomSeedStudent);
}


static void printMutexLockError(){
	cerr<<"Error in Mutex locking - generateStudent."<<endl;
	switch(errno){
		case E2BIG :{cerr<<"EINVAL"<<endl;break;}
		case EACCES :{cerr<<"EBUSY"<<endl;break;}
		case EAGAIN :{cerr<<"EAGAIN"<<endl;break;}
		case EFAULT :{cerr<<"EDEADLK"<<endl;break;}
		case EIDRM :{cerr<<"EPERM"<<endl;break;}
	}
}



void* initGenerateStudent(void* arg){
	pthread_mutex_lock(&cond_mutex[1]);
	initRandomStud();
	isWaiting[1] = false;
	goto waiting;

genStud:	
	if (pthread_mutex_lock(&wLock)!=0) {printMutexLockError();exit(1);}
	isWorking[1] = true;
	if (pthread_mutex_unlock(&wLock)!=0) {printMutexLockError();exit(1);}

	//assuming mutex is not required while reading 
	//set the time
	if (pthread_mutex_lock(&tLock)!=0) {printMutexLockError();exit(1);}
	if (mtime > nextTimeToRun) {cerr<<"error in time .";exit(1);}
	if (mtime != nextTimeToRun) mtime = nextTimeToRun;
	if (pthread_mutex_unlock(&tLock)!=0) {printMutexLockError();exit(1);}
	for(int i=0; i< N.universities.size() ; i++){
		University* curUniv = N.universities[i];
		
		int studentID = curUniv->students.size();        					//the ID of the next student is the size of
																			//vector of students
		for(int j = 0 ;j < curUniv->departments.size() ; j++){
				Department* curDep = curUniv->departments[j];
				int n_stud = curDep->numOfStudspy;
				for(int k=0 ; k<n_stud ; k++){

					studentID++;
					Student* curStud = new Student();
					
					//lock the Network and add the student
					pthread_mutex_lock(&Nlock);
					curStud->entryNumber = studentID;
					curStud->university =  curUniv;
					curStud->dep = curDep;
					curDep->students.push_back(curStud);					//adding the current student to the vector of students of department
					curUniv->students.push_back(curStud);					//adding the current student to the vector of students of University

					N.students.push_back(curStud);							//adding the current student to the vector of the students of the network
					curStud->UID = N.students.size();
					uniform_int_distribution<int> dist_hostel(0 , curUniv->hostels.size()-1);
					int randno = dist_hostel(rand_en_stud);
					curUniv->hostels[randno]->allotRoom(curStud);			//hostel alloted of student
					curStud->hostel = curUniv->hostels[randno];				//hostel locality added of student
					curUniv->hostels[randno]->hostlers.push_back(curStud);

					uniform_int_distribution<int> dist_interest(1, 100);	//adding interests

					vector<float> curPopularity = curUniv->popularity;
					for(int i=0;i<curPopularity.size() ; i++){
						if (curPopularity[i] == 0) continue;

						Interest* curInterest = N.globalInterests[i];
						randno = dist_interest(rand_en_stud);
						int popularityInt = curPopularity[i]*100;
						if (randno <= popularityInt){
							curStud->interests.push_back(curInterest);
							curInterest->students.push_back(curStud);
						}
					}

					uniform_int_distribution<int> dist_name(0,150);			//adding name
					curStud->name = names[dist_name(rand_en_stud)];

					//student added .. release the network lock
					pthread_mutex_unlock(&Nlock);
					
					//add to the log file
					pthread_mutex_lock(&logLock);
					logStream<<"Student "<<curStud->name<<" of Department "<<curDep->name
							<<" and of university "<<curUniv->name<<" generated .\n";
					pthread_mutex_unlock(&logLock);
			}
		}
	}
	
	//send message to TK 
	//assuming all months to be 30 days
	nextTimeToRun = mtime + 12*30*24*60;
	sndmsgToTK(nextTimeToRun , 1);

	//adding of all students done .. 
	if (pthread_mutex_lock(&wLock)!=0) {printMutexLockError();exit(1);}
	isWorking[1] = false;
	if (pthread_mutex_unlock(&wLock)!=0) {printMutexLockError();exit(1);}
	
	//all work of this thread has been done .. let the main of generator proceed .. 
	if (pthread_mutex_lock(&cond_mutex[1])!=0) {printMutexLockError();exit(1);}
	isMainWaiting[1] = false;


	pthread_cond_signal(&condBack[1]);

waiting:
	if (pthread_mutex_trylock(&cond_mutex[1]) != 0){
		//lock busy .. 
	}
	isWaiting[1] = true;

	while(isWaiting[1]){
		pthread_cond_wait(&cond[1] , &cond_mutex[1]);
	}
	if (pthread_mutex_unlock(&cond_mutex[1])!=0) {printMutexLockError();exit(1);}
	goto genStud;
	return NULL;
}