#pragma once
#include <string>
#include "TextManager.h"
#include "SDL.h"
#include <iostream> 
#include <boost/asio.hpp>
#include <Python.h>
#include "SDL2_gfxPrimitives.h"
#include <mutex>
#include <future>

extern char* SERVER_IP;
extern unsigned short PORT;

using boost::asio::ip::tcp;
using namespace std;

class Arrow {
public:
	int x, y, dir;
	int uuid;
	int color = 3;
	
	float dx, dy, ddir;
};

const int w = 30, h = 20;
const int tile_size = 32;

class OutputManager {
public:
	bool clicked = false;
	bool focused = false;

	int line = 0;
	TextManager* tm;
	OutputManager(TextManager* textmanager);
	~OutputManager();

	void onDraw(SDL_Renderer* renderer, int width, int height);
	void onUpdate(double dT);
	void onEvent(SDL_Event event);

	void execute(string cmd);
	PyObject* main_module;
	PyObject* main_namespace;

	int tile[w * h];
	int myId;

	tcp::endpoint* endpoint;
	tcp::socket* socket;

	void send_message(char* data);
	char* send_message_wait(char* data);

	vector<Arrow*> arrows;
	vector<char*> returns;

	thread* client_thread;
};

extern wchar_t python_home_dir[];

extern OutputManager* global_om;

extern void event_func(void);
