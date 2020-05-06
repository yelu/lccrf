#pragma once

#include <functional>
#include <vector>
#include <list>
#include <limits>
#include <cstring>

using std::function;
using std::vector;
using std::list;

class LCCRF;
class Query;
class Token;

class PerceptronTrainer
{
public:
    PerceptronTrainer(
        const vector<Query>& qs,
        LCCRF& model);

    virtual ~PerceptronTrainer(void) = default;

    const vector<double>& Run(double learningRate, int max_iter = 20);

    double UpdateWeights(const Query& q);

    uint32_t GetErrors(const Query& q, const vector<uint16_t> expected);

private:

    void UpdateNodeWeights(const Token& t, uint16_t label, double scale);
    void UpdateEdgeWeights(uint16_t pre_label, uint16_t label, double scale);

    const vector<Query>& _qs;
	LCCRF& _model;
    vector<double>& _weights;
    double _learning_rate;
};
