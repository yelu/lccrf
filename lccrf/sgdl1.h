#pragma once

#include <functional>
#include <vector>
#include <list>
#include <limits>
#include <cstring>
#include "log.h"
#include "types.h"
#include "fwbw.h"
using std::function;
using std::vector;
using std::list;

class SGDL1
{
public:
    SGDL1(const vector<X>& xs,
          const vector<Y>& ys,
		  const LCCRFFeatures& lccrfFeatures,
          vector<double>& initialWeights) :
          _xs(xs),
          _ys(ys),
		  _lccrfFeatures(lccrfFeatures),
          _weights(initialWeights)
    {
		// get tag count.
		_labelCount = _lccrfFeatures.LabelCount();
    }

    const vector<double>& Run(double learningRate, double l1, int maxIteration = 20);

    double UpdateWeights(const X& x, const Y& y, vector<double>& qs, double u);

    virtual ~SGDL1(void){}

private:
    const vector<X>& _xs;
    const vector<Y>& _ys;
	const LCCRFFeatures& _lccrfFeatures;
    vector<double>& _weights;
    int _labelCount;
    double _l1;
    double _learningRate;
};
