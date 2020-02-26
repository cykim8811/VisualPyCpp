
#include "Executer.h"


Data::Data() {}
Data::~Data() {}

bool Executer::reduce(Node* target) {
	if (target->name == "calc_add") {
		Node* A = &target->subnode[0];
		Node* B = &target->subnode[4];
		if (!reduce(A)) {
			return false;
		}
		if (!reduce(B)) {
			return false;
		}

	}
	else if (target->name == "int") {
		return true;
	}
}

Executer::Executer(vector<line> *lines) {
	source = lines;
	int lx = 0, ly = 0;
	for (int i = 0; i < lines->size(); i++) {
		if (lines->at(i).indent == -1) {
			ly++;
			continue;
		}
		lx += lines->at(i).indent;
		pieces.push_back(P(lines->at(i).data, &lx, &ly, { 96, 128, 128 } ));
		ly++;
	}
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

void Executer::draw(TextManager* tm) {
	for (int i = 0; i < pieces.size(); i++) {
		pieces.at(i).draw(tm);
	}
}