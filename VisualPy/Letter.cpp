
#include "Letter.h"

#include <iostream>


Letter::Letter() {
	std::cout << "Letter default constructor callled" << std::endl;
}

Letter::Letter(int _cx, int _cy, char c, SDL_Color _col) {
	cx = _cx;
	cy = _cy;
	col = _col;
}

void Letter::draw(TextManager* tm) {
	tm->draw_char(cx, cy, c, col);
}
