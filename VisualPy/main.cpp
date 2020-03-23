
#include "SDL.h"
#include "vector"
#include "SDL_ttf.h"
#include "TextManager.h"
#include "CodeManager.h"
#include "RamManager.h"
#include "OutputManager.h"
#include "Parser.h"
#include <string>
#include <iostream>

using namespace std;

SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font* font;
TTF_Font* arialfont;
TextManager* arialtm;

SDL_Cursor* arrowcursor;

CodeManager* CodeWindow;
OutputManager* OutputWindow;


namespace layout {
	int title_height = 48;
	int menu_height = 24;
	int statusbar_height = 18;
	int sepbar1_w = 970;
	int sepbar_width = 5;

	int font_width = 8;
	int font_height = 16;

	int title_color_r = 224;
	int title_color_g = 224;
	int title_color_b = 224;
	int body_color_r = 216;
	int body_color_g = 216;
	int body_color_b = 216;
}

void onEvent(SDL_Event event) {
	using namespace layout;
	int window_width, window_height;
	SDL_GetWindowSize(window, &window_width, &window_height);

	int sepbar_offset = int(floor((sepbar_width - 1) / 2));
	int sepbar1_x = sepbar1_w;
	bool cursorOut = false;
	switch (event.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		if (event.button.button == SDL_BUTTON_LEFT) {
			if (event.button.y > title_height + menu_height && event.button.y < window_height - statusbar_height) {
				if (event.button.x < sepbar1_x - sepbar_offset) {
					SDL_Event e(event);
					e.button.x -= sepbar_width;
					e.button.y -= title_height + menu_height;
					OutputWindow->onEvent(e);
					OutputWindow->clicked = true;

					CodeWindow->focused = false;
					OutputWindow->focused = true;
				}
				else {
					SDL_Event e(event);
					e.button.x -= sepbar1_x + sepbar_offset;
					e.button.y -= title_height + menu_height;
					CodeWindow->onEvent(e);
					CodeWindow->clicked = true;

					CodeWindow->focused = true;
					OutputWindow->focused = false;
				}
			}
		}
		break;
	case SDL_MOUSEMOTION:
		SDL_Event e1(event);
		e1.button.x -= sepbar_width;
		e1.button.y -= title_height + menu_height;
		OutputWindow->onEvent(e1);
		SDL_Event e2(event);
		e2.button.x -= sepbar1_x + sepbar_offset;
		e2.button.y -= title_height + menu_height;
		CodeWindow->onEvent(e2);
		if (!(event.button.y > title_height + menu_height && event.button.y < window_height - statusbar_height)) {
			// Mouse moved out of all screen
			// Set cursor to arrow(default)
			SDL_SetCursor(arrowcursor);
		}
		break;
	case SDL_MOUSEBUTTONUP:
		SDL_Event e(event);
		e.button.y -= title_height + menu_height;
		if (CodeWindow->clicked) {
			e.button.x -= sepbar_width;
			CodeWindow->onEvent(e);
			CodeWindow->clicked = false;
		}
		if (OutputWindow->clicked) {
			e.button.x -= sepbar1_x + sepbar_offset - sepbar_width;
			OutputWindow->onEvent(e);
			OutputWindow->clicked = false;
		}
		break;
	case SDL_KEYDOWN:
	case SDL_TEXTINPUT:
		if (CodeWindow->focused) {
			CodeWindow->onEvent(event);
		}
		if (OutputWindow->focused) {
			OutputWindow->onEvent(event);
		}
		break;
	default:
		break;
	}
}

void onUpdate(double dT) {
	CodeWindow->onUpdate(dT);
	OutputWindow->onUpdate(dT);
}

