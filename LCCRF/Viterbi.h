#pragma once

#include<functional>
#include<vector>
#include <boost/multi_array.hpp>
using std::function;
using std::vector;

class Viterbi
{
public:
	Viterbi(void);
	virtual ~Viterbi(void);
	static void GetPath(const boost::multi_array<double, 3>& graph, vector<int>& res);
private:
};

