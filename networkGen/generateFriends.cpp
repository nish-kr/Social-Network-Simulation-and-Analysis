/*
 * generateFriends.cpp
 *
 *  Created on: Oct 12, 2013
 *      Author: cs1120239
 */

#include <vector>
#include <math.h>
#include <random>
#include <fstream>
#include <errno.h>
#include "generateFriends.h"
#include "Network.h"
#include "generator.h"
using namespace std;

extern Network N;
default_random_engine rand_en_friend;										//a random engine whose seed will be the random friend seed and
																			//and which will be used for all the random values required
extern long mtime;
extern long timelimit;
extern pthread_cond_t cond[4];
extern pthread_cond_t condBack[4];
extern pthread_mutex_t Nlock;
extern pthread_mutex_t cond_mutex[4];
extern pthread_mutex_t logLock;
extern pthread_mutex_t wLock;
extern pthread_mutex_t tLock;
extern bool isWaiting[4];
extern bool isWorking[4];
extern bool isMainWaiting[4];

extern ofstream logStream;
extern Network N;
extern string names[];

static long nextTimeToRun;
exponential_distribution<float>** distribution_arr;

//initate the rand_en_friend engine
void initRandomFriends(){
	rand_en_friend.seed(N.randomSeedFriend);
	//creating an array of ditributions for each university
	distribution_arr = new exponential_distribution<float>*[N.universities.size()];	
	for(int i=0;i<N.universities.size();i++){
		distribution_arr[i] = new exponential_distribution<float>(N.universities[i]->friendshipRate);
	}
	//the cleanUp of this will be done in the generator
}


//this function takes in a prob and gives if the event is going to happen or not
bool isHappen(float p){														
	int prob = (int) (p*100);
	uniform_int_distribution<int> selectProb(1,100);
	return (prob>selectProb(rand_en_friend));
}

//this function tells if 2 faculties are already friends
bool isFriendAlready(Faculty* from, Faculty* to ){
	bool isfriend=false;
	if(to->friends.size()==0||from->friends.size()==0){
		return isfriend;
	}
	for(int i = 0;i<from->friends.size();i++){
		Faculty* thisfac= from->friends[i];

		//here have to add a condition to check if he is already friend or not..
		if((thisfac->empID==to->empID) && (thisfac->university->name.compare(to->university->name)==0)){
			isfriend = true;
			break;
		}
	}
	return isfriend;
}


//this function tells if 2 students are already friends
bool isFriendAlready(Student* from, Student* to ){
	bool isfriend=false;
	if(to->friends.size()==0||from->friends.size()==0){
		return isfriend;
	}
	for(int i =0;i<from->friends.size();i++){
		Student* thisStud= from->friends[i];

		//here have to add a condition to check if he is already friend or not..
		if((thisStud->entryNumber==to->entryNumber) && (thisStud->university->name.compare(to->university->name) == 0)){
			isfriend = true;
			break;
		}
	}
	return isfriend;
}

static 	void printMutexLockError(){
	cerr<<"Error in Mutex locking - generateFriends."<<endl;
	switch(errno){
		case E2BIG :{cerr<<"EINVAL"<<endl;break;}
		case EACCES :{cerr<<"EBUSY"<<endl;break;}
		case EAGAIN :{cerr<<"EAGAIN"<<endl;break;}
		case EFAULT :{cerr<<"EDEADLK"<<endl;break;}
		case EIDRM :{cerr<<"EPERM"<<endl;break;}
	}
}


