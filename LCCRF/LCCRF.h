#pragma once

#include <functional>
using std::function;
#include <list>
using std::list;
#include <string>
using std::wstring;
#include <vector>
using std::vector;
#include "IDAllocator.h"

typedef vector<wstring> X;
typedef int Y;
struct Token
{
	X x;
	Y y;
};
typedef vector<Token> Document;

class LCCRF
{
public:
	LCCRF(void);
	virtual ~LCCRF(void);

	static double Phi(int s1, int s2, int j,
		const Document& traningExample, 
		vector<double> weights,
		vector<function <int (const Document&, int, int, int)>> features);

	void MakeDervative(list<Document>&, int);

	void MakeLikelihood(list<Document>&);
private:

	function<double (vector<double>)> _likelihood;
	vector<function <double (vector<double>&)>> _derivatives;
	vector<function <int (const Document&, int, int, int)>> _features;
	IDAllocator _yIDAllocator;
};

