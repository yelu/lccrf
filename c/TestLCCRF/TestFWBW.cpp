#include "gtest/gtest.h"
#include <memory>
#define private public
#include "../lib/FWBW.h"

class FWBWTestSuite : public ::testing::Test
{
protected:

	static void MakeDocument(X& x, Y& y, LCCRFFeatures& lccrfFeatures)
	{
		x.AddFeature(0, 0);
		x.AddFeature(1, 1);

		y.SetTag(0, 0);
		y.SetTag(1, 1);

		lccrfFeatures.AddSample(x, y);
	}

	static void PrintMatrix(MultiArray<double, 2>& alphaMatrix)
	{
		for (int i = 0; i < alphaMatrix.Dim1(); i++)
		{
			for (int j = 0; j < alphaMatrix.Dim2(); j++)
			{
				std::cout << alphaMatrix[i][j] << "\t";
			}
			std::cout << std::endl;
		}
	}

	static void SetUpTestCase()
	{
		MakeDocument(x, y, lccrfFeatures);
		std::vector<double> weights(3, 1.0);
		fwbw = new FWBW(x, y, lccrfFeatures, weights);
	}

	static void TearDownTestCase()
	{
	}

	static FWBW* fwbw;
	static X x;
	static Y y;
	static LCCRFFeatures lccrfFeatures;
};

FWBW* FWBWTestSuite::fwbw;
X FWBWTestSuite::x(2);
Y FWBWTestSuite::y(2);
LCCRFFeatures FWBWTestSuite::lccrfFeatures;

TEST_F(FWBWTestSuite, TestLearn)
{
	FWBWTestSuite::PrintMatrix(fwbw->_alphaMatrix);
	EXPECT_EQ(fwbw->_alphaMatrix.Dim1(), 2);
	EXPECT_EQ(fwbw->_alphaMatrix.Dim2(), 2);

	// alpha matrix.
	EXPECT_NEAR(0.731058, fwbw->_alphaMatrix[0][0], 1e-4);
	EXPECT_NEAR(0.268941, fwbw->_alphaMatrix[0][1], 1e-4);
	EXPECT_NEAR(0.140195, fwbw->_alphaMatrix[1][0], 1e-4);
	EXPECT_NEAR(0.859804, fwbw->_alphaMatrix[1][1], 1e-4);

	EXPECT_NEAR(3.718281, fwbw->_alphaScales[0], 1e-4);
	EXPECT_NEAR(7.132891, fwbw->_alphaScales[1], 1e-4);

	FWBWTestSuite::PrintMatrix(fwbw->_betaMatrix);
	EXPECT_EQ(fwbw->_betaMatrix.Dim1(), 2);
	EXPECT_EQ(fwbw->_betaMatrix.Dim2(), 2);

	// beta matrix.
	EXPECT_NEAR(0.859804, fwbw->_betaMatrix[0][0], 1e-4);
	EXPECT_NEAR(0.140195, fwbw->_betaMatrix[0][1], 1e-4);
	EXPECT_NEAR(0.268941, fwbw->_betaMatrix[1][0], 1e-4);
	EXPECT_NEAR(0.731058, fwbw->_betaMatrix[1][1], 1e-4);

	EXPECT_NEAR(7.132891, fwbw->_betaScales[0], 1e-4);
	EXPECT_NEAR(3.718281, fwbw->_betaScales[1], 1e-4);
}

/*
int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();
	return ret;
}
*/
