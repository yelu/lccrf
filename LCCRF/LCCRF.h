#pragma once

#include "Types.h"
#include <list>
#include <functional>
#include "FWBW.h"
using std::list;
using std::function;
using std::pair;

class LCCRF
{
public:

	LCCRF(int, int);
	virtual ~LCCRF(void);

	void Fit(const vector<XSampleType>& xs, const vector<YSampleType>& ys, int maxIteration = 1, double learningRate = 0.001, double l2 = 0.001);

	void Fit(XType& xs, YType& ys, int maxIteration = 1, double learningRate = 0.001, double l2 = 0.001);

	void Predict(const XSampleType& x, YSampleType& y);

	void Predict(XType& x, YType& y);

    pair<list<list<pair<int, double>>>, double> Debug(const XSampleType&, const YSampleType&);

	vector<double>& GetWeights();

private:

	static double _Phi(int s1, int s2, int j,
		const XSampleType& doc, 
		vector<double>& weights,
        list<pair<int, double>>* hitFeatures = NULL,
        double c = 1.0);

	void _MakeDervative();

	void _MakeLikelihood();

private:

	function<double (const XSampleType&, const YSampleType&, vector<double>&, double)> _likelihood;
	function<double (const XSampleType&, const YSampleType&, vector<double>&, double, int)> _derivative;
	const vector<XSampleType>* _xs;
	const vector<YSampleType>* _ys;
	vector<double> _weights;
	int _featureCount;
	int _labelCount;
	// for accelarating derivitive calculation.
	shared_ptr<const FWBW::Matrix3> _cachedQMatrix;
	int _lastK;	// remember last derivative we are calculation. if k is increasing, we can use cached QMatrix.
};

