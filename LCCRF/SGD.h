#pragma once

#include <functional>
#include <vector>
#include <list>
#include <limits>
#include "Log.h"
#include "Types.h"
#include "FWBW.h"
using std::function;
using std::vector;
using std::list;

class SGD
{
public:
	SGD(const vector<XSampleType>& xs,
		const vector<YSampleType>& ys,
		vector<double>& weights,
		int labelCount):
		_xs(xs), 
		_ys(ys),
		_weights(weights),
		_labelCount(labelCount)
	{
        _iterationCount = 0;
		memset(_weights.data(), 0, sizeof(double) * _weights.size());
	}

	const vector<double>& Run(double learningRate, double l2, int maxIteration = 20);

	void Rescale();

	double UpdateWeights(const XSampleType& xSample, const YSampleType& ySample);

	virtual ~SGD(void){}

	static void MakePhiMatrix(const XSampleType& xSample, 
					vector<double>& weights, 
					double scale, 
					MultiArray<double, 3>& phiMatrix);

	static double _CaculateGradient(const XSampleType& x, 
									const YSampleType& y, 
									int k,
									const MultiArray<double, 3>& qMatrix);

	static double _CalculateLikelihood(const XSampleType& x, 
									   const YSampleType& y,
							           const MultiArray<double, 3>& phiMatrix,
							           double z);

private:
	const vector<XSampleType>& _xs;
	const vector<YSampleType>& _ys;
	int _labelCount;
	vector<double>& _weights;
    double _scale;
    double _l2;
    double _learningRate;
    int _iterationCount;
};

