/*
 * Dijkstra.h
 *
 *  Created on: Nov 8, 2013
 *      Author: cs1120250
 */

#ifndef DIJKSTRA_H_
#define DIJKSTRA_H_

#include "Network.h"
#include <string>
#include <vector>

bool comp(Node* x,Node* y);
vector<Node*> dijk(string s1, string s2, Graph G);


#endif
