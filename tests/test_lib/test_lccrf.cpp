#include "CppUnitTest.h"
#define private public
#include "..\lib\LCCRF.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestLib
{		
	TEST_CLASS(TestLCCRF)
	{
		void MakeDocument(vector<X>& xs, vector<Y>& ys, LCCRFFeatures& lccrfFeatures)
		{
			X x(2);
			x.AddFeature(0, 0);
			x.AddFeature(1, 1);

			Y y(2);
			y.SetTag(0, 0);
			y.SetTag(1, 1);

			lccrfFeatures.AddSample(x, y);
			xs.push_back(std::move(x));
			ys.push_back(std::move(y));		
		}

	public:

		TEST_METHOD(TestFitAndPredict)
		{
			LCCRF* lccrf;
			vector<X> xs;
			vector<Y> ys;
			LCCRFFeatures lccrfFeatures;

			MakeDocument(xs, ys, lccrfFeatures);
			lccrf = new LCCRF();

			// fit
			lccrf->Fit(xs, ys, 1000, 0.1, 0.1);
			Assert::AreEqual(0.76578, lccrf->GetWeights()[0], 1e-4);
			Assert::AreEqual(0.76578, lccrf->GetWeights()[1], 1e-4);
			Assert::AreEqual(1.7396, lccrf->GetWeights()[2], 1e-4);
			string weightsFile = "./lccrf.weights.txt";
			lccrf->Save(weightsFile);

			// check likelihood, without regularization.
			FWBW fwbw(xs.front(), ys.front(), lccrfFeatures, lccrf->GetWeights());
			double likehoold = fwbw.CalcLikelihood(xs.front(), ys.front());
			Assert::AreEqual(-0.18334, likehoold, 1e-4);

			// predit
			Y res = lccrf->Predict(xs.front());
			Assert::AreEqual((int)res.Tags[0], 0);
			Assert::AreEqual((int)res.Tags[1], 1);
		}

	};
}