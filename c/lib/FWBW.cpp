#include "FWBW.h"
#include "Log.h"
#include <cassert>
#include <limits>

FWBW::FWBW(const X& x,
		   const Y& y,
		   const LCCRFFeatures& lccrfFeatures,
           const vector<double>& weights) :
	        _lccrfFeatures(lccrfFeatures),
			_length(y.Tags.size()),
			_labelCount(_lccrfFeatures.LabelCount()),
			_edges(_length, _labelCount, _labelCount, 0.0),
			_nodes(_length, _labelCount, 0.0),
			_alphaMatrix(_length, _labelCount),
			_betaMatrix(_length, _labelCount),
			_alphaScales(_length),
			_betaScales(_length),
			_div(_length)
{
    _logNorm = std::numeric_limits<double>::lowest();
    MakeEdgesAndNodes(x, y, _lccrfFeatures, weights, _edges, _nodes);
    _edges.ExpInPlace();
    _nodes.ExpInPlace();
    _CalculateAlphaMatrix(_alphaMatrix, _alphaScales);
    _CalculateBetaMatrix(_betaMatrix, _betaScales);
    VectorDivide(_alphaScales, _betaScales, _div);

    // calculate log norm.
    _logNorm = 0.0;
    for (int i = 0; i < _alphaScales.Dim1(); i++)
    {
        _logNorm += std::log(_alphaScales[i]);
    }
}

FWBW::~FWBW(void)
{
}

void FWBW::_CalculateAlphaMatrix(MultiArray<double, 2>& alphaMatrix, MultiArray<double, 1, 10240>& scales)
{
    // for the first token, there will only be nodes.
    for(int s = 0; s < _labelCount; s++)
    {
        alphaMatrix(0, s) = _nodes(0, s);
    }
    scales[0] = alphaMatrix[0].NormalizeInPlace();
    for(int j = 1; j < _length; j++)
    {
        for(int s2 = 0; s2 < _labelCount; s2++)
        {
            alphaMatrix(j, s2) = 0.0;
            for(int s1 = 0; s1 < _labelCount; s1++)
            {
                alphaMatrix(j, s2) += (alphaMatrix(j - 1, s1) * _edges(j, s1, s2) * _nodes(j, s2));
            }
        }
        scales[j] = alphaMatrix[j].NormalizeInPlace();
    }
}

void FWBW::_CalculateBetaMatrix(MultiArray<double, 2>& betaMatrix, MultiArray<double, 1, 10240>& scales)
{
    for(int s = 0; s < _labelCount; s++)
    {
        betaMatrix(_length - 1, s) = _nodes(_length - 1, s);    // linear
    }
    scales[_length - 1] = betaMatrix[_length - 1].NormalizeInPlace();
    
    for(int j = _length - 2; j >= 0; j--)
    {
        for(int s1 = 0; s1 < _labelCount; s1++)
        {
            betaMatrix(j, s1) = 0.0;
            for(int s2 = 0; s2 < _labelCount; s2++)
            {
                double preNode = betaMatrix(j + 1, s2);
                double edge = _edges(j + 1, s1, s2);
                betaMatrix(j, s1) += (preNode * edge * _nodes(j, s1));
            }
        }
        scales[j] = betaMatrix[j].NormalizeInPlace();
    }
}

void FWBW::MakeEdgesAndNodes(const X& x,
							 const Y& y,
							 const LCCRFFeatures& lccrfFeatures,
                             const vector<double>& weights,
                             MultiArray<double, 3>& edges,
                             MultiArray<double, 2>& nodes)
{
	// update nodes.
	for (auto xidIte = x.Features.begin(); xidIte != x.Features.end(); xidIte++)
	{
		auto labelAndId = lccrfFeatures.UnigramFeatures.at(xidIte->first);
		for (auto posIte = xidIte->second->begin(); posIte != xidIte->second->end(); posIte++)
		{
			for (auto labelIte = labelAndId->begin(); labelIte != labelAndId->end(); labelIte++)
			{			
				nodes(*posIte, labelIte->first) += weights[labelIte->second];
			}
		}	
	}

	// update edges.
	for (size_t i = 1; i < x.Length(); i++)
	{
		for (auto tranFeature = lccrfFeatures.TransitionFeatures.begin();
		tranFeature != lccrfFeatures.TransitionFeatures.end(); tranFeature++)
		{
			edges(i, tranFeature->first.s1, tranFeature->first.s2) += weights[tranFeature->second];
		}
	}

}

