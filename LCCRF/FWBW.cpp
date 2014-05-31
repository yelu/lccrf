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
		double phi = _phi(-1, s, 0);
		_alphaMatrix[0][s] = phi;
	}
	for(int j = 1; j < _jCount; j++)
	{
		for(int s1 = 0; s1 < _sCount; s1++)
		{
			_alphaMatrix[j][s1] = std::numeric_limits<double>::lowest();
			for(int s2 = 0; s2 < _sCount; s2++)
			{
				double phi = ExpPlus(_alphaMatrix[j][s1], _alphaMatrix[j-1][s2] + _phi(s2, s1, j));
				_alphaMatrix[j][s1] = phi;
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
		for(int s = 0; s < _sCount; s++)
		{
			_betaMatrix[j][s] = std::numeric_limits<double>::lowest();
			for(int k = 0; k < _sCount; k++)
			{
				double phi = ExpPlus(_betaMatrix[j][s], _betaMatrix[j+1][k] + _phi(s, k, j+1));
				_betaMatrix[j][s] = phi;
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
						_muMatrix[j][s1][s2] = _alphaMatrix[j][s1] + _betaMatrix[j][s1];
					}
				}
				else
				{
					_muMatrix[j][s1][s2] = _alphaMatrix[j - 1][s1] + _phi(s1, s2, j) + _betaMatrix[j][s2];
				}
			}
		}
	}
}

void FWBW::_CalculateZ()
{
	_z = std::numeric_limits<double>::lowest();
	for(int s = 0; s < _sCount; s++)
	{
		_z = ExpPlus(_z, _alphaMatrix[_jCount - 1][s]);
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