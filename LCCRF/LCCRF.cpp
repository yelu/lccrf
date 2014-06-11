#include "LCCRF.h"
#include "FWBW.h"

LCCRF::LCCRF(FeatureManager& featureManager, double lambda = 1):_features(featureManager),_weights(featureManager.Size(), 0.0)
{
	_lambda = lambda;
	_pTraningSet = NULL;
}

LCCRF::~LCCRF(void)
{
}

double LCCRF::Phi(wstring s1, wstring s2, int j,
				  const Document& doc, 
				  vector<double>& weights,
				  FeatureManager& features)
{
	size_t n = features.Size();
	double ret = 0.0;
	set<int> hitFeatures;
	features.Transform(doc, s1, s2, j, hitFeatures);
	for(auto ite = hitFeatures.begin(); ite != hitFeatures.end(); ite++)
	{
		ret += (weights[*ite]);
	}
	return ret;
}

void LCCRF::MakeDervative()
{
	function<double (vector<double>&, Document&, int)> derivative = [&](vector<double>& weights, Document& doc, int k)
	{
		int labelCount = _yIDAllocator.Size();
		double res1 = 0.0; // linear
		double res2 = 0.0; // linear

		// forward-backword calculation.
		function<double (int, int, int)> phi = [&](int s1, int s2, int j) 
		{ return Phi(_yIDAllocator.GetText(s1), _yIDAllocator.GetText(s2), j, doc, weights, _features); };
		FWBW fwbw(phi, labelCount, doc.size());
		auto QMatrix = fwbw.GetQMatrix();
		//fwbw.PrintQMatrix();
		for(size_t j = 0; j < doc.size(); j++)
		{
			// If j is the first token, then j-1 is not avaliable, use -1 instead.
			int y1 = -1;
			if(0 == j){y1 = -1;}
			else {y1 = doc[j - 1].yID;}
			res1 += _features.IsHit(doc, _yIDAllocator.GetText(y1), _yIDAllocator.GetText(doc[j].yID), j, k);
			for(int y1 = 0; y1 < labelCount; y1++)
			{
				for(int y2 = 0; y2 < labelCount; y2++)
				{
					double q = QMatrix[j][y1][y2];
					double featureHit = _features.IsHit(doc, _yIDAllocator.GetText(y1), _yIDAllocator.GetText(y2), j, k);
					res2 += (q * featureHit);
				}
			}

		}
		return 0 - (res1 - res2 - _lambda * weights[k]);
	};
	for(size_t k = 0; k < _features.Size(); k++)
	{
		function<double (vector<double>&, Document&)> derivativeK = [=](vector<double>& weights, Document& doc) {return derivative(weights, doc, k);};
		_derivatives.push_back(derivativeK);
	}
}

void LCCRF::MakeLikelihood()
{
	function<double (vector<double>&, Document&)> likelihood= [&](vector<double>& weights, Document& doc)
	{
		int labelCount = _yIDAllocator.Size();
		double res1 = 0.0; // linear
		double res2 = 0.0; // linear

		// calculate res1
		for(size_t j = 0; j < doc.size(); j++)
		{
			int y1 = -1;
			if(0 == j){y1 = -1;}
			else {y1 = doc[j - 1].yID;};
			set<int> hitFeatures;
			_features.Transform(doc, _yIDAllocator.GetText(y1), _yIDAllocator.GetText(doc[j].yID), j, hitFeatures);
			for(auto ite = hitFeatures.begin(); ite != hitFeatures.end(); ite++)
			{			
				res1 += (weights[*ite]);
			}
		}

		// forward-backword calculation for res2.
		function<double (int, int, int)> phi = [&](int s1, int s2, int j) 
		{ return Phi(_yIDAllocator.GetText(s1), _yIDAllocator.GetText(s2), j, doc, weights, _features); };
		FWBW fwbw(phi, labelCount, doc.size());
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

void LCCRF::AllocateIDForY()
{
	for(auto ite = _pTraningSet->begin(); ite != _pTraningSet->end(); ite++)
	{
		for(auto token = ite->begin(); token != ite->end(); token++)
		{
			int id = _yIDAllocator.GetOrAllocateID(token->y);
			token->yID = id;
		}
	}
}

void LCCRF::Learn(list<Document>& traningSet, double learningRate, int batch, int maxIteration)
{
	_pTraningSet = &traningSet;
	AllocateIDForY();
	// 1. make deriveatives
	MakeDervative();
	// 2. make likilihood
	MakeLikelihood();
	// 3. SGD
	SGD<Document> sgd(traningSet, _weights, _derivatives, _likelihood);
	sgd.Run(learningRate, batch, maxIteration);
}
