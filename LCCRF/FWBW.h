#pragma once

#include <boost/multi_array.hpp>
#include <functional>
#include <memory>
using std::function;
using std::shared_ptr;

class FWBW
{
public:
	FWBW(void);
	FWBW(function<double (int, int, int)>&, int, int);
	virtual ~FWBW(void);

	shared_ptr<const boost::multi_array<double, 3>> GetQMatrix();
	double GetZ();

	static double ExpPlus(double, double);

	void PrintQMatrix();

private:
	void _CalculateAlphaMatrix();
	void _CalculateBetaMatrix();
	void _CalculateMuMatrix();
	void _CalculateZ();

private:
	int _sCount;
	int _jCount;
	boost::multi_array<double, 2> _alphaMatrix;	// log
	boost::multi_array<double, 2> _betaMatrix;	// log
	boost::multi_array<double, 3> _muMatrix;	// log
	shared_ptr<boost::multi_array<double, 3>> _pQMatrix;	//linear, probability [0,1]
	function<double (int, int, int)> _phi;
	double _z; // log
};

