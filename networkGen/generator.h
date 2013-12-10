/*
 * generate.h
 *
 *  Created on: Oct 14, 2013
 *      Author: cs1120239
 */

#ifndef GENERATE_H_
#define GENERATE_H_

#include <pthread.h>

//message to send to timekeeper
struct mymsgToTimeKeeper{
	long mtype;
	long timeToWake;
	int reqBy;
};

//message to be received from timekeeper
struct mymsgToGenerator{
	long mtype;
	bool toWake[4];
};

	
void errnoMsgsnd();					//errors in msgsnd .. 
void errnoMsgrcv();					//errors in msgrcv
void errnoMsgget();					//errors in msgget
void sndmsgToTK(long twake, int by);//function to send message to TimeKeeper

#endif /* GENERATE_H_ */
