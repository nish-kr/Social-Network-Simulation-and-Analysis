/*
 * generateFriends.h
 *
 *  Created on: Oct 12, 2013
 *      Author: cs1120239
 */

#ifndef GENERATEFRIENDS_H_
#define GENERATEFRIENDS_H_

bool isHappen(float p);								//tells if the event will happen given the probability of happening is p
void initRandomFriends();							//initializes everything
void doFriendRequest();								//function to execute the friend request
void* initGenerateFriends(void*);					//main function that is run by the thread

#endif /* GENERATEFRIENDS_H_ */
