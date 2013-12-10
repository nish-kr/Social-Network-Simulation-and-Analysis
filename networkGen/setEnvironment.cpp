/*
 * setEnvironment.cpp
 *
 *  Created on: Oct 10, 2013
 *      Author: cs1120239
 */

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include "setEnvironment.h"
#include "Network.h"

using namespace std;
extern Network N;

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

//parses the input file
void parseInputFile(string inputFile){
	ifstream infile;
	infile.open(inputFile.c_str());
	if (!infile.is_open()){
		cerr<<"Error reading input file : " <<inputFile<<" .Exiting ."<<endl;
		exit(1);
	}

	University* curUniv;
	Department* curDep;
	vector<string> readLine;
	string temp;
	while(!infile.eof()){
		getline(infile , temp);
		if (temp.find_first_not_of(" \t\n\r") == string::npos){
			//first not of these chars is at end of string => string is made only of these chars
			continue;
		}

		readLine = extractTokens(temp , " \t\n\r");
		if (readLine[0].compare("UNIVERSITY") == 0){
			curUniv = new University();
			curUniv->name = readLine[1];
			N.universities.push_back(curUniv);

			//as soon as a new university is created add into its popularity field as many entries as there are global
			//interests ... all these entries should be zero
			for(int i=0;i<N.globalInterests.size();i++){
				curUniv->popularity.push_back(0.0f);
			}
		}
		else if (readLine[0].compare("DEPARTMENT") == 0){
			curUniv->departments.push_back(new Department(readLine[1] ,
						atoi(readLine[2].c_str()) , atoi(readLine[3].c_str()) , atof(readLine[4].c_str())
						, atof(readLine[5].c_str()) , curUniv));
		}
		else if (readLine[0].compare("COURSE") == 0){
			//first search for the department to which this course belongs
			for(int i = 0; i<curUniv->departments.size() ; i++){
				if (curUniv->departments[i]->name.compare(readLine[2]) == 0){
					curDep = curUniv->departments[i];
					break;
				}
			}

			Course* curCourse = new Course (readLine[1] , curDep , atof(readLine[3].c_str()));

			//add the course to the list of all courses of the university and the list
			//of all courses of the  department
			curUniv->courses.push_back(curCourse);
			curDep->allCourses.push_back(curCourse);
		}
		else if (readLine[0].compare("INTEREST") == 0){
			bool isAlreadyThere = false;
			unsigned int i=0;
			for(;i<N.globalInterests.size();i++){
				if (N.globalInterests[i]->name.compare(readLine[1]) == 0){
					//the interest has been already added
					isAlreadyThere = true;
					break;
				}
			}
			if (isAlreadyThere){
				//go to the ith index in the popularity vector of this university and that interest's popularity
				//each university has the same index for popularity as the index of interest in the globalInterest
				curUniv->popularity[i] = atof(readLine[2].c_str());
			}
			else{
				//this is a new interest :add this interest to the global list of all interests and create space for
				//interest in the popularity vector of each university with a default popularity of zero
				Interest* curInterest = new Interest(readLine[1]);
				N.globalInterests.push_back(curInterest);

				//basically create space for the current interest in all the universities's popularity list
				//and add a default value of 0.0 for that popularity
				for(unsigned int i=0;i<N.universities.size();i++){
					N.universities[i]->popularity.push_back(0.0f);
				}
				curUniv->popularity[curUniv->popularity.size()-1] = atof(readLine[2].c_str());
			}
		}
		else if (readLine[0].compare("HOSTEL") == 0){
			curUniv->hostels.push_back(new Hostel(readLine[1]));
		}
		else if (readLine[0].compare("HOUSELOCALITY") == 0){
			curUniv->houselocalities.push_back(new Houselocality(readLine[1]));
		}
		else if (readLine[0].compare("FRIENDSHIPRATE") == 0){
			curUniv->friendshipRate = atoi(readLine[1].c_str());
		}
		else if (readLine[0].compare("OPENNESS") == 0){
			curUniv->openness = atof(readLine[1].c_str());
		}
		else if (readLine[0].compare("FRIENDLINESS") == 0){
			curUniv->friendliness = atof(readLine[1].c_str());
		}
		else if (readLine[0].compare("RANDOMSEED") == 0){
			N.randomSeedFaculty = atoi(readLine[1].c_str());
			N.randomSeedStudent = atoi(readLine[2].c_str());
			N.randomSeedCourse = atoi(readLine[3].c_str());
			N.randomSeedFriend = atoi(readLine[4].c_str());
		}
	}
}



