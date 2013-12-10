/*
 * generate.cpp
 *
 *  Created on: Oct 14, 2013
 *      Author: cs1120239
 */
#include "Network.h"
#include "generator.h"
#include "generateStudent.h"
#include "generateFaculty.h"
#include "generateCourses.h"
#include "generateFriends.h"
#include "setEnvironment.h"
#include "graphml.h"
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sstream>
#include <string>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <unistd.h>
using namespace std;

extern int totalReqq;
Network N;
string logFile ;
ofstream logStream ;
string inputFile;
key_t keytoTK = 121;			//key for the message queue used for sending messages to TimeKeeper
key_t keytoGE = 141;			//key for the message queue used for sending messages to Generator
int mqidtoTK ;		    		//msgq ID for the message queue used for sending messages to TimeKeeper
int mqidtoGE ;  				//msgq ID for the message queue used for sending messages to Generator
pid_t pidTK;					//process ID of the timekeeper : used to manually kill the timekeeper

pthread_t threads[4];			//the arrray containing the 4 threads
long mtime;						//the currernt time
int numOfDays;					//the number of days for which the simulation is done
static long timelimit ; 		//the timelimit for simulation
pthread_cond_t cond[4];			//array of conditions on which the threads wait
bool isWaiting[4];				//array of 4 booleans to test if that thread is waiting
bool isWorking[4];				//array of 4 booleans to test if that thread is working
pthread_mutex_t Nlock;			//lock to for blocking accesses to Network
pthread_mutex_t cond_mutex[4];	//4 conditional mutexes used for each thread
pthread_mutex_t logLock;		//mutex to lock access to logFile
pthread_mutex_t wLock;			//lock for locking access to isWorking
pthread_mutex_t tLock;			//lock for time ..
pthread_cond_t condBack[4];		//used by the threads to signal Generator thread that its done
bool isMainWaiting[4];			//used to ensure that main of generator proceeds only when other threads are complete......
mymsgToGenerator msg;			//the message received from Generator

								//distributions array containing each university's exponential distribution
extern exponential_distribution<float>** distribution_arr;		

//for printing errors in msgsnd
void errnoMsgsnd(){
	cerr<<"Error in msgsnd ."<<endl;
	switch(errno){
		case EAGAIN : {cerr<<"EAGAIN"<<endl;break;}
		case EACCES : {cerr<<"EACCES"<<endl;break;}
		case EFAULT : {cerr<<"EFAULT"<<endl;break;}
		case EIDRM : {cerr<<"EIDRM"<<endl;break;}
		case EINTR : {cerr<<"EINTR"<<endl;break;}
		case EINVAL : {cerr<<"EINVAL"<<endl;break;}
		case ENOMEM : {cerr<<"ENOMEM"<<endl;break;}
	}
}

//for printing errors in msgget
void errnoMsgget(){
	cerr<<"Error in msgget ."<<endl;
	switch(errno){
		case EACCES : {cerr<<"EACCES"<<endl;break;}
		case EEXIST : {cerr<<"EEXIST"<<endl;break;}
		case EIDRM : {cerr<<"EIDRM"<<endl;break;}
		case ENOENT : {cerr<<"ENOENT"<<endl;break;}
		case ENOMEM : {cerr<<"ENOMEM"<<endl;break;}
		case ENOSPC : {cerr<<"ENOSPC"<<endl;break;}
	}
}

//for printing errors in msgrcv
void errnoMsgrcv(){
	cerr<<"Error in msgrcv jk"<<endl;
	switch(errno){
		case E2BIG :{cerr<<"E2BIG"<<endl;break;}
		case EACCES :{cerr<<"EACCES"<<endl;break;}
		case EAGAIN :{cerr<<"EAGAIN"<<endl;break;}
		case EFAULT :{cerr<<"EFAULT"<<endl;break;}
		case EIDRM :{cerr<<"EIDRM"<<endl;break;}
		case EINTR :{cerr<<"EINTR"<<endl;break;}
		case EINVAL :{cerr<<"EINVAL"<<endl;break;}
		case ENOMSG :{cerr<<"ENOMSG"<<endl;break;}
	}
}

