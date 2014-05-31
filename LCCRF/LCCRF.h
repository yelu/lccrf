#pragma once

#include <functional>
#include <list>
#include <string>
#include <vector>
#include "IDAllocator.h"
using std::function;
using std::list;
using std::wstring;
using std::vector;

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
	LCCRF(double learningRate);
	virtual ~LCCRF(void);

	static double Phi(int s1, int s2, int j,
		const Document& traningExample, 
		vector<double> weights,
		vector<function <int (const Document&, int, int, int)>> features);

	void MakeDervative();

	void MakeLikelihood();
private:

	function<double (vector<double>&, Document&)> _likelihood;
	vector<function<double (vector<double>&, Document&)>> _derivatives;
	vector<function <int (const Document&, int, int, int)>> _features;
	IDAllocator _yIDAllocator;
	double _learningRate;
};

