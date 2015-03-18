#pragma once

#include <functional>
#include <vector>
#include <list>
#include <limits>
#include "Log.h"
#include "Types.h"
#include "FWBW.h"
#include <cstring>

using std::function;
using std::vector;
using std::list;

class SGDL1
{
public:
	SGDL1(const vector<XSampleType>& xs,
		  const vector<YSampleType>& ys,
		  vector<double>& initialWeights,
		  int labelCount) :
		  _xs(xs),
		  _ys(ys),
		  _weights(initialWeights),
		  _labelCount(labelCount)
	{
		_iterationCount = 0;
	}

	const vector<double>& Run(double learningRate, double l1, int maxIteration = 20);

	double UpdateWeights(const XSampleType& x, const YSampleType& y, vector<double>& qs, double u);

	virtual ~SGDL1(void){}

private:
	const vector<XSampleType>& _xs;
	const vector<YSampleType>& _ys;
	vector<double>& _weights;
	int _labelCount;
	double _l1;
	double _learningRate;
	int _iterationCount;
};
