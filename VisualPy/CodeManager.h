#pragma once
#include <string>
#include "TextManager.h"
#include "SDL.h"
#include <vector>
#include "Parser.h"
#include "Executer.h"

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
	bool focused = true;
	bool clicked = false;
	
	int index = 0;

	bool isRunning = false;

	int cur_line = 0;
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
	vector<line> lines;
	Executer* exec;

	int lastparse;

	void draw_code();
	void draw_node(Node target, int* lx, int* ly, SDL_Color col = { 96, 128, 128 });
	void draw_piece(Node target, int* lx, int* ly, SDL_Color col = { 96, 128, 128 });

	void analyze();
	void run_code(SDL_Renderer* renderer, TextManager* tm);

};