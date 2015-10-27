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
         const vector<double>& weights,
         int sCount);
    virtual ~FWBW(void);

    double GetModelExpectation(const X::PositionSet&);
    double GetLogNorm();

    static void VectorDivide(const MultiArray<double, 1, 100>& v1, 
        const MultiArray<double, 1, 100>& v2, 
        MultiArray<double, 1, 100>& res);

    static void MakeEdgesAndNodes(const X& x,
                                  const vector<double>& weights,
                                  MultiArray<double, 3>& edges,
                                  MultiArray<double, 2>& nodes);

    double CalcNodesExpectation(const int& s2, const X::PositionSet& positions);
    double CalcEdgesExpectation(const int& s1, const int& s2, const X::PositionSet& positions);
    double CalcLikelihood(const X& x, const Y& y);
    double CalcGradient(const X& x, const Y& y, int k, bool isCommon);

private:
    void _CalculateAlphaMatrix(MultiArray<double, 2>&, MultiArray<double, 1, 100>&);
    void _CalculateBetaMatrix(MultiArray<double, 2>&, MultiArray<double, 1, 100>&);

	static void _UpdateEdgesAndNodes(const X::FeaturesContainer& features,
		const X& x,
		const vector<double>& weights,
		MultiArray<double, 3>& edges,
		MultiArray<double, 2>& nodes);

	static const X::PositionSet& _GetRightPositionSet(const X&x, const X::FeaturesContainer::const_iterator&);

private:
    int _jCount;
    int _sCount;
    MultiArray<double, 3> _edges;
    MultiArray<double, 2> _nodes;
    MultiArray<double, 2> _alphaMatrix;
    MultiArray<double, 2> _betaMatrix;
    MultiArray<double, 1, 100> _alphaScales;    // used to avoid exp overflowed.
    MultiArray<double, 1, 100> _betaScales;     // used to avoid exp overflowed.
    MultiArray<double, 1, 100> _div;
    double _logNorm; // log
};