double FWBW::CalcNodesExpectation(const int& s2, const std::unordered_set<int>& positions)
{
    double res = 0.0;
    auto position = positions.begin();
    for(; position != positions.end(); position++)
    {
        int j = *position;
        double forward = _alphaMatrix(j, s2);
        double backward = _betaMatrix(j, s2);
        double probability = 0.0;
        probability = forward * backward * _div[j] * _betaScales[j];
        // _nodes[j, s2] has been included twice. divive out one.
        probability /= _nodes(j, s2);
        // assume feature value is 1.0 to avoid hash lookup.
        // actually it should be res1 += x.GetFeatureValue(j, s1, s2, k);
        res += (probability * 1.0);
    }
    return res;
}

double FWBW::CalcEdgesExpectation(const int& s1, const int& s2)
{
    double res = 0.0;
	for (int j = 1; j < _length; j++)
    {
        double forward = _alphaMatrix(j - 1, s1);
        double backword = _betaMatrix(j, s2);
        double probability = forward * _edges(j, s1, s2) * backword * _div[j - 1];
        res += (probability * 1.0);
    }
    return res;
}

double FWBW::CalcLikelihood(const X& x, const Y& y)
{
    double logOfTaggedPath = 0.0; // log
    for (int j = 0; j < y.Length(); j++)
    {
        if (j > 0)
        {
            logOfTaggedPath += std::log(_edges(j, y.Tags[j - 1], y.Tags[j]));
        }
        logOfTaggedPath += std::log(_nodes(j, y.Tags[j]));
    }

    return logOfTaggedPath - _logNorm;
}

double FWBW::CalcGradient(const X& x, const Y& y, const LCCRFFeatures::UnigramFeature& feature)
{
	double empirical = 0.0; // linear

	auto poses = x.Features.at(feature.id);
	for (auto posIte = poses->begin(); posIte != poses->end(); posIte++)
	{
		if (y.Tags[*posIte] == feature.s1) { empirical += 1.0; }
	}

    // get expected under model distribution.
	double expected = CalcNodesExpectation(feature.s1, *poses);

    //LOG_DEBUG("emperical:%f expeted:%f", res1, res2);
    return empirical - expected;
}

double FWBW::CalcGradient(const X& x, const Y& y, const LCCRFFeatures::TransitionFeature& feature)
{
	double empirical = 0.0; // linear
	for (size_t i = 1; i < y.Tags.size(); i++)
	{
		if (y.Tags[i - 1] == feature.s1 && y.Tags[i] == feature.s2)
		{
			empirical += 1.0;
		}
	}

	double expected = CalcEdgesExpectation(feature.s1, feature.s2);
	
	//LOG_DEBUG("emperical:%f expeted:%f", res1, res2);
	return empirical - expected;
}

double FWBW::GetLogNorm()
{
    return _logNorm;
}

void FWBW::VectorDivide(const MultiArray<double, 1, 10240>& v1, 
                        const MultiArray<double, 1, 10240>& v2, 
                        MultiArray<double, 1, 10240>& res)
{
    for(int i = 0; i < res.Dim1(); i++)
    {
        res[i] = std::log(v1[i]) - std::log(v2[i]);
    }
    for (int i = 1; i < res.Dim1(); i++)
    {
        res[i] += res[i - 1];
    }
    res.ExpInPlace();
    assert(std::abs(res[res.Dim1() - 1] - 1.0) < 1e-4);
}
