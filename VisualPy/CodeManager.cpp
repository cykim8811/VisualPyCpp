#include "CodeManager.h"
#include <math.h>
#include <stdio.h>
#include <iostream>

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
}

CodeManager::~CodeManager() {
	SDL_FreeCursor(cursor_arrow);
	SDL_FreeCursor(cursor_bar);
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
	for (int i = 0; i <lastline; i++) {
		// Drawing linenumber
		for (int j = 0; j < int(floor(log10(i + 1) + 1)); j++) {
			int ch = int(floor((i + 1) / pow(10, j))) % 10;
			const char c = '0' + ch;
			tm->draw_char(renderer, 30 - tm->width * (j + 1), tm->height * i + yoffset, c, {176, 176, 176});
		}

		// Draw Code
		int j = 0;
		for (; text[temp_codeindex] != '\n' && text.length() > temp_codeindex; temp_codeindex += 1) {
			tm->draw_char(renderer, 36 + tm->width * j, tm->height * i + yoffset, text[temp_codeindex], { 64, 64, 64 });
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
	if ((cursorStatus == 0) && cursorblink > cursorblinkmax * 0.5) {
		SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
		SDL_RenderDrawLine(renderer, 35 + tm->width * cursor_x, tm->height * cursor_y + yoffset, 35 + tm->width * cursor_x, tm->height * (cursor_y + 1) + yoffset - 1);
		SDL_SetRenderDrawColor(renderer, 128, 128, 128, 96);
		SDL_RenderDrawLine(renderer, 36 + tm->width * cursor_x, tm->height * cursor_y + yoffset, 36 + tm->width * cursor_x, tm->height * (cursor_y + 1) + yoffset - 1);
	}
}

void CodeManager::onUpdate(float dT) {
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
			cursorblink = cursorblinkmax;
			int cursor = get_cursor(),
				cursor_origin = get_cursor_origin();

			text.replace(min(cursor_origin, cursor),
				max(cursor_origin, cursor) - min(cursor_origin, cursor), "");
			cursor_origin = min(cursor_origin, cursor);
			cursor = cursor_origin;

			string target_text = SDL_GetClipboardText();
			text.insert(
				cursor,
				target_text
			);
			cursor += target_text.size();
			cursor_origin = cursor;
			text_update();
			set_cursor(cursor);
			set_cursor_origin(cursor_origin);
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
		break;
		// TODO: add undo and redo
		// TODO: tab key and others
		// TODO: add scroll bar system
	}
	default:
		break;
	}
}

void CodeManager::text_update() {
	linelength.clear();
	int num_line = count(text.begin(), text.end(), '\n') + 1;
	int cur_length = 0;
	for (int i = 0; i < text.length(); i++) {
		if (text[i] == '\n') {
			linelength.push_back(cur_length);
			cur_length = 0;
		}
		else {
			cur_length++;
		}
	}
	linelength.push_back(cur_length);
}
