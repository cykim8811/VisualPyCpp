#pragma once
#include <string>
#include "TextManager.h"
#include "SDL.h"
#include <iostream> 
#include <boost/asio.hpp>
#include <Python.h>

const char SERVER_IP[] = "127.0.0.1";
const unsigned short PORT = 8990;

using boost::asio::ip::tcp;
using namespace std;

class Arrow {
public:
	int x, y, dir;
	int uuid;
};

const int w = 30, h = 20;

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

	bool tile[w * h] = { 1 };
	int myId;
	thread* client_thread;
	/*
	boost::asio::io_service io_service;
	tcp::endpoint* endpoint;
	tcp::socket* socket;
	void send_message(char* data);
	char* send_message_wait(char* data);
*/
};

extern wchar_t python_home_dir[];