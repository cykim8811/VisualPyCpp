

#include "Piece.h"


bool isTrue(Data data) {
	return true;
}

P::P(Node target) {
	this->name = target.name;
	this->data = target.data;
	for (int i = 0; i < target.subnode.size(); i++) {
		subnode.push_back(P(target.subnode[i]));
	}

}

Data P::process(Executer exec) {
	if (name == "lines") {
		for (int i = 0; i < subnode.size(); i++) {
			subnode[i].process(exec);
		}
	}
	else if (name == "s_if") {
		int ind;
		while (subnode[ind].name != "condition") { ind++; }
		if (isTrue(subnode[ind].process(exec))) {
			while (subnode[ind].name != "lines") { ind++; }
			subnode[ind].process(exec);
			return Data();
		}
		else {
			return Data();
		}
	}
	else if (name == "") {
		
	}
	return Data();
}