void onDraw(SDL_Renderer* renderer) {
	using namespace layout;
	int window_width, window_height;
	SDL_GetWindowSize(window, &window_width, &window_height);

	int sepbar1_x = sepbar1_w;

	int sepbar_offset = int(floor((sepbar_width - 1) / 2));
	
	SDL_SetRenderTarget(renderer, NULL);
	// Title
	SDL_SetRenderDrawColor(renderer, title_color_r, title_color_g, title_color_b, 255);
	SDL_RenderFillRect(renderer, &SDL_Rect({ 0, 0, window_width, title_height }));

	// Menubox
	SDL_SetRenderDrawColor(renderer, body_color_r, body_color_g, body_color_b, 255);
	SDL_RenderFillRect(renderer, &SDL_Rect({ 0, title_height, window_width, menu_height }));
	const char* ch1 = "Screen";
	arialtm->draw_text(sepbar_width + 4, int(round(title_height + (menu_height / 2) - (arialtm->height / 2))) + 1, ch1, { 148, 148, 148 });
	const char* ch2 = "Code";
	arialtm->draw_text(sepbar1_x + sepbar_offset + 4, int(round(title_height + (menu_height / 2) - (arialtm->height / 2))) + 1, ch2, { 148, 148, 148 });

	/* Contents */

	int target_width, target_height;
	SDL_Rect targetRect;
	SDL_Texture* ramTexture, * codeTexture, * outputTexture;

	// RamWindow

	// OutputWindow
	target_width = sepbar1_x - sepbar_width;
	target_height = window_height - title_height - menu_height - statusbar_height;
	codeTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		target_width, target_height);
	SDL_SetRenderTarget(renderer, codeTexture);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	OutputWindow->onDraw(renderer, target_width, target_height);
	SDL_SetRenderTarget(renderer, NULL);
	targetRect = { sepbar_width, title_height + menu_height, target_width, target_height };
	SDL_RenderCopy(renderer, codeTexture, NULL, &targetRect);
	
	// CodeWindow
	target_width = int(round(window_width - sepbar1_x - sepbar_width * 1.5 + 0.5));
	target_height = window_height - title_height - menu_height - statusbar_height;
	outputTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		target_width, target_height);
	SDL_SetRenderTarget(renderer, outputTexture);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	CodeWindow->onDraw(renderer, target_width, target_height);
	SDL_SetRenderTarget(renderer, NULL);
	targetRect = { sepbar1_x + int((sepbar_width - 1) / 2), title_height + menu_height, target_width, target_height };
	SDL_RenderCopy(renderer, outputTexture, NULL, &targetRect);


	/* /Contents */

	int total_height = title_height + menu_height;
	// Shadow at the top
	SDL_SetRenderDrawColor(renderer, 8, 8, 8, 24);
	SDL_RenderDrawLine(renderer, 0, 0, window_width, 0);
	SDL_SetRenderDrawColor(renderer, 8, 8, 8, 16);
	SDL_RenderDrawLine(renderer, 0, 1, window_width, 1);

	// Shadow between title and menubox
	SDL_SetRenderDrawColor(renderer, 8, 8, 8, 16);
	SDL_RenderDrawLine(renderer, 0, title_height, window_width, title_height);
	SDL_SetRenderDrawColor(renderer, 8, 8, 8, 8);
	SDL_RenderDrawLine(renderer, 0, title_height+1, window_width, title_height+1);

	// Shadow under menubar
	SDL_SetRenderDrawColor(renderer, 8, 8, 8, 80);
	SDL_RenderDrawLine(renderer, 0, total_height, window_width, total_height);
	SDL_SetRenderDrawColor(renderer, 8, 8, 8, 56);
	SDL_RenderDrawLine(renderer, 0, total_height + 1, window_width, total_height + 1);
	SDL_SetRenderDrawColor(renderer, 8, 8, 8, 16);
	SDL_RenderDrawLine(renderer, 0, total_height + 2, window_width, total_height + 2);

	// Seperate bar
	SDL_SetRenderDrawColor(renderer, body_color_r, body_color_g, body_color_b, 255);
	SDL_RenderFillRect(renderer, &SDL_Rect({ 0, total_height, sepbar_width, window_height - total_height }));
	SDL_RenderFillRect(renderer, &SDL_Rect({ sepbar1_x - sepbar_offset, total_height, sepbar_width, window_height - total_height }));
	SDL_RenderFillRect(renderer, &SDL_Rect({ window_width - sepbar_width, total_height, sepbar_width, window_height - total_height }));

	// Shadow under Seperate bar
	SDL_SetRenderDrawColor(renderer, 8, 8, 8, 16);
	SDL_RenderDrawLine(renderer, sepbar1_x - sepbar_offset - 1, total_height, sepbar1_x - sepbar_offset - 1, window_height);
	SDL_RenderDrawLine(renderer, window_width - sepbar_width - 1, total_height, window_width - sepbar_width - 1, window_height);
	SDL_SetRenderDrawColor(renderer, 8, 8, 8, 8);
	SDL_RenderDrawLine(renderer, sepbar1_x - sepbar_offset - 2, total_height, sepbar1_x - sepbar_offset - 2, window_height);
	SDL_RenderDrawLine(renderer, window_width - sepbar_width - 2, total_height, window_width - sepbar_width - 2, window_height);

	SDL_SetRenderDrawColor(renderer, 8, 8, 8, 80);
	SDL_RenderDrawLine(renderer, sepbar1_x + sepbar_offset + 1, total_height + 1, sepbar1_x + sepbar_offset + 1, window_height);
	SDL_RenderDrawLine(renderer, 0 + sepbar_width, total_height + 1, 0 + sepbar_width, window_height);
	SDL_SetRenderDrawColor(renderer, 8, 8, 8, 56);
	SDL_RenderDrawLine(renderer, sepbar1_x + sepbar_offset + 2, total_height + 2, sepbar1_x + sepbar_offset + 2, window_height);
	SDL_RenderDrawLine(renderer, 0 + sepbar_width + 1, total_height + 2, 0 + sepbar_width + 1, window_height);
	SDL_SetRenderDrawColor(renderer, 8, 8, 8, 16);
	SDL_RenderDrawLine(renderer, sepbar1_x + sepbar_offset + 3, total_height + 2, sepbar1_x + sepbar_offset + 3, window_height);
	SDL_RenderDrawLine(renderer, 0 + sepbar_width + 2, total_height + 2, 0 + sepbar_width + 2, window_height);
	
	// Status bar under window
	SDL_SetRenderDrawColor(renderer, body_color_r, body_color_g, body_color_b, 255);
	SDL_RenderFillRect(renderer, &SDL_Rect({ 0, window_height - statusbar_height, window_width, statusbar_height }));

	SDL_DestroyTexture(codeTexture);
	SDL_DestroyTexture(outputTexture);
}


