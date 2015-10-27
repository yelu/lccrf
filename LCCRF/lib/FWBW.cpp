#include "FWBW.h"
#include "Log.h"
#include <cassert>
#include <limits>

FWBW::FWBW(const X& x,
    const vector<double>& weights,
    int sCount) :
    _jCount(x.Length()),
    _sCount(sCount),
    _edges(_jCount, _sCount, _sCount, 0.0),
    _nodes(_jCount, _sCount, 0.0),
    _alphaMatrix(_jCount, _sCount),
    _betaMatrix(_jCount, _sCount),
    _alphaScales(_jCount),
    _betaScales(_jCount),
    _div(_jCount)
{
    _logNorm = std::numeric_limits<double>::lowest();
    MakeEdgesAndNodes(x, weights, _edges, _nodes);
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

void FWBW::_CalculateAlphaMatrix(MultiArray<double, 2>& alphaMatrix, MultiArray<double, 1, 100>& scales)
{
    // for the first token, there will only be nodes.
    for(int s = 0; s < _sCount; s++)
    {
        alphaMatrix(0, s) = _nodes(0, s);
    }
    scales[0] = alphaMatrix[0].NormalizeInPlace();
    for(int j = 1; j < _jCount; j++)
    {
        for(int s2 = 0; s2 < _sCount; s2++)
        {
            alphaMatrix(j, s2) = 0.0;
            for(int s1 = 0; s1 < _sCount; s1++)
            {
                alphaMatrix(j, s2) += (alphaMatrix(j - 1, s1) * _edges(j, s1, s2) * _nodes(j, s2));
            }
        }
        scales[j] = alphaMatrix[j].NormalizeInPlace();
    }
}

void FWBW::_CalculateBetaMatrix(MultiArray<double, 2>& betaMatrix, MultiArray<double, 1, 100>& scales)
{
    for(int s = 0; s < _sCount; s++)
    {
        betaMatrix(_jCount - 1, s) = _nodes(_jCount - 1, s);    // linear
    }
    scales[_jCount - 1] = betaMatrix[_jCount - 1].NormalizeInPlace();
    
    for(int j = _jCount - 2; j >= 0; j--)
    {
        for(int s1 = 0; s1 < _sCount; s1++)
        {
            betaMatrix(j, s1) = 0.0;
            for(int s2 = 0; s2 < _sCount; s2++)
            {
                double preNode = betaMatrix(j + 1, s2);
                double edge = _edges(j + 1, s1, s2);
                betaMatrix(j, s1) += (preNode * edge * _nodes(j, s1));
            }
        }
        scales[j] = betaMatrix[j].NormalizeInPlace();
    }
}

void FWBW::_UpdateEdgesAndNodes(const X::FeaturesContainer& features,
					      const X& x,
						  const vector<double>& weights,
						  MultiArray<double, 3>& edges,
	                      MultiArray<double, 2>& nodes)
{
	X::FeaturesContainer::const_iterator ite = features.begin();
	for (; ite != features.end(); ++ite)
	{
		const X::PositionSet& positions = _GetRightPositionSet(x, ite);

		int featureID = ite->first.id;
		X::PositionSet::const_iterator p = positions.begin();
		for (; p != positions.end(); ++p)
		{
			// if it is a bigram feature(on labels), update edges.
			if (ite->first.s1 >= 0)
			{
				edges(*p, ite->first.s1, ite->first.s2) += weights[featureID];
			}
			else
			{
				nodes(*p, ite->first.s2) += weights[featureID];
			}
		}
	}
}

const X::PositionSet& FWBW::_GetRightPositionSet(const X&x, const X::FeaturesContainer::const_iterator& ite)
{
	const X::PositionSet* positions = NULL;
	if (-1 == *(ite->second->begin()))
	{
		if (ite->first.s1 < 0)
		{
			positions = &x.allUnigramPositions;
		}
		else
		{
			positions = &x.allBigramPositions;
		}
	}
	else
	{
		positions = ite->second.get();
	}
	return *(positions);
}

void FWBW::MakeEdgesAndNodes(const X& x,
                             const vector<double>& weights,
                             MultiArray<double, 3>& edges,
                             MultiArray<double, 2>& nodes)
{
	_UpdateEdgesAndNodes(x.Raw(), x, weights, edges, nodes);

	_UpdateEdgesAndNodes(x.commonFeatures, x, weights, edges, nodes);

}

double FWBW::CalcNodesExpectation(const int& s2, const X::PositionSet& positions)
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

double FWBW::CalcEdgesExpectation(const int& s1, const int& s2, const X::PositionSet& positions)
{
    double res = 0.0;
    auto position = positions.begin();
    for (; position != positions.end(); position++)
    {
        int j = *position;   
        assert(j - 1 >= 0);
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
        if (0 != j && y.Tags()[j - 1] >= 0)
        {
            logOfTaggedPath += std::log(_edges(j, y.Tags()[j - 1], y.Tags()[j]));
        }
        logOfTaggedPath += std::log(_nodes(j, y.Tags()[j]));
    }

    return logOfTaggedPath - _logNorm;
}

double FWBW::CalcGradient(const X& x, const Y& y, int k)
{
	X::Feature feat(k, 0, 0);
    auto pair = x.Raw().find(feat);
	if (pair == x.Raw().end())
	{
		return 0.0;
	}

	double empirical = 0.0; // linear
	double expected = 0.0; // linear

    bool isBigram = true;
    if (pair->first.s1 < 0)
    {
        isBigram = false;
    }
	int s1 = pair->first.s1;
	int s2 = pair->first.s2;

	const X::PositionSet& positions = _GetRightPositionSet(x, pair);

	for (auto p = positions.begin(); p != positions.end(); ++p)
	{
		int j = *p;
		if ((!isBigram && y.Tags()[j] == s2) ||
			(isBigram && y.Tags()[j - 1] == s1 && y.Tags()[j] == s2))
		{
			empirical += 1.0;
		}
	}

    // get expected under model distribution.
    if (isBigram)
    {
		expected = CalcEdgesExpectation(s1, s2, positions);
    }
    else
    {
		expected = CalcNodesExpectation(s2, positions);
    }

    //LOG_DEBUG("emperical:%f expeted:%f", res1, res2);
    return empirical - expected;
}

double FWBW::GetLogNorm()
{
    return _logNorm;
}

void FWBW::VectorDivide(const MultiArray<double, 1, 100>& v1, 
                        const MultiArray<double, 1, 100>& v2, 
                        MultiArray<double, 1, 100>& res)
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
