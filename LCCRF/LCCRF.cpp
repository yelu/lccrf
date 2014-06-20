#include "LCCRF.h"
#include "FWBW.h"
#include "Viterbi.h"
using std::wcout;

LCCRF::LCCRF(int featureCount, int labelCount, double lambda = 1):_weights(featureCount, 0.0)
{
	_lambda = lambda;
	_featureCount = featureCount;
	_labelCount = labelCount;
}

LCCRF::~LCCRF(void)
{
}

double LCCRF::_Phi(int s1, int s2, int j,
				  const X& doc, 
				  vector<double>& weights)
{
	size_t n = weights.size();
	double ret = 0.0;
	shared_ptr<std::set<int>> pFeatures = doc.GetFeatures(j, s1, s2);
	for(auto ite = (*pFeatures).begin(); ite != (*pFeatures).end(); ite++)
	{
		ret += (weights[*ite]);
	}
	return ret;
}

void LCCRF::_MakeDervative()
{
	function<double (vector<double>&, const X&, const Y&, int)> derivative = [&](vector<double>& weights, const X& x, const Y& y, int k)
	{
		int labelCount = _labelCount;
		double res1 = 0.0; // linear
		double res2 = 0.0; // linear

		// forward-backword calculation.
		function<double (int, int, int)> phi = [&](int s1, int s2, int j) 
		{ return _Phi(s1, s2, j, x, weights); };
		FWBW fwbw(phi, labelCount, x.Length());
		auto QMatrix = fwbw.GetQMatrix();
		//fwbw.PrintQMatrix();
		for(size_t j = 0; j < y.Length(); j++)
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
					double q = QMatrix[j][y1][y2];
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
		return 0 - (res1 - res2 - _lambda * weights[k]);
	};
	for(int k = 0; k < _featureCount; k++)
	{
		function<double (vector<double>&, const X&, const Y&)> derivativeK = [=](vector<double>& weights, const X& x, const Y& y) {return derivative(weights, x, y, k);};
		_derivatives.push_back(derivativeK);
	}
}

void LCCRF::_MakeLikelihood()
{
	function<double (vector<double>&, const X&, const Y&)> likelihood= [&](vector<double>& weights, const X& x, const Y& y)
	{
		int labelCount = _labelCount;
		double res1 = 0.0; // linear
		double res2 = 0.0; // linear

		// calculate res1
		for(size_t j = 0; j < y.Length(); j++)
		{
			int y1 = -1;
			if(0 == j){y1 = -1;}
			else {y1 = y.Tags()[j - 1];};
			shared_ptr<std::set<int>> pFeatures = x.GetFeatures(j, y1, y.Tags()[j]);
			for(auto ite = (*pFeatures).begin(); ite != (*pFeatures).end(); ite++)
			{
				res1 += (weights[*ite]);
			}
		}

		// forward-backword calculation for res2.
		function<double (int, int, int)> phi = [&](int s1, int s2, int j) 
		{ return _Phi(s1, s2, j, x, weights); };
		FWBW fwbw(phi, labelCount, x.Length());
		res2 += fwbw.GetZ(); // linear

		double res3 = 0.0;
		for(size_t w = 0; w < weights.size(); w++)
		{
			res3 += (weights[w] * weights[w]);
		}
		return 0 - (res1 - res2 - res3 * _lambda / 2.0);
	};
	_likelihood = likelihood;
}

void LCCRF::Fit(const ListX& xs, const ListY& ys, double learningRate, int batch, int maxIteration)
{
	_derivatives.clear();
	_xs = &xs;
	_ys = &ys;

	// reset _weights.
	vector<double> newWeights(_featureCount, 1.0);
	_weights.swap(newWeights);

	// 1. make deriveatives
	_MakeDervative();
	// 2. make likilihood
	_MakeLikelihood();
	// 3. SGD
	SGD<X, Y> sgd(_xs->GetX(), _ys->GetY(), _weights, _derivatives, _likelihood);
	sgd.Run(learningRate, batch, maxIteration);
}

void LCCRF::Predict(const X& doc, Y& tags)
{
	boost::multi_array<double, 3> graph(boost::extents[doc.Length()][_labelCount][_labelCount]);
	for(int j = 0; j < (int)doc.Length(); j++)
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
						graph[j][s1][s2] = _Phi(-1, s2, j, doc, _weights);
					}
				}
				else
				{
					graph[j][s1][s2] = _Phi(s1, s2, j, doc, _weights);
				}
			}
		}
	}
	vector<int> path(doc.Length(), -1);
	Viterbi::GetPath(graph, path);
	tags.Clear();
	for(auto ite = path.begin(); ite != path.end(); ite++)
	{
		tags.AppendTag(*ite);
	}
}

void LCCRF::Debug(const X& doc, const Y& y)
{
	int preState = -1;
	double score = 0.0;
	wcout << "Path : ";
	for(int j = 0; j < _labelCount; j++)
	{
		wcout << y.Tags()[j];
		if(y.Tags()[j] >= _labelCount)
		{
			wcout << L"(bad path)" << std::endl;
			return;
		}
		score += _Phi(preState, y.Tags()[j], j, doc, _weights);
		preState = y.Tags()[j];
		wcout << L" -> ";
	}
	wcout << " score : " << score << std::endl;
}
