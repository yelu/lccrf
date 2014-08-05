#include "LCCRF.h"
#include "Viterbi.h"
#include "SGD.h"
#include "FWBW.h"

LCCRF::LCCRF(int featureCount, int labelCount):_weights(featureCount, 0.0)
{
	_featureCount = featureCount;
	_labelCount = labelCount;
}

LCCRF::~LCCRF(void)
{
}

void LCCRF::Fit(const vector<XSampleType>& xs, 
				const vector<YSampleType>& ys, 
				int maxIteration, 
				double learningRate, 
				double l2)
{
	_xs = &xs;
	_ys = &ys;
	SGD sgd(*_xs, *_ys, _weights, _labelCount);
	sgd.Run(learningRate, l2, maxIteration);
}

void LCCRF::Fit(XType& xs, YType& ys, int maxIteration, double learningRate, double l2)
{
	Fit(xs.Raw(), ys.Raw(), maxIteration, learningRate, l2);
}

void LCCRF::Predict(const XSampleType& x, YSampleType& y)
{
    MultiArray<double, 3> graph(x.Length(), _labelCount, _labelCount, 0.0);
	SGD::MakePhiMatrix(x, _weights, 1.0, graph);
    vector<int> path(x.Length(), -1);
	Viterbi::GetPath(graph, path);
	y.Clear();
	for(auto ite = path.begin(); ite != path.end(); ite++)
	{
        y.AppendTag(*ite);
	}
}

void LCCRF::Predict(XType& xs, YType& ys)
{
	const vector<XSampleType>& rawXs = xs.Raw();
	for(auto ite = rawXs.begin(); ite != rawXs.end(); ite++)
	{
		YSampleType y;
		Predict(*ite, y);
        ys.Append(y);
	}
}

vector<double>& LCCRF::GetWeights()
{
	return _weights;
}

/*
   j  : current position
   s1 : previous state
   s2 : current state
*/
double LCCRF::_Phi(int s1, int s2, int j,
				  const XSampleType& x, 
				  vector<double>& weights,
                  list<pair<int, double>>* hitFeatures)
{
	double ret = 0.0;
	for(auto ite = x.Raw().begin(); ite != x.Raw().end(); ite++)
	{
		auto featureID = ite->first;
		auto positions = ite->second;
		for(auto position = positions->begin(); position != positions->end(); position++)
		{
			if(j == position->j && s1 == position->s1 && s2 == position->s2)
			{
				ret += (weights[featureID]);
				hitFeatures->push_back(pair<int, double>(featureID, weights[featureID]));
			}
		}
	}
	return ret;
}

pair<list<list<pair<int, double>>>, double> LCCRF::Debug(const XSampleType& x, 
	                                                     const YSampleType& y)
{
	int preState = -1;
	double score = 0.0;
	pair<list<list<pair<int, double>>>, double> res;
    for(int j = 0; j < x.Length(); j++)
	{
		if(y.Tags()[j] >= _labelCount)
		{
			return res;
		}
        res.first.push_back(list<pair<int, double>>());
        score += _Phi(preState, y.Tags()[j], j, x, _weights, &(res.first.back()));
		preState = y.Tags()[j];
	}
    res.second = score;
    return res;
}
