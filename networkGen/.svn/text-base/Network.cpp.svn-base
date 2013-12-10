/*
 * Network.cpp
 *
 *  Created on: Oct 13, 2013
 *      Author: cs1120239
 */

#include "Network.h"
#include <random>
#include <string>
using namespace std;

extern default_random_engine rand_en_fac;
extern default_random_engine rand_en_stud;

Network::~Network(){
	//delete all students , faculties , universities , intrests and all other classes dynamically alloted
	//delete the network itself

	//delete all the university 
	int temp = this->universities.size();
	for(int i=0;i<temp;i++){
		University* curUniv = this->universities[i];

		//delete each department
		for(int j=0;j<curUniv->departments.size();j++){
			delete curUniv->departments[j];
		}

		//delete each course
		for(int j=0;j<curUniv->courses.size();j++){
			delete curUniv->courses[j];
		}

		//delete each hostel
		for(int j=0;j<curUniv->hostels.size();j++){
			//for each hostel delete the rooms
			for(int k=0;k<curUniv->hostels[j]->rooms.size();k++){
				delete curUniv->hostels[j]->rooms[k];
			}
			delete curUniv->hostels[j];
		}

		//delete each house Locality
		for(int j=0;j<curUniv->houselocalities.size();j++){
			for(int k=0;k<curUniv->houselocalities[j]->houses.size();k++){
				delete curUniv->houselocalities[j]->houses[k];
			}
			delete curUniv->houselocalities[j];
		}
		delete this->universities[i];
	}

	//delete all the students 
	temp = this->students.size();
	for(int i=0;i<temp;i++){ delete this->students[i]; }

	//delete all the faculties 
	temp = this->faculties.size();
	for(int i=0;i<temp;i++){ delete this->faculties[i]; }
	

	//delete all the globalInterests 
	temp = this->globalInterests.size();
	for(int i=0;i<temp;i++){ delete this->globalInterests[i]; }
}

Interest::Interest(string name){
	this->name = name;
}

Course::Course(string name , Department* dep , float freq){
	this->name = name;
	this->dep = dep;
	this->freq = freq;
}

Hostel::Hostel(string n){
	this->name = n;
}

Houselocality::Houselocality(string name){
	this->name = name;
}

Department::Department(string name , int numf , int studs, float semdept , float nonsemdept,
					University* u){
	this->name = name;
	this->numOfFaculty = numf;
	this->numOfStudspy = studs;
	this->semDepCourses = semdept;
	this->nonsemDepCourses = nonsemdept;
	this->university = u;
}

House::House(int x, Faculty* f){
	this->houseNo = x;
	this->resident = f;
}

Room::Room(int x ,Student* s){
	roomNo = x;
	resident = s;
}

void Hostel::allotRoom(Student* stud){
	uniform_int_distribution<int> dist(1 , 6000);
	bool foundSpace = false;
	int randno;
	while(!foundSpace){
		randno = dist(rand_en_stud);
		if (this->rooms.size() == 0) foundSpace = true;
		for(unsigned int i=0 ;i < this->rooms.size() ; i++){
			if (this->rooms[i]->roomNo != randno){
				foundSpace = true;
				break;
			}
		}
	}
	Room* r = new Room(randno , stud);
	stud->myRoom = r;
	this->rooms.push_back(r);

}

void Houselocality::allotHouse(Faculty* fac){
	uniform_int_distribution<int> dist(1 , 6000);
	bool foundSpace = false;
	int randno;
	while(!foundSpace){
		randno = dist(rand_en_fac);
		if (this->houses.size() == 0) foundSpace = true;
		for(unsigned int i=0 ;i < this->houses.size() ; i++){
			if (this->houses[i]->houseNo != randno){
				foundSpace = true;
				break ;
			}
		}
	}
	House* r = new House(randno , fac);
	fac->myHouse = r;
	this->houses.push_back(r);
}

