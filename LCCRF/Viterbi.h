#pragma once

#include<functional>
#include<vector>
using std::function;
using std::vector;

class Viterbi
{
public:
	Viterbi(void);
	virtual ~Viterbi(void);

	typedef vector<vector<vector<double>>> Matrix3;

	static void GetPath(const Matrix3& graph, vector<int>& res);
private:
};

