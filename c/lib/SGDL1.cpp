#include "SGDL1.h"

const vector<double>& SGDL1::Run(double learningRate, double l1, int maxIteration)
{
    double lastObjective = std::numeric_limits<double>::infinity();
    _learningRate = learningRate;
    _l1 = l1;

    vector<double> qs(_weights.size(), 0.0);
    double u = 0.0;
    for (int i = 0; i < maxIteration; i++)
    {
        u += _learningRate * _l1;
        double sumLogLikelihood = 0.0;
        _iterationCount++;
        auto xIte = _xs.begin();
        auto yIte = _ys.begin();
        int cnt = 0;
        // calculate gradients and log-likelihood for every training sample(aka, mini-batch=1).
        for (; xIte != _xs.end() && yIte != _ys.end(); xIte++, yIte++)
        {
            sumLogLikelihood += UpdateWeights(*xIte, *yIte, qs, u);
            cnt += 1;
            if(cnt % 1000 == 0)
            {
                LOG("%d training samples processed", cnt);
            }
        }
		LOG("%d training samples in total", cnt);
        double objective = sumLogLikelihood;
        for (auto w = _weights.begin(); w != _weights.end(); w++)
        {
            objective -= (_l1 * std::abs(*w));
        }

        // one iteration(epoch) finished, check it converged.
        double improvementRatio = (objective - lastObjective) / std::abs(objective);
        LOG("Iteration:%d, loss:%f, improve by:%f", i, objective, improvementRatio);
        if (std::abs(improvementRatio) < 1e-3)
        {
            LOG("Converged.");
            break;
        }
        lastObjective = objective;
    }

    LOG("Total iterations = %d", _iterationCount);
    return _weights;
}

double SGDL1::UpdateWeights(const X& x, const Y& y, vector<double>& qs, double u)
{
    // save weights which has been changed.
    std::list<std::pair<int, double>> changedWeights;

    // for every x sample, forward-backward can be reused to save time.
    FWBW fwbw(x, y, _lccrfFeatures, _weights);

	// Updating features triggered on x. 
	// 1. unigram feature.
    for (auto f = x.Features.begin(); f != x.Features.end(); f++)
    {
		auto labelAndId = _lccrfFeatures.UnigramFeatures.at(f->first);
		for (auto fidIte = labelAndId->begin(); fidIte != labelAndId->end(); fidIte++)
		{
			LCCRFFeatures::UnigramFeature feature(f->first, fidIte->first);
			int fId = fidIte->second;
			double delta = fwbw.CalcGradient(x, y, feature);
			double newWeight = _weights[fId] + _learningRate * delta;
			double newWeightWithPenalty = newWeight;
			if (newWeight > 0)
			{
				newWeightWithPenalty = std::fmax(0.0, newWeight - u - qs[fId]);
			}
			else
			{
				newWeightWithPenalty = std::fmin(0.0, newWeight + u - qs[fId]);
			}
			qs[fId] += (newWeightWithPenalty - newWeight);
			changedWeights.push_back(std::pair<int, double>(fId, newWeightWithPenalty));
		}
        
    }

	// transition feature.
	for (auto f = _lccrfFeatures.TransitionFeatures.begin(); f != _lccrfFeatures.TransitionFeatures.end(); f++)
	{
		int fId = f->second;
		double delta = fwbw.CalcGradient(x, y, f->first);
		double newWeight = _weights[fId] + _learningRate * delta;
		double newWeightWithPenalty = newWeight;
		if (newWeight > 0)
		{
			newWeightWithPenalty = std::fmax(0.0, newWeight - u - qs[fId]);
		}
		else
		{
			newWeightWithPenalty = std::fmin(0.0, newWeight + u - qs[fId]);
		}
		qs[fId] += (newWeightWithPenalty - newWeight);
		changedWeights.push_back(std::pair<int, double>(fId, newWeightWithPenalty));
	}

    // update changed weights to _weights.
    for (auto ite = changedWeights.begin(); ite != changedWeights.end(); ite++)
    {
        _weights[ite->first] = ite->second;
    }

    // calculate log-likelihood.
    double likelihood = fwbw.CalcLikelihood(x, y);
    return likelihood;
}
