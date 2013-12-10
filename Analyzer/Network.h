/*
 * Network.h
 *
 *  Created on: Nov 11, 2013
 *      Author: cs1120250
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <string>
#include <vector>
using namespace std;

//class Node;
class Edge;

class Node{
	public:
		string name;
		vector<Edge*> edges;
		vector<Node*> neighbours;
		string type;
		int imp;
		long weight;
		Node* previous;
		string state;
		Node(string);
		Node();
		int indexs;
		bool chkNode(string);
};



class Edge{
	public:
		Node* source;
		Node* target;
		string type;
};



class Graph{
	public:
		vector<Node*> faculty;
		vector<Edge*> fEdg;
		vector<Node*> student;
		vector<Edge*> sEdg;
		Node* searchNode(string);
		bool chkEdg(string, string);
		int indexNode(string);
		~Graph();
};



#endif /* NETWORK_H_ */
