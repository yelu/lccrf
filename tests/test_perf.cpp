#include "gtest/gtest.h"
#include "lccrf.h"

TEST(TestLCCRF, TestPerf)
{
	LCCRF lccrf;
	string data = "./train.lccrf.converted.dat";
	string model = "./model.bin";
	lccrf.Fit(data, 50, 0.001, 0.001);
	lccrf.Save(model);
}

