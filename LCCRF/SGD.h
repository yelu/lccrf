#pragma once

#include <functional>
#include <vector>
#include <list>
#include <limits>
#include "Log.h"
using std::function;
using std::vector;
using std::list;

template<typename XSampleType, typename YSampleType>
class SGD
{
public:
	typedef function<double (vector<double>&, const XSampleType&, const YSampleType&)> DerivativeFunction;
	typedef function<double (vector<double>&, const XSampleType&, const YSampleType&)> ObjectFunction;

	SGD(const list<XSampleType>& xs,
		const list<YSampleType>& ys,
		vector<double>& weights,
		vector<DerivativeFunction>& derivatives,
		ObjectFunction& object):
		_xs(xs), _ys(ys), _derivatives(derivatives), _object(object), _weights(weights)
	{
		_isObjectProvided = true;
        _iterationCount = 0;
	}

	SGD(const list<XSampleType>& xs,
		const list<YSampleType>& ys,
		vector<double>& weights,
		vector<DerivativeFunction>& derivatives):
		_xs(xs), _ys(ys), _derivatives(derivatives), _weights(weights)
	{
		_isObjectProvided = false;
        _iterationCount = 0;
	}

	const vector<double>& Run(double learningRate, int maxIteration = 1)
	{
		double lastObjectValue = std::numeric_limits<double>::infinity();
		for(int i = 0; i < maxIteration; i++)
		{
			LOG_DEBUG("Iteration %d", i);
            _iterationCount++;
			auto xIte = _xs.begin();
			auto yIte = _ys.begin();
			for(; xIte != _xs.end() && yIte != _ys.end(); xIte++, yIte++)
			{
				UpdateWeights(learningRate, *xIte, *yIte);
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

	void UpdateWeights(double learningRate, const XSampleType& xSample, const YSampleType& ySample)
	{
		vector<double> oldWeights(_weights.size());
		oldWeights.swap(_weights);
		for(size_t i = 0; i < _weights.size(); i++)
		{
			double delta = _derivatives[i](oldWeights, xSample, ySample);
			_weights[i] = oldWeights[i] -  (learningRate * delta);
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
	const list<XSampleType>& _xs;
	const list<YSampleType>& _ys;
	vector<DerivativeFunction> _derivatives;
	ObjectFunction _object;
	vector<double>& _weights;
	bool _isObjectProvided;
    int _iterationCount;
};