void doFriendRequest(University* curUniv ){
	float sizeOfFac =  curUniv->faculties.size();
	float sizeOfStud = curUniv->students.size();
	float probofFac = sizeOfFac/(sizeOfFac + sizeOfStud);

	//executed if faculty is being selected to make request
	if(isHappen(probofFac)){											
		uniform_int_distribution<int> selFrmAlFac(0,N.faculties.size()-1);
		Faculty* fromFac =  N.faculties[selFrmAlFac(rand_en_friend)];

		//executed when the faculty choses from any where
		if(isHappen(fromFac->university->openness)){
			bool foundOtherFac =  false;
			Faculty* toFac;
			int count_iter = 0;
			int count_max = 1;
			while(!foundOtherFac && (count_iter < count_max)){
				toFac =  N.faculties[selFrmAlFac(rand_en_friend)];
				if (!isFriendAlready(fromFac , toFac)){
					if ((toFac->empID != fromFac->empID) || 
						((toFac->empID == fromFac->empID) && (toFac->university->name.compare(fromFac->university->name) != 0))){
						foundOtherFac = true;
						break;
					}
				}
				count_iter++;
			}

			if (count_iter > count_max) return;
			if (!foundOtherFac) return;

			//request is being made from here i.e we are checking if the person is going to accpt or not
			if(isHappen(toFac->university->friendliness)){
				pthread_mutex_lock(&Nlock);
				fromFac->friends.push_back(toFac);
				toFac->friends.push_back(fromFac);
				pthread_mutex_unlock(&Nlock);

				pthread_mutex_lock(&logLock);
				logStream<<"Friend request sent from faculty "<<fromFac->empID<<
					" of "<<fromFac->university->name<<" to "<<toFac->empID<< " of "<<toFac->university->name
					<<" randomly and request accepted ."<<endl;
				pthread_mutex_unlock(&logLock);
			}
			else{
				pthread_mutex_lock(&logLock);
				logStream<<"Friend reqest sent from faculty "<<fromFac->name<<
					" of University "<<fromFac->university->name<<" to "<<toFac->name << " of university "<<toFac->university->name
					<<" randomly and request denied ."<<endl;
				pthread_mutex_unlock(&logLock);
			}

		}
		//executed if faculty chooses from people with similar intrests/places /courses etc.
		else{
			uniform_int_distribution<int> selectProb(1,100);
			int internalProb = 	selectProb(rand_en_friend);

			if(internalProb <= 25){											//code for department
				bool foundOtherFac =  false;
				Faculty* toFac;
				uniform_int_distribution<int> facdist(0,fromFac->dept->faculties.size()-1);	

				int count_iter = 0;
				int count_max = 1;
				while(!foundOtherFac && (count_iter < count_max)){
					toFac =  fromFac->dept->faculties[facdist(rand_en_friend)];
					if(toFac->empID!=fromFac->empID && !isFriendAlready(fromFac,toFac)){
						foundOtherFac = true;
						break;
					}
					count_iter++;
				}
				if (count_iter > count_max) return;
				if (!foundOtherFac) return;

				if(isHappen(toFac->university->friendliness)){
					pthread_mutex_lock(&Nlock);
					fromFac->friends.push_back(toFac);
					toFac->friends.push_back(fromFac);									
					pthread_mutex_unlock(&Nlock);

					pthread_mutex_lock(&logLock);
					logStream<<"Friend request sent from faculty "<<fromFac->name<<
						" of "<<fromFac->university->name<<" to "<<toFac->name << " of "<<toFac->university->name
						<<" with same department and request accepted ."<<endl;
					pthread_mutex_unlock(&logLock);
				}
				else{
					pthread_mutex_lock(&logLock);
					logStream<<"Friend reqest sent from faculty "<<fromFac->name<<
						" of "<<fromFac->university->name<<" to "<<toFac->name << " of "<<toFac->university->name
						<<" with same department and request denied ."<<endl;
					pthread_mutex_unlock(&logLock);
				}
			}
			else if(internalProb >25 && internalProb <= 50){				//code for neighbourhood
				bool foundOtherFac =  false;
				Faculty* toFac;

				uniform_int_distribution<int> facdist(0,fromFac->housel->residents.size()-1);
				int count_iter = 0;
				int count_max = 1;
				while(!foundOtherFac && (count_iter < count_max)){
					toFac =  fromFac->housel->residents[facdist(rand_en_friend)];
					if(toFac->empID!=fromFac->empID && !isFriendAlready(fromFac,toFac)){
						foundOtherFac = true;
						break;
					}
					count_iter++;
				}

				if (count_iter > count_max) return;
				if (!foundOtherFac) return;

				if(isHappen(toFac->university->friendliness)){
					pthread_mutex_lock(&Nlock);
					fromFac->friends.push_back(toFac);
					toFac->friends.push_back(fromFac);
					pthread_mutex_unlock(&Nlock);

					pthread_mutex_lock(&logLock);
					logStream<<"Friend request sent from faculty "<<fromFac->name<<
						" of "<<fromFac->university->name<<" to "<<toFac->name << " of "<<toFac->university->name
						<<" of same neighbourhood and request accepted ."<<endl;
					pthread_mutex_unlock(&logLock);
				}
				else{
					pthread_mutex_lock(&logLock);								//requet denied - enter in log
					logStream<<"Friend reqest sent from faculty "<<fromFac->name<<
						" of "<<fromFac->university->name<<" to "<<toFac->name << " of "<<toFac->university->name
						<<" of same neighbourhood and request denied ."<<endl;
					pthread_mutex_unlock(&logLock);
				}
			}
			else if(internalProb >50 && internalProb <= 75){				//code for interest
				bool foundOtherFac =  false;
				Faculty* toFac;
				uniform_int_distribution<int> dist(0,fromFac->interests.size()-1);
				int count_iter = 0;
				int count_max = 1;
				while(!foundOtherFac && (count_iter < count_max)){
					int ind = dist(rand_en_friend);
					uniform_int_distribution<int> facdist(0,fromFac->interests[ind]->faculties.size()-1);
					toFac =  fromFac->interests[ind]->faculties[facdist(rand_en_friend)];
					if(toFac->empID!=fromFac->empID && !	isFriendAlready(fromFac,toFac)){
						foundOtherFac = true;
						break;
					}
					count_iter++;
				}

				if (count_iter>count_max) return;
				if (!foundOtherFac) return;

				if(isHappen(toFac->university->friendliness)){
					pthread_mutex_lock(&Nlock);
					fromFac->friends.push_back(toFac);
					toFac->friends.push_back(fromFac);
					pthread_mutex_unlock(&Nlock);

					pthread_mutex_lock(&logLock);
					logStream<<"Friend request sent from faculty "<<fromFac->name<<
						" of "<<fromFac->university->name<<" to "<<toFac->name << " of "<<toFac->university->name
						<<" with same interest and request accepted ."<<endl;
					pthread_mutex_unlock(&logLock);	
				}
				else{
					pthread_mutex_lock(&logLock);
					logStream<<"Friend reqest sent from faculty "<<fromFac->name<<
						" of "<<fromFac->university->name<<" to "<<toFac->name << " of "<<toFac->university->name
						<<" with same interest and request denied ."<<endl;
					pthread_mutex_unlock(&logLock);
				}

			}
			else if(internalProb >75 && internalProb <= 100){				    //code for course
				return;
			}
		}
	}

	//executed if student is being selected to make request
	else{
		uniform_int_distribution<int> selFrmAlStud(0,N.students.size()-1);
		Student* fromStud =  N.students[selFrmAlStud(rand_en_friend)];

		if(isHappen(fromStud->university->openness)){      					//executed when the faculty chooses from any where
			bool foundOtherStud =  false;
			Student* toStud;
			int count_iter = 0;
			int count_max = 1;
			while(!foundOtherStud && (count_iter < count_max)){
				toStud =  N.students [selFrmAlStud(rand_en_friend)];
				if (!isFriendAlready(fromStud , toStud)){
					if ((toStud->entryNumber != fromStud->entryNumber) || 
						((toStud->entryNumber == fromStud->entryNumber) && (toStud->university->name.compare(fromStud->university->name) != 0))){
						foundOtherStud = true;
						break;
					}
				}
				count_iter++;
			}

			if (count_iter > count_max ) return;
			if (!foundOtherStud) return ;

			//request is being made from here i.e we are checking if the person is going to accpt or not
			if(isHappen(toStud->university->friendliness)){
				pthread_mutex_lock(&Nlock);
				fromStud->friends.push_back(toStud);
				toStud->friends.push_back(fromStud);	
				pthread_mutex_unlock(&Nlock);
				pthread_mutex_lock(&logLock);
				logStream<<"Friend request sent from student "<<fromStud->name<<
					" of "<<fromStud->university->name<<" to "<<toStud->name << " of "<<toStud->university->name
					<<" randomly and request accepted ."<<endl;
				pthread_mutex_unlock(&logLock);
			}
			else{
				pthread_mutex_lock(&logLock);
				logStream<<"Friend request sent from student "<<fromStud->name<<
					" of "<<fromStud->university->name<<" to "<<toStud->name << " of "<<toStud->university->name
					<<" randomly and request denied ."<<endl;
				pthread_mutex_unlock(&logLock);
			}
		}

		//executed when the student choses from inside his university
		else{
			uniform_int_distribution<int> selectProb(1,100);
			int internalProb = 	selectProb(rand_en_friend);

			if(internalProb <= 25){											//code for department
				bool foundOtherStud =  false;
				Student* toStud;
				uniform_int_distribution<int> studist(0,fromStud->dep->students.size()-1);
				int count_iter = 0;
				int count_max = 1;//pow(fromStud->dep->students.size() , 1.5);
				while(!foundOtherStud && (count_iter < count_max)) {
					toStud =  fromStud->dep->students[studist(rand_en_friend)];
					if(toStud->entryNumber != fromStud->entryNumber && !isFriendAlready(fromStud,toStud)){
						foundOtherStud = true;
						break;
					}
					count_iter++;
				}

				if (count_iter > count_max) return;
				if (!foundOtherStud) return ;

				if(isHappen(toStud->university->friendliness)){
					pthread_mutex_lock(&Nlock);
					fromStud->friends.push_back(toStud);
					toStud->friends.push_back(fromStud);
					pthread_mutex_unlock(&Nlock);
					pthread_mutex_lock(&logLock);
					logStream<<"Friend request sent from student "<<fromStud->name<<
						" of "<<fromStud->university->name<<" to "<<toStud->name << " of "<<toStud->university->name
						<<" with same department and request accepted ."<<endl;
					pthread_mutex_unlock(&logLock);
				}
				else{
					pthread_mutex_lock(&logLock);
					logStream<<"Friend request sent from student "<<fromStud->name<<
						" of "<<fromStud->university->name<<" to "<<toStud->name << " of "<<toStud->university->name
						<<" with same department and request denied ."<<endl;
					pthread_mutex_unlock(&logLock);
				}
			}
			else if(internalProb >25 && internalProb <= 50){				//code for neighbourhood
				bool foundOtherStud =  false;
				Student* toStud;

				uniform_int_distribution<int> studist(0,fromStud->hostel->hostlers.size()-1);
				int count_iter = 0;
				int count_max = 1;
				while(!foundOtherStud && (count_iter < count_max)){
					toStud =  fromStud->hostel->hostlers[studist(rand_en_friend)];
					if(toStud->entryNumber!=fromStud->entryNumber && !isFriendAlready(fromStud,toStud)){
						foundOtherStud = true;
						break;
					}
					count_iter++;
				}

				if (count_iter > count_max) return;
				if (!foundOtherStud) return ;

				if(isHappen(toStud->university->friendliness)){
					pthread_mutex_lock(&Nlock);
					fromStud->friends.push_back(toStud);
					toStud->friends.push_back(fromStud);
					pthread_mutex_unlock(&Nlock);
					pthread_mutex_lock(&logLock);
					logStream<<"Friend request sent from student "<<fromStud->name<<
						" of "<<fromStud->university->name<<" to "<<toStud->name << " of "<<toStud->university->name
						<<" with same neighbourhood and request accepted ."<<endl;
					pthread_mutex_unlock(&logLock);
				}
				else{
					pthread_mutex_lock(&logLock);
					logStream<<"Friend request sent from student "<<fromStud->name<<
						" of University "<<fromStud->university->name<<" to "<<toStud->name << " of university "<<toStud->university->name
						<<" with same neighbourhood and request denied ."<<endl;
					pthread_mutex_unlock(&logLock);
				}
			}
			else if(internalProb >50 && internalProb <= 75){				//code for interest
				bool foundOtherStud =  false;
				Student* toStud;
				uniform_int_distribution<int> dist(0,fromStud->interests.size()-1);
				int count_iter = 0;
				int count_max = 1;
				while(!foundOtherStud && (count_iter < count_max)){
					int ind = dist(rand_en_friend);
					uniform_int_distribution<int> studist(0,fromStud->interests[ind]->students.size()-1);

					toStud =  fromStud->interests[ind]->students[studist(rand_en_friend)];
					if(toStud->entryNumber!=fromStud->entryNumber &&  !isFriendAlready(fromStud,toStud)){
						foundOtherStud = true;
						break;
					}
					count_iter++;
				}

				if (count_iter > count_max) return;
				if (!foundOtherStud) return ;

				if(isHappen(toStud->university->friendliness)){
					pthread_mutex_lock(&Nlock);
					fromStud->friends.push_back(toStud);
					toStud->friends.push_back(fromStud);
					pthread_mutex_unlock(&Nlock);
					pthread_mutex_lock(&logLock);
					logStream<<"Friend request sent from student "<<fromStud->name<<
						" of "<<fromStud->university->name<<" to "<<toStud->name << " of "<<toStud->university->name
						<<" with same interest and request accepted ."<<endl;
					pthread_mutex_unlock(&logLock);
				}
				else{
					pthread_mutex_lock(&logLock);
					logStream<<"Friend request sent from student "<<fromStud->name<<
						" of "<<fromStud->university->name<<" to "<<toStud->name << " of "<<toStud->university->name
						<<" with same interest and request denied ."<<endl;
					pthread_mutex_unlock(&logLock);
				}

			}
			else if(internalProb >75 && internalProb <= 100){				//code for course
				bool foundOtherStud =  false;
				Student* toStud;
				uniform_int_distribution<int> dist(0,fromStud->coursesTaken.size()-1);
				int count_iter = 0;
				int count_max = 1;//pow(fromStud->coursesTaken.size() , 2);
				while(!foundOtherStud && (count_iter < count_max)){
					int ind = dist(rand_en_friend);
					uniform_int_distribution<int> studist(0,fromStud->coursesTaken[ind]->students.size()-1);

					toStud =  fromStud->coursesTaken[ind]->students[studist(rand_en_friend)];
					if(toStud->entryNumber!=fromStud->entryNumber && !isFriendAlready(fromStud,toStud)){
						foundOtherStud = true;
						break;
					}
					count_iter++;
				}
				if (count_iter > count_max) return ;
				if (!foundOtherStud) return ;

				if(isHappen(toStud->university->friendliness)){
					pthread_mutex_lock(&Nlock);
					fromStud->friends.push_back(toStud);
					toStud->friends.push_back(fromStud);
					pthread_mutex_unlock(&Nlock);
					pthread_mutex_lock(&logLock);
					logStream<<"Friend request sent from student "<<fromStud->name<<
						" of "<<fromStud->university->name<<" to "<<toStud->name << " of "<<toStud->university->name
						<<" with same course and request accepted ."<<endl;
					pthread_mutex_unlock(&logLock);					
				}
				else{
					pthread_mutex_lock(&logLock);
					logStream<<"Friend request sent from student "<<fromStud->name<<
						" of "<<fromStud->university->name<<" to "<<toStud->name << " of "<<toStud->university->name
						<<" with same course and request denied ."<<endl;
					pthread_mutex_unlock(&logLock);
				}
			}
		}
	}
}

