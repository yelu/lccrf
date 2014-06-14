#pragma once

#include <functional>
#include <list>
#include <string>
#include <vector>
using std::wstring;
using std::vector;

typedef vector<wstring> X;
typedef wstring Y;
struct Token
{
	Token(X& _x, Y& _y):x(_x),y(_y){}
	X x;
	Y y;
};
typedef vector<Token> Document;
