#include "CppUnitTest.h"
#include <vector>
#include <iostream>
#define private public
#include "../lib/LCCRF.h"
#include "../lib/FWBW.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestLib
{
	TEST_CLASS(FWBWTestSuite)
	{
		void MakeDocument(X& x, Y& y, LCCRFFeatures& lccrfFeatures)
		{
			x.AddFeature(0, 0);
			x.AddFeature(1, 1);

			y.SetTag(0, 0);
			y.SetTag(1, 1);

			lccrfFeatures.AddSample(x, y);
		}

		void PrintMatrix(const MultiArray<double, 2>& alphaMatrix)
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

	public:

		TEST_METHOD(TestLearn)
		{
			FWBW* fwbw;
			X x(2);
			Y y(2);
			LCCRFFeatures lccrfFeatures;

			MakeDocument(x, y, lccrfFeatures);
			std::vector<double> weights(3, 1.0);
			fwbw = new FWBW(x, y, lccrfFeatures, weights);

			FWBWTestSuite::PrintMatrix(fwbw->_alphaMatrix);
			Assert::AreEqual(fwbw->GetAlphaMatrix().Dim1(), 2);
			Assert::AreEqual(fwbw->GetAlphaMatrix().Dim2(), 2);

			// alpha matrix.
			Assert::AreEqual(0.731058, fwbw->GetAlphaMatrix()[0][0], 1e-4);
			Assert::AreEqual(0.268941, fwbw->GetAlphaMatrix()[0][1], 1e-4);
			Assert::AreEqual(0.140195, fwbw->GetAlphaMatrix()[1][0], 1e-4);
			Assert::AreEqual(0.859804, fwbw->GetAlphaMatrix()[1][1], 1e-4);

			Assert::AreEqual(3.718281, fwbw->_alphaScales[0], 1e-4);
			Assert::AreEqual(7.132891, fwbw->_alphaScales[1], 1e-4);

			FWBWTestSuite::PrintMatrix(fwbw->GetBetaMatrix());
			Assert::AreEqual(fwbw->GetBetaMatrix().Dim1(), 2);
			Assert::AreEqual(fwbw->GetBetaMatrix().Dim2(), 2);

			// beta matrix.
			Assert::AreEqual(0.859804, fwbw->GetBetaMatrix()[0][0], 1e-4);
			Assert::AreEqual(0.140195, fwbw->GetBetaMatrix()[0][1], 1e-4);
			Assert::AreEqual(0.268941, fwbw->GetBetaMatrix()[1][0], 1e-4);
			Assert::AreEqual(0.731058, fwbw->GetBetaMatrix()[1][1], 1e-4);

			Assert::AreEqual(7.132891, fwbw->_betaScales[0], 1e-4);
			Assert::AreEqual(3.718281, fwbw->_betaScales[1], 1e-4);
		}
	};
}