#pragma once

#include <boost/multi_array.hpp>
#include <boost/function.hpp>

class FWBW
{
public:
	FWBW(void);
	FWBW(boost::function<double (int, int, int)>&, int, int);
	virtual ~FWBW(void);

	const boost::multi_array<double, 3>& GetQMatrix();
	double GetZ();

private:
	void _CalculateAlphaMatrix();
	void _CalculateBetaMatrix();
	void _CalculateMuMatrix();
	void _CalculateZ();

private:
	int _sCount;
	int _jCount;
	boost::multi_array<double, 2> _alphaMatrix;
	boost::multi_array<double, 2> _betaMatrix;
	boost::multi_array<double, 3> _muMatrix;
	boost::multi_array<double, 3> _qMatrix;
	boost::function<double (int, int, int)> _phi;
	double _z;
};

