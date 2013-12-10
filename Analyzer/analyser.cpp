/*
 * main.cpp
 *
 *  Created on: Nov 8, 2013
 *      Author: cs1120250
 */
#include <cstring>
#include <string>
#include <vector>
#include <queue>
#include <cstdlib>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include "Network.h"
#include "Dijkstra.h"
#include "FloydWar.h"
#include "Questions.h"
#include "parser.h"

key_t keyFromGyani = 101;
key_t keyToGyani = 111;
int mqFromGyani , mqToGyani;

struct msgAnsToGyani{
	long mtype;
	char s[256];
};

struct msgFromGyani{
	long mtype;
	char s[128];
};

int INF = 10000000;
Graph N;

int **facGrArr;
int **studGrArr;

int **facDistArr;
int **studDistArr;

int **facPathArr;
int **studPathArr;

int maxSort;
int start;
int end;
string spTyp;

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

//extracts the tokens of the string s splitted by delim
static vector<string> extractTokens(string s , string delim){
	char arr[s.size()];
	strcpy(arr , s.c_str());
	const char* de = delim.c_str();
	char* te = strtok(arr, de);
	vector<string> toret;
	while(te!=NULL){
		toret.push_back(string((const char*) te));
		te = strtok(NULL , de );
	}
	return toret;
}

void cleanUp(){
	int facSize = N.faculty.size();
	int studSize = N.student.size();
	for(int i=0;i<facSize;i++){
		delete[] facDistArr[i];
		delete[] facGrArr[i];
		delete[] facPathArr[i];
	}
	for(int i=0;i<studSize;i++){
		delete[] studDistArr[i];
		delete[] studGrArr[i];
		delete[] studPathArr[i];
	}
	//delete the message queue
	if (msgctl(mqFromGyani , IPC_RMID , NULL) == -1){
		cerr<<"Error deleting message queue.";
		exit(1);
	}
	if (msgctl(mqToGyani , IPC_RMID , NULL) == -1){
		cerr<<"Error deleting message queue.";
		exit(1);
	}

	//delete the network - destructor in network.cpp .. 
}

void sndMsgToGyani(int x){
	msgAnsToGyani ms;
	ms.mtype = 1;
	stringstream ss;
	ss<<x;
	string sr = ss.str();
	memcpy(ms.s , sr.c_str() , sr.size());
	ms.s[sr.size()] = '\0';
	if (msgsnd(mqToGyani,&ms, 256 ,0666 | IPC_NOWAIT) == -1){
		errnoMsgsnd();
		cerr<<"Error in main of Generator .Exiting."<<endl;
		exit(1);
	}
}

void sndMsgToGyani(string s){	
	vector<string> vec = extractTokens(s , "$");
	msgAnsToGyani ms;
	ms.mtype = 1;
	for(int i=0;i<vec.size();i++){
		memcpy(ms.s , vec[i].c_str() , vec[i].size());
		ms.s[vec[i].size()] = '\0';
		if (msgsnd(mqToGyani,&ms, 256 ,0666 | IPC_NOWAIT) == -1){
			errnoMsgsnd();
			cerr<<"Error in main of Generator .Exiting."<<endl;
			exit(1);
		}
	}
	string str = "end      ";
	memcpy(ms.s , str.c_str() , str.size());
	ms.s[str.size()] = '\0';
	if (msgsnd(mqToGyani,&ms, 256 ,0666 | IPC_NOWAIT) == -1){
		errnoMsgsnd();
		cerr<<"Error in main of Generator .Exiting."<<endl;
		exit(1);
	}
}

int main(){
	//all the following are initializations
	msgFromGyani msg;
	int bytesR;
	mqToGyani = msgget(keyToGyani , 0666|IPC_CREAT);
	if (mqToGyani == -1){
		errnoMsgget();
		cerr<<"Error in main of Generator .Exiting ."<<endl;
		exit(1);
	}
	mqFromGyani = msgget(keyFromGyani , 0666|IPC_CREAT);
	if (mqFromGyani == -1){
		errnoMsgget();
		cerr<<"Error in main of Generator .Exiting ."<<endl;
		exit(1);
	}
	
	bytesR = msgrcv(mqFromGyani,(void*)&msg , 128, 1, 0666);
	if (bytesR == -1){
		errnoMsgrcv();
		cerr<<"Error in msgrcv in main of Analyser .Exiting."<<endl;
		exit(1);
	}

	vector<string> xx = extractTokens(string((const char*)msg.s) , " ");

	string gFile = xx[0];
	parseGraphML(gFile);
	int facSize = N.faculty.size();
	int studSize = N.student.size();
	facGrArr = new int*[facSize];
	facDistArr = new int*[facSize];
	facPathArr = new int*[facSize];
	studDistArr = new int*[studSize];
	studGrArr = new int*[studSize];
	studPathArr = new int*[studSize];
	for(int i=0;i<facSize;i++){
		facDistArr[i] = new int[facSize];
		facGrArr[i] = new int[facSize];
		facPathArr[i] = new int[facSize];
	}
	for(int i=0;i<studSize;i++){
		studDistArr[i] = new int[studSize];
		studGrArr[i] = new int[studSize];
		studPathArr[i] = new int[studSize];
	}
	flWr();
	impCal();
	
	//initializations done .. now start to send and receive messages from gyani

	
	//send message to gyani that floyd warshall is done...
	sndMsgToGyani(0);
	while(true){
		bytesR = msgrcv(mqFromGyani,(void*)&msg , 128, 1, 0666);
		if (bytesR == -1){
			errnoMsgrcv();
			cerr<<"Error in msgrcv in main of Analyser .Exiting."<<endl;
			exit(1);
		}
		//msg.s is a 128 byte char array
		string input = string((const char*)msg.s);
		vector<string> message = extractTokens(input , ", \t");
		if (message[0].compare("end") == 0){
			//gyani has requested perl to exit
			cleanUp();
			exit(0);
		}
		int queNo = atoi(message[0].c_str());
		int x ; string s1,s2,s;
		switch(queNo){
			case 1:
				s = message[1]+"_"+message[2]+"_"+message[3];
				x = QuesOne(s);
				sndMsgToGyani(x);
				break;
			case 2:
				s1 = message[1]+"_"+message[2]+"_"+message[3];
				s2 = message[4]+"_"+message[5]+"_"+message[6];
				x = QuesTwo(s1 , s2);
				sndMsgToGyani(x);
				break;
			case 3:
				s1 = message[1]+"_"+message[2]+"_"+message[3];
				s2 = message[4]+"_"+message[5]+"_"+message[6];
				s = QuesThree(s1 ,s2);
				sndMsgToGyani(s);
				break;
			case 4:
				s = QuesFour();
				sndMsgToGyani(s);
				break;
			case 5:
				s = message[1]+"_"+message[2]+"_"+message[3];
				x = QuesFive(s);
				sndMsgToGyani(x);
				break;
			case 6:
				s1 = message[1]+"_"+message[2]+"_"+message[3];
				s = QuesSix(s1);
				if (s[0]=='y'){
					//yes
					sndMsgToGyani(1);
				}
				else if (s[0] == 'n'){
					//no
					sndMsgToGyani(0);
				}
				break;
		}
	}
}
