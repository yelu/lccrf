#pragma once

#include <functional>
#include <vector>
#include <cmath>
using std::function;
using std::vector;

template<TrainingExampleType>
class SGD
{
public:
	typedef function<double (vector<double>&, TrainingExampleType&)> DerivativeFunction;
	typedef function<double (vector<double>&, list<TrainingExampleType>&)> ObjectFunction;

	SGD(list<TrainingExampleType>& traniningSet,
		vector<double> weights,
		vector<DerivativeFunction>& derivatives,
		ObjectFunction& object):
		_trainingSet(traniningSet), _derivatives(derivatives), _object(object), _weights(weights)
	{
	}

	const vector<double>& Run(double learningRate, int batch = 1, int maxIteration = 1)
	{
		double lastObjectValue = 0.0;
		for(int i = 0; i < maxIteration; i++)
		{
			int count = 0;
			list<TrainingExampleType>::iterator begin = _trainingSet.begin();
			for(auto ite = _trainingSet.begin(); ite != _trainingSet.end(); ite++)
			{
				count++;
				if(count >= batch)
				{
					TrainABatch(learningRate, begin, ite);
					begin = ite + 1;
					count = 0;
				}
			}
			TrainABatch(learningRate, begin, _trainingSet.end());
			if(IsConveraged(lastObjectValue))
			{
				break;
			}
		}
	}

	void TrainABatch(double learningRate, list<TrainingExampleType>::iterator begin, list<TrainingExampleType>::iterator end)
	{
		for(int i = 0; i < _weights.size(); i++)
		{
			double delta = 0.0;
			for(auto ite = begin; ite != end; ite++)
			{
				delta += _derivative[i](_weights, *ite)
			}
			int& weight = _weights[i];
			weight += (learningRate * delta);
		}
	}

	bool IsConveraged(double lastObjectValue)
	{
		int newObjectValue = 0.0;
		for(auto ite = _trainingSet.begin(); ite != _trainingSet.end(); ite++)
		{
			newObjectValue += _object(_weights, *ite);
		}
		if(abs(newObjectValue - lastObjectValue) < 10e-1) 
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual ~SGD(void);
	
private:
	list<TrainingExampleType>& _trainingSet;
	vector<DerivativeFunction> _derivatives;
	ObjectFunction _object;
	vector<double>& _weights;
};

