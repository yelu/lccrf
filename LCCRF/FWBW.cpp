#include "FWBW.h"
#include "Log.h"
#include <cassert>
#include <limits>

FWBW::FWBW(const XSampleType& x,
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
	VectorDivide(_betaScales, _alphaScales, _div);

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
				alphaMatrix(j, s2) += (alphaMatrix(j-1, s1) * _edges(j, s1, s2) * _nodes(j, s2));
			}
		}
        scales[j] = alphaMatrix[j].NormalizeInPlace();
	}
}

void FWBW::_CalculateBetaMatrix(MultiArray<double, 2>& betaMatrix, MultiArray<double, 1, 100>& scales)
{
	for(int s = 0; s < _sCount; s++)
	{
		betaMatrix(_jCount - 1, s) = 1.0;	// linear
	}
    scales[_jCount - 1] = betaMatrix[_jCount - 1].NormalizeInPlace();
	for(int j = _jCount - 2; j >= 0; j--)
	{
		for(int s1 = 0; s1 < _sCount; s1++)
		{
			betaMatrix(j, s1) = 0.0;
			for(int s2 = 0; s2 < _sCount; s2++)
			{
				betaMatrix(j, s1) += (betaMatrix(j + 1, s2) * _edges(j + 1, s1, s2) * _nodes(j + 1, s1));
			}
		}
        scales[j] = betaMatrix[j].NormalizeInPlace();
	}
}

void FWBW::MakeEdgesAndNodes(const XSampleType& x,
							 const vector<double>& weights,
							 MultiArray<double, 3>& edges,
							 MultiArray<double, 2>& nodes)
{
	XSampleType::FeaturesContainer::const_iterator ite = x.Raw().begin();
	for (; ite != x.Raw().end(); ++ite)
	{
		const XSampleType::PositionSet& positions = *(ite->second);
		int featureID = ite->first;
		XSampleType::PositionSet::const_iterator position = positions.begin();
		for (; position != positions.end(); ++position)
		{
			const XSampleType::Position& p = *position;
			// if it is a bigram feature(on labels), update edges.
			if (p.s1 >= 0)
			{
				edges(p.j, p.s1, p.s2) += weights[featureID];
			}
			else
			{
				nodes(p.j, p.s2) += weights[featureID];
			}
		}
	}
}

double FWBW::CalcNodesExpectation(const XSampleType::PositionSet& positions)
{
	double res = 0.0;
	auto position = positions.begin();
	for(; position != positions.end(); position++)
	{
		int j = position->j;
		int s2 = position->s2;
		double probability = 0.0;
		probability = _alphaMatrix(j, s2) *
					  _betaMatrix(j, s2) *
					  _div[j] * _alphaScales[j];
		// assume feature value is 1.0 to avoid hash lookup.
		// actually it should be res1 += x.GetFeatureValue(j, s1, s2, k);
		res += (probability * 1.0);
	}
	return res;
}

double FWBW::CalcEdgesExpectation(const XSampleType::PositionSet& positions)
{
	double res = 0.0;
	auto position = positions.begin();
	for (; position != positions.end(); position++)
	{
		int j = position->j;
		int s1 = position->s1;
		int s2 = position->s2;
		if (0 == j && -1 == s1)
		{
			s1 = 0;
		}
		double probability = _alphaMatrix(j - 1, s1) *
				_edges(j, s1, s2) *
				_betaMatrix(j, s2) *
				_div[j];
		// assume feature value is 1.0 to avoid hash lookup.
		// actually it should be res1 += x.GetFeatureValue(j, s1, s2, k);
		res += (probability * 1.0);
	}
	return res;
}

double FWBW::CalcLikelihood(const XSampleType& x, const YSampleType& y)
{
	double logOfTaggedPath = 0.0; // log
	for (int j = 0; j < y.Length(); j++)
	{
		if (0 != j && y.Tags()[j - 1] >= 0)
		{
			logOfTaggedPath += (_edges(j, y.Tags()[j - 1], y.Tags()[j]));
		}
		logOfTaggedPath += _nodes(j, y.Tags()[j]);
	}

	return logOfTaggedPath - _logNorm;
}

double FWBW::CalcGradient(const XSampleType& x,  const YSampleType& y, int k)
{
	double empirical = 0.0; // linear
	double expected = 0.0; // linear

	auto positions = x.Raw().find(k);
	bool isBigram = true;
	if (positions->second->begin()->s1 < 0)
	{
		isBigram = false;
	}
	if (positions != x.Raw().end())
	{
		// get empirical.
		auto position = positions->second->begin();
		for (; position != positions->second->end(); ++position)
		{
			int j = position->j;
			int s1 = position->s1;
			int s2 = position->s2;
			if (0 == j && -1 == s1)
			{
				s1 = 0;
			}
			if ((0 == j && y.Tags()[j] == s2) ||
				(0 != j && y.Tags()[j - 1] == s1 && y.Tags()[j] == s2))
			{
				// assume feature value is 1.0 to avoid hash lookup.
				// actually it should be empirical += x.GetFeatureValue(j, s1, s2, k);
				// for the expected value, it is the same.
				empirical += 1.0;
			}
		}
		// get expected under model distribution.
		if (isBigram)
		{
			expected = CalcEdgesExpectation(*(positions->second));
		}
		else
		{
			expected = CalcNodesExpectation(*(positions->second));
		}
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
    for(int i = res.Dim1() - 2; i >= 0; i--)
	{
		res[i] += res[i + 1];
	}
    res.ExpInPlace();
}
