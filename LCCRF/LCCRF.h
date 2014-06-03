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
typedef wstring Y;
struct Token
{
	Token(X& _x, Y& _y):x(_x),y(_y){}
	X x;
	int yID;
	Y y;
};
typedef vector<Token> Document;

class LCCRF
{
public:

	typedef function <int (const Document&, wstring, wstring, int)> FeatureType;

	LCCRF(vector<FeatureType>&, double);
	virtual ~LCCRF(void);

	static double Phi(wstring s1, wstring s2, int j,
		const Document& doc, 
		vector<double>& weights,
		vector<FeatureType>& features);

	void MakeDervative();

	void MakeLikelihood();

	void AllocateIDForY();

	void Learn(list<Document>& traningSet, double learningRate = 0.01, int batch = 1, int maxIteration = 1);

	void Predict();

private:

	function<double (vector<double>&, Document&)> _likelihood;
	vector<function<double (vector<double>&, Document&)>> _derivatives;
	vector<FeatureType> _features;
	IDAllocator _yIDAllocator;
	double _lambda;
	vector<double> _weights;
	list<Document>* _pTraningSet;
};

