/*
 * parser.cpp
 *
 *  Created on: Nov 11, 2013
 *      Author: cs1120239
 */

#include "parser.h"
#include "Network.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <ctype.h>
using namespace std;

extern Graph N;


//extracts the tokens of the string s splitted by delim
vector<string> extractTokens(string s , string delim){
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

//tells is the given string is a faculty or a student : false for faculty and true for student
bool isStud(string s){
	size_t x = s.find_first_of("_");
	if (x == string::npos) {cerr<<"Error Wrong argument to isStud . Exiting .";exit(1);}
	char c = s[x+1];
	return (c=='s');
}


int giveID(string s){
	size_t x = s.find_last_of("_");
	string s1 = s.substr(x+1);
	return atoi(s1.c_str());
}

//changes case of the string to be in lower
string changeCase(string s){
	char arr[s.size()];
	for(int i=0;i<s.size();i++){
		if (isupper(s[i])){
			arr[i] = tolower(s[i]);
		}
		else{
			arr[i] = s[i];
		}
	}
	return string((const char*)arr);
}

string giveUniv(string s){
	size_t found = s.find_first_of("_");
	return s.substr(0,found);
}


void parseGraphML(string inFile){
	ifstream in;
	in.open(inFile.c_str());
	if (!in.is_open()){
		cerr<<"Error reading graphML file .Exiting ."<<endl;
		exit(1);
	}
	vector<string> readLine;
	string temp;
	vector<string> univName;
	vector<int> studLine;
	vector<int> facLine;
	vector<vector <string> > allLines;
	//first scan for all nodes
	while(!in.eof()){
		getline(in , temp);
		if (temp.find_first_not_of(" \t\n\r") == string::npos){
			//first not of these chars is at end of string => string is made only of these chars
			continue;
		}
		readLine = extractTokens(temp , " \t\n\r/<>\"=");
		if ((readLine[0].compare("node") == 0) && (readLine[1].compare("id") == 0)){
			//is a line containing node id
			string up = changeCase(readLine[2]);
			Node* n = new Node(up);
			bool isStudent = isStud(readLine[2]);
			string univ = giveUniv(readLine[2]);
			bool isUnivAlr = false;
			int idx = 0;
			for(int i=0;i<univName.size();i++){
				if (univName[i].compare(univ) == 0){
					//the university name matches .. 
					isUnivAlr = true;
					idx = i;
					break;
				}
			}

			if (!isUnivAlr){
				//university name is a new .. so add correspondingly
				univName.push_back(univ);
				if (isStudent){
					//the node is a student node
					studLine.push_back(N.student.size());
					facLine.push_back(-1);
					n->indexs = N.student.size();
					N.student.push_back(n);					
				}
				else{
					//the node is a faculty node
					facLine.push_back(N.faculty.size());
					studLine.push_back(-1);
					n->indexs = N.faculty.size();
					N.faculty.push_back(n);
				}
			}
			else{
				//university name already there
				if (isStudent){
					if (studLine[idx] == -1){
						//means that this is the first student node of that university
						studLine[idx] = (N.student.size());
						//facline must already be filled
						n->indexs = N.student.size();
						N.student.push_back(n);
					}
					else{
						//means this is not the first student node of that univ
						n->indexs = N.student.size();
						N.student.push_back(n);
					}
				}
				else{
					if (facLine[idx] == -1){
						//means that this is the first faculty node of that university
						facLine[idx] = (N.faculty.size());
						n->indexs = N.faculty.size();					
						N.faculty.push_back(n);
					}
					else{
						n->indexs = N.faculty.size();
						N.faculty.push_back(n);
					}
				}
			}
		}
		else{
			allLines.push_back(readLine);
		}
	}
	for(int i=0;i<allLines.size();i++){
		readLine = allLines[i];
		if ((readLine[0].compare("edge") == 0) && (readLine[1].compare("source") == 0)){
			//is a line containing edge info
			//assume all nodes have been created
			//also assume all nodes were added in the vector in serial order
			Edge* e = new Edge();
			string univ1 = giveUniv(readLine[2]);
			string univ2 = giveUniv(readLine[4]);
			int idx1 = 0;
			int idx2 = 0;
			for(int i=0;i<univName.size();i++){
				if (univName[i].compare(univ1) == 0){
					idx1 = i;
				}
				if (univName[i].compare(univ2) == 0){
					idx2 = i;
				}
			}

			if (isStud(readLine[2])){
				//source is a student and hence target is also a student
				e->type = "student";
				int id1 = giveID(readLine[2]);
				int id2 = giveID(readLine[4]);

				Node* n1 = N.student[id1-1+studLine[idx1]];
				Node* n2 = N.student[id2-1+studLine[idx2]];
				n1->edges.push_back(e);
				n1->neighbours.push_back(n2);
				n2->edges.push_back(e);
				n2->neighbours.push_back(n1);
				e->source = n1;
				e->target = n2;
				N.sEdg.push_back(e);
			}
			else{
				//source is a faculty
				e->type = "faculty";
				int id1 = giveID(readLine[2]);
				int id2 = giveID(readLine[4]);
				Node* n1 = N.faculty[id1-1+facLine[idx1]];
				Node* n2 = N.faculty[id2-1+facLine[idx2]];
				n1->edges.push_back(e);
				n1->neighbours.push_back(n2);
				n2->edges.push_back(e);
				n2->neighbours.push_back(n1);
				e->source = n1;
				e->target = n2;
				N.fEdg.push_back(e);
			}
		}
	}
}
