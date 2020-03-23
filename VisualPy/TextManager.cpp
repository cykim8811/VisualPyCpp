#include "TextManager.h"
#include <stdio.h>

TextManager::TextManager(SDL_Renderer* _renderer, TTF_Font* _font, int _width, int _height) {
	font = _font;
	width = _width;
	height = _height;
	renderer = _renderer;
	for (int i = 32; i < 128; i++) {
		SDL_Surface* tsurf = TTF_RenderText_Blended(font, (const char*)&i, { 255, 255, 255 });
		rendered[i] = SDL_CreateTextureFromSurface(renderer, tsurf);
		SDL_FreeSurface(tsurf);
	}
}

TextManager::~TextManager() {
	for (int i = 32; i < 128; i++) {
		SDL_DestroyTexture(rendered[i]);
	}
}

void TextManager::draw_text(int x, int y, const char* str, SDL_Color color) {
	for (int i = 0; str[i] != '\0'; i++) {
		int texW, texH;
		SDL_QueryTexture(rendered[(int)str[i]], NULL, NULL, &texW, &texH);
		SDL_Rect dstrect = { x + int(floor(width / 2 - texW / 2)), y + height - texH, texW, texH };
		x += width;
		SDL_SetTextureColorMod(rendered[(int)str[i]], color.r, color.g, color.b);
		SDL_RenderCopy(renderer, rendered[(int)str[i]], NULL, &dstrect);
	}
}

void TextManager::draw_char(int x, int y, const char str, SDL_Color color) {
	int texW, texH;
	SDL_QueryTexture(rendered[(int)str], NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { x + int(floor(width / 2 - texW / 2)), y + height - texH, texW, texH };
	SDL_SetTextureColorMod(rendered[(int)str], color.r, color.g, color.b);
	SDL_RenderCopy(renderer, rendered[(int)str], NULL, &dstrect);
}
