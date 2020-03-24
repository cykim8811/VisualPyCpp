#include "OutputManager.h"
#include <math.h>
#include <stdio.h>

using namespace std;

OutputManager* global_om;


int color_list[14][3] = {
{0x00, 0x00, 0x00},
{0xff, 0xff, 0xff},
{0x4a, 0xa8, 0xd8},
{0x8b, 0x30, 0xe6},
{0x00, 0x80, 0x00},
{0xd6, 0x59, 0x8b},
{0x5c, 0x5e, 0xd3},
{0x7f, 0x80, 0xd7},
{0x2e, 0x44, 0x9e},
{0x3e, 0xac, 0x40},
{0x2f, 0x0d, 0xda},
{0xbe, 0x62, 0xf4},
{0xdc, 0xdc, 0x5f},
{0xe2, 0xa6, 0x34}
};

void OutputManager::send_message(char* data) {
	char dt[128];
	strcpy(dt, data);
	strcat(dt, "!");
	boost::system::error_code r_error;
	int len_r_buf = strnlen_s(dt, 128 - 1);
	socket->write_some(boost::asio::buffer(dt, len_r_buf), r_error);
}

char* OutputManager::send_message_wait(char* data) {
	boost::system::error_code r_error;
	char dt[128];
	strcpy(dt, data);
	strcat(dt, "!");
	int len_r_buf = strnlen_s(dt, 128 - 1);
	returns.clear();

	socket->write_some(boost::asio::buffer(dt, len_r_buf), r_error);

	for (int i = 0; returns.size() == 0; i++) {
		Sleep(1);
		if (i > 100) {
			cout << "[Warning] return value timeout" << endl;
			return dt;
		}
	}

	char* ret = returns.back();

	return ret;
}

void handle_listen(OutputManager* self, char* data) {
	cout << "Server:" << data << endl;
	switch (data[0] - '0')
	{
	case 0: { // Join
		Arrow* arr = new Arrow();
		self->arrows.push_back(arr);
		arr->uuid = data[1] - '0';
		break;
	}
	case 1:
		for (int i = 0; i < self->arrows.size(); i++) {
			if (self->arrows[i]->uuid == data[1] - '0') {
				self->arrows[i]->x = data[2] - '0';
				self->arrows[i]->y = data[3] - '0';
				self->arrows[i]->dir = data[4] - '0';
				break;
			}
		}
		break;
	case 4:
		for (int i = 0; i < self->arrows.size(); i++) {
			if (self->arrows[i]->uuid == data[1] - '0') {
				self->arrows[i]->color = data[2] - '0';
				break;
			}
		}
		break;
	case 5:
		self->tile[data[2] - '0' + (data[3] - '0') * w] = (data[4] - '0');
		break;
	case 6:
		for (int i = 0; i < self->arrows.size(); i++) {
			if (self->arrows[i]->uuid == data[1] - '0') {
				delete self->arrows[i];
				self->arrows.erase(self->arrows.begin() + i);
				break;
			}
		}
		break;
	case 10: // get my uuid
		self->myId = data[1] - '0';
		break;
	case 7: // Return
		self->returns.push_back(&data[1]);
		break;
	default:
		break;
	}
}

vector<string> split(char* data, char ch) {
	string st = "";
	vector <string> ret;
	for (int i = 0; i < strlen(data); i++) {
		if (data[i] == ch) {
			ret.push_back(st);
			st = "";
		}
		else {
			st += data[i];
		}
	}
	if (st != "")
		ret.push_back(st);
	return ret;
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
		
		vector<string> splitted = split(buf, '!');
		for (int i = 0; i < splitted.size(); i++) {
			handle_listen(self, &splitted[i][0]);
		}

		this_thread::sleep_for(0.01s);
	}
}

