/*
 * timeakeeper.cpp
 *
 *  Created on: Oct 12, 2013
 *      Author: cs1120260
 */
#include <errno.h>
#include <cstdlib>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <queue>
#include "timekeeper.h"
#define MSGSZ 128

using namespace std;

static long TIME=0;

struct mymsgToTimeKeeper{
	long mtype;
	long timeToWake;
	int reqBy;
};

struct mymsgToGenerator{
	long mtype;
	bool toWake[4];
};

void printErrnoSnd(){
	cerr<<"Error in msgsnd TK."<<endl;
	switch(errno){
		case EAGAIN : {cout<<"EAGAIN"<<endl;break;}
		case EACCES : {cout<<"EACCES"<<endl;break;}
		case EFAULT : {cout<<"EFAULT"<<endl;break;}
		case EIDRM : {cout<<"EIDRM"<<endl;break;}
		case EINTR : {cout<<"EINTR"<<endl;break;}
		case EINVAL : {cout<<"EINVAL"<<endl;break;}
		case ENOMEM : {cout<<"ENOMEM"<<endl;break;}
	}
}

void printErrnoGet(){
	cerr<<"Error in msgget TK."<<endl;
	switch(errno){
		case EACCES : {cout<<"EACCES"<<endl;break;}
		case EEXIST : {cout<<"EEXIST"<<endl;break;}
		case EIDRM : {cout<<"EIDRM"<<endl;break;}
		case ENOENT : {cout<<"ENOENT"<<endl;break;}
		case ENOMEM : {cout<<"ENOMEM"<<endl;break;}
		case ENOSPC : {cout<<"ENOSPC"<<endl;break;}
	}
}

void printErrnoRcv(){
	cerr<<"Error in msgrcv TK ."<<endl;
	switch(errno){
		case E2BIG :{cout<<"E2BIG"<<endl;break;}
		case EACCES :{cout<<"EACCES"<<endl;break;}
		case EAGAIN :{cout<<"EAGAIN"<<endl;break;}
		case EFAULT :{cout<<"EFAULT"<<endl;break;}
		case EIDRM :{cout<<"EIDRM"<<endl;break;}
		case EINTR :{cout<<"EINTR"<<endl;break;}
		case EINVAL :{cout<<"EINVAL"<<endl;break;}
		case ENOMSG :{cout<<"ENOMSG"<<endl;break;}
	}
}	

priority_queue<myTuple,vector<myTuple>,CompareTuple> todolist;


string inttostring(int n){
	stringstream ss;
	ss<<n;
	string str=ss.str();
	return str;
}

void addintodolist(long timea, int funcno){
	if (!todolist.empty() && timea<TIME){
	}
	else{
		myTuple t;
		t.funcno=funcno;
		t.time=timea;
		todolist.push(t);
	}
}

myTuple recievemessagefromgenerator(){
	key_t keyf=121;
	int msgrid2=msgget(keyf, 0666|IPC_CREAT);
	if (msgrid2==-1){
		printErrnoGet();
		exit(1);
	}
	mymsgToTimeKeeper rbuf;
	int as=msgrcv(msgrid2, (void *)&rbuf, 128, 1, 0666);
	if (as==-1){
		printErrnoRcv();
		exit(1);
	}
	myTuple m;
	m.funcno=rbuf.reqBy;
	m.time=rbuf.timeToWake;
	return m;
}
/*
 *	Internal function for sending thereq. message
 * */
void sendreqmessage(int n1,int n2,int n3){
	bool bool2,bool3, bool4;
	key_t key=141;
	int msgsid=msgget(key, IPC_CREAT | 0666);
	if (msgsid==-1){
		printErrnoGet();
		exit(1);
	}
	mymsgToGenerator sbuf;
	sbuf.mtype = 1;
	if (n1==1 || n2==1 || n3==1){
		bool2=1;
	}
	if (n1==2 || n2==2 || n3==2){
		bool3=2;
	}
	if (n1==3 || n2==3 || n3==3){
		bool4=1;
	}
	sbuf.toWake[0]=0;
	sbuf.toWake[1]=bool2;
	sbuf.toWake[2]=bool3;
	sbuf.toWake[3]=bool4;
	//tested that the sizeof mymsgtogenerator - size of long = 8
	//so putting 8 in next line
	int asd = msgsnd(msgsid, &sbuf, 8, 0666|IPC_NOWAIT);
	if (asd == -1){
		printErrnoSnd();
		exit(1);
	}
}

/*
 * Sends the required message depending on which function to do next
 * It is only called when the generator sends a message that all the
 * threads are completed and is waiting for the message from the time
 * keeper.
 * */

void sendreqmessage(){
	if (!todolist.empty()){
		int tasks[2];
		myTuple t1=todolist.top();
		todolist.pop();
		tasks[0]=t1.funcno;
		TIME=t1.time;
		if (t1.time!=todolist.top().time){
			sendreqmessage(tasks[0],tasks[0],tasks[0]);
		}
		else{		// func1 and func2 at the same time
			myTuple t2=todolist.top();
			todolist.pop();
			tasks[1]=t2.funcno;
			if (t2.time!=todolist.top().time){ 		// func2 and func3 at different times
				sendreqmessage(tasks[0],tasks[1],tasks[1]);
			}
			else{ 			// all 3 funcs have the same to do timings
				todolist.pop();
				sendreqmessage(tasks[0],tasks[1],todolist.top().funcno);
			}
		}
	}
}

/*
 * The comparing operator for the priority queue
 * 
 */

bool CompareTuple::operator()(myTuple& tuple1,myTuple& tuple2){
	if (tuple1.time<tuple2.time){
		return false;
	}
	else if (tuple1.time==tuple2.time && tuple1.funcno<tuple2.funcno){
		return false;
	}
	else if (tuple1.time>tuple2.time){
		return true;
	}
	return true;
};

/*
 * 
 * 
 */
void getandsendmessages(){
	myTuple ms = recievemessagefromgenerator();
	long wktma=ms.time;
	int bya=ms.funcno;
	if (bya==10){
		sendreqmessage();
	}
	else{
		addintodolist( wktma, bya);
	}
	getandsendmessages();
}

int main(int argc, char** argv){
	long timelimit= atoi(argv[2])*24*60;
	while (TIME < timelimit){
		getandsendmessages();
	}
}