//function for printing errors in locking
static void printMutexLockError(){
	cerr<<"Error in Mutex locking -  main of generator."<<endl;
	switch(errno){
		case E2BIG :{cerr<<"EINVAL"<<endl;break;}
		case EACCES :{cerr<<"EBUSY"<<endl;break;}
		case EAGAIN :{cerr<<"EAGAIN"<<endl;break;}
		case EFAULT :{cerr<<"EDEADLK"<<endl;break;}
		case EIDRM :{cerr<<"EPERM"<<endl;break;}
	}
}

//sends message to TimeKeeper containing the time to wake and the thread's id
void sndmsgToTK(long twake, int by){
	mymsgToTimeKeeper msg ;
	msg.mtype = 1;
	msg.timeToWake = twake;
	msg.reqBy = by;
	if (msgsnd(mqidtoTK , &msg , 16 , 0666 | IPC_NOWAIT) == -1){
		errnoMsgsnd();
		cerr<<"Error in main of Generator .Exiting."<<endl;
		exit(1);
	}
}

//sends the message to TK - " I am done ."
void sndmsgToTKDone(){
	mymsgToTimeKeeper msgtk;
	msgtk.mtype = 1;
	msgtk.timeToWake = 0;
	msgtk.reqBy = 10;
	if (msgsnd(mqidtoTK , &msgtk , 16 , 0666 | IPC_NOWAIT) == -1){
		errnoMsgsnd();
		cerr<<"Error in main of Generator .Exiting."<<endl;
		exit(1);
	}
}

//called on exiting .. to free all the space
void cleanUp(){
	logStream.close();
	//remove space taken by conditions , mutex , message queues
	for(int i=0;i<4;i++) {
		pthread_cond_destroy(&cond[i]);
		pthread_mutex_destroy(&cond_mutex[i]);
	}

	if (msgctl(mqidtoTK , IPC_RMID , NULL) == -1){
		cerr<<"Error deleting message queue.";
		exit(1);
	}
	if (msgctl(mqidtoGE , IPC_RMID , NULL) == -1){
		cerr<<"Error deleting message queue.";
		exit(1);
	}

	//cleanup and delete the space taken by expoenential distribution
	for(int i=0;i<N.universities.size();i++){
		delete distribution_arr[i];
	}
	delete[] distribution_arr;

	//the Network destructor is called automatically
	//and it destructs the whole thing
}

//prints the universities in the file named - "universities.txt"
void outputUniv(){
	ofstream file;
	file.open("universities.txt");
	if (!file.is_open()){
		cerr<<"Error reading input file : " <<inputFile<<" .Exiting ."<<endl;
		exit(1);
	}
	for(int i=0;i<N.universities.size();i++){
		file<<N.universities[i]->name<<"\n";
	}
}	


