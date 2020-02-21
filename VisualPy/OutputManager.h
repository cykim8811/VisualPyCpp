#pragma once
#include <string>
#include "TextManager.h"
#include "SDL.h"

using namespace std;

class OutputManager {
public:
	bool clicked = false;
	bool focused = false;

	int line = 0;
	string text;
	TextManager* tm;
	OutputManager(TextManager* textmanager);
	~OutputManager();

	void onDraw(SDL_Renderer* renderer, int width, int height);
	void onUpdate(double dT);
	void onEvent(SDL_Event event);
};