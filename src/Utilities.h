#ifndef UTILITIES_H_INCLUDED_
#define UTILITIES_H_INCLUDED_ 

#include "Vector.h"

#include <cstring>
#include <ostream>
#include <string>
#include <string.h>
#include <string_view>
#include <iostream>
#include <istream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <bitset>
#include <time.h>

#define END_OF_ARGS "E.n.D.o.F.a.R.g.S."
#define MIN_AMOUNT_TRANSFER_ARGS 3

using namespace std;

string Hex2Bin(const string&);

bool is_float_or_double(const string &);
bool is_alphanumeric(const string &);
bool is_alphabetic(const string &);
bool is_alphanumeric_point(const string &);
bool is_int(const string &);
bool is_point(const string &);
int count_points(string);
void valid_init_args(Vector<string> &);
void valid_transfer_args_amount(Vector <string> &);
void valid_transfer_args( Vector <string>, float &, string &);
Vector <string> split_string(string &, string &);

#endif