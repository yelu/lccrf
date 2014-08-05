#include "SGD.h"

void SGD::MakePhiMatrix(const XSampleType& xSample, 
						   vector<double>& weights, 
						   double scale, 
						   MultiArray<double, 3>& phiMatrix)
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
			phiMatrix(j, s1, s2) += (scale * weights[featureID]);
		}
	}
}

double SGD::_CalculateLikelihood(const XSampleType& x, 
							  const YSampleType& y, 
							  const MultiArray<double, 3>& phiMatrix,
							  double z)
{
	// calculate res1
	double res1 = 0.0; // log
	for(int j = 0; j < y.Length(); j++)
	{
		int y1 = 0;
		if(0 == j){y1 = 0;}
		else {y1 = y.Tags()[j - 1];};
		res1 += (phiMatrix(j, y1, y.Tags()[j]));
	}

	// res2 is the norm.
	double res2 = z; // log

	return 0 - (res1 - res2);
}

double SGD::_CaculateGradient(const XSampleType& x, 
		                      const YSampleType& y, 
							  int k,
							  const MultiArray<double, 3>& qMatrix)
{
	double res1 = 0.0; // linear
	double res2 = 0.0; // linear

	// forward-backword calculation.
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
			res2 += (qMatrix(j, s1, s2) * 1.0);
		}
	}
	//LOG_DEBUG("emperical:%f expeted:%f", res1, res2);
	return 0 - (res1 - res2);
}

const vector<double>& SGD::Run(double learningRate, double l2, int maxIteration)
{
	double lastObjective = std::numeric_limits<double>::infinity();
	_scale = 1.0;  // scalar update factor.
    _learningRate = learningRate;
    _l2 = l2;
    for(int i = 0; i < maxIteration; i++)
	{
		double sumLogLikelihood = 0.0;
        _iterationCount++;
		auto xIte = _xs.begin();
		auto yIte = _ys.begin();
		// calculate gradients and log-likelihood for every instance.
		for(; xIte != _xs.end() && yIte != _ys.end(); xIte++, yIte++)
		{
			sumLogLikelihood += UpdateWeights(*xIte, *yIte);
		}         
		// one iteration(epoch) finished, check it converged.
		double objective = sumLogLikelihood;
		for(auto w = _weights.begin(); w != _weights.end(); w++)
		{
			objective += (0.5 * _l2 * (*w) * (*w) * _scale * _scale);
		}
		double improvementRatio = (lastObjective - objective) / objective;
		LOG_DEBUG("Iteration:%d, loss:%f, improvement ratio:%f", i, objective, improvementRatio);
		if(std::abs(improvementRatio) < 1e-6) 
		{
			LOG_DEBUG("Converged.");
			break;
		}
		lastObjective = objective;
	}
	// final rescale to get final weights.
	Rescale();
    LOG_DEBUG("total iterations = %d\n", _iterationCount);
	return _weights;
}

void SGD::Rescale()
{
	for(auto i = _weights.begin(); i != _weights.end(); i++)
    {
		(*i) = (*i) * _scale;
    }
	_scale = 1.0;
}

double SGD::UpdateWeights(const XSampleType& xSample, const YSampleType& ySample)
{
    // rescale to void dense update of weights resulted by l2 regularition.
	if(std::abs(_scale) > 1e6)
    {
        Rescale();
    }
	double oldScale = _scale;
	_scale *= (1 - _learningRate * _l2);
	double gain = _learningRate / _scale;
        
    // to save weights which has been changed.
    std::list<std::pair<int, double>> changedWeights;
    // skip updating the i-th feature if the feature is not triggered in xSample. 
	// Since the derivative will be zero.
	auto featureSet = xSample.Raw();
	// For every x sample, forward-nackward can be reused to save time.
	MultiArray<double, 3> phiMatrix(ySample.Length(), _labelCount, _labelCount, 0.0);
	SGD::MakePhiMatrix(xSample, _weights, _scale, phiMatrix);
	FWBW fwbw(phiMatrix);
	const MultiArray<double, 3>& qMatrix = fwbw.GetQMatrix();
	//fwbw.PrintQMatrix();
    for(auto f = featureSet.begin(); f != featureSet.end(); f++)
    {
		double delta = _CaculateGradient(xSample, ySample, f->first, qMatrix);
		changedWeights.push_back(std::pair<int, double>(f->first, 
				                    _weights[f->first] -  gain * delta));
    }
    // update changed weights to _weights.
    for(auto ite = changedWeights.begin(); ite != changedWeights.end(); ite++)
    {
        _weights[ite->first] = ite->second;
    }

	// calculate log-likelihood.
	double likelihood = _CalculateLikelihood(xSample, ySample, phiMatrix, fwbw.GetZ());
	return likelihood;
}