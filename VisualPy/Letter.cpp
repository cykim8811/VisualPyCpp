
#include "Letter.h"

#include <iostream>

using namespace std;

Letter::Letter() {
	std::cout << "Letter default constructor callled" << std::endl;
}

Letter::Letter(float _cx, float _cy, char _c, SDL_Color _col) {
	cx = _cx;
	cy = _cy;
	col = _col;
	c = _c;
}

void Letter::draw(TextManager* tm, int cur_line) {
	tm->draw_char(36 + tm->width * cx, tm->height * (cy - cur_line) + 2, c, col);
}
