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

	const MultiArray<double, 3>& GetQMatrix();
	double GetZ();

	void PrintQMatrix();

	static void VectorDivide(const MultiArray<double, 1, 100>& v1, 
		const MultiArray<double, 1, 100>& v2, 
		MultiArray<double, 1, 100>& res);

private:
    void _CalculateAlphaMatrix(MultiArray<double, 2>&, MultiArray<double, 1, 100>&);
	void _CalculateBetaMatrix(MultiArray<double, 2>&, MultiArray<double, 1, 100>&);

private:
    MultiArray<double, 3> _phiMatrix;
	int _jCount;
	int _sCount;
	MultiArray<double, 3> _qMatrix;	//linear, probability [0,1]
	double _z; // log
};

