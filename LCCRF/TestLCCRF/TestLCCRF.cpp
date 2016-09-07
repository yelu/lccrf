#include "gtest/gtest.h"
#include <memory>
#define private public
#include "../lib/LCCRF.h"

void MakeDocument(vector<X>& xs, vector<Y>& ys)
{
	X x(2);
	
	x.SetFeature(0, -1, 0, 0);
	x.SetFeature(1, 0, 1, 1);
	x.SetFeature(1, -1, 1, 2);

	x.SetLength(2);

	xs.push_back(x);

	Y y;
	y.SetTag(0, 0);
	y.SetTag(1, 1);

	ys.push_back(y);
}

class LCCRFTestSuite : public ::testing::Test
{
protected:
	static void SetUpTestCase() 
	{
		MakeDocument(xs, ys);
		lccrf = new LCCRF();
	}

	static void TearDownTestCase() 
	{
	}

	static LCCRF* lccrf;
	static vector<X> xs;
	static vector<Y> ys;
};

LCCRF* LCCRFTestSuite::lccrf;
vector<X> LCCRFTestSuite::xs;
vector<Y> LCCRFTestSuite::ys;

TEST_F(LCCRFTestSuite, TestLearn)
{
	lccrf->Fit(xs, ys, 10000, 0.1, 0.1);
	EXPECT_NEAR(0, lccrf->_weights[0], 1e-2);
	EXPECT_NEAR(3.27, lccrf->_weights[1], 1e-2);
	EXPECT_NEAR(0, lccrf->_weights[2], 1e-2);

	// check likelihood, without regularization.
	FWBW fwbw(xs.front(), lccrf->_weights, xs.front().Length());
	double likehoold = fwbw.CalcLikelihood(xs.front(), ys.front());
	EXPECT_NEAR(-0.107199, likehoold, 1e-3);
}

TEST_F(LCCRFTestSuite, TestPredict)
{
	//lccrf->Fit(xs, ys, 100000, 0.01, 0.1);

	Y res;
	lccrf->Predict(xs.front(), res);
}

/*
int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();
	return ret;
}
*/
