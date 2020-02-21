#pragma once

#include <vector>
#include <string>

#include <SDL.h>

#include "Executer.h"
#include "Statement.h"

using namespace std;

class P {
public:
	P(Node target);

	string name;
	string data;
	vector<P> subnode;

	Data process(Executer exec);
};