int totalReqq;
void* initGenerateFriends(void* arg){
	pthread_mutex_lock(&cond_mutex[3]);
	isWaiting[3] = false;
	initRandomFriends();
	goto waiting;
genFriends:
	if (pthread_mutex_lock(&wLock)!=0) {printMutexLockError();exit(1);}
	isWorking[3] = true;
	if (pthread_mutex_unlock(&wLock)!=0) {printMutexLockError();exit(1);}

	//set the time
	if (pthread_mutex_lock(&tLock)!=0) {printMutexLockError();exit(1);}
	if (mtime > nextTimeToRun) {cerr<<"error in time .";exit(1);}
	if (mtime != nextTimeToRun) mtime = nextTimeToRun;
	if (pthread_mutex_unlock(&tLock)!=0) {printMutexLockError();exit(1);}

	for(int i=0;i<N.universities.size();i++){
		University* curUniv = N.universities[i];

		float number_returned = (*(distribution_arr[i]))(rand_en_friend);
		int totReqToBeMade= (int) (1/number_returned) ;
		if (totReqToBeMade > 1000) totReqToBeMade = 1000;
		totalReqq += totReqToBeMade;
		//int totReqToBeMade= 10 ;
		for(int j = 0; j < totReqToBeMade; j++){
			//friend requests = totReqToBeMade  are done in this loop for the curUniv
			doFriendRequest(curUniv);
		}																				
	}

	//send message to TK 
	//assuming all months to be 30 days
	nextTimeToRun = mtime + 24*60;
	sndmsgToTK(nextTimeToRun , 3);

	if (pthread_mutex_lock(&wLock)!=0) {printMutexLockError();exit(1);}
	isWorking[3] = false;
	if (pthread_mutex_unlock(&wLock)!=0) {printMutexLockError();exit(1);}
	
	//all work of this thread has been done .. let the main of generator proceed .. 
	if (pthread_mutex_lock(&cond_mutex[3])!=0) {printMutexLockError();exit(1);}
	isMainWaiting[3] = false;

		pthread_cond_signal(&condBack[3]);

waiting:
	if (pthread_mutex_trylock(&cond_mutex[3]) !=0){
		//lock not acquired .. so just continue
	} 
	else{
		//lock acquired
	}

	isWaiting[3] = true;
	while(isWaiting[3]){
		pthread_cond_wait(&cond[3] , &cond_mutex[3]);
	}
	if (pthread_mutex_unlock(&cond_mutex[3])!=0) {printMutexLockError();exit(1);}
	//if generateStudent is running stall this and goto the statement waiting and wait .. donott do anything ..
	pthread_mutex_lock(&cond_mutex[1]);
	bool x =  isMainWaiting[1];
	pthread_mutex_unlock(&cond_mutex[1]);

	pthread_mutex_lock(&cond_mutex[2]);
	bool y = isMainWaiting[2];
	pthread_mutex_unlock(&cond_mutex[2]);

	if (x || y) {
		//set the time
		if (pthread_mutex_lock(&tLock)!=0) {printMutexLockError();exit(1);}
		if (mtime > nextTimeToRun) {cerr<<"error in time .";exit(1);}
		if (mtime != nextTimeToRun) mtime = nextTimeToRun;
		if (pthread_mutex_unlock(&tLock)!=0) {printMutexLockError();exit(1);}
		
		nextTimeToRun = mtime + 24*60;
		sndmsgToTK(nextTimeToRun , 3);


		//all work of this thread has been done .. let the main of generator proceed .. 
		if (pthread_mutex_lock(&cond_mutex[3])!=0) {printMutexLockError();exit(1);}
		isMainWaiting[3] = false;
		pthread_cond_signal(&condBack[3]);

		goto waiting ;
	}
	goto genFriends;
	return NULL;
}