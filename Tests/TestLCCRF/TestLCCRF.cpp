#include "gtest/gtest.h"
#define private public
#include "../../LCCRF/LCCRF.h"
#include <memory>

void MakeDocument(vector<XSampleType>& xs, vector<YSampleType>& ys)
{
	XSampleType x(2);
	XSampleType::Key key1(0, -1, 0);
	
	x.SetFeature(0, -1, 0, 0);
	x.SetFeature(1, 0, 1, 1);
	x.SetFeature(1, 1, 1, 1);

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
		lccrf = new LCCRF(2, 2);
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
	lccrf->Fit(xs, ys, 1000, 0.1, 0.1);
	EXPECT_NEAR(1.63, lccrf->_weights[0], 10e-2);
	EXPECT_NEAR(1.63, lccrf->_weights[1], 10e-2);
}

/*
TEST_F(LCCRFTestSuite, TestDerivative)
{
	// -d/dx = 1-(e^(x+y)+e^x)/(e^(x+y)+e^x+e^y+1)-0.1x
	// -d/dy = 1-(e^(x+y)+e^y)/(e^(x+y)+e^x+e^y+1)-0.1y
	lccrf->Fit(xs, ys, 0, 1, 0.1);
	double res = 0;
	vector<double> weights;

	weights.clear();
	weights.push_back(0);
	weights.push_back(0);
	res = lccrf->_derivatives[0](weights, *(xs.begin()), *(ys.begin()));
	EXPECT_NEAR(-0.5, res, 10e-6);
	res = lccrf->_derivatives[1](weights, *(xs.begin()), *(ys.begin()));
	EXPECT_NEAR(-0.5, res, 10e-6);
	
	weights.clear();
	weights.push_back(1.0);
	weights.push_back(1.0);
	res = lccrf->_derivatives[0](weights, *(xs.begin()), *(ys.begin()));
	EXPECT_NEAR(-0.168941, res, 10e-6);
	res = lccrf->_derivatives[1](weights, *(xs.begin()), *(ys.begin()));
	EXPECT_NEAR(-0.168941, res, 10e-6);

	weights.clear();
	weights.push_back(0.5);
	weights.push_back(2.0);
	res = lccrf->_derivatives[0](weights, *(xs.begin()), *(ys.begin()));
	EXPECT_NEAR(-0.327541, res, 10e-6);
	res = lccrf->_derivatives[1](weights, *(xs.begin()), *(ys.begin()));
	EXPECT_NEAR(0.0807971, res, 10e-6);
}

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

/*
TEST_F(LCCRFTestSuite, TestPredict)
{
	lccrf->Fit(xs, ys, 0.1, 1, 1000);

	Document doc;
	wstring xs[] = {L"I", L"love"};
	wstring ys[] = {L"tag1",L"tag2"};
	int n = sizeof(xs) / sizeof(wstring);
	MakeDocument(doc, xs, ys, n);

	vector<wstring> res;
	lccrf->Predict(doc, res);
	EXPECT_STREQ(L"tag1", res[0].c_str());
	EXPECT_STREQ(L"tag2", res[1].c_str());

	vector<wstring> path;

	path.clear();
	path.push_back(L"tag1");
	path.push_back(L"tag1");
	lccrf->Debug(doc, path);

	path.clear();
	path.push_back(L"tag1");
	path.push_back(L"tag2");
	lccrf->Debug(doc, path);

	path.clear();
	path.push_back(L"tag2");
	path.push_back(L"tag1");
	lccrf->Debug(doc, path);

	path.clear();
	path.push_back(L"tag2");
	path.push_back(L"tag2");
	lccrf->Debug(doc, path);

	// bad path.
	path.clear();
	path.push_back(L"tag1");
	path.push_back(L"tag3");
	lccrf->Debug(doc, path);
}

*/

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();
	return ret;
}