static PyObject* ftn_go(PyObject* self, PyObject* args) {
	int arg;
	if (!PyArg_ParseTuple(args, "i", &arg)) return NULL;
	if (arg < 0 || arg > 255) return NULL;
	char data_send[128];
	data_send[0] = '0';
	data_send[1] = (char)('0' + arg);
	global_om->send_message(data_send);
	for (int i = 0; i < 100; i++) {
		Sleep(10);
		global_om->onUpdate(0.01);
		event_func();
	}
	Py_RETURN_NONE;
}
static PyObject* ftn_turn(PyObject* self, PyObject* args) {
	int arg;
	if (!PyArg_ParseTuple(args, "i", &arg)) return NULL;
	if (arg < 0 || arg > 255) return NULL;
	char data_send[128];
	data_send[0] = '1';
	data_send[1] = (char)('0' + arg);
	global_om->send_message(data_send);
	for (int i = 0; i < 100; i++) {
		Sleep(10);
		global_om->onUpdate(0.01);
		event_func();
	}
	Py_RETURN_NONE;
}
static PyObject* ftn_get(PyObject* self, PyObject* args) {
	int arg;
	if (!PyArg_ParseTuple(args, "i", &arg)) return NULL;
	if (arg < 0 || arg > 255) return NULL;
	char data_send[128];
	data_send[0] = '2';
	data_send[1] = (char)('0' + arg);
	char* ret = global_om->send_message_wait(data_send);
	return Py_BuildValue("i", ret[0] - '0');
}
static PyObject* ftn_paint(PyObject* self, PyObject* args) {
	int arg;
	if (!PyArg_ParseTuple(args, "i", &arg)) return NULL;
	if (arg < 0 || arg > 255) return NULL;
	char data_send[128];
	data_send[0] = '3';
	data_send[1] = (char)('0' + arg);
	global_om->send_message(data_send);
	Py_RETURN_NONE;
}
static PyObject* ftn_rename(PyObject* self, PyObject* args) {
	char arg[128];
	if (!PyArg_ParseTuple(args, "s", arg)) return NULL;
	char data_send[128];
	data_send[0] = '4';
	strcpy_s(&data_send[1], strlen(arg), arg);
	global_om->send_message(data_send);
	Py_RETURN_NONE;
}
static PyObject* ftn_colorize(PyObject* self, PyObject* args) {
	int arg;
	if (!PyArg_ParseTuple(args, "i", &arg)) return NULL;
	if (arg < 0 || arg > 255) return NULL;
	char data_send[128];
	data_send[0] = '5';
	data_send[1] = (char)('0' + arg);
	global_om->send_message(data_send);
	Py_RETURN_NONE;
}
static struct PyMethodDef methods[] = {
	{"go", ftn_go, METH_VARARGS, ""},
	{"get", ftn_get, METH_VARARGS, ""},
	{"turn", ftn_turn, METH_VARARGS, ""},
	{"paint", ftn_paint, METH_VARARGS, ""},
	{"rename", ftn_rename, METH_VARARGS, ""},
	{"colorize", ftn_colorize, METH_VARARGS, ""},
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
	for (int i = 0; i < w * h; i++) {
		tile[i] = 1;
	}
	tm = textmanager;
	// SetPythonHome excluded
	Py_SetPythonHome(python_home_dir);
	PyImport_AppendInittab("PyClassroom", &PyInit_PyClassroom);
	Py_Initialize();
	PyRun_SimpleString("from PyClassroom import *");
	PyRun_SimpleString("front, left, back, right, here = 0, 1, 2, 3, 4");

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

