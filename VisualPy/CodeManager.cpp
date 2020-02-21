#include "CodeManager.h"
#include <math.h>
#include <stdio.h>
#include <iostream>

#include "Piece.h"

using namespace std;

int min(int a, int b) {
	if (a < b) {
		return a;
	}
	else {
		return b;
	}
}
int max(int a, int b) {
	if (a > b) {
		return a;
	}
	else {
		return b;
	}
}

void CodeManager::analyze() {
	lastparse = 0;
	parse = P(parse_source(&text, &lastparse)); // Stop Point
}

int CodeManager::get_cursor() {
	int total = 0;
	for (int i = 0; i < cursor_y; i++) {
		total += linelength[i] + 1;
	}
	return total + cursor_x;
}

int CodeManager::get_cursor_origin() {
	int total = 0;
	for (int i = 0; i < cursor_origin_y; i++) {
		total += linelength[i] + 1;
	}
	return total + cursor_origin_x;
}

void CodeManager::set_cursor(int index) {
	index = max(min(index, text.size()), 0);
	int tind = 0;
	for (int i = 0; i < linelength.size(); i++) {
		if (index < tind + linelength[i] + 1) {
			cursor_y = i;
			cursor_x = index - tind;
			return;
		}
		tind += linelength[i] + 1;
	}
}

void CodeManager::set_cursor_origin(int index) {
	index = max(min(index, text.size()), 0);
	int tind = 0;
	for (int i = 0; i < linelength.size(); i++) {
		if (index < tind + linelength[i] + 1) {
			cursor_origin_y = i;
			cursor_origin_x = index - tind;
			return;
		}
		tind += linelength[i] + 1;
	}
}

CodeManager::CodeManager(TextManager* textmanager) {
	text = "p = abc.d[e]+f[g](h)\nprint(p)\n";
	tm = textmanager;
	cursor_arrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	cursor_bar = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	text_update();
	index = 0;
	parse = parse_source(&text, &index);
}

CodeManager::~CodeManager() {
	SDL_FreeCursor(cursor_arrow);
	SDL_FreeCursor(cursor_bar);
}

typedef struct Coloring {
	string name;
	vector<string> key;
	vector<SDL_Color> col;
};


vector<Coloring> coloring_list{
	{"variable",
	{"address",     "int",          "float",        "string",       "\""            },
	{{84, 104, 92}, {80, 112, 180}, {80, 112, 180}, {192, 128, 96}, {184, 112, 48} }},
	
	{"attribute", {".", "name"}, {{128, 128, 128}, {72,112,104}, }},

	{"import", {"import", "as", }, {{24, 48, 160}, {24, 48, 160}, }},
	{"from", {"from", }, {{24, 48, 160}, }},

	{"s_def", {"def", ",", }, {{24, 48, 160}, {150, 112, 96}, }},
	{"s_if", {"if",}, {{24, 48, 160},}},
	{"s_for", {"for", "in",}, {{24, 48, 160}, {24, 48, 160}, }},
	{"s_while", {"while", }, {{24, 48, 160}, {24, 48, 160}, }},
	{"return", {"return",}, {{24, 48, 160},}},
	{"yield", {"yield",}, {{24, 48, 160},}},
	{"lines", {"pass", "continue", "break", }, {{24, 48, 160}, {24, 48, 160}, {24, 48, 160}, }},
	{"s_class", {"class", ",", }, {{24, 48, 160}, {130, 112, 96}, } },
	{"s_else", {"else",}, {{24, 48, 160},}},
	{"s_elif", {"elif",}, {{24, 48, 160},}},

	{"list", {",", }, {{130, 112, 96}, } },
	{"tuple", {",", }, {{130, 112, 96}, } },
	{"indexing", {",", }, {{130, 112, 96}, } },
	{"calling", {",", }, {{130, 112, 96}, } },

	{"calc_is", {"is", }, {{72, 102, 140}, }},
	{"calc_in", {"in", }, {{72, 102, 140}, }},
	{"calc_nin", {"not in", }, {{72, 102, 140}, }},
	{"calc_nis", {"is not", }, {{72, 102, 140}, }},

	{"calc_not", {"not", }, {{72, 102, 140}, }},
	{"calc_or", {"or", }, {{72, 102, 140}, }},
	{"calc_and", {"and", }, {{72, 102, 140}, }},

	{"calc_unpack", {"*", }, {{72, 102, 140}, }},

	{"comment", {"#", "text", }, {{32, 128, 48}, {32, 128, 48}, } },
};

