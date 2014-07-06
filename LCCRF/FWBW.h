#pragma once

#include <functional>
#include <memory>
#include <vector>
using std::function;
using std::shared_ptr;
using std::vector;

class FWBW
{
public:
	FWBW(void);
	FWBW(function<double (int, int, int)>&, int, int);
	virtual ~FWBW(void);

	typedef vector<vector<double>> Matrix2;
	typedef vector<vector<vector<double>>> Matrix3;

	shared_ptr<const Matrix3> GetQMatrix();
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
	Matrix2 _alphaMatrix;	// log
	Matrix2 _betaMatrix;	// log
	Matrix3 _muMatrix;	// log
	shared_ptr<Matrix3> _pQMatrix;	//linear, probability [0,1]
	function<double (int, int, int)> _phi;
	double _z; // log
};

