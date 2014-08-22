#pragma once

#include "Types.h"
#include <list>
#include <functional>
#include "FWBW.h"
#include "SGD.h"
using std::list;
using std::function;
using std::pair;

class LCCRF
{
public:

	LCCRF(int, int);
    LCCRF();
	virtual ~LCCRF(void);

    
    void Fit(const vector<XSampleType>& xs, 
		     const vector<YSampleType>& ys, 
			 int maxIteration = 1, 
			 double learningRate = 0.001, 
			 double l2 = 0.001);

	void Fit(XType& xs, 
		     YType& ys, 
			 int maxIteration = 1, 
			 double learningRate = 0.001, 
			 double l2 = 0.001);

	void Predict(const XSampleType& x, YSampleType& y);

	void Predict(XType& x, YType& y);

    pair<list<list<pair<int, double>>>, double> Debug(const XSampleType&, 
		                                              const YSampleType&);

	vector<double>& GetWeights();

private:

	static double _Phi(int s1, int s2, int j,
		const XSampleType& doc, 
		vector<double>& weights,
        list<pair<int, double>>* hitFeatures);

private:
	const vector<XSampleType>* _xs;
	const vector<YSampleType>* _ys;
	vector<double> _weights;
	int _featureCount;
	int _tagCount;
};

