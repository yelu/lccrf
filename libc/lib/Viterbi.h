#pragma once

#include<functional>
#include<vector>
#include "MultiArray.h"
using std::function;
using std::vector;

class Viterbi
{
public:
    Viterbi(void);
    virtual ~Viterbi(void);

    typedef vector<vector<vector<double>>> Matrix3;

    static void GetPath(const MultiArray<double, 3>& edges, 
                        const MultiArray<double, 2>& nodes, 
                        vector<int>& res);

private:
};

