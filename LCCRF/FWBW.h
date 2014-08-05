#pragma once

#include <functional>
#include <memory>
#include <vector>
#include "Types.h"
#include "MultiArray.h"
using std::function;
using std::shared_ptr;
using std::vector;

class FWBW
{
public:
	typedef vector<vector<double>> Matrix2;
	typedef vector<vector<vector<double>>> Matrix3;

	FWBW(MultiArray<double, 3>& phiMatrix);
	virtual ~FWBW(void);

	double GetModelExpectation(const std::set<XSampleType::Position, 
		                       XSampleType::Position::Compare>&);
	double GetLogNorm();

	static void VectorDivide(const MultiArray<double, 1, 100>& v1, 
		const MultiArray<double, 1, 100>& v2, 
		MultiArray<double, 1, 100>& res);

private:
    void _CalculateAlphaMatrix(MultiArray<double, 2>&, MultiArray<double, 1, 100>&);
	void _CalculateBetaMatrix(MultiArray<double, 2>&, MultiArray<double, 1, 100>&);

private:
	int _jCount;
	int _sCount;
    MultiArray<double, 3> _phiMatrix;
	MultiArray<double, 2> _alphaMatrix;
	MultiArray<double, 2> _betaMatrix;
	MultiArray<double, 1, 100> _alphaScales;
	MultiArray<double, 1, 100> _betaScales;
	MultiArray<double, 1, 100> _div;
	double _logNorm; // log
};

