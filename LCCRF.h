#pragma once

#include <boost/function.hpp>
using boost::function;
#include <list>
using std::list;
#include <string>
using std::wstring;
#include <vector>
using std::vector;
#include "IDAllocator.h"

struct Word
{
	vector<wstring> x;
	int y;
};
typedef vector<Word> TraningCase;

class LCCRF
{
public:
	LCCRF(void);
	virtual ~LCCRF(void);

	static double Phi(int s1, int s2, int j,
		const TraningCase& traningCase, 
		vector<double> weights,
		vector<function <int (const TraningCase&, int, int, int)>> hypothesises);
	void MakeDervative(list<TraningCase>& traningCases, int k);
	void MakeLikelihood(list<TraningCase>& traningCases);
private:

	function<double (vector<double>)> _likelihood;
	vector<function <double (vector<double>&)>> _derivatives;
	vector<function <int (const TraningCase&, int, int, int)>> _hypothesises;
	IDAllocator _yIDAllocator;
};

