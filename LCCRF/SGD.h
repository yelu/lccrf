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

	const vector<double>& Run(double learningRate, int batch = 1, int maxIteration = 1)
	{
		double lastObjectValue = std::numeric_limits<double>::infinity();
		for(int i = 0; i < maxIteration; i++)
		{
			LOG_DEBUG("Iteration %d", i);
            _iterationCount++;
			int count = 0;
			typename list<XSampleType>::const_iterator xBegin = _xs.begin();
			typename list<YSampleType>::const_iterator yBegin = _ys.begin();
			auto xIte = _xs.begin();
			auto yIte = _ys.begin();
			for(; xIte != _xs.end() && yIte != _ys.end(); xIte++, yIte++)
			{
				count++;
				if(count > batch)
				{
					TrainABatch(learningRate, xBegin, xIte, yBegin, yIte);
					xBegin = xIte;
					yBegin = yIte;
					count = 1;
				}
			}
			if(xBegin != _xs.end())
			{
				TrainABatch(learningRate, xBegin, _xs.end(), yBegin, _ys.end());
			}
			if(_isObjectProvided && i != 0 && IsConveraged(lastObjectValue))
			{
				break;
			}
		}
        printf("iterations = %d\n", _iterationCount);
		return _weights;
	}

	void TrainABatch(double learningRate, 
					 typename list<XSampleType>::const_iterator xBegin, 
					 typename list<XSampleType>::const_iterator xEnd, 
					 typename list<YSampleType>::const_iterator yBegin, 
					 typename list<YSampleType>::const_iterator yEnd)
	{
		vector<double> oldWeights(_weights.size());
		oldWeights.swap(_weights);
		for(size_t i = 0; i < _weights.size(); i++)
		{
			double delta = 0.0;
			auto xIte = xBegin;
			auto yIte = yBegin;
			for(; xIte != xEnd && yIte != yEnd; xIte++, yIte++)
			{
				double d = _derivatives[i](oldWeights, *xIte, *yIte);
				delta += d;
			}
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

