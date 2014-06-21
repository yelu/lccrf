#pragma once

#include "Types.h"

class LCCRF
{
public:

	LCCRF(int, int, double);
	virtual ~LCCRF(void);

	void Fit(const list<X>& xs, const list<Y>& ys, double learningRate = 0.01, int batch = 1, int maxIteration = 1);

	void Predict(const X& doc, Y& tags);

	void Debug(const X&, const Y&);

private:

	static double _Phi(int s1, int s2, int j,
		const X& doc, 
		vector<double>& weights);

	void _MakeDervative();

	void _MakeLikelihood();

private:

	function<double (vector<double>&, const X&, const Y&)> _likelihood;
	vector<function<double (vector<double>&, const X&, const Y&)>> _derivatives;
	const list<X>* _xs;
	const list<Y>* _ys;
	double _lambda;
	vector<double> _weights;
	int _featureCount;
	int _labelCount;
};

