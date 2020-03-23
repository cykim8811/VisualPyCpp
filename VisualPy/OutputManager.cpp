#include "OutputManager.h"
#include <math.h>
#include <stdio.h>

using namespace std;

void OutputManager::send_message(char* data) {
	boost::system::error_code r_error;
	int len_r_buf = strnlen_s(data, 128 - 1);
	socket->write_some(boost::asio::buffer(data, len_r_buf), r_error);
}

void client_run(OutputManager* self) {
	cout << "Connected" << endl;
	while (true) {
		char buf[128] = { 0 };
		boost::system::error_code error;
		self->socket->read_some(boost::asio::buffer(buf), error);
		if (error) {
			if (error == boost::asio::error::eof) {
				cout << "Disconnected" << endl;
				return;
			}
			else {
				cout << "Error " << error.value() << ":" << error.message() << endl;
				return;
			}
		}
		cout << "Server:" << buf << endl;
		this_thread::sleep_for(0.01s);
	}
}

static PyObject* a(PyObject* self, PyObject* args) {
	cout << "c++ a() called" << endl;
	return PyLong_FromLong(51);
}

static struct PyMethodDef methods[] = {
	{"a", a, METH_NOARGS, "a is a"},
	{NULL, NULL, 0, NULL}
};
static struct PyModuleDef modDef = {
	PyModuleDef_HEAD_INIT, "PyClassroom", NULL, -1, methods,
	NULL, NULL, NULL, NULL
};
static PyObject* PyInit_PyClassroom(void) {
	return PyModule_Create(&modDef);
}

OutputManager::OutputManager(TextManager* textmanager) {
	tm = textmanager;
	// SetPythonHome excluded
	Py_SetPythonHome(python_home_dir);
	PyImport_AppendInittab("PyClassroom", &PyInit_PyClassroom);
	Py_Initialize();
	PyRun_SimpleString("from PyClassroom import *");

	static boost::asio::io_service io_service; // ... sorry
	endpoint = new tcp::endpoint(boost::asio::ip::address::from_string(SERVER_IP), PORT);
	socket = new tcp::socket(io_service);
	boost::system::error_code c_error;

	socket->connect(*endpoint, c_error);
	if (c_error) {
		if (c_error == boost::asio::error::eof) {
			cout << "Disconnected" << endl;
		}
		else {
			cout << "Error " << c_error.value() << ":" << c_error.message() << endl;
			getchar();
			exit(-1);
		}
	}

	static thread client_thread(client_run, this);
}

OutputManager::~OutputManager() {
	Py_Finalize();
	if (socket->is_open()) {
		socket->close();
	}
}

void OutputManager::onDraw(SDL_Renderer* renderer, int width, int height) {
	// Draw background color
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	int lastline = int(floor(height / tm->height));

	SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			SDL_Rect rect = { x * 32 + 3, y * 32 + 3, 30, 30 };
			SDL_RenderFillRect(renderer, &rect);
		}
	}
}

void OutputManager::onUpdate(double dT) {

}

void OutputManager::onEvent(SDL_Event event) {

}

void OutputManager::execute(string cmd) {
	PyRun_SimpleString(&cmd[0]);
}