wchar_t python_home_dir[1000];

int main(int args, char* argv[]) {
	size_t size;
	if (args > 1) {
		mbstowcs_s(&size, python_home_dir, argv[1], strlen(argv[1]));
	}
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	window = SDL_CreateWindow(
		"VisualPy",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1280, 720, SDL_WINDOW_RESIZABLE
	);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE); // SDL_RENDERER_ACCELERATED|
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	font = TTF_OpenFont("Cascadia.ttf", 14);
	arialfont = TTF_OpenFont("Cascadia.ttf", 14);

	TextManager cmtm(renderer, font, 8, 16);
	CodeManager cm = CodeManager(&cmtm);
	CodeWindow = &cm;

	TextManager omtm(renderer, font, 8, 16);
	OutputManager om = OutputManager(&omtm);
	OutputWindow = &om;

	cm.om = OutputWindow;

	TextManager atm = TextManager(renderer, arialfont, 8, 16);

	arialtm = &atm;

	CodeWindow->text = "print(\"Hello, World!\")\n";
	CodeWindow->text_update();
	CodeWindow->analyze();

	arrowcursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);

	SDL_StartTextInput();

	SDL_Event event;

	Uint64 prevTime;
	Uint64 currTime;

	prevTime = SDL_GetPerformanceCounter();
	currTime = 0;

	bool running = true;
	while (running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			default:
				onEvent(event);
				break;
			}
		}
		currTime = SDL_GetPerformanceCounter();
		onUpdate((currTime - prevTime) / (float)SDL_GetPerformanceFrequency());
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		onDraw(renderer);
		SDL_RenderPresent(renderer);
		prevTime = currTime;
		SDL_Delay(10);
	}

	SDL_StopTextInput();

	SDL_FreeCursor(arrowcursor);
	TTF_CloseFont(font);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
	return 0;
}
