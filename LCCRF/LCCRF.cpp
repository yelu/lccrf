#include "LCCRF.h"
#include "FWBW.h"

LCCRF::LCCRF(void)
{
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

void LCCRF::MakeDervative(list<Document>& trainingSet, int k)
{
	int labelCount = _yIDAllocator.Size();
	function<double (vector<double>, int)> derivative = [&](vector<double> weights, int k)
	{
		double res1 = 0.0; // linear
		double res2 = 0.0; // linear
		for(auto ite = trainingSet.begin(); ite != trainingSet.end(); ite++)
		{
			// forward-backword calculation.
			function<double (int, int, int)> phi = [&](int s1, int s2, int j) 
			{ return Phi(s1, s2, j, *ite, weights, _features); };
			FWBW fwbw(phi, labelCount, ite->size());
			auto QMatrix = fwbw.GetQMatrix();
			for(size_t j = 0; j < ite->size(); j++)
			{
				int y1 = -1;
				if(0 == j){y1 = -1;}
				else {y1 = (*ite)[j - 1].y;}
				res1 += _features[k](*ite, y1, (*ite)[j].y, j);
				for(int y1 = 0; y1 < labelCount; y1++)
				{
					for(int y2 = 0; y2 < labelCount; y2++)
					{
						res2 += (QMatrix[j][y1][y2] * _features[k](*ite, y1, y2, j));
					}
				}
			}
		}
		const double lambda = 0.2;
		return res1 - res2 - lambda * weights[k];
	};
	for(size_t i = 0; i < _features.size(); i++)
	{
		function<double (vector<double>)> derivativeI = [&](vector<double> weights) {return derivative(weights, i);};
		_derivatives.push_back(derivativeI);
	}
}

void LCCRF::MakeLikelihood(list<Document>& trainingSet)
{
	int labelCount = _yIDAllocator.Size();
	const double lambda = 0.2;
	function<double (vector<double>)> likelihood = [&](vector<double> weights)
	{
		double res1 = 0.0; // linear
		double res2 = 0.0; // linear
		for(auto ite = trainingSet.begin(); ite != trainingSet.end(); ite++)
		{
			// calculate res1
			for(size_t j = 0; j < ite->size(); j++)
			{
				for(size_t h = 0; h < _features.size(); h++)
				{
					int y1 = -1;
					if(0 == j){y1 = -1;}
					else {y1 = (*ite)[j - 1].y;};
					res1 += (weights[h] * _features[h](*ite, y1, (*ite)[j].y, j));
				}
			}

			// forward-backword calculation for res2.
			function<double (int, int, int)> phi = [&](int s1, int s2, int j) 
			{ return Phi(s1, s2, j, *ite, weights, _features); };
			FWBW fwbw(phi, labelCount, ite->size());
			res2 += fwbw.GetZ(); // linear
		}
		double res3 = 0.0;
		for(size_t w = 0; w < _features.size(); w++)
		{
			res3 += (weights[w] * weights[w]);
		}
		return res1 - res2 -res3;
	};
	_likelihood = likelihood;
}
