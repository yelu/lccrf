#pragma once

#include <functional>
#include <list>
#include <string>
#include <vector>
#include "IDAllocator.h"
#include "SGD.h"
using std::function;
using std::list;
using std::wstring;
using std::vector;

typedef vector<wstring> X;
typedef int Y;
struct Token
{
	Token(X& _x, Y& _y):x(_x),y(_y){}
	X x;
	Y y;
};
typedef vector<Token> Document;

class LCCRF
{
public:

	typedef function <int (const Document&, int, int, int)> FeatureType;

	LCCRF(vector<function <int (const Document&, int, int, int)>>&, double learningRate);
	virtual ~LCCRF(void);

	static double Phi(int s1, int s2, int j,
		const Document& traningExample, 
		vector<double> weights,
		vector<function <int (const Document&, int, int, int)>> features);

	void MakeDervative();

	void MakeLikelihood();

	void Learn(list<Document>&, int maxIteration);

	void Predict();

private:

	function<double (vector<double>&, Document&)> _likelihood;
	vector<function<double (vector<double>&, Document&)>> _derivatives;
	vector<FeatureType> _features;
	IDAllocator _yIDAllocator;
	double _learningRate;
	vector<double> _weights;
};

