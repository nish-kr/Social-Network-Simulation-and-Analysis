/*
 * FloydWar.cpp
 *
 *  Created on: Nov 9, 2013
 *      Author: cs1120250
 */


#include "FloydWar.h"
#include "Network.h"
#include <iostream>
using namespace std;

extern int INF;
extern Graph N;
extern int **facPathArr;
extern int **studPathArr;
extern int **facGrArr;
extern int **studGrArr;
extern int **facDistArr;
extern int **studDistArr;
extern int maxSort;
extern int start;
extern int end;
extern string spTyp;

void flWr(){
	int lenF = N.faculty.size();
	int  i,j ,k;
								//facGrArr is the graph in matrix format
								//facDistArr is the shortest path distance in graph array
								//facPathArr what comes in shortest path
	maxSort =1;
	for(int i = 0 ; i< lenF;i++){
			for(int j =0; j<lenF;j++ ){
				facPathArr[i][j] = -1;
				facGrArr[i][j] = INF;
				facDistArr[i][j] = INF;
				if(i==j){
					facGrArr[i][j] = 0;
					facDistArr[i][j] = 0;
				}
			}
		}
	//checking every edge
	for(int i = 0; i<N.fEdg.size();i++){
		Edge*  curEdg = N.fEdg[i];
		int k = N.fEdg[i]->source->indexs;
		int l = N.fEdg[i]->target->indexs;
		if(k==l) continue;
		facGrArr[k][l] = 1;
		facDistArr[k][l] =  facGrArr[k][l];
		facGrArr[l][k] = 1;
		facDistArr[l][k] =  facGrArr[l][k];
	}
	//flwr for faculty started
	for (k = 0; k < lenF; k++){
		for (i = 0; i < lenF; i++){
			for (j = 0; j < lenF; j++){
				if (facDistArr[i][k] + facDistArr[k][j] < facDistArr[i][j]){
				facDistArr[i][j] = facDistArr[i][k] + facDistArr[k][j];
				facPathArr[i][j] = k;
				}
			}
		}
	}
	//flwr for faculty ended
	for(i=0;i<lenF;i++){
		for(j=0;j<lenF;j++){
			if (maxSort < facDistArr[i][j] && facDistArr[i][j] < INF ){								//this loop gives the largest
				maxSort = facDistArr[i][j];															// short path
				start = i;
				end = j;
				spTyp = "faculty";
			}
		}
	}
	//starting from here the array of distances
	//is created for the student
	int lenS = N.student.size();
	//puting every student to infinity
	for(int i = 0 ; i< lenS;i++){
		for(int j =0; j<lenS;j++ ){
			studPathArr[i][j] = -1;
			studGrArr[i][j] = INF;
			studDistArr[i][j] = INF;
			if(i==j){
				studGrArr[i][j] = 0;
				studDistArr[i][j] = 0;
			}
		}
	}


	//puting every edge to 1
	for(int i = 0; i<N.sEdg.size();i++){
		Edge*  curEdg = N.sEdg[i];
		int k = N.sEdg[i]->source->indexs;
		int l = N.sEdg[i]->target->indexs;
		if(k==l) continue;
		studGrArr[k][l] = 1;
		studDistArr[k][l] =  studGrArr[k][l];
		studGrArr[l][k] = 1;
		studDistArr[l][k] =  studGrArr[l][k];
	}

	//starting flwr for students
	for (k = 0; k < lenS; k++){
		for (i = 0; i < lenS; i++){
			for (j = 0; j < lenS; j++){
				if (studDistArr[i][k] + studDistArr[k][j] < studDistArr[i][j]){
				studDistArr[i][j] = studDistArr[i][k] + studDistArr[k][j];
				studPathArr[i][j] = k;																//write thr prev in the array
				}
			}
		}
	}
	//flwr for student ended
	for(i=0;i<lenS;i++){
		for(j=0;j<lenS;j++){
			if (maxSort < studDistArr[i][j] && studDistArr[i][j] < INF ){							//this loop gives the largest
				maxSort = studDistArr[i][j];														// short path
				start = i;
				end = j;
				spTyp = "student";
			}
		}
	}

}




void Spath(int i,int j){
	if(studPathArr[i][j] == -1 || i==j){}
	else{
		int k = studPathArr[i][j];
		N.student[k]->imp++ ;
		Spath(i,k);
		Spath(k,j);
	}
}

void Fpath(int i,int j){
	if(facPathArr[i][j] == -1|| i==j){	}
	else{
		int k = facPathArr[i][j];
		N.faculty[k]->imp++ ;
		Fpath(i,k);
		Fpath(k,j);
	}
}

string route(string s1, string s2){
	int i = s1.find_first_of("_");
	char c = s1[i+1];
	if (c=='f'){
		i = N.indexNode(s1);
		int j = N.indexNode(s2);
		if(facPathArr[i][j] == -1 ){
			if(!N.chkEdg(N.faculty[i]->name,N.faculty[j]->name) && i!=j){
				return "no existent route";
			}
			return "";
		}
		else{
			int k = facPathArr[i][j];
			string a = N.faculty[k]->name;
			string b = route(s1,a);
			string c =  route(a,s2);
			if(b.compare("no existent route") == 0 || c.compare("no existent route") == 0 ){
				return "no existent route";
			}
			return b+"$" +a + c;
		}
	}
	else {
		i = N.indexNode(s1);
		int j = N.indexNode(s2);
		if(studPathArr[i][j] == -1 ){
			if(!N.chkEdg(N.student[i]->name,N.student[j]->name) && i!=j){
				return "no existent route";
			}
		return "";
		}
		else{
			int k = studPathArr[i][j];
			string a = N.student[k]->name;
			string b = route(s1,a);
			string c =  route(a,s2);
			if(b.compare("no existent route") == 0 || c.compare("no existent route") == 0 ){
				return "no existent route";
			}
			return b+"$" +a  + c;
		}
	}
}

void impCal(){
	for(int i = 0; i<N.faculty.size();i++){
		for(int j = 0; j< N.faculty.size();j++){
			Fpath(i,j);
		}
	}
	for(int i = 0; i<N.student.size();i++){
		for(int j = 0; j< N.student.size();j++){
			Spath(i,j);
		}
	}
}