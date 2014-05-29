#pragma once

#include <functional>
#include <vector>
#include <list>
#include <cmath>
using std::function;
using std::vector;
using std::list;

template<typename TrainingExampleType>
class SGD
{
public:
	typedef function<double (vector<double>&, TrainingExampleType&)> DerivativeFunction;
	typedef function<double (vector<double>&, TrainingExampleType&)> ObjectFunction;

	SGD(list<TrainingExampleType>& traniningSet,
		vector<double>& weights,
		vector<DerivativeFunction>& derivatives,
		ObjectFunction& object):
		_trainingSet(traniningSet), _derivatives(derivatives), _object(object), _weights(weights)
	{
		_isObjectProvided = true;
	}

	SGD(list<TrainingExampleType>& traniningSet,
		vector<double>& weights,
		vector<DerivativeFunction>& derivatives):
		_trainingSet(traniningSet), _derivatives(derivatives), _weights(weights)
	{
		_isObjectProvided = false;
	}

	const vector<double>& Run(double learningRate, int batch = 1, int maxIteration = 1)
	{
		double lastObjectValue = 0.0;
		for(int i = 0; i < maxIteration; i++)
		{
			int count = 0;
			typename list<TrainingExampleType>::iterator begin = _trainingSet.begin();
			for(auto ite = _trainingSet.begin(); ite != _trainingSet.end(); ite++)
			{
				count++;
				if(count > batch)
				{
					TrainABatch(learningRate, begin, ite);
					begin = ite;
					count = 1;
				}
			}
			if(begin != _trainingSet.end())
			{
				TrainABatch(learningRate, begin, _trainingSet.end());
			}
			if(_isObjectProvided && IsConveraged(lastObjectValue))
			{
				printf("converged. %d \n", i);
				break;
			}
		}
		return _weights;
	}

	void TrainABatch(double learningRate, typename list<TrainingExampleType>::iterator begin, typename list<TrainingExampleType>::iterator end)
	{
		vector<double> oldWeights(_weights.size());
		oldWeights.swap(_weights);
		for(size_t i = 0; i < _weights.size(); i++)
		{
			double delta = 0.0;
			for(auto ite = begin; ite != end; ite++)
			{
				delta += _derivatives[i](oldWeights, *ite);
			}
			_weights[i] = oldWeights[i] -  (learningRate * delta);
		}
	}

	bool IsConveraged(double& lastObjectValue)
	{
		double newObjectValue = 0.0;
		for(auto ite = _trainingSet.begin(); ite != _trainingSet.end(); ite++)
		{
			newObjectValue += _object(_weights, *ite);
		}
		double delta = abs(newObjectValue - lastObjectValue);
		lastObjectValue = newObjectValue;
		if(delta < 10e-1) 
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
	list<TrainingExampleType>& _trainingSet;
	vector<DerivativeFunction> _derivatives;
	ObjectFunction _object;
	vector<double>& _weights;
	bool _isObjectProvided;
};

