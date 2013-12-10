/*
 * classes.cpp
 *
 *  Created on: Nov 8, 2013
 *      Author: cs1120250
*/



#include "Network.h"
#include <iostream>
using namespace std;
extern int INF;

Node::Node(string n){
	this->name = n;
	this->imp = 0;
	this->weight = INF;
	int i = n.find_first_of("_");
	char c = n[i+1];
	if (c=='f'){
		this->type = "faculty";
	}
	else{
		this->type = "student";
	}
	this->state = "unvisited";
}

Node::Node(){}

bool Node::chkNode(string s){
	int len = this->neighbours.size();
	bool temp = false;
	for(int i = 0 ; i<len;i++){
		if (this->neighbours[i]->name.compare(s) == 0){
			temp = true;
			break;
		}
	}
	return temp;
}

int Graph::indexNode(string s){
	int cur;
	int i = s.find_first_of("_");
	char c = s[i+1];
	int len;
	if (c=='s'){				// search in student
		len = this->student.size();
		for(int j = 0; j<len; j++){
			if(this->student[j]->name.compare(s) == 0){
				cur = j;
				break;
			}
		}
	}
	else{
		len = this->faculty.size();
		for(int j = 0; j<len; j++){
			if(this->faculty[j]->name.compare(s) == 0){
				cur = j;
				break;
			}
		}
	}
	return cur;
}


Node* Graph::searchNode(string s){
	Node* cur;
	int i = s.find_first_of("_");
	char c = s[i+1];
	int len;
	if (c=='s'){				// search in student
		len = this->student.size();
		for(int j = 0; j<len; j++){
			if(this->student[j]->name.compare(s) == 0){
				cur = this->student[j];
				break;
			}
		}
	}
	else{
		len = this->faculty.size();
		for(int j = 0; j<len; j++){
			if(this->faculty[j]->name.compare(s) == 0){
				cur = this->faculty[j];
				break;
			}
		}
	}
	return cur;
}

bool Graph::chkEdg(string s1, string s2){
	int i = s1.find_first_of("_");
		char c = s1[i+1];
		int len;
		bool temp =  false;
		if (c=='s'){				// search in student
			len = this->sEdg.size();
			for(int j = 0; j<len; j++){
				if(((this->sEdg[j]->source->name.compare(s1) == 0) && (this->sEdg[j]->target->name.compare(s2) == 0)
					)||(
					(this->sEdg[j]->source->name.compare(s2) == 0) && (this->sEdg[j]->target->name.compare(s1) == 0))){
					temp = true;
					break;
				}
			}
		}
		else{
			len = this->fEdg.size();
			for(int j = 0; j<len; j++){
				if(((this->fEdg[j]->source->name.compare(s1) == 0) && (this->fEdg[j]->target->name.compare(s2) == 0))
						||
					((this->fEdg[j]->source->name.compare(s2) == 0) && (this->fEdg[j]->target->name.compare(s1) == 0)))
				{
					temp = true;
					break;
				}
			}
		}
		return temp;
}

Graph::~Graph(){
	for(int i=0;i<this->faculty.size();i++){
		delete this->faculty[i];
	}
	for(int i=0;i<this->student.size();i++){
		delete this->student[i];
	}
	for(int i=0;i<this->sEdg.size();i++){
		delete this->sEdg[i];
	}
	for(int i=0;i<this->fEdg.size();i++){
		delete this->fEdg[i];
	}
}
