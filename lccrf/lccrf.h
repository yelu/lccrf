#pragma once

#include <list>
#include <functional>
#include <cmath>
#include <string>
#include "features.h"

using std::list;
using std::function;
using std::pair;
using std::string;

class LCCRF
{
public:

    LCCRF();
    virtual ~LCCRF(void);

    double Fit(vector<Query>& xs, 
             int maxIteration = 1, 
             double learningRate = 0.1, 
             double l1 = 0.001);

	double Fit(std::string& dataFile,
		    int maxIteration = 1,
			double learningRate = 0.1,
			double l1 = 0.001);

    std::vector<uint16_t> Predict(const Query& x);
	std::vector<uint8_t> Predict(const std::vector<pair<int, int>>& x, int length);

    const vector<double>& GetWeights() const { return _weights; }
    vector<double>& GetWeights() { return this->_weights; }
    LCCRFFeatures& GetFeatures() { return this->_features; }
    double GetEdgeWeight(uint16_t from_label, uint16_t to_label);
    double GetNodeWeight(uint32_t id, uint16_t label);
    vector<Query> LoadData(std::string& dataFile);

    void Save(const string& path);
    void Load(const string& path);
	
private:
	void _GenerateLCCRFFeatures(vector<Query>& qs);

private:

	LCCRFFeatures _features;
    vector<double> _weights;
};

