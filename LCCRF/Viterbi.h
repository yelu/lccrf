#pragma once

#include<functional>
#include<vector>
#include <boost/multi_array.hpp>
using std::function;
using std::vector;

class Viterbi
{
public:
	typedef function<double (int, int, int)> DistanceFunciton;
	Viterbi(DistanceFunciton distanceFunciton, int, int);
	virtual ~Viterbi(void);
	const vector<int>& GetPath();


private:
	DistanceFunciton _distance;
	int _nState;
	int _nStep;
	vector<int> _path;
	boost::multi_array<double, 2> _backTraceMatrix;
};

