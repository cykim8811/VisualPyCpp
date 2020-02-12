#pragma once

#include "Statement.h"



int is_digit_0(string* source, int index);
int is_digit_1(string* source, int index);
int is_alphabet_a(string* source, int index);
int is_alphabet_A(string* source, int index);


Node addr(string* source, int* index);
Node addr_calc(string* source, int* index);
Node addr_tuple(string* source, int* index);

Node parse_source(string* source, int* index);