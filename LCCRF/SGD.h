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
	typedef function<double (const XSampleType&, 
						     const YSampleType&, 
							 vector<double>&, 
							 double, 
							 int,
							 bool)> DerivativeFunction;

	typedef function<double (const XSampleType&, 
		                     const YSampleType&, 
							 vector<double>&, 
							 double)> ObjectFunction;

	SGD(const vector<XSampleType>& xs,
		const vector<YSampleType>& ys,
		vector<double>& weights,
		DerivativeFunction& derivative,
		ObjectFunction& object):
		_xs(xs), 
		_ys(ys),
		_derivative(derivative), 
		_object(object), 
		_weights(weights)
	{
		_isObjectProvided = true;
        _iterationCount = 0;
	}

	SGD(const vector<XSampleType>& xs,
		const vector<YSampleType>& ys,
		vector<double>& weights,
		DerivativeFunction& derivative):
		_xs(xs), 
		_ys(ys),
		_derivative(derivative), 
		_weights(weights)
	{
		_isObjectProvided = false;
        _iterationCount = 0;
	}

	const vector<double>& Run(double learningRate, double l2, int maxIteration = 1)
	{
		double lastObjectValue = std::numeric_limits<double>::infinity();
		_scale = 1.0;  // scalar update factor.
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
		// final rescale to get final weights.
		Rescale();
        LOG_DEBUG("total iterations = %d\n", _iterationCount);
		return _weights;
	}

	void Rescale()
	{
		for(auto i = _weights.begin(); i != _weights.end(); i++)
        {
			(*i) = (*i) * _scale;
        }
		_scale = 1.0;
	}

	void UpdateWeights(const XSampleType& xSample, const YSampleType& ySample)
	{
        // rescale to void dense update of weights resulted by l2 regularition.
		if(_scale <  0 - 1e6 || _scale > 1e6)
        {
            Rescale();
        }
		double oldScale = _scale;
		_scale *= (1 - _learningRate * _l2);
		double gain = _learningRate / _scale;
        
        // to save weights which has been changed.
        std::list<std::pair<int, double>> changedWeights;
        // skip updating the i-th feature if the feature is not triggered in xSample. 
		// Since the derivative will be zero.
		auto featureSet = xSample.Raw();
		// For every x sample, when _derivitive is called for the first time, 
		// we need to recalculate forward-nackward. otherwise, it can be 
		// reused to save time.
		bool reset = true;
        for(auto f = featureSet.begin(); f != featureSet.end(); f++)
        {
			double delta = _derivative(xSample, ySample, _weights, oldScale, f->first, reset);
			reset = false;
			changedWeights.push_back(std::pair<int, double>(f->first, _weights[f->first] -  gain * delta));
        }
        // update changed weights to _weights.
        for(auto ite = changedWeights.begin(); ite != changedWeights.end(); ite++)
        {
            _weights[ite->first] = ite->second;
        }
	}

	bool IsConveraged(double& lastObjectValue)
	{
		double newObjectValue = 0.0;
		auto xIte = _xs.begin();
		auto yIte = _ys.begin();
		for(; xIte != _xs.end() && yIte != _ys.end(); xIte++, yIte++)
		{
			newObjectValue += _object(*xIte, *yIte, _weights, _scale);
		}
		for(auto i = _weights.begin(); i != _weights.end(); i++)
		{
			newObjectValue += (0.5 * _l2 * (*i) * (*i) * _scale * _scale);
		}

		double delta = newObjectValue - lastObjectValue;
		lastObjectValue = newObjectValue;
		LOG_DEBUG("delta : %f\tobject : %f", 0 - delta, newObjectValue);
		if(delta <= 0)
		{
			delta = 0 - delta;
		}
		if(delta < 1e-6) 
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
    double _scale;
    double _l2;
    double _learningRate;
	bool _isObjectProvided;
    int _iterationCount;
};

