#pragma once

#include <list>
#include <functional>
#include <cmath>
#include <string>
#include "Types.h"
#include "FWBW.h"
using std::list;
using std::function;
using std::pair;
using std::string;

class LCCRF
{
public:

    LCCRF(int, int);
    LCCRF();
    virtual ~LCCRF(void);

    void Fit(const vector<X>& xs, 
             const vector<Y>& ys, 
             int maxIteration = 1, 
             double learningRate = 0.001, 
             double l1 = 0.001);

    void Fit(XList& xs, 
             YList& ys, 
             int maxIteration = 1, 
             double learningRate = 0.001, 
             double l1 = 0.001);

    void Predict(const X& x, Y& y);

    void Predict(XList& x, YList& y);

    pair<list<list<pair<int, double>>>, double> Debug(const X&, 
                                                      const Y&);

    vector<double>& GetWeights();

    void Save(string& path);
    void Load(string& path);

private:

    static double _Phi(int s1, int s2, int j,
                       const X& doc, 
                       vector<double>& weights,
                       list<pair<int, double>>* hitFeatures);

private:
    const vector<X>* _xs;
    const vector<Y>* _ys;
    vector<double> _weights;
    int _featureCount;
    int _tagCount;
};

