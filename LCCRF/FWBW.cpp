#include "FWBW.h"
#include "Log.h"
#include <cassert>
#include <limits>

FWBW::FWBW(MultiArray<double, 3>& phiMatrix):
	_jCount(phiMatrix.Dim1()),
    _sCount(phiMatrix.Dim2()),
	_phiMatrix(phiMatrix),
	_alphaMatrix(_jCount, _sCount),
	_betaMatrix(_jCount, _sCount),
	_alphaScales(_jCount),
	_betaScales(_jCount),
	_div(_jCount)
{
	_logNorm = std::numeric_limits<double>::lowest();
    _phiMatrix.ExpInPlace();
	_CalculateAlphaMatrix(_alphaMatrix, _alphaScales);
	_CalculateBetaMatrix(_betaMatrix, _betaScales);
	VectorDivide(_betaScales, _alphaScales, _div);

	// calculate log norm.
    _logNorm = 0.0;
    for(int i = 0; i < _alphaScales.Dim1(); i++)
	{
        _logNorm += std::log(_alphaScales[i]);
	}
}

FWBW::~FWBW(void)
{
}

void FWBW::_CalculateAlphaMatrix(MultiArray<double, 2>& alphaMatrix, MultiArray<double, 1, 100>& scales)
{
	for(int s = 0; s < _sCount; s++)
	{
		alphaMatrix(0,s) = _phiMatrix(0, 0, s);
	}
    scales[0] = alphaMatrix[0].NormalizeInPlace();
	for(int j = 1; j < _jCount; j++)
	{
		for(int s2 = 0; s2 < _sCount; s2++)
		{
			alphaMatrix(j, s2) = 0.0;
			for(int s1 = 0; s1 < _sCount; s1++)
			{
				alphaMatrix(j, s2) += (alphaMatrix(j-1, s1) * _phiMatrix(j, s1, s2));
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
				betaMatrix(j, s1) += (betaMatrix(j+1, s2) * _phiMatrix(j+1, s1, s2));
			}
		}
        scales[j] = betaMatrix[j].NormalizeInPlace();
	}
}

double FWBW::GetModelExpectation(const XSampleType::PositionSet& positions)
{
	double res = 0.0;
	auto position = positions.begin();
	for(; position != positions.end(); position++)
	{
		int j = position->j;
		int s1 = position->s1;
		int s2 = position->s2;
		if(0 == j && -1 == s1)
		{
			s1 = 0;
		}
		double probability = 0.0;
		if(0 == j && 0 == s1)
		{
			probability = _alphaMatrix(j, s2) *
						  _betaMatrix(j, s2) *
						  _div[j] * _alphaScales[j];
		}
		else if(0 != j)
		{
			probability = _alphaMatrix(j - 1, s1) * 
						  _phiMatrix(j, s1, s2) *
						  _betaMatrix(j, s2) *
						  _div[j];
		}
		// assume feature value is 1.0 to avoid hash lookup.
		// actually it should be res1 += x.GetFeatureValue(j, s1, s2, k);
		res += (probability * 1.0);
	}
	return res;
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
