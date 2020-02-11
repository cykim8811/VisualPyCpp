#pragma once

#include <string>
#include "TextManager.h"
#include "SDL.h"

using namespace std;

class RamManager {
public:
	bool clicked = false;

	int line = 0;
	string text;
	TextManager* tm;
	RamManager(TextManager* textmanager);
	~RamManager();

	void onDraw(SDL_Renderer* renderer, int width, int height);
	void onUpdate(double dT);
	void onEvent(SDL_Event event);
};