	client_thread = new thread(client_run, this); // T^T
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

	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			SDL_SetRenderDrawColor(renderer, color_list[tile[x + y * w]][0], color_list[tile[x + y * w]][1], color_list[tile[x + y * w]][2], 255);
			SDL_Rect rect = { x * tile_size + 3, y * tile_size + 3, tile_size - 2, tile_size - 2 };
			SDL_RenderFillRect(renderer, &rect);
		}
	}

	for (int i = 0; i < arrows.size(); i++) {
		Arrow* arr = arrows[i];
		SDL_Rect rect = { arr->dx * tile_size + 7, arr->dy * tile_size + 7, tile_size - 10, tile_size - 10 };
		int cx = arr->dx * tile_size + int(tile_size / 2) + 2,
			cy = arr->dy * tile_size + int(tile_size / 2) + 2;
		Sint16 tr1_x[] = { int(cx + 14 * cos(-arr->ddir * 1.57)) , int(cx + 13 * cos(-arr->ddir * 1.57 + 2.3)) , int(cx + 4 * cos(-arr->ddir * 1.57 + 3.14)) };
		Sint16 tr1_y[] = { int(cy + 14 * sin(-arr->ddir * 1.57)) , int(cy + 13 * sin(-arr->ddir * 1.57 + 2.3)) , int(cy + 4 * sin(-arr->ddir * 1.57 + 3.14)) };
		filledPolygonRGBA(renderer, tr1_x, tr1_y, 3, int(color_list[arr->color][0] * 0.7), int(color_list[arr->color][1] * 0.7), int(color_list[arr->color][2] * 0.7), 255);
		Sint16 tr2_x[] = { int(cx + 14 * cos(-arr->ddir * 1.57)) , int(cx + 13 * cos(-arr->ddir * 1.57 - 2.3)) , int(cx + 4 * cos(-arr->ddir * 1.57 + 3.14)) };
		Sint16 tr2_y[] = { int(cy + 14 * sin(-arr->ddir * 1.57)) , int(cy + 13 * sin(-arr->ddir * 1.57 - 2.3)) , int(cy + 4 * sin(-arr->ddir * 1.57 + 3.14)) };
		filledPolygonRGBA(renderer, tr2_x, tr2_y, 3, int(color_list[arr->color][0] * 0.5), int(color_list[arr->color][1] * 0.5), int(color_list[arr->color][2] * 0.5), 255);
		SDL_Point fp[] = {
		{cx + 14 * cos(-arr->ddir * 1.57), cy + 14 * sin(-arr->ddir * 1.57)},
		{cx + 13 * cos(-arr->ddir * 1.57 + 2.2), cy + 13 * sin(-arr->ddir * 1.57 + 2.3)},
		{cx + 4 * cos(-arr->ddir * 1.57 + 3.14), cy + 4 * sin(-arr->ddir * 1.57 + 3.14)},
		{cx + 13 * cos(-arr->ddir * 1.57 - 2.2), cy + 13 * sin(-arr->ddir * 1.57 - 2.3)},
		{cx + 14 * cos(-arr->ddir * 1.57), cy + 14 * sin(-arr->ddir * 1.57)},
		};
		SDL_SetRenderDrawColor(renderer, int(color_list[arr->color][0] * 0.3), int(color_list[arr->color][1] * 0.3), int(color_list[arr->color][2] * 0.3), 255);
		SDL_RenderDrawLines(renderer, fp, 5);
		if (arr->uuid == myId) {
			filledCircleRGBA(renderer, cx, cy, 3, 255, 255, 32, 168);
		}
		arr->dx += (arr->x - arr->dx) * 0.3;
		arr->dy += (arr->y - arr->dy) * 0.3;
		if ((cos(arr->ddir * 1.57) * sin(arr->dir * 1.57) - cos(arr->dir * 1.57) * sin(arr->ddir * 1.57)) > 0) {
			arr->ddir += (cos(arr->ddir * 1.57) * sin(arr->dir * 1.57) - cos(arr->dir * 1.57) * sin(arr->ddir * 1.57)) * 0.3;
		}
		else {
			arr->ddir += (cos(arr->ddir * 1.57) * sin(arr->dir * 1.57) - cos(arr->dir * 1.57) * sin(arr->ddir * 1.57)) * 0.3;
		}
		arr->ddir = fmod(arr->ddir + 4000, 4);
		arr->dir = (arr->dir + 4000 % 4);
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void OutputManager::onUpdate(double dT) {

}

void OutputManager::onEvent(SDL_Event event) {

}

void OutputManager::execute(string cmd) {
	PyRun_SimpleString(&cmd[0]);
}