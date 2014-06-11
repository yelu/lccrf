#include "gtest/gtest.h"
#define private public
#include "../LCCRF/LCCRF.h"
#include "../LCCRF/NGramFeaturizer.h"
#include <memory>

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
	
		std::shared_ptr<Featurizer> f(new NGramFeaturizer(1));
		f->Fit(doc);
		f->Serialize(L".\\NgramFeatures.txt");
		featureManager.AddFeaturizer(f);
		lccrf = new LCCRF(featureManager, 0.1);
	}

	static void TearDownTestCase() 
	{
	}

	static LCCRF* lccrf;
	static list<Document> trainingSet;
	static FeatureManager featureManager;
};

LCCRF* LCCRFTestSuite::lccrf;
list<Document> LCCRFTestSuite::trainingSet;
FeatureManager LCCRFTestSuite::featureManager;

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

