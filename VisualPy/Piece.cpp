

#include "Piece.h"
#include <iostream>

typedef struct Coloring {
	string name;
	vector<string> key;
	vector<SDL_Color> col;
};

extern vector<Coloring> coloring_list;

P::P() {
	std::cout << "Piece default constructor called" << endl;
}

P::P(Node target, int* lx, int* ly, SDL_Color tcol) {
	name = target.name;
	if (target.subnode.size() == 0) {
		for (int i = 0; i < target.data.size(); i++) {
			data.push_back(Letter(*lx, *ly, target.data.at(i), tcol));
			if (target.data.at(i) == '\n') {
				ly++;
				lx = 0;
			}
			else {
				lx++;
			}
		}
	}
	else {
		for (int i = 0; i < target.subnode.size(); i++) {
			SDL_Color tcol = { 96, 96, 96 };
			for (int j = 0; j < coloring_list.size(); j++) {
				if (target.name == coloring_list[j].name) {
					for (int k = 0; k < coloring_list[j].key.size(); k++) {
						if (coloring_list[j].key[k] == target.subnode[i].name) {
							tcol = coloring_list[j].col[k];
							break;
						}
					}
					break;
				}
			}
			subnode.push_back(P(target.subnode[i], lx, ly, tcol));
		}
	}
}

void P::draw(TextManager *tm) {
	if (subnode.size() == 0) {
		for (int i = 0; i < data.size(); i++) {
			data[i].draw(tm);
		}
	}
	else {
		for (int i = 0; i < subnode.size(); i++) {
			subnode[i].draw(tm);
		}
	}
}