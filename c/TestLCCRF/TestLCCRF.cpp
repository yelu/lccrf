#include "gtest/gtest.h"
#include <memory>
#define private public
#include "../lib/LCCRF.h"

void MakeDocument(vector<X>& xs, vector<Y>& ys, LCCRFFeatures& lccrfFeatures)
{
	X x(2);	
	x.AddFeature(0, 0);
	x.AddFeature(1, 1);

	xs.push_back(x);

	Y y(2);
	y.SetTag(0, 0);
	y.SetTag(1, 1);

	ys.push_back(y);

	lccrfFeatures.AddSample(x, y);
}

class LCCRFTestSuite : public ::testing::Test
{
protected:
	static void SetUpTestCase() 
	{
		MakeDocument(xs, ys, lccrfFeatures);
		lccrf = new LCCRF();
	}

	static void TearDownTestCase() 
	{
	}

	static LCCRF* lccrf;
	static vector<X> xs;
	static vector<Y> ys;
	static LCCRFFeatures lccrfFeatures;
};

LCCRF* LCCRFTestSuite::lccrf;
vector<X> LCCRFTestSuite::xs;
vector<Y> LCCRFTestSuite::ys;
LCCRFFeatures LCCRFTestSuite::lccrfFeatures;

TEST_F(LCCRFTestSuite, TestFitAndPredict)
{
	// fit
	lccrf->Fit(xs, ys, 10000, 0.1, 0.1);
	EXPECT_NEAR(0.76578, lccrf->_weights[0], 1e-4);
	EXPECT_NEAR(0.76578, lccrf->_weights[1], 1e-4);
	EXPECT_NEAR(1.7396, lccrf->_weights[2], 1e-4);
	string weightsFile = "./lccrf.weights.txt";
	lccrf->Save(weightsFile);

	// check likelihood, without regularization.
	FWBW fwbw(xs.front(), ys.front(), lccrfFeatures, lccrf->_weights);
	double likehoold = fwbw.CalcLikelihood(xs.front(), ys.front());
	EXPECT_NEAR(-0.18334, likehoold, 1e-4);

	// predit
	Y res = lccrf->Predict(xs.front());
	ASSERT_EQ(res.Tags[0], 0);
	ASSERT_EQ(res.Tags[1], 1);
}

