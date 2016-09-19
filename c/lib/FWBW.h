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

    FWBW(const X& x,
		 const Y& y,
		 const LCCRFFeatures& lccrfFeatures,
         const vector<double>& weights);
    virtual ~FWBW(void);

    double GetLogNorm();

    static void VectorDivide(const MultiArray<double, 1, 10240>& v1, 
        const MultiArray<double, 1, 10240>& v2, 
        MultiArray<double, 1, 10240>& res);

    static void MakeEdgesAndNodes(const X& x,
								  const Y& y,
		                          const LCCRFFeatures& lccrfFeatures,
                                  const vector<double>& weights,
                                  MultiArray<double, 3>& edges,
                                  MultiArray<double, 2>& nodes);

    double CalcNodesExpectation(const int& s2, const std::unordered_set<int>& positions);
    double CalcEdgesExpectation(const int& s1, const int& s2);
    double CalcLikelihood(const X& x, const Y& y);
    double CalcGradient(const X& x, const Y& y, const LCCRFFeatures::UnigramFeature& feature);
	double CalcGradient(const X& x, const Y& y, const LCCRFFeatures::TransitionFeature& feature);

private:
    void _CalculateAlphaMatrix(MultiArray<double, 2>&, MultiArray<double, 1, 10240>&);
    void _CalculateBetaMatrix(MultiArray<double, 2>&, MultiArray<double, 1, 10240>&);

private:
	const LCCRFFeatures& _lccrfFeatures;
	int _length;
	int _labelCount;
    MultiArray<double, 3> _edges;
    MultiArray<double, 2> _nodes;
    MultiArray<double, 2> _alphaMatrix;
    MultiArray<double, 2> _betaMatrix;
    MultiArray<double, 1, 10240> _alphaScales;    // used to avoid exp overflowed.
    MultiArray<double, 1, 10240> _betaScales;     // used to avoid exp overflowed.
    MultiArray<double, 1, 10240> _div;
    double _logNorm; // log
};

