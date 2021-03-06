/*
 * main.cpp
 *
 *  Created on: Oct 10, 2013
 *      Author: cs1120239
 */

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <cstdlib>
#include <sstream>
#include <errno.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string>
using namespace std;
int numOfDays;


void printErrnoGet(){
	cerr<<"Error in msgget ."<<endl;
	switch(errno){
		case EACCES : {cout<<"EACCES"<<endl;break;}
		case EEXIST : {cout<<"EEXIST"<<endl;break;}
		case EIDRM : {cout<<"EIDRM"<<endl;break;}
		case ENOENT : {cout<<"ENOENT"<<endl;break;}
		case ENOMEM : {cout<<"ENOMEM"<<endl;break;}
		case ENOSPC : {cout<<"ENOSPC"<<endl;break;}
	}
}


int main(int argc, char * argv[]){
	numOfDays = atoi(argv[2]);
	string inputFile = argv[3];
	//create the 2 message queues before calling generator or timekeeper
	if (msgget(121 , IPC_CREAT|0666) == -1) {
		printErrnoGet();
		cerr<<"Error in main ."<<endl;
		exit(1);
	}

	if (msgget(141 , IPC_CREAT|0666) == -1) {
		printErrnoGet();
		cerr<<"Error in main ."<<endl;
		exit(1);
	}
	
	stringstream ss ;
	ss<<numOfDays;
	string s1 = ss.str();
	ss.str("");
	const char* c1 = s1.c_str();
	pid_t pid = fork();
	if (pid < 0){
		cerr<<"Error forking .Exiting .";
		exit(1);
	}
	else if (pid == 0){
		//child process - this is the timekeeper and its PID is returned to the parent process 
		execlp("./timekeeper" , "./timekeeper" , "-d" , c1 , NULL);
		cerr<<"Error execlp failed ."<<endl;
	}
	else{
		ss<<pid;
		string s2 = ss.str();
		const char* c2 = s2.c_str();
		const char* c3 = inputFile.c_str();
		//parent process - the generator
		execlp("./generator" , "./generator" , "-d" , c1 , c2 ,c3 , NULL);
		cerr<<"Error execlp failed ."<<endl;
	}
}