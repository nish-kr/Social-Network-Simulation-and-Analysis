/*
 * Network.h
 *
 *  Created on: Oct 13, 2013
 *      Author: cs1120239
 */

#ifndef NETWORK_H_
#define NETWORK_H_
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <map>
using namespace std;

class Interest;
class Houselocality;
class Department;
class Course;
class Faculty;
class Student;
class University;
class Hostel;
class House;
class Room;

//Network class that contains all information
class Network{
	public :
		vector<Student*> students;
		vector<Faculty*> faculties;
		vector<University*> universities;
		vector<Interest*> globalInterests;
		int randomSeedFaculty ,randomSeedStudent , randomSeedCourse , randomSeedFriend ;
		~Network();
};

class Interest{
	public:
		string name;									//name of the interest
		vector<Student*> students;						//set of all students sharing the same set of interests
		vector<Faculty*> faculties;						//set of all faculties sharing the same set of interests
		Interest(string);
};

class Department{
	public:
		string name;
		University* university;
		vector<Faculty*> faculties;
		vector<Student*> students;
		vector<Course*> allCourses;
		vector<Course*> courseFloated;					//courses offered this sem
		float semDepCourses;
		float nonsemDepCourses;
		int numOfFaculty;
		int numOfStudspy;								//number of students per year
		Department(string name , int numf , int studs, float semdept , float nonsemdept
					,University* u);
};

class Course{
	public :
		string name;
		Department* dep;
		float freq;
		vector<Student*> students;
		Faculty* takenByFac;
		bool isFloated;
		Course(string name , Department* dep , float freq);
};

class Faculty{
	public :
		int empID; 										//the serial employeeID of the faculty
		string name;
		Department* dept;
		Houselocality* housel;
		vector<Interest*> interests;
		bool isTeaching;
		Course* coursesTaught;
		House* myHouse;
		vector<Faculty*> friends;
		University* university;
		int UID;
};


class Student{
	public :
		int entryNumber;
		Hostel* hostel;
		string name;
		Department* dep;
		vector<Interest*> interests;
		vector<Course*> coursesTaken;
		Room* myRoom;
		vector<Student*> friends;
		University* university;
		int UID;
};

class Hostel{
	public :
		vector<Room*> rooms;
		string name;
		vector<Student*> hostlers;
		Hostel(string name );										
		void allotRoom(Student*);									//allots the room to the student
};


class Houselocality{
	public :
		vector<House*> houses;
		string name;
		vector<Faculty*> residents;
		Houselocality(string);										
		void allotHouse(Faculty*);									//allots the house to the faculty
};

class University{
	public :
		string name;
		int friendshipRate;
		float openness;												//out_probability
		float friendliness;											//probability to accept request
		vector<Department*> departments;
		vector<Student*> students;
		vector<Faculty*> faculties;
		/*
		*each university will have the same set of global
		*interests .. the interests which donot have any
		*popularity among the students there will have a
		*popularity of 0
		*for accessing the set of interests see the
		*the corresponding interest int he global list of all
		*interests
		*/
		vector<float> popularity;									
		vector<Hostel*> hostels;
		vector<Course*> courses;
		vector<Course*> floatedCourses;
		vector<Houselocality*> houselocalities;
};

class House{
	public :
		int houseNo;
		Faculty* resident;
		House(int , Faculty*);
};

class Room{
	public :
		int roomNo;
		Student* resident;
		Room(int , Student*);
};

#endif /* NETWORK_H_ */
