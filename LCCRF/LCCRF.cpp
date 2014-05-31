#include "LCCRF.h"
#include "FWBW.h"

LCCRF::LCCRF(vector<function <int (const Document&, int, int, int)>>& features, double learningRate = 0.01):_weights(features.size(), 0.0)
{
	_features = features;
	_learningRate = learningRate;
}

LCCRF::~LCCRF(void)
{
}

double LCCRF::Phi(int s1, int s2, int j,
				  const Document& doc, 
				  vector<double> weights,
		          vector<function <int (const Document&, int, int, int)>> features)
{
	size_t n = features.size();
	double ret = 0.0;
	for(size_t i = 0; i < n; i++)
	{
		ret += (weights[i] * features[i](doc, s1, s2, j));
	}
	return ret;
}

void LCCRF::MakeDervative()
{
	int labelCount = _yIDAllocator.Size();
	function<double (vector<double>&, Document&, int)> derivative = [&](vector<double>& weights, Document& doc, int k)
	{
		double res1 = 0.0; // linear
		double res2 = 0.0; // linear

		// forward-backword calculation.
		function<double (int, int, int)> phi = [&](int s1, int s2, int j) 
		{ return Phi(s1, s2, j, doc, weights, _features); };
		FWBW fwbw(phi, labelCount, doc.size());
		auto QMatrix = fwbw.GetQMatrix();
		for(size_t j = 0; j < doc.size(); j++)
		{
			// If j is the first token, then j-1 is not avaliable, use -1 instead.
			int y1 = -1;
			if(0 == j){y1 = -1;}
			else {y1 = doc[j - 1].y;}
			res1 += _features[k](doc, y1, doc[j].y, j);
			for(int y1 = 0; y1 < labelCount; y1++)
			{
				for(int y2 = 0; y2 < labelCount; y2++)
				{
					res2 += (QMatrix[j][y1][y2] * _features[k](doc, y1, y2, j));
				}
			}

		}
		const double lambda = _learningRate;
		return res1 - res2 - lambda * weights[k];
	};
	for(size_t k = 0; k < _features.size(); k++)
	{
		function<double (vector<double>&, Document&)> derivativeK = [&](vector<double>& weights, Document& doc) {return derivative(weights, doc, k);};
		_derivatives.push_back(derivativeK);
	}
}

void LCCRF::MakeLikelihood()
{
	int labelCount = _yIDAllocator.Size();
	const double lambda = 0.2;
	function<double (vector<double>&, Document&)> likelihood= [&](vector<double>& weights, Document& doc)
	{
		double res1 = 0.0; // linear
		double res2 = 0.0; // linear

		// calculate res1
		for(size_t j = 0; j < doc.size(); j++)
		{
			for(size_t h = 0; h < _features.size(); h++)
			{
				int y1 = -1;
				if(0 == j){y1 = -1;}
				else {y1 = doc[j - 1].y;};
				res1 += (weights[h] * _features[h](doc, y1, doc[j].y, j));
			}
		}

		// forward-backword calculation for res2.
		function<double (int, int, int)> phi = [&](int s1, int s2, int j) 
		{ return Phi(s1, s2, j, doc, weights, _features); };
		FWBW fwbw(phi, labelCount, doc.size());
		res2 += fwbw.GetZ(); // linear

		double res3 = 0.0;
		for(size_t w = 0; w < _features.size(); w++)
		{
			res3 += (weights[w] * weights[w]);
		}
		return res1 - res2 - res3 * _learningRate / 2.0;
	};
	_likelihood = likelihood;
}

void LCCRF::Learn(list<Document>& traningSet, int maxIteration)
{
	// 1. make deriveatives
	MakeDervative();
	// 2. make likilihood
	MakeLikelihood();
	// 3. SGD
	SGD<Document> sgd(traningSet, _weights, _derivatives, _likelihood);
	sgd.Run(_learningRate, 1, maxIteration);
}
