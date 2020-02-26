
#include "Executer.h"


Data::Data() {}
Data::~Data() {}

Executer::Executer() {}

Executer::Executer(vector<line> *lines) {
	source = lines;
	float lx = 0, ly = 0;
	for (int i = 0; i < lines->size(); i++) {
		if (lines->at(i).indent == -1) {
			P pcs = P(lines->at(i).comment, &lx, &ly, { 96, 128, 128 });
			pieces.push_back(pcs);
			continue;
		}
		lx += lines->at(i).indent * indent_text.length();
		P pcs = P(lines->at(i).data, &lx, &ly, { 96, 128, 128 });
		pieces.push_back(pcs);
		P pcscmt = P(lines->at(i).comment, &lx, &ly, { 96, 128, 128 });
		pieces.push_back(pcscmt);
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
		pieces.at(i).draw(tm, cur_length);
	}
}