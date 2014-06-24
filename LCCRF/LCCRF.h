#pragma once

//#include <boost/multi_array.hpp>
#include "Types.h"
#include <list>
#include <functional>
using std::list;
using std::function;

namespace boost
{
template<typename T, std::size_t NumDims,
  typename Allocator>
class multi_array;
}

class LCCRF
{
public:

	LCCRF(int, int, double);
	virtual ~LCCRF(void);

	void Fit(const list<XType>& xs, const list<YType>& ys, double learningRate = 0.01, int batch = 1, int maxIteration = 1);

	void Fit(XListType& xs, YListType& ys, double learningRate = 0.01, int batch = 1, int maxIteration = 1);

	void Predict(const XType& x, YType& y);

	void Predict(XListType& x, YListType& y);

	void Debug(const XType&, const YType&);

private:

	static double _Phi(int s1, int s2, int j,
		const XType& doc, 
		vector<double>& weights);

	void _MakeDervative();

	void _MakeLikelihood();

private:

	function<double (vector<double>&, const XType&, const YType&)> _likelihood;
	vector<function<double (vector<double>&, const XType&, const YType&)>> _derivatives;
	const list<XType>* _xs;
	const list<YType>* _ys;
	double _lambda;
	vector<double> _weights;
	int _featureCount;
	int _labelCount;
	// for accelarating derivitive calculation.
	shared_ptr<const boost::multi_array<double, 3, std::allocator<double>>> _cachedQMatrix;
	int _lastK;	// remember last derivative we are calculation. if k is increasing, we can use cached QMatrix.
};

