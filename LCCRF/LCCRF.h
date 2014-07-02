#pragma once

//#include <boost/multi_array.hpp>
#include "Types.h"
#include <list>
#include <functional>
using std::list;
using std::function;
using std::pair;

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

	void Fit(const list<XSampleType>& xs, const list<YSampleType>& ys, double learningRate = 0.01, int batch = 1, int maxIteration = 1);

	void Fit(XType& xs, YType& ys, double learningRate = 0.01, int batch = 1, int maxIteration = 1);

	void Predict(const XSampleType& x, YSampleType& y);

	void Predict(XType& x, YType& y);

    pair<list<list<pair<int, double>>>, double> Debug(const XSampleType&, const YSampleType&);

	vector<double>& GetWeights();

private:

	static double _Phi(int s1, int s2, int j,
		const XSampleType& doc, 
		vector<double>& weights,
        list<pair<int, double>>* hitFeatures = NULL);

	void _MakeDervative();

	void _MakeLikelihood();

private:

	function<double (vector<double>&, const XSampleType&, const YSampleType&)> _likelihood;
	vector<function<double (vector<double>&, const XSampleType&, const YSampleType&)>> _derivatives;
	const list<XSampleType>* _xs;
	const list<YSampleType>* _ys;
	double _lambda;
	vector<double> _weights;
	int _featureCount;
	int _labelCount;
	// for accelarating derivitive calculation.
	shared_ptr<const boost::multi_array<double, 3, std::allocator<double>>> _cachedQMatrix;
	int _lastK;	// remember last derivative we are calculation. if k is increasing, we can use cached QMatrix.
};