void CodeManager::draw_node(SDL_Renderer* renderer, Node target, int* lx, int* ly, SDL_Color col) {
	if (target.subnode.size() == 0) {
		for (int i = 0; i < target.data.size(); i++) {
			if (*ly < line) continue;
			if (target.data[i] == '\n') {
				*lx = 0;
				*ly += 1;
				continue;
			}
			tm->draw_char(renderer, 36 + tm->width * (*lx), tm->height * (*ly - line) + 2, target.data[i], col);
			*lx += 1;
		}
	}
	else {
		for (int i = 0; i < target.subnode.size(); i++) {
			SDL_Color tcol = { 96, 96, 96 };
			for (int j = 0; j < coloring_list.size(); j++) {
				if (target.name == coloring_list[j].name) {
					for (int k = 0; k < coloring_list[j].key.size(); k++) {
						if (coloring_list[j].key[k] == target.subnode[i].name) {
							tcol = coloring_list[j].col[k];
							break;
						}
					}
					break;
				}
			}
			draw_node(renderer, target.subnode[i], lx, ly, tcol);
		}
	}
}

void CodeManager::draw_piece(SDL_Renderer* renderer, Node target, int* lx, int* ly, SDL_Color col) {
	if (target.subnode.size() == 0) {
		for (int i = 0; i < target.data.size(); i++) {
			if (*ly < line) continue;
			if (target.data[i] == '\n') {
				*lx = 0;
				*ly += 1;
				continue;
			}
			tm->draw_char(renderer, 36 + tm->width * (*lx), tm->height * (*ly - line) + 2, target.data[i], col);
			*lx += 1;
		}
	}
	else {
		for (int i = 0; i < target.subnode.size(); i++) {
			SDL_Color tcol = { 96, 96, 96 };
			for (int j = 0; j < coloring_list.size(); j++) {
				if (target.name == coloring_list[j].name) {
					for (int k = 0; k < coloring_list[j].key.size(); k++) {
						if (coloring_list[j].key[k] == target.subnode[i].name) {
							tcol = coloring_list[j].col[k];
							break;
						}
					}
					break;
				}
			}
			draw_piece(renderer, target.subnode[i], lx, ly, tcol);
		}
	}
}


void CodeManager::draw_code(SDL_Renderer* renderer) {
	int lx = 0, ly = 0;
	draw_piece(renderer, parse, &lx, &ly);
}


