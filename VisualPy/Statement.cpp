#include "Statement.h"
#include <iostream>

using namespace std;

string printNode(Node target) {
	if (target.subnode.size() == 0) {
		if (target.name == "ignore") {
			return "";
		}
		if (target.name == "indent") {
			return "";
		}
		if (target.name == target.data) {
			return "'" + target.name + "'";
		}
		return target.name + ":'" + target.data + "'";
	}
	string ret(target.name);
	ret.append("(");
	for (int i = 0; i < target.subnode.size(); i++) {
		if (i != 0) ret.append(" ");
		ret.append(printNode(target.subnode[i]));
	}
	ret.append(")");
	return ret;
}
string reformat(Node target) {
	if (target.subnode.size() == 0) {
		if (target.name == "ignore") {
			//return "";
		}
		return target.data;
	}
	string ret;
	for (int i = 0; i < target.subnode.size(); i++) {
		ret.append(reformat(target.subnode[i]));
	}
	return ret;
}

S::S(string _name, int _op, vector<S> _subnode) {
	op = _op;
	name = _name;
	subnode = _subnode;
	param = 0;
}

S::S(string _name, int _op, vector<S> _subnode, int _param) {
	op = _op;
	name = _name;
	subnode = _subnode;
	param = _param;
}

S::S(string _name, Format _data) {
	op = STR;
	name = _name;
	data = _data;
	param = 0;
}

S::S(string _name) {
	op = STR;
	name = _name;
	data = _name;
	param = 0;
}

S::S(string _name, string _data) {
	op = STR;
	name = _name;
	data = Format(_data);
	param = 0;
}

S::S(string _name, Node(*_ftn)(string*, int*)) {
	op = FTN;
	name = _name;
	ftn = _ftn;
	param = 0;
}

Node S::fit(string* source, int* index) {
	//	cout << name << "/" << data.print() << "/" << *index << endl;
	if (*index < 0 || *index >= source->size()) {
		*index = -1;
		return Node();
	}
	if (op == STR) {
		int ret = data.fit(source, *index);
		if (ret == -1) {
			*index = -1;
			return Node();
		}
		else {
			int _offset = *index,
				_size = ret - *index;
			*index = ret;
			return Node{ name, source->substr(_offset, _size), {} };
		}
	}
	else if (op == AND) {
		vector<Node> ret;
		for (int i = 0; i < subnode.size(); i++) {
			Node r = subnode[i].fit(source, index);
			if (*index == -1)
				return Node();
			if (r.name == "") {
				if (r.data != "") {
					ret.push_back(r);
				}
				ret.insert(ret.end(), r.subnode.begin(), r.subnode.end());
			}
			else {
				ret.push_back(r);
			}
		}
		return Node{ name, "", ret };
	}
	else if (op == OR) {
		for (int i = 0; i < subnode.size(); i++) {
			int ind = *index;
			Node r = subnode[i].fit(source, &ind);
			if (ind != -1) {
				*index = ind;
				if (name == "")
					return r;
				else
					return Node{ name, "", { r } };
			}
		}
		*index = -1;
		return Node();
	}
	else if (op == REP) {
		vector<Node> ret;
		for (int i = 0; true; i++) {
			int ind = *index;
			Node r = subnode[0].fit(source, &ind);
			if (ind != -1 && ind != *index) {
				*index = ind;
				if (r.name == "") {
					if (r.data != "") {
						ret.push_back(r);
					}
					ret.insert(ret.end(), r.subnode.begin(), r.subnode.end());
				}
				else {
					ret.push_back(r);
				}
			}
			else {
				if (i < param) {
					*index = -1;
					return Node();
				}
				return Node{ name, "", ret };
			}
		}
		*index = -1;
		return Node();
	}
	else if (op == FTN) {
		return ftn(source, index);
	}
	else {
		cout << "ERROR:" << op << endl;
		return Node();
	}
}
