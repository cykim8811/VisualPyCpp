#pragma once
#include <string>
#include <vector>

const int AND = 0;
const int OR = 1;
const int REP = 2;
const int STR = 3;
const int FTN = 4;

using namespace std;

class Format {
public:
	Format();
	Format(int _op, vector<Format> _subnode);
	Format(string _data);
	Format(int _op, int (*ftn)(string*, int));
	~Format();
	int op;
	vector<Format> subnode;
	string data;
	string print();
	int fit(string* source, int index);
	int (*ftn)(string*, int);
};