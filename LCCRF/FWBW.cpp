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
	_pQMatrix(new Matrix3(_jCount, vector<vector<double>>(_sCount, vector<double>(_sCount, 0.0))))
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
				//printf("pos:%d,%d,%d _phiMatrix:%d,%d,%d\n", j, s1, s2, _phiMatrix.size(), _phiMatrix[0].size(), _phiMatrix[0][0].size());
				double phi = ExpPlus(_alphaMatrix[j][s2], _alphaMatrix[j-1][s1] + _phiMatrix[j][s1][s2]);
				_alphaMatrix[j][s2] = phi;
				//printf("pos:%d,%d,%d _phiMatrix:%d,%d,%d\n", j, s1, s2, _phiMatrix.size(), _phiMatrix[0].size(), _phiMatrix[0][0].size());
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
    assert(abs(_z1 - _z) < 10e-4);
    if(abs(_z1 - _z) > 10e-4)
    {
        LOG_DEBUG("fb : %f\t%f", _z, _z1);
    }
}

shared_ptr<const FWBW::Matrix3> FWBW::GetQMatrix()
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
					(*_pQMatrix)[j][s1][s2] = 0.0;
					continue;
				}
				(*_pQMatrix)[j][s1][s2] = exp(_muMatrix[j][s1][s2] - _z);
			}
		}
	}
	return _pQMatrix;
}

double FWBW::GetZ()
{
	_CalculateZ();
	return _z;	// log
}


double FWBW::ExpPlus(double exp1, double exp2)
{
	if(exp1 < exp2)
	{
		double t = exp1;
		exp1 = exp2;
		exp2  = t;
	}
	if(exp1 - exp2 > 20)
	{
		return exp1;
	}
	return exp2 + log(exp(exp1 - exp2) + 1);
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
				double v = (*_pQMatrix)[j][s1][s2];
				LOG_DEBUG("%d,%d\t%f", s1, s2, v);
			}
		}
	}
}