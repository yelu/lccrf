#pragma once

#include <functional>
#include <memory>
#include <vector>
#include "Types.h"
using std::function;
using std::shared_ptr;
using std::vector;

class FWBW
{
public:
	typedef vector<vector<double>> Matrix2;
	typedef vector<vector<vector<double>>> Matrix3;

	FWBW(Matrix3& phiMatrix);
	virtual ~FWBW(void);

	const Matrix3& GetQMatrix();
	double GetZ();

	static double ExpPlus(double, double);

	void PrintQMatrix();

private:
	void _CalculateAlphaMatrix();
	void _CalculateBetaMatrix();
	void _CalculateMuMatrix();
	void _CalculateZ();

private:
	Matrix3& _phiMatrix;
	int _jCount;
	int _sCount;
	Matrix2 _alphaMatrix;	// log
	Matrix2 _betaMatrix;	// log
	Matrix3 _muMatrix;	// log
	Matrix3 _qMatrix;	//linear, probability [0,1]
	double _z; // log
};