//the main
int main(int argn , char** args){
	logFile = "logFile.txt";

	//the initial read position is at the beginning of the file ,so the file will be overwritten
	logStream.open(logFile.c_str());
	inputFile = args[4];

	mtime = 0;
	numOfDays = atoi(args[2]);
	timelimit = numOfDays*24*60;												//number of seconds as the timelimit
	pidTK = atoi(args[3]);														//PID of the TK is given as an argument

	for(int i=0;i<4;i++){
		isWaiting[i] = true;
	}
	for(int i=0;i<4;i++){
		isWorking[i] = false;
	}

	for(int i=0;i<4;i++){
		isMainWaiting[i] = false;
	}

	
	pthread_mutex_init(&Nlock , NULL);
	pthread_mutex_init(&logLock , NULL);
	pthread_mutex_init(&wLock , NULL);
	pthread_mutex_init(&tLock , NULL);

	for(int i=0;i<4;i++){
		pthread_cond_init(&cond[i] , NULL);										//initializes all the conditional variables 
																				//on which threads wait
		pthread_cond_init(&condBack[i] , NULL);
		pthread_mutex_init(&cond_mutex[i] , NULL);
	}	

	mqidtoTK = msgget(keytoTK , 0666|IPC_CREAT);
	if (mqidtoTK == -1){
		errnoMsgget();
		cerr<<"Error in main of Generator .Exiting ."<<endl;
		exit(1);
	}

	mqidtoGE = msgget(keytoGE , 0666|IPC_CREAT);
	if (mqidtoGE == -1){
		errnoMsgget();
		cerr<<"Error in main of Generator .Exiting ."<<endl;
		exit(1);
	}

	parseInputFile(inputFile);
	pthread_create(&threads[0] , NULL, initGenerateFaculty , NULL);
	pthread_join(threads[0] , NULL);											//wait for generateFaculty to complete
	
	pthread_mutex_lock(&cond_mutex[1]);
	pthread_create(&threads[1] , NULL, initGenerateStudent , NULL);
	pthread_mutex_unlock(&cond_mutex[1]);
	
	pthread_mutex_lock(&cond_mutex[2]);
	pthread_create(&threads[2] , NULL, initGenerateCourse , NULL);
	pthread_mutex_unlock(&cond_mutex[2]);
	

	pthread_mutex_lock(&cond_mutex[3]);
	pthread_create(&threads[3] , NULL, initGenerateFriends , NULL);
	pthread_mutex_unlock(&cond_mutex[3]);
	
	usleep(100);

	sndmsgToTK(0 , 1);
	sndmsgToTK(0 , 2);
	sndmsgToTK(0 , 3);
	sndmsgToTKDone();

	int iterations = 1;
	while(mtime < timelimit){
		if (msgrcv(mqidtoGE , (void*)&msg , 8 , 1 , 0666) == -1){
			errnoMsgrcv();
			cerr<<"Error in main of Generator .Exiting ."<<endl;
			exit(1);
		}

		for(int i=0;i<4;i++){
			isMainWaiting[i] = false;
		}
		for(int i=0;i<4;i++){
			if (msg.toWake[i]){
				//if the timeKeeper packed this thread to wake
				pthread_mutex_lock(&cond_mutex[i]);
				isWaiting[i] = false;
				isMainWaiting[i] = true;
				pthread_cond_signal(&cond[i]);
				pthread_mutex_unlock(&cond_mutex[i]);
			}
		}

		//stall this thread till all others have completed .. use isMainWaiting

		pthread_mutex_lock(&cond_mutex[1]);
		while(isMainWaiting[1]){
			pthread_cond_wait(&condBack[1] , &cond_mutex[1]);
		}
		pthread_mutex_unlock(&cond_mutex[1]);

		pthread_mutex_lock(&cond_mutex[2]);
		while(isMainWaiting[2]){
			pthread_cond_wait(&condBack[2] , &cond_mutex[2]);
		}
		pthread_mutex_unlock(&cond_mutex[2]);

		pthread_mutex_lock(&cond_mutex[3]);
		while(isMainWaiting[3]){
			pthread_cond_wait(&condBack[3] , &cond_mutex[3]);
		}
		pthread_mutex_unlock(&cond_mutex[3]);
		sndmsgToTKDone();
		iterations++;
	}

	saveGraphML();

	//print the universities in a file named universities.txt
	outputUniv();
	int x = 0;
	for(int i=0;i<N.universities.size();i++){
		x+=N.universities[i]->students.size();
	}
	int y = 0;
	for(int i=0;i<N.universities.size();i++){
		y+=N.universities[i]->faculties.size();
	}
	for(int i=1;i<4;i++) pthread_cancel(threads[i]);
	stringstream ss;
	ss<<pidTK;
	string command = "kill -9 " + ss.str();
	system(command.c_str());															//terminates the TimeKeeper manually
	cleanUp();
}
 