void CodeManager::onDraw(SDL_Renderer* renderer, int width, int height) {
	// Draw Background color
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	// Draw bgcolor of line number
	SDL_SetRenderDrawColor(renderer, 251, 251, 250, 255);
	SDL_Rect rect{0, 0, 32, height};
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
	SDL_RenderDrawLine(renderer, 32, 0, 32, height);

	int lastline = int(floor(height / tm->height));

	if (count(text.begin(), text.end(), '\n') + 1 - line < lastline) {
		lastline = count(text.begin(), text.end(), '\n') + 1 - line;
	}

	int yoffset = 2;
	
	int temp_codeindex = 0;

	int cursor = get_cursor();
	int cursor_origin = get_cursor_origin();
	// Drawing Screen

	// Draw Code
	draw_code(renderer);

	for (int i = 0; i <lastline; i++) {
		// Drawing linenumber
		for (int j = 0; j < int(floor(log10(i + 1) + 1)); j++) {
			int ch = int(floor((i + 1) / pow(10, j))) % 10;
			const char c = '0' + ch;
			tm->draw_char(renderer, 30 - tm->width * (j + 1), tm->height * i + yoffset, c, {176, 176, 176});
		}


		int j = 0;
		for (; text[temp_codeindex] != '\n' && text.length() > temp_codeindex; temp_codeindex += 1) {
			if (temp_codeindex >= lastparse) {
				tm->draw_char(renderer, 36 + tm->width * j, tm->height * i + yoffset, text[temp_codeindex], { 160, 160, 160 });
			}

			// Background color when selected
			if ((temp_codeindex >= cursor_origin && temp_codeindex < cursor) ||
				(temp_codeindex >= cursor && temp_codeindex < cursor_origin)) {
				SDL_SetRenderDrawColor(renderer, 96, 128, 192, 40);
				SDL_Rect rect{ 36 + tm->width * j, tm->height * i + yoffset, tm->width, tm->height };
				SDL_RenderFillRect(renderer, &rect);
			}

			j++;
		}
		temp_codeindex++;
	}
	// Draw Cursor
	if (focused && (cursorStatus == 0) && cursorblink > cursorblinkmax * 0.5) {
		SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
		SDL_RenderDrawLine(renderer, 35 + tm->width * cursor_x, tm->height * cursor_y + yoffset, 35 + tm->width * cursor_x, tm->height * (cursor_y + 1) + yoffset - 1);
		SDL_SetRenderDrawColor(renderer, 128, 128, 128, 96);
		SDL_RenderDrawLine(renderer, 36 + tm->width * cursor_x, tm->height * cursor_y + yoffset, 36 + tm->width * cursor_x, tm->height * (cursor_y + 1) + yoffset - 1);
	}
}

void CodeManager::onUpdate(float dT) {
	// analyze();
	if (doubleclick >= 0) { doubleclick--; }
	if (cursorblink < 0.0) {
		cursorblink = cursorblinkmax;
	}
	cursorblink -= dT;
}

