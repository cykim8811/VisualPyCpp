#include "OutputManager.h"
#include <math.h>
#include <stdio.h>

using namespace std;

OutputManager::OutputManager(TextManager* textmanager) {
	text = "<Output Window>";
	tm = textmanager;
}

OutputManager::~OutputManager() {

}

void OutputManager::onDraw(SDL_Renderer* renderer, int width, int height) {
	// Draw background color
	SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
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
			tm->draw_char(renderer, 8 + tm->width * j, tm->height * i + yoffset, text[temp_codeindex], { 240, 240, 240 });
			j++;
		}
		temp_codeindex++;
	}
}

void OutputManager::onUpdate(double dT) {

}

void OutputManager::onEvent(SDL_Event event) {

}