#include "gtest/gtest.h"
#define private public
#include "../LCCRF/LCCRF.h"

void MakeDocument(Document& doc, wstring xs[], wstring ys[], int len)
{
	for(int i = 0; i < len; i++)
	{
		X x;
		x.push_back(xs[i]);
		Y y = ys[i];
		Token t(x, y);
		doc.push_back(t);
	}
}

class LCCRFTestSuite : public ::testing::Test
{
protected:
	static void SetUpTestCase() 
	{
		Document doc;
		wstring xs[] = {L"I", L"love"};
		wstring ys[] = {L"0",L"1"};
		int n = sizeof(xs) / sizeof(wstring);
		MakeDocument(doc, xs, ys, n);
		trainingSet.push_back(doc);
	
		vector<LCCRF::FeatureType> fs;

		LCCRF::FeatureType f1 = [](const Document& doc, wstring s1, wstring s2, int j)
		{
			if(j < 0 || j >= doc.size()) {return 0;}
			if(doc[j].x[0] == L"I" && s2 == L"0") {return 1;}
			return 0;
		};
		LCCRF::FeatureType f2 = [](const Document& doc, wstring s1, wstring s2, int j)
		{
			if(j < 0 || j >= doc.size()) {return 0;}
			if(doc[j].x[0] == L"love" && s2 == L"1") {return 1;}
			return 0;
		};

		fs.push_back(f1);
		fs.push_back(f2);
		lccrf = new LCCRF(fs, 0.1);
	}

	static void TearDownTestCase() 
	{
	}

	static LCCRF* lccrf;
	static list<Document> trainingSet;
};

LCCRF* LCCRFTestSuite::lccrf;
list<Document> LCCRFTestSuite::trainingSet;

TEST_F(LCCRFTestSuite, TestLearn)
{
	lccrf->Learn(trainingSet, 0.1, 1, 1000);
	EXPECT_NEAR(1.63, lccrf->_weights[0], 10e-2);
	EXPECT_NEAR(1.63, lccrf->_weights[1], 10e-2);
}

TEST_F(LCCRFTestSuite, TestDerivative)
{
	// -d/dx = 1-(e^(x+y)+e^x)/(e^(x+y)+e^x+e^y+1)-0.1x
	// -d/dy = 1-(e^(x+y)+e^y)/(e^(x+y)+e^x+e^y+1)-0.1y

	lccrf->Learn(trainingSet, 1, 1, 0);
	double res = 0;
	vector<double> weights;

	weights.clear();
	weights.push_back(0);
	weights.push_back(0);
	res = lccrf->_derivatives[0](weights, *(trainingSet.begin()));
	EXPECT_NEAR(-0.5, res, 10e-6);
	res = lccrf->_derivatives[1](weights, *(trainingSet.begin()));
	EXPECT_NEAR(-0.5, res, 10e-6);
	
	weights.clear();
	weights.push_back(1.0);
	weights.push_back(1.0);
	res = lccrf->_derivatives[0](weights, *(trainingSet.begin()));
	EXPECT_NEAR(-0.168941, res, 10e-6);
	res = lccrf->_derivatives[1](weights, *(trainingSet.begin()));
	EXPECT_NEAR(-0.168941, res, 10e-6);

	weights.clear();
	weights.push_back(0.5);
	weights.push_back(2.0);
	res = lccrf->_derivatives[0](weights, *(trainingSet.begin()));
	EXPECT_NEAR(-0.327541, res, 10e-6);
	res = lccrf->_derivatives[1](weights, *(trainingSet.begin()));
	EXPECT_NEAR(0.0807971, res, 10e-6);
}

TEST_F(LCCRFTestSuite, TestLikelihood)
{
	// -d/dx = 1-(e^(x+y)+e^x)/(e^(x+y)+e^x+e^y+1)-0.1x
	// -d/dy = 1-(e^(x+y)+e^y)/(e^(x+y)+e^x+e^y+1)-0.1y

	lccrf->Learn(trainingSet, 1, 1, 0);
	double res = 0;
	vector<double> weights;

	weights.clear();
	weights.push_back(0);
	weights.push_back(0);
	res = lccrf->_likelihood(weights, *(trainingSet.begin()));
	EXPECT_NEAR(1.38629, res, 10e-6);
	
	weights.clear();
	weights.push_back(1.0);
	weights.push_back(1.0);
	res = lccrf->_likelihood(weights, *(trainingSet.begin()));
	EXPECT_NEAR(0.726523, res, 10e-6);

	weights.clear();
	weights.push_back(0.5);
	weights.push_back(2.0);
	res = lccrf->_likelihood(weights, *(trainingSet.begin()));
	EXPECT_NEAR(0.813505, res, 10e-6);
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();
	return ret;
}

