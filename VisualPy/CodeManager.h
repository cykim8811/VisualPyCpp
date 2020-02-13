#pragma once
#include <string>
#include "TextManager.h"
#include "SDL.h"
#include <vector>
#include "Parser.h"

using namespace std;

class CodeManager {
private:
	SDL_Cursor* cursor_arrow;
	SDL_Cursor* cursor_bar;
	
	int cursorStatus = 0;
	/*
	-1: something else (not shown, cursor arrow)
	0 : blink
	*/
	float cursorblink = 1.0;
	float cursorblinkmax = 1.1;
	
	vector<int> linelength;

	int pcursorx = 3;

	int doubleclick = -1;

public:
	int cursor_x = 3;
	int cursor_y = 0;
	int cursor_origin_x = 3;
	int cursor_origin_y = 0;
	bool clicked = false;
	
	int index = 0;

	int line = 0;
	string text;
	TextManager* tm;
	CodeManager(TextManager* textmanager);
	~CodeManager();

	void onDraw(SDL_Renderer* renderer, int width, int height);
	void onUpdate(float dT);
	void onEvent(SDL_Event event);
	void text_update();
	int get_cursor();
	int get_cursor_origin();
	void set_cursor(int index);
	void set_cursor_origin(int index);

	Node parse;
	int lastparse;

	void draw_code(SDL_Renderer* renderer);
	void draw_node(SDL_Renderer* renderer, Node target, int* lx, int* ly, SDL_Color col = {96, 128, 128});

	void analyze();

};