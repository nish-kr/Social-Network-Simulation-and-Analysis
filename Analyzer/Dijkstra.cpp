/*
 * Dijkstra.cpp
 *
 *  Created on: Nov 8, 2013
 *      Author: cs1120250
 */

#include "Dijkstra.h"
#include <iostream>
#include <algorithm>

using namespace std;

extern int INF;

bool comp(Node* x,Node* y){
	return x->weight < y->weight;
}

vector<Node*> dijk(string s1, string s2, Graph G){
	Node* starting;
	Node* ending;
	starting = G.searchNode(s1);
	ending = G.searchNode(s2);
	vector<Node*> testV;
	vector<Node*> visitedN;
	if (s1.compare(s2)==0){													//condition if path is to be found in same node
		vector<Node*> T;
				T.push_back(starting);
				return T;													//returns a node of size 1
	}
	if(starting->type.compare("faculty") == 0){
		testV =  G.faculty;
	}
	else{
		testV = G.student;
	}

	starting->weight = 0;

	make_heap(testV.begin(),testV.end(), comp);
	sort_heap(testV.begin(),testV.end(), comp);

	Node* curNod =  testV.front();
	visitedN.push_back(curNod);

	pop_heap (testV.begin(),testV.end(),comp);
	testV.pop_back();

	while((curNod->name.compare(s2) !=0) && (curNod->weight!=INF)){
		int len = curNod->neighbours.size();
		int fixW = curNod->weight + 1;

		for(int i = 0;i<len;i++){										// this loop resets the weights of the neighbours
			if(curNod->neighbours[i]->weight > fixW && (curNod->neighbours[i]->state.compare("visited") != 0)){
				curNod->neighbours[i]->weight = fixW;
				curNod->neighbours[i]->previous= curNod;
			}
		}

		curNod->state = "visited";
		make_heap(testV.begin(),testV.end(), comp);
		sort_heap(testV.begin(),testV.end(), comp);
		curNod =  testV.front();

		visitedN.push_back(curNod);

		pop_heap (testV.begin(),testV.end(),comp);
		testV.pop_back();
	}

	Node* cw = curNod;
	vector<Node*> path;
	if(cw->name.compare(s2)==0){										//when path has been found bet two nodes
		Node* temp;
		path.push_back(cw);
		bool fir = false;
		while (!fir){
			temp = cw->previous;
			if(temp->name.compare(s1)==0){
				path.push_back(temp);
				break;
			}
			path.push_back(temp);
			cw = temp;
		}
	}

	make_heap(path.begin(),path.end(), comp);
	sort_heap(path.begin(),path.end(), comp);

	int len = visitedN.size();
	for (int i=0;i<len;i++){
		visitedN[i]->state = "unvisited";
		visitedN[i]->weight = INF;
	}

	len = testV.size();
	for(int i = 0; i<len;i++){
		testV[i]->weight = INF;
	}

	if(curNod->name.compare(s2)!=0){										//when path has not been found
		vector<Node*> T;													//it returns a node of size zero
		return T;
	}

	return path;
}
