#include "FWBW.h"
#include "Log.h"
#include <cassert>

FWBW::FWBW(Matrix3& phiMatrix):
	_phiMatrix(phiMatrix),
	_jCount(phiMatrix.size()),
	_sCount(phiMatrix[0].size()),
	_alphaMatrix(_jCount, vector<double>(_sCount, 0.0)),
	_betaMatrix(_jCount, vector<double>(_sCount, 0.0)),
	_qMatrix(_jCount, vector<vector<double>>(_sCount, vector<double>(_sCount, 0.0))),
	_alphaScale(_jCount),
	_betaScale(_jCount)
{
	_z = std::numeric_limits<double>::lowest();
	Matrix3Exp(_phiMatrix);
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
		_alphaMatrix[0][s] = _phiMatrix[0][0][s];
	}
	_alphaScale[0] = VectorNormalize(_alphaMatrix[0]);
	for(int j = 1; j < _jCount; j++)
	{
		for(int s2 = 0; s2 < _sCount; s2++)
		{
			_alphaMatrix[j][s2] = 0.0;
			for(int s1 = 0; s1 < _sCount; s1++)
			{
				_alphaMatrix[j][s2] += (_alphaMatrix[j-1][s1] * _phiMatrix[j][s1][s2]);
			}
		}
		_alphaScale[j] = VectorNormalize(_alphaMatrix[j]);
	}
}

void FWBW::_CalculateBetaMatrix()
{
	for(int s = 0; s < _sCount; s++)
	{
		_betaMatrix[_jCount - 1][s] = 1.0;	// linear
	}
	_betaScale[_jCount - 1] = VectorNormalize(_betaMatrix[_jCount - 1]);
	for(int j = _jCount - 2; j >= 0; j--)
	{
		for(int s1 = 0; s1 < _sCount; s1++)
		{
			_betaMatrix[j][s1] = 0.0;
			for(int s2 = 0; s2 < _sCount; s2++)
			{
				_betaMatrix[j][s1] += (_betaMatrix[j+1][s2] * _phiMatrix[j+1][s1][s2]);
			}
		}
		_betaScale[j] = VectorNormalize(_betaMatrix[j]);
	}
}

const FWBW::Matrix3& FWBW::GetQMatrix()
{
	vector<double> div(_jCount);
	VectorDivide(_betaScale, _alphaScale, div);
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
						double a = _alphaMatrix[j][s2];
						double b = _betaMatrix[j][s2];
						_qMatrix[j][s1][s2] = (a * b * div[j] * _alphaScale[j]);
					}
				}
				else
				{
					double a = _alphaMatrix[j - 1][s1];
					double p = _phiMatrix[j][s1][s2];
					double b = _betaMatrix[j][s2];
					_qMatrix[j][s1][s2] = (a * p * b * div[j]);
				}
			}
		}
	}
	return _qMatrix;
}

double FWBW::GetZ()
{
	_z = 0.0;
	for(auto ite = _alphaScale.begin(); ite != _alphaScale.end(); ite++)
	{
		_z += std::log(*ite);
	}
	return _z;
	/*
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


void FWBW::Matrix3Exp(Matrix3& matrix3)
{
	for(auto ite1 = matrix3.begin(); ite1 != matrix3.end(); ite1++)
	{
		for(auto ite2 = ite1->begin(); ite2 != ite1->end(); ite2++)
		{
			for(auto ite3 = ite2->begin(); ite3 != ite2->end(); ite3++)
			{
				(*ite3) = std::exp(*ite3);
			}
		}
	}
}

double FWBW::VectorNormalize(vector<double>& v)
{
	double sum = 0.0;
	for(auto ite = v.begin(); ite != v.end(); ite++)
	{
		sum += (*ite);
	}
	for(auto ite = v.begin(); ite != v.end(); ite++)
	{
		(*ite) /= sum;
	}
	return sum;
}

void FWBW::VectorDivide(const vector<double>& v1, 
						const vector<double>& v2, 
						vector<double>& res)
{
	vector<double> v1Copy(v1);
	vector<double> v2Copy(v2);
	VectorLog(v1Copy);
	VectorLog(v2Copy);
	for(int i = (int)v1Copy.size() - 2; i >= 0; i--)
	{
		v1Copy[i] += v1Copy[i + 1];
	}
	for(int i = (int)v2Copy.size() - 2; i >= 0; i--)
	{
		v2Copy[i] += v2Copy[i + 1];
	}

	for(size_t i = 0; i < res.size(); i++)
	{
		res[i] = v1Copy[i] - v2Copy[i];
	}
	VectorExp(res);
}

void FWBW::VectorLog(vector<double>& v)
{
	for(auto ite = v.begin(); ite != v.end(); ite++)
	{
		(*ite) = std::log(*ite);
	}
}

void FWBW::VectorExp(vector<double>& v)
{
	for(auto ite = v.begin(); ite != v.end(); ite++)
	{
		(*ite) = std::exp(*ite);
	}
}