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

	void PrintQMatrix();

	static void Matrix3Exp(Matrix3&);

	static void VectorLog(vector<double>&);

	static void VectorExp(vector<double>&);

	static double VectorNormalize(vector<double>&);

	static void VectorDivide(const vector<double>& v1, 
		const vector<double>& v2, 
		vector<double>& res);

private:
	void _CalculateAlphaMatrix();
	void _CalculateBetaMatrix();

private:
	Matrix3 _phiMatrix;
	int _jCount;
	int _sCount;
	Matrix2 _alphaMatrix;	// normalized linear with scale at each j
	Matrix2 _betaMatrix;	// normalized linear with scale at each j
	Matrix3 _qMatrix;	//linear, probability [0,1]
	double _z; // log
	vector<double> _alphaScale;
	vector<double> _betaScale;
};

