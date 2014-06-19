#pragma once

#include <functional>
#include <vector>
#include <list>
#include <cmath>
using std::function;
using std::vector;
using std::list;

template<typename XType, typename YType>
class SGD
{
public:
	typedef function<double (vector<double>&, XType&, YType&)> DerivativeFunction;
	typedef function<double (vector<double>&, XType&, YType&)> ObjectFunction;

	SGD(list<XType>& xs,
		list<YType>& ys,
		vector<double>& weights,
		vector<DerivativeFunction>& derivatives,
		ObjectFunction& object):
		_xs(xs), _ys(ys), _derivatives(derivatives), _object(object), _weights(weights)
	{
		_isObjectProvided = true;
	}

	SGD(list<XType>& xs,
		list<YType>& ys,
		vector<double>& weights,
		vector<DerivativeFunction>& derivatives):
		_xs(xs), _ys(ys),, _derivatives(derivatives), _weights(weights)
	{
		_isObjectProvided = false;
	}

	const vector<double>& Run(double learningRate, int batch = 1, int maxIteration = 1)
	{
		double lastObjectValue = std::numeric_limits<double>::infinity();
		for(int i = 0; i < maxIteration; i++)
		{
			printf("Iteration %d \n", i);
			int count = 0;
			typename list<XType>::iterator xBegin = _xs.begin();
			typename list<YType>::iterator yBegin = _ys.begin();
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
			if(_isObjectProvided && IsConveraged(lastObjectValue))
			{
				break;
			}
		}
		return _weights;
	}

	void TrainABatch(double learningRate, 
					 typename list<XType>::iterator xBegin, 
					 typename list<XType>::iterator xEnd, 
					 typename list<YType>::iterator yBegin, 
					 typename list<YType>::iterator yEnd)
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
		printf("delta : %f\tobject : %f\n", 0 - delta, newObjectValue);
		if(abs(delta) < 10e-8) 
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual ~SGD(void){}
	
private:
	list<XType>& _xs;
	list<YType>& _ys;
	vector<DerivativeFunction> _derivatives;
	ObjectFunction _object;
	vector<double>& _weights;
	bool _isObjectProvided;
};

