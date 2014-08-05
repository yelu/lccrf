#include "FWBW.h"
#include "Log.h"
#include <cassert>

FWBW::FWBW(MultiArray<double, 3>& phiMatrix):
	_phiMatrix(phiMatrix),
    _jCount(phiMatrix.Dim1()),
    _sCount(phiMatrix.Dim2()),
	_qMatrix(_jCount, _sCount, _sCount)
{
	_z = std::numeric_limits<double>::lowest();
    _phiMatrix.ExpInPlace();
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

const MultiArray<double, 3>& FWBW::GetQMatrix()
{
    MultiArray<double, 2> alphaMatrix(_jCount, _sCount);
    MultiArray<double, 2> betaMatrix(_jCount, _sCount);
    MultiArray<double, 1, 100> alphaScales(_jCount);
    MultiArray<double, 1, 100> betaScales(_jCount);
    _CalculateAlphaMatrix(alphaMatrix, alphaScales);
    _CalculateBetaMatrix(betaMatrix, betaScales);

    MultiArray<double, 1, 100> div(_jCount);
    VectorDivide(betaScales, alphaScales, div);
	for(int j = 0; j < _jCount; j++)
	{
		for(int s1 = 0; s1 < _sCount; s1++)
		{
			for(int s2 = 0; s2 < _sCount; s2++)
			{
				if(0 == j)
				{
					// fill _muMatrix[0][1..._sCount-1][0..._sCount] with 0.
					if(s1 != 0)
					{
						_qMatrix[j][s1][s2] = 0.0;
					}
					else
					{
						double a = alphaMatrix(j, s2);
						double b = betaMatrix(j, s2);
						_qMatrix(j, s1, s2) = (a * b * div[j] * alphaScales[j]);
					}
				}
				else
				{
					double a = alphaMatrix(j - 1, s1);
					double p = _phiMatrix(j, s1, s2);
					double b = betaMatrix(j, s2);
					_qMatrix(j, s1, s2) = (a * p * b * div[j]);
				}
			}
		}
	}

    // calculate _z.
    _z = 0.0;
    for(int i = 0; i < alphaScales.Dim1(); i++)
	{
        _z += std::log(alphaScales[i]);
	}

	return _qMatrix;
}

double FWBW::GetZ()
{
	return _z;
}

void FWBW::PrintQMatrix()
{
	for(int j = 0; j < _jCount; j++)
	{
		LOG_DEBUG("\nj = %d", j);
		for(int s1 = 0; s1 < _sCount; s1++)
		{
			for(int s2 =0; s2 < _sCount; s2++)
			{
				double v = _qMatrix(j, s1, s2);
				LOG_DEBUG("%d,%d\t%f", s1, s2, v);
			}
		}
	}
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
