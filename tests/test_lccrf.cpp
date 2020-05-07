#include "gtest/gtest.h"
#define private public
#include "lccrf.h"

void CreateQueries(vector<Query>& qs)
{
	Token t1({ {0, 1.0f} }, 0);
	Token t2({ {1, 1.0f} }, 1);
	Query q;
	q.AddToken(t1);
	q.AddToken(t2);
	
	qs.push_back(std::move(q));
}

TEST(TestLCCRF, TestFitAndPredict)
{
	LCCRF* lccrf;
	vector<Query> qs;
	CreateQueries(qs);

	lccrf = new LCCRF();

	// fit
	lccrf->Fit(qs, 10, 0.1, 0.1);

	// predit
	auto res = lccrf->Predict(qs.front());
	ASSERT_EQ(res[0], 0);
	ASSERT_EQ(res[1], 1);

	/*
	Assert::AreEqual(0.76578, lccrf->GetWeights()[0], 1e-4);
	Assert::AreEqual(0.76578, lccrf->GetWeights()[1], 1e-4);
	Assert::AreEqual(1.7396, lccrf->GetWeights()[2], 1e-4);
	string weightsFile = "./lccrf.weights.txt";
	lccrf->Save(weightsFile);

	// check likelihood, without regularization.
	FWBW fwbw(xs.front(), ys.front(), lccrfFeatures, lccrf->GetWeights());
	double likehoold = fwbw.CalcLikelihood(xs.front(), ys.front());
	Assert::AreEqual(-0.18334, likehoold, 1e-4);
	*/
};
