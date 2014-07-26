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

/*
   j  : current position
   s1 : previous state
   s2 : current state
*/
double LCCRF::_Phi(int s1, int s2, int j,
				  const XSampleType& doc, 
				  vector<double>& weights,
                  list<pair<int, double>>* hitFeatures, double scale)
{
	double ret = 0.0;
	shared_ptr<std::set<int>> pFeatures = doc.GetFeatures(j, s1, s2);
	if(!pFeatures)
	{
		return ret;
	}
	for(auto ite = (*pFeatures).begin(); ite != (*pFeatures).end(); ite++)
	{
		ret += (scale * weights[*ite]);
        if(NULL != hitFeatures)
        {
            hitFeatures->push_back(pair<int, double>(*ite, scale * weights[*ite]));
        }
	}
	return ret;
}

void LCCRF::_MakePhiMatrix(const XSampleType& xSample, 
						   vector<double>& weights, 
						   double scale, 
						   FWBW::Matrix3& phiMatrix)
{
	for(XSampleType::FeaturesContainer::const_iterator ite = xSample.Raw().begin();
		ite != xSample.Raw().end(); ite++)
	{
		int j = ite->first.j;
		int s1 = ite->first.s1;
		int s2 = ite->first.s2;
		auto features = ite->second;
		if(0 == j && -1 == s1)
		{
			s1 = 0;
		}
		double phi = 0.0;
		if(features)
		{
			for(auto feature = features->begin(); feature != features->end(); feature++)
			{
				phi += (scale * weights[*feature]);
			}
		}
		phiMatrix[j][s1][s2] = phi;
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
		
		for(int j = 0; j < y.Length(); j++)
		{
			// If j is the first token, then j-1 is not avaliable, use -1 instead.
			int y1 = -1;
			if(0 == j){y1 = -1;}
			else {y1 = y.Tags()[j - 1];}
			res1 += x.GetFeatureValue(j, y1, y.Tags()[j], k);
			for(int y1 = 0; y1 < labelCount; y1++)
			{
				for(int y2 = 0; y2 < labelCount; y2++)
				{
					double q = (*_cachedQMatrix)[j][y1][y2];
					int y = y1;
					if(0 == j)
					{
						y = -1;
					}
					double featureHit = x.GetFeatureValue(j, y, y2, k);
					res2 += (q * featureHit);
				}
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
    for(int j = 0; j < (int)x.Length(); j++)
	{
		for(int s2 = 0; s2 < (int)_labelCount; s2++)
		{
			for(int s1 = 0; s1 < (int)_labelCount; s1++)
			{
				if(0 == j)
				{
					if(s1 != 0)
					{
						// fill graph[0][1..._sCount-1][0..._sCount] with 0.
						graph[j][s1][s2] = 0.0;
					}
					else
					{
						graph[j][s1][s2] = _Phi(-1, s2, j, x, _weights);
					}
				}
				else
				{
					graph[j][s1][s2] = _Phi(s1, s2, j, x, _weights);
				}
			}
		}
	}
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

pair<list<list<pair<int, double>>>, double> LCCRF::Debug(const XSampleType& doc, const YSampleType& y)
{
	int preState = -1;
	double score = 0.0;
	pair<list<list<pair<int, double>>>, double> res;
    for(int j = 0; j < doc.Length(); j++)
	{
		if(y.Tags()[j] >= _labelCount)
		{
			return res;
		}
        res.first.push_back(list<pair<int, double>>());
        score += _Phi(preState, y.Tags()[j], j, doc, _weights, &(res.first.back()));
		preState = y.Tags()[j];
	}
    res.second = score;
    return res;
}
