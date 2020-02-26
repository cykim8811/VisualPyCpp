#pragma once

#include <string>
#include <vector>
#include "Parser.h"
#include "Letter.h"
#include "Piece.h"


using namespace std;

class Data {
public:
	int ref_count = 0;
	Data();
	~Data();

	string type;
	string name;
	
	string data_string;
	int data_int;
	float data_float;
	vector<Data> data_list;

};

class Executer {
public:
	vector<line> *source;
	int current_line = 0;
	int current_indent = 0;

	Executer(vector<line>* lines);

	vector<P> pieces;
	vector<Data> ram;

	int get(string name);
	int put(string name, Data data);

	bool reduce(Node* target);

	void draw(TextManager* tm);
};
