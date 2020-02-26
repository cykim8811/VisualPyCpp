#pragma once

#include <vector>
#include <string>
#include "SDL.h"
#include "TextManager.h"

class Letter
{
public:

	Letter();
	Letter(float _cx, float _cy, char c, SDL_Color _col);


	float cx, cy;
	float tx, ty;
	char c;
	SDL_Renderer* renderer;
	SDL_Color col;

	void draw(TextManager* tm, int cur_line);
};
