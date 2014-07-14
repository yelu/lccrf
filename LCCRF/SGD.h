#pragma once

#include <functional>
#include <vector>
#include <list>
#include <limits>
#include "Log.h"
#include "Types.h"
using std::function;
using std::vector;
using std::list;


class SGD
{
public:
	typedef function<double (const XSampleType&, const YSampleType&, vector<double>&, double, int)> DerivativeFunction;
	typedef function<double (const XSampleType&, const YSampleType&, vector<double>&)> ObjectFunction;

	SGD(const vector<XSampleType>& xs,
		const vector<YSampleType>& ys,
		vector<double>& weights,
		DerivativeFunction& derivative,
		ObjectFunction& object):
		_xs(xs), _ys(ys), _derivative(derivative), _object(object), _weights(weights)
	{
		_isObjectProvided = true;
        _iterationCount = 0;
	}

	SGD(const vector<XSampleType>& xs,
		const vector<YSampleType>& ys,
		vector<double>& weights,
		DerivativeFunction& derivative):
		_xs(xs), _ys(ys), _derivative(derivative), _weights(weights)
	{
		_isObjectProvided = false;
        _iterationCount = 0;
	}

	const vector<double>& Run(double learningRate, double l2, int maxIteration = 1)
	{
		double lastObjectValue = std::numeric_limits<double>::infinity();
        _c = 1;  // scalar update factor.
        _learningRate = learningRate;
        _l2 = l2;
        for(int i = 0; i < maxIteration; i++)
		{
			LOG_DEBUG("Iteration %d", i);
            _iterationCount++;
			auto xIte = _xs.begin();
			auto yIte = _ys.begin();
			for(; xIte != _xs.end() && yIte != _ys.end(); xIte++, yIte++)
			{
				UpdateWeights(*xIte, *yIte);
			}         
			// one iteration(epoch) finished, check it converged.
			if(_isObjectProvided && i != 0 && IsConveraged(lastObjectValue))
			{
				break;
			}
		}
        LOG_DEBUG("total iterations = %d\n", _iterationCount);
		return _weights;
	}

	void UpdateWeights(const XSampleType& xSample, const YSampleType& ySample)
	{
		vector<double> oldWeights(_weights.size());
		oldWeights.swap(_weights);
        // rescale to void dense update of weights resulted by l2 regularition.
        if(_c <  0 - 10e6 || _c > 10e6)
        {
            for(int i = 0; i < _weights.size(); i++)
            {
                _weights[i] = _weights[i] * _c;
            }
            _c = 1.0;
        }
        _c = (1 - _learningRate * _l2) * _c;
        // skip updating the i-th feature if the feature is not triggered in xSample. Since the derivative will be zero.
        auto featureSet = xSample.GetFeatureSet();
        for(auto f = featureSet.begin(); f != featureSet.end(); f++)
        {
            double delta = _derivative(xSample, ySample, oldWeights, _c / (1 - _learningRate * _l2), *f);
            _weights[*f] = oldWeights[*f] -  _learningRate * delta / _c;
        }
	}

	bool IsConveraged(double& lastObjectValue)
	{
		double newObjectValue = 0.0;
		auto xIte = _xs.begin();
		auto yIte = _ys.begin();
		for(; xIte != _xs.end() && yIte != _ys.end(); xIte++, yIte++)
		{
			newObjectValue += _object(_weights, *xIte, *yIte);
		}
		double delta = newObjectValue - lastObjectValue;
		lastObjectValue = newObjectValue;
		LOG_DEBUG("delta : %f\tobject : %f", 0 - delta, newObjectValue);
		if(delta <= 0)
		{
			delta = 0 - delta;
		}
		if(delta < 10e-8) 
		{
			LOG_DEBUG("Converged. delta : %f", delta);
            // check if all derivatives equal to zero.
            /*
            for(int i = 0; i < _weights.size(); i++)
            {
                auto xIte = _xs.begin();
			    auto yIte = _ys.begin();
                double res = 0;
			    for(; xIte != _xs.end() && yIte != _ys.end(); xIte++, yIte++)
			    {
                    res += _derivatives[i](_weights, *xIte, *yIte);
                }
                if(abs(res) > 10e-4)
                {
                    LOG_DEBUG("not converged. i = %d, di=%f", i, res);
                }
            }*/
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual ~SGD(void){}
	
private:
	const vector<XSampleType>& _xs;
	const vector<YSampleType>& _ys;
	DerivativeFunction _derivative;
	ObjectFunction _object;
	vector<double>& _weights;
    double _c;
    double _l2;
    double _learningRate;
	bool _isObjectProvided;
    int _iterationCount;
};

