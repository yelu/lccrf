#include "FWBW.h"
#include "Log.h"
#include <cassert>

FWBW::FWBW(Matrix3& phiMatrix):
	_phiMatrix(phiMatrix),
	_jCount(phiMatrix.size()),
	_sCount(phiMatrix[0].size()),
	_alphaMatrix(_jCount, vector<double>(_sCount, 0.0)),
	_betaMatrix(_jCount, vector<double>(_sCount, 0.0)),
	_muMatrix(_jCount, vector<vector<double>>(_sCount, vector<double>(_sCount, 0.0))),
	_qMatrix(_jCount, vector<vector<double>>(_sCount, vector<double>(_sCount, 0.0)))
{
	_z = std::numeric_limits<double>::lowest();
	_CalculateAlphaMatrix();
	_CalculateBetaMatrix();
}

FWBW::~FWBW(void)
{
}

void FWBW::_CalculateAlphaMatrix()
{
	for(int s = 0; s < _sCount; s++)
	{
		double phi = _phiMatrix[0][0][s];
		_alphaMatrix[0][s] = phi;
	}
	for(int j = 1; j < _jCount; j++)
	{
		for(int s2 = 0; s2 < _sCount; s2++)
		{
			_alphaMatrix[j][s2] = std::numeric_limits<double>::lowest();
			for(int s1 = 0; s1 < _sCount; s1++)
			{
				double phi = ExpPlus(_alphaMatrix[j][s2], _alphaMatrix[j-1][s1] + _phiMatrix[j][s1][s2]);
				_alphaMatrix[j][s2] = phi;
			}
		}
	}
}

void FWBW::_CalculateBetaMatrix()
{
	for(int s = 0; s < _sCount; s++)
	{
		_betaMatrix[_jCount - 1][s] = 0.0;	// log
	}
	for(int j = _jCount - 2; j >= 0; j--)
	{
		for(int s1 = 0; s1 < _sCount; s1++)
		{
			_betaMatrix[j][s1] = std::numeric_limits<double>::lowest();
			for(int s2 = 0; s2 < _sCount; s2++)
			{
				double phi = ExpPlus(_betaMatrix[j][s1], _betaMatrix[j+1][s2] + _phiMatrix[j+1][s1][s2]);
				_betaMatrix[j][s1] = phi;
			}
		}
	}
}

void FWBW::_CalculateMuMatrix()
{
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
						_muMatrix[j][s1][s2] = 0.0;
					}
					else
					{
						double a = _alphaMatrix[j][s2];
						double b = _betaMatrix[j][s2];
						_muMatrix[j][s1][s2] = a + b;
					}
				}
				else
				{
					double a = _alphaMatrix[j - 1][s1];
					double p = _phiMatrix[j][s1][s2];
					double b = _betaMatrix[j][s2];
					_muMatrix[j][s1][s2] = a + p + b;
				}
			}
		}
	}
}

void FWBW::_CalculateZ()
{
	_z = std::numeric_limits<double>::lowest();
    double _z1 = std::numeric_limits<double>::lowest();
	for(int s = 0; s < _sCount; s++)
	{
		_z = ExpPlus(_z, _alphaMatrix[_jCount - 1][s]);
	}
    for(int k = 0; k < _sCount; k++)
	{
		_z1 = ExpPlus(_z1, _betaMatrix[0][k] + _phiMatrix[0][0][k]);
	}
    assert(abs(_z1 - _z) < 1e-4);
}

const FWBW::Matrix3& FWBW::GetQMatrix()
{
	_CalculateMuMatrix();
	_CalculateZ();
	for(int j = 0; j < _jCount; j++)
	{
		for(int s1 = 0; s1 < _sCount; s1++)
		{
			for(int s2 = 0; s2 < _sCount; s2++)
			{
				// fill _qMatrix[0][1..._sCount-1][0..._sCount] with 0.
				if(0 == j && s1 != 0)
				{
					_qMatrix[j][s1][s2] = 0.0;
					continue;
				}
                _qMatrix[j][s1][s2] = exp(_muMatrix[j][s1][s2] - _z);
			}
		}
	}
	return _qMatrix;
}

double FWBW::GetZ()
{
	return _z;	// log
}


double FWBW::ExpPlus(double exp1, double exp2)
{
    // log(exp(x) + exp(y)) = max(x,y) + log(1 + exp(-abs(x-y))),
    // if y-x > 20, then the later is 0. so result is max(x,y).
    double bigger = std::max(exp1, exp2);
    double gap = std::abs(exp1 - exp2);
    if(gap > 20)
    {
        return bigger;
    }
    return bigger + log(1 + exp(0 - gap));
    // if not, use taylor series to expand the later:
    // let x <= exp(-abs(x-y)), then 0<x<=1.
    // log(1+x) = x-x^2/2+x^3/3-x^4/4+...
    /*
    if(gap < 1e-6)
    {
        return bigger + 0.69314718;
    }
    // taylor expansion.
    double scale = exp(-gap);
    double x = scale;
    int n = 1;
    double res = bigger;
    do
    {
        res += (x/(double)n);
        x *= (-1 * scale);
    }while(std::abs(x) > 1e-6);
	return res;
    */
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
				double v = _qMatrix[j][s1][s2];
				LOG_DEBUG("%d,%d\t%f", s1, s2, v);
			}
		}
	}
}