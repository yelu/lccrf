#pragma once

#include<functional>
#include<vector>
#include "query.h"
#include "multi_array.h"
using std::function;
using std::vector;

class Evaluator
{
public:
    Evaluator(uint16_t label_cnt);
    virtual ~Evaluator(void) = default;

    typedef vector<tuple<int, int, int>> Report;

    void Eval(const vector<Query>& references, 
                     const vector<vector<uint16_t>> predicted_results);

    Report& GetReport();
    float Precision();
    float Recall();
    float F1();

    void PrintReport();

private:
    Report _report;
    uint32_t _tp;
    uint32_t _fn;
    uint32_t _fp;
};

