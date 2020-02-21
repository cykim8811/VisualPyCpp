
#include "Executer.h"


Data::Data() {}
Data::~Data() {}

Executer::Executer(string(*_log)(string)) {
	log = _log;
}

int Executer::get(string name) {
	for (int i = 0; i < ram.size(); i++) {
		if (ram[i].name == name) {
			return i;
		}
	}
	return -1;
}

int Executer::put(string name, Data data) {
	int ind = get(name);
	if (ind == -1) {
		Data target = Data(data);
		target.ref_count = 1;
		ram.push_back(target);
		return ram.size() - 1;
	}
	ram[ind].ref_count--;
	if (ram[ind].ref_count <= 0) {
		// Memory remove
	}
	ram[ind] = data;
	return ind;
}
