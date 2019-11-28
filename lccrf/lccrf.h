#pragma once

#include <list>
#include <functional>
#include <cmath>
#include <string>
#include "types.h"
#include "fwbw.h"
using std::list;
using std::function;
using std::pair;
using std::string;

class LCCRF
{
public:

    LCCRF();
    virtual ~LCCRF(void);

    void Fit(const vector<X>& xs, 
             const vector<Y>& ys, 
             int maxIteration = 1, 
             double learningRate = 0.001, 
             double l1 = 0.001);

	void Fit(const std::string& dataFile,
		    int maxIteration = 1,
			double learningRate = 0.001,
			double l1 = 0.001);

    Y Predict(const X& x);
	std::vector<uint8_t> Predict(const std::vector<pair<int, int>>& x, int length);

    pair<list<list<pair<int, double>>>, double> Debug(const X&, 
                                                      const Y&);

    const vector<double>& GetWeights() const;

    void Save(const string& path);
    void Load(const string& path);
	const vector<double>& GetWeights() { return this->_weights; }

private:

    static double _Phi(int s1, int s2, int j,
                       const X& x, const Y& y,
					   const LCCRFFeatures& lccrfFeatures,
                       vector<double>& weights,
                       list<pair<int, double>>* hitFeatures);

	void _GenerateLCCRFFeatures(const vector<X>& xs, const vector<Y>& ys);

private:

	LCCRFFeatures _lccrfFeatures;

    const vector<X>* _xs;
    const vector<Y>* _ys;
    vector<double> _weights;
};

