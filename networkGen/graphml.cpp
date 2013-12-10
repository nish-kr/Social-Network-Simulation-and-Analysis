/*
 * graphml.cpp
 *
 *  Created on: Oct 14, 2013
 *      Author: cs1120260
 */

#include <iostream>
#include <fstream>
#include <sstream>

#include "graphml.h"
#include "Network.h"

using namespace std;

extern Network N;

int stdcount=0, faccount=0;

/*
*	converts the integer to string used in converting the entry no and empID to string
*/
string inttostring(int x){
	stringstream ss;
	ss<<x;
	return ss.str();
}

/*
*	supplies the next name of the node depending on the boolean student which spedifies
*  	whether it is being asked for a student node or faculty node.
*	It keeps track of the next number with the stdcount andfaccount
*/

string getnextnode(Student * student){
	string toreturn;
	stringstream ss;
	int id=student->entryNumber;
	string univ=student->university->name;
	ss << id;
	toreturn = ss.str();
	return (univ+"_s_"+toreturn);
}

string getnextnode(Faculty * faculty){
	string toreturn;
	stringstream ss;
	int id=faculty->empID;
	string univ=faculty->university->name;
	ss << id ;
	toreturn = ss.str();
	return (univ+"_f_"+toreturn);
}

/*
*	the funtion which is the main function in this file.
* 	When this function is called in the generator it creates a graph.graphml file
*	storing the students and faculty as nodes and friends are connected by an edge.
*/

void saveGraphML(){
	bool student=true;
	string node;
	string data;
	ofstream graphfile("graph.graphml");
	if (graphfile.is_open()){
		graphfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		graphfile << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
		graphfile << "xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">\n";
		graphfile <<"<key id=\"d0\" for=\"node\" attr.name=\"color\" attr.type=\"string\">\n";
		graphfile << "<default>yellow</default>\n";
		graphfile <<" </key>\n";
		graphfile << "<graph id=\"G\" edgedefault=\"undirected\">\n";
		/*
		*	the first iteration for listing the student nodes only
		*	the edges(friends) are generated in the next iteration
		*/
		for (int j=0;j<N.universities.size();j++){
			University * currun=N.universities[j];
			for (int i=0;i<currun->students.size();i++){		
				Student * studenta = currun->students[i];
				node =  getnextnode(studenta);
				graphfile << "<node id=\"" +node +"\">" +"\n";
				graphfile << "<data key=\"d0\">blue</data>\n";
				graphfile << "</node>\n";
			}
		}
		/*
		*	this iteraion is for the generation of the faculty only
		*	the edges are generated in the last iteration
		*/
		for (int j=0;j<N.universities.size();j++){
			University * currun=N.universities[j];
			for (int i=0;i<currun->faculties.size();i++){
				Faculty * facultya=currun->faculties[i];
				node=getnextnode(facultya);
				graphfile << "<node id=\"" +node +"\" />" +"\n";
			}
		}
		//resetted beacuse we will have to start from the first student again

		stdcount =0;
		faccount =0;

		/*
		*	this iteration is for the generation of student - student edges
		*/

		for(int i=0;i<N.students.size();i++){
			Student * studenta=N.students[i];
			node=getnextnode(studenta);				//string name of the student
			for (int j=0;j<studenta->friends.size();j++){	// to generate the edges for the student graph
				Student* friendd=studenta->friends[j];
				int hisid=friendd->entryNumber;
				string funiv=friendd->university->name;
				string shisid=inttostring(hisid);
				//<edge source="n0" target="n2"/>
				if (studenta->entryNumber<hisid){
					graphfile << "<edge source=\""+node+"\" target=\""+funiv+"_s_"+shisid+"\"/>\n";
				}
			}
			//graphfile << "<node id=\"" +node +"\">" +"\n";
			//graphfile << "<data key=\"d0\">blue</data>\n";
			//graphfile << "</node>\n";
		}

		// end of student nodes

		/*
		*	this iteration is for the generation of faculty - faculty edges
		*/

		for (int i=0;i<N.faculties.size();i++){
			Faculty * facultya =N.faculties[i];
			node=getnextnode(facultya);
			for (int j=0;j<facultya->friends.size();j++){	// to generate the edges for the facullty
				Faculty* ffriend=facultya->friends[j];
				int hisid=ffriend->empID;
				string facuniv=ffriend->university->name;
				string shisid=inttostring(hisid);
				if (facultya->empID<hisid){
					graphfile << "<edge source=\""+node+"\" target=\""+facuniv+"_f_"+shisid+"\"/>\n";
				}
			}
			//graphfile << "<node id=\"" +node +"\" />" +"\n";
		}
		
		// end of faculty nodes
		
		graphfile << "</graph>\n";
		graphfile << "</graphml>\n";
	}
}
