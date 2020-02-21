#pragma once

#include <string>
#include <vector>

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


class Executer
{
public:
	Executer(string(*_log)(string));

	vector<Data> ram;

	int get(string name);
	int put(string name, Data data);

	string(*log)(string);

};
