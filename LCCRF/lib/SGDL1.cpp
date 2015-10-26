#include "SGDL1.h"

const vector<double>& SGDL1::Run(double learningRate, double l1, int maxIteration)
{
    double lastObjective = std::numeric_limits<double>::infinity();
    _learningRate = learningRate;
    _l1 = l1;
    //
    vector<double> qs(_weights.size(), 0.0);
    double u = 0.0;
    for (int i = 0; i < maxIteration; i++)
    {
        u += _learningRate * _l1;
        double sumLogLikelihood = 0.0;
        _iterationCount++;
        auto xIte = _xs.begin();
        auto yIte = _ys.begin();
        // calculate gradients and log-likelihood for every training sample(aka, mini-batch=1).
        for (; xIte != _xs.end() && yIte != _ys.end(); xIte++, yIte++)
        {
            sumLogLikelihood += UpdateWeights(*xIte, *yIte, qs, u);
        }
        double objective = sumLogLikelihood;
        for (auto w = _weights.begin(); w != _weights.end(); w++)
        {
            objective -= (_l1 * std::abs(*w));
        }

        // one iteration(epoch) finished, check it converged.
        double improvementRatio = (objective - lastObjective) / std::abs(objective);
        LOG("Iteration:%d, loss:%f, improvement ratio:%f", i, objective, improvementRatio);
        if (std::abs(improvementRatio) < 1e-3)
        {
            LOG("Converged.");
            break;
        }
        lastObjective = objective;
    }

    LOG("total iterations = %d\n", _iterationCount);
    return _weights;
}

double SGDL1::UpdateWeights(const X& x, const Y& y, vector<double>& qs, double u)
{
    // save weights which has been changed.
    std::list<std::pair<int, double>> changedWeights;
    // skip updating the i-th feature if the feature is not triggered in x. 
    // Since the derivative will be zero.
    const X::FeaturesContainer& featureSet = x.Raw();
    // For every x sample, forward-backward can be reused to save time.
    FWBW fwbw(x, _weights, _labelCount);

    for (auto f = featureSet.begin(); f != featureSet.end(); f++)
    {
        int fid = f->first;
        double delta = fwbw.CalcGradient(x, y, fid);
        double newWeight = _weights[fid] + _learningRate * delta;
        double newWeightWithPenalty = newWeight;
        if (newWeight > 0)
        {
            newWeightWithPenalty = std::fmax(0.0, newWeight - u - qs[fid]);
        }
        else
        {
            newWeightWithPenalty = std::fmin(0.0, newWeight + u - qs[fid]);
        }
        qs[fid] += (newWeightWithPenalty - newWeight);
        changedWeights.push_back(std::pair<int, double>(fid, newWeightWithPenalty));
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
