#pragma once

#include <vector>
#include <string>

#include <SDL.h>

#include "Executer.h"
#include "Statement.h"
#include "Letter.h"

using namespace std;

class P {
public:
	P();
	P(Node target, int *lx, int *ly, SDL_Color tcol);
	string name;
	vector<Letter> data;
	vector<P> subnode;

	void draw(TextManager* tm);
};
