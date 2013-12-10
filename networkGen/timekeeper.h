/*
 * timekeeper.h
 *
 *  Created on: Oct 13, 2013
 *      Author: cs1120260
 */

#ifndef TIMEKEEPER_H_
#define TIMEKEEPER_H_

#include <iostream>
#include <vector>

using namespace std;

//class to store alarm requests from generator
class myTuple{
	public:
		long time;
		int funcno;
};

//clas made for comparison ...
class CompareTuple{
	public:
		bool operator()(myTuple& tuple1,myTuple& tuple2);
};

#endif /* TIMEKEEPER_H_ */
