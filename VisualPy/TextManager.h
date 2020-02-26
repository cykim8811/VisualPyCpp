#pragma once
#include "SDL.h"
#include "SDL_ttf.h"

class TextManager
{
private:
	TTF_Font* font;
	SDL_Texture* rendered[128];
public:
	SDL_Renderer* renderer;
	int width, height;
	TextManager(SDL_Renderer* renderer, TTF_Font* _font, int _width, int _height);
	~TextManager();
	void draw_text(int x, int y, const char* ch, SDL_Color color);
	void draw_char(int x, int y, const char ch, SDL_Color color);
};
