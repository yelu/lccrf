#include "gtest/gtest.h"
#include <memory>
#define private public
#include "../../LCCRF/LCCRF.h"

void MakeDocument(vector<XSampleType>& xs, vector<YSampleType>& ys)
{
	XSampleType x(2);
	
	x.SetFeature(0, -1, 0, 0);
	x.SetFeature(1, 0, 1, 1);
	x.SetFeature(1, -1, 1, 2);

	x.SetLength(2);

	xs.push_back(x);

	YSampleType y;
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
	static vector<XSampleType> xs;
	static vector<YSampleType> ys;
};

LCCRF* LCCRFTestSuite::lccrf;
vector<XSampleType> LCCRFTestSuite::xs;
vector<YSampleType> LCCRFTestSuite::ys;

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

/*
TEST_F(LCCRFTestSuite, TestDerivative)
{
	// -d/dx = 1-(e^(x+y)+e^x)/(e^(x+y)+e^x+e^y+1)-0.1x
	// -d/dy = 1-(e^(x+y)+e^y)/(e^(x+y)+e^x+e^y+1)-0.1y
	XSampleType& x = xs[0];
	YSampleType& y  = ys[0];
	int labelCount = 2;
	double res  = 0.0;
	vector<double> weights;

	weights.clear();
	weights.push_back(0);
	weights.push_back(0);
    MultiArray<double, 3> phiMatrix1(y.Length(), labelCount, labelCount, 0.0);
	SGD::MakePhiMatrix(x, weights, 1.0, phiMatrix1);
	FWBW fwbw1(phiMatrix1);
	res = SGD::_CaculateGradient(x, y, 0, fwbw1);
	EXPECT_NEAR(-0.5, res, 1e-6);
	res = SGD::_CaculateGradient(x, y, 1, fwbw1);
	EXPECT_NEAR(-0.5, res, 1e-6);
	
	weights.clear();
	weights.push_back(1.0);
	weights.push_back(1.0);
	MultiArray<double, 3> phiMatrix2(y.Length(), labelCount, labelCount, 0.0);
	SGD::MakePhiMatrix(x, weights, 1.0, phiMatrix2);
	FWBW fwbw2(phiMatrix2);
	res = SGD::_CaculateGradient(x, y, 0, fwbw2);
	EXPECT_NEAR(-0.268941, res, 1e-6);
	res = SGD::_CaculateGradient(x, y, 1, fwbw2);
	EXPECT_NEAR(-0.268941, res, 1e-6);

	weights.clear();
	weights.push_back(0.5);
	weights.push_back(2.0);
	MultiArray<double, 3> phiMatrix3(y.Length(), labelCount, labelCount, 0.0);
	SGD::MakePhiMatrix(x, weights, 1.0, phiMatrix3);
	FWBW fwbw3(phiMatrix3);
	res = SGD::_CaculateGradient(x, y, 0, fwbw3);
	EXPECT_NEAR(-0.3775406, res, 1e-6);
	res = SGD::_CaculateGradient(x, y, 1, fwbw3);
	EXPECT_NEAR(-0.1192029, res, 1e-6);
}
*/

/*
TEST_F(LCCRFTestSuite, TestSaveLoad)
{
    lccrf->Save(L"./test.model");

    LCCRF lccrf1;
    lccrf1.Load(L"./test.model");
    YSampleType y;
    lccrf1.Predict(xs[0], y);
    EXPECT_EQ(y.Raw()[0], 0);
    EXPECT_EQ(y.Raw()[1], 1);
}
*/

/*
TEST_F(LCCRFTestSuite, TestLikelihood)
{
	// -d/dx = 1-(e^(x+y)+e^x)/(e^(x+y)+e^x+e^y+1)-0.1x
	// -d/dy = 1-(e^(x+y)+e^y)/(e^(x+y)+e^x+e^y+1)-0.1y
	lccrf->Fit(xs, ys, 0, 1, 0.1);
	double res = 0;
	vector<double> weights;

	weights.clear();
	weights.push_back(0);
	weights.push_back(0);
	res = lccrf->_likelihood(weights, *(xs.begin()), *(ys.begin()));
	EXPECT_NEAR(1.38629, res, 10e-6);
	
	weights.clear();
	weights.push_back(1.0);
	weights.push_back(1.0);
	res = lccrf->_likelihood(weights, *(xs.begin()), *(ys.begin()));
	EXPECT_NEAR(0.726523, res, 10e-6);

	weights.clear();
	weights.push_back(0.5);
	weights.push_back(2.0);
	res = lccrf->_likelihood(weights, *(xs.begin()), *(ys.begin()));
	EXPECT_NEAR(0.813505, res, 10e-6);
}
*/

TEST_F(LCCRFTestSuite, TestPredict)
{
	//lccrf->Fit(xs, ys, 100000, 0.01, 0.1);

	YSampleType res;
	lccrf->Predict(xs.front(), res);
}


int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();
	return ret;
}
