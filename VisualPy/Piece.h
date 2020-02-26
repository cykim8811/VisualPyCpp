#pragma once

#include <vector>
#include <string>

#include <SDL.h>

#include "Letter.h"
#include "Parser.h"

using namespace std;

class P {
public:
	P();
	P(Node target, float*lx, float*ly, SDL_Color tcol);
	string name;
	vector<Letter> data;
	vector<P> subnode;

	void draw(TextManager* tm, int cur_length);
};
