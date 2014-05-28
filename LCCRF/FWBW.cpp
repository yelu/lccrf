#include "FWBW.h"


FWBW::FWBW(void)
{
}

FWBW::FWBW(function<double (int, int, int)>& phi, int sCount, int jCount):
	_phi(phi), 
	_sCount(sCount),
	_jCount(jCount),
	_alphaMatrix(boost::extents[_jCount][_sCount]),
	_betaMatrix(boost::extents[_jCount][_sCount]),
	_muMatrix(boost::extents[_jCount][_sCount][_sCount]),
	_qMatrix(boost::extents[_jCount][_sCount][_sCount])
{
	_z = 0.0;
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
		_alphaMatrix[0][s] = _phi(-1, s, 0);
	}
	for(int j = 1; j < _jCount; j++)
	{
		for(int s = 0; s < _sCount; s++)
		{
			for(int k = 0; k < _sCount; k++)
			{
				_alphaMatrix[j][s] += (_alphaMatrix[j-1][k] * _phi(k, s, j));
			}
		}
	}
}

void FWBW::_CalculateBetaMatrix()
{
	for(int s = 0; s < _sCount; s++)
	{
		_betaMatrix[_jCount - 1][s] = 1.0;
	}
	for(int j = _jCount - 2; j >= 0; j--)
	{
		for(int s = 0; s < _sCount; s++)
		{
			for(int k = 0; k < _sCount; k++)
			{
				_betaMatrix[j][s] += (_betaMatrix[j+1][k] * _phi(s, k, j+1));
			}
		}
	}
}

void FWBW::_CalculateMuMatrix()
{
	for(int j = 0; j < _jCount - 1; j++)
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
						_muMatrix[j][s1][s2] = _alphaMatrix[j][s1] * _betaMatrix[j][s1];
					}
				}
				else
				{
					_muMatrix[j][s1][s2] = _alphaMatrix[j - 1][s1] * _phi(s1, s2, j) * _betaMatrix[j][s2];
				}
			}
		}
	}
}

void FWBW::_CalculateZ()
{
	for(int s = 0; s < _sCount; s++)
	{
		_z += (_alphaMatrix[_jCount - 1][s]);
	}
}

const boost::multi_array<double, 3>& FWBW::GetQMatrix()
{
	_CalculateMuMatrix();
	_CalculateZ();
	for(int j = 0; j < _jCount; j++)
	{
		for(int s1 = 0; s1 < _sCount; s1++)
		{
			for(int s2 = 0; s2 < _sCount; s2++)
			{
				_qMatrix[j][s1][s2] = _muMatrix[j][s1][s2] / _z;
			}
		}
	}
	return _qMatrix;
}

double FWBW::GetZ()
{
	// todo: return log(_z)
	_CalculateZ();
	return _z;
}
