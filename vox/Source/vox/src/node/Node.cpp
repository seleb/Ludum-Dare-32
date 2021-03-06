#pragma once

#include <node/Node.h>

#ifdef _DEBUG
std::vector<Node *>Node::nodes;
#endif

Node::~Node(){
#ifdef _DEBUG
	for(signed long int j = nodes.size()-1; j >= 0; --j){
		if(nodes.at(j) == this){
			nodes.erase(nodes.begin() + j);
		}
	}
#endif
}
Node::Node(){
#ifdef _DEBUG
	nodes.push_back(this);
#endif
}