void CodeManager::onEvent(SDL_Event event) {
	switch (event.type)
	{
	case SDL_MOUSEBUTTONDOWN: {
		if (event.button.button == SDL_BUTTON_LEFT) {
			text_update();
			cursorblink = cursorblinkmax;
			int px = int(floor((event.button.x - 32) / tm->width)) + line;
			int py = int(floor((event.button.y - 4) / tm->height));
			if (px < 0 || py < 0) {
				break;
			}
			int n_line = linelength.size();
			// Clicking after last line
			if (py > n_line - 1) {
				py = n_line - 1;
			}
			if (px > linelength[py]) {
				px = linelength[py];
			}
			cursor_x = px;
			cursor_y = py;

			if (!(SDL_GetModState() & KMOD_SHIFT)) {
				cursor_origin_x = cursor_x;
				cursor_origin_y = cursor_y;
			}
			pcursorx = px;
		}
		break;
	}
	case SDL_MOUSEBUTTONUP:
		break;
	case SDL_MOUSEMOTION:
		if (clicked) {
			SDL_SetCursor(cursor_bar);
			//text_update();
			cursorblink = cursorblinkmax;
			int px = max(0, int(floor((event.button.x - 32) / tm->width)) + line);
			int py = max(0, int(floor((event.button.y - 4) / tm->height)));
			int n_line = linelength.size();
			// Clicking after last line
			if (py > n_line - 1) {
				py = n_line - 1;
			}
			if (px > linelength[py]) {
				px = linelength[py];
			}
			cursor_x = px;
			cursor_y = py;

		}
		else {
			if (event.button.x > 32 && (cursorStatus == 0)) {
				SDL_SetCursor(cursor_bar);
			}
			else {
				SDL_SetCursor(cursor_arrow);
			}
		}
		break;
	case SDL_TEXTINPUT: {
		cursorblink = cursorblinkmax;
		int cursor = get_cursor(),
			cursor_origin = get_cursor_origin();
		text.replace(min(cursor_origin, cursor),
			max(cursor_origin, cursor) - min(cursor_origin, cursor),
			(char*)&event.text.text);
		cursor_origin = min(cursor_origin, cursor) + 1;
		cursor = cursor_origin;
		text_update();
		set_cursor(cursor);
		pcursorx = cursor_x;
		set_cursor_origin(cursor_origin);
		break;
	}
	case SDL_KEYDOWN: {
		if (event.key.keysym.sym == SDLK_BACKSPACE) {
			cursorblink = cursorblinkmax;
			int cursor = get_cursor(),
				cursor_origin = get_cursor_origin();

			if (cursor_origin == cursor && cursor != 0) {
				text.replace(cursor - 1, 1, "");
				cursor -= 1;
				cursor_origin -= 1;
			}
			else {
				text.replace(min(cursor_origin, cursor),
					max(cursor_origin, cursor) - min(cursor_origin, cursor),
					"");
				cursor_origin = min(cursor_origin, cursor);
				cursor = cursor_origin;
			}
			text_update();
			set_cursor(cursor);
			pcursorx = cursor_x;
			set_cursor_origin(cursor_origin);
		}
		else if (event.key.keysym.sym == SDLK_DELETE) {
			cursorblink = cursorblinkmax;
			int cursor = get_cursor(),
				cursor_origin = get_cursor_origin();

			if (cursor_origin == cursor && cursor < text.length()) {
				text.replace(cursor, 1, "");
			}
			else {
				text.replace(min(cursor_origin, cursor),
					max(cursor_origin, cursor) - min(cursor_origin, cursor),
					"");
				cursor_origin = min(cursor_origin, cursor);
				cursor = cursor_origin;
			}
			text_update();
			set_cursor(cursor);
			pcursorx = cursor_x;
			set_cursor_origin(cursor_origin);
		}
		else if (event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
		{
			cursorblink = cursorblinkmax;
			int cursor = get_cursor(),
				cursor_origin = get_cursor_origin();
			if (cursor_origin != cursor) {
				string sbstr = text.substr(
					min(cursor, cursor_origin),
					max(cursor, cursor_origin) - min(cursor, cursor_origin));
				SDL_SetClipboardText(sbstr.c_str());
			}
		}
		else if (event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
		{
			string target_text = SDL_GetClipboardText();
			if (target_text != "") {
				cursorblink = cursorblinkmax;
				int cursor = get_cursor(),
					cursor_origin = get_cursor_origin();

				text.replace(min(cursor_origin, cursor),
					max(cursor_origin, cursor) - min(cursor_origin, cursor), "");
				cursor_origin = min(cursor_origin, cursor);
				cursor = cursor_origin;

				text.insert(
					cursor,
					target_text
				);
				text_update();
				cursor += target_text.size() - count(target_text.begin(), target_text.end(), '\r');
				cursor_origin = cursor;
				set_cursor(cursor);
				set_cursor_origin(cursor_origin);
				pcursorx = cursor_x;
			}
		}
		else if (event.key.keysym.sym == SDLK_x && SDL_GetModState() & KMOD_CTRL)
		{
			cursorblink = cursorblinkmax;
			int cursor = get_cursor(),
				cursor_origin = get_cursor_origin();

			if (cursor_origin != cursor) {
				string sbstr = text.substr(
					min(cursor, cursor_origin),
					max(cursor, cursor_origin) - min(cursor, cursor_origin));
				SDL_SetClipboardText(sbstr.c_str());
			}
			text.replace(min(cursor_origin, cursor),
				max(cursor_origin, cursor) - min(cursor_origin, cursor), "");
			cursor_origin = min(cursor_origin, cursor);
			cursor = cursor_origin;
			text_update();
			set_cursor(cursor);
			pcursorx = cursor_x;
			set_cursor_origin(cursor_origin);
		}
		else if (event.key.keysym.sym == SDLK_a && SDL_GetModState() & KMOD_CTRL)
		{
			cursorblink = cursorblinkmax;
			int cursor = get_cursor(),
				cursor_origin = get_cursor_origin();
			cursor_origin = 0;
			cursor = text.length();
			set_cursor(cursor);
			set_cursor_origin(cursor_origin);
		}
		else if (event.key.keysym.sym == SDLK_RETURN) {
			cursorblink = cursorblinkmax;
			int cursor = get_cursor(),
				cursor_origin = get_cursor_origin();
			text.replace(min(cursor_origin, cursor),
				max(cursor_origin, cursor) - min(cursor_origin, cursor), "");
			text.insert(min(cursor_origin, cursor), "\n");
			cursor_origin = min(cursor_origin, cursor) + 1;
			cursor = cursor_origin;
			text_update();
			set_cursor(cursor);
			set_cursor_origin(cursor_origin);
		}
		else if (event.key.keysym.sym == SDLK_LEFT) {
			int cursor = get_cursor();
				cursorblink = cursorblinkmax;
			cursor = max(0, cursor - 1);
			set_cursor(cursor);
			pcursorx = cursor_x;
			if (!(SDL_GetModState() & KMOD_SHIFT)) {
				cursor_origin_x = cursor_x;
				cursor_origin_y = cursor_y;
			}
		}
		else if (event.key.keysym.sym == SDLK_RIGHT) {
			int cursor = get_cursor();
			cursorblink = cursorblinkmax;
			cursor = min(text.size(), cursor + 1);
			set_cursor(cursor);
			pcursorx = cursor_x;
			if (!(SDL_GetModState() & KMOD_SHIFT)) {
				cursor_origin_x = cursor_x;
				cursor_origin_y = cursor_y;
			}
		}
		else if (event.key.keysym.sym == SDLK_UP) {
			cursorblink = cursorblinkmax;
			cursor_y = max(0, cursor_y - 1);
			cursor_x = min(pcursorx, linelength[cursor_y]);
			if (!(SDL_GetModState() & KMOD_SHIFT)) {
				cursor_origin_x = cursor_x;
				cursor_origin_y = cursor_y;
			}
		}
		else if (event.key.keysym.sym == SDLK_DOWN) {
			cursorblink = cursorblinkmax;
			cursor_y = min(linelength.size() - 1, cursor_y + 1);
			cursor_x = min(pcursorx, linelength[cursor_y]);
			if (!(SDL_GetModState() & KMOD_SHIFT)) {
				cursor_origin_x = cursor_x;
				cursor_origin_y = cursor_y;
			}
		}
		else if (event.key.keysym.sym == SDLK_HOME) {
			cursorblink = cursorblinkmax;
			cursor_x = 0;
			pcursorx = cursor_x;
			if (!(SDL_GetModState() & KMOD_SHIFT)) {
				cursor_origin_x = 0;
			}
		}
		else if (event.key.keysym.sym == SDLK_END) {
			cursorblink = cursorblinkmax;
			cursor_x = linelength[cursor_y];
			pcursorx = cursor_x;
			if (!(SDL_GetModState() & KMOD_SHIFT)) {
				cursor_origin_x = cursor_x;
			}
		}
		else if (event.key.keysym.sym == SDLK_TAB) {
			cursorblink = cursorblinkmax;
			int cursor = get_cursor(),
				cursor_origin = get_cursor_origin();
			if (cursor_x == cursor_origin_x && cursor_y == cursor_origin_y) {
				text.insert(cursor, indent_text);
				cursor += indent_text.size();
				cursor_origin = cursor;
			}
			else {
				for (int i = min(cursor_y, cursor_origin_y); i < max(cursor_y, cursor_origin_y) + 1; i++) {
					//TODO: indent selected lines
				}
			}
			text_update();
			set_cursor(cursor);
			set_cursor_origin(cursor_origin);
		}
		break;
		// TODO: add undo and redo
		// TODO: tab key and others
		// TODO: add scroll bar system
	}
	case SDL_MOUSEWHEEL:

		break;
	default:
		break;
	}
}

void CodeManager::text_update() {
	linelength.clear();
	int num_line = count(text.begin(), text.end(), '\n') + 1;
	int cur_length = 0;
	for (int i = 0; i < text.length(); i++) {
		if (text[i] == '\r') {
			text.erase(text.begin() + i);
			i--;
			continue;
		}
		if (text[i] == '\n') {
			linelength.push_back(cur_length);
			cur_length = 0;
		}
		else {
			cur_length++;
		}
	}
	linelength.push_back(cur_length);
	analyze();
}
