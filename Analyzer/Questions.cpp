/*
 * Questions.cpp
 *
 *  Created on: Nov 11, 2013
 *      Author: cs1120250
 */

#include "Questions.h"
#include <vector>
#include <sstream>
#include <iostream>
#include "Dijkstra.h"				//delete both the header file and make a new header file
#include "FloydWar.h"
using namespace std;

extern Graph N;
extern int maxSort;
extern int start;
extern int end;
extern string spTyp;
extern int **facDistArr;
extern int **studDistArr;
vector<Node*> tempGr;
vector<Node*> Gr;

bool isClique(){
	bool isCl = true;
	for(int i=0;i<tempGr.size();i++){
		for(int j = 0;j<tempGr.size();j++){
			if(i == j){continue;}
			if(!tempGr[i]->chkNode(tempGr[j]->name)){
				isCl = false;
				break;
			}
			if(!isCl){break;}
		}
		if(!isCl){break;}
	}
	return isCl;
}

int ithBitis(int i , int num){
	num = num>>i;
	return num % 2;
}

int clqMax(Node* nod){
	int max=1;
	if(nod->neighbours.size() == 0){
		int x = 1;
		return x;
	}
	if(nod->neighbours.size() == 1){
			int x = 2;
			return x;
	}
	for(int i = 0;i<nod->neighbours.size();i++){
		Node* cur = nod->neighbours[i];
		bool toPut =false;
		for(int j = 0;j<nod->neighbours.size();j++){
			if(i == j) continue;
			if( cur->chkNode(nod->neighbours[j]->name)){
				toPut = true;
				break;
			}
		}
		if(toPut) Gr.push_back(cur);
	}
	int k = Gr.size();
	int temp = 1;
	for(int j =0;j<k;j++) {temp = temp*2;}								//we would have this many subsets
	//iterate over these sets one by one
	for(int l = 0;l<temp;l++){
		for(int m =0 ;m<k;m++){
			if(ithBitis(m,l) == 1){
				tempGr.push_back(Gr[m]);								//this code is generating a new graph
			}
		}
		if(isClique()){
			if(max < tempGr.size()){
				max = tempGr.size();
			}
		}
		tempGr.clear();
	}
	Gr.clear();
	return max + 1;
}

int QuesOne(string s){													// Size of Clique of a given person
	Node* T = N.searchNode(s);
	return clqMax(T);
}

int QuesTwo(string s1, string s2){										//The length of the shortest path between two given people

	if (s1.compare(s2) == 0) return -1;
	int x1 = s1.find_first_of("_");
	char c1 = s1[x1 +1];
	int x2 = s2.find_first_of("_");
	char c2 = s2[x2 +1];
	if(c1 != c2 ) {return -1;}

	Node* t1 = N.searchNode(s1);
	Node* t2 = N.searchNode(s2);
	if (c1=='s'){
		return studDistArr[t1->indexs][t2->indexs];
	}
	else{
		return facDistArr[t1->indexs][t2->indexs];
	}
}



string QuesThree(string s1, string s2){									//The list of people on the shortest path between two given people
	int x1 = s1.find_first_of("_");
	char c1 = s1[x1 +1];
	int x2 = s2.find_first_of("_");
	char c2 = s2[x2 +1];
	if(c1 != c2 ) {return "-1";}
	if(s1.compare(s2)==0) {return "-1";}
	string s = route(s1,s2);
	if(s.compare("no existent route")==0) {return "-1";}
	return s;
}

string QuesFour(){														//The shortest path in the graph
	if(maxSort == 1){
		string s1 =  N.fEdg[0]->source->name;
		string s2 = N.fEdg[0]->target->name;
		return "$"+s1+"$"+s2;
	}


	string s1;
	string s2;
	string res;
	if(spTyp == "student"){
		s1 =N.student[start]->name;
		s2 = N.student[end]->name;
		ostringstream ostr;
		ostr << maxSort;
		string th = ostr.str();
		res = "$"+s1 +  QuesThree(s1,s2)+ "$"+s2;
	}
	else{
		s1 =N.faculty[start]->name;
		s2 = N.faculty[end]->name;
		ostringstream ostr;
		ostr << maxSort;
		string th = ostr.str();
		res = "$"+s1+ QuesThree(s1,s2) + "$" + s2;
	}
	return res;
}


int QuesFive(string s){												//The importance of the given person
	int temp = N.searchNode(s)->imp;
	return temp;
}

string QuesSix(string s){											//Is any of the friends of a given person more important that him/he
	Node* nod = N.searchNode(s);
	int temp = nod->imp;
	bool first = true;
	string retS = "no";
	for(int i = 0; i<nod->neighbours.size();i++){
		Node* curNod = nod->neighbours[i];
		if(temp< curNod->imp){
			if(first){
				retS = "yes" ;
				first = false;
			}
			retS = retS+"$" + curNod->name ;
		}
	}
	return retS;
}

