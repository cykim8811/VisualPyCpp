#pragma once
#include "Format.h"


typedef struct Node {
	string name;
	string data;
	vector<Node> subnode;
};

Node copyNode(Node target);

string printNode(Node target);
string reformat(Node target);

class S {
public:
	S(string _name, int op, vector<S> _subnode);
	S(string _name, int op, vector<S> _subnode, int param);
	S(string _name, Format _data);
	S(string _name, string _data);
	S(string _name);
	S(string _name, Node(*_ftn)(string*, int*));
	int op;
	int param;
	string name;
	Node fit(string* source, int* index);
	// STR
	Format data;
	// AND, OR, REP
	vector<S> subnode;
	Node(*ftn)(string*, int*);
};
