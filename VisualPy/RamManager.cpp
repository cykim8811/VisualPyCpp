
#include "RamManager.h"
#include <math.h>
#include <stdio.h>

using namespace std;

RamManager::RamManager(TextManager* textmanager) {
	text = "<Ram Window>";
	tm = textmanager;
}

RamManager::~RamManager() {

}

void RamManager::onDraw(SDL_Renderer* renderer, int width, int height) {
	// Draw background color
	SDL_SetRenderDrawColor(renderer, 240, 240, 248, 255);
	SDL_RenderClear(renderer);

	int lastline = int(floor(height / tm->height));

	if (count(text.begin(), text.end(), '\n') + 1 - line < lastline) {
		lastline = count(text.begin(), text.end(), '\n') + 1 - line;
	}

	int yoffset = 2;

	int temp_codeindex = 0;

	// Drawing Screen
	for (int i = 0; i < lastline; i++) {
		// Draw Code
		int j = 0;
		for (; text[temp_codeindex] != '\n' && text.length() > temp_codeindex; temp_codeindex += 1) {
			tm->draw_char(8 + tm->width * j, tm->height * i + yoffset, text[temp_codeindex], { 192, 192, 192 });
			j++;
		}
		temp_codeindex++;
	}
}

void RamManager::onUpdate(double dT) {

}

void RamManager::onEvent(SDL_Event event) {

}