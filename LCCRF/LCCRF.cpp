#include "LCCRF.h"
#include "Viterbi.h"
#include "SGD.h"
#include "FWBW.h"

LCCRF::LCCRF(int featureCount, int labelCount):_weights(featureCount, 0.0)
{
	_featureCount = featureCount;
	_labelCount = labelCount;
	_cachedQMatrix = NULL;
}

LCCRF::~LCCRF(void)
{
}

void LCCRF::_MakePhiMatrix(const XSampleType& xSample, 
						   vector<double>& weights, 
						   double scale, 
						   FWBW::Matrix3& phiMatrix)
{
	for(XSampleType::FeaturesContainer::const_iterator ite = xSample.Raw().begin();
		ite != xSample.Raw().end(); ite++)
	{
		auto positions = ite->second;
		auto featureID = ite->first;
		for(auto position = positions->begin(); position != positions->end(); position++)
		{
			int j = position->j;
			int s1 = position->s1;
			int s2 = position->s2;
			if(0 == j && -1 == s1)
			{
				s1 = 0;
			}
			phiMatrix[j][s1][s2] += (scale * weights[featureID]);
		}
	}
}

void LCCRF::_MakeDervative()
{
	function<double (const XSampleType&, const YSampleType&, vector<double>&, double, int, bool)> derivative = 
		[&](const XSampleType& x, const YSampleType& y, vector<double>& weights, double scale, int k, bool reset)
	{
		int labelCount = _labelCount;
		double res1 = 0.0; // linear
		double res2 = 0.0; // linear

		// forward-backword calculation.
		// use a cache to avoid unnecessary calculation.
		if(reset)
		{
			FWBW::Matrix3 phiMatrix(y.Length(), vector<vector<double>>(labelCount, vector<double>(labelCount, 0.0)));
			LCCRF::_MakePhiMatrix(x, weights, scale, phiMatrix);
			FWBW fwbw(phiMatrix);
			_cachedQMatrix = fwbw.GetQMatrix();
            //fwbw.PrintQMatrix();
		}

		auto positions = x.Raw().find(k);
		if(positions != x.Raw().end())
		{
			auto position = positions->second->begin();
			for(; position != positions->second->end(); position++)
			{
				int j = position->j;
				int s1 = position->s1;
				int s2 = position->s2;
				if(0 == j && -1 == s1)
				{
					s1 = 0;
				}
				if((0 == j && y.Tags()[j] == s2 ) ||
				   (0 != j && y.Tags()[j-1] == s1 && y.Tags()[j] == s2))
				{
					// assume feature value is 1.0 to avoid hash lookup.
					// actually it should be res1 += x.GetFeatureValue(j, s1, s2, k);
					// for the following res2, it is the same.
					res1 += 1.0;
				}
				res2 += ((*_cachedQMatrix)[j][s1][s2] * 1.0);
			}
		}
        //LOG_DEBUG("emperical:%f expeted:%f", res1, res2);
		return 0 - (res1 - res2);
	};
    _derivative = derivative;
}

void LCCRF::_MakeLikelihood()
{
	function<double (const XSampleType&, const YSampleType&, vector<double>&, double)> likelihood = 
		[&]( const XSampleType& x, const YSampleType& y, vector<double>& weights, double scale)
	{
		int labelCount = _labelCount;
		double res1 = 0.0; // linear
		double res2 = 0.0; // linear

		FWBW::Matrix3 phiMaxtrix(y.Length(), vector<vector<double>>(labelCount, vector<double>(labelCount, 0.0)));
		LCCRF::_MakePhiMatrix(x, weights, scale, phiMaxtrix);

		// calculate res1
		for(int j = 0; j < y.Length(); j++)
		{
			int y1 = 0;
			if(0 == j){y1 = 0;}
			else {y1 = y.Tags()[j - 1];};
			res1 += phiMaxtrix[j][y1][y.Tags()[j]];
		}

		// forward-backword calculation for res2.
		FWBW fwbw(phiMaxtrix);
		res2 += fwbw.GetZ(); // linear

		return 0 - (res1 - res2);
	};
	_likelihood = likelihood;
}

void LCCRF::Fit(const vector<XSampleType>& xs, 
				const vector<YSampleType>& ys, 
				int maxIteration, 
				double learningRate, 
				double l2)
{
	_xs = &xs;
	_ys = &ys;

	// reset _weights.
	vector<double> newWeights(_featureCount, 0.0);
	_weights.swap(newWeights);

	// 1. make deriveatives
	_MakeDervative();
	// 2. make likilihood
	_MakeLikelihood();
	// 3. SGD
	SGD sgd(*_xs, *_ys, _weights, _derivative, _likelihood);
	sgd.Run(learningRate, l2, maxIteration);
}

void LCCRF::Fit(XType& xs, YType& ys, int maxIteration, double learningRate, double l2)
{
	Fit(xs.Raw(), ys.Raw(), maxIteration, learningRate, l2);
}

void LCCRF::Predict(const XSampleType& x, YSampleType& y)
{
	Viterbi::Matrix3 graph(x.Length(), vector<vector<double>>(_labelCount, vector<double>(_labelCount, 0.0)));
	LCCRF::_MakePhiMatrix(x, _weights, 1.0, graph);
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
