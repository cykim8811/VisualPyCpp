#include "Format.h"
#include <string>
#include <vector>
#include <stdio.h>

using namespace std;


/*
	definition of Format
*/
Format::Format() {}
Format::Format(int _op, vector<Format> _subnode) {
	op = _op;
	subnode = vector<Format>(_subnode);
}
Format::Format(string _data) {
	op = STR;
	data = string(_data);
}
Format::Format(int _op, int (*_ftn)(string*, int)) {
	op = _op;
	ftn = _ftn;
}

Format::~Format() {}

string Format::print() {
	if (op == STR) {
		return data;
	}
	else {
		string ret("");
		switch (op)
		{
		case AND:
			ret = "AND";
			break;
		case OR:
			ret = "OR";
			break;
		case REP:
			ret = "REP";
			break;
		case FTN:
			ret = "FTN";
			break;
		default:
			ret = "ERR";
			break;
		}
		ret.append("(");
		for (int i = 0; i < subnode.size(); i++) {
			if (i != 0) {
				ret.append(",");
			}
			ret.append(subnode[i].print());
		}
		ret.append(")");
		return ret;
	}
}

int Format::fit(string* source, int index) {
	if (index > source->size() || index < 0) {
		//if (op == REP) return index;
		return -1;
	}
	if (op == STR) {
		if (source->size() < index + data.size()) {
			return -1;
		}
		if (source->substr(index, data.size()) == data) {
			return index + data.size();
		}
		return -1;
	}
	else if (op == AND) {
		for (int i = 0; i < subnode.size(); i++) {
			index = subnode[i].fit(source, index);
			if (index == -1)
				return -1;
		}
		return index;
	}
	else if (op == OR) {
		for (int i = 0; i < subnode.size(); i++) {
			int r = subnode[i].fit(source, index);
			if (r != -1)
				return r;
		}
		return -1;
	}
	else if (op == REP) {
		while (true) {
			int r = subnode[0].fit(source, index);
			if (r == -1 || index == r)
				return index;
			index = r;
		}
	}
	else if (op == FTN) {
		if (source->size() <= index)
			return -1;
		return ftn(source, index);
	}
	else {
		return -1;
	}
}
